#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserThemeActivityChargeTable;
struct TUserThemeActivityChargeTableValueType;

class CRecordLogicThemeActivityCharge : public CLogicSQLTableRecordBase
{
public:
    CRecordLogicThemeActivityCharge() : m_pCache(nullptr) { }

    virtual ~CRecordLogicThemeActivityCharge();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetActivityID();
    virtual int GetTaskID();

    static std::vector<TUserThemeActivityChargeTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strTaskType;
    static const char* s_strTargetType;
    static const char* s_strTargetID;
    static const char* s_strTargetAmount;
    static const char* s_strProgress;
    static const char* s_strState;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strActivityID;
    static const char* s_strTaskID;

    TUserThemeActivityChargeTable* m_pCache;

    void SetTaskType(int iTaskType)
    {
        m_stIntData[s_strTaskType] += iTaskType;
    }
    void SetTargetType(int iTargetType)
    {
        m_stIntData[s_strTargetType] += iTargetType;
    }
    void SetTargetID(int iTargetID)
    {
        m_stIntData[s_strTargetID] += iTargetID;
    }
    void SetTargetAmount(int iTargetAmount)
    {
        m_stIntData[s_strTargetAmount] += iTargetAmount;
    }
    void SetProgress(int iProgress)
    {
        m_stIntData[s_strProgress] += iProgress;
    }
    void SetState(char cState)
    {
        m_stCharData[s_strState] += cState;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iActivityID, int iTaskID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iActivityID = iActivityID;
        m_iTaskID = iTaskID;
    }
    void SetCachePtr(TUserThemeActivityChargeTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iActivityID;
    int m_iTaskID;
};

struct TUserThemeActivityChargeTableValueType
{
    TUserThemeActivityChargeTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iActivityID(0)
    , m_iTaskID(0)
    , m_iTaskType(0)
    , m_iTargetType(0)
    , m_iTargetID(0)
    , m_iTargetAmount(0)
    , m_iProgress(0)
    , m_cState(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iActivityID;
    int                     m_iTaskID;
    int                     m_iTaskType;
    int                     m_iTargetType;
    int                     m_iTargetID;
    int                     m_iTargetAmount;
    int                     m_iProgress;
    char                    m_cState;
};

struct TUserThemeActivityChargeTable
{
    TUserThemeActivityChargeTable() : m_pRecord(nullptr) { }
    ~TUserThemeActivityChargeTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserThemeActivityChargeTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserThemeActivityChargeTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iActivityID, stData.m_iTaskID);
        m_pRecord->SetTaskType(stData.m_iTaskType);
        m_pRecord->SetTargetType(stData.m_iTargetType);
        m_pRecord->SetTargetID(stData.m_iTargetID);
        m_pRecord->SetTargetAmount(stData.m_iTargetAmount);
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
    const int& GetTaskType() const
    {
        return m_stData.m_iTaskType;
    }
    void AddTaskType(int iTaskType)
    {
        if (0 == iTaskType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskType(iTaskType);
        m_stData.m_iTaskType += iTaskType;
    }
    void SetTaskType(int iTaskType)
    {
        if (m_stData.m_iTaskType == iTaskType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTaskType - m_stData.m_iTaskType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskType(RealAdd);
        m_stData.m_iTaskType = iTaskType;
    }
    const int& GetTargetType() const
    {
        return m_stData.m_iTargetType;
    }
    void AddTargetType(int iTargetType)
    {
        if (0 == iTargetType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTargetType(iTargetType);
        m_stData.m_iTargetType += iTargetType;
    }
    void SetTargetType(int iTargetType)
    {
        if (m_stData.m_iTargetType == iTargetType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTargetType - m_stData.m_iTargetType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTargetType(RealAdd);
        m_stData.m_iTargetType = iTargetType;
    }
    const int& GetTargetID() const
    {
        return m_stData.m_iTargetID;
    }
    void AddTargetID(int iTargetID)
    {
        if (0 == iTargetID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTargetID(iTargetID);
        m_stData.m_iTargetID += iTargetID;
    }
    void SetTargetID(int iTargetID)
    {
        if (m_stData.m_iTargetID == iTargetID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTargetID - m_stData.m_iTargetID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTargetID(RealAdd);
        m_stData.m_iTargetID = iTargetID;
    }
    const int& GetTargetAmount() const
    {
        return m_stData.m_iTargetAmount;
    }
    void AddTargetAmount(int iTargetAmount)
    {
        if (0 == iTargetAmount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTargetAmount(iTargetAmount);
        m_stData.m_iTargetAmount += iTargetAmount;
    }
    void SetTargetAmount(int iTargetAmount)
    {
        if (m_stData.m_iTargetAmount == iTargetAmount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTargetAmount - m_stData.m_iTargetAmount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTargetAmount(RealAdd);
        m_stData.m_iTargetAmount = iTargetAmount;
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

    CRecordLogicThemeActivityCharge* m_pRecord;
    TUserThemeActivityChargeTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordLogicThemeActivityCharge* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordLogicThemeActivityCharge;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iActivityID, m_stData.m_iTaskID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordLogicThemeActivityCharge>(m_pRecord));
        }
        return m_pRecord;
    }
};
