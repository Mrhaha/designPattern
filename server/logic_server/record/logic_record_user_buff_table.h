#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserBuffTable;
struct TUserBuffTableValueType;

class CRecordUserBuffTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserBuffTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserBuffTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetEffectIndex();

    static std::vector<TUserBuffTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strBuffType;
    static const char* s_strBuffPara;
    static const char* s_strBuffParaEx;
    static const char* s_strEndTime;
    static const char* s_strEffectTimes;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strEffectIndex;

    TUserBuffTable* m_pCache;

    void SetBuffType(int iBuffType)
    {
        m_stIntData[s_strBuffType] += iBuffType;
    }
    void SetBuffPara(int iBuffPara)
    {
        m_stIntData[s_strBuffPara] += iBuffPara;
    }
    void SetBuffParaEx(int iBuffParaEx)
    {
        m_stIntData[s_strBuffParaEx] += iBuffParaEx;
    }
    void SetEndTime(int iEndTime)
    {
        m_stIntData[s_strEndTime] += iEndTime;
    }
    void SetEffectTimes(int iEffectTimes)
    {
        m_stIntData[s_strEffectTimes] += iEffectTimes;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iEffectIndex)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iEffectIndex = iEffectIndex;
    }
    void SetCachePtr(TUserBuffTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iEffectIndex;
};

struct TUserBuffTableValueType
{
    TUserBuffTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iEffectIndex(0)
    , m_iBuffType(0)
    , m_iBuffPara(0)
    , m_iBuffParaEx(0)
    , m_iEndTime(0)
    , m_iEffectTimes(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iEffectIndex;
    int                     m_iBuffType;
    int                     m_iBuffPara;
    int                     m_iBuffParaEx;
    int                     m_iEndTime;
    int                     m_iEffectTimes;
};

struct TUserBuffTable
{
    TUserBuffTable() : m_pRecord(nullptr) { }
    ~TUserBuffTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserBuffTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserBuffTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iEffectIndex);
        m_pRecord->SetBuffType(stData.m_iBuffType);
        m_pRecord->SetBuffPara(stData.m_iBuffPara);
        m_pRecord->SetBuffParaEx(stData.m_iBuffParaEx);
        m_pRecord->SetEndTime(stData.m_iEndTime);
        m_pRecord->SetEffectTimes(stData.m_iEffectTimes);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetBuffType() const
    {
        return m_stData.m_iBuffType;
    }
    void AddBuffType(int iBuffType)
    {
        if (0 == iBuffType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuffType(iBuffType);
        m_stData.m_iBuffType += iBuffType;
    }
    void SetBuffType(int iBuffType)
    {
        if (m_stData.m_iBuffType == iBuffType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuffType - m_stData.m_iBuffType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuffType(RealAdd);
        m_stData.m_iBuffType = iBuffType;
    }
    const int& GetBuffPara() const
    {
        return m_stData.m_iBuffPara;
    }
    void AddBuffPara(int iBuffPara)
    {
        if (0 == iBuffPara)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuffPara(iBuffPara);
        m_stData.m_iBuffPara += iBuffPara;
    }
    void SetBuffPara(int iBuffPara)
    {
        if (m_stData.m_iBuffPara == iBuffPara)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuffPara - m_stData.m_iBuffPara;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuffPara(RealAdd);
        m_stData.m_iBuffPara = iBuffPara;
    }
    const int& GetBuffParaEx() const
    {
        return m_stData.m_iBuffParaEx;
    }
    void AddBuffParaEx(int iBuffParaEx)
    {
        if (0 == iBuffParaEx)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuffParaEx(iBuffParaEx);
        m_stData.m_iBuffParaEx += iBuffParaEx;
    }
    void SetBuffParaEx(int iBuffParaEx)
    {
        if (m_stData.m_iBuffParaEx == iBuffParaEx)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuffParaEx - m_stData.m_iBuffParaEx;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuffParaEx(RealAdd);
        m_stData.m_iBuffParaEx = iBuffParaEx;
    }
    const int& GetEndTime() const
    {
        return m_stData.m_iEndTime;
    }
    void AddEndTime(int iEndTime)
    {
        if (0 == iEndTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEndTime(iEndTime);
        m_stData.m_iEndTime += iEndTime;
    }
    void SetEndTime(int iEndTime)
    {
        if (m_stData.m_iEndTime == iEndTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEndTime - m_stData.m_iEndTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEndTime(RealAdd);
        m_stData.m_iEndTime = iEndTime;
    }
    const int& GetEffectTimes() const
    {
        return m_stData.m_iEffectTimes;
    }
    void AddEffectTimes(int iEffectTimes)
    {
        if (0 == iEffectTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEffectTimes(iEffectTimes);
        m_stData.m_iEffectTimes += iEffectTimes;
    }
    void SetEffectTimes(int iEffectTimes)
    {
        if (m_stData.m_iEffectTimes == iEffectTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEffectTimes - m_stData.m_iEffectTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEffectTimes(RealAdd);
        m_stData.m_iEffectTimes = iEffectTimes;
    }

    CRecordUserBuffTable* m_pRecord;
    TUserBuffTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserBuffTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserBuffTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iEffectIndex);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserBuffTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
