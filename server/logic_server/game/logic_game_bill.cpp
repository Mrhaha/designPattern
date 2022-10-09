//
// Created by hzl on 2019/4/18.
//

#include "common_mysql.h"
#include "logic_record_sql_table.h"
#include "logic_record_user_mail_table.h"
#include "logic_record_user_constellation_table.h"
#include "logic_record_user_equip_table.h"
#include "logic_game_bill.h"
#include "logic_game_user.h"
#include "logic_config.h"
#include "logic_link_common.h"
#include "logic_link_define.h"
#include "tc_md5.h"

std::string escape_sql_string(const std::string& sql)
{
    return CLogicSQLTableRecordBase::GetMysqlConn()->escapeString(sql);
}

int32_t CLogicBill::m_iBillRationCmd = 0;
std::string CLogicBill::m_strBillRationCmdName;
int32_t CLogicBill::m_iBillRationSequence = 0;
std::string CLogicBill::m_strBillRation;

void CLogicBill::RefreshBillRation(int32_t iCmd, const std::string& strCmdName)
{
    m_iBillRationCmd = iCmd;
    m_strBillRationCmdName = strCmdName;
    m_strBillRation.clear();
}

const std::string &CLogicBill::GetBillRation()
{
    if (m_strBillRation.empty())
    {
        m_iBillRationSequence = m_iBillRationSequence % 10000;
        struct tm stNowDate = CLogicCommon::GetDate();

        char szBill[50];
        snprintf(szBill, sizeof(szBill),
                 "%04d%02d%02d%02d%02d%02d%04d%06d%04d",
                 stNowDate.tm_year + 1900,
                 stNowDate.tm_mon + 1,
                 stNowDate.tm_mday,
                 stNowDate.tm_hour,
                 stNowDate.tm_min,
                 stNowDate.tm_sec,
                 CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID,
                 m_iBillRationCmd,
                 m_iBillRationSequence++);

        m_strBillRation = szBill;
    }

    return m_strBillRation;
}

const std::string& CLogicBill::GetBillCmdName()
{
    return m_strBillRationCmdName;
}

void CLogicBill::SaveGameItemBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd,
                                         int32_t iItemType, int32_t iItemID,
                                         int32_t iCurrentNum, int32_t iDeltaNum, const std::string &strDescription)
{
    const auto& server_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    auto sql = build_insert_escape_sql("bill_item", {
            {"bill_ration",     GetBillRation()},
            {"platform",        server_config.m_strPlatform},
            {"channel",         iChannelID},
            {"uin",             iUin},
            {"group_id",        iGroupID},
            {"server_id",       server_config.m_iServerID},
            {"cmd",             iCmd},
            {"item_type",       iItemType},
            {"item_id",         iItemID},
            {"current_value",   iCurrentNum},
            {"delta_value",     iDeltaNum},
            {"description",     strDescription}
    }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), sql);

    // 同时更新 stat_item 表
    std::string sum_column_name = (iDeltaNum > 0 ? "sum_output_value" : "sum_consume_value");
    auto stat_sql = build_insert_or_update_escape_sql("stat_item", {
            {"stat_date", CLogicCommon::GetDateString()},
            {"platform", server_config.m_strPlatform},
            {"server_id", server_config.m_iServerID},
            {"cmd", iCmd},
            {"item_type", iItemType},
            {"item_id", iItemID},
            {sum_column_name, iDeltaNum, EMDKUT_Sum},
        }, escape_sql_string);

    // 参数错误，空sql语句，告警，将 bill_item 的 insert 语句打印
    if(stat_sql.empty())
    {
        LOGIC_LOG_TRACE_ERROR << "|ERROR MSG: statdb stat_item table insert error!"
                            << "bill_item sql|:" << sql << std::endl;

        LOGIC_SERVICE_API_INS->RequestServiceAlert("exec_sql_fail", {
                { "sql", sql },
                { "error", "statdb stat_item table insert error!"}
        });
    }
    else
    {
        LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), stat_sql);
    }
}

void CLogicBill::SaveConstelBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd, int32_t iDeltaValue, const TUserConstellationTableValueType& value)
{
    const auto& server_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    std::stringstream ss;
    ss << "[" << value.m_iMainAttrType;
    for(auto& attr : value.m_stExAttr)
    {
        ss << "-" << attr.first << ":" << attr.second.first << ":" << attr.second.second;
    }
    ss << "]";

    auto sql = build_insert_escape_sql("bill_constel", {
        {"platform",        server_config.m_strPlatform},
        {"channel",         iChannelID},
        {"uin",             iUin},
        {"group_id",        iGroupID},
        {"server_id",       server_config.m_iServerID},
        {"cmd",             iCmd},
        {"unique_id",       value.m_iUniqueID},
        {"grade",           value.m_iGrade},
        {"equip_tag",       value.m_iEquipmentTag},
        {"suit",            value.m_iSuit},
        {"level",            value.m_iLevel},
        {"attr_str",        ss.str()},
        {"delta_value",     iDeltaValue}
    }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), sql);
}

void CLogicBill::SaveEquipBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd, int32_t iDeltaValue, const TUserEquipTableValueType& value)
{
    const auto& server_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    auto sql = build_insert_escape_sql("bill_equip", {
            {"platform",        server_config.m_strPlatform},
            {"channel",         iChannelID},
            {"uin",             iUin},
            {"group_id",        iGroupID},
            {"server_id",       server_config.m_iServerID},
            {"cmd",             iCmd},
            {"unique_id",       value.m_iUniqueID},
            {"grade",           value.m_iGrade},
            {"equip_tag",       value.m_iEquipmentTag},
            {"attr_index",      value.m_iAttrIndex},
            {"delta_value",     iDeltaValue}
    }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), sql);
}

void CLogicBill::SaveFightPassBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd
        , int32_t iLevelID, int32_t iFightPower, const std::vector<int32_t>& iTeamVec, const std::string& strAddition)
{
    const auto& system_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    const std::string strJoinTeam = vector_join(iTeamVec, ", ");
    auto sql = build_insert_escape_sql("bill_fight_pass", {
            {"bill_ration",     GetBillRation()},
            {"platform",        system_config.m_strPlatform},
            {"channel",         iChannelID},
            {"uin",             iUin},
            {"group_id",        iGroupID},
            {"server_id",       system_config.m_iServerID},
            {"cmd",             iCmd},
            {"level_id",        iLevelID},
            {"fight_power",     iFightPower},
            {"fight_team",      strJoinTeam},
            {"addition",        strAddition}
    }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), sql);
}

void CLogicBill::SaveFirstFightBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd
        , int32_t iLevelID, int32_t iLevelStep, int32_t iFightPower)
{
    const auto& system_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    auto sql = build_insert_escape_sql("bill_first_fight", {
            {"bill_ration",     GetBillRation()},
            {"platform",        system_config.m_strPlatform},
            {"channel",         iChannelID},
            {"uin",             iUin},
            {"group_id",        iGroupID},
            {"server_id",       system_config.m_iServerID},
            {"cmd",             iCmd},
            {"level_id",        iLevelID},
            {"level_step",      iLevelStep},
            {"fight_power",     iFightPower}
    }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), sql);
}

void CLogicBill::SaveAntiScoreBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd
        , int32_t iPassDay, int32_t iScore,int32_t iScoreLimit, int32_t iFightPower, const std::vector<int32_t>& iTeamVec, const std::string& strAddition)
{
    const auto& system_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    const std::string strJoinTeam = vector_join(iTeamVec, ", ");
    auto sql = build_insert_escape_sql("bill_anti_score", {
            {"bill_ration",     GetBillRation()},
            {"platform",        system_config.m_strPlatform},
            {"channel",         iChannelID},
            {"uin",             iUin},
            {"group_id",        iGroupID},
            {"server_id",       system_config.m_iServerID},
            {"cmd",             iCmd},
            {"level_id",        iPassDay},
            {"pass_score",      iScore},
            {"score_limit",     iScoreLimit},
            {"fight_power",     iFightPower},
            {"fight_team",      strJoinTeam},
            {"addition",        strAddition}
        }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), sql);
}

void CLogicBill::SaveAntiCheatingBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd
        , int32_t iLevelID, int32_t iFightPower, int32_t iAntiFightPower, const std::vector<int32_t>& iTeamVec, const std::string& strAddition, int32_t iPassTime)
{
    const auto& system_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    const std::string strJoinTeam = vector_join(iTeamVec, ", ");
    auto sql = build_insert_escape_sql("bill_anti_cheating", {
            {"bill_ration",     GetBillRation()},
            {"platform",        system_config.m_strPlatform},
            {"channel",         iChannelID},
            {"uin",             iUin},
            {"group_id",        iGroupID},
            {"server_id",       system_config.m_iServerID},
            {"cmd",             iCmd},
            {"level_id",        iLevelID},
            {"fight_power",     iFightPower},
            {"fight_team",      strJoinTeam},
            {"anti_fight_power",    iAntiFightPower},
            {"addition",        strAddition},
            {"pass_time",        iPassTime}
        }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), sql);
}

void CLogicBill::SaveUserLoginBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd, const std::string& strCmdName)
{
    const auto& system_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    const stLogicSoClientInfo info = CLogicUser::GetInstance().GetClientInfo(iUin);

    int32_t iOnlineTime = (info.m_iGroupID == iGroupID ? CLogicCommon::GetSec() - info.m_iLoginTime : 0);

    auto sql = build_insert_escape_sql("bill_login", {
            {"bill_ration",         GetBillRation()},
            {"platform",            system_config.m_strPlatform},
            {"channel",             iChannelID},
            {"uin",                 iUin},
            {"group_id",            iGroupID},
            {"server_id",           system_config.m_iServerID},
            {"cmd",                 iCmd},
            {"account",             info.m_stStatInfo.m_strAccount},
            {"client_addr",         info.m_strClientIP},
            {"device_id",           info.m_stStatInfo.m_strDeviceID},
            {"device_model",        info.m_stStatInfo.m_strDeviceModel},
            {"device_os",           info.m_stStatInfo.m_strDeviceOS},
            {"device_ratio",        info.m_stStatInfo.m_strDeviceRatio},
            {"device_carrier",      info.m_stStatInfo.m_strDeviceCarrier},
            {"device_net",          info.m_stStatInfo.m_strDeviceNet},
            {"online_time",         iOnlineTime}
        }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, strCmdName, GetBillRation(), sql);

    if (CLogicConfig::GetSystemConfig().m_stServerConfig.m_iLoginPushover)
    {
        LOGIC_SERVICE_API_INS->RequestServiceAlert("have_user_login", {
                { "uin", std::to_string(iUin) },
                { "group_id", std::to_string(iGroupID) },
                { "channel", std::to_string(iChannelID) },
        });
    }
}

void CLogicBill::SaveChatBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iLevel
        , int32_t iVipLevel, const std::string& strNick, int32_t iChannel, const std::string& strMsg, int32_t iTalkUin)
{
    const auto& system_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    const std::string strEscapeMsg = LOGIC_SERVICE_API_INS->SkipText4ByteChars(strMsg);
    const std::string ip = CLogicUser::GetInstance().GetClientInfo(iUin).m_strClientIP;

    auto sql = build_insert_escape_sql("game_chat", {
            {"platform",            system_config.m_strPlatform},
            {"channel",             iChannelID},
            {"uin",                 iUin},
            {"group_id",            iGroupID},
            {"server_id",           system_config.m_iServerID},
            {"address",             ip},
            {"level",               iLevel},
            {"vip_level",           iVipLevel},
            {"nick",                strNick},
            {"talk_channel",        iChannel},
            {"talk_uin",            iTalkUin},
            {"talk_content",        strEscapeMsg}
        }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), sql);
}

void CLogicBill::SaveJubaoBill(int32_t iUin, int32_t iGroupID, int32_t iChannelID, const std::string &strNick
        , int32_t iDstUin, int32_t iDstGroupID, const std::string &strDstNick, int32_t iDstLevel, int32_t iDstVipLevel, int32_t iDstFightPower
        , int32_t iType, const std::string &strMsg, const std::string &strFromUI, int32_t iIsAward, const std::string& strUser)
{
    const auto& system_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    auto sql = build_insert_escape_sql("game_jubao", {
            {"platform",            system_config.m_strPlatform},
            {"channel",             iChannelID},
            {"uin",                 iUin},
            {"group_id",            iGroupID},
            {"server_id",           system_config.m_iServerID},
            {"nick",                strNick},
            {"dst_uin",             iDstUin},
            {"dst_group_id",        iDstGroupID},
            {"dst_nick",            strDstNick},
            {"dst_level",           iDstLevel},
            {"dst_vip_level",       iDstVipLevel},
            {"dst_fight_power",     iDstFightPower},
            {"type",                iType},
            {"from_ui",             strFromUI},
            {"content",             strMsg},
            {"is_award",            iIsAward},
            {"user",                strUser}
        }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), sql);
}

void CLogicBill::SaveMailBill(int32_t iChannelID, SaveMailBillType iHandleType, const std::string& strCmdName, const TUserMailTableValueType& mail, const std::vector<CPackGameItem>& items)
{
    const auto& server_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    msgpack::sbuffer stBuffer;
    msgpack::pack(stBuffer, items);

    auto sql = build_insert_escape_sql("bill_mail", {
            {"bill_ration",         GetBillRation()},
            {"platform",            server_config.m_strPlatform},
            {"channel",             iChannelID},
            {"uin",                 mail.m_iUid},
            {"group_id",            mail.m_iGroupID},
            {"server_id",           server_config.m_iServerID},
            {"mail_handle_type",    (int32_t)iHandleType},
            {"cmd_name",            strCmdName},
            {"mail_id",             mail.m_iMailID},
            {"read_time",           mail.m_iReadTime},
            {"award_multiple",      mail.m_iAwardMultiple},
            {"content",             mail.m_strContent},
            {"create_time",         mail.m_iCreateTime},
            {"sender",              mail.m_strSender},
            {"title",               mail.m_strTitle},
            {"attachment",          taf::TC_Base64::encode(std::string(stBuffer.data(), stBuffer.size()))}
    }, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(mail.m_iGroupID, mail.m_iUid, GetBillCmdName(), GetBillRation(), sql);
}

void CLogicBill::SaveGameQuestionnaire(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iQuestionnaireID, int32_t iLevel
        , int32_t iVipLevel, int32_t iCombat, const std::string& strNick, std::vector<std::pair<std::string, std::string>> stAnswers)
{
    const auto& server_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    std::vector<TMysqlColumn> stColumnVec = {
        {"uin",                 iUin},
        {"group_id",            iGroupID},
        {"platform",            server_config.m_strPlatform},
        {"channel_id",          iChannelID},
        {"server_id",           server_config.m_iServerID},
        {"questionnaire_id",    iQuestionnaireID},
        {"nick",                strNick},
        {"level",               iLevel},
        {"vip_level",           iVipLevel},
        {"combat",              iCombat},
    };

    for(std::size_t index = 0; index < stAnswers.size(); ++index)
    {
        auto& answer = stAnswers[index];

        // 选项内容
        if(!answer.first.empty())
        {
            TMysqlColumn stColumn;
            stColumn.m_strColumnName = "select" + std::to_string(index + 1);
            stColumn.m_varValue = answer.first;
            stColumnVec.emplace_back(stColumn);
        }

        // 自由输入内容
        if(!answer.second.empty())
        {
            TMysqlColumn stColumn;
            stColumn.m_strColumnName = "problem" + std::to_string(index + 1);
            stColumn.m_varValue = answer.second;
            stColumnVec.emplace_back(stColumn);
        }
    }

    auto sql = build_insert_escape_sql("game_questionnaire", stColumnVec, escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), sql);
}

void CLogicBill::SaveGuildWish(int iStatTime, int iUin, int iGroupID, int iDstUin, int iDstGroupID, int iCardID, int iSwapCardID, int iConsume, int iAccept)
{
    const auto& server_config = CLogicConfig::GetSystemConfig().m_stServerConfig;

    auto stat_sql = build_insert_or_update_escape_sql("stat_guild_wish",
                                                      {
                                                          {"platform",  server_config.m_strPlatform},
                                                          {"stat_time", iStatTime},
                                                          {"uin",       iUin},
                                                          {"group_id",  iGroupID},
                                                          {"dst_uin",   iDstUin},
                                                          {"dst_group_id", iDstGroupID},
                                                          {"card_id",   iCardID},
                                                          {"swap_card_id", iSwapCardID},

                                                          {"consume", iConsume, EMDKUT_Sum},
                                                          {"dst_accept", iAccept, EMDKUT_Sum},
                                                      },
                                                      escape_sql_string);

    LOGIC_SERVICE_API_INS->ExecStatSQL(iGroupID, iUin, GetBillCmdName(), GetBillRation(), stat_sql);
}
