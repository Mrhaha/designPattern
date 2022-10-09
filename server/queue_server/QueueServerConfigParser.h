#pragma once
#include "QueueAppConfig.h"
#include "common_fmtstring.h"
#include "common_log_utility.h"
#include "server_base_protocol.h"
#include <unordered_map>
#include <map>

struct queue_config
{
    bool m_bSwitch;
    int32_t m_iMaxOnlineNum;
    int32_t m_iLoginProtectTime;
    int32_t m_iLogoutProtectTime;
    int32_t m_iQueueAliveInterval;

    std::string m_strPlatform;
    std::string m_strHostname;
    std::string m_strHost;
    int32_t m_iPort;
    std::string m_strApiMailUrl = "";
};

struct logic_config
{
    int32_t m_iServerID;
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

class CQueueConfigParser
{
public:
	virtual ~CQueueConfigParser() = default;
	virtual std::pair<bool, std::string> Load(const std::string& strXMLContent) = 0;
};

class CZooQueueConfigParser : public CQueueConfigParser
{
public:
	std::pair<bool, std::string> Load(const std::string& strXMLContent) override;

	int32_t GetServerIDByConnection(const std::string& host, uint16_t unPort) const;
    int32_t GetServerIDByGroupID(int32_t iGroupID) const;

    queue_config m_stQueueConfig;
	std::map<int32_t, logic_config> m_stLogicConfigMap;
    std::map<int32_t, logic_config> m_stLogicPortMap;
    std::map<int32_t, group_config> m_stGroupConfigMap;
};

struct pushover_mail_notify
{
    bool m_bSwitch = false;     // 功能开关
    std::string m_strMailServerUrl = "";
    std::string m_strMailUserName = "";   // 发送者账号
    std::string m_strMailUserPw = "";     // 发送者密码
};

class CQueuePushoverConfigParser : public CQueueConfigParser
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
