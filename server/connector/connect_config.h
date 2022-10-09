#pragma once

#include "connector_define.h"
#include "common_ipv4.h"

/*
<?xml version="1.0" encoding="utf-8" ?>
	<connectorsvrd log_level="-1" check_socket_interval="10" sokcet_unalive_time="120" wait_firt_package_time="30" statistic_interval="90"
	socket_buffer="131072" core_key="810001" cs_channel_key="810002" sc_channel_key="810003" ac_channel_key="810004">
	<listen_port>844</listen_port>
	<listen_port>443</listen_port>
	<listen_port>80</listen_port>
	<router logic_ip="192.168.1.51" logic_listen_port="20001" logic_port="10010" local_port="10015" >
	<group_id>1</group_id>
	</router>
	</connectorsvrd>
*/

enum
{
	max_logic_group_id      = 0x3fff,   //logicsvrd最大服务器组ID
	max_listen_port_count   = 64,       //connsvrd 监听端口数
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_) || (__DEBUG__)
	max_router_server_count = 10,       //最大连接logicsvrd数量
#else
	max_router_server_count = 128,      //最大连接logicsvrd数量
#endif
};

typedef struct _stRouterServer
{
	char	m_szLogicIP[max_ipaddr_length];	//Logic Server IP 地址
	uint32_t m_uiLogicIP;		//Logic Server IP 地址
	uint16_t m_nLogicListenPort;	//Logic Server 监听端口
	uint16_t m_nLogicPort;	//Logic Server TCP 端口
	uint16_t m_nLocalPort;	//Connect Server TCP 端口
} stRouterServer;

class CConnectConfig
{
public:
	CConnectConfig();
	~CConnectConfig();

	int32_t Initialize();

	int32_t LoadConfig(const char* pszFileName);

	int32_t ReloadConfig(const char* pszFileName);

public:
	//获得服务器索引
	int32_t GetServerIndex(int32_t iGroupID);

	//获得客户端的连接
	int32_t GetClientIndex(uint32_t uIP, uint16_t nPort);

public:
	//关闭socket链接
	int32_t CloseServer(int32_t iGroupID);
	//合服
	int32_t MergeServer(int32_t iSrcGroupID, int32_t iDstGroupID);
	//加服
	int32_t ConnectServer(int32_t iGroupID, const char* pszIP, int32_t iListenPort, int32_t iLogicPort, int32_t iLocalPort);

public:
	//old
	int32_t m_iCheckSocketInterval;		//检查网络连接的时间间隔(单位：s)
	int32_t m_iMaxSocketUnaliveTime;	//socket连接最大不活动间隔(单位：s)
	int32_t m_iMaxWaitForFirstPackageTime;	//socket连接上等待第一个包的最大时间间隔（也就是连接建立后,等待接收第一个包的间隔）(单位：s)
	int32_t m_iLogStatisticInterval;		//统计间隔

	int32_t m_iSocketBufferSize;		//Socket Buffer 大小
	int32_t m_nSignatureValidPeriod;	//数字签名有效时长

	int32_t m_iIsAllowNoSignature;           //是否允许无签名的短连接

	int32_t m_nMaxfd;					//

	char m_szSessionKey[17];

	char m_szLocalIP[max_ipaddr_length]; //本机外网连接ip

	int32_t m_iLogLevel;
	//

	int32_t m_iOpenedPortCount;
	uint16_t m_unOpenedPort[max_open_port_count];	//监听的端口

	//new
	int32_t m_nRouterCount;
	stRouterServer	m_astRouterServer[max_router_server_count];

	int32_t m_aiServerIndex[max_logic_group_id];

private:
	time_t			m_tmConfigLastModifyTime;//配置文件最后一次修改的时间
	time_t			m_tmLastCheckConfigTime; //最后一次检查配置文件的时间
};
