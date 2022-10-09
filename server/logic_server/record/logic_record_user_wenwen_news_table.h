#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserWenWenNewsTable;
struct TUserWenWenNewsTableValueType;

class CRecordUserWenWenNewsTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserWenWenNewsTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserWenWenNewsTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetNewsType();
    virtual int GetNewsID();

    static std::vector<TUserWenWenNewsTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strPara;
    static const char* s_strParaEx;
    static const char* s_strTime;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strNewsType;
    static const char* s_strNewsID;

    TUserWenWenNewsTable* m_pCache;

    void SetPara(int iPara)
    {
        m_stIntData[s_strPara] += iPara;
    }
    void SetParaEx(int iParaEx)
    {
        m_stIntData[s_strParaEx] += iParaEx;
    }
    void SetTime(int iTime)
    {
        m_stIntData[s_strTime] += iTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iNewsType, int iNewsID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iNewsType = iNewsType;
        m_iNewsID = iNewsID;
    }
    void SetCachePtr(TUserWenWenNewsTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iNewsType;
    int m_iNewsID;
};

struct TUserWenWenNewsTableValueType
{
    TUserWenWenNewsTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iNewsType(0)
    , m_iNewsID(0)
    , m_iPara(0)
    , m_iParaEx(0)
    , m_iTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iNewsType;
    int                     m_iNewsID;
    int                     m_iPara;
    int                     m_iParaEx;
    int                     m_iTime;
};

struct TUserWenWenNewsTable
{
    TUserWenWenNewsTable() : m_pRecord(nullptr) { }
    ~TUserWenWenNewsTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserWenWenNewsTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserWenWenNewsTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iNewsType, stData.m_iNewsID);
        m_pRecord->SetPara(stData.m_iPara);
        m_pRecord->SetParaEx(stData.m_iParaEx);
        m_pRecord->SetTime(stData.m_iTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetPara() const
    {
        return m_stData.m_iPara;
    }
    void AddPara(int iPara)
    {
        if (0 == iPara)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPara(iPara);
        m_stData.m_iPara += iPara;
    }
    void SetPara(int iPara)
    {
        if (m_stData.m_iPara == iPara)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPara - m_stData.m_iPara;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPara(RealAdd);
        m_stData.m_iPara = iPara;
    }
    const int& GetParaEx() const
    {
        return m_stData.m_iParaEx;
    }
    void AddParaEx(int iParaEx)
    {
        if (0 == iParaEx)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetParaEx(iParaEx);
        m_stData.m_iParaEx += iParaEx;
    }
    void SetParaEx(int iParaEx)
    {
        if (m_stData.m_iParaEx == iParaEx)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iParaEx - m_stData.m_iParaEx;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetParaEx(RealAdd);
        m_stData.m_iParaEx = iParaEx;
    }
    const int& GetTime() const
    {
        return m_stData.m_iTime;
    }
    void AddTime(int iTime)
    {
        if (0 == iTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTime(iTime);
        m_stData.m_iTime += iTime;
    }
    void SetTime(int iTime)
    {
        if (m_stData.m_iTime == iTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTime - m_stData.m_iTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTime(RealAdd);
        m_stData.m_iTime = iTime;
    }

    CRecordUserWenWenNewsTable* m_pRecord;
    TUserWenWenNewsTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserWenWenNewsTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserWenWenNewsTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iNewsType, m_stData.m_iNewsID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserWenWenNewsTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
