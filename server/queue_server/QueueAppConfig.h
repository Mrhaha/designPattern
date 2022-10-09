#pragma once

#include "common_msg_head_connector.h"
#include "QueueAppCommon.h"
#include <map>

class CQueueAppConfig
{
public:
    CQueueAppConfig(void);
	bool Load(const char* pszFile);

public:
	~CQueueAppConfig(void);

public:
 	//日志部份
	LogParam m_LogParam;
    int32_t m_iServerID;
    int32_t m_iMaxClientNum;            //客户端最大连接数
    int32_t m_iMaxClientSocketBuffSize; //客户端最大数据包大小
    char m_szQueueSvrSessionKey[SESSION_KEY_LENGTH];    //本服务器key
    char m_szConnSvrSessionKey[SESSION_KEY_LENGTH];     //Connect server key
    int32_t m_nSignatureValidPeriod;        //数字签名有效时长

    int32_t m_iCheckSocketInterval;		//检查网络连接的时间间隔(单位：s)
    int32_t m_iMaxSocketUnaliveTime;	//client socket连接最大不活动间隔(单位：s)
    int32_t m_iMaxWaitForFirstPackageTime;	//client socket连接上等待第一个包的最大时间间隔（也就是连接建立后,等待接收第一个包的间隔）(单位：s)

	char m_szZookeeperHost[max_path_length];
	char m_szZookeeperConfigPath[max_path_length];
};

class CQueueRouterManager
{
public:
    CQueueRouterManager();

	//添加新的服务器连接
	ConnectionType& AddServerConnection(int32_t iFd, EnumEntityTypes entity_type, int32_t entity_id, const std::string& ip, uint16_t port);

    // 添加新的客户端连接
    void AddClientConnection(const TSocketInfo& stInfo);

    void ClearServerConnection(int32_t iFd);
    void ClearClientSocket(int32_t iFd, EnumClientCloseReason code);

public:
	SocketType m_stListenSocket;							// 监听套接字
    std::map<int32_t, TSocketInfo> m_stClientConMap;        // 客户端连接
	std::map<int32_t, ConnectionType> m_astConnectionMap;	// 连接监听套接字的TCP连接
    std::map<int32_t, int32_t> m_stServerIDToFd;            // [server_id, fd]
};
