#pragma once

#include "server_base_protocol.h"
#include "logic_config_base.h"
#include "common_fmtstring.h"

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicBankPriceElem
{
	int32_t m_iDay;
	int32_t m_iHour;
	int32_t m_iWeiPrice;
	int32_t m_iShuPrice;
	int32_t m_iWuPrice;
	std::string m_strEvent;
};

struct TLogicMulticolorLanternAwardElem
{
	std::string m_strSender;
	std::string m_strTitle;
	std::string m_strConetnt;
	std::string m_strDoubleTitle;
	std::string m_strDoubleContent;
	std::vector<CPackGameItem> m_stBonusVec;
};

struct TLogicMulticolorLanternElem
{
	int32_t m_iStartDatetime;
	int32_t m_iStillDay;
	int32_t m_iStopDay;
	std::vector<int32_t> m_iOpenHourVec;
	int32_t m_iDefaultAmount;
	int32_t m_iMonthCardAddAmount;
	int32_t m_iSilverMonthCardAddAmount;
	int32_t m_iGoldMonthCardAddAmount;
	int32_t m_iQuarterCardAddAmount;
	std::map<int32_t, TLogicMulticolorLanternAwardElem> m_stAwardMap;
};

struct TLogicHappyTurnCardElem
{
	int32_t m_iStartDatetime;
	int32_t m_iStillDay;
	int32_t m_iStopDay;
	int32_t m_iCostEnergy;
	std::string m_strRandBag;
	std::map<int32_t, CPackGameItem> m_stBonusItemMap;
};

struct TLogicRecommendedLineupElem
{
	int32_t m_iIndex;
	std::vector<int32_t> m_iHeroVec;
	std::vector<CPackGameItem> m_stBonusVec;
};

struct TLogicAwardDepositElem
{
    int32_t m_iAwardID;
    std::string m_strEmailSender;
    std::string m_strEmailTitle;
    std::string m_strEmailContent;
    int32_t m_iDuration;
    int32_t m_iExpired_day;
    std::vector<CPackGameItem> m_stBonusVec;
};

struct TLogicBuyBackElem
{
    int32_t m_iPara;                // 类型参数（梦世界开启需要通关8-3）
	int32_t m_iDailyCount = 0;
	int32_t m_iMaxCount = 0;
	int32_t m_iPrice = 0;
};

struct TLogicAddupDepositBonus
{
    int32_t                     m_iDepositNum;
    std::vector<CPackGameItem>  m_stBonus;
};

class CLogicConfigBankPriceParser : public CLogicConfigParserBase
{
public:
	CLogicConfigBankPriceParser() = default;

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::vector<TLogicBankPriceElem> m_stBankPriceConfigVec;
};

struct TLogicMonthCardConfig
{
    TLogicMonthCardConfig() : m_iExpiredDay(0), m_stLevelTimesMap(),m_stLevelAwardTimesMap(), m_iPhyStorePctInc(0), m_strSender(), m_strTitle(), m_strContent(),m_stAwardItems(){}

    int32_t         m_iExpiredDay;
    std::unordered_map<int, int> m_stLevelTimesMap;
    std::unordered_map<int, int> m_stLevelAwardTimesMap;
    int32_t         m_iPhyStorePctInc;
    std::string     m_strSender;
    std::string     m_strTitle;
    std::string     m_strContent;
    std::vector<CPackGameItem> m_stAwardItems;
};

struct TLogicFatMonthCardConfig
{
    TLogicFatMonthCardConfig() : m_iExpiredDay(0), m_strSender(), m_strTitle(), m_strContent(),m_stAwardItems(){}

    int32_t         m_iExpiredDay;
    std::string     m_strSender;
    std::string     m_strTitle;
    std::string     m_strContent;
    std::vector<CPackGameItem> m_stAwardItems;
};

struct TLogicPTRewardConfig
{
    TLogicPTRewardConfig() : m_strSender(""), m_strTitle(""), m_strContent(""), m_strAfterContent(""),m_stReward() {}

    std::string     m_strSender;
    std::string     m_strTitle;
    std::string     m_strContent;
    std::string     m_strAfterContent;
    std::vector<CPackGameItem> m_stReward;
};

struct TLogicDataChangeConfig
{
    TLogicDataChangeConfig(): m_iAvyTime(0),m_strSender(""),m_strTitle(""),m_strContent(""),m_iCardID(0),m_iNeedGrade(0),m_stReward(){}

    int32_t         m_iAvyTime;
    std::string     m_strSender;
    std::string     m_strTitle;
    std::string     m_strContent;
    int32_t         m_iCardID;
    int32_t         m_iNeedGrade;
    std::vector<CPackGameItem> m_stReward;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CLogicConfigAwardParser : public CLogicConfigParserBase
{
public:
	CLogicConfigAwardParser() :
		m_iBankDefaultCapacity(0), m_iBankMonthCardAddCapacity(0), m_iBankSilverMonthCardAddCapacity(0), 
		m_iBankGoldMonthCardAddCapacity(0), m_iBankQuarterCardAddCapacity(0),
		m_stRecommendedLineupMap(), m_stBuyBackMap(), m_stActivityTimeMap(),m_stAwardDepositMap(),
          m_stPTRewardConfig(),m_stMonthCardConfig(),m_stFatMonthCardConfig(),m_stDataChangeConfig(),iAntiCheatingRatio(0),iPassLevelTime(0) {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	bool IsInActivityTime(CEnumActivityTimeType iType, int32_t iTime) const;
    int GetMonthCardAddLevelTimes(int iLevelType) const;
    int GetMonthCardAdvLevelTimes(int iLevelType) const;

    const TLogicAwardDepositElem* GetAwardConfig(int32_t index) const;

	int32_t m_iBankDefaultCapacity;
	int32_t m_iBankMonthCardAddCapacity;
	int32_t m_iBankSilverMonthCardAddCapacity;
	int32_t m_iBankGoldMonthCardAddCapacity;
	int32_t m_iBankQuarterCardAddCapacity;

	TLogicMulticolorLanternElem m_stMultiColorLantern;
	TLogicHappyTurnCardElem m_stHappyTurnCard;
	std::map<int32_t, TLogicRecommendedLineupElem> m_stRecommendedLineupMap;
	std::map<int32_t, TLogicBuyBackElem> m_stBuyBackMap;
	std::map<int32_t, std::pair<int32_t, int32_t>> m_stActivityTimeMap;
    std::map<int32_t,TLogicAwardDepositElem> m_stAwardDepositMap;
    TLogicPTRewardConfig m_stPTRewardConfig;
    TLogicMonthCardConfig m_stMonthCardConfig;
    TLogicFatMonthCardConfig m_stFatMonthCardConfig;
    std::vector<CPackGameItem> m_stInitReward;
    std::vector<CPackGameItem> m_stAddReward;
    TLogicDataChangeConfig m_stDataChangeConfig;

    int32_t iAntiCheatingRatio;
    int32_t iPassLevelTime;

};

struct TLogicRedBagElem
{
    int32_t m_iTime = 0;

    // 旧红包数据
    int32_t m_iOldCount = 0;
    int32_t m_iOldTotalYuanbao = 0;
    int32_t m_iOldMinYuanbao = 0;
    int32_t m_iOldMaxYuanbao = 0;

    // 新红包数据
    std::string m_strNewRedBag = "";

    int32_t m_iSendChannel = 0;
    std::string m_strSendBroadcast;
    std::string m_strSendTalk;
};

struct TLogicNewRedBagReward
{
    int32_t m_iItemType = 0;
    int32_t m_iItemID = 0;
    int32_t m_iItemNumMin = 0;
    int32_t m_iItemNumMax = 0;
    int32_t m_iWeight = 0;
    int32_t m_iCount = 0;
    bool m_bIsBest = false;
};

struct TLogicRedBagEventElem
{
    int32_t m_iItemID = 0;
    int32_t m_iUniqueIndex = 0;
    int32_t m_iEventPara1 = 0;
    int32_t m_iEventPara2 = 0;
    std::string m_strMailSender = "";
    std::string m_strMailTitle = "";
    std::string m_strMailContent = "";
};

class CLogicConfigRedBagParser : public CLogicConfigParserBase
{
public:
    CLogicConfigRedBagParser() : m_stRedBagMap(), m_stRedBagReceiveTalkVec() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicRedBagElem* GetRedBagConfig(int32_t iItemID) const;
    const TLogicNewRedBagReward* GetNewRedBagReward(std::string strBagName, int32_t iIndex) const;

    std::map<int32_t, TLogicRedBagElem> m_stRedBagMap;
    std::vector<CFormatString> m_stRedBagReceiveTalkVec;
    std::map<std::string, std::map<int32_t, TLogicNewRedBagReward>> m_stNewRedBagMap;
    std::multimap<int32_t, TLogicRedBagEventElem> m_stRedBagEventMap;       // 红包触发事件表
};

struct TLogicWorldQuestElem
{
    int32_t     m_iQuestID = 0;
    std::string m_strName = "";
    int32_t     m_iOpenServerDay = 0;       // 任务开启服务器天数
    int32_t     m_iContinueTime = 0;        // 奖励持续时间
    CLogicConfigDefine::EnumCommonEventType m_eEventType = CLogicConfigDefine::ECET_Invalid;
    int32_t     m_iEventPara = 0;
    int32_t     m_iEventParaEx = 0;
    int32_t     m_iTargetProgress = 0;
    bool        m_bAfterCreateCanReceive = false;       // 新建角色是否可领取
    std::string m_strBagName = "";
    std::string m_strBroadcastID = "";
};

struct TLogicWorldQuestRewardBag
{
public:
    TLogicWorldQuestRewardBag() : m_stRewardBag(), m_iTotalWeight(0) {}

    std::map<int32_t, TLogicNewRedBagReward> m_stRewardBag;
    int32_t     m_iTotalWeight;
};

class CLogicConfigWorldQuestParser : public CLogicConfigParserBase
{
public:
    CLogicConfigWorldQuestParser() : m_iBigPrizeTimes(0), m_stQuestMap(), m_stQuestEventMap(), m_stRewardBagMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicWorldQuestElem* GetQuestConfig(int32_t iQuestID) const;
    const TLogicNewRedBagReward* GetRewardBagRandReward(std::string strBagName, bool bBigPrize) const;

    int32_t m_iBigPrizeTimes;           // 大奖保底次数
    std::map<int32_t, TLogicWorldQuestElem> m_stQuestMap;
    std::multimap<CLogicConfigDefine::EnumCommonEventType, TLogicWorldQuestElem> m_stQuestEventMap;
    std::map<std::string, TLogicWorldQuestRewardBag> m_stRewardBagMap;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicRankListBonusElem
{
	TLogicRankListBonusElem() : m_iBonusNum(0), m_stBonusList() { }

	int                                              m_iBonusNum; // 奖励的人数
	std::vector<CPackGameItem>   m_stBonusList;
};

struct TLogicRankListBonusDesc
{
	TLogicRankListBonusDesc()
		: m_strMailTitle(), m_strMailSender(), m_strMailContent1(), m_strMailContent2(), m_stRankListBonusVec() {}

	std::string                                     m_strMailTitle;
	std::string                                     m_strMailSender;
	std::string                                     m_strMailContent1;
	std::string                                     m_strMailContent2;
	std::vector<TLogicRankListBonusElem>    m_stRankListBonusVec;
};

class CLogicConfigRankListBonusParser : public CLogicConfigParserBase
{
public:
	CLogicConfigRankListBonusParser() : m_stPVPRanklistBonus() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	TLogicRankListBonusDesc         m_stPVPRanklistBonus;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CLogicConfigFirstDepositParser : public CLogicConfigParserBase
{
public:
	CLogicConfigFirstDepositParser() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::vector<CPackGameItem>              m_stBonusVec;
	std::vector<TLogicAddupDepositBonus>    m_stAddUpDepositBonus;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CLogicConfigDailyDepositParser : public CLogicConfigParserBase
{
public:
	CLogicConfigDailyDepositParser() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::vector<CPackGameItem>      m_stBonusVec;
};


////////////////////////////////////////////////////////////////////////////////
struct TLogicFightPowerRankActiveRankElem
{
	std::vector<CPackGameItem>  m_stBonusVec;
};

struct TLogicFightPowerRankActiveStageElem
{
	int                                             m_iFightPower;
	std::vector<CPackGameItem>  m_stBonusVec;
};

class CLogicConfigFightPowerRankActiveParser : public CLogicConfigParserBase
{
public:
	CLogicConfigFightPowerRankActiveParser()
		: m_iRankBonusHour(22), m_iRankBonusDay(7), m_iRankBonusMin(0)
		, m_strTitle(""), m_strSender(""), m_strContent1(""), m_strContent2("")
		, m_stRankInfo(), m_stStageInfo() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	const TLogicFightPowerRankActiveRankElem* GetRankInfo(int iIndex) const;
	const TLogicFightPowerRankActiveStageElem* GetStageInfo(int iIndex) const;

	int                             m_iRankBonusHour;
	int                             m_iRankBonusDay;
	int                             m_iRankBonusMin;

	std::string                     m_strTitle;
	std::string                     m_strSender;
	std::string                     m_strContent1;
	std::string                     m_strContent2;

	std::vector<TLogicFightPowerRankActiveRankElem>   m_stRankInfo;
	std::vector<TLogicFightPowerRankActiveStageElem>  m_stStageInfo;
};


///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicHighestRecordConfigElem
{
	TLogicHighestRecordConfigElem() : m_strMailSender(), m_strMailTitle(), m_stMailContentTemplateObj(),
          m_stFirstMailContentTemplateObj(),m_stRecordMap(), m_iBonusItemType(0), m_iBonusItemID(0) {}

	std::string                 m_strMailSender;
	std::string                 m_strMailTitle;
	CFormatString               m_stMailContentTemplateObj;
    CFormatString               m_stFirstMailContentTemplateObj;
	std::map<int, std::pair<int, int>> m_stRecordMap;           // <max, [min, amount]>
	int                         m_iBonusItemType;
	int                         m_iBonusItemID;
};

class CLogicConfigHighestRecordParser : public CLogicConfigParserBase
{
public:
	CLogicConfigHighestRecordParser() : m_stPVP() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	TLogicHighestRecordConfigElem           m_stPVP;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
class CLogicConfigVipDailyAwardParser : public CLogicConfigParserBase
{
public:
	CLogicConfigVipDailyAwardParser()
		: m_strTitle(""), m_strSender(""), m_strContent(""), m_stVipDailyAward() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::string         m_strTitle;
	std::string         m_strSender;
	std::string         m_strContent;

	std::map<int, CPackGameItem>    m_stVipDailyAward;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicVipBagElem
{
	int                                               m_iPrice;
	std::vector<CPackGameItem>    m_stBonusList;
};

class CLogicConfigVipBagParser : public CLogicConfigParserBase
{
public:
	CLogicConfigVipBagParser() : m_strDailyPrayBag(""), m_stVipBagMap(), m_stVipQuarterBagMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	const TLogicVipBagElem* GetVipBagConfigElem(int iVipLevel) const;

	std::string                                 m_strDailyPrayBag;          // 每日祈祷随机包
	std::map<int, TLogicVipBagElem>             m_stVipBagMap;
	std::map<int, std::vector<CPackGameItem>>   m_stVipQuarterBagMap;       // VIP至尊卡每日奖励
};

// 问卷调查题目属性
struct TLogicQuestionnaireProblem
{
    TLogicQuestionnaireProblem() : m_iType(0), m_bNotMustAnswer(false), m_iOtherIndex(0), m_iMaxChoice(0), m_strTitle(""), m_stAnswerList() {}

    int32_t         m_iType;            // 题目类型
    bool            m_bNotMustAnswer;   // true 非必答题
    int32_t         m_iOtherIndex;      // "其他"选项索引
    int32_t         m_iMaxChoice;       // 多选题最大选项数量
    std::string     m_strTitle;         // 题目
    std::vector<std::string> m_stAnswerList;    // 选项列表
};

// 问卷调查开启条件
struct TLogicQuestionnaireOpenType
{
    TLogicQuestionnaireOpenType() : m_iOpenType(0), m_iOpenPara(0), m_iOpenParaEx(0) {}

    int32_t     m_iOpenType;
    int32_t     m_iOpenPara;
    int32_t     m_iOpenParaEx;
};

// 问卷调查属性
struct TLogicQuestionnaireElem
{
    TLogicQuestionnaireElem() : m_iExpiredDateTime(0), m_stOpenTypeList(),m_strName(),m_strContent(), m_iDurationDays(0), m_stRewards(), m_stQuestionList() {}

    int32_t         m_iExpiredDateTime;         // 过期时间
    std::vector<TLogicQuestionnaireOpenType>    m_stOpenTypeList;  // 开启类型列表
    std::string     m_strName;                  // 问卷名
    std::string     m_strContent;               // 问卷标题内容
    int32_t         m_iDurationDays;            // 触发后持续天数
    std::vector<CPackGameItem>  m_stRewards;    // 奖励

    std::vector<TLogicQuestionnaireProblem>     m_stQuestionList;   // 题目列表
};

class CLogicConfigQuestionnaireParser : public CLogicConfigParserBase
{
public:
    enum
    {
        E_SelectMaxNum = 20,            // 答案选项限制在20个以为，如果要扩充，需要同步修改统计库对应表格字段大小
        E_ProblemMaxNum = 25,           // 题目数限制在25个以内，如果要扩充，需要同步修改统计库对应表格字段列数
        E_ContentMaxLength = 400,       // 自由输入内容，最大字数，如果要扩充，需要同步修改统计库表字段长度
    };

public:
    CLogicConfigQuestionnaireParser() : CLogicConfigParserBase(), m_stQuestionnaireMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicQuestionnaireElem* GetConfig(int32_t iID) const;

    std::map<int32_t, TLogicQuestionnaireElem> m_stQuestionnaireMap;
};

struct TLogicTotalSignReward
{
    TLogicTotalSignReward() : m_stRewards() {}

    std::vector<CPackGameItem> m_stRewards;
};

struct TLogicTodaySignReward
{
    TLogicTodaySignReward():m_strCardBag(""),m_iWeight(0),m_iNotify(0){}

    std::string m_strCardBag;
    int32_t m_iWeight;
    char m_iNotify;
};

class CLogicConfigSignParser : public CLogicConfigParserBase
{
public:
    CLogicConfigSignParser() : m_stDailySignVec(), m_stTotalSignRewardMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicTotalSignReward* GetTotalSignReward(int iTotalSign) const;

    std::vector<TLogicTodaySignReward>    m_stDailySignVec;
    std::unordered_map<int, TLogicTotalSignReward> m_stTotalSignRewardMap;
};
