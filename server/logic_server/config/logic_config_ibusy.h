#pragma once
#include "logic_config_base.h"
#include "logic_config_define.h"
#include "server_base_protocol.h"

/////////////////////////////////////////////////////////////////////////////////
struct TLogicIbusyEventElem
{
	int						m_iId;
	std::vector<int>		m_vecNeeds;
	std::vector<int>		m_vecCharactors;
	int						m_iHeroNum;
	int						m_iPinzhiMin;
	int						m_iPinzhiMax;
};

struct TLogicIbusyPinzhiElem
{
	int						m_iId;
	int						m_iScore;
	int						m_iExp;
	int						m_iHuoyue;
	int						m_iCoef;
	CPackGameItem	m_stReward;
};

struct TLogicIbusyLevelRewardElem
{
	int						m_iLevel;
	std::vector<CPackGameItem>	m_vecRewards;
    std::vector<CPackGameItem>	m_stDepositRewards;
};

struct TLogicIbusyRankRewardElem
{
	int						m_iId;
	int						m_iBeginLevel;
	int						m_iEndLevel;
	std::vector<CPackGameItem>	m_vecRewards;
};

struct TLogicIbusyMarketData
{
	std::vector<CPackGameItem>                      m_stRefreshConsume;
	std::map<int, std::vector<LiLogicCardbagRateVector> >       m_stCardBagInfo;
	std::vector<int>                                            m_stFlashTime;

	std::vector<TLogicCardbagInfoElem> GetCommodityElem(int iLevel) const;
};

//一个主公很忙活动信息
struct TLogicIbusyActivityInfo
{
	int		m_iId;
	int		m_iBeginTime;
	int		m_iEndTime;
	int32_t m_iDepositID;
	int32_t m_iDepositExp;
	int		m_iDayMaxHuoyue;
	std::string     m_strSender;
	std::string		m_strSendTitle;
	std::string		m_strSendContent;

	std::map<int, int>							m_mpLevelExp;
	std::vector<TLogicIbusyEventElem>		    m_vecEvents;
	std::vector<TLogicIbusyPinzhiElem>		    m_vecPinzhis;
	int											m_iTotalPinzhiCoef;
	std::map<int, int>							m_mpChangeCost;
	std::map<int, int>							m_mpCharactorAdd;
	std::map<int, int>							m_mpLevelAdd;
	std::unordered_map<int, TLogicIbusyLevelRewardElem>	m_mpLevelReward;
	std::vector<TLogicIbusyRankRewardElem>				m_vecRankReward;
	TLogicIbusyMarketData								m_stMarketData;
	std::map<std::tuple<int, int>, int>					m_mpTaskHuoyue;
};

/////////////////////////////////////////////////////////////////////////////////

class CLogicConfigIbusyParser : public CLogicConfigParserBase
{
public:
	CLogicConfigIbusyParser()
		: CLogicConfigParserBase()
	{}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::unordered_map<int, TLogicIbusyActivityInfo>	m_mpActivities;
};

struct TLogicBuffEffectElem
{
    TLogicBuffEffectElem() : m_iMutexType(0), m_iBuffType(0), m_iBuffPara(0), m_iBuffParaEx(0), m_stMutexVec(), m_stReplaceVec() {}

    int32_t     m_iMutexType;
    int32_t     m_iBuffType;
    int32_t     m_iBuffPara;
    int32_t     m_iBuffParaEx;
    std::set<int32_t> m_stMutexVec;
    std::set<int32_t> m_stReplaceVec;
};

struct TLogicBuffElem
{
    int32_t     m_iBuffEffectIndex = 0;
    int32_t     m_iDurTime = 0;
    int32_t     m_iDurDays = 0;             // 持续天数(dur_time 和 dur_days 不能同时存在)
    int32_t     m_iEffectTimes = 0;
};

struct TLogicGuildBuffElem
{
    int32_t    m_iBuffID;
    std::vector<float> m_iBuffParam;
};

class CLogicConfigBuffParser : public CLogicConfigParserBase
{
public:
    // BUFF互斥类型
    enum
    {
        EMT_Invalid = -1,
        EMT_Mutex = 0,                  // 互相排斥，不能同时存在，也就是不能使用
        EMT_Replace = 1,                // 互相替代，会覆盖掉已有BUFF
        EMT_Superpose = 2,           	// 叠加

        EMT_Max,
    };
    static bool IsValidMutexType(int iType) { return (EMT_Invalid < iType && iType < EMT_Max); }

    CLogicConfigBuffParser() : m_iMonthCardIndex(0), m_iFatMonthCardIndex(0),m_stBuffEffectMap(), m_stBuffConfig(),m_stGuildBuffConfig() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicBuffElem* GetConfig(int iBuffID) const;
    const TLogicBuffEffectElem* GetEffectConfig(int iIndex) const;

    int32_t m_iMonthCardIndex;
    int32_t m_iFatMonthCardIndex;
    std::unordered_map<int, TLogicBuffEffectElem> m_stBuffEffectMap;
    std::unordered_map<int, TLogicBuffElem>  m_stBuffConfig;
    std::unordered_map<int, TLogicGuildBuffElem> m_stGuildBuffConfig;
};
