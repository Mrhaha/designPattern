#include "logic_link_define.h"
#include "tc_uuid.h"

#include "logic_record_sql_table_cache.h"

CLogicRecordCache*
CLogicRecordCache::s_stInstance = nullptr;

void
CLogicRecordCache::ExecSQL(int iGroupID, int iUin, const std::string& strCmdName)
{
    if (false == m_stRecordVec.empty())
    {
        std::vector<std::string> stSQL;

        stSQL.push_back(CLogicSQLTableRecordBase::SQL_TRANSACTION_START);

        for (auto& stIter : m_stRecordVec)
        {
			std::string sql = stIter->MakeSQL();
			if (!sql.empty())
			{
				stSQL.push_back(std::move(sql));
			}
			else
			{
			    // 文文日报因为有互斥类型，可能频繁产生空sql语句
                // LOGIC_SERVICE_API_INS->RequestServiceAlert("empty_sql_operation", {{ "cmd_name", strCmdName}});
			}
        }

        stSQL.push_back(CLogicSQLTableRecordBase::SQL_TRANSACTION_END);

        const std::string strColorRation = taf::generateUUID();

        LOGIC_SERVICE_API_INS->ExecGameSQL(iGroupID, iUin, strCmdName, strColorRation, stSQL);

        m_stRecordVec.clear();
    }
}

CLogicRecordCache*
CLogicRecordCache::getInstance()
{
    if (s_stInstance == nullptr)
        s_stInstance = new CLogicRecordCache;
    return s_stInstance;
}
