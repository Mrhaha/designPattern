#pragma once
#include <stdint.h>

#include <mutex>
#include <thread>
#include <string>
#include <vector>
#include <set>
#include "tc_logger.h"
#include "tc_atomic.h"
#include "tc_singleton.h"
#include "blocking_queue.hpp"

class CCrossMysqlHandle
{
public:
    struct TWriterQueueData
    {
        int32_t m_iGroupID;
        int32_t m_iUin;
        std::string m_strCmdName;
        std::string m_strBillRation;
        std::vector<std::string> m_stSQL;
    };

public:
    CCrossMysqlHandle();

    ~CCrossMysqlHandle();

    bool Start();
    bool Stop();

    std::shared_ptr<taf::TC_Mysql> GetGameQueryConn();

    void ExecGameSQL(int iGroupID, int iUin, const std::string &strAPIName, const std::string &strColorRation, const std::vector<std::string> &stSQL);

private:
    void Run();

private:
    std::shared_ptr<taf::TC_Mysql> m_pSQLQueryConn;             // 数据库共享读连接

private:
    std::unique_ptr<taf::TC_Mysql> m_pSQLWriteConn;             // 数据库写连接
    std::unique_ptr<std::thread> m_pSQLThread;                  // 数据库写线程
    dobala::blocking_queue<TWriterQueueData> m_stSQLQueue;      // 写数据库队列
    std::unique_ptr<taf::TC_DayLogger> m_pSQLLogger;            // 数据库日志
};

//自己控制该单例的退出时机，如果默认在程序退出时析构会跟其他单例有顺序关系
#define CROSS_MYSQL_HANDLE taf::TC_Singleton<CCrossMysqlHandle, taf::CreateUsingNew, taf::NoDestroyLifetime>::getInstance()
