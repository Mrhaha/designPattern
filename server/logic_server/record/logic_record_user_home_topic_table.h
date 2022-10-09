#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserHomeTopicTable;
struct TUserHomeTopicTableValueType;

class CRecordUserHomeTopicTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserHomeTopicTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserHomeTopicTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetHomeTopicID();

    static std::vector<TUserHomeTopicTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strHomeTopicInfo;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strHomeTopicID;

    TUserHomeTopicTable* m_pCache;

    void SetHomeTopicInfo(const CPackHomeTopicElem& stHomeTopicInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stHomeTopicInfo);
        m_stVarData[s_strHomeTopicInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iHomeTopicID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iHomeTopicID = iHomeTopicID;
    }
    void SetCachePtr(TUserHomeTopicTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iHomeTopicID;
};

struct TUserHomeTopicTableValueType
{
    TUserHomeTopicTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iHomeTopicID(0)
    , m_stHomeTopicInfo()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iHomeTopicID;
    CPackHomeTopicElem      m_stHomeTopicInfo;
};

struct TUserHomeTopicTable
{
    TUserHomeTopicTable() : m_pRecord(nullptr) { }
    ~TUserHomeTopicTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserHomeTopicTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserHomeTopicTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iHomeTopicID);
        m_pRecord->SetHomeTopicInfo(stData.m_stHomeTopicInfo);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const CPackHomeTopicElem& GetHomeTopicInfo() const
    {
        return m_stData.m_stHomeTopicInfo;
    }
    void SetHomeTopicInfo(const CPackHomeTopicElem& stHomeTopicInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHomeTopicInfo(stHomeTopicInfo);
        m_stData.m_stHomeTopicInfo = stHomeTopicInfo;
    }

    CRecordUserHomeTopicTable* m_pRecord;
    TUserHomeTopicTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserHomeTopicTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserHomeTopicTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iHomeTopicID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserHomeTopicTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
