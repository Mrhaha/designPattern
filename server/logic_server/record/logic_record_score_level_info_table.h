#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TScoreLevelInfoTable;
struct TScoreLevelInfoTableValueType;

class CRecordScoreLevelInfoTable : public CLogicSQLTableRecordBase
{
public:
    CRecordScoreLevelInfoTable() : m_pCache(nullptr) { }

    virtual ~CRecordScoreLevelInfoTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetActivityID();

    static std::vector<TScoreLevelInfoTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strXingdongli;
    static const char* s_strLastXingdongliTime;
    static const char* s_strBuyXingdongliCount;
    static const char* s_strScore;
    static const char* s_strScoreAwardBitmap;
    static const char* s_strDepositAwardBitmap;
    static const char* s_strScoreLevelBitmap;
    static const char* s_strChallengeLevelBitmap;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strActivityID;

    TScoreLevelInfoTable* m_pCache;

    void SetXingdongli(int iXingdongli)
    {
        m_stIntData[s_strXingdongli] += iXingdongli;
    }
    void SetLastXingdongliTime(int iLastXingdongliTime)
    {
        m_stIntData[s_strLastXingdongliTime] += iLastXingdongliTime;
    }
    void SetBuyXingdongliCount(int iBuyXingdongliCount)
    {
        m_stIntData[s_strBuyXingdongliCount] += iBuyXingdongliCount;
    }
    void SetScore(int iScore)
    {
        m_stIntData[s_strScore] += iScore;
    }
    void SetScoreAwardBitmap(uint64_t ulScoreAwardBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strScoreAwardBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulScoreAwardBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulScoreAwardBitmap)
            {
                stDataRef.m_ulVal -= ulScoreAwardBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulScoreAwardBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetDepositAwardBitmap(uint64_t ulDepositAwardBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strDepositAwardBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulDepositAwardBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulDepositAwardBitmap)
            {
                stDataRef.m_ulVal -= ulDepositAwardBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulDepositAwardBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetScoreLevelBitmap(uint64_t ulScoreLevelBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strScoreLevelBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulScoreLevelBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulScoreLevelBitmap)
            {
                stDataRef.m_ulVal -= ulScoreLevelBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulScoreLevelBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetChallengeLevelBitmap(uint64_t ulChallengeLevelBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strChallengeLevelBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulChallengeLevelBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulChallengeLevelBitmap)
            {
                stDataRef.m_ulVal -= ulChallengeLevelBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulChallengeLevelBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iActivityID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iActivityID = iActivityID;
    }
    void SetCachePtr(TScoreLevelInfoTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iActivityID;
};

struct TScoreLevelInfoTableValueType
{
    TScoreLevelInfoTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iActivityID(0)
    , m_iXingdongli(0)
    , m_iLastXingdongliTime(0)
    , m_iBuyXingdongliCount(0)
    , m_iScore(0)
    , m_ulScoreAwardBitmap(0)
    , m_ulDepositAwardBitmap(0)
    , m_ulScoreLevelBitmap(0)
    , m_ulChallengeLevelBitmap(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iActivityID;
    int                     m_iXingdongli;
    int                     m_iLastXingdongliTime;
    int                     m_iBuyXingdongliCount;
    int                     m_iScore;
    uint64_t                m_ulScoreAwardBitmap;
    uint64_t                m_ulDepositAwardBitmap;
    uint64_t                m_ulScoreLevelBitmap;
    uint64_t                m_ulChallengeLevelBitmap;
};

struct TScoreLevelInfoTable
{
    TScoreLevelInfoTable() : m_pRecord(nullptr) { }
    ~TScoreLevelInfoTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TScoreLevelInfoTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TScoreLevelInfoTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iActivityID);
        m_pRecord->SetXingdongli(stData.m_iXingdongli);
        m_pRecord->SetLastXingdongliTime(stData.m_iLastXingdongliTime);
        m_pRecord->SetBuyXingdongliCount(stData.m_iBuyXingdongliCount);
        m_pRecord->SetScore(stData.m_iScore);
        m_pRecord->SetScoreAwardBitmap(stData.m_ulScoreAwardBitmap);
        m_pRecord->SetDepositAwardBitmap(stData.m_ulDepositAwardBitmap);
        m_pRecord->SetScoreLevelBitmap(stData.m_ulScoreLevelBitmap);
        m_pRecord->SetChallengeLevelBitmap(stData.m_ulChallengeLevelBitmap);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetXingdongli() const
    {
        return m_stData.m_iXingdongli;
    }
    void AddXingdongli(int iXingdongli)
    {
        if (0 == iXingdongli)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetXingdongli(iXingdongli);
        m_stData.m_iXingdongli += iXingdongli;
    }
    void SetXingdongli(int iXingdongli)
    {
        if (m_stData.m_iXingdongli == iXingdongli)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iXingdongli - m_stData.m_iXingdongli;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetXingdongli(RealAdd);
        m_stData.m_iXingdongli = iXingdongli;
    }
    const int& GetLastXingdongliTime() const
    {
        return m_stData.m_iLastXingdongliTime;
    }
    void AddLastXingdongliTime(int iLastXingdongliTime)
    {
        if (0 == iLastXingdongliTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastXingdongliTime(iLastXingdongliTime);
        m_stData.m_iLastXingdongliTime += iLastXingdongliTime;
    }
    void SetLastXingdongliTime(int iLastXingdongliTime)
    {
        if (m_stData.m_iLastXingdongliTime == iLastXingdongliTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastXingdongliTime - m_stData.m_iLastXingdongliTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastXingdongliTime(RealAdd);
        m_stData.m_iLastXingdongliTime = iLastXingdongliTime;
    }
    const int& GetBuyXingdongliCount() const
    {
        return m_stData.m_iBuyXingdongliCount;
    }
    void AddBuyXingdongliCount(int iBuyXingdongliCount)
    {
        if (0 == iBuyXingdongliCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyXingdongliCount(iBuyXingdongliCount);
        m_stData.m_iBuyXingdongliCount += iBuyXingdongliCount;
    }
    void SetBuyXingdongliCount(int iBuyXingdongliCount)
    {
        if (m_stData.m_iBuyXingdongliCount == iBuyXingdongliCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyXingdongliCount - m_stData.m_iBuyXingdongliCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyXingdongliCount(RealAdd);
        m_stData.m_iBuyXingdongliCount = iBuyXingdongliCount;
    }
    const int& GetScore() const
    {
        return m_stData.m_iScore;
    }
    void AddScore(int iScore)
    {
        if (0 == iScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetScore(iScore);
        m_stData.m_iScore += iScore;
    }
    void SetScore(int iScore)
    {
        if (m_stData.m_iScore == iScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iScore - m_stData.m_iScore;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetScore(RealAdd);
        m_stData.m_iScore = iScore;
    }
    const uint64_t& GetScoreAwardBitmap() const
    {
        return m_stData.m_ulScoreAwardBitmap;
    }
    void AddScoreAwardBitmap(uint64_t ulScoreAwardBitmap)
    {
        if (0 == ulScoreAwardBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetScoreAwardBitmap(ulScoreAwardBitmap);
        m_stData.m_ulScoreAwardBitmap += ulScoreAwardBitmap;
    }
    void SetScoreAwardBitmap(uint64_t ulScoreAwardBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulScoreAwardBitmap > ulScoreAwardBitmap)
        {
            m_pRecord->SetScoreAwardBitmap(m_stData.m_ulScoreAwardBitmap - ulScoreAwardBitmap, '-');
        }
        else
        {
            m_pRecord->SetScoreAwardBitmap(ulScoreAwardBitmap - m_stData.m_ulScoreAwardBitmap, '+');
        }
        m_stData.m_ulScoreAwardBitmap = ulScoreAwardBitmap;
    }
    const uint64_t& GetDepositAwardBitmap() const
    {
        return m_stData.m_ulDepositAwardBitmap;
    }
    void AddDepositAwardBitmap(uint64_t ulDepositAwardBitmap)
    {
        if (0 == ulDepositAwardBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDepositAwardBitmap(ulDepositAwardBitmap);
        m_stData.m_ulDepositAwardBitmap += ulDepositAwardBitmap;
    }
    void SetDepositAwardBitmap(uint64_t ulDepositAwardBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulDepositAwardBitmap > ulDepositAwardBitmap)
        {
            m_pRecord->SetDepositAwardBitmap(m_stData.m_ulDepositAwardBitmap - ulDepositAwardBitmap, '-');
        }
        else
        {
            m_pRecord->SetDepositAwardBitmap(ulDepositAwardBitmap - m_stData.m_ulDepositAwardBitmap, '+');
        }
        m_stData.m_ulDepositAwardBitmap = ulDepositAwardBitmap;
    }
    const uint64_t& GetScoreLevelBitmap() const
    {
        return m_stData.m_ulScoreLevelBitmap;
    }
    void AddScoreLevelBitmap(uint64_t ulScoreLevelBitmap)
    {
        if (0 == ulScoreLevelBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetScoreLevelBitmap(ulScoreLevelBitmap);
        m_stData.m_ulScoreLevelBitmap += ulScoreLevelBitmap;
    }
    void SetScoreLevelBitmap(uint64_t ulScoreLevelBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulScoreLevelBitmap > ulScoreLevelBitmap)
        {
            m_pRecord->SetScoreLevelBitmap(m_stData.m_ulScoreLevelBitmap - ulScoreLevelBitmap, '-');
        }
        else
        {
            m_pRecord->SetScoreLevelBitmap(ulScoreLevelBitmap - m_stData.m_ulScoreLevelBitmap, '+');
        }
        m_stData.m_ulScoreLevelBitmap = ulScoreLevelBitmap;
    }
    const uint64_t& GetChallengeLevelBitmap() const
    {
        return m_stData.m_ulChallengeLevelBitmap;
    }
    void AddChallengeLevelBitmap(uint64_t ulChallengeLevelBitmap)
    {
        if (0 == ulChallengeLevelBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChallengeLevelBitmap(ulChallengeLevelBitmap);
        m_stData.m_ulChallengeLevelBitmap += ulChallengeLevelBitmap;
    }
    void SetChallengeLevelBitmap(uint64_t ulChallengeLevelBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulChallengeLevelBitmap > ulChallengeLevelBitmap)
        {
            m_pRecord->SetChallengeLevelBitmap(m_stData.m_ulChallengeLevelBitmap - ulChallengeLevelBitmap, '-');
        }
        else
        {
            m_pRecord->SetChallengeLevelBitmap(ulChallengeLevelBitmap - m_stData.m_ulChallengeLevelBitmap, '+');
        }
        m_stData.m_ulChallengeLevelBitmap = ulChallengeLevelBitmap;
    }

    CRecordScoreLevelInfoTable* m_pRecord;
    TScoreLevelInfoTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordScoreLevelInfoTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordScoreLevelInfoTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iActivityID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordScoreLevelInfoTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
