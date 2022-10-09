#include "logic_service_mysql_writer_thread.h"
#include "common_tools.h"
#include "logic_service_logger.h"
#include "logic_service_api.h"

CLogicServiceMysqlWriterThread::CLogicServiceMysqlWriterThread(bool bIsGameDB, const taf::TC_DBConf& stDBConf)
    : m_bIsGameDB(bIsGameDB)
    , m_iIsQuit(0)
    , m_pstMysqlHandle(new taf::TC_Mysql(stDBConf))
    , m_pstMysqlLogger(new taf::TC_DayLogger())
    , m_stSQLQueue()
{
    try
    {
        m_pstMysqlHandle->connect();
    }
    catch(const taf::TC_Mysql_Exception& stEx)
    {
        SERVICE_LOG_TRACE_ERROR << "MYSQL CONNECTION FACTORY ERROR|CREATE CONNECTION ERROR|MSG:" << stEx.what() << std::endl;

        m_pstMysqlHandle.reset();
    }

    //初始化日志
    const std::string strLogDir = "./log/mysql";
    const std::string strLogName = strLogDir + "/logic_" + stDBConf._database;
    m_pstMysqlLogger->init(strLogName);
    m_pstMysqlLogger->setLogLevel(taf::TC_DayLogger::INFO_LOG);
}

void CLogicServiceMysqlWriterThread::Terminate()
{
    m_iIsQuit = 1;
    m_stSQLQueue.notifyT();
}

void CLogicServiceMysqlWriterThread::ExecSQL(int32_t iGroupID, int32_t iUin, const std::string &strCmdName,
                                           const std::string &strBillRation,
                                           const std::vector<std::string> &stSQL)
{
    TWriterQueueData stSQLData;
    stSQLData.m_iGroupID = iGroupID;
    stSQLData.m_iUin = iUin;
    stSQLData.m_strCmdName = strCmdName;
    stSQLData.m_strBillRation = strBillRation;
    stSQLData.m_stSQL = stSQL;

    m_stSQLQueue.push_back(stSQLData);
}

void CLogicServiceMysqlWriterThread::run()
{
    assert(m_pstMysqlHandle);

    while(m_iIsQuit.get() != 1)
    {
        //always wait
        runSQL(-1);
    }

    //never wait
    while (!m_stSQLQueue.empty())
    {
        runSQL(0);
    }

    m_pstMysqlHandle.reset();
    mysql_thread_end();
}

void CLogicServiceMysqlWriterThread::runSQL(size_t millsecond)
{
    TWriterQueueData stData;
    if(m_stSQLQueue.pop_front(stData, millsecond))
    {
        string sql_writer_result = "SQL_WRITER SUCCESS";
        string sql_result_msg = "";
        const std::string strSQL = vector_join(stData.m_stSQL, "");
        try
        {
            for (const auto& sql : stData.m_stSQL)
            {
                m_pstMysqlHandle->execute(sql);
            }
        }
        catch(const taf::TC_Mysql_Exception& stEx)
        {
            if(stData.m_stSQL.size() > 1)
            {
                m_pstMysqlHandle->execute("ROLLBACK");
            }

            sql_writer_result = "SQL_WRITER ERROR";
            sql_result_msg = std::string("|MSG:") + stEx.what();

            SERVICE_API_INS->RequestServiceAlert("exec_sql_fail", {
                    { "sql", strSQL },
                    { "error", stEx.what()}
            });

            if (m_bIsGameDB)
            {
                ::raise(SIGUSR2);
            }
        }

        m_pstMysqlLogger->info() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
                << sql_writer_result << "|CMD_NAME:" << stData.m_strCmdName << "|GROUP_ID:"
                << stData.m_iGroupID << "|UIN:" << stData.m_iUin << "|BILL_RATION:"
                << stData.m_strBillRation << "|SQL:" << strSQL << sql_result_msg << std::endl;
    }
}

