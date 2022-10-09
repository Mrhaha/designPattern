#pragma once

#include <string>

class CFieldInfoBase {
public:
    CFieldInfoBase() : m_bIsUnique(false), m_bIsUnsigned(false), m_iIncrement(-1) { }
    virtual ~CFieldInfoBase() {}

    virtual std::string MakeGetDataFromSQL() = 0;
	virtual std::string MakeTempName() = 0;
    virtual std::string MakeVarName() = 0;	
    virtual std::string MakeBashColumn() = 0;
    virtual std::string MakeDefaultValue() = 0;

    virtual void MakeSetFunction() = 0;

    virtual void MakeCacheAddFunction();
    virtual void MakeCacheSetFunction();
    virtual void MakeCacheGetFunction();

    std::string MakeBaseName(); 

    void Initialize(const std::string& strType, const std::string& strFieldName);

    bool                    m_bIsUnique;
    bool                    m_bIsUnsigned;
    int                     m_iIncrement;	
    std::string             m_strSize;
    std::string             m_strType;
	std::string				m_strTempName;
    std::string             m_strVarName;
    std::string             m_strFieldName;
    std::string             m_strBaseName;
    std::string             m_strTypeBaseName;
    std::string             m_strConstString;
    std::string             m_strUserClass;
    std::string             m_strDefaultVal;
	std::string				m_strComment;
};

class CIntFieldInfo : public CFieldInfoBase
{
public:
    virtual std::string MakeGetDataFromSQL();
	virtual std::string MakeTempName();
    virtual std::string MakeVarName();
    virtual std::string MakeBashColumn();

    virtual std::string MakeDefaultValue();

    virtual void MakeSetFunction();

    virtual ~CIntFieldInfo() {}
};

class CCharFieldInfo : public CFieldInfoBase
{
public:
    virtual std::string MakeGetDataFromSQL();
	virtual std::string MakeTempName();
    virtual std::string MakeVarName();
    virtual std::string MakeBashColumn();

    virtual std::string MakeDefaultValue();

    virtual void MakeSetFunction();

    virtual ~CCharFieldInfo() {}
};

class CUint64FieldInfo : public CFieldInfoBase
{
public:
    virtual std::string MakeGetDataFromSQL();
	virtual std::string MakeTempName();
    virtual std::string MakeVarName();
    virtual std::string MakeBashColumn();

    virtual std::string MakeDefaultValue();

    virtual void MakeSetFunction();

    virtual void MakeCacheSetFunction();

    virtual ~CUint64FieldInfo() {}
};

class CStringFieldInfo : public CFieldInfoBase
{
public:
    virtual std::string MakeGetDataFromSQL();
	virtual std::string MakeTempName();
    virtual std::string MakeVarName();
    virtual std::string MakeBashColumn();

    virtual std::string MakeDefaultValue();

    virtual void MakeSetFunction();

    virtual void MakeCacheAddFunction();
    virtual void MakeCacheSetFunction();

    virtual ~CStringFieldInfo() {}
};

class CUserDataFieldInfo : public CFieldInfoBase 
{
public:
    virtual std::string MakeGetDataFromSQL();
	virtual std::string MakeTempName();
    virtual std::string MakeVarName();	
    virtual std::string MakeBashColumn();

    virtual std::string MakeDefaultValue();

    virtual void MakeSetFunction();

    virtual void MakeCacheAddFunction();
    virtual void MakeCacheSetFunction();

    virtual ~CUserDataFieldInfo() {}
};
