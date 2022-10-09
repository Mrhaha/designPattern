#include <memory>

#include <assert.h>
#include <stdbool.h>

#include "tc_uuid.h"
#include "tc_mysql.h"
#include "cross_mysql_handle.h"
#include "ServerConfig.h"
#include "ServerAlert.h"
#include "common_log_utility.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCrossMysqlHandle::CCrossMysqlHandle() : m_pSQLQueryConn(), m_pSQLWriteConn(), m_pSQLLogger(new taf::TC_DayLogger())
{
    //初始化日志
    const std::string strLogDir = "./log/mysql";
    const std::string strLogName = strLogDir + "/cross_game";
    m_pSQLLogger->init(strLogName);
    m_pSQLLogger->setLogLevel(taf::TC_DayLogger::INFO_LOG);
}

CCrossMysqlHandle::~CCrossMysqlHandle()
{
    Stop();
}

bool CCrossMysqlHandle::Start()
{
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    const auto& config = cross_config->GetZooCrossConfig();

    try
    {
        //初始化Mysql 查询
        m_pSQLQueryConn.reset(new taf::TC_Mysql(config.m_stMyGameDB));
        m_pSQLQueryConn->connect();

        //初始化Mysql 写入
        m_pSQLWriteConn.reset(new taf::TC_Mysql(config.m_stMyGameDB));
        m_pSQLWriteConn->connect();
    }
    catch(const taf::TC_Mysql_Exception& stEx)
    {
        TRACESVR(log_mask_system_error, "CCrossMysqlHandle::Mysql connection factory error. msg:%s\n", stEx.what());

        m_pSQLQueryConn.reset();
        m_pSQLWriteConn.reset();
        return false;
    }

    //启动Mysql 写线程
    m_pSQLThread = std::make_unique<std::thread>([this] { Run(); });
    if(nullptr == m_pSQLThread) return false;

    TRACESVR(log_mask_system_warning, "CCrossMysqlHandle::Mysql handle start...\n");
    return true;
}

bool CCrossMysqlHandle::Stop()
{
    m_stSQLQueue.stop();
    if(m_pSQLThread)
    {
        m_pSQLThread->join();
    }

    m_pSQLQueryConn.reset();

    TRACESVR(log_mask_system_warning, "CCrossMysqlHandle::Mysql handle stop.\n");
    return true;
}

std::shared_ptr<taf::TC_Mysql> CCrossMysqlHandle::GetGameQueryConn()
{
    return m_pSQLQueryConn;
}

void CCrossMysqlHandle::ExecGameSQL(int iGroupID, int iUin, const std::string &strAPIName, const std::string &strColorRation, const std::vector<std::string> &stSQL)
{
    TWriterQueueData stSQLData;
    stSQLData.m_iGroupID = iGroupID;
    stSQLData.m_iUin = iUin;
    stSQLData.m_strCmdName = strAPIName;
    stSQLData.m_strBillRation = strColorRation;
    stSQLData.m_stSQL = stSQL;
    m_stSQLQueue.emplace_back(stSQLData);
}

void CCrossMysqlHandle::Run()
{
    m_pSQLLogger->info() << "INFO|" << __FILE_BASE_NAME__ << "{" << __FUNCTION__ << "} start" << std::endl;

    std::optional<TWriterQueueData> opt;
    while ((opt = m_stSQLQueue.pop_front()).has_value())
    {
        auto& stData = opt.value();

        string sql_writer_result = "SQL_WRITER SUCCESS";
        string sql_result_msg = "";
        const std::string strSQL = vector_join(stData.m_stSQL, "");
        try
        {
            for (const auto& sql : stData.m_stSQL)
            {
                m_pSQLWriteConn->execute(sql);
            }
        }
        catch(const taf::TC_Mysql_Exception& stEx)
        {
            if(stData.m_stSQL.size() > 1)
            {
                m_pSQLWriteConn->execute("ROLLBACK");
            }

            sql_writer_result = "SQL_WRITER ERROR";
            sql_result_msg = std::string("|MSG:") + stEx.what();

            // 告警
            std::shared_ptr<CCrossServerConfig> cross_config;
            CCrossServerConfig::GetConfig(cross_config);
            CServerAlert::SendAlertNotice("exec_sql_fail", {
                { "sql", strSQL},
                { "error", stEx.what()},
                { "hostname", cross_config->GetZooCrossConfig().m_stCrossConfig.m_strHostname },
                { "server_ip", cross_config->GetZooCrossConfig().m_stCrossConfig.m_strHost },
                { "platform", cross_config->GetZooCrossConfig().m_stCrossConfig.m_strPlatform },
            });

            ::raise(SIGUSR2);
        }

        m_pSQLLogger->info() << "INFO|" << __FILE_BASE_NAME__ << ":" << __LINE__ << "{" << __FUNCTION__ << "}|"
                                 << sql_writer_result << "|CMD_NAME:" << stData.m_strCmdName << "|GROUP_ID:"
                                 << stData.m_iGroupID << "|UIN:" << stData.m_iUin << "|BILL_RATION:"
                                 << stData.m_strBillRation << "|SQL:" << strSQL << sql_result_msg << std::endl;
    }

    m_pSQLWriteConn.reset();
    mysql_thread_end();
}
