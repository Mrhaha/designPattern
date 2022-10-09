#include "QueueAppConfig.h"
#include "common_config.h"
#include <iostream>

CQueueAppConfig::CQueueAppConfig(void)
{
	memset(&m_LogParam,0,sizeof(LogParam));
    m_iServerID = 0;
    m_iMaxClientNum = MIN_QUEUE_CLIENT_NUM;
    m_iMaxClientSocketBuffSize = 0;

    m_szQueueSvrSessionKey[0] = '\0';
    m_szConnSvrSessionKey[0] = '\0';

    m_nSignatureValidPeriod = 0;
    m_iCheckSocketInterval = 0;
    m_iMaxSocketUnaliveTime = 0;
    m_iMaxWaitForFirstPackageTime = 0;

	m_szZookeeperHost[0] = '\0';	
	m_szZookeeperConfigPath[0] = '\0';
}

CQueueAppConfig::~CQueueAppConfig(void)
{
	//
}

bool CQueueAppConfig::Load(const char* pszFile)
{
	CConfigFile cfg;
	cfg.open_file(pszFile);
	if (!cfg.opened())
	{
		std::cout << "ConfigFile Not Found:" << pszFile << std::endl;
		return false;
	}

    cfg.get_value_of("common","id",(int32_t&)m_iServerID);
    cfg.get_value_of("common","max_connection_num",(int32_t&)m_iMaxClientNum);
    cfg.get_value_of("common","max_buff_size",(int32_t&)m_iMaxClientSocketBuffSize);

    cfg.get_value_of("common","queue_session_key", &m_szQueueSvrSessionKey[0], SESSION_KEY_LENGTH, "");
    cfg.get_value_of("common","conn_session_key",&m_szConnSvrSessionKey[0], SESSION_KEY_LENGTH, "");
    cfg.get_value_of("common","signature_valid_period",(int32_t&)m_nSignatureValidPeriod);

    cfg.get_value_of("common","check_socket_interval",(int32_t&)m_iCheckSocketInterval);
    cfg.get_value_of("common","sokcet_unalive_time",(int32_t&)m_iMaxSocketUnaliveTime);
    cfg.get_value_of("common","wait_firt_package_time",(int32_t&)m_iMaxWaitForFirstPackageTime);

    if(strcmp(m_szQueueSvrSessionKey, "") == 0 || strcmp(m_szConnSvrSessionKey, "") == 0)
    {
        std::cout << "ConfigFile session_key error!" << std::endl;
        return false;
    }

    if(m_nSignatureValidPeriod <= 0)
    {
        m_nSignatureValidPeriod = 600;
    }

	cfg.get_value_of("zookeeper", "host", &m_szZookeeperHost[0], max_path_length, "");
	cfg.get_value_of("zookeeper", "config_path", &m_szZookeeperConfigPath[0], max_path_length, "/");
 
	//log
	strcpy(m_LogParam.m_szLogPath, LOG_PATH);
	cfg.get_value_of("log","logmask",(int32_t&)m_LogParam.m_nLogMask,0xffffffff);         //打印全部

    if(m_iMaxClientNum < MIN_QUEUE_CLIENT_NUM)
    {
        m_iMaxClientNum = MIN_QUEUE_CLIENT_NUM;
    }

    if(m_iMaxClientNum > MAX_QUEUE_CLIENT_NUM)
    {
        m_iMaxClientNum = MAX_QUEUE_CLIENT_NUM;
    }

    if(m_iMaxClientSocketBuffSize <= 0)
    {
        m_iMaxClientSocketBuffSize = Q_MAX_CLIENT_BUFFER_SIZE;
    }

	cfg.close_file();

	return true;
}

CQueueRouterManager::CQueueRouterManager()
{
}

ConnectionType& CQueueRouterManager::AddServerConnection(int32_t iFd, EnumEntityTypes entity_type, int32_t entity_id, const std::string& ip, uint16_t port)
{
    // 查重
    auto iterID = m_stServerIDToFd.find(entity_id);
    if(iterID != m_stServerIDToFd.end())
    {
        auto iterSock = m_astConnectionMap.find(iterID->second);
        if(iterSock != m_astConnectionMap.end())
        {
            iterSock->second.CloseSocket();
            m_astConnectionMap.erase(iterSock);

            TRACESVR(log_mask_system_error, "[CQueueRouterManager::%s]has a repeat logic_svr connection, server_id(%d), inof(%s:%ud), close fd:%d, new fd:%d\n",
                     __FUNCTION__, entity_id, ip.c_str(), port, iterID->second, iFd);
        }
    }

	const auto iter = m_astConnectionMap.find(iFd);
	if (iter != m_astConnectionMap.end())
	{
		m_astConnectionMap.erase(iter);
	}

	auto stRet = m_astConnectionMap.emplace(std::piecewise_construct, std::forward_as_tuple(iFd), std::forward_as_tuple());
	stRet.first->second.Init(entity_type, entity_id, ip, port);

    m_stServerIDToFd[entity_id] = iFd;

	return stRet.first->second;
}

void CQueueRouterManager::AddClientConnection(const TSocketInfo& stInfo)
{
    // 如果该连接已存在，出错，清除
    auto iter = m_stClientConMap.find(stInfo.m_socket_fd);
    if(iter != m_stClientConMap.end())
    {
        m_stClientConMap.erase(iter);
    }

    m_stClientConMap[stInfo.m_socket_fd] = std::move(stInfo);
}

void CQueueRouterManager::ClearServerConnection(int32_t iFd)
{
    const auto iter = m_astConnectionMap.find(iFd);
    if (iter != m_astConnectionMap.end())
    {
        TRACESVR(log_mask_system_warning, "[CQueueRouterManager::%s] close logic(ip:%s, port:%d, server:%d)\n",
                 __FUNCTION__, iter->second.GetStrIp().c_str(), iter->second.GetPort(), iter->second.GetEntityId());

        iter->second.CloseSocket();
        m_astConnectionMap.erase(iter);
    }
    else
    {
        TRACESVR(log_mask_system_warning, "[CQueueRouterManager::%s] close logic fd = %d\n", __FUNCTION__, iFd);
    }
}

void CQueueRouterManager::ClearClientSocket(int32_t iFd, EnumClientCloseReason code)
{
    auto iter = m_stClientConMap.find(iFd);
    if(iter != m_stClientConMap.end())
    {
        TRACESVR(log_mask_system_warning, "[CQueueRouterManager::%s] close client(ip:%s, port:%d, uin:%d) and errorCode = %d\n",
                 __FUNCTION__, iter->second.m_client_address, iter->second.m_peer_port, iter->second.m_uin, code);

        close(iFd);
        m_stClientConMap.erase(iter);
    }
    else
    {
        TRACESVR(log_mask_system_warning, "[CQueueRouterManager::%s] close client fd = %d, errorCode = %d\n", __FUNCTION__, iFd, code);
    }
}
