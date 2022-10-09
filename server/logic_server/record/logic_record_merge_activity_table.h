#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TMergeActivityTable;
struct TMergeActivityTableValueType;

class CRecordMergeActivityTable : public CLogicSQLTableRecordBase
{
public:
    CRecordMergeActivityTable() : m_pCache(nullptr) { }

    virtual ~CRecordMergeActivityTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetTaskID();

    static std::vector<TMergeActivityTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strProgress;
    static const char* s_strAward;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strTaskID;

    TMergeActivityTable* m_pCache;

    void SetProgress(int iProgress)
    {
        m_stIntData[s_strProgress] += iProgress;
    }
    void SetAward(char cAward)
    {
        m_stCharData[s_strAward] += cAward;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iTaskID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iTaskID = iTaskID;
    }
    void SetCachePtr(TMergeActivityTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iTaskID;
};

struct TMergeActivityTableValueType
{
    TMergeActivityTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iTaskID(0)
    , m_iProgress(0)
    , m_cAward(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iTaskID;
    int                     m_iProgress;
    char                    m_cAward;
};

struct TMergeActivityTable
{
    TMergeActivityTable() : m_pRecord(nullptr) { }
    ~TMergeActivityTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TMergeActivityTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TMergeActivityTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iTaskID);
        m_pRecord->SetProgress(stData.m_iProgress);
        m_pRecord->SetAward(stData.m_cAward);
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
    const char& GetAward() const
    {
        return m_stData.m_cAward;
    }
    void AddAward(char cAward)
    {
        if (0 == cAward)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAward(cAward);
        m_stData.m_cAward += cAward;
    }
    void SetAward(char cAward)
    {
        if (m_stData.m_cAward == cAward)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cAward - m_stData.m_cAward;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAward(RealAdd);
        m_stData.m_cAward = cAward;
    }

    CRecordMergeActivityTable* m_pRecord;
    TMergeActivityTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordMergeActivityTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordMergeActivityTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iTaskID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordMergeActivityTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
