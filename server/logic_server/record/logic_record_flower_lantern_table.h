#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TFlowerLanternTable;
struct TFlowerLanternTableValueType;

class CRecordFlowerLanternTable : public CLogicSQLTableRecordBase
{
public:
    CRecordFlowerLanternTable() : m_pCache(nullptr) { }

    virtual ~CRecordFlowerLanternTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetLogIndex();

    static std::vector<TFlowerLanternTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strLogTime;
    static const char* s_strActivityID;
    static const char* s_strEvent;
    static const char* s_strEventValue;
    static const char* s_strCandle;
    static const char* s_strTotalValue;
    static const char* s_strTotalCandle;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strLogIndex;

    TFlowerLanternTable* m_pCache;

    void SetLogTime(int iLogTime)
    {
        m_stIntData[s_strLogTime] += iLogTime;
    }
    void SetActivityID(int iActivityID)
    {
        m_stIntData[s_strActivityID] += iActivityID;
    }
    void SetEvent(int iEvent)
    {
        m_stIntData[s_strEvent] += iEvent;
    }
    void SetEventValue(int iEventValue)
    {
        m_stIntData[s_strEventValue] += iEventValue;
    }
    void SetCandle(int iCandle)
    {
        m_stIntData[s_strCandle] += iCandle;
    }
    void SetTotalValue(int iTotalValue)
    {
        m_stIntData[s_strTotalValue] += iTotalValue;
    }
    void SetTotalCandle(int iTotalCandle)
    {
        m_stIntData[s_strTotalCandle] += iTotalCandle;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iLogIndex)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iLogIndex = iLogIndex;
    }
    void SetCachePtr(TFlowerLanternTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iLogIndex;
};

struct TFlowerLanternTableValueType
{
    TFlowerLanternTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iLogIndex(0)
    , m_iLogTime(0)
    , m_iActivityID(0)
    , m_iEvent(0)
    , m_iEventValue(0)
    , m_iCandle(0)
    , m_iTotalValue(0)
    , m_iTotalCandle(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iLogIndex;
    int                     m_iLogTime;
    int                     m_iActivityID;
    int                     m_iEvent;
    int                     m_iEventValue;
    int                     m_iCandle;
    int                     m_iTotalValue;
    int                     m_iTotalCandle;
};

struct TFlowerLanternTable
{
    TFlowerLanternTable() : m_pRecord(nullptr) { }
    ~TFlowerLanternTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TFlowerLanternTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TFlowerLanternTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iLogIndex);
        m_pRecord->SetLogTime(stData.m_iLogTime);
        m_pRecord->SetActivityID(stData.m_iActivityID);
        m_pRecord->SetEvent(stData.m_iEvent);
        m_pRecord->SetEventValue(stData.m_iEventValue);
        m_pRecord->SetCandle(stData.m_iCandle);
        m_pRecord->SetTotalValue(stData.m_iTotalValue);
        m_pRecord->SetTotalCandle(stData.m_iTotalCandle);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetLogTime() const
    {
        return m_stData.m_iLogTime;
    }
    void AddLogTime(int iLogTime)
    {
        if (0 == iLogTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLogTime(iLogTime);
        m_stData.m_iLogTime += iLogTime;
    }
    void SetLogTime(int iLogTime)
    {
        if (m_stData.m_iLogTime == iLogTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLogTime - m_stData.m_iLogTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLogTime(RealAdd);
        m_stData.m_iLogTime = iLogTime;
    }
    const int& GetActivityID() const
    {
        return m_stData.m_iActivityID;
    }
    void AddActivityID(int iActivityID)
    {
        if (0 == iActivityID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActivityID(iActivityID);
        m_stData.m_iActivityID += iActivityID;
    }
    void SetActivityID(int iActivityID)
    {
        if (m_stData.m_iActivityID == iActivityID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iActivityID - m_stData.m_iActivityID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActivityID(RealAdd);
        m_stData.m_iActivityID = iActivityID;
    }
    const int& GetEvent() const
    {
        return m_stData.m_iEvent;
    }
    void AddEvent(int iEvent)
    {
        if (0 == iEvent)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEvent(iEvent);
        m_stData.m_iEvent += iEvent;
    }
    void SetEvent(int iEvent)
    {
        if (m_stData.m_iEvent == iEvent)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEvent - m_stData.m_iEvent;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEvent(RealAdd);
        m_stData.m_iEvent = iEvent;
    }
    const int& GetEventValue() const
    {
        return m_stData.m_iEventValue;
    }
    void AddEventValue(int iEventValue)
    {
        if (0 == iEventValue)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEventValue(iEventValue);
        m_stData.m_iEventValue += iEventValue;
    }
    void SetEventValue(int iEventValue)
    {
        if (m_stData.m_iEventValue == iEventValue)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEventValue - m_stData.m_iEventValue;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEventValue(RealAdd);
        m_stData.m_iEventValue = iEventValue;
    }
    const int& GetCandle() const
    {
        return m_stData.m_iCandle;
    }
    void AddCandle(int iCandle)
    {
        if (0 == iCandle)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCandle(iCandle);
        m_stData.m_iCandle += iCandle;
    }
    void SetCandle(int iCandle)
    {
        if (m_stData.m_iCandle == iCandle)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCandle - m_stData.m_iCandle;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCandle(RealAdd);
        m_stData.m_iCandle = iCandle;
    }
    const int& GetTotalValue() const
    {
        return m_stData.m_iTotalValue;
    }
    void AddTotalValue(int iTotalValue)
    {
        if (0 == iTotalValue)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalValue(iTotalValue);
        m_stData.m_iTotalValue += iTotalValue;
    }
    void SetTotalValue(int iTotalValue)
    {
        if (m_stData.m_iTotalValue == iTotalValue)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTotalValue - m_stData.m_iTotalValue;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalValue(RealAdd);
        m_stData.m_iTotalValue = iTotalValue;
    }
    const int& GetTotalCandle() const
    {
        return m_stData.m_iTotalCandle;
    }
    void AddTotalCandle(int iTotalCandle)
    {
        if (0 == iTotalCandle)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalCandle(iTotalCandle);
        m_stData.m_iTotalCandle += iTotalCandle;
    }
    void SetTotalCandle(int iTotalCandle)
    {
        if (m_stData.m_iTotalCandle == iTotalCandle)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTotalCandle - m_stData.m_iTotalCandle;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalCandle(RealAdd);
        m_stData.m_iTotalCandle = iTotalCandle;
    }

    CRecordFlowerLanternTable* m_pRecord;
    TFlowerLanternTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordFlowerLanternTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordFlowerLanternTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iLogIndex);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordFlowerLanternTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
