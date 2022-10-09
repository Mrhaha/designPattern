#pragma once

#include "zookeeper.h"

#ifdef __cplusplus
extern "C" {
#endif

extern ZOOAPI ZooLogLevel logLevel;
#define LOGSTREAM getLogStream()

#define LOG_ERROR(x) if(logLevel>=ZOO_LOG_LEVEL_ERROR) \
    log_message(ZOO_LOG_LEVEL_ERROR,__LINE__,__func__,format_log_message x)
#define LOG_WARN(x) if(logLevel>=ZOO_LOG_LEVEL_WARN) \
    log_message(ZOO_LOG_LEVEL_WARN,__LINE__,__func__,format_log_message x)
#define LOG_INFO(x) if(logLevel>=ZOO_LOG_LEVEL_INFO) \
    log_message(ZOO_LOG_LEVEL_INFO,__LINE__,__func__,format_log_message x)
#define LOG_DEBUG(x) if(logLevel==ZOO_LOG_LEVEL_DEBUG) \
    log_message(ZOO_LOG_LEVEL_DEBUG,__LINE__,__func__,format_log_message x)

ZOOAPI void log_message(ZooLogLevel curLevel, int line,const char* funcName,
    const char* message);

ZOOAPI const char* format_log_message(const char* format,...);

FILE* getLogStream();

#ifdef __cplusplus
}
#endif
