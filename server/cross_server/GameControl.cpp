#include "GameControl.h"
#include "server_cmd_cross_define.h"
#include "server_cmd_logic_define.h"
#include "common_tools.h"
#include "AppPlatform.h"
#include "ServerConfig.h"
#include "cross_mysql_handle.h"
#include "logic_record_sql_table.h"
#include "server_cmd_data_report_define.h"

#define SVRD_PID_NAME "crosssvrd.pid"

CAppConfig CGameControl::m_appCfg;//系统配置档
CAppConfigZookeeper CGameControl::m_stZookeeper;
bool CGameControl::s_bLoadZooConfgSuccess = false;
CRouterManager CGameControl::m_stRouterManager;
char m_szVerInfo[200];

CFlag g_byRunFlag;
CGameControl* g_pGameCtrl = nullptr;

CGameControl::CGameControl(void)
{
	memset(m_szVerInfo, 0, sizeof(m_szVerInfo));
}

CGameControl::~CGameControl(void)
{
    m_stMsgQueue.stop();
}

const char* CGameControl::GetVerInfo()
{
    if (strlen(m_szVerInfo) == 0)
	{
		sprintf(m_szVerInfo,"%s  %s!\n", VERSION_INFO,  get_build_info().c_str());
	}

	return m_szVerInfo;
}

CGameControl* CGameControl::GetInstance()
{
	//装载配置文件.
	if (!CGameControl::m_appCfg.Load(APPCONFIG_FILENAME))
	{
		printf("load config file['%s'] raise error!\n", APPCONFIG_FILENAME);
		return nullptr;
	}
	
	CGameControl* pNewCtrl = new CGameControl();
	if (nullptr == pNewCtrl)
	{
		printf("new CGameControl failed.\n");
	}

	return pNewCtrl;
}

bool CGameControl::CheckPath()
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

bool CGameControl::Initialize()
{
	if(!CheckPath())
	{
		return false;
	}

	//初始化log
	char szLogFile[MAX_FILEPATHLEN] = {0};
	sprintf(szLogFile, "%s/%s", m_appCfg.m_LogParam.m_szLogPath, APPLOG_PREFIX_FILENAME);

	TRACESVR(log_mask_initialize, "[CGameControl::Initialize]Verson Infomation:%s, log init success!\n", this->GetVerInfo());
	SETLOGMASK(m_appCfg.m_LogParam.m_nLogMask);
	

	//初始化Zookeeper
	if (!InitZookeeper())
	{
		TRACESVR(log_mask_initialize, "[CGameControl::Initialize] init zookeeper fail!!!\n");
		return false;
	}

    //cross_config
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

	if (0 != m_stRouterManager.m_stListenSocket.open_as_server(cross_config->GetZooCrossConfig().m_stCrossConfig.m_iPort,
	        cross_config->GetZooCrossConfig().m_stCrossConfig.m_strHost.c_str()))
	{
		TRACESVR(log_mask_initialize, "[CGameControl::Initialize] open listen port fail!\n");
		return false;
	}

	m_tmLastCheckRouterHeartBeat = time(nullptr);
	m_tmLastCheckConfigTime = m_tmLastCheckRouterHeartBeat;
	m_tmLastActiveProcessTime = m_tmLastCheckRouterHeartBeat;

	char szLogFileName[MAX_FILEPATHLEN] = {0};
	sprintf(szLogFileName, "%s/%s", m_appCfg.m_LogParam.m_szLogPath, CProcessor::GetProcessName());

	if (!m_stHttpProcessor.Initialize(m_appCfg.m_LogParam.m_nLogMask, (const char*)szLogFileName, 5))
	{
		TRACESVR(log_mask_system_error, "[CGameControl::%s]Http Processor[ID=5] init error.\n", __FUNCTION__);
		return false;
	}

	if (!m_stCentreDataProcessor.Initialize(m_appCfg.m_LogParam.m_nLogMask, (const char*)szLogFileName, 10))
	{
		TRACESVR(log_mask_system_error, "[CGameControl::%s]CentreData Processor[ID=10] init error.\n", __FUNCTION__);
		return false;
	}

    if (!m_stMatchProcessor.Initialize(m_appCfg.m_LogParam.m_nLogMask, (const char*)szLogFileName, 20))
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s]Match Processor[ID=20] init error.\n", __FUNCTION__);
        return false;
    }

    if (!m_stTeamProcessor.Initialize(m_appCfg.m_LogParam.m_nLogMask, (const char*)szLogFileName, 30))
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s]Team Processor[ID=30] init error.\n", __FUNCTION__);
        return false;
    }

	return true;
}
 
void CGameControl::CheckTimers()
{
	time_t tNow = time(nullptr);

	if (tNow != m_tmLastActiveProcessTime)
    {
        ActiveAllProcessor();
        m_tmLastActiveProcessTime = tNow;
    }

	if ((tNow - m_tmLastCheckRouterHeartBeat) >= CHECK_ROUTER_INTERVAL)
	{
		m_stRouterManager.CheckConnectionAlive();		
		m_tmLastCheckRouterHeartBeat = tNow;
	}

	if (tNow - m_tmLastCheckConfigTime >= 5)
	{
		m_stZookeeper.UpdateConfig();
		m_tmLastCheckConfigTime = tNow;
	}
}

bool CGameControl::Run()
{
    //删除掉pid文件
	delete_pid_file(SVRD_PID_NAME);

	//启动Handle
	TRACESVR(log_mask_initialize,"[CGameControl::Run]enter...\n");
    if(!m_stHttpProcessor.Start())
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s] Processor(id:%d) start failed!\n", __FUNCTION__, m_stHttpProcessor.GetProcessID());
        return false;
    }
    if(!m_stCentreDataProcessor.Start())
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s] Processor(id:%d) start failed!\n", __FUNCTION__, m_stCentreDataProcessor.GetProcessID());
        return false;
    }
    if(!m_stMatchProcessor.Start())
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s] Processor(id:%d) start failed!\n", __FUNCTION__, m_stMatchProcessor.GetProcessID());
        return false;
    }
    if(!m_stTeamProcessor.Start())
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s] Processor(id:%d) start failed!\n", __FUNCTION__, m_stTeamProcessor.GetProcessID());
        return false;
    }
	TRACESVR(log_mask_initialize, "[CGameControl::Run]start son processors!\n");

    // mysql 线程初始化
    mysql_library_init(0, nullptr, nullptr);
    if(!CROSS_MYSQL_HANDLE->Start())
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s] CROSS_MYSQL_HANDLE start failed!\n", __FUNCTION__);
        return false;
    }
    CLogicSQLTableRecordBase::SetMysqlConn(CROSS_MYSQL_HANDLE->GetGameQueryConn());

    //创建pid文件
	create_pid_file(SVRD_PID_NAME);
	//进入消息循环...
	while(CheckServerContext())
	{
		CheckTimers();

		//分发消息
		DispatchMessage();

        // 发送客户端消息
        SendClientMsg();
	}

	TRACESVR(log_mask_system_warning, "[CGameControl::Run]exit...\n");
	return true;
}

void CGameControl::SendClientMsg()
{
    static int64_t lStartMicroSecond = 0;
    static int64_t lCostMicroSecond = 0;

    lStartMicroSecond = now_msec();
    static int32_t iNum = 0;

    int32_t iMsgNum = m_stMsgQueue.size();
    if(iMsgNum <= 0)
        return;

    iNum = iMsgNum;

    std::optional<TCrossMsgData> opt;
    while (iMsgNum > 0 && ((opt = m_stMsgQueue.pop_front()).has_value()))
    {
        --iMsgNum;
        auto& msg = opt.value();

        auto iterSocket = m_stRouterManager.GetServerConnection(msg.m_stHeadInfo.m_iServerID);
        if(!iterSocket)
        {
            TRACESVR(log_mask_system_error, "[CGameControl::%s] socket error. from server(%d)\n", __FUNCTION__, msg.m_stHeadInfo.m_iServerID);
            continue;
        }

        iterSocket->SendData(msg.m_strBody.c_str(), msg.m_strBody.length());
    }

    lCostMicroSecond = now_msec() - lStartMicroSecond;

    if (lCostMicroSecond > 100)
    {
        TRACESVR(log_mask_system_error, "[CGameControl:%s] send client message too slow! COST %ld ms. msg_num:%d!\n", __FUNCTION__, lCostMicroSecond, iNum);
    }
}

void CGameControl::After_Run()
{
    TRACESVR(log_mask_system_warning, "[CGameControl::%s] RUNMODE_QUIT...start\n", __FUNCTION__);

    if (!m_stHttpProcessor.Stop())
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s] CHttpProcessor(id=%d) Stop return false!\n", __FUNCTION__, m_stHttpProcessor.GetProcessID());
    }

    if (!m_stCentreDataProcessor.Stop())
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s] CCentreDataProcessor(id=%d) Stop return false!\n", __FUNCTION__, m_stCentreDataProcessor.GetProcessID());
    }

    if (!m_stMatchProcessor.Stop())
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s] CMatchProcessor(id=%d) Stop return false!\n", __FUNCTION__, m_stMatchProcessor.GetProcessID());
    }

    if (!m_stTeamProcessor.Stop())
    {
        TRACESVR(log_mask_system_error, "[CGameControl::%s] CTeamProcessor(id=%d) Stop return false!\n", __FUNCTION__, m_stTeamProcessor.GetProcessID());
    }

    m_stMsgQueue.stop();

    // mysql 线程释放
    CROSS_MYSQL_HANDLE->Stop();
    mysql_library_end();

    TRACESVR(log_mask_system_warning, "[CGameControl::%s] RUNMODE_QUIT...end\n", __FUNCTION__);
}

bool CGameControl::InitZookeeper()
{	
	m_stZookeeper.SetZookeeperInfo(m_appCfg.m_szZookeeperHost, m_appCfg.m_szZookeeperConfigPath);

	for (size_t i = 0; i < CCrossServerConfig::GetConfigSize(); ++i)
	{
		m_stZookeeper.AddWatchFile(CCrossServerConfig::GetConfigLoader()[i].m_pFileName);
	}

	if (!m_stZookeeper.InitZookeeper())
	{
		TRACESVR(log_mask_system_error, "ZOOKEEPER INIT FAILED!!!\n");
		return false;
	}

	const CAppConfigZookeeper::reload_func_type stLoadConfig = std::bind(&CGameControl::LoadCrossConfig, this, std::placeholders::_1);

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

bool CGameControl::LoadCrossConfig(const CAppConfigZookeeper& stZookeeper)
{
	static int64_t lStartMicroSecond = 0;
    static int64_t lEndMicroSecond = 0;
    static int64_t lCostMicroSecond = 0;

    lStartMicroSecond = now_msec();

	std::shared_ptr<CCrossServerConfig> pConfig = std::make_shared<CCrossServerConfig>();

	std::pair<bool, std::string> stRet(true, "OK");
	for (size_t i = 0; i < CCrossServerConfig::GetConfigSize(); ++i)
	{
		const auto& stConfigLoader = CCrossServerConfig::GetConfigLoader()[i];
		auto pParser = stConfigLoader.m_pParserGetFunc(pConfig);

		stRet = pParser->Load(stZookeeper.GetNormalConfig(stConfigLoader.m_pFileName));

		if (stRet.first == false)
		{
			TRACESVR(log_mask_system_error, "[CGameControl:%s] PARSE CONFIG %s ERROR|MSG:%s\n", __FUNCTION__, stConfigLoader.m_pFileName, stRet.second.c_str());

			s_bLoadZooConfgSuccess = false;
			return false;
		}
	}

	lEndMicroSecond = now_msec();
	lCostMicroSecond = lEndMicroSecond - lStartMicroSecond;

	TRACESVR(log_mask_system_error, "[CGameControl:%s]PARSE CONFIG SUCCESS!COST %ld ms\n", __FUNCTION__, lCostMicroSecond);

	CCrossServerConfig::SetConfig(pConfig);
	s_bLoadZooConfgSuccess = true;

	return true;
}

//返回值:true:继续运行,false:运行退出....
bool CGameControl::CheckServerContext()
{
     if(g_byRunFlag.is_setted(run_flag_reload))
	 {
		 g_byRunFlag.unset(run_flag_reload);

		 //配置重载...
		 if(!this->m_appCfg.Load(APPCONFIG_FILENAME))
		 {
			 TRACESVR(log_mask_system_warning, "[CGameControl::%s]reload config fail!\n", __FUNCTION__);
			 return false;
		 }
		 TRACESVR(log_mask_system_warning, "[CGameControl::%s]reload config success!\n", __FUNCTION__);
		 
		 //重置日志等级...
         m_stHttpProcessor.Reload(m_appCfg.m_LogParam.m_nLogMask);
         m_stCentreDataProcessor.Reload(m_appCfg.m_LogParam.m_nLogMask);
         m_stMatchProcessor.Reload(m_appCfg.m_LogParam.m_nLogMask);
         m_stTeamProcessor.Reload(m_appCfg.m_LogParam.m_nLogMask);

		 SETLOGMASK(m_appCfg.m_LogParam.m_nLogMask);

		 TRACESVR(log_mask_system_warning, "[CGameControl::%s]RUNMODE_RELOAD...end\n", __FUNCTION__);
		 return true;//继续运行....
	 }
	 else if(g_byRunFlag.is_setted(run_flag_exit))
	 {
		 g_byRunFlag.unset(run_flag_exit);
		 TRACESVR(log_mask_system_warning, "[CGameControl::%s]RUNMODE_EXIT\n", __FUNCTION__);
		 return false;//退出....
	 }
	 else
	 {
		 return true;
	 }
}

//分发收到的数据
int32_t CGameControl::DispatchMessage()
{
	//建立事件集
	fd_set fds_read, fds_write;
	FD_ZERO(&fds_read);
	FD_ZERO(&fds_write);

	SetServerFDSet(fds_read, fds_write);

	//select
	struct timeval stMonTime;
	stMonTime.tv_sec = 0;
	stMonTime.tv_usec = 10000;

	int32_t iOpenFDNum = 0;
    // 处理连接请求和消息
	iOpenFDNum = select(FD_SETSIZE, &fds_read, &fds_write, nullptr, &stMonTime);
	ManServerFDSet(fds_read, fds_write, iOpenFDNum);

	return success;
}

bool CGameControl::DispatchOneMessage(const TCrossMsgData& stMsgData)
{
	CProcessor* processor = nullptr;
	switch (stMsgData.m_stHeadInfo.m_usCmd)
    {
        case MSG_CROSS_UPDATE_MATCH_USER:
        case MSG_CROSS_FIGHT_MATCH_PVP:
        case MSG_CROSS_FIGHT_FAIR_PVP:
        case MSG_CROSS_GET_ATTACK_DEFENSE_INFO:
        case MSG_CROSS_FIGHT_ATTACK_DEFENSE:
        case MSG_CROSS_FIGHT_ATTACK_DEFENSE_RESULT:
        case MSG_CROSS_REPAIR_ATTACK_DEFENSE:
        case MSG_CROSS_ATTACK_DEFENSE_SYNC:
        case MSG_CROSS_ATTACK_DEFENSE_SYNC_DONE:
        case MSG_CROSS_GET_LUCKY_HAMMER_RANK:
        case MSG_CROSS_GET_LUCKY_HAMMER_LOG:
        case MSG_CROSS_LUCKY_HAMMER_LOG_SCORE:
        case MSG_CROSS_SYNC_LUCKY_HAMMER_SCORE:
        case MSG_CROSS_SYNC_LUCKY_HAMMER_LOG:
            processor = &m_stMatchProcessor;
            break;

        case MSG_CROSS_SERVICE_ALERT:
        case MSG_REPORT_SET_MUSIC:
        case MSG_REPORT_GET_EQUIP:
        case MSG_REPORT_UPGRADE_SKILL:
        case MSG_REPORT_UPGRADE_CONSTEL:
        case MSG_REPORT_GET_CONSTEL:
        case MSG_REPORT_START_FIGHT_PVE:
        case MSG_REPORT_GAIN_EMAIL:
        case MSG_REPORT_ACHIEVE_QUEST:
        case MSG_REPORT_GET_REWARD_QUEST:
        case MSG_REPORT_GET_PASS_REWARD:
        case MSG_REPORT_PVP_FIGHT:
        case MSG_REPORT_REFRESH_PVP:
        case MSG_REPORT_REWARD_PVP:
        case MSG_REPORT_CHANGE_TEAM:
        case MSG_REPORT_JOIN_GUILD:
        case MSG_REPORT_LEAVE_GUILD:
        case MSG_REPORT_CHANGE_GUILD_NAME:
        case MSG_REPORT_DONATE_CARD:
        case MSG_REPORT_SET_WISH_CARD:
        case MSG_REPORT_BUG_GUILD_ITEM:
        case MSG_REPORT_PAY_CHOOSE:
        case MSG_REPORT_PAY:
        case MSG_REPORT_ITEM_MONITOR:
        case MSG_REPORT_OFFLINE:
            processor = &m_stHttpProcessor;
            break;

        case MSG_CROSS_CREATE_BANQUET_TEAM:
        case MSG_CROSS_JOIN_BANQUET_TEAM:
        case MSG_CROSS_GET_BANQUET_INFO:
        case MSG_CROSS_BANQUET_UPDATE_LOGIN:
        case MSG_LOGIC_BANQUET_INVITE:
        case MSG_CROSS_BANQUET_QUIT_TEAM:
        case MSG_CROSS_BANQUET_GET_FRIEND_OPEN:
        case MSG_CROSS_NEW_LIMIT_BOSS_NOTIFY:
        case MSG_CROSS_NEW_LIMIT_BOSS_HISTORY:
        case MSG_CROSS_NEW_LIMIT_BOSS_BEGIN:
        case MSG_CROSS_NEW_LIMIT_BOSS_END:
        case MSG_CROSS_NEW_LIMIT_BOSS_INVITE:
        case MSG_CROSS_NEW_LIMIT_BOSS_GET_INFO:
            processor = &m_stTeamProcessor;
            break;

        case MSG_CROSS_USER_BRIEF_UPDATE:
        case MSG_LOGIC_GET_FRIEND_LIST:
        case MSG_LOGIC_GET_FRIEND_APPLY_LIST:
        case MSG_LOGIC_SEARCH_FRIEND:
        case MSG_LOGIC_GET_GUILD_LIST:
        case MSG_LOGIC_GET_SELF_GUILD_INFO:
        case MSG_LOGIC_GET_GUILD_MEMBERS:
        case MSG_LOGIC_CREATE_GUILD:
        case MSG_LOGIC_LEAVE_GUILD:
        case MSG_LOGIC_UPDATE_GUILD_BULLETIN:
        case MSG_LOGIC_UPDATE_GUILD_POST:
        case MSG_LOGIC_TRANSFER_GUILD:
        case MSG_LOGIC_EXPEL_GUILD:
        case MSG_LOGIC_APPLY_JOIN_GUILD:
        case MSG_LOGIC_DEAL_GUILD_APPLY:
        case MSG_LOGIC_GET_GUILD_APPLY_LIST:
        case MSG_LOGIC_SEARCH_GUILD:
        case MSG_LOGIC_TOUHOU_CHANGE_GUILD_NAME:
        case MSG_CROSS_ADMIN_MODIFY_GUILD:
        case MSG_CROSS_ADMIN_DISMISS_GUILD:
        case MSG_CROSS_ADMIN_SET_GUILD_CHIEF:
        case MSG_CROSS_ADMIN_CHANGE_GUILD_NAME:
        case MSG_CROSS_GUILD_BOSS_HURT_NOTIFY:
        case MSG_LOGIC_GUILD_BOSS_HURT_LIST:
        case MSG_LOGIC_USER_TALK:
        case MSG_LOGIC_GUILD_GET_LOG:
        case MSG_CROSS_GUILD_WISH_NOTIFY:
        case MSG_CROSS_GUILD_WISH_GET_LIST:
        case MSG_LOGIC_GUILD_DONATE:
        case MSG_LOGIC_GUILD_SIGN_IN:
        case MSG_LOGIC_GUILD_SIGN_UPGRADE:
        case MSG_LOGIC_GUILD_SAVE_DECORATE:
        case MSG_LOGIC_GUILD_GET_BOSS_HURT_RANK:
        case MSG_LOGIC_GUILD_SIT_BAR:
        case MSG_LOGIC_GUILD_GET_BAR_REWARD:
        case MSG_LOGIC_GUILD_UNLOCK_COLLECTION:
        case MSG_LOGIC_GUILD_SET_DECORATE:
        case MSG_CROSS_GUILD_WAR_GET_INFO:
        case MSG_CROSS_GUILD_WAR_BEGIN:
        case MSG_CROSS_GUILD_WAR_END:
        case MSG_CROSS_GUILD_WAR_RANK:
        case MSG_CROSS_GUILD_WAR_GET_SELF_RANK:
            processor = &m_stCentreDataProcessor;
            break;

        default:
            TRACESVR(log_mask_system_error, "[CGameControl::%s]process error, can't found processor to do %d message\n", __FUNCTION__, stMsgData.m_stHeadInfo.m_usCmd);
            return false;
    }

    processor->AddMsgQueue(stMsgData);
    TRACESVR(log_mask_detail, "[CGameControl::%s]trans_type_by_key-->post code(uin=%d) to processor id:%d\n", __FUNCTION__, stMsgData.m_stHeadInfo.m_iServerID, processor->GetProcessID());

    return true;
}

int32_t CGameControl::ActiveAllProcessor()
{
    TCrossMsgData stData;
    stData.m_stHeadInfo.m_usCmd = 0;

    m_stHttpProcessor.AddMsgQueue(stData);
    m_stCentreDataProcessor.AddMsgQueue(stData);
    m_stMatchProcessor.AddMsgQueue(stData);
    m_stTeamProcessor.AddMsgQueue(stData);

    return success;
}

ConnectionType* CGameControl::GetServerConnection(int32_t iServerID)
{
	return m_stRouterManager.GetServerConnection(iServerID);
}

ConnectionType* CGameControl::GetGroupConnection(int32_t iGroupID)
{
	return GetServerConnection(GetServerIDByGroupID(iGroupID));
}

bool CGameControl::IsGroupConnecting(int32_t iGroupID)
{
    ConnectionType *pstConn = GetGroupConnection(iGroupID);
    return pstConn && pstConn->BeConnected();
}

int32_t CGameControl::GetServerIDByGroupID(int32_t iGroupID)
{
	std::shared_ptr<CCrossServerConfig> config;
	CCrossServerConfig::GetConfig(config);
	return config->GetZooCrossConfig().GetServerIDByGroupID(iGroupID);
}

void CGameControl::SetServerFDSet(fd_set& fds_read, fd_set& fds_write)
{
	if (m_stRouterManager.m_stListenSocket.be_listening())
	{
		FD_SET(m_stRouterManager.m_stListenSocket.get_fd(), &fds_read);
	}

	for (auto& tcp : m_stRouterManager.m_astConnectionMap)
	{
		if (tcp.second.BeConnected())
		{
			FD_SET(tcp.second.GetFD(), &fds_read);
		}
	}

	int32_t iZooFd, iZooEvent;
	m_stZookeeper.Interest(iZooFd, iZooEvent);
	if (iZooFd > 0)
	{
        if (iZooEvent & ZOOKEEPER_READ)
        {
            FD_SET(iZooFd, &fds_read);
        }

        if (iZooEvent & ZOOKEEPER_WRITE)
        {
            FD_SET(iZooFd, &fds_write);
        }

        m_stRouterManager.m_iZookeeperSocketFd = iZooFd;
	}
}

void CGameControl::ManServerFDSet(const fd_set& fds_read, const fd_set& fds_write, int32_t& iOpenFDNum)
{
	if (m_stRouterManager.m_stListenSocket.be_listening() && FD_ISSET(m_stRouterManager.m_stListenSocket.get_fd(), &fds_read))
	{
		--iOpenFDNum;

		struct sockaddr_in stConnectAddr;
		socklen_t iAddrLength = sizeof(stConnectAddr);
		const int32_t iNewSocket = accept(m_stRouterManager.m_stListenSocket.get_fd(), reinterpret_cast<struct sockaddr *>(&stConnectAddr), &iAddrLength);
		const std::string ip = inet_ntoa(stConnectAddr.sin_addr);
		const uint16_t port = ntohs(stConnectAddr.sin_port);
		std::shared_ptr<CCrossServerConfig> config;
		CCrossServerConfig::GetConfig(config);
		const int32_t iServerID = config->GetZooCrossConfig().GetServerIDByConnection(ip, port);

		if (iServerID > 0 && intranet_ip(stConnectAddr.sin_addr.s_addr) )
		{
			ConnectionType& rstConnection = m_stRouterManager.AddOneConnection(Enum_EET_EntityTypes_Logic, iServerID, ip, port);
			rstConnection.AcceptFd(iNewSocket);
			rstConnection.Set4ByteSize(true);
			rstConnection.Alive();
			TRACESVR(log_mask_system_warning, "[CGameControl::%s]has a new connection,inof(%s:%ud), fd:%d\n", __FUNCTION__, ip.c_str(), port, iNewSocket);
		}
		else
		{
			::close(iNewSocket);
			TRACESVR(log_mask_system_error, "[CGameControl::%s]has a outer connection,inof(%s:%ud), close fd:%d\n", __FUNCTION__, ip.c_str(), port, iNewSocket);
		}
	}

	if (m_stRouterManager.m_iZookeeperSocketFd > 0)
	{
	    if(FD_ISSET(m_stRouterManager.m_iZookeeperSocketFd, &fds_read))
        {
	        m_stZookeeper.Process(ZOOKEEPER_READ);
	        iOpenFDNum--;
        }

	    if(FD_ISSET(m_stRouterManager.m_iZookeeperSocketFd, &fds_write))
        {
	        m_stZookeeper.Process(ZOOKEEPER_WRITE);
	        iOpenFDNum--;
        }

		m_stRouterManager.m_iZookeeperSocketFd = -1;
	}

	size_t nTmpCodeLength = 0;

	for (auto& tcp : m_stRouterManager.m_astConnectionMap)
	{
		if (iOpenFDNum <= 0)
		{
			break;
		}

		if (!tcp.second.BeConnected())
		{
			continue;
		}

		if (FD_ISSET(tcp.second.GetFD(), &fds_read))
		{
			/* 每次收到消息，更新心跳包时间戳 */
			tcp.second.Alive();

			--iOpenFDNum;

			int32_t ret = tcp.second.RecvData();
			TRACESVR(log_mask_detail, "[CGameControl::%s]server(%d) had data to process, recv ret:%d.\n", __FUNCTION__, tcp.second.GetEntityId(), ret);
			if (enmConnErrorCode_peer_closed == ret)
			{
				TRACESVR(log_mask_system_warning, "[CGameControl::%s] peer(%d, %d) close link\n", __FUNCTION__, tcp.second.GetEntityType(), tcp.second.GetEntityId());
			}

			CMsgHeadCross stMsgHead;
			while (tcp.second.GetOneCode(static_cast<char *>(abyCodeBuf), nTmpCodeLength) > 0)
			{
				const int32_t iServerID = tcp.first;
				// TRACESVR(log_mask_detail, "[CGameControl::%s]get one code from (%d, %d), dispatch it. content=\n", __FUNCTION__, tcp.second.GetEntityType(), tcp.second.GetEntityId());
				// TRACEBIN(log_mask_detail, static_cast<char *>(abyCodeBuf), static_cast<int32_t>(nTmpCodeLength));

				if (success != stMsgHead.decode(abyCodeBuf, nTmpCodeLength))
				{
					TRACESVR(log_mask_system_error, "[CGameControl::%s]Error code, can't decode message head.\n", __FUNCTION__);
                    break;
				}
				
				//如果该服已经被合服，则不再接受其连接
				if (iServerID != GetServerIDByGroupID(iServerID))
				{
					TRACESVR(log_mask_system_warning, "[CGameControl::%s] receive package from service(%d), but it has already merge to service(%d), so disconnect, please check router.cfg.\n", __FUNCTION__, iServerID, GetServerIDByGroupID(iServerID));
					tcp.second.CloseSocket();
					break;
				}

                if (enmCmdType_heart == stMsgHead.m_byCmdType)
                {
                    //心跳包
                    TRACESVR(log_mask_detail, "[CGameControl::%s]recv heart beat. server_id:%d\n", __FUNCTION__, iServerID);
                    continue;
                }
                else if(enmCmdType_cross == stMsgHead.m_byCmdType)
                {
                    // 转发包
                    TCrossMsgData stTranspondData;
                    stTranspondData.m_strBody = std::string(abyCodeBuf, nTmpCodeLength);
                    stTranspondData.m_stHeadInfo.m_iServerID = GetServerIDByGroupID(stMsgHead.m_nGroupID);
                    stTranspondData.m_stHeadInfo.m_iUin = stMsgHead.m_iUin;
                    stTranspondData.m_stHeadInfo.m_iGroupID = stMsgHead.m_nGroupID;
                    stTranspondData.m_stHeadInfo.m_usCmd = stMsgHead.m_usCmd;

                    AddMsgQueue(stTranspondData);
                    continue;
                }

                TCrossMsgData stData;
                stData.m_strBody = std::string(abyCodeBuf + stMsgHead.SIZE, nTmpCodeLength - stMsgHead.SIZE);
                stData.m_stHeadInfo.m_iServerID = iServerID;
                stData.m_stHeadInfo.m_iUin = stMsgHead.m_iUin;
                stData.m_stHeadInfo.m_iGroupID = stMsgHead.m_nGroupID;
                stData.m_stHeadInfo.m_usCmd = stMsgHead.m_usCmd;

				DispatchOneMessage(stData);
			}	//end while
		}	//end if
	}
}
