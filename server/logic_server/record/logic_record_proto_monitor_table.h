#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TProtoMonitorTable;
struct TProtoMonitorTableValueType;

class CRecordProtoMonitorTable : public CLogicSQLTableRecordBase
{
public:
    CRecordProtoMonitorTable() : m_pCache(nullptr) { }

    virtual ~CRecordProtoMonitorTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetCmd();

    static std::vector<TProtoMonitorTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strCurNum;
    static const char* s_strLastRecordTime;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strCmd;

    TProtoMonitorTable* m_pCache;

    void SetCurNum(int iCurNum)
    {
        m_stIntData[s_strCurNum] += iCurNum;
    }
    void SetLastRecordTime(int iLastRecordTime)
    {
        m_stIntData[s_strLastRecordTime] += iLastRecordTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iCmd)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iCmd = iCmd;
    }
    void SetCachePtr(TProtoMonitorTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iCmd;
};

struct TProtoMonitorTableValueType
{
    TProtoMonitorTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iCmd(0)
    , m_iCurNum(0)
    , m_iLastRecordTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iCmd;
    int                     m_iCurNum;
    int                     m_iLastRecordTime;
};

struct TProtoMonitorTable
{
    TProtoMonitorTable() : m_pRecord(nullptr) { }
    ~TProtoMonitorTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TProtoMonitorTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TProtoMonitorTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iCmd);
        m_pRecord->SetCurNum(stData.m_iCurNum);
        m_pRecord->SetLastRecordTime(stData.m_iLastRecordTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetCurNum() const
    {
        return m_stData.m_iCurNum;
    }
    void AddCurNum(int iCurNum)
    {
        if (0 == iCurNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCurNum(iCurNum);
        m_stData.m_iCurNum += iCurNum;
    }
    void SetCurNum(int iCurNum)
    {
        if (m_stData.m_iCurNum == iCurNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCurNum - m_stData.m_iCurNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCurNum(RealAdd);
        m_stData.m_iCurNum = iCurNum;
    }
    const int& GetLastRecordTime() const
    {
        return m_stData.m_iLastRecordTime;
    }
    void AddLastRecordTime(int iLastRecordTime)
    {
        if (0 == iLastRecordTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastRecordTime(iLastRecordTime);
        m_stData.m_iLastRecordTime += iLastRecordTime;
    }
    void SetLastRecordTime(int iLastRecordTime)
    {
        if (m_stData.m_iLastRecordTime == iLastRecordTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastRecordTime - m_stData.m_iLastRecordTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastRecordTime(RealAdd);
        m_stData.m_iLastRecordTime = iLastRecordTime;
    }

    CRecordProtoMonitorTable* m_pRecord;
    TProtoMonitorTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordProtoMonitorTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordProtoMonitorTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iCmd);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordProtoMonitorTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
