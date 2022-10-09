#pragma once

#include "common_base.h"
#include "common_logengine.h"
#include "common_singleton.h"

enum
{
	log_mask_everything		= 0xffffffff,
	open_min_socket_count   = 2,
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_) || (__DEBUG__)
	open_max_socket_count   = 5000,
#else
	open_max_socket_count   = 65536,
#endif
};

class CConnectorLog
{
public:
	CConnectorLog();
	~CConnectorLog();

	int32_t WriteServerLog(int iLogLevel,
		const char* szContent, ...);

	int32_t WriteServerLog(
		const char* szContent, ...);

	int32_t PrintServerBin(int iLogLevel,		//日志级别
		char *pcBuffer,
		int iLength);

	void SetLogLevel(int iLogLevel);

protected:

private:

	CLogEngine m_stLogEngine;

	//文件限制
	int32_t m_iMaxSystemLogFileSize;
	int32_t m_iMaxSystemLogFileCount;


private:
};

typedef Singleton<CConnectorLog> CONNECTORSERVER_LOGENGINE;

#define TRACESVR CONNECTORSERVER_LOGENGINE::Instance().WriteServerLog


#define TRACEBIN CONNECTORSERVER_LOGENGINE::Instance().PrintServerBin

#define SETTRACELEVEL CONNECTORSERVER_LOGENGINE::Instance().SetLogLevel
