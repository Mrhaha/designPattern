#include "MatchProcessor.h"
#include <fstream>
#include <random>
#include "tc_common.h"
#include "tc_md5.h"
#include "common_datetime.h"

#include "server_errcode.h"
#include "server_cmd_cross_define.h"
#include "server_cross_protocol.h"

#include "ServerConfig.h"
#include "GameControl.h"

#define DUMP_MATCH_DATA_FILE "./match_data"

CMatchProcessor::CMatchProcessor(void) : m_stAtkDef(this)
{
    m_stMatchPlayerVec.resize(200);
}

CMatchProcessor::~CMatchProcessor(void)
{

}

bool CMatchProcessor::Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID)
{
    if (!CProcessor::Initialize(nLogMask, pszLogFileName, nProcessID))
    {
        return false;
    }

    if (success != RecoveryDataFromFile())
    {
        Log(log_mask_system_error, "[CMatchProcessor:%s]RecoveryDataFromFile error!\n", __FUNCTION__);
        return false;
    }

    return true;
}

bool CMatchProcessor::Stop()
{
    SaveDataToFile();

    return CProcessor::Stop();
}

int32_t CMatchProcessor::ProcessMessages(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    RankLuckyHammer();

    int32_t iRet = fail;
    switch(stHeadInfo.m_usCmd)
    {
    case MSG_CROSS_UPDATE_MATCH_USER:
        iRet = ProcessUpdateMatchUser(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_FIGHT_MATCH_PVP:
        iRet = ProcessFightMatchPvp(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_FIGHT_FAIR_PVP:
        iRet = ProcessFightFairPvp(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_GET_ATTACK_DEFENSE_INFO:
        iRet = ProcessGetAttackDefenseInfo(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_FIGHT_ATTACK_DEFENSE:
        iRet = ProcessFightAttackDefense(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_FIGHT_ATTACK_DEFENSE_RESULT:
        iRet = ProcessFightAttackDefenseResult(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_REPAIR_ATTACK_DEFENSE:
        iRet = ProcessRepairAttackDefense(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_ATTACK_DEFENSE_SYNC:
        iRet = ProcessAttackDefenseSync(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_ATTACK_DEFENSE_SYNC_DONE:
        iRet = ProcessAttackDefenseSyncDone(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_GET_LUCKY_HAMMER_RANK:
        iRet = ProcessGetLuckyHammerRank(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_GET_LUCKY_HAMMER_LOG:
        iRet = ProcessGetLuckyHammerLog(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_LUCKY_HAMMER_LOG_SCORE:
        iRet = ProcessLuckyHammerLogScore(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_SYNC_LUCKY_HAMMER_SCORE:
        iRet = ProcessSyncLuckyHammerScore(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_CROSS_SYNC_LUCKY_HAMMER_LOG:
        iRet = ProcessSyncLuckyHammerLog(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    default:
        Log(log_mask_system_error, "[CMatchProcessor:%s]match recv cmd %u not process\n", __FUNCTION__, stHeadInfo.m_usCmd);
        break;
    }

    return iRet;
}

int32_t CMatchProcessor::ProcessUpdateMatchUser(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestUpdateMatchUser stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    for (const auto& user : stReqBody.m_stUserVec)
    {
        UpdateMatchPlayer(user.m_iUin, user.m_iGroupID, user.m_iMatchLevel, user.m_iPvpFightPower);
    }

    return success;
}

int32_t CMatchProcessor::ProcessFightMatchPvp(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestFightMatchPvp stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    //UpdateMatchPlayer(rstMsgHead.m_iUin, rstMsgHead.m_nGroupID, stReqBody.m_iMatchLevel, stReqBody.m_iMaxPvpFightPower);

    stReqBody.m_iHadMatchUin.push_back(stHeadInfo.m_iUin);

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    const auto* pstMatchConfig = cross_config->GetMatchPvpConfig().GetMatchRuleConfig(stReqBody.m_iMatchLevel + stReqBody.m_iBaodiRobotDelta);
    if (nullptr == pstMatchConfig || stReqBody.m_iMaxPvpFightPower <= 0)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::NO_MATCH_PVP_PLAYER);
    }

    const double min_power_ratio = stReqBody.m_iMaxPvpFightPower > pstMatchConfig->m_iNormalPower ? pstMatchConfig->m_dMatchNormalPowerMinRatio
        : (stReqBody.m_iMaxPvpFightPower <= pstMatchConfig->m_iMinimumPower ? pstMatchConfig->m_dMatchMinimumPowerMinRatio : pstMatchConfig->m_dMatchPowerMinRatio);
    const double max_power_ratio = stReqBody.m_iMaxPvpFightPower > pstMatchConfig->m_iNormalPower ? pstMatchConfig->m_dMatchNormalPowerMaxRatio
        : (stReqBody.m_iMaxPvpFightPower <= pstMatchConfig->m_iMinimumPower ? pstMatchConfig->m_dMatchMinimumPowerMaxRatio : pstMatchConfig->m_dMatchPowerMaxRatio);
    const int32_t min_power = stReqBody.m_iMaxPvpFightPower * min_power_ratio;
    const int32_t max_power = stReqBody.m_iMaxPvpFightPower * max_power_ratio;
    const int32_t min_level = pstMatchConfig->m_iMatchMinLevel;
    const int32_t max_level = pstMatchConfig->m_iMatchMaxLevel;
    const int32_t min_robot_info = pstMatchConfig->m_iMatchRobotInfoStart;
    const int32_t max_robot_info = pstMatchConfig->m_iMatchRobotInfoEnd;
    const int32_t min_robot_fight = pstMatchConfig->m_iMatchRobotFightStart;
    const int32_t max_robot_fight = pstMatchConfig->m_iMatchRobotFightEnd;
    if (min_level > max_level || min_power > max_power || min_robot_info > max_robot_info || min_robot_fight > max_robot_fight)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::NO_MATCH_PVP_PLAYER);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> power_dis(min_power, max_power);
    std::uniform_int_distribution<> level_dis(min_level, max_level);

    if (0 == stReqBody.m_iBaodiRobotDelta)
    {
        //50次随机选人
        int32_t iMatchUin = 0, iMatchGroupID = 0, iMatchSimilar = std::numeric_limits<int32_t>::max();
        for (auto count = 0; count < 50; ++count)
        {
            const int32_t level = level_dis(gen);
            const int32_t power = power_dis(gen);

            if (level >= int32_t(m_stMatchPlayerVec.size()))
            {
                continue;
            }

            const auto pstMatchPlayer = m_stMatchPlayerVec[level].GetPlayerByFightPower(power);
            if (pstMatchPlayer && pstMatchPlayer->m_iFightPower >= min_power && pstMatchPlayer->m_iFightPower <= max_power)
            {
                const bool not_matched = std::find(stReqBody.m_iHadMatchUin.begin(), stReqBody.m_iHadMatchUin.end(), pstMatchPlayer->m_iUin) == stReqBody.m_iHadMatchUin.end();
                if (not_matched && g_pGameCtrl->IsGroupConnecting(pstMatchPlayer->m_nGroupID))
                {
                    const double fight_ratio = fabs(pstMatchPlayer->m_iFightPower * 1.0 / stReqBody.m_iMaxPvpFightPower - 1);
                    const int32_t iSimilar = abs(level - stReqBody.m_iMatchLevel) * 20 + 200 * fight_ratio + 5 * abs(stHeadInfo.m_iGroupID - pstMatchPlayer->m_nGroupID);

                    if (iSimilar < iMatchSimilar)
                    {
                        iMatchSimilar = iSimilar;
                        iMatchUin = pstMatchPlayer->m_iUin;
                        iMatchGroupID = pstMatchPlayer->m_nGroupID;
                    }
                }
            }
        }

        if (iMatchUin > 0 && iMatchGroupID > 0 && g_pGameCtrl->IsGroupConnecting(iMatchGroupID))
        {
            CRequestCrossUserInfo stNotify;
            stNotify.m_iDstUin = stHeadInfo.m_iUin;
            stNotify.m_iDstGroup = stHeadInfo.m_iGroupID;
            NotifyMessage(iMatchGroupID, iMatchUin, MSG_CROSS_NOTIFY_BE_MATCHED, stNotify);
            return success;
        }
    }

    //5次随机选机器人
    CResponseFightMatchPvp stRspBody;
    std::uniform_int_distribution<> info_dis(min_robot_info, max_robot_info);
    std::uniform_int_distribution<> fight_dis(min_robot_fight, max_robot_fight);
    for (auto count = 0; count < 10; ++count)
    {
        const int32_t info = info_dis(gen);
        const int32_t fight = fight_dis(gen);

        const bool not_matched = std::find(stReqBody.m_iHadMatchUin.begin(), stReqBody.m_iHadMatchUin.end(), info) == stReqBody.m_iHadMatchUin.end();
        if (not_matched || count > 6)
        {
            stRspBody.m_stChallengerPlayer = cross_config->GetMatchPvpRobotConfig().GetRobotPvpChallenger(info, fight);
            if (0 != stRspBody.m_stChallengerPlayer.m_stBriefInfo.m_iUid)
            {
                return SendResponse(stHeadInfo, stRspBody);
            }
        }
    }

    return SendErrCode(stHeadInfo, SERVER_ERRCODE::NO_MATCH_PVP_PLAYER);
}

int32_t CMatchProcessor::ProcessFightFairPvp(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestFightFairPvp stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    m_stFairPlayer.UpdateFairPlayer(stHeadInfo.m_iUin, stHeadInfo.m_iGroupID, stReqBody.m_iTeamVec);

    std::random_device rd;
    std::mt19937 gen(rd());
    const auto& vecFairPlayer = m_stFairPlayer.GetFairPlayerVector();
    std::uniform_int_distribution<> index_dis(0, vecFairPlayer.size() - 1);

    for (size_t count = 0; count < 50; ++count)
    {
        const auto& stDstPlayer = vecFairPlayer[index_dis(gen)];
        if (stDstPlayer.m_iUin == stHeadInfo.m_iUin)
        {
            continue;
        }

        const auto found_card = [&](int32_t iCardID) {return std::find(stReqBody.m_iNotCardVec.begin(), stReqBody.m_iNotCardVec.end(), iCardID) != stReqBody.m_iNotCardVec.end();};
        const bool have_card = std::find_if(stDstPlayer.m_iCardVec.begin(), stDstPlayer.m_iCardVec.end(), found_card) != stDstPlayer.m_iCardVec.end();
        if (!have_card && stDstPlayer.m_iUin > 0 && stDstPlayer.m_nGroupID > 0 && g_pGameCtrl->IsGroupConnecting(stDstPlayer.m_nGroupID))
        {
            CRequestSelectFairPvpTeam notify;
            notify.m_iTeamVec = stDstPlayer.m_iCardVec;
            notify.m_iDstUin = stHeadInfo.m_iUin;
            notify.m_iDstGroup = stHeadInfo.m_iGroupID;
            NotifyMessage(stDstPlayer.m_nGroupID, stDstPlayer.m_iUin, MSG_CROSS_NOTIFY_FAIR_MATCHED, notify);
            return success;
        }
    }

    CResponseFightFairPvp stRspBody;
    SendResponse(stHeadInfo, stRspBody);
    return success;
}

int32_t CMatchProcessor::ProcessGetAttackDefenseInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestGetAttackDefenseInfo stReqBody;
    CResponseGetAttackDefenseInfo stRspBody;

    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    if (!m_stAtkDef.IsFighting())
    {
        return SendResponse(stHeadInfo, stRspBody);
    }

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    const auto* pstSelfAtkDef = m_stAtkDef.GetAttackDefenseByGuild(stReqBody.m_iGuildID);
    if (!pstSelfAtkDef || pstSelfAtkDef->m_iDstGuildID <= 0)
    {
        return SendResponse(stHeadInfo, stRspBody);
    }

    const auto* pstDstAtkDef = m_stAtkDef.GetAttackDefenseByGuild(pstSelfAtkDef->m_iDstGuildID);
    if (!pstDstAtkDef || pstDstAtkDef->m_iDstGuildID != stReqBody.m_iGuildID)
    {
        return SendResponse(stHeadInfo, stRspBody);
    }

    stRspBody.m_iSelfFightStar = pstDstAtkDef->m_iLoseAreaNum;
    stRspBody.m_iDstShowGroupID = cross_config->GetZooCrossConfig().GetShowIDByGroupID(pstDstAtkDef->m_iServerID);
    stRspBody.m_strDstGroupName = cross_config->GetZooCrossConfig().GetGroupNameByGroupID(pstDstAtkDef->m_iServerID);
    stRspBody.m_iDstGuildID = pstDstAtkDef->m_iGuildID;
    stRspBody.m_strDstGuildName = pstDstAtkDef->m_strGuildName;
    stRspBody.m_iDstFightStar = pstSelfAtkDef->m_iLoseAreaNum;

    for (const auto& area : pstSelfAtkDef->m_stAreaMap)
    {
        CPackGuildAttackDefenseArea stArea;
        stArea.m_iAreaType = area.second.m_iAreaType;
        stArea.m_strAreaFightBitmap = int2bin(area.second.m_uiFightBitmap);
        stArea.m_stChallengerPlayer = area.second.m_stPlayer;
        stArea.m_stChallengerPlayer.m_stCardInfoList.clear();
        // stArea.m_stChallengerPlayer.m_aiHonorIDVec.clear();
        stArea.m_strFightNameVec = area.second.m_strFightNameVec;
        stRspBody.m_stSelfAreaVec.push_back(std::move(stArea));
    }

    for (const auto& area : pstDstAtkDef->m_stAreaMap)
    {
        CPackGuildAttackDefenseArea stArea;
        stArea.m_iAreaType = area.second.m_iAreaType;
        stArea.m_strAreaFightBitmap = int2bin(area.second.m_uiFightBitmap);
        stArea.m_stChallengerPlayer = area.second.m_stPlayer;
        stArea.m_strFightNameVec = area.second.m_strFightNameVec;
        stRspBody.m_stDstAreaVec.push_back(std::move(stArea));
    }

    return SendResponse(stHeadInfo, stRspBody);
}

int32_t CMatchProcessor::ProcessFightAttackDefense(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestFightAttackDefense stReqBody;
    CResponseFightAttackDefense stRspBody;

    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    if (!m_stAtkDef.IsFighting())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_ADVANCE_END);
    }

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    const auto* pstSelfAtkDef = m_stAtkDef.GetAttackDefenseByGuild(stReqBody.m_iGuildID);
    if (!pstSelfAtkDef || pstSelfAtkDef->m_iDstGuildID <= 0)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::CANT_GUILD_WAR);
    }

    const auto* pstDstAtkDef = m_stAtkDef.GetAttackDefenseByGuild(pstSelfAtkDef->m_iDstGuildID);
    if (!pstDstAtkDef || pstDstAtkDef->m_iDstGuildID != stReqBody.m_iGuildID)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_NOT_FOUND);
    }

    if (pstSelfAtkDef->m_iLoseAreaNum >= 150 || pstDstAtkDef->m_iLoseAreaNum >= 150)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_ADVANCE_END);
    }

    if (m_stAtkDefChallenge.IsPlayerChallenging(stHeadInfo.m_iUin))
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::PLAYER_IN_CHALLENGING);
    }

    if (m_stAtkDefChallenge.IsPlayerBeChallenged(stReqBody.m_iDstUid, stReqBody.m_iDstDifficult))
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::ATK_DEF_AREA_CHALLENGING);
    }

    const auto iterDstArea = pstDstAtkDef->m_stAreaMap.find(stReqBody.m_iDstUid);
    if (iterDstArea == pstDstAtkDef->m_stAreaMap.end())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::ATK_DEF_AREA_NOT_FOUND);
    }

    if (stReqBody.m_iDstDifficult <= 0 || stReqBody.m_iDstDifficult >= 30)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::PARAMETER_ERROR);
    }

    const uint32_t uiFlag = 1 << stReqBody.m_iDstDifficult;
    if (iterDstArea->second.m_uiFightBitmap & uiFlag)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::ATK_DEF_AREA_LOST);
    }

    CAttackDefenseChallenge::value_type stChallenge;
    stChallenge.m_iGuildID = pstSelfAtkDef->m_iGuildID;
    stChallenge.m_iUin = stHeadInfo.m_iUin;
    stChallenge.m_nGroupID = stHeadInfo.m_iGroupID;
    stChallenge.m_strNick = stReqBody.m_strNick;
    stChallenge.m_iDstGuildID = pstSelfAtkDef->m_iDstGuildID;
    stChallenge.m_iDstUin = stReqBody.m_iDstUid;
    stChallenge.m_nDstGroupID = stReqBody.m_iDstGroupID;
    stChallenge.m_strDstNick = iterDstArea->second.m_stPlayer.m_stBriefInfo.m_strNick;
    stChallenge.m_iDstAreaType = iterDstArea->second.m_iAreaType;
    stChallenge.m_iDstDifficult = stReqBody.m_iDstDifficult;
    stChallenge.m_iChallengeTime = GetNowSec();
    m_stAtkDefChallenge.AddPlayerChallenge(stChallenge);

    stRspBody.m_stChallengerPlayer = iterDstArea->second.m_stPlayer;
    stRspBody.m_iDstAreaType = iterDstArea->second.m_iAreaType;
    stRspBody.m_iDstDifficult = stReqBody.m_iDstDifficult;
    return SendResponse(stHeadInfo, stRspBody);
}

int32_t CMatchProcessor::ProcessFightAttackDefenseResult(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestFightAttackDefenseResult stReqBody;
    CSResponseFightAttackDefenseResult stRspBody;

    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    if (!m_stAtkDef.IsFighting())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_ADVANCE_END);
    }

    const auto* pstChallenge = m_stAtkDefChallenge.GetChallengeByPlayer(stHeadInfo.m_iUin);
    if (!pstChallenge)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::PLAYER_NOT_CHALLENGING);
    }

    const auto* pstSelfAtkDef = m_stAtkDef.GetAttackDefenseByGuild(pstChallenge->m_iGuildID);
    if (!pstSelfAtkDef || pstSelfAtkDef->m_iDstGuildID != pstChallenge->m_iDstGuildID)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::NOT_ENOUGH_GUILD_LEVEL);
    }

    const auto* pstDstAtkDef = m_stAtkDef.GetAttackDefenseByGuild(pstSelfAtkDef->m_iDstGuildID);
    if (!pstDstAtkDef || pstDstAtkDef->m_iDstGuildID != pstChallenge->m_iGuildID)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_NOT_FOUND);
    }

    if (pstSelfAtkDef->m_iLoseAreaNum >= 150 || pstDstAtkDef->m_iLoseAreaNum >= 150)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_ADVANCE_END);
    }

    const auto iterDstArea = pstDstAtkDef->m_stAreaMap.find(pstChallenge->m_iDstUin);
    if (iterDstArea == pstDstAtkDef->m_stAreaMap.end())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::ATK_DEF_AREA_NOT_FOUND);
    }

    if (stReqBody.m_bResult && pstChallenge->m_iDstDifficult > 0 && pstChallenge->m_iDstDifficult < 30)
    {
        m_stAtkDef.AttackDefenseAreaOccupied(pstChallenge->m_iDstGuildID, pstChallenge->m_iDstUin, pstChallenge->m_iDstDifficult, pstChallenge->m_strNick);

        CSNotifyGuildWarReport notify;
        notify.m_iGuildID = pstChallenge->m_iGuildID;
        notify.m_iReportType = enmFightReport_AttackDefense;
        notify.m_iReportValue = enmFightReport_AttackDefense_Occupy;
        notify.m_stParamMap = {{"nick", pstChallenge->m_strNick }, {"dst_area", pstChallenge->m_strDstNick}, {"num", std::to_string(pstChallenge->m_iDstDifficult)}};
        NotifyMessage(stHeadInfo.m_iServerID, 0, MSG_CROSS_GUILD_WAR_REPORT_NOTIFY, notify);

        notify.m_iGuildID = pstChallenge->m_iDstGuildID;
        notify.m_iReportType = enmFightReport_AttackDefense;
        notify.m_iReportValue = enmFightReport_AttackDefense_Occupied;
        notify.m_stParamMap = { { "dst_nick", pstChallenge->m_strNick },{ "area", pstChallenge->m_strDstNick },{ "num", std::to_string(pstChallenge->m_iDstDifficult) } };
        NotifyMessage(pstDstAtkDef->m_iServerID, 0, MSG_CROSS_GUILD_WAR_REPORT_NOTIFY, notify);
    }

    stRspBody.m_bResult = stReqBody.m_bResult;
    stRspBody.m_iDstAreaType = iterDstArea->second.m_iAreaType;
    stRspBody.m_iDstDifficult = pstChallenge->m_iDstDifficult;
    stRspBody.m_stChallengerPlayer = iterDstArea->second.m_stPlayer;
    stRspBody.m_strFightNameVec = iterDstArea->second.m_strFightNameVec;

    m_stAtkDefChallenge.RemoveChallengeByPlayer(stHeadInfo.m_iUin);

    return SendResponse(stHeadInfo, stRspBody);
}

int32_t CMatchProcessor::ProcessRepairAttackDefense(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestRepairAttackDefense stReqBody;
    CResponseRepairAttackDefense stRspBody;

    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    if (!m_stAtkDef.IsFighting())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_ADVANCE_END);
    }

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    const auto* pstSelfAtkDef = m_stAtkDef.GetAttackDefenseByGuild(stReqBody.m_iGuildID);
    if (!pstSelfAtkDef || pstSelfAtkDef->m_iDstGuildID <= 0)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::CANT_GUILD_WAR);
    }

    const auto* pstDstAtkDef = m_stAtkDef.GetAttackDefenseByGuild(pstSelfAtkDef->m_iDstGuildID);
    if (!pstDstAtkDef || pstDstAtkDef->m_iDstGuildID != stReqBody.m_iGuildID)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_NOT_FOUND);
    }

    if (pstSelfAtkDef->m_iLoseAreaNum >= 150 || pstDstAtkDef->m_iLoseAreaNum >= 150)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::GUILD_WAR_ADVANCE_END);
    }

    const auto* pstChallenge = m_stAtkDefChallenge.GetChallengeByPlayer(stHeadInfo.m_iUin);
    if (pstChallenge)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::PLAYER_IN_CHALLENGING);
    }

    const auto iterDstArea = pstSelfAtkDef->m_stAreaMap.find(stReqBody.m_iDstUid);
    if (iterDstArea == pstSelfAtkDef->m_stAreaMap.end())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::ATK_DEF_AREA_NOT_FOUND);
    }

    stRspBody.m_iDstUid = stReqBody.m_iDstUid;
    stRspBody.m_iDstGroupID = stReqBody.m_iDstGroupID;
    stRspBody.m_iRepairAreaType = iterDstArea->second.m_iAreaType;

    for (int32_t index = 30; index > 0; --index)
    {
        const uint32_t flag = 1 << index;
        if (iterDstArea->second.m_uiFightBitmap & flag)
        {
            m_stAtkDef.AttackDefenseAreaRepaired(pstSelfAtkDef->m_iGuildID, stReqBody.m_iDstUid, index);

            CSNotifyGuildWarReport notify;
            notify.m_iGuildID = pstSelfAtkDef->m_iGuildID;
            notify.m_iReportType = enmFightReport_AttackDefense;
            notify.m_iReportValue = enmFightReport_AttackDefense_Repair;
            notify.m_stParamMap = { { "nick", stReqBody.m_strNick },{ "area", iterDstArea->second.m_stPlayer.m_stBriefInfo.m_strNick },{ "num", std::to_string(index) } };
            NotifyMessage(stHeadInfo.m_iServerID, 0, MSG_CROSS_GUILD_WAR_REPORT_NOTIFY, notify);

            notify.m_iGuildID = pstSelfAtkDef->m_iDstGuildID;
            notify.m_iReportType = enmFightReport_AttackDefense;
            notify.m_iReportValue = enmFightReport_AttackDefense_Repaired;
            notify.m_stParamMap = { { "dst_nick", stReqBody.m_strNick },{ "dst_area",iterDstArea->second.m_stPlayer.m_stBriefInfo.m_strNick },{ "num", std::to_string(index) } };
            NotifyMessage(pstDstAtkDef->m_iServerID, 0, MSG_CROSS_GUILD_WAR_REPORT_NOTIFY, notify);

            stRspBody.m_iRepairDifficult = index;
            stRspBody.m_strAreaFightBitmap = int2bin(iterDstArea->second.m_uiFightBitmap);
            stRspBody.m_strFightNameVec = iterDstArea->second.m_strFightNameVec;
            return SendResponse(stHeadInfo, stRspBody);
        }
    }
    return SendErrCode(stHeadInfo, SERVER_ERRCODE::ATK_DEF_AREA_NOT_LOST);
}

int32_t CMatchProcessor::ProcessAttackDefenseSync(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestAttackDefenseSync stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    if (m_stAtkDef.IsFighting() || stReqBody.m_iAtkDefScore <= 0)
    {
        return success;
    }

    if(stReqBody.m_iGuildID <= 0)
    {
        return fail;
    }

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    static const std::map<int32_t, int32_t> area_type{{3, 3}, {10, 2}, {std::numeric_limits<int32_t>::max(), 1}};

    CPackAttackDefense stInfo;
    stInfo.m_iGuildID = stReqBody.m_iGuildID;
    stInfo.m_iGuildExp = stReqBody.m_iGuildExp;
    stInfo.m_iGuildLevel = stReqBody.m_iGuildLevel;
    stInfo.m_strGuildName = stReqBody.m_strGuildName;
    stInfo.m_iServerID = stHeadInfo.m_iServerID;
    stInfo.m_iOpenDay = abs(day_diff(cross_config->GetZooCrossConfig().GetGroupOpenTime(stHeadInfo.m_iServerID), GetNowSec()));
    stInfo.m_iDstGuildID = 0;
    stInfo.m_iActiveMemberCount = stReqBody.m_iActiveMemberCount;
    stInfo.m_iAtkDefScore = stReqBody.m_iAtkDefScore;
    stInfo.m_stAtkDefMatchHistory = stReqBody.m_stMatchHistory;
    for (size_t i = 0 ; i < stReqBody.m_stPlayerVec.size(); ++i)
    {
        const auto& player = stReqBody.m_stPlayerVec[i];
        CPackAttackDefenseArea stArea;
        stArea.m_iUin = player.m_stBriefInfo.m_iUid;
        stArea.m_nGroupID = player.m_stBriefInfo.m_iGroupID;
        stArea.m_iAreaType = area_type.lower_bound(i + 1)->second;
        stArea.m_uiFightBitmap = 0;
        stArea.m_stPlayer = player;
        stArea.m_strFightNameVec.resize(5);

        stInfo.m_stAreaMap.emplace(player.m_stBriefInfo.m_iUid, std::move(stArea));
    }

    Log(log_mask_system_error, "[CMatchProcessor:%s]server(%d) guild id %d exp=%d area=%zu to match\n"
        , __FUNCTION__, stHeadInfo.m_iServerID, stInfo.m_iGuildID, stInfo.m_iGuildExp, stReqBody.m_stPlayerVec.size());

    m_stAtkDef.AddAttackDefenseGuild(stInfo, stReqBody.m_bIsTibu);

    return success;
}

int32_t CMatchProcessor::ProcessAttackDefenseSyncDone(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    if (m_stAtkDef.IsFighting())
    {
        return fail;
    }

    auto logicIter = m_stAtkDefPrepareStat.find(stHeadInfo.m_iServerID);
    if(logicIter != m_stAtkDefPrepareStat.end())
    {
        logicIter->second.second = true;
    }

    return success;
}

int32_t CMatchProcessor::ProcessGetLuckyHammerRank(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGetLuckyHammerRank stReqBody;
    CResponseGetLuckyHammerRank stRspBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    stRspBody.m_iActivityID = stReqBody.m_iActivityID;
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    const int32_t iActivityServerID = cross_config->GetZooActivityConfig().GetActivityServer(stHeadInfo.m_iServerID);
    const auto key = std::make_pair(stReqBody.m_iActivityID, iActivityServerID);
    const auto iterActivity = m_stLuckyHammerMap.find(key);
    if (iterActivity != m_stLuckyHammerMap.end())
    {
        for (size_t i = 0 ; i < iterActivity->second.m_stRank.size(); ++i)
        {
            CPackUserRank rank;
            rank.m_iUid = iterActivity->second.m_stRank[i].m_iUid;
            rank.m_iGroupID = iterActivity->second.m_stRank[i].m_iGroupID;
            rank.m_strNick = iterActivity->second.m_stRank[i].m_strNick;
            rank.m_iRankScore = iterActivity->second.m_stRank[i].m_iScore;
            rank.m_iRank = i + 1;
            rank.m_iShowGroupID = cross_config->GetZooCrossConfig().GetShowIDByGroupID(rank.m_iGroupID);
            rank.m_strGroupName = cross_config->GetZooCrossConfig().GetGroupNameByGroupID(rank.m_iGroupID);
            stRspBody.m_stRankVec.push_back(rank);

            if (rank.m_iUid == stHeadInfo.m_iUin && rank.m_iGroupID == stHeadInfo.m_iGroupID)
            {
                stRspBody.m_iSelfRank = rank.m_iRank;
            }
        }
    }
    return SendResponse(stHeadInfo, stRspBody);
}

int32_t CMatchProcessor::ProcessGetLuckyHammerLog(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestGetLuckyHammerLog stReqBody;
    CResponseGetLuckyHammerLog stRspBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    stRspBody.m_iActivityID = stReqBody.m_iActivityID;
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    const int32_t iActivityServerID = cross_config->GetZooActivityConfig().GetActivityServer(stHeadInfo.m_iServerID);
    const auto key = std::make_pair(stReqBody.m_iActivityID, iActivityServerID);
    const auto iterActivity = m_stLuckyHammerMap.find(key);
    if (iterActivity != m_stLuckyHammerMap.end())
    {
        for (const auto& log : iterActivity->second.m_stLog)
        {
            CPackLuckyHammerLogElem pack;
            pack.m_iLogID = log.m_iLogID;
            pack.m_strText = log.m_strContent;
            pack.m_bLucky = log.m_bLucky;
            pack.m_bAward = log.m_iLuckyUin.find(stHeadInfo.m_iUin) != log.m_iLuckyUin.end();
            stRspBody.m_stLog.push_back(pack);
        }
    }
    return SendResponse(stHeadInfo, stRspBody);
}

int32_t CMatchProcessor::ProcessLuckyHammerLogScore(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CRequestLuckyHammerLogScore stReqBody;
    CResponseLuckyHammerLogScore stRspBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    const int32_t iActivityServerID = cross_config->GetZooActivityConfig().GetActivityServer(stHeadInfo.m_iServerID);
    const auto key = std::make_pair(stReqBody.m_iActivityID, iActivityServerID);
    const auto iterActivity = m_stLuckyHammerMap.find(key);
    if (iterActivity == m_stLuckyHammerMap.end())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }

    CPackLuckyHammerLog * pstLog = nullptr;
    for (auto& log : iterActivity->second.m_stLog)
    {
        if (log.m_iLogID == stReqBody.m_iLogID)
        {
            pstLog = &log;
            break;
        }
    }

    if (pstLog == nullptr)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::LUCKY_HAMMER_LOG_NOT_FOUND);
    }

    if (!pstLog->m_bLucky)
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::LUCKY_HAMMER_LOG_NOT_LUCKY);
    }

    if (pstLog->m_iLuckyUin.find(stHeadInfo.m_iUin) != pstLog->m_iLuckyUin.end())
    {
        return SendErrCode(stHeadInfo, SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
    }

    pstLog->m_iLuckyUin.insert(stHeadInfo.m_iUin);

    stRspBody.m_iActivityID = stReqBody.m_iActivityID;
    stRspBody.m_iLogID = stReqBody.m_iLogID;
    for (const auto& log : iterActivity->second.m_stLog)
    {
        CPackLuckyHammerLogElem pack;
        pack.m_iLogID = log.m_iLogID;
        pack.m_strText = log.m_strContent;
        pack.m_bLucky = log.m_bLucky;
        pack.m_bAward = log.m_iLuckyUin.find(stHeadInfo.m_iUin) != log.m_iLuckyUin.end();
        stRspBody.m_stLog.push_back(pack);
    }
    return SendResponse(stHeadInfo, stRspBody);
}

int32_t CMatchProcessor::ProcessSyncLuckyHammerScore(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestSyncLuckyHammerScore stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    const int32_t iNow = GetNowSec();
    stReqBody.m_iEndTime += 5_min;
    if (iNow < stReqBody.m_iStartTime || iNow > stReqBody.m_iEndTime)
    {
        return fail;
    }

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    const int32_t iActivityServerID = cross_config->GetZooActivityConfig().GetActivityServer(stHeadInfo.m_iServerID);
    const auto key = std::make_pair(stReqBody.m_iActivityID, iActivityServerID);
    auto& stLuckyHammerInfo = m_stLuckyHammerMap[key];
    stLuckyHammerInfo.m_iStartTime = stReqBody.m_iStartTime;
    stLuckyHammerInfo.m_iEndTime = stReqBody.m_iEndTime;
    bool bFound = false;
    for (auto& rank : stLuckyHammerInfo.m_stRank)
    {
        if (rank.m_iUid == stHeadInfo.m_iUin && rank.m_iGroupID == stHeadInfo.m_iGroupID)
        {
            rank.m_strNick = stReqBody.m_strNick;
            rank.m_iScore = stReqBody.m_iScore;
            bFound = true;
        }
    }
    if (!bFound)
    {
        CPackLuckyHammerRank rank;
        rank.m_iUid = stHeadInfo.m_iUin;
        rank.m_iGroupID = stHeadInfo.m_iGroupID;
        rank.m_iScore = stReqBody.m_iScore;
        rank.m_strNick = stReqBody.m_strNick;
        stLuckyHammerInfo.m_stRank.push_back(rank);
    }
    std::stable_sort(stLuckyHammerInfo.m_stRank.begin(), stLuckyHammerInfo.m_stRank.end(), [&](const auto& a, const auto& b){
        return a.m_iScore > b.m_iScore;
    });

    if (stLuckyHammerInfo.m_stRank.size() > 100)
    {
        stLuckyHammerInfo.m_stRank.resize(100);
    }

    return success;
}

int32_t CMatchProcessor::ProcessSyncLuckyHammerLog(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CSRequestSyncLuckyHammerLog stReqBody;
    if (!DecodeMsgPack(stReqBody, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    const int32_t iNow = GetNowSec();
    stReqBody.m_iEndTime += 5_min;
    if (iNow < stReqBody.m_iStartTime || iNow > stReqBody.m_iEndTime)
    {
        return fail;
    }

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    const int32_t iActivityServerID = cross_config->GetZooActivityConfig().GetActivityServer(stHeadInfo.m_iServerID);
    const auto key = std::make_pair(stReqBody.m_iActivityID, iActivityServerID);
    auto& stLuckyHammerInfo = m_stLuckyHammerMap[key];
    stLuckyHammerInfo.m_iStartTime = stReqBody.m_iStartTime;
    stLuckyHammerInfo.m_iEndTime = stReqBody.m_iEndTime;

    CPackLuckyHammerLog log;
    log.m_iLogID = stLuckyHammerInfo.m_stLog.empty() ? 1 : stLuckyHammerInfo.m_stLog.back().m_iLogID + 1;
    log.m_strContent = stReqBody.m_strContent;
    log.m_bLucky = stReqBody.m_iContentNotify == 3;
    stLuckyHammerInfo.m_stLog.push_back(log);

    while (stLuckyHammerInfo.m_stLog.size() > 50)
    {
        int32_t iLuckyCount = 0;
        for (const auto& log_info : stLuckyHammerInfo.m_stLog)
        {
            if (log_info.m_bLucky)
            {
                ++iLuckyCount;
            }
        }

        if (iLuckyCount > 20)
        {
            stLuckyHammerInfo.m_stLog.pop_front();
        }
        else
        {
            for (auto iterLog = stLuckyHammerInfo.m_stLog.begin(); iterLog != stLuckyHammerInfo.m_stLog.end(); ++iterLog)
            {
                if (!iterLog->m_bLucky)
                {
                    stLuckyHammerInfo.m_stLog.erase(iterLog);
                    break;
                }
            }
        }
    }

    const auto iterSet = cross_config->GetZooActivityConfig().m_stServerMap.find(iActivityServerID);
    if (iterSet != cross_config->GetZooActivityConfig().m_stServerMap.end())
    {
        CSNotifyBroadcast notify;
        notify.m_strBroadcastID = stReqBody.m_strBroadcastID;
        notify.m_stParamMap = stReqBody.m_stParamMap;
        for(auto groupID : iterSet->second)
        {
            NotifyMessage(groupID, 0, MSG_CROSS_NOTIFY_CROSS_BROADCAST, notify);
        }
    }

    return success;
}

int32_t CMatchProcessor::UpdateMatchPlayer(int32_t iUin, int16_t nGroupID, int32_t iMatchLevel, int32_t iPower)
{
    for (size_t level = 0; level < m_stMatchPlayerVec.size(); ++level)
    {
        m_stMatchPlayerVec[level].RemovePlayerByRole(iUin);
        if (int32_t(level) == iMatchLevel)
        {
            m_stMatchPlayerVec[level].AddPlayer(iUin, nGroupID, iPower);
        }
    }

    return success;
}

int32_t CMatchProcessor::RankLuckyHammer()
{
    const int32_t iNow = GetNowSec();
    std::vector<decltype(m_stLuckyHammerMap)::key_type> deleteHammer;

    for (const auto& hammer : m_stLuckyHammerMap)
    {
        const int32_t iActivityID = hammer.first.first;

        if (iNow > hammer.second.m_iEndTime)
        {
            deleteHammer.push_back(hammer.first);
            const auto& rank = hammer.second.m_stRank;

            for (size_t index = 0; index < rank.size(); ++index)
            {
                CSNotifyLuckyHammerRank notify;
                notify.m_iActivityID = iActivityID;
                notify.m_iRank = index + 1;
                notify.m_iScore = rank[index].m_iScore;
                NotifyMessage(rank[index].m_iGroupID, rank[index].m_iUid, MSG_CROSS_NOTIFY_LUCKY_HAMMER_RANK_AWARD, notify);

                Log(log_mask_system_error, "[CMatchProcessor:%s]group(%d) uin %d score is %d, get rank %d award!!!\n"
                    , __FUNCTION__, rank[index].m_iGroupID, rank[index].m_iUid, rank[index].m_iScore, notify.m_iRank);
            }
        }
    }

    while (!deleteHammer.empty())
    {
        m_stLuckyHammerMap.erase(deleteHammer.back());
        deleteHammer.pop_back();
    }

    return success;
}

int32_t CMatchProcessor::RecoveryDataFromFile()
{
    constexpr int32_t md5_length = 32;
    std::ifstream ifs(DUMP_MATCH_DATA_FILE);
    std::string strData = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    ifs.close();

    if (strData.empty())
    {
        return success;
    }

    if (strData.length() < md5_length)
    {
        Log(log_mask_system_error, "[CMatchProcessor:%s] match data file recovery fail, lack data\n" , __FUNCTION__);
        return fail;
    }

    std::string_view data_view = strData;
    auto md5_view = data_view.substr(0, 32);
    auto msg_view = data_view.substr(32);

    const auto calc_md5 = taf::TC_MD5::md5str(msg_view);
    if (md5_view != calc_md5)
    {
        Log(log_mask_system_error, "[CMatchProcessor:%s] match data file recovery fail, calc md5 is %s, not equal\n" , __FUNCTION__, calc_md5.c_str());
        return fail;
    }

    CPackMatchRecovery stRecoveryData;
    if (!DecodeMsgPack(stRecoveryData, msg_view.data(), msg_view.size()))
    {
        Log(log_mask_system_error, "[CMatchProcessor:%s] match data file recovery fail, decode msg pack fail\n" , __FUNCTION__);
        return fail;
    }

    //恢复Guild Attack Defense
    m_stAtkDef.RecoveryFromGuildWar(stRecoveryData.m_stGuildWar);

    //恢复Match Pvp
    for (size_t i = 0 ; i < stRecoveryData.m_stMatchPvpPlayer.size() && i < m_stMatchPlayerVec.size(); ++i)
    {
        for (const auto& player : stRecoveryData.m_stMatchPvpPlayer[i])
        {
            m_stMatchPlayerVec[i].AddPlayer(player.m_iUin, player.m_nGroupID, player.m_iFightPower);
        }
    }

    //恢复Fair Pvp
    for (const auto& player : stRecoveryData.m_stFairPvpPlayer)
    {
        m_stFairPlayer.UpdateFairPlayer(player.m_iUin, player.m_nGroupID, player.m_iCardVec);
    }

    //恢复Lucky Hammer
    m_stLuckyHammerMap = stRecoveryData.m_stLuckyHammer;

    return success;
}

int32_t CMatchProcessor::SaveDataToFile()
{
    CPackMatchRecovery stRecoveryData;

    //保存Guild Attack Defense
    m_stAtkDef.DumpToGuildWar(stRecoveryData.m_stGuildWar);

    //保存 Match Pvp
    stRecoveryData.m_stMatchPvpPlayer.resize(m_stMatchPlayerVec.size());
    for (size_t i = 0; i < m_stMatchPlayerVec.size(); ++i)
    {
        stRecoveryData.m_stMatchPvpPlayer[i] = m_stMatchPlayerVec[i].GetMatchPlayerList();
    }

    //保存 Fair Pvp
    stRecoveryData.m_stFairPvpPlayer = m_stFairPlayer.GetFairPlayerVector();

    //保存 Lucky Hammer
    stRecoveryData.m_stLuckyHammer = m_stLuckyHammerMap;

    msgpack::sbuffer stBuffer;
    msgpack::pack(stBuffer, stRecoveryData);
    const std::string strTotalData(stBuffer.data(), stBuffer.size());
    const std::string md5 = taf::TC_MD5::md5str(strTotalData);

    std::ofstream ofs(DUMP_MATCH_DATA_FILE, std::ofstream::out | std::ofstream::binary);
    ofs << md5 << strTotalData;
    ofs.close();

    return success;
}

void CMatchProcessor::Refresh()
{
    // 刷新公会战
    AttackDefenseRefresh();
}

void CMatchProcessor::AttackDefenseRefresh()
{
    const int32_t iNow = GetNowSec();
    static int32_t iLastCheckTime = 0;
    // 1秒刷新一次
    if(iLastCheckTime == iNow)
        return;

    iLastCheckTime = iNow;

    if(!m_stAtkDef.IsFighting() && IsAttackDefensePrepare())
    {// 准备阶段，向各个Logic请求数据
        AttackDefensePrePare();
    }
    else if (!m_stAtkDef.IsFighting() && IsAttackDefenseFighting())
    {//战役开始，匹配ing
        AttackDefenseToBegin();
    }
    else if (m_stAtkDef.IsFighting() && IsAttackDefenseFightingExt())
    {//战役进行中，强制结束挑战超时的挑战
        AttackDefenseChallenging();
    }
    else if (m_stAtkDef.IsFighting() && !IsAttackDefenseFightingExt())
    {//战役结束，发奖ing
        AttackDefenseToEnd();
    }
}

void CMatchProcessor::AttackDefensePrePare()
{
    std::shared_ptr<CCrossServerConfig> config;
    CCrossServerConfig::GetConfig(config);

    if(m_stAtkDefPrepareStat.empty())
    {
        for (const auto& logic : config->GetZooCrossConfig().m_stLogicConfigMap)
        {
            m_stAtkDefPrepareStat[logic.first] = std::make_pair(0, false);
        }
    }

    const int32_t iNow = GetNowSec();
    bool isAllPrepare = true;
    for(auto& logic : m_stAtkDefPrepareStat)
    {
        if(!logic.second.second) isAllPrepare = false;

        if(logic.second.second || (iNow - logic.second.first < 60)) continue;

        // 一次发送一个服务器数据同步请求
        std::string stEmptyBody;
        if(NotifyMessage(logic.first, 0, MSG_CROSS_ATTACK_DEFENSE_PREPARE, stEmptyBody) != success) continue;

        logic.second.first = iNow;

        break;
    }

    if(isAllPrepare && config->GetGuildWarConfig().m_stInfo.m_bAvyPrepareEndBegin)
    {
        // 所有服务器数据同步完成，且m_bAvyPrepareEndBegin=true
        AttackDefenseToBegin();
    }
}

int32_t CMatchProcessor::AttackDefenseToBegin()
{
    // 打印日志，将所有未同步成功的服务器打印出来
    for(auto& logic : m_stAtkDefPrepareStat)
    {
        if(!logic.second.second)
        {
            Log(log_mask_system_error, "[CMatchProcessor:%s] AttackDefensePrepare %d server request data fail !!!!!!!!!\n", __FUNCTION__, logic.first);
        }
    }

    m_stAtkDef.ToBegin();
    return success;
}

int32_t CMatchProcessor::AttackDefenseChallenging()
{
    const auto* pstChallenge = m_stAtkDefChallenge.GetChallengeByEarliest();
    if (pstChallenge && pstChallenge->m_iChallengeTime < int32_t(GetNowSec() - 3_min))
    {
        CSResponseFightAttackDefenseResult notify;
        notify.m_bResult = false;
        notify.m_bTimeOut = true;
        notify.m_iDstAreaType = pstChallenge->m_iDstAreaType;
        notify.m_iDstDifficult = pstChallenge->m_iDstDifficult;
        NotifyMessage(pstChallenge->m_nGroupID, pstChallenge->m_iUin, MSG_CROSS_FIGHT_ATTACK_DEFENSE_RESULT, notify);

        m_stAtkDefChallenge.RemoveChallengeByPlayer(pstChallenge->m_iUin);
    }
    return success;
}

int32_t CMatchProcessor::AttackDefenseToEnd()
{
    const auto& infos = m_stAtkDef.GetAttackDefenseList();
    for (const auto& info : infos)
    {
        if (info.m_iDstGuildID > info.m_iGuildID)
        {
            const auto* dst = m_stAtkDef.GetAttackDefenseByGuild(info.m_iDstGuildID);
            if (!dst || dst->m_iDstGuildID != info.m_iGuildID)
            {
                continue;
            }

            CSNotifyAttackDefenseAward notify;
            notify.m_iGuildID = info.m_iGuildID;
            notify.m_iStar = dst->m_iLoseAreaNum;
            notify.m_iDstGuildID = info.m_iDstGuildID;
            notify.m_strDstGuildName = dst->m_strGuildName;
            notify.m_iDstStar = info.m_iLoseAreaNum;
            Log(log_mask_system_error, "[CMatchProcessor:%s]guild(%d-%d) vs guild(%d-%d) star %d vs %d\n"
                    , __FUNCTION__, info.m_iServerID, info.m_iGuildID, dst->m_iServerID, dst->m_iGuildID, dst->m_iLoseAreaNum, info.m_iLoseAreaNum);
            NotifyMessage(info.m_iServerID, 0, MSG_CROSS_ATTACK_DEFENSE_AWARD, notify);

            notify.m_iGuildID = dst->m_iGuildID;
            notify.m_iStar = info.m_iLoseAreaNum;
            notify.m_iDstGuildID = dst->m_iDstGuildID;
            notify.m_strDstGuildName = info.m_strGuildName;
            notify.m_iDstStar = dst->m_iLoseAreaNum;
            Log(log_mask_system_error, "[CMatchProcessor:%s]guild(%d-%d) vs guild(%d-%d) star %d vs %d\n"
                    , __FUNCTION__, dst->m_iServerID, dst->m_iGuildID, info.m_iServerID, info.m_iGuildID, info.m_iLoseAreaNum, dst->m_iLoseAreaNum);
            NotifyMessage(dst->m_iServerID, 0, MSG_CROSS_ATTACK_DEFENSE_AWARD, notify);
        }
    }

    decltype(m_stAtkDef) atk_def(this);
    std::swap(m_stAtkDef, atk_def);

    // 清空攻防战战斗数据
    m_stAtkDefChallenge.ClearChallengeInfo();
    m_stAtkDefPrepareStat.clear();

    SaveDataToFile();

    return success;
}

bool CMatchProcessor::IsAttackDefenseFighting()
{
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    const int32_t iNowSec = GetNowSec();
    const int32_t iMondayTime = get_monday_time(iNowSec);
    const auto& config = cross_config->GetGuildWarConfig();
    for (auto iter = config.m_stTimeMap.lower_bound("attack_defense"); iter != config.m_stTimeMap.upper_bound("attack_defense"); ++iter)
    {
        const int32_t iStartTime = iMondayTime + (iter->second.m_iStartWeekDay - 1) * 24 * 60 * 60
                                   + iter->second.m_iStartHour * 60 * 60 + iter->second.m_iStartMinute * 60;

        const int32_t iEndTime = iMondayTime + (iter->second.m_iEndWeekDay - 1) * 24 * 60 * 60
                                 + iter->second.m_iEndHour * 60 * 60 + iter->second.m_iEndMinute * 60;

        if (iNowSec >= iStartTime && iNowSec <= iEndTime)
        {
            return true;
        }
    }
    return false;
}

// 是否在活动开始阶段，或者是否在准备阶段且m_bAvyPrepareEndBegin=true
bool CMatchProcessor::IsAttackDefenseFightingExt()
{
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    return (IsAttackDefenseFighting() || (cross_config->GetGuildWarConfig().m_stInfo.m_bAvyPrepareEndBegin && IsAttackDefensePrepare()));
}

bool CMatchProcessor::IsAttackDefensePrepare()
{
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    const int32_t iNowSec = GetNowSec();
    const int32_t iMondayTime = get_monday_time(iNowSec);
    const auto& config = cross_config->GetGuildWarConfig();
    for (auto iter = config.m_stTimeMap.lower_bound("attack_defense"); iter != config.m_stTimeMap.upper_bound("attack_defense"); ++iter)
    {
        const int32_t iStartTime = iMondayTime + (iter->second.m_iStartWeekDay - 1) * 24 * 60 * 60
                                   + iter->second.m_iStartHour * 60 * 60 + iter->second.m_iStartMinute * 60;

        if (iNowSec >= (iStartTime-config.m_stInfo.m_iAvyPrepareTime) && iNowSec < iStartTime)
        {
            return true;
        }
    }
    return false;
}
