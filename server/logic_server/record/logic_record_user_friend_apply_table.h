#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserFriendApplyTable;
struct TUserFriendApplyTableValueType;

class CRecordUserFriendApplyTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserFriendApplyTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserFriendApplyTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetFriendUid();
    virtual int GetFriendGroupID();

    static std::vector<TUserFriendApplyTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strApplyTime;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strFriendUid;
    static const char* s_strFriendGroupID;

    TUserFriendApplyTable* m_pCache;

    void SetApplyTime(int iApplyTime)
    {
        m_stIntData[s_strApplyTime] += iApplyTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iFriendUid, int iFriendGroupID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iFriendUid = iFriendUid;
        m_iFriendGroupID = iFriendGroupID;
    }
    void SetCachePtr(TUserFriendApplyTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iFriendUid;
    int m_iFriendGroupID;
};

struct TUserFriendApplyTableValueType
{
    TUserFriendApplyTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iFriendUid(0)
    , m_iFriendGroupID(0)
    , m_iApplyTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iFriendUid;
    int                     m_iFriendGroupID;
    int                     m_iApplyTime;
};

struct TUserFriendApplyTable
{
    TUserFriendApplyTable() : m_pRecord(nullptr) { }
    ~TUserFriendApplyTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserFriendApplyTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserFriendApplyTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iFriendUid, stData.m_iFriendGroupID);
        m_pRecord->SetApplyTime(stData.m_iApplyTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetApplyTime() const
    {
        return m_stData.m_iApplyTime;
    }
    void AddApplyTime(int iApplyTime)
    {
        if (0 == iApplyTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetApplyTime(iApplyTime);
        m_stData.m_iApplyTime += iApplyTime;
    }
    void SetApplyTime(int iApplyTime)
    {
        if (m_stData.m_iApplyTime == iApplyTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iApplyTime - m_stData.m_iApplyTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetApplyTime(RealAdd);
        m_stData.m_iApplyTime = iApplyTime;
    }

    CRecordUserFriendApplyTable* m_pRecord;
    TUserFriendApplyTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserFriendApplyTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserFriendApplyTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iFriendUid, m_stData.m_iFriendGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserFriendApplyTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
