#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserHomeFootclothTable;
struct TUserHomeFootclothTableValueType;

class CRecordUserHomeFootclothTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserHomeFootclothTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserHomeFootclothTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetHomeID();
    virtual int GetFootclothID();

    static std::vector<TUserHomeFootclothTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strPosVec;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strHomeID;
    static const char* s_strFootclothID;

    TUserHomeFootclothTable* m_pCache;

    void SetPosVec(const std::map<int, char>& stPosVec)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stPosVec);
        m_stVarData[s_strPosVec] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iHomeID, int iFootclothID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iHomeID = iHomeID;
        m_iFootclothID = iFootclothID;
    }
    void SetCachePtr(TUserHomeFootclothTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iHomeID;
    int m_iFootclothID;
};

struct TUserHomeFootclothTableValueType
{
    TUserHomeFootclothTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iHomeID(0)
    , m_iFootclothID(0)
    , m_stPosVec()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iHomeID;
    int                     m_iFootclothID;
    std::map<int, char>     m_stPosVec;
};

struct TUserHomeFootclothTable
{
    TUserHomeFootclothTable() : m_pRecord(nullptr) { }
    ~TUserHomeFootclothTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserHomeFootclothTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserHomeFootclothTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iHomeID, stData.m_iFootclothID);
        m_pRecord->SetPosVec(stData.m_stPosVec);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const std::map<int, char>& GetPosVec() const
    {
        return m_stData.m_stPosVec;
    }
    void SetPosVec(const std::map<int, char>& stPosVec)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPosVec(stPosVec);
        m_stData.m_stPosVec = stPosVec;
    }

    CRecordUserHomeFootclothTable* m_pRecord;
    TUserHomeFootclothTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserHomeFootclothTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserHomeFootclothTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iHomeID, m_stData.m_iFootclothID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserHomeFootclothTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
