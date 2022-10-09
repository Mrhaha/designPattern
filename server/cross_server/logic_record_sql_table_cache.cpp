#include "tc_uuid.h"

#include "logic_record_sql_table_cache.h"
#include "cross_mysql_handle.h"

CCrossRecordCache*
CCrossRecordCache::s_stInstance = nullptr;

void
CCrossRecordCache::ExecSQL(int iGroupID, int iUin, const std::string& strCmdName)
{
    if (!m_stRecordVec.empty())
    {
        std::vector<std::string> stSQL;

        stSQL.emplace_back(CLogicSQLTableRecordBase::SQL_TRANSACTION_START);

        for (auto& stIter : m_stRecordVec)
        {
			std::string sql = stIter->MakeSQL();
			if (!sql.empty())
			{
				stSQL.push_back(std::move(sql));
			}
        }

        stSQL.emplace_back(CLogicSQLTableRecordBase::SQL_TRANSACTION_END);

        const std::string strColorRation = taf::generateUUID();

        CROSS_MYSQL_HANDLE->ExecGameSQL(iGroupID, iUin, strCmdName, strColorRation, stSQL);

        m_stRecordVec.clear();
    }
}

CCrossRecordCache*
CCrossRecordCache::getInstance()
{
    if (s_stInstance == nullptr)
        s_stInstance = new CCrossRecordCache;
    return s_stInstance;
}
