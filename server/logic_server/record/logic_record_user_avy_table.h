#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserAvyTable;
struct TUserAvyTableValueType;

class CRecordUserAvyTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserAvyTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserAvyTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetAvyID();

    static std::vector<TUserAvyTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strCompleteBitmap;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strAvyID;

    TUserAvyTable* m_pCache;

    void SetCompleteBitmap(uint64_t ulCompleteBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strCompleteBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulCompleteBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulCompleteBitmap)
            {
                stDataRef.m_ulVal -= ulCompleteBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulCompleteBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iAvyID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iAvyID = iAvyID;
    }
    void SetCachePtr(TUserAvyTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iAvyID;
};

struct TUserAvyTableValueType
{
    TUserAvyTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iAvyID(0)
    , m_ulCompleteBitmap(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iAvyID;
    uint64_t                m_ulCompleteBitmap;
};

struct TUserAvyTable
{
    TUserAvyTable() : m_pRecord(nullptr) { }
    ~TUserAvyTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserAvyTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserAvyTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iAvyID);
        m_pRecord->SetCompleteBitmap(stData.m_ulCompleteBitmap);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const uint64_t& GetCompleteBitmap() const
    {
        return m_stData.m_ulCompleteBitmap;
    }
    void AddCompleteBitmap(uint64_t ulCompleteBitmap)
    {
        if (0 == ulCompleteBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCompleteBitmap(ulCompleteBitmap);
        m_stData.m_ulCompleteBitmap += ulCompleteBitmap;
    }
    void SetCompleteBitmap(uint64_t ulCompleteBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulCompleteBitmap > ulCompleteBitmap)
        {
            m_pRecord->SetCompleteBitmap(m_stData.m_ulCompleteBitmap - ulCompleteBitmap, '-');
        }
        else
        {
            m_pRecord->SetCompleteBitmap(ulCompleteBitmap - m_stData.m_ulCompleteBitmap, '+');
        }
        m_stData.m_ulCompleteBitmap = ulCompleteBitmap;
    }

    CRecordUserAvyTable* m_pRecord;
    TUserAvyTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserAvyTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserAvyTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iAvyID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserAvyTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
