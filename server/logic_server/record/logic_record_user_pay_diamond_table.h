#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserPayDiamondTable;
struct TUserPayDiamondTableValueType;

class CRecordUserPayDiamondTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserPayDiamondTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserPayDiamondTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();

    static std::vector<TUserPayDiamondTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strPayDiamondLeft;
    static const char* s_strPayDiamondConsume;
    static const char* s_strUid;
    static const char* s_strGroupID;

    TUserPayDiamondTable* m_pCache;

    void SetPayDiamondLeft(const std::vector<CPackPayDiamond>& stPayDiamondLeft)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stPayDiamondLeft);
        m_stVarData[s_strPayDiamondLeft] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPayDiamondConsume(const std::vector<CPackPayDiamond>& stPayDiamondConsume)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stPayDiamondConsume);
        m_stVarData[s_strPayDiamondConsume] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TUserPayDiamondTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
};

struct TUserPayDiamondTableValueType
{
    TUserPayDiamondTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_stPayDiamondLeft()
    , m_stPayDiamondConsume()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    std::vector<CPackPayDiamond>    m_stPayDiamondLeft;
    std::vector<CPackPayDiamond>    m_stPayDiamondConsume;
};

struct TUserPayDiamondTable
{
    TUserPayDiamondTable() : m_pRecord(nullptr) { }
    ~TUserPayDiamondTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserPayDiamondTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserPayDiamondTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID);
        m_pRecord->SetPayDiamondLeft(stData.m_stPayDiamondLeft);
        m_pRecord->SetPayDiamondConsume(stData.m_stPayDiamondConsume);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const std::vector<CPackPayDiamond>& GetPayDiamondLeft() const
    {
        return m_stData.m_stPayDiamondLeft;
    }
    void SetPayDiamondLeft(const std::vector<CPackPayDiamond>& stPayDiamondLeft)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayDiamondLeft(stPayDiamondLeft);
        m_stData.m_stPayDiamondLeft = stPayDiamondLeft;
    }
    const std::vector<CPackPayDiamond>& GetPayDiamondConsume() const
    {
        return m_stData.m_stPayDiamondConsume;
    }
    void SetPayDiamondConsume(const std::vector<CPackPayDiamond>& stPayDiamondConsume)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayDiamondConsume(stPayDiamondConsume);
        m_stData.m_stPayDiamondConsume = stPayDiamondConsume;
    }

    CRecordUserPayDiamondTable* m_pRecord;
    TUserPayDiamondTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserPayDiamondTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserPayDiamondTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserPayDiamondTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
