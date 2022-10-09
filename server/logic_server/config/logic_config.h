#pragma once

#include "logic_config_active.h"
#include "logic_config_award.h"
#include "logic_config_card.h"
#include "logic_config_cross.h"
#include "logic_config_entire_war.h"
#include "logic_config_fight.h"
#include "logic_config_game.h"
#include "logic_config_game_item.h"
#include "logic_config_grow.h"
#include "logic_config_guild.h"
#include "logic_config_home.h"
#include "logic_config_ibusy.h"
#include "logic_config_qiyu.h"
#include "logic_conifg_score_level.h"
#include "logic_config_store.h"
#include "logic_config_xuyuanqingdian.h"
#include "logic_config_theme_activity_charge.h"
#include "logic_config_protocol_monitor.h"
#include "logic_config_item_monitor.h"
#include "logic_config_proto_switch.h"
#include "logic_config_special_gift.h"
#include "logic_config_anti_addiction.h"
#include "logic_config_holiday_calendar.h"
#include "logic_config_card_dialog.h"
#include "logic_config_wenwen_news.h"
#include "logic_config_random_store.h"
#include "logic_config_constellation.h"
#include "logic_config_new_pass.h"
#include "logic_config_limit_boss.h"
#include "logic_config_rune.h"
#include "logic_config_double_reward.h"
#include "logic_config_register_reward.h"
#include "logic_config_gashapon.h"
#include "logic_config_atlas.h"

///////////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_CONFIG_PARSER(class_name, func_name, member_name) \
public: \
    static const class_name& func_name() { return (m_pConfig->member_name); } \
    static CLogicConfigParserBase* func_name##Parser(CLogicConfig* pConfig) { return (pConfig->func_name##Parser()); } \
    class_name* func_name##Parser() { return (&(member_name)); } \
private: \
    class_name member_name; \

///////////////////////////////////////////////////////////////////////////////////////////////
class CLogicConfig
{
public:
    CLogicConfig() : m_stMainTaskConfig(m_stMutiStepTaskConfig) {}

    typedef CLogicConfigParserBase* (*rk_logic_get_parser_func_t)(CLogicConfig* pConfig);

    struct config_loader_t
    {
        const char*                     m_pFileName;
        rk_logic_get_parser_func_t      m_pParserGetFunc;
    };

    static CLogicConfig* GetConfig() { return m_pConfig; }
    static void SetConfig(CLogicConfig* pConfig) { m_pConfig = pConfig; }
    static const MapLogicMutiStepTaskConfig& GetMutiStepTaskConfig() { return (m_pConfig->m_stMutiStepTaskConfig); }
    
    static config_loader_t* GetConfigLoader() { return (CONFIG_LOADER_LIST); }
private:
    MapLogicMutiStepTaskConfig  m_stMutiStepTaskConfig;

    DECLARE_CONFIG_PARSER(CLogicConfigSystemParser, GetSystemConfig, m_stSystemConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigZooLogicPlatformParser, GetZooLogicPlatformConfig, m_stZooLogicPlatformConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigZooLogicGameParser, GetZooLogicGameConfig, m_stZooLogicGameConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigGameGlobalParser, GetGlobalConfig, m_stGlobalConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHeroCardParser, GetHeroCardConfig, m_stHeroCardConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigLevelDropParser, GetLevelDropConfig, m_stLevelDropConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigLevelDropBagParser, GetLevelDropBagConfig, m_stLevelDropBagConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigChapterParser, GetChapterConfig, m_stChapterConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigLevelParser, GetLevelConfig, m_stLevelConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigUserExpParser, GetUserExpConfig, m_stUserExpConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigCalcParser, GetCalcConfig, m_stCalcConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigBossParser, GetBossConfig, m_stBossConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigUpgradeHeroCardStarParser, GetUpgradeHeroCardStarConfig, m_stUpgradeHeroCardStarConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigUpgradeHeroCardLevelParser, GetUpgradeHeroCardLevelConfig, m_stUpgradeHeroCardLevelConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigUpgradeHeroCardColorParser, GetUpgradeHeroCardColorConfig, m_stUpgradeHeroCardColorConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigMainTaskParser, GetMainTaskConfig, m_stMainTaskConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigMallParser, GetMallConfig, m_stMallConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigCardbagParser, GetCardbagConfig, m_stCardbagConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigNoCoinMonsterParser, GetNoCoinMonsterConfig, m_stNoCoinMonsterConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigRankListBonusParser, GetRankListBonusConfig, m_stRankListBonusConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigDirtyWordParser, GetDirtyWordConfig, m_stDirtyWordConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigDirtyWordChatParser, GetDirtyWordChatConfig, m_stDirtyWordChatConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigPvpRobotParser, GetPVPRobotConfig, m_stPVPRobotConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigPvpParser, GetPVPConfig, m_stPVPConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigPvpMatchRuleParser, GetPVPMatchRuleConfig, m_stPVPMatchRuleConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigFairPvpParser, GetFairPvpConfig, m_stFairPvpConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigMatchPvpParser, GetMatchPvpConfig, m_stMatchPvpConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigVipParser, GetVipConfig, m_stVipConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigBuyResourceParser, GetBuyResourceConfig, m_stBuyResourceConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigDepositParser, GetDepositConfig, m_stDepositConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigAvatarParser, GetAvatarConfig, m_stAvatarConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigActivityParser, GetActivityConfig, m_stActivityConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigActivitySerialPayParser, GetActivitySerialPayConfig, m_stActivitySerialPayConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigVipUnlockParser, GetVipUnlockConfig, m_stVipUnlockConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigMallBagParser, GetMallBagConfig, m_stMallBagConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigBroadcastParser, GetBroadcastConfig, m_stBroadcastConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigActivityBroadcastParser, GetActivityBroadcastConfig, m_stActivityBroadcastConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigFightReportParser, GetFightReportConfig, m_stFightReportConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigFirstDepositParser, GetFirstDepositConfig, m_stFirstDepositConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigDailyDepositParser, GetDailyDepositConfig, m_stDailyDepositConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHighestRecordParser, GetHighestRecordConfig, m_stHighestRecordConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigVipDailyAwardParser, GetVipDailyAwardConfig, m_stVipDailyAwardConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigVipBagParser, GetVipBagConfig, m_stVipBagConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigFragmentCombineParser, GetFragmentCombineConfig, m_stFragmentCombineConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigCrossFightParser, GetCrossFightConfig, m_stCrossFightConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigFightPowerRankActiveParser, GetFightPowerRankActiveConfig, m_stFightPowerRankActiveConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigItemParser, GetItemConfig, m_stItemConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigCurrencyParser, GetCurrencyConfig, m_stCurrencyConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigClimbTowerParser, GetClimbTowerConfig, m_stClimbTowerConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigGuildParser, GetGuildConfig, m_stGuildConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigGuildWarParser, GetGuildWarConfig, m_stGuildWarConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigMailParser, GetMailConfig, m_stMailConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigShopParser, GetShopConfig, m_stShopConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigBankPriceParser, GetBankPriceConfig, m_stBankPriceConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigAwardParser, GetAwardConfig, m_stAwardConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigActivityMultiTaskParser, GetActivityMultiTaskConfig, m_stActivityMultiTaskConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigFashionParser, GetFashionConfig, m_stFashionConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigHonorParser, GetHonorConfig, m_stHonorConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigActivityTaskParser, GetActivityTaskConfig, m_stActivityTaskConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigMergeActivityParser, GetMergeActivityConfig, m_stMergeActivityConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigEntireWarParser, GetEntireWarConfig, m_stEntireWarConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigQuestionParser, GetQuestionConfig, m_stQuestionConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigFriendParser, GetFriendConfig, m_stFriendConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigJubaoParser, GetJubaoConfig, m_stJubaoConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigQiyuParser, GetQiyuConfig, m_stQiyuConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigQiyuBotParser, GetQiyuBotConfig, m_stQiyuBotConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigThemeActivityChargeParser, GetThemeActivityChargeConfig, m_stThemeActivityChargeConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigIbusyParser, GetIBusyConfig, m_stIBusyConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigScoreLevelParser, GetScoreLevelConfig, m_stScoreLevelConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigDaySingleRechargeActivityParser, GetDaySingleRechargeConfig, m_stDaySingleRechargeConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigProtocolMonitorParser, GetProtocolMonitorConfig, m_stProtocolMonitorConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigItemMonitorParser, GetItemMonitorConfig, m_stItemMonitorConfig)
	DECLARE_CONFIG_PARSER(CLogicConfigProtoSwitchParser, GetProtoSwitchConfig, m_stProtoSwitchConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigSpecialGiftParser, GetSpecialGiftConfig, m_stSpecialGiftConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigMonthPassParser, GetMonthPassConfig, m_stMonthPassConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigAntiAddictionParser, GetAntiAddictionConfig, m_stAntiAddictionConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHolidayCalendarParser, GetHolidayCalendarConfig, m_stHolidayCalendarConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHeroHeartParser, GetHeroHeartConfig, m_stHeroHeartConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigCardDialogParser, GetCardDialogConfig, m_stCardDialogConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigWenWenNewsParser, GetWenWenNewsConfig, m_stWenWenNewsConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigMoneyTreeExpParser, GetMoneyTreeExpConfig, m_stMoneyTreeExpConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigBuffParser, GetBuffConfig, m_stBuffConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigRandomStoreParser, GetRandomStoreConfig, m_stRandomStoreConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHeartStoreDiscountParser, GetHeartStoreDiscountConfig, m_stHeartStoreDiscountConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHeroDatingParser, GetHeroDatingConfig, m_stHeroDatingConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHeroDatingDialogParser, GetHeroDatingDialogConfig, m_stHeroDatingDialogConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigSimpleGameParser, GetSimpleGameConfig, m_stSimpleGameConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigRedBagParser, GetRedBagConfig, m_stRedBagConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigWorldQuestParser, GetWorldQuestConfig, m_stWorldQuestConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigQuestionnaireParser, GetQuestionnaireConfig, m_stQuestionnaireConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigConstelAttrParser, GetConstelAttrConfig, m_stConstelAttrConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigConstelParser, GetConstelConfig, m_stConstelConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigRankParser, GetRankConfig, m_stRankConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigEntireWarMapParser, GetEntireWarMapConfig, m_stEntireWarMapConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigRewardBagParser, GetRewardBagConfig, m_stRewardBagConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigEntireWarLimitBossParser, GetEntireWarLimitBossConfig, m_stEntireWarLimitBoss)
    DECLARE_CONFIG_PARSER(CLogicConfigZooQueueParser, GetZooQueueConfig, m_stZooQueueConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigEquipAttrParser, GetEquipAttrConfig, m_stEquipAttrConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigEquipAttrRandParser, GetEquipAttrRandConfig, m_stEquipAttrRandConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigEquipItemParser, GetEquipItemConfig, m_stEquipItemConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigConstelLevelDropParser, GetConstelLevelDropConfig, m_stConstelLevelDropConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHomeGoodsParser, GetHomeGoodsConfig, m_stHomeGoodsConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHomeFuncParser, GetHomeFuncConfig, m_stHomeFuncConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHomeGardenExploreParser, GetHomeGardenExploreConfig, m_stHomeGardenExploreConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigQuestParser, GetQuestConfig, m_stQuestConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigGiftBagParser, GetGiftBagConfig, m_stGiftBagConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigAvyTimeParser, GetAvyTimeConfig, m_stAvyTimeConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigAvyScoreAwardParser, GetAvyScoreAwardConfig, m_stAvyScoreAwardConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigZhuHaiParser, GetZhuHaiConfig, m_stZhuHaiConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigSignParser, GetSignConfig, m_stSignConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigBadgeParser, GetBadgeConfig, m_stBadgeConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigNewPassParser, GetNewPassConfig, m_stNewPassConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigLimitBossParser, GetLimitBossConfig, m_stLimitBossConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigHomeLevelExpParser, GetHomeLevelExpConfig, m_stHomeLevelExpConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigRuneParser, GetRuneConfig, m_stRuneConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigDoubleRewardParser, GetDoubleRewardConfig, m_stDoubleRewardConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigRegisterRewardParser, GetRegisterRewardConfig, m_stRegisterRewardConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigGashaponParser, GetGashaponConfig, m_stGashaponConfig)
    DECLARE_CONFIG_PARSER(CLogicConfigCompleteAtlasParser, GetCompleteAtlasConfig, m_stCompleteAtlasConfig)


public:
    static void ParseWeekDay(const std::string& strWeekDay, char& refWeekDayArray);
    static void ParseDay(const std::string& strOpenDay, int& iStartDayTimestamp, int& iEndDayTimestamp);
    static void ParseHour(const std::string& strHour, int& iStartHour, int& iStartMinute, int& iStartSecond,
                                                      int& iEndHour, int& iEndMinute, int& iEndSecond);

    static int GetChapterTypeByLevelID(int iLevelID);
    static int GetChapterTypeByChapterID(int iChapterID);
    static int GetChapterIDByLevelID(int iLevelID);

	static std::string GetItemName(int32_t iItemType, int32_t iItemID);
    static std::string GenRewardContent(const vector<CPackGameItem> &items);
    static bool IsValidGameItem(int32_t iType, int32_t iItemID, int32_t iNum);
    static bool IsValidGameItem(int32_t iType, int32_t iItemID);
    static bool IsValidGameItem(const CPackGameItem& stItem);

    static size_t s_iConfigSize;

private:
    static config_loader_t                              CONFIG_LOADER_LIST[];

    static CLogicConfig*                              m_pConfig;
};
