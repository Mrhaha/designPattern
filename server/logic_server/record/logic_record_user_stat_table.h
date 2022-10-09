#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserStatTable;
struct TUserStatTableValueType;

class CRecordUserStatTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserStatTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserStatTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetStatDate();

    static std::vector<TUserStatTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strPayMoney;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strStatDate;

    TUserStatTable* m_pCache;

    void SetPayMoney(int iPayMoney)
    {
        m_stIntData[s_strPayMoney] += iPayMoney;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iStatDate)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iStatDate = iStatDate;
    }
    void SetCachePtr(TUserStatTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iStatDate;
};

struct TUserStatTableValueType
{
    TUserStatTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iStatDate(0)
    , m_iPayMoney(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iStatDate;
    int                     m_iPayMoney;
};

struct TUserStatTable
{
    TUserStatTable() : m_pRecord(nullptr) { }
    ~TUserStatTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserStatTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserStatTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iStatDate);
        m_pRecord->SetPayMoney(stData.m_iPayMoney);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetPayMoney() const
    {
        return m_stData.m_iPayMoney;
    }
    void AddPayMoney(int iPayMoney)
    {
        if (0 == iPayMoney)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayMoney(iPayMoney);
        m_stData.m_iPayMoney += iPayMoney;
    }
    void SetPayMoney(int iPayMoney)
    {
        if (m_stData.m_iPayMoney == iPayMoney)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPayMoney - m_stData.m_iPayMoney;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayMoney(RealAdd);
        m_stData.m_iPayMoney = iPayMoney;
    }

    CRecordUserStatTable* m_pRecord;
    TUserStatTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserStatTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserStatTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iStatDate);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserStatTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
