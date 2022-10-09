#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserHomeExploreTable;
struct TUserHomeExploreTableValueType;

class CRecordUserHomeExploreTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserHomeExploreTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserHomeExploreTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetExploreQuestID();

    static std::vector<TUserHomeExploreTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strTeam;
    static const char* s_strEndTime;
    static const char* s_strIsCbt;
    static const char* s_strState;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strExploreQuestID;

    TUserHomeExploreTable* m_pCache;

    void SetTeam(const CPackCardTeam& stTeam)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam);
        m_stVarData[s_strTeam] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetEndTime(int iEndTime)
    {
        m_stIntData[s_strEndTime] += iEndTime;
    }
    void SetIsCbt(char cIsCbt)
    {
        m_stCharData[s_strIsCbt] += cIsCbt;
    }
    void SetState(char cState)
    {
        m_stCharData[s_strState] += cState;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iExploreQuestID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iExploreQuestID = iExploreQuestID;
    }
    void SetCachePtr(TUserHomeExploreTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iExploreQuestID;
};

struct TUserHomeExploreTableValueType
{
    TUserHomeExploreTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iExploreQuestID(0)
    , m_stTeam()
    , m_iEndTime(0)
    , m_cIsCbt(0)
    , m_cState(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iExploreQuestID;
    CPackCardTeam           m_stTeam;
    int                     m_iEndTime;
    char                    m_cIsCbt;
    char                    m_cState;
};

struct TUserHomeExploreTable
{
    TUserHomeExploreTable() : m_pRecord(nullptr) { }
    ~TUserHomeExploreTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserHomeExploreTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserHomeExploreTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iExploreQuestID);
        m_pRecord->SetTeam(stData.m_stTeam);
        m_pRecord->SetEndTime(stData.m_iEndTime);
        m_pRecord->SetIsCbt(stData.m_cIsCbt);
        m_pRecord->SetState(stData.m_cState);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const CPackCardTeam& GetTeam() const
    {
        return m_stData.m_stTeam;
    }
    void SetTeam(const CPackCardTeam& stTeam)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam(stTeam);
        m_stData.m_stTeam = stTeam;
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
    const char& GetIsCbt() const
    {
        return m_stData.m_cIsCbt;
    }
    void AddIsCbt(char cIsCbt)
    {
        if (0 == cIsCbt)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsCbt(cIsCbt);
        m_stData.m_cIsCbt += cIsCbt;
    }
    void SetIsCbt(char cIsCbt)
    {
        if (m_stData.m_cIsCbt == cIsCbt)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cIsCbt - m_stData.m_cIsCbt;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsCbt(RealAdd);
        m_stData.m_cIsCbt = cIsCbt;
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

    CRecordUserHomeExploreTable* m_pRecord;
    TUserHomeExploreTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserHomeExploreTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserHomeExploreTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iExploreQuestID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserHomeExploreTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
