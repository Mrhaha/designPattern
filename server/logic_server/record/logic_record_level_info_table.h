#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TLevelInfoTable;
struct TLevelInfoTableValueType;

class CRecordLevelInfoTable : public CLogicSQLTableRecordBase
{
public:
    CRecordLevelInfoTable() : m_pCache(nullptr) { }

    virtual ~CRecordLevelInfoTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetLevelID();

    static std::vector<TLevelInfoTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strLevelStar;
    static const char* s_strSharedTimes;
    static const char* s_strPlayTimes;
    static const char* s_strLastPlayTime;
    static const char* s_strResetTimes;
    static const char* s_strLastResetTime;
    static const char* s_strTotalVictoryTimes;
    static const char* s_strCostTime;
    static const char* s_strAchieveTag;
    static const char* s_strBonusTag;
    static const char* s_strAdvRewardTimes;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strLevelID;

    TLevelInfoTable* m_pCache;

    void SetLevelStar(char cLevelStar)
    {
        m_stCharData[s_strLevelStar] += cLevelStar;
    }
    void SetSharedTimes(int iSharedTimes)
    {
        m_stIntData[s_strSharedTimes] += iSharedTimes;
    }
    void SetPlayTimes(int iPlayTimes)
    {
        m_stIntData[s_strPlayTimes] += iPlayTimes;
    }
    void SetLastPlayTime(int iLastPlayTime)
    {
        m_stIntData[s_strLastPlayTime] += iLastPlayTime;
    }
    void SetResetTimes(int iResetTimes)
    {
        m_stIntData[s_strResetTimes] += iResetTimes;
    }
    void SetLastResetTime(int iLastResetTime)
    {
        m_stIntData[s_strLastResetTime] += iLastResetTime;
    }
    void SetTotalVictoryTimes(int iTotalVictoryTimes)
    {
        m_stIntData[s_strTotalVictoryTimes] += iTotalVictoryTimes;
    }
    void SetCostTime(int iCostTime)
    {
        m_stIntData[s_strCostTime] += iCostTime;
    }
    void SetAchieveTag(char cAchieveTag)
    {
        m_stCharData[s_strAchieveTag] += cAchieveTag;
    }
    void SetBonusTag(char cBonusTag)
    {
        m_stCharData[s_strBonusTag] += cBonusTag;
    }
    void SetAdvRewardTimes(int iAdvRewardTimes)
    {
        m_stIntData[s_strAdvRewardTimes] += iAdvRewardTimes;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iLevelID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iLevelID = iLevelID;
    }
    void SetCachePtr(TLevelInfoTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iLevelID;
};

struct TLevelInfoTableValueType
{
    TLevelInfoTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iLevelID(0)
    , m_cLevelStar(0)
    , m_iSharedTimes(0)
    , m_iPlayTimes(0)
    , m_iLastPlayTime(0)
    , m_iResetTimes(0)
    , m_iLastResetTime(0)
    , m_iTotalVictoryTimes(0)
    , m_iCostTime(0)
    , m_cAchieveTag(0)
    , m_cBonusTag(0)
    , m_iAdvRewardTimes(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iLevelID;
    char                    m_cLevelStar;
    int                     m_iSharedTimes;
    int                     m_iPlayTimes;
    int                     m_iLastPlayTime;
    int                     m_iResetTimes;
    int                     m_iLastResetTime;
    int                     m_iTotalVictoryTimes;
    int                     m_iCostTime;
    char                    m_cAchieveTag;
    char                    m_cBonusTag;
    int                     m_iAdvRewardTimes;
};

struct TLevelInfoTable
{
    TLevelInfoTable() : m_pRecord(nullptr) { }
    ~TLevelInfoTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TLevelInfoTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TLevelInfoTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iLevelID);
        m_pRecord->SetLevelStar(stData.m_cLevelStar);
        m_pRecord->SetSharedTimes(stData.m_iSharedTimes);
        m_pRecord->SetPlayTimes(stData.m_iPlayTimes);
        m_pRecord->SetLastPlayTime(stData.m_iLastPlayTime);
        m_pRecord->SetResetTimes(stData.m_iResetTimes);
        m_pRecord->SetLastResetTime(stData.m_iLastResetTime);
        m_pRecord->SetTotalVictoryTimes(stData.m_iTotalVictoryTimes);
        m_pRecord->SetCostTime(stData.m_iCostTime);
        m_pRecord->SetAchieveTag(stData.m_cAchieveTag);
        m_pRecord->SetBonusTag(stData.m_cBonusTag);
        m_pRecord->SetAdvRewardTimes(stData.m_iAdvRewardTimes);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const char& GetLevelStar() const
    {
        return m_stData.m_cLevelStar;
    }
    void AddLevelStar(char cLevelStar)
    {
        if (0 == cLevelStar)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLevelStar(cLevelStar);
        m_stData.m_cLevelStar += cLevelStar;
    }
    void SetLevelStar(char cLevelStar)
    {
        if (m_stData.m_cLevelStar == cLevelStar)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cLevelStar - m_stData.m_cLevelStar;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLevelStar(RealAdd);
        m_stData.m_cLevelStar = cLevelStar;
    }
    const int& GetSharedTimes() const
    {
        return m_stData.m_iSharedTimes;
    }
    void AddSharedTimes(int iSharedTimes)
    {
        if (0 == iSharedTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSharedTimes(iSharedTimes);
        m_stData.m_iSharedTimes += iSharedTimes;
    }
    void SetSharedTimes(int iSharedTimes)
    {
        if (m_stData.m_iSharedTimes == iSharedTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSharedTimes - m_stData.m_iSharedTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSharedTimes(RealAdd);
        m_stData.m_iSharedTimes = iSharedTimes;
    }
    const int& GetPlayTimes() const
    {
        return m_stData.m_iPlayTimes;
    }
    void AddPlayTimes(int iPlayTimes)
    {
        if (0 == iPlayTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPlayTimes(iPlayTimes);
        m_stData.m_iPlayTimes += iPlayTimes;
    }
    void SetPlayTimes(int iPlayTimes)
    {
        if (m_stData.m_iPlayTimes == iPlayTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPlayTimes - m_stData.m_iPlayTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPlayTimes(RealAdd);
        m_stData.m_iPlayTimes = iPlayTimes;
    }
    const int& GetLastPlayTime() const
    {
        return m_stData.m_iLastPlayTime;
    }
    void AddLastPlayTime(int iLastPlayTime)
    {
        if (0 == iLastPlayTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastPlayTime(iLastPlayTime);
        m_stData.m_iLastPlayTime += iLastPlayTime;
    }
    void SetLastPlayTime(int iLastPlayTime)
    {
        if (m_stData.m_iLastPlayTime == iLastPlayTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastPlayTime - m_stData.m_iLastPlayTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastPlayTime(RealAdd);
        m_stData.m_iLastPlayTime = iLastPlayTime;
    }
    const int& GetResetTimes() const
    {
        return m_stData.m_iResetTimes;
    }
    void AddResetTimes(int iResetTimes)
    {
        if (0 == iResetTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetResetTimes(iResetTimes);
        m_stData.m_iResetTimes += iResetTimes;
    }
    void SetResetTimes(int iResetTimes)
    {
        if (m_stData.m_iResetTimes == iResetTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iResetTimes - m_stData.m_iResetTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetResetTimes(RealAdd);
        m_stData.m_iResetTimes = iResetTimes;
    }
    const int& GetLastResetTime() const
    {
        return m_stData.m_iLastResetTime;
    }
    void AddLastResetTime(int iLastResetTime)
    {
        if (0 == iLastResetTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastResetTime(iLastResetTime);
        m_stData.m_iLastResetTime += iLastResetTime;
    }
    void SetLastResetTime(int iLastResetTime)
    {
        if (m_stData.m_iLastResetTime == iLastResetTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastResetTime - m_stData.m_iLastResetTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastResetTime(RealAdd);
        m_stData.m_iLastResetTime = iLastResetTime;
    }
    const int& GetTotalVictoryTimes() const
    {
        return m_stData.m_iTotalVictoryTimes;
    }
    void AddTotalVictoryTimes(int iTotalVictoryTimes)
    {
        if (0 == iTotalVictoryTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalVictoryTimes(iTotalVictoryTimes);
        m_stData.m_iTotalVictoryTimes += iTotalVictoryTimes;
    }
    void SetTotalVictoryTimes(int iTotalVictoryTimes)
    {
        if (m_stData.m_iTotalVictoryTimes == iTotalVictoryTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTotalVictoryTimes - m_stData.m_iTotalVictoryTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalVictoryTimes(RealAdd);
        m_stData.m_iTotalVictoryTimes = iTotalVictoryTimes;
    }
    const int& GetCostTime() const
    {
        return m_stData.m_iCostTime;
    }
    void AddCostTime(int iCostTime)
    {
        if (0 == iCostTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCostTime(iCostTime);
        m_stData.m_iCostTime += iCostTime;
    }
    void SetCostTime(int iCostTime)
    {
        if (m_stData.m_iCostTime == iCostTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCostTime - m_stData.m_iCostTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCostTime(RealAdd);
        m_stData.m_iCostTime = iCostTime;
    }
    const char& GetAchieveTag() const
    {
        return m_stData.m_cAchieveTag;
    }
    void AddAchieveTag(char cAchieveTag)
    {
        if (0 == cAchieveTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAchieveTag(cAchieveTag);
        m_stData.m_cAchieveTag += cAchieveTag;
    }
    void SetAchieveTag(char cAchieveTag)
    {
        if (m_stData.m_cAchieveTag == cAchieveTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cAchieveTag - m_stData.m_cAchieveTag;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAchieveTag(RealAdd);
        m_stData.m_cAchieveTag = cAchieveTag;
    }
    const char& GetBonusTag() const
    {
        return m_stData.m_cBonusTag;
    }
    void AddBonusTag(char cBonusTag)
    {
        if (0 == cBonusTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBonusTag(cBonusTag);
        m_stData.m_cBonusTag += cBonusTag;
    }
    void SetBonusTag(char cBonusTag)
    {
        if (m_stData.m_cBonusTag == cBonusTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cBonusTag - m_stData.m_cBonusTag;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBonusTag(RealAdd);
        m_stData.m_cBonusTag = cBonusTag;
    }
    const int& GetAdvRewardTimes() const
    {
        return m_stData.m_iAdvRewardTimes;
    }
    void AddAdvRewardTimes(int iAdvRewardTimes)
    {
        if (0 == iAdvRewardTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAdvRewardTimes(iAdvRewardTimes);
        m_stData.m_iAdvRewardTimes += iAdvRewardTimes;
    }
    void SetAdvRewardTimes(int iAdvRewardTimes)
    {
        if (m_stData.m_iAdvRewardTimes == iAdvRewardTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAdvRewardTimes - m_stData.m_iAdvRewardTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAdvRewardTimes(RealAdd);
        m_stData.m_iAdvRewardTimes = iAdvRewardTimes;
    }

    CRecordLevelInfoTable* m_pRecord;
    TLevelInfoTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordLevelInfoTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordLevelInfoTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iLevelID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordLevelInfoTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
