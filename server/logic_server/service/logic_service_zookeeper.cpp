#include "logic_service_logger.h"
#include "logic_service_zookeeper.h"

#define SERVICE_LOG_ZOOKEEPER_INFO  SERVICE_LOG_TRACE_INFO << "ZOOKEEPER INFO"
#define SERVICE_LOG_ZOOKEEPER_DEBUG SERVICE_LOG_TRACE_DEBUG << "ZOOKEEPER INFO"
#define SERVICE_LOG_ZOOKEEPER_ERROR SERVICE_LOG_TRACE_ERROR << "ZOOKEEPER INFO"

#define LOGIC_CONFIG "logic_config"

const char* 
CLogicZookeeper::Type2String(int iEventType)
{
	if (iEventType == ZOO_CREATED_EVENT) return ("CREATED_EVENT");
	if (iEventType == ZOO_DELETED_EVENT) return ("DELETED_EVENT");
	if (iEventType == ZOO_CHANGED_EVENT) return ("CHANGED_EVENT");
	if (iEventType == ZOO_CHILD_EVENT) return ("CHILD_EVENT");
	if (iEventType == ZOO_SESSION_EVENT) return ("SESSION_EVENT");
	if (iEventType == ZOO_NOTWATCHING_EVENT) return ("NOTWATCHING_EVENT");
            
    return ("UNKNOWN_EVENT_TYPE");
}
    
const char* 
CLogicZookeeper::State2String(int iState)
{
    if(iState == 0) return ("CLOSED_STATE");
    if(iState == ZOO_CONNECTING_STATE) return ("CONNECTING_STATE");
    if(iState == ZOO_ASSOCIATING_STATE) return ("ASSOCIATING_STATE");
    if(iState == ZOO_CONNECTED_STATE) return ("CONNECTED_STATE");
    if(iState == ZOO_EXPIRED_SESSION_STATE) return ("EXPIRED_SESSION_STATE");
    if(iState == ZOO_AUTH_FAILED_STATE) return ("AUTH_FAILED_STATE");
    
    return ("INVALID_STATE");
}

const char* 
CLogicZookeeper::Errno2String(int iRet)
{
    return (zerror(iRet));
}

/////////////////////////////////////////////////////////////////////////////////
void 
CLogicZookeeper::ZooHandleEventWatcher(zhandle_t* pZooHandle, int iType, int iState, const char* szPath, void* pContext)
{
    CLogicZookeeper* pSelf = (CLogicZookeeper*)pContext;
    SERVICE_LOG_ZOOKEEPER_DEBUG << "|WATCHER:" << Type2String(iType) << "|STATE:" << State2String(iState) << std::endl;

    if(iType == ZOO_SESSION_EVENT)
    {
        if(iState == ZOO_CONNECTED_STATE)
        {
            const clientid_t* pClientID = zoo_client_id(pZooHandle);
            SERVICE_LOG_ZOOKEEPER_DEBUG << "|GOT A NEW SESSION ID:" << pClientID->client_id << std::endl;
        }
        else if(iState == ZOO_AUTH_FAILED_STATE)
        {
            SERVICE_LOG_ZOOKEEPER_ERROR << "|ZOOKEEPER AUTHENTICATION FAILURE" << std::endl;
            pSelf->SetZookeeperAbnormal();
        }
        else if(iState == ZOO_EXPIRED_SESSION_STATE)
        {
            SERVICE_LOG_ZOOKEEPER_ERROR << "|SESSION EXPIRED" << std::endl;
            pSelf->SetZookeeperAbnormal();
        }
    }
}

//地图配置文件列表状态的回调
void 
CLogicZookeeper::MapConfigListEventWatcher(zhandle_t* pZooHandle, int iType, int iState, const char* szPath, void* pContext)
{
    if(ZOO_SESSION_EVENT == iType)
    {
        SERVICE_LOG_ZOOKEEPER_DEBUG << "|STATUS NOTIFY|TYPE ID:" << iType << "|TYPE NAME" 
                        << Type2String(iType) << "|STATE ID:" << iState 
                        << "|STATE NAME:" << State2String(iState) << std::endl;
        return;
    }

    CLogicZookeeper* pSelf = (CLogicZookeeper*)pContext;
    if(pSelf->m_stZookeeperInfo.m_strMapConfigRootNodePath.compare(szPath) != 0)
    {
        SERVICE_LOG_ZOOKEEPER_DEBUG << "|MAYBE AN OLD WATCHER|PATH:" << szPath << "|CONFIG PATH:" 
                         << pSelf->m_stZookeeperInfo.m_strMapConfigRootNodePath << std::endl;
        return; /* maybe this is an old watcher, ignore it, zookeeper 3.5.0 has a better solution; */
    }

	SERVICE_LOG_ZOOKEEPER_DEBUG << "|MAP CONFIG NOTIFY|PATH:" << szPath << "|TYPE ID:" << iType << "|TYPE:" << Type2String(iType)
					 << "|STATE ID:" << iState << "|STATE:" << State2String(iState) << std::endl;

    int iRet = pSelf->WatchAndGetMapConfigList();
    if(ZOK != iRet)
    {
        SERVICE_LOG_ZOOKEEPER_ERROR << "|WATCH MAP CONFIG LIST ERROR|ERRCODE:" << iRet << "|ERRMSG:" << Errno2String(iRet) << std::endl;
        pSelf->SetZookeeperAbnormal();
    }
}

void 
CLogicZookeeper::MapConfigListEventComplete(int iRet, const struct String_vector* pStrings, const void* pContext)
{
	CLogicZookeeper* pSelf = (CLogicZookeeper*)pContext;
    if(ZOK == iRet)
    {
        for(int i = 0; i < pStrings->count; i ++)
        {
            std::string strFullMapPath = pSelf->m_stZookeeperInfo.m_strMapConfigRootNodePath + "/" + pStrings->data[i];
            pSelf->m_stConfigCompletePool.AddConfigFile(strFullMapPath, pSelf);
			SERVICE_LOG_ZOOKEEPER_DEBUG << "|MAP CONFIG LIST COMPLETE|PATH:" << strFullMapPath << std::endl;
        }

        iRet = pSelf->WatchAndGetConfigContent();
        if(ZOK != iRet)
        {
            SERVICE_LOG_ZOOKEEPER_ERROR << "|WATCH MAP CONFIG LIST ERROR|ERRCODE:" << iRet << "|ERRMSG:" << Errno2String(iRet) << std::endl;
            pSelf->SetZookeeperAbnormal();
        }
    }
    else
    {
        SERVICE_LOG_ZOOKEEPER_ERROR << "|MAP CONFIG LIST EVENT COMPLETE ERROR|ERRCODE:" << iRet << "|ERRMSG:" << Errno2String(iRet) << std::endl;
		pSelf->SetZookeeperAbnormal();
    }
}

//配置文件内容状态的回调
void 
CLogicZookeeper::ConfigContentEventWatcher(zhandle_t* pZooHandle, int iType, int iState, const char* szPath, void* pContext)
{
    if(ZOO_SESSION_EVENT == iType)
    {
        SERVICE_LOG_ZOOKEEPER_DEBUG << "|STATUS NOTIFY|TYPE ID:" << iType << "|TYPE NAME"
                                    << Type2String(iType) << "|STATE ID:" << iState
                                    << "|STATE NAME:" << State2String(iState) << std::endl;
        return;
    }

    /*
    SERVICE_LOG_ZOOKEEPER_INFO << "|CONFIG CONTENT NOTIFY|PATH:" << szPath << "|TYPE ID:" << iType 
                    << "|TYPE:" << Type2String(iType) << "|STATE ID:" << iState 
                    << "|STATE:" << State2String(iState) << std::endl;
                    */

    CLogicZookeeper::config_map_type::value_type* pValue = (CLogicZookeeper::config_map_type::value_type*)pContext;
    CLogicZookeeper* pSelf = pValue->second.second;
    if(ZOO_DELETED_EVENT == iType)
    {
        pSelf->m_stConfigCompletePool.DeleteConfigFile(std::string(szPath));
        SERVICE_LOG_ZOOKEEPER_DEBUG << "DELETE CONFIG|CONFIG:" << szPath << std::endl;
    }
    else
    {
        int iRet = zoo_awget(pSelf->m_pZooHandle, szPath, ConfigContentEventWatcher, pValue, ConfigContentEventComplete, pValue);
        if(ZOK != iRet)
        {
            SERVICE_LOG_ZOOKEEPER_ERROR << "|WATCH CONFIG ERROR|CONFIG:" << szPath << "|ERRCODE:" << iRet << "|ERRMSG:" << Errno2String(iRet) << std::endl;
            pSelf->SetZookeeperAbnormal();
        }
    }
}

void 
CLogicZookeeper::ConfigContentEventComplete(int iRet, const char* iValue, int iLength, const struct Stat* stStat, const void* pContext)
{
    (void)stStat;
	CLogicZookeeper::config_map_type::value_type* pValue = (CLogicZookeeper::config_map_type::value_type*)pContext;
	CLogicZookeeper* pSelf = pValue->second.second;
    if(ZOK == iRet)
    {
        pSelf->m_stConfigCompletePool.RefreshConfigFile(pValue->first, std::string(iValue, iLength));
		SERVICE_LOG_ZOOKEEPER_DEBUG << "|CONFIG CONTENT COMPLETE|PATH:" << pValue->first << std::endl;
    }
    else
    {
        SERVICE_LOG_ZOOKEEPER_ERROR << "|CONFIG CONTENT COMPLETE ERROR|CONFIG:" << pValue->first << "|ERRCODE:" << iRet << "|ERRMSG:" << Errno2String(iRet) << std::endl;
		pSelf->SetZookeeperAbnormal();
    }
}

void
CLogicZookeeper::CreateTempServerNodeComplete(int iRet, const char* pName, const void* pContext)
{
	CLogicZookeeper* pSelf = (CLogicZookeeper*)pContext;
    if(ZOK == iRet || ZNODEEXISTS == iRet)
    {
        int iMapRet = pSelf->WatchAndGetMapConfigList();
        if(ZOK != iMapRet)
        {
            SERVICE_LOG_ZOOKEEPER_ERROR << "|WATCH MAP CONFIG LIST ERROR|PATH:" << pSelf->m_stZookeeperInfo.m_strMapConfigRootNodePath
                             << "|ERRCODE:" << iMapRet << "|ERRMSG:" << Errno2String(iMapRet) << std::endl;
            pSelf->SetZookeeperAbnormal();
        }
    }
    else
    {
        std::string strPath;
        if(pName) strPath = pName;
        
        SERVICE_LOG_ZOOKEEPER_ERROR << "|CREATE TEMP SERVER NODE ERROR|PATH:" << strPath << "|ERRCODE:" << iRet << "|ERRMSG:" << Errno2String(iRet) << std::endl;
		pSelf->SetZookeeperAbnormal();
    }
}

/////////////////////////////////////////////////////////////////////////////////
CLogicZookeeper::CLogicZookeeper()
: m_stZookeeperInfo(), m_pZooHandle(nullptr), m_stConfigCompletePool(),
  m_enuZookeeperHandleStatus(NORMAL), m_stConfigObserver()
{  }

CLogicZookeeper::~CLogicZookeeper()
{
    DestroyZookeeper();
}

void 
CLogicZookeeper::SetZookeeperInfo(const zookeeper_info_t& stZookeeperInfo)
{
    m_stZookeeperInfo = stZookeeperInfo;
}

void 
CLogicZookeeper::RegisterObserver(const reload_func_type &stObserver)
{
    m_stConfigObserver.m_stConfigObserverList.push_back(stObserver);
}

void 
CLogicZookeeper::ClearZooKeeperInfo()
{
    m_pZooHandle = nullptr;
    m_enuZookeeperHandleStatus = NORMAL;
    m_stConfigCompletePool.ResetPool();
}

void 
CLogicZookeeper::SetZookeeperAbnormal()
{
    m_enuZookeeperHandleStatus = NEED_DESTORY;
}

bool 
CLogicZookeeper::IsZookeeperNormal() const
{
    return (m_enuZookeeperHandleStatus == NORMAL);
}

int
CLogicZookeeper::CreateTempServerNode()
{
    // ZOO_EPHEMERAL
    return (zoo_acreate(m_pZooHandle, m_stZookeeperInfo.m_strTempServerNodeName.c_str(), "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, CreateTempServerNodeComplete, (void*)this));
}

int 
CLogicZookeeper::WatchAndGetMapConfigList()
{
    return (zoo_awget_children(m_pZooHandle, m_stZookeeperInfo.m_strMapConfigRootNodePath.c_str(), MapConfigListEventWatcher, this, MapConfigListEventComplete, this));
}

int 
CLogicZookeeper::WatchAndGetConfigContent()
{
    int iRet = ZOK;
    for(auto& stConfigKV : m_stConfigCompletePool.m_stConfigFileMap)
    {
        iRet = zoo_awget(m_pZooHandle, stConfigKV.first.c_str(), ConfigContentEventWatcher, &stConfigKV, ConfigContentEventComplete, &stConfigKV);
        if(iRet != ZOK)
        {
            SERVICE_LOG_ZOOKEEPER_ERROR << "|WATCH CONFIG ERROR|CONFIG:" << stConfigKV.first << "|ERRCODE:" << iRet << "|ERRMSG:" << Errno2String(iRet) << std::endl;
            break;
        }
    }

    return (iRet);
}

bool 
CLogicZookeeper::InitZookeeper()
{
    // enable deterministic order
    zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
    zoo_deterministic_conn_order(1);
    m_enuZookeeperHandleStatus = NORMAL;
    m_pZooHandle = zookeeper_init(m_stZookeeperInfo.m_strHost.c_str(), ZooHandleEventWatcher, m_stZookeeperInfo.m_iTimeout, nullptr, (void*)this, 0);
    assert(m_pZooHandle != nullptr && "ZOOKEEPER INIT FAULT!!!");

    for (const auto& strFileName : m_stZookeeperInfo.m_stNoramlConfigFileList)
    {
        m_stConfigCompletePool.AddConfigFile(strFileName, this);
    }

    int iMapRet = WatchAndGetMapConfigList();
    if(ZOK != iMapRet)
    {
        SERVICE_LOG_ZOOKEEPER_ERROR << "|WATCH MAP CONFIG LIST ERROR|PATH:" << m_stZookeeperInfo.m_strMapConfigRootNodePath
                         << "|ERRCODE:" << iMapRet << "|ERRMSG:" << Errno2String(iMapRet) << std::endl;
        goto fail;
    }

    //int iRet = CreateTempServerNode();
	//if(ZOK != iRet)
    //{
    //    SERVICE_LOG_ZOOKEEPER_ERROR << "|CREATE TEMP SERVER NODE ERROR|PATH:" << m_stZookeeperInfo.m_strTempServerNodeName << "|ERRCODE:"
    //                     << iRet << "|ERRMSG:" << Errno2String(iRet) << std::endl;
//
    //    goto fail;
    //}
    
    return (true);
    
fail:
    DestroyZookeeper();
    return (false);
}

void 
CLogicZookeeper::DestroyZookeeper()
{
    zookeeper_close(m_pZooHandle);
    ClearZooKeeperInfo();
}

int
CLogicZookeeper::Interest(int& iFd, int& iEvent)
{
    iFd = -1;
    iEvent = 0;
    if(!IsZookeeperNormal()) DestroyZookeeper();
    
    if(!m_pZooHandle) InitZookeeper();
    
    if(m_pZooHandle)
    {        
        struct timeval stTimeout;
        int iRet = zookeeper_interest(m_pZooHandle, &iFd, &iEvent, &stTimeout);
        if(iRet != ZOK)
        {
            SERVICE_LOG_ZOOKEEPER_ERROR << "|ZOOKEEPER INTEREST ERROR|ERRCODE:" << iRet << "|MSG:" << Errno2String(iRet) << std::endl;
            DestroyZookeeper();
            return (-1);
        }
    }

    return (0);
}

int 
CLogicZookeeper::Process(int iEvent)
{
    if (IsZookeeperNormal())
    {
        zookeeper_process(m_pZooHandle, iEvent);
    }

    return (0);
}

void
CLogicZookeeper::UpdateConfig()
{
    if (m_stConfigCompletePool.m_iLastUpdateConfigTime >= m_stConfigObserver.m_iLastUpdateTime)
    {
        if (m_stConfigCompletePool.m_iLastUpdateConfigTime < (int)time(nullptr) - 3)
        {
            m_stConfigObserver.UpdateConfig(*this);
        }
    }
}

bool 
CLogicZookeeper::WaitAllConfigReady(int iTimeout)
{
    if (!IsZookeeperNormal()) return (false);

    fd_set rfds, wfds, efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

	int iZookeeperFd;

    struct timeval stTv;
    struct timeval* pTimeout = nullptr;
    if(iTimeout >= 0)
    {
        stTv.tv_sec = decltype(stTv.tv_sec)(iTimeout / 1000);
        stTv.tv_usec = decltype(stTv.tv_usec)((iTimeout % 1000) * 1000);
        pTimeout = &stTv;
    }
    
    while(true)
    {
        int iCurrentZookeeperIOEvent = 0;
        struct timeval stTmp;
		if(!IsZookeeperNormal())
		{
			SERVICE_LOG_ZOOKEEPER_ERROR << "|ZOOKEEPER ERROR" << std::endl;
			return (false);
		}

        int iRet = zookeeper_interest(m_pZooHandle, &iZookeeperFd, &iCurrentZookeeperIOEvent, &stTmp);
        if(iRet != ZOK)
        {
            SERVICE_LOG_ZOOKEEPER_ERROR << "|ZOOKEEPER INTEREST ERROR|ERRCODE:" << iRet << "|MSG:" << Errno2String(iRet) << std::endl;
            SetZookeeperAbnormal();
            return (false);
        }

        if(iCurrentZookeeperIOEvent & ZOOKEEPER_READ) FD_SET(iZookeeperFd, &rfds);
        else FD_CLR(iZookeeperFd, &rfds);

        if(iCurrentZookeeperIOEvent & ZOOKEEPER_WRITE) FD_SET(iZookeeperFd, &wfds);
        else FD_CLR(iZookeeperFd, &wfds);

        iCurrentZookeeperIOEvent = 0;
        iRet = select(iZookeeperFd + 1, &rfds, &wfds, &efds, pTimeout);
        if(iRet > 0)
        {
            if(FD_ISSET(iZookeeperFd, &rfds)) iCurrentZookeeperIOEvent |= ZOOKEEPER_READ;
            if(FD_ISSET(iZookeeperFd, &wfds)) iCurrentZookeeperIOEvent |= ZOOKEEPER_WRITE;
        }
        else
        {
            if(iRet == 0)
            {
                SERVICE_LOG_ZOOKEEPER_ERROR << "|WAIT CONFIG COMPLETE TIMEOUT" << std::endl;
            }
            else
            {
                //SERVICE_LOG_ZOOKEEPER_ERROR << "|WAIT CONFIG COMPLETE ERROR|ERRCODE:" << errno << "|ERRMSG:" << strerror(errno) << std::endl;
                if(errno != EINTR) return (false);
            }
        }

        zookeeper_process(m_pZooHandle, iCurrentZookeeperIOEvent);
        if(m_stConfigCompletePool.IsLoadComplete())
        {
            m_stConfigObserver.UpdateConfig(*this);
            return (true);
        }
    }
}

bool
CLogicZookeeper::GetZookeeperInfoFromFile(const std::string& strFileName, zookeeper_info_t& stZookeeperInfo)
{
    std::ifstream stFin(strFileName.c_str());
    if (false == stFin.good())
    {
        SERVICE_LOG_ZOOKEEPER_ERROR << "CAN NOT OPEN INFO FILE|FILE NAME:" << strFileName << std::endl;
        return false;
    }

    char szBuffer[128]; 

    // host
    stFin.getline(szBuffer, 128);
    stZookeeperInfo.m_strHost = szBuffer;

    if (!stFin.good())
    {
        SERVICE_LOG_ZOOKEEPER_ERROR << "SET ZOOKEEPER INFO FAILED, BAD ARGUMENT" << std::endl;
        return false;
    }

    // logic config
    stFin.getline(szBuffer, 128);
    for (int i = 0; szBuffer[i] != '\0'; ++i)
        if (szBuffer[i] < ' ')
            szBuffer[i] = '\0';

    if (szBuffer[0] == '/')
        stZookeeperInfo.m_strLogicConfigNodePath = szBuffer;
    else
        stZookeeperInfo.m_strLogicConfigNodePath = std::string("/") + szBuffer;

    if (!stFin.good())
    {
        SERVICE_LOG_ZOOKEEPER_ERROR << "SET ZOOKEEPER INFO FAILED, BAD ARGUMENT" << std::endl;
        return false;
    }

    // server node
    stFin.getline(szBuffer, 128);
    for (int i = 0; szBuffer[i] != '\0'; ++i)
        if (szBuffer[i] < ' ')
            szBuffer[i] = '\0';

    if (szBuffer[0] == '/')
        stZookeeperInfo.m_strTempServerNodeName = szBuffer;
    else
        stZookeeperInfo.m_strTempServerNodeName = std::string("/") + szBuffer;

    if (!stFin.good())
    {
        SERVICE_LOG_ZOOKEEPER_ERROR << "SET ZOOKEEPER INFO FAILED, BAD ARGUMENT" << std::endl;
        return false;
    }

    // base config
    stFin.getline(szBuffer, 128);
    for (int i = 0; szBuffer[i] != '\0'; ++i)
        if (szBuffer[i] < ' ')
            szBuffer[i] = '\0';

    if (szBuffer[0] == '/')
        stZookeeperInfo.m_strNormalConfigNodePath = szBuffer;
    else
        stZookeeperInfo.m_strNormalConfigNodePath = std::string("/") + szBuffer;

    if (!stFin.good())
    {
        SERVICE_LOG_ZOOKEEPER_ERROR << "SET ZOOKEEPER INFO FAILED, BAD ARGUMENT" << std::endl;
        return false;
    }

    // map config
    stFin.getline(szBuffer, 128);
    for (int i = 0; szBuffer[i] != '\0'; ++i)
        if (szBuffer[i] < ' ')
            szBuffer[i] = '\0';

    if (szBuffer[0] == '/')
        stZookeeperInfo.m_strMapConfigRootNodePath = szBuffer;
    else
        stZookeeperInfo.m_strMapConfigRootNodePath = std::string("/") + szBuffer;

    return true;
}

void 
CLogicZookeeper::AddWatchFile(const std::string& strFileName)
{
    std::string strFullName;
    if (strFileName.compare(LOGIC_CONFIG) != 0)
    {
        strFullName = m_stZookeeperInfo.m_strNormalConfigNodePath + "/" + strFileName;
    }
    else
    {
        strFullName = m_stZookeeperInfo.m_strLogicConfigNodePath + "/" + strFileName;
    }

    m_stZookeeperInfo.m_stNoramlConfigFileList.push_back(strFullName);
}

const std::string&
CLogicZookeeper::GetNormalConfig(const std::string& strFileName) const
{
    std::string strFullName;
    if (strFileName.compare(LOGIC_CONFIG) != 0)
    {
        strFullName = m_stZookeeperInfo.m_strNormalConfigNodePath + "/" + strFileName;
    }
    else
    {
        strFullName = m_stZookeeperInfo.m_strLogicConfigNodePath + "/" + strFileName;
    }

    return (m_stConfigCompletePool.GetConfigContent(strFullName));
}

const std::string& 
CLogicZookeeper::GetMapConfig(const std::string& strFileName) const
{
    std::string strFullName = m_stZookeeperInfo.m_strMapConfigRootNodePath + "/" + strFileName;
    return (m_stConfigCompletePool.GetConfigContent(strFullName));
}

void
CLogicZookeeper::SetErrorInfo(const std::string& strErrorInfo) const
{
    zoo_aset(m_pZooHandle, m_stZookeeperInfo.m_strTempServerNodeName.c_str(), strErrorInfo.c_str(), strErrorInfo.length(), -1, setDataSilentComplete, nullptr);
}
