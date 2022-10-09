#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserBuyCommodityTable;
struct TUserBuyCommodityTableValueType;

class CRecordUserBuyCommodityTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserBuyCommodityTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserBuyCommodityTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetCommodityID();

    static std::vector<TUserBuyCommodityTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strDailyLimitPayTimes;
    static const char* s_strLastPayTime;
    static const char* s_strTotalTimes;
    static const char* s_strPt;
    static const char* s_strBaodiTime;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strCommodityID;

    TUserBuyCommodityTable* m_pCache;

    void SetDailyLimitPayTimes(int iDailyLimitPayTimes)
    {
        m_stIntData[s_strDailyLimitPayTimes] += iDailyLimitPayTimes;
    }
    void SetLastPayTime(int iLastPayTime)
    {
        m_stIntData[s_strLastPayTime] += iLastPayTime;
    }
    void SetTotalTimes(int iTotalTimes)
    {
        m_stIntData[s_strTotalTimes] += iTotalTimes;
    }
    void SetPt(int iPt)
    {
        m_stIntData[s_strPt] += iPt;
    }
    void SetBaodiTime(int iBaodiTime)
    {
        m_stIntData[s_strBaodiTime] += iBaodiTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iCommodityID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iCommodityID = iCommodityID;
    }
    void SetCachePtr(TUserBuyCommodityTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iCommodityID;
};

struct TUserBuyCommodityTableValueType
{
    TUserBuyCommodityTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iCommodityID(0)
    , m_iDailyLimitPayTimes(0)
    , m_iLastPayTime(0)
    , m_iTotalTimes(0)
    , m_iPt(0)
    , m_iBaodiTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iCommodityID;
    int                     m_iDailyLimitPayTimes;
    int                     m_iLastPayTime;
    int                     m_iTotalTimes;
    int                     m_iPt;
    int                     m_iBaodiTime;
};

struct TUserBuyCommodityTable
{
    TUserBuyCommodityTable() : m_pRecord(nullptr) { }
    ~TUserBuyCommodityTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserBuyCommodityTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserBuyCommodityTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iCommodityID);
        m_pRecord->SetDailyLimitPayTimes(stData.m_iDailyLimitPayTimes);
        m_pRecord->SetLastPayTime(stData.m_iLastPayTime);
        m_pRecord->SetTotalTimes(stData.m_iTotalTimes);
        m_pRecord->SetPt(stData.m_iPt);
        m_pRecord->SetBaodiTime(stData.m_iBaodiTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetDailyLimitPayTimes() const
    {
        return m_stData.m_iDailyLimitPayTimes;
    }
    void AddDailyLimitPayTimes(int iDailyLimitPayTimes)
    {
        if (0 == iDailyLimitPayTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyLimitPayTimes(iDailyLimitPayTimes);
        m_stData.m_iDailyLimitPayTimes += iDailyLimitPayTimes;
    }
    void SetDailyLimitPayTimes(int iDailyLimitPayTimes)
    {
        if (m_stData.m_iDailyLimitPayTimes == iDailyLimitPayTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDailyLimitPayTimes - m_stData.m_iDailyLimitPayTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyLimitPayTimes(RealAdd);
        m_stData.m_iDailyLimitPayTimes = iDailyLimitPayTimes;
    }
    const int& GetLastPayTime() const
    {
        return m_stData.m_iLastPayTime;
    }
    void AddLastPayTime(int iLastPayTime)
    {
        if (0 == iLastPayTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastPayTime(iLastPayTime);
        m_stData.m_iLastPayTime += iLastPayTime;
    }
    void SetLastPayTime(int iLastPayTime)
    {
        if (m_stData.m_iLastPayTime == iLastPayTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastPayTime - m_stData.m_iLastPayTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastPayTime(RealAdd);
        m_stData.m_iLastPayTime = iLastPayTime;
    }
    const int& GetTotalTimes() const
    {
        return m_stData.m_iTotalTimes;
    }
    void AddTotalTimes(int iTotalTimes)
    {
        if (0 == iTotalTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalTimes(iTotalTimes);
        m_stData.m_iTotalTimes += iTotalTimes;
    }
    void SetTotalTimes(int iTotalTimes)
    {
        if (m_stData.m_iTotalTimes == iTotalTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTotalTimes - m_stData.m_iTotalTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalTimes(RealAdd);
        m_stData.m_iTotalTimes = iTotalTimes;
    }
    const int& GetPt() const
    {
        return m_stData.m_iPt;
    }
    void AddPt(int iPt)
    {
        if (0 == iPt)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPt(iPt);
        m_stData.m_iPt += iPt;
    }
    void SetPt(int iPt)
    {
        if (m_stData.m_iPt == iPt)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPt - m_stData.m_iPt;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPt(RealAdd);
        m_stData.m_iPt = iPt;
    }
    const int& GetBaodiTime() const
    {
        return m_stData.m_iBaodiTime;
    }
    void AddBaodiTime(int iBaodiTime)
    {
        if (0 == iBaodiTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBaodiTime(iBaodiTime);
        m_stData.m_iBaodiTime += iBaodiTime;
    }
    void SetBaodiTime(int iBaodiTime)
    {
        if (m_stData.m_iBaodiTime == iBaodiTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBaodiTime - m_stData.m_iBaodiTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBaodiTime(RealAdd);
        m_stData.m_iBaodiTime = iBaodiTime;
    }

    CRecordUserBuyCommodityTable* m_pRecord;
    TUserBuyCommodityTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserBuyCommodityTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserBuyCommodityTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iCommodityID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserBuyCommodityTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
