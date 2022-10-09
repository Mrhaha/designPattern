#include "common_logengine.h"
#include "common_datetime.h"
#include <cstdarg>



	stLogTitleEntry g_astTitleSet[] = 
	{
		{log_mask_none, ""},
		{log_mask_system_error, "error"},
		{log_mask_system_warning, "warning"},
		{log_mask_routine_call, "call"},
		{log_mask_system_config, "config"},
		{log_mask_system_performance, "performance"},
		{log_mask_detail, "detail"},
		{log_mask_package,"package"},

	};


bool CLogEngine::whether_do_log(int32_t mask)
{
	return((m_iLogMask & mask) != 0);
}

void CLogEngine::set_logmask(int32_t mask)
{
	m_iLogMask = mask;
}

int32_t CLogEngine::initialize(int32_t filesize, int32_t filenumber, int32_t logmask, char* logfile)
{
	m_iMaxLogFileSize = filesize;
	m_iMaxLogFileNumber = filenumber;
	m_iLogMask = logmask;

	if (NULL != logfile)
	{
		STRNCPY(m_szFileName, logfile, sizeof(m_szFileName));
	}

	return success;
}

int32_t CLogEngine::log(char* pszFileName, int32_t iMaxFileSize, int32_t iMaxFileNumber, const char* title, 
						const char* pszLogInfo, va_list vaList)
{
	const char* realTitle = "default";
	if (NULL != title)
	{
		realTitle = title;
	}

	char szCurrrentDateTime[max_datetime_string_length];
	char szTmpFileName[max_file_name_length];

	FILE* pfFile = NULL;

	snprintf(szTmpFileName, sizeof(szTmpFileName),"%s.log", pszFileName);
	szTmpFileName[sizeof(szTmpFileName)-1] ='\0';

	pfFile = fopen(szTmpFileName, "a+");
	if (NULL == pfFile)
	{
		return fail;
	}

	get_current_string_datetime(szCurrrentDateTime);

	fprintf(pfFile, "[[%s]][%s]", szCurrrentDateTime, realTitle);
	vfprintf(pfFile, pszLogInfo, vaList);
	fclose(pfFile);

	return shiftfile(pszFileName, iMaxFileSize, iMaxFileNumber);
}

int32_t CLogEngine::log(char* pszFileName, int32_t iMaxFileSize, int32_t iMaxFileNumber, const char* title,
						const char* pszLogInfo, ...)
{
	const char* realTitle = "default";
	if (NULL != title)
	{
		realTitle = title;
	}

	char szCurrrentDateTime[max_datetime_string_length];
	char szTmpFileName[max_file_name_length];

	FILE* pfFile = NULL;

	snprintf(szTmpFileName, sizeof(szTmpFileName),"%s.log", pszFileName);
	szTmpFileName[sizeof(szTmpFileName)-1] ='\0';

	pfFile = fopen(szTmpFileName, "a+");
	if (NULL == pfFile)
	{
		return fail;
	}

	get_current_string_datetime(szCurrrentDateTime);

	fprintf(pfFile, "[[%s]][%s]", szCurrrentDateTime, realTitle);

	va_list vaList;
	va_start(vaList, pszLogInfo);
	vfprintf(pfFile, pszLogInfo, vaList);
	va_end(vaList);

	fclose(pfFile);
	
	return shiftfile(pszFileName, iMaxFileSize, iMaxFileNumber);
}

int32_t CLogEngine::log(const char* title, const char* pszLogInfo, va_list vaList)
{
	const char* realTitle = "default";
	if (NULL != title)
	{
		realTitle = title;
	}

	char szCurrrentDateTime[max_datetime_string_length];

	FILE* pfFile = NULL;

	get_current_string_datetime(szCurrrentDateTime);
	pfFile = fopen(m_szFileName, "a+");
	if (NULL == pfFile)
	{
		return fail;
	}

	fprintf(pfFile, "[[%s]][%s]", szCurrrentDateTime, realTitle);
	vfprintf(pfFile, pszLogInfo, vaList);
	va_end(vaList);

	fclose(pfFile);

	return shiftfile(m_szFileName, m_iMaxLogFileSize, m_iMaxLogFileNumber);
}
// 
int32_t CLogEngine::log(const char* title, const char* pszLogInfo, ...)
{
	const char* realTitle = "default";
	if (NULL != title)
	{
		realTitle = title;
	}

	char szCurrrentDateTime[max_datetime_string_length];

	FILE* pfFile = NULL;

	get_current_string_datetime(szCurrrentDateTime);
	pfFile = fopen(m_szFileName, "a+");
	if (NULL == pfFile)
	{
		return fail;
	}

	fprintf(pfFile, "[[%s]][%s]", szCurrrentDateTime, realTitle);
	va_list vaList;
	va_start(vaList, pszLogInfo);
	vfprintf(pfFile, pszLogInfo, vaList);
	va_end(vaList);

	fclose(pfFile);

	return shiftfile(m_szFileName, m_iMaxLogFileSize, m_iMaxLogFileNumber);
}

int32_t CLogEngine::log( FILE* pstFile, char* pszFileName, int32_t iMaxFileSize, int32_t iMaxFileNumber, const char* title, const char* pszLogInfo, va_list vaList )
{
    if (NULL == pstFile)
    {
        return fail;
    }

    const char* realTitle = "default";
    if (NULL != title)
    {
        realTitle = title;
    }

    char szCurrrentDateTime[max_datetime_string_length];

    get_current_string_datetime(szCurrrentDateTime);

    fprintf(pstFile, "[[%s]][%s]", szCurrrentDateTime, realTitle);
    vfprintf(pstFile, pszLogInfo, vaList);
    fflush(pstFile);

    return shiftfile(pszFileName, iMaxFileSize, iMaxFileNumber);
}

int32_t CLogEngine::log( FILE* pstFile, char* pszFileName, int32_t iMaxFileSize, int32_t iMaxFileNumber, const char* title, const char* pszLogInfo, ... )
{
    if (NULL == pstFile)
    {
        return fail;
    }

    const char* realTitle = "default";
    if (NULL != title)
    {
        realTitle = title;
    }

    char szCurrrentDateTime[max_datetime_string_length];
    get_current_string_datetime(szCurrrentDateTime);

    fprintf(pstFile, "[[%s]][%s]", szCurrrentDateTime, realTitle);

    va_list vaList;
    va_start(vaList, pszLogInfo);
    vfprintf(pstFile, pszLogInfo, vaList);
    va_end(vaList);

    fflush(pstFile);

    return shiftfile(pszFileName, iMaxFileSize, iMaxFileNumber);
}

int32_t CLogEngine::dumpbin(char* pszFileName, const char* pszBuffer, int32_t iSize, const char* pTitle)
{
	if (NULL == pszBuffer || 0 >= iSize)
	{
		return fail;
	}

	char szCurrrentDateTime[max_datetime_string_length];
	char szTmpFileName[max_file_name_length];

	FILE* pfFile = NULL;

	snprintf(szTmpFileName, sizeof(szTmpFileName),"%s.log", pszFileName);
	szTmpFileName[sizeof(szTmpFileName)-1] ='\0';

	pfFile = fopen(szTmpFileName, "a+");
	if (NULL == pfFile)
	{
		return fail;
	}

	get_current_string_datetime(szCurrrentDateTime);
	if (iSize > max_dump_info_size)
	{
		iSize = max_dump_info_size;
	}

	if(NULL==pTitle)
	{
		fprintf(pfFile, "[[%s]] [buffer size:%4d]", szCurrrentDateTime, iSize);
	}
	else
	{
		fprintf(pfFile, "[[%s]] %s [buffer size:%4d]", szCurrrentDateTime, pTitle,iSize);
	}

	for(int i = 0; i < iSize; ++i)
	{
		if(!(i % max_dump_line_size))
		{
			fprintf(pfFile, "\n[%s] %04d>\t", szCurrrentDateTime, (i/max_dump_line_size + 1));
		}
		fprintf(pfFile, "%02X ", (unsigned char)pszBuffer[i]);
	}

	fprintf(pfFile, "\n");

	fclose(pfFile);


	return success;
}

int32_t CLogEngine::dumpbin(FILE* pstFile, char* pszFileName, const char* pszBuffer, int32_t iSize, const char* pTitle/*=NULL*/)
{
    if (NULL == pszBuffer || 0 >= iSize || NULL == pstFile)
    {
        return fail;
    }

    char szCurrrentDateTime[max_datetime_string_length];

    get_current_string_datetime(szCurrrentDateTime);
    if (iSize > max_dump_info_size)
    {
        iSize = max_dump_info_size;
    }

    if(NULL==pTitle)
    {
        fprintf(pstFile, "[[%s]] [buffer size:%4d]", szCurrrentDateTime, iSize);
    }
    else
    {
        fprintf(pstFile, "[[%s]] %s [buffer size:%4d]", szCurrrentDateTime, pTitle,iSize);
    }

    for(int i = 0; i < iSize; ++i)
    {
        if(!(i % max_dump_line_size))
        {
            fprintf(pstFile, "\n[%s] %04d>\t", szCurrrentDateTime, (i/max_dump_line_size + 1));
        }
        fprintf(pstFile, "%02X ", (unsigned char)pszBuffer[i]);
    }

    fprintf(pstFile, "\n");

    fflush(pstFile);

    return success;
}

int32_t CLogEngine::shiftfile(char* pszFileName, int32_t iMaxFileSize, int32_t iMaxFileNumber)
{
	struct stat stFileState;

	char szTmpFileName[max_file_name_length];
	char szAnotherFileName[max_file_name_length];
	int i;

	sprintf(szTmpFileName, "%s.log", pszFileName);

	if(stat(szTmpFileName, &stFileState) < 0)
	{
		return fail;
	}

	if(stFileState.st_size < iMaxFileSize)
	{
		return fail;
	}

	sprintf(szTmpFileName, "%s%d.log", pszFileName, (iMaxFileNumber - 1));
	if(access(szTmpFileName, F_OK) == 0)
	{
		if(remove(szTmpFileName) < 0)
		{
			return fail;
		}
	}

	for(i = (iMaxFileNumber-2); i >= 0; i--)
	{
		if(i == 0)
		{
			sprintf(szTmpFileName,"%s.log", pszFileName);
		}
		else
		{
			sprintf(szTmpFileName,"%s%d.log", pszFileName, i);
		}

		if(access(szTmpFileName, F_OK) == 0)
		{
			sprintf(szAnotherFileName, "%s%d.log", pszFileName, i + 1);
			if(rename(szTmpFileName, szAnotherFileName) < 0)
			{
				return fail;
			}
		}
	}

	return success;
}

char* CLogEngine::get_log_level_title(int32_t logLevel)
{
	if (logLevel <= log_mask_none)
	{
		return NULL;
	}

	for (size_t i = 0; i < sizeof(g_astTitleSet)/sizeof(g_astTitleSet[i]); ++i)
	{
		if (g_astTitleSet[i].m_iLogLevel == logLevel)
		{
			return g_astTitleSet[i].m_szTitle;
		}
	}

	return NULL;
}


