#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserHeroHistoryTable;
struct TUserHeroHistoryTableValueType;

class CRecordUserHeroHistoryTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserHeroHistoryTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserHeroHistoryTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetCardID();
    virtual int GetHistoryIndex();

    static std::vector<TUserHeroHistoryTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strUnlockTime;
    static const char* s_strContentIndex;
    static const char* s_strRewardTag;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strCardID;
    static const char* s_strHistoryIndex;

    TUserHeroHistoryTable* m_pCache;

    void SetUnlockTime(int iUnlockTime)
    {
        m_stIntData[s_strUnlockTime] += iUnlockTime;
    }
    void SetContentIndex(int iContentIndex)
    {
        m_stIntData[s_strContentIndex] += iContentIndex;
    }
    void SetRewardTag(char cRewardTag)
    {
        m_stCharData[s_strRewardTag] += cRewardTag;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iCardID, int iHistoryIndex)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iCardID = iCardID;
        m_iHistoryIndex = iHistoryIndex;
    }
    void SetCachePtr(TUserHeroHistoryTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iCardID;
    int m_iHistoryIndex;
};

struct TUserHeroHistoryTableValueType
{
    TUserHeroHistoryTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iCardID(0)
    , m_iHistoryIndex(0)
    , m_iUnlockTime(0)
    , m_iContentIndex(0)
    , m_cRewardTag(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iCardID;
    int                     m_iHistoryIndex;
    int                     m_iUnlockTime;
    int                     m_iContentIndex;
    char                    m_cRewardTag;
};

struct TUserHeroHistoryTable
{
    TUserHeroHistoryTable() : m_pRecord(nullptr) { }
    ~TUserHeroHistoryTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserHeroHistoryTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserHeroHistoryTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iCardID, stData.m_iHistoryIndex);
        m_pRecord->SetUnlockTime(stData.m_iUnlockTime);
        m_pRecord->SetContentIndex(stData.m_iContentIndex);
        m_pRecord->SetRewardTag(stData.m_cRewardTag);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetUnlockTime() const
    {
        return m_stData.m_iUnlockTime;
    }
    void AddUnlockTime(int iUnlockTime)
    {
        if (0 == iUnlockTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUnlockTime(iUnlockTime);
        m_stData.m_iUnlockTime += iUnlockTime;
    }
    void SetUnlockTime(int iUnlockTime)
    {
        if (m_stData.m_iUnlockTime == iUnlockTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iUnlockTime - m_stData.m_iUnlockTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUnlockTime(RealAdd);
        m_stData.m_iUnlockTime = iUnlockTime;
    }
    const int& GetContentIndex() const
    {
        return m_stData.m_iContentIndex;
    }
    void AddContentIndex(int iContentIndex)
    {
        if (0 == iContentIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetContentIndex(iContentIndex);
        m_stData.m_iContentIndex += iContentIndex;
    }
    void SetContentIndex(int iContentIndex)
    {
        if (m_stData.m_iContentIndex == iContentIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iContentIndex - m_stData.m_iContentIndex;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetContentIndex(RealAdd);
        m_stData.m_iContentIndex = iContentIndex;
    }
    const char& GetRewardTag() const
    {
        return m_stData.m_cRewardTag;
    }
    void AddRewardTag(char cRewardTag)
    {
        if (0 == cRewardTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRewardTag(cRewardTag);
        m_stData.m_cRewardTag += cRewardTag;
    }
    void SetRewardTag(char cRewardTag)
    {
        if (m_stData.m_cRewardTag == cRewardTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cRewardTag - m_stData.m_cRewardTag;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRewardTag(RealAdd);
        m_stData.m_cRewardTag = cRewardTag;
    }

    CRecordUserHeroHistoryTable* m_pRecord;
    TUserHeroHistoryTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserHeroHistoryTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserHeroHistoryTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iCardID, m_stData.m_iHistoryIndex);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserHeroHistoryTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
