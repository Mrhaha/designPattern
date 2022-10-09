#pragma once

#include <cstdint>
#include <set>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <cstddef>
#include <functional>
#include <algorithm>
#include <sstream>
#include <unordered_map>

#include "common_str2num.h"
#include "logic_config_base_define.h"

#define CONFIG_ERROR_MSG_HEADER "[" << __FILE__ << ":" << __LINE__ << "]|PARSE CONFIG ERROR|"

#define CONFIG_ASSERT_EXCEPTION(expr) \
	if (!(expr)) { strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CONFIG MUST " << #expr << std::endl; throw std::exception(); }

#define CONFIG_ASSERT_EXCEPTION_EX(expr, id) \
	if (!(expr)) { strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CONFIG MUST " << #expr << "|ID:" << id << std::endl; throw std::exception(); }

#define CONFIG_ASSERT_EXCEPTION_EXEX(expr, id, key) \
	if (!(expr)) { strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CONFIG MUST " << #expr << "|ID:" << id << "|KEY:" << key << std::endl; throw std::exception(); }

#define IS_VALID_ENUM_TYPE(enum_name, enum_head) \
    static bool IsValid##enum_name(int iType) { return (enum_head##_Invalid < iType && iType < enum_head##_Max); }

const int32_t EVERYDAY_REFRESH_HOUR = 5;
const int32_t VIP_MONTHLY_DEPOSIT_ID = 8;
const int32_t FAT_MONTHLY_DEPOSIT_ID = 16;
const int32_t CONSTELL_BASE_PRO = 5;
const int32_t UNRECEIVED_REWARD = 10;
const int32_t CONSTEL_SHOP_COUNT = 8;
const int32_t FRAGEMENT_HISTORY_SIZE = 1;
const int32_t COIN_LIMIT = 2000;
const int32_t HOME_TOPIC_UP_LIMIT = 5;
const int32_t HOME_TOPIC_NAME = 50;
const int32_t HOME_GOODS_PER_HOME_LIMIT = 50;
const int32_t CAL_TIME_INTERVAL = 10;

enum EnumFilterWordsType
{
    EnumFilterWordsType_Invalid_Type = 0,
    EnumFilterWordsType_Name_Type   = 1,
    EnumFilterWordsType_Chat_Type = 2,
};

enum EnumPvpRewardHeadBorder
{
    EnumPvpHeadBorderInvalidType = 0,
    EnumDefaultBorder = 20001,
    EnumPvpHeadBorderFirst = 20027,
    EnumPvpHeadBorderSecond = 20028,
    EnumPvpHeadBorderThird = 20029,
};


class CLogicConfigDefine
{
public:
    // 系统属性值定义
    enum EnumSysDefine
    {
        ESD_CardColorUpPosNum = 4,          // 伙伴进阶可镶嵌道具位置数量
        ESD_TeamMaxCardNum = 7,             // 伙伴阵容最大伙伴数量
        ESD_RandomWeight = 10000,
        ESD_NormalPctWeight = 100,          // 正常百分比分母值
        ESD_MaxBGMNum = 100,                // 背景音乐播放列表最大音乐数量
        ESD_LevelFullStarFlag = 7,          // 副本满星标记值
        ESD_LevelStarEvaluateNum = 3,       // 副本评星条件数量
        ESD_PvpHistoryMaxNum = 10,          // 竞技场最大战斗日志数量
        ESD_PvpFightMaxTime = 70,           // 竞技场战斗最长时间
        ESD_WorldTalkCacheCount = 100,      // 世界聊天缓存数量
        ESD_TalkLengthThreshold = 25,       // 聊天内容长度阈值
        ESD_TalkUserLevelThreshold = 30,    // 聊天玩家等级阈值
        ESD_PvpChallengeListNumber = 6,     // PVP挑战列表数量
        ESD_PvpWeeklyTaskNumber = 5,        // PVP每周任务数量
        ESD_PvpRandomSelectNumber = 20,     // PVP随机池的容量
        // **********************************************************************************

        // 各种类型道具ID范围 *****************************************************************
        // 伙伴(5)
        ESD_HeroCardIDMin = 100,
        ESD_HeroCardIDMax = 999,

        // 头像/头像框(21)
        ESD_AvatarIDMin = 10000,
        ESD_AvatarIDMax = 29999,

        // 货币(6)
        ESD_CurrencyIDMin = 60000,
        ESD_CurrencyIDMax = 69999,

        // 时装(7)
        ESD_FashionIDMin = 70000,
        ESD_FashionIDMax = 79999,

        // 各种道具(32)
        ESD_ExchangeItemIDMin = 3000,
        ESD_ExchangeItemIDMax = 9999,
        ESD_ExchangeItemExIDMin = 320000,
        ESD_ExchangeItemExIDMax = 329999,

        // 伙伴装备(43)
        ESD_EquipItemIDMin = 430000,
        ESD_EquipItemIDMax = 439999,

        // 星纹(47)
        ESD_ConstelIDMin = 470000,
        ESD_ConstelIDMax = 479999,
        // **********************************************************************************
    };
    static bool IsValidHeroCardItemID(int iID)      { return (ESD_HeroCardIDMin <= iID && iID <= ESD_HeroCardIDMax); }
    static bool IsValidAvatarItemID(int iID)        { return (ESD_AvatarIDMin <= iID && iID <= ESD_AvatarIDMax); }
    static bool IsValidCurrencyItemID(int iID)      { return (ESD_CurrencyIDMin <= iID && iID <= ESD_CurrencyIDMax); }
    static bool IsValidFashionItemID(int iID)       { return (ESD_FashionIDMin <= iID && iID <= ESD_FashionIDMax); }
    static bool IsValidExchangeItemID(int iID)      { return ((ESD_ExchangeItemIDMin <= iID && iID <= ESD_ExchangeItemIDMax)
                                                           || (ESD_ExchangeItemExIDMin <= iID && iID <= ESD_ExchangeItemExIDMax)); }
    static bool IsValidEquipItemID(int iID)         { return (ESD_EquipItemIDMin <= iID && iID <= ESD_EquipItemIDMax); }
    static bool IsValidConstelItemID(int iID)       { return (ESD_ConstelIDMin <= iID && iID <= ESD_ConstelIDMax); }

	enum ROLE_GENDER_ID
	{
		MALE_LEADING_HERO_ID = 101,
		FEMALE_LEADING_HERO_ID = MALE_LEADING_HERO_ID,
	};

	enum EnumCardType
	{
        ECT_Invalid = 0,
        ECT_Attack = 1,         // 攻
        ECT_Defense = 2,        // 防
        ECT_Support = 3,        // 辅

        ECT_Max,
	};
    IS_VALID_ENUM_TYPE(EnumCardType, ECT);

	// 伙伴装备位枚举
	enum EnumHeroEquipType
	{
	    EHET_Invalid = 0,
	    EHET_Weapon = 1,        // 武器
	    EHET_Clothes = 2,       // 衣服
	    EHET_Helmet = 3,        // 头饰
	    EHET_Decoration = 4,    // 饰品

	    EHET_Max,
	};
	IS_VALID_ENUM_TYPE(EnumHeroEquipType, EHET);

	enum DEPOSIT_TYPE
    {
	    DEPOSIT_TYPE_BEGIN = 0,
	    DEPOSIT_TYPE_YUANBAO = 1,           //元宝包
	    DEPOSIT_TYPE_MONTH_CARD = 2,        //月卡
	    DEPOSIT_TYPE_QUARTER_CARD = 3,      //至尊卡
	    DEPOSIT_TYPE_LIMITED_TIME = 4,      //限时礼包
	    DEPOSIT_TYPE_INSURE_FUND = 5,       //保险基金

	    DEPOSIT_TYPE_DAILY_BUY = 7,         //7每日限购
	    DEPOSIT_TYPE_TOTAL_BUY = 8,         //8终生限购
	    DEPOSIT_TYPE_WEEKLY_BUY = 9,        //9每周限购
	    DEPOSIT_TYPE_MERGE_GIFT = 10,       //10合服礼包
	    DEPOSIT_TYPE_THEME_CHARGE = 11,     //11充值主题活动的充值礼包
	    DEPOSIT_TYPE_IBUSY = 12,            //12主公很忙類型通行證
	    DEPOSIT_TYPE_SPECIAL_WEEK = 13,   //13特惠周礼
        DEPOSIT_TYPE_SPECIAL_MONTH = 14,   //14特惠月礼
        DEPOSIT_TYPE_SCORE_LEVEL = 15,      //15积分主题活动充值(学霸卡)
        DEPOSIT_TYPE_FACEBOOK = 16,         //facebook礼包
        DEPOSIT_TYPE_MONTH_PASS = 17,        //月通行证
        DEPOSIT_TYPE_MONTH_BUY = 18,        //18每月限购
        DEPOSIT_TYPE_OTHER_DEPOSIT = 19,    //19其他购买(金币或钻石购买，或者免费)

        DEPOSIT_TYPE_END,
    };

    // 道具类型
	enum GAME_ITEM_COLLECTION
    {
        GAME_ITEM_COLLECTION_START = 1,

        HERO_CARD     = 5,   // 伙伴
        CURRENCY      = 6,   // 货币
        FASHION       = 7,   // 时装(直接获得的永久时装)
        AVATAR        = 21,  // 头像，头像框，聊天气泡
        EXCHANGE_ITEM = 32,  // 普通道具
        EQUIPMENT     = 43,  // 伙伴装备
        CONSTELLATION = 47,  // 礼装

        GAME_ITEM_COLLECTION_END,
    };

    // 各种道具(32)子类型、功能类型
    enum EnumItemSubType
    {
        EIST_Invalid = -1,
        EIST_Normal = 0,            // 普通道具，默认类型

        EIST_Drink = 2,             // 经验酒   ---[经验值]
        EIST_Soul = 3,              // 伙伴碎片
        EIST_Music = 4,             // 音乐
        EIST_ThemeSkin = 5,         // 主界面皮肤
        EIST_SkillPoint = 6,        // 伙伴专属技能点
        EIST_RuneCard = 7,          // 符卡
        EIST_RuneConsume = 8,       // 合成符卡材料

        // 预留 20--30 给家园道具使用
        EIST_HomePage = 20,             // 墙纸
        EIST_HomeFloorBG = 21,          // 地板皮肤
        EIST_HomeFootcloth = 22,        // 地毯
        EIST_HomeOrnament = 23,         // 摆件
        EIST_HomeFishingRod = 24,       // 鱼竿
        EIST_HomeSeed = 25,             // 种子
        EIST_HomeComic = 26,            // 漫画
        EIST_HomeMeet = 27,            // 肉类
        EIST_HomeVage = 28,            // 菜类
        EIST_HomeCook = 29,            // 烹饪菜肴

        EIST_Max = 100,
    };
    IS_VALID_ENUM_TYPE(EnumItemSubType, EIST);

    // 道具使用效果类型
    enum EnumItemUseType
    {
        EIUT_Invalid = 0,
        EIUT_Fashion = 1,           // 获得时装
        EIUT_GiftBag = 2,           // 礼包
        EIUT_Buff = 3,              // BUFF     [buff_id]
        EIUT_ExploreCangBaoTu = 4,  // 探索藏宝图 [quest_id]

        EIUT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumItemUseType, EIUT);

    // 货币子类型/ID
    enum EnumCurrencyIDType
	{
        ECIT_GoldID = 60001,                // 金币
        ECIT_EnergyID = 60002,              // 体力
        ECIT_YuanBaoID = 60003,             // 免费元宝
        ECIT_RoleExpID = 60004,             // 玩家经验
        ECIT_RoleLevelID = 60005,           // 玩家等级

        ECIT_VIPExpID = 60007,              // VIP经验
        ECIT_EvilBrandID = 60008,           // 竞技币
        ECIT_VitalityID = 60009,            // 活力
        ECIT_GuildCoinID = 60010,           // 公会贡献

        ECIT_WarContributionID = 60011,     // 功勋值
        ECIT_IbusyExpID = 60012,            // XX很忙经验
        ECIT_ExploreExp = 60013,            // 家园探索经验
        ECIT_ScoreLevelID = 60014,          // 积分主题活动积分
        ECIT_ScoreLevelXingDongLiID = 60015, // 积分主题活动行动力
        ECIT_NewPassExpID = 60016,        // 月通行证经验

        ECIT_WarActionCoinID = 60018,       // 全名战役行动力
        ECIT_ConstelExpID = 60019,          // 礼装经验
        ECIT_PayYuanBaoID = 60020,          // 付费元宝
        ECIT_HomeEXP    = 60021,            // 家园经验
        ECIT_GuildTotalDonate = 60022,      // 社团总贡献
        ECIT_GuildSelfDonate = 60023,       // 社团个人贡献
        ECIT_ZhuHaiScore = 60024,           // 竹海积分
	};

    // 装备升阶方式
    enum EnumEquipmentUpgrade
    {
        Invalid = -1,
        USE_EQUIPMENT = 0,      // 使用同名装备
        USE_ITEM    = 1,        // 使用材料
    };


    // 角色封禁类型
    enum EnumUserBanType
    {
        EUBT_Invalid = -1,
        EUBT_BanUser = 0,           // 封号
        EUBT_BanTalk = 1,           // 禁言
        EUBT_DelUser = 2,           // 删号

        EUBT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumUserBanType, EUBT);

	enum CHAPTER_TYPE
	{
		NORMAL_CHAPTER = 1,
		ELITE_CHAPTER = 2,
		DEVIL_CHAPTER = 3,
		EVENT_CHAPTER = 4,
		UNLIMITED_CHAPTER = 5,
		TOWER_CHAPTER = 6,
		GUILD_CHAPTER = 7,
		TERRITORY_CHAPTER = 8,
	};

	enum CHAPTER_RANGE
	{
		NORMAL_CHAPTER_LIMIT = 10000,
		ELITE_CHAPTER_LIMIT = 20000,
		DEVIL_CHAPTER_LIMIT = 30000,
		EVENT_CHAPTER_LIMIT = 40000,
		TOWER_CHAPTER_LIMIT = 80000,
		TERRITORY_CHAPTER_LIMIT = 90000,
		GUILD_CHAPTER_LIMIT = 110000,
	};

	enum EVENT_LEVEL_ID
	{
		DIAOCHAN_LEVEL_ID = 30001,
		PAO_MA_LEVEL_ID = 30002,

		SINGLE_LEVEL_ID = 30007,
	};

	enum CHAPTER_ID_BASE
	{
		NORMAL_CHAPTER_BASE = 0,
		ELITE_CHAPTER_BASE = 1000,
		DEVIL_CHAPTER_BASE = 2000,
		EVENT_CHAPTER_BASE = 3000,
	};

    // BUFF效果类型
    enum EnumBuffType
    {
        EBT_Invalid = 0,
        EBT_MonthCard = 1,                  // 月卡
        EBT_AddAttr = 2,                    // 增加属性
        EBT_AddLevelTypeExplodeRate = 3,    // 指定类型副本爆率
        EBT_LastCard = 4,                   // 持续礼包
        EBT_FatMonthCard = 5,               // 体力月卡

        EBT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumBuffType, EBT);

    // 全民战役据点类型
	enum EnumEntireWarNodeType
	{
        EEWNT_Invalid = 0,
		EEWNT_PVE = 1,			// 普通PVE
        EEWNT_PVP = 2,			// PVP
        EEWNT_Boss = 3,			// BOSS
        EEWNT_Question = 4,		// 答题
        EEWNT_Market = 5,		// 集市
        EEWNT_Treasure = 6,		// 宝藏
        EEWNT_Creeps = 7,		// 野怪
        EEWNT_SimpleGame = 8,   // 小游戏

        EEWNT_Max,
	};
    IS_VALID_ENUM_TYPE(EnumEntireWarNodeType, EEWNT);

	enum HONOR_STATUS
	{
		HONOR_STATUS_NONE = 0,
		HONOR_STATUS_UNLOCK = 1,
		HONOR_STATUS_EQUIP = 2,
	};

	enum TIME_TYPE_ACTIVITY
	{
		TIME_TYPE_SERVER = 1,
		TIME_TYPE_CREATE = 2,
		TIME_TYPE_CUSTOM = 3,
		TIME_TYPE_MERGE = 5,
		TIME_TYPE_NORMAL_SERVER = 6,
	};

    // 活动开启时间类型
    enum EnumAvyOpeningType
    {
        EAOT_Invalid = 0,
        EAOT_Always = 1,            // 一直开放
        EAOT_Server = 2,            // 开服时间
        EAOT_Create = 3,            // 创角时间
        EAOT_CustomCreate = 4,      // 相对创角时间
        EAOT_MergeServer = 5,       // 合服时间
        EAOT_AbsoluteDate = 6,      // 绝对时间

        EAOT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumAvyOpeningType, EAOT);

    // 副本星级评价类型
	enum EnumLevelEvaluateType
	{
        ELET_Invalid = 0,
        ELET_GoldCollectRate = 1,           // 金币收集率 收集的金币大于关卡中所有金币的某个百分比
        ELET_MonsterKillRate = 2,           // 怪物击杀率 杀死的怪物占关卡总怪物数的比例大于特定值
        ELET_DeadCardNum = 3,               // 阵亡人物   阵亡人物小于某个值
        ELET_TeamRemainHpRate = 4,          // 队伍剩余生命百分比 所有队员（不包含救出的援军）通关时候的剩余生命值占总生命值百分比
        ELET_BossTimeLimit = 5,             // 限时击杀BOSS 遇到BOSS后,开始倒数,BOSS在XX秒以内被杀死
        ELET_GreaterAmount = 6,             // 大于参数的值 过关时指定指标大于某个值

        ELET_Max,
	};
    IS_VALID_ENUM_TYPE(EnumLevelEvaluateType, ELET);

	enum
	{
		MAX_PVP_RANK_AMOUNT = 30,
		MAX_FIGHT_POWER_RANK_AMOUNT = 30,
		MAX_CLIMB_TOWER_RANK_AMOUNT = 30,
		MAX_COUNTDOWN_HERO_RANK_AMOUNT = 30,
	};

	enum TASK_TYPE
	{
		MAIN_TASK = 0,
		ACHIEVEMENT_TASK = 1,
		DAILY_TASK = 2,
	};

	enum MONTH_PASS_TYPE
    {
	    MONTH_PASS_TYPE_DAILY_TASK = 1,
        MONTH_PASS_TYPE_WEEKLY_TASK = 2,
	    MONTH_PASS_TYPE_EXCHANGE = 3,
	    MONTH_PASS_TYPE_WISH = 4,
    };

	enum INSPIRE_EXTRA_ATTRIBUTE
	{
		INSPIRE_EXTRA_FIXED_HP = 4,
		INSPIRE_EXTRA_PERCENT_HP = 3,
		INSPIRE_EXTRA_FIXED_ATK = 9,
		INSPIRE_EXTRA_PERCENT_ATK = 8,
		INSPIRE_EXTRA_ATTR1 = 7,
		INSPIRE_EXTRA_ATTR2 = 14,
		INSPIRE_EXTRA_ATTR3 = 5,
		INSPIRE_EXTRA_ATTR4 = 6,
	};

	enum USER_MAIL
	{
		MAX_MAIL_AMOUNT = 100,
		UNREAD_MAIL_MAX_REMAIN_TIME = 30,
		READ_MAIL_SAVE_TIME = 1,
        READ_ITEM_MAIL_SAVE_TIME = 3,
	};

	enum struct AVATAR_DEFINE
	{
		ICON_TYPE = 1,
		BORDER_TYPE = 2,
	};

	enum
	{
		LOGIN_INSURE_CARD_ID = 410,
		HERO_INSURE_CARD_ID = 411,
		HOME_INSURE_CARD_ID = 412,
		PET_INSURE_CARD_ID = 413,
		LOGIN_FUND_1_CARD_ID = 415,
        LOGIN_FUND_2_CARD_ID = 416,
        LOGIN_FUND_3_CARD_ID = 417,
        LOGIN_FUND_4_CARD_ID = 418,
        LOGIN_FUND_5_CARD_ID = 419,

		DOUBLE_DEPOSIT_CARD_ID = 420,
		RESET_UNLIMIT_SCORE_CARD_ID = 421,
		SCORE_LEVEL_SCORE_CARD_ID = 422,

        GROW_FOUNDATION_CARD_ID = 423,      // 成长计划
        HEART_INSURE_CARD_ID = 424,         // 好感度基金
	};

	enum
	{
		NORMAL_FIRST_CHAPTER_BONUS = 1,
		NORMAL_SECOND_CHAPTER_BONUS = 2,
		NORMAL_THIRD_CHAPTER_BONUS = 3,
		MAX_NORMAL_CHAPTER_BONUS_AMOUNT = 3,
	};

	enum
	{
		COUNTRY_WEI = 1,
		COUNTRY_SHU = 2,
		COUNTRY_WU = 3,
	};

	enum
	{
		FLOWER_LANTERN_EVENT_LOGIN = 1,
		FLOWER_LANTERN_EVENT_PAY = 2,
	};

	enum ENTIRE_WAR_LIMIT_TYPE
	{
		ENTIRE_WAR_LIMIT_NULL = 0,
		ENTIRE_WAR_LIMIT_TEAN_MEMBER = 1,
		ENTIRE_WAR_LIMIT_HERO_TYPE = 2,
		ENTIRE_WAR_LIMIT_HERO_ID = 3,
		ENTIRE_WAR_LIMIT_HERO_SEX = 4,
	};

    // 商店道具开启规则类型
	enum EnumShopRuleType
	{
        ESRT_Invalid = -1,
        ESRT_Empty = 0,         // 无条件
        ESRT_VIP = 1,           // VIP等级
        ESRT_Level = 2,         // 玩家等级
        ESRT_Hero = 3,          // 拥有特定伙伴
        ESRT_Server = 4,        // 开服天数

        ESRT_Max,
	};
    IS_VALID_ENUM_TYPE(EnumShopRuleType, ESRT);

	enum
	{
		ACTIVITY_TYPE_NONE = 0,
		ACTIVITY_TYPE_DINNER = 1,								//领取体力
		ACTIVITY_TYPE_CUMULATE_LOGIN = 2,						//开服登录（目前活动用2个，开服计算）
		ACTIVITY_TYPE_CUMULATE_DEPOSIT = 3,						//累计充值（区间段）
		ACTIVITY_TYPE_CORNUCOPIA = 4,							//聚宝盆
		ACTIVITY_TYPE_FOUNDATION = 5,							//成长基金
		ACTIVITY_TYPE_LEVEL_RACE = 6,							//冲级礼包
		ACTIVITY_TYPE_HERO_FLASH_SALE = 7,						//名将抢购（以前的，目前未用）
		ACTIVITY_TYPE_OPEN_SERVER_LOGIN = 8,					//登录奖励2（开服狂欢里已注释）
		ACTIVITY_TYPE_OPEN_SERVER_BUY = 9,						//开服抢购
		ACTIVITY_TYPE_NORMAL_LEVEL = 10,						//普通通关
		ACTIVITY_TYPE_ELITE_LEVEL = 11,							//精英通关
		ACTIVITY_TYPE_PVP_RANK = 12,							//竞技排名
		ACTIVITY_TYPE_UNLIMIT_LEVEL_SCORE = 13,					//无限积分
		ACTIVITY_TYPE_PASS_LEVEL_STAR = 14,						//副本星数
		ACTIVITY_TYPE_PASS_LEVEL_TIMES = 15,					//通关次数
		ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_LEVEL = 16,		//装备强化
		ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_STAR = 17,			//装备觉醒
		ACTIVITY_TYPE_HERO_UPGRADE_COLOR = 18,					//伙伴升品
		ACTIVITY_TYPE_HERO_UPGRADE_LEVEL = 19,					//伙伴升级
		ACTIVITY_TYPE_HERO_UPGRADE_STAR = 20,					//伙伴升星
		ACTIVITY_TYPE_WUSHEN_UPGRADE_LEVEL = 21,				//星魂升级
		ACTIVITY_TYPE_TEAM_LEVEL_UPGRADE = 22,					//等级突破
		ACTIVITY_TYPE_UPGRADE_SKILL = 23,						//升级技能
		ACTIVITY_TYPE_FIGHT_POWER = 24,							//战力突破
		ACTIVITY_TYPE_HERO_ACHIEVE = 25,						//招募英雄
		ACTIVITY_TYPE_BENEFIT_STORE = 26,						//福利商店
		ACTIVITY_TYPE_EXCHANGE_STORE = 27,						//国庆兑换
		ACTIVITY_TYPE_SINGLE_DEPOSIT = 28,						//单笔充值（每日刷新）
		ACTIVITY_TYPE_DEADLINE_LOGIN = 29,						//限时登录（区间段登录）
		ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_JEWEL = 30,		//镶嵌宝石
		ACTIVITY_TYPE_BUY_COMMODITY = 31,						//限时抽卡（累计钻石招募消耗次数）
		ACTIVITY_TYPE_DAILY_CUMULATE_DEPOSIT = 32,				//今日累充（每日刷新）
		ACTIVITY_TYPE_FIGHT_THEME_ACTIVE_LEVEL = 33,			//通关主题活动关卡次数
		ACTIVITY_TYPE_PVP_FIGHT_TIMES = 34,						//PVP 挑战次数
		ACTIVITY_TYPE_ELITE_LEVEL_FIGHT_TIMES = 35,				//精英挑战次数
		ACTIVITY_TYPE_NORMAL_LEVEL_FIGHT_TIMES = 36,			//主线挑战次数
		ACTIVITY_TYPE_BUY_ENERGY_TIMES = 37,					//体力购买
		ACTIVITY_TYPE_BUY_GOLD_TIMES = 38,						//摇钱树购买
		ACTIVITY_TYPE_BUY_BLACK_STORE_TIMES = 39,				//黑市购买
		ACTIVITY_TYPE_CUMULATE_CONSUME_YUANBAO = 40,			//累计消耗钻石
		ACTIVITY_TYPE_CROSS_FIGHT_PASS_LEVEL_TIMES = 41,		//过关斩将过关次数
		ACTIVITY_TYPE_UNLIMIT_LEVEL_FIGHT_TIMES = 42,			//无限之境挑战次数
		ACTIVITY_TYPE_THEME_ACTIVE_LEVEL_PROGRESS = 43,			//主题活动关卡挑战进度
		ACTIVITY_TYPE_SINGLE_ITEM_EXCHANGE = 44,				//单条目物品兑换
		ACTIVITY_TYPE_UPGRADE_TACTICS = 45,						//升级战术
		ACTIVITY_TYPE_HERO_TRAIN = 46,							//伙伴培养
		ACTIVITY_TYPE_BREAK_EGG = 47,							//砸金蛋
		ACTIVITY_TYPE_YUANBAO_RECRUIT = 48,						//元宝招募
		ACTIVITY_TYPE_CONSUME_ENERGY = 49,						//消耗体力
		ACTIVITY_TYPE_CLIMB_TOWER_ORDER = 50,					//PVE爬塔层数
		ACTIVITY_TYPE_PVP_TERRITORY_ORDER = 51,					//PVP攻城略地层数
		ACTIVITY_TYPE_TACTICS_COUNT = 52,						//战术数量
		ACTIVITY_TYPE_GUILD_DONATE = 53,						//公会捐献次数
		ACTIVITY_TYPE_GUILD_TASK = 54,							//公会任务次数
		ACTIVITY_TYPE_DAILY_CONSUME_YUANBAO = 55,				//每日消耗钻石
		ACTIVITY_TYPE_LUCKY_BOX = 56,							//幸运魔盒抽奖
		ACTIVITY_TYPE_TALENT_SCORE = 57,						//天赋图鉴分数
		ACTIVITY_TYPE_TOTAL_HERO_TRAIN = 58,					//伙伴一生培养
		ACTIVITY_TYPE_TURN_TABLE = 59,							//转盘抽奖
		ACTIVITY_TYPE_REVELRY = 60,								//元气狂欢季
		ACTIVITY_TYPE_MULTI_TASK = 61,							//皇叔悬赏（多重任务）
		ACTIVITY_TYPE_LOGIN_INSURE = 62,						//登录保险
		ACTIVITY_TYPE_HERO_INSURE = 63,							//伙伴保险
		ACTIVITY_TYPE_HOME_INSURE = 64,							//家园保险
		ACTIVITY_TYPE_WISH_CHARACTER = 65,						//个性修炼
		ACTIVITY_TYPE_DOUBLE_RECHARGE = 66,						//双倍充值
		ACTIVITY_TYPE_QUN_LIAO_CARD = 67,                       //群疗神将
		ACTIVITY_TYPE_XUNBAO = 68,								//寻宝乐园
		ACTIVITY_TYPE_COUNTDOWN_HERO = 69,						//限时神将
		ACTIVITY_TYPE_FLOWER_LANTERN = 70,						//花灯
		ACTIVITY_TYPE_PET_INSURE = 71,							//灵宠险
		ACTIVITY_TYPE_SEND_FLOWER = 72,							//赠送鲜花
        ACTIVITY_TYPE_LIMIT_DEPOSIT_GIFT = 73,					//限时充值礼包
        ACTIVITY_TYPE_LUCKY_HAMMER = 74,						//幸运魔锤
        ACTIVITY_TYPE_LOGIN_FUND = 75,                          //登录基金(空)
        ACTIVITY_TYPE_LOGIN_FUND_1 = 76,                        //登录基金1
        ACTIVITY_TYPE_LOGIN_FUND_2 = 77,                        //登录基金2
        ACTIVITY_TYPE_LOGIN_FUND_3 = 78,                        //登录基金3
        ACTIVITY_TYPE_LOGIN_FUND_4 = 79,                        //登录基金4
        ACTIVITY_TYPE_LOGIN_FUND_5 = 80,                        //登录基金5
        ACTIVITY_TYPE_HEART_INSURE = 81,                        //好感度基金
	};

	enum
    {
        BANK_PRICE_NODE_COUNT = 8,
        HAPPY_TURN_CARD_COUNT = 12,
        MULTI_COLOR_OPEN_COUNT = 3,
        MULTI_COLOR_CARD_COUNT = 15,
        ENTIRE_WAR_TREASURE_COUNT = 6,
    };

    //日志类型
    enum
    {
        FIGHT_REPORT_DIANFENGJINGJI = 1,
        FIGHT_REPORT_ENTIRE_WAR = 2,
        FIGHT_REPORT_HOME = 3,
        FIGHT_REPORT_FAIR_PVP = 4,
        FIGHT_REPORT_GUILD_ATKDEF = 5,
        FIGHT_REPORT_QIYU = 6,
        FIGHT_REPORT_CANGBAOTU = 7,
        FIGHT_REPORT_MONTH_PASS = 8,
    };

    //各日志类型的子类型triggervalue定义
    enum
    {
        ENTIRE_WAR_FIGHT_REPORT_GOD_REPORT_TRIGGER_VALUE = 999,
    };

    // 协议监控，物品监控
	enum EnumMonitorTimeType
	{
		EMTT_Invalid       = -1,
		EMTT_Daily         = 0,        // 按日统计
		EMTT_Weekly        = 1,        // 按周统计
		EMTT_Monthly       = 2,        // 按月统计
		EMTT_Total         = 3,        // 终身统计
		EMTT_MAX,
	};

	// 特惠周，月礼
	enum EnumSpecialGiftType
    {
	    ESGT_Invalid    = 0,
	    ESGT_Week       = 1,
	    ESGT_Month      = 2,
	    ESGT_MAX,
    };

    // 文文日报事件类型
    enum CEnumWenWenNewsType
    {
        EWWNT_Invalid,
        EWWNT_PVE_Level     = 1,    // 通关关卡
        EWWNT_PVP_Score     = 2,    // pvp积分
        EWWNT_Role_Level    = 3,    // 角色等级
        EWWNT_PVE_Time      = 4,    // 关卡速通

        EWWNT_Max,
    };

    // 伙伴品阶
    enum EnumCardGradeType
    {
        ECGT_Invalid = 0,
        ECGT_R = 1,
        ECGT_SR = 2,
        ECGT_SSR = 3,
        ECGT_UR = 4,
        ECGT_MR = 5,
        ECGT_SP = 6,
        ECGT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumCardGradeType, ECGT);

    // 服务器保存阵容枚举
    enum EnumTeamType
    {
        ETT_Invalid = 0,
        ETT_MainTeam = 1,           // 主线队伍
        ETT_PvpTeam = 2,            // PVP阵容
        ETT_CrossFight = 3,         // 月球远征
        ETT_AtkTeam = 4,
        ETT_DefTeam = 5,
        ETT_SupTeam = 6,
        ETT_PvpDefense = 7,         // PVP防守阵容
        ETT_EntireWarTeam = 8,      // 全名战役
        ETT_QiyuTeam = 9,           // 奇遇
        ETT_CangbaotuTeam = 10,     // 藏宝图

        ETT_Team11 = 11,            // 装备副本
        ETT_Team12 = 12,            // 礼装副本
        ETT_ZhuHaiTeam = 13,        // 竹海
        ETT_Team14 = 14,
        ETT_Team15 = 15,
        ETT_Team16 = 16,
        ETT_Team17 = 17,
        ETT_Team18 = 18,
        ETT_Team19 = 19,
        ETT_Team20 = 20,

        ETT_Team21 = 21,
        ETT_Team22 = 22,
        ETT_Team23 = 23,
        ETT_Team24 = 24,
        ETT_Team25 = 25,
        ETT_Team26 = 26,
        ETT_Team27 = 27,
        ETT_Team28 = 28,
        ETT_Team29 = 29,
        ETT_Team30 = 30,

        ETT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumTeamType, ETT);

    // 商店刷新类型
    enum EnumStoreLimitType
    {
        ESLT_Invalid = 0,
        ESLT_Daily = 1,     // 每天
        ESLT_Weekly = 2,    // 每周
        ESLT_Monthly = 3,   // 每月
        ESLT_Permanent = 4, // 永久
		ESLT_Unlimit = 5,	// 无限制

        ESLT_Max,
    };
    // 判断枚举值有效性
    IS_VALID_ENUM_TYPE(EnumStoreLimitType, ESLT);

    // 副本类型
    enum EnumLevelType
    {
        ELT_Invalid = -1,
        ELT_Normal = 0,             // 普通类，未指定副本类型的副本
        ELT_HeartLevel = 1,         // 好感度副本
        ELT_Constel = 2,            // 礼装副本
        ELT_MainLineLevel = 3,      // 主线副本
        ELT_EliteLevel = 4,         // 精英副本
        ELT_GoldLevel = 5,          // 金币副本
        ELT_ExpLevel = 6,           // 经验副本
        ELT_EquipLevel = 7,         // 装备副本
        ELT_ZhuHaiLevel = 8,        // 竹海副本
        ELT_GuildBossLevel = 9,     // 公会BOSS副本
        ELT_ShowLevel = 10,         // 剧情展示关卡
        ELT_LimitBossLevel = 11,    // 限时boss关卡
        ELT_GuildWarBossLevel = 12, // 公会战关卡

        ELT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumLevelType, ELT);

    // 随机商店类型
    enum EnumRandomStoreType
    {
        ERST_Invalid = -1,
        ERST_Normal = 0,        // 普通随机商店
        ERST_HeartStore = 1,    // 好感度随机商店

        ERST_Max,
    };
    IS_VALID_ENUM_TYPE(EnumRandomStoreType, ERST);

    // 玩家一次性事件标记(关联字段 m_stIBusyData.GetOnceEventStat())
    enum EnumUserRewardStatType
    {
        EURST_Invalid = -1,
        EURST_GuildBossFirstAward = 0,          // 公会BOSS首通奖励

        EURST_Max,
    };
    IS_VALID_ENUM_TYPE(EnumUserRewardStatType, EURST);

    // 玩家每日奖励通用状态标志位类型(关联字段 m_stUserDailyTable.GetDailyAwardState())
    enum EnumDailyAwardStateType
    {
        EDAST_Invalid = -1,
        EDAST_HeartDialogAward = 0,             // 好感度伙伴互动奖励

        EDAST_Max,
    };
    IS_VALID_ENUM_TYPE(EnumDailyAwardStateType, EDAST);

    // 伙伴印象
    enum EnumHeroImpressionType
    {
        EHIT_Invalid = 0,
        EHIT_Tsundere = 1,          // 傲娇
        EHIT_Scheming = 2,          // 腹黑
        EHIT_Adorkable = 3,         // 呆萌
        EHIT_Healing = 4,           // 治愈

        EHIT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumHeroImpressionType, EHIT);

    // 小游戏奖励类型
    enum EnumSimpleGameRewardType
    {
        ESGRT_Invalid = 0,
        ESGRT_PointReward = 1,      // 积分类奖励
        ESGRT_VictoryReward = 2,    // 胜负类奖励

        ESGRT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumSimpleGameRewardType, ESGRT);

    // 小游戏类型
    enum EnumSimpleGameType
    {
        ESGAMET_Invalid = 0,
        ESGAMET_WhacAMole = 1,      // 打地鼠
        ESGAMET_XCrossingDice = 2,  // 猜骰子
        ESGAMET_FlyCannonball = 3,  // 飞天大炮弹

        ESGAMET_Max,
    };
    IS_VALID_ENUM_TYPE(EnumSimpleGameType, ESGAMET);

    // 月球远征道具类型
    enum EnumMoonItemType
    {
        EMIT_Invalid = 0,
        EMIT_SpecificItem = 1,  // 专属道具
        EMIT_Buff = 2,          // BUFF
        EMIT_DataItem = 3,      // 数据道具
        EMIT_HelperNpc = 4,     // 助战NPC

        EMIT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumMoonItemType, EMIT);

    // 月球远征BUFF五行属性类型
    enum EnumFiveElementsType
    {
        EFET_Invalid = 0,
        EFET_Metal = 1,         // 金
        EFET_Wood = 2,          // 木
        EFET_Water = 3,         // 水
        EFET_Fire = 4,          // 火
        EFET_Earth = 5,         // 土
        EFET_Debuff = 6,        // 毒
        EFET_Other = 7,         // 其他默认，主要用于地图专属技能BUFF

        EFET_Max,
    };
    IS_VALID_ENUM_TYPE(EnumFiveElementsType, EFET);

    enum EnumTalkChannelID
    {
        talk_channel_guild = 1,
    };

    // 通用事件类型总表
    enum EnumCommonEventType
    {
        ECET_Invalid = 0,
        ECET_GuildWarWin = 1,                   // 公会战胜利
        ECET_CardCommodityGradeNum = 2,         // 一次性抽到指定数量品阶卡的次数    ---[品阶, 数量] 任务目标里填次数
        ECET_LuckyHammerFinalAward = 3,         // 幸运魔盒抽中大奖
        ECET_LuckyTurnTableFinalAward = 4,      // 幸运转盘抽中大奖
        ECET_XunBaoFinalAward = 5,              // 寻宝乐园抽中大奖
        ECET_CardCommoditySameCardNum = 6,      // 一次性抽到指定数量相同伙伴的次数      ---[数量] 任务目标填次数
        ECET_MultiColorSpecialAward = 7,        // 缤纷彩球特等奖
        ECET_RoleLevelUp = 8,                   // 玩家等级提升       ---[等级]
        ECET_RoleCombatUp = 9,                  // 玩家战力提升
        ECET_EquipGradeNum = 10,                // 获得指定品阶(星数)装备的数量      ---[品阶]

        ECET_PvELevelFirstFight = 11,           // 首通指定副本       ---[副本ID] 如果有多个副本ID，则填写"列表参数"
        ECET_Tower = 12,                        // 爬塔层数
        ECET_GoldEggTimes = 13,                 // 砸金蛋次数
        ECET_XunBaoTimes = 14,                  // 寻宝次数
        ECET_LuckyTurnTableTimes = 15,          // 幸运转盘次数
        ECET_LuckyHammerTimes = 16,             // 幸运魔盒次数
        ECET_ConstelGradeNum = 17,              // 获得指定品阶(星数)礼装的数量      ---[品阶]
        ECET_CardCommodityNum = 18,             // 抽卡数量
        ECET_Empty = 19,                        // 空事件，直接达成
        ECET_ChapterAchieveCompleteNum = 20,    // 指定章节里完成所有副本成就的副本的数量     ---[章节ID] 目标里填副本数量

        ECET_UninterruptedCardCommodityNoGrade = 21,    // 连续任意次数抽卡未获得指定品阶伙伴    ---[品阶]  目标里填次数
        ECET_FightPvPTimes = 22,                // 竞技场参与次数
        ECET_FightPvPWinTimes = 23,             // 竞技场胜利次数
        ECET_Login = 24,                        // 登录
        ECET_LevelTypeTimes = 25,               // 通关指定类型副本次数  ---[副本类型]
        ECET_UpgradeCardLevel = 26,             // 任意数量伙伴升到指定等级     ---[等级]
        ECET_HomeExploreTimes = 27,             // 家园探索次数
        ECET_HomeVisitTimes = 28,               // 家园拜访次数       // TODO
        ECET_UpgradeCardTimes = 29,             // 升级伙伴消耗材料次数
        ECET_BuyEnergyTimes = 30,               // 购买体力次数

        ECET_QuestCondIndexTimes = 31,          // 完成指定编组任务的次数  ---[任务编组index] 如果有多个index，则填写"列表参数"
        ECET_UpgradeCardColor = 32,             // 任意数量伙伴升到指定阶  ---[阶数]
        ECET_HomeExploreCompleteTimes = 33,     // 家园探索完成次数
        ECET_FishingSuccessTimes = 34,          // 钓鱼成功次数
        ECET_CardHeartLevel = 35,               // 任意数量伙伴好感度升到指定阶   ---[好感度等级]
        ECET_ConstelLevelUp = 36,               // 任意数量礼装升到指定等级   ---[礼装等级]
        ECET_DivConstelTimes = 37,              // 分解礼装数量
        ECET_PvPRank = 38,                      // 竞技场排名(降序判断)
        ECET_CardGradeNum = 39,                 // 获得指定品阶伙伴整卡的数量(任意途径获得整卡)      ---[伙伴品阶]
        ECET_RuneCardNum = 40,                  // 获得符卡数量

        ECET_UpgradeCardStar = 41,              // 任意数量伙伴升到指定星数  ---[星数]
        ECET_HomeOrnamentNum = 42,              // 获得家具数量
        ECET_ConstelURSuitNum = 43,             // 获得任意套数6星套装(不重复套装类型)
        ECET_CurGoldNum = 44,                   // 持有金币数量           ---目标里填数量
        ECET_ExploreLevel = 45,                 // 探索等级             ---目标里填探索经验
        ECET_FishingBigFish = 46,               // 钓到不同大鱼的数量        ---目标里填数量
        ECET_FishingKingFish = 47,              // 钓到不同鱼王的数量        ---目标里填数量
        ECET_WipeFightTotalNum = 48,            // 累计擦弹次数            ---目标里填次数
        ECET_WipeFightOnceNum = 49,             // 单场战斗擦弹次数         ---[次数]   目标里填 1
        ECET_JoinGuild = 50,                    // 加入公会

        ECET_RefreshLevelTimes = 51,            // 刷新副指定本次数
        ECET_UseTalentPoint = 52,               // 使用天赋点

        ECET_PresentGuildWish = 53,             // 捐赠指定星数碎片X次
        ECET_UpLevelConstelMainAtr  = 54,       // 升级指定主属性的念写卡到指定等级
        ECET_CreateBanquetUseItem = 55,         // 使用指定品阶食材举办宴会X次
        ECET_JoinFriendBanquet = 56,            // 参加好友宴会X次
        ECET_GuildWish         = 57,            // 社团祈愿X次
        ECET_CreateBanquet     = 58,            // 举办宴会X次

        ECET_FishingTargetFish = 59,            // 钓到指定鱼X次
        ECET_FishingTotalBigFish = 60,          // 钓到大鱼X次
        ECET_FishingTotalKingFish = 61,         // 钓到鱼王X次
        ECET_FightPvPContinueWinTimes = 62,     // 竞技场连续胜利X场
        ECET_FightPvpNotJoinType = 63,          // 不上阵某种类型伙伴取得胜利X场
        ECET_FightPvpLeftAliveWin = 64,         // 剩余X伙伴获得胜利X场
        ECET_FightPvpAllSameType = 65,          // 只上阵某种类型获得胜利X场
        ECET_FightPvpWinInSecond = 66,          // X秒内获得胜利
        ECET_FightPvpNotWinInSecond = 67,       // X秒内不分胜负
        ECET_FightPvpLeftAliveLose = 68,        // 对方剩余X伙伴，我方失败,X场
        ECET_UserBindEmail          = 69,       // 绑定邮箱
        ECET_GetHomeLevelItem       = 70,       // 获取家园指定的增加经验物品
        ECET_GetHomeLevelFish       = 71,       // 获取家园指定的增加经验的鱼
        ECET_ConsumeGoldCoin        = 72,       // 累计消耗金币
        ECET_ConsumeDiamond         = 73,       // 累计消耗钻石
        ECET_TotalMonster           = 74,       // 累计消灭怪物
        ECET_HarvestCount           = 75,       // 累计收获作物
        ECET_HotSpringCount         = 76,       // 温泉体力回复次数
        ECET_PutOnConstelSuit       = 77,       // 佩戴任意套数任意星级的套装(不重复套装类型)
        ECET_GetHomeBuffItem        = 78,       // 获取家园指定的添加buff的道具
        ECET_GetTargetHeroCard      = 79,       // 获得指定的卡
        ECET_UpStarsHeroCard        = 80,       // 指定卡升到指定星级
        ECET_UpGradeHeroCard        = 81,       // 指定卡升到指定阶
        // TODO

        ECET_Max,
    };
    IS_VALID_ENUM_TYPE(EnumCommonEventType, ECET);

    // 任务类型
    enum EnumQuestType
    {
        EQT_Invalid = 0,
        EQT_DailyQuest = 1,         // 每日任务
        EQT_Achievement = 2,        // 成就任务
        EQT_AvyQuest = 3,           // 活动任务
        EQT_NEWPass_DAILY = 4,      // 通行证每日任务
        EQT_NewPass_Weekly = 5,     // 通行证每周任务
        EQT_NewPass_Monthly = 6,    // 通行证每月任务
        EQT_NewPvp_Weekly = 7,      // pvp每周随机任务

        EQT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumQuestType, EQT);

    // 通用任务状态
    enum EnumCommonQuestState
    {
        ECQS_Invalid = -1,
        ECQS_Accepted = 0,      // 已接取任务
        ECQS_Reached = 1,       // 目标已达成
        ECQS_Received = 2,      // 已领取奖励

        ECQS_Max,
    };
    IS_VALID_ENUM_TYPE(EnumCommonQuestState, ECQS);

    // 问卷调查题目类型
    enum EnumQuestionnaireProblemType
    {
        EQPT_Invalid = 0,
        EQPT_SingleChoice = 1,      // 单选
        EQPT_MultipleChoice = 2,    // 多选
        EQPT_Text = 3,              // 文本
        EQPT_BirthYear = 4,         // 出生年份

        EQPT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumQuestionnaireProblemType, EQPT);

    // 问卷调查开启类型
    enum EnumQuestionnaireOpenType
    {
        EQOT_Invalid = 0,
        EQOT_Level = 1,             // 等级
        EQOT_VIP = 2,               // VIP
        EQOT_Power = 3,             // 战力
        EQOT_ServerOpenDay = 4,     // 开服天数
        EQOT_CreateDay = 5,         // 创角天数
        EQOT_Date = 6,              // 绝对时间

        EQOT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumQuestionnaireOpenType, EQOT);

    // 月球远征关卡类型
    enum EnumCrossFightLevelType
    {
        ECFLT_Invalid = 0,
        ECFLT_Box = 1,              // 宝箱关
        ECFLT_PVP = 2,              // PVP关

        ECFLT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumCrossFightLevelType, ECFLT);

    // 星纹 装备位，套装 属性随机方式
    enum EnumConstelESRandomType
    {
        ECESRT_Invalid = 0,
        ECESRT_Given = 1,          // 指定
        ECESRT_Random = 2,         // 随机
        ECESRT_AutoSelect = 3,     // 自选

        ECESRT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumConstelESRandomType, ECESRT);

    // 伙伴属性类型(以 Pct 结尾的都是以一万(10000)为分母的分子值)
    enum EnumCardAttrType
    {
        ECAT_Invalid = 0,

        ECAT_Attack = 1,	        // 攻击
        ECAT_AttackPct = 2,         // 攻击百分比
        ECAT_Defense = 3,           // 防御
        ECAT_DefensePct = 4,        // 防御百分比
        ECAT_HP = 5,	            // 血量
        ECAT_HPPct = 6,	            // 血量百分比
        ECAT_Crit = 7,	            // 暴击
        ECAT_CritPct = 8,	        // 暴击百分比
        ECAT_CritDamagePct = 9,     // 暴击伤害百分比

        ECAT_AttackSpeedPct = 10,   // 攻速百分比
        ECAT_ED = 11,               // 增伤
        ECAT_EDPct = 12,            // 增伤百分比
        ECAT_RD = 13,               // 减伤
        ECAT_RDPct = 14,            // 减伤百分比
        ECAT_EffectHitPct = 15,     // 特殊效果命中百分比
        ECAT_EffectMissPct = 16,    // 特效效果Miss百分比
        ECAT_PenetrateDamagePct = 17,   // 穿透伤害加成百分比
        ECAT_MissPct = 18,          // 闪避百分比
        ECAT_SkillCDPct = 19,       // 技能冷却缩减百分比
        ECAT_ArmorPenetratePct = 20,    // 护甲穿透百分比
        ECAT_EnergyRecoveryPct = 21,    // 能量恢复百分比

        ECAT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumCardAttrType, ECAT);

    // 伙伴属性值是否有效
    static bool IsValidCardAttrValue(int iValue) { return 0 < iValue && iValue < 99999; }

    // 星纹套装效果类型
    enum EnumConstelSuitEffectType
    {
        ECSET_Invalid = 0,
        ECSET_NormalEffect = 1,      // 普通属性类型
        ECSET_SkillEffect = 2,       // 特殊技能效果

        ECSET_Max,
    };
    IS_VALID_ENUM_TYPE(EnumConstelSuitEffectType, ECSET);

    // 技能类型
    enum EnumSkillType
    {
        EST_Invalid = 0,
        EST_PassiveSkill = 1,   // 被动技
        EST_AutoSkill = 2,      // 自动技
        EST_UniqueSkill = 3,    // 必杀技

        EST_Max,
    };
    IS_VALID_ENUM_TYPE(EnumSkillType, EST);

    // 排行榜刷新类型
    enum EnumRankRefreshType
    {
        ERRT_Invalid = 0,
        ERRT_Forever = 1,       // 不更新
        ERRT_Week = 2,          // 每周刷新

        ERRT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumRankRefreshType, ERRT);

    // 家园房间类型
    enum EnumHomeType
    {
        EHT_Invalid = 0,
        EHT_LivingRoom = 1,         // 客厅
        EHT_Farmland = 2,           // 花田

        EHT_Bedroom = 4,            // 卧室
        EHT_Kitchen = 5,            // 厨房
        EHT_TeaHouse = 6,           // 茶馆
        EHT_HotSpring = 7,          // 温泉

        EHT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumHomeType, EHT);

    // 功能家具类型
    enum EnumHomeFuncGoodsType
    {
        EHFGT_Invalid = 0,
        EHFGT_Bed = 1,              // 床
        EHFGT_Hearth = 2,           // 厨台
        EHFGT_TeaSet = 3,           // 茶具
        EHFGT_DiningTable = 4,      // 餐桌

        EHFGT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumHomeFuncGoodsType, EHFGT);

    // 徽章成就条件类型
    enum EnumBadgeCondType
    {
        EBCT_Invalid = 0,
        EBCT_LevelFight = 1,        // 通关指定副本	 	---多个副本用 | 分隔("101|102")表示通关任意一个都完成
        EBCT_LikeFriend = 2,        // 点赞玩家
        EBCT_BeLike = 3,            // 被点赞
        EBCT_CollectFish = 4,       // 收集齐鱼
        EBCT_CookBookTimes = 5,     // 制作食谱次数
        EBCT_CompletSpecExplore = 6,    // 完成特殊探索
        EBCT_CollectRuneCard = 7,   // 收集灵符
        EBCT_CollectSpecRuneCard = 8,   // 解锁隐藏灵符
        EBCT_CollectGold = 9,       // 持有金币累计

        EBCT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumBadgeCondType, EBCT);

    // 副本成就
    enum EnumLevelAchieveType
    {
        ELAT_Invalid = 0,
        ELAT_Miss = 1,          // 未被击中
        ELAT_AllGold = 2,       // 获得全部金币
        ELAT_KillAll = 3,       // 杀死所有敌人

        ELAT_Max,
    };
};

enum class CEnumBuyBackType
{
	DINNER = 1,			//豪华大餐
	HORSE = 2,			//跑马夺金
	DIAOCHAN = 3,		//貂蝉献酒
	PVP_TERRITORY = 4,	//步步为营
	SINGLE_LEVEL = 5,	//单刀赴会
};

enum class CEnumActivityTimeType
{
	PVP = 1,

	FAIR_PVP = 3,
	ATKDEF_FIGHT = 4,
	ATKDEF_AWARD = 5,
	JOB_LEVEL = 6
};

enum class CESysOtherBuyType
{
  CESOBT_DINNER         = 1,      // 豪华大餐补吃
};

enum class EnumGuildDonateType
{
    GuildDonateTypeInvalid  = 0,
    GuildDonateTypeGold     = 1,  //金币捐献
    GuildDonateTypeDiamond  = 2,  //钻石捐献
    GuildDonateMax
};

enum class EnumGuildRandomType
{
    GuildRandomTypeInvalid = 0,
    GuildRandomTypeChat = 1,                    //聊天
    GuildRandomTypeExchange = 2,                //兑换
    GuildRandomTypePk = 3,                      //切磋
    GuildRandomTypeMax,
};

enum EnumRuneUnLockType
{
    RuneUnLockTypeInvalid = 0,
    RuneUnLockTypeLeveUp = 1,           //指定角色升级
    RuneUnLockTypeUpgrade = 2,          //指定角色升阶
    RuneUnLockTypeUseSkill = 3,         //指定角色使用奥义技能
    RuneUnLockTypeStarUp = 4,           //指定角色升星
    RuneUnLockTypePassLevel = 5,        //指定角色通关指定关卡
};

enum EnumZhuHaiStore
{
    ZhuHaiBuffInvalid = 0,
    ZhuHaiBuffBuff = 1,             //buff
    ZhuHaiBuffBless = 2,            //祝福
    ZhuHaiBuffRoleBuff = 3,         //个人buff
};

enum EnumGuildBuffType
{
    GuildBuffInvalid = 0,
    GuildBuffFishing = 1,           //钓鱼获取5星鱼成功率提升
    GuildBuffGuildBoss = 2,         //攻击社团Boss，伤害提高
    GuildBuffGoldAdd   = 3,         //所有关卡金币奖励增加
    GuildBuffGuildDonate = 4,       //社团捐献时贡献提高
    GuildBuffHotSpring  = 5,        //社团温泉增加15点
};

enum EnumCrushBossGradeType
{
    CrushBossGradeInvalid = 0,
    CrushBossGradeThreeStar = 3,    //遭遇Boss品质
    CrushBossGradeFourStar = 4,
    CrushBossGradeFiveStar = 5,
    CrushBossGradeSixStar = 6,
};

enum EnumStoreType
{
    StoreTypeInvalid = 0,
    StoreTypeHomeTopic = 1, //家园主题商店
};


enum EnumGashaponType
{
    GashaponTypeInvalid = 0,
    GashaponTypeAdv     = 1,    //高级卡池
    GashaponTypeNormal  = 2,    //初始卡池
};

enum EnumHomeGoodBuffType
{
    HomeGoodBuff_Invalid = 0,
    HomeGoodBuff_HotSpring = 1,  //温泉获取体力提高
    HomeGoodBuff_Garden  = 2,    //家园种植收获提高
    HomeGoodBuff_Cook    = 3,    //家园烹饪更高概率出高星料理

    HomeGoodBuff_Max,
};

//风土记地区buff
enum EnumCompleteAtlasBuffType
{
    AtlasBuff_Invalid = 0,
    AtlasBuff_Exp = 1,     //美酒副本增幅
    AtlasBuff_Gold = 2,     //金币副本增幅
};


class CLogicConfigParserBase
{
public:
	typedef std::function<const std::string&(const std::string&)> get_xml_content_callback_type;

	CLogicConfigParserBase() : m_stGetXMLContentCallback(GetEmptyXMLContent) { } ;

	virtual ~CLogicConfigParserBase() = default;

	virtual std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) = 0;

	void SetGetXMLContentCallback(get_xml_content_callback_type stCallback) { m_stGetXMLContentCallback = stCallback; };

private:
	static const std::string& GetEmptyXMLContent(const std::string& strName)
	{
		(void)strName;
		static std::string strEmptyContent;
		return (strEmptyContent);
	}

protected:
	get_xml_content_callback_type   m_stGetXMLContentCallback;
};




