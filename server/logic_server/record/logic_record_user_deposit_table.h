#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserDepositTable;
struct TUserDepositTableValueType;

class CRecordUserDepositTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserDepositTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserDepositTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetOrderID();

    static std::vector<TUserDepositTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strBuyTimes;
    static const char* s_strTodayBuyTimes;
    static const char* s_strWeekBuyTimes;
    static const char* s_strDoubleTimes;
    static const char* s_strLastDepositTime;
    static const char* s_strMonthBuyTimes;
    static const char* s_strDayIntervalBuyTimes;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strOrderID;

    TUserDepositTable* m_pCache;

    void SetBuyTimes(int iBuyTimes)
    {
        m_stIntData[s_strBuyTimes] += iBuyTimes;
    }
    void SetTodayBuyTimes(int iTodayBuyTimes)
    {
        m_stIntData[s_strTodayBuyTimes] += iTodayBuyTimes;
    }
    void SetWeekBuyTimes(int iWeekBuyTimes)
    {
        m_stIntData[s_strWeekBuyTimes] += iWeekBuyTimes;
    }
    void SetDoubleTimes(int iDoubleTimes)
    {
        m_stIntData[s_strDoubleTimes] += iDoubleTimes;
    }
    void SetLastDepositTime(int iLastDepositTime)
    {
        m_stIntData[s_strLastDepositTime] += iLastDepositTime;
    }
    void SetMonthBuyTimes(int iMonthBuyTimes)
    {
        m_stIntData[s_strMonthBuyTimes] += iMonthBuyTimes;
    }
    void SetDayIntervalBuyTimes(int iDayIntervalBuyTimes)
    {
        m_stIntData[s_strDayIntervalBuyTimes] += iDayIntervalBuyTimes;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iOrderID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iOrderID = iOrderID;
    }
    void SetCachePtr(TUserDepositTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iOrderID;
};

struct TUserDepositTableValueType
{
    TUserDepositTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iOrderID(0)
    , m_iBuyTimes(0)
    , m_iTodayBuyTimes(0)
    , m_iWeekBuyTimes(0)
    , m_iDoubleTimes(0)
    , m_iLastDepositTime(0)
    , m_iMonthBuyTimes(0)
    , m_iDayIntervalBuyTimes(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iOrderID;
    int                     m_iBuyTimes;
    int                     m_iTodayBuyTimes;
    int                     m_iWeekBuyTimes;
    int                     m_iDoubleTimes;
    int                     m_iLastDepositTime;
    int                     m_iMonthBuyTimes;
    int                     m_iDayIntervalBuyTimes;
};

struct TUserDepositTable
{
    TUserDepositTable() : m_pRecord(nullptr) { }
    ~TUserDepositTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserDepositTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserDepositTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iOrderID);
        m_pRecord->SetBuyTimes(stData.m_iBuyTimes);
        m_pRecord->SetTodayBuyTimes(stData.m_iTodayBuyTimes);
        m_pRecord->SetWeekBuyTimes(stData.m_iWeekBuyTimes);
        m_pRecord->SetDoubleTimes(stData.m_iDoubleTimes);
        m_pRecord->SetLastDepositTime(stData.m_iLastDepositTime);
        m_pRecord->SetMonthBuyTimes(stData.m_iMonthBuyTimes);
        m_pRecord->SetDayIntervalBuyTimes(stData.m_iDayIntervalBuyTimes);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetBuyTimes() const
    {
        return m_stData.m_iBuyTimes;
    }
    void AddBuyTimes(int iBuyTimes)
    {
        if (0 == iBuyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyTimes(iBuyTimes);
        m_stData.m_iBuyTimes += iBuyTimes;
    }
    void SetBuyTimes(int iBuyTimes)
    {
        if (m_stData.m_iBuyTimes == iBuyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyTimes - m_stData.m_iBuyTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyTimes(RealAdd);
        m_stData.m_iBuyTimes = iBuyTimes;
    }
    const int& GetTodayBuyTimes() const
    {
        return m_stData.m_iTodayBuyTimes;
    }
    void AddTodayBuyTimes(int iTodayBuyTimes)
    {
        if (0 == iTodayBuyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTodayBuyTimes(iTodayBuyTimes);
        m_stData.m_iTodayBuyTimes += iTodayBuyTimes;
    }
    void SetTodayBuyTimes(int iTodayBuyTimes)
    {
        if (m_stData.m_iTodayBuyTimes == iTodayBuyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTodayBuyTimes - m_stData.m_iTodayBuyTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTodayBuyTimes(RealAdd);
        m_stData.m_iTodayBuyTimes = iTodayBuyTimes;
    }
    const int& GetWeekBuyTimes() const
    {
        return m_stData.m_iWeekBuyTimes;
    }
    void AddWeekBuyTimes(int iWeekBuyTimes)
    {
        if (0 == iWeekBuyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWeekBuyTimes(iWeekBuyTimes);
        m_stData.m_iWeekBuyTimes += iWeekBuyTimes;
    }
    void SetWeekBuyTimes(int iWeekBuyTimes)
    {
        if (m_stData.m_iWeekBuyTimes == iWeekBuyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWeekBuyTimes - m_stData.m_iWeekBuyTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWeekBuyTimes(RealAdd);
        m_stData.m_iWeekBuyTimes = iWeekBuyTimes;
    }
    const int& GetDoubleTimes() const
    {
        return m_stData.m_iDoubleTimes;
    }
    void AddDoubleTimes(int iDoubleTimes)
    {
        if (0 == iDoubleTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDoubleTimes(iDoubleTimes);
        m_stData.m_iDoubleTimes += iDoubleTimes;
    }
    void SetDoubleTimes(int iDoubleTimes)
    {
        if (m_stData.m_iDoubleTimes == iDoubleTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDoubleTimes - m_stData.m_iDoubleTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDoubleTimes(RealAdd);
        m_stData.m_iDoubleTimes = iDoubleTimes;
    }
    const int& GetLastDepositTime() const
    {
        return m_stData.m_iLastDepositTime;
    }
    void AddLastDepositTime(int iLastDepositTime)
    {
        if (0 == iLastDepositTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastDepositTime(iLastDepositTime);
        m_stData.m_iLastDepositTime += iLastDepositTime;
    }
    void SetLastDepositTime(int iLastDepositTime)
    {
        if (m_stData.m_iLastDepositTime == iLastDepositTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastDepositTime - m_stData.m_iLastDepositTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastDepositTime(RealAdd);
        m_stData.m_iLastDepositTime = iLastDepositTime;
    }
    const int& GetMonthBuyTimes() const
    {
        return m_stData.m_iMonthBuyTimes;
    }
    void AddMonthBuyTimes(int iMonthBuyTimes)
    {
        if (0 == iMonthBuyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthBuyTimes(iMonthBuyTimes);
        m_stData.m_iMonthBuyTimes += iMonthBuyTimes;
    }
    void SetMonthBuyTimes(int iMonthBuyTimes)
    {
        if (m_stData.m_iMonthBuyTimes == iMonthBuyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMonthBuyTimes - m_stData.m_iMonthBuyTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthBuyTimes(RealAdd);
        m_stData.m_iMonthBuyTimes = iMonthBuyTimes;
    }
    const int& GetDayIntervalBuyTimes() const
    {
        return m_stData.m_iDayIntervalBuyTimes;
    }
    void AddDayIntervalBuyTimes(int iDayIntervalBuyTimes)
    {
        if (0 == iDayIntervalBuyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDayIntervalBuyTimes(iDayIntervalBuyTimes);
        m_stData.m_iDayIntervalBuyTimes += iDayIntervalBuyTimes;
    }
    void SetDayIntervalBuyTimes(int iDayIntervalBuyTimes)
    {
        if (m_stData.m_iDayIntervalBuyTimes == iDayIntervalBuyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDayIntervalBuyTimes - m_stData.m_iDayIntervalBuyTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDayIntervalBuyTimes(RealAdd);
        m_stData.m_iDayIntervalBuyTimes = iDayIntervalBuyTimes;
    }

    CRecordUserDepositTable* m_pRecord;
    TUserDepositTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserDepositTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserDepositTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iOrderID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserDepositTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
