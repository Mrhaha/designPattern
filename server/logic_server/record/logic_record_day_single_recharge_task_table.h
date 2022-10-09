#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TDaySingleRechargeTaskTable;
struct TDaySingleRechargeTaskTableValueType;

class CRecordDaySingleRechargeTaskTable : public CLogicSQLTableRecordBase
{
public:
    CRecordDaySingleRechargeTaskTable() : m_pCache(nullptr) { }

    virtual ~CRecordDaySingleRechargeTaskTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetActivityID();
    virtual int GetTaskID();

    static std::vector<TDaySingleRechargeTaskTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strRewardCount;
    static const char* s_strCanRewardCount;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strActivityID;
    static const char* s_strTaskID;

    TDaySingleRechargeTaskTable* m_pCache;

    void SetRewardCount(int iRewardCount)
    {
        m_stIntData[s_strRewardCount] += iRewardCount;
    }
    void SetCanRewardCount(int iCanRewardCount)
    {
        m_stIntData[s_strCanRewardCount] += iCanRewardCount;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iActivityID, int iTaskID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iActivityID = iActivityID;
        m_iTaskID = iTaskID;
    }
    void SetCachePtr(TDaySingleRechargeTaskTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iActivityID;
    int m_iTaskID;
};

struct TDaySingleRechargeTaskTableValueType
{
    TDaySingleRechargeTaskTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iActivityID(0)
    , m_iTaskID(0)
    , m_iRewardCount(0)
    , m_iCanRewardCount(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iActivityID;
    int                     m_iTaskID;
    int                     m_iRewardCount;
    int                     m_iCanRewardCount;
};

struct TDaySingleRechargeTaskTable
{
    TDaySingleRechargeTaskTable() : m_pRecord(nullptr) { }
    ~TDaySingleRechargeTaskTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TDaySingleRechargeTaskTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TDaySingleRechargeTaskTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iActivityID, stData.m_iTaskID);
        m_pRecord->SetRewardCount(stData.m_iRewardCount);
        m_pRecord->SetCanRewardCount(stData.m_iCanRewardCount);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetRewardCount() const
    {
        return m_stData.m_iRewardCount;
    }
    void AddRewardCount(int iRewardCount)
    {
        if (0 == iRewardCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRewardCount(iRewardCount);
        m_stData.m_iRewardCount += iRewardCount;
    }
    void SetRewardCount(int iRewardCount)
    {
        if (m_stData.m_iRewardCount == iRewardCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iRewardCount - m_stData.m_iRewardCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRewardCount(RealAdd);
        m_stData.m_iRewardCount = iRewardCount;
    }
    const int& GetCanRewardCount() const
    {
        return m_stData.m_iCanRewardCount;
    }
    void AddCanRewardCount(int iCanRewardCount)
    {
        if (0 == iCanRewardCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCanRewardCount(iCanRewardCount);
        m_stData.m_iCanRewardCount += iCanRewardCount;
    }
    void SetCanRewardCount(int iCanRewardCount)
    {
        if (m_stData.m_iCanRewardCount == iCanRewardCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCanRewardCount - m_stData.m_iCanRewardCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCanRewardCount(RealAdd);
        m_stData.m_iCanRewardCount = iCanRewardCount;
    }

    CRecordDaySingleRechargeTaskTable* m_pRecord;
    TDaySingleRechargeTaskTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordDaySingleRechargeTaskTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordDaySingleRechargeTaskTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iActivityID, m_stData.m_iTaskID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordDaySingleRechargeTaskTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
