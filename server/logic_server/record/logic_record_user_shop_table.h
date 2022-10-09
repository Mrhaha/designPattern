#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserShopTable;
struct TUserShopTableValueType;

class CRecordUserShopTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserShopTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserShopTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetItemIndex();

    static std::vector<TUserShopTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strBuyCount;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strItemIndex;

    TUserShopTable* m_pCache;

    void SetBuyCount(int iBuyCount)
    {
        m_stIntData[s_strBuyCount] += iBuyCount;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iItemIndex)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iItemIndex = iItemIndex;
    }
    void SetCachePtr(TUserShopTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iItemIndex;
};

struct TUserShopTableValueType
{
    TUserShopTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iItemIndex(0)
    , m_iBuyCount(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iItemIndex;
    int                     m_iBuyCount;
};

struct TUserShopTable
{
    TUserShopTable() : m_pRecord(nullptr) { }
    ~TUserShopTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserShopTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserShopTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iItemIndex);
        m_pRecord->SetBuyCount(stData.m_iBuyCount);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetBuyCount() const
    {
        return m_stData.m_iBuyCount;
    }
    void AddBuyCount(int iBuyCount)
    {
        if (0 == iBuyCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyCount(iBuyCount);
        m_stData.m_iBuyCount += iBuyCount;
    }
    void SetBuyCount(int iBuyCount)
    {
        if (m_stData.m_iBuyCount == iBuyCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyCount - m_stData.m_iBuyCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyCount(RealAdd);
        m_stData.m_iBuyCount = iBuyCount;
    }

    CRecordUserShopTable* m_pRecord;
    TUserShopTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserShopTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserShopTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iItemIndex);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserShopTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
