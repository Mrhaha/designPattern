#pragma once

#include "logic_record_sql_table.h"

#include <vector>
#include <string>
#include <memory>

class CCrossRecordCache
{
public:
    void ExecSQL(int iGroupID, int iUin, const std::string& strCmdName);

    static CCrossRecordCache* getInstance();

    std::vector<std::shared_ptr<CLogicSQLTableRecordBase>> m_stRecordVec;

private:
    static CCrossRecordCache* s_stInstance;
};

#define LOGIC_RECORD_CACHE_INS CCrossRecordCache::getInstance()
