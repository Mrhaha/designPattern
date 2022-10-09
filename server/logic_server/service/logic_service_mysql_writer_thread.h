#pragma once

#include <vector>
#include <memory>
#include "tc_logger.h"
#include "tc_mysql.h"
#include "tc_thread.h"
#include "tc_atomic.h"
#include "tc_thread_queue.h"

class CLogicServiceMysqlWriterThread : public taf::TC_Thread
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
    CLogicServiceMysqlWriterThread(bool bIsGameDB, const taf::TC_DBConf& stDBConf);

    void Terminate();

    void ExecSQL(int32_t iGroupID, int32_t iUin, const std::string& strCmdName, const std::string& strBillRation,
                 const std::vector<std::string>& stSQL);

protected:
    void run() override ;

    void runSQL(size_t millsecond);

private:
    bool m_bIsGameDB;

    taf::TC_Atomic m_iIsQuit;

    std::unique_ptr<taf::TC_Mysql> m_pstMysqlHandle;

    std::unique_ptr<taf::TC_DayLogger> m_pstMysqlLogger;

    taf::TC_ThreadQueue<TWriterQueueData> m_stSQLQueue;
};
