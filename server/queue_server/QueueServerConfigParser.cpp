#include "QueueAppPlatform.h"
#include "QueueGameControl.h"
#include "QueueServerConfigParser.h"
#include "common_str2num.h"
#include "common_tinyxml2.h"

using namespace tinyxml2;

std::pair<bool, std::string> CZooQueueConfigParser::Load(const std::string& strXMLContent)
{
    XMLDocument document;
    if (document.Parse(strXMLContent.c_str(), strXMLContent.length()) != XML_SUCCESS)
    {
        return std::make_pair(false, "parse queue system xml error");
    }

    const XMLElement* root = document.RootElement();
    if (!root)
    {
        return std::make_pair(false, "found root elem error");
    }

    const XMLElement* info = root->FirstChildElement("info");
    if(!info)
    {
        return std::make_pair(false, "found queue info elem error");
    }

    if (info->QueryAttribute("switch", &m_stQueueConfig.m_bSwitch) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found queue/switch attr");
    }

    if (info->QueryAttribute("max_online_num", &m_stQueueConfig.m_iMaxOnlineNum) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found queue/max_online_num attr");
    }

    if (info->QueryAttribute("login_protect_time", &m_stQueueConfig.m_iLoginProtectTime) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found queue/login_protect_time attr");
    }

    if (info->QueryAttribute("logout_protect_time", &m_stQueueConfig.m_iLogoutProtectTime) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found queue/logout_protect_time attr");
    }

    if (info->QueryAttribute("queue_alive_interval", &m_stQueueConfig.m_iQueueAliveInterval) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found queue/queue_alive_interval attr");
    }

    const XMLElement* queue = root->FirstChildElement("queue");
    while (queue)
    {
        int32_t iID = 0;
        if (queue->QueryAttribute("id", &iID) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found queue/id attr");
        }

        if(iID == CQueueGameControl::m_appCfg.m_iServerID)
        {
            break;
        }

        queue = queue->NextSiblingElement("queue");
    }

    if (!queue)
    {
        return std::make_pair(false, "found queue elem error");
    }

    if (queue->QueryAttribute("platform", &m_stQueueConfig.m_strPlatform) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found queue/platform attr");
    }

    if (queue->QueryAttribute("hostname", &m_stQueueConfig.m_strHostname) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found queue/hostname attr");
    }

    if (queue->QueryAttribute("host", &m_stQueueConfig.m_strHost) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found queue/host attr");
    }

    if (queue->QueryAttribute("port", &m_stQueueConfig.m_iPort) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found queue/port attr");
    }

    if (queue->QueryAttribute("api_mail_url", &m_stQueueConfig.m_strApiMailUrl) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found queue/api_mail_url attr");
    }

    const XMLElement* element = queue->FirstChildElement("logic_list")->FirstChildElement("logic");
    while (element)
    {
        logic_config config;
        if (element->QueryAttribute("id", &config.m_iServerID) != XML_SUCCESS || config.m_iServerID <= 0)
        {
            return std::make_pair(false, "not found logic_list/logic/id attr");
        }

        if (element->QueryAttribute("host", &config.m_strHost) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found logic_list/logic/host attr");
        }

        if (element->QueryAttribute("port", &config.m_iPort) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found logic_list/logic/port attr");
        }

        if (element->QueryAttribute("maintain_begin_time", &config.m_iMaintainBeginTime) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found logic_list/logic/maintain_begin_time attr");
        }

        if (element->QueryAttribute("maintain_end_time", &config.m_iMaintainEndTime) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found logic_list/logic/maintain_end_time attr");
        }

        m_stLogicConfigMap[config.m_iServerID] = config;
        m_stLogicPortMap[config.m_iPort] = config;

        element = element->NextSiblingElement("logic");
    }

    element = queue->FirstChildElement("group_list")->FirstChildElement("group");
    while (element)
    {
        int32_t id = 0;
        group_config config;
        if (element->QueryAttribute("id", &id) != XML_SUCCESS || id <= 0)
        {
            return std::make_pair(false, "not found group_list/group/id attr");
        }

        if (element->QueryAttribute("name", &config.m_strName) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found group_list/group/name attr");
        }

        if (element->QueryAttribute("show_id", &config.m_iShowID) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found group_list/group/show_id attr");
        }

        if (element->QueryAttribute("open_time", &config.m_iOpenTime) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found group_list/group/open_time attr");
        }

        if (element->QueryAttribute("merge_to", &config.m_iMergeTo) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found group_list/group/merge_to attr");
        }

        if (config.m_iMergeTo == 0)
        {
            config.m_iMergeTo = id;
        }

        m_stGroupConfigMap[id] = std::move(config);

        element = element->NextSiblingElement("group");
    }

	return std::make_pair(true, "OK");
}

int32_t CZooQueueConfigParser::GetServerIDByConnection(const std::string& host, uint16_t unPort) const
{
    auto iter = m_stLogicPortMap.find(unPort);
    if(iter != m_stLogicPortMap.end() && iter->second.m_strHost == host)
    {
        return iter->second.m_iServerID;
    }

	return -1;
}

int32_t CZooQueueConfigParser::GetServerIDByGroupID(int32_t iGroupID) const
{
	const auto iter = m_stGroupConfigMap.find(iGroupID);
	return iter == m_stGroupConfigMap.end() ? -1 : iter->second.m_iMergeTo;
}

std::pair<bool, std::string> CQueuePushoverConfigParser::Load(const std::string & strXMLContent)
{
    XMLDocument document;
    if (document.Parse(strXMLContent.c_str(), strXMLContent.length()) != XML_SUCCESS)
    {
        return std::make_pair(false, "parse pushover xml error");
    }

    const XMLElement* root = document.RootElement();
    if (!root)
    {
        return std::make_pair(false, "found root elem error");
    }

    const XMLElement* element = root->FirstChildElement("admin_list")->FirstChildElement("admin");
    while (element)
    {
        pushover_admin config;
        if (element->QueryAttribute("name", &config.m_strName) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/admin/name attr");
        }

        if (element->QueryAttribute("url", &config.m_strUrl) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/admin/url attr");
        }

        if (element->QueryAttribute("user_key", &config.m_strUserKey) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/admin/user_key attr");
        }

        if (element->QueryAttribute("dev_token", &config.m_strDevToken) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/admin/dev_token attr");
        }

        for(int i = 1; i <= 10; i++)
        {
            std::string platform_list;
            std::string token;
            std::string name = "platform" + std::to_string(i);
            if (!element->FindAttribute(name.c_str()))
            {
                break;
            }

            if (element->QueryAttribute(name.c_str(), &platform_list) != XML_SUCCESS || platform_list.empty())
            {
                return std::make_pair(false, "pushover/admin/platform attr error");
            }

            name = "token" + std::to_string(i);
            if (element->QueryAttribute(name.c_str(), &token) != XML_SUCCESS || token.empty())
            {
                return std::make_pair(false, "pushover/admin/token attr error");
            }

            auto split_list = string_split(platform_list, "|");
            for(auto& plat : split_list)
            {
                const std::string platform = "BUILD_PLATFORM_" + plat;
                config.m_stPlatformTokenMap[platform] = token;
            }
        }

        m_stPushoverAdminList.push_back(std::move(config));

        element = element->NextSiblingElement("admin");
    }

    const XMLElement* mem_list_ele = root->FirstChildElement("member_list");
    if(mem_list_ele)
    {
        const XMLElement* server_mail_ele = mem_list_ele->FirstChildElement("server_mail");
        if(!server_mail_ele)
        {
            return std::make_pair(false, "found server_mail elem error");
        }

        if (server_mail_ele->QueryAttribute("switch", &m_stMailNotify.m_bSwitch) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/switch attr");
        }

        if(m_stMailNotify.m_bSwitch)
        {
            if (server_mail_ele->QueryAttribute("server_url", &m_stMailNotify.m_strMailServerUrl) != XML_SUCCESS)
            {
                return std::make_pair(false, "not found pushover/server_url attr");
            }

            if (server_mail_ele->QueryAttribute("user_name", &m_stMailNotify.m_strMailUserName) != XML_SUCCESS)
            {
                return std::make_pair(false, "not found pushover/user_name attr");
            }

            if (server_mail_ele->QueryAttribute("user_pw", &m_stMailNotify.m_strMailUserPw) != XML_SUCCESS)
            {
                return std::make_pair(false, "not found pushover/user_pw attr");
            }

            element = mem_list_ele->FirstChildElement("member");
            while (element)
            {
                pushover_other_member stMember;
                int32_t iPos = 0;
                if (element->QueryAttribute("pos", &iPos) != XML_SUCCESS || !IsValidPos(iPos))
                {
                    return std::make_pair(false, "invalid pushover/pos attr");
                }

                if (element->QueryAttribute("name", &stMember.m_strName) != XML_SUCCESS || stMember.m_strName.empty())
                {
                    return std::make_pair(false, "not found pushover/name attr");
                }

                if (element->QueryAttribute("mail_addr", &stMember.m_strMailAddr) != XML_SUCCESS || stMember.m_strMailAddr.empty())
                {
                    return std::make_pair(false, "not found pushover/mail_addr attr");
                }

                m_stOtherMember.emplace(iPos, stMember);

                element = element->NextSiblingElement("member");
            }
        }
    }

    element = root->FirstChildElement("pushover_list")->FirstChildElement("pushover");
    while (element)
    {
        std::string id;
        pushover config;
        if (element->QueryAttribute("id", &id) != XML_SUCCESS || id.empty())
        {
            return std::make_pair(false, "not found pushover/id attr");
        }

        std::string strPosList = "";
        if (element->QueryAttribute("pos_list", &strPosList) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/pos_list attr");
        }

        if(!strPosList.empty())
        {
            auto stList = string_split(strPosList, "|");
            config.m_stPosVec.clear();
            for(auto& type : stList)
            {
                int iType = ::atoi(type.c_str());
                if(!IsValidPos(iType))
                {
                    return std::make_pair(false, "invalid pushover/pos_list attr");
                }

                config.m_stPosVec.push_back(iType);
            }
        }

        const char* pszValue = element->Attribute("message");
        if (pszValue == nullptr)
        {
            return std::make_pair(false, "not found pushover/message attr");
        }
        config.m_strMessage = CFormatString::Compile(pszValue);

        if (element->QueryAttribute("title", &config.m_strTitle) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/title attr");
        }

        if (element->QueryAttribute("priority", &config.m_iPriority) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/priority attr");
        }

        if (element->QueryAttribute("retry", &config.m_iRetry) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/retry attr");
        }

        if (element->QueryAttribute("expire", &config.m_iExpire) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/expire attr");
        }

        if (element->QueryAttribute("sound", &config.m_strSound) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found pushover/sound attr");
        }

        m_stPushoverMap[id] = std::move(config);

        element = element->NextSiblingElement("pushover");
    }

    return std::make_pair(true, "OK");
}

const pushover& CQueuePushoverConfigParser::GetPushoverConfig(const std::string& id) const
{
    static pushover config = {
            CFormatString::Compile("hostname:$hostname$|platform:$platform|host:$server_ip$|server:$server_id$|error:$error$|cmd:$cmd_name$|uin:$uin$|group:$group_id$"),
            "not found id " + id,
            -1,
            0,
            0,
            "pushover",
            {},
    };

    const auto iter = m_stPushoverMap.find(id);
    return iter == m_stPushoverMap.end() ? config : iter->second;
}
