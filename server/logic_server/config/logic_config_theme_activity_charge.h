#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"

/////////////////////////////////////////////////////////////////////////////////
//单项任务
struct TLogicThemeActivityChargeTaskItem
{
	int		m_id;
	int		m_iTaskType;//1.登陆好礼，2.充值活动，3.限时特卖
	int		m_iType;
	int		m_iTarget;
	int		m_iTargetAmount;
	std::vector<CPackGameItem>		m_aReward;
};

//充值礼包
struct TLogicThemeActivityChargeItem
{
	int		m_id;
	int		m_iDepositID;
	int		m_iDayBuyNum;
};

//购买
struct TLogicThemeActivityBuyItem
{
	int		m_id;
	int		m_iExchangeCount;
	std::vector<CPackGameItem>		m_aConsume;
	std::vector<CPackGameItem>		m_aReward;
};

//每天所有任务
struct TLogicThemeActivityOneday
{
	int		m_iStartTime;
	int		m_iEndTime;
	
	std::unordered_map<int, TLogicThemeActivityChargeItem>          m_stChargeItem;	//depositid:TLogicThemeActivityChargeItem
	std::unordered_map<int, TLogicThemeActivityBuyItem>             m_stBuyItem;

	std::unordered_map<int, TLogicThemeActivityChargeTaskItem>      m_stTask;
	std::multimap<std::tuple<int, int>, TLogicThemeActivityChargeTaskItem>	m_stTypeTask;
};

//一个主题活动
struct TLogicThemeActivityItem
{
	int				m_id;
	int				m_iStartTime;
	int				m_iEndTime;

	std::string     m_strSender;
	std::string		m_strSendTitle;
	std::string		m_strSendContent;

	std::unordered_map<int, TLogicThemeActivityOneday>	m_stDayActivities;
};

/////////////////////////////////////////////////////////////////////////////////

class CLogicConfigThemeActivityChargeParser : public CLogicConfigParserBase
{
public:
	CLogicConfigThemeActivityChargeParser()
		: CLogicConfigParserBase()
	{}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::unordered_map<int, TLogicThemeActivityItem>		m_stAllActivities;
};
