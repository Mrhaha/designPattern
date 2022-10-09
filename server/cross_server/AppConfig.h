#pragma once

#include "AppCommon.h"
#include <map>

class CAppConfig
{
public:
	CAppConfig(void);
	bool Load(const char* pszFile);

public:
	~CAppConfig(void);

public:
 	//日志部份
	LogParam m_LogParam;

    int32_t m_iServerID;        // 当前跨服服务器ID
	char m_szZookeeperHost[max_path_length];
	char m_szZookeeperConfigPath[max_path_length];
};

class CRouterManager
{
public:
	CRouterManager();

	//通过Server ID得到对应的连接
	ConnectionType* GetServerConnection(int32_t iServerID);
	//添加新的连接
	ConnectionType& AddOneConnection(EnumEntityTypes entity_type, int32_t entity_id, const std::string& ip, uint16_t port);
	//检查连接是否存活
	void CheckConnectionAlive();

public:
	int32_t m_iZookeeperSocketFd;
	SocketType m_stListenSocket;							//监听套接字
	std::map<int32_t, ConnectionType> m_astConnectionMap;	 //连接监听套接字的TCP连接		
};
