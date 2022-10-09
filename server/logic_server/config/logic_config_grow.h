#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"
#include "Markup.h"

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicUserExpElem
{
	TLogicUserExpElem()
		: m_iExpValue(0), m_iAddPhyPower(0), m_iCardLevelLimit(0) {}

	int                 m_iExpValue;
	int                 m_iAddPhyPower;
	int                 m_iCardLevelLimit;
};

class CLogicConfigUserExpParser : public CLogicConfigParserBase
{
public:
	CLogicConfigUserExpParser()
		: CLogicConfigParserBase(), m_stUserExpConfig() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::pair<int, int> ReCalcUserLevelExp(int iLevel, int iExp) const;

	std::vector<TLogicUserExpElem>    m_stUserExpConfig;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicMutiStepTaskConfigStepDetail
{
	TLogicMutiStepTaskConfigStepDetail()
		: m_iTargetID(0), m_iTargetAmount(0), m_stBonusItemList() {  }

	int                                     m_iTargetID;
	int                                     m_iTargetAmount;
	std::vector<CPackGameItem>              m_stBonusItemList;
};

struct TLogicMutiStepTaskConfigElem
{
	TLogicMutiStepTaskConfigElem()
		: m_iTaskID(0), m_eumTaskType(CLogicConfigDefine::MAIN_TASK), m_stStepDetail() {  }

	int                                                         m_iTaskID;
	enum CLogicConfigDefine::TASK_TYPE                        m_eumTaskType;
	int                                                         m_iOpenTaskUserLevelLimit;
	bool                                                        m_bIsCheckAfterAccept;
	std::vector<TLogicMutiStepTaskConfigStepDetail>   m_stStepDetail;
};

typedef std::map<int, TLogicMutiStepTaskConfigElem> MapLogicMutiStepTaskConfig;

class CLogicConfigMainTaskParser : public CLogicConfigParserBase
{
public:
	CLogicConfigMainTaskParser(MapLogicMutiStepTaskConfig& stConfig)
		: m_stConfig(stConfig) {  }

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	MapLogicMutiStepTaskConfig&           m_stConfig;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicSerialPayAwardConfigElem
{
	TLogicSerialPayAwardConfigElem() : m_iBoxID(0), m_iPayDay(0), m_stPackageElem() {}

	int m_iBoxID;
	int m_iPayDay;
	std::vector<CPackGameItem> m_stPackageElem;
};

struct TLogicSerialPayConfigElem
{
	TLogicSerialPayConfigElem() : m_iSerialID(0), m_stAwardElem() {}

	const TLogicSerialPayAwardConfigElem* GetAwardConfigByID(int iBoxID) const;

	int m_iSerialID;
	int m_iStartDay;
	int m_iActivityDay;
	int m_iPayMoney;
	std::vector<TLogicSerialPayAwardConfigElem> m_stAwardElem;
};

class CLogicConfigActivitySerialPayParser : public CLogicConfigParserBase
{
public:
	CLogicConfigActivitySerialPayParser() : CLogicConfigParserBase(), m_stSerialPayConfig() {  }

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	const TLogicSerialPayConfigElem* GetPayConfigByID(int iSerialID) const;

	std::vector<TLogicSerialPayConfigElem> m_stSerialPayConfig;
};

class CLogicConfigMoneyTreeExpParser : public CLogicConfigParserBase
{
public:
    CLogicConfigMoneyTreeExpParser() : m_stExpMap() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent);

    int32_t GetBaseMoney(int32_t iTotalExp) const;

    std::map<int32_t, int32_t> m_stExpMap;
};

struct TLogicQuestEventPara
{
    TLogicQuestEventPara() : m_iEventPara1(0), m_iEventPara2(0), m_iEventTarget(0), m_stParaSet(), m_stReward() {}

    int32_t         m_iEventPara1;
    int32_t         m_iEventPara2;
    int32_t         m_iEventTarget;
    std::unordered_set<int>     m_stParaSet;
    std::vector<CPackGameItem>  m_stReward;
};

struct TLogicQuestElem
{
    TLogicQuestElem() : m_iQuestID(0), m_iType(0), m_iLevelLimit(0), m_iAvyID(0), m_bNoEarlyRecord(false),
          m_iOpenHour(0), m_bIsMult(false), m_bIsDesc(false), m_iCondIndex(0), m_iEventType(0), m_stEventVec() {}

    int32_t         m_iQuestID;
    int32_t         m_iType;
    int32_t         m_iLevelLimit;
    int32_t         m_iAvyID;
    bool            m_bNoEarlyRecord;
    int32_t         m_iOpenHour;
    bool            m_bIsMult;
    bool            m_bIsDesc;              // 是否降序判断(比如排名参数，需要判断小于等于算完成)
    int32_t         m_iCondIndex;           // 任务编组的组索引
    int32_t         m_iEventType;
    std::vector<TLogicQuestEventPara> m_stEventVec;
};

class CLogicConfigQuestParser : public CLogicConfigParserBase
{
public:
    CLogicConfigQuestParser() : m_stQuestMap(), m_stEventMap(), m_stDailyQuestList() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent);

    const TLogicQuestElem* GetConfig(int32_t iID) const;

    std::map<int32_t, TLogicQuestElem> m_stQuestMap;
    std::multimap<int32_t, const TLogicQuestElem*> m_stEventMap;

    std::vector<int32_t> m_stDailyQuestList;        // 每日任务ID列表
    std::vector<int32_t> m_stPassDailyQuestList;     // 通行证每日任务
    std::vector<int32_t> m_stPassWeeklyQuestList;    // 通行证每周任务
    std::vector<int32_t> m_stPassMonthlyQuestList;   // 通行证每月任务

    std::vector<int32_t> m_stPvpWeeklyQuestList;     // PVP每周任务
};

struct TLogicBadgeCondElem
{
public:
    TLogicBadgeCondElem() : m_iBadgeID(0), m_iType(0), m_iPara(0), m_stParaSet(), m_bIsAll(false), m_stTeamCard(), m_iBuffID(0),
          m_strMissBoss(), m_strKillBoss(), m_bAllGold(false), m_bNoDeath(false), m_iNoCardType(0) {}

    int                 m_iBadgeID;
    int                 m_iType;
    int                 m_iPara;
    std::set<int>       m_stParaSet;
    bool                m_bIsAll;               // 不填表示 para 里的多个副本完成任意一个就可以，填1表示 para 里的多个副本必须全部完成
    std::set<int>       m_stTeamCard;           // 携带某些伙伴		---多个伙伴用 | 分隔，且的关系
    int                 m_iBuffID;              // 携带某种BUFF
    std::string         m_strMissBoss;          // 不被指定BOSS技能击中
    std::string         m_strKillBoss;          // 击败指定BOSS
    bool                m_bAllGold;             // 吃到所有金币
    bool                m_bNoDeath;             // 没有伙伴阵亡
    int                 m_iNoCardType;          // 不携带某种类型伙伴
};

class CLogicConfigBadgeParser : public CLogicConfigParserBase
{
public:
    CLogicConfigBadgeParser() : m_stBadgeMap(), m_stCondTypeMap() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent);

    std::unordered_map<int, TLogicBadgeCondElem>     m_stBadgeMap;
    std::multimap<int, const TLogicBadgeCondElem*>   m_stCondTypeMap;
};
