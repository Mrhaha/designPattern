#include "HttpProcessor.h"
#include "server_cmd_cross_define.h"
#include "server_cross_protocol.h"
#include "ServerConfig.h"
#include "ServerAlert.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"
#include "ServerDataReport.h"
#include "json.hpp"
using Json = nlohmann::json;


int32_t CHttpProcessor::ProcessMessages(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    int32_t iReturn = fail;
#if CROSS_BUILD_PLATFORM == BUILD_PLATFORM_LOCAL
    return success;
#endif
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    const auto &dataReportConfig = cross_config->GetDataReportConfig();

    if (!dataReportConfig.m_iOpen && stHeadInfo.m_usCmd != MSG_REPORT_PAY
        && stHeadInfo.m_usCmd != MSG_REPORT_OFFLINE)
    {
        return success;
    }

    switch (stHeadInfo.m_usCmd)
    {
    case MSG_CROSS_SERVICE_ALERT:
        iReturn = ProcessServiceAlert(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_SET_MUSIC:
        iReturn = ProcessServiceDataReportSetMusic(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_GET_EQUIP:
        iReturn = ProcessServiceDataReportGetEquip(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_UPGRADE_SKILL:
        iReturn = ProcessServiceDataReportUpgradeSkill(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_UPGRADE_CONSTEL:
        iReturn = ProcessServiceDataReportUpgradeConstel(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_GET_CONSTEL:
        iReturn = ProcessServiceDataReportGetConstel(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_START_FIGHT_PVE:
        iReturn = ProcessServiceDataReportStartFightPVE(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_GAIN_EMAIL:
        iReturn = ProcessServiceDataReportGainEmail(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_ACHIEVE_QUEST:
        iReturn = ProcessServiceDataReportAchieveMission(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_GET_REWARD_QUEST:
        iReturn = ProcessServiceDataReportGetMissionReward(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_GET_PASS_REWARD:
        iReturn = ProcessServiceDataReportGetPassReward(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_PVP_FIGHT:
        iReturn = ProcessServiceDataReportPVPFight(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_REFRESH_PVP:
        iReturn = ProcessServiceDataReportRefreshPVP(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_REWARD_PVP:
        iReturn = ProcessServiceDataReportRewardPVP(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_CHANGE_TEAM:
        iReturn = ProcessServiceDataReportChangeTeam(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_CREATE_GUILD:
        iReturn = ProcessServiceDataReportCreateGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_JOIN_GUILD:
        iReturn = ProcessServiceDataReportJoinGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_LEAVE_GUILD:
        iReturn = ProcessServiceDataReportLeaveGuild(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_CHANGE_GUILD_NAME:
        iReturn = ProcessServiceDataReportChangeGuildName(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_DONATE_CARD:
        iReturn = ProcessServiceDataReportDonateCard(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_SET_WISH_CARD:
        iReturn = ProcessServiceDataReportSetWishCard(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_BUG_GUILD_ITEM:
        iReturn = ProcessServiceDataReportBuyGuildItem(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_PAY_CHOOSE:
        iReturn = ProcessServiceDataReportChoosePay(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_ITEM_MONITOR:
        iReturn = ProcessServiceDataReportItemMonitor(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_PAY:
        iReturn = ProcessServiceDataReportPay(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    case MSG_REPORT_OFFLINE:
        iReturn = ProcessServiceDataReportOffline(stHeadInfo, pszMsgBody, iBodyLength);
        break;
    default:
        Log(log_mask_system_error, "[CHttpProcessor::%s]recv unknown msg, cmd=%d\n", __FUNCTION__, stHeadInfo.m_usCmd);
        break;
    }
    return iReturn;
}

int32_t CHttpProcessor::ProcessServiceAlert(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestServiceAlert stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    // pushover告警
    auto result = CServerAlert::SendAlertNotice(stReqBody.m_strID, stReqBody.m_stParams);
    Log(log_mask_detail, "[CHttpProcessor::%s]send alert notice. id:%s|result:%s\n", __FUNCTION__, stReqBody.m_strID.c_str(), result.c_str());

    // 邮件告警
    auto apiResult = CServerAlert::SendAlertApiMailNotice(stReqBody.m_strID, stReqBody.m_stParams);
    Log(log_mask_detail, "[CHttpProcessor::%s]send alert api mail notice. id:%s|result:%s\n", __FUNCTION__, stReqBody.m_strID.c_str(), apiResult.c_str());

    /* 弃用自己发邮件逻辑，改为向API发送邮件通知请求，由API发送邮件
    std::string strCode = CServerAlert::SendAlertMailNotice(stReqBody.m_strID, stReqBody.m_stParams);
    Log(log_mask_detail, "[CHttpProcessor::%s]send alert mail notice. id:%s|result %s \n", __FUNCTION__, stReqBody.m_strID.c_str(), strCode.c_str());
    if(false == strCode.empty())
    {
        // 如果其他管理人员的邮件告警失败，通知高级管理员pushover告警
        static const std::string strMailFailedId = "member_mail_notify_fail";
        std::shared_ptr<CCrossServerConfig> cross_config;
        CCrossServerConfig::GetConfig(cross_config);
        const auto& stConfig = cross_config->GetZooCrossConfig().m_stCrossConfig;
        std::map<std::string, std::string> stParams = {
            {"hostname", stConfig.m_strHostname},
            {"server_ip", stConfig.m_strHost},
            {"platform", stConfig.m_strPlatform},
            {"error", strCode},
        };

        strCode = CServerAlert::SendAlertNotice(strMailFailedId, stParams);
        Log(log_mask_detail, "[CHttpProcessor::%s]send alert mail failed notice. id:%s|result:%s\n", __FUNCTION__, strMailFailedId.c_str(), strCode.c_str());
    }
     */

    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportSetMusic(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportSetMusic stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["music_id"] = stReqBody.m_stMusicID;
    dataJson["operation_id"] = stReqBody.m_iPos;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportGetEquip(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportGetEquip stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }
    auto to_json = [](CDataReportPackEquipInfo info) -> Json {
        Json structData;
        structData["equip_id"] = info.m_iEquipID;
        structData["equip_star"] = info.m_iEquipStar;
        structData["equip_type"] = info.m_iEquipType;
        return structData;
    };

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    for(auto equipItem : stReqBody.m_vecEquipInfo) dataJson["equip_infos"].emplace_back(to_json(equipItem));
    dataJson["get_way"] = stReqBody.m_iGetLevelID;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportUpgradeSkill(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportUpdateTalent stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["chara_id"] = stReqBody.m_iCardID;
    dataJson["chara_level"] = stReqBody.m_iCardLevel;
    dataJson["skill_id"] = stReqBody.m_iSkillID;
    dataJson["skill_after_level"] = stReqBody.m_iSkillAfterLevel;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;


    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportUpgradeConstel(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportUpdateConstel stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["constel_id"] = stReqBody.m_iConstelID;
    dataJson["constel_star"] = stReqBody.m_iConstelStar;
    dataJson["constel_suit"] = stReqBody.m_iConstelSuit;
    dataJson["constel_position"] = stReqBody.m_iConstelPos;
    dataJson["constel_after_level"] = stReqBody.m_iConstelAfterLevel;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportGetConstel(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportGetConstel stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }
    auto to_json = [](CDataReportPackConstelInfo info) -> Json {
        Json structData;
        structData["constel_id"] = info.m_iConstelID;
        structData["constel_star"] = info.m_iConstelStar;
        structData["constel_suit"] = info.m_iConstelSuit;
        structData["constel_pos"] = info.m_iConstelPos;
        return structData;
    };

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    for(auto constelItem : stReqBody.m_vecConstelInfo) dataJson["constel_infos"].emplace_back(to_json(constelItem));
    dataJson["get_way"] = stReqBody.m_iGetLevelID;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportStartFightPVE(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportStartFightPVE stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["level_id"] = stReqBody.m_iLevelID;
    dataJson["formation_chara"] = stReqBody.m_vecTeamCardID;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportGainEmail(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportGainEmail stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }
    auto to_json = [](CPackGameItem item) -> Json {
        Json structData;
        structData["item_type"] = item.m_iItemType;
        structData["item_id"] = item.m_iCardID;
        structData["item_num"] = item.m_iNum;
        return structData;
    };

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    for(auto equipItem : stReqBody.m_stReward)
    {
        dataJson["item_infos"].emplace_back(to_json(equipItem));
    }
    dataJson["email_id"] = stReqBody.m_iEmailID;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportAchieveMission(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportAchieveMission stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["achieve_id"] = stReqBody.m_iQuestID;
    dataJson["achieve_type"] = stReqBody.m_iQuestType;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportGetMissionReward(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportTotalRewardMission stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();

    auto reward_to_json = [](CPackGameItem item) -> Json {
        Json structData;
        structData["item_type"] = item.m_iItemType;
        structData["item_id"] = item.m_iCardID;
        structData["item_num"] = item.m_iNum;
        return structData;
    };

    auto to_json = [](CDataReportGetRewardMission item) -> Json {
        Json structData;
        structData["quest_id"] = item.m_iQuestID;
        structData["quest_type"] = item.m_iQuestType;
        return structData;
    };

    for(auto missionInfo : stReqBody.m_stMissionReward)
    {
        dataJson["quest_infos"].emplace_back(to_json(missionInfo));
    }

    for(auto rewardItem : stReqBody.m_stRewardItem)
    {
        dataJson["reward"].emplace_back(reward_to_json(rewardItem));
    }
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;


    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportGetPassReward(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportGetPassReward stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["normal_level"] = stReqBody.m_stNormalLevel;
    dataJson["advanced_level"] = stReqBody.m_stAdvancedLevel;
    auto reward_to_json = [](CPackGameItem item) -> Json {
        Json structData;
        structData["item_type"] = item.m_iItemType;
        structData["item_id"] = item.m_iCardID;
        structData["item_num"] = item.m_iNum;
        return structData;
    };

    for(auto rewardItem : stReqBody.m_stTotalReward)
    {
        dataJson["reward"].emplace_back(reward_to_json(rewardItem));
    }

    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportPVPFight(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportPVP stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["attack_uin"] = stReqBody.m_iAttackUin;
    dataJson["defend_uid"] = stReqBody.m_iDefendUin;
    dataJson["attack_rank"] = stReqBody.m_iAttackRank;
    dataJson["defend_rank"] = stReqBody.m_iDefendRank;
    dataJson["is_win"] = stReqBody.m_bWin;
    dataJson["is_bot"] = stReqBody.m_bBot;
    dataJson["attack_formation"] = stReqBody.m_iAttackCardID;
    dataJson["defend_formation"] = stReqBody.m_iDefendCardID;
    dataJson["attack_point"] = stReqBody.m_iAttackPoint;
    dataJson["defend_point"] = stReqBody.m_iDefendPoint;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}


int32_t CHttpProcessor::ProcessServiceDataReportRefreshPVP(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportRefreshPVP stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    auto to_json = [](CDataReportPVPInfo pvpInfo) -> Json {
        Json structData;
        structData["uin_id"] = pvpInfo.m_iUin;
        structData["rank_score"] = pvpInfo.m_iRank;
        return structData;
    };

    for(auto pvpInfo: stReqBody.m_stPVPInfo)
    {
        dataJson["pvpInfo"].emplace_back(to_json(pvpInfo));
    }
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportRewardPVP(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportRewardPVP stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["uin_id"]  = stReqBody.m_iUin;
    dataJson["rank_score"] = stReqBody.m_iRankScore;

    auto reward_to_json = [](CPackGameItem item) -> Json {
        Json structData;
        structData["item_type"] = item.m_iItemType;
        structData["item_id"] = item.m_iCardID;
        structData["item_num"] = item.m_iNum;
        return structData;
    };

    for(auto rewardItem : stReqBody.m_stReward)
    {
        dataJson["reward"].emplace_back(reward_to_json(rewardItem));
    }
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportChangeTeam(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportChangeTeam stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["before_formation"]  = stReqBody.m_stBeforeFormation;
    dataJson["after_formation"] = stReqBody.m_stAfterFormation;
    dataJson["team_type"] = stReqBody.m_iTeamType;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportCreateGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportCreateGuild stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["guild_id"]  = stReqBody.m_iGuildID;
    dataJson["guild_name"] = stReqBody.m_strGuildName;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportJoinGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportJoinGuild stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["guild_id"]  = stReqBody.m_iGuildID;
    dataJson["guild_name"] = stReqBody.m_strGuildName;
    dataJson["guild_uid"] = stReqBody.m_iGuildUid;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportLeaveGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportLeave stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["guild_id"]  = stReqBody.m_iGuildID;
    dataJson["guild_name"] = stReqBody.m_strGuildName;
    dataJson["guild_exp"] = stReqBody.m_iGuildExp;
    dataJson["activeness"] = stReqBody.m_iActiveness;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportChangeGuildName(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportChangeName stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["guild_id"]  = stReqBody.m_iGuildID;
    dataJson["guild_new_name"] = stReqBody.m_strGuildNewName;
    dataJson["guild_old_name"] = stReqBody.m_strGuildOldName;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportGetGuildInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportGetGuildInfo stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["guild_id"]  = stReqBody.m_iGuildID;
    dataJson["chief_uid"] = stReqBody.m_iChiefUid;
    dataJson["guild_exp"] = stReqBody.m_iGuildExp;
    dataJson["activeness"] = stReqBody.m_iActiveness;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportDonateCard(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportDonateCard stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["dst_uin"]  = stReqBody.m_iDestUin;
    dataJson["reward"] = stReqBody.m_iCardID;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportSetWishCard(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportSetWishCard stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["wish_card"] = stReqBody.m_iCardID;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportBuyGuildItem(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportBuyGuildItem stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["guild_id"]  = stReqBody.m_iGuildID;
    dataJson["guild_name"] = stReqBody.m_strGuildName;
    dataJson["store_type"] = stReqBody.m_iType;
    dataJson["item_index"] = stReqBody.m_iIndex;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportChoosePay(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportPayChoose stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["age_limit_index"]  = stReqBody.m_iIndex;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportPay(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportPay stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["deposit_id"] = stReqBody.m_iDepositID;
    dataJson["product_id"] = stReqBody.m_iProductID;
    dataJson["channel_id"] = stReqBody.m_iChannelID;
    dataJson["order_id"] = stReqBody.m_strOrderID;
    dataJson["pay_time"] = stReqBody.m_iPayTime;
    dataJson["pay_price"] = stReqBody.m_strPayPrice;
    dataJson["pay_method"] = stReqBody.m_strPayMethod;
    dataJson["is_gm"] = stReqBody.m_bIsGM;
    dataJson["is_first"] = stReqBody.m_bFirst;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportItemMonitor(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportItemChange stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["item_id"] = stReqBody.m_iItemID;
    dataJson["item_type"] = stReqBody.m_iType;
    dataJson["item_grade"] = stReqBody.m_iGrade;
    dataJson["item_count"] = stReqBody.m_iCount;
    dataJson["item_total_count"] = stReqBody.m_iTotal;
    dataJson["cmd"] = stReqBody.m_iCmd;
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}

int32_t CHttpProcessor::ProcessServiceDataReportOffline(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CDataReportOffline stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    Json dataJson;
    dataJson["time"] = CProcessor::GetNowSec();
    dataJson["name"] = stReqBody.m_stCommon.m_strNickName;
    dataJson["online_time"] = stReqBody.m_iOnlineTime;
    dataJson["register_date"] = stReqBody.m_iRegisterDate;

    // 数据上报
    auto result = CServerDataReport::SendDataReport(stHeadInfo.m_iServerID, to_string(stHeadInfo.m_iUin),stReqBody.m_stCommon,to_string(dataJson));
    Log(log_mask_detail, "[CHttpProcessor::%s]send dataReport id:%d|result:%s sendJson:%s\n", __FUNCTION__,stHeadInfo.m_usCmd,result.c_str(),stReqBody.m_stCommon.m_strEven.c_str());
    return success;
}


