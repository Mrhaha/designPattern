#pragma once
#include "common_fmtstring.h"
#include "server_base_protocol.h"
#include "logicso_define.h"
#include "logic_config_define.h"
#include "logic_config_base.h"

class CLogicConfigSystemParser : public CLogicConfigParserBase
{
public:
	CLogicConfigSystemParser()
		: m_stServerConfig(), m_stRouterConfig(), m_stMysqlConfig() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	bool isSelfGroup(int iGroupID) const;

	std::string GetGroupName(int32_t iGroupID) const;

	int32_t GetGroupShowID(int32_t iGroupID) const;

	bool IsPrintPackInfo() const;

	int32_t GetOpenSvrTime() const;

	//获取开服天数，开服当天算第一天
	int32_t GetOpenSvrDay() const;

	bool isServerMaintain() const;

	TLogicSoServerConfig m_stServerConfig;
	TLogicSoRouterConfig m_stRouterConfig;
	TLogicSoMysqlConfig m_stMysqlConfig;
};

class CLogicConfigGameGlobalParser : public CLogicConfigParserBase
{
public:
	CLogicConfigGameGlobalParser()
		: CLogicConfigParserBase(),
		m_iMaxEnergy(0), m_iNumSecondByAutoIncreaseOneEnergy(0), m_iEnergyStorePct(0), m_iEnergyStoreMax(0),
		m_iMaxVitality(0), m_iNumSecondByAutoIncreaseOneVitality(0),
		m_iChangeNickYuanbao(0), m_iMaxNickNameCharWidth(0), m_iMinNickNameCharWidth(0), m_iSaoDangLevelPrice(0),
		m_iOpenBoxGetCoinNum(0), m_iKillMonsterGetCoinNum(0), m_iKillBossGetCoinNum(0),
		m_iSettlementBonusGold(0), m_iRanklistAwardTime(0),
		m_iTalkWordLimit(0), m_iTalkLevelLimit(15), m_iMaxTalkInterval(0), m_iSweepLevelLimit(0),
		m_iBuyRefreshTokenCostYuanbao(0), m_iActivitySeparateOpenTime(0), m_iActivitySeparateOpenAddDay(0),
        m_iTeamPresetNum(0) {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	int             m_iMaxEnergy;
	int             m_iNumSecondByAutoIncreaseOneEnergy;
    int             m_iEnergyStorePct;                      // 溢出体力存储在食盒中的百分比
    int             m_iEnergyStoreMax;                      // 溢出体力存储在食盒中的上限
	int             m_iMaxVitality;
	int             m_iNumSecondByAutoIncreaseOneVitality;

	int				m_iChangeNickYuanbao;
	std::string		m_strChangeNickStr;
	int             m_iMaxNickNameCharWidth;
	int             m_iMinNickNameCharWidth;

	int             m_iSaoDangLevelPrice;
	int             m_iOpenBoxGetCoinNum;
	int             m_iKillMonsterGetCoinNum;
	int             m_iKillBossGetCoinNum;
	int             m_iSettlementBonusGold;

	int             m_iRanklistAwardTime;

	int             m_iTalkWordLimit;
	int				m_iTalkLevelLimit;
	int				m_iBanIPTalkLevelLimit;
	int             m_iMaxTalkInterval;
	int             m_iSweepLevelLimit;
	int             m_iBuyRefreshTokenCostYuanbao;

	int32_t m_iActivitySeparateOpenTime;
    int32_t m_iActivitySeparateOpenAddDay;

    int             m_iTeamPresetNum;
};

///////////////////////////////////////////////////////////////////////////////////////////
struct TLogicChannelConfig
{
    TLogicChannelConfig() : m_iH5PaySwitch(0), m_strH5PayLink(), m_iThreePaySwitch(0), m_strThreePayLink(), m_strEvaluateUrl() {};

    int32_t m_iH5PaySwitch;
    std::string m_strH5PayLink;
    int32_t m_iThreePaySwitch;
    std::string m_strThreePayLink;
    std::string m_strEvaluateUrl;
};

struct TLogicBulletinConfig
{
	TLogicBulletinConfig() : m_iID(0), m_iBeginTime(0), m_iEndTime(0), m_iIntervals(0), m_strContent() {}

	int			m_iID;
	int			m_iBeginTime;
	int			m_iEndTime;
	int			m_iIntervals;
	std::string m_strContent;
	std::set<int32_t> m_iGroups;
	std::set<int32_t> m_iChannels;
};

class CLogicConfigZooLogicPlatformParser : public CLogicConfigParserBase
{
public:
	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	bool isWhiteAccount(const std::string& account) const;

	const TLogicChannelConfig& GetChannelConfig(int32_t iChannelID) const;

	bool isBannedIp(const std::string &ip) const;

	double m_dPayCashMultiple {0.0};

    std::map<int32_t, TLogicChannelConfig> m_stChannelMap;

	std::vector<TLogicBulletinConfig> m_stBulletins;

	std::set<std::string> m_stWhiteAccountSet;

	std::set<std::string> m_stBlackAccountSet;

	std::set<std::string> m_stBannedIpSet;
};
///////////////////////////////////////////////////////////////////////////////////////////////

struct TLogicCalcElem
{
	TLogicCalcElem() :
		m_fHPFightPowerRatio(0), m_fATKFightPowerRatio(0),
		m_fDEFFightPowerRatio(0), m_fCRIFightPowerRatio(0),
        m_fCritDamageFightPowerRatio(0), m_fAutoSkillFightPowerRatio(0),
        m_fUniqueSkillFightPowerRatio(0), m_fPassiveSkillFightPowerRatio(0),
		m_fWushenHPFightPowerRatio(0), m_fWushenATKFightPowerRatio(0),
		m_fAttribute1FightPowerRatio(0),
		m_fAttribute2FightPowerRatio(0),
		m_fAttribute3FightPowerRatio(0),
		m_fAttribute4FightPowerRatio(0),
		m_fAttribute5FightPowerRatio(0),
		m_fAttribute6FightPowerRatio(0),
		m_fSkill1LevelFightPowerRatio(0),
		m_fSkill2LevelFightPowerRatio(0),
		m_fSkill3LevelFightPowerRatio(0),
		m_fSkill4LevelFightPowerRatio(0),
		m_fGoldScoreRatio(0), m_iPropsDragonScore(0),
		m_iPropsQTEScore(0), m_iPropsMagnetScore(0),
		m_iPropsSkillScore(0), m_iPropsRushScore(0) {}

	float               m_fHPFightPowerRatio;
	float               m_fATKFightPowerRatio;
	float               m_fDEFFightPowerRatio;
	float               m_fCRIFightPowerRatio;
    float               m_fCritDamageFightPowerRatio;
    float               m_fAutoSkillFightPowerRatio;
    float               m_fUniqueSkillFightPowerRatio;
    float               m_fPassiveSkillFightPowerRatio;

	float               m_fWushenHPFightPowerRatio;
	float               m_fWushenATKFightPowerRatio;

	float               m_fAttribute1FightPowerRatio;
	float               m_fAttribute2FightPowerRatio;
	float               m_fAttribute3FightPowerRatio;
	float               m_fAttribute4FightPowerRatio;
	float               m_fAttribute5FightPowerRatio;
	float               m_fAttribute6FightPowerRatio;

	float               m_fSkill1LevelFightPowerRatio;
	float               m_fSkill2LevelFightPowerRatio;
	float               m_fSkill3LevelFightPowerRatio;
	float               m_fSkill4LevelFightPowerRatio;

	float               m_fGoldScoreRatio;
	int                 m_iPropsDragonScore;
	int                 m_iPropsQTEScore;
	int                 m_iPropsMagnetScore;
	int                 m_iPropsSkillScore;
	int                 m_iPropsRushScore;
};

class CLogicConfigCalcParser : public CLogicConfigParserBase
{
public:
	CLogicConfigCalcParser() : CLogicConfigParserBase() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	int32_t GetSkillPower(int32_t iSkillID) const;

	TLogicCalcElem        m_stCalcConfig;

	std::map<int32_t, int32_t> m_aiSkillPower;
};

class CLogicConfigDirtyWordParser : public CLogicConfigParserBase
{
public:
	CLogicConfigDirtyWordParser() : m_stDirtyWordList() { }

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::vector<std::string>        m_stDirtyWordList;
};

class CLogicConfigDirtyWordChatParser : public CLogicConfigParserBase
{
public:
    CLogicConfigDirtyWordChatParser() : m_stDirtyWordList() { }

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    std::vector<std::string>        m_stDirtyWordList;
};

////////////////////////////////////////////////////////////////////////////////
struct TLogicDepositConfigElem
{
	int32_t m_iProductID = 0;
	int32_t m_iType = 0;
	std::string m_strName = "";

	int32_t m_iShowLevel = 0;
	int32_t m_iShowMaxLevel = 0;
	int32_t m_iVipLvMin = 0;
	int32_t m_iVipLvMax = 0;

	int32_t m_iMaxBuyCount = 0;
	int32_t m_iDailyBuyCount = 0;
	int32_t m_iWeeklyBuyCount = 0;
	int32_t m_iMonthBuyCount = 0;
	int32_t m_iDayIntervalBuyCount = 0;
	int32_t m_iDayIntervalNum = 0;

	int32_t m_iNormalBuyAmount = 0;
	int32_t m_iBuyItemType = 0;
	int32_t m_iBuyItemID = 0;
	std::string m_strPrice;  //价格，因为有小数，则用string存储
	double m_dPrice = 0.0;		 //价格，double 存储
	int32_t m_iVipExp = 0;

	int32_t m_iAvyID = 0;       // 关联活动
    std::vector<CPackGameItem> m_stFirstReward;
	// 特殊类型消耗物品
	CPackGameItem m_stConsumeItem;  // DEPOSIT_TYPE_OTHER_DEPOSIT = 20,  其他购买方式消耗货币
	int32_t m_iNotify = 0;
};

struct TLogicAgeLimitConfigElem
{
    int32_t m_iStartAge = 0;
    int32_t m_iEndAge = 0;
    int32_t m_iLimitNumber = 0;
};

//付费钻石比例配制
struct TLogicPayDiamondConfigElem
{
    TLogicPayDiamondConfigElem():m_iDepositID(0),m_iDiamondNum(0),m_iPriceNum(0) {}

    int32_t m_iDepositID;
    int32_t m_iDiamondNum;
    int32_t m_iPriceNum;
};


class CLogicConfigDepositParser : public CLogicConfigParserBase
{
public:
	CLogicConfigDepositParser() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::map<int32_t, TLogicDepositConfigElem>   m_stDepositConfig;
    std::map<int32_t, TLogicAgeLimitConfigElem>  m_stAgeLimitConfig;
    std::map<int32_t, TLogicPayDiamondConfigElem> m_stPayDiamondConfig;

	const TLogicDepositConfigElem* GetDepositConfig(int iID) const;
    const TLogicAgeLimitConfigElem* GetAgeLimitConfig(int iID) const;
    const TLogicPayDiamondConfigElem* GetPayDiamondRatioConfig(int iDepositID) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicBroadcastConfig
{
	TLogicBroadcastConfig()
		: m_strBroadcastID(), m_iPara(0), m_iCmd(0), m_iTriggerMinValue(0), m_iTriggerMaxValue(0),
		m_iShowPriority(0), m_stBroadcastFmt(), m_stTalkContentFmt(){}

	std::string m_strBroadcastID;
	int32_t m_iPara;       // 额外参数
	int32_t m_iCmd;         // CMD
	int32_t m_iTriggerMinValue;
    int32_t m_iTriggerMaxValue;
    int32_t m_iShowPriority;
	CFormatString m_stBroadcastFmt;
	CFormatString m_stTalkContentFmt;
    int32_t m_iJump;
};

class CLogicConfigBroadcastParser : public CLogicConfigParserBase
{
public:
	CLogicConfigBroadcastParser()
		: m_stConfigMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::multimap<std::string, TLogicBroadcastConfig>    m_stConfigMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicBroadcastTimeRefElem
{
	TLogicBroadcastTimeRefElem()
		: m_iTimeType(0), m_iStartDay(0), m_iStartTime(0), m_stOpenWeekDayVec(){}

	int					m_iTimeType;
	int                 m_iStartDay;
	int                 m_iStartTime;
	std::vector<std::pair<int32_t, int32_t>> m_stOpenWeekDayVec;	
};

struct TLogicActivityBroadcastConfigElem
{
	TLogicActivityBroadcastConfigElem()
		: m_iBroadcastID(0), m_iShowPriority(0), m_iBeginTimeOffset(0),
		m_iInterval(0), m_iCount(0), m_strContent(), m_stActivityTime(){}

	int							m_iBroadcastID;
	int                         m_iShowPriority;
	int							m_iBeginTimeOffset;
	int							m_iInterval;
	int							m_iCount;
	std::string					m_strContent;
	TLogicBroadcastTimeRefElem	m_stActivityTime;
};

class CLogicConfigActivityBroadcastParser : public CLogicConfigParserBase
{
public:
	CLogicConfigActivityBroadcastParser()
		: m_stConfigMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::unordered_map<int, TLogicActivityBroadcastConfigElem>    m_stConfigMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicFightReportConfigElem
{
	TLogicFightReportConfigElem()
		: m_strFightReportID(), m_iTriggerValue(0),
		m_stContentObj(){}

	std::string                 m_strFightReportID;
	int                         m_iTriggerValue;
	CFormatString         m_stContentObj;
};

class CLogicConfigFightReportParser : public CLogicConfigParserBase
{
public:
	CLogicConfigFightReportParser()
		: m_stConfigMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	int32_t GetMaxFightReportCount(int32_t iFightReportType) const;

	std::multimap<std::string, TLogicFightReportConfigElem>    m_stConfigMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicCreateNpcMailElem
{
	int32_t m_iCreateDay = 0;
	int32_t m_iExpiredDay = 0;
	int32_t m_iActivityID = 0;
	TLogicMailElem m_stMail;
};

struct TLogicServerNpcMailElem
{
	int32_t m_iServerDay = 0;
	int32_t m_iExpiredDay = 0;
	int32_t m_iActivityID = 0;
	TLogicMailElem m_stMail;
};

struct TLogicDatetimeNpcMailElem
{
	int32_t m_iDatetime = 0;
	int32_t m_iExpiredTime = 0;
	int32_t m_iServerDay = 0;
	int32_t m_iActivityID = 0;
	TLogicMailElem m_stMail;
};

struct TLogicLevelNpcMailElem
{
	int32_t m_iLevel = 0;
	TLogicMailElem m_stMail;
};

class CLogicConfigMailParser : public CLogicConfigParserBase
{
public:
	CLogicConfigMailParser() : CLogicConfigParserBase() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::vector<TLogicCreateNpcMailElem> 	m_astCreateMail;
	std::vector<TLogicServerNpcMailElem> 	m_astServerMail;
	std::vector<TLogicDatetimeNpcMailElem> 	m_astDateTimeMail;
	std::vector<TLogicLevelNpcMailElem> 	m_astLevelMail;

	std::map<std::string, TLogicMailElem> 	m_astEventMail;
};

///////////////////////////////////////////////////////////////
class CLogicConfigZooLogicGameParser : public CLogicConfigParserBase
{
public:
	CLogicConfigZooLogicGameParser() : CLogicConfigParserBase(), m_iMinResVersion(0),m_iSwitchControl() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::string 				m_strNormalService;
	std::string 				m_strSuperService;
	int32_t 					m_iMinResVersion;
	std::string 				m_strGuildBulletin;
	int32_t 					m_iEvaluateLevel;
	int32_t 					m_iEvaluateRatio;
	std::vector<std::string> 	m_strDirtyWordVec;
    std::vector<int32_t>        m_iSwitchControl;
};

////////////////////////////////////////////////////////////////////////////////////////////////
class CLogicConfigJubaoParser : public CLogicConfigParserBase
{
public:
	CLogicConfigJubaoParser() : CLogicConfigParserBase() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::unordered_map<int32_t, std::string> m_aszJubaoNameMap;
};

// 排行榜奖励
struct TLogicRankAwardElem
{
    TLogicRankAwardElem() : m_iMinRank(0), m_iMaxRank(0), m_strMailTitle(), m_strMailSender(), m_strMailContent(), m_stBonusVec() {}

    int32_t     m_iMinRank = 0;
    int32_t     m_iMaxRank = 0;
    std::string m_strMailTitle;
    std::string m_strMailSender;
    std::string m_strMailContent;
    std::vector<CPackGameItem> m_stBonusVec;
};

struct TLogicRankElem
{
    TLogicRankElem() : m_iRankID(0), m_bIsLess(false), m_iMaxRank(0), m_iRefreshType(0), m_iRefreshPara(0), m_bIsClear(false), m_stAwardMap() {}

    int32_t     m_iRankID;
    bool        m_bIsLess;
    int32_t     m_iMaxRank;
    int32_t     m_iRefreshType;
    int32_t     m_iRefreshPara;
    bool        m_bIsClear;

    std::map<int32_t, TLogicRankAwardElem> m_stAwardMap;
};

class CLogicConfigRankParser : public CLogicConfigParserBase
{
  public:
    CLogicConfigRankParser() : CLogicConfigParserBase(), m_stRankConfig() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicRankElem* GetConfig(int32_t iRankID) const;

    std::map<int32_t, TLogicRankElem> m_stRankConfig;
};
