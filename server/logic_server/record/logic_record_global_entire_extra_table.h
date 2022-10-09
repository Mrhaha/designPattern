#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGlobalEntireExtraTable;
struct TGlobalEntireExtraTableValueType;

class CRecordGlobalEntireExtraTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGlobalEntireExtraTable() : m_pCache(nullptr) { }

    virtual ~CRecordGlobalEntireExtraTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetServerID();
    virtual int GetWarID();
    virtual int GetExtraID();
    virtual int GetUid() { return 0; }
    virtual int GetGroupID() { return 0; }

    static std::vector<TGlobalEntireExtraTableValueType> SelectDataFromSQL(int iServerID);

    static const char* s_strTableName;
    static const char* s_strCommodityItemType;
    static const char* s_strCommodityItemID;
    static const char* s_strCommodityItemNum;
    static const char* s_strConsumeItemType;
    static const char* s_strConsumeItemID;
    static const char* s_strConsumeItemNum;
    static const char* s_strDiscount;
    static const char* s_strServerID;
    static const char* s_strWarID;
    static const char* s_strExtraID;

    TGlobalEntireExtraTable* m_pCache;

    void SetCommodityItemType(int iCommodityItemType)
    {
        m_stIntData[s_strCommodityItemType] += iCommodityItemType;
    }
    void SetCommodityItemID(int iCommodityItemID)
    {
        m_stIntData[s_strCommodityItemID] += iCommodityItemID;
    }
    void SetCommodityItemNum(int iCommodityItemNum)
    {
        m_stIntData[s_strCommodityItemNum] += iCommodityItemNum;
    }
    void SetConsumeItemType(int iConsumeItemType)
    {
        m_stIntData[s_strConsumeItemType] += iConsumeItemType;
    }
    void SetConsumeItemID(int iConsumeItemID)
    {
        m_stIntData[s_strConsumeItemID] += iConsumeItemID;
    }
    void SetConsumeItemNum(int iConsumeItemNum)
    {
        m_stIntData[s_strConsumeItemNum] += iConsumeItemNum;
    }
    void SetDiscount(int iDiscount)
    {
        m_stIntData[s_strDiscount] += iDiscount;
    }
    void SetPrimaryKey(int iServerID, int iWarID, int iExtraID)
    {
        m_iServerID = iServerID;
        m_iWarID = iWarID;
        m_iExtraID = iExtraID;
    }
    void SetCachePtr(TGlobalEntireExtraTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iServerID;
    int m_iWarID;
    int m_iExtraID;
};

struct TGlobalEntireExtraTableValueType
{
    TGlobalEntireExtraTableValueType()
    : m_iServerID(0)
    , m_iWarID(0)
    , m_iExtraID(0)
    , m_iCommodityItemType(0)
    , m_iCommodityItemID(0)
    , m_iCommodityItemNum(0)
    , m_iConsumeItemType(0)
    , m_iConsumeItemID(0)
    , m_iConsumeItemNum(0)
    , m_iDiscount(0)
    {}

    int                     m_iServerID;
    int                     m_iWarID;
    int                     m_iExtraID;
    int                     m_iCommodityItemType;
    int                     m_iCommodityItemID;
    int                     m_iCommodityItemNum;
    int                     m_iConsumeItemType;
    int                     m_iConsumeItemID;
    int                     m_iConsumeItemNum;
    int                     m_iDiscount;
};

struct TGlobalEntireExtraTable
{
    TGlobalEntireExtraTable() : m_pRecord(nullptr) { }
    ~TGlobalEntireExtraTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGlobalEntireExtraTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGlobalEntireExtraTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iServerID, stData.m_iWarID, stData.m_iExtraID);
        m_pRecord->SetCommodityItemType(stData.m_iCommodityItemType);
        m_pRecord->SetCommodityItemID(stData.m_iCommodityItemID);
        m_pRecord->SetCommodityItemNum(stData.m_iCommodityItemNum);
        m_pRecord->SetConsumeItemType(stData.m_iConsumeItemType);
        m_pRecord->SetConsumeItemID(stData.m_iConsumeItemID);
        m_pRecord->SetConsumeItemNum(stData.m_iConsumeItemNum);
        m_pRecord->SetDiscount(stData.m_iDiscount);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetCommodityItemType() const
    {
        return m_stData.m_iCommodityItemType;
    }
    void AddCommodityItemType(int iCommodityItemType)
    {
        if (0 == iCommodityItemType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCommodityItemType(iCommodityItemType);
        m_stData.m_iCommodityItemType += iCommodityItemType;
    }
    void SetCommodityItemType(int iCommodityItemType)
    {
        if (m_stData.m_iCommodityItemType == iCommodityItemType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCommodityItemType - m_stData.m_iCommodityItemType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCommodityItemType(RealAdd);
        m_stData.m_iCommodityItemType = iCommodityItemType;
    }
    const int& GetCommodityItemID() const
    {
        return m_stData.m_iCommodityItemID;
    }
    void AddCommodityItemID(int iCommodityItemID)
    {
        if (0 == iCommodityItemID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCommodityItemID(iCommodityItemID);
        m_stData.m_iCommodityItemID += iCommodityItemID;
    }
    void SetCommodityItemID(int iCommodityItemID)
    {
        if (m_stData.m_iCommodityItemID == iCommodityItemID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCommodityItemID - m_stData.m_iCommodityItemID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCommodityItemID(RealAdd);
        m_stData.m_iCommodityItemID = iCommodityItemID;
    }
    const int& GetCommodityItemNum() const
    {
        return m_stData.m_iCommodityItemNum;
    }
    void AddCommodityItemNum(int iCommodityItemNum)
    {
        if (0 == iCommodityItemNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCommodityItemNum(iCommodityItemNum);
        m_stData.m_iCommodityItemNum += iCommodityItemNum;
    }
    void SetCommodityItemNum(int iCommodityItemNum)
    {
        if (m_stData.m_iCommodityItemNum == iCommodityItemNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCommodityItemNum - m_stData.m_iCommodityItemNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCommodityItemNum(RealAdd);
        m_stData.m_iCommodityItemNum = iCommodityItemNum;
    }
    const int& GetConsumeItemType() const
    {
        return m_stData.m_iConsumeItemType;
    }
    void AddConsumeItemType(int iConsumeItemType)
    {
        if (0 == iConsumeItemType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConsumeItemType(iConsumeItemType);
        m_stData.m_iConsumeItemType += iConsumeItemType;
    }
    void SetConsumeItemType(int iConsumeItemType)
    {
        if (m_stData.m_iConsumeItemType == iConsumeItemType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConsumeItemType - m_stData.m_iConsumeItemType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConsumeItemType(RealAdd);
        m_stData.m_iConsumeItemType = iConsumeItemType;
    }
    const int& GetConsumeItemID() const
    {
        return m_stData.m_iConsumeItemID;
    }
    void AddConsumeItemID(int iConsumeItemID)
    {
        if (0 == iConsumeItemID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConsumeItemID(iConsumeItemID);
        m_stData.m_iConsumeItemID += iConsumeItemID;
    }
    void SetConsumeItemID(int iConsumeItemID)
    {
        if (m_stData.m_iConsumeItemID == iConsumeItemID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConsumeItemID - m_stData.m_iConsumeItemID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConsumeItemID(RealAdd);
        m_stData.m_iConsumeItemID = iConsumeItemID;
    }
    const int& GetConsumeItemNum() const
    {
        return m_stData.m_iConsumeItemNum;
    }
    void AddConsumeItemNum(int iConsumeItemNum)
    {
        if (0 == iConsumeItemNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConsumeItemNum(iConsumeItemNum);
        m_stData.m_iConsumeItemNum += iConsumeItemNum;
    }
    void SetConsumeItemNum(int iConsumeItemNum)
    {
        if (m_stData.m_iConsumeItemNum == iConsumeItemNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConsumeItemNum - m_stData.m_iConsumeItemNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConsumeItemNum(RealAdd);
        m_stData.m_iConsumeItemNum = iConsumeItemNum;
    }
    const int& GetDiscount() const
    {
        return m_stData.m_iDiscount;
    }
    void AddDiscount(int iDiscount)
    {
        if (0 == iDiscount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDiscount(iDiscount);
        m_stData.m_iDiscount += iDiscount;
    }
    void SetDiscount(int iDiscount)
    {
        if (m_stData.m_iDiscount == iDiscount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDiscount - m_stData.m_iDiscount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDiscount(RealAdd);
        m_stData.m_iDiscount = iDiscount;
    }

    CRecordGlobalEntireExtraTable* m_pRecord;
    TGlobalEntireExtraTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGlobalEntireExtraTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGlobalEntireExtraTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iServerID, m_stData.m_iWarID, m_stData.m_iExtraID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGlobalEntireExtraTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
