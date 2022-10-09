#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserTeamPresetInfoTable;
struct TUserTeamPresetInfoTableValueType;

class CRecordUserTeamPresetInfoTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserTeamPresetInfoTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserTeamPresetInfoTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetTeamIndex();

    static std::vector<TUserTeamPresetInfoTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strName;
    static const char* s_strTeam;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strTeamIndex;

    TUserTeamPresetInfoTable* m_pCache;

    void SetName(const std::string& strName)
    {
        m_stStringData[s_strName] = strName;
    }
    void SetTeam(const CPackCardTeam& stTeam)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam);
        m_stVarData[s_strTeam] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iTeamIndex)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iTeamIndex = iTeamIndex;
    }
    void SetCachePtr(TUserTeamPresetInfoTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iTeamIndex;
};

struct TUserTeamPresetInfoTableValueType
{
    TUserTeamPresetInfoTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iTeamIndex(0)
    , m_strName("")
    , m_stTeam()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iTeamIndex;
    std::string             m_strName;
    CPackCardTeam           m_stTeam;
};

struct TUserTeamPresetInfoTable
{
    TUserTeamPresetInfoTable() : m_pRecord(nullptr) { }
    ~TUserTeamPresetInfoTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserTeamPresetInfoTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserTeamPresetInfoTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iTeamIndex);
        m_pRecord->SetName(stData.m_strName);
        m_pRecord->SetTeam(stData.m_stTeam);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const std::string& GetName() const
    {
        return m_stData.m_strName;
    }
    void SetName(const std::string& strName)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetName(strName);
        m_stData.m_strName = strName;
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

    CRecordUserTeamPresetInfoTable* m_pRecord;
    TUserTeamPresetInfoTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserTeamPresetInfoTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserTeamPresetInfoTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iTeamIndex);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserTeamPresetInfoTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
