#pragma once

#include <unordered_map>

#include "logic_config_base.h"
#include "logic_config_define.h"

struct TLogicActiveCommodityElem
{
	struct TMallBag
	{
		int32_t         m_iBuyCount = 0;
		int32_t         m_iBuyCycle = 0;
		std::string     m_strBuyBag;
	};
	int32_t m_iDailyFreeCount{ 0 };
	int32_t m_iDailyLuckyCount{ 0 };
	int32_t m_iLuckyScore{ 0 };
	int32_t m_iCountScore{ 0 };
	int32_t m_iRankScore{ 0 };
	int32_t m_iBuyItemType{ 0 };
	int32_t m_iBuyItemID{ 0 };
	int32_t m_iBuyItemNum{ 0 };
	int32_t m_iBuyItemNum2{ 0 };
	int32_t m_iOpenLevel{ 0 };
	int32_t m_iCondition{ 0 };
	int32_t m_iDiscount{ 0 };
	std::vector<TMallBag> m_astMallBagVec;
	CPackGameItem m_stConsumeItem;
};

struct TLogicActivityTaskElem
{
	TLogicActivityTaskElem() : m_iTaskID(0), m_iType(0), m_iTarget(0), m_iTargetAmount(0), m_iBonusScore(0) {}

	int32_t m_iTaskID;
	int32_t m_iType;
	int32_t m_iTarget;
	int32_t m_iTargetAmount;
	int32_t m_iBonusScore;
	std::vector<CPackGameItem> m_stBonusItemVec;
};

// 特别任务，任务条件为 完成序列任务
struct TLogicActivitySequenceTask
{
    TLogicActivitySequenceTask() : m_iTaskID(0), m_stTargetTaskList(), m_stRewards() {}

    int32_t                     m_iTaskID;
    std::vector<int32_t>        m_stTargetTaskList;
    std::vector<CPackGameItem>  m_stRewards;
};

struct TLogicActivityMultiTaskElem
{
	TLogicActivityMultiTaskElem() : m_iActivityID(0), m_iActivityType(0), m_iTimeType(0), m_iStartDay(0), m_iEndDay(0)
		, m_iServerDay(0), m_iStartTime(0), m_iEndTime(0), m_stTaskMultiMap(), m_stSequenceTaskMap() {}

	int32_t m_iActivityID;
	int32_t m_iActivityType;
	int32_t m_iTimeType;
	int32_t m_iStartDay;
	int32_t m_iEndDay;
	int32_t m_iServerDay;
	int32_t m_iStartTime;
	int32_t m_iEndTime;
	std::multimap<int32_t, TLogicActivityTaskElem> m_stTaskMultiMap;		// type => task
    std::map<int32_t, TLogicActivitySequenceTask>  m_stSequenceTaskMap;     // task_id => task
};

class CLogicConfigActivityMultiTaskParser : public CLogicConfigParserBase
{
public:
	CLogicConfigActivityMultiTaskParser() = default;

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::map<int32_t, TLogicActivityMultiTaskElem> m_stMultiTaskMap;
};

class CLogicConfigActivityTaskParser : public CLogicConfigParserBase
{
public:
	CLogicConfigActivityTaskParser() : m_stActiveTaskMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    std::multimap<int32_t, TLogicActivityMultiTaskElem> m_stActiveTaskMap;  //type=>elem
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicActiveDinnerElem
{
	TLogicActiveDinnerElem() : m_iStartTime(0), m_iEndTime(0), m_iEnergyAmount(0) {}

	int             m_iStartTime;
	int             m_iEndTime;
	int             m_iEnergyAmount;
};

struct TLogicActiveNewServerElem
{
	TLogicActiveNewServerElem() : m_iSignDay(1), m_stBonusVec() {}

	int                                             m_iSignDay;
	std::vector<CPackGameItem>  m_stBonusVec;
};

struct TLogicActiveDepositElem
{
	TLogicActiveDepositElem()
		: m_iAddUpRmb(0), m_stBonusVec() {}

	int             m_iAddUpRmb;

	std::vector<CPackGameItem>  m_stBonusVec;
};

struct TLogicActiveCornucopiaElem
{
	TLogicActiveCornucopiaElem()
		: m_iNeedYuanBao(0), m_iVipLevel(0), m_iRealRebateMin(0), m_iRealRebateMax(0) {}

	int             m_iNeedYuanBao;
	int             m_iVipLevel;
	int             m_iRealRebateMin;
	int             m_iRealRebateMax;
};

struct TLogicActiveFoundationElem
{
	TLogicActiveFoundationElem() : m_stUserLevel(), m_stRewards() {}

	std::vector<int>            m_stUserLevel;
	std::vector<CPackGameItem>  m_stRewards;
};

struct TLogicActiveLevelRaceAwardElem
{
	TLogicActiveLevelRaceAwardElem() : m_iUserLevel(0), m_stQuarterReward(), m_stBonusItemVec() {}

	int                                                 m_iUserLevel;
    CPackGameItem                   m_stQuarterReward;
	std::vector<CPackGameItem>      m_stBonusItemVec;
};

struct TLogicActiveLevelRaceElem
{
	TLogicActiveLevelRaceElem() : m_iExtraAwardLevel(0), m_iExtraAwardMemberLimit(0) {}

	int                                                     m_iExtraAwardLevel;
	int                                                     m_iExtraAwardMemberLimit;
	CPackGameItem                       m_stExtraAwardItem;
	std::vector<TLogicActiveLevelRaceAwardElem>    m_stAwardVec;
};

struct TLogicHeroFlashSaleElem
{
	TLogicHeroFlashSaleElem() : m_iLimitLevelID(0) {}

	int                                 m_iLimitLevelID;
	CPackGameItem   m_stAwardItem;

	CPackGameItem   m_stLimitConsumeItem;
	CPackGameItem   m_stConsumeItem;
};

struct TLogicActiveOpenServerLoginElem
{
	TLogicActiveOpenServerLoginElem() {}

	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveOpenServerBuyElem
{
	TLogicActiveOpenServerBuyElem() {}

	std::vector<CPackGameItem> m_stBonusItemVec;
	std::vector<CPackGameItem> m_stConsumeItemVec;
};

struct TLogicActiveNormalLevelElem
{
	TLogicActiveNormalLevelElem() : m_iLevelID(0) {}

	int                                             m_iLevelID;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveEliteLevelElem
{
	TLogicActiveEliteLevelElem() : m_iLevelID(0) {}

	int                                             m_iLevelID;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActivePvpRankElem
{
	TLogicActivePvpRankElem() : m_iRank(0) {}

	int                                             m_iRank;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveUnlimitLevelScoreLevelElem
{
	TLogicActiveUnlimitLevelScoreLevelElem() : m_iScore(0) {}

	int                                             m_iScore;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActivePassLevelStarElem
{
	TLogicActivePassLevelStarElem() : m_iStar(0) {}

	int                                             m_iStar;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActivePassLevelTimesElem
{
	TLogicActivePassLevelTimesElem() : m_iTimes(0) {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveHeroEquipmentUpgradeLevelElem
{
	TLogicActiveHeroEquipmentUpgradeLevelElem() : m_iHeroNum(0), m_iEquipmentLevel(0) {}

	int                                             m_iHeroNum;
	int                                             m_iEquipmentLevel;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};


struct TLogicActiveHeroEquipmentUpgradeJewelElem
{
	TLogicActiveHeroEquipmentUpgradeJewelElem() : m_iJewelNum(0), m_iJewelLevel(0) {}

	int                                             m_iJewelNum;
	int                                             m_iJewelLevel;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveHeroEquipmentUpgradeStarElem
{
	TLogicActiveHeroEquipmentUpgradeStarElem() : m_iEquipmentNum(0), m_iEquipmentStar(0) {}

	int                                             m_iEquipmentNum;
	int                                             m_iEquipmentStar;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveHeroUpgradeColorElem
{
	TLogicActiveHeroUpgradeColorElem() : m_iHeroNum(0), m_iColor(0) {}

	int                                             m_iHeroNum;
	int                                             m_iColor;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveHeroUpgradeLevelElem
{
	TLogicActiveHeroUpgradeLevelElem() : m_iHeroNum(0), m_iLevel(0) {}

	int                                             m_iHeroNum;
	int                                             m_iLevel;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveHeroUpgradeStarElem
{
	TLogicActiveHeroUpgradeStarElem() : m_iHeroNum(0), m_iStar(0) {}

	int                                             m_iHeroNum;
	int                                             m_iStar;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveWushenLevelElem
{
	TLogicActiveWushenLevelElem() : m_iLevel(0) {}

	int                                             m_iLevel;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveTeamLevelUpgradeElem
{
	TLogicActiveTeamLevelUpgradeElem() : m_iLevel(0) {}

	int                                             m_iLevel;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveUpgradeSkillElem
{
	TLogicActiveUpgradeSkillElem() : m_iTimes(0) {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveFightPowerElem
{
	TLogicActiveFightPowerElem() : m_iFightPower(0) {}

	int                                             m_iFightPower;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveHeroAchieveElem
{
	TLogicActiveHeroAchieveElem() : m_iCardID(0), m_stBonusItem() {}

	int                                 m_iCardID;
	CPackGameItem   m_stBonusItem;
};

struct TLogicActiveBenefitStoreElem
{
	TLogicActiveBenefitStoreElem() : m_stBonusItem(), m_stConsumeItem() {}

	CPackGameItem   m_stBonusItem;
	CPackGameItem   m_stConsumeItem;
};

struct TLogicActiveExchangeStoreElem
{
	TLogicActiveExchangeStoreElem() : m_stBonusItem(), m_stConsumeItemVec() {}

	CPackGameItem               m_stBonusItem;
	std::vector<CPackGameItem>  m_stConsumeItemVec;
};

struct TLogicActiveSingleDepositElem
{
	TLogicActiveSingleDepositElem() : m_iRmb(0), m_stBonusItemVec() {}

	int                                             m_iRmb;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveDailyCumulateDepositElem
{
	TLogicActiveDailyCumulateDepositElem() : m_iRmb(0), m_stBonusItemVec() {}

	int                                             m_iRmb;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveDeadlineLoginElem
{
	TLogicActiveDeadlineLoginElem() : m_iSignDay(0), m_stBonusItemVec() {}

	int                                             m_iSignDay;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveBuyCommodityElem
{
	TLogicActiveBuyCommodityElem() : m_iTimes(0), m_stBonusItemVec() {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveFightThemeActiveLevelElem
{
	TLogicActiveFightThemeActiveLevelElem() : m_iTimes(0), m_stBonusItemVec() {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActivePvpFightTimesElem
{
	TLogicActivePvpFightTimesElem() : m_iTimes(0), m_stBonusItemVec() {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveEliteLevelFightTimesElem
{
	TLogicActiveEliteLevelFightTimesElem() : m_iTimes(0), m_stBonusItemVec() {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveNormalLevelFightTimesElem
{
	TLogicActiveNormalLevelFightTimesElem() : m_iTimes(0), m_stBonusItemVec() {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveBuyEnergyTimesElem
{
	TLogicActiveBuyEnergyTimesElem() : m_iTimes(0), m_stBonusItemVec() {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveBuyGoldTimesElem
{
	TLogicActiveBuyGoldTimesElem() : m_iTimes(0), m_stBonusItemVec() {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveBuyBlackStoreTimesElem
{
	TLogicActiveBuyBlackStoreTimesElem() : m_iTimes(0), m_stBonusItemVec() {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveCrossFightPassLevelTimesElem
{
	TLogicActiveCrossFightPassLevelTimesElem() : m_iTimes(0), m_stBonusItemVec() {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveUnlimitLevelFightTimesElem
{
	TLogicActiveUnlimitLevelFightTimesElem() : m_iTimes(0), m_stBonusItemVec() {}

	int                                             m_iTimes;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveCalculateCountElem
{
	TLogicActiveCalculateCountElem() : m_iCount(), m_strBuyMallBag(""), m_stBonusItemVec(), m_stNotify(), m_bIsBigPrize(false) {}

	int32_t             m_iCount;
	std::string         m_strBuyMallBag;
	std::vector<CPackGameItem> m_stBonusItemVec;
	std::vector<int>    m_stNotify;
    bool                m_bIsBigPrize; // 是否包含大奖
};

// 好感度基金
struct TLogicActiveHeartInsureElem
{
    TLogicActiveHeartInsureElem() : m_iHeartLevel(0), m_iCount(0), m_stBonusItemVec() {}

    int32_t m_iHeartLevel;
    int32_t m_iCount;
    std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveSingleItemExchangeElem
{
	TLogicActiveSingleItemExchangeElem() : m_iTimes(0), m_iVipLevel(0), m_stBonusItem(), m_stConsumeItemVec() {}

	int32_t m_iTimes;
	int32_t m_iVipLevel;
	CPackGameItem               m_stBonusItem;
	std::vector<CPackGameItem>  m_stConsumeItemVec;
};

struct TLogicActiveUpgradeTacticsElem
{
	TLogicActiveUpgradeTacticsElem() : m_iUpgradeTacticsCount(0), m_stBonusItemVec() {}

	int32_t m_iUpgradeTacticsCount;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveHeroTrainElem
{
	TLogicActiveHeroTrainElem() : m_iHeroTrainCount(0), m_stBonusItemVec() {}

	int32_t m_iHeroTrainCount;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveBreakEggElem
{
	TLogicActiveBreakEggElem() : m_iBreakEggCount(0), m_stBonusItemVec() {}

	int32_t m_iBreakEggCount;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveYuanbaoRecruitElem
{
	TLogicActiveYuanbaoRecruitElem() : m_iYuanbaoRecruitCount(0), m_stBonusItemVec() {}

	int32_t m_iYuanbaoRecruitCount;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveConsumeEnergyElem
{
	TLogicActiveConsumeEnergyElem() : m_iConsumeEnergyValue(0), m_stBonusItemVec() {}

	int32_t m_iConsumeEnergyValue;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveClimbTowerElem
{
	TLogicActiveClimbTowerElem() : m_iClimbTowerOrder(0), m_stBonusItemVec() {}

	int32_t m_iClimbTowerOrder;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActivePvpTerritoryElem
{
	TLogicActivePvpTerritoryElem() : m_iPvpTerritoryOrder(0), m_stBonusItemVec() {}

	int32_t m_iPvpTerritoryOrder;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveTacticsCountElem
{
	TLogicActiveTacticsCountElem() : m_iTacticsCount(0), m_stBonusItemVec() {}

	int32_t m_iTacticsCount;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveGuildDonateElem
{
	TLogicActiveGuildDonateElem() : m_iGuildDonateCount(0), m_stBonusItemVec() {}

	int32_t m_iGuildDonateCount;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicActiveGuildTaskElem
{
	TLogicActiveGuildTaskElem() : m_iGuildTaskCount(0), m_stBonusItemVec() {}

	int32_t m_iGuildTaskCount;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};


struct TLogicActiveRankElem
{
	int32_t m_iIndex;//索引
	int32_t m_iMinRank;//最小排名
	int32_t m_iMaxRank;//最大排名
	int32_t m_iNeedPoint;//积分
	std::string m_strSender;//发件人
	std::string m_strTitle;//标题
	std::string m_strContent;//内容
	std::vector<CPackGameItem>  m_stBonusItemVec;//奖励
};

struct TLogicActiveFlowerLanternInfoElem
{
	int32_t m_iDailyLoginCandle = 0;
	int32_t m_iUnitRecharge = 0;
	int32_t m_iUnitRechargeCandle = 0;
	int32_t m_iCandleReturnDiamond = 0;
	std::string m_strReturnSender;
	std::string m_strReturnTitle;
	CFormatString m_stReturnContent;
};

class CLogicConfigActivityParser : public CLogicConfigParserBase
{
public:
	CLogicConfigActivityParser() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::string GetActiveCommodityBuyMallBag(int32_t iActivityID, int32_t iBuyCount) const;

	std::map<int32_t, TLogicActivityRefElem>         			m_stActivityRef;
	std::map<int32_t, TLogicActiveCommodityElem>						m_stActiveCommodity;
	std::map<int32_t, std::vector<TLogicActiveRankElem>>	 			m_stActiveRankConfig;//排行配置
	std::multimap<int32_t, int32_t> m_stActivityType2IDMap;

	std::map<int32_t, std::vector<TLogicActiveDinnerElem>>              m_stDinnerConfig;
	std::map<int32_t, std::vector<TLogicActiveNewServerElem>>      		m_stNewServerConfig;
	std::map<int32_t, std::vector<TLogicActiveDepositElem>>             m_stDepositConfig;
	std::map<int32_t, std::vector<TLogicActiveCornucopiaElem>>          m_stCornucopiaConfig;
	std::map<int32_t, TLogicActiveFoundationElem>                       m_stFoundationConfig;
	std::map<int32_t, TLogicActiveLevelRaceElem>                        m_stLevelRaceConfig;
	std::map<int32_t, TLogicHeroFlashSaleElem>                          m_stHeroFlashSaleConfig;
	std::map<int32_t, std::vector<TLogicActiveOpenServerLoginElem>>    	m_stOpenServerLoginConfig;
	std::map<int32_t, std::vector<TLogicActiveOpenServerBuyElem>>      	m_stOpenServerBuyConfig;
	std::map<int32_t, std::vector<TLogicActiveNormalLevelElem>>         m_stNormalLevelConfig;
	std::map<int32_t, std::vector<TLogicActiveEliteLevelElem>>          m_stEliteLevelConfig;
	std::map<int32_t, std::vector<TLogicActivePvpRankElem>>             m_stPVPRankConfig;
	std::map<int32_t, std::vector<TLogicActivePassLevelStarElem>>      	m_stPassLevelStarConfig;
	std::map<int32_t, std::vector<TLogicActivePassLevelTimesElem>>     	m_stPassLevelTimesConfig;
	std::map<int32_t, std::vector<TLogicActiveHeroUpgradeColorElem>>   	m_stHeroUpgradeColorConfig;
	std::map<int32_t, std::vector<TLogicActiveHeroUpgradeLevelElem>>   	m_stHeroUpgradeLevelConfig;
	std::map<int32_t, std::vector<TLogicActiveHeroUpgradeStarElem>>    	m_stHeroUpgradeStarConfig;
	std::map<int32_t, std::vector<TLogicActiveWushenLevelElem>>         m_stWushenLevelConfig;
	std::map<int32_t, std::vector<TLogicActiveTeamLevelUpgradeElem>>   	m_stTeamLevelUpgradeConfig;
	std::map<int32_t, std::vector<TLogicActiveUpgradeSkillElem>>        m_stUpgradeSkillConfig;
	std::map<int32_t, std::vector<TLogicActiveFightPowerElem>>          m_stFightPowerConfig;
	std::map<int32_t, std::vector<TLogicActiveHeroAchieveElem>>         m_stHeroAchieveConfig;
	std::map<int32_t, std::vector<TLogicActiveBenefitStoreElem>>        m_stBenefitStoreConfig;
	std::map<int32_t, std::vector<TLogicActiveExchangeStoreElem>>       m_stExchangeStoreConfig;
	std::map<int32_t, std::vector<TLogicActiveSingleDepositElem>>       m_stSingleDepositConfig;
	std::map<int32_t, std::vector<TLogicActiveDeadlineLoginElem>>       m_stDeadlineLoginConfig;
	std::map<int32_t, std::vector<TLogicActiveBuyCommodityElem>>        m_stBuyCommodityConfig;
	std::map<int32_t, std::vector<TLogicActivePvpFightTimesElem>>      	m_stPVPFightTimesConfig;
	std::map<int32_t, std::vector<TLogicActiveBuyEnergyTimesElem>>     	m_stBuyEnergyTimesConfig;
	std::map<int32_t, std::vector<TLogicActiveBuyGoldTimesElem>>       	m_stBuyGoldTimesConfig;
	std::map<int32_t, std::vector<TLogicActiveSingleItemExchangeElem>> 	m_stSingleItemExchangeConfig;

	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stCumulateConsumeYuanBaoConfig;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stDailyConsumeYuanBaoConfig;
	std::map<int32_t, std::vector<TLogicActiveBuyBlackStoreTimesElem>> 	m_stBuyBlackStoreTimesConfig;
	std::map<int32_t, std::vector<TLogicActiveCrossFightPassLevelTimesElem>> 	m_stCrossFightPassLevelTimesConfig;
	std::map<int32_t, std::vector<TLogicActiveUnlimitLevelFightTimesElem>> 		m_stUnlimitLevelFightTimesConfig;
	std::map<int32_t, std::vector<TLogicActiveEliteLevelFightTimesElem>> 		m_stEliteLevelFightTimesConfig;
	std::map<int32_t, std::vector<TLogicActiveNormalLevelFightTimesElem>> 		m_stNormalLevelFightTimesConfig;
	std::map<int32_t, std::vector<TLogicActiveFightThemeActiveLevelElem>> 		m_stFightThemeActiveLevelConfig;
	std::map<int32_t, std::vector<TLogicActiveDailyCumulateDepositElem>> 		m_stDailyCumulateDepositConfig;
	std::map<int32_t, std::vector<TLogicActiveUnlimitLevelScoreLevelElem>> 		m_stUnlimitLevelScoreConfig;
	std::map<int32_t, std::vector<TLogicActiveHeroEquipmentUpgradeStarElem>> 	m_stHeroEquipmentUpgradeStarConfig;
	std::map<int32_t, std::vector<TLogicActiveHeroEquipmentUpgradeLevelElem>> 	m_stHeroEquipmentUpgradeLevelConfig;
	std::map<int32_t, std::vector<TLogicActiveHeroEquipmentUpgradeJewelElem>> 	m_stHeroEquipmentUpgradeJewelConfig;

	std::map<int32_t, std::vector<TLogicActiveUpgradeTacticsElem>> 		m_stUpgradeTacticsConfig;
	std::map<int32_t, std::vector<TLogicActiveHeroTrainElem>> 			m_stHeroTrainConfig;
	std::map<int32_t, std::vector<TLogicActiveBreakEggElem>> 			m_stBreakEggConfig;
	std::map<int32_t, std::vector<TLogicActiveYuanbaoRecruitElem>> 		m_stYuanbaoRecruitConfig;
	std::map<int32_t, std::vector<TLogicActiveConsumeEnergyElem>> 		m_stConsumeEnergyConfig;
	std::map<int32_t, std::vector<TLogicActiveClimbTowerElem>> 			m_stClimbTowerConfig;
	std::map<int32_t, std::vector<TLogicActivePvpTerritoryElem>> 		m_stPvpTerritoryConfig;
	std::map<int32_t, std::vector<TLogicActiveTacticsCountElem>> 		m_stTacticsCountConfig;
	std::map<int32_t, std::vector<TLogicActiveGuildDonateElem>> 		m_stGuildDonateConfig;
	std::map<int32_t, std::vector<TLogicActiveGuildTaskElem>> 			m_stGuildTaskConfig;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stLuckyBoxConfig;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stTalentScoreConfig;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stTotalHeroTrainConfig;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stTurnTableScoreConfig;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stSendFlowerConfig;

	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stLoginInsureMap;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stHeroInsureMap;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stHomeInsureMap;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stPetInsureMap;
	std::map<int32_t, std::vector<TLogicActiveHeartInsureElem>>         m_stHeartInsureMap;
    std::map<int32_t, std::pair<int32_t, std::vector<TLogicActiveCalculateCountElem>>> 		m_stLoginFundMap;

	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stWishCharacterConfig;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stFlowerLanterConfig;
	std::map<int32_t, TLogicActiveFlowerLanternInfoElem> 				m_stFlowerLanterInfoConfig;
	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_stLuckyHammerConfig;

	std::map<int32_t, std::vector<TLogicActiveCalculateCountElem>> 		m_mapCountDownHeroConfig;//限时神将配置
};

//合服活动
struct TLogicMergeActivityTaskElem
{
	TLogicMergeActivityTaskElem() : m_iType(0), m_iTarget(0), m_iTargetAmount(0) {}

	int32_t m_iType;
	int32_t m_iTarget;
	int32_t m_iTargetAmount;
	std::vector<CPackGameItem> m_stBonusItemVec;
};

struct TLogicMergeActivityExchangeElem
{
	TLogicMergeActivityExchangeElem() : m_iExchangeCount(0) {}

	int32_t m_iExchangeCount;
	std::vector<CPackGameItem> m_stConsumeVec;
	std::vector<CPackGameItem> m_stBonusVec;
};

struct TLogicMergeActivityElem
{	
	TLogicMergeActivityElem() :m_iTimeType(0), m_iStartDay(0), m_iEndDay(0), m_iDailyLevelCount(0){}

	int32_t m_iTimeType;
	int32_t m_iStartDay;
	int32_t m_iEndDay;
	int32_t m_iDailyLevelCount;

	std::map<int32_t, TLogicMergeActivityTaskElem> m_stTaskMap;

	std::map<int32_t, TLogicMergeActivityExchangeElem> m_stExchangeMap;

	std::map<int32_t, int32_t> m_stLevelMap;

	std::set<int32_t> m_stDepositIDSet;

	std::set<int32_t> m_stDoubleTypeSet;
};

class CLogicConfigMergeActivityParser : public CLogicConfigParserBase
{
public:
	CLogicConfigMergeActivityParser() = default;

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::map<int32_t, TLogicMergeActivityElem> m_stMergeActivityMap;
};

struct TLogicDaySingleRechargeTaskElem
{
	TLogicDaySingleRechargeTaskElem() : m_iTaskID(0), m_iTaskNum(0), m_stBonusItemVec() {}

	int32_t m_iTaskID;
	int32_t m_iTaskNum;
	std::vector<CPackGameItem>  m_stBonusItemVec;
};

struct TLogicDaySingleRechargeActivityInfo
{
	int				m_iID = 0;
	TLogicActivityRefElem m_stActivityRef;
	std::string     m_strSender;
	std::string		m_strSendTitle;
	std::string		m_strSendContent;
	std::map<int, TLogicDaySingleRechargeTaskElem>       m_stTaskMap;
};

class CLogicConfigDaySingleRechargeActivityParser : public CLogicConfigParserBase
{
public:
	CLogicConfigDaySingleRechargeActivityParser() = default;

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	const TLogicDaySingleRechargeActivityInfo* GetActivityConfig(int32_t iActivityID) const;

	const TLogicDaySingleRechargeTaskElem* GetActivityTaskConfig(int32_t iActivityID, int32_t iTaskID) const;

    std::map<int32_t, TLogicDaySingleRechargeActivityInfo> m_stActivityMap;
};

struct TLogicAvyTimeInfo
{
    TLogicAvyTimeInfo() : m_stAvyTime() {}

    TLogicAvyOpeningElem m_stAvyTime;
};

class CLogicConfigAvyTimeParser : public CLogicConfigParserBase
{
public:
    CLogicConfigAvyTimeParser() : m_stAvyTimeMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicAvyTimeInfo* GetConfig(int iAvyID) const;

    std::unordered_map<int, TLogicAvyTimeInfo> m_stAvyTimeMap;
};

struct TLogicAvyScoreAwardInfo
{
    TLogicAvyScoreAwardInfo() : m_stNeedItem(), m_stAwards() {}

    CPackGameItem               m_stNeedItem;
    std::vector<CPackGameItem>  m_stAwards;
};

class CLogicConfigAvyScoreAwardParser : public CLogicConfigParserBase
{
public:
    CLogicConfigAvyScoreAwardParser() : m_stAwardMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicAvyScoreAwardInfo* GetAwardConfig(int iAvyID, int iIndex) const;

    std::unordered_map<int, std::vector<TLogicAvyScoreAwardInfo>> m_stAwardMap;
};
