#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGlobalEntireMapTable;
struct TGlobalEntireMapTableValueType;

class CRecordGlobalEntireWarMapTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGlobalEntireWarMapTable() : m_pCache(nullptr) { }

    virtual ~CRecordGlobalEntireWarMapTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetServerID();
    virtual int GetWarID();
    virtual int GetUid() { return 0; }
    virtual int GetGroupID() { return 0; }

    static std::vector<TGlobalEntireMapTableValueType> SelectDataFromSQL(int iServerID);

    static const char* s_strTableName;
    static const char* s_strTargetProgress;
    static const char* s_strCurProgress;
    static const char* s_strIsPassed;
    static const char* s_strServerID;
    static const char* s_strWarID;

    TGlobalEntireMapTable* m_pCache;

    void SetTargetProgress(uint64_t ulTargetProgress, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strTargetProgress];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulTargetProgress;
        }
        else
        {
            if (stDataRef.m_ulVal > ulTargetProgress)
            {
                stDataRef.m_ulVal -= ulTargetProgress;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulTargetProgress - stDataRef.m_ulVal;
            }
        }
    }
    void SetCurProgress(uint64_t ulCurProgress, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strCurProgress];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulCurProgress;
        }
        else
        {
            if (stDataRef.m_ulVal > ulCurProgress)
            {
                stDataRef.m_ulVal -= ulCurProgress;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulCurProgress - stDataRef.m_ulVal;
            }
        }
    }
    void SetIsPassed(int iIsPassed)
    {
        m_stIntData[s_strIsPassed] += iIsPassed;
    }
    void SetPrimaryKey(int iServerID, int iWarID)
    {
        m_iServerID = iServerID;
        m_iWarID = iWarID;
    }
    void SetCachePtr(TGlobalEntireMapTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iServerID;
    int m_iWarID;
};

struct TGlobalEntireMapTableValueType
{
    TGlobalEntireMapTableValueType()
    : m_iServerID(0)
    , m_iWarID(0)
    , m_ulTargetProgress(0)
    , m_ulCurProgress(0)
    , m_iIsPassed(0)
    {}

    int                     m_iServerID;
    int                     m_iWarID;
    uint64_t                m_ulTargetProgress;
    uint64_t                m_ulCurProgress;
    int                     m_iIsPassed;
};

struct TGlobalEntireMapTable
{
    TGlobalEntireMapTable() : m_pRecord(nullptr) { }
    ~TGlobalEntireMapTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGlobalEntireMapTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGlobalEntireMapTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iServerID, stData.m_iWarID);
        m_pRecord->SetTargetProgress(stData.m_ulTargetProgress);
        m_pRecord->SetCurProgress(stData.m_ulCurProgress);
        m_pRecord->SetIsPassed(stData.m_iIsPassed);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const uint64_t& GetTargetProgress() const
    {
        return m_stData.m_ulTargetProgress;
    }
    void AddTargetProgress(uint64_t ulTargetProgress)
    {
        if (0 == ulTargetProgress)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTargetProgress(ulTargetProgress);
        m_stData.m_ulTargetProgress += ulTargetProgress;
    }
    void SetTargetProgress(uint64_t ulTargetProgress)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulTargetProgress > ulTargetProgress)
        {
            m_pRecord->SetTargetProgress(m_stData.m_ulTargetProgress - ulTargetProgress, '-');
        }
        else
        {
            m_pRecord->SetTargetProgress(ulTargetProgress - m_stData.m_ulTargetProgress, '+');
        }
        m_stData.m_ulTargetProgress = ulTargetProgress;
    }
    const uint64_t& GetCurProgress() const
    {
        return m_stData.m_ulCurProgress;
    }
    void AddCurProgress(uint64_t ulCurProgress)
    {
        if (0 == ulCurProgress)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCurProgress(ulCurProgress);
        m_stData.m_ulCurProgress += ulCurProgress;
    }
    void SetCurProgress(uint64_t ulCurProgress)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulCurProgress > ulCurProgress)
        {
            m_pRecord->SetCurProgress(m_stData.m_ulCurProgress - ulCurProgress, '-');
        }
        else
        {
            m_pRecord->SetCurProgress(ulCurProgress - m_stData.m_ulCurProgress, '+');
        }
        m_stData.m_ulCurProgress = ulCurProgress;
    }
    const int& GetIsPassed() const
    {
        return m_stData.m_iIsPassed;
    }
    void AddIsPassed(int iIsPassed)
    {
        if (0 == iIsPassed)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsPassed(iIsPassed);
        m_stData.m_iIsPassed += iIsPassed;
    }
    void SetIsPassed(int iIsPassed)
    {
        if (m_stData.m_iIsPassed == iIsPassed)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iIsPassed - m_stData.m_iIsPassed;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsPassed(RealAdd);
        m_stData.m_iIsPassed = iIsPassed;
    }

    CRecordGlobalEntireWarMapTable* m_pRecord;
    TGlobalEntireMapTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGlobalEntireWarMapTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGlobalEntireWarMapTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iServerID, m_stData.m_iWarID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGlobalEntireWarMapTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
