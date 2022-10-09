#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGlobalBasicTable;
struct TGlobalBasicTableValueType;

class CRecordGlobalBasicTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGlobalBasicTable() : m_pCache(nullptr) { }

    virtual ~CRecordGlobalBasicTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetServerID();
    virtual int GetUid() { return 0; }
    virtual int GetGroupID() { return 0; }

    static std::vector<TGlobalBasicTableValueType> SelectDataFromSQL(int iServerID);

    static const char* s_strTableName;
    static const char* s_strPrimaryKeyIndex;
    static const char* s_strLastAwardRankTime;
    static const char* s_strFightPowerAwardRankTime;
    static const char* s_strLastMergeServerTime;
    static const char* s_strCountdownHeroActivityID;
    static const char* s_strCheckUpdateTime;
    static const char* s_strEntireLastUpdateTime;
    static const char* s_strEntireWarLevel;
    static const char* s_strEntireWarThemeID;
    static const char* s_strEntireWarGodUid;
    static const char* s_strEntireWarGodGroupID;
    static const char* s_strEntireWarBuff;
    static const char* s_strNewsData;
    static const char* s_strAffixVec;
    static const char* s_strGuildWarCurBoss;
    static const char* s_strServerID;

    TGlobalBasicTable* m_pCache;

    void SetPrimaryKeyIndex(int iPrimaryKeyIndex)
    {
        m_stIntData[s_strPrimaryKeyIndex] += iPrimaryKeyIndex;
    }
    void SetLastAwardRankTime(int iLastAwardRankTime)
    {
        m_stIntData[s_strLastAwardRankTime] += iLastAwardRankTime;
    }
    void SetFightPowerAwardRankTime(int iFightPowerAwardRankTime)
    {
        m_stIntData[s_strFightPowerAwardRankTime] += iFightPowerAwardRankTime;
    }
    void SetLastMergeServerTime(int iLastMergeServerTime)
    {
        m_stIntData[s_strLastMergeServerTime] += iLastMergeServerTime;
    }
    void SetCountdownHeroActivityID(int iCountdownHeroActivityID)
    {
        m_stIntData[s_strCountdownHeroActivityID] += iCountdownHeroActivityID;
    }
    void SetCheckUpdateTime(int iCheckUpdateTime)
    {
        m_stIntData[s_strCheckUpdateTime] += iCheckUpdateTime;
    }
    void SetEntireLastUpdateTime(int iEntireLastUpdateTime)
    {
        m_stIntData[s_strEntireLastUpdateTime] += iEntireLastUpdateTime;
    }
    void SetEntireWarLevel(int iEntireWarLevel)
    {
        m_stIntData[s_strEntireWarLevel] += iEntireWarLevel;
    }
    void SetEntireWarThemeID(int iEntireWarThemeID)
    {
        m_stIntData[s_strEntireWarThemeID] += iEntireWarThemeID;
    }
    void SetEntireWarGodUid(int iEntireWarGodUid)
    {
        m_stIntData[s_strEntireWarGodUid] += iEntireWarGodUid;
    }
    void SetEntireWarGodGroupID(int iEntireWarGodGroupID)
    {
        m_stIntData[s_strEntireWarGodGroupID] += iEntireWarGodGroupID;
    }
    void SetEntireWarBuff(const CPackEntireWarBuffVector& stEntireWarBuff)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stEntireWarBuff);
        m_stVarData[s_strEntireWarBuff] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetNewsData(const CPackWenWenNewsServerData& stNewsData)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stNewsData);
        m_stVarData[s_strNewsData] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetAffixVec(const std::vector<int>& stAffixVec)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stAffixVec);
        m_stVarData[s_strAffixVec] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetGuildWarCurBoss(const std::vector<int>& stGuildWarCurBoss)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildWarCurBoss);
        m_stVarData[s_strGuildWarCurBoss] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iServerID)
    {
        m_iServerID = iServerID;
    }
    void SetCachePtr(TGlobalBasicTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iServerID;
};

struct TGlobalBasicTableValueType
{
    TGlobalBasicTableValueType()
    : m_iServerID(0)
    , m_iPrimaryKeyIndex(1)
    , m_iLastAwardRankTime(0)
    , m_iFightPowerAwardRankTime(0)
    , m_iLastMergeServerTime(0)
    , m_iCountdownHeroActivityID(0)
    , m_iCheckUpdateTime(0)
    , m_iEntireLastUpdateTime(0)
    , m_iEntireWarLevel(0)
    , m_iEntireWarThemeID(0)
    , m_iEntireWarGodUid(0)
    , m_iEntireWarGodGroupID(0)
    , m_stEntireWarBuff()
    , m_stNewsData()
    , m_stAffixVec()
    , m_stGuildWarCurBoss()
    {}

    int                     m_iServerID;
    int                     m_iPrimaryKeyIndex;
    int                     m_iLastAwardRankTime;
    int                     m_iFightPowerAwardRankTime;
    int                     m_iLastMergeServerTime;
    int                     m_iCountdownHeroActivityID;
    int                     m_iCheckUpdateTime;
    int                     m_iEntireLastUpdateTime;
    int                     m_iEntireWarLevel;
    int                     m_iEntireWarThemeID;
    int                     m_iEntireWarGodUid;
    int                     m_iEntireWarGodGroupID;
    CPackEntireWarBuffVector    m_stEntireWarBuff;
    CPackWenWenNewsServerData    m_stNewsData;
    std::vector<int>        m_stAffixVec;
    std::vector<int>        m_stGuildWarCurBoss;
};

struct TGlobalBasicTable
{
    TGlobalBasicTable() : m_pRecord(nullptr) { }
    ~TGlobalBasicTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGlobalBasicTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGlobalBasicTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iServerID);
        m_pRecord->SetPrimaryKeyIndex(stData.m_iPrimaryKeyIndex);
        m_pRecord->SetLastAwardRankTime(stData.m_iLastAwardRankTime);
        m_pRecord->SetFightPowerAwardRankTime(stData.m_iFightPowerAwardRankTime);
        m_pRecord->SetLastMergeServerTime(stData.m_iLastMergeServerTime);
        m_pRecord->SetCountdownHeroActivityID(stData.m_iCountdownHeroActivityID);
        m_pRecord->SetCheckUpdateTime(stData.m_iCheckUpdateTime);
        m_pRecord->SetEntireLastUpdateTime(stData.m_iEntireLastUpdateTime);
        m_pRecord->SetEntireWarLevel(stData.m_iEntireWarLevel);
        m_pRecord->SetEntireWarThemeID(stData.m_iEntireWarThemeID);
        m_pRecord->SetEntireWarGodUid(stData.m_iEntireWarGodUid);
        m_pRecord->SetEntireWarGodGroupID(stData.m_iEntireWarGodGroupID);
        m_pRecord->SetEntireWarBuff(stData.m_stEntireWarBuff);
        m_pRecord->SetNewsData(stData.m_stNewsData);
        m_pRecord->SetAffixVec(stData.m_stAffixVec);
        m_pRecord->SetGuildWarCurBoss(stData.m_stGuildWarCurBoss);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetPrimaryKeyIndex() const
    {
        return m_stData.m_iPrimaryKeyIndex;
    }
    void AddPrimaryKeyIndex(int iPrimaryKeyIndex)
    {
        if (0 == iPrimaryKeyIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPrimaryKeyIndex(iPrimaryKeyIndex);
        m_stData.m_iPrimaryKeyIndex += iPrimaryKeyIndex;
    }
    void SetPrimaryKeyIndex(int iPrimaryKeyIndex)
    {
        if (m_stData.m_iPrimaryKeyIndex == iPrimaryKeyIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPrimaryKeyIndex - m_stData.m_iPrimaryKeyIndex;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPrimaryKeyIndex(RealAdd);
        m_stData.m_iPrimaryKeyIndex = iPrimaryKeyIndex;
    }
    const int& GetLastAwardRankTime() const
    {
        return m_stData.m_iLastAwardRankTime;
    }
    void AddLastAwardRankTime(int iLastAwardRankTime)
    {
        if (0 == iLastAwardRankTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastAwardRankTime(iLastAwardRankTime);
        m_stData.m_iLastAwardRankTime += iLastAwardRankTime;
    }
    void SetLastAwardRankTime(int iLastAwardRankTime)
    {
        if (m_stData.m_iLastAwardRankTime == iLastAwardRankTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastAwardRankTime - m_stData.m_iLastAwardRankTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastAwardRankTime(RealAdd);
        m_stData.m_iLastAwardRankTime = iLastAwardRankTime;
    }
    const int& GetFightPowerAwardRankTime() const
    {
        return m_stData.m_iFightPowerAwardRankTime;
    }
    void AddFightPowerAwardRankTime(int iFightPowerAwardRankTime)
    {
        if (0 == iFightPowerAwardRankTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightPowerAwardRankTime(iFightPowerAwardRankTime);
        m_stData.m_iFightPowerAwardRankTime += iFightPowerAwardRankTime;
    }
    void SetFightPowerAwardRankTime(int iFightPowerAwardRankTime)
    {
        if (m_stData.m_iFightPowerAwardRankTime == iFightPowerAwardRankTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFightPowerAwardRankTime - m_stData.m_iFightPowerAwardRankTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightPowerAwardRankTime(RealAdd);
        m_stData.m_iFightPowerAwardRankTime = iFightPowerAwardRankTime;
    }
    const int& GetLastMergeServerTime() const
    {
        return m_stData.m_iLastMergeServerTime;
    }
    void AddLastMergeServerTime(int iLastMergeServerTime)
    {
        if (0 == iLastMergeServerTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastMergeServerTime(iLastMergeServerTime);
        m_stData.m_iLastMergeServerTime += iLastMergeServerTime;
    }
    void SetLastMergeServerTime(int iLastMergeServerTime)
    {
        if (m_stData.m_iLastMergeServerTime == iLastMergeServerTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastMergeServerTime - m_stData.m_iLastMergeServerTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastMergeServerTime(RealAdd);
        m_stData.m_iLastMergeServerTime = iLastMergeServerTime;
    }
    const int& GetCountdownHeroActivityID() const
    {
        return m_stData.m_iCountdownHeroActivityID;
    }
    void AddCountdownHeroActivityID(int iCountdownHeroActivityID)
    {
        if (0 == iCountdownHeroActivityID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCountdownHeroActivityID(iCountdownHeroActivityID);
        m_stData.m_iCountdownHeroActivityID += iCountdownHeroActivityID;
    }
    void SetCountdownHeroActivityID(int iCountdownHeroActivityID)
    {
        if (m_stData.m_iCountdownHeroActivityID == iCountdownHeroActivityID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCountdownHeroActivityID - m_stData.m_iCountdownHeroActivityID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCountdownHeroActivityID(RealAdd);
        m_stData.m_iCountdownHeroActivityID = iCountdownHeroActivityID;
    }
    const int& GetCheckUpdateTime() const
    {
        return m_stData.m_iCheckUpdateTime;
    }
    void AddCheckUpdateTime(int iCheckUpdateTime)
    {
        if (0 == iCheckUpdateTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCheckUpdateTime(iCheckUpdateTime);
        m_stData.m_iCheckUpdateTime += iCheckUpdateTime;
    }
    void SetCheckUpdateTime(int iCheckUpdateTime)
    {
        if (m_stData.m_iCheckUpdateTime == iCheckUpdateTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCheckUpdateTime - m_stData.m_iCheckUpdateTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCheckUpdateTime(RealAdd);
        m_stData.m_iCheckUpdateTime = iCheckUpdateTime;
    }
    const int& GetEntireLastUpdateTime() const
    {
        return m_stData.m_iEntireLastUpdateTime;
    }
    void AddEntireLastUpdateTime(int iEntireLastUpdateTime)
    {
        if (0 == iEntireLastUpdateTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireLastUpdateTime(iEntireLastUpdateTime);
        m_stData.m_iEntireLastUpdateTime += iEntireLastUpdateTime;
    }
    void SetEntireLastUpdateTime(int iEntireLastUpdateTime)
    {
        if (m_stData.m_iEntireLastUpdateTime == iEntireLastUpdateTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEntireLastUpdateTime - m_stData.m_iEntireLastUpdateTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireLastUpdateTime(RealAdd);
        m_stData.m_iEntireLastUpdateTime = iEntireLastUpdateTime;
    }
    const int& GetEntireWarLevel() const
    {
        return m_stData.m_iEntireWarLevel;
    }
    void AddEntireWarLevel(int iEntireWarLevel)
    {
        if (0 == iEntireWarLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarLevel(iEntireWarLevel);
        m_stData.m_iEntireWarLevel += iEntireWarLevel;
    }
    void SetEntireWarLevel(int iEntireWarLevel)
    {
        if (m_stData.m_iEntireWarLevel == iEntireWarLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEntireWarLevel - m_stData.m_iEntireWarLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarLevel(RealAdd);
        m_stData.m_iEntireWarLevel = iEntireWarLevel;
    }
    const int& GetEntireWarThemeID() const
    {
        return m_stData.m_iEntireWarThemeID;
    }
    void AddEntireWarThemeID(int iEntireWarThemeID)
    {
        if (0 == iEntireWarThemeID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarThemeID(iEntireWarThemeID);
        m_stData.m_iEntireWarThemeID += iEntireWarThemeID;
    }
    void SetEntireWarThemeID(int iEntireWarThemeID)
    {
        if (m_stData.m_iEntireWarThemeID == iEntireWarThemeID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEntireWarThemeID - m_stData.m_iEntireWarThemeID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarThemeID(RealAdd);
        m_stData.m_iEntireWarThemeID = iEntireWarThemeID;
    }
    const int& GetEntireWarGodUid() const
    {
        return m_stData.m_iEntireWarGodUid;
    }
    void AddEntireWarGodUid(int iEntireWarGodUid)
    {
        if (0 == iEntireWarGodUid)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarGodUid(iEntireWarGodUid);
        m_stData.m_iEntireWarGodUid += iEntireWarGodUid;
    }
    void SetEntireWarGodUid(int iEntireWarGodUid)
    {
        if (m_stData.m_iEntireWarGodUid == iEntireWarGodUid)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEntireWarGodUid - m_stData.m_iEntireWarGodUid;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarGodUid(RealAdd);
        m_stData.m_iEntireWarGodUid = iEntireWarGodUid;
    }
    const int& GetEntireWarGodGroupID() const
    {
        return m_stData.m_iEntireWarGodGroupID;
    }
    void AddEntireWarGodGroupID(int iEntireWarGodGroupID)
    {
        if (0 == iEntireWarGodGroupID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarGodGroupID(iEntireWarGodGroupID);
        m_stData.m_iEntireWarGodGroupID += iEntireWarGodGroupID;
    }
    void SetEntireWarGodGroupID(int iEntireWarGodGroupID)
    {
        if (m_stData.m_iEntireWarGodGroupID == iEntireWarGodGroupID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEntireWarGodGroupID - m_stData.m_iEntireWarGodGroupID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarGodGroupID(RealAdd);
        m_stData.m_iEntireWarGodGroupID = iEntireWarGodGroupID;
    }
    const CPackEntireWarBuffVector& GetEntireWarBuff() const
    {
        return m_stData.m_stEntireWarBuff;
    }
    void SetEntireWarBuff(const CPackEntireWarBuffVector& stEntireWarBuff)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarBuff(stEntireWarBuff);
        m_stData.m_stEntireWarBuff = stEntireWarBuff;
    }
    const CPackWenWenNewsServerData& GetNewsData() const
    {
        return m_stData.m_stNewsData;
    }
    void SetNewsData(const CPackWenWenNewsServerData& stNewsData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewsData(stNewsData);
        m_stData.m_stNewsData = stNewsData;
    }
    const std::vector<int>& GetAffixVec() const
    {
        return m_stData.m_stAffixVec;
    }
    void SetAffixVec(const std::vector<int>& stAffixVec)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAffixVec(stAffixVec);
        m_stData.m_stAffixVec = stAffixVec;
    }
    const std::vector<int>& GetGuildWarCurBoss() const
    {
        return m_stData.m_stGuildWarCurBoss;
    }
    void SetGuildWarCurBoss(const std::vector<int>& stGuildWarCurBoss)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildWarCurBoss(stGuildWarCurBoss);
        m_stData.m_stGuildWarCurBoss = stGuildWarCurBoss;
    }

    CRecordGlobalBasicTable* m_pRecord;
    TGlobalBasicTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGlobalBasicTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGlobalBasicTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iServerID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGlobalBasicTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
