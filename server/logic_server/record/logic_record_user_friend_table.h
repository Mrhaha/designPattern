#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserFriendTable;
struct TUserFriendTableValueType;

class CRecordUserFriendTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserFriendTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserFriendTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetFriendUid();
    virtual int GetFriendGroupID();

    static std::vector<TUserFriendTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strFriendUid;
    static const char* s_strFriendGroupID;

    TUserFriendTable* m_pCache;

    void SetPrimaryKey(int iUid, int iGroupID, int iFriendUid, int iFriendGroupID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iFriendUid = iFriendUid;
        m_iFriendGroupID = iFriendGroupID;
    }
    void SetCachePtr(TUserFriendTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iFriendUid;
    int m_iFriendGroupID;
};

struct TUserFriendTableValueType
{
    TUserFriendTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iFriendUid(0)
    , m_iFriendGroupID(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iFriendUid;
    int                     m_iFriendGroupID;
};

struct TUserFriendTable
{
    TUserFriendTable() : m_pRecord(nullptr) { }
    ~TUserFriendTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserFriendTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserFriendTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iFriendUid, stData.m_iFriendGroupID);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }

    CRecordUserFriendTable* m_pRecord;
    TUserFriendTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserFriendTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserFriendTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iFriendUid, m_stData.m_iFriendGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserFriendTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
