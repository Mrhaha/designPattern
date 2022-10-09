#include "field_info.h"
#include "print_tool.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
////////// CFieldInfoBase

void
CFieldInfoBase::Initialize(const std::string& strType, const std::string& strFieldName)
{
    m_strType = strType;	
    m_strFieldName = strFieldName;	
    m_strBaseName = MakeBaseName();
    m_strVarName = MakeVarName();
	m_strTempName = MakeTempName();
    m_strTypeBaseName = m_strVarName.substr(m_strVarName.find_first_of('_') + 1);
    m_strConstString = "s_str" + m_strBaseName;
}

void
CFieldInfoBase::MakeCacheAddFunction()
{
    PrintTabSpace(1); std::cout << "void Add" << m_strBaseName << "(" << m_strType << " " << m_strTypeBaseName << ")" << std::endl;
    PrintTabSpace(1); std::cout << "{" << std::endl;
	PrintTabSpace(2); std::cout << "if (0 == " << m_strTypeBaseName << ")" << std::endl;
	PrintTabSpace(3); std::cout << "return;" << std::endl;
    PrintTabSpace(2); std::cout << "if (m_pRecord == nullptr)" << std::endl;
    PrintTabSpace(3); std::cout << "m_pRecord = GetRecord();" << std::endl;
    PrintTabSpace(2); std::cout << "m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);" << std::endl;
    PrintTabSpace(2); std::cout << "m_pRecord->Set" << m_strBaseName << "(" << m_strTypeBaseName << ");" << std::endl;
    PrintTabSpace(2); std::cout << "m_stData." << m_strVarName << " += " << m_strTypeBaseName << ";" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}

void
CFieldInfoBase::MakeCacheGetFunction() {
    PrintTabSpace(1); std::cout << "const " << m_strType << "& Get" << m_strBaseName << "() const" << std::endl;
    PrintTabSpace(1); std::cout << "{" << std::endl;
    PrintTabSpace(2); std::cout << "return m_stData." << m_strVarName << ";" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}

void
CFieldInfoBase::MakeCacheSetFunction()
{
    PrintTabSpace(1); std::cout << "void Set" << m_strBaseName << "(" << m_strType << " " << m_strTypeBaseName << ")" << std::endl;
    PrintTabSpace(1); std::cout << "{" << std::endl;
	PrintTabSpace(2); std::cout << "if (m_stData." << m_strVarName << " == " << m_strTypeBaseName << ")" << std::endl;
	PrintTabSpace(3); std::cout << "return;" << std::endl;
    PrintTabSpace(2); std::cout << "if (m_pRecord == nullptr)" << std::endl;
    PrintTabSpace(3); std::cout << "m_pRecord = GetRecord();" << std::endl;
    PrintTabSpace(2); std::cout << m_strType <<" RealAdd = " << m_strTypeBaseName << " - " << "m_stData." << m_strVarName << ";" << std::endl;
    PrintTabSpace(2); std::cout << "m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);" << std::endl;
    PrintTabSpace(2); std::cout << "m_pRecord->Set" << m_strBaseName << "(RealAdd);" << std::endl;
    PrintTabSpace(2); std::cout << "m_stData." << m_strVarName << " = " << m_strTypeBaseName << ";" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}

std::string
CFieldInfoBase::MakeBaseName ()
{
    std::string strRet;
    size_t beg = 0, end = 0;
    while (true)
    {
        end = m_strFieldName.find_first_of('_', beg);
        std::string strSub = m_strFieldName.substr(beg, end - beg);
        if (strSub.compare("id") == 0)
            strSub = "ID";
        else if (strSub.compare("pk") == 0)
            strSub = "PK";
        else
            strSub[0] = toupper(strSub[0]);
        strRet += strSub;
        beg = end + 1;
        if (end == std::string::npos)
            break;
    }
    return strRet;
}

////////////////////////////////////////////////////////////////////////////////
////////// CIntFieldInfo

std::string
CIntFieldInfo::MakeGetDataFromSQL()
{
    return "stValue." + m_strVarName + " = ::atoi(stData[i][\"" + m_strFieldName + "\"].c_str());";
}

std::string
CIntFieldInfo::MakeTempName()
{
	return std::string("i") + m_strBaseName;
}

std::string
CIntFieldInfo::MakeVarName()
{
    return "m_" + std::string("i") + m_strBaseName;
}

std::string
CIntFieldInfo::MakeBashColumn()
{
    std::string strRet = m_strFieldName + " int(10) ";
    if (m_bIsUnsigned) { strRet += "UNSIGNED "; }
    strRet += "NOT NULL ";

    if (-1 != m_iIncrement)
    {
        strRet += "AUTO_INCREMENT ";
    }
    else
    {
        if (m_strDefaultVal.empty())
        {
            strRet += "DEFAULT 0 ";
        }
        else
        {
            strRet += "DEFAULT " + m_strDefaultVal + " ";
        }
    }

	if (!m_strComment.empty())
	{
		strRet += " COMMENT '" + m_strComment + "' ";
	}
    return strRet;
}

std::string
CIntFieldInfo::MakeDefaultValue()
{
    if (m_strDefaultVal.empty())
    {
        return m_strVarName + "(0)";
    }
    else
    {
        return m_strVarName + "(" + m_strDefaultVal + ")";
    }
}

void
CIntFieldInfo::MakeSetFunction()
{
    PrintTabSpace(1); std::cout << "void Set" << m_strBaseName << "(" << m_strType << " " << m_strTypeBaseName << ")" << std::endl;
    PrintTabSpace(1); std::cout << "{\n";
    PrintTabSpace(2); std::cout << "m_stIntData[" << m_strConstString << "]" << " += " << m_strTypeBaseName << ";" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
////////// CCharFieldInfo

std::string
CCharFieldInfo::MakeGetDataFromSQL()
{
    return "stValue." + m_strVarName + " = (char)::atoi(stData[i][\"" + m_strFieldName + "\"].c_str());";
}

std::string
CCharFieldInfo::MakeTempName()
{
	return std::string("c") + m_strBaseName;
}

std::string
CCharFieldInfo::MakeVarName()
{
    return "m_" + std::string("c") + m_strBaseName;
}

std::string
CCharFieldInfo::MakeBashColumn()
{
    std::string strRet = m_strFieldName + " tinyint(3) ";
    if (m_bIsUnsigned) { strRet += "UNSIGNED "; }
    strRet += "NOT NULL ";

    if (-1 != m_iIncrement)
    {
        strRet += "AUTO_INCREMENT ";
    }
    else
    {
        if (m_strDefaultVal.empty())
        {
            strRet += "DEFAULT 0 ";
        }
        else
        {
            strRet += "DEFAULT " + m_strDefaultVal + " ";
        }
    }

	if (!m_strComment.empty())
	{
		strRet += " COMMENT '" + m_strComment + "' ";
	}
    return strRet;
}

std::string
CCharFieldInfo::MakeDefaultValue()
{
    if (m_strDefaultVal.empty())
    {
        return m_strVarName + "(0)";
    }
    else
    {
        return m_strVarName + "(" + m_strDefaultVal + ")";
    }
}

void
CCharFieldInfo::MakeSetFunction()
{
    PrintTabSpace(1); std::cout << "void Set" << m_strBaseName << "(" << m_strType << " " << m_strTypeBaseName << ")" << std::endl;
    PrintTabSpace(1); std::cout << "{\n";
    PrintTabSpace(2); std::cout << "m_stCharData[" << m_strConstString << "]" << " += " << m_strTypeBaseName << ";" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
////////// CUint64FieldInfo

std::string
CUint64FieldInfo::MakeGetDataFromSQL()
{
    return "stValue." + m_strVarName + " = (uint64_t)strtoull(stData[i][\"" + m_strFieldName + "\"].c_str(), nullptr, 10);";
}

std::string
CUint64FieldInfo::MakeTempName()
{
	return std::string("ul") + m_strBaseName;
}

std::string
CUint64FieldInfo::MakeVarName()
{
    return "m_" + std::string("ul") + m_strBaseName;
}

std::string
CUint64FieldInfo::MakeBashColumn()
{
    std::string strRet = m_strFieldName + " bigint(20) ";
    if (m_bIsUnsigned) { strRet += "UNSIGNED "; }
    strRet += "NOT NULL ";
    if (-1 != m_iIncrement)
    {
        strRet += "AUTO_INCREMENT ";
    }
    else
    {
        if (m_strDefaultVal.empty())
        {
            strRet += "DEFAULT 0 ";
        }
        else
        {
            strRet += "DEFAULT " + m_strDefaultVal + " ";
        }
    }

	if (!m_strComment.empty())
	{
		strRet += " COMMENT '" + m_strComment + "' ";
	}
    return strRet;
}

std::string
CUint64FieldInfo::MakeDefaultValue()
{
    if (m_strDefaultVal.empty())
    {
        return m_strVarName + "(0)";
    }
    else
    {
        return m_strVarName + "(" + m_strDefaultVal + ")";
    }
}

void
CUint64FieldInfo::MakeSetFunction()
{
    PrintTabSpace(1); std::cout << "void Set" << m_strBaseName << "(" << m_strType << " " << m_strTypeBaseName << ", char cOperator = '+')" << std::endl;
    PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "auto& stDataRef = m_stUInt64Data[" << m_strConstString << "];" << std::endl;
        PrintTabSpace(2); std::cout << "if (stDataRef.m_cOperator == cOperator)" << std::endl;
        PrintTabSpace(2); std::cout << "{" << std::endl;
            PrintTabSpace(3); std::cout << "stDataRef.m_ulVal += " << m_strTypeBaseName << ";" << std::endl;
        PrintTabSpace(2); std::cout << "}" << std::endl;
        PrintTabSpace(2); std::cout << "else" << std::endl;
        PrintTabSpace(2); std::cout << "{" << std::endl;
            PrintTabSpace(3); std::cout << "if (stDataRef.m_ulVal > " << m_strTypeBaseName << ")" << std::endl;
            PrintTabSpace(3); std::cout << "{" << std::endl;
                PrintTabSpace(4); std::cout << "stDataRef.m_ulVal -= " << m_strTypeBaseName << ";" << std::endl;
            PrintTabSpace(3); std::cout << "}" << std::endl;
            PrintTabSpace(3); std::cout << "else" << std::endl;
            PrintTabSpace(3); std::cout << "{" << std::endl;
                PrintTabSpace(4); std::cout << "stDataRef.m_cOperator = cOperator;" << std::endl;
                PrintTabSpace(4); std::cout << "stDataRef.m_ulVal = " << m_strTypeBaseName << " - stDataRef.m_ulVal;" << std::endl;
            PrintTabSpace(3); std::cout << "}" << std::endl;
        PrintTabSpace(2); std::cout << "}" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}

void
CUint64FieldInfo::MakeCacheSetFunction()
{
    PrintTabSpace(1); std::cout << "void Set" << m_strBaseName << "(" << m_strType << " " << m_strTypeBaseName << ")" << std::endl;
    PrintTabSpace(1); std::cout << "{" << std::endl;
        PrintTabSpace(2); std::cout << "if (m_pRecord == nullptr)" << std::endl;
            PrintTabSpace(3); std::cout << "m_pRecord = GetRecord();" << std::endl;
        PrintTabSpace(2); std::cout << "m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);" << std::endl;

        PrintTabSpace(2); std::cout << "if (m_stData." << m_strVarName << " > " << m_strTypeBaseName << ")" << std::endl;
        PrintTabSpace(2); std::cout << "{" << std::endl;
            PrintTabSpace(3); std::cout << "m_pRecord->Set" << m_strBaseName << "(m_stData." << m_strVarName << " - " << m_strTypeBaseName << ", '-');" << std::endl;
        PrintTabSpace(2); std::cout << "}" << std::endl;
        PrintTabSpace(2); std::cout << "else" << std::endl;
        PrintTabSpace(2); std::cout << "{" << std::endl;
            PrintTabSpace(3); std::cout << "m_pRecord->Set" << m_strBaseName << "(" << m_strTypeBaseName  << " - m_stData." << m_strVarName << ", '+');" << std::endl;
        PrintTabSpace(2); std::cout << "}" << std::endl;
        PrintTabSpace(2); std::cout << "m_stData." << m_strVarName << " = " << m_strTypeBaseName << ";" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
////////// CStringFieldInfo

std::string
CStringFieldInfo::MakeGetDataFromSQL()
{
    return "stValue." + m_strVarName + " = stData[i][\"" + m_strFieldName + "\"];";
}

std::string
CStringFieldInfo::MakeTempName()
{
	return std::string("str") + m_strBaseName;
}

std::string
CStringFieldInfo::MakeVarName()
{
    return "m_" + std::string("str") + m_strBaseName;
}

std::string
CStringFieldInfo::MakeBashColumn()
{
    std::string strRet = m_strFieldName + " varchar(" + m_strSize + ") ";
    if (m_bIsUnsigned) { strRet += "UNSIGNED "; }
    strRet += "NOT NULL ";
    if (m_strDefaultVal.empty())
    {
        strRet += "DEFAULT ''";
    }
    else
    {
        strRet += "DEFAULT " + m_strDefaultVal;
    }

	if (!m_strComment.empty())
	{
		strRet += " COMMENT '" + m_strComment + "' ";
	}
    return strRet;
}

std::string
CStringFieldInfo::MakeDefaultValue()
{
    if (m_strDefaultVal.empty())
    {
        return m_strVarName + "(\"\")";
    }
    else
    {
        return m_strVarName + "(" + m_strDefaultVal + ")";
    }
}

void
CStringFieldInfo::MakeSetFunction()
{
    PrintTabSpace(1); std::cout << "void Set" << m_strBaseName << "(const " << m_strType << "& " << m_strTypeBaseName << ")" << std::endl;
    PrintTabSpace(1); std::cout << "{\n";
    PrintTabSpace(2); std::cout << "m_stStringData[" << m_strConstString << "]" << " = " << m_strTypeBaseName << ";" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}

void
CStringFieldInfo::MakeCacheAddFunction()
{
    // string 没有 cache add 方法, 如果需要可以在这里重写
}

void 
CStringFieldInfo::MakeCacheSetFunction()
{
    PrintTabSpace(1); std::cout << "void Set" << m_strBaseName << "(const " << m_strType << "& " << m_strTypeBaseName << ")" << std::endl;
    PrintTabSpace(1); std::cout << "{" << std::endl;
    PrintTabSpace(2); std::cout << "if (m_pRecord == nullptr)" << std::endl;
    PrintTabSpace(3); std::cout << "m_pRecord = GetRecord();" << std::endl;
    PrintTabSpace(2); std::cout << "m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);" << std::endl;
    PrintTabSpace(2); std::cout << "m_pRecord->Set" << m_strBaseName << "(" << m_strTypeBaseName << ");" << std::endl;
    PrintTabSpace(2); std::cout << "m_stData." << m_strVarName << " = " << m_strTypeBaseName << ";" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
////////// CUserDataFieldInfo

std::string
CUserDataFieldInfo::MakeGetDataFromSQL()
{
    return "FormatDBBin(stData[i][\"" + m_strFieldName + "\"], stValue." + m_strVarName + ");";
}

std::string CUserDataFieldInfo::MakeTempName()
{
	return std::string("st") + m_strBaseName;
}


std::string
CUserDataFieldInfo::MakeVarName()
{
    return "m_" + std::string("st") + m_strBaseName;
}

std::string
CUserDataFieldInfo::MakeBashColumn()
{
    std::string strRet = m_strFieldName + " varbinary(" + m_strSize + ") ";
	if (!m_strComment.empty())
	{
		strRet += " COMMENT '" + m_strComment + "' ";
	}
    return strRet;
}

std::string
CUserDataFieldInfo::MakeDefaultValue()
{
    return m_strVarName + "()";
}

void
CUserDataFieldInfo::MakeSetFunction()
{
    PrintTabSpace(1); std::cout << "void Set" << m_strBaseName << "(const " << m_strType << "& " << m_strTypeBaseName << ")" << std::endl;
    PrintTabSpace(1); std::cout << "{\n";
    PrintTabSpace(2); std::cout << "msgpack::sbuffer stBuffer;" << std::endl;
    PrintTabSpace(2); std::cout << "msgpack::pack(stBuffer, " << m_strTypeBaseName << ");" << std::endl;
    PrintTabSpace(2); std::cout << "m_stVarData[" << m_strConstString << "]" << " = std::string(stBuffer.data(), stBuffer.size());" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}

void
CUserDataFieldInfo::MakeCacheAddFunction()
{
}

void 
CUserDataFieldInfo::MakeCacheSetFunction()
{
    PrintTabSpace(1); std::cout << "void Set" << m_strBaseName << "(const " << m_strType << "& " << m_strTypeBaseName << ")" << std::endl;
    PrintTabSpace(1); std::cout << "{" << std::endl;
    PrintTabSpace(2); std::cout << "if (m_pRecord == nullptr)" << std::endl;
    PrintTabSpace(3); std::cout << "m_pRecord = GetRecord();" << std::endl;
    PrintTabSpace(2); std::cout << "m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);" << std::endl;
    PrintTabSpace(2); std::cout << "m_pRecord->Set" << m_strBaseName << "(" << m_strTypeBaseName << ");" << std::endl;
    PrintTabSpace(2); std::cout << "m_stData." << m_strVarName << " = " << m_strTypeBaseName << ";" << std::endl;
    PrintTabSpace(1); std::cout << "}" << std::endl;
}


