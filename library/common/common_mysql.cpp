//
// Created by hzl on 2019/4/18.
//

#include <sstream>
#include "common_mysql.h"

template<class... Ts> struct overloaded_function : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded_function(Ts...) -> overloaded_function<Ts...>;

std::string build_insert_sql(const std::string& strTableName, const std::vector<TMysqlColumn>& stColumnVec)
{
    auto fcEscape = [](const std::string& str){return str;};
    return build_insert_escape_sql(strTableName, stColumnVec, fcEscape);
}

std::string build_insert_escape_sql(const std::string &strTableName, const std::vector<TMysqlColumn> &stColumnVec,
                                    std::function<std::string(const std::string &)> fcEscape)
{
    std::ostringstream strColumnNames;
    std::ostringstream strColumnValues;

    overloaded_function fcBuildValue{
        [&](const int32_t& value) {strColumnValues << value;},
        [&](const std::string& value) {strColumnValues << "'" << fcEscape(value) << "'";}
    };

    std::string strBeginComma;
    for (const auto& column : stColumnVec)
    {
        strColumnNames << strBeginComma;
        strColumnValues << strBeginComma;

        strColumnNames << "`" << column.m_strColumnName << "`";
        std::visit(fcBuildValue, column.m_varValue);

        strBeginComma = ", ";
    }

    std::ostringstream os;
    os << "insert into " << strTableName << " (" << strColumnNames.str() << ") values (" << strColumnValues.str() << ");";
    return os.str();
}

std::string build_insert_or_update_escape_sql(const std::string &strTableName, const std::vector<TMysqlColumn> &stColumnVec, std::function<std::string(const std::string &)> fcEscape)
{
    std::ostringstream strColumnNames;
    std::ostringstream strColumnValues;
    std::ostringstream strColumnUpdates;

    overloaded_function fcBuildValue{
            [&](const int32_t& value) {strColumnValues << value;},
            [&](const std::string& value) {strColumnValues << "'" << fcEscape(value) << "'";}
    };

    overloaded_function fcUpdateValue{
            [&](const int32_t& value) {strColumnUpdates << value;},
            [&](const std::string& value) {strColumnUpdates << "'" << fcEscape(value) << "'";}
    };

    std::string strBeginComma;
    for (const auto& column : stColumnVec)
    {
        strColumnNames << strBeginComma;
        strColumnValues << strBeginComma;

        strColumnNames << "`" << column.m_strColumnName << "`";
        std::visit(fcBuildValue, column.m_varValue);

        switch(column.m_eType)
        {
            case EMDKUT_Set:
                if(!strColumnUpdates.str().empty())
                {
                    strColumnUpdates << strBeginComma;
                }

                strColumnUpdates << "`" << column.m_strColumnName << "` = ";
                std::visit(fcUpdateValue, column.m_varValue);
                break;

            case EMDKUT_Sum:
                if(std::get_if<int32_t>(&column.m_varValue) == nullptr)
                {
                    return "";
                }

                if(!strColumnUpdates.str().empty())
                {
                    strColumnUpdates << strBeginComma;
                }

                strColumnUpdates << "`" << column.m_strColumnName << "` = `" << column.m_strColumnName << "` + ";
                std::visit(fcUpdateValue, column.m_varValue);
                break;

            default:
                break;
        }

        strBeginComma = ", ";
    }

    std::ostringstream os;
    os << "insert into " << strTableName << " (" << strColumnNames.str() << ") values (" << strColumnValues.str() << ") ON DUPLICATE KEY UPDATE " << strColumnUpdates.str() << ";";
    return os.str();
}
