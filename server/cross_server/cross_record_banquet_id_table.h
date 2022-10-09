#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TBanquetIDTable;
struct TBanquetIDTableValueType;

class CRecordBanquetIDTable : public CLogicSQLTableRecordBase
{
public:
    CRecordBanquetIDTable() : m_pCache(nullptr) { }

    virtual ~CRecordBanquetIDTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetGroupID();
    virtual int GetUid() { return 0; }

    static std::vector<TBanquetIDTableValueType> SelectDataFromSQL(int iGroupID);

    static const char* s_strTableName;
    static const char* s_strBanquetID;
    static const char* s_strGroupID;

    TBanquetIDTable* m_pCache;

    void SetBanquetID(uint64_t ulBanquetID, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strBanquetID];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulBanquetID;
        }
        else
        {
            if (stDataRef.m_ulVal > ulBanquetID)
            {
                stDataRef.m_ulVal -= ulBanquetID;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulBanquetID - stDataRef.m_ulVal;
            }
        }
    }
    void SetPrimaryKey(int iGroupID)
    {
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TBanquetIDTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iGroupID;
};

struct TBanquetIDTableValueType
{
    TBanquetIDTableValueType()
    : m_iGroupID(0)
    , m_ulBanquetID(0)
    {}

    int                     m_iGroupID;
    uint64_t                m_ulBanquetID;
};

struct TBanquetIDTable
{
    TBanquetIDTable() : m_pRecord(nullptr) { }
    ~TBanquetIDTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TBanquetIDTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TBanquetIDTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iGroupID);
        m_pRecord->SetBanquetID(stData.m_ulBanquetID);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const uint64_t& GetBanquetID() const
    {
        return m_stData.m_ulBanquetID;
    }
    void AddBanquetID(uint64_t ulBanquetID)
    {
        if (0 == ulBanquetID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBanquetID(ulBanquetID);
        m_stData.m_ulBanquetID += ulBanquetID;
    }
    void SetBanquetID(uint64_t ulBanquetID)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulBanquetID > ulBanquetID)
        {
            m_pRecord->SetBanquetID(m_stData.m_ulBanquetID - ulBanquetID, '-');
        }
        else
        {
            m_pRecord->SetBanquetID(ulBanquetID - m_stData.m_ulBanquetID, '+');
        }
        m_stData.m_ulBanquetID = ulBanquetID;
    }

    CRecordBanquetIDTable* m_pRecord;
    TBanquetIDTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordBanquetIDTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordBanquetIDTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordBanquetIDTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
