#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGlobalGuildWarRankTable;
struct TGlobalGuildWarRankTableValueType;

class CRecordGlobalGuildWarRankTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGlobalGuildWarRankTable() : m_pCache(nullptr) { }

    virtual ~CRecordGlobalGuildWarRankTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetRankID();
    virtual int GetGuildID();
    virtual int GetGroupID();
    virtual int GetUid() { return 0; }

    static std::vector<TGlobalGuildWarRankTableValueType> SelectDataFromSQL(int iRankID);

    static const char* s_strTableName;
    static const char* s_strRankScore;
    static const char* s_strRankID;
    static const char* s_strGuildID;
    static const char* s_strGroupID;

    TGlobalGuildWarRankTable* m_pCache;

    void SetRankScore(uint64_t ulRankScore, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strRankScore];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulRankScore;
        }
        else
        {
            if (stDataRef.m_ulVal > ulRankScore)
            {
                stDataRef.m_ulVal -= ulRankScore;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulRankScore - stDataRef.m_ulVal;
            }
        }
    }
    void SetPrimaryKey(int iRankID, int iGuildID, int iGroupID)
    {
        m_iRankID = iRankID;
        m_iGuildID = iGuildID;
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TGlobalGuildWarRankTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iRankID;
    int m_iGuildID;
    int m_iGroupID;
};

struct TGlobalGuildWarRankTableValueType
{
    TGlobalGuildWarRankTableValueType()
    : m_iRankID(0)
    , m_iGuildID(0)
    , m_iGroupID(0)
    , m_ulRankScore(0)
    {}

    int                     m_iRankID;
    int                     m_iGuildID;
    int                     m_iGroupID;
    uint64_t                m_ulRankScore;
};

struct TGlobalGuildWarRankTable
{
    TGlobalGuildWarRankTable() : m_pRecord(nullptr) { }
    ~TGlobalGuildWarRankTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGlobalGuildWarRankTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGlobalGuildWarRankTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iRankID, stData.m_iGuildID, stData.m_iGroupID);
        m_pRecord->SetRankScore(stData.m_ulRankScore);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const uint64_t& GetRankScore() const
    {
        return m_stData.m_ulRankScore;
    }
    void AddRankScore(uint64_t ulRankScore)
    {
        if (0 == ulRankScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRankScore(ulRankScore);
        m_stData.m_ulRankScore += ulRankScore;
    }
    void SetRankScore(uint64_t ulRankScore)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulRankScore > ulRankScore)
        {
            m_pRecord->SetRankScore(m_stData.m_ulRankScore - ulRankScore, '-');
        }
        else
        {
            m_pRecord->SetRankScore(ulRankScore - m_stData.m_ulRankScore, '+');
        }
        m_stData.m_ulRankScore = ulRankScore;
    }

    CRecordGlobalGuildWarRankTable* m_pRecord;
    TGlobalGuildWarRankTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGlobalGuildWarRankTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGlobalGuildWarRankTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iRankID, m_stData.m_iGuildID, m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGlobalGuildWarRankTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
