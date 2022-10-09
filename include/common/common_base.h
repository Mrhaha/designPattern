#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/timeb.h>
#include <ctime>
#include <cerrno>

#include <csignal>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/select.h>
#include <cassert>
#include <msgpack.hpp>

#define CHECK_ROUTER_INTERVAL	30		//检查router连接间隔
#define MAX_UNACTIVE_INTERVAL	60		//router间可最大不进行数据交互的时间间隔

enum enmFuncRtn
{
	success = 0,
	fail = 1,
};

enum enumQueueMode
{
	queue_buffer_is_not_enough = 2,   /**< 队列空间不够*/
	queue_is_empty = 3,   /**< 队列为空*/
};

enum
{
	mysql_success = 0,
	mysql_default_port = 3306,
	mysql_string_length = 128,
	mysql_dbname_length = 256,
};

//some const variables
enum
{
	socket_buffer_size = 0x200000,	//该定义为server之间的socket buffer长度，注意必要要保证大于server间通讯的最大包长
	max_package_size = 0x40000,		//256k 通讯协议包的最大长度
	max_datetime_string_length = 32,  /**<日期时间字符串的最大长度 <*/
	max_file_name_length = 255,		  /**<文件名最大长度<*/
	max_path_length = 255,			  /**<路径的最大长度*/
	max_ipaddr_length = 30,		//IP地址的长度
	max_stat_sql_length = 1024 * 10,
	max_admin_client_pkg_size = max_package_size,
    max_admin_recv_pkg_size = 4096,
};

enum enmCmdType
{
	enmCmdType_request = 1,
	enmCmdType_response = 2,
	enmCmdType_notify = 3,
	enmCmdType_cross = 4,
	enmCmdType_heart = 5,
};

//connection status
enum enmTCPConnectionStatus
{
	enmTCPConnectionStatus_closed = 0,
	enmTCPConnectionStatus_opened,
	enmTCPConnectionStatus_connected,
	enmTCPConnectionStatus_listening,
	enmTCPConnectionStatus_error
};

enum enmConnErrorCode
{
	enmConnErrorCode_success = 0,
	enmConnErrorCode_invalid_socket = -1,
	enmConnErrorCode_recv_notenouth_buffer = -2,
	enmConnErrorCode_peer_closed = -3,
	enmConnErrorCode_send_notenouth_buffer = -4,
	enmConnErrorCode_invalid_param = -5,
	enmConnErrorCode_unknow = -6,
};

enum EnumMsgHeadPackERROR
{
    Enum_MHPE_Success        	= 1,
	// get_one_code error
    Enum_MHPE_GetOneCode_NoData             = 0,
	Enum_MHPE_GetOneCode_Invalid       		= -1,
	Enum_MHPE_GetOneCode_CodeSizeNotEnough	= -2,

	// MsgHead common Encode Decode error
	Enum_MHPE_BUFFER_TOO_SMALL  = -10,
	Enum_MHPE_DATA_ERROR        = -11,
};

//socket type
enum enmSocketTypes
{
	enmSocketTypes_listen = 0,
	enmSocketTypes_comm = 1,
};

enum EnumEntityTypes
{
	Enum_EET_EntityTypes_Null = 0,
	Enum_EET_EntityTypes_Logic = 1,
	Enum_EET_EntityTypes_Cross = 2,
	Enum_EET_EntityTypes_Connector = 3,
    Enum_EET_EntityTypes_Queue = 4,
};

enum enmRunFlag
{
	run_flag_reload = 0x00000001,
	run_flag_exit 	= 0x00000002,
};
