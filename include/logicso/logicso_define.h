//
// Created by hzl on 2019/1/31.
//

#pragma once

#include <unistd.h>
#include <cstdint>
#include <map>
#include "tc_mysql.h"
#include "common_base.h"

struct stLogicSoSenderUser
{
    int32_t m_iConnectionIndex{-1};
    int32_t m_iUserGold{0};
    int32_t m_iUserRmb{0};
    int32_t m_iUserPhyPower{0};
    int32_t m_iUserLastAutoAddTime{0};
};

struct stLogicSoStatInfo
{
    stLogicSoStatInfo()
            : m_strAccount("default_account"), m_strDeviceID("default_uuid"), m_strDeviceModel("default_model"),
              m_strDeviceOS("ios"), m_strDeviceRatio("640x960"), m_strDeviceNet("2G"),
              m_strDeviceCarrier("net"), m_strPlatformID("default_id"), m_strPlatformSign("default_sign")  {}

    std::string             m_strAccount;           //用户帐号
    std::string             m_strDeviceID;          //设备唯一标识
    std::string             m_strDeviceModel;       //设备型号
    std::string             m_strDeviceOS;          //设备操作系统
    std::string             m_strDeviceRatio;       //设备分辨率
    std::string             m_strDeviceNet;         //设备联网方式(2G 3G 4G WIFI)
    std::string             m_strDeviceCarrier;     //设备运营商
    std::string             m_strPlatformID;        //平台ID
    std::string             m_strPlatformSign;      //平台标示
};

struct stLogicSoClientInfo
{
    stLogicSoClientInfo()
            : m_iClientIP(0), m_iGroupID(0), m_usClientPort(0), m_usConnectionIndex(0), m_strClientIP()
            , m_iLoginTime(0), m_iLastOnlineTime(0), m_iResVersion(0), m_iChannelID(0) {}

    int                 m_iClientIP;//network order
    int                 m_iGroupID;
    unsigned short      m_usClientPort;
    unsigned short      m_usConnectionIndex;
    std::string         m_strClientIP;
    int                 m_iLoginTime;
    int					m_iLastOnlineTime;
    int					m_iResVersion;
    int                 m_iChannelID;

    stLogicSoStatInfo   m_stStatInfo;
};

struct stLogicSoAdminClient
{
    stLogicSoAdminClient()
    {
        m_iFd = -1;
        m_strClientIP = "";
        m_usClientPort = 0;
        m_uiLength = 0;
        m_uiOffset = 0;
    }

    void Clear()
    {
        if(m_iFd >= 0) ::close(m_iFd);
        m_strClientIP = "";
        m_usClientPort = 0;
        m_uiLength = 0;
        m_uiOffset = 0;
    }

    char* GetBuffer() { return (m_pBuffer + m_uiOffset); }

    int                 m_iFd;
    std::string         m_strClientIP;
    unsigned short      m_usClientPort;

    char                m_pBuffer[max_admin_recv_pkg_size];
    size_t              m_uiLength;
    size_t              m_uiOffset;
};


struct TLogicSoGroupConfig
{
    TLogicSoGroupConfig(): m_strName(), m_iShowID(0) {}

    std::string m_strName;
    int32_t m_iShowID;
};

struct TLogicSoServerConfig
{
    TLogicSoServerConfig() : m_iServerID(0)
        , m_strPlatform()
        , m_strHostname()
        , m_strHost()
        , m_iListenPort(0)
        , m_iAdminListenPort(0)
        , m_iLogLevel(-1)
        , m_iBinLogLevel(0)
        , m_iDebugTimeOffset(0)
        , m_iLoginPushover(0)
        , m_iOpenTime(0)
        , m_iMaintainBeginTime(0)
        , m_iMaintainEndTime(0)
        , m_iMergeTime(0)
        , m_iMergeCount(0)
        , m_stMergedGroupMap()
    {}

    int32_t				m_iServerID;
    std::string			m_strPlatform;
    std::string         m_strHostname;
    std::string			m_strHost;
    int32_t             m_iListenPort;
    int32_t             m_iAdminListenPort;
    int32_t             m_iLogLevel;
    int32_t				m_iBinLogLevel;
    int32_t             m_iDebugTimeOffset;
    int32_t             m_iLoginPushover;
    int32_t				m_iOpenTime;
    int32_t			    m_iMaintainBeginTime;
    int32_t				m_iMaintainEndTime;
    int32_t				m_iMergeTime;
    int32_t				m_iMergeCount;
    std::map<int32_t, TLogicSoGroupConfig> m_stMergedGroupMap;
};

struct TLogicSoRouterServerConfig
{
    TLogicSoRouterServerConfig() : m_strHost()
        , m_iListenPort(0)
        , m_iSendPort(0)
        , m_iLogicSendPort(0)
    {}

    std::string m_strHost;
    int32_t m_iListenPort;
    int32_t m_iSendPort;
    int32_t m_iLogicSendPort;
};

struct TLogicSoRouterConfig
{
    TLogicSoRouterConfig() : m_stConnector()
        , m_stCross(), m_stQueue()
    {}

    TLogicSoRouterServerConfig m_stConnector;
    TLogicSoRouterServerConfig m_stCross;
    TLogicSoRouterServerConfig m_stQueue;
};

struct TLogicSoMysqlConfig
{
    TLogicSoMysqlConfig() : m_stGameDB(), m_stStatDB()
    {}

    taf::TC_DBConf m_stGameDB;

    taf::TC_DBConf m_stStatDB;
};

