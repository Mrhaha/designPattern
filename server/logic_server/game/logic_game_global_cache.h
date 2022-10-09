//
// Created by hzl on 2019/5/28.
//

#pragma once

#include "server_recovery_protocol.h"
#include "logic_record_rank_sql_container.hpp"
#include "logic_record_global_basic_table.h"
#include "logic_record_global_charm_rank_table.h"
#include "logic_record_global_countdown_rank_table.h"
#include "logic_record_global_entire_rank_table.h"
#include "logic_record_global_guild_rank_table.h"
#include "logic_record_global_home_rank_table.h"
#include "logic_record_global_ibusy_rank_table.h"
#include "logic_record_global_match_rank_table.h"
#include "logic_record_global_military_rank_table.h"
#include "logic_record_global_power_rank_table.h"
#include "logic_record_global_pvp_rank_table.h"
#include "logic_record_global_tower_rank_table.h"
#include "logic_record_global_last_pvp_rank_table.h"
#include "logic_record_global_mail_table.h"
#include "logic_record_fight_report_table.h"
#include "logic_record_global_entire_map_table.h"
#include "logic_record_global_entire_extra_table.h"
#include "logic_record_global_world_quest_table.h"
#include "logic_record_global_common_rank_table.h"
#include "logic_record_global_store_reward_table.h"
#include "logic_record_global_guild_war_rank_table.h"

struct TGlobalBroadcastContent
{
    int32_t m_iPriority;
    std::string m_strContent;
    int32_t m_iTalkPriority;
    std::string m_strTalkContent;
    int32_t m_iJump;
    std::vector<int> m_stJumpParams;
    int32_t m_iSendTime;
    std::set<int32_t> m_iChannels;
};

class TGlobalAsyncUserMail
{
public:
    TGlobalAsyncUserMail()
            : m_iUin(0)
            , m_iGroupID(0)
            , m_strSender()
            , m_strTitle()
            , m_strContent()
            , m_stGameItemList()
            , m_strCmdName()
            , m_iAwardMultiple()
    {}

    int32_t m_iUin;
    int32_t m_iGroupID;
    std::string m_strSender;
    std::string m_strTitle;
    std::string m_strContent;
    std::vector<CPackGameItem>  m_stGameItemList;
    std::string m_strCmdName;
    int32_t m_iAwardMultiple;
    int32_t m_iExpiredTime;
};

struct TGlobalOnlyCache
{
    std::vector<TGlobalBroadcastContent> m_stBroadcastContent;
    std::unordered_map<int32_t, int32_t> m_aiLastPublishBulletinTime;
    std::unordered_map<int32_t, std::pair<int32_t, int32_t>> m_astLastPublishActivityBroadcast;	//id => [broadcastTime, count]
    std::list<TGlobalAsyncUserMail> m_stAsyncUserMailList;
    int32_t m_iBankPriceRefreshCount = 0;
    int32_t m_iBankPriceNextRefreshTime = 0;
    std::list<CPackBankPrice> m_stBankPriceList;
    std::unordered_map<uint64_t, std::pair<int32_t, int32_t>>   m_stAntiAddictionMap;     // 防沉迷玩家在线时间列表 RoleID => [age, online_time] // age == 0 游客
};

struct TLogicGlobalCache
{
    using charm_rank_container = CLogicSQLRankAssociateContainer<uint64_t, TGlobalCharmRankTable*, TGlobalCharmRankTableValueType>;

    using countdown_rank_container = CLogicSQLRankAssociateContainer<uint64_t, TGlobalCountdownRankTable*, TGlobalCountdownRankTableValueType>;

    using entire_war_rank_container = CLogicSQLRankAssociateContainer<uint64_t, TGlobalEntireRankTable*, TGlobalEntireRankTableValueType>;

    using guild_rank_container =  CLogicSQLRankAssociateContainer<int32_t , TGlobalGuildRankTable*, TGlobalGuildRankTableValueType>;

    using home_rank_container = CLogicSQLRankAssociateContainer<uint64_t , TGlobalHomeRankTable*, TGlobalHomeRankTableValueType>;

    using ibusy_rank_container = CLogicSQLRankAssociateContainer<uint64_t , TGlobalIbusyRankTable*, TGlobalIbusyRankTableValueType>;

    using match_pvp_rank_container = CLogicSQLRankAssociateContainer<uint64_t , TGlobalMatchRankTable*, TGlobalMatchRankTableValueType>;

    using military_rank_container = CLogicSQLRankAssociateContainer<uint64_t , TGlobalMilitaryRankTable*, TGlobalMilitaryRankTableValueType>;

    using fight_power_rank_container = CLogicSQLRankAssociateContainer<uint64_t , TGlobalPowerRankTable*, TGlobalPowerRankTableValueType>;

    using pvp_rank_container = CLogicSQLRankAssociateContainer<uint64_t , TGlobalPvpRankTable*, TGlobalPvpRankTableValueType>;

    using tower_rank_container = CLogicSQLRankAssociateContainer<uint64_t , TGlobalTowerRankTable*, TGlobalTowerRankTableValueType>;

    using last_pvp_rank_container = CLogicSQLRecordAssociateContainer<int32_t, TGlobalLastPvpRankTable*, TGlobalLastPvpRankTableValueType>;

    using global_mail_container = CLogicSQLRecordAssociateContainer<int32_t, TGlobalMailTable*, TGlobalMailTableValueType>;

    using fight_report_container = CLogicSQLRecordAssociateContainer<int32_t, TFightReportTable*, TFightReportTableValueType> ;

    using entire_war_map_container = CLogicSQLRecordAssociateContainer<int32_t, TGlobalEntireMapTable*, TGlobalEntireMapTableValueType>;

    using entire_war_extra_container = CLogicSQLRecordAssociateContainer<int32_t, TGlobalEntireExtraTable*, TGlobalEntireExtraTableValueType>;

    using world_quest_container = CLogicSQLRecordAssociateContainer<int32_t, TGlobalWorldQuestTable*, TGlobalWorldQuestTableValueType>;

    using common_rank_container = CLogicSQLRankAssociateContainer<uint64_t, TGlobalCommonRankTable*, TGlobalCommonRankTableValueType>;


    TGlobalOnlyCache m_stOnlyCache;
    CPackGlobalRecoveryData m_stRecoveryData;
    TGlobalBasicTable m_stBasicTable;
    charm_rank_container m_stCharmRank;
    countdown_rank_container m_stCountdownRank;
    entire_war_rank_container m_stEntireWarRank;
    guild_rank_container m_stGuildRank;
    home_rank_container m_stHomeRank;
    ibusy_rank_container m_stIbusyRank;
    match_pvp_rank_container m_stMatchPvpRank;
    military_rank_container m_stMilitaryRank;
    fight_power_rank_container m_stFightPowerRank;
    pvp_rank_container m_stPvpRank;
    tower_rank_container m_stTowerRank;

    last_pvp_rank_container m_stLastDayPvpRank;
    std::map<int32_t, global_mail_container> m_stGlobalMailMap;
    std::map<int32_t, fight_report_container> m_stFightReportMap;
    entire_war_map_container m_stEntireWarMap;
    std::map<int32_t, entire_war_extra_container> m_stEntireWarExtra;
    world_quest_container m_stWorldQuestMap;
    std::map<int32_t, common_rank_container> m_stCommonRankMap;
    std::vector<TGlobalStoreRewardTableValueType>    m_stStoreRewardTable;
};

#define LOGIC_GLOBAL_CACHE_INS CLogicGlobal::GetGlobalCache()

//////////////////////////////////////////////////////////////////////////////////////////////////////
///魅力值排行
#define LOGIC_CHARM_RANK_INS LOGIC_GLOBAL_CACHE_INS->m_stCharmRank

//////////////////////////////////////////////////////////////////////////////////////////////////////
///限时神将排行
#define LOGIC_COUNTDOWN_HERO_RANK_INS LOGIC_GLOBAL_CACHE_INS->m_stCountdownRank

///////////////////////////////////////////////////////////////////////////////////////////////////
//全民战役排行
#define LOGIC_ENTIRE_WAR_RANK_INS LOGIC_GLOBAL_CACHE_INS->m_stEntireWarRank

//////////////////////////////////////////////////////////////////////////////////////////////////
///公会排行
#define LOGIC_GUILD_RANK_INS LOGIC_GLOBAL_CACHE_INS->m_stGuildRank

//////////////////////////////////////////////////////////////////////////////////////////////////////
///家园排行
#define LOGIC_HOME_RANK_INS LOGIC_GLOBAL_CACHE_INS->m_stHomeRank

//////////////////////////////////////////////////////////////////////////////////////////////////////
///主公很忙排行
#define LOGIC_IBUSY_SCORE_RANK_INS LOGIC_GLOBAL_CACHE_INS->m_stIbusyRank

//////////////////////////////////////////////////////////////////////////////////////////////////////
///巅峰对决段位排行
#define LOGIC_MATCH_PVP_RANK_INS  LOGIC_GLOBAL_CACHE_INS->m_stMatchPvpRank

//////////////////////////////////////////////////////////////////////////////////////////////////////
///军衔排行
#define LOGIC_MILITARY_LEVEL_RANK_INS LOGIC_GLOBAL_CACHE_INS->m_stMilitaryRank

//////////////////////////////////////////////////////////////////////////////////////////////////
///战斗力排行
#define LOGIC_FIGHT_POWER_RANK_INS LOGIC_GLOBAL_CACHE_INS->m_stFightPowerRank

//////////////////////////////////////////////////////////////////////////////////////////////////
///竞技排行
#define LOGIC_PVP_RANK_INS LOGIC_GLOBAL_CACHE_INS->m_stPvpRank

//////////////////////////////////////////////////////////////////////////////////////////////////////
///镇魔之塔排行
#define LOGIC_CLIMB_TOWER_RANK_INS LOGIC_GLOBAL_CACHE_INS->m_stTowerRank
