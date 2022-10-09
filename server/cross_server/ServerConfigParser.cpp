#include "AppPlatform.h"
#include "ServerConfigParser.h"
#include "common_tinyxml2.h"
#include "common_str2num.h"
#include "GameControl.h"
#include "Markup.h"

using namespace tinyxml2;

std::pair<bool, std::string> CZooCrossConfigParser::Load(const std::string& strXMLContent)
{
    XMLDocument document;
    if (document.Parse(strXMLContent.c_str(), strXMLContent.length()) != XML_SUCCESS)
    {
        return std::make_pair(false, "parse cross system xml error");
    }

    const XMLElement* root = document.RootElement();
    if (!root)
    {
        return std::make_pair(false, "found root elem error");
    }

    // game数据库
    const XMLElement* element = root->FirstChildElement("game_db");
    if (!element)
    {
        return std::make_pair(false, "found cross game_db error");
    }

    if (element->QueryAttribute("host", &m_stMyGameDB._host) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found game_db/host attr");
    }
    if (element->QueryAttribute("port", &m_stMyGameDB._port) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found game_db/port attr");
    }
    if (element->QueryAttribute("user", &m_stMyGameDB._user) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found game_db/user attr");
    }
    if (element->QueryAttribute("password", &m_stMyGameDB._password) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found game_db/password attr");
    }
    if (element->QueryAttribute("database", &m_stMyGameDB._database) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found game_db/database attr");
    }
    m_stMyGameDB._charset = "utf8";
    m_stMyGameDB._flag = 0;

    const XMLElement* cross = root->FirstChildElement("cross");
    while (cross)
    {
        int32_t iID = 0;
        if (cross->QueryAttribute("id", &iID) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found cross/id attr");
        }

        if(iID == CGameControl::m_appCfg.m_iServerID)
        {
            break;
        }

        cross = cross->NextSiblingElement("cross");
    }

    if (!cross)
    {
        return std::make_pair(false, "found cross elem error");
    }

    if (cross->QueryAttribute("platform", &m_stCrossConfig.m_strPlatform) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found cross/platform attr");
    }

    if (cross->QueryAttribute("hostname", &m_stCrossConfig.m_strHostname) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found cross/hostname attr");
    }

    if (cross->QueryAttribute("host", &m_stCrossConfig.m_strHost) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found cross/host attr");
    }

    if (cross->QueryAttribute("port", &m_stCrossConfig.m_iPort) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found cross/port attr");
    }

    if (cross->QueryAttribute("api_mail_url", &m_stCrossConfig.m_strApiMailUrl) != XML_SUCCESS)
    {
        return std::make_pair(false, "not found cross/api_mail_url attr");
    }

    m_stCrossConfig.m_iDebugTimeOffset = 0;
    element = cross->FirstChildElement("logic_list")->FirstChildElement("logic");
    while (element)
    {
        int32_t id = 0;
        logic_config config;
        if (element->QueryAttribute("id", &id) != XML_SUCCESS || id <= 0)
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

#if CROSS_BUILD_PLATFORM == BUILD_PLATFORM_LOCAL
        if (element->QueryAttribute("debug_time_offset", &m_stCrossConfig.m_iDebugTimeOffset) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found logic_list/logic/debug_time_offset attr");
        }
#endif

        m_stLogicConfigMap[id] = std::move(config);

        element = element->NextSiblingElement("logic");
    }

    element = cross->FirstChildElement("group_list")->FirstChildElement("group");
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

int32_t CZooCrossConfigParser::GetServerIDByConnection(const std::string& host, uint16_t unPort) const
{
	for (const auto& logic : m_stLogicConfigMap)
	{
		if (logic.second.m_strHost == host && logic.second.m_iPort == unPort)
		{
			return logic.first;
		}
	}
	return -1;
}

int32_t CZooCrossConfigParser::GetServerIDByGroupID(int32_t iGroupID) const
{
	const auto iter = m_stGroupConfigMap.find(iGroupID);
	return iter == m_stGroupConfigMap.end() ? -1 : iter->second.m_iMergeTo;
}

std::string CZooCrossConfigParser::GetGroupNameByGroupID(int32_t iGroupID) const
{
	const auto iter = m_stGroupConfigMap.find(iGroupID);
	return iter == m_stGroupConfigMap.end() ? "" : iter->second.m_strName;
}

int32_t CZooCrossConfigParser::GetShowIDByGroupID(int32_t iGroupID) const
{
	const auto iter = m_stGroupConfigMap.find(iGroupID);
	return iter == m_stGroupConfigMap.end() ? -1 : iter->second.m_iShowID;
}

int32_t CZooCrossConfigParser::GetGroupOpenTime(int32_t iGroupID) const
{
	const auto iter = m_stGroupConfigMap.find(iGroupID);
	return iter == m_stGroupConfigMap.end() ? 0 : iter->second.m_iOpenTime;
}

std::pair<bool, std::string> CZooActivityConfigParser::Load(const std::string& strXMLContent)
{
    XMLDocument document;
    if (document.Parse(strXMLContent.c_str(), strXMLContent.length()) != XML_SUCCESS)
    {
        return std::make_pair(false, "parse cross activity xml error");
    }

    const XMLElement* root = document.RootElement();
    if (!root)
    {
        return std::make_pair(false, "found root elem error");
    }

    const XMLElement* element = root->FirstChildElement("join_list")->FirstChildElement("join");
    while (element)
    {
        const char* join = element->GetText();
        if (nullptr == join)
        {
            return std::make_pair(false, "found root/join_list/join text error");
        }

        std::set<int32_t> server_set;
        auto split = string_split(join, ",");
        for (const auto& sid : split)
        {
            int32_t id = 0;
            if (!try_str2num(sid, id))
            {
                return std::make_pair(false, "root/join_list/join text to int error");
            }
            server_set.insert(id);
        }

        if (server_set.empty())
        {
            return std::make_pair(false, "found root/join_list/join server is empty error");
        }

        int32_t id = *server_set.begin();
        m_stServerMap[id] = std::move(server_set);

        element = element->NextSiblingElement("join");
    }

    return std::make_pair(true, "OK");
}

int32_t CZooActivityConfigParser::GetActivityServer(int32_t iServerID) const
{
	for (const auto& s : m_stServerMap)
	{
		if (s.second.find(iServerID) != s.second.end())
		{
			return s.first;
		}
	}
	return iServerID;
}

std::pair<bool, std::string> CMatchPvpConfigParser::Load(const std::string& strXMLContent)
{
    XMLDocument document;
    if (document.Parse(strXMLContent.c_str(), strXMLContent.length()) != XML_SUCCESS)
    {
        return std::make_pair(false, "parse match pvp xml error");
    }

    const XMLElement* root = document.RootElement();
    if (!root)
    {
        return std::make_pair(false, "found root elem error");
    }

    const XMLElement* element = root->FirstChildElement("match_level")->FirstChildElement("level");
    while (element)
    {
        int32_t level = 0;
        match_pvp_rule config;
        if (element->QueryAttribute("level", &level) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/level attr");
        }

        if (element->QueryAttribute("match_min_level", &config.m_iMatchMinLevel) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_min_level attr");
        }

        if (element->QueryAttribute("match_max_level", &config.m_iMatchMaxLevel) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_max_level attr");
        }

        if (element->QueryAttribute("match_power_min_ratio", &config.m_dMatchPowerMinRatio) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_power_min_ratio attr");
        }

        if (element->QueryAttribute("match_power_max_ratio", &config.m_dMatchPowerMaxRatio) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_power_max_ratio attr");
        }

        if (element->QueryAttribute("minimum_power", &config.m_iMinimumPower) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/minimum_power attr");
        }

        if (element->QueryAttribute("match_minimum_power_min_ratio", &config.m_dMatchMinimumPowerMinRatio) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_minimum_power_min_ratio attr");
        }

        if (element->QueryAttribute("match_minimum_power_max_ratio", &config.m_dMatchMinimumPowerMaxRatio) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_minimum_power_max_ratio attr");
        }

        if (element->QueryAttribute("normal_power", &config.m_iNormalPower) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/normal_power attr");
        }

        if (element->QueryAttribute("match_normal_power_min_ratio", &config.m_dMatchNormalPowerMinRatio) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_normal_power_min_ratio attr");
        }

        if (element->QueryAttribute("match_normal_power_max_ratio", &config.m_dMatchNormalPowerMaxRatio) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_normal_power_max_ratio attr");
        }

        if (element->QueryAttribute("match_robot_info_start", &config.m_iMatchRobotInfoStart) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_robot_info_start attr");
        }

        if (element->QueryAttribute("match_robot_info_end", &config.m_iMatchRobotInfoEnd) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_robot_info_end attr");
        }

        if (element->QueryAttribute("match_robot_fight_start", &config.m_iMatchRobotFightStart) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_robot_fight_start attr");
        }

        if (element->QueryAttribute("match_robot_fight_end", &config.m_iMatchRobotFightEnd) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found match_level/match_robot_fight_end attr");
        }

        m_stRuleMap.insert(std::make_pair(level, config));

        element = element->NextSiblingElement("level");
    }

	return std::make_pair(true, "OK");
}

const match_pvp_rule* CMatchPvpConfigParser::GetMatchRuleConfig(int32_t iLevel) const
{
	const auto iter = m_stRuleMap.find(iLevel);
	return iter == m_stRuleMap.end() ? nullptr : &iter->second;
}

std::pair<bool, std::string> CMatchPvpRobotConfigParser::Load(const std::string& strXMLContent)
{
    XMLDocument document;
    if (document.Parse(strXMLContent.c_str(), strXMLContent.length()) != XML_SUCCESS)
    {
        return std::make_pair(false, "parse match pvp robot xml error");
    }

    const XMLElement* root = document.RootElement();
    if (!root)
    {
        return std::make_pair(false, "found root elem error");
    }

    const XMLElement* element = root->FirstChildElement("robot_info")->FirstChildElement("info");
    while (element)
    {
        int32_t id = 0;
        match_robot_info config;

        if (element->QueryAttribute("id", &id) != XML_SUCCESS || id <= 0)
        {
            return std::make_pair(false, "not found robot_info/info/id attr");
        }

        if (element->QueryAttribute("name", &config.m_strName) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found robot_info/info/name attr");
        }

        if (element->QueryAttribute("avatar_id", &config.m_iAvatarID) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found robot_info/info/avatar_id attr");
        }

        if (element->QueryAttribute("avatar_border", &config.m_iAvatarBorder) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found robot_info/info/avatar_border attr");
        }

        int32_t iCardID = 0;
        if (element->QueryAttribute("card_id_1", &iCardID) != XML_SUCCESS || iCardID <= 0)
        {
            return std::make_pair(false, "not found robot_info/info/card_id_1 attr");
        }

        config.m_iCardIDVec.push_back(iCardID);
        config.m_iCardIDVec.push_back(element->IntAttribute("card_id_2", 0));
        config.m_iCardIDVec.push_back(element->IntAttribute("card_id_3", 0));
        config.m_iCardIDVec.push_back(element->IntAttribute("card_id_4", 0));
        config.m_iCardIDVec.push_back(element->IntAttribute("card_id_5", 0));

        m_stInfoMap.insert(std::make_pair(id, config));
        element = element->NextSiblingElement("info");
    }

    element = root->FirstChildElement("robot_fight")->FirstChildElement("fight");
    while (element)
    {
        int32_t id = 0;
        match_robot_fight config;
        if (element->QueryAttribute("id", &id) != XML_SUCCESS || id <= 0)
        {
            return std::make_pair(false, "not found robot_fight/fight/id attr");
        }

        if (element->QueryAttribute("fight_power", &config.m_iFightPower) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found robot_fight/fight/fight_power attr");
        }

        if (element->QueryAttribute("team_level", &config.m_iTeamLevel) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found robot_fight/fight/team_level attr");
        }

        if (element->QueryAttribute("match_score", &config.m_iMatchScore) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found robot_fight/fight/match_score attr");
        }

        if (element->QueryAttribute("fhp", &config.m_dHp) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found robot_fight/fight/fhp attr");
        }

        if (element->QueryAttribute("fatk", &config.m_dAtk) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found robot_fight/fight/fatk attr");
        }

        if (element->QueryAttribute("fdef", &config.m_dDef) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found robot_fight/fight/fdef attr");
        }

        if (element->QueryAttribute("fcri", &config.m_dCri) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found robot_fight/fight/fcri attr");
        }

        for (int32_t i = 1; i <= 5; ++i)
        {
            match_robot_card card;
            std::string name = "card_color_" + std::to_string(i);
            card.m_iCardColor = element->IntAttribute(name.c_str(), 0);
            name = "card_level_" + std::to_string(i);
            card.m_iCardLevel = element->IntAttribute(name.c_str(), 0);
            name = "card_star_" + std::to_string(i);
            card.m_iCardStar = element->IntAttribute(name.c_str(), 0);
            name = "skill_level_" + std::to_string(i);
            card.m_iSkillLevel = element->IntAttribute(name.c_str(), 0);
            config.m_stCardVec.push_back(card);
        }

        m_stFightMap.insert(std::make_pair(id, config));
        element = element->NextSiblingElement("fight");
    }

	return std::make_pair(true, "OK");
}


CPackPvpChallengerInfo CMatchPvpRobotConfigParser::GetRobotPvpChallenger(int32_t iInfoID, int32_t iFightID) const
{
	CPackPvpChallengerInfo stChallenger;
	const auto iterInfo = m_stInfoMap.find(iInfoID);
	const auto iterFight = m_stFightMap.find(iFightID);

	if (iterInfo != m_stInfoMap.end() && iterFight != m_stFightMap.end())
	{
		stChallenger.m_stBriefInfo.m_iUid = iInfoID;
		stChallenger.m_stBriefInfo.m_iGroupID = 0;
		stChallenger.m_stBriefInfo.m_strNick = iterInfo->second.m_strName;
		stChallenger.m_stBriefInfo.m_iCombat = iterFight->second.m_iFightPower;
		stChallenger.m_stBriefInfo.m_iLevel = iterFight->second.m_iTeamLevel;
		stChallenger.m_stBriefInfo.m_iAvatarID = iterInfo->second.m_iAvatarID;
		stChallenger.m_stBriefInfo.m_iAvatarBorder = iterInfo->second.m_iAvatarBorder;

		for (size_t i = 0 ; i < iterInfo->second.m_iCardIDVec.size() && i < iterFight->second.m_stCardVec.size(); ++i)
		{
			CPackPvpChallengerCardInfo stCard;
			stCard.m_iCardID = iterInfo->second.m_iCardIDVec[i];
			stCard.m_iColor = iterFight->second.m_stCardVec[i].m_iCardColor;
			stCard.m_iStar = iterFight->second.m_stCardVec[i].m_iCardStar;
			stCard.m_iLevel = iterFight->second.m_stCardVec[i].m_iCardLevel;
			// stCard.m_iCardSkill1Level = stCard.m_iCardSkill2Level = iterFight->second.m_stCardVec[i].m_iSkillLevel;
			stChallenger.m_stCardInfoList.push_back(std::move(stCard));
		}
	}

	return stChallenger;
}

std::pair<bool, std::string> CGuildWarConfigParser::Load(const std::string & strXMLContent)
{
    XMLDocument document;
    if (document.Parse(strXMLContent.c_str(), strXMLContent.length()) != XML_SUCCESS)
    {
        return std::make_pair(false, "parse guild war xml error");
    }

    const XMLElement* root = document.RootElement();
    if (!root)
    {
        return std::make_pair(false, "found root elem error");
    }

    const XMLElement* element = root->FirstChildElement("guild_war_time")->FirstChildElement("time");
    while (element)
    {
        std::string war;
        if (element->QueryAttribute("tag", &war) != XML_SUCCESS || war.empty())
        {
            return std::make_pair(false, "not found guild_war_time/time/tag attr");
        }

        for (int32_t i = 1; i < 10; ++i)
        {
            std::string name = "start_week_day_" + std::to_string(i);
            if (element->FindAttribute(name.c_str()))
            {
                guild_war_time config;
                if (element->QueryAttribute(name.c_str(), &config.m_iStartWeekDay) != XML_SUCCESS)
                {
                    return std::make_pair(false, "not found guild_war_time/time/start_week_day attr");
                }

                name = "start_hour_" + std::to_string(i);
                if (element->QueryAttribute(name.c_str(), &config.m_iStartHour) != XML_SUCCESS)
                {
                    return std::make_pair(false, "not found guild_war_time/time/start_hour attr");
                }

                name = "start_minute_" + std::to_string(i);
                if (element->QueryAttribute(name.c_str(), &config.m_iStartMinute) != XML_SUCCESS)
                {
                    return std::make_pair(false, "not found guild_war_time/time/start_minute attr");
                }

                name = "end_week_day_" + std::to_string(i);
                if (element->QueryAttribute(name.c_str(), &config.m_iEndWeekDay) != XML_SUCCESS)
                {
                    return std::make_pair(false, "not found guild_war_time/time/end_week_day attr");
                }

                name = "end_hour_" + std::to_string(i);
                if (element->QueryAttribute(name.c_str(), &config.m_iEndHour) != XML_SUCCESS)
                {
                    return std::make_pair(false, "not found guild_war_time/time/end_hour attr");
                }

                name = "end_minute_" + std::to_string(i);
                if (element->QueryAttribute(name.c_str(), &config.m_iEndMinute) != XML_SUCCESS)
                {
                    return std::make_pair(false, "not found guild_war_time/time/end_minute attr");
                }

                m_stTimeMap.emplace(war, config);
            }
        }

        element = element->NextSiblingElement("time");
    }

    const XMLElement* infoEle = root->FirstChildElement("attack_defense_info");
    if(infoEle)
    {
        if (infoEle->QueryAttribute("avy_prepare_time", &m_stInfo.m_iAvyPrepareTime) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found team_war/attack_defense_info/avy_prepare_time attr");
        }

        if(m_stInfo.m_iAvyPrepareTime < 300)
        {
            return std::make_pair(false, "team_war/attack_defense_info/avy_prepare_time < 300");
        }

        if (infoEle->QueryAttribute("avy_prepare_end_begin", &m_stInfo.m_bAvyPrepareEndBegin) != XML_SUCCESS)
        {
            return std::make_pair(false, "not found team_war/attack_defense_info/avy_prepare_end_begin attr");
        }
    }

	return std::make_pair(true, "OK");
}

std::pair<bool, std::string> CPushoverConfigParser::Load(const std::string & strXMLContent)
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

const pushover& CPushoverConfigParser::GetPushoverConfig(const std::string& id) const
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

std::pair<bool, std::string> CCrossConfigGuildParser::Load(const std::string& strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if (strXMLContent.empty())
        {
            strErrorInfo << "MSG:guild XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if (!stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        if (!stXML.FindElem("root"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        if (!stXML.FindElem("guild_info"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        std::string strStartDate;
        strStartDate = stXML.GetAttrib("opening_para1");
        if (!strStartDate.empty())
        {
            struct tm stTM = {};
            strptime(strStartDate.c_str(), "%Y-%m-%d %T", &stTM);
            m_iGuildWarStartTime = (int)mktime(&stTM);
        }

        std::string strEndDate;
        strEndDate = stXML.GetAttrib("opening_para2");
        if (!strEndDate.empty())
        {
            struct tm stTM = {};
            strptime(strEndDate.c_str(), "%Y-%m-%d %T", &stTM);
            m_iGuildWarEndTime = (int)mktime(&stTM);
        }

        m_iRankID = atoi(stXML.GetAttrib("rank_id").c_str());

        m_iChangeNameCD = atoi(stXML.GetAttrib("change_name_cd").c_str());
        if(m_iChangeNameCD < 3600) m_iChangeNameCD = 3600;
        m_iChangeNameCost = atoi(stXML.GetAttrib("change_name_cost").c_str());

        m_iChiefOffLineDay = atoi(stXML.GetAttrib("chief_offline_day").c_str());
        if(m_iChiefOffLineDay <= 0) m_iChiefOffLineDay = 3;

        if (!stXML.FindElem("guild_level"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("level"))
        {
            TCrossGuildLevelElem stConfig;
            stConfig.m_iLevel = atoi(stXML.GetAttrib("level").c_str());
            stConfig.m_iTotalExp = atoi(stXML.GetAttrib("nextexp").c_str());
            stConfig.m_iMember = atoi(stXML.GetAttrib("member").c_str());
            stConfig.m_iAdminCount = atoi(stXML.GetAttrib("admin_count").c_str());

            m_stLevelConfig[stConfig.m_iTotalExp] = stConfig;
        }
        stXML.OutOfElem();

        if(m_stLevelConfig.empty())
        {
            strErrorInfo << "MSG: m_stLevelConfig.empty()" << std::endl;
            throw std::exception();
        }

        if (!stXML.FindElem("guild_log"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("log"))
        {
            int iType = atoi(stXML.GetAttrib("type").c_str());
            auto stFmt = CFormatString::Compile(stXML.GetAttrib("content"));

            m_stLogMap[iType] = stFmt;
        }
        stXML.OutOfElem();

        if (!stXML.FindElem("guild_donate"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("donate_type"))
        {
            TLogicGuildDonateElem donateElem;
            donateElem.m_iDonateType = atoi(stXML.GetAttrib("type").c_str());
            donateElem.m_iGuildDonate = atoi(stXML.GetAttrib("guild_donate").c_str());
            donateElem.m_iPrivateDonate = atoi(stXML.GetAttrib("private_donate").c_str());
            donateElem.m_iGuildExp = atoi(stXML.GetAttrib("guild_exp").c_str());
            m_stGuildDonate[donateElem.m_iDonateType] = donateElem;
        }
        stXML.OutOfElem();

        if (!stXML.FindElem("guild_sign"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("sign_reward"))
        {
            TLogicGuildSignElem signElem;
            signElem.m_iLevel = atoi(stXML.GetAttrib("level").c_str());
            signElem.m_iNeedLevel = atoi(stXML.GetAttrib("need_guild_level").c_str());
            signElem.m_iCostGuildDonate = atoi(stXML.GetAttrib("cost_guild_donate").c_str());
            m_stGuildSign[signElem.m_iLevel] = signElem;
        }
        stXML.OutOfElem();

        if (!stXML.FindElem("guild_decorate"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("decorate_item"))
        {
            TLogicGuildDecorate decorateElem;
            decorateElem.m_iIndex = atoi(stXML.GetAttrib("index").c_str());
            decorateElem.m_iCostGuildDonate = atoi(stXML.GetAttrib("cost_guild_donate").c_str());
            m_stGuildDecorate[decorateElem.m_iIndex] = decorateElem;
        }
        stXML.OutOfElem();

        if (!stXML.FindElem("guild_collection"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("collection_item"))
        {
            TLogicGuildCollection collectionElem;
            collectionElem.m_iItemID = atoi(stXML.GetAttrib("id").c_str());
            collectionElem.m_iCostGuildDonate = atoi(stXML.GetAttrib("cost_guild_donate").c_str());
            collectionElem.m_iUnlockLevel = atoi(stXML.GetAttrib("lock_guild_level").c_str());
            collectionElem.m_iGuildBuffID = atoi(stXML.GetAttrib("guild_buff_id").c_str());
            m_stGuildCollection[collectionElem.m_iItemID] = collectionElem;
        }
        stXML.OutOfElem();


        if (!stXML.FindElem("guild_home_init"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("homeType"))
        {
            TLogicGuildDefaultTheme defaultTheme;
            defaultTheme.m_iType = atoi(stXML.GetAttrib("type").c_str());
            defaultTheme.m_iDefaultInit = atoi(stXML.GetAttrib("default_theme").c_str());
            m_stGuildDefaultTheme[defaultTheme.m_iType] = defaultTheme;
        }
        stXML.OutOfElem();

        //公会战
        if (!stXML.FindElem("guild_war_reward"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }


        if (!stXML.FindElem("guild_war_level"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("level_id"))
        {
            TLogicGuildWarLevel guildWarLevel;
            guildWarLevel.m_id = atoi(stXML.GetAttrib("id").c_str());
            std::string strLevelList = stXML.GetAttrib("next_level");
            std::string strBossHpList = stXML.GetAttrib("boss_hp");
            auto stNextLevelVec = string_split(strLevelList, "|");
            auto stBossHpVec = string_split(strBossHpList, "|");
            for(std::size_t i = 0; i < stNextLevelVec.size() && i < stBossHpVec.size(); ++i)
            {
                TLogicGuildWarInfo info;
                int nextLevel = ::atoi(stNextLevelVec[i].c_str());
                int bossHp = ::atoi(stBossHpVec[i].c_str());
                info.m_iBossHp = bossHp;
                guildWarLevel.m_stGuildWarInfo[nextLevel] = info;
            }
            m_stGuildWarLevel[guildWarLevel.m_id] = guildWarLevel;
        }
        stXML.OutOfElem();

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TCrossGuildLevelElem* CCrossConfigGuildParser::GetLevelConfig(int32_t iTotalExp) const
{
    auto iter = m_stLevelConfig.upper_bound(iTotalExp);
    if(iter == m_stLevelConfig.end()) --iter;

    return &(iter->second);
}

int32_t CCrossConfigGuildParser::GetGuildLevel(int32_t iTotalExp) const
{
    auto iter = m_stLevelConfig.upper_bound(iTotalExp);
    if(iter == m_stLevelConfig.end()) --iter;

    return iter->second.m_iLevel;
}

const TLogicGuildDonateElem* CCrossConfigGuildParser::GetDonateConfig(int32_t donateType) const
{
    auto iter = m_stGuildDonate.find(donateType);
    return iter == m_stGuildDonate.end() ? nullptr : &(iter->second);
}

const TLogicGuildSignElem* CCrossConfigGuildParser::GetGuildSignConfig(int32_t signLevel) const
{
    auto iter = m_stGuildSign.find(signLevel);
    return iter == m_stGuildSign.end() ? nullptr : &(iter->second);
}

const TLogicGuildDecorate* CCrossConfigGuildParser::GetGuildDecorateConfig(int32_t index) const
{
    auto iter = m_stGuildDecorate.find(index);
    return iter == m_stGuildDecorate.end() ? nullptr : &(iter->second);
}

const TLogicGuildCollection* CCrossConfigGuildParser::GetGuildCollectionConfig(int32_t index) const
{
    auto iter = m_stGuildCollection.find(index);
    return iter == m_stGuildCollection.end() ? nullptr : &(iter->second);
}

const TLogicGuildDefaultTheme* CCrossConfigGuildParser::GetGuildDefaultThemeConfig(int32_t index) const
{
    auto iter = m_stGuildDefaultTheme.find(index);
    return iter == m_stGuildDefaultTheme.end() ? nullptr : &(iter->second);
}


int32_t CCrossConfigGuildParser::GetMaxAdminCount(int32_t iTotalExp) const
{
    auto iter = m_stLevelConfig.upper_bound(iTotalExp);
    if(iter == m_stLevelConfig.end()) --iter;

    return iter->second.m_iAdminCount;
}

int32_t CCrossConfigGuildParser::GetMaxMemberCount(int32_t iTotalExp) const
{
    int32_t iTotalExpConfig = 0;
    for(auto &levelConfig : m_stLevelConfig)
    {
        iTotalExpConfig += levelConfig.second.m_iTotalExp;
        if (iTotalExp < iTotalExpConfig)
        {
            return levelConfig.second.m_iMember;
        }
    }

    return (--m_stLevelConfig.end())->second.m_iMember;
}

std::pair<bool, std::string> CCrossConfigDataReportParser::Load(const std::string& strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if (true == strXMLContent.empty())
        {
            strErrorInfo << "MSG:guild XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if (false == stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        if (false == stXML.FindElem("root"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        if (false == stXML.FindElem("info"))
        {
            strErrorInfo << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }
        m_strAppID = stXML.GetAttrib("app_id");
        m_strURL = stXML.GetAttrib("data_report_url");
        m_iOpen = ::atoi(stXML.GetAttrib("is_open").c_str()) > 0;
        m_hour = ::atoi(stXML.GetAttrib("record_hour").c_str());
        m_min = ::atoi(stXML.GetAttrib("record_min").c_str());
        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

