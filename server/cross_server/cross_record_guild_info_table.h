#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGuildInfoTable;
struct TGuildInfoTableValueType;

class CRecordGuildInfoTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGuildInfoTable() : m_pCache(nullptr) { }

    virtual ~CRecordGuildInfoTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetGuildID();
    virtual int GetUid() { return 0; }
    virtual int GetGroupID() { return 0; }

    static std::vector<TGuildInfoTableValueType> SelectDataFromSQL(int iGuildID);

    static const char* s_strTableName;
    static const char* s_strGuildName;
    static const char* s_strGuildExp;
    static const char* s_strChiefUid;
    static const char* s_strChiefGroup;
    static const char* s_strBulletin;
    static const char* s_strGuildBossTotalHurt;
    static const char* s_strLastRefreshTime;
    static const char* s_strChangeNameCd;
    static const char* s_strChangeNameFree;
    static const char* s_strGuildDonate;
    static const char* s_strGuildSignRewardLevel;
    static const char* s_strGuildUnlockItem;
    static const char* s_strGuildUnlockDecorate;
    static const char* s_strGuildFootCloth;
    static const char* s_strGuildDecorate;
    static const char* s_strGuildWarBossInfo;
    static const char* s_strGuildWarRecord;
    static const char* s_strGuildWarScore;
    static const char* s_strGuildWarEnd;
    static const char* s_strGuildID;

    TGuildInfoTable* m_pCache;

    void SetGuildName(const std::string& strGuildName)
    {
        m_stStringData[s_strGuildName] = strGuildName;
    }
    void SetGuildExp(int iGuildExp)
    {
        m_stIntData[s_strGuildExp] += iGuildExp;
    }
    void SetChiefUid(int iChiefUid)
    {
        m_stIntData[s_strChiefUid] += iChiefUid;
    }
    void SetChiefGroup(int iChiefGroup)
    {
        m_stIntData[s_strChiefGroup] += iChiefGroup;
    }
    void SetBulletin(const std::string& strBulletin)
    {
        m_stStringData[s_strBulletin] = strBulletin;
    }
    void SetGuildBossTotalHurt(uint64_t ulGuildBossTotalHurt, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strGuildBossTotalHurt];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulGuildBossTotalHurt;
        }
        else
        {
            if (stDataRef.m_ulVal > ulGuildBossTotalHurt)
            {
                stDataRef.m_ulVal -= ulGuildBossTotalHurt;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulGuildBossTotalHurt - stDataRef.m_ulVal;
            }
        }
    }
    void SetLastRefreshTime(int iLastRefreshTime)
    {
        m_stIntData[s_strLastRefreshTime] += iLastRefreshTime;
    }
    void SetChangeNameCd(int iChangeNameCd)
    {
        m_stIntData[s_strChangeNameCd] += iChangeNameCd;
    }
    void SetChangeNameFree(char cChangeNameFree)
    {
        m_stCharData[s_strChangeNameFree] += cChangeNameFree;
    }
    void SetGuildDonate(int iGuildDonate)
    {
        m_stIntData[s_strGuildDonate] += iGuildDonate;
    }
    void SetGuildSignRewardLevel(int iGuildSignRewardLevel)
    {
        m_stIntData[s_strGuildSignRewardLevel] += iGuildSignRewardLevel;
    }
    void SetGuildUnlockItem(const std::vector<int>& stGuildUnlockItem)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildUnlockItem);
        m_stVarData[s_strGuildUnlockItem] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetGuildUnlockDecorate(const std::vector<int>& stGuildUnlockDecorate)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildUnlockDecorate);
        m_stVarData[s_strGuildUnlockDecorate] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetGuildFootCloth(const std::map<int,std::map<int,std::map<int,char>>>& stGuildFootCloth)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildFootCloth);
        m_stVarData[s_strGuildFootCloth] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetGuildDecorate(const std::map<int,CPackGuildHomeInfo>& stGuildDecorate)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildDecorate);
        m_stVarData[s_strGuildDecorate] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetGuildWarBossInfo(const std::map<int,CPackGuildWarBossInfo>& stGuildWarBossInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildWarBossInfo);
        m_stVarData[s_strGuildWarBossInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetGuildWarRecord(const std::vector<CPackGuildWarRecord>	& stGuildWarRecord)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildWarRecord);
        m_stVarData[s_strGuildWarRecord] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetGuildWarScore(uint64_t ulGuildWarScore, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strGuildWarScore];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulGuildWarScore;
        }
        else
        {
            if (stDataRef.m_ulVal > ulGuildWarScore)
            {
                stDataRef.m_ulVal -= ulGuildWarScore;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulGuildWarScore - stDataRef.m_ulVal;
            }
        }
    }
    void SetGuildWarEnd(int iGuildWarEnd)
    {
        m_stIntData[s_strGuildWarEnd] += iGuildWarEnd;
    }
    void SetPrimaryKey(int iGuildID)
    {
        m_iGuildID = iGuildID;
    }
    void SetCachePtr(TGuildInfoTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iGuildID;
};

struct TGuildInfoTableValueType
{
    TGuildInfoTableValueType()
    : m_iGuildID(0)
    , m_strGuildName("")
    , m_iGuildExp(0)
    , m_iChiefUid(0)
    , m_iChiefGroup(0)
    , m_strBulletin("")
    , m_ulGuildBossTotalHurt(0)
    , m_iLastRefreshTime(0)
    , m_iChangeNameCd(0)
    , m_cChangeNameFree(0)
    , m_iGuildDonate(0)
    , m_iGuildSignRewardLevel(1)
    , m_stGuildUnlockItem()
    , m_stGuildUnlockDecorate()
    , m_stGuildFootCloth()
    , m_stGuildDecorate()
    , m_stGuildWarBossInfo()
    , m_stGuildWarRecord()
    , m_ulGuildWarScore(0)
    , m_iGuildWarEnd(0)
    {}

    int                     m_iGuildID;
    std::string             m_strGuildName;
    int                     m_iGuildExp;
    int                     m_iChiefUid;
    int                     m_iChiefGroup;
    std::string             m_strBulletin;
    uint64_t                m_ulGuildBossTotalHurt;
    int                     m_iLastRefreshTime;
    int                     m_iChangeNameCd;
    char                    m_cChangeNameFree;
    int                     m_iGuildDonate;
    int                     m_iGuildSignRewardLevel;
    std::vector<int>        m_stGuildUnlockItem;
    std::vector<int>        m_stGuildUnlockDecorate;
    std::map<int,std::map<int,std::map<int,char>>>    m_stGuildFootCloth;
    std::map<int,CPackGuildHomeInfo>    m_stGuildDecorate;
    std::map<int,CPackGuildWarBossInfo>    m_stGuildWarBossInfo;
    std::vector<CPackGuildWarRecord>	    m_stGuildWarRecord;
    uint64_t                m_ulGuildWarScore;
    int                     m_iGuildWarEnd;
};

struct TGuildInfoTable
{
    TGuildInfoTable() : m_pRecord(nullptr) { }
    ~TGuildInfoTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGuildInfoTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGuildInfoTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iGuildID);
        m_pRecord->SetGuildName(stData.m_strGuildName);
        m_pRecord->SetGuildExp(stData.m_iGuildExp);
        m_pRecord->SetChiefUid(stData.m_iChiefUid);
        m_pRecord->SetChiefGroup(stData.m_iChiefGroup);
        m_pRecord->SetBulletin(stData.m_strBulletin);
        m_pRecord->SetGuildBossTotalHurt(stData.m_ulGuildBossTotalHurt);
        m_pRecord->SetLastRefreshTime(stData.m_iLastRefreshTime);
        m_pRecord->SetChangeNameCd(stData.m_iChangeNameCd);
        m_pRecord->SetChangeNameFree(stData.m_cChangeNameFree);
        m_pRecord->SetGuildDonate(stData.m_iGuildDonate);
        m_pRecord->SetGuildSignRewardLevel(stData.m_iGuildSignRewardLevel);
        m_pRecord->SetGuildUnlockItem(stData.m_stGuildUnlockItem);
        m_pRecord->SetGuildUnlockDecorate(stData.m_stGuildUnlockDecorate);
        m_pRecord->SetGuildFootCloth(stData.m_stGuildFootCloth);
        m_pRecord->SetGuildDecorate(stData.m_stGuildDecorate);
        m_pRecord->SetGuildWarBossInfo(stData.m_stGuildWarBossInfo);
        m_pRecord->SetGuildWarRecord(stData.m_stGuildWarRecord);
        m_pRecord->SetGuildWarScore(stData.m_ulGuildWarScore);
        m_pRecord->SetGuildWarEnd(stData.m_iGuildWarEnd);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const std::string& GetGuildName() const
    {
        return m_stData.m_strGuildName;
    }
    void SetGuildName(const std::string& strGuildName)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildName(strGuildName);
        m_stData.m_strGuildName = strGuildName;
    }
    const int& GetGuildExp() const
    {
        return m_stData.m_iGuildExp;
    }
    void AddGuildExp(int iGuildExp)
    {
        if (0 == iGuildExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildExp(iGuildExp);
        m_stData.m_iGuildExp += iGuildExp;
    }
    void SetGuildExp(int iGuildExp)
    {
        if (m_stData.m_iGuildExp == iGuildExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGuildExp - m_stData.m_iGuildExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildExp(RealAdd);
        m_stData.m_iGuildExp = iGuildExp;
    }
    const int& GetChiefUid() const
    {
        return m_stData.m_iChiefUid;
    }
    void AddChiefUid(int iChiefUid)
    {
        if (0 == iChiefUid)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChiefUid(iChiefUid);
        m_stData.m_iChiefUid += iChiefUid;
    }
    void SetChiefUid(int iChiefUid)
    {
        if (m_stData.m_iChiefUid == iChiefUid)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iChiefUid - m_stData.m_iChiefUid;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChiefUid(RealAdd);
        m_stData.m_iChiefUid = iChiefUid;
    }
    const int& GetChiefGroup() const
    {
        return m_stData.m_iChiefGroup;
    }
    void AddChiefGroup(int iChiefGroup)
    {
        if (0 == iChiefGroup)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChiefGroup(iChiefGroup);
        m_stData.m_iChiefGroup += iChiefGroup;
    }
    void SetChiefGroup(int iChiefGroup)
    {
        if (m_stData.m_iChiefGroup == iChiefGroup)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iChiefGroup - m_stData.m_iChiefGroup;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChiefGroup(RealAdd);
        m_stData.m_iChiefGroup = iChiefGroup;
    }
    const std::string& GetBulletin() const
    {
        return m_stData.m_strBulletin;
    }
    void SetBulletin(const std::string& strBulletin)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBulletin(strBulletin);
        m_stData.m_strBulletin = strBulletin;
    }
    const uint64_t& GetGuildBossTotalHurt() const
    {
        return m_stData.m_ulGuildBossTotalHurt;
    }
    void AddGuildBossTotalHurt(uint64_t ulGuildBossTotalHurt)
    {
        if (0 == ulGuildBossTotalHurt)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildBossTotalHurt(ulGuildBossTotalHurt);
        m_stData.m_ulGuildBossTotalHurt += ulGuildBossTotalHurt;
    }
    void SetGuildBossTotalHurt(uint64_t ulGuildBossTotalHurt)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulGuildBossTotalHurt > ulGuildBossTotalHurt)
        {
            m_pRecord->SetGuildBossTotalHurt(m_stData.m_ulGuildBossTotalHurt - ulGuildBossTotalHurt, '-');
        }
        else
        {
            m_pRecord->SetGuildBossTotalHurt(ulGuildBossTotalHurt - m_stData.m_ulGuildBossTotalHurt, '+');
        }
        m_stData.m_ulGuildBossTotalHurt = ulGuildBossTotalHurt;
    }
    const int& GetLastRefreshTime() const
    {
        return m_stData.m_iLastRefreshTime;
    }
    void AddLastRefreshTime(int iLastRefreshTime)
    {
        if (0 == iLastRefreshTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastRefreshTime(iLastRefreshTime);
        m_stData.m_iLastRefreshTime += iLastRefreshTime;
    }
    void SetLastRefreshTime(int iLastRefreshTime)
    {
        if (m_stData.m_iLastRefreshTime == iLastRefreshTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastRefreshTime - m_stData.m_iLastRefreshTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastRefreshTime(RealAdd);
        m_stData.m_iLastRefreshTime = iLastRefreshTime;
    }
    const int& GetChangeNameCd() const
    {
        return m_stData.m_iChangeNameCd;
    }
    void AddChangeNameCd(int iChangeNameCd)
    {
        if (0 == iChangeNameCd)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChangeNameCd(iChangeNameCd);
        m_stData.m_iChangeNameCd += iChangeNameCd;
    }
    void SetChangeNameCd(int iChangeNameCd)
    {
        if (m_stData.m_iChangeNameCd == iChangeNameCd)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iChangeNameCd - m_stData.m_iChangeNameCd;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChangeNameCd(RealAdd);
        m_stData.m_iChangeNameCd = iChangeNameCd;
    }
    const char& GetChangeNameFree() const
    {
        return m_stData.m_cChangeNameFree;
    }
    void AddChangeNameFree(char cChangeNameFree)
    {
        if (0 == cChangeNameFree)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChangeNameFree(cChangeNameFree);
        m_stData.m_cChangeNameFree += cChangeNameFree;
    }
    void SetChangeNameFree(char cChangeNameFree)
    {
        if (m_stData.m_cChangeNameFree == cChangeNameFree)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cChangeNameFree - m_stData.m_cChangeNameFree;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChangeNameFree(RealAdd);
        m_stData.m_cChangeNameFree = cChangeNameFree;
    }
    const int& GetGuildDonate() const
    {
        return m_stData.m_iGuildDonate;
    }
    void AddGuildDonate(int iGuildDonate)
    {
        if (0 == iGuildDonate)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildDonate(iGuildDonate);
        m_stData.m_iGuildDonate += iGuildDonate;
    }
    void SetGuildDonate(int iGuildDonate)
    {
        if (m_stData.m_iGuildDonate == iGuildDonate)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGuildDonate - m_stData.m_iGuildDonate;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildDonate(RealAdd);
        m_stData.m_iGuildDonate = iGuildDonate;
    }
    const int& GetGuildSignRewardLevel() const
    {
        return m_stData.m_iGuildSignRewardLevel;
    }
    void AddGuildSignRewardLevel(int iGuildSignRewardLevel)
    {
        if (0 == iGuildSignRewardLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildSignRewardLevel(iGuildSignRewardLevel);
        m_stData.m_iGuildSignRewardLevel += iGuildSignRewardLevel;
    }
    void SetGuildSignRewardLevel(int iGuildSignRewardLevel)
    {
        if (m_stData.m_iGuildSignRewardLevel == iGuildSignRewardLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGuildSignRewardLevel - m_stData.m_iGuildSignRewardLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildSignRewardLevel(RealAdd);
        m_stData.m_iGuildSignRewardLevel = iGuildSignRewardLevel;
    }
    const std::vector<int>& GetGuildUnlockItem() const
    {
        return m_stData.m_stGuildUnlockItem;
    }
    void SetGuildUnlockItem(const std::vector<int>& stGuildUnlockItem)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildUnlockItem(stGuildUnlockItem);
        m_stData.m_stGuildUnlockItem = stGuildUnlockItem;
    }
    const std::vector<int>& GetGuildUnlockDecorate() const
    {
        return m_stData.m_stGuildUnlockDecorate;
    }
    void SetGuildUnlockDecorate(const std::vector<int>& stGuildUnlockDecorate)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildUnlockDecorate(stGuildUnlockDecorate);
        m_stData.m_stGuildUnlockDecorate = stGuildUnlockDecorate;
    }
    const std::map<int,std::map<int,std::map<int,char>>>& GetGuildFootCloth() const
    {
        return m_stData.m_stGuildFootCloth;
    }
    void SetGuildFootCloth(const std::map<int,std::map<int,std::map<int,char>>>& stGuildFootCloth)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildFootCloth(stGuildFootCloth);
        m_stData.m_stGuildFootCloth = stGuildFootCloth;
    }
    const std::map<int,CPackGuildHomeInfo>& GetGuildDecorate() const
    {
        return m_stData.m_stGuildDecorate;
    }
    void SetGuildDecorate(const std::map<int,CPackGuildHomeInfo>& stGuildDecorate)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildDecorate(stGuildDecorate);
        m_stData.m_stGuildDecorate = stGuildDecorate;
    }
    const std::map<int,CPackGuildWarBossInfo>& GetGuildWarBossInfo() const
    {
        return m_stData.m_stGuildWarBossInfo;
    }
    void SetGuildWarBossInfo(const std::map<int,CPackGuildWarBossInfo>& stGuildWarBossInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildWarBossInfo(stGuildWarBossInfo);
        m_stData.m_stGuildWarBossInfo = stGuildWarBossInfo;
    }
    const std::vector<CPackGuildWarRecord>	& GetGuildWarRecord() const
    {
        return m_stData.m_stGuildWarRecord;
    }
    void SetGuildWarRecord(const std::vector<CPackGuildWarRecord>	& stGuildWarRecord)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildWarRecord(stGuildWarRecord);
        m_stData.m_stGuildWarRecord = stGuildWarRecord;
    }
    const uint64_t& GetGuildWarScore() const
    {
        return m_stData.m_ulGuildWarScore;
    }
    void AddGuildWarScore(uint64_t ulGuildWarScore)
    {
        if (0 == ulGuildWarScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildWarScore(ulGuildWarScore);
        m_stData.m_ulGuildWarScore += ulGuildWarScore;
    }
    void SetGuildWarScore(uint64_t ulGuildWarScore)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulGuildWarScore > ulGuildWarScore)
        {
            m_pRecord->SetGuildWarScore(m_stData.m_ulGuildWarScore - ulGuildWarScore, '-');
        }
        else
        {
            m_pRecord->SetGuildWarScore(ulGuildWarScore - m_stData.m_ulGuildWarScore, '+');
        }
        m_stData.m_ulGuildWarScore = ulGuildWarScore;
    }
    const int& GetGuildWarEnd() const
    {
        return m_stData.m_iGuildWarEnd;
    }
    void AddGuildWarEnd(int iGuildWarEnd)
    {
        if (0 == iGuildWarEnd)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildWarEnd(iGuildWarEnd);
        m_stData.m_iGuildWarEnd += iGuildWarEnd;
    }
    void SetGuildWarEnd(int iGuildWarEnd)
    {
        if (m_stData.m_iGuildWarEnd == iGuildWarEnd)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGuildWarEnd - m_stData.m_iGuildWarEnd;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildWarEnd(RealAdd);
        m_stData.m_iGuildWarEnd = iGuildWarEnd;
    }

    CRecordGuildInfoTable* m_pRecord;
    TGuildInfoTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGuildInfoTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGuildInfoTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iGuildID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGuildInfoTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
