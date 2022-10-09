#pragma once

#include "common_platform.h"
#include "logicso_game_api.h"

extern CLogicSoGameApi* g_pstGameApi;

#define LOGIC_SERVICE_API_INS g_pstGameApi->GetLogicServiceApi()
#define LOGIC_LOG_TRACE_INS LOGIC_SERVICE_API_INS->GetLogTraceIns()
#define LOGIC_LOG_SLOW_INS LOGIC_SERVICE_API_INS->GetLogSlowIns()
#define LOGIC_STATISTICS_INS LOGIC_SERVICE_API_INS->GetStatisticsIns()
#define LOGIC_CONNECTOR_SENDER LOGIC_SERVICE_API_INS->GetConnectorSender()

#define LOGIC_LOG_TRACE_INFO	LOGIC_LOG_TRACE_INS->info() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define LOGIC_LOG_TRACE_DEBUG	LOGIC_LOG_TRACE_INS->debug() << "DEBUG|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define LOGIC_LOG_TRACE_WARN	LOGIC_LOG_TRACE_INS->warn() << "WARN|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define LOGIC_LOG_TRACE_ERROR	LOGIC_LOG_TRACE_INS->error() << "ERROR|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define LOGIC_LOG_TRACE_ANY	    LOGIC_LOG_TRACE_INS->any() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"

#define LOGIC_LOG_SLOW_INFO	    LOGIC_LOG_SLOW_INS->info() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define LOGIC_LOG_SLOW_DEBUG	LOGIC_LOG_SLOW_INS->debug() << "DEBUG|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define LOGIC_LOG_SLOW_WARN	    LOGIC_LOG_SLOW_INS->warn() << "WARN|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define LOGIC_LOG_SLOW_ERROR	LOGIC_LOG_SLOW_INS->error() << "ERROR|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define LOGIC_LOG_SLOW_ANY	    LOGIC_LOG_SLOW_INS->any() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
