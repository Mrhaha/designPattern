#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TCrossFightInfoTable;
struct TCrossFightInfoTableValueType;

class CRecordCrossFightInfoTable : public CLogicSQLTableRecordBase
{
public:
    CRecordCrossFightInfoTable() : m_pCache(nullptr) { }

    virtual ~CRecordCrossFightInfoTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetTeamID();

    static std::vector<TCrossFightInfoTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strPassed;
    static const char* s_strFirstPass;
    static const char* s_strPlayer;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strTeamID;

    TCrossFightInfoTable* m_pCache;

    void SetPassed(int iPassed)
    {
        m_stIntData[s_strPassed] += iPassed;
    }
    void SetFirstPass(int iFirstPass)
    {
        m_stIntData[s_strFirstPass] += iFirstPass;
    }
    void SetPlayer(const CPackPvpChallengerInfo& stPlayer)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stPlayer);
        m_stVarData[s_strPlayer] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iTeamID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iTeamID = iTeamID;
    }
    void SetCachePtr(TCrossFightInfoTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iTeamID;
};

struct TCrossFightInfoTableValueType
{
    TCrossFightInfoTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iTeamID(0)
    , m_iPassed(0)
    , m_iFirstPass(0)
    , m_stPlayer()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iTeamID;
    int                     m_iPassed;
    int                     m_iFirstPass;
    CPackPvpChallengerInfo  m_stPlayer;
};

struct TCrossFightInfoTable
{
    TCrossFightInfoTable() : m_pRecord(nullptr) { }
    ~TCrossFightInfoTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TCrossFightInfoTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TCrossFightInfoTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iTeamID);
        m_pRecord->SetPassed(stData.m_iPassed);
        m_pRecord->SetFirstPass(stData.m_iFirstPass);
        m_pRecord->SetPlayer(stData.m_stPlayer);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetPassed() const
    {
        return m_stData.m_iPassed;
    }
    void AddPassed(int iPassed)
    {
        if (0 == iPassed)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPassed(iPassed);
        m_stData.m_iPassed += iPassed;
    }
    void SetPassed(int iPassed)
    {
        if (m_stData.m_iPassed == iPassed)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPassed - m_stData.m_iPassed;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPassed(RealAdd);
        m_stData.m_iPassed = iPassed;
    }
    const int& GetFirstPass() const
    {
        return m_stData.m_iFirstPass;
    }
    void AddFirstPass(int iFirstPass)
    {
        if (0 == iFirstPass)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFirstPass(iFirstPass);
        m_stData.m_iFirstPass += iFirstPass;
    }
    void SetFirstPass(int iFirstPass)
    {
        if (m_stData.m_iFirstPass == iFirstPass)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFirstPass - m_stData.m_iFirstPass;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFirstPass(RealAdd);
        m_stData.m_iFirstPass = iFirstPass;
    }
    const CPackPvpChallengerInfo& GetPlayer() const
    {
        return m_stData.m_stPlayer;
    }
    void SetPlayer(const CPackPvpChallengerInfo& stPlayer)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPlayer(stPlayer);
        m_stData.m_stPlayer = stPlayer;
    }

    CRecordCrossFightInfoTable* m_pRecord;
    TCrossFightInfoTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordCrossFightInfoTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordCrossFightInfoTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iTeamID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordCrossFightInfoTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
