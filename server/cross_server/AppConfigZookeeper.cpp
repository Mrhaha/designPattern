#include "AppConfigZookeeper.h"
#include "AppLog.h"
#include "GameControl.h"
#include <fstream>


const char* CAppConfigZookeeper::Type2String(int iEventType)
{
    if (iEventType == ZOO_CREATED_EVENT) return ("CREATED_EVENT");
    if (iEventType == ZOO_DELETED_EVENT) return ("DELETED_EVENT");
    if (iEventType == ZOO_CHANGED_EVENT) return ("CHANGED_EVENT");
    if (iEventType == ZOO_CHILD_EVENT) return ("CHILD_EVENT");
    if (iEventType == ZOO_SESSION_EVENT) return ("SESSION_EVENT");
    if (iEventType == ZOO_NOTWATCHING_EVENT) return ("NOTWATCHING_EVENT");

    return ("UNKNOWN_EVENT_TYPE");
}

const char* CAppConfigZookeeper::State2String(int iState)
{
    if (iState == 0) return ("CLOSED_STATE");
    if (iState == ZOO_CONNECTING_STATE) return ("CONNECTING_STATE");
    if (iState == ZOO_ASSOCIATING_STATE) return ("ASSOCIATING_STATE");
    if (iState == ZOO_CONNECTED_STATE) return ("CONNECTED_STATE");
    if (iState == ZOO_EXPIRED_SESSION_STATE) return ("EXPIRED_SESSION_STATE");
    if (iState == ZOO_AUTH_FAILED_STATE) return ("AUTH_FAILED_STATE");

    return ("INVALID_STATE");
}

const char* CAppConfigZookeeper::Errno2String(int iRet)
{
    return (zerror(iRet));
}

/////////////////////////////////////////////////////////////////////////////////
void CAppConfigZookeeper::ZooHandleEventWatcher(zhandle_t* pZooHandle, int iType, int iState, const char* szPath, void* pContext)
{
    CAppConfigZookeeper* pSelf = static_cast<CAppConfigZookeeper*>(pContext);
    TRACESVR(log_mask_detail, "[CAppConfigZookeeper::%s]ZOOKEEPER|WATCHER:%s|STATE:%s\n", __FUNCTION__, Type2String(iType), State2String(iState));

    if (iType == ZOO_SESSION_EVENT)
    {
        if (iState == ZOO_CONNECTED_STATE)
        {
            const clientid_t* pClientID = zoo_client_id(pZooHandle);
            TRACESVR(log_mask_detail, "[CAppConfigZookeeper::%s]ZOOKEEPER||GOT A NEW SESSION ID:%ld\n", __FUNCTION__, pClientID->client_id);

        }
        else if (iState == ZOO_AUTH_FAILED_STATE)
        {
            TRACESVR(log_mask_system_error, "[CAppConfigZookeeper::%s]ZOOKEEPER|ZOOKEEPER AUTHENTICATION FAILURE\n", __FUNCTION__);

            pSelf->SetZookeeperAbnormal();
        }
        else if (iState == ZOO_EXPIRED_SESSION_STATE)
        {
            TRACESVR(log_mask_system_error, "[CAppConfigZookeeper::%s]ZOOKEEPER|SESSION EXPIRED\n", __FUNCTION__);

            pSelf->SetZookeeperAbnormal();
        }
    }
}

//配置文件内容状态的回调
void CAppConfigZookeeper::ConfigContentEventWatcher(zhandle_t* pZooHandle, int iType, int iState, const char* szPath, void* pContext)
{
    if (ZOO_SESSION_EVENT == iType)
    {
        //RK_ZOO_LOG_DEBUG << "|STATUS NOTIFY|TYPE:" << iType << "|STATE:" << iState << std::endl;
        return;
    }

    /*
    RK_ZOO_LOG_INFO << "|CONFIG CONTENT NOTIFY|PATH:" << szPath << "|TYPE ID:" << iType
    << "|TYPE:" << Type2String(iType) << "|STATE ID:" << iState
    << "|STATE:" << State2String(iState) << std::endl;
    */

    CAppConfigZookeeper::config_map_type::value_type* pValue = (CAppConfigZookeeper::config_map_type::value_type*)pContext;
    CAppConfigZookeeper* pSelf = pValue->second.second;
    if (ZOO_DELETED_EVENT == iType)
    {
        pSelf->m_stConfigCompletePool.DeleteConfigFile(std::string(szPath));
        TRACESVR(log_mask_detail, "[CAppConfigZookeeper::ConfigContentEventWatcher]DELETE CONFIG|CONFIG::%s\n", szPath);
    }
    else
    {
        int iRet = zoo_awget(pSelf->m_pZooHandle, szPath, ConfigContentEventWatcher, pValue, ConfigContentEventComplete, pValue);
        if (ZOK != iRet)
        {
            TRACESVR(log_mask_system_error, "[CAppConfigZookeeper::ConfigContentEventComplete]WATCH CONFIG ERROR|CONFIG:%s|ERRCODE:%d|ERRMSG:%s\n",
                szPath, iRet, Errno2String(iRet));
            pSelf->SetZookeeperAbnormal();
        }
    }
}

void CAppConfigZookeeper::ConfigContentEventComplete(int iRet, const char* iValue, int iLength, const struct Stat* stStat, const void* pContext)
{
    (void)stStat;
    CAppConfigZookeeper::config_map_type::value_type* pValue = (CAppConfigZookeeper::config_map_type::value_type*)pContext;
    CAppConfigZookeeper* pSelf = pValue->second.second;
    if (ZOK == iRet)
    {
        pSelf->m_stConfigCompletePool.RefreshConfigFile(pValue->first, std::string(iValue, iLength));
        TRACESVR(log_mask_system_error, "[CAppConfigZookeeper::ConfigContentEventComplete]CONFIG CONTENT COMPLETE|PATH:%s\n", pValue->first.c_str());
    }
    else
    {
        TRACESVR(log_mask_system_error, "[CAppConfigZookeeper::ConfigContentEventComplete]CONFIG CONTENT COMPLETE ERROR|CONFIG:%s|ERRCODE:%d|ERRMSG:%s\n",
            pValue->first.c_str(), iRet, Errno2String(iRet));
        pSelf->SetZookeeperAbnormal();
    }
}

/////////////////////////////////////////////////////////////////////////////////
CAppConfigZookeeper::CAppConfigZookeeper()
    : m_stZookeeperInfo(), m_pZooHandle(nullptr), m_stConfigCompletePool(),
    m_enuZookeeperHandleStatus(NORMAL), m_stConfigObserver()
{  }

CAppConfigZookeeper::~CAppConfigZookeeper()
{
    DestoryZookeeper();
}

void CAppConfigZookeeper::SetZookeeperInfo(const std::string& host, const std::string& config_path)
{
    m_stZookeeperInfo.m_strHost = host;
    m_stZookeeperInfo.m_strNormalConfigNodePath = config_path;
}

void CAppConfigZookeeper::RegsterObserver(const reload_func_type& stObserver)
{
    m_stConfigObserver.m_stConfigObserverList.push_back(stObserver);
}

void CAppConfigZookeeper::ClearZooKeeperInfo()
{
    m_pZooHandle = nullptr;
    m_enuZookeeperHandleStatus = NORMAL;
    m_stConfigCompletePool.ResetPool();
}

void CAppConfigZookeeper::SetZookeeperAbnormal()
{
    m_enuZookeeperHandleStatus = NEED_DESTORY;
}

bool CAppConfigZookeeper::IsZookeeperNormal() const
{
    return (m_enuZookeeperHandleStatus == NORMAL);
}

int CAppConfigZookeeper::WatchAndGetConfigContent()
{
    int iRet = ZOK;
    for (auto& stConfigKV : m_stConfigCompletePool.m_stConfigFileMap)
    {
        iRet = zoo_awget(m_pZooHandle, stConfigKV.first.c_str(), ConfigContentEventWatcher, &stConfigKV, ConfigContentEventComplete, &stConfigKV);
        if (iRet != ZOK)
        {
            TRACESVR(log_mask_system_error, "[CAppConfigZookeeper::WatchAndGetConfigContent]WATCH CONFIG ERROR|CONFIG:%s|ERRCODE:%d|ERRMSG:%s\n",
                stConfigKV.first.c_str(), iRet, Errno2String(iRet));
            break;
        }
    }

    return (iRet);
}

bool CAppConfigZookeeper::InitZookeeper()
{
    // enable deterministic order
    zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
    zoo_deterministic_conn_order(1);
    m_enuZookeeperHandleStatus = NORMAL;
    m_pZooHandle = zookeeper_init(m_stZookeeperInfo.m_strHost.c_str(), ZooHandleEventWatcher, 30000, nullptr, static_cast<void*>(this), 0);
    assert(m_pZooHandle != nullptr && "ZOOKEEPER INIT FAULT!!!");

    for (const auto& strFileName : m_stZookeeperInfo.m_stNoramlConfigFileList)
    {
        m_stConfigCompletePool.AddConfigFile(strFileName, this);
    }

    const int32_t iRet = WatchAndGetConfigContent();
    if (ZOK != iRet)
    {
        TRACESVR(log_mask_system_error, "[CAppConfigZookeeper::InitZookeeper]watch and get config content error, path:%s code:%d msg:%s\n",
            m_stZookeeperInfo.m_strNormalConfigNodePath.c_str(), iRet, Errno2String(iRet));
        DestoryZookeeper();
        return false;
    }

    return (true);
}

void CAppConfigZookeeper::DestoryZookeeper()
{
    zookeeper_close(m_pZooHandle);
    ClearZooKeeperInfo();
}

int CAppConfigZookeeper::Interest(int& iFd, int& iEvent)
{
    iFd = -1;
    iEvent = 0;
    if (!IsZookeeperNormal()) DestoryZookeeper();

    if (!m_pZooHandle) InitZookeeper();

    if (m_pZooHandle)
    {
        struct timeval stTimeout;
        int iRet = zookeeper_interest(m_pZooHandle, &iFd, &iEvent, &stTimeout);
        if (iRet != ZOK)
        {
            TRACESVR(log_mask_system_error, "[CAppConfigZookeeper::Interest]ZOOKEEPER INTEREST ERROR|ERRCODE:%d msg:%s\n",
                iRet, Errno2String(iRet));
            DestoryZookeeper();
            return (-1);
        }
    }

    return (0);
}

int CAppConfigZookeeper::Process(int iEvent)
{
    if (IsZookeeperNormal())
    {
        zookeeper_process(m_pZooHandle, iEvent);
    }

    return (0);
}

void CAppConfigZookeeper::UpdateConfig()
{
    if (m_stConfigCompletePool.m_iLastUpdateConfigTime >= m_stConfigObserver.m_iLastUpdateTime)
    {
        const time_t now = time(nullptr);
        if (m_stConfigCompletePool.m_iLastUpdateConfigTime < now)
        {
            m_stConfigObserver.UpdateConfig(now, *this);
        }
    }
}

bool CAppConfigZookeeper::WaitAllConfigReady(int iTimeout)
{
    if (!IsZookeeperNormal()) return (false);

    fd_set rfds, wfds, efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

    int iZookeeperFd;

    struct timeval stTv;
    struct timeval* pTimeout = nullptr;
    if (iTimeout >= 0)
    {
        stTv.tv_sec = decltype(stTv.tv_sec)(iTimeout / 1000);
        stTv.tv_usec = decltype(stTv.tv_usec)((iTimeout % 1000) * 1000);
        pTimeout = &stTv;
    }

    while (true)
    {
        int iCurrentZookeeperIOEvent = 0;
        struct timeval stTmp;
        if (!IsZookeeperNormal())
        {
            TRACESVR(log_mask_system_error, "ZOOKEEPER ERROR, IS NOT NORMAL\n");
            return (false);
        }

        int iRet = zookeeper_interest(m_pZooHandle, &iZookeeperFd, &iCurrentZookeeperIOEvent, &stTmp);
        if (iRet != ZOK)
        {
            TRACESVR(log_mask_system_error, "ZOOKEEPER INTEREST ERROR, ERRCODE:%d, MSG:%s\n", iRet, Errno2String(iRet));
            SetZookeeperAbnormal();
            return (false);
        }

        if (iCurrentZookeeperIOEvent & ZOOKEEPER_READ) FD_SET(iZookeeperFd, &rfds);
        else FD_CLR(iZookeeperFd, &rfds);

        if (iCurrentZookeeperIOEvent & ZOOKEEPER_WRITE) FD_SET(iZookeeperFd, &wfds);
        else FD_CLR(iZookeeperFd, &wfds);

        iCurrentZookeeperIOEvent = 0;
        iRet = select(iZookeeperFd + 1, &rfds, &wfds, &efds, pTimeout);
        if (iRet > 0)
        {
            if (FD_ISSET(iZookeeperFd, &rfds)) iCurrentZookeeperIOEvent |= ZOOKEEPER_READ;
            if (FD_ISSET(iZookeeperFd, &wfds)) iCurrentZookeeperIOEvent |= ZOOKEEPER_WRITE;
        }
        else
        {
            if (iRet == 0)
            {
                TRACESVR(log_mask_system_error, "WAIT CONFIG COMPLETE TIMEOUT\n");
            }
            else
            {
                TRACESVR(log_mask_system_error, "WAIT CONFIG COMPLETE ERROR|ERRCODE:%d, ERRMSG:%s\n", errno, strerror(errno));
                if (errno != EINTR) return (false);
            }
        }

        zookeeper_process(m_pZooHandle, iCurrentZookeeperIOEvent);
        if (m_stConfigCompletePool.IsLoadComplete())
        {
            m_stConfigObserver.UpdateConfig(time(nullptr), *this);
            return (true);
        }
    }
}

void CAppConfigZookeeper::AddWatchFile(const std::string& strFileName)
{
    const std::string strFullName = m_stZookeeperInfo.m_strNormalConfigNodePath + "/" + strFileName;
    m_stZookeeperInfo.m_stNoramlConfigFileList.push_back(strFullName);
}

const std::string& CAppConfigZookeeper::GetNormalConfig(const std::string& strFileName) const
{
    const std::string strFullName = m_stZookeeperInfo.m_strNormalConfigNodePath + "/" + strFileName;
    return (m_stConfigCompletePool.GetConfigContent(strFullName));
}
