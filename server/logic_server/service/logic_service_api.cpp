#include "logic_service_api.h"

#include "server_cmd_cross_define.h"
#include "server_cross_protocol.h"
#include "logic_service_logger.h"
#include "logic_service_statistics.h"
#include "logic_service_mysql_handle.h"
#include "logic_service_dirty_word_checker.h"
#include "logic_service_text_checker.h"
#include "logic_service_platform.h"
#include "../config/logic_config_base.h"

CLogicServiceApi::CLogicServiceApi()
	: connector_sender_(nullptr)
	, cross_connection_(nullptr)
    , queue_connection_(nullptr)
{
}

void CLogicServiceApi::SetConnectorSender(ILogicConnectorSender *connector_sender)
{
	connector_sender_ = connector_sender;
}


void CLogicServiceApi::SetCrossConnection(ILogicTCPConnection *cross_connection)
{
	cross_connection_ = cross_connection;
}

void CLogicServiceApi::SetQueueConnection(ILogicTCPConnection *queue_connection)
{
    queue_connection_ = queue_connection;
}

CLogicSoGameApi *CLogicServiceApi::GetLogicSoGameApi()
{
	return game_api_.get();
}

std::string_view CLogicServiceApi::GetServiceVersion()
{
    if (version_.empty())
    {
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_) || (__DEBUG__)
        version_ = "logic server debug version ";
#else
        version_ = "logic server release version ";
#endif
        version_.append(get_build_info());
    }

    return version_;
}

ILogicConnectorSender *CLogicServiceApi::GetConnectorSender()
{
	return connector_sender_;
}

ILogicTCPConnection *CLogicServiceApi::GetCrossConnection()
{
	return cross_connection_;
}

ILogicTCPConnection *CLogicServiceApi::GetQueueConnection()
{
    return queue_connection_;
}

taf::TC_DayLogger* CLogicServiceApi::GetLogTraceIns()
{
	return SERVICE_LOG_TRACE_INS::getInstance();
}

taf::TC_DayLogger* CLogicServiceApi::GetLogSlowIns()
{
	return SERVICE_LOG_SLOW_INS::getInstance();
}

ILogicStatistics* CLogicServiceApi::GetStatisticsIns()
{
	return SERVICE_STATISTICS;
}

void CLogicServiceApi::LoadConfigSuccess()
{
	SERVICE_DIRTY_WORD_FILTER_INS->BuildACFSM(GetLogicSoGameApi()->GetUtf8DirtyWordNameList(),EnumFilterWordsType::EnumFilterWordsType_Name_Type);
    SERVICE_DIRTY_WORD_FILTER_INS->BuildACFSM(GetLogicSoGameApi()->GetUtf8DirtyWordChatList(),EnumFilterWordsType::EnumFilterWordsType_Chat_Type);

	SERVICE_LOG_TRACE_INS::getInstance()->setLogLevel(GetLogicSoGameApi()->GetGameServerConfig().m_iLogLevel);
}

void CLogicServiceApi::RequestServiceAlert(const std::string &id, const map<string, string> &params)
{
    CSRequestServiceAlert stReqBody;
    stReqBody.m_strID = id;
    stReqBody.m_stParams = params;
    stReqBody.m_stParams["hostname"] = GetLogicSoGameApi()->GetGameServerConfig().m_strHostname;
    stReqBody.m_stParams["server_id"] = std::to_string(GetLogicSoGameApi()->GetGameServerConfig().m_iServerID);
    stReqBody.m_stParams["server_ip"] = GetLogicSoGameApi()->GetGameServerConfig().m_strHost;
    stReqBody.m_stParams["platform"] = GetLogicSoGameApi()->GetGameServerConfig().m_strPlatform;

    CMsgHeadCross stMsgHead;
    stMsgHead.m_byCmdType = enmCmdType_request;
    stMsgHead.m_usCmd = MSG_CROSS_SERVICE_ALERT;
    stMsgHead.m_iUin = 0;
    stMsgHead.m_nGroupID = 0;
    GetCrossConnection()->SendToServer(stMsgHead, stReqBody);
}

void CLogicServiceApi::ExecGameSQL(int iGroupID, int iUin, const std::string &strAPIName,
                                     const std::string &strColorRation, const std::vector<std::string> &stSQL)
{
    SERVICE_MYSQL_HANDLE->ExecGameSQL(iGroupID, iUin, strAPIName, strColorRation, stSQL);
}

void CLogicServiceApi::ExecStatSQL(int iGroupID, int iUin, const std::string &strAPIName,
                                     const std::string &strColorRation, const std::string & strSQL)
{
    SERVICE_MYSQL_HANDLE->ExecStatSQL(iGroupID, iUin, strAPIName, strColorRation,  strSQL );
}

std::string CLogicServiceApi::FilterTextEmoji(const std::string& strContent)
{
	return CLogicTextChecker::FilterEmoji(strContent);
}

std::string CLogicServiceApi::SkipText4ByteChars(const std::string& strContent)
{
	return CLogicTextChecker::Skip4ByteChars(strContent);
}

bool CLogicServiceApi::CheckNickHasDirtyWord(const std::string& strNick)
{
	return CLogicTextChecker::CheckNickHasDirtyWord(strNick);
}

bool CLogicServiceApi::FilterTalkContent(std::string& strContent)
{
	return CLogicTextChecker::FilterTalkContent(strContent);
}

int CLogicServiceApi::CheckNickCorrect(const std::string& strNick, int& iDisplayCharWidth)
{
	return CLogicTextChecker::CheckNickCorrect(strNick, iDisplayCharWidth);
}


