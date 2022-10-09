#include <map>
#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <dirent.h>
#include <unistd.h>

#include <sys/stat.h>

#include "common_tinyxml2.h"

#include "field_info.h"
#include "print_tool.h"
#include "mysql_table_info.h"
#include "mysql_global_info.h"

using namespace std;

#define MEMBER_SPACE_NUM 24

class CTableInfo {
public:

    ~CTableInfo() 
    {
        for ( size_t i = 0; i < m_stFieldInfo.size(); ++i )
            delete m_stFieldInfo[i];

        for ( size_t i = 0; i < m_stPrimaryKey.size(); ++i )
            delete m_stPrimaryKey[i];
    }

    void InitFromXml(std::string strFile) 
    {
        tinyxml2::XMLDocument doc;

        doc.LoadFile(strFile.c_str());

        tinyxml2::XMLElement* pRoot = doc.RootElement();
        m_strTableName = pRoot->Attribute("table_name");
        m_strClassName = pRoot->Attribute("class_name");
        m_strCacheTypeName = pRoot->Attribute("cache_name");
        //m_strCacheTypeNameValueType = m_strCacheTypeName.substr(0, m_strCacheTypeName.length() - 2)  + "_value_type";
        m_strCacheTypeNameValueType = m_strCacheTypeName  + "ValueType";
        m_iDivNum = ::atoi(pRoot->Attribute("div_num"));
        m_strInstance = pRoot->Attribute("storage_instance");
        m_strFileName = pRoot->Attribute("file_name");
        m_iAutoIncrement = 0;

        if (pRoot->Attribute("engine") != NULL)
            m_strStorageEngine = pRoot->Attribute("engine");
        else
            m_strStorageEngine = cszDefaultEngine;

        if (pRoot->Attribute("charset") != NULL)
            m_strCharset = pRoot->Attribute("charset");
        else
            m_strCharset = cszDefaultCharSet;

        if (pRoot->Attribute("global") != NULL)
            m_bGlobal = true;
        else
            m_bGlobal = false;

        std::vector<CFieldInfoBase*>* pStore = NULL;
        tinyxml2::XMLElement* pElement = pRoot->FirstChildElement("field_info");
        while (pElement)
        {
            if (pElement->Attribute("primary_key") == NULL)
                pStore = &m_stFieldInfo;
            else
                pStore = &m_stPrimaryKey;

            std::string strType = pElement->Attribute("type");
            std::string strFieldName = pElement->Attribute("field_name");

            CFieldInfoBase* pFiledBase = NULL;

            if (strType.compare("int") == 0)
            {
                pFiledBase = new CIntFieldInfo;
            }
            else if (strType.compare("char") == 0)
            {
                pFiledBase = new CCharFieldInfo;
            }
            else if (strType.compare("uint64_t") == 0)
            {
                pFiledBase = new CUint64FieldInfo;
            }
            else if (strType.compare("std::string") == 0)
            {
                pFiledBase = new CStringFieldInfo;
            }
            else if (strType.compare("userdata") == 0)
            {
                pFiledBase = new CUserDataFieldInfo;
                strType = pElement->Attribute("class");
            }
            else
                assert(!"UNKNOWN TYPE");

            if (pElement->Attribute("unique") != NULL)
            {
                pFiledBase->m_bIsUnique = true;
                m_stUniqueString.push_back(strFieldName);
            }

            if (pElement->Attribute("increment") != NULL)
            {
                m_iAutoIncrement = pFiledBase->m_iIncrement = ::atoi(pElement->Attribute("increment"));
            }

            if (pElement->Attribute("default") != NULL)
                pFiledBase->m_strDefaultVal = pElement->Attribute("default");

            if (pElement->Attribute("size") != NULL)
                pFiledBase->m_strSize = pElement->Attribute("size");

            if (pElement->Attribute("unsigned") != NULL)
                pFiledBase->m_bIsUnsigned = true;

			if (pElement->Attribute("where") != NULL)
				m_stWhereKey.push_back(pFiledBase);

			if (pElement->Attribute("index") != NULL)
				m_stIndexKey.push_back(pFiledBase);

			if (pElement->Attribute("comment") != NULL)
				pFiledBase->m_strComment = pElement->Attribute("comment");

            pFiledBase->Initialize(strType, strFieldName);

            if (pElement->Attribute("div") != NULL)
                m_strDivVarName = pFiledBase->m_strVarName;

            pStore->push_back(pFiledBase);

            pElement = pElement->NextSiblingElement("field_info");
        }

        tinyxml2::XMLNode* pUserClassInfo = pRoot->FirstChildElement("class_info");
        if (pUserClassInfo)
        {
            tinyxml2::XMLNode* pClassInfo = pUserClassInfo->FirstChild();
            while (pClassInfo)
            {
                std::string strClass = pClassInfo->Value();
                std::string strClassCode = pClassInfo->ToElement()->GetText();

                m_stUserClassInfo[strClass] = strClassCode;

                pClassInfo = pClassInfo->NextSibling();
            }
        }
    
		InitWhereStatement();
    }

	void InitWhereStatement()
    {
	    if (m_stWhereKey.empty())
	    {
			m_strWhereParam = "int iUin, int iGroupID";
			m_strWhereSelect = "uid=%d AND group_id=%d";
			m_strWhereQuery = "iUin, iGroupID";
	    }
		else
		{
			for (const auto& key : m_stWhereKey)
			{
				std::string param = key->m_strType + std::string(" ") + key->m_strTempName;
                std::string select;
                if (key->m_bIsUnsigned)
                {
                    select = key->m_strFieldName + std::string("=%ld");
                } else
                {
                    select = key->m_strFieldName + std::string("=%d");
                }
				std::string query = key->m_strTempName;
				if (m_strWhereParam.empty())
				{
					m_strWhereParam.append(param);
					m_strWhereSelect.append(select);
					m_strWhereQuery.append(query);
				}
				else
				{
					m_strWhereParam.append(std::string(", ") + param);
					m_strWhereSelect.append(std::string(" AND ") + select);
					m_strWhereQuery.append(std::string(", ") + query);
				}
			}
		}
    }

    bool                            m_bGlobal;
    int                             m_iDivNum;
    int                             m_iAutoIncrement;
    std::string                     m_strCacheTypeName;
    std::string                     m_strCacheTypeNameValueType;
    std::string                     m_strTableName;
    std::string                     m_strClassName;
    std::string                     m_strInstance;
    std::string                     m_strFileName;
    std::string                     m_strDivVarName;
    std::string                     m_strStorageEngine;
    std::string                     m_strCharset;

    std::vector<CFieldInfoBase*>    m_stFieldInfo;
    std::vector<CFieldInfoBase*>    m_stPrimaryKey;
	std::vector<CFieldInfoBase*>	m_stIndexKey;
	std::vector<CFieldInfoBase*>	m_stWhereKey;
	std::string						m_strWhereParam;
	std::string						m_strWhereSelect;
	std::string						m_strWhereQuery;

    std::vector<std::string>        m_stUniqueString;

    std::map<std::string, std::string>  m_stUserClassInfo;

    void GenerateInclude(const std::string& strSrcPath)
    {
        int fd = dup(1);
        freopen((strSrcPath + m_strFileName + ".h").c_str(), "w", stdout);

        std::string strINCLUDE = m_strFileName;
        std::transform(strINCLUDE.begin(), strINCLUDE.end(), strINCLUDE.begin(), ::toupper);

        // std::cout << "#ifndef __" << strINCLUDE << "_H__" << std::endl;
        // std::cout << "#define __" << strINCLUDE << "_H__" << std::endl;
        std::cout << "#pragma once" << std::endl;

        PrintEnter();

        std::cout << "#include \"server_common_protocol.h\"" << std::endl;
        std::cout << "#include \"logic_record_sql_table.h\"" << std::endl;
        std::cout << "#include \"logic_record_sql_table_cache.h\"" << std::endl;
        PrintEnter();

        std::cout << "struct " << m_strCacheTypeName << ";" << std::endl;
        std::cout << "struct " << m_strCacheTypeNameValueType << ";" << std::endl;

        auto stUserClassInfoBeg = m_stUserClassInfo.begin();
        auto stUserClassInfoEnd = m_stUserClassInfo.end();
        for ( ; stUserClassInfoBeg != stUserClassInfoEnd; ++stUserClassInfoBeg)
        {
            std::cout << stUserClassInfoBeg->second << std::endl;
        }

        PrintEnter();

        // class begin
        std::cout << "class " << m_strClassName << " : public CLogicSQLTableRecordBase\n{\npublic:" << std::endl;
        // constructor
        PrintTabSpace(1); std::cout << m_strClassName << "() : m_pCache(nullptr) { }" << std::endl;
        PrintEnter();
        // virtual destory
        PrintTabSpace(1); std::cout << "virtual ~" << m_strClassName << "();" << std::endl;
        // virtual MakeInsertSQL, MakeUpdateSQL MakeDeleteSQL
        PrintEnter();
        PrintTabSpace(1); std::cout << "virtual std::string MakeInsertSQL();" << std::endl;
        PrintTabSpace(1); std::cout << "virtual std::string MakeUpdateSQL();" << std::endl;
        PrintTabSpace(1); std::cout << "virtual std::string MakeDeleteSQL();" << std::endl;
        PrintEnter();

		//Get primary key
		bool hasUid = false, hasGroup = false;
		for (size_t i = 0; i < m_stPrimaryKey.size(); ++i)
		{
			if (m_stPrimaryKey[i]->m_strBaseName.compare("Uid") == 0)	hasUid = true;
			if (m_stPrimaryKey[i]->m_strBaseName.compare("GroupID") == 0)	hasGroup = true;
			PrintTabSpace(1); std::cout << "virtual " << m_stPrimaryKey[i]->m_strType << " Get" << m_stPrimaryKey[i]->m_strBaseName << "();" << std::endl;
		}
	    if (!hasUid)
	    {
			PrintTabSpace(1); std::cout << "virtual int GetUid() { return 0; }" << std::endl;
	    }
		if (!hasGroup)
		{
			PrintTabSpace(1); std::cout << "virtual int GetGroupID() { return 0; }" << std::endl;
		}

        PrintEnter();
        if (m_bGlobal)
        {
            PrintTabSpace(1); std::cout << "static std::vector<" << m_strCacheTypeNameValueType << "> SelectDataFromSQL();" << std::endl;
        }
        else
        {
			PrintTabSpace(1); std::cout << "static std::vector<" << m_strCacheTypeNameValueType << "> SelectDataFromSQL(" << m_strWhereParam << ");" << std::endl;
        }
        
		//select data from sql with index
		for (size_t i = 0; i < m_stIndexKey.size(); ++i)
		{
            if (m_stIndexKey[i]->m_bIsUnsigned)
            {
                PrintTabSpace(1); std::cout << "static std::vector<" << m_strCacheTypeNameValueType << "> SelectDataFromSQLWith" << m_stIndexKey[i]->m_strBaseName << "(uint64_t i" << m_stIndexKey[i]->m_strBaseName << ");" << std::endl;
            } else
            {
                PrintTabSpace(1); std::cout << "static std::vector<" << m_strCacheTypeNameValueType << "> SelectDataFromSQLWith" << m_stIndexKey[i]->m_strBaseName << "(int i" << m_stIndexKey[i]->m_strBaseName << ");" << std::endl;
            }
		}

		PrintEnter();

        // div_num
        if (!m_strDivVarName.empty())
        {
            PrintTabSpace(1);
            std::cout << "static const int TABLE_DIV;" << std::endl;
        }

        // table name const string
        PrintTabSpace(1); std::cout << "static const char* s_strTableName;" << std::endl;

        // field name const string
        for (size_t i = 0; i < m_stFieldInfo.size(); ++i)
        {
            PrintTabSpace(1); std::cout << "static const char* " << m_stFieldInfo[i]->m_strConstString << ";" << std::endl;
        }
        for (size_t i = 0; i < m_stPrimaryKey.size(); ++i)
        {
            PrintTabSpace(1); std::cout << "static const char* " << m_stPrimaryKey[i]->m_strConstString << ";" << std::endl;
        }

        PrintEnter();
        PrintTabSpace(1); std::cout << m_strCacheTypeName << "* m_pCache;" << std::endl;
        PrintEnter();

        // set function
        for (size_t i = 0; i < m_stFieldInfo.size(); ++i)
        {
            m_stFieldInfo[i]->MakeSetFunction();
        }

        // SetPrimary Key;
        PrintTabSpace(1); std::cout << "void SetPrimaryKey(" << m_stPrimaryKey[0]->m_strType << " " << m_stPrimaryKey[0]->m_strTypeBaseName;
        for (size_t i = 1; i < m_stPrimaryKey.size(); ++i)
        {
            std::cout << ", " << m_stPrimaryKey[i]->m_strType << " " << m_stPrimaryKey[i]->m_strTypeBaseName;
        }
        std::cout << ")" << std::endl;
        PrintTabSpace(1);  std::cout << "{" << std::endl;
        for (size_t i = 0; i < m_stPrimaryKey.size(); ++i)
        {
            PrintTabSpace(2); std::cout << m_stPrimaryKey[i]->m_strVarName << " = " << m_stPrimaryKey[i]->m_strTypeBaseName << ";" << std::endl;

        }
        PrintTabSpace(1);  std::cout << "}" << std::endl;

        // set cache point
        PrintTabSpace(1); std::cout << "void SetCachePtr(" << m_strCacheTypeName << "* pCache)" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "m_pCache = pCache;" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;

        // set operation
        /* 
        PrintTabSpace(1); std::cout << "void SetOperation(int iOperation)" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "m_iOperation = iOperation;" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;
        */

        // private: PrimaryKey
        std::cout << "private:" << std::endl;
        for (size_t i = 0; i < m_stPrimaryKey.size(); ++i)
        {
            PrintTabSpace(1); std::cout << m_stPrimaryKey[i]->m_strType << " " << m_stPrimaryKey[i]->m_strVarName << ";" << std::endl;
        }

        // class end
        std::cout << "};" << std::endl;

        PrintEnter();
        std::cout << "struct " << m_strCacheTypeNameValueType << std::endl;
        std::cout << "{" << std::endl;

        PrintTabSpace(1); std::cout << m_strCacheTypeNameValueType << "()" << std::endl;
		PrintTabSpace(1); std::cout << ": " << m_stPrimaryKey[0]->MakeDefaultValue() << std::endl;
        for (size_t i = 1; i < m_stPrimaryKey.size(); ++i)
        {
			PrintTabSpace(1);std::cout << ", "  << m_stPrimaryKey[i]->MakeDefaultValue() << std::endl;
        }

        for (size_t i = 0; i < m_stFieldInfo.size(); ++i)
        {
            PrintTabSpace(1);std::cout << ", " << m_stFieldInfo[i]->MakeDefaultValue() << std::endl;
        }
		
		PrintTabSpace(1);
		std::cout << "{}" << std::endl;

        PrintEnter();

        for (size_t i = 0; i < m_stPrimaryKey.size(); ++i)
        {
            PrintTabSpace(1); std::cout << m_stPrimaryKey[i]->m_strType;
            int iLen = MEMBER_SPACE_NUM - m_stPrimaryKey[i]->m_strType.length();
            if (iLen <= 0) iLen = 4;
            PrintSpace(iLen);
            std::cout << m_stPrimaryKey[i]->m_strVarName << ";" << std::endl;
        }

        for (size_t i = 0; i < m_stFieldInfo.size(); ++i)
        {
            PrintTabSpace(1); std::cout << m_stFieldInfo[i]->m_strType;
            int iLen = MEMBER_SPACE_NUM - m_stFieldInfo[i]->m_strType.length();
            if (iLen <= 0) iLen = 4;
            PrintSpace(iLen);
            std::cout << m_stFieldInfo[i]->m_strVarName << ";" << std::endl;
        }
        std::cout << "};" << std::endl;

        PrintEnter();

        std::cout << "struct " << m_strCacheTypeName << std::endl;
        std::cout << "{" << std::endl;
        PrintTabSpace(1); std::cout << m_strCacheTypeName << "() : m_pRecord(nullptr) { }" << std::endl;

        PrintTabSpace(1); std::cout << "~" << m_strCacheTypeName << "()" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;

        PrintEnter();
       
        //InitializeWithoutSQL
        PrintTabSpace(1); std::cout << "void InitializeWithoutSQL(const " << m_strCacheTypeNameValueType << "& stData)" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "m_stData = stData;" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;


        // Initialize
        PrintTabSpace(1); std::cout << "void Initialize(const " << m_strCacheTypeNameValueType << "& stData)" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "if (m_pRecord == nullptr)" << std::endl;
        PrintTabSpace(3); std::cout << "m_pRecord = GetRecord();" << std::endl;
        PrintTabSpace(2); std::cout << "m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);" << std::endl;

        PrintTabSpace(2); std::cout << "m_pRecord->SetPrimaryKey(";

        std::cout << "stData." << m_stPrimaryKey[0]->m_strVarName;
        for ( size_t i = 1; i < m_stPrimaryKey.size(); ++i )
        {
            std::cout << ", stData." << m_stPrimaryKey[i]->m_strVarName;
        }
        std::cout << ");" << std::endl;

        for (size_t i = 0; i < m_stFieldInfo.size(); ++i)
        {
            PrintTabSpace(2); std::cout << "m_pRecord->Set" << m_stFieldInfo[i]->m_strBaseName << "(stData." << m_stFieldInfo[i]->m_strVarName << ");" << std::endl;
        }

        PrintTabSpace(2); std::cout << "m_stData = stData;" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;

        // clear from sql
        PrintTabSpace(1); std::cout << "void ClearFromSQL()" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "if (m_pRecord == nullptr)" << std::endl;
        PrintTabSpace(3); std::cout << "m_pRecord = GetRecord();" << std::endl;
        PrintTabSpace(2); std::cout << "m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;

        // set get add function
        for (size_t i = 0; i < m_stFieldInfo.size(); ++i)
        {
            m_stFieldInfo[i]->MakeCacheGetFunction();
            m_stFieldInfo[i]->MakeCacheAddFunction();
            m_stFieldInfo[i]->MakeCacheSetFunction();
        }
        // member
        PrintEnter();        
		PrintTabSpace(1); std::cout << m_strClassName << "* m_pRecord;" << std::endl;
        PrintTabSpace(1); std::cout << m_strCacheTypeNameValueType << " m_stData;" << std::endl;

		PrintEnter();
		std::cout << "private:" << std::endl;
		// HasRecord
		PrintTabSpace(1); std::cout << "bool HasRecord()" << std::endl;
		PrintTabSpace(1); std::cout << "{" << std::endl;
		PrintTabSpace(2); std::cout << "return m_pRecord != nullptr;" << std::endl;
		PrintTabSpace(1); std::cout << "}" << std::endl;

		// GetRecord
		PrintTabSpace(1); std::cout << m_strClassName << "* GetRecord()" << std::endl;
		PrintTabSpace(1); std::cout << "{" << std::endl;
		PrintTabSpace(2); std::cout << "if(!m_pRecord)" << std::endl;
		PrintTabSpace(2); std::cout << "{" << std::endl;
		PrintTabSpace(3); std::cout << "m_pRecord = new " << m_strClassName << ";" << std::endl;
		PrintTabSpace(3); std::cout << "m_pRecord->SetPrimaryKey(";

		std::cout << "m_stData." << m_stPrimaryKey[0]->m_strVarName;
		for (size_t i = 1; i < m_stPrimaryKey.size(); ++i)
		{
			std::cout << ", m_stData." << m_stPrimaryKey[i]->m_strVarName;
		}
		std::cout << ");" << std::endl;

		PrintTabSpace(3); std::cout << "m_pRecord->SetCachePtr(this);" << std::endl;
		PrintTabSpace(3); std::cout << CGlobalConfig::s_strStorageInstance << "->m_stRecordVec.push_back(std::shared_ptr<" << m_strClassName << ">(m_pRecord));" << std::endl;
		PrintTabSpace(2); std::cout << "}" << std::endl;
		PrintTabSpace(2); std::cout << "return m_pRecord;" << std::endl;
		PrintTabSpace(1); std::cout << "}" << std::endl;
				
        std::cout << "};" << std::endl;
        PrintEnter();
        // std::cout << "#endif" << std::endl;

        dup2(fd, 1);
    }

    void GenerateCpp(const std::string& strSrcPath) 
    {
        int fd = dup(1);
        freopen((strSrcPath + m_strFileName + ".cpp").c_str(), "w", stdout);
        std::cout << "#include \""<< m_strFileName << ".h\"" << std::endl;
        PrintEnter();
        std::cout << "#include <cassert>" << std::endl;
        PrintEnter();
        // TABLE_DIV
        if (!m_strDivVarName.empty())
        {
            std::cout << "const int " << m_strClassName << "::TABLE_DIV = " << m_iDivNum << ";" << std::endl;
        }

        PrintEnter();
        
        // static const char*
        for (size_t i = 0; i < m_stFieldInfo.size(); ++i )
        {
            std::cout << "const char* " << m_strClassName << "::" << m_stFieldInfo[i]->m_strConstString << " = \"" << m_stFieldInfo[i]->m_strFieldName << "\";" << std::endl;
        }

        PrintEnter();

        std::cout << m_strClassName << "::~" << m_strClassName << "()\n{" << std::endl; 
        PrintTabSpace(1); std::cout << "if (m_pCache != nullptr)" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "m_pCache->m_pRecord = nullptr;" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;
        std::cout << "}" << std::endl;

        PrintEnter();

        MakeInsertSQLFunction();
        MakeUpdateSQLFunction();
        MakeDeleteSQLFunction();

       
		//Get primary key
		for (size_t i = 0; i < m_stPrimaryKey.size(); ++i)
		{
			PrintEnter();
            if (!m_stPrimaryKey[i]->m_bIsUnsigned)
            {
                std::cout << "int" << std::endl;
            } else
            {
                std::cout << "uint64_t" << std::endl;
            }
			std::cout << m_strClassName << "::Get" << m_stPrimaryKey[i]->m_strBaseName << "()\n{" << std::endl;
			PrintTabSpace(1); std::cout << "return " << m_stPrimaryKey[i]->m_strVarName << ";" << std::endl;
			std::cout << "}" << std::endl;
		}

		PrintEnter();
        std::cout << "std::vector<" << m_strCacheTypeNameValueType << ">" << std::endl;
        if (m_bGlobal)
        {
            std::cout << m_strClassName << "::SelectDataFromSQL()" << std::endl;
        }
        else
        {
            std::cout << m_strClassName << "::SelectDataFromSQL(" << m_strWhereParam << ")" << std::endl;
        }
        std::cout << "{" << std::endl;
        
        if (!m_strDivVarName.empty())
        {
            PrintTabSpace(1); std::cout << "int iTableMod = iUin % TABLE_DIV;" << std::endl;
        }

        PrintTabSpace(1); std::cout << "char szBuffer[1024];" << std::endl;
        if (m_bGlobal)
        {
            PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"SELECT * FROM " << m_strTableName << ";\");" << std::endl;
        }
        else if (!m_strDivVarName.empty())
        {
            PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"SELECT * FROM " << m_strTableName << "_%d WHERE " << m_strWhereSelect << ";\" , iTableMod, " << m_strWhereQuery << ");" << std::endl;
        }
        else
        {
            PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"SELECT * FROM " << m_strTableName << " WHERE " << m_strWhereSelect << ";\" , " << m_strWhereQuery << ");" << std::endl;
        }
        PrintTabSpace(1); std::cout << "std::vector<" << m_strCacheTypeNameValueType << "> stRet;" << std::endl;
        PrintTabSpace(1); std::cout << m_strCacheTypeNameValueType << " stValue;" << std::endl;
        PrintEnter();
        PrintTabSpace(1); std::cout << "try {" << std::endl;
        PrintTabSpace(2); std::cout << "taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);" << std::endl;
        PrintTabSpace(2); std::cout << "for (size_t i = 0; i < stData.size(); ++i)" << std::endl;
        PrintTabSpace(2); std::cout << "{" << std::endl;

        for ( size_t i = 0; i < m_stPrimaryKey.size(); ++i )
        {
            PrintTabSpace(3); std::cout << m_stPrimaryKey[i]->MakeGetDataFromSQL() << std::endl;
        }

        for ( size_t i = 0; i < m_stFieldInfo.size(); ++i )
        {
            PrintTabSpace(3); std::cout << m_stFieldInfo[i]->MakeGetDataFromSQL() << std::endl;
        }
        PrintTabSpace(3); std::cout << "stRet.push_back(stValue);" << std::endl;

        PrintTabSpace(2); std::cout << "}" << std::endl;

        PrintTabSpace(1); std::cout << "} catch(const taf::TC_Mysql_Exception& stEx) {" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;
        PrintTabSpace(1); std::cout << "return stRet;" << std::endl;
        std::cout << "}" << std::endl;

		//select data from sql with index
		for (size_t i = 0; i < m_stIndexKey.size(); ++i)
		{
			PrintEnter(1);
			std::cout << "std::vector<" << m_strCacheTypeNameValueType << ">" << std::endl;
			std::cout << m_strClassName << "::SelectDataFromSQLWith" << m_stIndexKey[i]->m_strBaseName << "(int i" << m_stIndexKey[i]->m_strBaseName << ")" << std::endl;
			std::cout << "{" << std::endl;

			PrintTabSpace(1); std::cout << "char szBuffer[1024];" << std::endl;

			if (!m_strDivVarName.empty())
			{
				PrintTabSpace(1); std::cout << "int iTableMod = iUin % TABLE_DIV;" << std::endl;
				PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"SELECT * FROM " << m_strTableName << "_%d WHERE " << m_stIndexKey[i]->m_strFieldName  << "=%d;\" , iTableMod, i" << m_stIndexKey[i]->m_strBaseName << ");" << std::endl;
			}
			else
			{
				PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"SELECT * FROM " << m_strTableName << " WHERE " << m_stIndexKey[i]->m_strFieldName << "=%d;\" , i" << m_stIndexKey[i]->m_strBaseName << ");" << std::endl;
			}

			PrintTabSpace(1); std::cout << "std::vector<" << m_strCacheTypeNameValueType << "> stRet;" << std::endl;
			PrintTabSpace(1); std::cout << m_strCacheTypeNameValueType << " stValue;" << std::endl;
			PrintEnter();
			PrintTabSpace(1); std::cout << "try {" << std::endl;
			PrintTabSpace(2); std::cout << "taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);" << std::endl;
			PrintTabSpace(2); std::cout << "for (size_t i = 0; i < stData.size(); ++i)" << std::endl;
			PrintTabSpace(2); std::cout << "{" << std::endl;

			for (size_t j = 0; j < m_stPrimaryKey.size(); ++j)
			{
				PrintTabSpace(3); std::cout << m_stPrimaryKey[j]->MakeGetDataFromSQL() << std::endl;
			}

			for (size_t j = 0; j < m_stFieldInfo.size(); ++j)
			{
				PrintTabSpace(3); std::cout << m_stFieldInfo[j]->MakeGetDataFromSQL() << std::endl;
			}
			PrintTabSpace(3); std::cout << "stRet.push_back(stValue);" << std::endl;

			PrintTabSpace(2); std::cout << "}" << std::endl;

			PrintTabSpace(1); std::cout << "} catch(const taf::TC_Mysql_Exception& stEx) {" << std::endl;
			PrintTabSpace(1); std::cout << "}" << std::endl;
			PrintTabSpace(1); std::cout << "return stRet;" << std::endl;
			std::cout << "}" << std::endl;
		}
        
        dup2(fd, 1);
    }

    void GenerateCreateSQLBash(const std::string& strSrcPath)
    {
        int fd = dup(1);
        freopen((strSrcPath + m_strTableName + ".sh").c_str(), "w", stdout);
        PrintTabSpace(0); std::cout << "TABLE_NAME=\"" << m_strTableName << "\"" << std::endl;

        int iLevel = 0;
        if (!m_strDivVarName.empty())
        {
            PrintTabSpace(iLevel); std::cout << "for((i=0;i<${TABLE_NUM};i++))" << std::endl;
            PrintTabSpace(iLevel); std::cout << "do" << std::endl;
            ++iLevel;
        }

        PrintTabSpace(iLevel); std::cout << "sql=\"" << std::endl;
        if (!m_strDivVarName.empty())
        {
            PrintTabSpace(iLevel); std::cout << "CREATE TABLE IF NOT EXISTS ${TABLE_NAME}${CONNECTOR_SIGN}${i}" << std::endl;
        }
        else
        {
            PrintTabSpace(iLevel); std::cout << "CREATE TABLE IF NOT EXISTS ${TABLE_NAME}" << std::endl;
        }

        PrintTabSpace(iLevel); std::cout << "(" << std::endl;

        for ( size_t i = 0; i < m_stPrimaryKey.size(); ++i )
        {
            PrintTabSpace(iLevel + 1); std::cout << m_stPrimaryKey[i]->MakeBashColumn() << "," << std::endl;
        }

        for ( size_t i = 0; i < m_stFieldInfo.size(); ++i )
        {
            PrintTabSpace(iLevel + 1); std::cout << m_stFieldInfo[i]->MakeBashColumn() << "," << std::endl;
        }

        PrintTabSpace(iLevel + 1); std::cout << "PRIMARY KEY(";

        std::string strPrimaryKey;
        for ( size_t i = 0; i < m_stPrimaryKey.size(); ++i )
        {
            strPrimaryKey += m_stPrimaryKey[i]->m_strFieldName + ",";
        }
        strPrimaryKey[strPrimaryKey.length() - 1] = ')';
        std::cout << strPrimaryKey;

        if (!m_stUniqueString.empty())
        {
            std::cout << ", UNIQUE(";
            std::string strUniqueString;
            for (size_t i = 0; i < m_stUniqueString.size(); ++i)
            {
                strUniqueString += m_stUniqueString[i] + ",";
            }
            strUniqueString[strUniqueString.length() - 1] = ')';
            std::cout << strUniqueString;
        }

		for (size_t i = 0; i < m_stIndexKey.size(); ++i)
		{
			std::cout << ", INDEX(" << m_stIndexKey[i]->m_strFieldName << ")";
		}

        PrintEnter();

        PrintTabSpace(iLevel); std::cout << ")ENGINE = " << m_strStorageEngine << ", CHARSET = " << m_strCharset;

        if (m_iAutoIncrement > 1)
        {
            std::cout << ", AUTO_INCREMENT = " << m_iAutoIncrement;
        }

        std::cout << ";" << std::endl;

        PrintTabSpace(iLevel); std::cout << "\"" << std::endl;
        PrintTabSpace(iLevel); std::cout << "echo ${sql} | ${MYSQL_AUTH} ${DATABASE}" << std::endl;
        PrintTabSpace(iLevel); std::cout << "if [ $? -ne 0 ]" << std::endl;
        PrintTabSpace(iLevel); std::cout << "then" << std::endl;
        PrintTabSpace(iLevel + 1); std::cout << "echo \"create table ${TABLE_NAME} failed!!!\"" << std::endl;
        PrintTabSpace(iLevel + 1); std::cout << "exit 1" << std::endl;
        PrintTabSpace(iLevel); std::cout << "fi" << std::endl;

        if (!m_strDivVarName.empty())
            std::cout << "done" << std::endl;
        std::cout << "echo \"create table ${TABLE_NAME} success!!!\"" << std::endl;

        dup2(fd, 1);
    }

    void Data2InsertString()
    {
        PrintTabSpace(1); std::cout << "auto stIntDataITBeg = m_stIntData.begin();" <<std::endl;
        PrintTabSpace(1); std::cout << "auto stIntDataITEnd = m_stIntData.end();" <<std::endl;
        PrintTabSpace(1); std::cout << "for(; stIntDataITBeg != stIntDataITEnd; ++stIntDataITBeg)" <<std::endl;
        PrintTabSpace(1); std::cout << "{" <<std::endl;
        PrintTabSpace(2); std::cout << "strCol += stIntDataITBeg->first + std::string(\",\");" <<std::endl;
        PrintTabSpace(2); std::cout << "strVal += Int2DBString(stIntDataITBeg->second) + \",\";" <<std::endl;
        PrintTabSpace(1); std::cout << "}" <<std::endl;

        PrintTabSpace(1); std::cout << "auto stUInt64DataITBeg = m_stUInt64Data.begin();" <<std::endl;
        PrintTabSpace(1); std::cout << "auto stUInt64DataITEnd = m_stUInt64Data.end();" <<std::endl;
        PrintTabSpace(1); std::cout << "for(; stUInt64DataITBeg != stUInt64DataITEnd; ++stUInt64DataITBeg)" <<std::endl;
        PrintTabSpace(1); std::cout << "{" <<std::endl;
        PrintTabSpace(2); std::cout << "strCol += stUInt64DataITBeg->first + std::string(\",\");" <<std::endl;
        PrintTabSpace(2); std::cout << "strVal += Uint642DBString(stUInt64DataITBeg->second.m_ulVal) + \",\";" <<std::endl;
        PrintTabSpace(1); std::cout << "}" <<std::endl;

        PrintTabSpace(1); std::cout << "auto stStringDataITBeg = m_stStringData.begin();" <<std::endl;
        PrintTabSpace(1); std::cout << "auto stStringDataITEnd = m_stStringData.end();" <<std::endl;
        PrintTabSpace(1); std::cout << "for(; stStringDataITBeg != stStringDataITEnd; ++stStringDataITBeg)" <<std::endl;
        PrintTabSpace(1); std::cout << "{" <<std::endl;
        PrintTabSpace(2); std::cout << "strCol += stStringDataITBeg->first + std::string(\",\");" <<std::endl;
        PrintTabSpace(2); std::cout << "strVal += String2DBString(stStringDataITBeg->second) + \",\";" <<std::endl;
        PrintTabSpace(1); std::cout << "}" <<std::endl;

        PrintTabSpace(1); std::cout << "auto stVarDataITBeg = m_stVarData.begin();" <<std::endl;
        PrintTabSpace(1); std::cout << "auto stVarDataITEnd = m_stVarData.end();" <<std::endl;
        PrintTabSpace(1); std::cout << "for(; stVarDataITBeg != stVarDataITEnd; ++stVarDataITBeg)" <<std::endl;
        PrintTabSpace(1); std::cout << "{" <<std::endl;
        PrintTabSpace(2); std::cout << "strCol += stVarDataITBeg->first + std::string(\",\");" <<std::endl;
        PrintTabSpace(2); std::cout << "strVal += Var2DBString(stVarDataITBeg->second) + \",\";" <<std::endl;
        PrintTabSpace(1); std::cout << "}" <<std::endl;

        PrintTabSpace(1); std::cout << "auto stCharDataITBeg = m_stCharData.begin();" <<std::endl;
        PrintTabSpace(1); std::cout << "auto stCharDataITEnd = m_stCharData.end();" <<std::endl;
        PrintTabSpace(1); std::cout << "for(; stCharDataITBeg != stCharDataITEnd; ++stCharDataITBeg)" <<std::endl;
        PrintTabSpace(1); std::cout << "{" <<std::endl;
        PrintTabSpace(2); std::cout << "strCol += stCharDataITBeg->first + std::string(\",\");" <<std::endl;
        PrintTabSpace(2); std::cout << "strVal += Char2DBString(stCharDataITBeg->second) + \",\";" <<std::endl;
        PrintTabSpace(1); std::cout << "}" <<std::endl;

        PrintTabSpace(1); std::cout << "assert(!strCol.empty() && !strVal.empty());" <<std::endl;
        PrintTabSpace(1); std::cout << "strCol[strCol.size() - 1] = ')';" <<std::endl;
        PrintTabSpace(1); std::cout << "strVal[strVal.size() - 1] = ')';" <<std::endl;
    }

    void Data2UpdateString()
    {
        PrintTabSpace(1); std::cout << "std::string strSet;" << std::endl;

        PrintTabSpace(1); std::cout << "auto stIntDataITBeg = m_stIntData.begin();" << std::endl;
        PrintTabSpace(1); std::cout << "auto stIntDataITEnd = m_stIntData.end();" << std::endl;
        PrintTabSpace(1); std::cout << "for(; stIntDataITBeg != stIntDataITEnd; ++stIntDataITBeg)" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "strSet += std::string(stIntDataITBeg->first) + \"=\" + std::string(stIntDataITBeg->first) + \"+\" + Int2DBString(stIntDataITBeg->second);" << std::endl;
        PrintTabSpace(2); std::cout << "strSet.push_back(',');" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;

        PrintTabSpace(1); std::cout << "auto stUInt64DataITBeg = m_stUInt64Data.begin();" << std::endl;
        PrintTabSpace(1); std::cout << "auto stUInt64DataITEnd = m_stUInt64Data.end();" << std::endl;
        PrintTabSpace(1); std::cout << "for(; stUInt64DataITBeg != stUInt64DataITEnd; ++stUInt64DataITBeg)" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "strSet += std::string(stUInt64DataITBeg->first) + \"=\" + std::string(stUInt64DataITBeg->first) + std::string(1, stUInt64DataITBeg->second.m_cOperator) + Uint642DBString(stUInt64DataITBeg->second.m_ulVal);" << std::endl;
        PrintTabSpace(2); std::cout << "strSet.push_back(',');" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;

        PrintTabSpace(1); std::cout << "auto stStringDataITBeg = m_stStringData.begin();" << std::endl;
        PrintTabSpace(1); std::cout << "auto stStringDataITEnd = m_stStringData.end();" << std::endl;
        PrintTabSpace(1); std::cout << "for(; stStringDataITBeg != stStringDataITEnd; ++stStringDataITBeg)" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "strSet += std::string(stStringDataITBeg->first) + \"=\" + String2DBString(stStringDataITBeg->second);" << std::endl;
        PrintTabSpace(2); std::cout << "strSet.push_back(',');" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;

        PrintTabSpace(1); std::cout << "auto stVarDataITBeg = m_stVarData.begin();" << std::endl;
        PrintTabSpace(1); std::cout << "auto stVarDataITEnd = m_stVarData.end();" << std::endl;
        PrintTabSpace(1); std::cout << "for(; stVarDataITBeg != stVarDataITEnd; ++stVarDataITBeg)" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "strSet += std::string(stVarDataITBeg->first) + \"=\" + Var2DBString(stVarDataITBeg->second);" << std::endl;
        PrintTabSpace(2); std::cout << "strSet.push_back(',');" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;

        PrintTabSpace(1); std::cout << "auto stCharDataITBeg = m_stCharData.begin();" << std::endl;
        PrintTabSpace(1); std::cout << "auto stCharDataITEnd = m_stCharData.end();" << std::endl;
        PrintTabSpace(1); std::cout << "for(; stCharDataITBeg != stCharDataITEnd; ++stCharDataITBeg)" << std::endl;
        PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "strSet += std::string(stCharDataITBeg->first) + \"=\" + std::string(stCharDataITBeg->first) + \"+\" + Char2DBString(stCharDataITBeg->second);" << std::endl;
        PrintTabSpace(2); std::cout << "strSet.push_back(',');" << std::endl;
        PrintTabSpace(1); std::cout << "}" << std::endl;
        PrintTabSpace(1); std::cout << "assert(!strSet.empty());" << std::endl;
        PrintTabSpace(1); std::cout << "strSet[strSet.size() - 1] = ' ';" << std::endl;
    }

    //bad
    void MakeInsertSQLFunction()
    {
        std::cout << "std::string" << std::endl;
        std::cout << m_strClassName << "::MakeInsertSQL()" << std::endl;
        std::cout << "{" << std::endl;

        if (!m_strDivVarName.empty())
        {
            PrintTabSpace(1); std::cout << "int iTableMod = "<< m_strDivVarName << " % TABLE_DIV;" << std::endl;
        }

        PrintTabSpace(1); std::cout << "std::string strCol = \"(\";" << std::endl;
        PrintTabSpace(1); std::cout << "std::string strVal = \"(\";" << std::endl;

        PrintEnter();

        for(size_t i = 0; i < m_stPrimaryKey.size(); ++i)
        {
            if (-1 == m_stPrimaryKey[i]->m_iIncrement)
            {
                PrintTabSpace(1); std::cout << "strCol += \"" << m_stPrimaryKey[i]->m_strFieldName << "\" + std::string(\",\");" << std::endl;
                PrintTabSpace(1); std::cout << "strVal += Int2DBString(" << m_stPrimaryKey[i]->m_strVarName << ") + \",\";" << std::endl;
            }
        }
        PrintEnter();

        Data2InsertString();

        PrintTabSpace(1); std::cout << "char szBuffer[12480];" << std::endl;

        if (!m_strDivVarName.empty())
        {
            PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"INSERT INTO " << m_strTableName << "_%d %s VALUES %s;\" , iTableMod, strCol.c_str(), strVal.c_str());" << std::endl;
        }
        else
        {
            PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"INSERT INTO " << m_strTableName << " %s VALUES %s;\" , strCol.c_str(), strVal.c_str());" << std::endl;
        }

        PrintTabSpace(1); std::cout << "return szBuffer;" <<std::endl;
        std::cout << "}" << std::endl;
    }

    void MakeUpdateSQLFunction()
    {
        std::cout << "std::string" << std::endl;
        std::cout << m_strClassName << "::MakeUpdateSQL()" << std::endl;
        std::cout << "{" << std::endl;

        if (!m_strDivVarName.empty())
        {
            PrintTabSpace(1); std::cout << "int iTableMod = "<< m_strDivVarName << " % TABLE_DIV;" << std::endl;
        }

        Data2UpdateString();

        PrintTabSpace(1); std::cout << "char szBuffer[12480];" << std::endl;
        if (!m_strDivVarName.empty())
        {
            PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"UPDATE " << m_strTableName << "_%d ";
        }
        else
        {
            PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"UPDATE " << m_strTableName << " ";
        }
                
        std::cout << "SET %s WHERE ";
        if (m_stPrimaryKey[0]->m_bIsUnsigned)
        {
            std::cout << m_stPrimaryKey[0]->m_strFieldName << "=%ld";
        } else
        {
            std::cout << m_stPrimaryKey[0]->m_strFieldName << "=%d";
        }
        for (size_t i = 1; i < m_stPrimaryKey.size(); ++i)
        {
            if (m_stPrimaryKey[i]->m_bIsUnsigned)
            {
                std::cout << " AND " << m_stPrimaryKey[i]->m_strFieldName << "=%ld";
            } else
            {
                std::cout << " AND " << m_stPrimaryKey[i]->m_strFieldName << "=%d";
            }
        }
        std::cout << ";\"";
        if (!m_strDivVarName.empty())
            std::cout << ", iTableMod";
        std::cout << ", strSet.c_str()";

        for (size_t i = 0; i < m_stPrimaryKey.size(); ++i)
        {
            std::cout << ", " << m_stPrimaryKey[i]->m_strVarName;
        }
        std::cout << ");" << std::endl;

        PrintTabSpace(1); std::cout << "return szBuffer;" <<std::endl;
        std::cout << "}" << std::endl;
    }

    void MakeDeleteSQLFunction()
    {
        std::cout << "std::string" << std::endl;
        std::cout << m_strClassName << "::MakeDeleteSQL()" << std::endl;
        std::cout << "{" << std::endl;

        if (!m_strDivVarName.empty())
        {
            PrintTabSpace(1); std::cout << "int iTableMod = "<< m_strDivVarName << " % TABLE_DIV;" << std::endl;
        }

        PrintTabSpace(1); std::cout << "char szBuffer[1024];" << std::endl;
        if (!m_strDivVarName.empty())
        {
            PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"DELETE FROM " << m_strTableName << "_%d";
        }
        else
        {
            PrintTabSpace(1); std::cout << "snprintf(szBuffer, sizeof(szBuffer), \"DELETE FROM " << m_strTableName;
        }
                
        std::cout << " WHERE ";
        if (m_stPrimaryKey[0]->m_bIsUnsigned)
        {
            std::cout << m_stPrimaryKey[0]->m_strFieldName << "=%ld";
        } else
        {
            std::cout << m_stPrimaryKey[0]->m_strFieldName << "=%d";
        }
        for (size_t i = 1; i < m_stPrimaryKey.size(); ++i)
        {
            if (m_stPrimaryKey[i]->m_bIsUnsigned)
            {
                std::cout << " AND " << m_stPrimaryKey[i]->m_strFieldName << "=%ld";
            } else
            {
                std::cout << " AND " << m_stPrimaryKey[i]->m_strFieldName << "=%d";
            }
        }
        std::cout << ";\"";
        if (!m_strDivVarName.empty())
            std::cout << ", iTableMod";

        for (size_t i = 0; i < m_stPrimaryKey.size(); ++i)
        {
            std::cout << ", " << m_stPrimaryKey[i]->m_strVarName;
        }
        std::cout << ");" << std::endl;

        PrintTabSpace(1); std::cout << "return szBuffer;" <<std::endl;
        std::cout << "}" << std::endl;
    }

};

std::vector<std::string> ListXmlConfig(const char* szPath)
{
    std::string strFieldFile;
    std::vector<std::string> stRet;

    DIR *pDir;
    struct dirent* ent = NULL;
    pDir = opendir(szPath);
    if (pDir == NULL)
    {
        return stRet;
    }

    while (NULL != (ent = readdir(pDir)))
    {
        if (ent->d_type == 8)
        {
            std::string strFileName = ent->d_name;
            size_t uiPos = strFileName.find_last_of(".");
            if (uiPos == std::string::npos)
                continue;
            if (strFileName.substr(uiPos + 1, 3).compare("xml") != 0)
                continue;

            strFieldFile = szPath + std::string("/") + ent->d_name;
            stRet.push_back(strFieldFile);
        }
    }

    return stRet;
}

void GenerateCreateSQL(const std::string& strPath,
                       const std::vector<std::string>& stTableList,
                       const std::vector<CMysqlTableInfo>& stDBTableInfoList)
{
    std::ofstream fout;
    std::string strSQLFile = strPath + "/create_sql.sh";
    fout.open(strSQLFile, ios::trunc);
    if (!fout)
    {
        std::cout << "create file create_sql.sh failed!!!" << std::endl;
        return;
    }

    fout << "HOST=\"" << CGlobalConfig::s_strHost << "\"" << std::endl;
    fout << "PORT=\"" << CGlobalConfig::s_strPort << "\"" << std::endl;
    fout << "USER=\"" << CGlobalConfig::s_strUser << "\"" << std::endl;
    fout << "PASSWD=\"" << CGlobalConfig::s_strPasswd << "\"" << std::endl;
    fout << "DATABASE=\"" << CGlobalConfig::s_strDatabase << "\"" << std::endl;
	fout << "CHARSET=\"" << cszDefaultCharSet << "\"" << std::endl;
    fout << "MYSQL_AUTH=\"mysql -h${HOST} -P${PORT} -u${USER} -p${PASSWD} --default-character-set=${CHARSET}\"" << std::endl;
    fout << "TABLE_NUM=\"" << CGlobalConfig::s_iDiv << "\"" << std::endl;
    fout << "CONNECTOR_SIGN=\"_\"" << std::endl;
    fout << std::endl;

	fout << "if [ `mysql -P${PORT} -u${USER} -p${PASSWD} -h${HOST} -e \"show databases\"|grep -w ${DATABASE}|wc -l` == 0 ]; then" << std::endl;
	fout << "\techo \"${DATABASE} will be created!\"" << std::endl;
	fout << "else" << std::endl;
	fout << "\techo \"${DATABASE} exist!\"" << std::endl;
	fout << "\texit 1" << std::endl;
	fout << "fi" << std::endl;
	fout << std::endl;

    fout << "sql=\"DROP DATABASE IF EXISTS ${DATABASE}; CREATE DATABASE IF NOT EXISTS \\`${DATABASE}\\` DEFAULT CHARSET utf8 COLLATE utf8_general_ci;\"" << std::endl;
    fout << "echo ${sql} | ${MYSQL_AUTH}" << std::endl;
    fout << "if [ $? -ne 0 ]" << std::endl;
    fout << "then" << std::endl;
    fout << "\techo CREATE DATABASE \\`${DATABASE}\\` failed!!!" << std::endl;
    fout << "\texit 1" << std::endl;
    fout << "fi" << std::endl;
    fout << "echo \"create database ${DATABASE} success!!!\"" << std::endl;
    fout << std::endl;

    // table_info_t
    fout << "TABLE_NAME=\"table_info_t\"" << std::endl;
    fout << "sql=\"" << std::endl;
    fout << "CREATE TABLE IF NOT EXISTS ${TABLE_NAME}" << std::endl;
    fout << "(" << std::endl;
    fout << "\ttable_name char(48) NOT NULL," << std::endl;
    fout << "\ttable_div int(10) NOT NULL," << std::endl;
    fout << "\ttable_info longblob," << std::endl;
    fout << "\tPRIMARY KEY(table_name)" << std::endl;
    fout << ") ENGINE = InnoDB, CHARSET = utf8;" << std::endl;
    fout << "\"" << std::endl;

    fout << "echo ${sql} | ${MYSQL_AUTH} ${DATABASE}" << std::endl;
    fout << "if [ $? -ne 0 ]" << std::endl;
    fout << "then" << std::endl;
    fout << "\techo \"create table ${TABLE_NAME} failed!!!\"" << std::endl;
    fout << "\texit 1" << std::endl;
    fout << "fi" << std::endl;
    fout << "echo \"create table ${TABLE_NAME} success!!!\"" << std::endl;
    fout << std::endl;
    fout << "##insert table info" << std::endl;

    // insert table info
    fout << "sql=\"" << std::endl;

    std::string strSQL;
    for (size_t i = 0; i < stDBTableInfoList.size(); ++i)
    {
        strSQL += stDBTableInfoList[i].GenerateInsertSQL() + "\n";
    }

    fout << strSQL << std::endl;
    fout << "\"" << std::endl;
    fout << "echo ${sql} | ${MYSQL_AUTH} ${DATABASE}" << std::endl;
    fout << "if [ $? -ne 0 ]" << std::endl;
    fout << "then" << std::endl;
    fout << "\techo \"insert table info failed!!!\"" << std::endl;
    fout << "\texit 1" << std::endl;
    fout << "fi" << std::endl;
    fout << "echo \"insert table info success!!!\"" << std::endl;
    fout << std::endl;

    // other table
    for (size_t i = 0; i < stTableList.size(); ++i)
    {
        int iPos = stTableList[i].find_last_of('/');
        int iLength = stTableList[i].find_last_of('_') - stTableList[i].find_last_of('/');
        std::string strFile = stTableList[i].substr(iPos, iLength);
        fout << ". ." << strFile << "_t.sh" << std::endl;
    }

    chmod(strSQLFile.c_str(), (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH));
}

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout << "usage : " << argv[0] << " code global_config_dir table_xml_dir output_code_dir" << std::endl;
        std::cout << "usage : " << argv[0] << " create global_config_dir table_xml_dir" << std::endl;
        std::cout << "usage : " << argv[0] << " update global_config_dir table_xml_dir" << std::endl;
        return (0); 
    }

    std::string strCmd = std::string(argv[1]);

    if (strCmd.compare("code") == 0)
    {
        std::string strGlobalPath = std::string(argv[2]);
        std::string strXmlPath = std::string(argv[3]);
        std::string strCodePath = std::string(argv[4]);

        CGlobalConfig::Initialize(strGlobalPath);

        std::vector<std::string> stTableList = ListXmlConfig(strXmlPath.c_str());

        for (size_t i = 0; i < stTableList.size(); ++i)
        {
            CTableInfo tb;
            tb.InitFromXml(stTableList[i]);
            tb.GenerateInclude(strCodePath + "/");
            tb.GenerateCpp(strCodePath + "/");

            std::cout << "do " << stTableList[i] << std::endl;
        }
    }
    else if (strCmd.compare("create") == 0)
    {
        std::string strGlobalPath = std::string(argv[2]);
        std::string strXmlPath = std::string(argv[3]);

        CGlobalConfig::Initialize(strGlobalPath);

        std::vector<CMysqlTableInfo> stMysqlDBInfo;
        std::vector<std::string> stTableList = ListXmlConfig(strXmlPath.c_str());

        for (size_t i = 0; i < stTableList.size(); ++i)
        {
            CTableInfo tb;
            tb.InitFromXml(stTableList[i]);
            tb.GenerateCreateSQLBash(strGlobalPath + "/");

            std::cout << "create table sh " << stTableList[i] << std::endl;
        }

        for(size_t i = 0; i < stTableList.size(); ++i)
        {
            CMysqlTableInfo stTableInfo;
            stTableInfo.InitFromXml(stTableList[i]);
            stMysqlDBInfo.push_back(stTableInfo);
        }

        GenerateCreateSQL(strGlobalPath, stTableList, stMysqlDBInfo);
    }
    else if (strCmd.compare("update") == 0)
    {
        std::string strGlobalPath = std::string(argv[2]);
        std::string strXmlPath = std::string(argv[3]);

        CGlobalConfig::Initialize(strGlobalPath);
        CGlobalConfig::InitMysqlConn();

        std::vector<std::string> stTableList = ListXmlConfig(strXmlPath.c_str());

        std::map<std::string, CMysqlTableInfo> stOldMysqlDBInfo;
        std::map<std::string, CMysqlTableInfo> stNewMysqlDBInfo;

        // old mysql db info
        std::string strSQL = "SELECT * FROM table_info_t;";
        try
        {
            taf::TC_Mysql::MysqlData stData = CGlobalConfig::s_pMysqlHandle->queryRecord(strSQL);
            if (stData.size())
            {
                for (size_t i = 0; i < stData.size(); ++i)
                {
                    CMysqlTableInfo stTableInfo;

                    stTableInfo.Decode(::atoi(stData[i]["table_div"].c_str()), stData[i]["table_name"], stData[i]["table_info"]);

                    stOldMysqlDBInfo[stData[i]["table_name"]] = stTableInfo;
                }
            }
        }
        catch (const taf::TC_Mysql_Exception& stEx)
        {
            std::cout << stEx.what() << std::endl;
            return -1;
        }

        // new mysql db info
        for(size_t i = 0; i < stTableList.size(); ++i)
        {
            CMysqlTableInfo stTableInfo;

            stTableInfo.InitFromXml(stTableList[i]);

            stNewMysqlDBInfo[stTableInfo.m_strTableName] = stTableInfo;
        }

        std::vector<std::string> stSQLVec;
        auto stNewInfoIterBeg = stNewMysqlDBInfo.begin();
        auto stNewInfoIterEnd = stNewMysqlDBInfo.end();
        for ( ; stNewInfoIterBeg != stNewInfoIterEnd; ++stNewInfoIterBeg)
        {
            auto stOldIter = stOldMysqlDBInfo.find(stNewInfoIterBeg->first);
            if (stOldIter == stOldMysqlDBInfo.end())
            {
                std::vector<std::string> stRet = stNewInfoIterBeg->second.GenerateCreateSQL();
                stSQLVec.insert(stSQLVec.end(), stRet.begin(), stRet.end());
                std::string strCreateTableInfo = stNewInfoIterBeg->second.GenerateInsertSQL();
                stSQLVec.push_back(strCreateTableInfo);

                std::cout << "CREATE TABLE | TABLE_NAME: " << stNewInfoIterBeg->second.m_strTableName << std::endl;
            }
            else
            {
                std::vector<std::string> stRet = stNewInfoIterBeg->second.GenerateAlterSQL(stOldIter->second);
                stSQLVec.insert(stSQLVec.end(), stRet.begin(), stRet.end());
                if (stRet.size() > 0)
                {
                    std::string strUpdateTableInfo = stNewInfoIterBeg->second.GenerateUpdateSQL();
                    stSQLVec.push_back(strUpdateTableInfo);
                    std::cout << "UPDATE TABLE | TABLE_NAME: " << stNewInfoIterBeg->second.m_strTableName << std::endl;
                }
            }
        }

        try
        {
            for (size_t i = 0; i < stSQLVec.size(); ++i)
            {
                std::cout << stSQLVec[i] << std::endl;

                CGlobalConfig::s_pMysqlHandle->execute(stSQLVec[i]);
            }

            std::cout << "UPDATE TABLE SUCCESS !!!" << std::endl;
        }
        catch (const taf::TC_Mysql_Exception& stEx)
        {
            std::cout << stEx.what() << std::endl;
            return -1;
        }
    }
    else
    {
        std::cout << "usage : " << argv[0] << " code global_config_dir table_xml_dir output_code_dir" << std::endl;
        std::cout << "usage : " << argv[0] << " create global_config_dir table_xml_dir" << std::endl;
        std::cout << "usage : " << argv[0] << " update global_config_dir table_xml_dir" << std::endl;
        return (0); 
    }

    return (0);
}

