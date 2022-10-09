#pragma once

#include "common_tcp_connection.h"
#include "common_object.h"
#include "common_base.h"

#include "AppLog.h"
#include "common_platform.h"

//基本
#define LOG_PATH "./log"
#define PATH_BILL "./bill"
#define APPLOG_PREFIX_FILENAME "cross" //主程序日志文件名前缀
#define APPLOG_GLOBAL_PREFIX_FILENAME "crosssvr" //主线程日志文件名前缀
#define APPCONFIG_FILENAME	"./config/crossserver.config"	//系统的配置文件
#define MAX_FILEPATHLEN 255 //最大的文件名


//Processor相关常量
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_) || defined(__DEBUG__)
	#define VERSION_INFO "Cross Server Debug build"
	#define MAX_CONNECTTION_COUNT											16					//最大连接数
#else
	#define VERSION_INFO "Cross Server Release build"
	#define MAX_CONNECTTION_COUNT											900					//最大连接数
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

struct TCrossMsgHeadInfo
{
    int32_t     m_iServerID = 0;
    int32_t     m_iUin = 0;
    int32_t     m_iGroupID = 0;
    uint16_t    m_usCmd = 0;
};

struct TCrossMsgData
{
    TCrossMsgHeadInfo   m_stHeadInfo;
    std::string         m_strBody;              // 消息体
};
