#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TMutiStepTaskTable;
struct TMutiStepTaskTableValueType;

class CRecordMutiStepTaskTable : public CLogicSQLTableRecordBase
{
public:
    CRecordMutiStepTaskTable() : m_pCache(nullptr) { }

    virtual ~CRecordMutiStepTaskTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetTaskID();

    static std::vector<TMutiStepTaskTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strTaskStep;
    static const char* s_strTargetNum;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strTaskID;

    TMutiStepTaskTable* m_pCache;

    void SetTaskStep(int iTaskStep)
    {
        m_stIntData[s_strTaskStep] += iTaskStep;
    }
    void SetTargetNum(int iTargetNum)
    {
        m_stIntData[s_strTargetNum] += iTargetNum;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iTaskID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iTaskID = iTaskID;
    }
    void SetCachePtr(TMutiStepTaskTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iTaskID;
};

struct TMutiStepTaskTableValueType
{
    TMutiStepTaskTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iTaskID(0)
    , m_iTaskStep(1)
    , m_iTargetNum(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iTaskID;
    int                     m_iTaskStep;
    int                     m_iTargetNum;
};

struct TMutiStepTaskTable
{
    TMutiStepTaskTable() : m_pRecord(nullptr) { }
    ~TMutiStepTaskTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TMutiStepTaskTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TMutiStepTaskTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iTaskID);
        m_pRecord->SetTaskStep(stData.m_iTaskStep);
        m_pRecord->SetTargetNum(stData.m_iTargetNum);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetTaskStep() const
    {
        return m_stData.m_iTaskStep;
    }
    void AddTaskStep(int iTaskStep)
    {
        if (0 == iTaskStep)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskStep(iTaskStep);
        m_stData.m_iTaskStep += iTaskStep;
    }
    void SetTaskStep(int iTaskStep)
    {
        if (m_stData.m_iTaskStep == iTaskStep)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTaskStep - m_stData.m_iTaskStep;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskStep(RealAdd);
        m_stData.m_iTaskStep = iTaskStep;
    }
    const int& GetTargetNum() const
    {
        return m_stData.m_iTargetNum;
    }
    void AddTargetNum(int iTargetNum)
    {
        if (0 == iTargetNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTargetNum(iTargetNum);
        m_stData.m_iTargetNum += iTargetNum;
    }
    void SetTargetNum(int iTargetNum)
    {
        if (m_stData.m_iTargetNum == iTargetNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTargetNum - m_stData.m_iTargetNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTargetNum(RealAdd);
        m_stData.m_iTargetNum = iTargetNum;
    }

    CRecordMutiStepTaskTable* m_pRecord;
    TMutiStepTaskTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordMutiStepTaskTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordMutiStepTaskTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iTaskID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordMutiStepTaskTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
