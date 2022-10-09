#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserHonorTable;
struct TUserHonorTableValueType;

class CRecordUserHonorTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserHonorTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserHonorTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetHonorID();

    static std::vector<TUserHonorTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strHonorStatus;
    static const char* s_strExpireTime;
    static const char* s_strTaskProgress1;
    static const char* s_strTaskProgress2;
    static const char* s_strTaskProgress3;
    static const char* s_strTaskProgress4;
    static const char* s_strTaskProgress5;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strHonorID;

    TUserHonorTable* m_pCache;

    void SetHonorStatus(int iHonorStatus)
    {
        m_stIntData[s_strHonorStatus] += iHonorStatus;
    }
    void SetExpireTime(int iExpireTime)
    {
        m_stIntData[s_strExpireTime] += iExpireTime;
    }
    void SetTaskProgress1(int iTaskProgress1)
    {
        m_stIntData[s_strTaskProgress1] += iTaskProgress1;
    }
    void SetTaskProgress2(int iTaskProgress2)
    {
        m_stIntData[s_strTaskProgress2] += iTaskProgress2;
    }
    void SetTaskProgress3(int iTaskProgress3)
    {
        m_stIntData[s_strTaskProgress3] += iTaskProgress3;
    }
    void SetTaskProgress4(int iTaskProgress4)
    {
        m_stIntData[s_strTaskProgress4] += iTaskProgress4;
    }
    void SetTaskProgress5(int iTaskProgress5)
    {
        m_stIntData[s_strTaskProgress5] += iTaskProgress5;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iHonorID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iHonorID = iHonorID;
    }
    void SetCachePtr(TUserHonorTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iHonorID;
};

struct TUserHonorTableValueType
{
    TUserHonorTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iHonorID(0)
    , m_iHonorStatus(0)
    , m_iExpireTime(0)
    , m_iTaskProgress1(0)
    , m_iTaskProgress2(0)
    , m_iTaskProgress3(0)
    , m_iTaskProgress4(0)
    , m_iTaskProgress5(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iHonorID;
    int                     m_iHonorStatus;
    int                     m_iExpireTime;
    int                     m_iTaskProgress1;
    int                     m_iTaskProgress2;
    int                     m_iTaskProgress3;
    int                     m_iTaskProgress4;
    int                     m_iTaskProgress5;
};

struct TUserHonorTable
{
    TUserHonorTable() : m_pRecord(nullptr) { }
    ~TUserHonorTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserHonorTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserHonorTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iHonorID);
        m_pRecord->SetHonorStatus(stData.m_iHonorStatus);
        m_pRecord->SetExpireTime(stData.m_iExpireTime);
        m_pRecord->SetTaskProgress1(stData.m_iTaskProgress1);
        m_pRecord->SetTaskProgress2(stData.m_iTaskProgress2);
        m_pRecord->SetTaskProgress3(stData.m_iTaskProgress3);
        m_pRecord->SetTaskProgress4(stData.m_iTaskProgress4);
        m_pRecord->SetTaskProgress5(stData.m_iTaskProgress5);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetHonorStatus() const
    {
        return m_stData.m_iHonorStatus;
    }
    void AddHonorStatus(int iHonorStatus)
    {
        if (0 == iHonorStatus)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHonorStatus(iHonorStatus);
        m_stData.m_iHonorStatus += iHonorStatus;
    }
    void SetHonorStatus(int iHonorStatus)
    {
        if (m_stData.m_iHonorStatus == iHonorStatus)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iHonorStatus - m_stData.m_iHonorStatus;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHonorStatus(RealAdd);
        m_stData.m_iHonorStatus = iHonorStatus;
    }
    const int& GetExpireTime() const
    {
        return m_stData.m_iExpireTime;
    }
    void AddExpireTime(int iExpireTime)
    {
        if (0 == iExpireTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExpireTime(iExpireTime);
        m_stData.m_iExpireTime += iExpireTime;
    }
    void SetExpireTime(int iExpireTime)
    {
        if (m_stData.m_iExpireTime == iExpireTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iExpireTime - m_stData.m_iExpireTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExpireTime(RealAdd);
        m_stData.m_iExpireTime = iExpireTime;
    }
    const int& GetTaskProgress1() const
    {
        return m_stData.m_iTaskProgress1;
    }
    void AddTaskProgress1(int iTaskProgress1)
    {
        if (0 == iTaskProgress1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskProgress1(iTaskProgress1);
        m_stData.m_iTaskProgress1 += iTaskProgress1;
    }
    void SetTaskProgress1(int iTaskProgress1)
    {
        if (m_stData.m_iTaskProgress1 == iTaskProgress1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTaskProgress1 - m_stData.m_iTaskProgress1;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskProgress1(RealAdd);
        m_stData.m_iTaskProgress1 = iTaskProgress1;
    }
    const int& GetTaskProgress2() const
    {
        return m_stData.m_iTaskProgress2;
    }
    void AddTaskProgress2(int iTaskProgress2)
    {
        if (0 == iTaskProgress2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskProgress2(iTaskProgress2);
        m_stData.m_iTaskProgress2 += iTaskProgress2;
    }
    void SetTaskProgress2(int iTaskProgress2)
    {
        if (m_stData.m_iTaskProgress2 == iTaskProgress2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTaskProgress2 - m_stData.m_iTaskProgress2;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskProgress2(RealAdd);
        m_stData.m_iTaskProgress2 = iTaskProgress2;
    }
    const int& GetTaskProgress3() const
    {
        return m_stData.m_iTaskProgress3;
    }
    void AddTaskProgress3(int iTaskProgress3)
    {
        if (0 == iTaskProgress3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskProgress3(iTaskProgress3);
        m_stData.m_iTaskProgress3 += iTaskProgress3;
    }
    void SetTaskProgress3(int iTaskProgress3)
    {
        if (m_stData.m_iTaskProgress3 == iTaskProgress3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTaskProgress3 - m_stData.m_iTaskProgress3;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskProgress3(RealAdd);
        m_stData.m_iTaskProgress3 = iTaskProgress3;
    }
    const int& GetTaskProgress4() const
    {
        return m_stData.m_iTaskProgress4;
    }
    void AddTaskProgress4(int iTaskProgress4)
    {
        if (0 == iTaskProgress4)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskProgress4(iTaskProgress4);
        m_stData.m_iTaskProgress4 += iTaskProgress4;
    }
    void SetTaskProgress4(int iTaskProgress4)
    {
        if (m_stData.m_iTaskProgress4 == iTaskProgress4)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTaskProgress4 - m_stData.m_iTaskProgress4;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskProgress4(RealAdd);
        m_stData.m_iTaskProgress4 = iTaskProgress4;
    }
    const int& GetTaskProgress5() const
    {
        return m_stData.m_iTaskProgress5;
    }
    void AddTaskProgress5(int iTaskProgress5)
    {
        if (0 == iTaskProgress5)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskProgress5(iTaskProgress5);
        m_stData.m_iTaskProgress5 += iTaskProgress5;
    }
    void SetTaskProgress5(int iTaskProgress5)
    {
        if (m_stData.m_iTaskProgress5 == iTaskProgress5)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTaskProgress5 - m_stData.m_iTaskProgress5;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskProgress5(RealAdd);
        m_stData.m_iTaskProgress5 = iTaskProgress5;
    }

    CRecordUserHonorTable* m_pRecord;
    TUserHonorTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserHonorTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserHonorTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iHonorID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserHonorTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
