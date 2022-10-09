#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserItemTable;
struct TUserItemTableValueType;

class CRecordUserItemTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserItemTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserItemTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetItemID();

    static std::vector<TUserItemTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strAmount;
    static const char* s_strAddUpAmount;
    static const char* s_strOpenedCount;
    static const char* s_strNextOpenTime;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strItemID;

    TUserItemTable* m_pCache;

    void SetAmount(int iAmount)
    {
        m_stIntData[s_strAmount] += iAmount;
    }
    void SetAddUpAmount(int iAddUpAmount)
    {
        m_stIntData[s_strAddUpAmount] += iAddUpAmount;
    }
    void SetOpenedCount(int iOpenedCount)
    {
        m_stIntData[s_strOpenedCount] += iOpenedCount;
    }
    void SetNextOpenTime(int iNextOpenTime)
    {
        m_stIntData[s_strNextOpenTime] += iNextOpenTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iItemID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iItemID = iItemID;
    }
    void SetCachePtr(TUserItemTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iItemID;
};

struct TUserItemTableValueType
{
    TUserItemTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iItemID(0)
    , m_iAmount(0)
    , m_iAddUpAmount(0)
    , m_iOpenedCount(0)
    , m_iNextOpenTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iItemID;
    int                     m_iAmount;
    int                     m_iAddUpAmount;
    int                     m_iOpenedCount;
    int                     m_iNextOpenTime;
};

struct TUserItemTable
{
    TUserItemTable() : m_pRecord(nullptr) { }
    ~TUserItemTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserItemTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserItemTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iItemID);
        m_pRecord->SetAmount(stData.m_iAmount);
        m_pRecord->SetAddUpAmount(stData.m_iAddUpAmount);
        m_pRecord->SetOpenedCount(stData.m_iOpenedCount);
        m_pRecord->SetNextOpenTime(stData.m_iNextOpenTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetAmount() const
    {
        return m_stData.m_iAmount;
    }
    void AddAmount(int iAmount)
    {
        if (0 == iAmount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAmount(iAmount);
        m_stData.m_iAmount += iAmount;
    }
    void SetAmount(int iAmount)
    {
        if (m_stData.m_iAmount == iAmount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAmount - m_stData.m_iAmount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAmount(RealAdd);
        m_stData.m_iAmount = iAmount;
    }
    const int& GetAddUpAmount() const
    {
        return m_stData.m_iAddUpAmount;
    }
    void AddAddUpAmount(int iAddUpAmount)
    {
        if (0 == iAddUpAmount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAddUpAmount(iAddUpAmount);
        m_stData.m_iAddUpAmount += iAddUpAmount;
    }
    void SetAddUpAmount(int iAddUpAmount)
    {
        if (m_stData.m_iAddUpAmount == iAddUpAmount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAddUpAmount - m_stData.m_iAddUpAmount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAddUpAmount(RealAdd);
        m_stData.m_iAddUpAmount = iAddUpAmount;
    }
    const int& GetOpenedCount() const
    {
        return m_stData.m_iOpenedCount;
    }
    void AddOpenedCount(int iOpenedCount)
    {
        if (0 == iOpenedCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetOpenedCount(iOpenedCount);
        m_stData.m_iOpenedCount += iOpenedCount;
    }
    void SetOpenedCount(int iOpenedCount)
    {
        if (m_stData.m_iOpenedCount == iOpenedCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iOpenedCount - m_stData.m_iOpenedCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetOpenedCount(RealAdd);
        m_stData.m_iOpenedCount = iOpenedCount;
    }
    const int& GetNextOpenTime() const
    {
        return m_stData.m_iNextOpenTime;
    }
    void AddNextOpenTime(int iNextOpenTime)
    {
        if (0 == iNextOpenTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNextOpenTime(iNextOpenTime);
        m_stData.m_iNextOpenTime += iNextOpenTime;
    }
    void SetNextOpenTime(int iNextOpenTime)
    {
        if (m_stData.m_iNextOpenTime == iNextOpenTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNextOpenTime - m_stData.m_iNextOpenTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNextOpenTime(RealAdd);
        m_stData.m_iNextOpenTime = iNextOpenTime;
    }

    CRecordUserItemTable* m_pRecord;
    TUserItemTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserItemTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserItemTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iItemID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserItemTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
