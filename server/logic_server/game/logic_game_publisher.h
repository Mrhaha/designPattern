#pragma once

#include "logic_game_cache_table.h"

template<typename T>
class CLogicGameInfoChannel
{
public:
    typedef T data_type;

    typedef void (*channel_call_back_func)(user_data_table_ptr_type pUserData, const T& stData);

    void RegisterCallBack(channel_call_back_func pFunc)
    {
        m_stSubscriberList.push_back(pFunc);
    }

    void PublishInfo(user_data_table_ptr_type pUserData, const T& stData)
    {
        auto stChannelITBeg = m_stSubscriberList.begin();
        auto stChannelITEnd = m_stSubscriberList.end();
        for ( ; stChannelITBeg != stChannelITEnd; ++ stChannelITBeg)
            (*stChannelITBeg)(pUserData, stData);
    }

private:
    std::list<channel_call_back_func> m_stSubscriberList;
};

////////////////////////////////////////////////////////////////////////////////////////////////
class CLogicGameCombatChannel
{
public:
    typedef void (*channel_call_back_func)(user_data_table_ptr_type pUserData);

    void RegisterCallBack(channel_call_back_func pFunc)
    {
        m_stSubscriberList.push_back(pFunc);
    }

    void PublishInfo(user_data_table_ptr_type pUserData)
    {
        auto stChannelITBeg = m_stSubscriberList.begin();
        auto stChannelITEnd = m_stSubscriberList.end();
        for ( ; stChannelITBeg != stChannelITEnd; ++ stChannelITBeg)
            (*stChannelITBeg)(pUserData);
    }

private:
    std::list<channel_call_back_func> m_stSubscriberList;
};

////////////////////////////////////////////////////////////////////////////////////////////////
class CLogicGameUserLevelUpChannel
{
public:
    typedef void (*channel_call_back_func)(user_data_table_ptr_type pUserData);

    void RegisterCallBack(channel_call_back_func pFunc)
    {
        m_stSubscriberList.push_back(pFunc);
    }

    void PublishInfo(user_data_table_ptr_type pUserData)
    {
        auto stChannelITBeg = m_stSubscriberList.begin();
        auto stChannelITEnd = m_stSubscriberList.end();
        for ( ; stChannelITBeg != stChannelITEnd; ++ stChannelITBeg)
            (*stChannelITBeg)(pUserData);
    }

private:
    std::list<channel_call_back_func> m_stSubscriberList;
};

////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_game_item_add_channel_data_t
{
    logic_game_item_add_channel_data_t() : m_iItemType(0), m_iCardID(0), m_iNum(0) { }

    int     m_iItemType;
    int     m_iCardID;
    int     m_iNum;
};

typedef CLogicGameInfoChannel<logic_game_item_add_channel_data_t> CLogicGameItemAddChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_award_activity_multi_task_channel_data_t
{
	logic_award_activity_multi_task_channel_data_t() : m_iActivityID(0), m_iAwardNum(0) { }

	int32_t m_iActivityID;
	int32_t m_iAwardNum;
};

typedef CLogicGameInfoChannel<logic_award_activity_multi_task_channel_data_t> CLogicAwardActivityMultiTaskChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_game_energy_channel_data_t
{
	logic_game_energy_channel_data_t() : m_iNum(0) { }

	int32_t     m_iNum;
};

typedef CLogicGameInfoChannel<logic_game_energy_channel_data_t> CLogicGameEnergyChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_functional_game_item_channel_data_t
{
	logic_functional_game_item_channel_data_t() : m_iItemType(0), m_iItemID(0), m_iCurrentNum(0), m_iDeltaNum(0), m_iCmd(0), m_strInfo() { }

	int32_t		m_iItemType;
	int32_t     m_iItemID;
	int32_t     m_iCurrentNum;
	int32_t     m_iDeltaNum;
	int32_t		m_iCmd;
	std::string m_strInfo;
};

typedef CLogicGameInfoChannel<logic_functional_game_item_channel_data_t> CLogicFunctionalGameItemChannel;

////////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_climb_tower_order_channel_data_t
{
	logic_climb_tower_order_channel_data_t() : m_iOrder(0) {}

	int32_t m_iOrder;
};

typedef CLogicGameInfoChannel<logic_climb_tower_order_channel_data_t> CLogicClimbTowerOrderChannel;

////////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_pvp_territory_order_channel_data_t
{
	logic_pvp_territory_order_channel_data_t() : m_iOrder(0) {}

	int32_t m_iOrder;
};

typedef CLogicGameInfoChannel<logic_pvp_territory_order_channel_data_t> CLogicPvpTerritoryOrderChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_game_level_channel_data_t
{
    logic_game_level_channel_data_t() : m_iLevelID(0), m_iTimes(0) { }

    int     m_iLevelID;
    int     m_iTimes;
};

typedef CLogicGameInfoChannel<logic_game_level_channel_data_t> CLogicGameLevelChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
namespace CLogicGameChannelEnum {
    enum GAME_CARD_OPTION_ENUM
    {
        UPGRADE_CARD_STAR_OPTION = 0, 
        UPGRADE_CARD_LEVEL_OPTION = 1,
        UPGRADE_CARD_COLOR_OPTION = 2,
        UPGRADE_CARD_SKILL_LEVEL_OPTION = 3,
        UPGRADE_CARD_USE_EXP_DRINK = 4,
    };
};

struct logic_game_card_channel_data_t
{
    logic_game_card_channel_data_t() : m_iCardOption(CLogicGameChannelEnum::UPGRADE_CARD_STAR_OPTION) { }

    CLogicGameChannelEnum::GAME_CARD_OPTION_ENUM    m_iCardOption;
    int                                             m_iPara;
};

typedef CLogicGameInfoChannel<logic_game_card_channel_data_t> CLogicGameCardChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_game_mall_channel_data_t
{
    logic_game_mall_channel_data_t() : m_iCommodityNum(0), m_stGameItemList() { }

    int                                             m_iCommodityID;
    int                                             m_iCommodityNum;
    std::vector<CPackGameItem>  m_stGameItemList;
	std::vector<int>		m_stAmuletList;                     // TODO
};

typedef CLogicGameInfoChannel<logic_game_mall_channel_data_t> CLogicGameMallChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_game_deposit_channel_data_t
{
    logic_game_deposit_channel_data_t() : m_iNum(0) { }

    int     m_iNum;
};

typedef CLogicGameInfoChannel<logic_game_deposit_channel_data_t> CLogicGameDepositChannel;

/////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_pvp_rank_channel_data_t
{
    logic_pvp_rank_channel_data_t() : m_iCurrRank(0), m_iRankDelta(0) {}
    
    int                 m_iCurrRank;
    int                 m_iRankDelta;
};

typedef CLogicGameInfoChannel<logic_pvp_rank_channel_data_t> CLogicPVPRankChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
namespace CLogicGameChannelEnum {
	enum GAME_USER_ACTION_ENUM
	{
		ACTION_NONE = 0,
		ACTION_USE_SHAKE_GOLD = 1,			//使用摇钱树
		ACTION_PLAY_UNLIMIT_LEVEL = 2,		//通关无尽
		ACTION_PLAY_PVP = 3,				//竞技场
		ACTION_UPGRADE_WUSHEN_LEVEL = 4,	//升级武神
		ACTION_BUY_ENERGY = 5,				//购买体力
		ACTION_BUY_GROCERY = 6,				//杂货店购买道具
		ACTION_CROSS_FIGHT = 7,				//过关斩将关卡

		ACTION_BENEFIT_CARD_ACTIVE = 12,	//
		ACTION_BUY_BLACK_MARKET = 13,		//购买黑市商店
		ACTION_FIGHT_THEME_ACTIVE_LEVEL = 14,	//
		ACTION_BUY_VITALITY = 15,				//购买活力
		ACTION_COMPLETE_GUILD_TASK = 16,		//完成公会任务
		ACTION_HERO_TRAIN = 17,					//英雄培养
		ACTION_UPGRADE_TACTICS = 18,			//战术升级
		ACTION_GUILD_DONATE = 19,				//公会捐献
		ACTION_UNLOCK_TACTICS = 20,				//
		ACTION_ONLINE_TIME = 21,				//
		ACTION_LUCKY_BOX = 22,					//
		ACTION_SEND_ENERGY = 23,				//赠送体力
		ACTION_CONSUME_ENERGY = 24,				//消耗体力
		ACTION_PVP_WIN = 25,					//竞技场胜利
		ACTION_RECHARGE = 26,					//充值元宝
		ACTION_TURN_TABLE = 27,					//转盘
		ACTION_WISH_CHARACTER = 28,				//个性洗练
		ACTION_BREAK_EGG = 29,					//砸金蛋
		ACTION_ONLINE_SECOND = 30,				//在线时长(秒)
		ACTION_COUNTDOWN_HERO = 31,				//
		ACTION_PET_UPGRADE = 32,				//灵宠升级次数
		ACTION_SEND_FLOWER = 33,				//
		ACTION_LEVEL_PASS_COUNT = 34,			//
		ACTION_XUYUAN_TREE_COUNT = 35,			//
		ACTION_ENTIRE_WAR_COUNT = 36,			//全民战役成功次数
		ACTION_CREATE_FURNITURE_COUNT = 37,		//
		ACTION_VISIT_HOME_COUNT = 38,			//
		ACTION_SUMMON_AMULET = 39,				//命符抽取
		ACTION_GUILD_YUANBAO_DONATE = 40,		//公会钻石捐献次数
		ACTION_SIGN_COUNT = 41,					//签到次数
		ACTION_FAIR_PVP_12_WIN = 42,			//公平竞技12胜次数
		ACTION_BANK_EARN_GOLD = 43,				//诸葛钱庄盈利元气币数量
		ACTION_MULTI_COLOR_GRAND = 44,			//猜中彩灯特等级次数
		ACTION_GIVE_GUILD_WISH_COUNT = 45,		//公会许愿池赠送次数
		ACTION_ATK_DEF_WIN_COUNT = 46,			//据点攻防胜利次数
		ACTION_OPEN_CANGBAOTU_COUNT = 47,		//开启藏宝图次数
		ACTION_COMPLETE_TURN_CARD_COUNT = 48,	//开心乐翻天翻完次数
		ACTION_GUILD_GOLD_DONATE = 49,			//公会金币捐献次数
		ACTION_LUCKY_HAMMER = 50,				//幸运魔锤
		ACTION_EQUIP_UPGRADE_STAR = 51,         //装备升星次数
		ACTION_FIGHT_TEAM_WAR_COUNT = 52,       //组队副本战斗次数
		ACTION_MATCH_PVP_WIN_COUNT = 53,        //匹配PVP胜利次数
		ACTION_GUILD_TASK_5_STAR_COUNT = 54,    //接取5星公会任务
		ACTION_HORSE_LEVEL_COUNT = 55,          //金币副本次数
        ACTION_DAILY_QUEST_COUNT = 56,          //每日任务完成次数
        ACTION_QUN_LIAO_CARD_TASK_COUNT = 57,   //群疗神将任务完成次数
    };
};

struct logic_game_user_action_channel_data_t
{
    logic_game_user_action_channel_data_t()
    : m_iTimes(1) , m_iData(0),
    m_iAction(CLogicGameChannelEnum::ACTION_NONE) {}

    int                                             m_iTimes;
    int                                             m_iData;    
    CLogicGameChannelEnum::GAME_USER_ACTION_ENUM  m_iAction;
};

typedef CLogicGameInfoChannel<logic_game_user_action_channel_data_t> CLogicGameUserActionChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_game_unlimit_level_score_channel_data_t
{
    logic_game_unlimit_level_score_channel_data_t() : m_iAddScore(0) {}

    int                       m_iAddScore;
};

typedef CLogicGameInfoChannel<logic_game_unlimit_level_score_channel_data_t> CLogicGameUnlimitLevelScoreChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
struct logic_game_add_level_star_channel_data_t
{
    logic_game_add_level_star_channel_data_t() : m_iAddStar(0) {}

    int                       m_iAddStar;
};

typedef CLogicGameInfoChannel<logic_game_add_level_star_channel_data_t> CLogicGameAddLevelStarChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
namespace CLogicGameChannelEnum {
    enum GAME_HERO_EQUIPMENT_ACTION_ENUM
    {
        ACTION_UPGRADE_EQUIPMENT_LEVEL = 1,
        ACTION_UPGRADE_EQUIPMENT_COLOR = 2,
        ACTION_UPGRADE_EQUIPMENT_STAR = 3,
        ACTION_UPGRADE_EQUIPMENT_JEWEL = 4,
    };
};

struct logic_game_hero_equipment_channel_data_t
{
    logic_game_hero_equipment_channel_data_t() : m_iCardID(0), m_iIndex(0), m_iNum(0), m_enmActionType(CLogicGameChannelEnum::ACTION_UPGRADE_EQUIPMENT_LEVEL) {}

    int                                                         m_iCardID;
    int                                                         m_iIndex;
    int                                                         m_iNum;
    CLogicGameChannelEnum::GAME_HERO_EQUIPMENT_ACTION_ENUM    m_enmActionType;
};

typedef CLogicGameInfoChannel<logic_game_hero_equipment_channel_data_t> CLogicGameHeroEquipmentChannel;

////////////////////////////////////////////////////////////////////////////////////////////////
//
class CLogicPublisher
{
public:
    CLogicPublisher();

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameItemAdd
    void SubscribeGameItemAddChannel(CLogicGameItemAddChannel::channel_call_back_func pCallBack)
    {
        m_stGameItemAddChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameItemAddInfo(user_data_table_ptr_type pUserData, const CLogicGameItemAddChannel::data_type& stData)
    {
        m_stGameItemAddChannel.PublishInfo(pUserData, stData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// AwardActivityMultiTask
	void SubscribeAwardActivityMultiTaskChannel(CLogicAwardActivityMultiTaskChannel::channel_call_back_func pCallBack)
	{
		m_stAwardActivityMultiTaskChannel.RegisterCallBack(pCallBack);
	}

	void PublishAwardActivityMultiTaskInfo(user_data_table_ptr_type pUserData, const CLogicAwardActivityMultiTaskChannel::data_type& stData)
	{
		m_stAwardActivityMultiTaskChannel.PublishInfo(pUserData, stData);
	}

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameCard 
    void SubscribeGameCardChannel(CLogicGameCardChannel::channel_call_back_func pCallBack)
    {
        m_stGameCardChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameCardInfo(user_data_table_ptr_type pUserData, const CLogicGameCardChannel::data_type& stData)
    {
        m_stGameCardChannel.PublishInfo(pUserData, stData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameLevel
    void SubscribeGameLevelChannel(CLogicGameLevelChannel::channel_call_back_func pCallBack)
    {
        m_stGameLevelChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameLevelInfo(user_data_table_ptr_type pUserData, const CLogicGameLevelChannel::data_type& stData)
    {
        m_stGameLevelChannel.PublishInfo(pUserData, stData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameMall
    void SubscribeGameMallChannel(CLogicGameMallChannel::channel_call_back_func pCallBack)
    {
        m_stGameMallChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameMallInfo(user_data_table_ptr_type pUserData, const CLogicGameMallChannel::data_type& stData)
    {
        m_stGameMallChannel.PublishInfo(pUserData, stData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameCombat
    void SubscribeGameCombatChannel(CLogicGameCombatChannel::channel_call_back_func pCallBack)
    {
        m_stGameCombatChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameCombatInfo(user_data_table_ptr_type pUserData)
    {
        m_stGameCombatChannel.PublishInfo(pUserData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameDeposit
    void SubscribeGameDepositChannel(CLogicGameDepositChannel::channel_call_back_func pCallBack)
    {
        m_stGameDepositChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameDepositInfo(user_data_table_ptr_type pUserData, const CLogicGameDepositChannel::data_type& stData)
    {
        m_stGameDepositChannel.PublishInfo(pUserData, stData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GamePVPRank
    void SubscribeGamePVPRankChannel(CLogicPVPRankChannel::channel_call_back_func pCallBack)
    {
        m_stGamePVPRankChannel.RegisterCallBack(pCallBack);
    }
    
    void PublishGamePVPRank(user_data_table_ptr_type pUserData, const CLogicPVPRankChannel::data_type& stData)
    {
        m_stGamePVPRankChannel.PublishInfo(pUserData, stData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameUserLevel
    void SubscribeGameUserLevelUpChannel(CLogicGameUserLevelUpChannel::channel_call_back_func pCallBack)
    {
        m_stGameUserLevelUpChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameUserLevelUpInfo(user_data_table_ptr_type pUserData)
    {
        m_stGameUserLevelUpChannel.PublishInfo(pUserData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameUserAction 
    void SubscribeGameUserActionChannel(CLogicGameUserActionChannel::channel_call_back_func pCallBack)
    {
        m_stGameUserActionChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameUserActionInfo(user_data_table_ptr_type pUserData, const CLogicGameUserActionChannel::data_type& stData)
    {
        m_stGameUserActionChannel.PublishInfo(pUserData, stData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameUnlimitLevelScore
    void SubscribeGameUnlimitLevelScoreChannel(CLogicGameUnlimitLevelScoreChannel::channel_call_back_func pCallBack)
    {
        m_stGameUnlimitLevelScoreChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameUnlimitLevelScoreInfo(user_data_table_ptr_type pUserData, const CLogicGameUnlimitLevelScoreChannel::data_type& stData)
    {
        m_stGameUnlimitLevelScoreChannel.PublishInfo(pUserData, stData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameAddLevelStar
    void SubscribeGameAddLevelStarChannel(CLogicGameAddLevelStarChannel::channel_call_back_func pCallBack)
    {
        m_stGameAddLevelStarChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameAddLevelStarInfo(user_data_table_ptr_type pUserData, const CLogicGameAddLevelStarChannel::data_type& stData)
    {
        m_stGameAddLevelStarChannel.PublishInfo(pUserData, stData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameHeroEquipment
    void SubscribeGameHeroEquipmentChannel(CLogicGameHeroEquipmentChannel::channel_call_back_func pCallBack)
    {
        m_stGameHeroEquipmentChannel.RegisterCallBack(pCallBack);
    }

    void PublishGameHeroEquipmentInfo(user_data_table_ptr_type pUserData, const CLogicGameHeroEquipmentChannel::data_type& stData)
    {
        m_stGameHeroEquipmentChannel.PublishInfo(pUserData, stData);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//// GameEnergyChannel
	void SubscribeGameEnergyChannel(CLogicGameEnergyChannel::channel_call_back_func pCallBack)
	{
		m_stGameEnergyChannel.RegisterCallBack(pCallBack);
	}

	void PublishGameEnergyInfo(user_data_table_ptr_type pUserData, const CLogicGameEnergyChannel::data_type& stData)
	{
		m_stGameEnergyChannel.PublishInfo(pUserData, stData);
	}

////////////////////////////////////////////////////////////////////////////////////////////////
//// FunctionalGameItemChannel
	void SubscribeFunctionalGameItemChannel(CLogicFunctionalGameItemChannel::channel_call_back_func pCallBack)
	{
		m_stFunctionalGameItemChannel.RegisterCallBack(pCallBack);
	}

	void PublishFunctionalGameItemInfo(user_data_table_ptr_type pUserData, const CLogicFunctionalGameItemChannel::data_type& stData)
	{
		m_stFunctionalGameItemChannel.PublishInfo(pUserData, stData);
	}

//////////////////////////////////////////////////////////////////////////////////////////////
//// ClimbTowerOrderChannel
	void SubscribeClimbTowerOrderChannel(CLogicClimbTowerOrderChannel::channel_call_back_func pCallBack)
	{
		m_stClimbTowerOrderChannel.RegisterCallBack(pCallBack);
	}

	void PublishClimbTowerOrderInfo(user_data_table_ptr_type pUserData, const CLogicClimbTowerOrderChannel::data_type& stData)
	{
		m_stClimbTowerOrderChannel.PublishInfo(pUserData, stData);
	}

//////////////////////////////////////////////////////////////////////////////////////////////
//// PvpTerritoryOrderChannel
	void SubscribePvpTerritoryOrderChannel(CLogicPvpTerritoryOrderChannel::channel_call_back_func pCallBack)
	{
		m_stPvpTerritoryOrderChannel.RegisterCallBack(pCallBack);
	}

	void PublishPvpTerritoryOrderInfo(user_data_table_ptr_type pUserData, const CLogicPvpTerritoryOrderChannel::data_type& stData)
	{
		m_stPvpTerritoryOrderChannel.PublishInfo(pUserData, stData);
	}

private:
    CLogicPVPRankChannel                  m_stGamePVPRankChannel;
    CLogicGameCardChannel                 m_stGameCardChannel;
    CLogicGameMallChannel                 m_stGameMallChannel;
    CLogicGameLevelChannel                m_stGameLevelChannel;
    CLogicGameCombatChannel               m_stGameCombatChannel;
    CLogicGameDepositChannel              m_stGameDepositChannel;
    CLogicGameItemAddChannel              m_stGameItemAddChannel;
    CLogicGameUserActionChannel           m_stGameUserActionChannel;
    CLogicGameUserLevelUpChannel          m_stGameUserLevelUpChannel;
    CLogicGameAddLevelStarChannel         m_stGameAddLevelStarChannel;
    CLogicGameHeroEquipmentChannel        m_stGameHeroEquipmentChannel;
    CLogicGameUnlimitLevelScoreChannel    m_stGameUnlimitLevelScoreChannel;
	CLogicGameEnergyChannel				m_stGameEnergyChannel;				//体力变化
	CLogicFunctionalGameItemChannel       m_stFunctionalGameItemChannel;		//各种道具变化
	CLogicClimbTowerOrderChannel			m_stClimbTowerOrderChannel;			//PVE爬塔最高层数
	CLogicPvpTerritoryOrderChannel		m_stPvpTerritoryOrderChannel;		//PVP爬塔最高层数
	CLogicAwardActivityMultiTaskChannel   m_stAwardActivityMultiTaskChannel;	//完成悬赏任务
};

typedef taf::TC_Singleton<CLogicPublisher> LOGIC_PUBLISHER_INS_TYPE;

#define LOGIC_PUBLISHER_INS LOGIC_PUBLISHER_INS_TYPE::getInstance()
