#include "AppConfig.h"
#include "common_config.h"
#include <iostream>

CAppConfig::CAppConfig(void)
{
	memset(&m_LogParam,0,sizeof(LogParam));

    m_iServerID = 0;
	m_szZookeeperHost[0] = '\0';	
	m_szZookeeperConfigPath[0] = '\0';
}

CAppConfig::~CAppConfig(void)
{
	//
}

bool CAppConfig::Load(const char* pszFile)
{
	CConfigFile cfg;
	cfg.open_file(pszFile);
	if (!cfg.opened())
	{
		std::cout << "ConfigFile Not Found:" << pszFile << std::endl;
		return false;
	}

    cfg.get_value_of("common","id",(int32_t&)m_iServerID);
	cfg.get_value_of("zookeeper", "host", &m_szZookeeperHost[0], max_path_length, "");
	cfg.get_value_of("zookeeper", "config_path", &m_szZookeeperConfigPath[0], max_path_length, "/");
 
	//log
	strcpy(m_LogParam.m_szLogPath, LOG_PATH);
	cfg.get_value_of("log","logmask",(int32_t&)m_LogParam.m_nLogMask,0xffffffff);         //打印全部

	cfg.close_file();

	return true;
}

CRouterManager::CRouterManager() : m_iZookeeperSocketFd(-1)
{
}

void CRouterManager::CheckConnectionAlive()
{
	for (auto& tcp : m_astConnectionMap)
	{
		const bool before = tcp.second.BeConnected();
		tcp.second.CheckAlive();
		const bool after = tcp.second.BeConnected();

		if (before && !after)
		{
			TRACESVR(log_mask_system_error, "[CRouterManager::%s] too long alive, close peer(%d:%d), ip(%s:%d)\n",
				__FUNCTION__, tcp.second.GetEntityType(), tcp.second.GetEntityId()
				, tcp.second.GetStrIp().c_str(), tcp.second.GetPort());
		}
	}
}

ConnectionType* CRouterManager::GetServerConnection(int32_t iServerID)
{
	auto iter = m_astConnectionMap.find(iServerID);
	return iter == m_astConnectionMap.end() ? nullptr : &iter->second;
}

ConnectionType& CRouterManager::AddOneConnection(EnumEntityTypes entity_type, int32_t entity_id, const std::string& ip, uint16_t port)
{
	const auto iter = m_astConnectionMap.find(entity_id);
	if (iter != m_astConnectionMap.end())
	{
		iter->second.CloseSocket();
		m_astConnectionMap.erase(iter);
	}

	auto stRet = m_astConnectionMap.emplace(std::piecewise_construct, std::forward_as_tuple(entity_id), std::forward_as_tuple());
	stRet.first->second.Init(entity_type, entity_id, ip, port);
	return stRet.first->second;
}
