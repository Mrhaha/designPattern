#pragma once

#include "logic_record_sql_table.h"

#include <vector>
#include <string>
#include <memory>

class CLogicRecordCache
{
public:
    void ExecSQL(int iGroupID, int iUin, const std::string& strCmdName);

    static CLogicRecordCache* getInstance();

    std::vector<std::shared_ptr<CLogicSQLTableRecordBase>> m_stRecordVec;

private:
    static CLogicRecordCache* s_stInstance;
};

#define LOGIC_RECORD_CACHE_INS CLogicRecordCache::getInstance()


