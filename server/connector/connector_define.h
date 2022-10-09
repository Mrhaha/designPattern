#pragma once

#include <stdint.h>

#include "common_base.h"
#include "gy_session_creator.h"

#define GY_MAX_CLIENT_BUFFER_SIZE 10240

enum
{
	http_head_length = 49,				//http头的固定长度
										//http头: POST /51.game HTTP/1.1\r\nContent-Length:65535\r\n\r\n

};

struct stConnectorConfig
{
	int32_t m_iCoreKey;

	int32_t m_iLogLevel;
	int32_t m_nMaxfd;
};

enum enmSocketType
{
	enmSocketType_invalid = 0,
	enmSocketType_listen =	1,			//监听socket
	enmSocketType_common =  2,			//普通socket
	enmSocketType_server =  3,			//连接服务器的普通socket
	enmSocketType_client =  4,			//来至服务器的普通socket
};


enum
{
	max_open_port_count = 8,			//服务器打开的端口最大数目

#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_) || (__DEBUG__)
	max_socket_count = 0x00f0,          //支持的最大连接数
#else
	max_socket_count = 0x010000,          //支持的最大连接数
#endif

	auxiliary_handler_count = 2,		//

	max_s2c_package_size = 65535,		//service server发送给前端最大的包大小

	max_package_count_per_sending = 512,//

	max_file_path = 255,				//

	socket_fd_invalid = -1,				//
};

#define CONNECT_CONFIG				"./config/connectorsvrd.xml"

//
struct stSocketInfo 
{
	int32_t m_socket_fd;
	char m_client_address[max_ipaddr_length];
	enmSocketType m_socket_type;
	int32_t m_uin;											//与应用层领域相关联的ID，用于唯一标识
	int32_t m_iGroupID;
	int32_t m_iPlatformID;

	uint32_t m_peer_address;								//对端的ip
	uint16_t m_peer_port;									//对端的port
	uint16_t m_local_port;									//从哪个监听端口连接上的
	int32_t m_recv_bytes;									//收到的数据大小

	bool m_is_sended_data_to_peer;							//标识是否有向对端发送过数据
	bool m_is_recved_data_from_peer;						//标识是否收到过对端的数据
	time_t m_create_time;									//socket创建时间
	time_t m_lastrecv_timestamp;							//最近一次从对端接收到数据的timestamp
	char m_package_buffer[GY_MAX_CLIENT_BUFFER_SIZE];		//组包用的缓冲区

	CGYSessionCreator::gy_session_t m_stPlaySession;
};
