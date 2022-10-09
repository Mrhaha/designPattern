#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct THomeStoreTable;
struct THomeStoreTableValueType;

class CRecordHomeStoreTable : public CLogicSQLTableRecordBase
{
public:
    CRecordHomeStoreTable() : m_pCache(nullptr) { }

    virtual ~CRecordHomeStoreTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetStoreID();

    static std::vector<THomeStoreTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strStoreOpen;
    static const char* s_strSellItemType;
    static const char* s_strSellItemID;
    static const char* s_strSellItemNum;
    static const char* s_strBuyItemType;
    static const char* s_strBuyItemID;
    static const char* s_strBuyItemNum;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strStoreID;

    THomeStoreTable* m_pCache;

    void SetStoreOpen(int iStoreOpen)
    {
        m_stIntData[s_strStoreOpen] += iStoreOpen;
    }
    void SetSellItemType(int iSellItemType)
    {
        m_stIntData[s_strSellItemType] += iSellItemType;
    }
    void SetSellItemID(int iSellItemID)
    {
        m_stIntData[s_strSellItemID] += iSellItemID;
    }
    void SetSellItemNum(int iSellItemNum)
    {
        m_stIntData[s_strSellItemNum] += iSellItemNum;
    }
    void SetBuyItemType(int iBuyItemType)
    {
        m_stIntData[s_strBuyItemType] += iBuyItemType;
    }
    void SetBuyItemID(int iBuyItemID)
    {
        m_stIntData[s_strBuyItemID] += iBuyItemID;
    }
    void SetBuyItemNum(int iBuyItemNum)
    {
        m_stIntData[s_strBuyItemNum] += iBuyItemNum;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iStoreID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iStoreID = iStoreID;
    }
    void SetCachePtr(THomeStoreTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iStoreID;
};

struct THomeStoreTableValueType
{
    THomeStoreTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iStoreID(0)
    , m_iStoreOpen(0)
    , m_iSellItemType(0)
    , m_iSellItemID(0)
    , m_iSellItemNum(0)
    , m_iBuyItemType(0)
    , m_iBuyItemID(0)
    , m_iBuyItemNum(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iStoreID;
    int                     m_iStoreOpen;
    int                     m_iSellItemType;
    int                     m_iSellItemID;
    int                     m_iSellItemNum;
    int                     m_iBuyItemType;
    int                     m_iBuyItemID;
    int                     m_iBuyItemNum;
};

struct THomeStoreTable
{
    THomeStoreTable() : m_pRecord(nullptr) { }
    ~THomeStoreTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const THomeStoreTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const THomeStoreTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iStoreID);
        m_pRecord->SetStoreOpen(stData.m_iStoreOpen);
        m_pRecord->SetSellItemType(stData.m_iSellItemType);
        m_pRecord->SetSellItemID(stData.m_iSellItemID);
        m_pRecord->SetSellItemNum(stData.m_iSellItemNum);
        m_pRecord->SetBuyItemType(stData.m_iBuyItemType);
        m_pRecord->SetBuyItemID(stData.m_iBuyItemID);
        m_pRecord->SetBuyItemNum(stData.m_iBuyItemNum);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetStoreOpen() const
    {
        return m_stData.m_iStoreOpen;
    }
    void AddStoreOpen(int iStoreOpen)
    {
        if (0 == iStoreOpen)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetStoreOpen(iStoreOpen);
        m_stData.m_iStoreOpen += iStoreOpen;
    }
    void SetStoreOpen(int iStoreOpen)
    {
        if (m_stData.m_iStoreOpen == iStoreOpen)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iStoreOpen - m_stData.m_iStoreOpen;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetStoreOpen(RealAdd);
        m_stData.m_iStoreOpen = iStoreOpen;
    }
    const int& GetSellItemType() const
    {
        return m_stData.m_iSellItemType;
    }
    void AddSellItemType(int iSellItemType)
    {
        if (0 == iSellItemType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSellItemType(iSellItemType);
        m_stData.m_iSellItemType += iSellItemType;
    }
    void SetSellItemType(int iSellItemType)
    {
        if (m_stData.m_iSellItemType == iSellItemType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSellItemType - m_stData.m_iSellItemType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSellItemType(RealAdd);
        m_stData.m_iSellItemType = iSellItemType;
    }
    const int& GetSellItemID() const
    {
        return m_stData.m_iSellItemID;
    }
    void AddSellItemID(int iSellItemID)
    {
        if (0 == iSellItemID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSellItemID(iSellItemID);
        m_stData.m_iSellItemID += iSellItemID;
    }
    void SetSellItemID(int iSellItemID)
    {
        if (m_stData.m_iSellItemID == iSellItemID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSellItemID - m_stData.m_iSellItemID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSellItemID(RealAdd);
        m_stData.m_iSellItemID = iSellItemID;
    }
    const int& GetSellItemNum() const
    {
        return m_stData.m_iSellItemNum;
    }
    void AddSellItemNum(int iSellItemNum)
    {
        if (0 == iSellItemNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSellItemNum(iSellItemNum);
        m_stData.m_iSellItemNum += iSellItemNum;
    }
    void SetSellItemNum(int iSellItemNum)
    {
        if (m_stData.m_iSellItemNum == iSellItemNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSellItemNum - m_stData.m_iSellItemNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSellItemNum(RealAdd);
        m_stData.m_iSellItemNum = iSellItemNum;
    }
    const int& GetBuyItemType() const
    {
        return m_stData.m_iBuyItemType;
    }
    void AddBuyItemType(int iBuyItemType)
    {
        if (0 == iBuyItemType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyItemType(iBuyItemType);
        m_stData.m_iBuyItemType += iBuyItemType;
    }
    void SetBuyItemType(int iBuyItemType)
    {
        if (m_stData.m_iBuyItemType == iBuyItemType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyItemType - m_stData.m_iBuyItemType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyItemType(RealAdd);
        m_stData.m_iBuyItemType = iBuyItemType;
    }
    const int& GetBuyItemID() const
    {
        return m_stData.m_iBuyItemID;
    }
    void AddBuyItemID(int iBuyItemID)
    {
        if (0 == iBuyItemID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyItemID(iBuyItemID);
        m_stData.m_iBuyItemID += iBuyItemID;
    }
    void SetBuyItemID(int iBuyItemID)
    {
        if (m_stData.m_iBuyItemID == iBuyItemID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyItemID - m_stData.m_iBuyItemID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyItemID(RealAdd);
        m_stData.m_iBuyItemID = iBuyItemID;
    }
    const int& GetBuyItemNum() const
    {
        return m_stData.m_iBuyItemNum;
    }
    void AddBuyItemNum(int iBuyItemNum)
    {
        if (0 == iBuyItemNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyItemNum(iBuyItemNum);
        m_stData.m_iBuyItemNum += iBuyItemNum;
    }
    void SetBuyItemNum(int iBuyItemNum)
    {
        if (m_stData.m_iBuyItemNum == iBuyItemNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyItemNum - m_stData.m_iBuyItemNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyItemNum(RealAdd);
        m_stData.m_iBuyItemNum = iBuyItemNum;
    }

    CRecordHomeStoreTable* m_pRecord;
    THomeStoreTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordHomeStoreTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordHomeStoreTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iStoreID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordHomeStoreTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
