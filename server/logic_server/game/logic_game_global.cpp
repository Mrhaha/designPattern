//
// Created by hzl on 2019/5/28.
//

#include <server_user_protocol.h>
#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_cross_protocol.h"
#include "common_datetime.h"
#include "logic_link_common.h"
#include "logic_config.h"
#include "logic_game_global.h"
#include "logic_game_data_recovery.h"
#include "logic_game_active.h"
#include "logic_game_active_info.h"
#include "logic_game_cache_common.h"
#include "logic_game_global.h"
#include "logic_game_user.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_global_frame.h"
#include "logic_game_common_publisher.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"
#include "logic_record_global_guild_war_rank_table.h"

TLogicGlobalCache* CLogicGlobal::GetGlobalCache()
{
    static TLogicGlobalCache* s_pstGlobalCacheTable = nullptr;

    if (s_pstGlobalCacheTable == nullptr)
    {
        s_pstGlobalCacheTable = GetGlobalCacheFromDB();
    }

    assert(s_pstGlobalCacheTable);
    //////////////////////////////////////////////////////////////////////////
    ///////////// PVP排行榜为空, 需要从配置初始化排行榜
    if (s_pstGlobalCacheTable->m_stPvpRank.empty())
    {
        for (const auto& stVal : CLogicConfig::GetPVPRobotConfig().m_stRobotList)
        {

            user_info_key_t key(stVal.first);
            TGlobalPvpRankTableValueType value;
            value.m_iUid = key.m_stRoleIDDetail.m_iUin;
            value.m_iGroupID = key.m_stRoleIDDetail.m_iGroupID = 0;
            value.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
            value.m_ulRankScore = stVal.second.m_iPvpScore;
            s_pstGlobalCacheTable->m_stPvpRank.Insert(std::make_pair(key.m_ulRoleID, value));
        }
    }

    return s_pstGlobalCacheTable;
}

TLogicGlobalCache* CLogicGlobal::GetGlobalCacheFromDB()
{
    auto pstGlobalData = std::make_unique<TLogicGlobalCache>();
    const auto iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;

    auto stGlobalBasicList = CRecordGlobalBasicTable::SelectDataFromSQL(iServerID);
    if (stGlobalBasicList.empty())
    {
        TGlobalBasicTableValueType value;
        value.m_iServerID = iServerID;
        value.m_stAffixVec = CLogicConfig::GetEquipAttrConfig().GetRandAffix();
        pstGlobalData->m_stBasicTable.Initialize(value);
    }
    else
    {
        pstGlobalData->m_stBasicTable.InitializeWithoutSQL(stGlobalBasicList[0]);
    }

    auto stCharmRankList = CRecordGlobalCharmRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stCharmRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stCharmRank.InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stCountdownRankList = CRecordGlobalCountdownRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stCountdownRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stCountdownRank.InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stEntireRankList = CRecordGlobalEntireRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stEntireRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stEntireWarRank.InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stGuildRankList = CRecordGlobalGuildRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stGuildRankList)
    {
        pstGlobalData->m_stGuildRank.InsertWithoutSQL(std::make_pair(rank.m_iGuildID, rank));
    }

    auto stHomeRankList = CRecordGlobalHomeRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stHomeRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stHomeRank.InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stIBusyRankList = CRecordGlobalIbusyRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stIBusyRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stIbusyRank.InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stMatchRankList = CRecordGlobalMatchRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stMatchRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stMatchPvpRank.InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stMilitaryRankList = CRecordGlobalMilitaryRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stMilitaryRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stMilitaryRank.InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stPowerRankList = CRecordGlobalPowerRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stPowerRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stFightPowerRank.InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stPvpRankList = CRecordGlobalPvpRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stPvpRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stPvpRank.InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stTowerRankList = CRecordGlobalTowerRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stTowerRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stTowerRank.InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stLastPvpRankList = CRecordGlobalLastPvpRankTable::SelectDataFromSQL(iServerID);
    for (const auto& rank : stLastPvpRankList)
    {
        pstGlobalData->m_stLastDayPvpRank.InsertWithoutSQL(std::make_pair(rank.m_iLastRank, rank));
    }

    auto stGlobalMailList = CRecordGlobalMailTable::SelectDataFromSQL(iServerID);
    for (const auto& mail : stGlobalMailList)
    {
        if (CLogicCommon::GetSec() - mail.m_iEndTime > int32_t(7_day))
        {//过期超过一周的邮件自动删除
            TGlobalMailTable stMailTable;
            stMailTable.InitializeWithoutSQL(mail);
            stMailTable.ClearFromSQL();
        }
        else
        {
            pstGlobalData->m_stGlobalMailMap[mail.m_iBeginTime].InsertWithoutSQL(std::make_pair(mail.m_iMailID, mail));
        }
    }

    auto stFightReportList = CRecordFightReportTable::SelectDataFromSQL(0, CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID);
    for (auto stIter = stFightReportList.begin(); stIter != stFightReportList.end(); ++stIter)
    {
        pstGlobalData->m_stFightReportMap[stIter->m_iReportType].InsertWithoutSQL(std::make_pair(stIter->m_iReportID, *stIter));
    }

    auto stEntireMapList = CRecordGlobalEntireWarMapTable::SelectDataFromSQL(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID);
    for (const auto& war : stEntireMapList)
    {
        pstGlobalData->m_stEntireWarMap.InsertWithoutSQL(make_pair(war.m_iWarID, war));
    }

    auto stEntireExtraList = CRecordGlobalEntireExtraTable::SelectDataFromSQL(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID);
    for (const auto& extra : stEntireExtraList)
    {
        pstGlobalData->m_stEntireWarExtra[extra.m_iWarID].InsertWithoutSQL(std::make_pair(extra.m_iExtraID, extra));
    }

    auto stWorldQuestList = CRecordGlobalWorldQuestTable::SelectDataFromSQL(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID);
    for(const auto& info : stWorldQuestList)
    {
        pstGlobalData->m_stWorldQuestMap.InsertWithoutSQL(std::make_pair(info.m_iQuestID, info));
    }

    // 通用排行榜初始化
    const auto& stRankConfig = CLogicConfig::GetRankConfig().m_stRankConfig;
    for(const auto& config : stRankConfig)
    {
        // 设置排行榜最大人数
        pstGlobalData->m_stCommonRankMap[config.first].SetMaxRank(config.second.m_iMaxRank);

        // 设置排行榜排序规则
        pstGlobalData->m_stCommonRankMap[config.first].SetSortRule(config.second.m_bIsLess);
    }

    auto stCommonRankList = CRecordGlobalCommonRankTable::SelectDataFromSQL(iServerID);
    for(const auto& rank : stCommonRankList)
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = rank.m_iUid;
        stKey.m_stRoleIDDetail.m_iGroupID = rank.m_iGroupID;
        pstGlobalData->m_stCommonRankMap[rank.m_iRankID].InsertWithoutSQL(std::make_pair(stKey.m_ulRoleID, rank));
    }

    auto stStoreReward = CRecordGlobalStoreRewardTable::SelectDataFromSQL(iServerID);
    for(const auto& reward : stStoreReward)
    {
        TGlobalStoreRewardTable storeRewardTable;
        storeRewardTable.InitializeWithoutSQL(reward);
        pstGlobalData->m_stStoreRewardTable.emplace_back(reward);
    }
    return pstGlobalData.release();
}

bool CLogicGlobal::Initialize()
{
    auto pstGlobalData = CLogicGlobal::GetGlobalCache();
    if (pstGlobalData == nullptr)
    {
        return false;
    }

    const auto result = CLogicDataRecovery::RecoveryDataFromFile(pstGlobalData->m_stRecoveryData);
    if (result != 0)
    {
        LOGIC_LOG_TRACE_ERROR << "RECOVERY DATA FROM FILE FAIL:ERROR CODE IS " << result << std::endl;
        return false;
    }

    LoadRecoveryUser();

    // 全民战役开启时间
    RefreshEntireWarActivityTime();

    return true;
}

void CLogicGlobal::Refresh()
{
    const auto now_time = CLogicCommon::GetSec();

    // 小时刷新
    static int32_t s_iLastCheckHourTime = now_time;
    if (now_time / 3600 != s_iLastCheckHourTime / 3600)
    {
        s_iLastCheckHourTime = now_time;

        OnCrossOneHour();
    }

    const auto iCheckUpdate = GetGlobalCache()->m_stBasicTable.GetCheckUpdateTime();
    if (LOGIC_CHECK_DAY_REFRESH(iCheckUpdate))
    {
        // 跨天刷新
        OnCrossOneDay();

        // 跨周刷新
        if(LOGIC_CHECK_WEEK_REFRESH(iCheckUpdate))
            OnCrossOneWeek();

        // 记录时间
        GetGlobalCache()->m_stBasicTable.SetCheckUpdateTime(now_time);

        // 存数据库
        LOGIC_RECORD_CACHE_INS->ExecSQL(0, 0, "LOGIC_GLOBAL_CROSS_DAY_OR_WEEK");
    }

    if (GetGlobalCache()->m_stBasicTable.GetLastMergeServerTime() < CLogicConfig::GetSystemConfig().m_stServerConfig.m_iMergeTime)
    {
        GetGlobalCache()->m_stBasicTable.SetLastMergeServerTime(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iMergeTime);

        OnServerMerge();
    }

    RefreshMultiColorOpen();

    RefreshBankPrice();

    RefreshBroadcast();

    RefreshGlobalMail();

    // 暂时关掉全名战役
    // RefreshEntireWar();
}

void CLogicGlobal::OnCrossOneHour()
{
    RefreshAwardRankList();

    LOGIC_RECORD_CACHE_INS->ExecSQL(0, 0, "LOGIC_GLOBAL_CROSS_ONE_HOUR");

}

void CLogicGlobal::OnCrossOneDay()
{
    // 全民战役开启时间
    RefreshEntireWarActivityTime();

    // 防沉迷跨天刷新
    AntiAddCrossDayRefreshData();

    //发送购买礼包的每日邮件(无论是否上线)
    RefreshCrossUserEmailReward();
}

void CLogicGlobal::OnCrossOneWeek()
{
    // 更新通用排行榜
    RefreshCommonRank(CLogicConfigDefine::ERRT_Week);

    // 更新装备副本词缀
    RefreshEquipLevelAffix();

    //更新道馆PVP排行榜
    RefreshPvpRank();

}

void CLogicGlobal::OnServerMerge()
{
    DelMergedGlobalMail();

    LOGIC_PVP_RANK_INS.clear();

    LOGIC_RECORD_CACHE_INS->ExecSQL(0, 0, "LOGIC_GLOBAL_SERVER_MERGED");
}

ILogicConnectorSender* CLogicGlobal::GetConnectorSender()
{
    return LOGIC_SERVICE_API_INS->GetConnectorSender();
}

void CLogicGlobal::LoadRecoveryUser()
{
    for (const auto& user : GetGlobalCache()->m_stRecoveryData.m_stRecoveryUser)
    {
        CLogicCacheCommon::LoadUserData(user.first, user.second);
    }
    GetGlobalCache()->m_stRecoveryData.m_stRecoveryUser.clear();
}

/************************************************************************************************************/
/******************************************排    行    榜*****************************************************/
/************************************************************************************************************/
uint32_t CLogicGlobal::UpdateOrInsertCharmRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    return UpdateOrInsertUserRank(LOGIC_CHARM_RANK_INS, iUid, iGroupID, ulRankScore);
}

uint32_t CLogicGlobal::UpdateOrInsertCountdownRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    return UpdateOrInsertUserRank(LOGIC_COUNTDOWN_HERO_RANK_INS, iUid, iGroupID, ulRankScore);
}

uint32_t CLogicGlobal::UpdateOrInsertEntireWarRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    return UpdateOrInsertUserRank(LOGIC_ENTIRE_WAR_RANK_INS, iUid, iGroupID, ulRankScore);
}

uint32_t CLogicGlobal::UpdateOrInsertGuildRank(int32_t iGuildID, uint64_t ulRankScore)
{
    TGlobalGuildRankTableValueType value;
    value.m_iGuildID = iGuildID;
    value.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
    value.m_ulRankScore = ulRankScore;
    return LOGIC_GUILD_RANK_INS.UpdateOrInsert(std::make_pair(iGuildID, value));
}

uint32_t CLogicGlobal::UpdateOrInsertHomeRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    return UpdateOrInsertUserRank(LOGIC_HOME_RANK_INS, iUid, iGroupID, ulRankScore);
}

uint32_t CLogicGlobal::UpdateOrInsertIbusyRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    return UpdateOrInsertUserRank(LOGIC_IBUSY_SCORE_RANK_INS, iUid, iGroupID, ulRankScore);
}

uint32_t CLogicGlobal::UpdateOrInsertMatchRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    return UpdateOrInsertUserRank(LOGIC_MATCH_PVP_RANK_INS, iUid, iGroupID, ulRankScore);
}

uint32_t CLogicGlobal::UpdateOrInsertMilitaryRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    return UpdateOrInsertUserRank(LOGIC_MILITARY_LEVEL_RANK_INS, iUid, iGroupID, ulRankScore);
}

uint32_t CLogicGlobal::UpdateOrInsertPowerRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    return UpdateOrInsertUserRank(LOGIC_FIGHT_POWER_RANK_INS, iUid, iGroupID, ulRankScore);
}

uint32_t CLogicGlobal::UpdateOrInsertPvpRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    return UpdateOrInsertUserRank(LOGIC_PVP_RANK_INS, iUid, iGroupID, ulRankScore);
}

uint32_t CLogicGlobal::UpdateOrInsertTowerRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    return UpdateOrInsertUserRank(LOGIC_CLIMB_TOWER_RANK_INS, iUid, iGroupID, ulRankScore);
}


uint32_t CLogicGlobal::UpdateOrInsertCommonRank(int32_t iRankID, int32_t iUid, int32_t iGroupID, uint64_t ulRankScore)
{
    auto& stRankMap = GetGlobalCache()->m_stCommonRankMap;
    auto iterRank = stRankMap.find(iRankID);
    if(iterRank == stRankMap.end())
    {
        const auto pConfig = CLogicConfig::GetRankConfig().GetConfig(iRankID);
        if(nullptr == pConfig) return 0;

        stRankMap[iRankID].SetMaxRank(pConfig->m_iMaxRank);
        stRankMap[iRankID].SetSortRule(pConfig->m_bIsLess);

        iterRank = stRankMap.find(iRankID);
    }

    if(iterRank != stRankMap.end())
    {
        user_info_key_t key;
        TGlobalCommonRankTableValueType value;
        value.m_iUid = key.m_stRoleIDDetail.m_iUin = iUid;
        value.m_iGroupID = key.m_stRoleIDDetail.m_iGroupID = iGroupID;
        value.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
        value.m_iRankID = iRankID;
        value.m_ulRankScore = ulRankScore;
        return iterRank->second.UpdateOrInsert(std::make_pair(key.m_ulRoleID, value));
    }

    return 0;
}

uint32_t CLogicGlobal::GetRankByCommonRank(int32_t iRankID, int32_t iUid, int32_t iGroupID)
{
    auto& stRankMap = GetGlobalCache()->m_stCommonRankMap;
    auto iterRank = stRankMap.find(iRankID);
    if(iterRank == stRankMap.end())
        return 0;

    user_info_key_t key;
    key.m_stRoleIDDetail.m_iUin = iUid;
    key.m_stRoleIDDetail.m_iGroupID = iGroupID;
    return iterRank->second.GetRank(key.m_ulRoleID);
}

void CLogicGlobal::RefreshCommonRank(int32_t iRefreshType)
{
    for(const auto& config : CLogicConfig::GetRankConfig().m_stRankConfig)
    {
        if(config.second.m_iRefreshType == iRefreshType)
        {
            AwardCommonRank(config.first);
        }
    }
}

int32_t CLogicGlobal::AwardCommonRank(int32_t iRankID)
{
    auto& stRankMap = GetGlobalCache()->m_stCommonRankMap;
    auto iterRank = stRankMap.find(iRankID);
    if(iterRank == stRankMap.end())
        return 0;

    const auto pConfig = CLogicConfig::GetRankConfig().GetConfig(iRankID);
    if(pConfig == nullptr || pConfig->m_stAwardMap.empty())
        return 0;

    // 发奖
    int32_t iRank = 1;
    for (auto iter = iterRank->second.begin(); iter != iterRank->second.end(); ++iter, ++iRank)
    {
        const user_info_key_t stKey(iter->first);
        const auto iterAward = pConfig->m_stAwardMap.lower_bound(iRank);
        if (iterAward != pConfig->m_stAwardMap.end())
        {
            CLogicGlobal::AddAsyncUserMail(stKey.m_stRoleIDDetail.m_iUin, stKey.m_stRoleIDDetail.m_iGroupID
                                           , iterAward->second.m_strMailSender, iterAward->second.m_strMailTitle
                                           , iterAward->second.m_strMailContent, iterAward->second.m_stBonusVec
                                           , "SEND_COMMON_RANK_AWARD");
        }
    }

    // 清空
    if(pConfig->m_bIsClear)
        iterRank->second.clear();

    return (iRank - 1);
}

void CLogicGlobal::RefreshEquipLevelAffix()
{
    GetGlobalCache()->m_stBasicTable.SetAffixVec(CLogicConfig::GetEquipAttrConfig().GetRandAffix());
}


void CLogicGlobal::RefreshPvpRank()
{
    //直接清空排行榜，每次发奖励在玩家登录的时候进行发奖励
    LOGIC_PVP_RANK_INS.clear();
}


void CLogicGlobal::RefreshAwardRankList()
{
    auto& rstGlobalBasicTable = GetGlobalCache()->m_stBasicTable;
    const int32_t now_time = CLogicCommon::GetSec();
    const int32_t today_begin_time = get_day_begin_time(now_time);
    const int32_t last_day_begin_time = today_begin_time - 1_day;
    //竞技场排行发奖
//    int32_t iRankAwardHour = CLogicConfig::GetGlobalConfig().m_iRanklistAwardTime;
//    if (is_pass_some_hour(rstGlobalBasicTable.GetLastAwardRankTime(), CLogicCommon::GetSec(), iRankAwardHour))
//    {
//        int32_t iBonusPVPUserCount = AwardPvpRankList();
//
//        rstGlobalBasicTable.SetLastAwardRankTime(CLogicCommon::GetSec() + 1);
//
//        LOGIC_LOG_TRACE_DEBUG << "RANK_REWARD_BONUS|TIMESTAMP:" << rstGlobalBasicTable.GetLastAwardRankTime()
//                       << "|PVP_RANK_TOTAL_USER_NUM:" << iBonusPVPUserCount << std::endl;
//    }

    //战斗力活动排行榜
    if (rstGlobalBasicTable.GetFightPowerAwardRankTime() == 0)
    {
        int32_t iOpenServerDay = CLogicConfig::GetSystemConfig().GetOpenSvrDay();
        int32_t iCurrentHour = CLogicCommon::GetDate().tm_hour;
        int32_t iRankBonusDay = CLogicConfig::GetFightPowerRankActiveConfig().m_iRankBonusDay;
        int32_t iRankBonusHour = CLogicConfig::GetFightPowerRankActiveConfig().m_iRankBonusHour;

        if (iOpenServerDay * 100 + iCurrentHour >= iRankBonusDay * 100 + iRankBonusHour)
        {
            int32_t iBonusUserCount = AwardFightPowerActiveRankList();

            rstGlobalBasicTable.SetFightPowerAwardRankTime(CLogicCommon::GetSec());

            LOGIC_LOG_TRACE_DEBUG << "|FIGHT_POWER_ACTIVE_RANK_BONUS_COUNT:" << iBonusUserCount << std::endl;
        }
    }

    //限时神将活动排行榜
    const int32_t iOpenCountdownHeroActivityID = CLogicActive::GetOpenCountdownHeroActivityID();
    if (iOpenCountdownHeroActivityID != rstGlobalBasicTable.GetCountdownHeroActivityID())
    {
        //有活动记录 不在活动时间内了 说明需要领奖了
        if (rstGlobalBasicTable.GetCountdownHeroActivityID() > 0 && !CLogicActiveBase::IsActiveOpen(nullptr, rstGlobalBasicTable.GetCountdownHeroActivityID()))
        {
            int32_t iBonusUserCount = AwardCountdownHeroRankList();

            LOGIC_LOG_TRACE_DEBUG << "|COUNTDOWN_HERO_RANK_BONUS_COUNT:" << iBonusUserCount << std::endl;
        }

        rstGlobalBasicTable.SetCountdownHeroActivityID(iOpenCountdownHeroActivityID);
    }

    //主公很忙排行奖励
    int32_t old_ibusy_activity_id = 0;
    int32_t new_ibusy_activity_id = 0;
    for (const auto& ibusy : CLogicConfig::GetIBusyConfig().m_mpActivities)
    {
        if (today_begin_time >= ibusy.second.m_iBeginTime && today_begin_time < ibusy.second.m_iEndTime)
        {
            new_ibusy_activity_id = ibusy.first;
        }

        if (last_day_begin_time >= ibusy.second.m_iBeginTime && last_day_begin_time < ibusy.second.m_iEndTime)
        {
            old_ibusy_activity_id = ibusy.first;
        }
    }
    if (old_ibusy_activity_id != new_ibusy_activity_id && old_ibusy_activity_id > 0)
    {
        int32_t iBonusUserCount = AwardIbusyRankList(old_ibusy_activity_id);

        LOGIC_LOG_TRACE_DEBUG << "|IBUSY_RANK_BONUS_COUNT:" << iBonusUserCount << std::endl;
    }

}

int32_t CLogicGlobal::AwardPvpRankList()
{
    auto pstGlobalCache = GetGlobalCache();
    const auto& stPVPRankListBonusVec = CLogicConfig::GetRankListBonusConfig().m_stPVPRanklistBonus.m_stRankListBonusVec;

    auto iLastRank = 1;
    auto stPVPRankIter = LOGIC_PVP_RANK_INS.begin();
    auto stPVPRankITEnd = LOGIC_PVP_RANK_INS.end();
    for(auto stIter = LOGIC_PVP_RANK_INS.begin(); stIter != LOGIC_PVP_RANK_INS.end() && iLastRank <= CLogicConfigDefine::MAX_PVP_RANK_AMOUNT; ++stIter, ++iLastRank)
    {
        user_info_key_t stUserInfoKey;
        stUserInfoKey.m_ulRoleID = stIter->first;
        auto iterLast = pstGlobalCache->m_stLastDayPvpRank.Find(iLastRank);
        if (iterLast == pstGlobalCache->m_stLastDayPvpRank.End())
        {
            TGlobalLastPvpRankTableValueType value;
            value.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
            value.m_iLastRank = iLastRank;
            value.m_iUid = stUserInfoKey.m_stRoleIDDetail.m_iUin;
            value.m_iGroupID = stUserInfoKey.m_stRoleIDDetail.m_iGroupID;
            pstGlobalCache->m_stLastDayPvpRank.Insert(std::make_pair(value.m_iLastRank, value));
        }
        else
        {
            iterLast->second->SetUid(stUserInfoKey.m_stRoleIDDetail.m_iUin);
            iterLast->second->SetGroupID(stUserInfoKey.m_stRoleIDDetail.m_iGroupID);
        }
    }

    char szRank[12];
    int iTotalBonusUser = 0, iRank = 1;
    user_info_key_t stUserInfoKey;

    std::string strTitle = CLogicConfig::GetRankListBonusConfig().m_stPVPRanklistBonus.m_strMailTitle;
    std::string strSender = CLogicConfig::GetRankListBonusConfig().m_stPVPRanklistBonus.m_strMailSender;
    std::string strContent1=CLogicConfig::GetRankListBonusConfig().m_stPVPRanklistBonus.m_strMailContent1;
    std::string strContent2=CLogicConfig::GetRankListBonusConfig().m_stPVPRanklistBonus.m_strMailContent2;

    for (size_t i = 0; i < stPVPRankListBonusVec.size(); ++i)
    {
        if (stPVPRankIter == stPVPRankITEnd)
            break;

        for (int j = 0; j < stPVPRankListBonusVec[i].m_iBonusNum; ++j)
        {
            if (stPVPRankIter == stPVPRankITEnd)
                break;

            stUserInfoKey.m_ulRoleID = stPVPRankIter->first;
            if(stUserInfoKey.m_stRoleIDDetail.m_iGroupID <= 0)
            {
                ++iRank;
                ++stPVPRankIter;
                continue;
            }

            auto& stBonusVec = stPVPRankListBonusVec[i].m_stBonusList;

            snprintf(szRank, sizeof(szRank), "%d", iRank);
            std::string strContent = strContent1 + szRank + strContent2;
            CLogicGlobal::AddAsyncUserMail(stUserInfoKey.m_stRoleIDDetail.m_iUin,
                                        stUserInfoKey.m_stRoleIDDetail.m_iGroupID, strSender, strTitle, strContent,
                                        stBonusVec, "SEND_PVP_RANK_AWARD_MAIL");

            ++iTotalBonusUser;

            LOGIC_LOG_TRACE_DEBUG << "PVP RANK BONUS ADD TO USER MAIL|GROUP_ID:" << stUserInfoKey.m_stRoleIDDetail.m_iGroupID
                          << "|UIN:" << stUserInfoKey.m_stRoleIDDetail.m_iUin
                          << "|RANK:" << iTotalBonusUser << std::endl;

            ++stPVPRankIter;
            ++iRank;
        }
    }

    return (iTotalBonusUser);
}

int32_t CLogicGlobal::AwardFightPowerActiveRankList()
{
    const auto& stRankListBonus = CLogicConfig::GetFightPowerRankActiveConfig().m_stRankInfo;

    int iIndex = 0;
    int iGroupID, iUin;
    int iTotalBonusUser = 0, iRank = 1;
    user_info_key_t stKey;
    TUserMailTableValueType stVal;

    int iFightPowerLimit = CLogicConfig::GetFightPowerRankActiveConfig().m_iRankBonusMin;
    std::string strTitle = CLogicConfig::GetFightPowerRankActiveConfig().m_strTitle;
    std::string strSender = CLogicConfig::GetFightPowerRankActiveConfig().m_strSender;
    std::string strContent1 = CLogicConfig::GetFightPowerRankActiveConfig().m_strContent1;
    std::string strContent2 = CLogicConfig::GetFightPowerRankActiveConfig().m_strContent2;

    for (auto stIter = LOGIC_FIGHT_POWER_RANK_INS.begin(); stIter != LOGIC_FIGHT_POWER_RANK_INS.end(); stIter ++, iIndex ++, iRank ++)
    {
        if (iIndex >= (int)stRankListBonus.size()) break;

        stKey.m_ulRoleID = stIter->first;
        iUin = stKey.m_stRoleIDDetail.m_iUin;
        iGroupID = stKey.m_stRoleIDDetail.m_iGroupID;

        auto pUser = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
        if (pUser != nullptr)
        {
            if (iFightPowerLimit > pUser->m_stUserInfoTable.GetRoleCombat()) break;

            const std::string strContent = strContent1 + std::to_string(iRank) + strContent2;

            CLogicGlobal::AddAsyncUserMail(iUin, iGroupID, strSender, strTitle, strContent,
                                        stRankListBonus[iIndex].m_stBonusVec, "SEND_FIGHT_POWER_RANK_AWARD_MAIL");

            ++iTotalBonusUser;

            LOGIC_LOG_TRACE_DEBUG << "FIGHT POWER RANK ACTIVE BONUS ADD TO USER MAIL|GROUP_ID:" << iGroupID
                          << "|UIN:" << iUin << "|RANK:" << iTotalBonusUser
                          << "|POWER:" << pUser->m_stUserInfoTable.GetRoleCombat() << std::endl;
        }
    }

    return (iTotalBonusUser);
}

int32_t CLogicGlobal::AwardCountdownHeroRankList()
{
    //发奖
    int32_t iRankCount = 0;
    auto iterRankAward = CLogicConfig::GetActivityConfig().m_stActiveRankConfig.find(GetGlobalCache()->m_stBasicTable.GetCountdownHeroActivityID());
    if (iterRankAward != CLogicConfig::GetActivityConfig().m_stActiveRankConfig.end())
    {
        int32_t iRank = 1;
        for (auto iterRankUser = LOGIC_COUNTDOWN_HERO_RANK_INS.begin(); iterRankUser != LOGIC_COUNTDOWN_HERO_RANK_INS.end(); ++iterRankUser, ++iRank)
        {
            user_info_key_t stKey(iterRankUser->first);
            for (const auto & iterAward : iterRankAward->second)
            {
                if (iRank <= iterAward.m_iMinRank && iterRankUser->second >= uint64_t (iterAward.m_iNeedPoint))
                {
                    CLogicGlobal::AddAsyncUserMail(stKey.m_stRoleIDDetail.m_iUin,
                                                stKey.m_stRoleIDDetail.m_iGroupID,
                                                iterAward.m_strSender,
                                                iterAward.m_strTitle,
                                                iterAward.m_strContent,
                                                iterAward.m_stBonusItemVec,
                                                "AUTO_ADD_COUNTDOWN_HERO_AWARD_MAIL");

                    LOGIC_LOG_TRACE_DEBUG << "COUNTDOWN HERO RANK ACTIVE BONUS ADD TO USER MAIL|GROUP_ID:" << stKey.m_stRoleIDDetail.m_iGroupID
                                  << "|UIN:" << stKey.m_stRoleIDDetail.m_iUin << "|RANK:" << iRank
                                  << "|SCORE:" << iterRankUser->second << std::endl;

                    ++ iRankCount;
                    break;
                }
            }
        }

        //清排行
        LOGIC_COUNTDOWN_HERO_RANK_INS.clear();
    }

    return iRankCount;
}

int32_t CLogicGlobal::AwardIbusyRankList(int32_t iBusyActivityID)
{
    int32_t iBonusUserCount = 0;
    const auto iterIbusyConfig = CLogicConfig::GetIBusyConfig().m_mpActivities.find(iBusyActivityID);
    if (iterIbusyConfig != CLogicConfig::GetIBusyConfig().m_mpActivities.end() && !iterIbusyConfig->second.m_vecRankReward.empty())
    {
        int32_t iRank = 1;
        auto iterAwardConfig = iterIbusyConfig->second.m_vecRankReward.begin();
        for (auto iter = LOGIC_IBUSY_SCORE_RANK_INS.begin(); iter != LOGIC_IBUSY_SCORE_RANK_INS.end(); ++iter, ++iRank)
        {
            while (iterAwardConfig != iterIbusyConfig->second.m_vecRankReward.end() && iRank > iterAwardConfig->m_iEndLevel)
            {
                ++iterAwardConfig;
            }
            if (iterAwardConfig == iterIbusyConfig->second.m_vecRankReward.end())
            {
                break;
            }

            const user_info_key_t stKey(iter->first);
            CLogicGlobal::AddAsyncUserMail(stKey.m_stRoleIDDetail.m_iUin, stKey.m_stRoleIDDetail.m_iGroupID,
                                                     iterIbusyConfig->second.m_strSender, iterIbusyConfig->second.m_strSendTitle,
                                                     iterIbusyConfig->second.m_strSendContent, iterAwardConfig->m_vecRewards,
                                                     "SEND_IBUSY_SCORE_RANK_AWARD");

            ++ iBonusUserCount;
        }

        LOGIC_IBUSY_SCORE_RANK_INS.clear();
    }
    return iBonusUserCount;
}

template<typename RankContainer>
uint32_t CLogicGlobal::UpdateOrInsertUserRank(RankContainer &stRankIns, int32_t iUid, int32_t iGroupID,
                                                        uint64_t ulRankScore)
{
    user_info_key_t key;
    typename RankContainer::table_mapped_type value;
    value.m_iUid = key.m_stRoleIDDetail.m_iUin = iUid;
    value.m_iGroupID = key.m_stRoleIDDetail.m_iGroupID = iGroupID;
    value.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
    value.m_ulRankScore = ulRankScore;
    return stRankIns.UpdateOrInsert(std::make_pair(key.m_ulRoleID, value));
}

/************************************************************************************************************/
/******************************************缤  纷  彩  灯*****************************************************/
/************************************************************************************************************/
bool CLogicGlobal::RefreshMultiColorOpen()
{
    static int32_t s_iLastCheckTime = 0;
    const int32_t iNow = CLogicCommon::GetSec();
    if (s_iLastCheckTime / 3600 == iNow / 3600)
    {
        return false;
    }
    s_iLastCheckTime = iNow;

    auto& rstRecoveryData = GetGlobalCache()->m_stRecoveryData;

    const int32_t iNextOpenTime = GetMultiColorNextOpenTime(rstRecoveryData.m_iLastMultiColorOpenTime);
    if (iNextOpenTime <= rstRecoveryData.m_iLastMultiColorOpenTime || iNextOpenTime > iNow)
    {
        return false;
    }
    rstRecoveryData.m_iLastMultiColorOpenTime = iNextOpenTime;

    MultiColorOpenCardVec();

    if (rstRecoveryData.m_iLastMultiColorOpenVec.size() != CLogicConfigDefine::MULTI_COLOR_OPEN_COUNT)
    {
        return false;
    }

    for (auto& user : rstRecoveryData.m_stMultiColorUserData)
    {
        int32_t iMissCount = CLogicConfigDefine::MULTI_COLOR_OPEN_COUNT - GetGuessMultiColorCount(user.second);
        const auto iterAward = CLogicConfig::GetAwardConfig().m_stMultiColorLantern.m_stAwardMap.find(iMissCount);
        if (iterAward == CLogicConfig::GetAwardConfig().m_stMultiColorLantern.m_stAwardMap.end())
        {
            continue;
        }

        const user_info_key_t stKey(user.first);
        auto pUser = CLogicCacheCommon::LoadUserData(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
        if(nullptr == pUser)
            continue;

        if (0 == iMissCount)
        {
            PublishBroadcast("multi_color_award", 0, { { "nick", pUser->m_strNick } });

            logic_game_user_action_channel_data_t stActionData;
            stActionData.m_iTimes = 1;
            stActionData.m_iAction = CLogicGameChannelEnum::ACTION_MULTI_COLOR_GRAND;
            LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(pUser, stActionData);

            // 通用事件
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_MultiColorSpecialAward;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(pUser, stEventData);
        }

        std::string strTitle = iterAward->second.m_strTitle;
        std::string strContent = iterAward->second.m_strConetnt;
        std::vector<CPackGameItem> stBonus(iterAward->second.m_stBonusVec);

        CLogicGlobal::AddAsyncUserMail(stKey.m_stRoleIDDetail.m_iUin, stKey.m_stRoleIDDetail.m_iGroupID, iterAward->second.m_strSender,
                strTitle, strContent, stBonus, "SEND_MULTI_COLOR_GUESS_AWARD");

        user.second.clear();
        if (CLogicUser::GetInstance().IsUserOnline(stKey.m_stRoleIDDetail.m_iUin, stKey.m_stRoleIDDetail.m_iGroupID))
        {
            CLogicUserNotifySystem::NotifyUserDailyData(pUser);
        }
    }

    rstRecoveryData.m_stMultiColorUserData.clear();
    return true;
}

bool CLogicGlobal::MultiColorOpenCardVec()
{
    auto& rstLastOpenVec = GetGlobalCache()->m_stRecoveryData.m_iLastMultiColorOpenVec;
    std::mt19937 stRand19937(GetMultiColorSeedAssist());
	auto RandFunc = [&](int32_t iNum) { return stRand19937() % iNum; };

    rstLastOpenVec.clear();
	for (uint32_t i = 1; i <= CLogicConfigDefine::MULTI_COLOR_CARD_COUNT; ++i)
	{
        rstLastOpenVec.push_back(i);
	}
	std::random_shuffle(rstLastOpenVec.begin(), rstLastOpenVec.end(), RandFunc);
    rstLastOpenVec.erase(std::next(rstLastOpenVec.begin(), CLogicConfigDefine::MULTI_COLOR_OPEN_COUNT), rstLastOpenVec.end());

    return true;
}

int32_t CLogicGlobal::GetMultiColorNextOpenTime(int32_t iLastOpenTime)
{
    const int32_t iNow = CLogicCommon::GetSec();
    auto open_hour = CLogicConfig::GetAwardConfig().m_stMultiColorLantern.m_iOpenHourVec;
    open_hour.push_back(24);
    std::vector<int32_t> open_time;

    struct tm stDate;
    time_t stTimestamp = iNow;
    ::localtime_r(&stTimestamp, &stDate);

    for (auto& hour : open_hour)
    {
        stDate.tm_hour = hour;
        stDate.tm_min = 0;
        stDate.tm_sec = 0;
        open_time.push_back(::mktime(&stDate));
    }
    for (uint32_t i = 1; i < open_time.size(); ++i)
    {
        if (iNow < open_time[i] && iLastOpenTime < open_time[i-1])
        {
            return open_time[i - 1];
        }
    }
    return 0;
}

int32_t CLogicGlobal::GetGuessMultiColorCount(const std::vector<int32_t>& stCardVec)
{
    int32_t iCount = 0;
    auto& rstRecoveryData = GetGlobalCache()->m_stRecoveryData;
    for (uint32_t i = 0 ; i < rstRecoveryData.m_iLastMultiColorOpenVec.size(); ++i)
    {
        for (uint32_t j = 0 ; j < stCardVec.size(); ++j)
        {
            if (rstRecoveryData.m_iLastMultiColorOpenVec[i] == stCardVec[j])
            {
                ++iCount;
                break;
            }
        }
    }
    return iCount;
}

const std::vector<int32_t>& CLogicGlobal::GetUserSelectLanternVec(int32_t iUin, int32_t iGroupID)
{
    static std::vector<int32_t> emptyVec;
    const user_info_key_t stKey(iUin, iGroupID);
    const auto iter = GetGlobalCache()->m_stRecoveryData.m_stMultiColorUserData.find(stKey.m_ulRoleID);
    return iter == GetGlobalCache()->m_stRecoveryData.m_stMultiColorUserData.end() ? emptyVec : iter->second;
}

void CLogicGlobal::SetUserSelectLanternVec(int32_t iUin, int32_t iGroupID, const std::vector<int32_t>& stCardVec)
{
    const user_info_key_t stKey(iUin, iGroupID);
    GetGlobalCache()->m_stRecoveryData.m_stMultiColorUserData[stKey.m_ulRoleID] = stCardVec;
}

uint32_t CLogicGlobal::GetMultiColorSeedAssist()
{
    char key[1024] = {0};
    uint32_t uiMultiColorSeedAssist = 0;

    string_view version_sv = LOGIC_SERVICE_API_INS->GetServiceVersion();
    const int32_t key_length = snprintf(key, sizeof(key), "version:%.*s, last open time: %d", int32_t(version_sv.length()), version_sv.data(), GetGlobalCache()->m_stRecoveryData.m_iLastMultiColorOpenTime);

    auto* pTmp = reinterpret_cast<uint32_t*>(key);
    for (uint32_t i = 0; i < key_length / sizeof(uint32_t); ++i)
    {
        uiMultiColorSeedAssist += pTmp[i];
    }

    if (key_length % sizeof(uint32_t) > 0)
    {
        uint32_t uiTmp = 0;
        char* pszTmp = (key + (key_length - (key_length % sizeof(uint32_t))));
        memcpy(static_cast<void*>(&uiTmp), static_cast<const void*>(pszTmp), size_t(key_length % sizeof(uint32_t)));
        uiMultiColorSeedAssist += uiTmp;
    }

    return uiMultiColorSeedAssist;
}


/************************************************************************************************************/
/******************************************招    财    猫*****************************************************/
/************************************************************************************************************/
std::list<CPackLuckyCatHistory> CLogicGlobal::GetLuckCatHistory()
{
    return GetGlobalCache()->m_stRecoveryData.m_stLuckyCatHistoryList;
}

void CLogicGlobal::AddLuckCatHistory(const CPackLuckyCatHistory& stHistory)
{
    GetGlobalCache()->m_stRecoveryData.m_stLuckyCatHistoryList.push_back(stHistory);
    int iLen = GetGlobalCache()->m_stRecoveryData.m_stLuckyCatHistoryList.size();
    if (iLen > 10)
    {
        GetGlobalCache()->m_stRecoveryData.m_stLuckyCatHistoryList.pop_front();
    }
}

/************************************************************************************************************/
/******************************************诸  葛  钱  庄*****************************************************/
/************************************************************************************************************/
int32_t CLogicGlobal::GetLastBankPrice(const int32_t iCountry)
{
    const auto& rstBankPrice = GetGlobalCache()->m_stOnlyCache.m_stBankPriceList.back();
    auto iPrice = 0;
    switch (iCountry)
    {
        case CLogicConfigDefine::COUNTRY_WEI:
            iPrice = rstBankPrice.m_iWeiPrice;
            break;

        case CLogicConfigDefine::COUNTRY_SHU:
            iPrice = rstBankPrice.m_iShuPrice;
            break;

        case CLogicConfigDefine::COUNTRY_WU:
            iPrice = rstBankPrice.m_iWuPrice;
            break;

        default:
            break;
    }

    return iPrice;
}

const std::list<CPackBankPrice>& CLogicGlobal::GetBankPriceList()
{
    return GetGlobalCache()->m_stOnlyCache.m_stBankPriceList;
}

int32_t CLogicGlobal::GetBankNextRefreshTime()
{
    return GetGlobalCache()->m_stOnlyCache.m_iBankPriceNextRefreshTime;
}

std::string CLogicGlobal::GetBankPriceEvent()
{
    std::string strEvent;
    const auto iEventSize = CLogicConfig::GetBankPriceConfig().m_stBankPriceConfigVec.size();
    if (iEventSize > 0)
    {
        const int32_t iEventIndex = GetGlobalCache()->m_stOnlyCache.m_iBankPriceRefreshCount % iEventSize;
        strEvent = CLogicConfig::GetBankPriceConfig().m_stBankPriceConfigVec[iEventIndex].m_strEvent;
    }
    return strEvent;
}

bool CLogicGlobal::RefreshBankPrice()
{
    auto& rstGlobalCache = GetGlobalCache()->m_stOnlyCache;

    if (rstGlobalCache.m_iBankPriceNextRefreshTime == 0)
    {
        rstGlobalCache.m_iBankPriceNextRefreshTime = 1559318400;	//2019-06-01 00:00:00 UTC+8（假设从这个时间点算起）
        rstGlobalCache.m_iBankPriceRefreshCount = 0;
        rstGlobalCache.m_stBankPriceList.clear();
    }

    const int32_t iNow = CLogicCommon::GetSec();
    const int32_t iTotalCycle = CLogicConfig::GetBankPriceConfig().m_stBankPriceConfigVec.size();
    if (iNow > rstGlobalCache.m_iBankPriceNextRefreshTime && iTotalCycle > 1)
    {
        const auto &rstCurrentNode = CLogicConfig::GetBankPriceConfig().m_stBankPriceConfigVec[rstGlobalCache.m_iBankPriceRefreshCount % iTotalCycle];
        const auto &rstNextNode = CLogicConfig::GetBankPriceConfig().m_stBankPriceConfigVec[(rstGlobalCache.m_iBankPriceRefreshCount + 1) % iTotalCycle];

        AddBankPriceNode(rstGlobalCache.m_iBankPriceNextRefreshTime, rstCurrentNode.m_iWeiPrice, rstCurrentNode.m_iShuPrice, rstCurrentNode.m_iWuPrice);

        ++rstGlobalCache.m_iBankPriceRefreshCount;
        if (rstCurrentNode.m_iDay == rstNextNode.m_iDay)
        {
            rstGlobalCache.m_iBankPriceNextRefreshTime += (rstNextNode.m_iHour - rstCurrentNode.m_iHour) * 3600;
        }
        else
        {
            rstGlobalCache.m_iBankPriceNextRefreshTime += 24 * 3600;

            const time_t tmTime = rstGlobalCache.m_iBankPriceNextRefreshTime;
            struct tm stRefreshDate;
            ::localtime_r(&tmTime, &stRefreshDate);
            stRefreshDate.tm_hour = rstNextNode.m_iHour;
            stRefreshDate.tm_min = 0;
            stRefreshDate.tm_sec = 0;
            rstGlobalCache.m_iBankPriceNextRefreshTime = mktime(&stRefreshDate);
        }

        return true;
    }

    return false;
}

void CLogicGlobal::AddBankPriceNode(int32_t iTimeStamp, int32_t iWeiPrice, int32_t iShuPrice, int32_t iWuPrice)
{
    GetGlobalCache()->m_stOnlyCache.m_stBankPriceList.emplace_back(iTimeStamp, iWeiPrice, iShuPrice, iWuPrice);

    if (GetGlobalCache()->m_stOnlyCache.m_stBankPriceList.size() > CLogicConfigDefine::BANK_PRICE_NODE_COUNT)
    {
        GetGlobalCache()->m_stOnlyCache.m_stBankPriceList.pop_front();
    }

}

/************************************************************************************************************/
/******************************************聊  天  广  播*****************************************************/
/************************************************************************************************************/
void CLogicGlobal::PublishBuyMallBroadcast(int32_t iCommodityID, const std::string &strNick,
                                                        const std::vector<CPackGameItem> &rstGameItemList)
{
    for (const auto& item : rstGameItemList)
    {
        if (item.m_iItemType == CLogicConfigDefine::HERO_CARD)
        {
            auto pCardConfigItem = CLogicConfig::GetHeroCardConfig().GetConfig(item.m_iCardID);
            if (pCardConfigItem)
            {
                PublishMixBroadcast("summon_champion", iCommodityID, pCardConfigItem->m_iInitStar, pCardConfigItem->m_iInitStar - 2, {
                        {"nick",      strNick},
                        {"card_name", pCardConfigItem->m_strHeroCardName},
                        {"star",      CLogicConfig::GetUpgradeHeroCardStarConfig().GetGradeName(pCardConfigItem->m_iInitGrade)}
                });
            }
        }
    }
}

void CLogicGlobal::PublishGashaponMachinesMallBroadcast(int32_t iCommodityID, const std::string &strNick, const CPackGameItem &stItem, int iNotify)
{
    PublishMixBroadcast("summon_champion", iCommodityID, 0, iNotify, {
            {"nick",        strNick},
            {"item_name",   CLogicConfig::GetItemName(stItem.m_iItemType, stItem.m_iCardID)},
            {"num",         std::to_string(stItem.m_iNum)}
    });
}

void CLogicGlobal::PublishCrossFightBroadcast(const std::string &strNick, const std::vector<CPackGameItem> &rstGameItemList)
{
    for (const auto& item : rstGameItemList)
    {
        if (item.m_iItemType == CLogicConfigDefine::HERO_CARD)
        {
            auto pCardConfigItem = CLogicConfig::GetHeroCardConfig().GetConfig(item.m_iCardID);
            if (pCardConfigItem)
            {
                PublishBroadcast("cross_fight_card", pCardConfigItem->m_iInitStar, {
                        {"nick", strNick},
                        {"star", CLogicConfig::GetUpgradeHeroCardStarConfig().GetGradeName(pCardConfigItem->m_iInitGrade)},
                        {"card_name", pCardConfigItem->m_strHeroCardName}
                });
            }
        }
    }
}

void CLogicGlobal::PublishBroadcast(const std::string &strBroadcastID, int32_t iTriggerValue, int32_t iNotifyLevel,
        const std::map<std::string, std::string> &rstParamMap, const std::vector<int> &rstJumpParams/* = {}*/, int32_t iPara/* = 0*/, int32_t iCmd/* = 0*/)
{
    if (0 == iNotifyLevel)  return;
    const auto& configMap = CLogicConfig::GetBroadcastConfig().m_stConfigMap;
    for (auto iter = configMap.lower_bound(strBroadcastID); iter != configMap.upper_bound(strBroadcastID); ++iter)
    {
        if (iTriggerValue >= iter->second.m_iTriggerMinValue && iTriggerValue <= iter->second.m_iTriggerMaxValue &&
            iPara == iter->second.m_iPara && (iter->second.m_iCmd == 0 || iter->second.m_iCmd == iCmd))
        {
            TGlobalBroadcastContent content = {};
            content.m_iPriority = iter->second.m_iShowPriority;
            content.m_strContent = iter->second.m_stBroadcastFmt.Replace(rstParamMap);
            content.m_iTalkPriority = iNotifyLevel > 0 ? iNotifyLevel - 1 : iter->second.m_iShowPriority;
            content.m_strTalkContent = iter->second.m_stTalkContentFmt.Replace(rstParamMap);
            content.m_iJump = iter->second.m_iJump;
            content.m_stJumpParams = rstJumpParams;
            content.m_iSendTime = CLogicCommon::GetSec();

            PublishBroadcast(content);
        }
    }
}

void CLogicGlobal::PublishBroadcast(const std::string &strBroadcastID, int32_t iTriggerValue,
                                                 const std::map<std::string, std::string> &rstParamMap,
                                                 const std::vector<int> &rstJumpParams)
{
    return PublishBroadcast(strBroadcastID, iTriggerValue, -1, rstParamMap, rstJumpParams);
}

void CLogicGlobal::PublishMixBroadcast(const std::string &strBroadcastID, int32_t iBroadcastIndex,
                                                    int32_t iTriggerValue, int32_t iNotifyLevel,
                                                    const std::map<std::string, std::string> &rstParamMap,
                                                    const std::vector<int> &rstJumpParams)
{
    PublishBroadcast(strBroadcastID + "#" + std::to_string(iBroadcastIndex), iTriggerValue, iNotifyLevel, rstParamMap,
                     rstJumpParams);
}

void CLogicGlobal::PublishMixBroadcast(const std::string &strBroadcastID, int32_t iBroadcastIndex,
                                                    int32_t iTriggerValue,
                                                    const std::map<std::string, std::string> &rstParamMap,
                                                    const std::vector<int> &rstJumpParams)
{
    PublishBroadcast(strBroadcastID + "#" + std::to_string(iBroadcastIndex), iTriggerValue, rstParamMap, rstJumpParams);
}

void CLogicGlobal::PublishRewardCardBroadcast(const std::string& strNick, int32_t iCardID, int32_t iCmd)
{
    const auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iCardID);
    if(nullptr == pConfig) return;

    PublishBroadcast("reward_card", 0, -1, {
            {"nick",        strNick},
            {"card_name",   pConfig->m_strHeroCardName},
            {"grade",       CLogicConfig::GetUpgradeHeroCardStarConfig().GetGradeName(pConfig->m_iInitGrade)}
    }, {}, iCardID, iCmd);
}

void CLogicGlobal::PublishDepositBroadcast(const std::string& strNick, int32_t iDepositID)
{
    const auto pstDeposit = CLogicConfig::GetDepositConfig().GetDepositConfig(iDepositID);
    if(nullptr == pstDeposit || pstDeposit->m_iNotify <= 0) return;

    PublishBroadcast("deposit_broad", 0, pstDeposit->m_iNotify, {
            {"nick",        strNick},
            {"item_name",   pstDeposit->m_strName}
    }, {}, iDepositID);
}

void CLogicGlobal::PublishWishCoinBroadcast(const std::string& strNick, int32_t iCardID)
{
    const auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iCardID);
    if(nullptr == pConfig) return;

    PublishBroadcast("wish_coin", 0, -1, {
            {"nick",        strNick},
            {"card_name",   pConfig->m_strHeroCardName},
            {"grade",       CLogicConfig::GetUpgradeHeroCardStarConfig().GetGradeName(pConfig->m_iInitGrade)}
    });
}

void CLogicGlobal::RefreshBroadcast()
{
    CResponseBroadcastTalk talk_msg;
    std::map<int32_t, CResponseBroadcast> broadcast_msg;

    PublishSystemBulletins();

    PublishActivityBroadcast();

    for (const auto& content :  GetGlobalCache()->m_stOnlyCache.m_stBroadcastContent)
    {
        if (content.m_iPriority >= 0)
        {
            CPackBroadcastMsg broadcast_pack;
            broadcast_pack.m_iPriority = content.m_iPriority;
            broadcast_pack.m_strContent = content.m_strContent;
            if (content.m_iChannels.empty())
            {
                broadcast_msg[0].m_stMsgList.push_back(broadcast_pack);
            }
            else
            {
                for (const auto& channel_id : content.m_iChannels)
                {
                    broadcast_msg[channel_id].m_stMsgList.push_back(broadcast_pack);
                }
            }
        }

        if ((content.m_iTalkPriority != 0) && (!content.m_strTalkContent.empty()))
        {
            CPackBroadcastTalkMsg talk_pack;
            talk_pack.m_iSendTime = content.m_iSendTime;
            talk_pack.m_iPriority = content.m_iTalkPriority;
            talk_pack.m_strContent = content.m_strTalkContent;
            talk_pack.m_iJump = content.m_iJump;
            talk_pack.m_vecJumpParam = content.m_stJumpParams;

            LOGIC_GLOBAL_FRAME_INS->AddBroadcastPushProto(talk_pack);
            talk_msg.m_stMsgList.push_back(talk_pack);
        }
    }

    for (auto& broadcast_channel : broadcast_msg)
    {
        if (!broadcast_channel.second.m_stMsgList.empty())
        {
            BroadcastToClient(MSG_LOGIC_NOTIFY_BROADCAST, broadcast_channel.second, broadcast_channel.first);
        }
    }

    if (!talk_msg.m_stMsgList.empty())
    {
        BroadcastToClient(MSG_LOGIC_NOTIFY_BROAD_CAST_TALK, talk_msg);
    }

    GetGlobalCache()->m_stOnlyCache.m_stBroadcastContent.clear();
}

void CLogicGlobal::PublishSystemBulletins()
{
    const int32_t iNowTime = CLogicCommon::GetSec();
    static int32_t s_iLastPublishTime = iNowTime;
    if (iNowTime / 60 != s_iLastPublishTime / 60)
    {
        const auto& rstBulletins = CLogicConfig::GetZooLogicPlatformConfig().m_stBulletins;
        for (const auto& rstBulletin : rstBulletins)
        {
            if (rstBulletin.m_iGroups.count(0) > 0 || rstBulletin.m_iGroups.count(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID) > 0)
            {
                if (iNowTime >= rstBulletin.m_iBeginTime && iNowTime <= rstBulletin.m_iEndTime)
                {
                    int32_t id = rstBulletin.m_iID;
                    int32_t iLastTime = GetGlobalCache()->m_stOnlyCache.m_aiLastPublishBulletinTime[id];

                    if (iNowTime - iLastTime >= rstBulletin.m_iIntervals * 60)
                    {
                        TGlobalBroadcastContent stContent;
                        stContent.m_iSendTime = iNowTime;
                        stContent.m_iPriority = 1;
                        stContent.m_iTalkPriority = 0;
                        stContent.m_strContent = rstBulletin.m_strContent;
                        stContent.m_strTalkContent = rstBulletin.m_strContent;
                        stContent.m_iChannels = rstBulletin.m_iChannels;
                        PublishBroadcast(stContent);

                        GetGlobalCache()->m_stOnlyCache.m_aiLastPublishBulletinTime[id] = iNowTime;
                    }
                }
            }
        }
    }
}

void CLogicGlobal::PublishActivityBroadcast()
{
    int32_t iNowTime = CLogicCommon::GetSec();
    static int32_t s_iLastPublishTime = iNowTime;

    if (iNowTime / 60 != s_iLastPublishTime / 60)
    {
        for (const auto& config : CLogicConfig::GetActivityBroadcastConfig().m_stConfigMap)
        {
            if (IsInActiveBroadcastTime(config.first))
            {
                auto& lastPublishActivityBroadcast = GetGlobalCache()->m_stOnlyCache.m_astLastPublishActivityBroadcast[config.first];
                if ((lastPublishActivityBroadcast.second < config.second.m_iCount) && (lastPublishActivityBroadcast.first + config.second.m_iInterval <= iNowTime))
                {
                    TGlobalBroadcastContent stContent;
                    stContent.m_iPriority = config.second.m_iShowPriority;
                    stContent.m_iSendTime = iNowTime;
                    stContent.m_strContent = config.second.m_strContent;
                    stContent.m_iTalkPriority = 0;
                    PublishBroadcast(stContent);

                    ++lastPublishActivityBroadcast.second;
                    lastPublishActivityBroadcast.first = iNowTime;
                }
            }
            else
            {
                GetGlobalCache()->m_stOnlyCache.m_astLastPublishActivityBroadcast.erase(config.first);
            }
        }

        s_iLastPublishTime = iNowTime;
    }
}

bool CLogicGlobal::IsInActiveBroadcastTime(int broadcastID)
{
    auto stIter = CLogicConfig::GetActivityBroadcastConfig().m_stConfigMap.find(broadcastID);
    if (stIter == CLogicConfig::GetActivityBroadcastConfig().m_stConfigMap.end())
        return false;

    int iNowTime = CLogicCommon::GetSec();
    switch (stIter->second.m_stActivityTime.m_iTimeType)
    {
        case CLogicConfigDefine::TIME_TYPE_SERVER:
        {
            if (stIter->second.m_stActivityTime.m_iStartDay != 0)
            {
                int beginTime = get_day_begin_time(CLogicConfig::GetSystemConfig().GetOpenSvrTime() + (stIter->second.m_stActivityTime.m_iStartDay - 1) * 86400) + stIter->second.m_iBeginTimeOffset;
                int endTime = beginTime + (stIter->second.m_iCount * stIter->second.m_iInterval);

                if ((iNowTime >= beginTime) && (iNowTime < endTime))
                    return true;
            }
            else if (stIter->second.m_stActivityTime.m_iStartTime != 0)
            {
                int beginTime = stIter->second.m_stActivityTime.m_iStartTime + stIter->second.m_iBeginTimeOffset;
                int endTime = beginTime + (stIter->second.m_iCount * stIter->second.m_iInterval);

                if ((iNowTime >= beginTime) && (iNowTime < endTime))
                    return true;
            }

            break;
        }
        case 4:
        {
            for (const auto& open : stIter->second.m_stActivityTime.m_stOpenWeekDayVec)
            {
                int beginTime = get_week_day_timestamp(iNowTime, open.first, open.second) + stIter->second.m_iBeginTimeOffset;
                int endTime = beginTime + (stIter->second.m_iCount * stIter->second.m_iInterval);

                if ((iNowTime >= beginTime) && (iNowTime < endTime))
                    return true;
            }

            break;
        }
    }

    return false;
}


void CLogicGlobal::PublishBroadcast(TGlobalBroadcastContent stContent)
{
    if (stContent.m_iChannels.empty())
    {
        stContent.m_iChannels = { 0 };
    }
    GetGlobalCache()->m_stOnlyCache.m_stBroadcastContent.push_back(std::move(stContent));
}

/************************************************************************************************************/
/******************************************邮  件  系  统*****************************************************/
/************************************************************************************************************/
bool CLogicGlobal::AddGlobalMail(int32_t iBeginTime, int32_t iEndTime, int32_t iVipLevel, const std::string& strSender, const std::string& strTitle,
                                 const std::string& strContent, const std::vector<CPackGameItem>& stGameItemList, const std::set<int32_t>& stChannelVec/* = {}*/)
{
    TGlobalMailTableValueType stVal;
    stVal.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
    stVal.m_iMailID = GetGlobalCache()->m_stBasicTable.GetPrimaryKeyIndex();
    stVal.m_iBeginTime = iBeginTime;
    stVal.m_iEndTime = iEndTime;
    stVal.m_iVipLevel = iVipLevel;
    stVal.m_strTitle = strTitle;
    stVal.m_strSender = strSender;
    stVal.m_strContent = strContent;
    stVal.m_stAttachment = stGameItemList;
    stVal.m_stChannelIDVec = stChannelVec;

    GetGlobalCache()->m_stGlobalMailMap[iBeginTime].Insert(std::make_pair(stVal.m_iMailID, stVal));
    GetGlobalCache()->m_stBasicTable.AddPrimaryKeyIndex(1);

    LOGIC_LOG_TRACE_ERROR << "PROCESS ADD SYSTEM MAIL SUCCESS:" << "|GROUP_ID:" << stVal.m_iServerID
                   << "|MAIL_ID:" << stVal.m_iMailID << "|BEGIN_TIME:" << stVal.m_iBeginTime << "|END_TIME:" << stVal.m_iEndTime
                   << "|TITLE:" << stVal.m_strTitle << "|CONTENT:" << stVal.m_strContent
                   << std::endl;

    if (CLogicCommon::GetSec() > iBeginTime && CLogicCommon::GetSec() < iEndTime)
    {
        BroadcastHasNewGlobalMail();
    }

    return true;
}

void CLogicGlobal::DelGlobalMail(int32_t iBeginTime, int32_t iMailID)
{
    auto iter = GetGlobalCache()->m_stGlobalMailMap.find(iBeginTime);
    if (iter != GetGlobalCache()->m_stGlobalMailMap.end())
    {
        iter->second.Erase(iMailID);
        if (iter->second.Empty())
        {
            GetGlobalCache()->m_stGlobalMailMap.erase(iter);
        }
    }
}

void CLogicGlobal::AddAsyncUserMail(int32_t iUin, int32_t iGroupID, const std::string &strSender, const std::string &strTitle, const std::string &strContent,
                      const std::vector<CPackGameItem> &stGameItemList, const std::string &strCmdName, int32_t iAwardMultiple, bool bSync,int32_t iExpiredTime)
{
    TGlobalAsyncUserMail stMail;
    stMail.m_iUin = iUin;
    stMail.m_iGroupID = iGroupID;
    stMail.m_strSender = strSender;
    stMail.m_strTitle = strTitle;
    stMail.m_strContent = strContent;
    stMail.m_strCmdName = strCmdName;
    stMail.m_iAwardMultiple = iAwardMultiple;
    stMail.m_iExpiredTime = (iExpiredTime == 0) ? 0 : CLogicCommon::GetSec() + iExpiredTime * 1_day;

    if (stGameItemList.empty())
    {
        AddAsyncUserMail(stMail, bSync);
    }

    const size_t maxItemCount = 10;
    for (size_t start = 0; start < stGameItemList.size(); start += maxItemCount)
    {
        stMail.m_stGameItemList.clear();
        for (size_t index = start; index < start + maxItemCount && index < stGameItemList.size(); ++index)
        {
            stMail.m_stGameItemList.push_back(stGameItemList[index]);
        }

        AddAsyncUserMail(stMail, bSync);
    }


}

void CLogicGlobal::AddAsyncUserMail(const TGlobalAsyncUserMail& stMail, bool bSync/* = false*/)
{
    //在线邮件及赠送邮件优先发送
    if (CLogicUser::GetInstance().IsUserOnline(stMail.m_iUin, stMail.m_iGroupID) || bSync)
    {
        GetGlobalCache()->m_stOnlyCache.m_stAsyncUserMailList.emplace_front(stMail);

        // 同步的话，立即发送
        if(bSync)
        {
            SendOneAsyncUserMail(true);
        }
    }
    else
    {
        GetGlobalCache()->m_stOnlyCache.m_stAsyncUserMailList.emplace_back(stMail);
    }
}

bool CLogicGlobal::AddAsyncEventMail(int32_t iUin, int32_t iGroupID, const std::string& strEventID, const std::map<std::string, std::string>& stValMap, int32_t iAwardMultiple)
{
    const auto iterMail = CLogicConfig::GetMailConfig().m_astEventMail.find(strEventID);
    if (iterMail != CLogicConfig::GetMailConfig().m_astEventMail.end())
    {
        AddAsyncUserMail(iUin, iGroupID, iterMail->second.m_strSender, iterMail->second.m_strTitle,
                     iterMail->second.m_stContent.Replace(stValMap), iterMail->second.m_astItem,
                     "SEND_EVENT_MAIL_" + strEventID, iAwardMultiple);
        return true;
    }
    return false;
}

void CLogicGlobal::AddAsyncNpcMail(int32_t iUin, int32_t iGroupID)
{
    auto pUserData = CLogicCacheCommon::GetInCacheUserData(iGroupID, iUin);
    if (pUserData)
    {
        const int32_t iCreateDay = day_diff(pUserData->m_stUserInfoTable.GetCreateTime(), CLogicCommon::GetSec()) + 1;
        const int32_t iServerDay = CLogicConfig::GetSystemConfig().GetOpenSvrDay();
        const int32_t iDatetime = CLogicCommon::GetSec();
        const int32_t iLevel = pUserData->m_stUserInfoTable.GetLevel();
        const int32_t iCardTime = CLogicCommon::GetSec();

        const int32_t iMailCreate = pUserData->m_stUserInfoTable.GetNpcMailDay();
        const int32_t iMailServerDay = pUserData->m_stUserInfoTable.GetNpcMailServer();
        const int32_t iMailDatetime = pUserData->m_stUserInfoTable.GetNpcMailDatetime();
        const int32_t iMailLevel = pUserData->m_stUserInfoTable.GetNpcMailLevel();
        const int32_t iMailCardTime = std::max(pUserData->m_stUserInfoTable.GetNpcMailCardtime(), pUserData->m_stUserInfoTable.GetCreateTime());

        if (iCreateDay > iMailCreate)
        {
            for (const auto& mail : CLogicConfig::GetMailConfig().m_astCreateMail)
            {
                if (iCreateDay <= mail.m_iExpiredDay && iMailCreate < mail.m_iCreateDay && iCreateDay >= mail.m_iCreateDay)
                {
                    if (0 == mail.m_iActivityID || CLogicActiveBase::IsActiveOpen(pUserData, mail.m_iActivityID))
                    {
                        AddAsyncUserMail(pUserData->m_iUin, pUserData->m_iGroupID, mail.m_stMail.m_strSender,
                                         mail.m_stMail.m_strTitle, mail.m_stMail.m_stContent.Replace({}),
                                         mail.m_stMail.m_astItem, "SEND_NPC_CREATE_DAY_MAIL");
                    }
                }
            }
            pUserData->m_stUserInfoTable.SetNpcMailDay(iCreateDay);
        }

        if (iServerDay > iMailServerDay)
        {
            for (const auto& mail : CLogicConfig::GetMailConfig().m_astServerMail)
            {
                if (iServerDay <= mail.m_iExpiredDay && iMailServerDay < mail.m_iServerDay && iServerDay >= mail.m_iServerDay)
                {
                    if (0 == mail.m_iActivityID || CLogicActiveBase::IsActiveOpen(pUserData, mail.m_iActivityID))
                    {
                        AddAsyncUserMail(pUserData->m_iUin, pUserData->m_iGroupID, mail.m_stMail.m_strSender,
                                         mail.m_stMail.m_strTitle, mail.m_stMail.m_stContent.Replace({}),
                                         mail.m_stMail.m_astItem, "SEND_NPC_SERVER_DAY_MAIL");
                    }
                }
            }
            pUserData->m_stUserInfoTable.SetNpcMailServer(iServerDay);
        }

        if (iDatetime > iMailDatetime)
        {
            for (const auto& mail : CLogicConfig::GetMailConfig().m_astDateTimeMail)
            {
                const int32_t iSenderTime = mail.m_iDatetime;
                const int32_t iExpiredTime = mail.m_iExpiredTime;
                const int32_t iSenderServerDay = mail.m_iServerDay;
                const int32_t iSenderOpenDay = day_diff(CLogicConfig::GetSystemConfig().GetOpenSvrTime(), iSenderTime) + 1;
                if (iDatetime <= iExpiredTime && iMailDatetime < iSenderTime && iDatetime >= iSenderTime)
                {
                    if (0 == iSenderServerDay || iSenderOpenDay >= iSenderServerDay)
                    {
                        if (0 == mail.m_iActivityID || CLogicActiveBase::IsActiveOpen(pUserData, mail.m_iActivityID))
                        {
                            AddAsyncUserMail(pUserData->m_iUin, pUserData->m_iGroupID, mail.m_stMail.m_strSender,
                                             mail.m_stMail.m_strTitle, mail.m_stMail.m_stContent.Replace({}),
                                             mail.m_stMail.m_astItem, "SEND_NPC_DATE_TIME_MAIL");
                        }
                    }
                }
            }
            pUserData->m_stUserInfoTable.SetNpcMailDatetime(iDatetime);
        }

        if (iLevel > iMailLevel)
        {
            for (const auto& mail : CLogicConfig::GetMailConfig().m_astLevelMail)
            {
                if (mail.m_iLevel > iMailLevel && mail.m_iLevel <= iLevel)
                {
                    AddAsyncUserMail(pUserData->m_iUin, pUserData->m_iGroupID, mail.m_stMail.m_strSender,
                                     mail.m_stMail.m_strTitle, mail.m_stMail.m_stContent.Replace({}), mail.m_stMail.m_astItem,
                                     "SEND_NPC_LEVEL_MAIL");
                }
            }
            pUserData->m_stUserInfoTable.SetNpcMailLevel(iLevel);
        }

        if (iCardTime > iMailCardTime)
        {
            for (auto iterFashion = pUserData->m_stFashionInfoMap.Begin(); iterFashion != pUserData->m_stFashionInfoMap.End(); ++iterFashion)
            {
                if (iterFashion->second->GetExpiredTime() > 0 && iterFashion->second->GetExpiredTime() <= iCardTime && iterFashion->second->GetExpiredTime() > iMailCardTime)
                {
                    const auto iterFashionConfig = CLogicConfig::GetFashionConfig().m_stFashionMap.find(iterFashion->first);
                    if (iterFashionConfig != CLogicConfig::GetFashionConfig().m_stFashionMap.end())
                    {
                        AddAsyncEventMail(pUserData->m_iUin, pUserData->m_iGroupID, "expired_fashion", { { "fashion", iterFashionConfig->second.m_strName } });
                    }
                }
            }

            pUserData->m_stUserInfoTable.SetNpcMailCardtime(iCardTime);
        }
    }
}

void CLogicGlobal::AddAsyncUserGlobalMail(int32_t iUin, int32_t iGroupID)
{
    int iNowTime = CLogicCommon::GetSec();
    auto pUserData = CLogicCacheCommon::GetInCacheUserData(iGroupID, iUin);
    if (nullptr != pUserData && pUserData->m_stUserInfoTable.GetGlobalMailTimePos() < iNowTime)
    {
        TUserMailTableValueType stVal;

        int iGlobalMailTimePos = pUserData->m_stUserInfoTable.GetGlobalMailTimePos();
        const auto& rstGlobalMailMap = GetGlobalCache()->m_stGlobalMailMap;

        if(iGlobalMailTimePos > iNowTime)
        {
            LOGIC_LOG_TRACE_ERROR << "AddAsyncUserGlobalMail ERROR: iGlobalMailTimePos > iNowTime" << "|GROUP_ID:" << iGroupID
                                  << "|Uin:" << iUin << "|GlobalMailTimePos:" << iGlobalMailTimePos << "|NowTime:" << iNowTime << std::endl;
            return;
        }

        for (auto iterTime = rstGlobalMailMap.upper_bound(iGlobalMailTimePos); iterTime != rstGlobalMailMap.upper_bound(iNowTime); ++iterTime)
        {
            for (auto iterMail = iterTime->second.Begin(); iterMail != iterTime->second.End(); ++iterMail)
            {
                // 检查渠道
                const auto& stChannelVec = iterMail->second->m_stData.m_stChannelIDVec;
                if(!stChannelVec.empty() && stChannelVec.find(pUserData->m_iChannelID) == stChannelVec.end())
                {
                    continue;
                }

                if (iNowTime < iterMail->second->GetEndTime() && pUserData->m_stUserInfoTable.GetVipLevel() >= iterMail->second->GetVipLevel())
                {
                    CLogicGlobal::AddAsyncUserMail(iUin,
                            iGroupID,
                            iterMail->second->GetSender(),
                            iterMail->second->GetTitle(),
                            iterMail->second->GetContent(),
                            iterMail->second->GetAttachment(),
                            "ADD_GLOBAL_SYSTEM_MAIL");
                }
            }
            iGlobalMailTimePos = iterTime->first;
        }

        pUserData->m_stUserInfoTable.SetGlobalMailTimePos(iGlobalMailTimePos);
    }
}


void CLogicGlobal::SendAllAsyncUserMail()
{
    while (!GetGlobalCache()->m_stOnlyCache.m_stAsyncUserMailList.empty())
    {
        SendOneAsyncUserMail();
    }
}

void CLogicGlobal::DelMergedGlobalMail()
{
    for (const auto& group : CLogicConfig::GetSystemConfig().m_stServerConfig.m_stMergedGroupMap)
    {
        const auto group_id = group.first;
        if (group_id != CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID)
        {
            auto stSystemMailVec = CRecordGlobalMailTable::SelectDataFromSQL(group_id);
            for (auto & stIter : stSystemMailVec)
            {
                TGlobalMailTable stMailTable;
                stMailTable.InitializeWithoutSQL(stIter);
                stMailTable.ClearFromSQL();
            }

            LOGIC_RECORD_CACHE_INS->ExecSQL(group_id, 0, "DELETE_MERGED_GROUP_GLOBAL_MAIL");
        }
    }
}

void CLogicGlobal::RefreshGlobalMail()
{
    SendOneAsyncUserMail();

    CheckNewGlobalMail();
}

void CLogicGlobal::SendOneAsyncUserMail(bool bForce/* = false*/)
{
    if (GetGlobalCache()->m_stOnlyCache.m_stAsyncUserMailList.empty())
        return;

    static int32_t s_iTick = 0;
    ++s_iTick;
    if (s_iTick % 3 != 0 && !bForce)
    {
        return;
    }

    auto& rstMail = GetGlobalCache()->m_stOnlyCache.m_stAsyncUserMailList.front();
    auto pUserData = CLogicCacheCommon::LoadUserData(rstMail.m_iGroupID, rstMail.m_iUin);
    if (pUserData)
    {
        std::string strItems = "[";
        for (const auto& item : rstMail.m_stGameItemList)
        {
            strItems += "[" + std::to_string(item.m_iItemType) + "," + std::to_string(item.m_iCardID) + "," + std::to_string(item.m_iNum) + "],";
        }
        strItems += "]";

        while (pUserData->m_stUserMailMap.Size() > CLogicConfigDefine::MAX_MAIL_AMOUNT && pUserData->m_stUserMailMap.Begin()->second->GetCreateTime() < CLogicCommon::GetSec() - int32_t(30_day))
        {
            std::string strDeleteItems = "[";
            const auto iterBegin = pUserData->m_stUserMailMap.Begin();
            for (const auto& item : iterBegin->second->GetAttachment())
            {
                strDeleteItems += "[" + std::to_string(item.m_iItemType) + "," + std::to_string(item.m_iCardID) + "," + std::to_string(item.m_iNum) + "],";
            }
            strDeleteItems += "]";

            LOGIC_LOG_TRACE_WARN << "DELETE USER MAIL|UIN:" << rstMail.m_iUin << "|GROUP_ID:" << rstMail.m_iGroupID
                          << "|MAIL_ID:" << iterBegin->second->m_stData.m_iMailID << "|MAIL_TIME:" << iterBegin->second->GetCreateTime()
                          << "|SENDER:" << iterBegin->second->GetSender() << "|TITLE:" << iterBegin->second->GetTitle()
                          << "|CONTENT:" << iterBegin->second->GetContent() << "|ITEMS:" << strDeleteItems
                          << "|IS_RECEIVED:" << iterBegin->second->GetIsReceived()  << std::endl;

            // bill record
            if(!iterBegin->second->GetAttachment().empty() && !iterBegin->second->GetIsReceived())
            {
                CLogicBill::RefreshBillRation(iterBegin->second->m_stData.m_iMailID, GET_MACRO_STRING(CLogicBill::ESMBT_TIMEOUT_DELETE));
                CLogicBill::SaveMailBill(pUserData->m_iChannelID, CLogicBill::ESMBT_TIMEOUT_DELETE
                        , GET_MACRO_STRING(CLogicBill::ESMBT_TIMEOUT_DELETE), iterBegin->second->m_stData, iterBegin->second->GetAttachment());
            }

            pUserData->m_stUserMailMap.Erase(iterBegin);
        }

        TUserMailTableValueType stVal;
        stVal.m_iUid = pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        stVal.m_iMailID = pUserData->m_stUserInfoTable.GetPrimaryKeyIndex();
        stVal.m_iIsNew = 1;
        stVal.m_iIsReceived = 0;
        stVal.m_iReadTime = -1;
        stVal.m_iCreateTime = CLogicCommon::GetSec();
        stVal.m_strTitle = rstMail.m_strTitle;
        stVal.m_strSender = rstMail.m_strSender;
        stVal.m_strContent = rstMail.m_strContent;
        stVal.m_iAwardMultiple = rstMail.m_iAwardMultiple;
        stVal.m_stAttachment = rstMail.m_stGameItemList;
        stVal.m_iExpiredTime = rstMail.m_iExpiredTime;

        pUserData->m_stUserInfoTable.AddPrimaryKeyIndex(1);
        pUserData->m_stUserMailMap.Insert(std::make_pair(stVal.m_iMailID, stVal));
        LOGIC_RECORD_CACHE_INS->ExecSQL(pUserData->m_iGroupID, pUserData->m_iUin, rstMail.m_strCmdName);

        CLogicUserNotifySystem::NotifyHasNewMail(pUserData);

        // bill record
        if(!stVal.m_stAttachment.empty())
        {
            CLogicBill::RefreshBillRation(stVal.m_iMailID, rstMail.m_strCmdName);
            CLogicBill::SaveMailBill(pUserData->m_iChannelID, CLogicBill::ESMBT_ADD, rstMail.m_strCmdName, stVal, stVal.m_stAttachment);
        }

        LOGIC_LOG_TRACE_WARN << "SEND USER MAIL CACHE|UIN:" << stVal.m_iUid << "|GROUP_ID:" << stVal.m_iGroupID
                      << "|MAIL_ID:" << stVal.m_iMailID << "|MAIL_TIME:" << stVal.m_iCreateTime
                      << "|SENDER:" << stVal.m_strSender << "|TITLE:" << stVal.m_strTitle
                      << "|CONTENT:" <<  stVal.m_strContent << "|ITEMS:" << strItems << std::endl;
    }

    GetGlobalCache()->m_stOnlyCache.m_stAsyncUserMailList.pop_front();
}

void CLogicGlobal::CheckNewGlobalMail()
{
    auto iNowTime = CLogicCommon::GetSec();
    static int32_t s_iLastCheckMailTime = iNowTime;
    if (iNowTime - s_iLastCheckMailTime > int32_t(30_sec))
    {
        const auto& rstGlobalMailMap = GetGlobalCache()->m_stGlobalMailMap;
        if (rstGlobalMailMap.upper_bound(s_iLastCheckMailTime) != rstGlobalMailMap.upper_bound(iNowTime))
        {
            BroadcastHasNewGlobalMail();
        }

        s_iLastCheckMailTime = iNowTime;
    }
}

void CLogicGlobal::BroadcastHasNewGlobalMail()
{
    CResponseBody stNotify;
    const auto& stInstance = CLogicUser::GetInstance();
    for(auto stIter = stInstance.Begin(); stIter != stInstance.End(); ++ stIter)
    {
        AddAsyncUserGlobalMail(stIter->first, stIter->second.m_iGroupID);
    }
}

void CLogicGlobal::AntiAddNotifyUpdateUserAge(int32_t iUin, int32_t iGroupID)
{
    CNotifyUpdateUserAge notify;
    GetConnectorSender()->NotifyClientUser(iGroupID, iUin, MSG_LOGIC_UPDATE_USER_AGE, notify);
}

int CLogicGlobal::AntiAddCheckCanLogin(int32_t iUin, int32_t iGroupID, int32_t iAge, bool bHandleVisitor/* = false*/)
{
    const auto &attrConfig = CLogicConfig::GetAntiAddictionConfig().m_stAntiAddictionAttr;
    // 总开关
    if(!attrConfig.m_byMainSwitch)
        return SERVER_ERRCODE::SUCCESS;

    if(iAge >= attrConfig.m_nAgeLimit)
        return SERVER_ERRCODE::SUCCESS;

    if(iAge == 0 && !bHandleVisitor)
        return SERVER_ERRCODE::SUCCESS;

    auto serverConfig = CLogicConfig::GetAntiAddictionConfig().GetServerAntiAddictionConfig(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID);
    if(!serverConfig)
    {
        LOGIC_LOG_TRACE_ERROR << "GetServerAntiAddictionConfig Error|ServerID:" << CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID << std::endl;
        return SERVER_ERRCODE::SUCCESS;
    }

    if(!serverConfig->m_byTimeLimitSwitch && !serverConfig->m_byVisitorSwitch)
        return SERVER_ERRCODE::SUCCESS;

    // 检查非游客玩家可登陆时间段，踢下线
    if(iAge > 0 && serverConfig->m_byTimeLimitSwitch && (CLogicCommon::GetDate().tm_hour < attrConfig.m_byLoginBeginHour || CLogicCommon::GetDate().tm_hour >= attrConfig.m_byLoginEndHour))
    {
        auto notifications = CLogicConfig::GetAntiAddictionConfig().GetAntiAddNotificationsByType(CEAntiAddNotifyType::EAANT_LoginLimit);
        if(notifications != nullptr && notifications->size() > 0)
        {
            // 发送提示
            CCommonNotify proto;
            proto.m_iType = (int)CCommonNotify::ECCN_AntiAddiction;
            proto.m_strNotify = notifications->front().second;
            GetConnectorSender()->NotifyClientUser(iGroupID, iUin, MSG_LOGIC_COMMON_NOTIFY, proto);
        }

        return SERVER_ERRCODE::ANTI_ADDICTION_LOGIN_TIME_LIMIT;
    }

    user_info_key_t stKey;
    stKey.m_stRoleIDDetail.m_iGroupID = iGroupID;
    stKey.m_stRoleIDDetail.m_iUin = iUin;

    auto iter = GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.find(stKey.m_ulRoleID);
    if(iter != GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.end())
    {
        int32_t iLimitTime = 0;
        if(iAge == 0)
        {
            if(serverConfig->m_byVisitorSwitch)
                iLimitTime = attrConfig.m_iVisitorTime*60;
        }
        else
        {
            if(serverConfig->m_byTimeLimitSwitch)
            {
                // 获取当天节假日状态
                int8_t holidayStat = CLogicConfig::GetHolidayCalendarConfig().GetHolidayCalendarStat(CLogicCommon::GetDate().tm_year+1900,
                                                                                                     CLogicCommon::GetDate().tm_mon+1,
                                                                                                     CLogicCommon::GetDate().tm_mday);

                iLimitTime = (holidayStat == (int8_t)CEHolidayStateType::EHST_Normal ? attrConfig.m_iNormalTime*60 : attrConfig.m_iHolidayTime*60);
            }
        }

        // 超过时间限制，踢下线
        if(iLimitTime > 0 && iter->second.second >= iLimitTime)
        {
            return SERVER_ERRCODE::ANTI_ADDICTION_ONLINE_TIME_LIMIT;
        }
    }

    return SERVER_ERRCODE::SUCCESS;
}

void CLogicGlobal::AntiAddCheckUser(int32_t iUin, int32_t iGroupID, int32_t iAge)
{
    const auto &attrConfig = CLogicConfig::GetAntiAddictionConfig().m_stAntiAddictionAttr;
    // 总开关
    if(!attrConfig.m_byMainSwitch)
        return;

    user_info_key_t stKey;
    stKey.m_stRoleIDDetail.m_iGroupID = iGroupID;
    stKey.m_stRoleIDDetail.m_iUin = iUin;

    auto iter = GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.find(stKey.m_ulRoleID);

    if(attrConfig.m_nAgeLimit > iAge)
    {
        if(iter == GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.end())
        {
            GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap[stKey.m_ulRoleID] = {iAge, 0};
        }
        else
        {
            if(iter->second.first == 0 && iAge > 0)
            {
                iter->second.first = iAge;
                iter->second.second = 0;
            }
        }
    }
    else if(iter != GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.end())
    {
        GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.erase(iter);
    }
}

bool CLogicGlobal::AntiAddHandleNotify(int32_t iUin, int32_t iGroupID, int32_t iAge, int32_t iOnlineTime, bool bHandleVisitor/* = false*/, int32_t iInterval/* = 20*/)
{
    const auto &attrConfig = CLogicConfig::GetAntiAddictionConfig().m_stAntiAddictionAttr;
    if(!attrConfig.m_byMainSwitch)
        return true;

    const std::vector<std::pair<int32_t, std::string>>* notifications = nullptr;

    // 检查非游客玩家可登陆时间段
    if(iAge > 0 && (CLogicCommon::GetDate().tm_hour < attrConfig.m_byLoginBeginHour || CLogicCommon::GetDate().tm_hour >= attrConfig.m_byLoginEndHour))
    {
        notifications = CLogicConfig::GetAntiAddictionConfig().GetAntiAddNotificationsByType(CEAntiAddNotifyType::EAANT_LoginLimit);
        if(notifications == nullptr || notifications->size() < 1)
            return false;

        // 发送提示
        CCommonNotify proto;
        proto.m_iType = (int)CCommonNotify::ECCN_AntiAddiction;
        proto.m_strNotify = notifications->front().second;
        GetConnectorSender()->NotifyClientUser(iGroupID, iUin, MSG_LOGIC_COMMON_NOTIFY, proto);

        return true;
    }

    // 游客模式
    if(iAge == 0)
    {
        if(!bHandleVisitor) return true;

        notifications = CLogicConfig::GetAntiAddictionConfig().GetAntiAddNotificationsByType(CEAntiAddNotifyType::EAANT_Visitor);
    }
    else
    {
        // 获取当天节假日状态
        int8_t holidayStat = CLogicConfig::GetHolidayCalendarConfig().GetHolidayCalendarStat(CLogicCommon::GetDate().tm_year+1900,
                                                                                             CLogicCommon::GetDate().tm_mon+1,
                                                                                             CLogicCommon::GetDate().tm_mday);

        notifications = CLogicConfig::GetAntiAddictionConfig().GetAntiAddNotificationsByType(
                holidayStat == (int8_t)CEHolidayStateType::EHST_Normal ? CEAntiAddNotifyType::EAANT_NormalDay : CEAntiAddNotifyType::EAANT_Holiday);
    }

    if(notifications == nullptr)
        return false;

    for(const auto& notify : *notifications)
    {
        int32_t iTotalSeconds = notify.first * 60;
        if(iOnlineTime > iTotalSeconds-iInterval && iOnlineTime <= iTotalSeconds)
        {
            // 发送提示
            CCommonNotify proto;
            proto.m_iType = (int)CCommonNotify::ECCN_AntiAddiction;
            proto.m_strNotify = notify.second;
            GetConnectorSender()->NotifyClientUser(iGroupID, iUin, MSG_LOGIC_COMMON_NOTIFY, proto);

            return true;
        }
    }

    return true;
}

bool CLogicGlobal::AntiAddHandleOnlineTime(int32_t iUin, int32_t iGroupID, int32_t iOnlineTime)
{
    const auto &attrConfig = CLogicConfig::GetAntiAddictionConfig().m_stAntiAddictionAttr;
    if(!attrConfig.m_byMainSwitch)
        return true;

    // 检查非游客玩家可登陆时间段，踢下线
    if(CLogicCommon::GetDate().tm_hour < attrConfig.m_byLoginBeginHour || CLogicCommon::GetDate().tm_hour >= attrConfig.m_byLoginEndHour)
    {
        CResponseNotifyClientLogout proto;
        proto.m_iLogoutType = CResponseNotifyClientLogout::ENCLT_AntiAdd_TimeLimit;
        GetConnectorSender()->NotifyClientUser(iGroupID, iUin, MSG_LOGIC_NOTIFY_CLIENT_LOGOUT, proto);

        return true;
    }

    // 获取当天节假日状态
    int8_t holidayStat = CLogicConfig::GetHolidayCalendarConfig().GetHolidayCalendarStat(CLogicCommon::GetDate().tm_year+1900,
                                                                                         CLogicCommon::GetDate().tm_mon+1,
                                                                                         CLogicCommon::GetDate().tm_mday);

    int32_t iLimitTime = (holidayStat == (int8_t)CEHolidayStateType::EHST_Normal ? attrConfig.m_iNormalTime*60 : attrConfig.m_iHolidayTime*60);

    // 超过时间限制，踢下线
    if(iOnlineTime >= iLimitTime)
    {
        CResponseNotifyClientLogout proto;
        proto.m_iLogoutType = CResponseNotifyClientLogout::ENCLT_AntiAdd_OnlineTimeLimit;
        GetConnectorSender()->NotifyClientUser(iGroupID, iUin, MSG_LOGIC_NOTIFY_CLIENT_LOGOUT, proto);
    }

    return true;
}

bool CLogicGlobal::AntiAddHandleVisitor(int32_t iUin, int32_t iGroupID)
{
    const auto &attrConfig = CLogicConfig::GetAntiAddictionConfig().m_stAntiAddictionAttr;
    if(!attrConfig.m_byMainSwitch)
        return false;

    auto serverConfig = CLogicConfig::GetAntiAddictionConfig().GetServerAntiAddictionConfig(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID);
    if(!serverConfig)
    {
        LOGIC_LOG_TRACE_ERROR << "GetServerAntiAddictionConfig Error|ServerID:" << CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID << std::endl;
        return false;
    }

    user_info_key_t stKey;
    stKey.m_stRoleIDDetail.m_iGroupID = iGroupID;
    stKey.m_stRoleIDDetail.m_iUin = iUin;
    auto iter = GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.find(stKey.m_ulRoleID);
    if(iter == GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.end())
        return false;

    int32_t iOnlineTime = iter->second.second;

    const auto notifications = CLogicConfig::GetAntiAddictionConfig().GetAntiAddNotificationsByType(CEAntiAddNotifyType::EAANT_Visitor);
    if(notifications == nullptr)
        return false;

    if(serverConfig->m_byNotifySwitch && notifications->back().first == attrConfig.m_iVisitorTime && iOnlineTime >= attrConfig.m_iVisitorTime*60)
    {
        // 发送提示
        CCommonNotify proto;
        proto.m_iType = (int)CCommonNotify::ECCN_AntiAddiction;
        proto.m_strNotify = notifications->back().second;
        GetConnectorSender()->NotifyClientUser(iGroupID, iUin, MSG_LOGIC_COMMON_NOTIFY, proto);
    }

    if(serverConfig->m_byVisitorSwitch && iOnlineTime >= attrConfig.m_iVisitorTime*60)
        return true;

    return false;
}

void CLogicGlobal::AntiAddUpdateOnlineTime(int32_t iInterval)
{
    const auto &attrConfig = CLogicConfig::GetAntiAddictionConfig().m_stAntiAddictionAttr;
    if(!attrConfig.m_byMainSwitch)
        return;

    auto serverConfig = CLogicConfig::GetAntiAddictionConfig().GetServerAntiAddictionConfig(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID);
    if(!serverConfig)
    {
        LOGIC_LOG_TRACE_ERROR << "GetServerAntiAddictionConfig Error|ServerID:" << CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID << std::endl;
        return;
    }

    for(auto &iter : GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap)
    {
        user_info_key_t stKey;
        stKey.m_ulRoleID = iter.first;

        // 只处理在线玩家
        if(!CLogicUser::GetInstance().IsUserOnline(stKey.m_stRoleIDDetail.m_iUin, stKey.m_stRoleIDDetail.m_iGroupID))
            continue;

        iter.second.second += iInterval;

        // 处理时间限制提示
        if(serverConfig->m_byNotifySwitch)
        {
            AntiAddHandleNotify(stKey.m_stRoleIDDetail.m_iUin, stKey.m_stRoleIDDetail.m_iGroupID, iter.second.first, iter.second.second, true, iInterval);
        }

        // 处理在线时间限制,不做游客模式的检查
        if(iter.second.first > 0 && serverConfig->m_byTimeLimitSwitch)
        {
            AntiAddHandleOnlineTime(stKey.m_stRoleIDDetail.m_iUin, stKey.m_stRoleIDDetail.m_iGroupID, iter.second.second);
        }
    }
}

void CLogicGlobal::AntiAddCrossDayRefreshData()
{
    const auto &attrConfig = CLogicConfig::GetAntiAddictionConfig().m_stAntiAddictionAttr;
    if(!attrConfig.m_byMainSwitch)
        return;

    for(auto iter = GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.begin(); iter != GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.end();)
    {
        user_info_key_t stKey;
        stKey.m_ulRoleID = iter->first;

        // 游客不做更新
        if(iter->second.first == 0)
        {
            ++iter;
            continue;
        }

        // 在线玩家
        if (CLogicUser::GetInstance().IsUserOnline(stKey.m_stRoleIDDetail.m_iUin, stKey.m_stRoleIDDetail.m_iGroupID))
        {
            iter->second.second = 0;
            ++iter;
        }
        else
        {
            GetGlobalCache()->m_stOnlyCache.m_stAntiAddictionMap.erase(iter++);
        }
    }
}

void CLogicGlobal::UpdateWenWenNewsEvent(int32_t iUin, int32_t iGroupID, int32_t iType, std::vector<std::string> iPara)
{
    auto pUserData = CLogicCacheCommon::GetInCacheUserData(iGroupID, iUin);
    if(nullptr == pUserData) return;

    LOGIC_LOG_TRACE_ERROR << "iUin: " << iUin   << "iType: " << iType << "IPara: " << iPara[0] << std::endl;

    if(!CLogicConfig::GetWenWenNewsConfig().IsValidType(iType))
    {
        LOGIC_LOG_TRACE_ERROR << "iType Is Invalid!|UIN:" << iUin << "|ORDER_ID:" << iGroupID
                              << "|TYPE:" << iType << std::endl;
        return;
    }

    //auto iterPersonCfg = CLogicConfig::GetWenWenNewsConfig().m_stNewsThemeMap.find(iType);
    auto ServerCfg = CLogicConfig::GetWenWenNewsConfig().m_stNewsDynamicMap;

    // 处理个人日报数据
//    if(iterPersonCfg != CLogicConfig::GetWenWenNewsConfig().m_stNewsThemeMap.end())
//    {
//        for(auto& themeElem : iterPersonCfg->second.m_stElemList)
//        {
//            if(themeElem.m_iPara1 == 0 && themeElem.m_iPara2 == 0)
//            {
//                UpdateWenWenNewsPersonData(iUin, iGroupID, iPara, iParaEx, themeElem.m_iID, iType, iterPersonCfg->second.m_iLimitType);
//                break;
//            }
//            else if(themeElem.m_iPara1 > 0 && themeElem.m_iPara2 > 0)
//            {
//                if(themeElem.m_iPara1 <= iPara && iPara <= themeElem.m_iPara2)
//                {
//                    UpdateWenWenNewsPersonData(iUin, iGroupID, iPara, iParaEx, themeElem.m_iID, iType, iterPersonCfg->second.m_iLimitType);
//                    break;
//                }
//            }
//            else if(themeElem.m_iPara1 > 0)
//            {
//                if(themeElem.m_iPara1 == iPara)
//                {
//                    UpdateWenWenNewsPersonData(iUin, iGroupID, iPara, iParaEx, themeElem.m_iID, iType, iterPersonCfg->second.m_iLimitType);
//                    break;
//                }
//            }
//        }
//    }

    // 处理全服数据
    for(auto serverThemeElem: ServerCfg)
    {
        if (serverThemeElem.second.m_iType == iType)
        {
            if (iType == CLogicConfigDefine::EWWNT_PVE_Level)
            {
                for (auto infoElem : serverThemeElem.second.m_iInfo)
                {
                    if (atoi(infoElem.second[0].c_str()) == atoi(iPara[1].c_str()))
                    {
                       LOGIC_LOG_TRACE_ERROR << "22iUin: " << iUin   << "iType: " << iType << "IPara: " << iPara[0] << std::endl;
                        UpdateWenWenNewsServerData(iUin, iGroupID, serverThemeElem.second.m_iIndex,iType, iPara, infoElem.first, serverThemeElem.second.m_iLimitType);
                        break;
                    }
                }
            }

            if (iType == CLogicConfigDefine::EWWNT_PVP_Score)
            {
                for (auto infoElem : serverThemeElem.second.m_iInfo)
                {
                    if (atoi(infoElem.second[0].c_str()) <= atoi(iPara[1].c_str()))
                    {
                        LOGIC_LOG_TRACE_ERROR << "33iUin: " << iUin   << "iType: " << iType << "IPara: " << iPara[0] << std::endl;
                        UpdateWenWenNewsServerData(iUin, iGroupID, serverThemeElem.second.m_iIndex,iType, iPara, infoElem.first, serverThemeElem.second.m_iLimitType);
                        break;
                    }
                }
            }

            if (iType == CLogicConfigDefine::EWWNT_Role_Level)
            {
                for (auto infoElem : serverThemeElem.second.m_iInfo)
                {
                    if (atoi(infoElem.second[0].c_str()) <= atoi(iPara[1].c_str()))
                    {
                        LOGIC_LOG_TRACE_ERROR << "44iUin: " << iUin   << "iType: " << iType << "IPara: " << iPara[0] << std::endl;
                        UpdateWenWenNewsServerData(iUin, iGroupID, serverThemeElem.second.m_iIndex,iType, iPara, infoElem.first, serverThemeElem.second.m_iLimitType);
                        break;
                    }
                }
            }

            if (iType == CLogicConfigDefine::EWWNT_PVE_Time)
            {
                for (auto infoElem : serverThemeElem.second.m_iInfo)
                {
                    if (atoi(infoElem.second[0].c_str()) == atoi(iPara[1].c_str()))
                    {
                        LOGIC_LOG_TRACE_ERROR << "55iUin: " << iUin   << "iType: " << iType << "IPara: " << iPara[0] << std::endl;
                        UpdateWenWenNewsServerData(iUin, iGroupID, serverThemeElem.second.m_iIndex,iType, iPara, infoElem.first, serverThemeElem.second.m_iLimitType);
                        break;
                    }
                }
            }
        }
    }
}

void CLogicGlobal::UpdateWenWenNewsPersonData(int32_t iUin, int32_t iGroupID, int32_t iPara, int32_t iParaEx,
        int32_t iID, int32_t iType, int32_t iLimitType)
{
    auto pUserData = CLogicCacheCommon::GetInCacheUserData(iGroupID, iUin);
    if(nullptr == pUserData) return;

    auto iter = pUserData->m_stWenWenNewsMap.find(iType);
    if(iter == pUserData->m_stWenWenNewsMap.end())
    {
        TUserWenWenNewsTableValueType stValue;
        stValue.m_iUid = iUin;
        stValue.m_iGroupID = iGroupID;
        stValue.m_iNewsType = iType;
        stValue.m_iNewsID = iID;
        stValue.m_iPara = iPara;
        stValue.m_iParaEx = iParaEx;
        stValue.m_iTime = CLogicCommon::GetSec();

        pUserData->m_stWenWenNewsMap[iType].Insert(std::make_pair(iID, stValue));
    }
    else
    {
        if(iLimitType == EWWNLT_Normal)
        {
            auto iterID = iter->second.Find(iID);
            if(iterID != iter->second.End())
            {
                iterID->second->SetPara(iPara);
                iterID->second->SetParaEx(iParaEx);
            }
            else
            {
                TUserWenWenNewsTableValueType stValue;
                stValue.m_iUid = iUin;
                stValue.m_iGroupID = iGroupID;
                stValue.m_iNewsType = iType;
                stValue.m_iNewsID = iID;
                stValue.m_iPara = iPara;
                stValue.m_iParaEx = iParaEx;
                stValue.m_iTime = CLogicCommon::GetSec();

                iter->second.Insert(std::make_pair(iID, stValue));
            }
        }
        else if(iLimitType == EWWNLT_Mutex)
        {
            auto iterBegin = iter->second.Begin();
            for(; iterBegin != iter->second.End(); ++iterBegin)
            {
                if(iterBegin->first == iID)
                {
                    iterBegin->second->SetPara(iPara);
                    iterBegin->second->SetParaEx(iParaEx);
                    break;
                }
                else if(iterBegin->first > iID)
                {
                    break;
                }
            }

            if(iterBegin == iter->second.End())
            {
                while (!iter->second.Empty())
                {
                    iter->second.Erase(iter->second.Begin());
                }

                TUserWenWenNewsTableValueType stValue;
                stValue.m_iUid = iUin;
                stValue.m_iGroupID = iGroupID;
                stValue.m_iNewsType = iType;
                stValue.m_iNewsID = iID;
                stValue.m_iPara = iPara;
                stValue.m_iParaEx = iParaEx;
                stValue.m_iTime = CLogicCommon::GetSec();

                iter->second.Insert(std::make_pair(iID, stValue));
            }
        }
        else
        {
            return;
        }
    }

//    if(isNewEvent && 1 == iWeight)
//    {
        pUserData->m_stIBusyData.SetNewsLastTime(CLogicCommon::GetSec());
        pUserData->m_stIBusyData.SetNewsLastData(CLogicConfig::GetWenWenNewsConfig().EnCodePersonData(iType, iID));
    //}
}

void CLogicGlobal::UpdateWenWenNewsServerData(int32_t iUin, int32_t iGroupID, int32_t iIndexID,int32_t iType, std::vector<std::string> iPara, int32_t iID, int8_t iLimitType)
{
    auto serverData = GetGlobalCache()->m_stBasicTable.GetNewsData();
    auto iterBegin = serverData.m_stDataMap.lower_bound(iIndexID);
    auto iterEnd = serverData.m_stDataMap.upper_bound(iIndexID);
    if(iterBegin == iterEnd)
    {
        LOGIC_LOG_TRACE_ERROR << "55iUin: " << iUin   << "iType: " << iType << "IPara: " << iPara[0]  <<  "index: " << iIndexID << std::endl;
        CPackWenWenNewsServerElem elem;
        elem.m_iType = iType;
        elem.m_iID = iID;
        elem.m_iPara = iPara;
        elem.m_iUin = iUin;
        elem.m_iGroupID = iGroupID;
        elem.m_iIndexID = iIndexID;

        serverData.m_stDataMap.emplace(iIndexID, elem);
    }
    else
    {
        switch (iLimitType)
        {
            case EWWNLT_Normal:
            {
                LOGIC_LOG_TRACE_ERROR << "66iUin: " << iUin   << "iType: " << iType << "IPara: " << iPara[0]  <<  "index: " << iIndexID << std::endl;
                auto iter = iterBegin;
                for(; iter != iterEnd; ++iter)
                {
                    if(iter->second.m_iID == iID)
                    {
                        if (iType == CLogicConfigDefine::EWWNT_PVE_Time && atoi(iPara[1].c_str()) < atoi(iter->second.m_iPara[1].c_str()))
                        {
                            iter->second.m_iUin = iUin;
                            iter->second.m_iGroupID = iGroupID;
                            iter->second.m_iPara = iPara;
                            iter->second.m_iIndexID = iIndexID;
                        }
                        break;
                    }
                }
                if(iter == iterEnd)
                {
                    CPackWenWenNewsServerElem elem;
                    elem.m_iIndexID = iIndexID;
                    elem.m_iType = iType;
                    elem.m_iID = iID;
                    elem.m_iPara = iPara;
                    elem.m_iUin = iUin;
                    elem.m_iGroupID = iGroupID;

                    serverData.m_stDataMap.emplace(iIndexID, elem);
                }
                break;
            }
            case EWWNST_Mutex:
            {

                LOGIC_LOG_TRACE_ERROR << "77iUin: " << iUin   << "iType: " << iType << "IPara: " << iPara[0]  <<  "index: " << iIndexID << std::endl;
                auto iter = iterBegin;
                for(; iter != iterEnd; ++iter)
                {
                    if(iter->second.m_iID >= iID)
                    {
                        break;
                    }
                }
                //只有比当前id大才能添加
                if(iter == iterEnd)
                {
                    iterBegin->second.m_iID = iID;
                    iterBegin->second.m_iPara = iPara;
                    iterBegin->second.m_iUin = iUin;
                    iterBegin->second.m_iGroupID = iGroupID;
                    iterBegin->second.m_iType = iType;
                    iterBegin->second.m_iIndexID = iIndexID;

                    // 差错检查，始终保持一条记录
                    for(++iterBegin; iterBegin != iterEnd;)
                    {
                        serverData.m_stDataMap.erase(iterBegin++);
                    }
                }
                break;
            }
        }
    }

    GetGlobalCache()->m_stBasicTable.SetNewsData(serverData);
}

std::vector<int32_t> CLogicGlobal::FairPvpRandomOptionCard(user_data_table_ptr_type pUserInfo)
{
    std::vector<int32_t> stSelectCard;
    std::map<int32_t, int32_t> stSelectCount;
    std::map<int32_t, std::vector<std::pair<int32_t, int32_t>>> stTypeCard;

    for (const auto& config : CLogicConfig::GetHeroCardConfig().m_stHeroCardMap)
    {
        auto& card = config.second;
        const auto iterRandom = CLogicConfig::GetFairPvpConfig().m_stRandomCardMap.find(card.m_iQuality);
        if (iterRandom != CLogicConfig::GetFairPvpConfig().m_stRandomCardMap.end() && card.m_iIsShow)
        {
            stTypeCard[card.m_iCardType].emplace_back(card.m_iHeroCardID, card.m_iQuality);
        }
    }

    //攻防辅各随机一个
    std::map<int32_t, std::vector<int32_t>> stQualityCard;
    for (auto& type : stTypeCard)
    {
        std::random_shuffle(type.second.begin(), type.second.end());
        for (auto iter = type.second.begin(); iter != type.second.end(); ++iter)
        {
            if (iter == type.second.begin())
            {
                stSelectCard.push_back(iter->first);
                stSelectCount[iter->second]++;
            }
            else
            {
                stQualityCard[iter->second].emplace_back(iter->first);
            }
        }
    }

    //各配置需求随机最小值
    std::vector<int32_t> stLeftCardVec;
    for (const auto& rule : CLogicConfig::GetFairPvpConfig().m_stRandomCardMap)
    {
        auto& card = stQualityCard[rule.first];
        std::random_shuffle(card.begin(), card.end());
        while (!card.empty() && stSelectCount[rule.first] < rule.second.first)
        {
            stSelectCard.push_back(card.back());
            card.pop_back();
            stSelectCount[rule.first]++;
        }
        while (!card.empty() && stSelectCount[rule.first] < rule.second.second)
        {
            stLeftCardVec.push_back(card.back());
            card.pop_back();
            stSelectCount[rule.first]++;
        }
    }

    //剩下的随机要求
    std::random_shuffle(stLeftCardVec.begin(), stLeftCardVec.end());
    stSelectCard.insert(stSelectCard.end(), stLeftCardVec.begin(), stLeftCardVec.end());
    stSelectCard.resize(CLogicConfig::GetFairPvpConfig().m_iOptionCardCount);
    std::random_shuffle(stSelectCard.begin(), stSelectCard.end());

    return stSelectCard;
}

void CLogicGlobal::UpdateHeroHistory(user_data_table_ptr_type pUserInfo, int32_t iCardID)
{
    if (nullptr == pUserInfo)
    {
        LOGIC_LOG_TRACE_ERROR << "nullptr == pUserInfo !!!" << std::endl;

        return;
    }

    const auto cardIter = pUserInfo->m_stHeroCardTableMap.Find(iCardID);
    if (cardIter == pUserInfo->m_stHeroCardTableMap.End() || cardIter->second->GetCardLevel() <= 0)
    {
        LOGIC_LOG_TRACE_ERROR << "CardId not Find in user cards!|UIN:" << pUserInfo->m_iUin << "|GROUP_ID:"
                              << pUserInfo->m_iGroupID << "|Card_ID:" << iCardID << std::endl;

        return;
    }

    auto& cardHistory = pUserInfo->m_stHeroHistoryMap[iCardID];
    for(const auto& config : CLogicConfig::GetHeroHeartConfig().m_stHistoryMap)
    {
        if(cardHistory.Find(config.first) != cardHistory.End())
            continue;

        if(config.second.m_iUnlockHeartLevel <= cardIter->second->GetHeartLevel() &&
           config.second.m_iUnlockStar <= cardIter->second->GetCardStar() &&
           config.second.m_iUnlockDatingTimes <= cardIter->second->GetDatingTimes())
        {
            TUserHeroHistoryTableValueType newValue;
            newValue.m_iUid = pUserInfo->m_iUin;
            newValue.m_iGroupID = pUserInfo->m_iGroupID;
            newValue.m_iCardID = iCardID;
            newValue.m_iHistoryIndex = config.first;
            newValue.m_iUnlockTime = CLogicCommon::GetSec();
            newValue.m_iContentIndex = CLogicCommon::GetRandNum() % config.second.m_iContentNum + 1;
            newValue.m_cRewardTag = 0;

            cardHistory.Insert(std::make_pair(newValue.m_iHistoryIndex, newValue));

            CLogicUserNotifySystem::NotifyUserNewHeroHistory(pUserInfo, iCardID, newValue.m_iHistoryIndex, newValue.m_iUnlockTime, newValue.m_iContentIndex);
        }
        else
        {
            break;
        }
    }
}

void CLogicGlobal::UpdateQuestionnaire(user_data_table_ptr_type pUserInfo, bool bForceNotify/* = false*/)
{
    if (nullptr == pUserInfo)
    {
        LOGIC_LOG_TRACE_ERROR << "nullptr == pUserInfo !!!" << std::endl;

        return;
    }

    bool bState = false;
    bool bDurationTime = false;
    auto stState = pUserInfo->m_stIBusyData.GetQuestionnaireState();
    auto stDurationTime = pUserInfo->m_stIBusyData.GetQuestionnaireDurationTime();

    for(auto iter = stDurationTime.begin(); iter != stDurationTime.end();)
    {
        if(iter->second > 0 && iter->second < CLogicCommon::GetSec())
        {
            bState = true;
            bDurationTime = true;
            CLogicCacheCommon::VecBitmapSetState(stState, iter->first);
            stDurationTime.erase(iter++);
        }
        else
            ++iter;
    }

    for(const auto& stConfig : CLogicConfig::GetQuestionnaireConfig().m_stQuestionnaireMap)
    {
        if(stConfig.second.m_iExpiredDateTime > 0 && stConfig.second.m_iExpiredDateTime < CLogicCommon::GetSec())
            continue;

        if(CLogicCacheCommon::VecBitmapCheckState(stState, stConfig.first))
            continue;

        if(stDurationTime.find(stConfig.first) != stDurationTime.end())
            continue;

        bool bCheck = true;
        for(const auto& stType : stConfig.second.m_stOpenTypeList)
        {
            bCheck = CheckQuestionnaireOpenType(pUserInfo, stType.m_iOpenType, stType.m_iOpenPara, stType.m_iOpenParaEx);

            if(!bCheck) break;
        }

        if(!bCheck) continue;

        // 新问卷
        int32_t iDurationTime = 0;
        if(stConfig.second.m_iDurationDays > 0)
            iDurationTime = CLogicCommon::GetSec() + 1_day * stConfig.second.m_iDurationDays;

        stDurationTime[stConfig.first] = iDurationTime;
        bDurationTime = true;
    }

    if(bState) pUserInfo->m_stIBusyData.SetQuestionnaireState(stState);
    if(bDurationTime) pUserInfo->m_stIBusyData.SetQuestionnaireDurationTime(stDurationTime);

    if(bForceNotify || bDurationTime) CLogicUserNotifySystem::NotifyQuestionnaire(pUserInfo);
}

bool CLogicGlobal::CheckQuestionnaireOpenType(user_data_table_ptr_type pUserInfo, int32_t iType, int32_t iPara, int32_t iParaEx)
{
    int32_t iCurPara = 0;
    switch (iType)
    {
    case CLogicConfigDefine::EQOT_Level:
        iCurPara = pUserInfo->m_stUserInfoTable.GetLevel();
        break;
    case CLogicConfigDefine::EQOT_VIP:
        iCurPara = pUserInfo->m_stUserInfoTable.GetVipLevel();
        break;
    case CLogicConfigDefine::EQOT_Power:
        iCurPara = pUserInfo->m_stUserInfoTable.GetRoleCombatMax();
        break;
    case CLogicConfigDefine::EQOT_ServerOpenDay:
        iCurPara = day_diff(CLogicConfig::GetSystemConfig().GetOpenSvrTime(), CLogicCommon::GetSec()) + 1;
        break;
    case CLogicConfigDefine::EQOT_CreateDay:
        iCurPara = day_diff(pUserInfo->m_stUserInfoTable.GetCreateTime(), CLogicCommon::GetSec()) + 1;
        break;
    case CLogicConfigDefine::EQOT_Date:
        iCurPara = CLogicCommon::GetSec();
        break;
    default:
        return false;
    }

    if(iCurPara < iPara || (iParaEx > 0 && iCurPara > iParaEx))
        return false;

    return true;
}

void CLogicGlobal::NotifyQueueSvrUserLogin(int32_t iUin, bool bLogin, bool bProtect/* = true*/)
{
    if (LOGIC_SERVICE_API_INS->GetQueueConnection()->IsConnected())
    {
        CMsgHeadQueue stMsgHead;
        stMsgHead.m_usCmd = MSG_QUEUE_USER_LOGIN_NOTIFY;

        CQueueUserLoginNotify stData;
        stData.m_iUin = iUin;
        stData.m_bLogin = bLogin;
        stData.m_bProtect = bProtect;

        LOGIC_SERVICE_API_INS->GetQueueConnection()->SendToServer(stMsgHead, stData);
    }
}

void CLogicGlobal::NotifyQueueSvrUserListOnline()
{
    if (LOGIC_SERVICE_API_INS->GetQueueConnection()->IsConnected())
    {
        CMsgHeadQueue stMsgHead;
        stMsgHead.m_usCmd = MSG_QUEUE_USER_LIST_ONLINE_NOTIFY;

        CQueueUserListOnlineNotify stData;
        for (auto iterUser = CLogicUser::GetInstance().Begin(); iterUser != CLogicUser::GetInstance().End(); ++iterUser)
        {
            stData.m_stUinVec.push_back(iterUser->first);
        }

        LOGIC_SERVICE_API_INS->GetQueueConnection()->SendToServer(stMsgHead, stData);
    }
}

void CLogicGlobal::NotifyQueueSvrReconnectCheck(int32_t iUin)
{
    if (LOGIC_SERVICE_API_INS->GetQueueConnection()->IsConnected())
    {
        CMsgHeadQueue stMsgHead;
        stMsgHead.m_usCmd = MSG_QUEUE_USER_RECONNECT_CHECK;

        CQueueRequestUserReconnectCheck stData;
        stData.m_iUin = iUin;

        LOGIC_SERVICE_API_INS->GetQueueConnection()->SendToServer(stMsgHead, stData);
    }
}

void CLogicGlobal::NotifyCrossUserBriefUpdate(user_data_table_ptr_type pUserInfo, bool bLogin)
{
    if (LOGIC_SERVICE_API_INS->GetCrossConnection()->IsConnected())
    {
        CMsgHeadCross stMsgHead;
        stMsgHead.m_byCmdType = enmCmdType_request;
        stMsgHead.m_usCmd = MSG_CROSS_USER_BRIEF_UPDATE;
        stMsgHead.m_iUin = pUserInfo->m_iUin;
        stMsgHead.m_nGroupID = pUserInfo->m_iGroupID;

        CPackUserBrief cross = CLogicCacheCommon::GetUserBriefInfo(pUserInfo);
        cross.m_bIsOnline = bLogin;
        LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stMsgHead, cross);
    }
}

void CLogicGlobal::RefreshCrossUserEmailReward()
{
    for(auto iter = GetGlobalCache()->m_stStoreRewardTable.begin(); iter != GetGlobalCache()->m_stStoreRewardTable.end(); )
    {

        auto emailAwardConfig = CLogicConfig::GetAwardConfig();
        auto awardConfig = emailAwardConfig.GetAwardConfig(iter->m_iDepositID);
        if (nullptr == awardConfig)
        {
            LOGIC_LOG_TRACE_DEBUG << "AWARD_CONFIG IS NOT FOUND ID:" << iter->m_iDepositID << std::endl;
            TGlobalStoreRewardTable storeRewardTable;
            storeRewardTable.InitializeWithoutSQL(*iter);
            storeRewardTable.ClearFromSQL();
            iter = GetGlobalCache()->m_stStoreRewardTable.erase(iter);
            continue;
        }

        if (iter->m_iEndTime < CLogicCommon::GetSec())
        {
            LOGIC_LOG_TRACE_DEBUG << "The Award is Time out:" << iter->m_iDepositID << std::endl;
            TGlobalStoreRewardTable storeRewardTable;
            storeRewardTable.InitializeWithoutSQL(*iter);
            storeRewardTable.ClearFromSQL();
            iter = GetGlobalCache()->m_stStoreRewardTable.erase(iter);
            continue;
        }

        AddAsyncUserMail(iter->m_iUid, iter->m_iGroupID,
                         awardConfig->m_strEmailSender,awardConfig->m_strEmailTitle, awardConfig->m_strEmailContent,
                         awardConfig->m_stBonusVec, "STORE_DAILY_AWARD",1,false,awardConfig->m_iExpired_day);
        ++iter;
    }
}

void CLogicGlobal::NotifyCrossUserVisitorLogout(user_data_table_ptr_type pUserInfo, CRequestHomeVisit visitInfo)
{
    if (LOGIC_SERVICE_API_INS->GetCrossConnection()->IsConnected())
    {
        CMsgHeadCross stMsgHead;
        stMsgHead.m_byCmdType = enmCmdType_cross;
        stMsgHead.m_usCmd = MSG_CROSS_HOME_VISIT_OTHER;
        stMsgHead.m_iUin = visitInfo.m_iUid;
        stMsgHead.m_nGroupID = visitInfo.m_iGroupID;

        visitInfo.m_iHomeID = 0;
        visitInfo.m_iUid = pUserInfo->m_iUin;
        visitInfo.m_iGroupID = pUserInfo->m_iGroupID;
        LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stMsgHead, visitInfo);
    }
}


