#pragma once

#include <map>
#include <string>
#include <vector>
#include <functional>

#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "zookeeper.h"

#include "tc_singleton.h"

class CLogicZookeeper
{
public:
    enum ZOOKEEPER_LOGIC_STATUS
    {
        NORMAL = 0,
        NEED_DESTORY,
    };

    typedef std::function<bool(const CLogicZookeeper&)> reload_func_type;

    struct config_observer_t
    {
        config_observer_t() : m_stConfigObserverList(), m_iLastUpdateTime(0) {  }

        void UpdateConfig(const CLogicZookeeper& stConfigZookeeper)
        {
            for (const auto& stObserver : m_stConfigObserverList)
            {
                stObserver(stConfigZookeeper);
            }

            m_iLastUpdateTime = (int)time(nullptr) + 1;
        }

        std::vector<reload_func_type>   m_stConfigObserverList;
        int                             m_iLastUpdateTime;
    };

public:
    struct zookeeper_info_t
    {
        zookeeper_info_t() 
        : m_strHost(), m_iTimeout(30000), m_strLogicConfigNodePath(),
          m_strNormalConfigNodePath(), m_stNoramlConfigFileList(),
          m_strMapConfigRootNodePath(), m_strTempServerNodeName() {  }
        
        std::string                 m_strHost;
        int                         m_iTimeout;
        std::string                 m_strLogicConfigNodePath;
        std::string                 m_strNormalConfigNodePath;
        std::vector<std::string>    m_stNoramlConfigFileList;
        std::string                 m_strMapConfigRootNodePath;
        std::string                 m_strTempServerNodeName;
    };

    typedef std::map<std::string, std::pair<std::string, CLogicZookeeper*>> config_map_type;

    struct config_complete_pool_t
    {
        config_complete_pool_t() 
        : m_stConfigFileMap(), m_uiHasCompletedConfigFileNum(0), m_iLastUpdateConfigTime(0), m_strEmptyContent() {}

        bool IsLoadComplete() 
        { 
            return ((m_stConfigFileMap.size() <= m_uiHasCompletedConfigFileNum) && (m_uiHasCompletedConfigFileNum != 0));
        }

        void AddConfigFile(const std::string& strFileName, CLogicZookeeper* pZookeeperConfig)
        {
            auto stIter = m_stConfigFileMap.find(strFileName);
            if(stIter == m_stConfigFileMap.end())
            {
				m_stConfigFileMap[strFileName] = CLogicZookeeper::config_map_type::mapped_type("", pZookeeperConfig);
            }
        }

        void RefreshConfigFile(const std::string& strFileName, const std::string& strFileContent)
        {
            if (!strFileContent.empty())
            {
                auto stIter = m_stConfigFileMap.find(strFileName);
                if(stIter != m_stConfigFileMap.end())
                {
                    if(stIter->second.first.empty()) m_uiHasCompletedConfigFileNum ++;

                    stIter->second.first = strFileContent;
                    m_iLastUpdateConfigTime = (int)time(nullptr);
                }
            }
        }

        void DeleteConfigFile(const std::string& strFileName)
        {
            auto stIter = m_stConfigFileMap.find(strFileName);
            if(stIter != m_stConfigFileMap.end())
            {
                if(!stIter->second.first.empty())
                {
                    m_uiHasCompletedConfigFileNum --;
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
            if(stIter == m_stConfigFileMap.end())
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
    CLogicZookeeper();

    ~CLogicZookeeper();

    void SetZookeeperInfo(const zookeeper_info_t& stZookeeperInfo);

    bool GetZookeeperInfoFromFile(const std::string& strFileName, zookeeper_info_t& stZookeeperInfo);

    void RegisterObserver(const reload_func_type &stObserver);
    
    bool InitZookeeper();
    
    int Interest(int& iFd, int& iEvent);

    int Process(int iEvent);

    void UpdateConfig();

    bool WaitAllConfigReady(int iTimeout);//timeout:ms

    void AddWatchFile(const std::string& strFileName);

    void SetErrorInfo(const std::string& strErrorInfo) const;

    const std::string& GetNormalConfig(const std::string& strFileName) const;

    const std::string& GetMapConfig(const std::string& strFileName) const;

private:
    void DestroyZookeeper();

    void ClearZooKeeperInfo();
    
    void SetZookeeperAbnormal();
    
    bool IsZookeeperNormal() const;

    int CreateTempServerNode();

    int WatchAndGetMapConfigList();

    int WatchAndGetConfigContent();
    
private:
    //zoo handle状态变更的回调
    static void ZooHandleEventWatcher(zhandle_t* pZooHandle, int iType, int iState, const char* szPath, void* pContext);
    //地图配置文件列表状态的回调
    static void MapConfigListEventWatcher(zhandle_t* pZooHandle, int iType, int iState, const char* szPath, void* pContext);
    //地图配置文件列表变更内容的回调
    static void MapConfigListEventComplete(int iRet, const struct String_vector* pStrings, const void* pContext);
    //配置文件内容状态的回调
    static void ConfigContentEventWatcher(zhandle_t* pZooHandle, int iType, int iState, const char* szPath, void* pContext);
    //配置文件内容变更的回调
    static void ConfigContentEventComplete(int iRet, const char* iValue, int iLength, const struct Stat* stStat, const void* pContext);
    //名字服务节点创建的回调
    static void CreateTempServerNodeComplete(int iRet, const char* pName, const void* pContext);
    //设置错误信息完成的回调
    static void setDataSilentComplete(int iRet, const struct Stat* stStat, const void* szContext) { }

private:
    zookeeper_info_t                        m_stZookeeperInfo;
    zhandle_t*                              m_pZooHandle;
    config_complete_pool_t                  m_stConfigCompletePool;
    enum ZOOKEEPER_LOGIC_STATUS      m_enuZookeeperHandleStatus;
    config_observer_t                       m_stConfigObserver;
};


