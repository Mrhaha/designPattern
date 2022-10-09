#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserHomeGardenTable;
struct TUserHomeGardenTableValueType;

class CRecordUserHomeGardenTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserHomeGardenTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserHomeGardenTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetHomeID();
    virtual int GetLandID();

    static std::vector<TUserHomeGardenTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strSeedID;
    static const char* s_strSeedStat;
    static const char* s_strStatTime;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strHomeID;
    static const char* s_strLandID;

    TUserHomeGardenTable* m_pCache;

    void SetSeedID(int iSeedID)
    {
        m_stIntData[s_strSeedID] += iSeedID;
    }
    void SetSeedStat(int iSeedStat)
    {
        m_stIntData[s_strSeedStat] += iSeedStat;
    }
    void SetStatTime(int iStatTime)
    {
        m_stIntData[s_strStatTime] += iStatTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iHomeID, int iLandID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iHomeID = iHomeID;
        m_iLandID = iLandID;
    }
    void SetCachePtr(TUserHomeGardenTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iHomeID;
    int m_iLandID;
};

struct TUserHomeGardenTableValueType
{
    TUserHomeGardenTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iHomeID(0)
    , m_iLandID(0)
    , m_iSeedID(0)
    , m_iSeedStat(0)
    , m_iStatTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iHomeID;
    int                     m_iLandID;
    int                     m_iSeedID;
    int                     m_iSeedStat;
    int                     m_iStatTime;
};

struct TUserHomeGardenTable
{
    TUserHomeGardenTable() : m_pRecord(nullptr) { }
    ~TUserHomeGardenTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserHomeGardenTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserHomeGardenTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iHomeID, stData.m_iLandID);
        m_pRecord->SetSeedID(stData.m_iSeedID);
        m_pRecord->SetSeedStat(stData.m_iSeedStat);
        m_pRecord->SetStatTime(stData.m_iStatTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetSeedID() const
    {
        return m_stData.m_iSeedID;
    }
    void AddSeedID(int iSeedID)
    {
        if (0 == iSeedID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSeedID(iSeedID);
        m_stData.m_iSeedID += iSeedID;
    }
    void SetSeedID(int iSeedID)
    {
        if (m_stData.m_iSeedID == iSeedID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSeedID - m_stData.m_iSeedID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSeedID(RealAdd);
        m_stData.m_iSeedID = iSeedID;
    }
    const int& GetSeedStat() const
    {
        return m_stData.m_iSeedStat;
    }
    void AddSeedStat(int iSeedStat)
    {
        if (0 == iSeedStat)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSeedStat(iSeedStat);
        m_stData.m_iSeedStat += iSeedStat;
    }
    void SetSeedStat(int iSeedStat)
    {
        if (m_stData.m_iSeedStat == iSeedStat)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSeedStat - m_stData.m_iSeedStat;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSeedStat(RealAdd);
        m_stData.m_iSeedStat = iSeedStat;
    }
    const int& GetStatTime() const
    {
        return m_stData.m_iStatTime;
    }
    void AddStatTime(int iStatTime)
    {
        if (0 == iStatTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetStatTime(iStatTime);
        m_stData.m_iStatTime += iStatTime;
    }
    void SetStatTime(int iStatTime)
    {
        if (m_stData.m_iStatTime == iStatTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iStatTime - m_stData.m_iStatTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetStatTime(RealAdd);
        m_stData.m_iStatTime = iStatTime;
    }

    CRecordUserHomeGardenTable* m_pRecord;
    TUserHomeGardenTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserHomeGardenTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserHomeGardenTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iHomeID, m_stData.m_iLandID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserHomeGardenTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
