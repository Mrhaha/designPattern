#pragma once

#include "logic_config_base.h"
#include "server_base_protocol.h"

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicMatchLevelElem
{
	int32_t m_iScore = 0;
	int32_t m_iWinScore = 0;
	int32_t m_iLoseScore = 0;
	int32_t m_iMatchMinLevel = 0;
	int32_t m_iMatchMaxLevel = 0;
	double m_dMatchPowerMinRatio = 0.0;
	double m_dMatchPowerMaxRatio = 0.0;
};

struct TLogicMatchStoreElem
{
	int32_t m_iMatchLevelLimit = 0;
	int32_t m_iDailyBuyCount = 0;
	int32_t m_iTotalBuyCount = 0;
	CPackGameItem m_stBuyItem;
	CPackGameItem m_stConsumeItem;
};

struct TLogicMatchDailyAwardElem
{
	std::string m_strSender;
	std::string m_strTitle;
	std::string m_strContent;
	std::vector<CPackGameItem> m_stBonusVec;
};

class CLogicConfigMatchPvpParser : public CLogicConfigParserBase
{
public:
	CLogicConfigMatchPvpParser(): m_iMatchMinLevel(0), m_iDailyAwardCount(0), m_iBaodiMatchLevel(0),
	                                      m_iBaodiFailCount(0), m_iBaodiMatchRobotDelta(0)
	{
	}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	int32_t GetMatchLevelByScore(int32_t iScore) const;

	const TLogicMatchLevelElem* GetMatchLevelConfig(int32_t iLevel) const;

	const TLogicMatchStoreElem* GetMatchStoreConfig(int32_t iStoreID) const;

	const TLogicMatchDailyAwardElem* GetDailyAwardConfig(int32_t iLevel) const;

	const std::vector<CPackGameItem>* GetMaxAwardConfig(int32_t iLevel) const;

	int32_t m_iMatchMinLevel;		//进入匹配库的最小等级
	int32_t m_iDailyAwardCount;		//段位每日奖励需求次数
	int32_t m_iBaodiMatchLevel;		//需要保底的段位等级
	int32_t m_iBaodiFailCount;		//保底连续失败次数
	int32_t m_iBaodiMatchRobotDelta;	//保底时机器人的段位差

	std::map<int32_t, TLogicMatchLevelElem> m_stLevelMap;		//段位配置

	std::map<int32_t, TLogicMatchStoreElem> m_stStoreMap;		//商店配置

	std::map<int32_t, TLogicMatchDailyAwardElem> m_stDailyAwardMap;		//每日奖励

	std::map<int32_t, std::vector<CPackGameItem>> m_stMaxAwardMap;	//最大段位奖励
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///公平竞技
struct TLogicFairPvpWinElem
{
	double m_dHp = 0.0;
	double m_dAtk = 0.0;
	double m_dDef = 0.0;
	double m_dCri = 0.0;
	std::string m_strSender;
	std::string m_strTitle;
	std::string m_strContent;
	std::string m_strBroadCastID;
	std::vector<CPackGameItem> m_stBonusItemVec;
	std::vector<int32_t> m_iNotMatchCardVec;
};

struct TLogicFairPvpRobotElem
{
	int32_t m_iID = 0;
	std::string m_strName;
	int32_t m_iAvatarID = 0;
	int32_t m_iAvatarBorder = 0;
	std::vector<int32_t> m_iCardVec;
};

class CLogicConfigFairPvpParser : public CLogicConfigParserBase
{
public:
	CLogicConfigFairPvpParser() : m_iTotalQuality(0), m_iTotalWinCount(0), m_iTotalFailCount(0),
	                                      m_iOptionCardCount(0), m_iOpenDurationSecond(0), m_iFightCardColor(0),
	                                      m_iFightCardStar(0), m_iFightCardLevel(0), m_iFightSkillLevel1(0),
	                                      m_iFightSkillLevel2(0), m_iFightSkillLevel3(0), m_iFightSkillLevel4(0)
	{
	}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	TLogicFairPvpRobotElem RandomRobot() const;

	const TLogicFairPvpWinElem* GetFairPvpWinConfig(int32_t iWinCount, int32_t iTime) const;

	bool IsActivityOpen() const;

	int32_t m_iTotalQuality;
	int32_t m_iTotalWinCount;
	int32_t m_iTotalFailCount;
	int32_t m_iOptionCardCount;
	CPackGameItem m_stRefreshItem;

	int32_t m_iOpenDurationSecond;
	std::vector<std::pair<int32_t, int32_t>> m_stOpenWeekDayVec;

	int32_t m_iFightCardColor;
	int32_t m_iFightCardStar;
	int32_t m_iFightCardLevel;
	int32_t m_iFightSkillLevel1;
	int32_t m_iFightSkillLevel2;
	int32_t m_iFightSkillLevel3;
	int32_t m_iFightSkillLevel4;

	std::map<int32_t, TLogicFairPvpWinElem> m_stWinMap;

	std::map<int32_t, TLogicFairPvpWinElem> m_stActivityWinMap;

	std::vector<TLogicFairPvpRobotElem> m_stRobotVec;

	std::map<int32_t, std::pair<int32_t, int32_t>> m_stRandomCardMap;
};
