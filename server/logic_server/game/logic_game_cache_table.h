#pragma once

#include "tc_singleton.h"
#include "server_base_protocol.h"
#include "server_pvp_protocol.h"
#include "server_home_protocol.h"
#include "logic_link_define.h"
#include "logic_game_lru_hashmap.h"
#include "logic_config_base.h"
#include "server_new_limit_boss_protocol.h"
#include "logic_record_sql_associate_container.hpp"
#include "logic_record_rank_sql_container.hpp"
#include "logic_record_user_avatar_table.h"
#include "logic_record_user_info_table.h"
#include "logic_record_user_mail_table.h"
#include "logic_record_level_info_table.h"
#include "logic_record_user_active_table.h"
#include "logic_record_user_deposit_table.h"
#include "logic_record_user_team_info_table.h"
#include "logic_record_hero_card_info_table.h"
#include "logic_record_muti_step_task_table.h"
#include "logic_record_cross_fight_info_table.h"
#include "logic_record_user_buy_commodity_table.h"
#include "logic_record_user_item_table.h"
#include "logic_record_user_stat_table.h"
#include "logic_record_first_recharge_table.h"
#include "logic_record_user_daily_table.h"
#include "logic_record_activity_task_table.h"
#include "logic_record_score_level_info_table.h"
#include "logic_record_user_home_garden_table.h"
#include "logic_record_user_home_explore_table.h"
#include "logic_record_fashion_info_table.h"
#include "logic_record_user_buy_back_table.h"
#include "logic_record_flower_lantern_table.h"
#include "logic_record_fight_report_table.h"
#include "logic_record_user_theme_activity_charge_table.h"
#include "logic_record_user_ibusy_market_table.h"
#include "logic_record_user_ibusy_info_table.h"
#include "logic_record_day_single_recharge_task_table.h"
#include "logic_record_merge_activity_table.h"
#include "logic_record_user_qiyu_table.h"
#include "logic_record_user_honor_table.h"
#include "logic_record_proto_monitor_table.h"
#include "logic_record_item_monitor_table.h"
#include "logic_record_month_pass_task_table.h"
#include "logic_record_user_wenwen_news_table.h"
#include "logic_record_user_hero_history_table.h"
#include "logic_record_user_random_store_table.h"
#include "logic_record_user_constellation_table.h"
#include "logic_record_user_constel_preset_table.h"
#include "logic_record_user_equip_table.h"
#include "logic_record_user_home_table.h"
#include "logic_record_user_home_goods_table.h"
#include "logic_record_user_home_footcloth_table.h"
#include "logic_record_user_fish_item_table.h"
#include "logic_record_user_quest_table.h"
#include "logic_record_user_friend_table.h"
#include "logic_record_user_friend_apply_table.h"
#include "logic_record_user_shop_table.h"
#include "logic_record_user_buff_table.h"
#include "logic_record_user_avy_table.h"
#include "logic_record_user_team_preset_info_table.h"
#include "logic_record_user_badge_table.h"
#include "logic_record_user_pay_diamond_table.h"
#include "logic_config_ibusy.h"
#include "logic_record_user_home_topic_table.h"

////////////////////////////////////////////////////////////////////////////
union user_info_key_t
{
    user_info_key_t() : user_info_key_t(0) { }

	user_info_key_t(int32_t iUin, int32_t iGroupID) : user_info_key_t()
    {
		m_stRoleIDDetail.m_iUin = iUin;
		m_stRoleIDDetail.m_iGroupID = iGroupID;
    }

	explicit user_info_key_t(uint64_t ulKey) : m_ulRoleID(ulKey) {}

    struct
    {
        int32_t m_iUin;
        int32_t m_iGroupID;
    }m_stRoleIDDetail;

    uint64_t    m_ulRoleID;
};

// 属性
struct attribute_elem_t
{
    attribute_elem_t() : m_stAttrValue{0} {}

    attribute_elem_t& operator += (const attribute_elem_t& other)
    {
        for(int iType = CLogicConfigDefine::ECAT_Invalid; iType < CLogicConfigDefine::ECAT_Max; ++iType)
        {
            this->m_stAttrValue[iType] += other.m_stAttrValue[iType];
        }

        return *this;
    }

    int32_t m_stAttrValue[CLogicConfigDefine::ECAT_Max];
};

struct banquet_invite
{
    CPackUserBrief      m_stBrief;
    int                 m_iHomeID;
    int                 m_iItemID;
    int                 m_iEndTime;
};

struct hot_spring_info
{
    int32_t m_iEndTime;
    int32_t m_iCardID;
    int32_t m_iSkinID;
    CPackUserBrief m_stUser;
};

struct only_cache_data_t
{
    only_cache_data_t() : m_iPVPFighterGroupID(0), m_iPVPFighterUin(0),m_iPVPFighterWinCount(0)
          ,m_stPVPFightHistory(),m_stPVPUserInfo(),m_stPVPRewardState(), m_iNextCheckLazyDataTime(0), m_iLimitBossLastRefreshOpenTime(0), m_stLimitBossInvite()
          , m_setPingbiUser(), m_stPrivateTalkCache(), m_iTodayLongTalkCount(0), m_iLastWorldTalkTime(0), m_iLastStartMatchTime(0), m_iMatchBaodiFailCount(0), m_iLastStartFairTime(0)
          , m_stFashionAttribute(), m_stThemeAttribute(), m_stMilitaryAttribute(), m_stHeartLevelAttribute(), m_stTalentAttributeMap()
          , m_stHonorAttributeMap(), m_stGuildAttributeMap(), m_iCacheFishID(0), m_iCacheFishValue(0), m_stCacheQuestIDSet()
          , m_stCacheBadgeIDSet(), m_iGuildWishLastTime(0), m_bHasLimitBoss(false),m_stBanquetInviteMap(),m_stUid2HomeVisit(),
          m_iVisitedInfo(),m_stSelfHotSpringInfo(),m_stFirstHotSpringInfo(),m_stHotSpringOtherInfo(),m_mapGuildBuff(),m_IndexAttr(){}
    
    int                                         m_iPVPFighterGroupID;
    int                                         m_iPVPFighterUin;
    int                                         m_iPVPFighterWinCount;  // pvp当此页面胜利场次
    std::list<CPackPvpFightLog>                 m_stPVPFightHistory;
    std::vector<CPackPvpUserInfo>               m_stPVPUserInfo;        // pvp刷新的挑战玩家信息
    std::map<int32_t ,bool>                     m_stPVPRewardState;     // 各个奖励领取情况


	int32_t										m_iNextCheckLazyDataTime;
    int32_t                                     m_iLimitBossLastRefreshOpenTime;

    // 限时BOSS邀请
    std::set<int32_t>                           m_stLimitBossInvite;

	//聊天屏蔽玩家id
	std::set<int>								m_setPingbiUser;
    std::unordered_map<int, CPackPrivateTalkOneUser> m_stPrivateTalkCache;
	int32_t										m_iTodayLongTalkCount;
    int32_t                                     m_iLastWorldTalkTime;

	//巅峰对决
	std::list<int32_t>							m_stMatchPvpHadUins;
	CPackPvpChallengerInfo			m_stMatchPvpPlayer;
	int32_t										m_iLastStartMatchTime;
	int32_t										m_iMatchBaodiFailCount;

	//公平竞技
	CPackPvpChallengerInfo			m_stFairPvpPlayer;
	int32_t										m_iLastStartFairTime;

	//缓存部分系统计算出的属性
    attribute_elem_t    m_stFashionAttribute;
    attribute_elem_t    m_stThemeAttribute;
    attribute_elem_t    m_stMilitaryAttribute;
    attribute_elem_t    m_stHeartLevelAttribute;
    std::map<int32_t, attribute_elem_t> m_stTalentAttributeMap;     //key：card_type
    std::map<int32_t, attribute_elem_t> m_stHonorAttributeMap;      //key: card_type or sexual
    std::map<int32_t, attribute_elem_t> m_stGuildAttributeMap;      //key：card_type

    int32_t                                     m_iCacheFishID;     // 家园钓鱼鱼ID
    int32_t                                     m_iCacheFishValue;  // 家园钓鱼的体型，0表示小鱼

    std::set<int32_t>                           m_stCacheQuestIDSet;    // 缓存更新的任务列表
    std::set<int32_t>                           m_stCacheBadgeIDSet;    // 缓存更新的徽章列表

    int32_t                                     m_iGuildWishLastTime;   // 公会祈愿上一次操作时间

    //是否出现限时boss
    bool                                     m_bHasLimitBoss;   //是否出现限时Boss

    // 宴会邀请
    std::unordered_map<uint64_t, banquet_invite>   m_stBanquetInviteMap;

    // 家园拜访(uin->HomeVisit)
    std::unordered_map<int32_t,CRequestHomeVisit> m_stUid2HomeVisit;
    // 当前拜访的玩家
    CRequestHomeVisit                   m_iVisitedInfo;

    //家园温泉信息
    hot_spring_info                     m_stSelfHotSpringInfo;
    hot_spring_info                     m_stFirstHotSpringInfo;
    std::unordered_map<int32_t,hot_spring_info> m_stHotSpringOtherInfo;

    //公会藏品解锁信息,每次获取公会信息的时候进行更新
    std::unordered_map<int32_t,TLogicGuildBuffElem> m_mapGuildBuff;

    //装备洗练的属性缓存
    std::unordered_map<int32_t,int32_t> m_IndexAttr;
};

typedef struct user_data_table_s
{
    typedef CLogicSQLRecordAssociateContainer<int32_t, TUserItemTable*, TUserItemTableValueType> item_container_type;

    typedef CLogicSQLRecordAssociateContainer<int32_t,
                                                THeroCardInfoTable*,
                                                THeroCardInfoTableValueType> hero_card_container_type;

    typedef CLogicSQLRecordAssociateContainer<int32_t,
                                                TLevelInfoTable*,
                                                TLevelInfoTableValueType> level_info_container_type;

    typedef CLogicSQLRecordAssociateContainer<int32_t,
                                                TUserBuyCommodityTable*,
                                                TUserBuyCommodityTableValueType> user_buy_commodity_container_type;

    typedef CLogicSQLRecordAssociateContainer<int32_t,
                                                TMutiStepTaskTable*,
                                                TMutiStepTaskTableValueType> muti_step_task_container_type;

    typedef CLogicSQLRecordAssociateContainer<int32_t,
                                                TUserMailTable*,
                                                TUserMailTableValueType> user_mail_container_type;

    typedef CLogicSQLRecordAssociateContainer<int,
                                                TActiveTable*,
                                                TActiveTableValueType> user_active_container_type;

	typedef CLogicSQLRecordAssociateContainer<int,
												TFlowerLanternTable*,
												TFlowerLanternTableValueType> flower_lantern_container_type;

    typedef CLogicSQLRecordAssociateContainer<int,
                                                TUserDepositTable*,
                                                TUserDepositTableValueType> user_deposit_container_type;

    typedef CLogicSQLRecordAssociateContainer<int,
                                                TCrossFightInfoTable*,
                                                TCrossFightInfoTableValueType> cross_fight_info_container_type;

	typedef CLogicSQLRecordAssociateContainer<int,
                                                TUserStatTable*,
                                                TUserStatTableValueType> user_stat_container_type;

	typedef CLogicSQLRecordAssociateContainer<int,
												TFirstRechargeTable*,
												TFirstRechargeTableValueType> first_recharge_container_type;

	typedef CLogicSQLRecordAssociateContainer<std::tuple<int32_t ,int32_t, int32_t>,
												TActivityTaskTable*,
												TActivityTaskTableValueType> activity_task_container_type;

	typedef CLogicSQLRecordAssociateContainer<int32_t,
												TMergeActivityTable*,
												TMergeActivityTableValueType> merge_activity_container_type;

	typedef CLogicSQLRecordAssociateContainer<int, TUserHomeGardenTable*, TUserHomeGardenTableValueType> home_garden_container_type;

	typedef CLogicSQLRecordAssociateContainer<int, TUserHomeExploreTable*, TUserHomeExploreTableValueType> home_explore_container_type;

	typedef CLogicSQLRecordAssociateContainer<int32_t,
												TUserBuyBackTable*,
												TUserBuyBackTableValueType> buy_back_container_type;

	typedef CLogicSQLRecordAssociateContainer<int32_t,
												TFashionInfoTable*,
												TFashionInfoTableValueType> fashion_info_container_type;

	typedef CLogicSQLRecordAssociateContainer<int32_t,
												TUserHonorTable*,
												TUserHonorTableValueType> user_honor_container_type;

	typedef CLogicSQLRecordAssociateContainer<int32_t,
												TFightReportTable*,
												TFightReportTableValueType> fight_report_container_type;

	typedef CLogicSQLRecordAssociateContainer<int32_t,
												TUserQiyuTable*,
												TUserQiyuTableValueType> qiyu_container_type;

	typedef CLogicSQLRecordAssociateContainer<int32_t,
												TUserThemeActivityChargeTable*,
												TUserThemeActivityChargeTableValueType> theme_activity_charge_container_type;

	typedef CLogicSQLRecordAssociateContainer<int,
												TUserIbusyMarketTable*,
												TUserIbusyMarketTableValueType> user_ibusy_market_container_type;

	typedef CLogicSQLRecordAssociateContainer<int,
			TScoreLevelInfoTable*,
			TScoreLevelInfoTableValueType> score_level_container_type;

	typedef CLogicSQLRecordAssociateContainer<std::pair<int32_t, int32_t>,
	        TDaySingleRechargeTaskTable*,
	        TDaySingleRechargeTaskTableValueType> day_single_recharge_task_container_type;

	typedef CLogicSQLRecordAssociateContainer<int32_t, TProtoMonitorTable*, TProtoMonitorTableValueType> proto_monitor_container_type;
	typedef CLogicSQLRecordAssociateContainer<std::pair<int32_t, int32_t>, TItemMonitorTable*, TItemMonitorTableValueType> item_monitor_container_type;

	using month_pass_task_container_type = CLogicSQLRecordAssociateContainer<int32_t, TMonthPassTaskTable*, TMonthPassTaskTableValueType>;

    using wenwen_news_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserWenWenNewsTable*, TUserWenWenNewsTableValueType>;

    using hero_history_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserHeroHistoryTable*, TUserHeroHistoryTableValueType>;

    using random_store_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserRandomStoreTable*, TUserRandomStoreTableValueType>;

    using constellation_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserConstellationTable*, TUserConstellationTableValueType>;

    using constel_preset_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserConstelPresetTable*, TUserConstelPresetTableValueType>;

    using equip_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserEquipTable*, TUserEquipTableValueType>;
    using home_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserHomeTable*, TUserHomeTableValueType>;
    using home_goods_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserHomeGoodsTable*, TUserHomeGoodsTableValueType>;
    using home_footcloth_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserHomeFootclothTable*, TUserHomeFootclothTableValueType>;
    using home_fish_item_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserFishItemTable*, TUserFishItemTableValueType>;
    using user_quest_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserQuestTable*, TUserQuestTableValueType>;
    using user_friend_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserFriendTable*, TUserFriendTableValueType>;
    using user_friend_apply_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserFriendApplyTable*, TUserFriendApplyTableValueType>;
    using user_shop_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserShopTable*, TUserShopTableValueType>;
    using user_buff_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserBuffTable*, TUserBuffTableValueType>;
    using user_avy_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserAvyTable*, TUserAvyTableValueType>;
    using user_team_preset_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserTeamPresetInfoTable*, TUserTeamPresetInfoTableValueType>;
    using user_badge_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserBadgeTable*, TUserBadgeTableValueType>;
    using user_avatar_container_type = CLogicSQLRecordAssociateContainer<int, TUserAvatarTable*, TUserAvatarTableValueType>;
    using user_home_topic_container_type = CLogicSQLRecordAssociateContainer<int32_t, TUserHomeTopicTable*, TUserHomeTopicTableValueType>;

  user_data_table_s()
    : m_strNick(""), m_strPid(""), m_strDeviceID(""),m_iChannelID(0), m_iUin(0), m_iGroupID(0), m_iCommand(0),
      m_stCacheBonusData(), m_stCacheFightLevelDropBonus(), m_stCacheZhuHaiDropBonus(), m_stUserInfoTable(), m_stUserTeamInfoTable(),
      m_stUserTeamPresetMap(), m_stItemMap(), m_stHeroCardTableMap(), m_stLevelInfoTableMap(), m_stMutiStepTaskTableMap(),
      m_stUserBuyCommodityMap(), m_stUserAvyMap(), m_stFirstRechargeMap(),
	  m_stUserStatTableMap(), m_stUserShopMap(), m_stUserBuffMap(), m_stBuyBackMap(),
	  m_stFashionInfoMap(), m_stFightReportMap(), m_stQiyuMap(), m_stThemeActivityChargeMap(),
	  m_stMonthPassTaskMap(), m_stWenWenNewsMap(), m_stHeroHistoryMap(), m_stRandomStoreMap(), m_stConstelMap(),
      m_stConstelPresetMap(), m_stEquipMap(), m_stHomeMap(), m_stHomeGoodsMap(), m_stHomeFootclothMap(), m_stFishItemMap(),
      m_stQuestMap(), m_stFriendMap(), m_stFriendApplyMap(), m_stUserBadgeMap(),m_stUserPayDiamondTable(),m_stUserHomeTopicMap() {}

    std::string                                     m_strNick;
    std::string                                     m_strPid;
    std::string                                     m_strDeviceID;
	int32_t											m_iChannelID;
    int                                             m_iUin;
    int                                             m_iGroupID;
    int                                             m_iCommand;

    CPackTotalItemData                              m_stCacheBonusData;
    only_cache_data_t                               m_stOnlyInCacheData;
    CPackItemVec                                    m_stCacheFightLevelDropBonus;
    std::vector<CPackItemVec>                       m_stCacheZhuHaiDropBonus;

    TUserInfoTable                                  m_stUserInfoTable;
	TUserDailyTable							        m_stUserDailyTable;
    TUserIbusyInfoTable						        m_stIBusyData;
    TUserTeamInfoTable                              m_stUserTeamInfoTable;
    user_team_preset_container_type                 m_stUserTeamPresetMap;
    item_container_type                             m_stItemMap;
    hero_card_container_type                        m_stHeroCardTableMap;
    level_info_container_type                       m_stLevelInfoTableMap;
    muti_step_task_container_type                   m_stMutiStepTaskTableMap;
    user_buy_commodity_container_type               m_stUserBuyCommodityMap;
    user_mail_container_type                        m_stUserMailMap;
    user_active_container_type                      m_stUserActiveMap;
	flower_lantern_container_type					m_stFlowerLanternMap;
    user_deposit_container_type                     m_stUserDepositMap;
    user_avatar_container_type                      m_stUserAvatarMap;
    cross_fight_info_container_type                 m_stCrossFightInfoMap;
	activity_task_container_type					m_stActivityTaskMap;
	merge_activity_container_type					m_stMergeActivityMap;
    user_avy_container_type                         m_stUserAvyMap;
	first_recharge_container_type					m_stFirstRechargeMap;
	user_stat_container_type						m_stUserStatTableMap;
	user_shop_container_type                        m_stUserShopMap;
    user_buff_container_type                        m_stUserBuffMap;
	buy_back_container_type							m_stBuyBackMap;
	fashion_info_container_type						m_stFashionInfoMap;
	user_honor_container_type						m_stUserHonorMap;
	std::map<int, fight_report_container_type>		m_stFightReportMap;
	std::map<int, qiyu_container_type>				m_stQiyuMap;
	std::map<int, theme_activity_charge_container_type>	m_stThemeActivityChargeMap;
	user_ibusy_market_container_type				m_stIBusyMarket;
	score_level_container_type					    m_stScoreLevelMap;
	day_single_recharge_task_container_type			m_stDaySingleRechargeMap;
    proto_monitor_container_type        m_stProtoMonitorMap;
    item_monitor_container_type			m_stItemMonitorMap;
    std::map<int32_t, month_pass_task_container_type> m_stMonthPassTaskMap;
    std::map<int32_t, wenwen_news_container_type>   m_stWenWenNewsMap;
    std::map<int32_t, hero_history_container_type>  m_stHeroHistoryMap;
    std::map<int32_t, random_store_container_type>  m_stRandomStoreMap;

    constellation_container_type                    m_stConstelMap;
    constel_preset_container_type                   m_stConstelPresetMap;
    equip_container_type                            m_stEquipMap;

    home_container_type                             m_stHomeMap;
    std::map<int32_t, home_goods_container_type>    m_stHomeGoodsMap;
    std::map<int32_t, home_footcloth_container_type> m_stHomeFootclothMap;
    home_explore_container_type					    m_stHomeExploreMap;
    std::map<int32_t, home_garden_container_type>	m_stHomeGardenMap;
    home_fish_item_container_type                   m_stFishItemMap;

    user_quest_container_type                       m_stQuestMap;
    user_friend_container_type                      m_stFriendMap;
    user_friend_apply_container_type                m_stFriendApplyMap;
    user_badge_container_type                       m_stUserBadgeMap;
    TUserPayDiamondTable                            m_stUserPayDiamondTable;
    user_home_topic_container_type                  m_stUserHomeTopicMap;
}user_data_table_t;

///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef user_data_table_t* user_data_table_ptr_type;

struct LRUCallBackFunctor {
    void operator() (const uint64_t& ulKey, const user_data_table_ptr_type & pUserInfo)
    {
        user_info_key_t stKey;
        stKey.m_ulRoleID = ulKey;
        
        delete pUserInfo;

        LOGIC_LOG_TRACE_DEBUG << "USER INFO HASH MAP KICK OFF|GROUP_ID:" << stKey.m_stRoleIDDetail.m_iGroupID
                        << "|UIN:" << stKey.m_stRoleIDDetail.m_iUin << std::endl;
    }
};


typedef CLogicLRUHashmap<uint64_t, user_data_table_ptr_type> LOGIC_USER_DATA_HASHMAP_TYPE;

template <class T>
class CLogicUserDataLRUHashmapCreator
{
public:
    static LOGIC_USER_DATA_HASHMAP_TYPE* create()
    {
        LOGIC_USER_DATA_HASHMAP_TYPE* pCacheUserData = new LOGIC_USER_DATA_HASHMAP_TYPE;
        assert(pCacheUserData != nullptr);
        std::function<void(const uint64_t&, const user_data_table_ptr_type &)> stLRUCallBackFunctor = LRUCallBackFunctor();
        pCacheUserData->SetKickOutNotifier(stLRUCallBackFunctor);
        
        return pCacheUserData;
    }

    static void destroy(LOGIC_USER_DATA_HASHMAP_TYPE* pCacheUserData)
    {
        if (pCacheUserData != nullptr)
        {
            for(auto stIter = pCacheUserData->begin(); stIter != pCacheUserData->end(); stIter ++)
            {
                delete stIter->second;
            }
            
            delete pCacheUserData;
        }
    }
};

typedef taf::TC_Singleton<LOGIC_USER_DATA_HASHMAP_TYPE, CLogicUserDataLRUHashmapCreator> LOGIC_USER_DATA_HASHMAP_INS_TYPE;

#define LOGIC_USER_DATA_HASHMAP_INS LOGIC_USER_DATA_HASHMAP_INS_TYPE::getInstance()
