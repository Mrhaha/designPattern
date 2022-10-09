#include "logic_link_define.h"
#include "logic_record_sql_table.h"

const char* CLogicSQLTableRecordBase::SQL_TRANSACTION_START = "START TRANSACTION;";
const char* CLogicSQLTableRecordBase::SQL_TRANSACTION_END = "COMMIT;";

std::shared_ptr<taf::TC_Mysql> CLogicSQLTableRecordBase::s_pMysqlHandle;

void CLogicSQLTableRecordBase::SetOperation(int iOperation)
{
    if (m_iOperation == iOperation)
        return;

    switch(m_iOperation)
    {
        case UNDEFINED:
            m_iOperation = iOperation;
            break;
        case INSERT:
            if (iOperation == DELETE)
            {
                m_iOperation = UNDEFINED;
            }
            else if (iOperation == UPDATE)
            {
                m_iOperation = INSERT;
            }
            break;
        case UPDATE:
            if (iOperation == DELETE)
            {
                m_iOperation = DELETE;
            }
            else if (iOperation == INSERT)
            {
                LOGIC_LOG_TRACE_ERROR << "SYS_FAULT|INSERT, AFTER UPDATE" << std::endl;
                break;
            }
            break;
        case DELETE:
            break;
    }
}

std::string CLogicSQLTableRecordBase::MakeSQL()
{
    switch(m_iOperation)
    {
        case UNDEFINED:
            return "";
        case INSERT:
            return MakeInsertSQL();
        case UPDATE:
            return MakeUpdateSQL();
        case DELETE:
            return MakeDeleteSQL();
        default:
            return "";
    }
}

