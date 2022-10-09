#include "logic_config_game.h"
#include "logic_config.h"
#include "logic_link_common.h"
#include "Markup.h"
#include "common_datetime.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigSystemParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	do
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			break;
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		if (false == stXML.FindElem("root"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		if (false == stXML.FindElem("service"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		m_stServerConfig.m_iServerID = atoi(stXML.GetAttrib("id").c_str());
		if (m_stServerConfig.m_iServerID <= 0)
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "service id should Be more than zero, now is " << m_stServerConfig.m_iServerID << std::endl;
			break;
		}

		m_stServerConfig.m_strPlatform = stXML.GetAttrib("platform");
		m_stServerConfig.m_strHostname = stXML.GetAttrib("hostname");
		m_stServerConfig.m_strHost = stXML.GetAttrib("host");
		m_stServerConfig.m_iListenPort = atoi(stXML.GetAttrib("listen_port").c_str());
		m_stServerConfig.m_iAdminListenPort = atoi(stXML.GetAttrib("admin_listen_port").c_str());
        m_stServerConfig.m_iLogLevel = atoi(stXML.GetAttrib("log_level").c_str());
        m_stServerConfig.m_iBinLogLevel = atoi(stXML.GetAttrib("bin_log_level").c_str());
        m_stServerConfig.m_iDebugTimeOffset = atoi(stXML.GetAttrib("debug_time_offset").c_str());
        m_stServerConfig.m_iLoginPushover = atoi(stXML.GetAttrib("").c_str());
        m_stServerConfig.m_iOpenTime = atoi(stXML.GetAttrib("open_time").c_str());
        m_stServerConfig.m_iMaintainBeginTime = atoi(stXML.GetAttrib("maintain_begin_time").c_str());
		m_stServerConfig.m_iMaintainEndTime = atoi(stXML.GetAttrib("maintain_end_time").c_str());
		m_stServerConfig.m_iMergeTime = atoi(stXML.GetAttrib("merge_time").c_str());
		m_stServerConfig.m_iMergeCount = atoi(stXML.GetAttrib("merge_count").c_str());

		if (m_stServerConfig.m_iOpenTime <= 0)
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "open server time is " << m_stServerConfig.m_iOpenTime << " less than zero" << std::endl;
			break;
		}

		stXML.IntoElem();
		while (stXML.FindElem("group"))
		{
			const int id = atoi(stXML.GetAttrib("id").c_str());
			TLogicSoGroupConfig group;
            group.m_iShowID = atoi(stXML.GetAttrib("show_id").c_str());
			group.m_strName = stXML.GetAttrib("name");
			m_stServerConfig.m_stMergedGroupMap[id] = std::move(group);
		}
		stXML.OutOfElem();

		if (false == stXML.FindElem("router"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		if (false == stXML.FindElem("connector"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }

        m_stRouterConfig.m_stConnector.m_strHost = stXML.GetAttrib("host").c_str();
        m_stRouterConfig.m_stConnector.m_iListenPort = atoi(stXML.GetAttrib("listen_port").c_str());
        m_stRouterConfig.m_stConnector.m_iSendPort = atoi(stXML.GetAttrib("send_port").c_str());
        m_stRouterConfig.m_stConnector.m_iLogicSendPort = atoi(stXML.GetAttrib("logic_send_port").c_str());

        if (false == stXML.FindElem("cross"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }

        m_stRouterConfig.m_stCross.m_strHost = stXML.GetAttrib("host").c_str();
        m_stRouterConfig.m_stCross.m_iListenPort = atoi(stXML.GetAttrib("listen_port").c_str());
        m_stRouterConfig.m_stCross.m_iSendPort = atoi(stXML.GetAttrib("send_port").c_str());
        m_stRouterConfig.m_stCross.m_iLogicSendPort = atoi(stXML.GetAttrib("logic_send_port").c_str());

        if (false == stXML.FindElem("queue"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }

        m_stRouterConfig.m_stQueue.m_strHost = stXML.GetAttrib("host").c_str();
        m_stRouterConfig.m_stQueue.m_iListenPort = atoi(stXML.GetAttrib("listen_port").c_str());
        m_stRouterConfig.m_stQueue.m_iSendPort = atoi(stXML.GetAttrib("send_port").c_str());
        m_stRouterConfig.m_stQueue.m_iLogicSendPort = atoi(stXML.GetAttrib("logic_send_port").c_str());

        stXML.OutOfElem();


		if (false == stXML.FindElem("mysql"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		if (false == stXML.FindElem("game"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		m_stMysqlConfig.m_stGameDB._host = stXML.GetAttrib("host").c_str();
		m_stMysqlConfig.m_stGameDB._port = atoi(stXML.GetAttrib("port").c_str());
		m_stMysqlConfig.m_stGameDB._user = stXML.GetAttrib("user");
		m_stMysqlConfig.m_stGameDB._password = stXML.GetAttrib("password");
		m_stMysqlConfig.m_stGameDB._database = stXML.GetAttrib("database");
		m_stMysqlConfig.m_stGameDB._charset = "utf8";
		m_stMysqlConfig.m_stGameDB._flag = 0;

        if (false == stXML.FindElem("stat"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }

        m_stMysqlConfig.m_stStatDB._host = stXML.GetAttrib("host").c_str();
        m_stMysqlConfig.m_stStatDB._port = atoi(stXML.GetAttrib("port").c_str());
        m_stMysqlConfig.m_stStatDB._user = stXML.GetAttrib("user");
        m_stMysqlConfig.m_stStatDB._password = stXML.GetAttrib("password");
        m_stMysqlConfig.m_stStatDB._database = stXML.GetAttrib("database");
        m_stMysqlConfig.m_stStatDB._charset = "utf8";
        m_stMysqlConfig.m_stStatDB._flag = 0;

        stXML.OutOfElem();

	} while (0);

	if (strErrorInfo.str().empty())
	{
		return (std::pair<bool, std::string>(true, "OK"));
	}
	else
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
}

bool CLogicConfigSystemParser::isSelfGroup(int iGroupID) const
{
	return m_stServerConfig.m_stMergedGroupMap.find(iGroupID) != m_stServerConfig.m_stMergedGroupMap.end();
}

std::string CLogicConfigSystemParser::GetGroupName(int32_t iGroupID) const
{
	const auto iter = m_stServerConfig.m_stMergedGroupMap.find(iGroupID);
	return iter == m_stServerConfig.m_stMergedGroupMap.end() ? "" : iter->second.m_strName;
}

int32_t CLogicConfigSystemParser::GetGroupShowID(int32_t iGroupID) const
{
	const auto iter = m_stServerConfig.m_stMergedGroupMap.find(iGroupID);
	return iter == m_stServerConfig.m_stMergedGroupMap.end() ? 0 : iter->second.m_iShowID;
}

bool CLogicConfigSystemParser::IsPrintPackInfo() const
{
	return m_stServerConfig.m_iBinLogLevel > 0;
}

int32_t CLogicConfigSystemParser::GetOpenSvrTime() const
{
    if (m_stServerConfig.m_iOpenTime < CLogicConfig::GetGlobalConfig().m_iActivitySeparateOpenTime)
    {
        return m_stServerConfig.m_iOpenTime - CLogicConfig::GetGlobalConfig().m_iActivitySeparateOpenAddDay * 1_day;
    }

	return m_stServerConfig.m_iOpenTime;
}

int32_t CLogicConfigSystemParser::GetOpenSvrDay() const
{
	return day_diff(GetOpenSvrTime(), CLogicCommon::GetSec()) + 1;
}

bool CLogicConfigSystemParser::isServerMaintain() const
{
	return m_stServerConfig.m_iMaintainBeginTime < CLogicCommon::GetSec() && m_stServerConfig.m_iMaintainEndTime > CLogicCommon::GetSec();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigGameGlobalParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	do
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			break;
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		if (false == stXML.FindElem("config_list"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		if (false == stXML.FindElem("config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		m_iMaxEnergy = atoi(stXML.GetAttrib("energy").c_str());
		m_iNumSecondByAutoIncreaseOneEnergy = atoi(stXML.GetAttrib("energy_speed").c_str());
        m_iEnergyStorePct = atoi(stXML.GetAttrib("energy_store_pct").c_str());
        m_iEnergyStoreMax = atoi(stXML.GetAttrib("energy_store_max").c_str());
		if (m_iMaxEnergy <= 0 || m_iNumSecondByAutoIncreaseOneEnergy <= 0)
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "energy <= 0 or energy_speed <= 0" << std::endl;
			break;
		}

        if(m_iEnergyStorePct <= 0 || m_iEnergyStoreMax < 0)
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "energy_store_pct <= 0 or energy_store_max < 0" << std::endl;
            break;
        }

		m_iMaxVitality = atoi(stXML.GetAttrib("vitality").c_str());
		m_iNumSecondByAutoIncreaseOneVitality = atoi(stXML.GetAttrib("vitality_speed").c_str());
		if (m_iMaxVitality <= 0 || m_iNumSecondByAutoIncreaseOneVitality <= 0)
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "vitality <=0 or vitality_speed <= 0" << std::endl;
			break;
		}

		m_iChangeNickYuanbao = atoi(stXML.GetAttrib("change_nick_yuanbao").c_str());
		m_strChangeNickStr = stXML.GetAttrib("change_nick_str");
		m_iMaxNickNameCharWidth = atoi(stXML.GetAttrib("max_nick_name_show_width").c_str());
		m_iMinNickNameCharWidth = atoi(stXML.GetAttrib("min_nick_name_show_width").c_str());
		if (m_iMinNickNameCharWidth >= m_iMaxNickNameCharWidth || m_strChangeNickStr.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "min_nick_name_show_width must less than max_nick_name_show_width or change_nick_str is empty" << std::endl;
			break;
		}

		m_iSaoDangLevelPrice = atoi(stXML.GetAttrib("saodang_price").c_str());;
		m_iOpenBoxGetCoinNum = atoi(stXML.GetAttrib("open_box_gold").c_str());;
		m_iKillMonsterGetCoinNum = atoi(stXML.GetAttrib("monster_drop_gold").c_str());;
		m_iKillBossGetCoinNum = atoi(stXML.GetAttrib("boss_drop_gold").c_str());;

		m_iSweepLevelLimit = atoi(stXML.GetAttrib("sweep_level_limit").c_str());

		m_iSettlementBonusGold = atoi(stXML.GetAttrib("bonus_gold").c_str());
		m_iBuyRefreshTokenCostYuanbao = atoi(stXML.GetAttrib("refresh_token_cost_yuanbao").c_str());
		if (m_iSettlementBonusGold <= 0 || m_iBuyRefreshTokenCostYuanbao <= 0)
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "one of [bonus_gold, refresh_token_cost_yuanbao] <= 0" << std::endl;
			break;
		}

		if (false == stXML.FindElem("ranklist"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND ranklist TAG" << std::endl;
			break;
		}

		m_iRanklistAwardTime = atoi(stXML.GetAttrib("award_time").c_str());

		if (false == stXML.FindElem("talk"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND talk TAG" << std::endl;
			break;
		}

		m_iTalkWordLimit = atoi(stXML.GetAttrib("talk_word_limit").c_str());
		m_iTalkLevelLimit = atoi(stXML.GetAttrib("talk_level_limit").c_str());
		m_iBanIPTalkLevelLimit = atoi(stXML.GetAttrib("ban_ip_talk_level_limit").c_str());
		m_iMaxTalkInterval = atoi(stXML.GetAttrib("max_talk_interval").c_str());

		if (m_iBanIPTalkLevelLimit <= 0)
		{
			m_iBanIPTalkLevelLimit = 60;
		}

		if (false == stXML.FindElem("activity"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND activity TAG" << std::endl;
			break;
		}
		if (!try_time_from_string(stXML.GetAttrib("separate_open_time"), m_iActivitySeparateOpenTime))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:separate_open_time format error" << std::endl;
            break;
        }
		m_iActivitySeparateOpenAddDay = atoi(stXML.GetAttrib("separate_open_add_day").c_str());

        if (false == stXML.FindElem("other"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND other TAG" << std::endl;
            break;
        }
        m_iTeamPresetNum = atoi(stXML.GetAttrib("team_preset_num").c_str());

	} while (0);

	if (strErrorInfo.str().empty())
	{
		return (std::pair<bool, std::string>(true, "OK"));
	}
	else
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigZooLogicPlatformParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	do
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			break;
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		if (false == stXML.FindElem("root"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

        if (false == stXML.FindElem("platform"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }
        if (!try_str2num(stXML.GetAttrib("pay_cash_multiple"), m_dPayCashMultiple))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "pay_cash_multiple error!" << std::endl;
            break;
        }

        if (false == stXML.FindElem("channels"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }

        stXML.IntoElem();

        m_stChannelMap.clear();
        while (stXML.FindElem("channel"))
        {
            const int32_t id = atoi(stXML.GetAttrib("id").c_str());

            TLogicChannelConfig stElem;
            stElem.m_iH5PaySwitch = atoi(stXML.GetAttrib("h5pay_switch").c_str());
            stElem.m_strH5PayLink = stXML.GetAttrib("h5pay_link");
            stElem.m_iThreePaySwitch = atoi(stXML.GetAttrib("three_pay_switch").c_str());
            stElem.m_strThreePayLink = stXML.GetAttrib("three_pay_link");
            stElem.m_strEvaluateUrl = stXML.GetAttrib("evaluate_url");

            m_stChannelMap[id] = std::move(stElem);
        }
        stXML.OutOfElem();

		if (false == stXML.FindElem("bulletins"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}
		stXML.IntoElem();

		m_stBulletins.clear();
		while (stXML.FindElem("bulletin"))
		{
			TLogicBulletinConfig stConfig;
			stConfig.m_iID = atoi(stXML.GetAttrib("id").c_str());
			stConfig.m_iBeginTime = atoi(stXML.GetAttrib("start_time").c_str());
			stConfig.m_iEndTime = atoi(stXML.GetAttrib("end_time").c_str());
			stConfig.m_iIntervals = atoi(stXML.GetAttrib("intervals").c_str());
			stConfig.m_strContent = stXML.GetAttrib("content");

			stXML.IntoElem();

			while (stXML.FindElem("server_id"))
            {
                stConfig.m_iGroups.insert(atoi(stXML.GetData().c_str()));
            }

            while (stXML.FindElem("channel_id"))
            {
                stConfig.m_iChannels.insert(atoi(stXML.GetData().c_str()));
            }

			stXML.OutOfElem();

			m_stBulletins.push_back(stConfig);
		}
		stXML.OutOfElem();

		if (false == stXML.FindElem("white_account"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		m_stWhiteAccountSet.clear();
		while (stXML.FindElem("account"))
		{
			m_stWhiteAccountSet.insert(stXML.GetData());
		}
		stXML.OutOfElem();

        if (false == stXML.FindElem("black_account"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }

        stXML.IntoElem();

        m_stBlackAccountSet.clear();
        while (stXML.FindElem("account"))
        {
            m_stBlackAccountSet.insert(stXML.GetData());
        }
        stXML.OutOfElem();

		if (false == stXML.FindElem("banned_ip"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		m_stBannedIpSet.clear();
		while (stXML.FindElem("ip"))
		{
            m_stBannedIpSet.insert(stXML.GetData());
		}
		stXML.OutOfElem();

	} while (0);

	if (strErrorInfo.str().empty())
	{
		return (std::pair<bool, std::string>(true, "OK"));
	}
	else
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
}

bool CLogicConfigZooLogicPlatformParser::isWhiteAccount(const std::string& account) const
{
	return m_stWhiteAccountSet.find(account) != m_stWhiteAccountSet.end();
}

const TLogicChannelConfig& CLogicConfigZooLogicPlatformParser::GetChannelConfig(int32_t iChannelID) const
{
    static TLogicChannelConfig default_config;
	const auto iter = m_stChannelMap.find(iChannelID);
	return iter == m_stChannelMap.end() ? default_config : iter->second;
}

bool CLogicConfigZooLogicPlatformParser::isBannedIp(const std::string &ip) const
{
	return m_stBannedIpSet.find(ip) != m_stBannedIpSet.end();
}

////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigCalcParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	m_stCalcConfig = TLogicCalcElem();

	do
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			break;
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		if (false == stXML.FindElem("calculator"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		if (false == stXML.FindElem("ratio"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << "|:FIRST RATIO NOT FOUND" << std::endl;
			break;
		}

		m_stCalcConfig.m_fHPFightPowerRatio = ::atof(stXML.GetAttrib("hp_fight_power_ratio").c_str());
		m_stCalcConfig.m_fATKFightPowerRatio = ::atof(stXML.GetAttrib("atk_fight_power_ratio").c_str());
		m_stCalcConfig.m_fDEFFightPowerRatio = ::atof(stXML.GetAttrib("def_fight_power_ratio").c_str());
		m_stCalcConfig.m_fCRIFightPowerRatio = ::atof(stXML.GetAttrib("cri_fight_power_ratio").c_str());
        m_stCalcConfig.m_fCritDamageFightPowerRatio = ::atof(stXML.GetAttrib("crit_damage_fight_power_ratio").c_str());
        m_stCalcConfig.m_fAutoSkillFightPowerRatio = ::atof(stXML.GetAttrib("auto_skill_fight_power_ratio").c_str());
        m_stCalcConfig.m_fUniqueSkillFightPowerRatio = ::atof(stXML.GetAttrib("unique_skill_fight_power_ratio").c_str());
        m_stCalcConfig.m_fPassiveSkillFightPowerRatio = ::atof(stXML.GetAttrib("passive_skill_fight_power_ratio").c_str());

		m_stCalcConfig.m_fWushenHPFightPowerRatio = ::atof(stXML.GetAttrib("wushen_hp_fight_power_ratio").c_str());
		m_stCalcConfig.m_fWushenATKFightPowerRatio = ::atof(stXML.GetAttrib("wushen_atk_fight_power_ratio").c_str());

		m_stCalcConfig.m_fAttribute1FightPowerRatio = ::atof(stXML.GetAttrib("attribute1_fight_power_ratio").c_str());
		m_stCalcConfig.m_fAttribute2FightPowerRatio = ::atof(stXML.GetAttrib("attribute2_fight_power_ratio").c_str());
		m_stCalcConfig.m_fAttribute3FightPowerRatio = ::atof(stXML.GetAttrib("attribute3_fight_power_ratio").c_str());
		m_stCalcConfig.m_fAttribute4FightPowerRatio = ::atof(stXML.GetAttrib("attribute4_fight_power_ratio").c_str());
		m_stCalcConfig.m_fAttribute5FightPowerRatio = ::atof(stXML.GetAttrib("attribute5_fight_power_ratio").c_str());
		m_stCalcConfig.m_fAttribute6FightPowerRatio = ::atof(stXML.GetAttrib("attribute6_fight_power_ratio").c_str());

		m_stCalcConfig.m_fSkill1LevelFightPowerRatio = ::atof(stXML.GetAttrib("skill1_level_fight_power_ratio").c_str());
		m_stCalcConfig.m_fSkill2LevelFightPowerRatio = ::atof(stXML.GetAttrib("skill2_level_fight_power_ratio").c_str());
		m_stCalcConfig.m_fSkill3LevelFightPowerRatio = ::atof(stXML.GetAttrib("skill3_level_fight_power_ratio").c_str());
		m_stCalcConfig.m_fSkill4LevelFightPowerRatio = ::atof(stXML.GetAttrib("skill4_level_fight_power_ratio").c_str());

		if (false == stXML.FindElem("ratio"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << "|:ITEM SCORE RATIO NOT FOUND " << std::endl;
			break;
		}

		m_stCalcConfig.m_fGoldScoreRatio = ::atof(stXML.GetAttrib("gold_ratio").c_str());
		m_stCalcConfig.m_iPropsDragonScore = ::atoi(stXML.GetAttrib("item_dragon_value").c_str());
		m_stCalcConfig.m_iPropsQTEScore = ::atoi(stXML.GetAttrib("item_qte_value").c_str());
		m_stCalcConfig.m_iPropsMagnetScore = ::atoi(stXML.GetAttrib("item_magnet_value").c_str());
		m_stCalcConfig.m_iPropsSkillScore = ::atoi(stXML.GetAttrib("item_skill_value").c_str());
		m_stCalcConfig.m_iPropsRushScore = ::atoi(stXML.GetAttrib("item_rush_value").c_str());

		while (stXML.FindElem("tag"))
		{
			if (stXML.GetAttrib("tag").compare("skillpower") == 0)
			{
				stXML.IntoElem();

				while (stXML.FindElem("ratio"))
				{
					int32_t iSkillID = ::atoi(stXML.GetAttrib("skillid").c_str());
					int32_t iPower = ::atoi(stXML.GetAttrib("fightpower").c_str());
					m_aiSkillPower[iSkillID] = iPower;
				}

				stXML.OutOfElem();
			}
		}

	} while (0);

	if (strErrorInfo.str().empty())
	{
		return (std::pair<bool, std::string>(true, "OK"));
	}
	else
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
}

int32_t CLogicConfigCalcParser::GetSkillPower(int32_t iSkillID) const
{
	const auto iter = m_aiSkillPower.find(iSkillID);
	return iter == m_aiSkillPower.end() ? 0 : iter->second;
}

///////////////////////////////////////////////////////////////////////////////////////////////

std::pair<bool, std::string>
CLogicConfigDirtyWordParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("dirty_word"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("word"))
		{
			std::string strDirtyWord = stXML.GetData();
			if (strDirtyWord.empty() == false)
			{
				m_stDirtyWordList.push_back(strDirtyWord);
			}
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string>
CLogicConfigDirtyWordChatParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if (true == strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if (false == stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        if (false == stXML.FindElem("dirty_word"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        while (stXML.FindElem("word"))
        {
            std::string strDirtyWord = stXML.GetData();
            if (strDirtyWord.empty() == false)
            {
                m_stDirtyWordList.push_back(strDirtyWord);
            }
        }
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
    return (std::pair<bool, std::string>(true, "OK"));
}


std::pair<bool, std::string>
CLogicConfigDepositParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

        if (false == stXML.FindElem("root"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }
        stXML.IntoElem();

		if (false == stXML.FindElem("deposit_list"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		std::map<int32_t, std::string> product_price;

		while (stXML.FindElem("deposit"))
		{
			TLogicDepositConfigElem stElem;

			int32_t iDepositID = atoi(stXML.GetAttrib("deposit_id").c_str());

			stElem.m_iProductID = atoi(stXML.GetAttrib("product_id").c_str());
            stElem.m_iType = atoi(stXML.GetAttrib("type").c_str());
            stElem.m_strName = stXML.GetAttrib("name");
            stElem.m_iShowLevel = atoi(stXML.GetAttrib("show_level").c_str());
            stElem.m_iShowMaxLevel = atoi(stXML.GetAttrib("show_max_level").c_str());
            if(stElem.m_iShowMaxLevel <= 0) stElem.m_iShowMaxLevel = 999;

            stElem.m_iVipLvMin = atoi(stXML.GetAttrib("vip_lv_min").c_str());
            stElem.m_iVipLvMax = atoi(stXML.GetAttrib("vip_lv_max").c_str());
            if(stElem.m_iVipLvMax <= 0) stElem.m_iVipLvMax = 999;

            stElem.m_stConsumeItem.m_iItemType = atoi(stXML.GetAttrib("consume_item_type").c_str());
            stElem.m_stConsumeItem.m_iCardID = atoi(stXML.GetAttrib("consume_item_id").c_str());
            stElem.m_stConsumeItem.m_iNum = atoi(stXML.GetAttrib("consume_item_num").c_str());

            stElem.m_iNotify = atoi(stXML.GetAttrib("notify").c_str());
			stElem.m_iMaxBuyCount = atoi(stXML.GetAttrib("max_buy_count").c_str());
			stElem.m_iDailyBuyCount = atoi(stXML.GetAttrib("daily_buy_count").c_str());
			stElem.m_iWeeklyBuyCount = atoi(stXML.GetAttrib("weekly_buy_count").c_str());
			stElem.m_iMonthBuyCount = atoi(stXML.GetAttrib("month_buy_count").c_str());
			stElem.m_iDayIntervalBuyCount = atoi(stXML.GetAttrib("day_interval_buy_count").c_str());
			stElem.m_iDayIntervalNum = atoi(stXML.GetAttrib("day_interval_num").c_str());

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("first_reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("first_reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("first_reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("first_reward_num" + stStream.str()).c_str());
                stElem.m_stFirstReward.emplace_back(stItem);
            }

			stElem.m_iNormalBuyAmount = atoi(stXML.GetAttrib("normal_amount").c_str());
			stElem.m_iBuyItemType = atoi(stXML.GetAttrib("buy_item_type").c_str());
			stElem.m_iBuyItemID = atoi(stXML.GetAttrib("buy_item_id").c_str());
            if(stElem.m_iBuyItemType > 0)
            {
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stElem.m_iBuyItemType, stElem.m_iBuyItemID), iDepositID)
            }

			stElem.m_iVipExp = atoi(stXML.GetAttrib("vip_exp").c_str());
			stElem.m_strPrice = stXML.GetAttrib("price");
            stElem.m_iAvyID = atoi(stXML.GetAttrib("avy_id").c_str());

			if (!try_str2num(stElem.m_strPrice, stElem.m_dPrice))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:PRICE ERROR|CAN'T TO DOUBLE:" << stElem.m_strPrice << std::endl;
				throw std::exception();
			}

			auto iterProductPrice = product_price.find(stElem.m_iProductID);
			if (iterProductPrice == product_price.end())
            {
			    product_price.emplace(stElem.m_iProductID, stElem.m_strPrice);
            }
			else if (iterProductPrice->second != stElem.m_strPrice)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:PRICE ERROR|PRODUCT ID:" << stElem.m_iProductID << "|DEPOSIT ID:" << iDepositID
                    << "HAS TWO PRICE, ERROR! OLD PRICE:" << iterProductPrice->second << "|CURRENT PRICE:" << stElem.m_strPrice << std::endl;
                throw std::exception();
            }

			if (stElem.m_iType <= CLogicConfigDefine::DEPOSIT_TYPE_BEGIN || stElem.m_iType >= CLogicConfigDefine::DEPOSIT_TYPE_END)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "DEPOSIT TYPE:" << stElem.m_iType
                        << "|ID:" << iDepositID << " NOT FOUND!" << std::endl;
                throw std::exception();
            }

            /*if (stElem.m_iType != CLogicConfigDefine::DEPOSIT_TYPE_MONTH_CARD
                && stElem.m_iType != CLogicConfigDefine::DEPOSIT_TYPE_QUARTER_CARD
                && stElem.m_iType != CLogicConfigDefine::DEPOSIT_TYPE_INSURE_FUND)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "DEPOSIT TYPE:" << stElem.m_iType
                        << "|ID:" << iDepositID << " HAVE BENEFITS_CARD!" << std::endl;
                throw std::exception();
            }*/

			if (stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_DAILY_BUY && stElem.m_iDailyBuyCount <= 0)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "DAILY BUY DEPOSIT TYPE:" << stElem.m_iType
                        << "|ID:" << iDepositID << " NOT HAVE DAILY BUY COUNT!" << std::endl;
                throw std::exception();
            }

            if (stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_WEEKLY_BUY && stElem.m_iWeeklyBuyCount <= 0)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "WEEKLY BUY DEPOSIT TYPE:" << stElem.m_iType
                        << "|ID:" << iDepositID << " NOT HAVE WEEKLY BUY COUNT!" << std::endl;
                throw std::exception();
            }

            if(stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_MONTH_BUY && stElem.m_iMonthBuyCount <= 0)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "MONTH BUY DEPOSIT TYPE:" << stElem.m_iType
                             << "|ID:" << iDepositID << " NOT HAVE MONTH BUY COUNT!" << std::endl;
                throw std::exception();
            }

            if(stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_OTHER_DEPOSIT)
            {
                // 免费购买
                if(stElem.m_stConsumeItem.m_iItemType <= 0 &&
                    stElem.m_iMaxBuyCount <= 0 && stElem.m_iDailyBuyCount <= 0 &&
                    stElem.m_iWeeklyBuyCount <= 0 && stElem.m_iMonthBuyCount <= 0 &&
                    stElem.m_iDayIntervalBuyCount <= 0)
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "FREE DEPOSIT TYPE:" << stElem.m_iType
                                 << "|ID:" << iDepositID << " NOT HAVE ANY BUY COUNT!" << std::endl;
                    throw std::exception();
                }
            }

            if(stElem.m_iDayIntervalBuyCount > 0 && stElem.m_iDayIntervalNum <= 0)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "FREE DEPOSIT TYPE:" << stElem.m_iType
                             << "|ID:" << iDepositID << " day_interval_num <= 0" << std::endl;
                throw std::exception();
            }

            if (stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_FACEBOOK &&
                stElem.m_iMaxBuyCount <= 0 && stElem.m_iDailyBuyCount <= 0 && stElem.m_iWeeklyBuyCount <= 0 &&
                stElem.m_iMonthBuyCount <= 0 && stElem.m_iDayIntervalBuyCount <= 0)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "BUY DEPOSIT TYPE:" << stElem.m_iType
                             << "|ID:" << iDepositID << "  BUY COUNT is UNLIMITED!" << std::endl;
                throw std::exception();
            }

            if (stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_MERGE_GIFT
                || stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_IBUSY
                || stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_SCORE_LEVEL)
            {
                if (stElem.m_iMaxBuyCount != 1)
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "BUY DEPOSIT TYPE:" << stElem.m_iType
                            << "|ID:" << iDepositID << "  MAX BUY COUNT NOT 1!" << std::endl;
                    throw std::exception();
                }
            }

            if (stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_LIMITED_TIME
                || stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_TOTAL_BUY
                || stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_MERGE_GIFT
                || stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_THEME_CHARGE
                || stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_IBUSY
                || stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_SPECIAL_WEEK
                || stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_SPECIAL_MONTH
                || stElem.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_SCORE_LEVEL)
            {
                if (stElem.m_iMaxBuyCount <= 0)
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "BUY DEPOSIT TYPE:" << stElem.m_iType
                                 << "|ID:" << iDepositID << " NOT HAVE MAX BUY COUNT!" << std::endl;
                    throw std::exception();
                }
            }

            m_stDepositConfig[iDepositID] = stElem;
		}

        stXML.OutOfElem();

        if (false == stXML.FindElem("deposit_ratio"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("ratio"))
        {
            TLogicPayDiamondConfigElem stElem;
            stElem.m_iDepositID = atoi(stXML.GetAttrib("deposit_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(m_stDepositConfig.find(stElem.m_iDepositID) != m_stDepositConfig.end(),stElem.m_iDepositID)
            stElem.m_iDiamondNum = atoi(stXML.GetAttrib("diamond").c_str());
            stElem.m_iPriceNum = atoi(stXML.GetAttrib("price").c_str());
            CONFIG_ASSERT_EXCEPTION_EX((stElem.m_iDiamondNum > 0 && stElem.m_iPriceNum > 0),stElem.m_iDepositID)
            m_stPayDiamondConfig[ stElem.m_iDepositID] = stElem;
        }
        stXML.OutOfElem();


        if (false == stXML.FindElem("age_month_limit"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("age_limit_index"))
        {
            TLogicAgeLimitConfigElem stElem;
            stElem.m_iStartAge = atoi(stXML.GetAttrib("start_age").c_str());
            stElem.m_iEndAge = atoi(stXML.GetAttrib("end_age").c_str());
            stElem.m_iLimitNumber = atoi(stXML.GetAttrib("limit_add_up").c_str());

            int32_t index = atoi(stXML.GetAttrib("index").c_str());
            m_stAgeLimitConfig[index] = stElem;
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

const TLogicDepositConfigElem*
CLogicConfigDepositParser::GetDepositConfig(int iID) const
{
	auto stDepositIter = m_stDepositConfig.find(iID);
	if (stDepositIter == m_stDepositConfig.end())
	{
		return (nullptr);
	}

	return &(stDepositIter->second);
}

const TLogicAgeLimitConfigElem*
CLogicConfigDepositParser::GetAgeLimitConfig(int index) const
{
    auto stAgeLimitIter = m_stAgeLimitConfig.find(index);
    return  (stAgeLimitIter == m_stAgeLimitConfig.end()) ? nullptr : &(stAgeLimitIter->second);
}

const TLogicPayDiamondConfigElem*
CLogicConfigDepositParser::GetPayDiamondRatioConfig(int iDepositID)  const
{
    auto stPayDiamondRatioIter = m_stPayDiamondConfig.find(iDepositID);
    return  (stPayDiamondRatioIter == m_stPayDiamondConfig.end()) ? nullptr : &(stPayDiamondRatioIter->second);
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigBroadcastParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (!stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (!stXML.FindElem("broadcast_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();
		while (stXML.FindElem("broadcast"))
		{
			TLogicBroadcastConfig stElem;
			stElem.m_strBroadcastID = stXML.GetAttrib("id");
			stElem.m_iPara = atoi(stXML.GetAttrib("para").c_str());
			stElem.m_iCmd = atoi(stXML.GetAttrib("cmd").c_str());
			stElem.m_iTriggerMinValue = atoi(stXML.GetAttrib("trigger_min_value").c_str());
			stElem.m_iTriggerMaxValue = atoi(stXML.GetAttrib("trigger_max_value").c_str());
			stElem.m_iShowPriority = atoi(stXML.GetAttrib("show_priority").c_str());
			stElem.m_stBroadcastFmt = CFormatString::Compile(stXML.GetAttrib("content"));
			stElem.m_stTalkContentFmt = CFormatString::Compile(stXML.GetAttrib("talk_content"));
			stElem.m_iJump = atoi(stXML.GetAttrib("jump").c_str());

			m_stConfigMap.emplace(stElem.m_strBroadcastID, stElem);
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string>
CLogicConfigActivityBroadcastParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("broadcast_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();
		while (stXML.FindElem("broadcast"))
		{
			TLogicActivityBroadcastConfigElem stElem;
			stElem.m_iBroadcastID = std::atoi(stXML.GetAttrib("id").c_str());
			stElem.m_iShowPriority = atoi(stXML.GetAttrib("show_priority").c_str());
			stElem.m_iBeginTimeOffset = atoi(stXML.GetAttrib("beginTimeOffset").c_str());
			stElem.m_iInterval = atoi(stXML.GetAttrib("interval").c_str());
			stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());
			stElem.m_strContent = stXML.GetAttrib("content");			

			{
				TLogicBroadcastTimeRefElem &stActivityElem = stElem.m_stActivityTime;

				stActivityElem.m_iTimeType = atoi(stXML.GetAttrib("time_type").c_str());
				stActivityElem.m_iStartDay = atoi(stXML.GetAttrib("start_day").c_str());

				std::string strDate;
				strDate = stXML.GetAttrib("start_date");
				if (!strDate.empty())
				{
					struct tm stTM;
					if (strptime(strDate.c_str(), "%Y-%m-%d %T", &stTM) == nullptr)
					{
						strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT TIME ERROR|ACTIVE ID:" << stElem.m_iBroadcastID << std::endl;
						throw std::exception();
					}
					stActivityElem.m_iStartTime = (int)mktime(&stTM);
				}

				for (int32_t i = 1; i < 10; ++i)
				{
					if (!stXML.GetAttrib("start_week_day_" + std::to_string(i)).empty())
					{
						int32_t iStartWeekDay = 0, iStartHour = 0;
						iStartWeekDay = atoi(stXML.GetAttrib("start_week_day_" + std::to_string(i)).c_str());
						iStartHour = atoi(stXML.GetAttrib("start_hour_" + std::to_string(i)).c_str());
						stActivityElem.m_stOpenWeekDayVec.emplace_back(iStartWeekDay, iStartHour);
					}
				}
			}			

			m_stConfigMap.insert(std::make_pair(stElem.m_iBroadcastID, stElem));
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigFightReportParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("fight_report_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();
		while (stXML.FindElem("fight_report"))
		{
			TLogicFightReportConfigElem stElem;
			stElem.m_strFightReportID = stXML.GetAttrib("id");
			stElem.m_iTriggerValue = atoi(stXML.GetAttrib("trigger_value").c_str());

			std::string strContent = stXML.GetAttrib("content");
			stElem.m_stContentObj = CFormatString::Compile(strContent);

			m_stConfigMap.insert(decltype(m_stConfigMap)::value_type(stElem.m_strFightReportID, stElem));
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

int32_t CLogicConfigFightReportParser::GetMaxFightReportCount(int32_t iFightReportType) const
{
    static std::map<int32_t, int32_t> maxFightReportCount = {
            { CLogicConfigDefine::FIGHT_REPORT_DIANFENGJINGJI, 20 },
            { CLogicConfigDefine::FIGHT_REPORT_ENTIRE_WAR, 20 },
            { CLogicConfigDefine::FIGHT_REPORT_HOME, 20 },
            { CLogicConfigDefine::FIGHT_REPORT_FAIR_PVP, 20 },
            { CLogicConfigDefine::FIGHT_REPORT_GUILD_ATKDEF, 40 },
            { CLogicConfigDefine::FIGHT_REPORT_QIYU, 50 },
            { CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU, 50 },
            { CLogicConfigDefine::FIGHT_REPORT_MONTH_PASS, 30 },
    };
    const auto iter = maxFightReportCount.find(iFightReportType);
    return iter != maxFightReportCount.end() ? iter->second : 20 ;
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigMailParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("mail_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (stXML.FindElem("activity_mail"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("mail"))
			{
				TLogicMailElem stMail;
				stMail.m_strSender = stXML.GetAttrib("sender");
				stMail.m_strTitle = stXML.GetAttrib("title");
				stMail.m_stContent = CFormatString::Compile(stXML.GetAttrib("content"));

				CPackGameItem stGameItem;

				for (int i = 1; i < 5; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

					stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
					stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
					stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

					stMail.m_astItem.push_back(stGameItem);
				}

				const int32_t iCreateDay = atoi(stXML.GetAttrib("create_day").c_str());
				const int32_t iServerDay = atoi(stXML.GetAttrib("server_day").c_str());
				const int32_t iExpiredDay = atoi(stXML.GetAttrib("expired_day").c_str());
				const int32_t iLevel = atoi(stXML.GetAttrib("level").c_str());
				const int32_t iDateTime = get_time_from_string(stXML.GetAttrib("date_time"));
				const int32_t iExpiredTime = get_time_from_string(stXML.GetAttrib("expired_time"));
				const int32_t iActivityID = atoi(stXML.GetAttrib("activity_id").c_str());

				if (!stXML.GetAttrib("date_time").empty() && iDateTime <= 0)
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "date_time format error" << std::endl;
					throw std::exception();
				}

				if (!stXML.GetAttrib("expired_time").empty() && iExpiredTime <= 0)
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "expired_time format error" << std::endl;
					throw std::exception();
				}
				
				if (iCreateDay > 0)
				{
					TLogicCreateNpcMailElem elem;
					elem.m_iCreateDay = iCreateDay;
					elem.m_iExpiredDay = iExpiredDay ? iExpiredDay : std::numeric_limits<int32_t>::max();
					elem.m_iActivityID = iActivityID;
					elem.m_stMail = stMail;
					m_astCreateMail.emplace_back(elem);
				}
				else if (iServerDay > 0 && iDateTime <= 0)
				{
					TLogicServerNpcMailElem elem;
					elem.m_iServerDay = iServerDay;
					elem.m_iExpiredDay = iExpiredDay ? iExpiredDay : std::numeric_limits<int32_t>::max();
					elem.m_iActivityID = iActivityID;
					elem.m_stMail = stMail;
					m_astServerMail.emplace_back(elem);
				}
				else if (iLevel > 0)
				{
					TLogicLevelNpcMailElem elem;
					elem.m_iLevel = iLevel;
					elem.m_stMail = stMail;
					m_astLevelMail.emplace_back(elem);
				}
				else if (iDateTime > 0)
				{
					TLogicDatetimeNpcMailElem elem;
					elem.m_iDatetime = iDateTime;
					elem.m_iExpiredTime = iExpiredTime ? iExpiredTime : std::numeric_limits<int32_t>::max();
					elem.m_iActivityID = iActivityID;
					elem.m_iServerDay = iServerDay;
					elem.m_iActivityID = iActivityID;
					elem.m_stMail = stMail;
					m_astDateTimeMail.emplace_back(elem);
				}
				else
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:MAIL FORMAT TIME ERROR" << std::endl;
					throw std::exception();
				}
			}

			stXML.OutOfElem();
		}

		if (stXML.FindElem("event_mail"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("mail"))
			{
				TLogicMailElem stMail;
				stMail.m_strSender = stXML.GetAttrib("sender");
				stMail.m_strTitle = stXML.GetAttrib("title");
				stMail.m_stContent = CFormatString::Compile(stXML.GetAttrib("content"));

				CPackGameItem stGameItem;

				for (int i = 1; i < 5; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

					stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
					stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
					stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

					stMail.m_astItem.push_back(stGameItem);
				}

				std::string id = stXML.GetAttrib("id");
				m_astEventMail[id] = stMail;
			}

			stXML.OutOfElem();
		}
		

		stXML.OutOfElem();
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigZooLogicGameParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("root"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (stXML.FindElem("game"))
		{
			m_strNormalService = stXML.GetAttrib("normal_service");
			m_strSuperService = stXML.GetAttrib("super_service");
			m_iMinResVersion = atoi(stXML.GetAttrib("min_res_version").c_str());
			m_strGuildBulletin = stXML.GetAttrib("guild_bulletin");
			m_iEvaluateLevel = atoi(stXML.GetAttrib("evaluate_level").c_str());
			m_iEvaluateRatio = atoi(stXML.GetAttrib("evaluate_ratio").c_str());
            std::string strStarList = stXML.GetAttrib("price_change");
            auto stStarVec = string_split(strStarList, "|");
            for(std::size_t i = 0; i < stStarVec.size(); ++i)
            {
                int iControl = ::atoi(stStarVec[i].c_str());
                m_iSwitchControl.push_back(iControl);
            }
		}

		if (stXML.FindElem("dirty_word"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("word"))
			{
				std::string strDirtyWord = stXML.GetData();
				if (strDirtyWord.empty() == false)
				{
					m_strDirtyWordVec.push_back(strDirtyWord);
				}
			}

			stXML.OutOfElem();
		}

		stXML.OutOfElem();
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigJubaoParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("root"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("jubao"))
		{
			auto id = atoi(stXML.GetAttrib("id").c_str());
			auto name = stXML.GetAttrib("jubao_name");
			m_aszJubaoNameMap.emplace(id, name);
		}

		stXML.OutOfElem();
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string> CLogicConfigRankParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if (true == strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if (false == stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        if (false == stXML.FindElem("root"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("rank_list"));

        stXML.IntoElem();
        while (stXML.FindElem("rank"))
        {
            TLogicRankElem stElem;
            stElem.m_iRankID = atoi(stXML.GetAttrib("rank_id").c_str());
            CONFIG_ASSERT_EXCEPTION(stElem.m_iRankID > 0);
            CONFIG_ASSERT_EXCEPTION(m_stRankConfig.find(stElem.m_iRankID) == m_stRankConfig.end());

            stElem.m_bIsLess = (bool)atoi(stXML.GetAttrib("is_less").c_str());
            stElem.m_iMaxRank = atoi(stXML.GetAttrib("max_rank").c_str());
            CONFIG_ASSERT_EXCEPTION(stElem.m_iMaxRank >= 10 && stElem.m_iMaxRank <= 5000);

            stElem.m_iRefreshType = atoi(stXML.GetAttrib("refresh_type").c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumRankRefreshType(stElem.m_iRefreshType));

            stElem.m_iRefreshPara = atoi(stXML.GetAttrib("refresh_para").c_str());
            stElem.m_bIsClear = (bool)atoi(stXML.GetAttrib("is_clear").c_str());

            m_stRankConfig[stElem.m_iRankID] = stElem;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("rank_reward"));

        stXML.IntoElem();
        while (stXML.FindElem("rank"))
        {
            int32_t iRankID = atoi(stXML.GetAttrib("rank_id").c_str());
            auto iter = m_stRankConfig.find(iRankID);
            if(iter == m_stRankConfig.end())
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:rank_reward id error! rank_id:" << iRankID << std::endl;
                throw std::exception();
            }

            stXML.IntoElem();
            int32_t iLastRank = 0;
            while (stXML.FindElem("reward"))
            {
                TLogicRankAwardElem stReward;
                stReward.m_iMinRank = atoi(stXML.GetAttrib("min").c_str());
                stReward.m_iMaxRank = atoi(stXML.GetAttrib("max").c_str());
                CONFIG_ASSERT_EXCEPTION(stReward.m_iMinRank > iLastRank);
                CONFIG_ASSERT_EXCEPTION(stReward.m_iMinRank <= stReward.m_iMaxRank);

                iLastRank = stReward.m_iMaxRank;

                stReward.m_strMailTitle = stXML.GetAttrib("mail_title");
                stReward.m_strMailSender = stXML.GetAttrib("mail_sender");
                stReward.m_strMailContent = stXML.GetAttrib("mail_content");

                for (int32_t i = 1; i <= 10; ++i)
                {
                    if (stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
                        break;

                    CPackGameItem stItem;
                    stItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).c_str());
                    stItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)).c_str());
                    stItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)).c_str());
                    stReward.m_stBonusVec.push_back(std::move(stItem));
                }

                iter->second.m_stAwardMap[stReward.m_iMaxRank] = std::move(stReward);
            }
            stXML.OutOfElem();
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

const TLogicRankElem* CLogicConfigRankParser::GetConfig(int32_t iRankID) const
{
    auto iter = m_stRankConfig.find(iRankID);
    return iter == m_stRankConfig.end() ? nullptr : &iter->second;
}
