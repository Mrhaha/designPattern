#pragma once

#include "common_tcp_connection.h"
#include "common_object.h"
#include "common_base.h"
#include "QueueAppLog.h"
#include "common_platform.h"
#include "gy_session_creator.h"

#define Q_MAX_CLIENT_BUFFER_SIZE 1024

//基本
#define LOG_PATH "./log"
#define PATH_BILL "./bill"
#define APPLOG_PREFIX_FILENAME "queue" //主程序日志文件名前缀
#define APPLOG_GLOBAL_PREFIX_FILENAME "queuesvr" //主线程日志文件名前缀
#define APPCONFIG_FILENAME	"./config/queueserver.config"	//系统的配置文件
#define SESSION_KEY_LENGTH 17    //session_key长度
#define MAX_FILEPATHLEN 255 //最大的文件名
#define MIN_QUEUE_CLIENT_NUM 100    //客户端最小支持连接数
#define SERVER_RESERVED_CONNECTION_NUM 500    // zookeeper, listen, logic_svr 连接预留数量

//Processor相关常量
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_) || defined(__DEBUG__)
	#define VERSION_INFO "Queue Server Debug build"
    #define MAX_QUEUE_CLIENT_NUM                                            0x00f0              //客户端最大支持连接数
    #define MSG_PROCESSOR_NUM                                               0x0002              //消息处理线程数量
#else
	#define VERSION_INFO "Queue Server Release build"
    #define MAX_QUEUE_CLIENT_NUM                                            0x2710              //最大 1W
    #define MSG_PROCESSOR_NUM                                               0x0005              //最大 5个
#endif

//日志参数
typedef struct _tagLogParam
{
	char m_szLogPath[MAX_FILEPATHLEN];//系统日志目录
	int32_t m_nLogMask;//掩码
}LogParam,*PLogParam;


using SocketType = CTCPSocket<CSocketEmptyPolicy>;
using ConnectionType = CTCPConnectionHandle<CSocketEmptyPolicy>;

union TUserRoleID
{
    TUserRoleID() : TUserRoleID(0) { }

    TUserRoleID(int32_t iUin, int32_t iGroupID) : TUserRoleID()
    {
        m_stRoleIDDetail.m_iUin = iUin;
        m_stRoleIDDetail.m_iGroupID = iGroupID;
    }

    explicit TUserRoleID(uint64_t ulKey) : m_ulRoleID(ulKey) {}

    struct
    {
        int32_t m_iUin;
        int32_t m_iGroupID;
    }m_stRoleIDDetail;

    uint64_t    m_ulRoleID;
};

struct TSocketInfo
{
    int32_t m_socket_fd = -1;
    char m_client_address[max_ipaddr_length];
    int32_t m_uin = 0;											//与应用层领域相关联的ID，用于唯一标识
    int32_t m_iGroupID = 0;

    uint32_t m_peer_address = 0;								//对端的ip
    uint16_t m_peer_port = 0;									//对端的port
    int32_t m_recv_bytes = 0;									//收到的数据大小

    bool m_is_sended_data_to_peer = false;							//标识是否有向对端发送过数据
    bool m_is_recved_data_from_peer = false;						//标识是否收到过对端的数据
    time_t m_create_time = 0;									//socket创建时间
    time_t m_lastrecv_timestamp = 0;							//最近一次从对端接收到数据的timestamp
    char m_package_buffer[Q_MAX_CLIENT_BUFFER_SIZE];		//组包用的缓冲区
};

enum EnumQueueMsgSrcType
{
    EQMST_Logic = 1,            // Logic服务器消息
    EQMST_Client = 2,           // Client消息
};

struct TQueueMsgHeadInfo
{
    int32_t     m_iMsgSrcType = 0;
    int32_t     m_iServerID = 0;
    int32_t     m_iFd = -1;
    int32_t     m_iUin = 0;
    int32_t     m_iGroupID = 0;
    uint16_t    m_usCmd = 0;
};

struct TQueueMsgData
{
    TQueueMsgHeadInfo   m_stHeadInfo;
    std::string         m_strBody;              // 消息体
};
