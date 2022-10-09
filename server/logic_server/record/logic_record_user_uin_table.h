#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserUinTable;
struct TUserUinTableValueType;

class CRecordUserUinTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserUinTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserUinTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID() { return 0; }

    static std::vector<TUserUinTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const char* s_strTableName;
    static const char* s_strAccount;
    static const char* s_strPwd;
    static const char* s_strPlatform;
    static const char* s_strOs;
    static const char* s_strUid;

    TUserUinTable* m_pCache;

    void SetAccount(const std::string& strAccount)
    {
        m_stStringData[s_strAccount] = strAccount;
    }
    void SetPwd(const std::string& strPwd)
    {
        m_stStringData[s_strPwd] = strPwd;
    }
    void SetPlatform(const std::string& strPlatform)
    {
        m_stStringData[s_strPlatform] = strPlatform;
    }
    void SetOs(const std::string& strOs)
    {
        m_stStringData[s_strOs] = strOs;
    }
    void SetPrimaryKey(int iUid)
    {
        m_iUid = iUid;
    }
    void SetCachePtr(TUserUinTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
};

struct TUserUinTableValueType
{
    TUserUinTableValueType()
    : m_iUid(0)
    , m_strAccount("")
    , m_strPwd("")
    , m_strPlatform("")
    , m_strOs("")
    {}

    int                     m_iUid;
    std::string             m_strAccount;
    std::string             m_strPwd;
    std::string             m_strPlatform;
    std::string             m_strOs;
};

struct TUserUinTable
{
    TUserUinTable() : m_pRecord(nullptr) { }
    ~TUserUinTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserUinTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserUinTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid);
        m_pRecord->SetAccount(stData.m_strAccount);
        m_pRecord->SetPwd(stData.m_strPwd);
        m_pRecord->SetPlatform(stData.m_strPlatform);
        m_pRecord->SetOs(stData.m_strOs);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const std::string& GetAccount() const
    {
        return m_stData.m_strAccount;
    }
    void SetAccount(const std::string& strAccount)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAccount(strAccount);
        m_stData.m_strAccount = strAccount;
    }
    const std::string& GetPwd() const
    {
        return m_stData.m_strPwd;
    }
    void SetPwd(const std::string& strPwd)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPwd(strPwd);
        m_stData.m_strPwd = strPwd;
    }
    const std::string& GetPlatform() const
    {
        return m_stData.m_strPlatform;
    }
    void SetPlatform(const std::string& strPlatform)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPlatform(strPlatform);
        m_stData.m_strPlatform = strPlatform;
    }
    const std::string& GetOs() const
    {
        return m_stData.m_strOs;
    }
    void SetOs(const std::string& strOs)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetOs(strOs);
        m_stData.m_strOs = strOs;
    }

    CRecordUserUinTable* m_pRecord;
    TUserUinTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserUinTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserUinTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserUinTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
