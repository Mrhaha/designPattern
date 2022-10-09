#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserBuyBackTable;
struct TUserBuyBackTableValueType;

class CRecordUserBuyBackTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserBuyBackTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserBuyBackTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetBackType();

    static std::vector<TUserBuyBackTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strUseBackCount;
    static const char* s_strBuyBackCount;
    static const char* s_strCanBackCount;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strBackType;

    TUserBuyBackTable* m_pCache;

    void SetUseBackCount(int iUseBackCount)
    {
        m_stIntData[s_strUseBackCount] += iUseBackCount;
    }
    void SetBuyBackCount(int iBuyBackCount)
    {
        m_stIntData[s_strBuyBackCount] += iBuyBackCount;
    }
    void SetCanBackCount(int iCanBackCount)
    {
        m_stIntData[s_strCanBackCount] += iCanBackCount;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iBackType)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iBackType = iBackType;
    }
    void SetCachePtr(TUserBuyBackTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iBackType;
};

struct TUserBuyBackTableValueType
{
    TUserBuyBackTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iBackType(0)
    , m_iUseBackCount(0)
    , m_iBuyBackCount(0)
    , m_iCanBackCount(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iBackType;
    int                     m_iUseBackCount;
    int                     m_iBuyBackCount;
    int                     m_iCanBackCount;
};

struct TUserBuyBackTable
{
    TUserBuyBackTable() : m_pRecord(nullptr) { }
    ~TUserBuyBackTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserBuyBackTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserBuyBackTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iBackType);
        m_pRecord->SetUseBackCount(stData.m_iUseBackCount);
        m_pRecord->SetBuyBackCount(stData.m_iBuyBackCount);
        m_pRecord->SetCanBackCount(stData.m_iCanBackCount);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetUseBackCount() const
    {
        return m_stData.m_iUseBackCount;
    }
    void AddUseBackCount(int iUseBackCount)
    {
        if (0 == iUseBackCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUseBackCount(iUseBackCount);
        m_stData.m_iUseBackCount += iUseBackCount;
    }
    void SetUseBackCount(int iUseBackCount)
    {
        if (m_stData.m_iUseBackCount == iUseBackCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iUseBackCount - m_stData.m_iUseBackCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUseBackCount(RealAdd);
        m_stData.m_iUseBackCount = iUseBackCount;
    }
    const int& GetBuyBackCount() const
    {
        return m_stData.m_iBuyBackCount;
    }
    void AddBuyBackCount(int iBuyBackCount)
    {
        if (0 == iBuyBackCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyBackCount(iBuyBackCount);
        m_stData.m_iBuyBackCount += iBuyBackCount;
    }
    void SetBuyBackCount(int iBuyBackCount)
    {
        if (m_stData.m_iBuyBackCount == iBuyBackCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyBackCount - m_stData.m_iBuyBackCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyBackCount(RealAdd);
        m_stData.m_iBuyBackCount = iBuyBackCount;
    }
    const int& GetCanBackCount() const
    {
        return m_stData.m_iCanBackCount;
    }
    void AddCanBackCount(int iCanBackCount)
    {
        if (0 == iCanBackCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCanBackCount(iCanBackCount);
        m_stData.m_iCanBackCount += iCanBackCount;
    }
    void SetCanBackCount(int iCanBackCount)
    {
        if (m_stData.m_iCanBackCount == iCanBackCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCanBackCount - m_stData.m_iCanBackCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCanBackCount(RealAdd);
        m_stData.m_iCanBackCount = iCanBackCount;
    }

    CRecordUserBuyBackTable* m_pRecord;
    TUserBuyBackTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserBuyBackTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserBuyBackTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iBackType);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserBuyBackTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
