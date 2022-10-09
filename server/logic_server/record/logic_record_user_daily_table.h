#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserDailyTable;
struct TUserDailyTableValueType;

class CRecordUserDailyTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserDailyTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserDailyTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();

    static std::vector<TUserDailyTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strLastRefreshTime;
    static const char* s_strFreeTurnTableCount;
    static const char* s_strFreeCountdownHeroTableCount;
    static const char* s_strCostEnergy;
    static const char* s_strLeftTurnCardCount;
    static const char* s_strTurnCardBitmap;
    static const char* s_strTurnCardAwardBitmap;
    static const char* s_strVipDailyAwardBitmap;
    static const char* s_strWorshipWarGodCount;
    static const char* s_strEntireWarWorshippedCount;
    static const char* s_strEntireWarDailyGameTimes;
    static const char* s_strBuyActionCoinCount;
    static const char* s_strJubaoCount;
    static const char* s_strMergeLevelCount;
    static const char* s_strLuckyHammerLogCount;
    static const char* s_strQuarterVipAwardLevel;
    static const char* s_strDailyAwardState;
    static const char* s_strHeartDialogCard;
    static const char* s_strDatingPlaceTimes;
    static const char* s_strSimpleGameTimes;
    static const char* s_strFlyArrayReward;
    static const char* s_strFlyArrayRewardState;
    static const char* s_strRedBagEarnings;
    static const char* s_strLimitBossDailyNum;
    static const char* s_strLimitBoxDailyNum;
    static const char* s_strLimitBossAction;
    static const char* s_strFishingTimes;
    static const char* s_strWishSwapTimes;
    static const char* s_strUid;
    static const char* s_strGroupID;

    TUserDailyTable* m_pCache;

    void SetLastRefreshTime(int iLastRefreshTime)
    {
        m_stIntData[s_strLastRefreshTime] += iLastRefreshTime;
    }
    void SetFreeTurnTableCount(int iFreeTurnTableCount)
    {
        m_stIntData[s_strFreeTurnTableCount] += iFreeTurnTableCount;
    }
    void SetFreeCountdownHeroTableCount(int iFreeCountdownHeroTableCount)
    {
        m_stIntData[s_strFreeCountdownHeroTableCount] += iFreeCountdownHeroTableCount;
    }
    void SetCostEnergy(int iCostEnergy)
    {
        m_stIntData[s_strCostEnergy] += iCostEnergy;
    }
    void SetLeftTurnCardCount(int iLeftTurnCardCount)
    {
        m_stIntData[s_strLeftTurnCardCount] += iLeftTurnCardCount;
    }
    void SetTurnCardBitmap(uint64_t ulTurnCardBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strTurnCardBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulTurnCardBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulTurnCardBitmap)
            {
                stDataRef.m_ulVal -= ulTurnCardBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulTurnCardBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetTurnCardAwardBitmap(uint64_t ulTurnCardAwardBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strTurnCardAwardBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulTurnCardAwardBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulTurnCardAwardBitmap)
            {
                stDataRef.m_ulVal -= ulTurnCardAwardBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulTurnCardAwardBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetVipDailyAwardBitmap(uint64_t ulVipDailyAwardBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strVipDailyAwardBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulVipDailyAwardBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulVipDailyAwardBitmap)
            {
                stDataRef.m_ulVal -= ulVipDailyAwardBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulVipDailyAwardBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetWorshipWarGodCount(int iWorshipWarGodCount)
    {
        m_stIntData[s_strWorshipWarGodCount] += iWorshipWarGodCount;
    }
    void SetEntireWarWorshippedCount(int iEntireWarWorshippedCount)
    {
        m_stIntData[s_strEntireWarWorshippedCount] += iEntireWarWorshippedCount;
    }
    void SetEntireWarDailyGameTimes(int iEntireWarDailyGameTimes)
    {
        m_stIntData[s_strEntireWarDailyGameTimes] += iEntireWarDailyGameTimes;
    }
    void SetBuyActionCoinCount(int iBuyActionCoinCount)
    {
        m_stIntData[s_strBuyActionCoinCount] += iBuyActionCoinCount;
    }
    void SetJubaoCount(int iJubaoCount)
    {
        m_stIntData[s_strJubaoCount] += iJubaoCount;
    }
    void SetMergeLevelCount(int iMergeLevelCount)
    {
        m_stIntData[s_strMergeLevelCount] += iMergeLevelCount;
    }
    void SetLuckyHammerLogCount(int iLuckyHammerLogCount)
    {
        m_stIntData[s_strLuckyHammerLogCount] += iLuckyHammerLogCount;
    }
    void SetQuarterVipAwardLevel(int iQuarterVipAwardLevel)
    {
        m_stIntData[s_strQuarterVipAwardLevel] += iQuarterVipAwardLevel;
    }
    void SetDailyAwardState(uint64_t ulDailyAwardState, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strDailyAwardState];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulDailyAwardState;
        }
        else
        {
            if (stDataRef.m_ulVal > ulDailyAwardState)
            {
                stDataRef.m_ulVal -= ulDailyAwardState;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulDailyAwardState - stDataRef.m_ulVal;
            }
        }
    }
    void SetHeartDialogCard(int iHeartDialogCard)
    {
        m_stIntData[s_strHeartDialogCard] += iHeartDialogCard;
    }
    void SetDatingPlaceTimes(const std::map<uint8_t, uint8_t>& stDatingPlaceTimes)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stDatingPlaceTimes);
        m_stVarData[s_strDatingPlaceTimes] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetSimpleGameTimes(const std::map<uint8_t, uint8_t>& stSimpleGameTimes)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stSimpleGameTimes);
        m_stVarData[s_strSimpleGameTimes] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetFlyArrayReward(const std::vector<uint8_t>& stFlyArrayReward)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stFlyArrayReward);
        m_stVarData[s_strFlyArrayReward] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetFlyArrayRewardState(uint64_t ulFlyArrayRewardState, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strFlyArrayRewardState];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulFlyArrayRewardState;
        }
        else
        {
            if (stDataRef.m_ulVal > ulFlyArrayRewardState)
            {
                stDataRef.m_ulVal -= ulFlyArrayRewardState;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulFlyArrayRewardState - stDataRef.m_ulVal;
            }
        }
    }
    void SetRedBagEarnings(const std::vector<CPackGameItem>& stRedBagEarnings)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stRedBagEarnings);
        m_stVarData[s_strRedBagEarnings] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetLimitBossDailyNum(char cLimitBossDailyNum)
    {
        m_stCharData[s_strLimitBossDailyNum] += cLimitBossDailyNum;
    }
    void SetLimitBoxDailyNum(char cLimitBoxDailyNum)
    {
        m_stCharData[s_strLimitBoxDailyNum] += cLimitBoxDailyNum;
    }
    void SetLimitBossAction(int iLimitBossAction)
    {
        m_stIntData[s_strLimitBossAction] += iLimitBossAction;
    }
    void SetFishingTimes(int iFishingTimes)
    {
        m_stIntData[s_strFishingTimes] += iFishingTimes;
    }
    void SetWishSwapTimes(int iWishSwapTimes)
    {
        m_stIntData[s_strWishSwapTimes] += iWishSwapTimes;
    }
    void SetPrimaryKey(int iUid, int iGroupID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TUserDailyTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
};

struct TUserDailyTableValueType
{
    TUserDailyTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iLastRefreshTime(0)
    , m_iFreeTurnTableCount(0)
    , m_iFreeCountdownHeroTableCount(0)
    , m_iCostEnergy(0)
    , m_iLeftTurnCardCount(0)
    , m_ulTurnCardBitmap(0)
    , m_ulTurnCardAwardBitmap(0)
    , m_ulVipDailyAwardBitmap(0)
    , m_iWorshipWarGodCount(0)
    , m_iEntireWarWorshippedCount(0)
    , m_iEntireWarDailyGameTimes(0)
    , m_iBuyActionCoinCount(0)
    , m_iJubaoCount(0)
    , m_iMergeLevelCount(0)
    , m_iLuckyHammerLogCount(0)
    , m_iQuarterVipAwardLevel(0)
    , m_ulDailyAwardState(0)
    , m_iHeartDialogCard(0)
    , m_stDatingPlaceTimes()
    , m_stSimpleGameTimes()
    , m_stFlyArrayReward()
    , m_ulFlyArrayRewardState(0)
    , m_stRedBagEarnings()
    , m_cLimitBossDailyNum(0)
    , m_cLimitBoxDailyNum(0)
    , m_iLimitBossAction(0)
    , m_iFishingTimes(0)
    , m_iWishSwapTimes(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iLastRefreshTime;
    int                     m_iFreeTurnTableCount;
    int                     m_iFreeCountdownHeroTableCount;
    int                     m_iCostEnergy;
    int                     m_iLeftTurnCardCount;
    uint64_t                m_ulTurnCardBitmap;
    uint64_t                m_ulTurnCardAwardBitmap;
    uint64_t                m_ulVipDailyAwardBitmap;
    int                     m_iWorshipWarGodCount;
    int                     m_iEntireWarWorshippedCount;
    int                     m_iEntireWarDailyGameTimes;
    int                     m_iBuyActionCoinCount;
    int                     m_iJubaoCount;
    int                     m_iMergeLevelCount;
    int                     m_iLuckyHammerLogCount;
    int                     m_iQuarterVipAwardLevel;
    uint64_t                m_ulDailyAwardState;
    int                     m_iHeartDialogCard;
    std::map<uint8_t, uint8_t>    m_stDatingPlaceTimes;
    std::map<uint8_t, uint8_t>    m_stSimpleGameTimes;
    std::vector<uint8_t>    m_stFlyArrayReward;
    uint64_t                m_ulFlyArrayRewardState;
    std::vector<CPackGameItem>    m_stRedBagEarnings;
    char                    m_cLimitBossDailyNum;
    char                    m_cLimitBoxDailyNum;
    int                     m_iLimitBossAction;
    int                     m_iFishingTimes;
    int                     m_iWishSwapTimes;
};

struct TUserDailyTable
{
    TUserDailyTable() : m_pRecord(nullptr) { }
    ~TUserDailyTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserDailyTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserDailyTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID);
        m_pRecord->SetLastRefreshTime(stData.m_iLastRefreshTime);
        m_pRecord->SetFreeTurnTableCount(stData.m_iFreeTurnTableCount);
        m_pRecord->SetFreeCountdownHeroTableCount(stData.m_iFreeCountdownHeroTableCount);
        m_pRecord->SetCostEnergy(stData.m_iCostEnergy);
        m_pRecord->SetLeftTurnCardCount(stData.m_iLeftTurnCardCount);
        m_pRecord->SetTurnCardBitmap(stData.m_ulTurnCardBitmap);
        m_pRecord->SetTurnCardAwardBitmap(stData.m_ulTurnCardAwardBitmap);
        m_pRecord->SetVipDailyAwardBitmap(stData.m_ulVipDailyAwardBitmap);
        m_pRecord->SetWorshipWarGodCount(stData.m_iWorshipWarGodCount);
        m_pRecord->SetEntireWarWorshippedCount(stData.m_iEntireWarWorshippedCount);
        m_pRecord->SetEntireWarDailyGameTimes(stData.m_iEntireWarDailyGameTimes);
        m_pRecord->SetBuyActionCoinCount(stData.m_iBuyActionCoinCount);
        m_pRecord->SetJubaoCount(stData.m_iJubaoCount);
        m_pRecord->SetMergeLevelCount(stData.m_iMergeLevelCount);
        m_pRecord->SetLuckyHammerLogCount(stData.m_iLuckyHammerLogCount);
        m_pRecord->SetQuarterVipAwardLevel(stData.m_iQuarterVipAwardLevel);
        m_pRecord->SetDailyAwardState(stData.m_ulDailyAwardState);
        m_pRecord->SetHeartDialogCard(stData.m_iHeartDialogCard);
        m_pRecord->SetDatingPlaceTimes(stData.m_stDatingPlaceTimes);
        m_pRecord->SetSimpleGameTimes(stData.m_stSimpleGameTimes);
        m_pRecord->SetFlyArrayReward(stData.m_stFlyArrayReward);
        m_pRecord->SetFlyArrayRewardState(stData.m_ulFlyArrayRewardState);
        m_pRecord->SetRedBagEarnings(stData.m_stRedBagEarnings);
        m_pRecord->SetLimitBossDailyNum(stData.m_cLimitBossDailyNum);
        m_pRecord->SetLimitBoxDailyNum(stData.m_cLimitBoxDailyNum);
        m_pRecord->SetLimitBossAction(stData.m_iLimitBossAction);
        m_pRecord->SetFishingTimes(stData.m_iFishingTimes);
        m_pRecord->SetWishSwapTimes(stData.m_iWishSwapTimes);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
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
    const int& GetFreeTurnTableCount() const
    {
        return m_stData.m_iFreeTurnTableCount;
    }
    void AddFreeTurnTableCount(int iFreeTurnTableCount)
    {
        if (0 == iFreeTurnTableCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFreeTurnTableCount(iFreeTurnTableCount);
        m_stData.m_iFreeTurnTableCount += iFreeTurnTableCount;
    }
    void SetFreeTurnTableCount(int iFreeTurnTableCount)
    {
        if (m_stData.m_iFreeTurnTableCount == iFreeTurnTableCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFreeTurnTableCount - m_stData.m_iFreeTurnTableCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFreeTurnTableCount(RealAdd);
        m_stData.m_iFreeTurnTableCount = iFreeTurnTableCount;
    }
    const int& GetFreeCountdownHeroTableCount() const
    {
        return m_stData.m_iFreeCountdownHeroTableCount;
    }
    void AddFreeCountdownHeroTableCount(int iFreeCountdownHeroTableCount)
    {
        if (0 == iFreeCountdownHeroTableCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFreeCountdownHeroTableCount(iFreeCountdownHeroTableCount);
        m_stData.m_iFreeCountdownHeroTableCount += iFreeCountdownHeroTableCount;
    }
    void SetFreeCountdownHeroTableCount(int iFreeCountdownHeroTableCount)
    {
        if (m_stData.m_iFreeCountdownHeroTableCount == iFreeCountdownHeroTableCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFreeCountdownHeroTableCount - m_stData.m_iFreeCountdownHeroTableCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFreeCountdownHeroTableCount(RealAdd);
        m_stData.m_iFreeCountdownHeroTableCount = iFreeCountdownHeroTableCount;
    }
    const int& GetCostEnergy() const
    {
        return m_stData.m_iCostEnergy;
    }
    void AddCostEnergy(int iCostEnergy)
    {
        if (0 == iCostEnergy)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCostEnergy(iCostEnergy);
        m_stData.m_iCostEnergy += iCostEnergy;
    }
    void SetCostEnergy(int iCostEnergy)
    {
        if (m_stData.m_iCostEnergy == iCostEnergy)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCostEnergy - m_stData.m_iCostEnergy;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCostEnergy(RealAdd);
        m_stData.m_iCostEnergy = iCostEnergy;
    }
    const int& GetLeftTurnCardCount() const
    {
        return m_stData.m_iLeftTurnCardCount;
    }
    void AddLeftTurnCardCount(int iLeftTurnCardCount)
    {
        if (0 == iLeftTurnCardCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLeftTurnCardCount(iLeftTurnCardCount);
        m_stData.m_iLeftTurnCardCount += iLeftTurnCardCount;
    }
    void SetLeftTurnCardCount(int iLeftTurnCardCount)
    {
        if (m_stData.m_iLeftTurnCardCount == iLeftTurnCardCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLeftTurnCardCount - m_stData.m_iLeftTurnCardCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLeftTurnCardCount(RealAdd);
        m_stData.m_iLeftTurnCardCount = iLeftTurnCardCount;
    }
    const uint64_t& GetTurnCardBitmap() const
    {
        return m_stData.m_ulTurnCardBitmap;
    }
    void AddTurnCardBitmap(uint64_t ulTurnCardBitmap)
    {
        if (0 == ulTurnCardBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTurnCardBitmap(ulTurnCardBitmap);
        m_stData.m_ulTurnCardBitmap += ulTurnCardBitmap;
    }
    void SetTurnCardBitmap(uint64_t ulTurnCardBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulTurnCardBitmap > ulTurnCardBitmap)
        {
            m_pRecord->SetTurnCardBitmap(m_stData.m_ulTurnCardBitmap - ulTurnCardBitmap, '-');
        }
        else
        {
            m_pRecord->SetTurnCardBitmap(ulTurnCardBitmap - m_stData.m_ulTurnCardBitmap, '+');
        }
        m_stData.m_ulTurnCardBitmap = ulTurnCardBitmap;
    }
    const uint64_t& GetTurnCardAwardBitmap() const
    {
        return m_stData.m_ulTurnCardAwardBitmap;
    }
    void AddTurnCardAwardBitmap(uint64_t ulTurnCardAwardBitmap)
    {
        if (0 == ulTurnCardAwardBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTurnCardAwardBitmap(ulTurnCardAwardBitmap);
        m_stData.m_ulTurnCardAwardBitmap += ulTurnCardAwardBitmap;
    }
    void SetTurnCardAwardBitmap(uint64_t ulTurnCardAwardBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulTurnCardAwardBitmap > ulTurnCardAwardBitmap)
        {
            m_pRecord->SetTurnCardAwardBitmap(m_stData.m_ulTurnCardAwardBitmap - ulTurnCardAwardBitmap, '-');
        }
        else
        {
            m_pRecord->SetTurnCardAwardBitmap(ulTurnCardAwardBitmap - m_stData.m_ulTurnCardAwardBitmap, '+');
        }
        m_stData.m_ulTurnCardAwardBitmap = ulTurnCardAwardBitmap;
    }
    const uint64_t& GetVipDailyAwardBitmap() const
    {
        return m_stData.m_ulVipDailyAwardBitmap;
    }
    void AddVipDailyAwardBitmap(uint64_t ulVipDailyAwardBitmap)
    {
        if (0 == ulVipDailyAwardBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetVipDailyAwardBitmap(ulVipDailyAwardBitmap);
        m_stData.m_ulVipDailyAwardBitmap += ulVipDailyAwardBitmap;
    }
    void SetVipDailyAwardBitmap(uint64_t ulVipDailyAwardBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulVipDailyAwardBitmap > ulVipDailyAwardBitmap)
        {
            m_pRecord->SetVipDailyAwardBitmap(m_stData.m_ulVipDailyAwardBitmap - ulVipDailyAwardBitmap, '-');
        }
        else
        {
            m_pRecord->SetVipDailyAwardBitmap(ulVipDailyAwardBitmap - m_stData.m_ulVipDailyAwardBitmap, '+');
        }
        m_stData.m_ulVipDailyAwardBitmap = ulVipDailyAwardBitmap;
    }
    const int& GetWorshipWarGodCount() const
    {
        return m_stData.m_iWorshipWarGodCount;
    }
    void AddWorshipWarGodCount(int iWorshipWarGodCount)
    {
        if (0 == iWorshipWarGodCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWorshipWarGodCount(iWorshipWarGodCount);
        m_stData.m_iWorshipWarGodCount += iWorshipWarGodCount;
    }
    void SetWorshipWarGodCount(int iWorshipWarGodCount)
    {
        if (m_stData.m_iWorshipWarGodCount == iWorshipWarGodCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWorshipWarGodCount - m_stData.m_iWorshipWarGodCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWorshipWarGodCount(RealAdd);
        m_stData.m_iWorshipWarGodCount = iWorshipWarGodCount;
    }
    const int& GetEntireWarWorshippedCount() const
    {
        return m_stData.m_iEntireWarWorshippedCount;
    }
    void AddEntireWarWorshippedCount(int iEntireWarWorshippedCount)
    {
        if (0 == iEntireWarWorshippedCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarWorshippedCount(iEntireWarWorshippedCount);
        m_stData.m_iEntireWarWorshippedCount += iEntireWarWorshippedCount;
    }
    void SetEntireWarWorshippedCount(int iEntireWarWorshippedCount)
    {
        if (m_stData.m_iEntireWarWorshippedCount == iEntireWarWorshippedCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEntireWarWorshippedCount - m_stData.m_iEntireWarWorshippedCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarWorshippedCount(RealAdd);
        m_stData.m_iEntireWarWorshippedCount = iEntireWarWorshippedCount;
    }
    const int& GetEntireWarDailyGameTimes() const
    {
        return m_stData.m_iEntireWarDailyGameTimes;
    }
    void AddEntireWarDailyGameTimes(int iEntireWarDailyGameTimes)
    {
        if (0 == iEntireWarDailyGameTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarDailyGameTimes(iEntireWarDailyGameTimes);
        m_stData.m_iEntireWarDailyGameTimes += iEntireWarDailyGameTimes;
    }
    void SetEntireWarDailyGameTimes(int iEntireWarDailyGameTimes)
    {
        if (m_stData.m_iEntireWarDailyGameTimes == iEntireWarDailyGameTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEntireWarDailyGameTimes - m_stData.m_iEntireWarDailyGameTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarDailyGameTimes(RealAdd);
        m_stData.m_iEntireWarDailyGameTimes = iEntireWarDailyGameTimes;
    }
    const int& GetBuyActionCoinCount() const
    {
        return m_stData.m_iBuyActionCoinCount;
    }
    void AddBuyActionCoinCount(int iBuyActionCoinCount)
    {
        if (0 == iBuyActionCoinCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyActionCoinCount(iBuyActionCoinCount);
        m_stData.m_iBuyActionCoinCount += iBuyActionCoinCount;
    }
    void SetBuyActionCoinCount(int iBuyActionCoinCount)
    {
        if (m_stData.m_iBuyActionCoinCount == iBuyActionCoinCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyActionCoinCount - m_stData.m_iBuyActionCoinCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyActionCoinCount(RealAdd);
        m_stData.m_iBuyActionCoinCount = iBuyActionCoinCount;
    }
    const int& GetJubaoCount() const
    {
        return m_stData.m_iJubaoCount;
    }
    void AddJubaoCount(int iJubaoCount)
    {
        if (0 == iJubaoCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetJubaoCount(iJubaoCount);
        m_stData.m_iJubaoCount += iJubaoCount;
    }
    void SetJubaoCount(int iJubaoCount)
    {
        if (m_stData.m_iJubaoCount == iJubaoCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iJubaoCount - m_stData.m_iJubaoCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetJubaoCount(RealAdd);
        m_stData.m_iJubaoCount = iJubaoCount;
    }
    const int& GetMergeLevelCount() const
    {
        return m_stData.m_iMergeLevelCount;
    }
    void AddMergeLevelCount(int iMergeLevelCount)
    {
        if (0 == iMergeLevelCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMergeLevelCount(iMergeLevelCount);
        m_stData.m_iMergeLevelCount += iMergeLevelCount;
    }
    void SetMergeLevelCount(int iMergeLevelCount)
    {
        if (m_stData.m_iMergeLevelCount == iMergeLevelCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMergeLevelCount - m_stData.m_iMergeLevelCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMergeLevelCount(RealAdd);
        m_stData.m_iMergeLevelCount = iMergeLevelCount;
    }
    const int& GetLuckyHammerLogCount() const
    {
        return m_stData.m_iLuckyHammerLogCount;
    }
    void AddLuckyHammerLogCount(int iLuckyHammerLogCount)
    {
        if (0 == iLuckyHammerLogCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLuckyHammerLogCount(iLuckyHammerLogCount);
        m_stData.m_iLuckyHammerLogCount += iLuckyHammerLogCount;
    }
    void SetLuckyHammerLogCount(int iLuckyHammerLogCount)
    {
        if (m_stData.m_iLuckyHammerLogCount == iLuckyHammerLogCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLuckyHammerLogCount - m_stData.m_iLuckyHammerLogCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLuckyHammerLogCount(RealAdd);
        m_stData.m_iLuckyHammerLogCount = iLuckyHammerLogCount;
    }
    const int& GetQuarterVipAwardLevel() const
    {
        return m_stData.m_iQuarterVipAwardLevel;
    }
    void AddQuarterVipAwardLevel(int iQuarterVipAwardLevel)
    {
        if (0 == iQuarterVipAwardLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQuarterVipAwardLevel(iQuarterVipAwardLevel);
        m_stData.m_iQuarterVipAwardLevel += iQuarterVipAwardLevel;
    }
    void SetQuarterVipAwardLevel(int iQuarterVipAwardLevel)
    {
        if (m_stData.m_iQuarterVipAwardLevel == iQuarterVipAwardLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iQuarterVipAwardLevel - m_stData.m_iQuarterVipAwardLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQuarterVipAwardLevel(RealAdd);
        m_stData.m_iQuarterVipAwardLevel = iQuarterVipAwardLevel;
    }
    const uint64_t& GetDailyAwardState() const
    {
        return m_stData.m_ulDailyAwardState;
    }
    void AddDailyAwardState(uint64_t ulDailyAwardState)
    {
        if (0 == ulDailyAwardState)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyAwardState(ulDailyAwardState);
        m_stData.m_ulDailyAwardState += ulDailyAwardState;
    }
    void SetDailyAwardState(uint64_t ulDailyAwardState)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulDailyAwardState > ulDailyAwardState)
        {
            m_pRecord->SetDailyAwardState(m_stData.m_ulDailyAwardState - ulDailyAwardState, '-');
        }
        else
        {
            m_pRecord->SetDailyAwardState(ulDailyAwardState - m_stData.m_ulDailyAwardState, '+');
        }
        m_stData.m_ulDailyAwardState = ulDailyAwardState;
    }
    const int& GetHeartDialogCard() const
    {
        return m_stData.m_iHeartDialogCard;
    }
    void AddHeartDialogCard(int iHeartDialogCard)
    {
        if (0 == iHeartDialogCard)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeartDialogCard(iHeartDialogCard);
        m_stData.m_iHeartDialogCard += iHeartDialogCard;
    }
    void SetHeartDialogCard(int iHeartDialogCard)
    {
        if (m_stData.m_iHeartDialogCard == iHeartDialogCard)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iHeartDialogCard - m_stData.m_iHeartDialogCard;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeartDialogCard(RealAdd);
        m_stData.m_iHeartDialogCard = iHeartDialogCard;
    }
    const std::map<uint8_t, uint8_t>& GetDatingPlaceTimes() const
    {
        return m_stData.m_stDatingPlaceTimes;
    }
    void SetDatingPlaceTimes(const std::map<uint8_t, uint8_t>& stDatingPlaceTimes)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDatingPlaceTimes(stDatingPlaceTimes);
        m_stData.m_stDatingPlaceTimes = stDatingPlaceTimes;
    }
    const std::map<uint8_t, uint8_t>& GetSimpleGameTimes() const
    {
        return m_stData.m_stSimpleGameTimes;
    }
    void SetSimpleGameTimes(const std::map<uint8_t, uint8_t>& stSimpleGameTimes)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSimpleGameTimes(stSimpleGameTimes);
        m_stData.m_stSimpleGameTimes = stSimpleGameTimes;
    }
    const std::vector<uint8_t>& GetFlyArrayReward() const
    {
        return m_stData.m_stFlyArrayReward;
    }
    void SetFlyArrayReward(const std::vector<uint8_t>& stFlyArrayReward)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFlyArrayReward(stFlyArrayReward);
        m_stData.m_stFlyArrayReward = stFlyArrayReward;
    }
    const uint64_t& GetFlyArrayRewardState() const
    {
        return m_stData.m_ulFlyArrayRewardState;
    }
    void AddFlyArrayRewardState(uint64_t ulFlyArrayRewardState)
    {
        if (0 == ulFlyArrayRewardState)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFlyArrayRewardState(ulFlyArrayRewardState);
        m_stData.m_ulFlyArrayRewardState += ulFlyArrayRewardState;
    }
    void SetFlyArrayRewardState(uint64_t ulFlyArrayRewardState)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulFlyArrayRewardState > ulFlyArrayRewardState)
        {
            m_pRecord->SetFlyArrayRewardState(m_stData.m_ulFlyArrayRewardState - ulFlyArrayRewardState, '-');
        }
        else
        {
            m_pRecord->SetFlyArrayRewardState(ulFlyArrayRewardState - m_stData.m_ulFlyArrayRewardState, '+');
        }
        m_stData.m_ulFlyArrayRewardState = ulFlyArrayRewardState;
    }
    const std::vector<CPackGameItem>& GetRedBagEarnings() const
    {
        return m_stData.m_stRedBagEarnings;
    }
    void SetRedBagEarnings(const std::vector<CPackGameItem>& stRedBagEarnings)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRedBagEarnings(stRedBagEarnings);
        m_stData.m_stRedBagEarnings = stRedBagEarnings;
    }
    const char& GetLimitBossDailyNum() const
    {
        return m_stData.m_cLimitBossDailyNum;
    }
    void AddLimitBossDailyNum(char cLimitBossDailyNum)
    {
        if (0 == cLimitBossDailyNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBossDailyNum(cLimitBossDailyNum);
        m_stData.m_cLimitBossDailyNum += cLimitBossDailyNum;
    }
    void SetLimitBossDailyNum(char cLimitBossDailyNum)
    {
        if (m_stData.m_cLimitBossDailyNum == cLimitBossDailyNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cLimitBossDailyNum - m_stData.m_cLimitBossDailyNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBossDailyNum(RealAdd);
        m_stData.m_cLimitBossDailyNum = cLimitBossDailyNum;
    }
    const char& GetLimitBoxDailyNum() const
    {
        return m_stData.m_cLimitBoxDailyNum;
    }
    void AddLimitBoxDailyNum(char cLimitBoxDailyNum)
    {
        if (0 == cLimitBoxDailyNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBoxDailyNum(cLimitBoxDailyNum);
        m_stData.m_cLimitBoxDailyNum += cLimitBoxDailyNum;
    }
    void SetLimitBoxDailyNum(char cLimitBoxDailyNum)
    {
        if (m_stData.m_cLimitBoxDailyNum == cLimitBoxDailyNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cLimitBoxDailyNum - m_stData.m_cLimitBoxDailyNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBoxDailyNum(RealAdd);
        m_stData.m_cLimitBoxDailyNum = cLimitBoxDailyNum;
    }
    const int& GetLimitBossAction() const
    {
        return m_stData.m_iLimitBossAction;
    }
    void AddLimitBossAction(int iLimitBossAction)
    {
        if (0 == iLimitBossAction)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBossAction(iLimitBossAction);
        m_stData.m_iLimitBossAction += iLimitBossAction;
    }
    void SetLimitBossAction(int iLimitBossAction)
    {
        if (m_stData.m_iLimitBossAction == iLimitBossAction)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLimitBossAction - m_stData.m_iLimitBossAction;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBossAction(RealAdd);
        m_stData.m_iLimitBossAction = iLimitBossAction;
    }
    const int& GetFishingTimes() const
    {
        return m_stData.m_iFishingTimes;
    }
    void AddFishingTimes(int iFishingTimes)
    {
        if (0 == iFishingTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFishingTimes(iFishingTimes);
        m_stData.m_iFishingTimes += iFishingTimes;
    }
    void SetFishingTimes(int iFishingTimes)
    {
        if (m_stData.m_iFishingTimes == iFishingTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFishingTimes - m_stData.m_iFishingTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFishingTimes(RealAdd);
        m_stData.m_iFishingTimes = iFishingTimes;
    }
    const int& GetWishSwapTimes() const
    {
        return m_stData.m_iWishSwapTimes;
    }
    void AddWishSwapTimes(int iWishSwapTimes)
    {
        if (0 == iWishSwapTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWishSwapTimes(iWishSwapTimes);
        m_stData.m_iWishSwapTimes += iWishSwapTimes;
    }
    void SetWishSwapTimes(int iWishSwapTimes)
    {
        if (m_stData.m_iWishSwapTimes == iWishSwapTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWishSwapTimes - m_stData.m_iWishSwapTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWishSwapTimes(RealAdd);
        m_stData.m_iWishSwapTimes = iWishSwapTimes;
    }

    CRecordUserDailyTable* m_pRecord;
    TUserDailyTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserDailyTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserDailyTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserDailyTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
