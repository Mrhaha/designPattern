#pragma once

#include "msgpack.hpp"

#include <map>
#include <vector>
#include <string>
#include <iostream>

class CMysqlTableInfo
{
public:
    struct table_info_t
    {
        MSGPACK_DEFINE(m_strFieldName,
                       m_strUnsigned,
                       m_strSize,
                       m_strType,
                       m_strDefaultVal,
                       m_strAutoIncreament,
					   m_strComment);

        bool operator == (const table_info_t& stTableInfo)
        {
            if (m_strFieldName != stTableInfo.m_strFieldName)
                return false;

            if (m_strDefaultVal != stTableInfo.m_strDefaultVal)
                return false;

			if (m_strComment != stTableInfo.m_strComment)
				return false;

            if (m_strUnsigned != stTableInfo.m_strUnsigned)
            {
                std::cout << "WARNING!!! " << m_strFieldName << " WANT TO CHANGE COLUMN ATTRIBUTE" << std::endl;
            }

            if (m_strSize != stTableInfo.m_strSize)
            {
                std::cout << "WARNING!!! " << m_strFieldName << " WANT TO CHANGE COLUMN ATTRIBUTE" << std::endl;
            }

            if (m_strType != stTableInfo.m_strType)
            {
                std::cout << "WARNING!!! " << m_strFieldName << " WANT TO CHANGE COLUMN ATTRIBUTE" << std::endl;
            }

            return true;
        }

        std::string     m_strFieldName;
        std::string     m_strUnsigned;
        std::string     m_strSize;
        std::string     m_strType;
        std::string     m_strDefaultVal;
        std::string     m_strAutoIncreament;
		std::string		m_strComment;
    };

    typedef std::map<std::string, table_info_t> mysql_table_info_t;

    int InitFromXml(const std::string& strFile);

    int Encode(int& iDivNum, std::string& strTableName, std::string& strTableBinInfo) const;
    int Decode(int iDivNum, const std::string& strTableName, const std::string& strTableBinInfo);

    std::string GenerateInsertSQL() const;

    std::string GenerateUpdateSQL() const;

    std::vector<std::string> GenerateCreateSQL() const;

    std::vector<std::string> GenerateAlterSQL(const CMysqlTableInfo& stMysqlTableInfo);

    std::string Dump() const;

public:
    int                         m_iDivNum;
    std::string                 m_strTableName;
    std::string                 m_strAutoIncreament;
    std::vector<std::string>    m_stUniqueKey;
    std::vector<std::string>    m_stPrimaryKey;
	std::vector<std::string>	m_stIndexKey;
    mysql_table_info_t          m_stMysqlTableInfo;
};
