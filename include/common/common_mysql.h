//
// Created by hzl on 2019/4/18.
//
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <variant>

enum EMysqlDupKeyUpdateType
{
    EMDKUT_Null = 0,    // 不更新
    EMDKUT_Set,         // 更新为新值
    EMDKUT_Sum,         // 求和
};

struct TMysqlColumn
{
    std::string             m_strColumnName;
    std::variant<int32_t, std::string>  m_varValue;
    EMysqlDupKeyUpdateType  m_eType = EMDKUT_Null;
};

std::string build_insert_sql(const std::string& strTableName, const std::vector<TMysqlColumn>& stColumnVec);

std::string build_insert_escape_sql(const std::string& strTableName, const std::vector<TMysqlColumn>& stColumnVec
        , std::function<std::string(const std::string&)> fcEscape);

std::string build_insert_or_update_escape_sql(const std::string& strTableName, const std::vector<TMysqlColumn>& stColumnVec
        , std::function<std::string(const std::string&)> fcEscape);
