#include "ServerConfig.h"
#include "AppCommon.h"

std::shared_ptr<CCrossServerConfig> CCrossServerConfig::m_pConfig;

std::mutex CCrossServerConfig::m_stLock;

CCrossServerConfig::config_loader_t CCrossServerConfig::CONFIG_LOADER_LIST[] =
{
    { "zoo_cross_config",                        CCrossServerConfig::GetZooCrossConfigParser },
    { "zoo_cross_activity",                      CCrossServerConfig::GetZooActivityConfigParser },
    { "match_pvp",								 CCrossServerConfig::GetMatchPvpConfigParser },
    { "match_pvp_robot",						 CCrossServerConfig::GetMatchPvpRobotConfigParser },
    { "guild_war",								 CCrossServerConfig::GetGuildWarConfigParser },
    { "pushover",								 CCrossServerConfig::GetPushoverConfigParser },
    { "guild",				                     CCrossServerConfig::GetGuildConfigParser },
    { "data_report",				             CCrossServerConfig::GetDataReportConfigParser },

};

size_t CCrossServerConfig::s_iConfigSize = sizeof(CCrossServerConfig::CONFIG_LOADER_LIST) / sizeof(CCrossServerConfig::config_loader_t);

void CCrossServerConfig::GetConfig(std::shared_ptr<CCrossServerConfig>& pConfig)
{
	pConfig.reset();
	{
		std::unique_lock<std::mutex> locker(m_stLock);
		pConfig = m_pConfig;
	}
}

void CCrossServerConfig::SetConfig(std::shared_ptr<CCrossServerConfig> pConfig)
{
	{
		std::unique_lock<std::mutex> locker(m_stLock);
		m_pConfig.swap(pConfig);
	}
}