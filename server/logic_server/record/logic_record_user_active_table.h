#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TActiveTable;
struct TActiveTableValueType;

class CRecordUserActiveTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserActiveTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserActiveTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetActiveID();

    static std::vector<TActiveTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strActiveTargetNum;
    static const char* s_strActiveReceivedBitmap;
    static const char* s_strActiveCompleteBitmap;
    static const char* s_strParaEx;
    static const char* s_strActiveTime;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strActiveID;

    TActiveTable* m_pCache;

    void SetActiveTargetNum(int iActiveTargetNum)
    {
        m_stIntData[s_strActiveTargetNum] += iActiveTargetNum;
    }
    void SetActiveReceivedBitmap(uint64_t ulActiveReceivedBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strActiveReceivedBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulActiveReceivedBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulActiveReceivedBitmap)
            {
                stDataRef.m_ulVal -= ulActiveReceivedBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulActiveReceivedBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetActiveCompleteBitmap(uint64_t ulActiveCompleteBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strActiveCompleteBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulActiveCompleteBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulActiveCompleteBitmap)
            {
                stDataRef.m_ulVal -= ulActiveCompleteBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulActiveCompleteBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetParaEx(uint64_t ulParaEx, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strParaEx];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulParaEx;
        }
        else
        {
            if (stDataRef.m_ulVal > ulParaEx)
            {
                stDataRef.m_ulVal -= ulParaEx;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulParaEx - stDataRef.m_ulVal;
            }
        }
    }
    void SetActiveTime(int iActiveTime)
    {
        m_stIntData[s_strActiveTime] += iActiveTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iActiveID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iActiveID = iActiveID;
    }
    void SetCachePtr(TActiveTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iActiveID;
};

struct TActiveTableValueType
{
    TActiveTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iActiveID(0)
    , m_iActiveTargetNum(0)
    , m_ulActiveReceivedBitmap(0)
    , m_ulActiveCompleteBitmap(0)
    , m_ulParaEx(0)
    , m_iActiveTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iActiveID;
    int                     m_iActiveTargetNum;
    uint64_t                m_ulActiveReceivedBitmap;
    uint64_t                m_ulActiveCompleteBitmap;
    uint64_t                m_ulParaEx;
    int                     m_iActiveTime;
};

struct TActiveTable
{
    TActiveTable() : m_pRecord(nullptr) { }
    ~TActiveTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TActiveTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TActiveTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iActiveID);
        m_pRecord->SetActiveTargetNum(stData.m_iActiveTargetNum);
        m_pRecord->SetActiveReceivedBitmap(stData.m_ulActiveReceivedBitmap);
        m_pRecord->SetActiveCompleteBitmap(stData.m_ulActiveCompleteBitmap);
        m_pRecord->SetParaEx(stData.m_ulParaEx);
        m_pRecord->SetActiveTime(stData.m_iActiveTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetActiveTargetNum() const
    {
        return m_stData.m_iActiveTargetNum;
    }
    void AddActiveTargetNum(int iActiveTargetNum)
    {
        if (0 == iActiveTargetNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActiveTargetNum(iActiveTargetNum);
        m_stData.m_iActiveTargetNum += iActiveTargetNum;
    }
    void SetActiveTargetNum(int iActiveTargetNum)
    {
        if (m_stData.m_iActiveTargetNum == iActiveTargetNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iActiveTargetNum - m_stData.m_iActiveTargetNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActiveTargetNum(RealAdd);
        m_stData.m_iActiveTargetNum = iActiveTargetNum;
    }
    const uint64_t& GetActiveReceivedBitmap() const
    {
        return m_stData.m_ulActiveReceivedBitmap;
    }
    void AddActiveReceivedBitmap(uint64_t ulActiveReceivedBitmap)
    {
        if (0 == ulActiveReceivedBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActiveReceivedBitmap(ulActiveReceivedBitmap);
        m_stData.m_ulActiveReceivedBitmap += ulActiveReceivedBitmap;
    }
    void SetActiveReceivedBitmap(uint64_t ulActiveReceivedBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulActiveReceivedBitmap > ulActiveReceivedBitmap)
        {
            m_pRecord->SetActiveReceivedBitmap(m_stData.m_ulActiveReceivedBitmap - ulActiveReceivedBitmap, '-');
        }
        else
        {
            m_pRecord->SetActiveReceivedBitmap(ulActiveReceivedBitmap - m_stData.m_ulActiveReceivedBitmap, '+');
        }
        m_stData.m_ulActiveReceivedBitmap = ulActiveReceivedBitmap;
    }
    const uint64_t& GetActiveCompleteBitmap() const
    {
        return m_stData.m_ulActiveCompleteBitmap;
    }
    void AddActiveCompleteBitmap(uint64_t ulActiveCompleteBitmap)
    {
        if (0 == ulActiveCompleteBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActiveCompleteBitmap(ulActiveCompleteBitmap);
        m_stData.m_ulActiveCompleteBitmap += ulActiveCompleteBitmap;
    }
    void SetActiveCompleteBitmap(uint64_t ulActiveCompleteBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulActiveCompleteBitmap > ulActiveCompleteBitmap)
        {
            m_pRecord->SetActiveCompleteBitmap(m_stData.m_ulActiveCompleteBitmap - ulActiveCompleteBitmap, '-');
        }
        else
        {
            m_pRecord->SetActiveCompleteBitmap(ulActiveCompleteBitmap - m_stData.m_ulActiveCompleteBitmap, '+');
        }
        m_stData.m_ulActiveCompleteBitmap = ulActiveCompleteBitmap;
    }
    const uint64_t& GetParaEx() const
    {
        return m_stData.m_ulParaEx;
    }
    void AddParaEx(uint64_t ulParaEx)
    {
        if (0 == ulParaEx)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetParaEx(ulParaEx);
        m_stData.m_ulParaEx += ulParaEx;
    }
    void SetParaEx(uint64_t ulParaEx)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulParaEx > ulParaEx)
        {
            m_pRecord->SetParaEx(m_stData.m_ulParaEx - ulParaEx, '-');
        }
        else
        {
            m_pRecord->SetParaEx(ulParaEx - m_stData.m_ulParaEx, '+');
        }
        m_stData.m_ulParaEx = ulParaEx;
    }
    const int& GetActiveTime() const
    {
        return m_stData.m_iActiveTime;
    }
    void AddActiveTime(int iActiveTime)
    {
        if (0 == iActiveTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActiveTime(iActiveTime);
        m_stData.m_iActiveTime += iActiveTime;
    }
    void SetActiveTime(int iActiveTime)
    {
        if (m_stData.m_iActiveTime == iActiveTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iActiveTime - m_stData.m_iActiveTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActiveTime(RealAdd);
        m_stData.m_iActiveTime = iActiveTime;
    }

    CRecordUserActiveTable* m_pRecord;
    TActiveTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserActiveTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserActiveTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iActiveID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserActiveTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
