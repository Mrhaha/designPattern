#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <string>
#include <fstream>

#include "logic_service_statistics.h"
#include "logic_service_timer.h"
#include "logic_service_poller.h"
#include "logic_service_logger.h"
#include "logic_service.h"
#include "logic_service_mysql_handle.h"
#include "server_cmd_cross_define.h"

#define EPOLL_WAIT_TIME_OUT 100

bool CLogicService::s_bLoadConfigSuccess = false;

CLogicService::CLogicService()
	: m_iLogicAcceptFd(0), m_iAdminAcceptFd(0), m_stConnectorReceiver(), m_stAdminContainer()
{
	SERVICE_API_INS->SetConnectorSender(&m_stConnectorSender);
	SERVICE_API_INS->SetCrossConnection(&m_stCrossConnection);
    SERVICE_API_INS->SetQueueConnection(&m_stQueueConnection);
}

void CLogicService::RealRun(int iArgc, char** pArgv)
{
	if (!BeforeRun(iArgc, pArgv))
	{
		SERVICE_LOG_TRACE_ERROR << "SERVER START FAILED !!!" << std::endl;
		return;
	}

	while (!IsQuit())
	{
		TickRunning(iArgc, pArgv);

		EpollRunning(iArgc, pArgv);
	}
   
	AfterRun(iArgc, pArgv);
}

bool CLogicService::BeforeRun(int iArgc, char** pArgv)
{
	if (!InitZookeeper())
	{
		return false;
	}

	if (!InitLogicConn())
	{
		return false;
	}

	if (!InitAdminConn())
	{
		return false;
	}

	if (!InitCrossConn())
	{
		return false;
	}

    if(!InitQueueConn())
    {
        return false;
    }

	if (!InitMysqlThread())
	{
		return false;
	}

	if (!InitConnectorSend())
	{
		return false;
	}

	if (!InitOtherSystem())
	{
		return false;
	}

    SERVICE_API_INS->GetLogicSoGameApi()->StartGameApiSystem();

	SERVICE_LOG_TRACE_DEBUG << "SERVER START NOW !!!" << std::endl;

	return true;
}

void CLogicService::TickRunning(int iArgc, char** pArgv)
{
    SERVICE_API_INS->GetLogicSoGameApi()->OnTimerTick();

	const int32_t iNowSec = SERVICE_API_INS->GetLogicSoGameApi()->GetNowSec();

	m_stConnectorSender.Check(iNowSec);
	m_stConnectorReceiver.HeartBeat(iNowSec);
	RefreshCrossConn(iNowSec);
    RefreshQueueConn(iNowSec);

	SERVICE_TIMER->Schedule();
    SERVICE_STATISTICS->Refresh();
}

void CLogicService::EpollRunning(int iArgc, char** pArgv)
{
	int iZooFd = -1, iZooEvent = 0, iZooEpollEvent = 0;
	m_stZookeeper.Interest(iZooFd, iZooEvent);

	if (iZooEvent & ZOOKEEPER_READ) iZooEpollEvent |= EPOLLIN;
	if (iZooEvent & ZOOKEEPER_WRITE) iZooEpollEvent |= EPOLLOUT;

	SERVICE_POLLER_INS->add(iZooFd, (long long)iZooFd, iZooEpollEvent);

	int iEventNum = SERVICE_POLLER_INS->wait(EPOLL_WAIT_TIME_OUT);
	if (iEventNum > 0)
	{
		for (int i = 0; i < iEventNum; i++)
		{
			struct epoll_event& stEvent = SERVICE_POLLER_INS->get(i);

			if (stEvent.data.fd == m_iLogicAcceptFd)
			{
				struct sockaddr_in stClientAddr;
				socklen_t iClientAddrLen = sizeof(stClientAddr);
				int iClientFd = ::accept(m_iLogicAcceptFd, (struct sockaddr*)&stClientAddr, &iClientAddrLen);
				if (iClientFd < 0)
				{
					SERVICE_LOG_TRACE_ERROR << "ACCEPT CLIENT ERROR|MSG:" << ::strerror(errno) << std::endl;
				}
				else
				{
					std::string strClientIP = std::string(::inet_ntoa(stClientAddr.sin_addr));
					unsigned short usClientPort = ::ntohs(stClientAddr.sin_port);
					if (IsConnector(strClientIP, usClientPort))
					{
						m_stConnectorReceiver.SetFd(iClientFd, strClientIP, usClientPort);
						SERVICE_POLLER_INS->add(iClientFd, (long long)iClientFd, EPOLLIN);
						SERVICE_LOG_TRACE_DEBUG << "CONNECTOR RECEIVER IS ESTABLISH!|CONNECTOR:"
							<< strClientIP << ":" << usClientPort << std::endl;
					}
					else
					{
						::close(iClientFd);
						SERVICE_LOG_TRACE_ERROR << "UNKNOWN CLIENT CONNECT|CLOSE CONNECTION|" << strClientIP
							<< ":" << usClientPort << std::endl;
					}
				}
			}
			else if (stEvent.data.fd == m_iAdminAcceptFd)
			{
				struct sockaddr_in stClientAddr;
				socklen_t iClientAddrLen = sizeof(stClientAddr);
				int iClientFd = ::accept(m_iAdminAcceptFd, (struct sockaddr*)&stClientAddr, &iClientAddrLen);
				if (iClientFd < 0)
				{
					SERVICE_LOG_TRACE_ERROR << "ACCEPT CLIENT ERROR|MSG:" << ::strerror(errno) << std::endl;
				}
				else
				{
					std::string strClientIP = std::string(::inet_ntoa(stClientAddr.sin_addr));
					unsigned short usClientPort = ::ntohs(stClientAddr.sin_port);
					if (intranet_ip(stClientAddr.sin_addr.s_addr))
					{
						m_stAdminContainer.AddAdminClient(iClientFd, strClientIP, usClientPort);
						SERVICE_POLLER_INS->add(iClientFd, (long long)iClientFd, EPOLLIN);
						SERVICE_LOG_TRACE_DEBUG << "ADMIN IS ESTABLISH!|ADMIN:" << strClientIP << ":" << usClientPort << std::endl;
					}
					else
					{
						SERVICE_LOG_TRACE_ERROR << "ACCEPT INVALID ADMIN CONNECT|HOST:" << strClientIP << ":" << usClientPort << std::endl;
					}
				}
			}
			else if (stEvent.data.fd == m_stConnectorReceiver.GetFd())
			{
				int iLeftLength = m_stConnectorReceiver.Read();
				if (iLeftLength < 0)
				{
					SERVICE_LOG_TRACE_ERROR << "CONNECTOR RECEIVER ERROR" << std::endl;
				}
				else
				{
					SERVICE_LOG_TRACE_DEBUG << "CONNECTOR RECEIVER READ SUCCESS|LEFT PACKET SIZE:" << iLeftLength << std::endl;
				}
			}
			else if (stEvent.data.fd == m_stConnectorSender.GetFd())
			{
				m_stConnectorSender.Read();
			}
			else if (stEvent.data.fd == iZooFd)
			{
				iZooEvent = 0;
				if (stEvent.events & EPOLLIN) iZooEvent |= ZOOKEEPER_READ;
				if (stEvent.events & EPOLLOUT) iZooEvent |= ZOOKEEPER_WRITE;

				SERVICE_POLLER_INS->del(iZooFd, (long long)iZooFd, stEvent.events);

				m_stZookeeper.Process(iZooEvent);
			}
			else if (stEvent.data.fd == m_stCrossConnection.GetFD())
			{
				m_stCrossConnection.RecvData();
			}
            else if(stEvent.data.fd == m_stQueueConnection.GetFD())
            {
                m_stQueueConnection.RecvData();
            }
			else
			{
				m_stAdminContainer.RecvData(stEvent.data.fd);
			}
		}
	}
}

void CLogicService::AfterRun(int iArgc, char** pArgv)
{
	SERVICE_API_INS->GetLogicSoGameApi()->StopGameApiSystem();

    SERVICE_STATISTICS->LastExec();

	delete SERVICE_MYSQL_HANDLE;

	mysql_library_end();

	SERVICE_LOG_TRACE_DEBUG << "SERVER STOP NOW !!!" << std::endl;
}

bool
CLogicService::LoadConfig(const CLogicZookeeper& stZookeeper)
{
	int64_t lStartMicroSecond = 0;
    int64_t lEndMicroSecond = 0;
    int64_t lCostMicroSecond = 0;

    lStartMicroSecond = taf::TC_Common::now2us();

    auto GetXMLContentFunc = std::bind(&CLogicZookeeper::GetNormalConfig, &stZookeeper, std::placeholders::_1);

    const auto stRet = SERVICE_API_INS->GetLogicSoGameApi()->LoadGameConfig(GetXMLContentFunc);

	//stZookeeper.SetErrorInfo(stRet.second);

	s_bLoadConfigSuccess = stRet.first;

	lEndMicroSecond = taf::TC_Common::now2us();
	lCostMicroSecond = lEndMicroSecond - lStartMicroSecond;
	SERVICE_LOG_TRACE_DEBUG << "LOAD CONFIG END!COST " << lCostMicroSecond << " us" << std::endl;

	return stRet.first;
}

bool	
CLogicService::IsConnector(const std::string& strIP, unsigned short usPort)
{
	const auto& config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameRouterConfig();
	if ((strIP == config.m_stConnector.m_strHost) && (usPort == config.m_stConnector.m_iSendPort))
	{
        SERVICE_LOG_TRACE_ERROR << "CONNECTOR CONNECT TO LOGIC SERVICE!|CONNECTOR_SERVICE:" << config.m_stConnector.m_strHost << ":"
                        << config.m_stConnector.m_iSendPort << std::endl;
        
		return (true);
	}
	else
	{
        SERVICE_LOG_TRACE_ERROR << "UNKNOWN CONNECTOR CONNECT TO LOGIC SERVICE|" << strIP << ":" << usPort << "|CONNECTOR_SERVICE:"
                        << config.m_stConnector.m_strHost << ":" << config.m_stConnector.m_iSendPort << std::endl;
		return (false);
	}
}

bool CLogicService::RefreshCrossConn(int32_t iNowSec)
{
	const auto& server_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameServerConfig();
	const auto& router_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameRouterConfig().m_stCross;
	if (router_config.m_strHost != m_stCrossConnection.GetStrIp() || router_config.m_iListenPort != m_stCrossConnection.GetPort())
	{
		if (m_stCrossConnection.BeConnected())
		{
			SERVICE_LOG_TRACE_DEBUG << "CHANGE CROSS SERVER FROM " << m_stCrossConnection.GetStrIp() << ":" << m_stCrossConnection.GetPort()
				<< "TO "<< router_config.m_strHost << ":" << router_config.m_iListenPort << std::endl;

			m_stCrossConnection.CloseSocket();
		}

		m_stCrossConnection.Init(m_stCrossConnection.GetEntityType(), m_stCrossConnection.GetEntityId(), router_config.m_strHost, router_config.m_iListenPort);
	}

	static int32_t s_iLastCheckConnTime = iNowSec;
	if (!m_stCrossConnection.BeConnected() && abs(iNowSec - s_iLastCheckConnTime) > 10)
	{
		s_iLastCheckConnTime = iNowSec;
		const auto ret_conn = m_stCrossConnection.ConnectTo(server_config.m_strHost.c_str(), uint16_t(router_config.m_iLogicSendPort));

		SERVICE_LOG_TRACE_DEBUG << "TRY TO CONNECT CROSS SERVER|" << router_config.m_strHost << ":" << router_config.m_iLogicSendPort << "|RET:" << ret_conn << std::endl;
	}
	static int32_t s_iLastHeartBeatTime = iNowSec;
	if (m_stCrossConnection.BeConnected() && abs(iNowSec - s_iLastHeartBeatTime) > 30)
	{
		s_iLastHeartBeatTime = iNowSec;
		m_stCrossConnection.HeartBeat();
	}

	return true;
}

bool CLogicService::RefreshQueueConn(int32_t iNowSec)
{
    const auto& server_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameServerConfig();
    const auto& router_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameRouterConfig().m_stQueue;
    if (router_config.m_strHost != m_stQueueConnection.GetStrIp() || router_config.m_iListenPort != m_stQueueConnection.GetPort())
    {
        if (m_stQueueConnection.BeConnected())
        {
            SERVICE_LOG_TRACE_DEBUG << "CHANGE QUEUE SERVER FROM " << m_stQueueConnection.GetStrIp() << ":" << m_stQueueConnection.GetPort()
                                    << "TO "<< router_config.m_strHost << ":" << router_config.m_iListenPort << std::endl;

            m_stQueueConnection.CloseSocket();
        }

        m_stQueueConnection.Init(m_stQueueConnection.GetEntityType(), m_stQueueConnection.GetEntityId(), router_config.m_strHost, router_config.m_iListenPort);
    }

    static int32_t s_iLastCheckConnTime = iNowSec;
    if (!m_stQueueConnection.BeConnected() && abs(iNowSec - s_iLastCheckConnTime) > 10 && !m_stQueueConnection.GetStrIp().empty())
    {
        s_iLastCheckConnTime = iNowSec;
        const auto ret_conn = m_stQueueConnection.ConnectTo(server_config.m_strHost.c_str(), uint16_t(router_config.m_iLogicSendPort));

        SERVICE_LOG_TRACE_DEBUG << "TRY TO CONNECT QUEUE SERVER|" << router_config.m_strHost << ":" << router_config.m_iLogicSendPort << "|RET:" << ret_conn << std::endl;

        // 连接成功后通知排队服务器
        if(m_stQueueConnection.BeConnected())
        {
            SERVICE_API_INS->GetLogicSoGameApi()->NotifyQueueSvrUserListOnline();
        }
    }
    static int32_t s_iLastHeartBeatTime = iNowSec;
    if (m_stQueueConnection.BeConnected() && abs(iNowSec - s_iLastHeartBeatTime) > 30)
    {
        s_iLastHeartBeatTime = iNowSec;
        m_stQueueConnection.CmdHeartBeat(MSG_CROSS_HEART_BEAT);
    }

    return true;
}

bool CLogicService::InitZookeeper()
{
    CLogicZookeeper::zookeeper_info_t stZookeeperInfo;
    if (!m_stZookeeper.GetZookeeperInfoFromFile("./config/hosts", stZookeeperInfo))
    {
        SERVICE_LOG_TRACE_ERROR << "CAN NOT FOUND hosts FILE" << std::endl;
        return false;
    }
    
    m_stZookeeper.SetZookeeperInfo(stZookeeperInfo);

    const auto file_names = SERVICE_API_INS->GetLogicSoGameApi()->GetConfigFileNames();
	for (const auto& file_name : file_names)
	{
		m_stZookeeper.AddWatchFile(file_name);
	}

    if (!m_stZookeeper.InitZookeeper())
    {
        SERVICE_LOG_TRACE_ERROR << "ZOOKEEPER INIT FAILED!!!" << std::endl;
        return false;
    }

    CLogicZookeeper::reload_func_type stLoadConfig = std::bind(&CLogicService::LoadConfig, this, std::placeholders::_1);

	m_stZookeeper.RegisterObserver(stLoadConfig);

    if (!m_stZookeeper.WaitAllConfigReady(-1))
    {
        SERVICE_LOG_TRACE_ERROR << "ZOOKEEPER FIRST LOAD CONFIG FAILED!!!" << std::endl;
        return false;
    }

    if (!s_bLoadConfigSuccess)
    {
        SERVICE_LOG_TRACE_ERROR << "ZOOKEEPER FIRST LOAD CONFIG FAILED!!!" << std::endl;
        return false;
    }

    return true;
}

bool
CLogicService::InitLogicConn()
{
	const auto& server_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameServerConfig();

    m_iLogicAcceptFd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_iLogicAcceptFd < 0)
	{
		SERVICE_LOG_TRACE_ERROR << "SOCKET ACCEPT FD ERROR:" << ::strerror(errno) << std::endl;
		return false;
	}

    int iFlag = 1;
    ::setsockopt(m_iLogicAcceptFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&iFlag, sizeof(iFlag));

	struct sockaddr_in stBindAddr;
   	stBindAddr.sin_family = AF_INET;
   	stBindAddr.sin_port = ::htons((uint16_t)server_config.m_iListenPort);
   	stBindAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
   	if (::bind(m_iLogicAcceptFd, (struct sockaddr*)&stBindAddr, sizeof(stBindAddr)) != 0)
   	{
   		SERVICE_LOG_TRACE_ERROR << "BIND PORT ERROR:" << ::strerror(errno) << "|BIND_PORT:" 
                        << server_config.m_iListenPort << std::endl;
                  
   		return false;
   	}

   	::listen(m_iLogicAcceptFd, 1024);

    SERVICE_POLLER_INS->add(m_iLogicAcceptFd, (long long)m_iLogicAcceptFd, EPOLLIN);

    return true;
}

bool
CLogicService::InitAdminConn()
{
    m_iAdminAcceptFd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_iAdminAcceptFd < 0)
	{
		SERVICE_LOG_TRACE_ERROR << "SOCKET ADMIN FD ERROR:" << ::strerror(errno) << std::endl;
		return false;
	}

	const auto& server_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameServerConfig();

    int iFlag = 1;
    ::setsockopt(m_iAdminAcceptFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&iFlag, sizeof(iFlag));

	struct sockaddr_in stBindAddr;
   	stBindAddr.sin_family = AF_INET;
   	stBindAddr.sin_port = ::htons(server_config.m_iAdminListenPort);
   	stBindAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
   	if (::bind(m_iAdminAcceptFd, (struct sockaddr*)&stBindAddr, sizeof(stBindAddr)) != 0)
   	{
   		SERVICE_LOG_TRACE_ERROR << "BIND ADMIN_PORT ERROR:" << ::strerror(errno) << "|ADMIN_PORT:" 
                        <<  server_config.m_iAdminListenPort << std::endl;
                  
   		return false;
   	}

   	::listen(m_iAdminAcceptFd, 1024);

    SERVICE_POLLER_INS->add(m_iAdminAcceptFd, (long long)m_iAdminAcceptFd, EPOLLIN);

    return true;
}

bool CLogicService::InitCrossConn()
{
	const auto& server_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameServerConfig();
	const auto& router_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameRouterConfig().m_stCross;
	m_stCrossConnection.Init(Enum_EET_EntityTypes_Cross, server_config.m_iServerID, router_config.m_strHost, router_config.m_iListenPort);
	m_stCrossConnection.Set4ByteSize(true);

	return true;
}

bool CLogicService::InitQueueConn()
{
    const auto& server_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameServerConfig();
    const auto& router_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameRouterConfig().m_stQueue;
    m_stQueueConnection.Init(Enum_EET_EntityTypes_Queue, server_config.m_iServerID, router_config.m_strHost, router_config.m_iListenPort);
    m_stQueueConnection.Set4ByteSize(true);

    return true;
}

bool
CLogicService::InitMysqlThread()
{
    mysql_library_init(0, nullptr, nullptr);

    SERVICE_MYSQL_HANDLE->InitMysql();

    SERVICE_API_INS->GetLogicSoGameApi()->SetMysqlTableRecordConn(SERVICE_MYSQL_HANDLE->GetGameQueryConn());

    return true;
}

bool
CLogicService::InitConnectorSend()
{
	const auto& router_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameRouterConfig().m_stConnector;

	if (!m_stConnectorSender.Initialize(router_config.m_strHost, router_config.m_iListenPort,
                                        router_config.m_iLogicSendPort))
	{
		SERVICE_LOG_TRACE_ERROR << "CONNECT TO CONNECTOR ERROR" << std::endl;
		return false;
	}

    return true;
}

bool
CLogicService::InitOtherSystem()
{
	if (!SERVICE_API_INS->GetLogicSoGameApi()->InitGameApiSystem())
	{
		SERVICE_LOG_TRACE_ERROR << "LOGIC SO GAME API INIT FAIL" << std::endl;
		return false;
	}

#ifndef _DEBUG_
    taf::TC_Functor<void> stZookeeperConfigFunc(&m_stZookeeper, &CLogicZookeeper::UpdateConfig);

	const taf::TC_FunctorWrapper<taf::TC_Functor<void>> stZookeeperConfigWrapper(stZookeeperConfigFunc);

    SERVICE_TIMER->SetTimerTask(10, stZookeeperConfigWrapper);
#endif

    taf::TC_Functor<void> st15SecTimerFunc(SERVICE_API_INS->GetLogicSoGameApi(), &CLogicSoGameApi::OnTimer15Sec);

    const taf::TC_FunctorWrapper<taf::TC_Functor<void>> st15SecTimerFuncWrapper(st15SecTimerFunc);

    SERVICE_TIMER->SetTimerTask(15, st15SecTimerFuncWrapper);

    return true;
}
