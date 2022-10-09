#pragma once
#include <stdint.h>

#include <mutex>
#include <thread>
#include <string>
#include <vector>
#include <set>

#include "tc_thread.h"
#include "tc_atomic.h"
#include "tc_singleton.h"
#include "tc_thread_queue.h"
#include "logic_service_mysql_writer_thread.h"

class CLogicServiceMysqlHandle
{
public:
    CLogicServiceMysqlHandle();

    ~CLogicServiceMysqlHandle();

    void InitMysql();

    std::shared_ptr<taf::TC_Mysql> GetGameQueryConn();

    void ExecGameSQL(int iGroupID, int iUin, const std::string &strAPIName, const std::string &strColorRation,
                     const std::vector<std::string> &stSQL);

    void ExecStatSQL(int iGroupID, int iUin, const std::string &strAPIName, const std::string &strColorRation,
                     const string &strSQL);

private:
    std::shared_ptr<taf::TC_Mysql> m_pstGameQueryConn;

    std::unique_ptr<CLogicServiceMysqlWriterThread> m_pstGameSQLThread;

    std::unique_ptr<CLogicServiceMysqlWriterThread> m_pstStatSQLThread;
};

//自己控制该单例的退出时机，如果默认在程序退出时析构会跟其他单例有顺序关系
#define SERVICE_MYSQL_HANDLE taf::TC_Singleton<CLogicServiceMysqlHandle, taf::CreateUsingNew, taf::NoDestroyLifetime>::getInstance()



