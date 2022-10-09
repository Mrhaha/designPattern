#pragma once

#include <functional>
#include <vector>

#include "zookeeper.h"
#include <map>
#include <string>


class CQueueZookeeperInfo
{
public:
    std::string                 m_strHost;
    std::string                 m_strNormalConfigNodePath;
    std::vector<std::string>    m_stNoramlConfigFileList;
};

class CQueueAppConfigZookeeper
{
public:
    enum ZOOKEEPER_CONFIG_STATUS
    {
        NORMAL = 0,
        NEED_DESTORY,
    };

    typedef std::function<bool(const CQueueAppConfigZookeeper&)> reload_func_type;

    struct config_observer_t
    {
        config_observer_t() : m_stConfigObserverList(), m_iLastUpdateTime(0) {  }

        void UpdateConfig(time_t iLoadTime, const CQueueAppConfigZookeeper& stConfigZookeeper)
        {
            for (const auto& stObserver : m_stConfigObserverList)
            {
                stObserver(stConfigZookeeper);
            }

            m_iLastUpdateTime = iLoadTime;
        }

        std::vector<reload_func_type>   m_stConfigObserverList;
        int                             m_iLastUpdateTime;
    };

public:
    typedef std::map<std::string, std::pair<std::string, CQueueAppConfigZookeeper*>> config_map_type;

    struct config_complete_pool_t
    {
        config_complete_pool_t()
            : m_uiHasCompletedConfigFileNum(0), m_iLastUpdateConfigTime(0), m_strEmptyContent() {}

        bool IsLoadComplete() const
        {
            return ((m_stConfigFileMap.size() <= m_uiHasCompletedConfigFileNum) && (m_uiHasCompletedConfigFileNum != 0));
        }

        void AddConfigFile(const std::string& strFileName, CQueueAppConfigZookeeper* pZookeeperConfig)
        {
            const auto stIter = m_stConfigFileMap.find(strFileName);
            if (stIter == m_stConfigFileMap.end())
            {
                m_stConfigFileMap[strFileName] = std::make_pair("", pZookeeperConfig);
            }
        }

        void RefreshConfigFile(const std::string& strFileName, const std::string& strFileContent)
        {
            if (!strFileContent.empty())
            {
                auto stIter = m_stConfigFileMap.find(strFileName);
                if (stIter != m_stConfigFileMap.end())
                {
                    if (stIter->second.first.empty()) m_uiHasCompletedConfigFileNum++;

                    stIter->second.first = strFileContent;
                    m_iLastUpdateConfigTime = (int)time(nullptr);
                }
            }
        }

        void DeleteConfigFile(const std::string& strFileName)
        {
            const auto stIter = m_stConfigFileMap.find(strFileName);
            if (stIter != m_stConfigFileMap.end())
            {
                if (!stIter->second.first.empty())
                {
                    m_uiHasCompletedConfigFileNum--;
                }

                m_stConfigFileMap.erase(stIter);
            }
        }

        void ResetPool()
        {
            m_stConfigFileMap.clear();
            m_uiHasCompletedConfigFileNum = 0;
            m_iLastUpdateConfigTime = 0;
        }

        const std::string& GetConfigContent(const std::string& strFileName) const
        {
            const auto& stIter = m_stConfigFileMap.find(strFileName);
            if (stIter == m_stConfigFileMap.end())
            {
                return (m_strEmptyContent);
            }

            return (stIter->second.first);
        }

        config_map_type         m_stConfigFileMap;
        size_t                  m_uiHasCompletedConfigFileNum;
        int                     m_iLastUpdateConfigTime;
        std::string             m_strEmptyContent;
    };

public:
    static const char* Type2String(int iEventType);
    static const char* State2String(int iState);
    static const char* Errno2String(int iRet);

public:
  CQueueAppConfigZookeeper();

    ~CQueueAppConfigZookeeper();

    void SetZookeeperInfo(const std::string& host, const std::string& config_path);

    void RegsterObserver(const reload_func_type& stObserver);

    bool InitZookeeper();

    int Interest(int& iFd, int& iEvent);

    int Process(int iEvent);

    void UpdateConfig();

    bool WaitAllConfigReady(int iTimeout);//timeout:ms

    void AddWatchFile(const std::string& strFileName);

    const std::string& GetNormalConfig(const std::string& strFileName) const;

private:
    void DestoryZookeeper();

    void ClearZooKeeperInfo();

    void SetZookeeperAbnormal();

    bool IsZookeeperNormal() const;

    int WatchAndGetConfigContent();

private:
    //zoo handle状态变更的回调
    static void ZooHandleEventWatcher(zhandle_t* pZooHandle, int iType, int iState, const char* szPath, void* pContext);
    //配置文件内容状态的回调
    static void ConfigContentEventWatcher(zhandle_t* pZooHandle, int iType, int iState, const char* szPath, void* pContext);
    //配置文件内容变更的回调
    static void ConfigContentEventComplete(int iRet, const char* iValue, int iLength, const struct Stat* stStat, const void* pContext);
    //设置错误信息完成的回调
    static void setDataSilentComplete(int iRet, const struct Stat* stStat, const void* szContext) { }

private:
  CQueueZookeeperInfo m_stZookeeperInfo;
    zhandle_t*                              m_pZooHandle;
    config_complete_pool_t                  m_stConfigCompletePool;
    enum ZOOKEEPER_CONFIG_STATUS			m_enuZookeeperHandleStatus;
    config_observer_t                       m_stConfigObserver;
};

