#pragma once
#include <map>
#include "AppConfig.h"
#include "tc_mysql.h"
#include "common_log_utility.h"
#include <unordered_map>
#include "common_fmtstring.h"
#include "server_base_protocol.h"

struct cross_config
{
    std::string m_strPlatform;
    std::string m_strHostname;
    std::string m_strHost;
    int32_t m_iPort;
    int32_t m_iDebugTimeOffset;
    std::string m_strApiMailUrl = "";
};

struct logic_config
{
	std::string m_strHost;
	int32_t m_iPort;
	int32_t m_iMaintainBeginTime;
	int32_t m_iMaintainEndTime;
};

struct group_config
{
	std::string m_strName;
	int32_t m_iShowID;
	int32_t m_iMergeTo;
	int32_t m_iOpenTime;
};

struct match_pvp_rule
{
	int32_t m_iMatchMinLevel = 0;
	int32_t m_iMatchMaxLevel = 0;
	double m_dMatchPowerMinRatio = 0.0;
	double m_dMatchPowerMaxRatio = 0.0;
	int32_t m_iMinimumPower = 0;
	double m_dMatchMinimumPowerMinRatio = 0.0;
	double m_dMatchMinimumPowerMaxRatio = 0.0;
	int32_t m_iNormalPower = 0;
	double m_dMatchNormalPowerMinRatio = 0.0;
	double m_dMatchNormalPowerMaxRatio = 0.0;
	int32_t m_iMatchRobotInfoStart = 0;
	int32_t m_iMatchRobotInfoEnd = 0;
	int32_t m_iMatchRobotFightStart = 0;
	int32_t m_iMatchRobotFightEnd = 0;
};

struct match_robot_info
{
	std::string m_strName;
	int32_t m_iAvatarID = 0;
	int32_t m_iAvatarBorder = 0;
	std::vector<int32_t> m_iCardIDVec;
};

struct match_robot_card
{
	int32_t m_iCardLevel = 0;
	int32_t m_iCardColor = 0;
	int32_t m_iCardStar = 0;
	int32_t m_iSkillLevel = 0;
};

struct match_robot_fight
{
	int32_t m_iFightPower = 0;
	double m_dHp = 0.0;
	double m_dAtk = 0.0;
	double m_dDef = 0.0;
	double m_dCri = 0.0;
	int32_t m_iTeamLevel = 0;
	int32_t m_iMatchScore = 0;
	std::vector<match_robot_card> m_stCardVec;
};

struct guild_war_time
{
	int32_t m_iStartWeekDay = 0;
	int32_t m_iStartHour = 0;
	int32_t m_iStartMinute = 0;
	int32_t m_iEndWeekDay = 0;
	int32_t m_iEndHour = 0;
	int32_t m_iEndMinute = 0;
};

struct guild_war_info
{
    int32_t m_iAvyPrepareTime = 0;
    bool    m_bAvyPrepareEndBegin = 0;
};

struct pushover_admin
{
    std::string     m_strName;
    std::string     m_strUrl;
    std::string     m_strUserKey;
    std::string     m_strDevToken;
    std::unordered_map<std::string, std::string>    m_stPlatformTokenMap;
};

struct pushover_other_member
{
    std::string     m_strName;
    std::string     m_strMailAddr;
};

struct pushover
{
    CFormatString m_strMessage;
    std::string m_strTitle;
    int32_t m_iPriority;
    int32_t m_iRetry;
    int32_t m_iExpire;
    std::string m_strSound;
    std::vector<int32_t> m_stPosVec;
};

class CCrossConfigParser
{
public:
	virtual ~CCrossConfigParser() = default;
	virtual std::pair<bool, std::string> Load(const std::string& strXMLContent) = 0;
};

class CZooCrossConfigParser : public CCrossConfigParser
{
public:
	std::pair<bool, std::string> Load(const std::string& strXMLContent) override;

	int32_t GetServerIDByConnection(const std::string& host, uint16_t unPort) const;
    int32_t GetServerIDByGroupID(int32_t iGroupID) const;
    std::string GetGroupNameByGroupID(int32_t iGroupID) const;
    int32_t GetShowIDByGroupID(int32_t iGroupID) const;
    int32_t GetGroupOpenTime(int32_t iGroupID) const;

    taf::TC_DBConf m_stMyGameDB;
    taf::TC_DBConf m_stMyStatDB;
    cross_config m_stCrossConfig;
	std::map<int32_t, logic_config> m_stLogicConfigMap;
    std::map<int32_t, group_config> m_stGroupConfigMap;
};

class CZooActivityConfigParser : public CCrossConfigParser
{
public:
	std::pair<bool, std::string> Load(const std::string& strXMLContent) override;

	int32_t GetActivityServer(int32_t iServerID) const;

	std::map<int32_t, std::set<int32_t>> m_stServerMap;
};

class CMatchPvpConfigParser : public CCrossConfigParser
{
public:
	std::pair<bool, std::string> Load(const std::string& strXMLContent) override;

	const match_pvp_rule* GetMatchRuleConfig(int32_t iLevel) const;

	std::map<int32_t, match_pvp_rule> m_stRuleMap;
};

class CMatchPvpRobotConfigParser : public CCrossConfigParser
{
public:
	std::pair<bool, std::string> Load(const std::string& strXMLContent) override;

	CPackPvpChallengerInfo GetRobotPvpChallenger(int32_t iInfoID, int32_t iFightID) const;

	std::map<int32_t, match_robot_info> m_stInfoMap;

	std::map<int32_t, match_robot_fight> m_stFightMap;
};

class CGuildWarConfigParser : public CCrossConfigParser
{
public:
	std::pair<bool, std::string> Load(const std::string& strXMLContent) override;

	std::multimap<std::string, guild_war_time> m_stTimeMap;
    guild_war_info  m_stInfo;
};

struct pushover_mail_notify
{
    bool m_bSwitch = false;     // 功能开关
    std::string m_strMailServerUrl = "";
    std::string m_strMailUserName = "";   // 发送者账号
    std::string m_strMailUserPw = "";     // 发送者密码
};

class CPushoverConfigParser : public CCrossConfigParser
{
public:
    enum EnumMemberPos
    {
        EMP_Invalid = 0,
        EMP_Admin = 1,
        EMP_Operations = 2,         // 运营
        EMP_Planning = 3,           // 策划

        EMP_Max,
    };

    std::pair<bool, std::string> Load(const std::string& strXMLContent) override;

    const pushover& GetPushoverConfig(const std::string& id) const;
    bool IsValidPos(const int32_t iPos) const { return (EMP_Invalid < iPos && iPos < EMP_Max); }

    pushover_mail_notify        m_stMailNotify;
    std::list<pushover_admin>   m_stPushoverAdminList;      // 需要接收通知的管理员列表
    std::multimap<int32_t, pushover_other_member>   m_stOtherMember;  // 其他成员
    std::unordered_map<std::string, pushover>       m_stPushoverMap;
};

struct TCrossGuildLevelElem
{
    TCrossGuildLevelElem() : m_iLevel(0), m_iTotalExp(0), m_iMember(0), m_iAdminCount(0) {}

    int32_t m_iLevel;
    int32_t m_iTotalExp;
    int32_t m_iMember;
    int32_t m_iAdminCount;
};

struct TLogicGuildDonateElem
{
    TLogicGuildDonateElem():m_iDonateType(0),m_iGuildDonate(0),m_iPrivateDonate(0),m_iGuildExp(0){}

    int32_t m_iDonateType;
    int32_t m_iGuildDonate;
    int32_t m_iPrivateDonate;
    int32_t m_iGuildExp;
};

struct TLogicGuildSignElem
{
    TLogicGuildSignElem():m_iLevel(0),m_iNeedLevel(0),m_iCostGuildDonate(0){}

    int32_t m_iLevel;
    int32_t m_iNeedLevel;
    int32_t m_iCostGuildDonate;
};

struct TLogicGuildDecorate
{
    TLogicGuildDecorate():m_iIndex(0),m_iCostGuildDonate(0){}

    int32_t m_iIndex;
    int32_t m_iCostGuildDonate;
};

struct TLogicGuildCollection
{
    TLogicGuildCollection():m_iItemID(0),m_iCostGuildDonate(0),m_iUnlockLevel(0),m_iGuildBuffID(0){}

    int32_t m_iItemID;
    int32_t m_iCostGuildDonate;
    int32_t m_iUnlockLevel;
    int32_t m_iGuildBuffID;
};

struct TLogicGuildDefaultTheme
{
    TLogicGuildDefaultTheme():m_iType(0),m_iDefaultInit(0){}

    int32_t m_iType;
    int32_t m_iDefaultInit;
};

struct TLogicGuildWarInfo
{
    TLogicGuildWarInfo():m_iBossHp(0){};

    uint64_t m_iBossHp;
};

struct TLogicGuildWarLevel
{
    TLogicGuildWarLevel():m_id(0),m_stGuildWarInfo() {}

    int32_t m_id;
    std::map<int32_t,TLogicGuildWarInfo> m_stGuildWarInfo;
};




class CCrossConfigGuildParser : public CCrossConfigParser
{
public:
    CCrossConfigGuildParser() : m_iChangeNameCD(0), m_iChangeNameCost(0), m_stLevelConfig(), m_stLogMap(),m_stGuildDonate(),m_stGuildDecorate(),m_iChiefOffLineDay(0),m_stGuildWarLevel(),
              m_iGuildWarStartTime(0),m_iGuildWarEndTime(0),m_iRankID(0){}

    std::pair<bool, std::string> Load(const std::string& strXMLContent) override;

    const TCrossGuildLevelElem* GetLevelConfig(int32_t iTotalExp) const;
    const TLogicGuildDonateElem* GetDonateConfig(int32_t donateType) const;
    const TLogicGuildSignElem* GetGuildSignConfig(int32_t signLevel) const;
    const TLogicGuildDecorate* GetGuildDecorateConfig(int32_t index) const;
    const TLogicGuildCollection* GetGuildCollectionConfig(int32_t index) const;
    const TLogicGuildDefaultTheme* GetGuildDefaultThemeConfig(int32_t index) const;



    int32_t GetGuildLevel(int32_t iTotalExp) const;
    int32_t GetMaxAdminCount(int32_t iTotalExp) const;
    int32_t GetMaxMemberCount(int32_t iTotalExp) const;


    int32_t             m_iChangeNameCD;
    int32_t             m_iChangeNameCost;
    std::map<int32_t, TCrossGuildLevelElem> m_stLevelConfig;
    std::unordered_map<int, CFormatString>  m_stLogMap;
    std::unordered_map<int32_t, TLogicGuildDonateElem> m_stGuildDonate;
    std::unordered_map<int32_t, TLogicGuildSignElem> m_stGuildSign;
    std::unordered_map<int32_t, TLogicGuildDecorate> m_stGuildDecorate;
    std::unordered_map<int32_t, TLogicGuildCollection> m_stGuildCollection;
    std::unordered_map<int32_t, TLogicGuildDefaultTheme> m_stGuildDefaultTheme;
    int32_t             m_iChiefOffLineDay;
    std::unordered_map<int32_t, TLogicGuildWarLevel> m_stGuildWarLevel;
    int32_t m_iGuildWarStartTime;
    int32_t m_iGuildWarEndTime;
    int32_t m_iRankID;
};


class CCrossConfigDataReportParser : public CCrossConfigParser
{
public:
    CCrossConfigDataReportParser() : m_strAppID(""), m_strURL(""),m_iOpen(true),m_hour(0),m_min(0){}

    std::pair<bool, std::string> Load(const std::string& strXMLContent) override;

    std::string m_strAppID;
    std::string m_strURL;
    bool m_iOpen;
    int32_t m_hour;
    int32_t m_min;
};
