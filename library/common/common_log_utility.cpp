#include "common_log_utility.h"
#include "common_datetime.h"
#include <cstdarg>

	CServerLog::CServerLog()
	{
		m_iMaxSystemLogFileSize = max_log_file_size;
		m_iMaxSystemLogFileCount = max_log_file_count;

		SetLogMask(log_mask_everything);
		SetLogFilePrefix("server");
	}

	CServerLog::~CServerLog()
	{
		//do nothing
	}

	bool CServerLog::InitLog(const char* pszFileNamePrefix, int32_t iLogMask)
	{
		m_iMaxSystemLogFileSize = max_log_file_size;
		m_iMaxSystemLogFileCount = max_log_file_count;

		SetLogMask(iLogMask);
		SetLogFilePrefix(pszFileNamePrefix);
		return true;
	};

	int32_t CServerLog::WriteServerLog(int32_t iLogMask, const char* szContent, ...)
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];

		if(!m_stLogEngine.whether_do_log(iLogMask))
		{
			return fail;
		}

		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/%s_%s", m_szLogFileNamePrefix, szCurDate);

		char* title = m_stLogEngine.get_log_level_title(iLogMask);

		va_list ap;
		va_start(ap, szContent);
		iRet = m_stLogEngine.log(szFileName, m_iMaxSystemLogFileSize, m_iMaxSystemLogFileCount, title, szContent,ap);
		va_end(ap);

		return iRet;
	}	

	int32_t CServerLog::WriteServerLog(const char* szContent, ...)
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];
		
		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/%s_%s", m_szLogFileNamePrefix, szCurDate);

		const char* title = "detail";

		va_list ap;
		va_start(ap, szContent);
		iRet = m_stLogEngine.log(szFileName, m_iMaxSystemLogFileSize, m_iMaxSystemLogFileCount, title, szContent,ap);
		va_end(ap);

		return iRet;
	}

	int32_t CServerLog::PrintServerBin(int32_t iLogMask, char *pcBuffer, int32_t iLength)
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];

		if(!m_stLogEngine.whether_do_log(iLogMask))
		{
			return fail;
		}

		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/%s_%s", m_szLogFileNamePrefix, szCurDate);

		iRet = m_stLogEngine.dumpbin(szFileName,pcBuffer,iLength);

		return iRet;
	}


