#pragma once

#include "common_fmtstring.h"
#include "server_base_protocol.h"
#include "logic_config_base.h"

using LiLogicEntireWarMarketBagVectorElem = std::vector<std::pair<std::string, int32_t>>;
using LiLogicEntireWarMarketElem = std::vector<LiLogicEntireWarMarketBagVectorElem>;
using LiLogicEntireWarTreasureElem = std::vector<std::pair<std::string, int32_t>>;

struct TLogicEntireWarMilitaryRankElem
{
	int32_t m_iATK = 0;
	int32_t m_iDEF = 0;
	int32_t m_iHP = 0;
	int32_t m_iCRI = 0;
	int32_t m_iCostContribution = 0;
};

struct TLogicEntireWarFinalAwardElem
{
	int32_t m_iLevelCount = 0;
	std::vector<CPackGameItem> m_stBonusItemVec;
};

struct TLogicEntireWarRankAwardElem
{
	int32_t m_iMinRank = 0;
	int32_t m_iMaxRank = 0;
	std::string m_strMailTitle;
	std::string m_strMailSender;
	std::string m_strMailContent;
	std::vector<CPackGameItem> m_stBonusItemVec;
};

class CLogicConfigEntireWarParser : public CLogicConfigParserBase
{
public:
	CLogicConfigEntireWarParser() : m_iInitActionCoin(0), m_iMaxActionCoin(0), m_iActionCoinInterval(0)
		, m_iDefaultTreasureCount(0), m_iMonthAddTreasureCount(0), m_iSilverAddTreasureCount(0), m_iGoldAddTreasureCount(0) {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	double GetWorldLevelRatio(int32_t iLevel) const;

	const LiLogicEntireWarMarketElem* GetEntireWarMarketConfig(int32_t iWarLevelID, int32_t iMarketLevel) const;

	const LiLogicEntireWarTreasureElem* GetEntireWarTreasureConfig(int32_t iWarLevelID, int32_t iWorldLevel) const;

	const TLogicEntireWarRankAwardElem* GetEntireRankAwardConfig(int32_t iRank) const;

	int32_t GetWinCount(int32_t iCountID, int32_t iWarLevel) const;

    std::string RandEntireWarMarketBag(int32_t iWarLevelID, int32_t iMarketLevel, int32_t iMarketIndex) const;

    std::string RandEntireWarTreasureBag(int32_t iWarLevelID, int32_t iMarketLevel) const;

	int32_t m_iInitActionCoin;
	int32_t m_iMaxActionCoin;
	int32_t m_iActionCoinInterval;
	int32_t m_iDefaultTreasureCount;
	int32_t m_iMonthAddTreasureCount;
	int32_t m_iSilverAddTreasureCount;
	int32_t m_iGoldAddTreasureCount;
	std::string m_strStartMailSender;
	std::string m_strStartMailTitle;
	std::string m_strStartMailContent;

	std::map<int32_t, double> m_stWorldLevelRatio;

	std::map<int32_t, std::map<int32_t, LiLogicEntireWarMarketElem>> m_stMarketVec;

	std::map<int32_t, std::map<int32_t, LiLogicEntireWarTreasureElem>> m_stTreasureVec;

	std::map<int32_t, TLogicEntireWarMilitaryRankElem> m_stMilitaryRankMap;

	std::map<int32_t, TLogicEntireWarFinalAwardElem> m_stFinalAwardMap;

	std::map<int32_t, TLogicEntireWarRankAwardElem> m_stRankAwardMap;

	int32_t m_iWorshipCharm;
	int32_t m_iDoubleProbability;
	int32_t m_iThreeProbability;
	std::vector<CPackGameItem> m_stWorshipItemVec;
	std::vector<CPackGameItem> m_stWarGodWorshippedItemVec;
	std::string m_strWarGodWorshippedMailTitle;
	std::string m_strWarGodWorshippedMailSender;
	CFormatString m_stWarGodWorshippedMailContent;

	std::map<int32_t, int32_t> m_stBuyActionCoinCost;

	std::map<int32_t, std::map<int32_t, int32_t>> m_iWinCountMap;
};

struct TLogicEntireWarMapElem
{
    TLogicEntireWarMapElem() : m_iWarID(0), m_iPreWarID(0), m_strName(), m_iIconType(0), m_iNotify(0), m_iNodeType(0), m_iNodePara1(0)
          , m_iNodePara2(0), m_iWinCountID(0), m_stLevelLimitVec(), m_iBonusBuffType(0), m_iBonusBuffPercentage(0), m_iBonusBuffTime(0)
          , m_iBonusContribution(0), m_iBonusActionCoin(0), m_stBonusItemVec(), m_iPower(0), m_stUnlockList() {}

    int32_t         m_iWarID;
    int32_t         m_iPreWarID;
    std::string     m_strName;
    int32_t         m_iIconType;
    int32_t         m_iNotify;
    int32_t         m_iNodeType;
    int32_t         m_iNodePara1;
    int32_t         m_iNodePara2;
    int32_t         m_iWinCountID;
    std::vector<std::pair<int32_t, int32_t>> m_stLevelLimitVec;
    int32_t         m_iBonusBuffType;
    int32_t         m_iBonusBuffPercentage;
    int32_t         m_iBonusBuffTime;
    int32_t         m_iBonusContribution;
    int32_t         m_iBonusActionCoin;
    std::vector<CPackGameItem> m_stBonusItemVec;
    int32_t         m_iPower;

    std::vector<int32_t> m_stUnlockList;
};

struct TLogicEntireWarThemeElem
{
    TLogicEntireWarThemeElem() : m_iSimpleGameTimes(0), m_stWarMap() {}

    int32_t     m_iSimpleGameTimes;
    std::map<int32_t, TLogicEntireWarMapElem> m_stWarMap;
};

class CLogicConfigEntireWarMapParser : public CLogicConfigParserBase
{
  public:
    CLogicConfigEntireWarMapParser() : m_stThemeIDVec(), m_stThemeMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    int32_t GetNextThemeID(int32_t iThemeID) const;
    const TLogicEntireWarMapElem* GetConfig(int32_t iThemeID, int32_t iWarID) const;

    std::vector<int32_t> m_stThemeIDVec;
    std::map<int32_t, TLogicEntireWarThemeElem> m_stThemeMap;
};

class CLogicConfigQuestionParser : public CLogicConfigParserBase
{
public:
	CLogicConfigQuestionParser() = default;

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::vector<int32_t> m_stQuestionVec;
};

// 特殊规则奖励包
struct TLogicRewardBagElem
{
    TLogicRewardBagElem() : m_iWeight(0), m_strTotalBag(""), m_stCardBagVec() {}

    int32_t     m_iWeight;
    std::string m_strTotalBag;                  // 固定奖励包(cardbag.xml)
    std::vector<std::string> m_stCardBagVec;    // 随机包(每个包随机一个道具 cardbag.xml 表)
};

struct TLogicRewardBagList
{
    TLogicRewardBagList() : m_iWeight(0), m_iTotalWeight(0), m_stBagList() {}

    int32_t m_iWeight;
    int32_t m_iTotalWeight;
    std::vector<TLogicRewardBagElem> m_stBagList;
};

struct TLogicRewardBagRateList
{
    TLogicRewardBagRateList() : m_iTotalWeight(0), m_stRateList() {}

    int32_t m_iTotalWeight;
    std::vector<TLogicRewardBagList> m_stRateList;
};

using RewardBagKeyMap = std::map<int32_t, std::map<int32_t, TLogicRewardBagRateList>>;

class CLogicConfigRewardBagParser : public CLogicConfigParserBase
{
  public:
    CLogicConfigRewardBagParser() : m_stBagMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    std::vector<CPackGameItem> GetReward(const std::string& strName, const int32_t& iKey, const int32_t& iKeyEx) const;

    std::map<std::string, RewardBagKeyMap> m_stBagMap;
};

// 全民战役限时BOSS
struct TLogicLimitBossHard
{
    int32_t     m_iHard;
    int32_t     m_iDurTime;
    int32_t     m_iOpenTime;
    int32_t     m_iKillRewardCoin;
    int32_t     m_iHardScore;
    std::string m_strKillRewardBagID;
    std::string m_strFinderRewardBagID;
    std::vector<std::string> m_stBagVec;
};

// 限时BOSS难度概率
struct TLogicLimitBossHardRate
{
    int32_t m_iTotalWeight = 0;
    std::map<int32_t, int32_t> m_stHardRate;
};

class CLogicConfigEntireWarLimitBossParser : public CLogicConfigParserBase
{
  public:
    enum
    {
        E_MaxAcceptBossNum = 20,            // 最多接受多少个邀请
        E_MaxHurtLogNum = 100,              // 最多纪录多少个玩家的伤害
        E_MaxAttackerNum = 100,             // 最多有多少个被邀请的攻击者
        E_MaxBossNum = 10000,               // 规定限时BOSS最大数量
    };

  public:
    CLogicConfigEntireWarLimitBossParser() : m_stBossFightReward(), m_stBossHpRewardPro(), m_stHardInfo(), m_stHardRateMap(), m_stThemeHardLevelIDMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicLimitBossHard* GetHardConfig(int32_t iHard) const;
    const TLogicLimitBossHard* GetRandHardConfig(int32_t iLevel) const;
    int32_t GetLevelID(int32_t iThemeID, int32_t iHard) const;
    int32_t GetHurtRewardPro(int32_t iHurtPrecent) const;

    int32_t         m_iScoreRankID;             // 积分排行榜
    int32_t         m_iBossActionCoin;          // 每日限时BOSS行动力
    int32_t         m_iFightConsumeCoin;        // 每次战斗消耗行动力
    int32_t         m_iDailyBossTimes;          // 每人每日触发BOSS次数
    int32_t         m_iDailyBoxTimes;           // 每人每日触发宝箱次数

    int32_t         m_iEmptyRate;               // 随机空概率
    int32_t         m_iBoxRate;                 // 随机到宝箱概率
    int32_t         m_iBossRate;                // 随机到BOSS概率

    int32_t         m_iBossDeathWaitTime;       // BOSS死亡等待时间
    int32_t         m_iBaseScore;               // BOSS战积分基础分数
    std::string     m_strBoxRewardBagID;        // 宝箱奖励ID
    int32_t         m_iBoxDurTime;              // 宝箱持续时间

    int32_t         m_iAcceptBossNum;           // 每个玩家最多接受多少个BOSS邀请
    int32_t         m_iAttackerNum;             // 每个BOSS最多接受多少个攻击者
    int32_t         m_iRandBossNum;             // 随机到的BOSS数量
    int32_t         m_iLimitRefreshNewBossTime; // 最后倒计时多少分钟不再刷新新BOSS
    int32_t         m_iLastBossEndTime;         // 活动结束前多少时间为BOSS最后存在时间

    std::string     m_strMailSender;
    std::string     m_strKillerMailTitle;
    std::string     m_strKillerMailContent;
    std::string     m_strOwnerMailTitle;
    std::string     m_strOwnerMailContent;

    std::vector<CPackGameItem>  m_stBossFightReward;    // BOSS战固定奖励，基于伤害百分比的奖励
    std::map<int32_t, int32_t>  m_stBossHpRewardPro;    // BOSS战固定奖励伤害百分比加成

    std::map<int32_t, TLogicLimitBossHard> m_stHardInfo;      // 难度信息
    std::map<int32_t, TLogicLimitBossHardRate> m_stHardRateMap; // 难度概率表
    std::map<int32_t, std::map<int32_t, int32_t>> m_stThemeHardLevelIDMap;    // 主题对应BOSS难度关卡ID
};
