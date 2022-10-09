#include <sys/mman.h>
#include <netinet/tcp.h>

#include "common_codeengine.h"
#include "common_flag.h"
#include "common_logengine.h"
#include "common_datetime.h"
#include "common_msg_head_connector.h"
#include "common_msg_head_client.h"
#include "gy_session_creator.h"
#include "connector_log.h"
#include "connector_frame.h"

stConnectorConfig g_stSvrConfig;
CFlag g_byRunFlag;


CConnFrame::CConnFrame()
{
	m_tmLastCheckConfigTime = 0;
	m_tmConfigLastModifyTime= 0;
}

CConnFrame::~CConnFrame()
{
}


void* CConnFrame::operator new( size_t size ) throw()
{
	return (::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
}


void CConnFrame::operator delete(void* ptr)
{
	::munmap(ptr, sizeof(CConnFrame));
}

int32_t CConnFrame::size()
{
	int32_t iTempSize  =  (int32_t)sizeof(CConnFrame);
	return iTempSize;
}

CConnFrame* CConnFrame::spawn()
{
	return new CConnFrame;
}

int32_t CConnFrame::initialize()
{
	m_stSocketSatistics.m_unSocketCount = 0;
	m_stSocketSatistics.m_unSocketAdd = 0;

	int32_t ret = load_config();
	if( success != ret)
	{
		return ret;
	}

	for(size_t i = 0; i < sizeof(m_stSocketInfo)/sizeof(m_stSocketInfo[0]); ++i)
	{
		memset(&m_stSocketInfo[i], 0x0, sizeof(m_stSocketInfo[i]));
		m_stSocketInfo[i].m_socket_fd  = socket_fd_invalid;
	}

	//epoll event entity array
	m_pevents = NULL;

	//open listen socket
	for (int32_t i = 0; i < m_stConfig.m_iOpenedPortCount; ++i)
	{
		// add by liuqy 2011-3-15
		ret = open_epoll_socket(m_stConfig.m_unOpenedPort[i], &m_stConfig.m_szLocalIP[0]);
		if (success != ret)
		{
			TRACESVR(log_mask_system_error, "[CConnFrame::%s] listen on port[%d] failed!\n",
				__FUNCTION__, m_stConfig.m_unOpenedPort[i]);
			return ret;
		}

		TRACESVR(log_mask_system_config, "[CConnFrame::%s] listening on port[%d] ready!\n",
				 __FUNCTION__, m_stConfig.m_unOpenedPort[i]);

		m_stSocketInfo[m_iSocket].m_socket_fd     = m_iSocket;
		m_stSocketInfo[m_iSocket].m_socket_type = enmSocketType_listen;
		m_stSocketInfo[m_iSocket].m_local_port = m_stConfig.m_unOpenedPort[i];
	}

	++m_iSocket;

	m_tmLastCheckConnection = 0;
	ret = refresh();
	if (success != ret)
	{
		return fail;
	}

	time(&m_tmLastCheckTime);
	time(&m_tmLastStatisticTime);

	TRACESVR("[%s] Connector Server initialize successfully!\n", __FUNCTION__);

	return success;
}

int32_t CConnFrame::refresh(void)
{
	time_t tmNow;
	time(&tmNow);

	if (tmNow - m_tmLastCheckConnection < check_interval)
	{
		return 0;
	}
	m_tmLastCheckConnection = tmNow;

	int32_t time_gap = 0;

	for (int16_t i=0; i< m_nRouterCount; ++i)
	{
		if (m_astServer[i].m_bHaveHello)
		{
			time_gap = int32_t(tmNow - m_astServer[i].m_tmLastActiveTime);

		}
		else
		{
			time_gap = 0;
		}

		if (m_astTCPConnection[i].GetStatus() == enmTCPConnectionStatus_connected &&
			m_astTCPConnection[i].GetFD() >= 0 && time_gap < max_unactive_interval)
		{
			continue;		//everything is ok!
		}

		TRACESVR(log_mask_detail, "[CConnFrame::%s] router index(%d) server(%s:%d) will to connect!\n", __FUNCTION__, i, m_astServer[i].m_szIP, m_astServer[i].m_nPort);

		int32_t iSocketFD = m_astTCPConnection[i].GetFD();
		if (iSocketFD > 0)
		{
			if (m_stSocketSatistics.m_unSocketCount > 0)
			{
				m_stSocketSatistics.m_unSocketCount--;
			}
			m_astTCPConnection[i].CloseSocket();
			TRACESVR(log_mask_detail, "[CConnFrame::%s] router index(%d) server(%s:%d) closed, but socket fd(%d) not zero!\n", __FUNCTION__, i, m_astServer[i].m_szIP, m_astServer[i].m_nPort, iSocketFD);

			if (iSocketFD < max_socket_count)
			{
				m_stSocketInfo[iSocketFD].m_socket_fd = socket_fd_invalid;
				m_stSocketInfo[iSocketFD].m_peer_address = 0;

				m_stSocketInfo[iSocketFD].m_peer_port = 0;
				m_stSocketInfo[iSocketFD].m_local_port = 0;
				m_stSocketInfo[iSocketFD].m_recv_bytes  = 0;
				m_stSocketInfo[iSocketFD].m_socket_type = enmSocketType_invalid;
				m_stSocketInfo[iSocketFD].m_uin = 0;

				m_stSocketInfo[iSocketFD].m_create_time = 0;
				m_stSocketInfo[iSocketFD].m_lastrecv_timestamp = 0;

				m_stSocketInfo[iSocketFD].m_is_sended_data_to_peer = false;
				m_stSocketInfo[iSocketFD].m_is_recved_data_from_peer = false; //
			}
		}

		int32_t ret = m_astTCPConnection[i].ConnectTo(m_stConfig.m_szLocalIP, (u_short)m_astServer[i].m_nLocalPort);
		if (0 == ret)
		{
			TRACESVR(log_mask_detail, "[CConnFrame::%s] router index(%d) connect to server(%s:%d) ok!\n", __FUNCTION__, i, m_astServer[i].m_szIP, m_astServer[i].m_nPort);

			m_astServer[i].m_tmLastActiveTime = tmNow;

			//与server之间的连接上传送来的数据，以4字节表示包长度
			m_astTCPConnection[i].Set4ByteSize(true);

			iSocketFD = m_astTCPConnection[i].GetFD();
			//连接统计
			++ m_stSocketSatistics.m_unSocketCount;
			++ m_stSocketSatistics.m_unSocketAdd;

			if((uint32_t)m_maxfd < m_stSocketSatistics.m_unSocketCount || max_socket_count <= iSocketFD)
			{
				TRACESVR(log_mask_system_warning, "[CConnFrame::%s] router index(%d) connection is so many, just refused!\n", __FUNCTION__, i);
				-- m_stSocketSatistics.m_unSocketCount;
				close(iSocketFD);
				continue;
			}

			//设置为非阻塞socket
			int flags = 1;
			if ( ioctl(iSocketFD, FIONBIO, &flags) &&  ((flags = fcntl(iSocketFD, F_GETFL, 0)) < 0
				||  fcntl(iSocketFD, F_SETFL, flags | O_NONBLOCK) < 0) )
			{
				TRACESVR(log_mask_system_warning, "[CConnFrame::%s] router index(%d) set accepted socket O_NONBLOCK failed, just close it!\n", __FUNCTION__, i);
				--m_stSocketSatistics.m_unSocketCount;
				close(iSocketFD);
				continue;
			}

			ret = epoll_new_socket(iSocketFD);
			if (ret < 0)
			{
				TRACESVR(log_mask_detail, "[CConnFrame::%s] router index(%d), epoll_new_socket(%d) fail!\n", __FUNCTION__, i, iSocketFD);
				continue;
			}

			//record socket info
			m_stSocketInfo[iSocketFD].m_socket_fd     = iSocketFD;
			m_stSocketInfo[iSocketFD].m_socket_type   = enmSocketType_server;

			m_stSocketInfo[iSocketFD].m_peer_address = m_astServer[i].m_iIP;//stSocketAddress.sin_addr.s_addr;
			m_stSocketInfo[iSocketFD].m_peer_port = m_astServer[i].m_nPort;//stSocketAddress.sin_port;
			STRNCPY(m_stSocketInfo[iSocketFD].m_client_address, m_astServer[i].m_szIP, sizeof(m_stSocketInfo[iSocketFD].m_client_address));
			m_stSocketInfo[iSocketFD].m_create_time = time(NULL);

			m_stSocketInfo[iSocketFD].m_lastrecv_timestamp = m_stSocketInfo[iSocketFD].m_create_time;

			m_stSocketInfo[iSocketFD].m_local_port = m_astServer[i].m_nLocalPort;
			m_stSocketInfo[iSocketFD].m_uin = 0;
			m_stSocketInfo[iSocketFD].m_is_recved_data_from_peer = true;
			m_stSocketInfo[iSocketFD].m_is_sended_data_to_peer = true;
		}
		else
		{
			//未连接上
			TRACESVR(log_mask_system_error, "[CConnFrame::%s] router index(%d) connect to server(%s:%d) failed, ret(%d)!\n",
					 __FUNCTION__, i, m_astServer[i].m_szIP, m_astServer[i].m_nPort, ret);
		}
	}//for


	for (int16_t i = 0; i < m_nClientCount; ++ i)
	{
		if (tmNow - m_astClient[i].m_tmLastActiveTime > hearbeat_interval)
		{
			m_astClient[i].m_tmLastActiveTime = tmNow;

			char cHello = 'H';
			int32_t ret = m_astClientConnection[i].SendData(&cHello, sizeof(cHello));
			if (success != ret)
			{
				TRACESVR(log_mask_detail, "in [%s] send package of message fail. and error code=%d|logic service:%s:%u. \n",
						 __FUNCTION__, ret, m_astClientConnection[i].GetStrIp().c_str(), m_astClientConnection[i].GetPort());
			}
			else
			{
				TRACESVR(log_mask_detail, "in [%s] send package of message success. and content = client hello\n", __FUNCTION__);
			}
		}
	}// for

	return success;
}

int32_t CConnFrame::on_hello( int32_t iSrcIP, int16_t nSrcPort )
{
	for (int32_t i=0; i<m_nRouterCount; ++i)
	{
		if (m_astTCPConnection[i].GetIp() == (uint32_t)iSrcIP &&
			m_astTCPConnection[i].GetPort() == (uint16_t)nSrcPort)
		{
			m_astServer[i].m_tmLastActiveTime = time(NULL);
			break;
		}
	}

	return success;
}


int32_t CConnFrame::check_server_to(int32_t iGroupID)
{
	int32_t iIndex = m_stConfig.GetServerIndex(iGroupID);

	if (iIndex < 0 || iIndex >= max_router_server_count)
	{
		return fail;
	}

	if (m_astTCPConnection[iIndex].GetFD() < 0 ||
		m_astTCPConnection[iIndex].GetStatus() != enmTCPConnectionStatus_connected)
	{
		//reconnect to router
		int32_t ret = m_astTCPConnection[iIndex].ConnectTo(m_stConfig.m_szLocalIP, (u_short)m_astServer[iIndex].m_nLocalPort);
		if (0 == ret)
		{
			TRACESVR(log_mask_detail, "[CConnFrame::%s] reconnect to [%s:%d] success.\n", __FUNCTION__, m_astServer[iIndex].m_szIP, m_astServer[iIndex].m_nPort);
			m_astServer[iIndex].m_tmLastActiveTime = time(NULL);
			m_astTCPConnection[iIndex].Set4ByteSize(true);

			int32_t iSocketFD = m_astTCPConnection[iIndex].GetFD();
			//连接统计
			++m_stSocketSatistics.m_unSocketCount;
			++m_stSocketSatistics.m_unSocketAdd;

			if((uint32_t)m_maxfd < m_stSocketSatistics.m_unSocketCount || max_socket_count <= iSocketFD)
			{
				TRACESVR(log_mask_system_warning, "[CConnFrame::%s] connection is so many, just refused!\n", __FUNCTION__);
				-- m_stSocketSatistics.m_unSocketCount;
				close(iSocketFD);
				return fail;
			}

			//设置为非阻塞socket
			int flags = 1;
			if ( ioctl(iSocketFD, FIONBIO, &flags) &&  ((flags = fcntl(iSocketFD, F_GETFL, 0)) < 0
				||  fcntl(iSocketFD, F_SETFL, flags | O_NONBLOCK) < 0) )
			{
				TRACESVR(log_mask_system_warning, "[CConnFrame::%s] set accepted socket O_NONBLOCK failed, just close it!\n", __FUNCTION__);
				--m_stSocketSatistics.m_unSocketCount;
				close(iSocketFD);
				return fail;
			}

			if (0 > epoll_new_socket(iSocketFD))
			{
				return fail;
			}

			//record socket info
			m_stSocketInfo[iSocketFD].m_socket_fd     = iSocketFD;
			m_stSocketInfo[iSocketFD].m_socket_type   = enmSocketType_server;

			m_stSocketInfo[iSocketFD].m_peer_address = m_astServer[iIndex].m_iIP;//stSocketAddress.sin_addr.s_addr;
			m_stSocketInfo[iSocketFD].m_peer_port = m_astServer[iIndex].m_nPort;//stSocketAddress.sin_port;
			STRNCPY(m_stSocketInfo[iSocketFD].m_client_address, m_astServer[iIndex].m_szIP, sizeof(m_stSocketInfo[iSocketFD].m_client_address));
			m_stSocketInfo[iSocketFD].m_create_time = time(NULL);

			m_stSocketInfo[iSocketFD].m_lastrecv_timestamp = m_stSocketInfo[iSocketFD].m_create_time;

			m_stSocketInfo[iSocketFD].m_local_port = m_astServer[iIndex].m_nLocalPort;
			m_stSocketInfo[iSocketFD].m_uin = 0;
			m_stSocketInfo[iSocketFD].m_is_recved_data_from_peer = true;
			m_stSocketInfo[iSocketFD].m_is_sended_data_to_peer = true;

		}
		else
		{
			TRACESVR(log_mask_system_error, "[CConnFrame::%s] reconnect to [%s:%d] failed, ret=%d.\n", __FUNCTION__, m_astServer[iIndex].m_szIP, m_astServer[iIndex].m_nPort, ret);
			return fail;
		}
	}
	return success;
}

TcpConnect* CConnFrame::get_connection_to(int32_t iGroupID)
{
	int32_t iIndex = m_stConfig.GetServerIndex(iGroupID);

	if (iIndex < 0 || iIndex >= max_router_server_count)
	{
		return NULL;
	}

	if (m_astTCPConnection[iIndex].GetFD() < 0 ||
		m_astTCPConnection[iIndex].GetStatus() != enmTCPConnectionStatus_connected)
	{
		return NULL;
	}
	return &m_astTCPConnection[iIndex];
}

int32_t CConnFrame::check_self()
{
	time_t tmNow = time(NULL);

	if (tmNow - m_tmLastCheckTime < m_stConfig.m_iCheckSocketInterval)
	{
		return success;
	}

	if (tmNow - m_tmLastStatisticTime >= m_stConfig.m_iLogStatisticInterval)
	{
		//TODO: log statistics
		log_statistics();
		m_tmLastStatisticTime = tmNow;
	}

	for (int32_t i = m_iSocket; i < max_socket_count; ++i) //m_iSocket是以建立的连接的起始位置
	{
		m_pstCurrentSocket = &m_stSocketInfo[i];
		if (enmSocketType_common != m_pstCurrentSocket->m_socket_type)
		{
			continue;
		}

		if (m_pstCurrentSocket->m_socket_fd <= 0)
		{
			continue;
		}

		int32_t inverval = 0;
		//判断此连接是否发送过数据
		if(m_pstCurrentSocket->m_is_sended_data_to_peer)
		{
			inverval = tmNow - m_pstCurrentSocket->m_lastrecv_timestamp;

			if (inverval >= m_stConfig.m_iMaxSocketUnaliveTime)
			{
				TRACESVR(log_mask_detail, "[CConnFrame::%s] client[%s:%d, uin:%d] socket[id:%d], socket array index[%d] on listening port[%d] was timeout, timestamp[%d] so close it!\n",
					__FUNCTION__, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_uin,
					m_pstCurrentSocket->m_socket_fd, i, m_pstCurrentSocket->m_local_port, m_pstCurrentSocket->m_lastrecv_timestamp);


				TRACESVR(log_mask_detail, "[CConnFrame::%s] inverval[%d] = tnNow[%d]- timestamp[%d] \n", __FUNCTION__, inverval, tmNow, m_pstCurrentSocket->m_lastrecv_timestamp);
				clear_current_socket(enmClosedReason_ClientTimeOut);
			}
		}
		else
		{
			inverval = tmNow - m_pstCurrentSocket->m_create_time;
			if (inverval >= m_stConfig.m_iMaxWaitForFirstPackageTime)
			{
				TRACESVR(log_mask_detail, "[CConnFrame::%s] client[%s:%d, uin:%d] socket[id:%d], socket array index[%d] on listening port[%d] was timeout, timestamp[%d] so close it!\n",
					__FUNCTION__, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_uin, m_pstCurrentSocket->m_socket_fd, i,
					m_pstCurrentSocket->m_local_port, m_pstCurrentSocket->m_create_time);

				clear_current_socket(enmClosedReason_InvalidConnection);	//ÎÞÐ§Á´½Ó£¬³¬Ê±Ã»ÓÐ·¢ËÍ¹ýÓÐÐ§µÄÊý¾Ý°ü
			}
		}
	}//end for

	//
	m_tmLastCheckTime = tmNow;

	return success;
}

long GetTimeUS()
{
	struct timeval t_start;
	gettimeofday(&t_start, NULL);

	return t_start.tv_sec*1000000 + t_start.tv_usec;
}

int32_t CConnFrame::working()
{
	TRACESVR("connector server now is working ...\n");
	long	lbegin	= 0;
	long	lend	= 0;

	for(/**/; /**/;)
	{
		check_flag();
		lbegin = GetTimeUS();
		recv_messages();
		lend = GetTimeUS();

		if ( (lend-lbegin) > 10000 )
		{
			TRACESVR(log_mask_system_error, "recv_messages cost time = %d\n",lend-lbegin);
		}

		lbegin = GetTimeUS();
		check_and_send_data();
		lend = GetTimeUS();

		if ( (lend-lbegin) > 10000 )
		{
			TRACESVR(log_mask_system_error, "check_and_send_data cost time = %d\n",lend-lbegin);
		}

		check_self();

		refresh();
		//add by hecb 2012-12-17 begin
		check_config();
		//add by hecb 2012-12-17 end
	}

	return 0;
}

int32_t CConnFrame::reload_config()
{
	struct stat stFileStat;
	if (0 == stat(CONNECT_CONFIG, &stFileStat))
	{
		m_tmConfigLastModifyTime = stFileStat.st_mtime;
	}

	if (success != m_stConfig.LoadConfig(CONNECT_CONFIG))
	{
		TRACESVR(log_mask_system_error, "[CConnFrame::%s] reload config from %s failed.\n", __FUNCTION__, CONNECT_CONFIG);
		return fail;
	}
	else
	{
		TRACESVR(log_mask_system_config, "[CConnFrame::%s] reload config from %s success.\n", __FUNCTION__, CONNECT_CONFIG);
	}

	//重新设置日志等级
	SETTRACELEVEL(m_stConfig.m_iLogLevel);

	for (int16_t i = 0; i < m_nRouterCount; ++ i)
	{
		if (m_astTCPConnection[i].GetStatus() == enmTCPConnectionStatus_connected &&
			m_astTCPConnection[i].GetFD() >= 0)
		{
			int32_t iSocketFD = m_astTCPConnection[i].GetFD();
			if (iSocketFD > 0)
			{
				if (m_stSocketSatistics.m_unSocketCount > 0)
				{
					m_stSocketSatistics.m_unSocketCount--;
				}
				m_astTCPConnection[i].CloseSocket();
				if (iSocketFD < max_socket_count)
				{
					m_stSocketInfo[iSocketFD].m_socket_fd = socket_fd_invalid;
					m_stSocketInfo[iSocketFD].m_peer_address = 0;

					m_stSocketInfo[iSocketFD].m_peer_port = 0;
					m_stSocketInfo[iSocketFD].m_local_port = 0;
					m_stSocketInfo[iSocketFD].m_recv_bytes  = 0;
					m_stSocketInfo[iSocketFD].m_socket_type = enmSocketType_invalid;
					m_stSocketInfo[iSocketFD].m_uin = 0;

					m_stSocketInfo[iSocketFD].m_create_time = 0;
					m_stSocketInfo[iSocketFD].m_lastrecv_timestamp = 0;

					m_stSocketInfo[iSocketFD].m_is_sended_data_to_peer = false;
					m_stSocketInfo[iSocketFD].m_is_recved_data_from_peer = false; //
				}
			}
		}//if
	}//for

	for (int16_t i = 0; i < m_nClientCount; ++ i)
	{
		if (m_astClientConnection[i].GetStatus() == enmTCPConnectionStatus_connected &&
			m_astClientConnection[i].GetFD() >= 0)
		{
			int32_t iSocketFD = m_astClientConnection[i].GetFD();
			if (iSocketFD > 0)
			{
				if (m_stSocketSatistics.m_unSocketCount > 0)
				{
					m_stSocketSatistics.m_unSocketCount--;
				}
				m_astClientConnection[i].CloseSocket();
				if (iSocketFD < max_socket_count)
				{
					m_stSocketInfo[iSocketFD].m_socket_fd = socket_fd_invalid;
					m_stSocketInfo[iSocketFD].m_peer_address = 0;

					m_stSocketInfo[iSocketFD].m_peer_port = 0;
					m_stSocketInfo[iSocketFD].m_local_port = 0;
					m_stSocketInfo[iSocketFD].m_recv_bytes  = 0;
					m_stSocketInfo[iSocketFD].m_socket_type = enmSocketType_invalid;
					m_stSocketInfo[iSocketFD].m_uin = 0;

					m_stSocketInfo[iSocketFD].m_create_time = 0;
					m_stSocketInfo[iSocketFD].m_lastrecv_timestamp = 0;

					m_stSocketInfo[iSocketFD].m_is_sended_data_to_peer = false;
					m_stSocketInfo[iSocketFD].m_is_recved_data_from_peer = false; //
				}
			}
		}//if
	}//for

	m_nRouterCount = m_stConfig.m_nRouterCount;
	m_nClientCount = m_stConfig.m_nRouterCount;
	for (int16_t i = 0; i < m_stConfig.m_nRouterCount; ++ i)
	{
		STRNCPY(m_astServer[i].m_szIP, m_stConfig.m_astRouterServer[i].m_szLogicIP,
			sizeof(m_astServer[i].m_szIP));
		m_astServer[i].m_iIP = m_stConfig.m_astRouterServer[i].m_uiLogicIP;
		m_astServer[i].m_nPort = (int16_t)m_stConfig.m_astRouterServer[i].m_nLogicListenPort;
		m_astServer[i].m_bHaveHello = true;
		m_astServer[i].m_tmLastActiveTime = 0;
		m_astServer[i].m_nLocalPort = (int16_t)m_stConfig.m_astRouterServer[i].m_nLocalPort;

		STRNCPY(m_astClient[i].m_szIP, m_stConfig.m_astRouterServer[i].m_szLogicIP,
			sizeof(m_astClient[i].m_szIP));
		m_astClient[i].m_iIP = m_astServer[i].m_iIP;
		m_astClient[i].m_nPort = (int16_t)m_stConfig.m_astRouterServer[i].m_nLogicPort;
		m_astClient[i].m_bHaveHello = true;
		m_astClient[i].m_tmLastActiveTime = 0;
	}

	for (int16_t i=0; i < m_nRouterCount; ++i)
	{
		m_astTCPConnection[i].Init(Enum_EET_EntityTypes_Logic, 0, m_astServer[i].m_szIP, (u_short)m_astServer[i].m_nPort);
		//add by hecb 2013-05-17
		m_astTCPConnection[i].Set4ByteSize(true);
	}

	for (int16_t i = 0; i < m_nClientCount; ++ i)
	{
		//add by hecb 2013-05-17
		m_astClientConnection[i].Set4ByteSize(true);
	}

	//确保连接会重新连上
	m_tmLastCheckConnection = 0;

	refresh();

	return success;
}

int32_t CConnFrame::load_config()
{
	//add by hecb 2012-12-17 begin
	struct stat stFileStat;
	if (0 == stat(CONNECT_CONFIG, &stFileStat))
	{
		m_tmConfigLastModifyTime = stFileStat.st_mtime;
	}
	//add by hecb 2012-12-17 end

	if (success != m_stConfig.LoadConfig(CONNECT_CONFIG))
	{
		return fail;
	}

	m_nRouterCount = m_stConfig.m_nRouterCount;
	m_nClientCount = m_stConfig.m_nRouterCount;
	for (int16_t i = 0; i < m_stConfig.m_nRouterCount; ++ i)
	{
		STRNCPY(m_astServer[i].m_szIP, m_stConfig.m_astRouterServer[i].m_szLogicIP,
			sizeof(m_astServer[i].m_szIP));
		m_astServer[i].m_iIP = m_stConfig.m_astRouterServer[i].m_uiLogicIP;
		m_astServer[i].m_nPort = (int16_t)m_stConfig.m_astRouterServer[i].m_nLogicListenPort;
		m_astServer[i].m_bHaveHello = true;
		m_astServer[i].m_tmLastActiveTime = 0;
		m_astServer[i].m_nLocalPort = (int16_t)m_stConfig.m_astRouterServer[i].m_nLocalPort;

		STRNCPY(m_astClient[i].m_szIP, m_stConfig.m_astRouterServer[i].m_szLogicIP,
			sizeof(m_astClient[i].m_szIP));
		m_astClient[i].m_iIP = m_astServer[i].m_iIP;
		m_astClient[i].m_nPort = (int16_t)m_stConfig.m_astRouterServer[i].m_nLogicPort;
		m_astClient[i].m_bHaveHello = true;
		m_astClient[i].m_tmLastActiveTime = 0;
	}

	for (int32_t i = 0; i < m_nRouterCount; ++i)
	{
		m_astTCPConnection[i].Init(Enum_EET_EntityTypes_Logic, 0, m_astServer[i].m_szIP, (uint16_t)m_astServer[i].m_nPort);
	}

	return success;
}

void CConnFrame::clear_current_socket(EnumClientCloseReason code)
{
	TRACESVR(log_mask_system_warning, "[CConnFrame::%s] clear client(ip:%s, port:%d, uin:%d) and errorCode = %d\n",
		__FUNCTION__, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_uin, code);

	//非正常的断开连接，则通知业务进程连接中断
	if(enmClosedReason_Service != code 	&& enmClosedReason_InvalidConnection != code && enmClosedReason_InvalidSignature != code)
	{
		CMsgHeadConnector netHead;

		netHead.m_usConnectionIndex = (uint16_t)m_pstCurrentSocket->m_socket_fd;
		netHead.m_byControlType = head_type_disconnect_notify;
		netHead.m_stControlInfo.m_stClientDisconnect.m_iUin = m_pstCurrentSocket->m_uin;
		netHead.m_stControlInfo.m_stClientDisconnect.m_byDisconnectReason = (int8_t)code;
		//add by hecb 2013-01-12 begin
		netHead.m_stControlInfo.m_stClientDisconnect.m_iPlatformID = (int16_t)m_pstCurrentSocket->m_iPlatformID;
		netHead.m_stControlInfo.m_stClientDisconnect.m_iGroupID = (int16_t)m_pstCurrentSocket->m_iGroupID;
		//add by hecb 2013-01-12 end
		netHead.m_iPackageSize = netHead.Size();

		int32_t iOutLength = netHead.Encode(m_szMsgBuf, sizeof(m_szMsgBuf));
		if ( iOutLength < 0)
		{
			return;
		}

		check_server_to(m_pstCurrentSocket->m_iGroupID);
		auto pstTCPConnection = get_connection_to(m_pstCurrentSocket->m_iGroupID);
		if (NULL != pstTCPConnection)
		{
			pstTCPConnection->SendData(m_szMsgBuf, iOutLength);
		}
	}

	if (m_pstCurrentSocket->m_socket_fd > 0)
	{
		if (m_stSocketSatistics.m_unSocketCount > 0)
		{
			m_stSocketSatistics.m_unSocketCount--;
		}
		close(m_pstCurrentSocket->m_socket_fd);
	}

	//reset field value
	m_pstCurrentSocket->m_socket_fd = socket_fd_invalid;
	m_pstCurrentSocket->m_peer_address = 0;
	m_pstCurrentSocket->m_peer_port = 0;
	m_pstCurrentSocket->m_local_port = 0;
	m_pstCurrentSocket->m_recv_bytes  = 0;
	m_pstCurrentSocket->m_socket_type = enmSocketType_invalid;
	m_pstCurrentSocket->m_uin = 0;

	m_pstCurrentSocket->m_create_time = 0;
	m_pstCurrentSocket->m_lastrecv_timestamp = 0;

	m_pstCurrentSocket->m_is_sended_data_to_peer = false;
	m_pstCurrentSocket->m_is_recved_data_from_peer = false; //

	return;
}

int CConnFrame::recv_messages()
{
	int32_t timeout = 2;		//单位:ms
	int fd_event_count = 0;

	fd_event_count = epoll_wait(m_epoll_fd, m_pevents, m_maxfd, timeout);
	if (fd_event_count < 0)
	{
		return fail;
	}

	int sock_fd = -1;
	struct epoll_event *pstEvent = m_pevents;

	for(int i = 0; i < fd_event_count; ++i, pstEvent++)
	{
		sock_fd = pstEvent->data.fd;
		if (sock_fd < 0 || sock_fd >= int(sizeof(m_stSocketInfo)/sizeof(m_stSocketInfo[0])))
		{
			continue;
		}

		m_pstCurrentSocket = &m_stSocketInfo[sock_fd];

		if (0 != (EPOLLERR & pstEvent->events))
		{
			int error_code = 0;
			socklen_t len = (socklen_t)sizeof(error_code);
			getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &error_code, &len);

			TRACESVR(log_mask_system_warning, "[CConnFrame::%s] client[UIN:%d, IP:%s] socket(%d) errno:%d epoll error so closed! and clear socket info\n",
				__FUNCTION__, m_pstCurrentSocket->m_uin, inet_ntoa(*(struct in_addr*)&m_pstCurrentSocket->m_peer_address), sock_fd, error_code);

			if(false == m_pstCurrentSocket->m_is_recved_data_from_peer)
			{
				clear_current_socket(enmClosedReason_InvalidConnection);
			}
			else
			{
				clear_current_socket(enmClosedReason_Client);
			}

			continue;
		}

		if (0 == (EPOLLIN & pstEvent->events))
		{
			continue;
		}

		int32_t accepted_sockfd = -1;
		struct sockaddr_in stSocketAddress;
		socklen_t sockaddress_len = (socklen_t)sizeof(stSocketAddress);
		char szAcceptLogBuf[512] = {'\0',};

		//若是监听端口
		if (enmSocketType_listen == m_pstCurrentSocket->m_socket_type)
		{
			accepted_sockfd = accept(sock_fd, (struct sockaddr *)&stSocketAddress, &sockaddress_len);
			if(0 >= accepted_sockfd)
			{
				sprintf(szAcceptLogBuf, "[CConnFrame::%s] accept client(%s:%d) error, code = %d, msg = %s\n",
					__FUNCTION__, inet_ntoa(stSocketAddress.sin_addr), stSocketAddress.sin_port,
					errno, strerror(errno));

				TRACESVR(log_mask_system_error, szAcceptLogBuf);
				continue;
			}

			//连接统计
			++m_stSocketSatistics.m_unSocketCount;
			++m_stSocketSatistics.m_unSocketAdd;

			if((uint32_t)m_maxfd < m_stSocketSatistics.m_unSocketCount || max_socket_count <= accepted_sockfd)
			{
				TRACESVR(log_mask_system_warning, "[CConnFrame::%s] connection is so many, just refused!\n", __FUNCTION__);
				--m_stSocketSatistics.m_unSocketCount;
				close(accepted_sockfd);
				continue;
			}

			//设置为非阻塞socket
			int flags = 1;
			if ( ioctl(accepted_sockfd, FIONBIO, &flags) &&  ((flags = fcntl(accepted_sockfd, F_GETFL, 0)) < 0
				||  fcntl(accepted_sockfd, F_SETFL, flags | O_NONBLOCK) < 0) )
			{
				TRACESVR(log_mask_system_warning, "[CConnFrame::%s] set accepted socket O_NONBLOCK failed, just close it!\n", __FUNCTION__);
				--m_stSocketSatistics.m_unSocketCount;
				close(accepted_sockfd);
				continue;
			}

			int ret = 0;
			ret = epoll_new_socket(accepted_sockfd);
			if (ret < 0)
			{
				continue;
			}

			//record socket info
			m_stSocketInfo[accepted_sockfd].m_socket_fd   = accepted_sockfd;
			m_stSocketInfo[accepted_sockfd].m_socket_type = enmSocketType_common;

			char *pszTempIP = nullptr;
			pszTempIP = inet_ntoa(stSocketAddress.sin_addr);

			m_stSocketInfo[accepted_sockfd].m_peer_address = stSocketAddress.sin_addr.s_addr;
			m_stSocketInfo[accepted_sockfd].m_peer_port = ntohs(stSocketAddress.sin_port);
			STRNCPY(m_stSocketInfo[accepted_sockfd].m_client_address, pszTempIP,sizeof(m_stSocketInfo[accepted_sockfd].m_client_address));
			m_stSocketInfo[accepted_sockfd].m_create_time = time(NULL);

			m_stSocketInfo[accepted_sockfd].m_lastrecv_timestamp = m_stSocketInfo[accepted_sockfd].m_create_time;


			m_stSocketInfo[accepted_sockfd].m_local_port = m_pstCurrentSocket->m_local_port;
			m_stSocketInfo[accepted_sockfd].m_uin = 0;
			m_stSocketInfo[accepted_sockfd].m_is_recved_data_from_peer = false;
			m_stSocketInfo[accepted_sockfd].m_is_sended_data_to_peer = false;

			auto pstTCPConnection = get_client_connection(m_stSocketInfo[accepted_sockfd].m_peer_address, m_stSocketInfo[accepted_sockfd].m_peer_port);
			if (nullptr != pstTCPConnection)
			{
				pstTCPConnection->AcceptFd(accepted_sockfd);
				pstTCPConnection->Set4ByteSize(true);

				m_stSocketInfo[accepted_sockfd].m_socket_type = enmSocketType_client;

				TRACESVR(log_mask_detail, "[CConnFrame::%s] established one connection[client %s:%d]"
				" on listening port :%d, socket id:%d, type : logic service client\n", __FUNCTION__,
				m_stSocketInfo[accepted_sockfd].m_client_address, m_stSocketInfo[accepted_sockfd].m_peer_port,
				m_stSocketInfo[accepted_sockfd].m_local_port, accepted_sockfd);
			}
			else
			{
				TRACESVR(log_mask_detail, "[CConnFrame::%s] established one connection[client %s:%d]"
				" on listening port :%d, socket id:%d, type : user client\n", __FUNCTION__,
				m_stSocketInfo[accepted_sockfd].m_client_address, m_stSocketInfo[accepted_sockfd].m_peer_port,
				m_stSocketInfo[accepted_sockfd].m_local_port, accepted_sockfd);
			}
		}
		else if (enmSocketType_server == m_pstCurrentSocket->m_socket_type)
		{
			sock_fd = m_pstCurrentSocket->m_socket_fd;
			int32_t offset = 0;

			int32_t iReceived = 0;
			char* pszRecvBuffer = m_pstCurrentSocket->m_package_buffer + offset;
			int32_t buffer_size = (int32_t)sizeof(m_pstCurrentSocket->m_package_buffer) - offset;
			iReceived = socket_recv(sock_fd, pszRecvBuffer, buffer_size);
			if( 0 >= iReceived )
			{
				TRACESVR ("[CConnFrame::%s]  Client[%s, uin:%d] socket id = %d close connect port %d!\n", __FUNCTION__,
					m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_uin, m_pstCurrentSocket->m_socket_fd, m_pstCurrentSocket->m_local_port);

				//add by hecb 2012-12-14 关闭链接
				for (int16_t j = 0; j < m_nRouterCount; ++ j)
				{
					if ((uint32_t)m_astServer[j].m_iIP == m_stSocketInfo[sock_fd].m_peer_address
						&& (uint16_t)m_astServer[j].m_nPort == m_stSocketInfo[sock_fd].m_peer_port)
					{
						m_astTCPConnection[j].CloseSocket();
					}
				}
				//add by hecb 2012-12-14 关闭链接

				if(false == m_pstCurrentSocket->m_is_recved_data_from_peer)
				{
					clear_current_socket(enmClosedReason_InvalidConnection);
				}
				else
				{
					clear_current_socket(enmClosedReason_Client);
				}

				return fail;
			}

			//记录最近一次收到包的时间
			m_pstCurrentSocket->m_lastrecv_timestamp = time(NULL);
			for (int16_t j = 0; j < m_nRouterCount; ++ j)
			{
				if ((uint32_t)m_astServer[j].m_iIP == m_stSocketInfo[sock_fd].m_peer_address
					&& (uint16_t)m_astServer[j].m_nPort == m_stSocketInfo[sock_fd].m_peer_port)
				{
					m_astServer[j].m_tmLastActiveTime = time(NULL);
				}
			}

		}
		else if (enmSocketType_client == m_pstCurrentSocket->m_socket_type)
		{
			TRACESVR ("[CConnFrame::%s]recv data from logic service|%s:%u\n", __FUNCTION__,
					  m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port);

			auto pstTCPConnection = get_client_connection(m_pstCurrentSocket->m_peer_address, m_pstCurrentSocket->m_peer_port);
			if(nullptr != pstTCPConnection)
			{
				pstTCPConnection->RecvData();
				if (pstTCPConnection->GetFD() < 0)
				{
					// peer close
					TRACESVR("[CConnFrame::%s]logic service connection invalid|%s:%u\n", __FUNCTION__,
							 m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port);

					clear_current_socket(enmClosedReason_Client);
					return fail;

				}

				size_t data_size = sizeof(m_szMsgBuf);
				while(1)
				{
					//TRACEBIN(log_mask_detail, m_szMsgBuf, data_size);

					int iRet = pstTCPConnection->GetOneCode(m_szMsgBuf, data_size, sizeof(m_szMsgBuf));
					if(iRet > 0)
					{
						//处理从service进程收到的包
						process_code_from_service(m_szMsgBuf, data_size);
					}
					else
					{
						if(0 != iRet)
						{
							TRACESVR("[CConnFrame::%s]logic service get_one_code failed|%s:%u|%d\n", __FUNCTION__,
									 pstTCPConnection->GetStrIp().c_str(), pstTCPConnection->GetPort(), iRet);
						}

						break;
					}
				}


			}
			else
			{
				TRACESVR("[CConnFrame::%s]logic service is never initialize|%s:%u\n", __FUNCTION__,
						 m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port);
			}

		}
		else //用户客户端连接
		{
			recv_client_messages(sock_fd);
		}
	}//for


	return success;
}

int CConnFrame::check_and_send_data()
{
	return success;
}

int32_t CConnFrame::socket_recv(int fd, char* data, int32_t size)
{
	if (NULL == data || size <= 0)
	{
		return -1;
	}

	ssize_t recved = 0;
	while(true)
	{
		recved = recv(fd, data, size, 0);
		if (0 < recved)
		{
			return recved;
		}
		else
		{
			if (0 > recved &&  EINTR == errno)
			{
				continue;
			}

			return recved;
		}
	}
	return -1;
}

int32_t CConnFrame::transfer_one_code(int32_t iIndex, const char* pszCode, int32_t iCodeSize)
{
	TRACESVR(log_mask_detail, "[CConnFrame::%s] receive code from client [socket idx: %d] size: %d code = \n",
		__FUNCTION__, iIndex, iCodeSize);

	//TRACEBIN(log_mask_detail, (char*)pszCode, iCodeSize);

	/*判断是否第一个数据包，如果成立则需要解码csHead，解码出session key,并保存在stSocketInfo中;
	否则，只需要利用已经保存的session key,解码出sequnce以及messageBody*/

	static char szCodeBuffer[GY_MAX_CLIENT_BUFFER_SIZE + 100];
	CMsgHeadConnector stNetHead;

	//因为netHead的长度受到m_cControlType字段的影响，所以必须要在解密数据并拷贝到目的buffer之前来计算
	if(true == m_pstCurrentSocket->m_is_recved_data_from_peer)
	{
		stNetHead.m_byControlType = head_type_transform_request;
	}
	else
	{
		stNetHead.m_byControlType = head_type_start_request;
	}

	int32_t coded_length = stNetHead.Size();	//先空出填充netHead的部分

	//解码网关头
	CMsgHeadClient stClientProto;
	int iClientProtoRet = stClientProto.Decode(pszCode, iCodeSize);
	if(iClientProtoRet <= 0 || iClientProtoRet > iCodeSize)
	{
		//解码网关头错误，可能是攻击
		TRACESVR(log_mask_system_error, "[CConnFrame::%s] invalid package, gateway packet header length = %d, [socket idx: %d]\n",
				 __FUNCTION__, iClientProtoRet, iIndex);

		if(false == m_pstCurrentSocket->m_is_recved_data_from_peer)
		{
			clear_current_socket(enmClosedReason_InvalidSignature);
		}
		else
		{
			clear_current_socket(enmClosedReason_DataError);	//需要上报给后端service
		}

		return fail;
	}

	//得到socket指针
	m_pstCurrentSocket = &m_stSocketInfo[iIndex];
	int iClientUin = stClientProto.m_stStaticData.m_iUin;
	int iGroupID = stClientProto.m_stStaticData.m_iGroupID;
	int iPlatformID = stClientProto.m_stStaticData.m_iChannelID;

	//判断是否第一个数据包
	if(false == m_pstCurrentSocket->m_is_recved_data_from_peer)	//与后端Service还没有建立联系,不需要通知后端的Service
	{
		//判断code中是否含有session key
		int16_t nOptionalLen = (int16_t)stClientProto.m_stStaticData.m_ucSessionLen;
		if (nOptionalLen <= 0 || nOptionalLen > 128)
		{
			TRACESVR(log_mask_system_error, "[CConnFrame::%s] invalid session length = %d, [socket idx: %d]\n",
				__FUNCTION__, nOptionalLen, iIndex);

			clear_current_socket(enmClosedReason_InvalidSignature);
			return fail;
		}

		int iRet = CGYSessionCreator::Decrypt(m_stConfig.m_szSessionKey,
											  stClientProto.m_szSessionKey,
											  (size_t)stClientProto.m_stStaticData.m_ucSessionLen,
											  m_pstCurrentSocket->m_stPlaySession);
		if(0 != iRet)
		{
			TRACESVR(log_mask_system_warning, "[CConnFrame::%s] invalid signature, session length = %d "
				" [socket idx: %d]. \n", __FUNCTION__, nOptionalLen, iIndex);

			clear_current_socket(enmClosedReason_InvalidSignature);
			return fail;
		}

		//验证session中的uin和group_id是否与gateway头中的uin和group_id一致
		if((m_pstCurrentSocket->m_stPlaySession.m_iGroupID != iGroupID) ||
		   (m_pstCurrentSocket->m_stPlaySession.m_iUin != iClientUin))
		{
			TRACESVR(log_mask_system_warning, "[CConnFrame::%s] uin or group_id error , session.uin=%d, head.uin=%d, "
					 "session.group_id=%d,head.group_id=%d [socket idx: %d] \n",
					 __FUNCTION__, m_pstCurrentSocket->m_stPlaySession.m_iUin, iClientUin,
					 m_pstCurrentSocket->m_stPlaySession.m_iGroupID, iGroupID, iIndex);

			clear_current_socket(enmClosedReason_InvalidSignature);
			return fail;
		}

		//检查签名的有效期限
		time_t iNowTime = 0;
		time(&iNowTime);
		if (iNowTime - (time_t)m_pstCurrentSocket->m_stPlaySession.m_iTimestamp >= m_stConfig.m_nSignatureValidPeriod)
		{
			TRACESVR(log_mask_system_warning, "[CConnFrame::%s] the signature of Uin[%d] was expired! so discarded package! \n",
					 __FUNCTION__, iClientUin);

			clear_current_socket(enmClosedReason_InvalidSignature);
			return fail;
		}

		m_pstCurrentSocket->m_uin = iClientUin;
		m_pstCurrentSocket->m_iGroupID = iGroupID;
		m_pstCurrentSocket->m_iPlatformID = iPlatformID;
	}
	else
	{
		if (iClientUin != m_pstCurrentSocket->m_uin)
		{
			TRACESVR(log_mask_system_warning, "[CConnFrame::%s] uin error, m_pstCurrentSocket->m_uin = %d, nClientUin = %d "
				" so discard the package,  [socket idx: %d].\n", __FUNCTION__, m_pstCurrentSocket->m_uin, iClientUin, iIndex);
			clear_current_socket(enmClosedReason_InvalidConnection);
			return fail;
		}
	}

	if(stClientProto.m_stStaticData.m_usCmd != CMsgHeadClient::ECTCC_HEART_BEAT_CMD)
	{
		//user cmd,forwarding message to logic server
		//接下来处理CS协议部分，首先保存好CS协议包长的指针，因为解密后需要重新计算并改写的
		char *pPackageLen = &szCodeBuffer[coded_length];

		//先将gateway的用户id头拷贝到转发包里
		size_t uiSessionLength = (size_t)stClientProto.m_stStaticData.m_ucSessionLen;
		stClientProto.m_stStaticData.m_ucSessionLen = 0;
		::memcpy(&szCodeBuffer[coded_length], pszCode, sizeof(stClientProto.m_stStaticData));
		szCodeBuffer[coded_length + sizeof(stClientProto.m_stStaticData) - 1] = 0;
		coded_length += sizeof(stClientProto.m_stStaticData);

		//计算数据包的内容
		const char* pszTmp = pszCode + sizeof(TClientProtoStatic) + uiSessionLength;
		int iTmpSize = iCodeSize - sizeof(TClientProtoStatic) - uiSessionLength;
		memcpy(&szCodeBuffer[coded_length], pszTmp, iTmpSize);
		coded_length += iTmpSize;

		//重新计算数据包长度
		CCodeEngine::encode_int32(&pPackageLen, (uint32_t)(iCodeSize - (int)uiSessionLength));

		//准备NetHead
		stNetHead.m_usConnectionIndex = (uint16_t)iIndex;
		if (stNetHead.m_byControlType == head_type_transform_request)
		{
            stNetHead.m_stControlInfo.m_iTime = time(nullptr);
		}
		else
		{
			stNetHead.m_stControlInfo.m_stSourceIPInfo.m_iSourceIP = (int32_t)m_pstCurrentSocket->m_peer_address;
			stNetHead.m_stControlInfo.m_stSourceIPInfo.m_usSourcePort = (int16_t)m_pstCurrentSocket->m_peer_port;
		}
		stNetHead.m_iPackageSize = coded_length;

		//编码到预留的Buffer区域，这里传入sizeof(CMsgHeadConnector)，以跳过Encode里的size判断
		stNetHead.Encode(szCodeBuffer, sizeof(CMsgHeadConnector));

		check_server_to(iGroupID);
		auto pstTCPConnection = get_connection_to(iGroupID);
		if (NULL == pstTCPConnection)
		{
			TRACESVR(log_mask_system_error, "[CConnFrame::%s] do not find logic service Connection group_id = %d.\n",
					 __FUNCTION__, iGroupID);

			return fail;
		}

		int32_t ret = pstTCPConnection->SendData(szCodeBuffer, coded_length);
		if (success != ret)
		{
			TRACESVR(log_mask_system_error, "[CConnFrame::%s] send player uin (%d) code to server failed.\n",
				__FUNCTION__, m_pstCurrentSocket->m_uin);

			return fail;
		}

		TRACESVR(log_mask_detail, "[CConnFrame::%s] get player's uin = %d\n", __FUNCTION__, m_pstCurrentSocket->m_uin);

		TRACESVR(log_mask_detail, "[CConnFrame::%s] and  transfer to service, size: %d code = \n",
				 __FUNCTION__, coded_length);
	}
	else
	{
		//client heart beat
		TRACESVR(log_mask_detail, "[CConnFrame::%s] uin=%d|heart beat\n", __FUNCTION__, m_pstCurrentSocket->m_uin);
	}

	//TRACEBIN(log_mask_detail, szCodeBuffer, coded_length);

	//设置已经从peer收到数据了
	m_pstCurrentSocket->m_is_recved_data_from_peer = true;

	return success;
}

void CConnFrame::transfer_code(int32_t iIndex)
{
	int32_t remained_size = m_pstCurrentSocket->m_recv_bytes;


	char* pPackageStart = m_pstCurrentSocket->m_package_buffer; //°üÆðÊ¼Î»ÖÃ
//		char* pTmp = pPackageStart;

	if (remained_size <= 0)
	{
		return;
	}

	int32_t iPackageSize = 0;

	while (true)
	{
//			if (*pPackageStart != 0)//ÓÐHTTPÍ··â×°
//			{
//				//TRACEBIN(log_mask_detail, &m_pstCurrentSocket->m_package_buffer[0], m_pstCurrentSocket->m_recv_bytes);
//
//				//ÓÃÓÚ´¦Àí²ßÂÔÎÄ¼þÇëÇó   '<policy-file-request/>\n'
//				if('<' == pTmp[0] && 'p' == pTmp[1] && 'o' == pTmp[2] && 'l' == pTmp[3] && 'i' == pTmp[4] 
//				&& 'c' == pTmp[5] && 'y' == pTmp[6] && '-' == pTmp[7] && 23 == remained_size)
//				{
//					TRACESVR(log_mask_detail, "[CConnFrame::%s]   recieve the policy-file-request, size: %d code = \n",
//						__FUNCTION__, remained_size);
//
//					//TRACEBIN(log_mask_detail, pTmp, remained_size);
//
//					socket_send(m_pstCurrentSocket->m_socket_fd, (char *)&flash_policy_file, strlen((const char*)&flash_policy_file) +1);
//
//					//clear_current_socket(enmClosedReason_Service);
//
//					return;
//				}
//				else if (remained_size < int32_t(http_head_length + sizeof(int32_t)))//ÈôÒÑ½ÓÊÕµ½µÄÊý¾Ý´óÐ¡²»×ãÒ»¸öhttpÐ­ÒéÍ·ÒÔ¼°Ó¦ÓÃ²ãÊý¾Ý°ü³¤¶È×Ö¶Î(4B)£¬
//																	//Ôò¼ÌÐøµÈ´ýÊý¾Ý. ÌØ±ð×¢Òâ: ´Ë´¦Ó¦¸Ã¸ù¾ÝÓ¦ÓÃ²ãÐ­ÒéµÄ°ü³¤¶È×Ö¶Î£¬½øÐÐ²»Í¬µÄ´¦Àí
//				{
//					break;
//					//return;
//				}
//				if (remained_size >= 65)
//				{
//					pTmp = pPackageStart + (http_head_length); //Ö¸ÏòhttpÍ·ºóµÄÔªËØ
//					iPackageSize = ntohl(*(int32_t*)pTmp); 
//					//add by hecb 2013-07-05 for QQ
//					TRACESVR(log_mask_detail, "[CConnFrame::%s] http request, recv bytes(%d), package len(%d).\n", 
//						__FUNCTION__, m_pstCurrentSocket->m_recv_bytes, iPackageSize);
//
//					remained_size -= 65;
//					pPackageStart = m_pstCurrentSocket->m_package_buffer + 65;
//					pTmp = pPackageStart;
//					
//					continue;
//				}
//				
//				if (pTmp[-1] !='\n' || pTmp[-2] != '\r' || pTmp[-3] != '\n' || pTmp[-4] !='\r')//httpÍ·±ØÐëÒÔ"\r\n\r\n"½áÎ²
//				{
//					TRACESVR(log_mask_system_error, "[CConnFrame::%s] package has flag of http head, but the " 
//						"head was invalid! so close the socket[%d] ip:port= %s:%d, uin= %d\n", __FUNCTION__, iIndex, 
//						m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_uin);
//					TRACESVR(log_mask_system_error, "the invalid package head = \n");
//					//TRACEBIN(log_mask_system_error, pPackageStart, http_head_length);
//					
//					clear_current_socket(enmClosedReason_DataError);
//					
//					return;
//				}
//				
//				iPackageSize = ntohl(*(int32_t*)pTmp); //ÌØ±ð×¢Òâ: ´Ë´¦Ó¦¸Ã¸ù¾ÝÓ¦ÓÃ²ãÐ­ÒéµÄ°ü³¤¶È×Ö¶Î£¬½øÐÐ²»Í¬µÄ´¦Àí
//
//				//¼Ù¶¨¿Í»§¶Ë·¢À´µÄÊý¾Ý°ü²»³¬¹ý max_client_pkg_size£¡£¡
//				if (iPackageSize >= GY_MAX_CLIENT_BUFFER_SIZE || iPackageSize <= (int32_t)(sizeof(gy_client_proto_static_t)))
//				{
//					TRACESVR(log_mask_system_error, "[CConnFrame::%s] package size was invalid," 
//						" so close the socket[%d] ip:port= %s:%d, uin =%d, iPackageSize = %d\n", __FUNCTION__, iIndex, 
//						m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_uin, iPackageSize);
//
//					clear_current_socket(enmClosedReason_DataError);
//					
//					return;
//				}
//
//
//				if (remained_size - int32_t(http_head_length ) < iPackageSize)//Èô½ÓÊÕµ½µÄÊý¾Ý»¹²»×ãÒÔ×é³ÉÒ»¸öÍêÕûµÄ°ü£¬Ôò¼ÌÐøµÈ´ýÊý¾Ý
//				{
//					//É¾³ýÒÑ¾­´¦Àí¹ýµÄÊý¾Ý£¬°ÑÎ´´¦ÀíµÄÊý¾ÝÒÆµ½½ÓÊÕ»º³åÇøÍ·²¿
//					//memmove(m_pstCurrentSocket->m_szMsgBuf, pTmp, remained_size);
//					//m_pstCurrentSocket->m_iRecvBytes = remained_size; //
//					//return;
//					break;
//				}
//
//				//ÈôÒÑÊÕµ½Ò»¸öÍêÕûµÄÓ¦ÓÃ²ãÊý¾Ý°ü£¬Ôò½øÐÐ×ª·¢
//				if (success == transfer_one_code(iIndex, (const char*)pTmp, (const int32_t)iPackageSize))
//				{
//					//pTmp += iPackageSize; //ÒÆ¶¯Ö¸ÕëÖ¸ÏòÏÂÒ»¸ö´ý×ª·¢µÄÊý¾Ý°ü
//					//remained_size -= iPackageSize;
//					pPackageStart += (http_head_length + iPackageSize);
//					remained_size -= (http_head_length + iPackageSize);
//
//					TRACESVR(log_mask_detail, "[CConnFrame::%s] transfer one code from client[%s:%d] to service, size = %d\n",
//						__FUNCTION__, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, iPackageSize);
//
//					//TRACEBIN(log_mask_detail, pTmp, iPackageSize);
//				}
//				else
//				{
//					//
//					TRACESVR(log_mask_system_error, "[CConnFrame::%s] socket(%d, client ip:%s, uin:%d) transfer_one_code failed!\n", 
//						__FUNCTION__, m_pstCurrentSocket->m_socket_fd, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_uin);
//					//clear_current_socket(enmClosedReason_PushServiceChannelError);
//					
//					return;
//				}
//				
//
//			}//if (*pTmp != 0)//ÓÐHTTPÍ··â×°
//			else//Ã»ÓÐHTTPÍ··â×°
//			{

		if (remained_size < (int32_t)sizeof(int32_t) + 1) //Ä¬ÈÏÊ¹ÓÃ4B±£´æ°ü³¤¶È .ÌØ±ð×¢Òâ: ´Ë´¦Ó¦¸Ã¸ù¾ÝÓ¦ÓÃ²ãÐ­ÒéµÄ°ü³¤¶È×Ö¶Î£¬½øÐÐ²»Í¬µÄ´¦Àí
		{
			//return;		//¼ÌÐøµÈ´ýÊý¾Ý
			break;
		}

		iPackageSize = ntohl(*(int32_t*)pPackageStart); //ÌØ±ð×¢Òâ: ´Ë´¦Ó¦¸Ã¸ù¾ÝÓ¦ÓÃ²ãÐ­ÒéµÄ°ü³¤¶È×Ö¶Î£¬½øÐÐ²»Í¬µÄ´¦Àí

		//¼Ù¶¨¿Í»§¶Ë·¢À´µÄÊý¾Ý°ü²»³¬¹ý max_client_pkg_size£¡£¡
		if (iPackageSize >= m_stConfig.m_iSocketBufferSize || iPackageSize < (int32_t)sizeof(TClientProtoStatic))
		{
			TRACESVR(log_mask_system_error, "[CConnFrame::%s] package size was invalid,iPackageSize = %d"
				" so close the socket[%d] ip:port= %s:%d, uin:%d, package head = \n", __FUNCTION__,iPackageSize, iIndex,
					 m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_uin);

			//TRACEBIN(log_mask_detail, pTmp, sizeof(gy_client_proto_static_t));

			clear_current_socket(enmClosedReason_DataError);

			return;
		}

		if (remained_size < iPackageSize)//Èô½ÓÊÕµ½µÄÊý¾Ý»¹²»×ãÒÔ×é³ÉÒ»¸öÍêÕûµÄ°ü£¬Ôò¼ÌÐøµÈ´ýÊý¾Ý, -1±íÊ¾³ýÈ¥1×Ö½ÚµÄ±êÊ¶·û
		{
			break;
		}

		//ÈôÒÑÊÕµ½Ò»¸öÍêÕûµÄÓ¦ÓÃ²ãÊý¾Ý°ü£¬Ôò½øÐÐ×ª·¢
		if (success == transfer_one_code(iIndex, (const char*)pPackageStart, (const int32_t)iPackageSize))
		{
			//pszTmp += iPackageSize; //ÒÆ¶¯Ö¸ÕëÖ¸ÏòÏÂÒ»¸ö´ý×ª·¢µÄÊý¾Ý°ü
			pPackageStart += iPackageSize;
			remained_size -= iPackageSize;
		}
		else
		{
			TRACESVR(log_mask_system_error, "[CConnFrame::%s] socket(%d, client ip:%s, uin:%d) transfer_one_code failed!\n",
					 __FUNCTION__, m_pstCurrentSocket->m_socket_fd, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_uin);

			return;
		}


//			}//else
	}//end while(true)

	m_pstCurrentSocket->m_recv_bytes = remained_size; //

	//É¾³ýÒÑ¾­´¦Àí¹ýµÄÊý¾Ý£¬°ÑÎ´´¦ÀíµÄÊý¾ÝÒÆµ½½ÓÊÕ»º³åÇøÍ·²¿
	if (pPackageStart != m_pstCurrentSocket->m_package_buffer && m_pstCurrentSocket->m_recv_bytes > 0)
	{
		memmove(m_pstCurrentSocket->m_package_buffer, pPackageStart, remained_size);
		m_pstCurrentSocket->m_recv_bytes = remained_size; //
		//return;
	}

	return ;
}

int32_t CConnFrame::recv_client_messages(int32_t fd)
{
	int32_t sock_fd = m_pstCurrentSocket->m_socket_fd;
	int32_t offset = m_pstCurrentSocket->m_recv_bytes;

	int32_t iReceived = 0;
	char* pszRecvBuffer = m_pstCurrentSocket->m_package_buffer + offset;
	int32_t buffer_size = (int32_t)sizeof(m_pstCurrentSocket->m_package_buffer) - offset;
	iReceived = socket_recv(sock_fd, pszRecvBuffer, buffer_size);
	if( 0 >= iReceived )
	{
		TRACESVR (log_mask_system_warning, "[CConnFrame::%s]  Client[%s, uin:%d] socket id = %d close connect port %d!\n", __FUNCTION__,
			m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_uin, m_pstCurrentSocket->m_socket_fd, m_pstCurrentSocket->m_local_port);

		if(false == m_pstCurrentSocket->m_is_recved_data_from_peer)
		{
			clear_current_socket(enmClosedReason_InvalidConnection);
		}
		else
		{
			clear_current_socket(enmClosedReason_Client);
		}

		return fail;
	}

	m_pstCurrentSocket->m_recv_bytes += iReceived;

	//¼ÇÂ¼×î½üÒ»´ÎÊÕµ½°üµÄÊ±¼ä
	m_pstCurrentSocket->m_lastrecv_timestamp = time(NULL);

	transfer_code(fd);

	check_and_send_data();

	return success;
}

//返回已经发送的字节数
int32_t CConnFrame::socket_send(int fd, char* data, int32_t size)
{
	if (NULL == data || size <= 0)
	{
		return -1;
	}

	int32_t remainded = size;
	int32_t sended = 0;
	int32_t nTime = 0;

	char* pszTmp = data;

	while(remainded > 0)
	{
		//TODO:¼ì²é´Ë´¦µÄ´¦ÀíÂß¼­£¬ÊÇ·ñ»áÔì³ÉÓÉÓÚµ¥¸öÁ¬½Ó¶øÍÏÀÛÕû¸ö·þÎñ
		sended = send(fd, pszTmp, (size_t)remainded, 0);
		if (sended > 0)
		{
			pszTmp += sended;
			remainded -= sended;
		}
		else // sended <= 0
		{
			if (errno != EINTR  || errno != EAGAIN)
			{
				//added by aprilliu, 2008-09-08
				TRACESVR(log_mask_system_warning, "[CConnFrame::%s] send data(size:%d) error, msg = %s\n",
					__FUNCTION__, remainded, strerror(errno));
				break;
			}

		}

		++nTime;
	}

	if (nTime > 1 && remainded == 0)	//È«²¿´«ÊäÍê£¬²¢ÇÒ´«Êä´ÎÊý³¬¹ýÁ½´Î
	{
		TRACESVR(log_mask_system_warning, "[CConnFrame::%s] fd(%d) send data bytes(%d), need %d times. \n",
			__FUNCTION__, fd, size, nTime);
	}

	return (size - remainded);
}

int32_t CConnFrame::open_epoll_socket(uint16_t port)
{
	if (epoll_init() < 0)
	{
		return fail;
	}

	m_iSocket = epoll_socket(AF_INET, SOCK_STREAM, 0);
	if (m_iSocket < 0)
	{
		//destroy epoll
		epoll_destroy();
		return fail;
	}

	int flags = 1;
	struct linger ling = {0, 0};
	setsockopt(m_iSocket, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags));
	setsockopt(m_iSocket, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));
	setsockopt(m_iSocket, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));

	setsockopt(m_iSocket, IPPROTO_TCP, TCP_NODELAY, &flags, sizeof(flags)); //set TCP_CORK

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = 0;
	ret = bind(m_iSocket, (struct sockaddr *) &addr, sizeof(addr));
	if (ret < 0)
	{
		TRACESVR(log_mask_system_error, "[CConnFrame::%s] bind on port :%d error, error msg = %s\n",
			__FUNCTION__, port, strerror(errno));
		epoll_close(m_iSocket);
		epoll_destroy();
		return fail;
	}

	int32_t iOptValue = 0;
	socklen_t iOptLen = 0;

	iOptLen = (socklen_t)sizeof(iOptValue);
	iOptValue = m_stConfig.m_iSocketBufferSize;


	if (0 != setsockopt(m_iSocket, SOL_SOCKET, SO_SNDBUF, (const void *)&iOptValue, iOptLen))
	{
		return fail;
	}

	if (0 != setsockopt(m_iSocket, SOL_SOCKET, SO_RCVBUF, (const void *)&iOptValue, iOptLen))
	{
		return fail;
	}

	ret = listen(m_iSocket, 128);
	if(ret < 0)
	{
		return fail;
	}

	if (epoll_new_socket(m_iSocket) < 0)
	{
		epoll_close(m_iSocket);
		epoll_destroy();
		return fail;
	}

	return success;
}
int32_t CConnFrame::open_epoll_socket(uint16_t port, char* local_ip)
{
	if (epoll_init() < 0)
	{
		return fail;
	}

	m_iSocket = epoll_socket(AF_INET, SOCK_STREAM, 0);
	if (m_iSocket < 0)
	{
		//destroy epoll
		epoll_destroy();
		return fail;
	}

	int flags = 1;
	struct linger ling = {0, 0};
	setsockopt(m_iSocket, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags));
	setsockopt(m_iSocket, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));
	setsockopt(m_iSocket, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));

	int on = 1;

	setsockopt(m_iSocket, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)); //set TCP_CORK


	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (strlen(local_ip) > 0
		&& 0 < inet_pton(AF_INET, local_ip, &(addr.sin_addr)))
	{
			//do nothing
		TRACESVR(log_mask_detail, "[CConnFrame::%s]inet_pton success!\n", __FUNCTION__);
	}
	else
	{
		TRACESVR(log_mask_system_warning,"[CConnFrame::%s]inet_pton error,local_ip=%s\n", __FUNCTION__, local_ip);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	int ret = 0;
	ret = bind(m_iSocket, (struct sockaddr *) &addr, sizeof(addr));
	if (ret < 0)
	{
		TRACESVR(log_mask_system_error, "[CConnFrame::%s] bind on port :%d error, error msg = %s\n",
			__FUNCTION__, port, strerror(errno));
		epoll_close(m_iSocket);
		epoll_destroy();
		return fail;
	}

	int32_t iOptValue = 0;
	socklen_t iOptLen = 0;

	iOptLen = (socklen_t)sizeof(iOptValue);
	iOptValue = m_stConfig.m_iSocketBufferSize;


	if (0 != setsockopt(m_iSocket, SOL_SOCKET, SO_SNDBUF, (const void *)&iOptValue, iOptLen))
	{
		return fail;
	}

	if (0 != setsockopt(m_iSocket, SOL_SOCKET, SO_RCVBUF, (const void *)&iOptValue, iOptLen))
	{
		return fail;
	}

	ret = listen(m_iSocket, 128);
	if(ret < 0)
	{
		return fail;
	}

	if (epoll_new_socket(m_iSocket) < 0)
	{
		epoll_close(m_iSocket);
		epoll_destroy();
		return fail;
	}

	return success;
}
int32_t CConnFrame::epoll_init(void)
{
	if (NULL != m_pevents) //±£Ö¤eventsÖ»±»´´½¨Ò»´Î
	{
		return 0;
	}

	m_maxfd = (int)m_stConfig.m_nMaxfd;

	memset(&m_epoll_event, 0, sizeof(m_epoll_event));
	m_epoll_event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
	m_epoll_event.data.ptr = NULL;
	m_epoll_event.data.fd  = -1;

	m_pevents = (struct epoll_event *) malloc(m_maxfd * sizeof(struct epoll_event));
	if (NULL == m_pevents)
	{
		return -1;
	}

	m_epoll_fd = epoll_create(m_maxfd);
	if (m_epoll_fd < 0)
	{
		return -1;
	}

	return 0;
}

int CConnFrame::epoll_socket(int domain, int type, int protocol)
{
	int fd = socket(domain, type, protocol);
	if (fd < 0)
	{
		return -1;
	}

	int flags = 1;

	if (ioctl(fd, FIONBIO, &flags) && ((flags = fcntl(fd, F_GETFL, 0)) < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0))
	{
		close(fd);
		return -1;
	}

	return fd;
}

int CConnFrame::epoll_new_socket(int fd)
{
	m_epoll_event.data.fd  = fd;
	if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &m_epoll_event) < 0)
	{
		return -1;
	}

	return 0;
}

void CConnFrame::epoll_close(int fd)
{
	close(fd);
	return;
}

void CConnFrame::epoll_destroy(void)
{
	free(m_pevents);
	close(m_epoll_fd);

	return;
}

void CConnFrame::check_flag()
{
	if (g_byRunFlag.is_setted(run_flag_reload))
	{
		g_byRunFlag.unset(run_flag_reload);
		TRACESVR(log_mask_system_error, "[CConnFrame::%s] to reload config from [%s]\n",
				 __FUNCTION__, CONNECT_CONFIG);

		reload_config();

		TRACESVR("[CConnFrame::%s] connectorsvrd reload config ...\n!", __FUNCTION__);
		return;
	}

	if (g_byRunFlag.is_setted(run_flag_exit))
	{
		g_byRunFlag.unset(run_flag_exit);
		TRACESVR("[CConnFrame::%s] connectorsvrd exit ...\n!", __FUNCTION__);
		exit(0);
		return;
	}

}

int32_t CConnFrame::process_code_from_service( const char* pszInCode, const int32_t iInCodeSize )
{
	int32_t  nIndex = 0;

	if(!pszInCode || iInCodeSize <= 0 )
	{
		return fail;
	}

	CMsgHeadConnector stNetHead;
	int16_t nNetHeadLength = stNetHead.Decode(pszInCode, iInCodeSize);
	if (nNetHeadLength < 0)
	{
		TRACESVR(log_mask_system_warning, "[CConnFrame::%s] decode net head failed, just discard package, socket index[%d].\n",
			__FUNCTION__, nIndex);

		return fail;
	}

	if (stNetHead.m_iPackageSize != (uint32_t)iInCodeSize)
	{
		TRACESVR(log_mask_system_warning, "[CConnFrame::%s] the package size decoded from service"
			" was not equal with received size, socket index[%d].\n",	__FUNCTION__, nIndex);
		return fail;
	}

	int32_t iBodySize = stNetHead.m_iPackageSize - nNetHeadLength;
	char* pTmp = (char*)pszInCode + nNetHeadLength;

	nIndex = (int32_t)stNetHead.m_usConnectionIndex;

	if(1 > nIndex || max_socket_count <= nIndex)
	{
		TRACESVR( log_mask_system_error, "[CConnFrame::%s] Invalid socket Index %d !\n",__FUNCTION__, nIndex);
		return fail;
	}

	m_pstCurrentSocket = &m_stSocketInfo[nIndex];/*得到socket指针*/

	switch (stNetHead.m_byControlType)
	{

	case head_type_transform_request:		//普通中转包
	{
		CMsgHeadClient stClientHeader;
		if(stClientHeader.Decode(pTmp, iBodySize) < 0)
		{
			TRACESVR(log_mask_system_warning, "[CConnFrame::%s] transfrom_request decode client header error, "
					 " socket index[%d].\n", __FUNCTION__, nIndex);
			return fail;
		}

		int32_t nClientUin = stClientHeader.m_stStaticData.m_iUin;
		if (nClientUin != m_pstCurrentSocket->m_uin)	//防止Index被复用
		{
			TRACESVR( log_mask_system_warning, "[CConnFrame::%s] uin error,  uin = %d in code, m_pstCurrentSocket->m_uin = %d, "
				" socket index[%d].\n", __FUNCTION__, nClientUin, m_pstCurrentSocket->m_uin, nIndex);

			return fail;
		}

		//普通的中转包或者关闭当前连接的包
		if (0 == stNetHead.m_stControlInfo.m_iTime || m_pstCurrentSocket->m_uin == stNetHead.m_stControlInfo.m_iTime)
		{
			int32_t nEncryptSize = 0;
			char *pEncryptBuf = NULL;
			int32_t iSendBytes = 0;

			pEncryptBuf = pTmp;
			nEncryptSize = iBodySize;

			iSendBytes = socket_send(m_pstCurrentSocket->m_socket_fd, pEncryptBuf, nEncryptSize);
			if (iSendBytes == nEncryptSize)
			{
				TRACESVR(log_mask_detail, "[CConnFrame::%s] send data to client[uin:%d] sended = %d bytes!\n",
					__FUNCTION__, m_pstCurrentSocket->m_uin, iSendBytes);

				m_pstCurrentSocket->m_is_sended_data_to_peer = true; //标识在当前连接上，服务进程已经发送了数据

				//modified by aprilliu, 2008-09-09 修改了与业务层的协议，使用m_stControlInfo.m_iTime字段标识
				//要关闭的连接对应的uin,若字段为0，则不关闭相应的连接
				//if (stNetHead.m_stControlInfo.m_iTime > 0) //若service进程要求关闭连接
				if (stNetHead.m_stControlInfo.m_iTime == m_pstCurrentSocket->m_uin)
				{
					TRACESVR(log_mask_detail, "[CConnFrame::%s] service request to close socket with"
						" client[%s:%d, uin:%d] on Listening Port:%d, socket index[%d]\n",__FUNCTION__,
						m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_uin,
						m_pstCurrentSocket->m_local_port, nIndex);

					//若service进程主动要求connector server关闭连接，则不用通知service 进行
					clear_current_socket(enmClosedReason_Service);
					return success;
				}
				else //若不需要关闭client的连接，则直接返回
				{
					return success;
				}
			}
			else
			{
				TRACESVR(log_mask_system_warning, "[CConnFrame::%s] send error to client[%s:%d, uin:%d] on Listening Port:%d,"
					" socket index[%d], so close it, package size = %d, but sended = %d byte!\n",__FUNCTION__, m_pstCurrentSocket->m_client_address,
					m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_uin, m_pstCurrentSocket->m_local_port, nIndex,
					iBodySize, iSendBytes);

				//¶ÔÓÚÆÕÍ¨µÄÖÐ×ª°ü£¬ÐèÒªÍ¨Öªservice À´ÇåÀíÊý¾Ý
				if (0 == stNetHead.m_stControlInfo.m_iTime)
				{
					clear_current_socket(enmClosedReason_Client);
				}
				else //if(stNetHead.m_stControlInfo.m_iTime == m_pstCurrentSocket->m_uin) ·þÎñÆ÷¹Ø±ÕÁ´½Ó
				{
					clear_current_socket(enmClosedReason_Service);
				}

				return success;
			}
		}
		else
		{
			TRACESVR(log_mask_system_warning, "[CConnFrame::%s] service request to send code to socket(fd:%d, uin:%d) ip:port = %s:%d "
				"but service uin = %d, so just discard the packet!\n", __FUNCTION__, m_pstCurrentSocket->m_socket_fd,
				m_pstCurrentSocket->m_uin, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port,
				(u_int)stNetHead.m_stControlInfo.m_iTime);
		}

		break;
	}

	case head_type_force_close_connection:	//业务进程通知connector server断开与client的连接, 该类型未使用
	{
		TRACESVR(log_mask_detail, "[CConnFrame::%s] service request to close socket with"
			" client[%s:%d, uin:%d] on Listening Port:%d, socket index[%d]\n",__FUNCTION__,
			m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_uin,
			m_pstCurrentSocket->m_local_port, nIndex);

		if (m_pstCurrentSocket->m_peer_address == (uint32_t)stNetHead.m_stControlInfo.m_stDisconnectInfo.m_iSourceIP
			&& m_pstCurrentSocket->m_peer_port == (uint16_t)stNetHead.m_stControlInfo.m_stDisconnectInfo.m_usSourcePort)
		{
			clear_current_socket(enmClosedReason_Service);//若service进程主动要求connector server关闭连接，则不用通知service 进行
		}
		else
		{
			char* pTmpIP = inet_ntoa(*(struct in_addr*)(&stNetHead.m_stControlInfo.m_stDisconnectInfo.m_iSourceIP));
			TRACESVR(log_mask_detail, "[CConnFrame::%s] service request to close socket with"
				" client[%s:%d] on Listening Port:%d, socket index[%d], BUT local info of peer(client:%s, port:%d, uin:%d) not equal, so ignore!!\n",
				__FUNCTION__, pTmpIP, stNetHead.m_stControlInfo.m_stDisconnectInfo.m_usSourcePort, m_pstCurrentSocket->m_local_port,
				nIndex, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_uin);
		}

		return success;

		break;
	}

	default:
		TRACESVR(log_mask_system_warning, "[CConnFrame::%s] send invalid type =%d package"
			" to client[%s:%d] on Listening Port:%d,"
			" socket index[%d], so ignore(discard) it!\n", __FUNCTION__, stNetHead.m_byControlType,
			m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_peer_port, m_pstCurrentSocket->m_local_port, nIndex);

		break;
	}

	return success;
}

void CConnFrame::log_statistics()
{
	time_t tmNow = time(NULL);
	int32_t interval = tmNow - m_tmLastStatisticTime;
	TRACESVR(log_mask_system_performance, "================ statistics ================\n");
	TRACESVR(log_mask_system_performance, "========= connection(%d) in (%d) seconds =========\n",
		m_stSocketSatistics.m_unSocketAdd, interval);

	TRACESVR(log_mask_system_performance, "========= connection total (%d) =========\n",
		m_stSocketSatistics.m_unSocketCount);

	int32_t iLogicCount = 0;
	for (size_t i = 0; i < max_logic_group_id; ++i)
	{
		if (m_stConfig.m_aiServerIndex[i] >= 0)
		{
			TRACESVR(log_mask_system_performance, "========= logic group %d => router index %d =======\n", int32_t(i), m_stConfig.m_aiServerIndex[i]);
			++iLogicCount;
		}
	}
	TRACESVR(log_mask_system_performance, "========= logic group total (%d) =========\n", iLogicCount);

	m_stSocketSatistics.m_unSocketAdd = 0;
}

int32_t CConnFrame::getUniqueID()
{
	static uint16_t g_nUinqueID = 1;

	int32_t iID = (int32_t)g_nUinqueID++ % 10000;
	iID = (iID == 0) ? 1 : iID;

	return (iID);
}

TcpConnect* CConnFrame::get_client_connection(uint32_t uIP, uint16_t nPort)
{
	for (int16_t i = 0; i < m_nClientCount; ++ i)
	{
		if ((uint32_t)m_astClient[i].m_iIP == uIP && (uint16_t)m_astClient[i].m_nPort == nPort)
		{
			return &m_astClientConnection[i];
		}
	}
	return nullptr;
}

int32_t	CConnFrame::check_config()
{
	time_t tmNowTime = time(NULL);
	if (tmNowTime - m_tmLastCheckConfigTime < (long)1_min)
	{
		return success;
	}
	m_tmLastCheckConfigTime = tmNowTime;

	struct stat stFileStat;
	if (0 == stat(CONNECT_CONFIG, &stFileStat))
	{
		if (m_tmConfigLastModifyTime != stFileStat.st_mtime)
		{
			//保存最后修改时间
			m_tmConfigLastModifyTime = stFileStat.st_mtime;

			if (success == reload_config())
			{
				TRACESVR(log_mask_system_warning, "[CConnFrame::%s] reload config from (%s) success!\n",
						 __FUNCTION__, CONNECT_CONFIG);
			}
			else
			{
				TRACESVR(log_mask_system_warning, "[CConnFrame::%s] reload config from (%s) failed!\n",
						 __FUNCTION__, CONNECT_CONFIG);
			}
		}//if
	}//if

	return success;
}
