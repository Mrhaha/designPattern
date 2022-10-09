#pragma once

#include <dirent.h>
#include "common_logengine.h"
#include "common_datetime.h"
#include "common_log_utility.h"

class CAppLog
{
public:
	bool CreateDir(const char* pszDir)
	{
		DIR* pstBillDir = opendir(pszDir);
		if (nullptr == pstBillDir)
		{
			if( mkdir(pszDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) )
			{
				return false;
			}
		}
		else
		{
			closedir(pstBillDir);
			pstBillDir = nullptr;
		}
		return true;
	};

    bool InitLog( int32_t logmask, char* pazFileName)
    {
    	if(pazFileName != nullptr)
		{
			STRNCPY(m_szLogFileNamePrefix,pazFileName,sizeof(m_szLogFileNamePrefix));
		}

		m_nLogFileSize = max_log_file_size;
		m_nLogFileCount = max_log_file_count;
		m_LogEngine.set_logmask(logmask);
    	return true;
    };


	void WriteLog(int32_t nLogMask,const char* pszLogInfo,...)
	{
		char szFileName[max_file_name_length] = {0};
		char szCurDate[max_datetime_string_length] = {0};

		if (!m_LogEngine.whether_do_log(nLogMask))
		{
			return;
		}

		get_current_string_date((char *)szCurDate);
		snprintf(szFileName,(max_file_name_length - 1), "%s_%s", m_szLogFileNamePrefix, szCurDate);

		va_list vaList;
		va_start(vaList, pszLogInfo);


		char* pszTtile = m_LogEngine.get_log_level_title(nLogMask);
		m_LogEngine.log(szFileName, m_nLogFileSize, m_nLogFileCount, pszTtile ,pszLogInfo, vaList);
		//m_LogEngine.log(szFileName, m_nLogFileSize, m_nLogFileCount, pszLogInfo, vaList);

		va_end(vaList);

		return; 
	};

	void WriteLog(int32_t nLogMask,const char* pszLogInfo,va_list vaList)
	{
		char szFileName[max_file_name_length] = {0};
		char szCurDate[max_datetime_string_length] = {0};

		if (!m_LogEngine.whether_do_log(nLogMask))
		{
			return;
		}

		get_current_string_date((char *)szCurDate);
		snprintf(szFileName,(max_file_name_length - 1), "%s_%s", m_szLogFileNamePrefix, szCurDate);
		char* pszTitle = m_LogEngine.get_log_level_title(nLogMask);
		m_LogEngine.log(szFileName, m_nLogFileSize, m_nLogFileCount, pszTitle ,pszLogInfo, vaList);
		return; 
	};

	void WriteLogB(char* pszFileName,int32_t nLogMask,const char* pszBuf,int32_t nBufLen,const char* pszTitle=nullptr)
	{
		char szFileName[max_file_name_length] = {0};
		char szCurDate[max_datetime_string_length] = {0};

		if (!m_LogEngine.whether_do_log(nLogMask))
		{
			return;
		}

		get_current_string_date((char *)szCurDate);
		snprintf(szFileName,(max_file_name_length - 1),"%s_%s", pszFileName, szCurDate);

		m_LogEngine.dumpbin(szFileName, pszBuf, nBufLen, pszTitle);

		return; 
	};

	void WriteLogBin(int32_t nLogMask,const char* pszBuf,int32_t nBufLen,const char* pszTitle=nullptr)
	{
		char szFileName[max_file_name_length] = {0};
		char szCurDate[max_datetime_string_length] = {0};

		if (!m_LogEngine.whether_do_log(nLogMask))
		{
			return;
		}

		get_current_string_date((char *)szCurDate);
		snprintf(szFileName,(max_file_name_length - 1),"%s-bin_%s", m_szLogFileNamePrefix, szCurDate);

		m_LogEngine.dumpbin(szFileName, pszBuf, nBufLen, pszTitle);

		return; 
	};

private:
   CLogEngine m_LogEngine;
   int32_t m_nLogFileSize;
   int32_t m_nLogFileCount;
   char    m_szLogFileNamePrefix[max_log_file_name_prefix_length];
};


