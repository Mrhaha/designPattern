#include <memory>

#include <assert.h>
#include <stdbool.h>

#include "tc_uuid.h"
#include "tc_mysql.h"
#include "logic_service_mysql_handle.h"
#include "logic_service_api.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CLogicServiceMysqlHandle::CLogicServiceMysqlHandle() : m_pstGameQueryConn(), m_pstGameSQLThread(), m_pstStatSQLThread()
{
    
}

CLogicServiceMysqlHandle::~CLogicServiceMysqlHandle()
{
    m_pstGameSQLThread->Terminate();
    m_pstStatSQLThread->Terminate();

    m_pstGameSQLThread->getThreadControl().join();
    m_pstStatSQLThread->getThreadControl().join();
}

void CLogicServiceMysqlHandle::InitMysql()
{
    const auto& config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameMysqlConfig();
    //初始化Mysql 查询
    m_pstGameQueryConn.reset(new taf::TC_Mysql(config.m_stGameDB));
    m_pstGameQueryConn->connect();

    //启动Mysql 写线程
    m_pstGameSQLThread = std::make_unique<CLogicServiceMysqlWriterThread>(true, config.m_stGameDB);
    m_pstStatSQLThread = std::make_unique<CLogicServiceMysqlWriterThread>(false, config.m_stStatDB);

    assert(m_pstGameSQLThread);
    assert(m_pstStatSQLThread);

    m_pstGameSQLThread->start();
    m_pstStatSQLThread->start();
}

std::shared_ptr<taf::TC_Mysql> CLogicServiceMysqlHandle::GetGameQueryConn()
{
    return m_pstGameQueryConn;
}

void CLogicServiceMysqlHandle::ExecGameSQL(int iGroupID, int iUin, const std::string &strAPIName,
                                    const std::string &strColorRation,
                                    const std::vector<std::string> &stSQL)
{
    m_pstGameSQLThread->ExecSQL(iGroupID, iUin, strAPIName, strColorRation, stSQL);
}

void CLogicServiceMysqlHandle::ExecStatSQL(int iGroupID, int iUin, const std::string &strAPIName,
                                           const std::string &strColorRation,
                                           const string &strSQL)
{
    m_pstStatSQLThread->ExecSQL(iGroupID, iUin, strAPIName, strColorRation, { strSQL });
}
