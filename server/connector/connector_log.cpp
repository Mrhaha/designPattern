#include "connector_log.h"
#include "common_datetime.h"

CConnectorLog::CConnectorLog()
{
	//文件限制
	m_iMaxSystemLogFileSize = 0x2000000;
	m_iMaxSystemLogFileCount = 5;
}

CConnectorLog::~CConnectorLog()
{

}

int32_t CConnectorLog::WriteServerLog( int iLogLevel, const char* szContent, ... )
{
	int iRet = success;
	char szFileName[256];
	char szCurDate[32];
	//char szErrInfo[256];

	if(!m_stLogEngine.whether_do_log(iLogLevel))
	{
		return fail;
	}

	get_current_string_date((char *)szCurDate);
	sprintf(szFileName, "./log/connectorsvrd_%s", szCurDate);

	char* title = m_stLogEngine.get_log_level_title(iLogLevel);

	va_list ap;

	va_start(ap, szContent);
	iRet = m_stLogEngine.log(szFileName,
		m_iMaxSystemLogFileSize, m_iMaxSystemLogFileCount, title, szContent,ap);
	va_end(ap);

	return iRet;
}

int32_t CConnectorLog::WriteServerLog( const char* szContent, ... )
{
	int iRet = success;
	char szFileName[256];
	char szCurDate[32];
	//char szErrInfo[256];


	get_current_string_date((char *)szCurDate);
	sprintf(szFileName, "./log/connectorsvrd_%s", szCurDate);

	const char* title = "detail";

	va_list ap;

	va_start(ap, szContent);
	iRet = m_stLogEngine.log(szFileName,
		m_iMaxSystemLogFileSize, m_iMaxSystemLogFileCount, title, szContent,ap);
	va_end(ap);

	return iRet;
}

int32_t CConnectorLog::PrintServerBin( int iLogLevel, /*日志级别 */ char *pcBuffer, int iLength )
{
	int iRet = success;
	char szFileName[256];
	char szCurDate[32];
	//char szErrInfo[256];


	if(!m_stLogEngine.whether_do_log(iLogLevel))
	{
		return fail;
	}

	get_current_string_date((char *)szCurDate);
	sprintf(szFileName, "./log/connectorsvrd_%s", szCurDate);


	iRet = m_stLogEngine.dumpbin(szFileName,pcBuffer,iLength);

	return iRet;
}

void CConnectorLog::SetLogLevel( int iLogLevel )
{
	return m_stLogEngine.set_logmask(iLogLevel);
}
