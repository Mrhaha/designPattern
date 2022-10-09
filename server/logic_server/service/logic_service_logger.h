/* 
 * File:   rk_logic_logger.h
 * Author: plantang
 *
 * Created on 2014年10月17日, 下午4:51
 */

#pragma once

#include <string>

#include "tc_logger.h"
#include "tc_singleton.h"
#include "common_platform.h"

template<typename T>
class CreateRollLogger
{
public:
    static taf::TC_RollLogger* create()
    {
        std::string strLogName = m_strLogDir + "/" + m_strLogPrefix;
        taf::TC_RollLogger* pLogger = new taf::TC_RollLogger();
        pLogger->init(strLogName, 1024*1024*500, 2);
        if((m_iLogLevel < taf::TC_RollLogger::NONE_LOG) || (m_iLogLevel > taf::TC_RollLogger::INFO_LOG))
        {
            m_iLogLevel = taf::TC_RollLogger::INFO_LOG;
        }

        pLogger->setLogLevel(m_iLogLevel);

        return (pLogger);
    }

    static void destroy(taf::TC_RollLogger* t)
    {
        delete t;
    }

    static int              m_iLogLevel;
    static std::string      m_strLogDir;
    static std::string      m_strLogPrefix;
};

template<typename T> int CreateRollLogger<T>::m_iLogLevel = taf::TC_RollLogger::INFO_LOG;
template<typename T> std::string CreateRollLogger<T>::m_strLogDir = "./log/internal";
template<typename T> std::string CreateRollLogger<T>::m_strLogPrefix = "logic_internal";

template<typename T>
class CreateTraceLogger
{
public:
    static taf::TC_DayLogger* create()
    {
        std::string strLogName = m_strLogDir + "/" + m_strLogPrefix;
        taf::TC_DayLogger* pLogger = new taf::TC_DayLogger();
        pLogger->init(strLogName);
        if((m_iLogLevel < taf::TC_DayLogger::NONE_LOG) || (m_iLogLevel > taf::TC_DayLogger::INFO_LOG))
        {
            m_iLogLevel = taf::TC_DayLogger::INFO_LOG;
        }

        pLogger->setLogLevel(m_iLogLevel);

        return (pLogger);
    }

    static void destroy(taf::TC_DayLogger* t)
    {
        delete t;
    }

    static int              m_iLogLevel;
    static std::string      m_strLogDir;
    static std::string      m_strLogPrefix;
};

template<typename T> int CreateTraceLogger<T>::m_iLogLevel = taf::TC_DayLogger::INFO_LOG;
template<typename T> std::string CreateTraceLogger<T>::m_strLogDir = "./log/trace";
template<typename T> std::string CreateTraceLogger<T>::m_strLogPrefix = "logic_trace";

template<typename T>
class CreateSlowLogger
{
public:
    static taf::TC_DayLogger* create()
    {
        std::string strLogName = m_strLogDir + "/" + m_strLogPrefix;
        taf::TC_DayLogger* pLogger = new taf::TC_DayLogger();
        pLogger->init(strLogName);
        if((m_iLogLevel < taf::TC_DayLogger::NONE_LOG) || (m_iLogLevel > taf::TC_DayLogger::INFO_LOG))
        {
            m_iLogLevel = taf::TC_DayLogger::INFO_LOG;
        }

        pLogger->setLogLevel(m_iLogLevel);

        return (pLogger);
    }

    static void destroy(taf::TC_DayLogger* t)
    {
        delete t;
    }

    static int              m_iLogLevel;
    static std::string      m_strLogDir;
    static std::string      m_strLogPrefix;
};

template<typename T> int CreateSlowLogger<T>::m_iLogLevel = taf::TC_DayLogger::INFO_LOG;
template<typename T> std::string CreateSlowLogger<T>::m_strLogDir = "./log/slow";
template<typename T> std::string CreateSlowLogger<T>::m_strLogPrefix = "logic_slow";

typedef taf::TC_Singleton<taf::TC_RollLogger, CreateRollLogger> SERVICE_LOG_ROLL_INS;
typedef taf::TC_Singleton<taf::TC_DayLogger, CreateTraceLogger> SERVICE_LOG_TRACE_INS;
typedef taf::TC_Singleton<taf::TC_DayLogger, CreateSlowLogger> SERVICE_LOG_SLOW_INS;

#define SERVICE_LOG_ROLL_INFO      SERVICE_LOG_ROLL_INS::getInstance()->info() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_ROLL_DEBUG     SERVICE_LOG_ROLL_INS::getInstance()->debug() << "DEBUG|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_ROLL_WARN      SERVICE_LOG_ROLL_INS::getInstance()->warn() << "WARN|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_ROLL_ERROR     SERVICE_LOG_ROLL_INS::getInstance()->error() << "ERROR|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_ROLL_ANY       SERVICE_LOG_ROLL_INS::getInstance()->any() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"

#define SERVICE_LOG_TRACE_INFO     SERVICE_LOG_TRACE_INS::getInstance()->info() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_TRACE_DEBUG    SERVICE_LOG_TRACE_INS::getInstance()->debug() << "DEBUG|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_TRACE_WARN     SERVICE_LOG_TRACE_INS::getInstance()->warn() << "WARN|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_TRACE_ERROR    SERVICE_LOG_TRACE_INS::getInstance()->error() << "ERROR|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_TRACE_ANY      SERVICE_LOG_TRACE_INS::getInstance()->any() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"

#define SERVICE_LOG_SLOW_INFO      SERVICE_LOG_SLOW_INS::getInstance()->info() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_SLOW_DEBUG     SERVICE_LOG_SLOW_INS::getInstance()->debug() << "DEBUG|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_SLOW_WARN      SERVICE_LOG_SLOW_INS::getInstance()->warn() << "WARN|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_SLOW_ERROR     SERVICE_LOG_SLOW_INS::getInstance()->error() << "ERROR|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
#define SERVICE_LOG_SLOW_ANY       SERVICE_LOG_SLOW_INS::getInstance()->any() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"


