#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGuildIDTable;
struct TGuildIDTableValueType;

class CRecordGuildIDTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGuildIDTable() : m_pCache(nullptr) { }

    virtual ~CRecordGuildIDTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetGroupID();
    virtual int GetUid() { return 0; }

    static std::vector<TGuildIDTableValueType> SelectDataFromSQL(int iGroupID);

    static const char* s_strTableName;
    static const char* s_strGuildID;
    static const char* s_strGroupID;

    TGuildIDTable* m_pCache;

    void SetGuildID(int iGuildID)
    {
        m_stIntData[s_strGuildID] += iGuildID;
    }
    void SetPrimaryKey(int iGroupID)
    {
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TGuildIDTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iGroupID;
};

struct TGuildIDTableValueType
{
    TGuildIDTableValueType()
    : m_iGroupID(0)
    , m_iGuildID(0)
    {}

    int                     m_iGroupID;
    int                     m_iGuildID;
};

struct TGuildIDTable
{
    TGuildIDTable() : m_pRecord(nullptr) { }
    ~TGuildIDTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGuildIDTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGuildIDTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iGroupID);
        m_pRecord->SetGuildID(stData.m_iGuildID);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetGuildID() const
    {
        return m_stData.m_iGuildID;
    }
    void AddGuildID(int iGuildID)
    {
        if (0 == iGuildID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildID(iGuildID);
        m_stData.m_iGuildID += iGuildID;
    }
    void SetGuildID(int iGuildID)
    {
        if (m_stData.m_iGuildID == iGuildID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGuildID - m_stData.m_iGuildID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildID(RealAdd);
        m_stData.m_iGuildID = iGuildID;
    }

    CRecordGuildIDTable* m_pRecord;
    TGuildIDTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGuildIDTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGuildIDTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGuildIDTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
