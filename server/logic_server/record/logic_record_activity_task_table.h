#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TActivityTaskTable;
struct TActivityTaskTableValueType;

class CRecordActivityTaskTable : public CLogicSQLTableRecordBase
{
public:
    CRecordActivityTaskTable() : m_pCache(nullptr) { }

    virtual ~CRecordActivityTaskTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetActivityType();
    virtual int GetActivityID();
    virtual int GetTaskID();

    static std::vector<TActivityTaskTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strProgress;
    static const char* s_strState;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strActivityType;
    static const char* s_strActivityID;
    static const char* s_strTaskID;

    TActivityTaskTable* m_pCache;

    void SetProgress(int iProgress)
    {
        m_stIntData[s_strProgress] += iProgress;
    }
    void SetState(char cState)
    {
        m_stCharData[s_strState] += cState;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iActivityType, int iActivityID, int iTaskID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iActivityType = iActivityType;
        m_iActivityID = iActivityID;
        m_iTaskID = iTaskID;
    }
    void SetCachePtr(TActivityTaskTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iActivityType;
    int m_iActivityID;
    int m_iTaskID;
};

struct TActivityTaskTableValueType
{
    TActivityTaskTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iActivityType(0)
    , m_iActivityID(0)
    , m_iTaskID(0)
    , m_iProgress(0)
    , m_cState(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iActivityType;
    int                     m_iActivityID;
    int                     m_iTaskID;
    int                     m_iProgress;
    char                    m_cState;
};

struct TActivityTaskTable
{
    TActivityTaskTable() : m_pRecord(nullptr) { }
    ~TActivityTaskTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TActivityTaskTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TActivityTaskTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iActivityType, stData.m_iActivityID, stData.m_iTaskID);
        m_pRecord->SetProgress(stData.m_iProgress);
        m_pRecord->SetState(stData.m_cState);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
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
    const char& GetState() const
    {
        return m_stData.m_cState;
    }
    void AddState(char cState)
    {
        if (0 == cState)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetState(cState);
        m_stData.m_cState += cState;
    }
    void SetState(char cState)
    {
        if (m_stData.m_cState == cState)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cState - m_stData.m_cState;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetState(RealAdd);
        m_stData.m_cState = cState;
    }

    CRecordActivityTaskTable* m_pRecord;
    TActivityTaskTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordActivityTaskTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordActivityTaskTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iActivityType, m_stData.m_iActivityID, m_stData.m_iTaskID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordActivityTaskTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
