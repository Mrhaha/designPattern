#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGlobalTowerRankTable;
struct TGlobalTowerRankTableValueType;

class CRecordGlobalTowerRankTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGlobalTowerRankTable() : m_pCache(nullptr) { }

    virtual ~CRecordGlobalTowerRankTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetServerID();
    virtual int GetUid();
    virtual int GetGroupID();

    static std::vector<TGlobalTowerRankTableValueType> SelectDataFromSQL(int iServerID);

    static const char* s_strTableName;
    static const char* s_strRankScore;
    static const char* s_strServerID;
    static const char* s_strUid;
    static const char* s_strGroupID;

    TGlobalTowerRankTable* m_pCache;

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
    void SetPrimaryKey(int iServerID, int iUid, int iGroupID)
    {
        m_iServerID = iServerID;
        m_iUid = iUid;
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TGlobalTowerRankTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iServerID;
    int m_iUid;
    int m_iGroupID;
};

struct TGlobalTowerRankTableValueType
{
    TGlobalTowerRankTableValueType()
    : m_iServerID(0)
    , m_iUid(0)
    , m_iGroupID(0)
    , m_ulRankScore(0)
    {}

    int                     m_iServerID;
    int                     m_iUid;
    int                     m_iGroupID;
    uint64_t                m_ulRankScore;
};

struct TGlobalTowerRankTable
{
    TGlobalTowerRankTable() : m_pRecord(nullptr) { }
    ~TGlobalTowerRankTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGlobalTowerRankTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGlobalTowerRankTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iServerID, stData.m_iUid, stData.m_iGroupID);
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

    CRecordGlobalTowerRankTable* m_pRecord;
    TGlobalTowerRankTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGlobalTowerRankTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGlobalTowerRankTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iServerID, m_stData.m_iUid, m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGlobalTowerRankTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
