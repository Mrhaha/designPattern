#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGuildMemberTable;
struct TGuildMemberTableValueType;

class CRecordGuildMemberTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGuildMemberTable() : m_pCache(nullptr) { }

    virtual ~CRecordGuildMemberTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();

    static std::vector<TGuildMemberTableValueType> SelectDataFromSQL(int iUin, int iGroupID);
    static std::vector<TGuildMemberTableValueType> SelectDataFromSQLWithGuildID(int iGuildID);

    static const char* s_strTableName;
    static const char* s_strGuildID;
    static const char* s_strPost;
    static const char* s_strContributionTotal;
    static const char* s_strJoinTime;
    static const char* s_strGuildWarHurt;
    static const char* s_strUid;
    static const char* s_strGroupID;

    TGuildMemberTable* m_pCache;

    void SetGuildID(int iGuildID)
    {
        m_stIntData[s_strGuildID] += iGuildID;
    }
    void SetPost(char cPost)
    {
        m_stCharData[s_strPost] += cPost;
    }
    void SetContributionTotal(int iContributionTotal)
    {
        m_stIntData[s_strContributionTotal] += iContributionTotal;
    }
    void SetJoinTime(int iJoinTime)
    {
        m_stIntData[s_strJoinTime] += iJoinTime;
    }
    void SetGuildWarHurt(uint64_t ulGuildWarHurt, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strGuildWarHurt];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulGuildWarHurt;
        }
        else
        {
            if (stDataRef.m_ulVal > ulGuildWarHurt)
            {
                stDataRef.m_ulVal -= ulGuildWarHurt;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulGuildWarHurt - stDataRef.m_ulVal;
            }
        }
    }
    void SetPrimaryKey(int iUid, int iGroupID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TGuildMemberTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
};

struct TGuildMemberTableValueType
{
    TGuildMemberTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iGuildID(0)
    , m_cPost(0)
    , m_iContributionTotal(0)
    , m_iJoinTime(0)
    , m_ulGuildWarHurt(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iGuildID;
    char                    m_cPost;
    int                     m_iContributionTotal;
    int                     m_iJoinTime;
    uint64_t                m_ulGuildWarHurt;
};

struct TGuildMemberTable
{
    TGuildMemberTable() : m_pRecord(nullptr) { }
    ~TGuildMemberTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGuildMemberTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGuildMemberTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID);
        m_pRecord->SetGuildID(stData.m_iGuildID);
        m_pRecord->SetPost(stData.m_cPost);
        m_pRecord->SetContributionTotal(stData.m_iContributionTotal);
        m_pRecord->SetJoinTime(stData.m_iJoinTime);
        m_pRecord->SetGuildWarHurt(stData.m_ulGuildWarHurt);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetGuildID() const
    {
        return m_stData.m_iGuildID;
    }
    void AddGuildID(int iGuildID)
    {
        if (0 == iGuildID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildID(iGuildID);
        m_stData.m_iGuildID += iGuildID;
    }
    void SetGuildID(int iGuildID)
    {
        if (m_stData.m_iGuildID == iGuildID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGuildID - m_stData.m_iGuildID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildID(RealAdd);
        m_stData.m_iGuildID = iGuildID;
    }
    const char& GetPost() const
    {
        return m_stData.m_cPost;
    }
    void AddPost(char cPost)
    {
        if (0 == cPost)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPost(cPost);
        m_stData.m_cPost += cPost;
    }
    void SetPost(char cPost)
    {
        if (m_stData.m_cPost == cPost)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cPost - m_stData.m_cPost;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPost(RealAdd);
        m_stData.m_cPost = cPost;
    }
    const int& GetContributionTotal() const
    {
        return m_stData.m_iContributionTotal;
    }
    void AddContributionTotal(int iContributionTotal)
    {
        if (0 == iContributionTotal)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetContributionTotal(iContributionTotal);
        m_stData.m_iContributionTotal += iContributionTotal;
    }
    void SetContributionTotal(int iContributionTotal)
    {
        if (m_stData.m_iContributionTotal == iContributionTotal)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iContributionTotal - m_stData.m_iContributionTotal;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetContributionTotal(RealAdd);
        m_stData.m_iContributionTotal = iContributionTotal;
    }
    const int& GetJoinTime() const
    {
        return m_stData.m_iJoinTime;
    }
    void AddJoinTime(int iJoinTime)
    {
        if (0 == iJoinTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetJoinTime(iJoinTime);
        m_stData.m_iJoinTime += iJoinTime;
    }
    void SetJoinTime(int iJoinTime)
    {
        if (m_stData.m_iJoinTime == iJoinTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iJoinTime - m_stData.m_iJoinTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetJoinTime(RealAdd);
        m_stData.m_iJoinTime = iJoinTime;
    }
    const uint64_t& GetGuildWarHurt() const
    {
        return m_stData.m_ulGuildWarHurt;
    }
    void AddGuildWarHurt(uint64_t ulGuildWarHurt)
    {
        if (0 == ulGuildWarHurt)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildWarHurt(ulGuildWarHurt);
        m_stData.m_ulGuildWarHurt += ulGuildWarHurt;
    }
    void SetGuildWarHurt(uint64_t ulGuildWarHurt)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulGuildWarHurt > ulGuildWarHurt)
        {
            m_pRecord->SetGuildWarHurt(m_stData.m_ulGuildWarHurt - ulGuildWarHurt, '-');
        }
        else
        {
            m_pRecord->SetGuildWarHurt(ulGuildWarHurt - m_stData.m_ulGuildWarHurt, '+');
        }
        m_stData.m_ulGuildWarHurt = ulGuildWarHurt;
    }

    CRecordGuildMemberTable* m_pRecord;
    TGuildMemberTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGuildMemberTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGuildMemberTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGuildMemberTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
