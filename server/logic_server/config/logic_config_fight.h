#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"

struct TLogicBarrierBossElem
{
    TLogicBarrierBossElem() : m_iNum(0), m_iHp(0) {}

    int32_t     m_iNum;
    int32_t     m_iHp;
};

struct TLogicBarrierMapDetail
{
	TLogicBarrierMapDetail()
		: m_iCoinNum(0), m_iBoxNum(0), m_iMaxMonsterNum(0), m_stMonsterMap(), m_stBossMap() {}

	int                             m_iCoinNum;
	int                             m_iBoxNum;
    int                             m_iMaxMonsterNum;
	std::map<std::string, int>      m_stMonsterMap;
	std::map<std::string, TLogicBarrierBossElem> m_stBossMap;
};

class CLogicConfigBarrierMapParser : public CLogicConfigParserBase
{
public:
	CLogicConfigBarrierMapParser(TLogicBarrierMapDetail& stBarrierMapDetailRef)
		: CLogicConfigParserBase(), m_stBarrierMapDetailRef(stBarrierMapDetailRef) {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    void ResetData() { m_stBarrierMapDetailRef = TLogicBarrierMapDetail(); };

    TLogicBarrierMapDetail&   m_stBarrierMapDetailRef;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicGameChapterElem
{
	TLogicGameChapterElem()
		: m_iChapterType(0)
		, m_iPefectScore(0)
		, m_iMaxFightHelperNum(0)
		, m_stLevelIDList()
		, m_stStarLimit()
		, m_stChapterBonus()
	{ }

	int                                             m_iChapterType;
	int                                             m_iPefectScore;
	int                                             m_iMaxFightHelperNum;
	std::set<int>                                   m_stLevelIDList;
	std::vector<int>                                m_stStarLimit;
	std::vector<std::vector<CPackGameItem>>  m_stChapterBonus;
};

class CLogicConfigChapterParser : public CLogicConfigParserBase
{
public:
	CLogicConfigChapterParser() : CLogicConfigParserBase(), m_stChapterMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::map<int, TLogicGameChapterElem>  m_stChapterMap;

	const TLogicGameChapterElem* GetChapterConfigElem(int iChapterID) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////

struct TLogicLevelOpenTime
{
	TLogicLevelOpenTime()
		: m_iStartDayTimestamp(0), m_iEndDayTimestamp(0), m_iStartHour(0),
		m_iStartMinute(0), m_iStartSecond(0), m_iEndHour(0), m_iEndMinute(0),
		m_iEndSecond(0), m_cWeekDays(0xff), m_bHasOpenDate(false),
		m_bHasOpenHour(false) {}

	int     m_iStartDayTimestamp;
	int     m_iEndDayTimestamp;

	int     m_iStartHour;
	int     m_iStartMinute;
	int     m_iStartSecond;

	int     m_iEndHour;
	int     m_iEndMinute;
	int     m_iEndSecond;

	char    m_cWeekDays;
	bool    m_bHasOpenDate;
	bool    m_bHasOpenHour;
};

struct TLogicEvaluateElem
{
	TLogicEvaluateElem() : m_iType(0), m_iAmount(0) {}

	int         m_iType;
	int         m_iAmount;
};

struct TLogicLevelElem
{
	TLogicLevelElem() : m_iChapterID(0), m_iLevelType(0), m_iLevelTypePara(0), m_iNeedLevel(0), m_iNeedHeartLevel(0), m_iSharedLevel(0), m_iSharedTimes(0),m_iAdvRewardTimes(0),
          m_iPreLevel(-1), m_iTeamType(0), m_iEnterTimes(-1), m_bFirstPassFree(false), m_iPassFightPower(0), m_iBonusExp(0),
          m_iBonusTeamExp(0), m_iMaxCoinNum(0), m_ulBossHp(0), m_strBonusCardBag(), m_strPerfectCardBag(),
          m_strDropBag(), m_stEvaluate(), m_stFirstBonusVec(), m_stLevelOpenTime(), m_stBarrierMapDetail(), m_stAchieveBonus(), m_stSweepItem(),m_iTimeLimit(0) {}

    int                                     m_iChapterID;                   // 章节ID
    int                                     m_iLevelType;                   // 副本类型 EnumLevelType
	int                                     m_iLevelTypePara;               // 副本类型参数
	int                                     m_iNeedLevel;
	int                                     m_iNeedHeartLevel;              // 需要好感度等级（好感度副本）
    int                                     m_iSharedLevel;                 // 与指定副本共享副本次数
    int                                     m_iSharedTimes;                 // 共享副本次数
    int                                     m_iAdvRewardTimes;              // 高级奖励次数
	int                                     m_iPreLevel;
    int                                     m_iTeamType;                    // 阵容类型
	int                                     m_iEnterTimes;                  // 每日次数
    bool                                    m_bFirstPassFree;               // 首通不消耗次数
	int										m_iPassFightPower;
	int                                     m_iBonusExp;
	int                                     m_iBonusTeamExp;
	int                                     m_iMaxCoinNum;
    uint64_t                                m_ulBossHp;
	std::string                             m_strBonusCardBag;
	std::string                             m_strPerfectCardBag;
    std::string                             m_strDropBag;
	std::vector<TLogicEvaluateElem>   		m_stEvaluate;
	std::vector<CPackGameItem>            	m_stFirstBonusVec;
	TLogicLevelOpenTime              		m_stLevelOpenTime;
	TLogicBarrierMapDetail           		m_stBarrierMapDetail;
    std::vector<int>                        m_stAchieveBonus;
    CPackGameItem                           m_stSweepItem;                  // 扫荡消耗
    int                                     m_iTimeLimit;
};

class CLogicConfigLevelParser : public CLogicConfigParserBase
{
public:
	CLogicConfigLevelParser() : CLogicConfigParserBase(), m_stLevelConfig() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::unordered_map<int, TLogicLevelElem> m_stLevelConfig;

	const TLogicLevelElem* GetLevelConfigElem(int iLevelID) const;

	uint64_t GetLevelBossHp(int32_t iLevelID) const;

	// 判断副本类型
	static bool IsFightThemeLevel(int32_t iLevelID) { return (iLevelID > 8000 && iLevelID < 10000); }        // 判断是否是活动主题副本
};

///////////////////////////////////////////////////////////////////////////////////////////////

struct TLogicBossElem
{
	std::string m_strBossID;
	int32_t m_iBaseHp;
};

class CLogicConfigBossParser : public CLogicConfigParserBase
{
public:
	CLogicConfigBossParser() : CLogicConfigParserBase() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	const TLogicBossElem* GetBossConfig(const std::string strBossID) const;

	std::map<std::string, TLogicBossElem>    m_stBossConfig;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
class CLogicConfigNoCoinMonsterParser : public CLogicConfigParserBase
{
public:
	CLogicConfigNoCoinMonsterParser() : CLogicConfigParserBase(), m_stNoCoinMonsterSet(), m_stNoCoinBossSet() {  }

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::set<std::string>               m_stNoCoinMonsterSet;
	std::set<std::string>               m_stNoCoinBossSet;
};

struct TLogicZhuHaiRewardElem
{
    TLogicZhuHaiRewardElem() : m_stRewards(), m_stBagVec(), m_stLevelDropVec() {}

    std::vector<CPackGameItem>  m_stRewards;
    std::vector<std::string>    m_stBagVec;
    std::vector<std::string>    m_stLevelDropVec;
};

struct TLogicZhuHaiStoreElem
{
    TLogicZhuHaiStoreElem():m_iStoreID(0),m_iStoreType(0),m_iGetBuffID(0),
          m_stCostItem(){}

    int32_t m_iStoreID;             //商品id
    int32_t m_iStoreType;           //商品类型
    int32_t m_iGetBuffID;           //获取的buffID
    std::vector<CPackGameItem> m_stCostItem;     //消耗的物品
};

struct TLogicZhuHaiRefreshStoreElem
{
    TLogicZhuHaiRefreshStoreElem():m_iCount(0),m_iConsumeItem(){}

    int32_t m_iCount;
    std::vector<CPackGameItem> m_iConsumeItem;
};


using ZhuHaiRewardVec = std::vector<TLogicZhuHaiRewardElem>;

class CLogicConfigZhuHaiParser : public CLogicConfigParserBase
{
public:
    enum
    {
        E_MaxBuffSize = 5,         // 最大BUFF数量
    };
    enum
    {
        E_MaxBlessGroup = 5,        //祝福最大组数
    };

    enum
    {
        E_MaxBlessSingleGroup = 2,  //单组祝福最大数量
    };

    CLogicConfigZhuHaiParser() : m_stRefreshStoreMap(),m_stLevelRewardMap(),m_stStoreMap(){}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const ZhuHaiRewardVec* GetConfig(int iLevel) const;
    static CPackItemVec GetRewards(const TLogicZhuHaiRewardElem& stRewardConfig);
    const TLogicZhuHaiStoreElem* GetStoreItem(int index,int storeID) const;
    const TLogicZhuHaiRefreshStoreElem* GetRefreshStore(int count) const;



    std::unordered_map<int, TLogicZhuHaiRefreshStoreElem> m_stRefreshStoreMap;
    std::unordered_map<int, ZhuHaiRewardVec> m_stLevelRewardMap;
    std::unordered_map<int,std::unordered_map<int,TLogicZhuHaiStoreElem>> m_stStoreMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicPvpCardInfo
{
	int             m_iCardID;
    int             m_iAtkPara;
	int             m_iHpPara;
    int             m_iDefPara;
	int             m_iDiffculty;
	int             m_iSkillLevelIndex;
};

struct TLogicPvpRobotElem
{
	std::string                                 m_strNick;
	int                                         m_iLevel;
	int                                         m_iTotalCombat;
	int                                         m_iAvatarID;
	int                                         m_iAvatarBorder;
    int                                         m_iPvpScore;
    int                                         m_iUinKey;
	std::vector<TLogicPvpCardInfo>       		m_stCardInfoList;
};

class CLogicConfigPvpRobotParser : public CLogicConfigParserBase
{
public:
	CLogicConfigPvpRobotParser() : m_stBotsNameList(), m_stBaseRobot(), m_stRobotList() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicPvpRobotElem* GetConfig(int iUin, int iGroupID) const;

	std::vector<std::string>    m_stBotsNameList;
    TLogicPvpRobotElem          m_stBaseRobot;
    std::map<int32_t,TLogicPvpRobotElem> m_stRobotList;
	std::map<int32_t, std::map<int32_t ,TLogicPvpRobotElem>> m_stPvpRobotList;     //map<level,map<score,robotElem>>
};

struct TLogicPvpWeeklyRewardElem
{
    int32_t m_iIndex;
    int32_t m_iMin;
    int32_t m_iMax;
    std::string m_strMailSender;
    std::string m_strMailTitle;
    std::string m_strMailContent;
    std::vector<CPackGameItem> m_stWeeklyReward;

};


struct TLogicPvpRefreshRewardElem
{
    int32_t m_iWinCount;
    std::vector<CPackGameItem> m_stRefreshReward;
};

struct TLogicPvpRewardElem
{
    std::vector<CPackRandomGameItem> m_stLogicReward;
};

class CLogicConfigPvpParser : public CLogicConfigParserBase
{
public:
	CLogicConfigPvpParser() : m_iWinLower(0),m_iWinUpper(0),m_iWinEqual(0),m_iLose(0),
          m_iDefendWin(0),m_iDefendLose(0),m_iConsumeItemID(0),m_stWeeklyReward(),m_stRefreshReward(),m_stPvpReward(){}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicPvpWeeklyRewardElem* GetPVPWeeklyRewardElemByRank(int32_t rank) const;
    //const TLogicPvpRefreshRewardElem* GetPVPRefreshRewardElemByWinCount(int32_t count) const;

    int32_t                 m_iWinLower;
    int32_t                 m_iWinUpper;
    int32_t                 m_iWinEqual;
    int32_t                 m_iLose;
    int32_t                 m_iDefendWin;
    int32_t                 m_iDefendLose;
    int32_t                 m_iConsumeItemID;

    std::map<int32_t,TLogicPvpWeeklyRewardElem> m_stWeeklyReward;
    std::vector<TLogicPvpRefreshRewardElem> m_stRefreshReward;
    std::vector<TLogicPvpRewardElem> m_stPvpReward;
};
//等级区间匹配规则
struct TLogicPvpMatchLevelRuleElem
{
    int32_t m_iMinLevel;
    int32_t m_iMaxLevel;
    int32_t m_iLowerNumberMin;
    int32_t m_iLowerNumberMax;
    float m_iLowerPointLimit;
    float m_iUpperPointLimit;
};
//积分区间匹配规则
struct TLogicPvpMatchPointRuleElem
{
    int32_t m_iMinPoint;
    int32_t m_iMaxPoint;
    float m_iRobotPro;
};
//刷新次数匹配规则
struct TLogicPvpMatchRefreshRuleElem
{
    int32_t m_iRefreshCount;
    float m_iRobotPro;
};

struct TLogicPvpMatchRuleElem
{
	int m_iMax;

	std::vector<std::pair<double, double>> m_stMatchRadioVec;
};

class CLogicConfigPvpMatchRuleParser : public CLogicConfigParserBase
{
public:
	CLogicConfigPvpMatchRuleParser() : m_stMatchRuleMap(),m_stLevelMatchRules(), m_stPointMatchRules(),m_stRefreshMatchRules(){}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicPvpMatchLevelRuleElem* FindMatchRuleByLevel(int32_t level) const;
    const TLogicPvpMatchPointRuleElem* FindMatchRuleByPoint(int32_t point) const;
    const TLogicPvpMatchRefreshRuleElem* FindMatchRuleByRefreshCount(int32_t refreshCount) const;


	std::map<int, TLogicPvpMatchRuleElem>     m_stMatchRuleMap;
    std::map<int32_t,TLogicPvpMatchLevelRuleElem> m_stLevelMatchRules;
    std::map<int32_t,TLogicPvpMatchPointRuleElem> m_stPointMatchRules;
    std::map<int32_t,TLogicPvpMatchRefreshRuleElem> m_stRefreshMatchRules;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicCrossFightBonusElem
{
	TLogicCrossFightBonusElem() : m_iType(0), m_strCardBag(""), m_strBigPrizeBag(""), m_strFirstCardBag(""), m_stGameItem() {}

    int32_t         m_iType;                // 关卡类型
	std::string     m_strCardBag;
    std::string     m_strBigPrizeBag;
	std::string     m_strFirstCardBag;
	CPackGameItem   m_stGameItem;
};

struct TLogicCrossFightMatchElem
{
	TLogicCrossFightMatchElem() : m_iMaxRank(0), m_iMinRank(0) {}

	int             m_iMaxRank;
	int             m_iMinRank;
};

struct TLogicCrossFightConfig
{
	TLogicCrossFightConfig() : m_iPlayTimesPerDay(1), m_iAutoFightLevel(0), m_iAutoFightTeamCount(0), m_aiRefreshResetCost(), m_stBonusMap(), m_stMatchMap() {}

	int m_iPlayTimesPerDay;
	int m_iAutoFightLevel;
	int m_iAutoFightTeamCount;
	std::map<int32_t, int32_t> m_aiRefreshResetCost;
	std::map<int32_t, TLogicCrossFightBonusElem> m_stBonusMap;
	std::map<int32_t, std::vector<TLogicCrossFightMatchElem>, std::greater<int>> m_stMatchMap;
};

class CLogicConfigCrossFightParser : public CLogicConfigParserBase
{
public:
	CLogicConfigCrossFightParser() : CLogicConfigParserBase(), m_stConfig() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	std::vector<int> GetMatchRank(int iRank) const;

	bool GetFightBonus(int iPosition, bool bFirst, bool bBigPrize, std::vector<CPackGameItem>& stBonusVec) const;

	TLogicCrossFightConfig           m_stConfig;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicClimbTowerOrderElem
{
	TLogicClimbTowerOrderElem() :
		m_iOrder(0), m_iMapID(0), m_iPVP(0) {}

	int m_iOrder;
	int m_iMapID;
	int m_iPVP;
};

class CLogicConfigClimbTowerParser : public CLogicConfigParserBase
{
public:
	CLogicConfigClimbTowerParser() : CLogicConfigParserBase(), m_iSweepCostYuanbao(0), m_iSweepCostTime(0), m_stConfig(), m_iResetConst() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	const TLogicClimbTowerOrderElem* GetConfigElem(int order)const;
	const TLogicClimbTowerOrderElem* GetConfigElemByLevel(int level)const;

	int32_t GetResetCost(int32_t count)const;

	int												m_iSweepCostYuanbao;
	int												m_iSweepCostTime;
	std::map<int, TLogicClimbTowerOrderElem> 		m_stConfig;
	std::map<int, int>								m_iResetConst;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicConstelLevelDrop
{
    TLogicConstelLevelDrop() : m_iTotalWeight(0), m_iExtraTotalWeight(0),m_stConstelNumRand(), m_stConstelExtraNumRand(),m_strCardBag("") {}

    int32_t     m_iTotalWeight;
    int32_t     m_iExtraTotalWeight;
    std::vector<std::pair<int32_t, int32_t>> m_stConstelNumRand;
    std::vector<std::pair<int32_t, int32_t>> m_stConstelExtraNumRand;
    std::string m_strCardBag;
};

struct TLogicEquipLevelDrop
{
    TLogicEquipLevelDrop() : m_stCardBagVec() ,m_stAdvCardBagVec(),m_stExtraCardBagVec(){}

    std::vector<std::string> m_stCardBagVec;            //普通掉落
    std::vector<std::string> m_stAdvCardBagVec;         //高级掉落
    std::vector<std::string> m_stExtraCardBagVec;       //额外掉落
};

class CLogicConfigConstelLevelDropParser : public CLogicConfigParserBase
{
public:
    CLogicConfigConstelLevelDropParser() : m_stConstelDropMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicConstelLevelDrop* GetConstelDropConfig(int32_t iLevelID) const;
    const TLogicEquipLevelDrop* GetEquipDropConfig(int32_t iLevelID) const;

    std::unordered_map<int32_t, TLogicConstelLevelDrop> m_stConstelDropMap;
    std::unordered_map<int32_t, TLogicEquipLevelDrop> m_stEquipDropMap;
};
