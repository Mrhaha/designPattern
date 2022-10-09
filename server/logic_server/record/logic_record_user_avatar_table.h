#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserAvatarTable;
struct TUserAvatarTableValueType;

class CRecordUserAvatarTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserAvatarTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserAvatarTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetAvatarID();

    static std::vector<TUserAvatarTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strAvatarType;
    static const char* s_strTime;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strAvatarID;

    TUserAvatarTable* m_pCache;

    void SetAvatarType(int iAvatarType)
    {
        m_stIntData[s_strAvatarType] += iAvatarType;
    }
    void SetTime(int iTime)
    {
        m_stIntData[s_strTime] += iTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iAvatarID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iAvatarID = iAvatarID;
    }
    void SetCachePtr(TUserAvatarTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iAvatarID;
};

struct TUserAvatarTableValueType
{
    TUserAvatarTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iAvatarID(0)
    , m_iAvatarType(0)
    , m_iTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iAvatarID;
    int                     m_iAvatarType;
    int                     m_iTime;
};

struct TUserAvatarTable
{
    TUserAvatarTable() : m_pRecord(nullptr) { }
    ~TUserAvatarTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserAvatarTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserAvatarTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iAvatarID);
        m_pRecord->SetAvatarType(stData.m_iAvatarType);
        m_pRecord->SetTime(stData.m_iTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetAvatarType() const
    {
        return m_stData.m_iAvatarType;
    }
    void AddAvatarType(int iAvatarType)
    {
        if (0 == iAvatarType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAvatarType(iAvatarType);
        m_stData.m_iAvatarType += iAvatarType;
    }
    void SetAvatarType(int iAvatarType)
    {
        if (m_stData.m_iAvatarType == iAvatarType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAvatarType - m_stData.m_iAvatarType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAvatarType(RealAdd);
        m_stData.m_iAvatarType = iAvatarType;
    }
    const int& GetTime() const
    {
        return m_stData.m_iTime;
    }
    void AddTime(int iTime)
    {
        if (0 == iTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTime(iTime);
        m_stData.m_iTime += iTime;
    }
    void SetTime(int iTime)
    {
        if (m_stData.m_iTime == iTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTime - m_stData.m_iTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTime(RealAdd);
        m_stData.m_iTime = iTime;
    }

    CRecordUserAvatarTable* m_pRecord;
    TUserAvatarTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserAvatarTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserAvatarTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iAvatarID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserAvatarTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
