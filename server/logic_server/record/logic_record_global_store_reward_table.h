#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGlobalStoreRewardTable;
struct TGlobalStoreRewardTableValueType;

class CRecordGlobalStoreRewardTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGlobalStoreRewardTable() : m_pCache(nullptr) { }

    virtual ~CRecordGlobalStoreRewardTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetServerID();
    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetDepositID();
    virtual int GetIncreaseIndex();

    static std::vector<TGlobalStoreRewardTableValueType> SelectDataFromSQL(int iServerID);

    static const char* s_strTableName;
    static const char* s_strEndTime;
    static const char* s_strServerID;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strDepositID;
    static const char* s_strIncreaseIndex;

    TGlobalStoreRewardTable* m_pCache;

    void SetEndTime(int iEndTime)
    {
        m_stIntData[s_strEndTime] += iEndTime;
    }
    void SetPrimaryKey(int iServerID, int iUid, int iGroupID, int iDepositID, int iIncreaseIndex)
    {
        m_iServerID = iServerID;
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iDepositID = iDepositID;
        m_iIncreaseIndex = iIncreaseIndex;
    }
    void SetCachePtr(TGlobalStoreRewardTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iServerID;
    int m_iUid;
    int m_iGroupID;
    int m_iDepositID;
    int m_iIncreaseIndex;
};

struct TGlobalStoreRewardTableValueType
{
    TGlobalStoreRewardTableValueType()
    : m_iServerID(0)
    , m_iUid(0)
    , m_iGroupID(0)
    , m_iDepositID(0)
    , m_iIncreaseIndex(0)
    , m_iEndTime(0)
    {}

    int                     m_iServerID;
    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iDepositID;
    int                     m_iIncreaseIndex;
    int                     m_iEndTime;
};

struct TGlobalStoreRewardTable
{
    TGlobalStoreRewardTable() : m_pRecord(nullptr) { }
    ~TGlobalStoreRewardTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGlobalStoreRewardTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGlobalStoreRewardTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iServerID, stData.m_iUid, stData.m_iGroupID, stData.m_iDepositID, stData.m_iIncreaseIndex);
        m_pRecord->SetEndTime(stData.m_iEndTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetEndTime() const
    {
        return m_stData.m_iEndTime;
    }
    void AddEndTime(int iEndTime)
    {
        if (0 == iEndTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEndTime(iEndTime);
        m_stData.m_iEndTime += iEndTime;
    }
    void SetEndTime(int iEndTime)
    {
        if (m_stData.m_iEndTime == iEndTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEndTime - m_stData.m_iEndTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEndTime(RealAdd);
        m_stData.m_iEndTime = iEndTime;
    }

    CRecordGlobalStoreRewardTable* m_pRecord;
    TGlobalStoreRewardTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGlobalStoreRewardTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGlobalStoreRewardTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iServerID, m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iDepositID, m_stData.m_iIncreaseIndex);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGlobalStoreRewardTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
