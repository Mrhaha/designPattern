#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TServerNickTable;
struct TServerNickTableValueType;

class CRecordServerNickTable : public CLogicSQLTableRecordBase
{
public:
    CRecordServerNickTable() : m_pCache(nullptr) { }

    virtual ~CRecordServerNickTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();

    static std::vector<TServerNickTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const char* s_strTableName;
    static const char* s_strNick;
    static const char* s_strUid;
    static const char* s_strGroupID;

    TServerNickTable* m_pCache;

    void SetNick(const std::string& strNick)
    {
        m_stStringData[s_strNick] = strNick;
    }
    void SetPrimaryKey(int iUid, int iGroupID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TServerNickTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
};

struct TServerNickTableValueType
{
    TServerNickTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_strNick("")
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    std::string             m_strNick;
};

struct TServerNickTable
{
    TServerNickTable() : m_pRecord(nullptr) { }
    ~TServerNickTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TServerNickTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TServerNickTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID);
        m_pRecord->SetNick(stData.m_strNick);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const std::string& GetNick() const
    {
        return m_stData.m_strNick;
    }
    void SetNick(const std::string& strNick)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNick(strNick);
        m_stData.m_strNick = strNick;
    }

    CRecordServerNickTable* m_pRecord;
    TServerNickTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordServerNickTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordServerNickTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordServerNickTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
