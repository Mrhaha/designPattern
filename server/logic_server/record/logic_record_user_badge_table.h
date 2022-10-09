#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserBadgeTable;
struct TUserBadgeTableValueType;

class CRecordUserBadgeTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserBadgeTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserBadgeTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetBadgeID();

    static std::vector<TUserBadgeTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strIsOwned;
    static const char* s_strProgress;
    static const char* s_strOwnedTime;
    static const char* s_strProSet;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strBadgeID;

    TUserBadgeTable* m_pCache;

    void SetIsOwned(char cIsOwned)
    {
        m_stCharData[s_strIsOwned] += cIsOwned;
    }
    void SetProgress(int iProgress)
    {
        m_stIntData[s_strProgress] += iProgress;
    }
    void SetOwnedTime(int iOwnedTime)
    {
        m_stIntData[s_strOwnedTime] += iOwnedTime;
    }
    void SetProSet(const std::set<int32_t>& stProSet)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stProSet);
        m_stVarData[s_strProSet] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iBadgeID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iBadgeID = iBadgeID;
    }
    void SetCachePtr(TUserBadgeTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iBadgeID;
};

struct TUserBadgeTableValueType
{
    TUserBadgeTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iBadgeID(0)
    , m_cIsOwned(0)
    , m_iProgress(0)
    , m_iOwnedTime(0)
    , m_stProSet()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iBadgeID;
    char                    m_cIsOwned;
    int                     m_iProgress;
    int                     m_iOwnedTime;
    std::set<int32_t>       m_stProSet;
};

struct TUserBadgeTable
{
    TUserBadgeTable() : m_pRecord(nullptr) { }
    ~TUserBadgeTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserBadgeTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserBadgeTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iBadgeID);
        m_pRecord->SetIsOwned(stData.m_cIsOwned);
        m_pRecord->SetProgress(stData.m_iProgress);
        m_pRecord->SetOwnedTime(stData.m_iOwnedTime);
        m_pRecord->SetProSet(stData.m_stProSet);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const char& GetIsOwned() const
    {
        return m_stData.m_cIsOwned;
    }
    void AddIsOwned(char cIsOwned)
    {
        if (0 == cIsOwned)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsOwned(cIsOwned);
        m_stData.m_cIsOwned += cIsOwned;
    }
    void SetIsOwned(char cIsOwned)
    {
        if (m_stData.m_cIsOwned == cIsOwned)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cIsOwned - m_stData.m_cIsOwned;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsOwned(RealAdd);
        m_stData.m_cIsOwned = cIsOwned;
    }
    const int& GetProgress() const
    {
        return m_stData.m_iProgress;
    }
    void AddProgress(int iProgress)
    {
        if (0 == iProgress)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetProgress(iProgress);
        m_stData.m_iProgress += iProgress;
    }
    void SetProgress(int iProgress)
    {
        if (m_stData.m_iProgress == iProgress)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iProgress - m_stData.m_iProgress;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetProgress(RealAdd);
        m_stData.m_iProgress = iProgress;
    }
    const int& GetOwnedTime() const
    {
        return m_stData.m_iOwnedTime;
    }
    void AddOwnedTime(int iOwnedTime)
    {
        if (0 == iOwnedTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetOwnedTime(iOwnedTime);
        m_stData.m_iOwnedTime += iOwnedTime;
    }
    void SetOwnedTime(int iOwnedTime)
    {
        if (m_stData.m_iOwnedTime == iOwnedTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iOwnedTime - m_stData.m_iOwnedTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetOwnedTime(RealAdd);
        m_stData.m_iOwnedTime = iOwnedTime;
    }
    const std::set<int32_t>& GetProSet() const
    {
        return m_stData.m_stProSet;
    }
    void SetProSet(const std::set<int32_t>& stProSet)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetProSet(stProSet);
        m_stData.m_stProSet = stProSet;
    }

    CRecordUserBadgeTable* m_pRecord;
    TUserBadgeTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserBadgeTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserBadgeTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iBadgeID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserBadgeTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
