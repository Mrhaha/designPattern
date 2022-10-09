#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGlobalLastPvpRankTable;
struct TGlobalLastPvpRankTableValueType;

class CRecordGlobalLastPvpRankTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGlobalLastPvpRankTable() : m_pCache(nullptr) { }

    virtual ~CRecordGlobalLastPvpRankTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetServerID();
    virtual int GetLastRank();
    virtual int GetUid() { return 0; }
    virtual int GetGroupID() { return 0; }

    static std::vector<TGlobalLastPvpRankTableValueType> SelectDataFromSQL(int iServerID);

    static const char* s_strTableName;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strServerID;
    static const char* s_strLastRank;

    TGlobalLastPvpRankTable* m_pCache;

    void SetUid(int iUid)
    {
        m_stIntData[s_strUid] += iUid;
    }
    void SetGroupID(int iGroupID)
    {
        m_stIntData[s_strGroupID] += iGroupID;
    }
    void SetPrimaryKey(int iServerID, int iLastRank)
    {
        m_iServerID = iServerID;
        m_iLastRank = iLastRank;
    }
    void SetCachePtr(TGlobalLastPvpRankTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iServerID;
    int m_iLastRank;
};

struct TGlobalLastPvpRankTableValueType
{
    TGlobalLastPvpRankTableValueType()
    : m_iServerID(0)
    , m_iLastRank(0)
    , m_iUid(0)
    , m_iGroupID(0)
    {}

    int                     m_iServerID;
    int                     m_iLastRank;
    int                     m_iUid;
    int                     m_iGroupID;
};

struct TGlobalLastPvpRankTable
{
    TGlobalLastPvpRankTable() : m_pRecord(nullptr) { }
    ~TGlobalLastPvpRankTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGlobalLastPvpRankTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGlobalLastPvpRankTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iServerID, stData.m_iLastRank);
        m_pRecord->SetUid(stData.m_iUid);
        m_pRecord->SetGroupID(stData.m_iGroupID);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetUid() const
    {
        return m_stData.m_iUid;
    }
    void AddUid(int iUid)
    {
        if (0 == iUid)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUid(iUid);
        m_stData.m_iUid += iUid;
    }
    void SetUid(int iUid)
    {
        if (m_stData.m_iUid == iUid)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iUid - m_stData.m_iUid;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUid(RealAdd);
        m_stData.m_iUid = iUid;
    }
    const int& GetGroupID() const
    {
        return m_stData.m_iGroupID;
    }
    void AddGroupID(int iGroupID)
    {
        if (0 == iGroupID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGroupID(iGroupID);
        m_stData.m_iGroupID += iGroupID;
    }
    void SetGroupID(int iGroupID)
    {
        if (m_stData.m_iGroupID == iGroupID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGroupID - m_stData.m_iGroupID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGroupID(RealAdd);
        m_stData.m_iGroupID = iGroupID;
    }

    CRecordGlobalLastPvpRankTable* m_pRecord;
    TGlobalLastPvpRankTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGlobalLastPvpRankTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGlobalLastPvpRankTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iServerID, m_stData.m_iLastRank);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGlobalLastPvpRankTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
