#pragma once
#include "logic_config_base.h"
#include "logic_config_define.h"
#include "server_base_protocol.h"

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicGuildWish
{
    TLogicGuildWish() : m_iWishCount(0), m_iSwapCount(0), m_iRewardGuildCoin(0) {}

    int32_t     m_iWishCount;
    int32_t     m_iSwapCount;
    int32_t m_iRewardGuildCoin;
};

struct TLogicGuildBossItem
{
    int32_t     m_iType = 0;
    int32_t     m_iID = 0;
    int32_t     m_iNumMin = 0;
    int32_t     m_iNumMax = 0;
};

struct TLogicGuildBossPersonReward
{
public:
    TLogicGuildBossPersonReward() : m_strCardBag(), m_stRewards(),m_iRewardTimes(0) {}

    std::string     m_strCardBag;
    std::vector<TLogicGuildBossItem> m_stRewards;
    int32_t m_iRewardTimes;
};

using guild_boss_person_level_map = std::map<int, TLogicGuildBossPersonReward>;
using guild_boss_person_hurt_map = std::map<uint64_t, guild_boss_person_level_map>;

struct TLogicGuildBossReward
{
    TLogicGuildBossReward() : m_stRewards() {}

    std::vector<CPackGameItem> m_stRewards;
};

struct TLogicGuildLevelNum
{
    TLogicGuildLevelNum():m_iLevel(0),m_iNextExp(0),m_iMember(0),m_iAdminMember(0){}

    int32_t m_iLevel;
    int32_t m_iNextExp;
    int32_t m_iMember;
    int32_t m_iAdminMember;
};

struct TLogicGuildDonateElem
{
    TLogicGuildDonateElem(){}

    int32_t m_iDonateType;
    int32_t m_iPrivateDonate;
    int32_t m_iGuildDonate;
    std::vector<CPackGameItem> m_stConsumeItem;
    std::vector<CPackGameItem> m_stRewardItem;
};

struct TLogicGuildSignBag
{
    TLogicGuildSignBag(){}
    std::string m_strBagName;
    int32_t m_iNum;
};

struct TLogicGuildSignRewardElem
{
    TLogicGuildSignRewardElem(){}

    int32_t m_iLevel;
    std::vector<TLogicGuildSignBag> m_stReward;
};

struct TLogicGuildRandomChatElem
{
    TLogicGuildRandomChatElem(){}

    std::vector<CPackGameItem> m_stReward;
};

struct TLogicGuildRandomPkElem
{
    TLogicGuildRandomPkElem(){}

    std::vector<CPackGameItem> m_stLoseReward;
    std::vector<CPackGameItem> m_stWinReward;
};

struct TLogicGuildRandomExchangeElem
{
    TLogicGuildRandomExchangeElem(){}

    std::vector<CPackGameItem> m_stConsume;
    std::vector<CPackGameItem> m_stReward;
};

struct TLogicGuildCollection
{
    TLogicGuildCollection():m_iItemID(0),m_iBuffID(0){}

    int32_t m_iItemID;
    int32_t m_iBuffID;
};

struct TLogicGuildBarConfig
{
    TLogicGuildBarConfig():m_iSpecialCardID(0),m_iCostTime(0),m_stReward(),m_stSpecialReward(){}

    int32_t  m_iSpecialCardID;
    int32_t  m_iCostTime;
    std::vector<CPackGameItem> m_stReward;
    std::vector<CPackGameItem> m_stSpecialReward;
};

struct TLogicGuildWarRankReward
{
    TLogicGuildWarRankReward():m_id(0),m_iMin(0),m_iMax(0),m_stReward(),m_strGuildWarSender(""),m_strGuildWarContent(""),m_strGuildWarAfterContent(""),m_strGuildWarTitle(""){}

    int32_t m_id;
    int32_t m_iMin;
    int32_t m_iMax;
    std::vector<CPackGameItem> m_stReward;
    std::string m_strGuildWarSender;
    std::string m_strGuildWarContent;
    std::string m_strGuildWarAfterContent;
    std::string m_strGuildWarTitle;
};


class CLogicConfigGuildParser : public CLogicConfigParserBase
{
public:
    enum
    {
        E_MaxGuildBoss = 7,             // 公会BOSS最大数量
        E_MaxWishSwapCardNum = 20,      // 祈愿最大交换伙伴数量
        E_MaxBulletinLength = 300,
        E_MaxWishCount = 10,            // 每天最大可获取赠送碎片数量
    };

	CLogicConfigGuildParser() : CLogicConfigParserBase(),m_iDailyGuildWarCount(0), m_iDailyRandomCount(0),m_iCreateCost(0), m_iCreateLevel(0), m_iJoinGuildCD(0), m_iChangeNameCost(0),
          m_strBossMailSender(), m_strBossMailTitle(), m_strBossMailContent(), m_iWishSwapTimes(0), m_stBossFirstAwards(),
          m_stWishMap(), m_stBossPerson(), m_stBossAll(),m_stGuildNum(),m_stGuildDonate(),m_stSignReward(),m_stChatReward(),m_stGuildBarConfig(),m_stGuildUnlockItemBuff(),m_iGuildWarStartTime(0),
          m_iGuildWarEndTime(0),m_iGuildWarCalTime(0),m_iRankID(0),m_stGuildWarRankReward(){}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicGuildBossPersonReward* GetBossPersonReward(int iWeek, uint64_t ulHurt, int iLevel) const;
    const TLogicGuildBossReward* GetBossAllReward(int iWeek, uint64_t ulHurt) const;
    const TLogicGuildWish* GetWishConfig(int iGrade) const;
    const TLogicGuildDonateElem* GetDonateConfig (int32_t donateType) const;
    const TLogicGuildSignRewardElem* GetSignRewardConfig (int32_t level) const;
    const TLogicGuildRandomChatElem* GetChatRewardConfig (int32_t index) const;
    const TLogicGuildRandomPkElem* GetPkConfig (int32_t index) const;
    const TLogicGuildRandomExchangeElem* GetExchangeRewardConfig (int32_t index) const;
    const TLogicGuildWarRankReward* GetGuildWarRankRewardConfig(int32_t rank) const;


    int32_t             m_iDailyGuildWarCount;
    int32_t             m_iDailyRandomCount;
	int32_t 			m_iCreateCost;
	int32_t 			m_iCreateLevel;
	int32_t 			m_iUpdateBulletinLevel;
	int32_t 			m_iJoinGuildCD;
    int32_t             m_iChangeNameCost;
    std::string         m_strBossMailSender;
    std::string         m_strBossMailTitle;
    std::string         m_strBossMailContent;
    int32_t             m_iWishSwapTimes;
    std::vector<CPackGameItem> m_stBossFirstAwards;
    std::unordered_map<int32_t, TLogicGuildWish> m_stWishMap;
    std::unordered_map<int32_t, guild_boss_person_hurt_map> m_stBossPerson;
    std::unordered_map<int32_t, std::map<uint64_t, TLogicGuildBossReward>> m_stBossAll;
    std::unordered_map<int32_t,TLogicGuildLevelNum> m_stGuildNum;
    std::unordered_map<int32_t,TLogicGuildDonateElem> m_stGuildDonate;
    std::unordered_map<int32_t,TLogicGuildSignRewardElem> m_stSignReward;
    std::unordered_map<int32_t,TLogicGuildRandomChatElem> m_stChatReward;
    std::unordered_map<int32_t,TLogicGuildRandomPkElem> m_stPkReward;
    std::unordered_map<int32_t,TLogicGuildRandomExchangeElem> m_stExchangeReward;
    TLogicGuildBarConfig m_stGuildBarConfig;
    std::unordered_map<int32_t,TLogicGuildCollection> m_stGuildUnlockItemBuff;
    int32_t m_iGuildWarStartTime;
    int32_t m_iGuildWarEndTime;
    int32_t m_iGuildWarCalTime;
    int32_t m_iRankID;
    std::map<int32_t, TLogicGuildWarRankReward> m_stGuildWarRankReward;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////
///公会战
struct TLogicGuildWarTimeElem
{
	int32_t m_iStartWeekDay = 0;
	int32_t m_iStartHour = 0;
	int32_t m_iStartMinute = 0;
	int32_t m_iEndWeekDay = 0;
	int32_t m_iEndHour = 0;
	int32_t m_iEndMinute = 0;
};

struct TLogicAttackDefenseInfoElem
{
	int32_t m_iInitArmyPoint = 0;
	int32_t m_iMaxArmyPoint = 0;
	int32_t m_iArmyPointInterval = 0;
	int32_t m_iFightFailCD = 0;
	int32_t m_iRemoveCDCost = 0;
	int32_t m_iFightCostArmyPoint = 0;
	int32_t m_iRepairCostArmyPoint = 0;
	int32_t m_iFightGuildLevel = 0;
	int32_t m_iFightGuildMember = 0;
    int32_t m_iAvyPrepareTime = 0;
    bool    m_bAvyPrepareEndBegin = 0;
};

struct TLogicAttackDefenseFightElem
{
	double m_dHp = 0.0;
	double m_dAtk = 0.0;
	double m_dDef = 0.0;
	double m_dCri = 0.0;
	std::string m_strFightAwardBag;
	std::string m_strActivityFightAwardBag;
	std::string m_strRepairAwardBag;
	std::string m_strActivityRepairAwardBag;
};

class CLogicConfigGuildWarParser : public CLogicConfigParserBase
{
public:
	CLogicConfigGuildWarParser() = default;

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	bool IsAttackDefenseReady(int32_t iTime)const;

	bool IsAttackDefenseFighting(int32_t iTime)const;

	const TLogicAttackDefenseFightElem* GetAttackDefenseFightConfig(int32_t iType, int32_t iDifficult)const;

	double GetAttackDefenseMatchMultiple(int32_t iWinCount) const;

	std::multimap<std::string, TLogicGuildWarTimeElem> m_stTimeMap;

	TLogicAttackDefenseInfoElem m_stAtkDefInfo;

	std::map<std::pair<int32_t, int32_t>, TLogicAttackDefenseFightElem> m_stAtkDefFightMap;

	std::map<std::string, TLogicMailElem> m_stAtkDefAwardMap;

	std::map<std::string, TLogicMailElem> m_stActivityAtkDefAwardMap;

	std::map<int32_t, std::vector<std::pair<double, int32_t>>> m_stAtkDefMatchMap;
 };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicIntimateLevelElem
{
	int32_t m_iIntimacy = 0;
	int32_t m_iPraiseIntimacy = 0;
	int32_t m_iPraiseEnergy = 0;
};

class CLogicConfigFriendParser : public CLogicConfigParserBase
{
public:
    enum
    {
        E_MaxFriendApplyCount = 50,
    };

	CLogicConfigFriendParser() : m_iMaxFriendCount(0), m_iDailyPraiseCount(0) {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	int32_t GetIntimateLevel(int32_t iIntimacy) const;

	int32_t m_iMaxFriendCount;
	int32_t m_iDailyPraiseCount;

	std::map<int32_t, TLogicIntimateLevelElem> m_stIntimateLevelMap;
};
