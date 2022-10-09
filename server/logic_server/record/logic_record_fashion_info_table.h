#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TFashionInfoTable;
struct TFashionInfoTableValueType;

class CRecordFashionInfoTable : public CLogicSQLTableRecordBase
{
public:
    CRecordFashionInfoTable() : m_pCache(nullptr) { }

    virtual ~CRecordFashionInfoTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetFashionID();

    static std::vector<TFashionInfoTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strExpiredTime;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strFashionID;

    TFashionInfoTable* m_pCache;

    void SetExpiredTime(int iExpiredTime)
    {
        m_stIntData[s_strExpiredTime] += iExpiredTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iFashionID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iFashionID = iFashionID;
    }
    void SetCachePtr(TFashionInfoTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iFashionID;
};

struct TFashionInfoTableValueType
{
    TFashionInfoTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iFashionID(0)
    , m_iExpiredTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iFashionID;
    int                     m_iExpiredTime;
};

struct TFashionInfoTable
{
    TFashionInfoTable() : m_pRecord(nullptr) { }
    ~TFashionInfoTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TFashionInfoTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TFashionInfoTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iFashionID);
        m_pRecord->SetExpiredTime(stData.m_iExpiredTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetExpiredTime() const
    {
        return m_stData.m_iExpiredTime;
    }
    void AddExpiredTime(int iExpiredTime)
    {
        if (0 == iExpiredTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExpiredTime(iExpiredTime);
        m_stData.m_iExpiredTime += iExpiredTime;
    }
    void SetExpiredTime(int iExpiredTime)
    {
        if (m_stData.m_iExpiredTime == iExpiredTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iExpiredTime - m_stData.m_iExpiredTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExpiredTime(RealAdd);
        m_stData.m_iExpiredTime = iExpiredTime;
    }

    CRecordFashionInfoTable* m_pRecord;
    TFashionInfoTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordFashionInfoTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordFashionInfoTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iFashionID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordFashionInfoTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
