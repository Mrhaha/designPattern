#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"

/////////////////////////////////////////////////////////////////////////////////
//积分宝箱奖励
struct TLogicScoreLevelRewardElem
{
	int32_t	m_iTarget;
	std::vector<CPackGameItem> m_stBonusItemVec;
	std::vector<CPackGameItem> m_stDepositItemVec;
};

//关卡信息
struct TLogicScoreLevelFightElem
{
	int32_t m_iLevelID;
	int32_t m_iXingdongli;
	int32_t m_iScore;
	std::vector<std::pair<int32_t, int32_t>> m_stNeedParams;
};

//购买信息
struct TLogicScoreLevelBuyElem
{
	int32_t m_iCost;
	int32_t m_iNeedVipLevel;
};

//一个积分关卡活动信息
struct TLogicScoreLevelElem
{
	int32_t m_iActivityID;
	int32_t m_iBeginTime;
	int32_t m_iEndTime;
	int32_t m_iSingleBuyXingdongli;
	int32_t m_iMaxXingdongli;
	int32_t m_iInitXingdongli;
	int32_t m_iXingdongliSpeed;
	int32_t m_iXingdongliItemID;
	int32_t m_iScoreDepositID;
	std::string		m_strScoreMailTitle;
	std::string		m_strScoreMailContent;
	std::string     m_strScoreMailSender;

	std::unordered_map<int32_t, TLogicScoreLevelRewardElem> m_stRewardMap;

	std::unordered_map<int32_t, TLogicScoreLevelFightElem> m_stScoreLevelMap;

	std::unordered_map<int32_t, TLogicScoreLevelFightElem> m_stChallengeLevelMap;

	std::map<int32_t, TLogicScoreLevelBuyElem>	m_stBuyCostMap;
};

/////////////////////////////////////////////////////////////////////////////////

class CLogicConfigScoreLevelParser : public CLogicConfigParserBase
{
public:
	CLogicConfigScoreLevelParser();

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	const TLogicScoreLevelElem* GetActivityConfig(int32_t iActivityID) const;

	std::unordered_map<int32_t, TLogicScoreLevelElem> 	m_stActivityMap;
};

class CLogicConfigZooQueueParser : public CLogicConfigParserBase
{
public:
    CLogicConfigZooQueueParser() : m_bSwitch(false), m_iLogoutProtectTime(0) {};

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    bool        m_bSwitch;
    int32_t     m_iLogoutProtectTime;
};
