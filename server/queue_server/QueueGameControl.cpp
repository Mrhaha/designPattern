#include "QueueAppConfigZookeeper.h"
#include "QueueAppPlatform.h"
#include "QueueGameControl.h"
#include "QueueProcessor.h"
#include "QueueServerConfig.h"
#include "common_config.h"
#include "common_tools.h"
#include "server_cmd_cross_define.h"
#include "common_msg_head_client.h"

CQueueAppConfig CQueueGameControl::m_appCfg;//系统配置档
CQueueAppConfigZookeeper CQueueGameControl::m_stZookeeper;
bool CQueueGameControl::s_bLoadZooConfgSuccess = false;
CQueueRouterManager CQueueGameControl::m_stRouterManager;
char m_szVerInfo[200];

CFlag g_byRunFlag;
CQueueGameControl* g_pQueueGameCtrl = nullptr;

CQueueGameControl::CQueueGameControl(void)
{
	memset(m_szVerInfo, 0, sizeof(m_szVerInfo));
}

CQueueGameControl::~CQueueGameControl(void)
{
    epoll_destroy();
    m_stMsgQueue.stop();
}

const char* CQueueGameControl::GetVerInfo()
{
    if (strlen(m_szVerInfo) == 0)
	{
		sprintf(m_szVerInfo,"%s  %s!\n", VERSION_INFO,  get_build_info().c_str());
	}

	return m_szVerInfo;
}

CQueueGameControl* CQueueGameControl::GetInstance()
{
	//装载配置文件.
	if (!CQueueGameControl::m_appCfg.Load(APPCONFIG_FILENAME))
	{
		printf("load config file['%s'] raise error!\n", APPCONFIG_FILENAME);
		return nullptr;
	}
	
	CQueueGameControl* pNewCtrl = new CQueueGameControl();
	if (nullptr == pNewCtrl)
	{
		printf("new CQueueGameControl failed.\n");
	}

	return pNewCtrl;
}

int32_t CQueueGameControl::epoll_init()
{
    if (NULL != m_pevents)
    {
        epoll_destroy();
        return -1;
    }

    m_epoll_max_events = m_appCfg.m_iMaxClientNum + SERVER_RESERVED_CONNECTION_NUM;

    memset(&m_epoll_event, 0, sizeof(m_epoll_event));
    m_epoll_event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    m_epoll_event.data.ptr = NULL;
    m_epoll_event.data.fd  = -1;

    m_pevents = (struct epoll_event *) malloc(m_epoll_max_events * sizeof(struct epoll_event));
    if (NULL == m_pevents)
    {
        return -1;
    }

    m_epoll_fd = epoll_create(m_epoll_max_events);
    if (m_epoll_fd < 0)
    {
        epoll_destroy();
        return -1;
    }

    return 0;
}

bool CQueueGameControl::epoll_new_socket(int fd)
{
    m_epoll_event.data.fd = fd;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &m_epoll_event) < 0)
    {
        return false;
    }

    return true;
}

bool CQueueGameControl::epoll_op_socket(int fd, int op, __uint32_t events)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = events | EPOLLERR | EPOLLET;

    if(epoll_ctl(m_epoll_fd, op, fd, &ev) < 0)
    {
        return false;
    }

    return true;
}

void CQueueGameControl::epoll_destroy(void)
{
    if(nullptr != m_pevents)
    {
        free(m_pevents);
        m_pevents = nullptr;
    }

    close(m_epoll_fd);

    return;
}

bool CQueueGameControl::CheckPath()
{
	DIR* dpBill = opendir(PATH_BILL);
	if(nullptr == dpBill)
	{
		printf("the dir(='%s') is not exist..\n",PATH_BILL);
		return false;
	}
	else
	{
		closedir(dpBill);
	}

	dpBill = opendir(m_appCfg.m_LogParam.m_szLogPath);
	if(nullptr == dpBill)
	{
		printf("the dir(='%s') is not exist..\n",m_appCfg.m_LogParam.m_szLogPath);
		return false;
	}
	else
	{
		closedir(dpBill);
	}

	dpBill = opendir("./config");
	if(nullptr == dpBill)
	{
		printf("the dir(='./config') is not exist..\n");
		return false;
	}
	else
	{
		closedir(dpBill);
	}

	return true;
}

bool CQueueGameControl::Initialize()
{
	if(!CheckPath())
	{
		return false;
	}

	//初始化log
	char szLogFile[MAX_FILEPATHLEN] = {0};
	sprintf(szLogFile, "%s/%s", m_appCfg.m_LogParam.m_szLogPath, APPLOG_PREFIX_FILENAME);

	TRACESVR(log_mask_initialize, "[CQueueGameControl::Initialize] Verson Infomation:%s, log init success!\n", this->GetVerInfo());
	SETLOGMASK(m_appCfg.m_LogParam.m_nLogMask);

	//初始化Zookeeper
	if (!InitZookeeper())
	{
		TRACESVR(log_mask_initialize, "[CQueueGameControl::Initialize] init zookeeper fail!!!\n");
		return false;
	}

    // epoll init
    if(epoll_init() < 0)
    {
        TRACESVR(log_mask_initialize, "[CQueueGameControl::Initialize] init epoll fail!!!\n");
        return false;
    }

    //queue_config
    std::shared_ptr<CQueueServerConfig> queue_config;
    CQueueServerConfig::GetConfig(queue_config);

	if (0 != m_stRouterManager.m_stListenSocket.open_as_server(queue_config->GetZooQueueConfig().m_stQueueConfig.m_iPort,
	        queue_config->GetZooQueueConfig().m_stQueueConfig.m_strHost.c_str()))
	{
		TRACESVR(log_mask_initialize, "[CQueueGameControl::Initialize] open listen port fail!\n");
		return false;
	}

    if(epoll_new_socket(m_stRouterManager.m_stListenSocket.get_fd()) == false)
    {
        epoll_destroy();
        m_stRouterManager.m_stListenSocket.close_socket();

        TRACESVR(log_mask_initialize, "[CQueueGameControl::Initialize] epoll listen socket fail!\n");
        return false;
    }

	m_tmLastCheckRouterHeartBeat = time(nullptr);
    m_tmLastCheckClientSocketTime = m_tmLastCheckRouterHeartBeat;
	m_tmLastCheckConfigTime = m_tmLastCheckRouterHeartBeat;
	m_tmLastActiveProcessTime = m_tmLastCheckRouterHeartBeat;

	char szLogFileName[MAX_FILEPATHLEN] = {0};
	sprintf(szLogFileName, "%s/%s", m_appCfg.m_LogParam.m_szLogPath, CQueueProcessor::GetProcessName());

    // 告警线程初始化，ID固定 5
    const int32_t iAlertProID = 5;
    if(!m_stAlertProcessor.Initialize(m_appCfg.m_LogParam.m_nLogMask, (const char*)szLogFileName, iAlertProID))
    {
        TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] AlertProcessor[ID=%d] Init error.\n", __FUNCTION__, iAlertProID);
        return false;
    }

	// 消息线程初始化
    int32_t iMsgProID = 10;
    for(auto& msgPro : m_stMsgProcessor)
    {
        if (!msgPro.Initialize(m_appCfg.m_LogParam.m_nLogMask, (const char*)szLogFileName, iMsgProID))
        {
            TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] MsgProcessor[ID=%d] Init error.\n", __FUNCTION__, iMsgProID);
            return false;
        }

        iMsgProID += 10;
    }

	return true;
}
 
void CQueueGameControl::CheckTimers()
{
	time_t tNow = time(nullptr);

	if ((tNow - m_tmLastActiveProcessTime) >= 5)
    {
        ActiveAllProcessor();
        m_tmLastActiveProcessTime = tNow;
    }

	if ((tNow - m_tmLastCheckRouterHeartBeat) >= CHECK_ROUTER_INTERVAL)
	{
		CheckConnectionAlive();
		m_tmLastCheckRouterHeartBeat = tNow;
	}

    if((tNow - m_tmLastCheckClientSocketTime) >= m_appCfg.m_iCheckSocketInterval)
    {
        CheckClientAlive();
        m_tmLastCheckClientSocketTime = tNow;
    }

	if (tNow - m_tmLastCheckConfigTime >= 5)
	{
		m_stZookeeper.UpdateConfig();
		m_tmLastCheckConfigTime = tNow;
	}
}

void CQueueGameControl::CheckConnectionAlive()
{
    // 检查服务器连接
    auto iter = m_stRouterManager.m_astConnectionMap.begin();
    auto iterEnd = m_stRouterManager.m_astConnectionMap.end();
    for (; iter != iterEnd; )
    {
        iter->second.CheckAlive();
        if (false == iter->second.BeConnected())
        {
            TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] logic too long alive, close peer(%d:%d), ip(%s:%d)\n",
                     __FUNCTION__, iter->second.GetEntityType(), iter->second.GetEntityId(), iter->second.GetStrIp().c_str(), iter->second.GetPort());

            m_stRouterManager.m_astConnectionMap.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

void CQueueGameControl::CheckClientAlive()
{
    time_t tmNow = time(NULL);

    // 检查客户端连接
    auto iter = m_stRouterManager.m_stClientConMap.begin();
    auto iterEnd = m_stRouterManager.m_stClientConMap.end();
    for (; iter != iterEnd; )
    {
        //判断此连接是否发送过数据
        if(iter->second.m_is_sended_data_to_peer)
        {
            if ((tmNow - iter->second.m_lastrecv_timestamp) >= m_appCfg.m_iMaxSocketUnaliveTime)
            {
                TRACESVR(log_mask_detail, "[CQueueGameControl::%s] client[%s:%d, uin:%d] socket[id:%d], socket was timeout, timestamp[%d] so close it!\n",
                         __FUNCTION__, iter->second.m_client_address, iter->second.m_peer_port, iter->second.m_uin, iter->second.m_socket_fd, (int32_t)iter->second.m_lastrecv_timestamp);

                m_stRouterManager.m_stClientConMap.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }
        else
        {
            if ((tmNow - iter->second.m_create_time) >= m_appCfg.m_iMaxWaitForFirstPackageTime)
            {
                TRACESVR(log_mask_detail, "[CQueueGameControl::%s] client[%s:%d, uin:%d] socket[id:%d], socket was timeout, timestamp[%d] so close it!\n",
                         __FUNCTION__, iter->second.m_client_address, iter->second.m_peer_port, iter->second.m_uin, iter->second.m_socket_fd, (int32_t)iter->second.m_create_time);

                m_stRouterManager.m_stClientConMap.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }
    }//end for
}

bool CQueueGameControl::Run()
{
	//启动Handle
	TRACESVR(log_mask_initialize,"[CQueueGameControl::Run]enter...\n");

	for(auto& msgPro : m_stMsgProcessor)
    {
        if(!msgPro.Start())
        {
            TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] MsgProcessor(id:%d) start failed!\n", __FUNCTION__, msgPro.GetProcessID());
            return false;
        }
    }

    if(!m_stAlertProcessor.Start())
    {
        TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] AlertProcessor(id:%d) start failed!\n", __FUNCTION__, m_stAlertProcessor.GetProcessID());
        return false;
    }

	TRACESVR(log_mask_initialize, "[CQueueGameControl::Run]start son processors!\n");

	//进入消息循环...
	while(CheckServerContext())
	{
		CheckTimers();

		//分发消息
		DispatchMessage();

        // 发送客户端消息
        SendClientMsg();
	}

	TRACESVR(log_mask_system_warning, "[CQueueGameControl::Run]exit...\n");
	return true;
}

void CQueueGameControl::SendClientMsg()
{
    static int64_t lStartMicroSecond = 0;
    static int64_t lCostMicroSecond = 0;

    lStartMicroSecond = now_msec();
    static int32_t iNum = 0;

    int32_t iMsgNum = m_stMsgQueue.size();
    if(iMsgNum <= 0)
        return;

    iNum = iMsgNum;

    std::optional<TQueueMsgData> opt;
    while (iMsgNum > 0 && ((opt = m_stMsgQueue.pop_front()).has_value()))
    {
        --iMsgNum;
        auto& msg = opt.value();

        if(msg.m_stHeadInfo.m_iMsgSrcType == EQMST_Logic)
        {
            // 服务器消息
            auto iterSocket = m_stRouterManager.m_astConnectionMap.find(msg.m_stHeadInfo.m_iFd);
            if(iterSocket == m_stRouterManager.m_astConnectionMap.end() || iterSocket->second.GetEntityId() != msg.m_stHeadInfo.m_iServerID)
            {
                TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] logic socket error. from fd(%d) or server(%d)\n",
                         __FUNCTION__, msg.m_stHeadInfo.m_iFd, msg.m_stHeadInfo.m_iServerID);
                continue;
            }

            iterSocket->second.SendData(msg.m_strBody.c_str(), msg.m_strBody.length());
        }
        else if(msg.m_stHeadInfo.m_iMsgSrcType == EQMST_Client)
        {
            // 客户端消息
            auto iterSocket = m_stRouterManager.m_stClientConMap.find(msg.m_stHeadInfo.m_iFd);
            if(iterSocket == m_stRouterManager.m_stClientConMap.end())
                continue;

            if(iterSocket->second.m_uin != msg.m_stHeadInfo.m_iUin || iterSocket->second.m_iGroupID != msg.m_stHeadInfo.m_iGroupID)
                continue;

            if(socket_send(msg.m_stHeadInfo.m_iFd, msg.m_strBody.c_str(), msg.m_strBody.length()) != int(msg.m_strBody.length()))
            {
                TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] send client message failed. from fd(%d) or player(%d,%d)\n",
                         __FUNCTION__, msg.m_stHeadInfo.m_iFd, msg.m_stHeadInfo.m_iUin, msg.m_stHeadInfo.m_iGroupID);

                m_stRouterManager.ClearClientSocket(msg.m_stHeadInfo.m_iFd, enmClosedReason_SendBuffError);
            }

            iterSocket->second.m_is_sended_data_to_peer = true;
        }
    }

    lCostMicroSecond = now_msec() - lStartMicroSecond;

    if (lCostMicroSecond > 100)
    {
        TRACESVR(log_mask_system_error, "[CQueueGameControl:%s] process message too slow! COST %ld ms. msg_num:%d!\n",
                 __FUNCTION__, lCostMicroSecond, iNum);
    }
}

void CQueueGameControl::After_Run()
{
    TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] RUNMODE_QUIT...start\n", __FUNCTION__);

    for(auto& msgPro : m_stMsgProcessor)
    {
        if (!msgPro.Stop())
        {
            TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] CQueueMsgProcessor(id=%d) Stop return false!\n", __FUNCTION__, msgPro.GetProcessID());
        }
    }

	if (!m_stAlertProcessor.Stop())
	{
		TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] CQueueAlertProcessor(id=%d) Stop return false!\n", __FUNCTION__, m_stAlertProcessor.GetProcessID());
	}

    epoll_destroy();

    m_stMsgQueue.stop();

    TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] RUNMODE_QUIT...end\n", __FUNCTION__);
}

bool CQueueGameControl::InitZookeeper()
{	
	m_stZookeeper.SetZookeeperInfo(m_appCfg.m_szZookeeperHost, m_appCfg.m_szZookeeperConfigPath);

	for (size_t i = 0; i < CQueueServerConfig::GetConfigSize(); ++i)
	{
		m_stZookeeper.AddWatchFile(CQueueServerConfig::GetConfigLoader()[i].m_pFileName);
	}

	if (!m_stZookeeper.InitZookeeper())
	{
		TRACESVR(log_mask_system_error, "ZOOKEEPER INIT FAILED!!!\n");
		return false;
	}

	const CQueueAppConfigZookeeper::reload_func_type stLoadConfig = std::bind(&CQueueGameControl::LoadQueueConfig, this, std::placeholders::_1);

	m_stZookeeper.RegsterObserver(stLoadConfig);

	if (!m_stZookeeper.WaitAllConfigReady(-1))
	{
		TRACESVR(log_mask_system_error, "ZOOKEEPER FIRST LOAD CONFIG FAILED!!!\n");
		return false;
	}

	if (!s_bLoadZooConfgSuccess)
	{
		TRACESVR(log_mask_system_error, "ZOOKEEPER FIRST LOAD CONFIG FAILED!!!\n");
		return false;
	}

	return true;
}

bool CQueueGameControl::LoadQueueConfig(const CQueueAppConfigZookeeper& stZookeeper)
{
	static int64_t lStartMicroSecond = 0;
    static int64_t lEndMicroSecond = 0;
    static int64_t lCostMicroSecond = 0;

    lStartMicroSecond = now_msec();

	std::shared_ptr<CQueueServerConfig> pConfig = std::make_shared<CQueueServerConfig>();

	std::pair<bool, std::string> stRet(true, "OK");
	for (size_t i = 0; i < CQueueServerConfig::GetConfigSize(); ++i)
	{
		const auto& stConfigLoader = CQueueServerConfig::GetConfigLoader()[i];
		auto pParser = stConfigLoader.m_pParserGetFunc(pConfig);

		stRet = pParser->Load(stZookeeper.GetNormalConfig(stConfigLoader.m_pFileName));

		if (stRet.first == false)
		{
			TRACESVR(log_mask_system_error, "[CQueueGameControl:%s] PARSE CONFIG %s ERROR|MSG:%s\n", __FUNCTION__, stConfigLoader.m_pFileName, stRet.second.c_str());

			s_bLoadZooConfgSuccess = false;
			return false;
		}
	}

	lEndMicroSecond = now_msec();
	lCostMicroSecond = lEndMicroSecond - lStartMicroSecond;

	TRACESVR(log_mask_system_error, "[CQueueGameControl:%s]PARSE CONFIG SUCCESS!COST %ld ms\n", __FUNCTION__, lCostMicroSecond);

    CQueueServerConfig::SetConfig(pConfig);
	s_bLoadZooConfgSuccess = true;

	return true;
}

int32_t CQueueGameControl::recv_client_messages(TSocketInfo& stSocket)
{
    int32_t sock_fd = stSocket.m_socket_fd;
    int32_t offset = stSocket.m_recv_bytes;

    int32_t iReceived = 0;
    char* pszRecvBuffer = stSocket.m_package_buffer + offset;
    int32_t buffer_size = (int32_t)sizeof(stSocket.m_package_buffer) - offset;
    iReceived = socket_recv(sock_fd, pszRecvBuffer, buffer_size);
    if( 0 >= iReceived )
    {
        TRACESVR (log_mask_system_warning, "[CQueueGameControl::%s]  Client[%s, uin:%d] socket id = %d close connect!\n", __FUNCTION__,
                 stSocket.m_client_address, stSocket.m_uin, stSocket.m_socket_fd);

        m_stRouterManager.ClearClientSocket(sock_fd, enmClosedReason_Client);
        return fail;
    }

    stSocket.m_recv_bytes += iReceived;

    stSocket.m_lastrecv_timestamp = time(NULL);

    transfer_code(stSocket);

    return success;
}

void CQueueGameControl::transfer_code(TSocketInfo& stSocket)
{
    int32_t remained_size = stSocket.m_recv_bytes;
    char* pPackageStart = stSocket.m_package_buffer;
    if (remained_size <= 0)
    {
        return;
    }

    int32_t iPackageSize = 0;
    while (true)
    {
        if (remained_size < (int32_t)sizeof(int32_t) + 1)
        {
            break;
        }

        iPackageSize = ntohl(*(int32_t*)pPackageStart);

        if (iPackageSize >= m_appCfg.m_iMaxClientSocketBuffSize || iPackageSize < (int32_t)sizeof(TClientProtoStatic))
        {
            TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] package size was invalid, iPackageSize = %d so close the socket ip:port(%s:%d), uin:%d\n",
                     __FUNCTION__, iPackageSize, stSocket.m_client_address, stSocket.m_peer_port, stSocket.m_uin);

            m_stRouterManager.ClearClientSocket(stSocket.m_socket_fd, enmClosedReason_DataError);
            return;
        }

        if (remained_size < iPackageSize)
        {
            break;
        }

        // 处理一条消息
        if (success == transfer_one_code(stSocket, (const char*)pPackageStart, (const int32_t)iPackageSize))
        {
            // 更新缓冲池
            pPackageStart += iPackageSize;
            remained_size -= iPackageSize;
        }
        else
        {
            TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] socket(%d, client ip:%s, uin:%d) transfer_one_code failed!\n",
                     __FUNCTION__, stSocket.m_socket_fd, stSocket.m_client_address, stSocket.m_uin);

            return;
        }
    }//end while(true)

    stSocket.m_recv_bytes = remained_size;

    // 如果有未处理完buff，移动到缓存头位置
    if (pPackageStart != stSocket.m_package_buffer && stSocket.m_recv_bytes > 0)
    {
        memmove(stSocket.m_package_buffer, pPackageStart, remained_size);
        stSocket.m_recv_bytes = remained_size;
    }

    return ;
}

int32_t CQueueGameControl::socket_recv(int fd, char* data, int32_t size)
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

int32_t CQueueGameControl::socket_send(int fd, const char* data, int32_t size)
{
    if (NULL == data || size <= 0)
    {
        return -1;
    }

    int32_t remainded = size;
    int32_t sended = 0;
    int32_t nTime = 0;

    const char* pszTmp = data;

    while(remainded > 0)
    {
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
                TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] send data(size:%d) error, msg = %s\n", __FUNCTION__, remainded, strerror(errno));
                break;
            }

        }

        ++nTime;
    }

    if (nTime > 1 && remainded == 0)
    {
        TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] fd(%d) send data bytes(%d), need %d times\n", __FUNCTION__, fd, size, nTime);
    }

    return (size - remainded);
}

bool CQueueGameControl::AddMessageQueueProcessor(const TQueueMsgData& stMsgData)
{
    if(stMsgData.m_stHeadInfo.m_iServerID <= 0 || stMsgData.m_stHeadInfo.m_iFd < 0 || stMsgData.m_strBody.empty())
        return false;

    const int32_t iIndex = stMsgData.m_stHeadInfo.m_iServerID % MSG_PROCESSOR_NUM;

    switch (stMsgData.m_stHeadInfo.m_usCmd)
    {
        // 客户端消息
    case MSG_QUEUE_USER_QUEUE_HEARTBEAT:
        if(stMsgData.m_stHeadInfo.m_iMsgSrcType != EQMST_Client)
        {
            TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] process type error, cmd=%d type=%d\n", __FUNCTION__, stMsgData.m_stHeadInfo.m_usCmd, stMsgData.m_stHeadInfo.m_iMsgSrcType);
            return false;
        }
        break;

        // logic 消息
    case MSG_QUEUE_USER_LOGIN_NOTIFY:
    case MSG_QUEUE_USER_LIST_ONLINE_NOTIFY:
    case MSG_QUEUE_USER_RECONNECT_CHECK:
        if(stMsgData.m_stHeadInfo.m_iMsgSrcType != EQMST_Logic)
        {
            TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] process type error, cmd=%d type=%d\n", __FUNCTION__, stMsgData.m_stHeadInfo.m_usCmd, stMsgData.m_stHeadInfo.m_iMsgSrcType);
            return false;
        }
        break;

    default:
        TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] process error, can't found processor to do %d message\n", __FUNCTION__, stMsgData.m_stHeadInfo.m_usCmd);
        return false;
    }

    // 加入对应线程的消息队列
    m_stMsgProcessor[iIndex].AddMsgQueue(stMsgData);

    TRACESVR(log_mask_detail, "[CQueueGameControl::%s] AddMsgQueue code (server=%d) to processor id:%d.\n", __FUNCTION__, stMsgData.m_stHeadInfo.m_iServerID, m_stMsgProcessor[iIndex].GetProcessID());

    return true;
}

int32_t CQueueGameControl::transfer_one_code(TSocketInfo& stSocket, const char* pszCode, int32_t iCodeSize)
{
    TRACESVR(log_mask_detail, "[CQueueGameControl::%s] receive code from client [socket ip:%s uin:%d] size:%d\n",
             __FUNCTION__, stSocket.m_client_address, stSocket.m_uin, iCodeSize);

    /*判断是否第一个数据包，如果成立则需要解码csHead，解码出session key*/
    //解码网关头
    CMsgHeadClient stClientProto;
    int iClientProtoRet = stClientProto.Decode(pszCode, iCodeSize);
    if(iClientProtoRet <= 0 || iClientProtoRet > iCodeSize)
    {
        //解码网关头错误，可能是攻击
        TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] invalid package, gateway packet header length = %d, [socket id: %d]\n",
                 __FUNCTION__, iClientProtoRet, stSocket.m_socket_fd);

        m_stRouterManager.ClearClientSocket(stSocket.m_socket_fd, enmClosedReason_DataError);

        return fail;
    }

    int iClientUin = stClientProto.m_stStaticData.m_iUin;
    int iGroupID = stClientProto.m_stStaticData.m_iGroupID;

    //判断是否第一个数据包
    if(false == stSocket.m_is_recved_data_from_peer)	//与后端Service还没有建立联系
    {
        //判断code中是否含有session key
        int16_t nOptionalLen = (int16_t)stClientProto.m_stStaticData.m_ucSessionLen;
        if (nOptionalLen <= 0 || nOptionalLen > 128)
        {
            TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] invalid session length = %d, [socket id: %d]\n",
                     __FUNCTION__, nOptionalLen, stSocket.m_socket_fd);

            m_stRouterManager.ClearClientSocket(stSocket.m_socket_fd, enmClosedReason_InvalidSignature);
            return fail;
        }

        CGYSessionCreator::gy_session_t stPlaySession;
        int iRet = CGYSessionCreator::Decrypt(m_appCfg.m_szQueueSvrSessionKey, stClientProto.m_szSessionKey,
                                              (size_t)stClientProto.m_stStaticData.m_ucSessionLen, stPlaySession);
        if(0 != iRet)
        {
            TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] invalid signature, session length = %d [socket id: %d]. \n",
                     __FUNCTION__, nOptionalLen, stSocket.m_socket_fd);

            m_stRouterManager.ClearClientSocket(stSocket.m_socket_fd, enmClosedReason_InvalidSignature);
            return fail;
        }

        //验证session中的uin和group_id是否与gateway头中的uin和group_id一致
        if((stPlaySession.m_iGroupID != iGroupID) || (stPlaySession.m_iUin != iClientUin))
        {
            TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] uin or group_id error , session.uin=%d, head.uin=%d, session.group_id=%d,head.group_id=%d [socket idx: %d] \n",
                     __FUNCTION__, stPlaySession.m_iUin, iClientUin, stPlaySession.m_iGroupID, iGroupID, stSocket.m_socket_fd);

            m_stRouterManager.ClearClientSocket(stSocket.m_socket_fd, enmClosedReason_InvalidSignature);
            return fail;
        }

        //检查签名的有效期限
        time_t iNowTime = 0;
        time(&iNowTime);
        if (iNowTime - (time_t)stPlaySession.m_iTimestamp >= m_appCfg.m_nSignatureValidPeriod)
        {
            TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] the signature of Uin[%d] was expired! so discarded package! \n",
                     __FUNCTION__, iClientUin);

            m_stRouterManager.ClearClientSocket(stSocket.m_socket_fd, enmClosedReason_InvalidSignature);
            return fail;
        }

        stSocket.m_uin = iClientUin;
        stSocket.m_iGroupID = iGroupID;
    }
    else
    {
        if (iClientUin != stSocket.m_uin)
        {
            TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] uin error, m_uin = %d, nClientUin = %d so discard the package, [socket id: %d]\n",
                     __FUNCTION__, stSocket.m_uin, iClientUin, stSocket.m_socket_fd);

            m_stRouterManager.ClearClientSocket(stSocket.m_socket_fd, enmClosedReason_InvalidConnection);
            return fail;
        }
    }

    if(stClientProto.m_stStaticData.m_usCmd != CMsgHeadClient::ECTCC_HEART_BEAT_CMD)
    {
        const char* pszTmp = pszCode + iClientProtoRet;
        int iTmpSize = iCodeSize - iClientProtoRet;

        TQueueMsgData stData;
        stData.m_strBody = std::string(pszTmp, iTmpSize);
        stData.m_stHeadInfo.m_iMsgSrcType = EQMST_Client;
        stData.m_stHeadInfo.m_iServerID = GetServerIDByGroupID(iGroupID);
        stData.m_stHeadInfo.m_iFd = stSocket.m_socket_fd;
        stData.m_stHeadInfo.m_iUin = iClientUin;
        stData.m_stHeadInfo.m_iGroupID = iGroupID;
        stData.m_stHeadInfo.m_usCmd = stClientProto.m_stStaticData.m_usCmd;

        // 加入消息队列
        if(AddMessageQueueProcessor(stData))
        {
            TRACESVR(log_mask_detail, "[CConnFrame::%s] get player's uin = %d and transfer to service, size:%d\n", __FUNCTION__, iClientUin, iTmpSize);
        }
    }
    else
    {
        //client heart beat
        TRACESVR(log_mask_detail, "[CConnFrame::%s] uin=%d|heart beat\n", __FUNCTION__, iClientUin);
    }

    //设置已经从peer收到数据了
    stSocket.m_is_recved_data_from_peer = true;

    return success;
}

//返回值:true:继续运行,false:运行退出....
bool CQueueGameControl::CheckServerContext()
{
     if(g_byRunFlag.is_setted(run_flag_reload))
	 {
		 g_byRunFlag.unset(run_flag_reload);

		 //配置重载...
		 if(!this->m_appCfg.Load(APPCONFIG_FILENAME))
		 {
			 TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s]reload config fail!\n", __FUNCTION__);
			 return false;
		 }
		 TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s]reload config success!\n", __FUNCTION__);
		 
		 //重置日志等级...
         for(auto& msgPro : m_stMsgProcessor)
         {
             msgPro.Reload(m_appCfg.m_LogParam.m_nLogMask);
         }

		 m_stAlertProcessor.Reload(m_appCfg.m_LogParam.m_nLogMask);

         SETLOGMASK(m_appCfg.m_LogParam.m_nLogMask);

		 TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s]RUNMODE_RELOAD...end\n", __FUNCTION__);
		 return true;//继续运行....
	 }
	 else if(g_byRunFlag.is_setted(run_flag_exit))
	 {
		 g_byRunFlag.unset(run_flag_exit);
		 return false;//退出....
	 }
	 else
	 {
		 return true;
	 }
}

int32_t CQueueGameControl::ActiveAllProcessor()
{
    TQueueMsgData stData;
    stData.m_stHeadInfo.m_usCmd = 0;

    for(auto& pro : m_stMsgProcessor)
    {
        pro.AddMsgQueue(stData);
    }

    m_stAlertProcessor.AddMsgQueue(stData);

    return success;
}

int32_t CQueueGameControl::GetServerIDByGroupID(int32_t iGroupID)
{
	std::shared_ptr<CQueueServerConfig> config;
    CQueueServerConfig::GetConfig(config);
	return config->GetZooQueueConfig().GetServerIDByGroupID(iGroupID);
}

bool CQueueGameControl::DispatchMessage()
{
    int32_t timeout = 100;		//单位:ms
    int fd_event_count = 0;

    // zookeeper
    int iZooFd = -1, iZooEvent = 0, iZooEpollEvent = 0;
    m_stZookeeper.Interest(iZooFd, iZooEvent);

    if (iZooEvent & ZOOKEEPER_READ) iZooEpollEvent |= EPOLLIN;
    if (iZooEvent & ZOOKEEPER_WRITE) iZooEpollEvent |= EPOLLOUT;

    epoll_op_socket(iZooFd, EPOLL_CTL_ADD, iZooEpollEvent);

    fd_event_count = epoll_wait(m_epoll_fd, m_pevents, m_epoll_max_events, timeout);
    if (fd_event_count < 0)
    {
        return fail;
    }

    int sock_fd = -1;
    struct epoll_event *pstEvent = m_pevents;

    for(int i = 0; i < fd_event_count; ++i, pstEvent++)
    {
        sock_fd = pstEvent->data.fd;
        if (sock_fd < 0) continue;

        if (0 != (EPOLLERR & pstEvent->events))
        {
            int error_code = 0;
            socklen_t len = (socklen_t)sizeof(error_code);
            getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &error_code, &len);


            TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] socket(%d) errno:%d epoll error so closed! and clear socket info\n", __FUNCTION__, sock_fd, error_code);

            m_stRouterManager.ClearServerConnection(sock_fd);
            m_stRouterManager.ClearClientSocket(sock_fd, enmClosedReason_Client);

            continue;
        }

        // zookeeper
        if(sock_fd == iZooFd)
        {
            iZooEvent = 0;
            if (pstEvent->events & EPOLLIN) iZooEvent |= ZOOKEEPER_READ;
            if (pstEvent->events & EPOLLOUT) iZooEvent |= ZOOKEEPER_WRITE;

            epoll_op_socket(iZooFd, EPOLL_CTL_DEL, pstEvent->events);

            m_stZookeeper.Process(iZooEvent);

            continue;
        }

        if (0 == (EPOLLIN & pstEvent->events))
        {
            continue;
        }

        // listen
        if (m_stRouterManager.m_stListenSocket.get_fd() == sock_fd)
        {
            struct sockaddr_in stConnectAddr;
            socklen_t iAddrLength = sizeof(stConnectAddr);
            const int32_t iNewSocket = accept(sock_fd, reinterpret_cast<struct sockaddr *>(&stConnectAddr), &iAddrLength);
            const std::string ip = inet_ntoa(stConnectAddr.sin_addr);
            const uint16_t port = ntohs(stConnectAddr.sin_port);
            if(iNewSocket <= 0)
            {
                ::close(iNewSocket);

                TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] accept client(%s:%d) error, code = %d, msg = %s\n",
                         __FUNCTION__, ip.c_str(), port, errno, strerror(errno));
                continue;
            }

            std::shared_ptr<CQueueServerConfig> config;
            CQueueServerConfig::GetConfig(config);
            const int32_t iServerID = config->GetZooQueueConfig().GetServerIDByConnection(ip, port);

            // logic连接
            if (iServerID > 0)
            {
                if(intranet_ip(stConnectAddr.sin_addr.s_addr) && epoll_new_socket(iNewSocket))
                {
                    ConnectionType& rstConnection = m_stRouterManager.AddServerConnection(iNewSocket, Enum_EET_EntityTypes_Logic, iServerID, ip, port);
                    rstConnection.AcceptFd(iNewSocket);
                    rstConnection.Set4ByteSize(true);
                    rstConnection.Alive();

                    TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s]has a new logic_svr connection, server_id(%d), inof(%s:%ud), fd:%d\n",
                             __FUNCTION__, iServerID, ip.c_str(), port, iNewSocket);
                }
                else
                {
                    close(iNewSocket);
                    TRACESVR(log_mask_system_error, "[CQueueGameControl::%s]has a outer logic_svr connection, server_id(%d), inof(%s:%ud), close fd:%d\n",
                             __FUNCTION__, iServerID, ip.c_str(), port, iNewSocket);
                }
            }
            else
            {
                // client连接
                if(int(m_stRouterManager.m_stClientConMap.size()) >= m_appCfg.m_iMaxClientNum)
                {
                    TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] connection is so many, just refused!\n", __FUNCTION__);

                    close(iNewSocket);
                    continue;
                }

                //设置为非阻塞socket
                int flags = 1;
                if (ioctl(iNewSocket, FIONBIO, &flags) && ((flags = fcntl(iNewSocket, F_GETFL, 0)) < 0 || fcntl(iNewSocket, F_SETFL, flags | O_NONBLOCK) < 0))
                {
                    TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] set accepted socket O_NONBLOCK failed, just close it!\n", __FUNCTION__);

                    close(iNewSocket);
                    continue;
                }

                if (epoll_new_socket(iNewSocket) == false)
                {
                    TRACESVR(log_mask_system_warning, "[CQueueGameControl::%s] epoll add socket failed, just close it!\n", __FUNCTION__);

                    close(iNewSocket);
                    continue;
                }

                //record socket info
                TSocketInfo stSocket;
                stSocket.m_socket_fd = iNewSocket;
                stSocket.m_peer_address = stConnectAddr.sin_addr.s_addr;
                stSocket.m_peer_port = port;
                STRNCPY(stSocket.m_client_address, ip.c_str(),sizeof(stSocket.m_client_address));
                stSocket.m_create_time = time(NULL);
                stSocket.m_lastrecv_timestamp = stSocket.m_create_time;
                stSocket.m_uin = 0;
                stSocket.m_iGroupID = 0;
                stSocket.m_is_recved_data_from_peer = false;
                stSocket.m_is_sended_data_to_peer = false;
                stSocket.m_recv_bytes = 0;

                m_stRouterManager.AddClientConnection(stSocket);

                TRACESVR(log_mask_detail, "[CQueueGameControl::%s]has a new client connection, inof(%s:%ud), fd:%d\n", __FUNCTION__, ip.c_str(), port, iNewSocket);
            }

            continue;
        }

        // logic server
        auto iterServer = m_stRouterManager.m_astConnectionMap.find(sock_fd);
        if(iterServer != m_stRouterManager.m_astConnectionMap.end())
        {
            auto& stServerSock = iterServer->second;
            if(false == stServerSock.BeConnected())
            {
                m_stRouterManager.ClearServerConnection(sock_fd);
                continue;
            }

            size_t nTmpCodeLength = 0;
            /* 每次收到消息，更新心跳包时间戳 */
            stServerSock.Alive();

            int32_t ret = stServerSock.RecvData();
            TRACESVR(log_mask_detail, "[CQueueGameControl::%s]server(%d) had data to process, recv ret:%d.\n", __FUNCTION__, stServerSock.GetEntityId(), ret);
            if (false == stServerSock.BeConnected())
            {
                TRACESVR(log_mask_system_error, "[CQueueGameControl::%s] server(%d) close link\n", __FUNCTION__, stServerSock.GetEntityId());
                m_stRouterManager.ClearServerConnection(sock_fd);
                continue;
            }

            CMsgHeadQueue stMsgHead;
            while (stServerSock.GetOneCode(static_cast<char *>(abyCodeBuf), nTmpCodeLength) > 0)
            {
                const int32_t iServerID = stServerSock.GetEntityId();
                TRACESVR(log_mask_detail, "[CQueueGameControl::%s]get one code from server(%d), dispatch it. content=\n", __FUNCTION__, iServerID);
                TRACEBIN(log_mask_detail, static_cast<char *>(abyCodeBuf), static_cast<int32_t>(nTmpCodeLength));

                if (success != stMsgHead.decode(abyCodeBuf, nTmpCodeLength))
                {
                    TRACESVR(log_mask_system_error, "[CQueueGameControl::%s]Error code, can't decode message head.\n", __FUNCTION__);
                    break;
                }

                //如果该服已经被合服，则不再接受其连接
                if (iServerID != GetServerIDByGroupID(iServerID))
                {
                    m_stRouterManager.ClearServerConnection(sock_fd);
                    TRACESVR(log_mask_system_warning, "[CConnFrame::%s] receive package from service(%d), but it has already merge to service(%d), so disconnect, please check router.cfg.\n",
                             __FUNCTION__, iServerID, GetServerIDByGroupID(iServerID));
                    break;
                }

                if (MSG_CROSS_HEART_BEAT == stMsgHead.m_usCmd)
                {//心跳包
                    TRACESVR(log_mask_detail, "[CQueueGameControl::%s]recv heart beat. server_id:%d\n", __FUNCTION__, iServerID);
                    continue;
                }

                TQueueMsgData stData;
                stData.m_strBody = std::string(abyCodeBuf + stMsgHead.SIZE, nTmpCodeLength - stMsgHead.SIZE);
                stData.m_stHeadInfo.m_iMsgSrcType = EQMST_Logic;
                stData.m_stHeadInfo.m_iServerID = iServerID;
                stData.m_stHeadInfo.m_iFd = sock_fd;
                stData.m_stHeadInfo.m_usCmd = stMsgHead.m_usCmd;

                AddMessageQueueProcessor(stData);
            }	//end while
        }
        else
        {
            // client
            auto iterClient = m_stRouterManager.m_stClientConMap.find(sock_fd);
            if(iterClient != m_stRouterManager.m_stClientConMap.end())
            {
                recv_client_messages(iterClient->second);
            }
        }
    }

    return true;
}
