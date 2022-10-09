#include <cassert>
#include <sstream>

#include "tc_common.h"
#include "common_tinyxml2.h"

#include "mysql_table_info.h"
#include "mysql_global_info.h"

int
CMysqlTableInfo::InitFromXml(const std::string& strFile)
{
    int iAmount = 0;
    tinyxml2::XMLDocument doc;
    doc.LoadFile(strFile.c_str());
    tinyxml2::XMLElement* pRoot = doc.RootElement();

    m_iDivNum = 0;
    if (pRoot->Attribute("div_num") != NULL)
        m_iDivNum = ::atoi(pRoot->Attribute("div_num"));

    m_strTableName = pRoot->Attribute("table_name");

    tinyxml2::XMLElement* pElement = pRoot->FirstChildElement("field_info");
    while (pElement)
    {
        table_info_t stTableInfo;

        std::string strSize, strDefaultVal, strUnsigned, strAutoIncrement;
        std::string strDesc = pElement->Attribute("desc");
        std::string strType = pElement->Attribute("type");
        std::string strFieldName = pElement->Attribute("field_name");
		std::string strComment = "";

        if (strType.compare("int") == 0)
        {
            strSize = "10";
            strDefaultVal = "0";

            if (pElement->Attribute("default") != NULL)
                strDefaultVal = pElement->Attribute("default");
        }
        else if (strType.compare("char") == 0)
        {
            strSize = "3";
            strType = "tinyint";
            strDefaultVal = "0";

            if (pElement->Attribute("default") != NULL)
                strDefaultVal = pElement->Attribute("default");
        }
        else if (strType.compare("uint64_t") == 0)
        {
            strSize = "20";
            strType = "bigint";
            strDefaultVal = "0";
            strUnsigned = "";

            if (pElement->Attribute("unsigned") != NULL)
                strUnsigned = "UNSIGNED";

            if (pElement->Attribute("default") != NULL)
                strDefaultVal = pElement->Attribute("default");
        }
        else if (strType.compare("std::string") == 0)
        {
            strSize = "32";
            strType = "varchar";
            strDefaultVal = "''";

            if (pElement->Attribute("size") != NULL)
                strSize = pElement->Attribute("size");
        }
        else if (strType.compare("userdata") == 0)
        {
            strSize = "128";
            strType = "varbinary";
            strDefaultVal = "''";

            if (pElement->Attribute("size") != NULL)
                strSize = pElement->Attribute("size");
        }
        else
        {
            assert(!"UNKNOWN TYPE");
        }

        if (pElement->Attribute("increment") != NULL)
        {
            m_strAutoIncreament = strAutoIncrement = pElement->Attribute("increment");
        }

		if (pElement->Attribute("comment") != NULL)
		{
			strComment = pElement->Attribute("comment");
		}

        if (pElement->Attribute("primary_key") != NULL)
        {
            m_stPrimaryKey.push_back(strFieldName);
        }

        if (pElement->Attribute("unique") != NULL)
        {
            m_stUniqueKey.push_back(strFieldName);
        }

		if (pElement->Attribute("index") != NULL)
		{
			m_stIndexKey.push_back(strFieldName);
		}

        stTableInfo.m_strFieldName = strFieldName;
        stTableInfo.m_strUnsigned = strUnsigned;
        stTableInfo.m_strSize = strSize;
        stTableInfo.m_strType = strType;
        stTableInfo.m_strDefaultVal = strDefaultVal;
        stTableInfo.m_strAutoIncreament = strAutoIncrement;
		stTableInfo.m_strComment = strComment.empty() ? "" : (" COMMENT '" + strComment + "'");

        m_stMysqlTableInfo[strDesc] = stTableInfo;

        ++iAmount;

        pElement = pElement->NextSiblingElement("field_info");
    }

    return iAmount;
}

int
CMysqlTableInfo::Encode(int& iDivNum, std::string& strTableName, std::string& strTableBinInfo) const
{
    iDivNum = m_iDivNum;
    strTableName = m_strTableName;

    msgpack::sbuffer stBuffer;                                                                                                                       
    msgpack::pack(stBuffer, m_stMysqlTableInfo); 

    strTableBinInfo = taf::TC_Common::bin2str(stBuffer.data(), stBuffer.size());

    return 0;
}

int
CMysqlTableInfo::Decode(int iDivNum, const std::string& strTableName, const std::string& strTableBinInfo)
{
    m_iDivNum = iDivNum;
    m_strTableName = strTableName;

    const char* pBuffer = strTableBinInfo.c_str(); 
    size_t uiLength = strTableBinInfo.size();
    msgpack::unpacked stResult;     
    if(uiLength > 0)
    {
        try            
        {
            msgpack::unpack(&stResult, pBuffer, uiLength);
        }              
        catch(const std::runtime_error& stEx)
        {              
            return -1;
        }              

        const msgpack::object& stMsgObj = stResult.get();
        try            
        {              
            stMsgObj.convert(&m_stMysqlTableInfo);      
        }
        catch(const std::bad_cast& stEx)
        {
            return -2;
        }
    }
    return 0;
}

std::vector<std::string>
CMysqlTableInfo::GenerateAlterSQL(const CMysqlTableInfo& stMysqlTable)
{
    assert(stMysqlTable.m_strTableName == m_strTableName);

    std::vector<std::string> stRet;

    auto stMysqlTableInfoBeg = m_stMysqlTableInfo.begin();
    auto stMysqlTableInfoEnd = m_stMysqlTableInfo.end();

    for ( ; stMysqlTableInfoBeg != stMysqlTableInfoEnd; ++stMysqlTableInfoBeg)
    {
        auto stMysqlTableInfoIter = stMysqlTable.m_stMysqlTableInfo.find(stMysqlTableInfoBeg->first);
        if (stMysqlTableInfoIter == stMysqlTable.m_stMysqlTableInfo.end())
        {
            auto& stFieldInfo = stMysqlTableInfoBeg->second;
			std::string strDefaultValue = " DEFAULT " + stFieldInfo.m_strDefaultVal + " ";
	        if (!stFieldInfo.m_strAutoIncreament.empty())
	        {
				strDefaultValue = " AUTO_INCREMENT ";
	        }

            if (m_iDivNum == 0)
            {
                std::string strAlter = "ALTER TABLE " + m_strTableName + " ADD " + stFieldInfo.m_strFieldName + " " + stFieldInfo.m_strType + "(" + stFieldInfo.m_strSize + ") " + stFieldInfo.m_strUnsigned + strDefaultValue + stFieldInfo.m_strComment + ";";

                stRet.push_back(strAlter);
            }
            else
            {
                for (int i = 0; i < m_iDivNum; ++i)
                {
                    std::ostringstream oss;
                    oss << i;
                    std::string strAlter = "ALTER TABLE " + m_strTableName + "_" + oss.str() + " ADD " + stFieldInfo.m_strFieldName + " " + stFieldInfo.m_strType + "(" + stFieldInfo.m_strSize + ") " + stFieldInfo.m_strUnsigned + strDefaultValue + stFieldInfo.m_strComment + ";";
                    stRet.push_back(strAlter);
                }
            }
        }
        else
        {
            // not equal
            if (!(stMysqlTableInfoBeg->second == stMysqlTableInfoIter->second))
            {
                auto& stFieldInfo = stMysqlTableInfoBeg->second;
				std::string strDefaultValue = " DEFAULT " + stFieldInfo.m_strDefaultVal + " ";
				if (!stFieldInfo.m_strAutoIncreament.empty())
				{
					strDefaultValue = " AUTO_INCREMENT ";
				}
                if (m_iDivNum == 0)
                {
                    std::string strAlter = "ALTER TABLE " + m_strTableName + " CHANGE " + stMysqlTableInfoIter->second.m_strFieldName + " " + stFieldInfo.m_strFieldName + " " + stFieldInfo.m_strType + "(" + stFieldInfo.m_strSize + ") " + stFieldInfo.m_strUnsigned + strDefaultValue + stFieldInfo.m_strComment + ";";

                    stRet.push_back(strAlter);
                }
                else
                {
                    for (int i = 0; i < m_iDivNum; ++i)
                    {
                        std::ostringstream oss;
                        oss << i;
                        std::string strAlter = "ALTER TABLE " + m_strTableName + "_" + oss.str() + " CHANGE " + stMysqlTableInfoIter->second.m_strFieldName + " " + stFieldInfo.m_strFieldName + " " + stFieldInfo.m_strType + "(" + stFieldInfo.m_strSize + ") " + stFieldInfo.m_strUnsigned + strDefaultValue + stFieldInfo.m_strComment + ";";
                        stRet.push_back(strAlter);
                    }
                }
            }
        }
    }

    return stRet;
}

std::string
CMysqlTableInfo::GenerateInsertSQL() const
{
    std::string strRet = "INSERT INTO table_info_t VALUES(";

    int iDivNum;
    std::string strTableName, strTableBinInfo;

    Encode(iDivNum, strTableName, strTableBinInfo);

    std::ostringstream oss;
    oss << iDivNum;

    strRet += "'" + strTableName + "',";
    strRet += oss.str() + ",";
    strRet += "0x" + strTableBinInfo;
    strRet += ");";

    return strRet;
}

std::string
CMysqlTableInfo::GenerateUpdateSQL() const
{
    std::string strRet = "UPDATE table_info_t SET table_info=0x";

    msgpack::sbuffer stBuffer;
    msgpack::pack(stBuffer, m_stMysqlTableInfo); 

    strRet += taf::TC_Common::bin2str(stBuffer.data(), stBuffer.size());

    strRet += " WHERE table_name='" + m_strTableName + "';";

    return strRet;
}

std::vector<std::string>
CMysqlTableInfo::GenerateCreateSQL() const
{
    std::vector<std::string> stRet;

    auto stMysqlTableInfoBeg = m_stMysqlTableInfo.begin();
    auto stMysqlTableInfoEnd = m_stMysqlTableInfo.end();

    std::string strAllFiled = "(";
    for ( ; stMysqlTableInfoBeg != stMysqlTableInfoEnd; ++stMysqlTableInfoBeg)
    {
        strAllFiled += stMysqlTableInfoBeg->second.m_strFieldName + " " +
                       stMysqlTableInfoBeg->second.m_strType + "(" + 
                       stMysqlTableInfoBeg->second.m_strSize + ") NOT NULL ";

        if (stMysqlTableInfoBeg->second.m_strAutoIncreament.empty())
        {
            strAllFiled += "DEFAULT " + stMysqlTableInfoBeg->second.m_strDefaultVal + " ";
        }
        else
        {
            strAllFiled += "AUTO_INCREMENT ";
        }

	    if (!stMysqlTableInfoBeg->second.m_strComment.empty())
	    {
			strAllFiled += stMysqlTableInfoBeg->second.m_strComment;
	    }

		strAllFiled += ",";
    }
    
    strAllFiled += "PRIMARY KEY(";
    for (size_t i = 0; i < m_stPrimaryKey.size(); ++i)
    {
        strAllFiled += m_stPrimaryKey[i] + ",";
    }
    strAllFiled[strAllFiled.size() - 1] = ')';

    if (!m_stUniqueKey.empty())
    {
        strAllFiled += ", UNIQUE(";
        std::string strUniqueKey;
        for (size_t i = 0; i < m_stUniqueKey.size(); ++i)
        {
            strUniqueKey += m_stUniqueKey[i] + ",";
        }
        strUniqueKey[strUniqueKey.size() - 1] = ')';
        strAllFiled += strUniqueKey;
    }

	for (size_t i = 0; i < m_stIndexKey.size(); ++i)
	{
		strAllFiled += ", INDEX(" + m_stIndexKey[i] + ")";
	}

    strAllFiled += ")ENGINE = InnoDB, CHARSET = utf8";

    if (!m_strAutoIncreament.empty())
    {
        strAllFiled += ", AUTO_INCREMENT = " + m_strAutoIncreament;
    }

    strAllFiled += ";";

    if (m_iDivNum != 0)
    {
        std::string strCreate = "CREATE TABLE IF NOT EXISTS " + m_strTableName + "_";
        for (int i = 0; i < m_iDivNum; ++i)
        {
            ostringstream oss;
            oss << i;
            std::string strSQL = strCreate + oss.str() + " " + strAllFiled;
            stRet.push_back(strSQL);
        }
    }
    else
    {
        std::string strCreate = "CREATE TABLE IF NOT EXISTS ";
        std::string strSQL = strCreate + m_strTableName + strAllFiled;
        stRet.push_back(strSQL);
    }

    return stRet;
}

std::string
CMysqlTableInfo::Dump() const
{
    std::ostringstream oss;
    oss << "table name : " << m_strTableName;
    oss << " div : " << m_iDivNum << std::endl;
    auto stIterBeg = m_stMysqlTableInfo.begin();
    auto stIterEnd = m_stMysqlTableInfo.end();

    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        oss << " desc : " << stIterBeg->first;
        oss << " field : " << stIterBeg->second.m_strFieldName;
        oss << " size : " << stIterBeg->second.m_strSize;
        oss << " type : " << stIterBeg->second.m_strType;
        oss << " default : " << stIterBeg->second.m_strDefaultVal;
        oss << std::endl;
    }
    return (oss.str());
}

