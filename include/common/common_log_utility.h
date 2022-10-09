#pragma once

#include "common_base.h"
#include "common_logengine.h"
#include "common_singleton.h"


enum enmLogMaskBit
{
	log_mask_initialize = int32_t(0x00000100),			//初始化信息
	log_mask_everything = 0xffffffff,
};

enum enmLogFileStrategy
{
	max_log_file_name_prefix_length = 64,			//日志文件名前缀的最大长度
	max_log_file_size = 0x40000000,					//单个日志文件最大为1G
	max_log_file_count = 50,						//日志按天分文件，一天内最多允许产生50个日志文件
};

class CServerLog
{
public:
	CServerLog();
	~CServerLog();

	bool InitLog(const char* pszFileNamePrefix, int32_t iLogMask = log_mask_everything);

	inline void SetLogMask(int32_t iLogMask)
	{
		m_stLogEngine.set_logmask(iLogMask);
	}

	inline void SetLogFilePrefix(const char* pszFileNamePrefix)
	{
		if (pszFileNamePrefix != NULL)
		{
			STRNCPY(m_szLogFileNamePrefix, pszFileNamePrefix, sizeof(m_szLogFileNamePrefix));
		}
	}

public:
	int32_t WriteServerLog(int32_t iLogMask,
		const char* szContent, ...) __attribute__((format(printf, 3, 4)));

	int32_t WriteServerLog(
		const char* szContent, ...) __attribute__((format(printf, 2, 3)));

	int32_t PrintServerBin(int32_t iLogMask,
		char* pcBuffer,
		int32_t iLength);

private:
	CLogEngine m_stLogEngine;

	//文件限制
	int32_t m_iMaxSystemLogFileSize;
	int32_t m_iMaxSystemLogFileCount;
	//文件名前缀
	char m_szLogFileNamePrefix[max_log_file_name_prefix_length];
};


#define TRACESVR Singleton<CServerLog>::Instance().WriteServerLog

#define TRACEBIN Singleton<CServerLog>::Instance().PrintServerBin

#define SETLOGMASK Singleton<CServerLog>::Instance().SetLogMask

#define INITLOG Singleton<CServerLog>::Instance().InitLog
