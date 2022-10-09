#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserInfoTable;
struct TUserInfoTableValueType;

class CRecordUserInfoTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserInfoTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserInfoTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();

    static std::vector<TUserInfoTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strAge;
    static const char* s_strVipLevel;
    static const char* s_strGuideMajorStep;
    static const char* s_strGuideMinorStep;
    static const char* s_strGuideSpecialStep;
    static const char* s_strPrimaryKeyIndex;
    static const char* s_strNick;
    static const char* s_strBanType;
    static const char* s_strBanBeginTime;
    static const char* s_strBanEndTime;
    static const char* s_strBanComment;
    static const char* s_strJubaoCount;
    static const char* s_strJubaoedCount;
    static const char* s_strGuildID;
    static const char* s_strGuildName;
    static const char* s_strChangeNickCount;
    static const char* s_strAvatarID;
    static const char* s_strAvatarBorder;
    static const char* s_strLevel;
    static const char* s_strExp;
    static const char* s_strVipExp;
    static const char* s_strAddUpRmb;
    static const char* s_strMonthAddUpRmb;
    static const char* s_strFirstDepositBonus;
    static const char* s_strLastLogin;
    static const char* s_strLastLogout;
    static const char* s_strAddUpLoginDay;
    static const char* s_strCreateTime;
    static const char* s_strFightLevelID;
    static const char* s_strGlobalMailTimePos;
    static const char* s_strNpcMailDay;
    static const char* s_strNpcMailServer;
    static const char* s_strNpcMailDatetime;
    static const char* s_strNpcMailLevel;
    static const char* s_strNpcMailCardtime;
    static const char* s_strThemeSkin;
    static const char* s_strRuneCard;
    static const char* s_strLastSignTime;
    static const char* s_strTotalSign;
    static const char* s_strActiveDoubleDepositCount;
    static const char* s_strRefreshDoubleDepositTime;
    static const char* s_strRmb;
    static const char* s_strPayRmb;
    static const char* s_strGold;
    static const char* s_strEvilBrand;
    static const char* s_strCharm;
    static const char* s_strPhyPower;
    static const char* s_strPhyStore;
    static const char* s_strLastAutoAddTime;
    static const char* s_strVitality;
    static const char* s_strAddVitalityTime;
    static const char* s_strLastMergeRefreshTime;
    static const char* s_strRoleCombat;
    static const char* s_strRoleCombatMax;
    static const char* s_strDailyDeposit;
    static const char* s_strDailyDepositBonus;
    static const char* s_strLastDepositTime;
    static const char* s_strScoreRankBonusTime;
    static const char* s_strCombatRankBonusTime;
    static const char* s_strDailyTaskChangeTime;
    static const char* s_strFightPowerHighestRank;
    static const char* s_strBuyGoldTimes;
    static const char* s_strBuyGoldTotalTimes;
    static const char* s_strBuyEnergyTimes;
    static const char* s_strLastBuyEnergyTime;
    static const char* s_strBuyVitalityTimes;
    static const char* s_strLastBuyVitalityTime;
    static const char* s_strTotalOnlineTime;
    static const char* s_strRefreshPvpTimes;
    static const char* s_strLastRefreshPvpTime;
    static const char* s_strCrossFightResetTime;
    static const char* s_strCrossFightResetTimes;
    static const char* s_strCrossFightRefreshResetTimesTimes;
    static const char* s_strCrossFightLastRefreshResetTimesTime;
    static const char* s_strPvpHighestRank;
    static const char* s_strPvpFightTimes;
    static const char* s_strPvpVictoryTimes;
    static const char* s_strMatchScore;
    static const char* s_strMatchMaxScore;
    static const char* s_strMatchWinCount;
    static const char* s_strMatchLoseCount;
    static const char* s_strMatchMaxLevelAwardBitmap;
    static const char* s_strMatchDailyCount;
    static const char* s_strMatchStoreBuyCount;
    static const char* s_strFairPvpWinCount;
    static const char* s_strFairPvpLoseCount;
    static const char* s_strFairPvpStoreCard;
    static const char* s_strFairPvpTeamCard;
    static const char* s_strNormalChapterBonus1;
    static const char* s_strNormalChapterBonus2;
    static const char* s_strNormalChapterBonus3;
    static const char* s_strEliteChapterBonus1;
    static const char* s_strEliteChapterBonus2;
    static const char* s_strEliteChapterBonus3;
    static const char* s_strVipLevelBuyBonus;
    static const char* s_strRecommendedLineupBitmap;
    static const char* s_strFightPowerRankActive;
    static const char* s_strClimbTowerCurrentOrder;
    static const char* s_strClimbTowerMaxOrder;
    static const char* s_strClimbTowerResetCount;
    static const char* s_strLastClimbTowerResetTime;
    static const char* s_strClimbTowerSweepEndTime;
    static const char* s_strClimbTowerHighestRank;
    static const char* s_strSerialPayAwarded;
    static const char* s_strCornucopiaProfitYuanbao;
    static const char* s_strGuildCoin;
    static const char* s_strLastLeaveGuildTime;
    static const char* s_strJubaoAwardCount;
    static const char* s_strTodayJubaoAwardCount;
    static const char* s_strLastJubaoAwardTime;
    static const char* s_strWeiBuyNum;
    static const char* s_strWeiAvgPrice;
    static const char* s_strShuBuyNum;
    static const char* s_strShuAvgPrice;
    static const char* s_strWuBuyNum;
    static const char* s_strWuAvgPrice;
    static const char* s_strEntireWarContribution;
    static const char* s_strEntireWarMilitarkLevel;
    static const char* s_strEntireWarWorshippedCount;
    static const char* s_strLastEntireWarTime;
    static const char* s_strWeekWarContribution;
    static const char* s_strWeekLimitBossScore;
    static const char* s_strActionCoin;
    static const char* s_strLastAddActionTime;
    static const char* s_strEntireWarAwardBitmap;
    static const char* s_strEntireWarNodeAwardStat;
    static const char* s_strEntireWarExtraStatus;
    static const char* s_strEntireWarNodePara;
    static const char* s_strDayQiyuID;
    static const char* s_strCangbaotuQiyuID;
    static const char* s_strQiyuEndtime;
    static const char* s_strCangbaotuEndtime;
    static const char* s_strQiyuBuff;
    static const char* s_strCangbaotuBuff;
    static const char* s_strCangbaotuSuipian;
    static const char* s_strBanquetAllTeamID;
    static const char* s_strDataUpdateVersion;
    static const char* s_strNewPassTotalExp;
    static const char* s_strNewPassNormalLevel;
    static const char* s_strNewPassAdvancedLevel;
    static const char* s_strLimitBossTotalPoint;
    static const char* s_strFindLimitBoss;
    static const char* s_strPvpScore;
    static const char* s_strPvpHighestScore;
    static const char* s_strPvpRefreshCount;
    static const char* s_strPvpWeeklyTask;
    static const char* s_strHomeExp;
    static const char* s_strHomeExpReward;
    static const char* s_strMonthAgeLimit;
    static const char* s_strLastMonthAgeChange;
    static const char* s_strLastLimitCommodityID;
    static const char* s_strHotSpringRewardEmail;
    static const char* s_strHotSpringRewardOther;
    static const char* s_strHotSpringReward;
    static const char* s_strHotSpringInvite;
    static const char* s_strHotSpringJoin;
    static const char* s_strIsBindEmail;
    static const char* s_strDailyGuildDonate;
    static const char* s_strGuildPersonDonate;
    static const char* s_strGuildExtraLevel;
    static const char* s_strDailySignFlag;
    static const char* s_strDailySignReward;
    static const char* s_strCompleteSignReward;
    static const char* s_strZhuhaiScore;
    static const char* s_strUid;
    static const char* s_strGroupID;

    TUserInfoTable* m_pCache;

    void SetAge(int iAge)
    {
        m_stIntData[s_strAge] += iAge;
    }
    void SetVipLevel(char cVipLevel)
    {
        m_stCharData[s_strVipLevel] += cVipLevel;
    }
    void SetGuideMajorStep(char cGuideMajorStep)
    {
        m_stCharData[s_strGuideMajorStep] += cGuideMajorStep;
    }
    void SetGuideMinorStep(char cGuideMinorStep)
    {
        m_stCharData[s_strGuideMinorStep] += cGuideMinorStep;
    }
    void SetGuideSpecialStep(uint64_t ulGuideSpecialStep, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strGuideSpecialStep];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulGuideSpecialStep;
        }
        else
        {
            if (stDataRef.m_ulVal > ulGuideSpecialStep)
            {
                stDataRef.m_ulVal -= ulGuideSpecialStep;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulGuideSpecialStep - stDataRef.m_ulVal;
            }
        }
    }
    void SetPrimaryKeyIndex(int iPrimaryKeyIndex)
    {
        m_stIntData[s_strPrimaryKeyIndex] += iPrimaryKeyIndex;
    }
    void SetNick(const std::string& strNick)
    {
        m_stStringData[s_strNick] = strNick;
    }
    void SetBanType(int iBanType)
    {
        m_stIntData[s_strBanType] += iBanType;
    }
    void SetBanBeginTime(int iBanBeginTime)
    {
        m_stIntData[s_strBanBeginTime] += iBanBeginTime;
    }
    void SetBanEndTime(int iBanEndTime)
    {
        m_stIntData[s_strBanEndTime] += iBanEndTime;
    }
    void SetBanComment(const std::string& strBanComment)
    {
        m_stStringData[s_strBanComment] = strBanComment;
    }
    void SetJubaoCount(int iJubaoCount)
    {
        m_stIntData[s_strJubaoCount] += iJubaoCount;
    }
    void SetJubaoedCount(int iJubaoedCount)
    {
        m_stIntData[s_strJubaoedCount] += iJubaoedCount;
    }
    void SetGuildID(int iGuildID)
    {
        m_stIntData[s_strGuildID] += iGuildID;
    }
    void SetGuildName(const std::string& strGuildName)
    {
        m_stStringData[s_strGuildName] = strGuildName;
    }
    void SetChangeNickCount(int iChangeNickCount)
    {
        m_stIntData[s_strChangeNickCount] += iChangeNickCount;
    }
    void SetAvatarID(int iAvatarID)
    {
        m_stIntData[s_strAvatarID] += iAvatarID;
    }
    void SetAvatarBorder(int iAvatarBorder)
    {
        m_stIntData[s_strAvatarBorder] += iAvatarBorder;
    }
    void SetLevel(int iLevel)
    {
        m_stIntData[s_strLevel] += iLevel;
    }
    void SetExp(int iExp)
    {
        m_stIntData[s_strExp] += iExp;
    }
    void SetVipExp(int iVipExp)
    {
        m_stIntData[s_strVipExp] += iVipExp;
    }
    void SetAddUpRmb(int iAddUpRmb)
    {
        m_stIntData[s_strAddUpRmb] += iAddUpRmb;
    }
    void SetMonthAddUpRmb(int iMonthAddUpRmb)
    {
        m_stIntData[s_strMonthAddUpRmb] += iMonthAddUpRmb;
    }
    void SetFirstDepositBonus(int iFirstDepositBonus)
    {
        m_stIntData[s_strFirstDepositBonus] += iFirstDepositBonus;
    }
    void SetLastLogin(int iLastLogin)
    {
        m_stIntData[s_strLastLogin] += iLastLogin;
    }
    void SetLastLogout(int iLastLogout)
    {
        m_stIntData[s_strLastLogout] += iLastLogout;
    }
    void SetAddUpLoginDay(int iAddUpLoginDay)
    {
        m_stIntData[s_strAddUpLoginDay] += iAddUpLoginDay;
    }
    void SetCreateTime(int iCreateTime)
    {
        m_stIntData[s_strCreateTime] += iCreateTime;
    }
    void SetFightLevelID(int iFightLevelID)
    {
        m_stIntData[s_strFightLevelID] += iFightLevelID;
    }
    void SetGlobalMailTimePos(int iGlobalMailTimePos)
    {
        m_stIntData[s_strGlobalMailTimePos] += iGlobalMailTimePos;
    }
    void SetNpcMailDay(int iNpcMailDay)
    {
        m_stIntData[s_strNpcMailDay] += iNpcMailDay;
    }
    void SetNpcMailServer(int iNpcMailServer)
    {
        m_stIntData[s_strNpcMailServer] += iNpcMailServer;
    }
    void SetNpcMailDatetime(int iNpcMailDatetime)
    {
        m_stIntData[s_strNpcMailDatetime] += iNpcMailDatetime;
    }
    void SetNpcMailLevel(int iNpcMailLevel)
    {
        m_stIntData[s_strNpcMailLevel] += iNpcMailLevel;
    }
    void SetNpcMailCardtime(int iNpcMailCardtime)
    {
        m_stIntData[s_strNpcMailCardtime] += iNpcMailCardtime;
    }
    void SetThemeSkin(int iThemeSkin)
    {
        m_stIntData[s_strThemeSkin] += iThemeSkin;
    }
    void SetRuneCard(int iRuneCard)
    {
        m_stIntData[s_strRuneCard] += iRuneCard;
    }
    void SetLastSignTime(int iLastSignTime)
    {
        m_stIntData[s_strLastSignTime] += iLastSignTime;
    }
    void SetTotalSign(int iTotalSign)
    {
        m_stIntData[s_strTotalSign] += iTotalSign;
    }
    void SetActiveDoubleDepositCount(int iActiveDoubleDepositCount)
    {
        m_stIntData[s_strActiveDoubleDepositCount] += iActiveDoubleDepositCount;
    }
    void SetRefreshDoubleDepositTime(int iRefreshDoubleDepositTime)
    {
        m_stIntData[s_strRefreshDoubleDepositTime] += iRefreshDoubleDepositTime;
    }
    void SetRmb(int iRmb)
    {
        m_stIntData[s_strRmb] += iRmb;
    }
    void SetPayRmb(int iPayRmb)
    {
        m_stIntData[s_strPayRmb] += iPayRmb;
    }
    void SetGold(int iGold)
    {
        m_stIntData[s_strGold] += iGold;
    }
    void SetEvilBrand(int iEvilBrand)
    {
        m_stIntData[s_strEvilBrand] += iEvilBrand;
    }
    void SetCharm(int iCharm)
    {
        m_stIntData[s_strCharm] += iCharm;
    }
    void SetPhyPower(int iPhyPower)
    {
        m_stIntData[s_strPhyPower] += iPhyPower;
    }
    void SetPhyStore(int iPhyStore)
    {
        m_stIntData[s_strPhyStore] += iPhyStore;
    }
    void SetLastAutoAddTime(int iLastAutoAddTime)
    {
        m_stIntData[s_strLastAutoAddTime] += iLastAutoAddTime;
    }
    void SetVitality(int iVitality)
    {
        m_stIntData[s_strVitality] += iVitality;
    }
    void SetAddVitalityTime(int iAddVitalityTime)
    {
        m_stIntData[s_strAddVitalityTime] += iAddVitalityTime;
    }
    void SetLastMergeRefreshTime(int iLastMergeRefreshTime)
    {
        m_stIntData[s_strLastMergeRefreshTime] += iLastMergeRefreshTime;
    }
    void SetRoleCombat(int iRoleCombat)
    {
        m_stIntData[s_strRoleCombat] += iRoleCombat;
    }
    void SetRoleCombatMax(int iRoleCombatMax)
    {
        m_stIntData[s_strRoleCombatMax] += iRoleCombatMax;
    }
    void SetDailyDeposit(int iDailyDeposit)
    {
        m_stIntData[s_strDailyDeposit] += iDailyDeposit;
    }
    void SetDailyDepositBonus(int iDailyDepositBonus)
    {
        m_stIntData[s_strDailyDepositBonus] += iDailyDepositBonus;
    }
    void SetLastDepositTime(int iLastDepositTime)
    {
        m_stIntData[s_strLastDepositTime] += iLastDepositTime;
    }
    void SetScoreRankBonusTime(int iScoreRankBonusTime)
    {
        m_stIntData[s_strScoreRankBonusTime] += iScoreRankBonusTime;
    }
    void SetCombatRankBonusTime(int iCombatRankBonusTime)
    {
        m_stIntData[s_strCombatRankBonusTime] += iCombatRankBonusTime;
    }
    void SetDailyTaskChangeTime(int iDailyTaskChangeTime)
    {
        m_stIntData[s_strDailyTaskChangeTime] += iDailyTaskChangeTime;
    }
    void SetFightPowerHighestRank(int iFightPowerHighestRank)
    {
        m_stIntData[s_strFightPowerHighestRank] += iFightPowerHighestRank;
    }
    void SetBuyGoldTimes(int iBuyGoldTimes)
    {
        m_stIntData[s_strBuyGoldTimes] += iBuyGoldTimes;
    }
    void SetBuyGoldTotalTimes(int iBuyGoldTotalTimes)
    {
        m_stIntData[s_strBuyGoldTotalTimes] += iBuyGoldTotalTimes;
    }
    void SetBuyEnergyTimes(int iBuyEnergyTimes)
    {
        m_stIntData[s_strBuyEnergyTimes] += iBuyEnergyTimes;
    }
    void SetLastBuyEnergyTime(int iLastBuyEnergyTime)
    {
        m_stIntData[s_strLastBuyEnergyTime] += iLastBuyEnergyTime;
    }
    void SetBuyVitalityTimes(int iBuyVitalityTimes)
    {
        m_stIntData[s_strBuyVitalityTimes] += iBuyVitalityTimes;
    }
    void SetLastBuyVitalityTime(int iLastBuyVitalityTime)
    {
        m_stIntData[s_strLastBuyVitalityTime] += iLastBuyVitalityTime;
    }
    void SetTotalOnlineTime(int iTotalOnlineTime)
    {
        m_stIntData[s_strTotalOnlineTime] += iTotalOnlineTime;
    }
    void SetRefreshPvpTimes(int iRefreshPvpTimes)
    {
        m_stIntData[s_strRefreshPvpTimes] += iRefreshPvpTimes;
    }
    void SetLastRefreshPvpTime(int iLastRefreshPvpTime)
    {
        m_stIntData[s_strLastRefreshPvpTime] += iLastRefreshPvpTime;
    }
    void SetCrossFightResetTime(int iCrossFightResetTime)
    {
        m_stIntData[s_strCrossFightResetTime] += iCrossFightResetTime;
    }
    void SetCrossFightResetTimes(int iCrossFightResetTimes)
    {
        m_stIntData[s_strCrossFightResetTimes] += iCrossFightResetTimes;
    }
    void SetCrossFightRefreshResetTimesTimes(int iCrossFightRefreshResetTimesTimes)
    {
        m_stIntData[s_strCrossFightRefreshResetTimesTimes] += iCrossFightRefreshResetTimesTimes;
    }
    void SetCrossFightLastRefreshResetTimesTime(int iCrossFightLastRefreshResetTimesTime)
    {
        m_stIntData[s_strCrossFightLastRefreshResetTimesTime] += iCrossFightLastRefreshResetTimesTime;
    }
    void SetPvpHighestRank(int iPvpHighestRank)
    {
        m_stIntData[s_strPvpHighestRank] += iPvpHighestRank;
    }
    void SetPvpFightTimes(int iPvpFightTimes)
    {
        m_stIntData[s_strPvpFightTimes] += iPvpFightTimes;
    }
    void SetPvpVictoryTimes(int iPvpVictoryTimes)
    {
        m_stIntData[s_strPvpVictoryTimes] += iPvpVictoryTimes;
    }
    void SetMatchScore(int iMatchScore)
    {
        m_stIntData[s_strMatchScore] += iMatchScore;
    }
    void SetMatchMaxScore(int iMatchMaxScore)
    {
        m_stIntData[s_strMatchMaxScore] += iMatchMaxScore;
    }
    void SetMatchWinCount(int iMatchWinCount)
    {
        m_stIntData[s_strMatchWinCount] += iMatchWinCount;
    }
    void SetMatchLoseCount(int iMatchLoseCount)
    {
        m_stIntData[s_strMatchLoseCount] += iMatchLoseCount;
    }
    void SetMatchMaxLevelAwardBitmap(const std::string& strMatchMaxLevelAwardBitmap)
    {
        m_stStringData[s_strMatchMaxLevelAwardBitmap] = strMatchMaxLevelAwardBitmap;
    }
    void SetMatchDailyCount(int iMatchDailyCount)
    {
        m_stIntData[s_strMatchDailyCount] += iMatchDailyCount;
    }
    void SetMatchStoreBuyCount(const CPackMatchStoreBuyCount& stMatchStoreBuyCount)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stMatchStoreBuyCount);
        m_stVarData[s_strMatchStoreBuyCount] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetFairPvpWinCount(int iFairPvpWinCount)
    {
        m_stIntData[s_strFairPvpWinCount] += iFairPvpWinCount;
    }
    void SetFairPvpLoseCount(int iFairPvpLoseCount)
    {
        m_stIntData[s_strFairPvpLoseCount] += iFairPvpLoseCount;
    }
    void SetFairPvpStoreCard(const std::vector<int32_t>& stFairPvpStoreCard)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stFairPvpStoreCard);
        m_stVarData[s_strFairPvpStoreCard] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetFairPvpTeamCard(const std::vector<int32_t>& stFairPvpTeamCard)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stFairPvpTeamCard);
        m_stVarData[s_strFairPvpTeamCard] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetNormalChapterBonus1(uint64_t ulNormalChapterBonus1, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strNormalChapterBonus1];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulNormalChapterBonus1;
        }
        else
        {
            if (stDataRef.m_ulVal > ulNormalChapterBonus1)
            {
                stDataRef.m_ulVal -= ulNormalChapterBonus1;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulNormalChapterBonus1 - stDataRef.m_ulVal;
            }
        }
    }
    void SetNormalChapterBonus2(uint64_t ulNormalChapterBonus2, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strNormalChapterBonus2];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulNormalChapterBonus2;
        }
        else
        {
            if (stDataRef.m_ulVal > ulNormalChapterBonus2)
            {
                stDataRef.m_ulVal -= ulNormalChapterBonus2;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulNormalChapterBonus2 - stDataRef.m_ulVal;
            }
        }
    }
    void SetNormalChapterBonus3(uint64_t ulNormalChapterBonus3, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strNormalChapterBonus3];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulNormalChapterBonus3;
        }
        else
        {
            if (stDataRef.m_ulVal > ulNormalChapterBonus3)
            {
                stDataRef.m_ulVal -= ulNormalChapterBonus3;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulNormalChapterBonus3 - stDataRef.m_ulVal;
            }
        }
    }
    void SetEliteChapterBonus1(uint64_t ulEliteChapterBonus1, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strEliteChapterBonus1];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulEliteChapterBonus1;
        }
        else
        {
            if (stDataRef.m_ulVal > ulEliteChapterBonus1)
            {
                stDataRef.m_ulVal -= ulEliteChapterBonus1;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulEliteChapterBonus1 - stDataRef.m_ulVal;
            }
        }
    }
    void SetEliteChapterBonus2(uint64_t ulEliteChapterBonus2, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strEliteChapterBonus2];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulEliteChapterBonus2;
        }
        else
        {
            if (stDataRef.m_ulVal > ulEliteChapterBonus2)
            {
                stDataRef.m_ulVal -= ulEliteChapterBonus2;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulEliteChapterBonus2 - stDataRef.m_ulVal;
            }
        }
    }
    void SetEliteChapterBonus3(uint64_t ulEliteChapterBonus3, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strEliteChapterBonus3];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulEliteChapterBonus3;
        }
        else
        {
            if (stDataRef.m_ulVal > ulEliteChapterBonus3)
            {
                stDataRef.m_ulVal -= ulEliteChapterBonus3;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulEliteChapterBonus3 - stDataRef.m_ulVal;
            }
        }
    }
    void SetVipLevelBuyBonus(uint64_t ulVipLevelBuyBonus, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strVipLevelBuyBonus];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulVipLevelBuyBonus;
        }
        else
        {
            if (stDataRef.m_ulVal > ulVipLevelBuyBonus)
            {
                stDataRef.m_ulVal -= ulVipLevelBuyBonus;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulVipLevelBuyBonus - stDataRef.m_ulVal;
            }
        }
    }
    void SetRecommendedLineupBitmap(uint64_t ulRecommendedLineupBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strRecommendedLineupBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulRecommendedLineupBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulRecommendedLineupBitmap)
            {
                stDataRef.m_ulVal -= ulRecommendedLineupBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulRecommendedLineupBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetFightPowerRankActive(uint64_t ulFightPowerRankActive, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strFightPowerRankActive];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulFightPowerRankActive;
        }
        else
        {
            if (stDataRef.m_ulVal > ulFightPowerRankActive)
            {
                stDataRef.m_ulVal -= ulFightPowerRankActive;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulFightPowerRankActive - stDataRef.m_ulVal;
            }
        }
    }
    void SetClimbTowerCurrentOrder(int iClimbTowerCurrentOrder)
    {
        m_stIntData[s_strClimbTowerCurrentOrder] += iClimbTowerCurrentOrder;
    }
    void SetClimbTowerMaxOrder(int iClimbTowerMaxOrder)
    {
        m_stIntData[s_strClimbTowerMaxOrder] += iClimbTowerMaxOrder;
    }
    void SetClimbTowerResetCount(int iClimbTowerResetCount)
    {
        m_stIntData[s_strClimbTowerResetCount] += iClimbTowerResetCount;
    }
    void SetLastClimbTowerResetTime(int iLastClimbTowerResetTime)
    {
        m_stIntData[s_strLastClimbTowerResetTime] += iLastClimbTowerResetTime;
    }
    void SetClimbTowerSweepEndTime(int iClimbTowerSweepEndTime)
    {
        m_stIntData[s_strClimbTowerSweepEndTime] += iClimbTowerSweepEndTime;
    }
    void SetClimbTowerHighestRank(int iClimbTowerHighestRank)
    {
        m_stIntData[s_strClimbTowerHighestRank] += iClimbTowerHighestRank;
    }
    void SetSerialPayAwarded(int iSerialPayAwarded)
    {
        m_stIntData[s_strSerialPayAwarded] += iSerialPayAwarded;
    }
    void SetCornucopiaProfitYuanbao(int iCornucopiaProfitYuanbao)
    {
        m_stIntData[s_strCornucopiaProfitYuanbao] += iCornucopiaProfitYuanbao;
    }
    void SetGuildCoin(int iGuildCoin)
    {
        m_stIntData[s_strGuildCoin] += iGuildCoin;
    }
    void SetLastLeaveGuildTime(int iLastLeaveGuildTime)
    {
        m_stIntData[s_strLastLeaveGuildTime] += iLastLeaveGuildTime;
    }
    void SetJubaoAwardCount(int iJubaoAwardCount)
    {
        m_stIntData[s_strJubaoAwardCount] += iJubaoAwardCount;
    }
    void SetTodayJubaoAwardCount(int iTodayJubaoAwardCount)
    {
        m_stIntData[s_strTodayJubaoAwardCount] += iTodayJubaoAwardCount;
    }
    void SetLastJubaoAwardTime(int iLastJubaoAwardTime)
    {
        m_stIntData[s_strLastJubaoAwardTime] += iLastJubaoAwardTime;
    }
    void SetWeiBuyNum(int iWeiBuyNum)
    {
        m_stIntData[s_strWeiBuyNum] += iWeiBuyNum;
    }
    void SetWeiAvgPrice(int iWeiAvgPrice)
    {
        m_stIntData[s_strWeiAvgPrice] += iWeiAvgPrice;
    }
    void SetShuBuyNum(int iShuBuyNum)
    {
        m_stIntData[s_strShuBuyNum] += iShuBuyNum;
    }
    void SetShuAvgPrice(int iShuAvgPrice)
    {
        m_stIntData[s_strShuAvgPrice] += iShuAvgPrice;
    }
    void SetWuBuyNum(int iWuBuyNum)
    {
        m_stIntData[s_strWuBuyNum] += iWuBuyNum;
    }
    void SetWuAvgPrice(int iWuAvgPrice)
    {
        m_stIntData[s_strWuAvgPrice] += iWuAvgPrice;
    }
    void SetEntireWarContribution(int iEntireWarContribution)
    {
        m_stIntData[s_strEntireWarContribution] += iEntireWarContribution;
    }
    void SetEntireWarMilitarkLevel(int iEntireWarMilitarkLevel)
    {
        m_stIntData[s_strEntireWarMilitarkLevel] += iEntireWarMilitarkLevel;
    }
    void SetEntireWarWorshippedCount(int iEntireWarWorshippedCount)
    {
        m_stIntData[s_strEntireWarWorshippedCount] += iEntireWarWorshippedCount;
    }
    void SetLastEntireWarTime(int iLastEntireWarTime)
    {
        m_stIntData[s_strLastEntireWarTime] += iLastEntireWarTime;
    }
    void SetWeekWarContribution(int iWeekWarContribution)
    {
        m_stIntData[s_strWeekWarContribution] += iWeekWarContribution;
    }
    void SetWeekLimitBossScore(int iWeekLimitBossScore)
    {
        m_stIntData[s_strWeekLimitBossScore] += iWeekLimitBossScore;
    }
    void SetActionCoin(int iActionCoin)
    {
        m_stIntData[s_strActionCoin] += iActionCoin;
    }
    void SetLastAddActionTime(int iLastAddActionTime)
    {
        m_stIntData[s_strLastAddActionTime] += iLastAddActionTime;
    }
    void SetEntireWarAwardBitmap(uint64_t ulEntireWarAwardBitmap, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strEntireWarAwardBitmap];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulEntireWarAwardBitmap;
        }
        else
        {
            if (stDataRef.m_ulVal > ulEntireWarAwardBitmap)
            {
                stDataRef.m_ulVal -= ulEntireWarAwardBitmap;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulEntireWarAwardBitmap - stDataRef.m_ulVal;
            }
        }
    }
    void SetEntireWarNodeAwardStat(const CVecBitmap& stEntireWarNodeAwardStat)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stEntireWarNodeAwardStat);
        m_stVarData[s_strEntireWarNodeAwardStat] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetEntireWarExtraStatus(const CPackEntireWarExtra& stEntireWarExtraStatus)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stEntireWarExtraStatus);
        m_stVarData[s_strEntireWarExtraStatus] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetEntireWarNodePara(const std::map<int32_t, int32_t>& stEntireWarNodePara)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stEntireWarNodePara);
        m_stVarData[s_strEntireWarNodePara] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetDayQiyuID(int iDayQiyuID)
    {
        m_stIntData[s_strDayQiyuID] += iDayQiyuID;
    }
    void SetCangbaotuQiyuID(int iCangbaotuQiyuID)
    {
        m_stIntData[s_strCangbaotuQiyuID] += iCangbaotuQiyuID;
    }
    void SetQiyuEndtime(int iQiyuEndtime)
    {
        m_stIntData[s_strQiyuEndtime] += iQiyuEndtime;
    }
    void SetCangbaotuEndtime(int iCangbaotuEndtime)
    {
        m_stIntData[s_strCangbaotuEndtime] += iCangbaotuEndtime;
    }
    void SetQiyuBuff(const CPackEntireWarBuffVector& stQiyuBuff)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stQiyuBuff);
        m_stVarData[s_strQiyuBuff] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetCangbaotuBuff(const CPackEntireWarBuffVector& stCangbaotuBuff)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stCangbaotuBuff);
        m_stVarData[s_strCangbaotuBuff] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetCangbaotuSuipian(const CPackCangbaotuSuipian& stCangbaotuSuipian)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stCangbaotuSuipian);
        m_stVarData[s_strCangbaotuSuipian] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetBanquetAllTeamID(const CPackBanquetTeamIds& stBanquetAllTeamID)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stBanquetAllTeamID);
        m_stVarData[s_strBanquetAllTeamID] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetDataUpdateVersion(int iDataUpdateVersion)
    {
        m_stIntData[s_strDataUpdateVersion] += iDataUpdateVersion;
    }
    void SetNewPassTotalExp(int iNewPassTotalExp)
    {
        m_stIntData[s_strNewPassTotalExp] += iNewPassTotalExp;
    }
    void SetNewPassNormalLevel(const std::vector<int32_t>& stNewPassNormalLevel)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stNewPassNormalLevel);
        m_stVarData[s_strNewPassNormalLevel] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetNewPassAdvancedLevel(const std::vector<int32_t>& stNewPassAdvancedLevel)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stNewPassAdvancedLevel);
        m_stVarData[s_strNewPassAdvancedLevel] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetLimitBossTotalPoint(int iLimitBossTotalPoint)
    {
        m_stIntData[s_strLimitBossTotalPoint] += iLimitBossTotalPoint;
    }
    void SetFindLimitBoss(const std::map<int32_t,int32_t>& stFindLimitBoss)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stFindLimitBoss);
        m_stVarData[s_strFindLimitBoss] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPvpScore(int iPvpScore)
    {
        m_stIntData[s_strPvpScore] += iPvpScore;
    }
    void SetPvpHighestScore(int iPvpHighestScore)
    {
        m_stIntData[s_strPvpHighestScore] += iPvpHighestScore;
    }
    void SetPvpRefreshCount(int iPvpRefreshCount)
    {
        m_stIntData[s_strPvpRefreshCount] += iPvpRefreshCount;
    }
    void SetPvpWeeklyTask(const std::vector<int32_t>& stPvpWeeklyTask)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stPvpWeeklyTask);
        m_stVarData[s_strPvpWeeklyTask] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetHomeExp(int iHomeExp)
    {
        m_stIntData[s_strHomeExp] += iHomeExp;
    }
    void SetHomeExpReward(const std::vector<int32_t>& stHomeExpReward)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stHomeExpReward);
        m_stVarData[s_strHomeExpReward] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetMonthAgeLimit(char cMonthAgeLimit)
    {
        m_stCharData[s_strMonthAgeLimit] += cMonthAgeLimit;
    }
    void SetLastMonthAgeChange(int iLastMonthAgeChange)
    {
        m_stIntData[s_strLastMonthAgeChange] += iLastMonthAgeChange;
    }
    void SetLastLimitCommodityID(int iLastLimitCommodityID)
    {
        m_stIntData[s_strLastLimitCommodityID] += iLastLimitCommodityID;
    }
    void SetHotSpringRewardEmail(char cHotSpringRewardEmail)
    {
        m_stCharData[s_strHotSpringRewardEmail] += cHotSpringRewardEmail;
    }
    void SetHotSpringRewardOther(char cHotSpringRewardOther)
    {
        m_stCharData[s_strHotSpringRewardOther] += cHotSpringRewardOther;
    }
    void SetHotSpringReward(char cHotSpringReward)
    {
        m_stCharData[s_strHotSpringReward] += cHotSpringReward;
    }
    void SetHotSpringInvite(char cHotSpringInvite)
    {
        m_stCharData[s_strHotSpringInvite] += cHotSpringInvite;
    }
    void SetHotSpringJoin(char cHotSpringJoin)
    {
        m_stCharData[s_strHotSpringJoin] += cHotSpringJoin;
    }
    void SetIsBindEmail(char cIsBindEmail)
    {
        m_stCharData[s_strIsBindEmail] += cIsBindEmail;
    }
    void SetDailyGuildDonate(char cDailyGuildDonate)
    {
        m_stCharData[s_strDailyGuildDonate] += cDailyGuildDonate;
    }
    void SetGuildPersonDonate(int iGuildPersonDonate)
    {
        m_stIntData[s_strGuildPersonDonate] += iGuildPersonDonate;
    }
    void SetGuildExtraLevel(int iGuildExtraLevel)
    {
        m_stIntData[s_strGuildExtraLevel] += iGuildExtraLevel;
    }
    void SetDailySignFlag(char cDailySignFlag)
    {
        m_stCharData[s_strDailySignFlag] += cDailySignFlag;
    }
    void SetDailySignReward(const CPackGameItem& stDailySignReward)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stDailySignReward);
        m_stVarData[s_strDailySignReward] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetCompleteSignReward(const CPackGameItem& stCompleteSignReward)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stCompleteSignReward);
        m_stVarData[s_strCompleteSignReward] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetZhuhaiScore(int iZhuhaiScore)
    {
        m_stIntData[s_strZhuhaiScore] += iZhuhaiScore;
    }
    void SetPrimaryKey(int iUid, int iGroupID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TUserInfoTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
};

struct TUserInfoTableValueType
{
    TUserInfoTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iAge(0)
    , m_cVipLevel(0)
    , m_cGuideMajorStep(1)
    , m_cGuideMinorStep(0)
    , m_ulGuideSpecialStep(0)
    , m_iPrimaryKeyIndex(1)
    , m_strNick("")
    , m_iBanType(0)
    , m_iBanBeginTime(0)
    , m_iBanEndTime(0)
    , m_strBanComment("")
    , m_iJubaoCount(0)
    , m_iJubaoedCount(0)
    , m_iGuildID(0)
    , m_strGuildName("")
    , m_iChangeNickCount(0)
    , m_iAvatarID(0)
    , m_iAvatarBorder(0)
    , m_iLevel(1)
    , m_iExp(0)
    , m_iVipExp(0)
    , m_iAddUpRmb(0)
    , m_iMonthAddUpRmb(0)
    , m_iFirstDepositBonus(0)
    , m_iLastLogin(0)
    , m_iLastLogout(0)
    , m_iAddUpLoginDay(0)
    , m_iCreateTime(0)
    , m_iFightLevelID(0)
    , m_iGlobalMailTimePos(0)
    , m_iNpcMailDay(0)
    , m_iNpcMailServer(0)
    , m_iNpcMailDatetime(0)
    , m_iNpcMailLevel(0)
    , m_iNpcMailCardtime(0)
    , m_iThemeSkin(0)
    , m_iRuneCard(0)
    , m_iLastSignTime(0)
    , m_iTotalSign(0)
    , m_iActiveDoubleDepositCount(0)
    , m_iRefreshDoubleDepositTime(0)
    , m_iRmb(0)
    , m_iPayRmb(0)
    , m_iGold(0)
    , m_iEvilBrand(0)
    , m_iCharm(0)
    , m_iPhyPower(0)
    , m_iPhyStore(0)
    , m_iLastAutoAddTime(0)
    , m_iVitality(0)
    , m_iAddVitalityTime(0)
    , m_iLastMergeRefreshTime(0)
    , m_iRoleCombat(0)
    , m_iRoleCombatMax(0)
    , m_iDailyDeposit(0)
    , m_iDailyDepositBonus(0)
    , m_iLastDepositTime(0)
    , m_iScoreRankBonusTime(0)
    , m_iCombatRankBonusTime(0)
    , m_iDailyTaskChangeTime(0)
    , m_iFightPowerHighestRank(0)
    , m_iBuyGoldTimes(0)
    , m_iBuyGoldTotalTimes(0)
    , m_iBuyEnergyTimes(0)
    , m_iLastBuyEnergyTime(0)
    , m_iBuyVitalityTimes(0)
    , m_iLastBuyVitalityTime(0)
    , m_iTotalOnlineTime(0)
    , m_iRefreshPvpTimes(0)
    , m_iLastRefreshPvpTime(0)
    , m_iCrossFightResetTime(0)
    , m_iCrossFightResetTimes(0)
    , m_iCrossFightRefreshResetTimesTimes(0)
    , m_iCrossFightLastRefreshResetTimesTime(0)
    , m_iPvpHighestRank(0)
    , m_iPvpFightTimes(0)
    , m_iPvpVictoryTimes(0)
    , m_iMatchScore(0)
    , m_iMatchMaxScore(0)
    , m_iMatchWinCount(0)
    , m_iMatchLoseCount(0)
    , m_strMatchMaxLevelAwardBitmap("")
    , m_iMatchDailyCount(0)
    , m_stMatchStoreBuyCount()
    , m_iFairPvpWinCount(0)
    , m_iFairPvpLoseCount(0)
    , m_stFairPvpStoreCard()
    , m_stFairPvpTeamCard()
    , m_ulNormalChapterBonus1(0)
    , m_ulNormalChapterBonus2(0)
    , m_ulNormalChapterBonus3(0)
    , m_ulEliteChapterBonus1(0)
    , m_ulEliteChapterBonus2(0)
    , m_ulEliteChapterBonus3(0)
    , m_ulVipLevelBuyBonus(0)
    , m_ulRecommendedLineupBitmap(0)
    , m_ulFightPowerRankActive(0)
    , m_iClimbTowerCurrentOrder(0)
    , m_iClimbTowerMaxOrder(0)
    , m_iClimbTowerResetCount(0)
    , m_iLastClimbTowerResetTime(0)
    , m_iClimbTowerSweepEndTime(0)
    , m_iClimbTowerHighestRank(0)
    , m_iSerialPayAwarded(0)
    , m_iCornucopiaProfitYuanbao(0)
    , m_iGuildCoin(0)
    , m_iLastLeaveGuildTime(0)
    , m_iJubaoAwardCount(0)
    , m_iTodayJubaoAwardCount(0)
    , m_iLastJubaoAwardTime(0)
    , m_iWeiBuyNum(0)
    , m_iWeiAvgPrice(0)
    , m_iShuBuyNum(0)
    , m_iShuAvgPrice(0)
    , m_iWuBuyNum(0)
    , m_iWuAvgPrice(0)
    , m_iEntireWarContribution(0)
    , m_iEntireWarMilitarkLevel(0)
    , m_iEntireWarWorshippedCount(0)
    , m_iLastEntireWarTime(0)
    , m_iWeekWarContribution(0)
    , m_iWeekLimitBossScore(0)
    , m_iActionCoin(0)
    , m_iLastAddActionTime(0)
    , m_ulEntireWarAwardBitmap(0)
    , m_stEntireWarNodeAwardStat()
    , m_stEntireWarExtraStatus()
    , m_stEntireWarNodePara()
    , m_iDayQiyuID(0)
    , m_iCangbaotuQiyuID(0)
    , m_iQiyuEndtime(0)
    , m_iCangbaotuEndtime(0)
    , m_stQiyuBuff()
    , m_stCangbaotuBuff()
    , m_stCangbaotuSuipian()
    , m_stBanquetAllTeamID()
    , m_iDataUpdateVersion(0)
    , m_iNewPassTotalExp(0)
    , m_stNewPassNormalLevel()
    , m_stNewPassAdvancedLevel()
    , m_iLimitBossTotalPoint(0)
    , m_stFindLimitBoss()
    , m_iPvpScore(1200)
    , m_iPvpHighestScore(0)
    , m_iPvpRefreshCount(0)
    , m_stPvpWeeklyTask()
    , m_iHomeExp(0)
    , m_stHomeExpReward()
    , m_cMonthAgeLimit(0)
    , m_iLastMonthAgeChange(0)
    , m_iLastLimitCommodityID(0)
    , m_cHotSpringRewardEmail(0)
    , m_cHotSpringRewardOther(0)
    , m_cHotSpringReward(0)
    , m_cHotSpringInvite(0)
    , m_cHotSpringJoin(0)
    , m_cIsBindEmail(0)
    , m_cDailyGuildDonate(0)
    , m_iGuildPersonDonate(0)
    , m_iGuildExtraLevel(0)
    , m_cDailySignFlag(0)
    , m_stDailySignReward()
    , m_stCompleteSignReward()
    , m_iZhuhaiScore(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iAge;
    char                    m_cVipLevel;
    char                    m_cGuideMajorStep;
    char                    m_cGuideMinorStep;
    uint64_t                m_ulGuideSpecialStep;
    int                     m_iPrimaryKeyIndex;
    std::string             m_strNick;
    int                     m_iBanType;
    int                     m_iBanBeginTime;
    int                     m_iBanEndTime;
    std::string             m_strBanComment;
    int                     m_iJubaoCount;
    int                     m_iJubaoedCount;
    int                     m_iGuildID;
    std::string             m_strGuildName;
    int                     m_iChangeNickCount;
    int                     m_iAvatarID;
    int                     m_iAvatarBorder;
    int                     m_iLevel;
    int                     m_iExp;
    int                     m_iVipExp;
    int                     m_iAddUpRmb;
    int                     m_iMonthAddUpRmb;
    int                     m_iFirstDepositBonus;
    int                     m_iLastLogin;
    int                     m_iLastLogout;
    int                     m_iAddUpLoginDay;
    int                     m_iCreateTime;
    int                     m_iFightLevelID;
    int                     m_iGlobalMailTimePos;
    int                     m_iNpcMailDay;
    int                     m_iNpcMailServer;
    int                     m_iNpcMailDatetime;
    int                     m_iNpcMailLevel;
    int                     m_iNpcMailCardtime;
    int                     m_iThemeSkin;
    int                     m_iRuneCard;
    int                     m_iLastSignTime;
    int                     m_iTotalSign;
    int                     m_iActiveDoubleDepositCount;
    int                     m_iRefreshDoubleDepositTime;
    int                     m_iRmb;
    int                     m_iPayRmb;
    int                     m_iGold;
    int                     m_iEvilBrand;
    int                     m_iCharm;
    int                     m_iPhyPower;
    int                     m_iPhyStore;
    int                     m_iLastAutoAddTime;
    int                     m_iVitality;
    int                     m_iAddVitalityTime;
    int                     m_iLastMergeRefreshTime;
    int                     m_iRoleCombat;
    int                     m_iRoleCombatMax;
    int                     m_iDailyDeposit;
    int                     m_iDailyDepositBonus;
    int                     m_iLastDepositTime;
    int                     m_iScoreRankBonusTime;
    int                     m_iCombatRankBonusTime;
    int                     m_iDailyTaskChangeTime;
    int                     m_iFightPowerHighestRank;
    int                     m_iBuyGoldTimes;
    int                     m_iBuyGoldTotalTimes;
    int                     m_iBuyEnergyTimes;
    int                     m_iLastBuyEnergyTime;
    int                     m_iBuyVitalityTimes;
    int                     m_iLastBuyVitalityTime;
    int                     m_iTotalOnlineTime;
    int                     m_iRefreshPvpTimes;
    int                     m_iLastRefreshPvpTime;
    int                     m_iCrossFightResetTime;
    int                     m_iCrossFightResetTimes;
    int                     m_iCrossFightRefreshResetTimesTimes;
    int                     m_iCrossFightLastRefreshResetTimesTime;
    int                     m_iPvpHighestRank;
    int                     m_iPvpFightTimes;
    int                     m_iPvpVictoryTimes;
    int                     m_iMatchScore;
    int                     m_iMatchMaxScore;
    int                     m_iMatchWinCount;
    int                     m_iMatchLoseCount;
    std::string             m_strMatchMaxLevelAwardBitmap;
    int                     m_iMatchDailyCount;
    CPackMatchStoreBuyCount m_stMatchStoreBuyCount;
    int                     m_iFairPvpWinCount;
    int                     m_iFairPvpLoseCount;
    std::vector<int32_t>    m_stFairPvpStoreCard;
    std::vector<int32_t>    m_stFairPvpTeamCard;
    uint64_t                m_ulNormalChapterBonus1;
    uint64_t                m_ulNormalChapterBonus2;
    uint64_t                m_ulNormalChapterBonus3;
    uint64_t                m_ulEliteChapterBonus1;
    uint64_t                m_ulEliteChapterBonus2;
    uint64_t                m_ulEliteChapterBonus3;
    uint64_t                m_ulVipLevelBuyBonus;
    uint64_t                m_ulRecommendedLineupBitmap;
    uint64_t                m_ulFightPowerRankActive;
    int                     m_iClimbTowerCurrentOrder;
    int                     m_iClimbTowerMaxOrder;
    int                     m_iClimbTowerResetCount;
    int                     m_iLastClimbTowerResetTime;
    int                     m_iClimbTowerSweepEndTime;
    int                     m_iClimbTowerHighestRank;
    int                     m_iSerialPayAwarded;
    int                     m_iCornucopiaProfitYuanbao;
    int                     m_iGuildCoin;
    int                     m_iLastLeaveGuildTime;
    int                     m_iJubaoAwardCount;
    int                     m_iTodayJubaoAwardCount;
    int                     m_iLastJubaoAwardTime;
    int                     m_iWeiBuyNum;
    int                     m_iWeiAvgPrice;
    int                     m_iShuBuyNum;
    int                     m_iShuAvgPrice;
    int                     m_iWuBuyNum;
    int                     m_iWuAvgPrice;
    int                     m_iEntireWarContribution;
    int                     m_iEntireWarMilitarkLevel;
    int                     m_iEntireWarWorshippedCount;
    int                     m_iLastEntireWarTime;
    int                     m_iWeekWarContribution;
    int                     m_iWeekLimitBossScore;
    int                     m_iActionCoin;
    int                     m_iLastAddActionTime;
    uint64_t                m_ulEntireWarAwardBitmap;
    CVecBitmap              m_stEntireWarNodeAwardStat;
    CPackEntireWarExtra     m_stEntireWarExtraStatus;
    std::map<int32_t, int32_t>    m_stEntireWarNodePara;
    int                     m_iDayQiyuID;
    int                     m_iCangbaotuQiyuID;
    int                     m_iQiyuEndtime;
    int                     m_iCangbaotuEndtime;
    CPackEntireWarBuffVector    m_stQiyuBuff;
    CPackEntireWarBuffVector    m_stCangbaotuBuff;
    CPackCangbaotuSuipian   m_stCangbaotuSuipian;
    CPackBanquetTeamIds     m_stBanquetAllTeamID;
    int                     m_iDataUpdateVersion;
    int                     m_iNewPassTotalExp;
    std::vector<int32_t>    m_stNewPassNormalLevel;
    std::vector<int32_t>    m_stNewPassAdvancedLevel;
    int                     m_iLimitBossTotalPoint;
    std::map<int32_t,int32_t>    m_stFindLimitBoss;
    int                     m_iPvpScore;
    int                     m_iPvpHighestScore;
    int                     m_iPvpRefreshCount;
    std::vector<int32_t>    m_stPvpWeeklyTask;
    int                     m_iHomeExp;
    std::vector<int32_t>    m_stHomeExpReward;
    char                    m_cMonthAgeLimit;
    int                     m_iLastMonthAgeChange;
    int                     m_iLastLimitCommodityID;
    char                    m_cHotSpringRewardEmail;
    char                    m_cHotSpringRewardOther;
    char                    m_cHotSpringReward;
    char                    m_cHotSpringInvite;
    char                    m_cHotSpringJoin;
    char                    m_cIsBindEmail;
    char                    m_cDailyGuildDonate;
    int                     m_iGuildPersonDonate;
    int                     m_iGuildExtraLevel;
    char                    m_cDailySignFlag;
    CPackGameItem           m_stDailySignReward;
    CPackGameItem           m_stCompleteSignReward;
    int                     m_iZhuhaiScore;
};

struct TUserInfoTable
{
    TUserInfoTable() : m_pRecord(nullptr) { }
    ~TUserInfoTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserInfoTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserInfoTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID);
        m_pRecord->SetAge(stData.m_iAge);
        m_pRecord->SetVipLevel(stData.m_cVipLevel);
        m_pRecord->SetGuideMajorStep(stData.m_cGuideMajorStep);
        m_pRecord->SetGuideMinorStep(stData.m_cGuideMinorStep);
        m_pRecord->SetGuideSpecialStep(stData.m_ulGuideSpecialStep);
        m_pRecord->SetPrimaryKeyIndex(stData.m_iPrimaryKeyIndex);
        m_pRecord->SetNick(stData.m_strNick);
        m_pRecord->SetBanType(stData.m_iBanType);
        m_pRecord->SetBanBeginTime(stData.m_iBanBeginTime);
        m_pRecord->SetBanEndTime(stData.m_iBanEndTime);
        m_pRecord->SetBanComment(stData.m_strBanComment);
        m_pRecord->SetJubaoCount(stData.m_iJubaoCount);
        m_pRecord->SetJubaoedCount(stData.m_iJubaoedCount);
        m_pRecord->SetGuildID(stData.m_iGuildID);
        m_pRecord->SetGuildName(stData.m_strGuildName);
        m_pRecord->SetChangeNickCount(stData.m_iChangeNickCount);
        m_pRecord->SetAvatarID(stData.m_iAvatarID);
        m_pRecord->SetAvatarBorder(stData.m_iAvatarBorder);
        m_pRecord->SetLevel(stData.m_iLevel);
        m_pRecord->SetExp(stData.m_iExp);
        m_pRecord->SetVipExp(stData.m_iVipExp);
        m_pRecord->SetAddUpRmb(stData.m_iAddUpRmb);
        m_pRecord->SetMonthAddUpRmb(stData.m_iMonthAddUpRmb);
        m_pRecord->SetFirstDepositBonus(stData.m_iFirstDepositBonus);
        m_pRecord->SetLastLogin(stData.m_iLastLogin);
        m_pRecord->SetLastLogout(stData.m_iLastLogout);
        m_pRecord->SetAddUpLoginDay(stData.m_iAddUpLoginDay);
        m_pRecord->SetCreateTime(stData.m_iCreateTime);
        m_pRecord->SetFightLevelID(stData.m_iFightLevelID);
        m_pRecord->SetGlobalMailTimePos(stData.m_iGlobalMailTimePos);
        m_pRecord->SetNpcMailDay(stData.m_iNpcMailDay);
        m_pRecord->SetNpcMailServer(stData.m_iNpcMailServer);
        m_pRecord->SetNpcMailDatetime(stData.m_iNpcMailDatetime);
        m_pRecord->SetNpcMailLevel(stData.m_iNpcMailLevel);
        m_pRecord->SetNpcMailCardtime(stData.m_iNpcMailCardtime);
        m_pRecord->SetThemeSkin(stData.m_iThemeSkin);
        m_pRecord->SetRuneCard(stData.m_iRuneCard);
        m_pRecord->SetLastSignTime(stData.m_iLastSignTime);
        m_pRecord->SetTotalSign(stData.m_iTotalSign);
        m_pRecord->SetActiveDoubleDepositCount(stData.m_iActiveDoubleDepositCount);
        m_pRecord->SetRefreshDoubleDepositTime(stData.m_iRefreshDoubleDepositTime);
        m_pRecord->SetRmb(stData.m_iRmb);
        m_pRecord->SetPayRmb(stData.m_iPayRmb);
        m_pRecord->SetGold(stData.m_iGold);
        m_pRecord->SetEvilBrand(stData.m_iEvilBrand);
        m_pRecord->SetCharm(stData.m_iCharm);
        m_pRecord->SetPhyPower(stData.m_iPhyPower);
        m_pRecord->SetPhyStore(stData.m_iPhyStore);
        m_pRecord->SetLastAutoAddTime(stData.m_iLastAutoAddTime);
        m_pRecord->SetVitality(stData.m_iVitality);
        m_pRecord->SetAddVitalityTime(stData.m_iAddVitalityTime);
        m_pRecord->SetLastMergeRefreshTime(stData.m_iLastMergeRefreshTime);
        m_pRecord->SetRoleCombat(stData.m_iRoleCombat);
        m_pRecord->SetRoleCombatMax(stData.m_iRoleCombatMax);
        m_pRecord->SetDailyDeposit(stData.m_iDailyDeposit);
        m_pRecord->SetDailyDepositBonus(stData.m_iDailyDepositBonus);
        m_pRecord->SetLastDepositTime(stData.m_iLastDepositTime);
        m_pRecord->SetScoreRankBonusTime(stData.m_iScoreRankBonusTime);
        m_pRecord->SetCombatRankBonusTime(stData.m_iCombatRankBonusTime);
        m_pRecord->SetDailyTaskChangeTime(stData.m_iDailyTaskChangeTime);
        m_pRecord->SetFightPowerHighestRank(stData.m_iFightPowerHighestRank);
        m_pRecord->SetBuyGoldTimes(stData.m_iBuyGoldTimes);
        m_pRecord->SetBuyGoldTotalTimes(stData.m_iBuyGoldTotalTimes);
        m_pRecord->SetBuyEnergyTimes(stData.m_iBuyEnergyTimes);
        m_pRecord->SetLastBuyEnergyTime(stData.m_iLastBuyEnergyTime);
        m_pRecord->SetBuyVitalityTimes(stData.m_iBuyVitalityTimes);
        m_pRecord->SetLastBuyVitalityTime(stData.m_iLastBuyVitalityTime);
        m_pRecord->SetTotalOnlineTime(stData.m_iTotalOnlineTime);
        m_pRecord->SetRefreshPvpTimes(stData.m_iRefreshPvpTimes);
        m_pRecord->SetLastRefreshPvpTime(stData.m_iLastRefreshPvpTime);
        m_pRecord->SetCrossFightResetTime(stData.m_iCrossFightResetTime);
        m_pRecord->SetCrossFightResetTimes(stData.m_iCrossFightResetTimes);
        m_pRecord->SetCrossFightRefreshResetTimesTimes(stData.m_iCrossFightRefreshResetTimesTimes);
        m_pRecord->SetCrossFightLastRefreshResetTimesTime(stData.m_iCrossFightLastRefreshResetTimesTime);
        m_pRecord->SetPvpHighestRank(stData.m_iPvpHighestRank);
        m_pRecord->SetPvpFightTimes(stData.m_iPvpFightTimes);
        m_pRecord->SetPvpVictoryTimes(stData.m_iPvpVictoryTimes);
        m_pRecord->SetMatchScore(stData.m_iMatchScore);
        m_pRecord->SetMatchMaxScore(stData.m_iMatchMaxScore);
        m_pRecord->SetMatchWinCount(stData.m_iMatchWinCount);
        m_pRecord->SetMatchLoseCount(stData.m_iMatchLoseCount);
        m_pRecord->SetMatchMaxLevelAwardBitmap(stData.m_strMatchMaxLevelAwardBitmap);
        m_pRecord->SetMatchDailyCount(stData.m_iMatchDailyCount);
        m_pRecord->SetMatchStoreBuyCount(stData.m_stMatchStoreBuyCount);
        m_pRecord->SetFairPvpWinCount(stData.m_iFairPvpWinCount);
        m_pRecord->SetFairPvpLoseCount(stData.m_iFairPvpLoseCount);
        m_pRecord->SetFairPvpStoreCard(stData.m_stFairPvpStoreCard);
        m_pRecord->SetFairPvpTeamCard(stData.m_stFairPvpTeamCard);
        m_pRecord->SetNormalChapterBonus1(stData.m_ulNormalChapterBonus1);
        m_pRecord->SetNormalChapterBonus2(stData.m_ulNormalChapterBonus2);
        m_pRecord->SetNormalChapterBonus3(stData.m_ulNormalChapterBonus3);
        m_pRecord->SetEliteChapterBonus1(stData.m_ulEliteChapterBonus1);
        m_pRecord->SetEliteChapterBonus2(stData.m_ulEliteChapterBonus2);
        m_pRecord->SetEliteChapterBonus3(stData.m_ulEliteChapterBonus3);
        m_pRecord->SetVipLevelBuyBonus(stData.m_ulVipLevelBuyBonus);
        m_pRecord->SetRecommendedLineupBitmap(stData.m_ulRecommendedLineupBitmap);
        m_pRecord->SetFightPowerRankActive(stData.m_ulFightPowerRankActive);
        m_pRecord->SetClimbTowerCurrentOrder(stData.m_iClimbTowerCurrentOrder);
        m_pRecord->SetClimbTowerMaxOrder(stData.m_iClimbTowerMaxOrder);
        m_pRecord->SetClimbTowerResetCount(stData.m_iClimbTowerResetCount);
        m_pRecord->SetLastClimbTowerResetTime(stData.m_iLastClimbTowerResetTime);
        m_pRecord->SetClimbTowerSweepEndTime(stData.m_iClimbTowerSweepEndTime);
        m_pRecord->SetClimbTowerHighestRank(stData.m_iClimbTowerHighestRank);
        m_pRecord->SetSerialPayAwarded(stData.m_iSerialPayAwarded);
        m_pRecord->SetCornucopiaProfitYuanbao(stData.m_iCornucopiaProfitYuanbao);
        m_pRecord->SetGuildCoin(stData.m_iGuildCoin);
        m_pRecord->SetLastLeaveGuildTime(stData.m_iLastLeaveGuildTime);
        m_pRecord->SetJubaoAwardCount(stData.m_iJubaoAwardCount);
        m_pRecord->SetTodayJubaoAwardCount(stData.m_iTodayJubaoAwardCount);
        m_pRecord->SetLastJubaoAwardTime(stData.m_iLastJubaoAwardTime);
        m_pRecord->SetWeiBuyNum(stData.m_iWeiBuyNum);
        m_pRecord->SetWeiAvgPrice(stData.m_iWeiAvgPrice);
        m_pRecord->SetShuBuyNum(stData.m_iShuBuyNum);
        m_pRecord->SetShuAvgPrice(stData.m_iShuAvgPrice);
        m_pRecord->SetWuBuyNum(stData.m_iWuBuyNum);
        m_pRecord->SetWuAvgPrice(stData.m_iWuAvgPrice);
        m_pRecord->SetEntireWarContribution(stData.m_iEntireWarContribution);
        m_pRecord->SetEntireWarMilitarkLevel(stData.m_iEntireWarMilitarkLevel);
        m_pRecord->SetEntireWarWorshippedCount(stData.m_iEntireWarWorshippedCount);
        m_pRecord->SetLastEntireWarTime(stData.m_iLastEntireWarTime);
        m_pRecord->SetWeekWarContribution(stData.m_iWeekWarContribution);
        m_pRecord->SetWeekLimitBossScore(stData.m_iWeekLimitBossScore);
        m_pRecord->SetActionCoin(stData.m_iActionCoin);
        m_pRecord->SetLastAddActionTime(stData.m_iLastAddActionTime);
        m_pRecord->SetEntireWarAwardBitmap(stData.m_ulEntireWarAwardBitmap);
        m_pRecord->SetEntireWarNodeAwardStat(stData.m_stEntireWarNodeAwardStat);
        m_pRecord->SetEntireWarExtraStatus(stData.m_stEntireWarExtraStatus);
        m_pRecord->SetEntireWarNodePara(stData.m_stEntireWarNodePara);
        m_pRecord->SetDayQiyuID(stData.m_iDayQiyuID);
        m_pRecord->SetCangbaotuQiyuID(stData.m_iCangbaotuQiyuID);
        m_pRecord->SetQiyuEndtime(stData.m_iQiyuEndtime);
        m_pRecord->SetCangbaotuEndtime(stData.m_iCangbaotuEndtime);
        m_pRecord->SetQiyuBuff(stData.m_stQiyuBuff);
        m_pRecord->SetCangbaotuBuff(stData.m_stCangbaotuBuff);
        m_pRecord->SetCangbaotuSuipian(stData.m_stCangbaotuSuipian);
        m_pRecord->SetBanquetAllTeamID(stData.m_stBanquetAllTeamID);
        m_pRecord->SetDataUpdateVersion(stData.m_iDataUpdateVersion);
        m_pRecord->SetNewPassTotalExp(stData.m_iNewPassTotalExp);
        m_pRecord->SetNewPassNormalLevel(stData.m_stNewPassNormalLevel);
        m_pRecord->SetNewPassAdvancedLevel(stData.m_stNewPassAdvancedLevel);
        m_pRecord->SetLimitBossTotalPoint(stData.m_iLimitBossTotalPoint);
        m_pRecord->SetFindLimitBoss(stData.m_stFindLimitBoss);
        m_pRecord->SetPvpScore(stData.m_iPvpScore);
        m_pRecord->SetPvpHighestScore(stData.m_iPvpHighestScore);
        m_pRecord->SetPvpRefreshCount(stData.m_iPvpRefreshCount);
        m_pRecord->SetPvpWeeklyTask(stData.m_stPvpWeeklyTask);
        m_pRecord->SetHomeExp(stData.m_iHomeExp);
        m_pRecord->SetHomeExpReward(stData.m_stHomeExpReward);
        m_pRecord->SetMonthAgeLimit(stData.m_cMonthAgeLimit);
        m_pRecord->SetLastMonthAgeChange(stData.m_iLastMonthAgeChange);
        m_pRecord->SetLastLimitCommodityID(stData.m_iLastLimitCommodityID);
        m_pRecord->SetHotSpringRewardEmail(stData.m_cHotSpringRewardEmail);
        m_pRecord->SetHotSpringRewardOther(stData.m_cHotSpringRewardOther);
        m_pRecord->SetHotSpringReward(stData.m_cHotSpringReward);
        m_pRecord->SetHotSpringInvite(stData.m_cHotSpringInvite);
        m_pRecord->SetHotSpringJoin(stData.m_cHotSpringJoin);
        m_pRecord->SetIsBindEmail(stData.m_cIsBindEmail);
        m_pRecord->SetDailyGuildDonate(stData.m_cDailyGuildDonate);
        m_pRecord->SetGuildPersonDonate(stData.m_iGuildPersonDonate);
        m_pRecord->SetGuildExtraLevel(stData.m_iGuildExtraLevel);
        m_pRecord->SetDailySignFlag(stData.m_cDailySignFlag);
        m_pRecord->SetDailySignReward(stData.m_stDailySignReward);
        m_pRecord->SetCompleteSignReward(stData.m_stCompleteSignReward);
        m_pRecord->SetZhuhaiScore(stData.m_iZhuhaiScore);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetAge() const
    {
        return m_stData.m_iAge;
    }
    void AddAge(int iAge)
    {
        if (0 == iAge)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAge(iAge);
        m_stData.m_iAge += iAge;
    }
    void SetAge(int iAge)
    {
        if (m_stData.m_iAge == iAge)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAge - m_stData.m_iAge;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAge(RealAdd);
        m_stData.m_iAge = iAge;
    }
    const char& GetVipLevel() const
    {
        return m_stData.m_cVipLevel;
    }
    void AddVipLevel(char cVipLevel)
    {
        if (0 == cVipLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetVipLevel(cVipLevel);
        m_stData.m_cVipLevel += cVipLevel;
    }
    void SetVipLevel(char cVipLevel)
    {
        if (m_stData.m_cVipLevel == cVipLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cVipLevel - m_stData.m_cVipLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetVipLevel(RealAdd);
        m_stData.m_cVipLevel = cVipLevel;
    }
    const char& GetGuideMajorStep() const
    {
        return m_stData.m_cGuideMajorStep;
    }
    void AddGuideMajorStep(char cGuideMajorStep)
    {
        if (0 == cGuideMajorStep)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuideMajorStep(cGuideMajorStep);
        m_stData.m_cGuideMajorStep += cGuideMajorStep;
    }
    void SetGuideMajorStep(char cGuideMajorStep)
    {
        if (m_stData.m_cGuideMajorStep == cGuideMajorStep)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cGuideMajorStep - m_stData.m_cGuideMajorStep;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuideMajorStep(RealAdd);
        m_stData.m_cGuideMajorStep = cGuideMajorStep;
    }
    const char& GetGuideMinorStep() const
    {
        return m_stData.m_cGuideMinorStep;
    }
    void AddGuideMinorStep(char cGuideMinorStep)
    {
        if (0 == cGuideMinorStep)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuideMinorStep(cGuideMinorStep);
        m_stData.m_cGuideMinorStep += cGuideMinorStep;
    }
    void SetGuideMinorStep(char cGuideMinorStep)
    {
        if (m_stData.m_cGuideMinorStep == cGuideMinorStep)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cGuideMinorStep - m_stData.m_cGuideMinorStep;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuideMinorStep(RealAdd);
        m_stData.m_cGuideMinorStep = cGuideMinorStep;
    }
    const uint64_t& GetGuideSpecialStep() const
    {
        return m_stData.m_ulGuideSpecialStep;
    }
    void AddGuideSpecialStep(uint64_t ulGuideSpecialStep)
    {
        if (0 == ulGuideSpecialStep)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuideSpecialStep(ulGuideSpecialStep);
        m_stData.m_ulGuideSpecialStep += ulGuideSpecialStep;
    }
    void SetGuideSpecialStep(uint64_t ulGuideSpecialStep)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulGuideSpecialStep > ulGuideSpecialStep)
        {
            m_pRecord->SetGuideSpecialStep(m_stData.m_ulGuideSpecialStep - ulGuideSpecialStep, '-');
        }
        else
        {
            m_pRecord->SetGuideSpecialStep(ulGuideSpecialStep - m_stData.m_ulGuideSpecialStep, '+');
        }
        m_stData.m_ulGuideSpecialStep = ulGuideSpecialStep;
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
    const std::string& GetNick() const
    {
        return m_stData.m_strNick;
    }
    void SetNick(const std::string& strNick)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNick(strNick);
        m_stData.m_strNick = strNick;
    }
    const int& GetBanType() const
    {
        return m_stData.m_iBanType;
    }
    void AddBanType(int iBanType)
    {
        if (0 == iBanType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBanType(iBanType);
        m_stData.m_iBanType += iBanType;
    }
    void SetBanType(int iBanType)
    {
        if (m_stData.m_iBanType == iBanType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBanType - m_stData.m_iBanType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBanType(RealAdd);
        m_stData.m_iBanType = iBanType;
    }
    const int& GetBanBeginTime() const
    {
        return m_stData.m_iBanBeginTime;
    }
    void AddBanBeginTime(int iBanBeginTime)
    {
        if (0 == iBanBeginTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBanBeginTime(iBanBeginTime);
        m_stData.m_iBanBeginTime += iBanBeginTime;
    }
    void SetBanBeginTime(int iBanBeginTime)
    {
        if (m_stData.m_iBanBeginTime == iBanBeginTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBanBeginTime - m_stData.m_iBanBeginTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBanBeginTime(RealAdd);
        m_stData.m_iBanBeginTime = iBanBeginTime;
    }
    const int& GetBanEndTime() const
    {
        return m_stData.m_iBanEndTime;
    }
    void AddBanEndTime(int iBanEndTime)
    {
        if (0 == iBanEndTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBanEndTime(iBanEndTime);
        m_stData.m_iBanEndTime += iBanEndTime;
    }
    void SetBanEndTime(int iBanEndTime)
    {
        if (m_stData.m_iBanEndTime == iBanEndTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBanEndTime - m_stData.m_iBanEndTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBanEndTime(RealAdd);
        m_stData.m_iBanEndTime = iBanEndTime;
    }
    const std::string& GetBanComment() const
    {
        return m_stData.m_strBanComment;
    }
    void SetBanComment(const std::string& strBanComment)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBanComment(strBanComment);
        m_stData.m_strBanComment = strBanComment;
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
    const int& GetJubaoedCount() const
    {
        return m_stData.m_iJubaoedCount;
    }
    void AddJubaoedCount(int iJubaoedCount)
    {
        if (0 == iJubaoedCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetJubaoedCount(iJubaoedCount);
        m_stData.m_iJubaoedCount += iJubaoedCount;
    }
    void SetJubaoedCount(int iJubaoedCount)
    {
        if (m_stData.m_iJubaoedCount == iJubaoedCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iJubaoedCount - m_stData.m_iJubaoedCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetJubaoedCount(RealAdd);
        m_stData.m_iJubaoedCount = iJubaoedCount;
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
    const int& GetChangeNickCount() const
    {
        return m_stData.m_iChangeNickCount;
    }
    void AddChangeNickCount(int iChangeNickCount)
    {
        if (0 == iChangeNickCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChangeNickCount(iChangeNickCount);
        m_stData.m_iChangeNickCount += iChangeNickCount;
    }
    void SetChangeNickCount(int iChangeNickCount)
    {
        if (m_stData.m_iChangeNickCount == iChangeNickCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iChangeNickCount - m_stData.m_iChangeNickCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChangeNickCount(RealAdd);
        m_stData.m_iChangeNickCount = iChangeNickCount;
    }
    const int& GetAvatarID() const
    {
        return m_stData.m_iAvatarID;
    }
    void AddAvatarID(int iAvatarID)
    {
        if (0 == iAvatarID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAvatarID(iAvatarID);
        m_stData.m_iAvatarID += iAvatarID;
    }
    void SetAvatarID(int iAvatarID)
    {
        if (m_stData.m_iAvatarID == iAvatarID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAvatarID - m_stData.m_iAvatarID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAvatarID(RealAdd);
        m_stData.m_iAvatarID = iAvatarID;
    }
    const int& GetAvatarBorder() const
    {
        return m_stData.m_iAvatarBorder;
    }
    void AddAvatarBorder(int iAvatarBorder)
    {
        if (0 == iAvatarBorder)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAvatarBorder(iAvatarBorder);
        m_stData.m_iAvatarBorder += iAvatarBorder;
    }
    void SetAvatarBorder(int iAvatarBorder)
    {
        if (m_stData.m_iAvatarBorder == iAvatarBorder)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAvatarBorder - m_stData.m_iAvatarBorder;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAvatarBorder(RealAdd);
        m_stData.m_iAvatarBorder = iAvatarBorder;
    }
    const int& GetLevel() const
    {
        return m_stData.m_iLevel;
    }
    void AddLevel(int iLevel)
    {
        if (0 == iLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLevel(iLevel);
        m_stData.m_iLevel += iLevel;
    }
    void SetLevel(int iLevel)
    {
        if (m_stData.m_iLevel == iLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLevel - m_stData.m_iLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLevel(RealAdd);
        m_stData.m_iLevel = iLevel;
    }
    const int& GetExp() const
    {
        return m_stData.m_iExp;
    }
    void AddExp(int iExp)
    {
        if (0 == iExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExp(iExp);
        m_stData.m_iExp += iExp;
    }
    void SetExp(int iExp)
    {
        if (m_stData.m_iExp == iExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iExp - m_stData.m_iExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExp(RealAdd);
        m_stData.m_iExp = iExp;
    }
    const int& GetVipExp() const
    {
        return m_stData.m_iVipExp;
    }
    void AddVipExp(int iVipExp)
    {
        if (0 == iVipExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetVipExp(iVipExp);
        m_stData.m_iVipExp += iVipExp;
    }
    void SetVipExp(int iVipExp)
    {
        if (m_stData.m_iVipExp == iVipExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iVipExp - m_stData.m_iVipExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetVipExp(RealAdd);
        m_stData.m_iVipExp = iVipExp;
    }
    const int& GetAddUpRmb() const
    {
        return m_stData.m_iAddUpRmb;
    }
    void AddAddUpRmb(int iAddUpRmb)
    {
        if (0 == iAddUpRmb)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAddUpRmb(iAddUpRmb);
        m_stData.m_iAddUpRmb += iAddUpRmb;
    }
    void SetAddUpRmb(int iAddUpRmb)
    {
        if (m_stData.m_iAddUpRmb == iAddUpRmb)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAddUpRmb - m_stData.m_iAddUpRmb;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAddUpRmb(RealAdd);
        m_stData.m_iAddUpRmb = iAddUpRmb;
    }
    const int& GetMonthAddUpRmb() const
    {
        return m_stData.m_iMonthAddUpRmb;
    }
    void AddMonthAddUpRmb(int iMonthAddUpRmb)
    {
        if (0 == iMonthAddUpRmb)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthAddUpRmb(iMonthAddUpRmb);
        m_stData.m_iMonthAddUpRmb += iMonthAddUpRmb;
    }
    void SetMonthAddUpRmb(int iMonthAddUpRmb)
    {
        if (m_stData.m_iMonthAddUpRmb == iMonthAddUpRmb)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMonthAddUpRmb - m_stData.m_iMonthAddUpRmb;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthAddUpRmb(RealAdd);
        m_stData.m_iMonthAddUpRmb = iMonthAddUpRmb;
    }
    const int& GetFirstDepositBonus() const
    {
        return m_stData.m_iFirstDepositBonus;
    }
    void AddFirstDepositBonus(int iFirstDepositBonus)
    {
        if (0 == iFirstDepositBonus)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFirstDepositBonus(iFirstDepositBonus);
        m_stData.m_iFirstDepositBonus += iFirstDepositBonus;
    }
    void SetFirstDepositBonus(int iFirstDepositBonus)
    {
        if (m_stData.m_iFirstDepositBonus == iFirstDepositBonus)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFirstDepositBonus - m_stData.m_iFirstDepositBonus;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFirstDepositBonus(RealAdd);
        m_stData.m_iFirstDepositBonus = iFirstDepositBonus;
    }
    const int& GetLastLogin() const
    {
        return m_stData.m_iLastLogin;
    }
    void AddLastLogin(int iLastLogin)
    {
        if (0 == iLastLogin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastLogin(iLastLogin);
        m_stData.m_iLastLogin += iLastLogin;
    }
    void SetLastLogin(int iLastLogin)
    {
        if (m_stData.m_iLastLogin == iLastLogin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastLogin - m_stData.m_iLastLogin;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastLogin(RealAdd);
        m_stData.m_iLastLogin = iLastLogin;
    }
    const int& GetLastLogout() const
    {
        return m_stData.m_iLastLogout;
    }
    void AddLastLogout(int iLastLogout)
    {
        if (0 == iLastLogout)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastLogout(iLastLogout);
        m_stData.m_iLastLogout += iLastLogout;
    }
    void SetLastLogout(int iLastLogout)
    {
        if (m_stData.m_iLastLogout == iLastLogout)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastLogout - m_stData.m_iLastLogout;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastLogout(RealAdd);
        m_stData.m_iLastLogout = iLastLogout;
    }
    const int& GetAddUpLoginDay() const
    {
        return m_stData.m_iAddUpLoginDay;
    }
    void AddAddUpLoginDay(int iAddUpLoginDay)
    {
        if (0 == iAddUpLoginDay)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAddUpLoginDay(iAddUpLoginDay);
        m_stData.m_iAddUpLoginDay += iAddUpLoginDay;
    }
    void SetAddUpLoginDay(int iAddUpLoginDay)
    {
        if (m_stData.m_iAddUpLoginDay == iAddUpLoginDay)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAddUpLoginDay - m_stData.m_iAddUpLoginDay;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAddUpLoginDay(RealAdd);
        m_stData.m_iAddUpLoginDay = iAddUpLoginDay;
    }
    const int& GetCreateTime() const
    {
        return m_stData.m_iCreateTime;
    }
    void AddCreateTime(int iCreateTime)
    {
        if (0 == iCreateTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCreateTime(iCreateTime);
        m_stData.m_iCreateTime += iCreateTime;
    }
    void SetCreateTime(int iCreateTime)
    {
        if (m_stData.m_iCreateTime == iCreateTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCreateTime - m_stData.m_iCreateTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCreateTime(RealAdd);
        m_stData.m_iCreateTime = iCreateTime;
    }
    const int& GetFightLevelID() const
    {
        return m_stData.m_iFightLevelID;
    }
    void AddFightLevelID(int iFightLevelID)
    {
        if (0 == iFightLevelID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightLevelID(iFightLevelID);
        m_stData.m_iFightLevelID += iFightLevelID;
    }
    void SetFightLevelID(int iFightLevelID)
    {
        if (m_stData.m_iFightLevelID == iFightLevelID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFightLevelID - m_stData.m_iFightLevelID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightLevelID(RealAdd);
        m_stData.m_iFightLevelID = iFightLevelID;
    }
    const int& GetGlobalMailTimePos() const
    {
        return m_stData.m_iGlobalMailTimePos;
    }
    void AddGlobalMailTimePos(int iGlobalMailTimePos)
    {
        if (0 == iGlobalMailTimePos)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGlobalMailTimePos(iGlobalMailTimePos);
        m_stData.m_iGlobalMailTimePos += iGlobalMailTimePos;
    }
    void SetGlobalMailTimePos(int iGlobalMailTimePos)
    {
        if (m_stData.m_iGlobalMailTimePos == iGlobalMailTimePos)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGlobalMailTimePos - m_stData.m_iGlobalMailTimePos;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGlobalMailTimePos(RealAdd);
        m_stData.m_iGlobalMailTimePos = iGlobalMailTimePos;
    }
    const int& GetNpcMailDay() const
    {
        return m_stData.m_iNpcMailDay;
    }
    void AddNpcMailDay(int iNpcMailDay)
    {
        if (0 == iNpcMailDay)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNpcMailDay(iNpcMailDay);
        m_stData.m_iNpcMailDay += iNpcMailDay;
    }
    void SetNpcMailDay(int iNpcMailDay)
    {
        if (m_stData.m_iNpcMailDay == iNpcMailDay)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNpcMailDay - m_stData.m_iNpcMailDay;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNpcMailDay(RealAdd);
        m_stData.m_iNpcMailDay = iNpcMailDay;
    }
    const int& GetNpcMailServer() const
    {
        return m_stData.m_iNpcMailServer;
    }
    void AddNpcMailServer(int iNpcMailServer)
    {
        if (0 == iNpcMailServer)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNpcMailServer(iNpcMailServer);
        m_stData.m_iNpcMailServer += iNpcMailServer;
    }
    void SetNpcMailServer(int iNpcMailServer)
    {
        if (m_stData.m_iNpcMailServer == iNpcMailServer)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNpcMailServer - m_stData.m_iNpcMailServer;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNpcMailServer(RealAdd);
        m_stData.m_iNpcMailServer = iNpcMailServer;
    }
    const int& GetNpcMailDatetime() const
    {
        return m_stData.m_iNpcMailDatetime;
    }
    void AddNpcMailDatetime(int iNpcMailDatetime)
    {
        if (0 == iNpcMailDatetime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNpcMailDatetime(iNpcMailDatetime);
        m_stData.m_iNpcMailDatetime += iNpcMailDatetime;
    }
    void SetNpcMailDatetime(int iNpcMailDatetime)
    {
        if (m_stData.m_iNpcMailDatetime == iNpcMailDatetime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNpcMailDatetime - m_stData.m_iNpcMailDatetime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNpcMailDatetime(RealAdd);
        m_stData.m_iNpcMailDatetime = iNpcMailDatetime;
    }
    const int& GetNpcMailLevel() const
    {
        return m_stData.m_iNpcMailLevel;
    }
    void AddNpcMailLevel(int iNpcMailLevel)
    {
        if (0 == iNpcMailLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNpcMailLevel(iNpcMailLevel);
        m_stData.m_iNpcMailLevel += iNpcMailLevel;
    }
    void SetNpcMailLevel(int iNpcMailLevel)
    {
        if (m_stData.m_iNpcMailLevel == iNpcMailLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNpcMailLevel - m_stData.m_iNpcMailLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNpcMailLevel(RealAdd);
        m_stData.m_iNpcMailLevel = iNpcMailLevel;
    }
    const int& GetNpcMailCardtime() const
    {
        return m_stData.m_iNpcMailCardtime;
    }
    void AddNpcMailCardtime(int iNpcMailCardtime)
    {
        if (0 == iNpcMailCardtime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNpcMailCardtime(iNpcMailCardtime);
        m_stData.m_iNpcMailCardtime += iNpcMailCardtime;
    }
    void SetNpcMailCardtime(int iNpcMailCardtime)
    {
        if (m_stData.m_iNpcMailCardtime == iNpcMailCardtime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNpcMailCardtime - m_stData.m_iNpcMailCardtime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNpcMailCardtime(RealAdd);
        m_stData.m_iNpcMailCardtime = iNpcMailCardtime;
    }
    const int& GetThemeSkin() const
    {
        return m_stData.m_iThemeSkin;
    }
    void AddThemeSkin(int iThemeSkin)
    {
        if (0 == iThemeSkin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetThemeSkin(iThemeSkin);
        m_stData.m_iThemeSkin += iThemeSkin;
    }
    void SetThemeSkin(int iThemeSkin)
    {
        if (m_stData.m_iThemeSkin == iThemeSkin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iThemeSkin - m_stData.m_iThemeSkin;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetThemeSkin(RealAdd);
        m_stData.m_iThemeSkin = iThemeSkin;
    }
    const int& GetRuneCard() const
    {
        return m_stData.m_iRuneCard;
    }
    void AddRuneCard(int iRuneCard)
    {
        if (0 == iRuneCard)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRuneCard(iRuneCard);
        m_stData.m_iRuneCard += iRuneCard;
    }
    void SetRuneCard(int iRuneCard)
    {
        if (m_stData.m_iRuneCard == iRuneCard)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iRuneCard - m_stData.m_iRuneCard;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRuneCard(RealAdd);
        m_stData.m_iRuneCard = iRuneCard;
    }
    const int& GetLastSignTime() const
    {
        return m_stData.m_iLastSignTime;
    }
    void AddLastSignTime(int iLastSignTime)
    {
        if (0 == iLastSignTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastSignTime(iLastSignTime);
        m_stData.m_iLastSignTime += iLastSignTime;
    }
    void SetLastSignTime(int iLastSignTime)
    {
        if (m_stData.m_iLastSignTime == iLastSignTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastSignTime - m_stData.m_iLastSignTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastSignTime(RealAdd);
        m_stData.m_iLastSignTime = iLastSignTime;
    }
    const int& GetTotalSign() const
    {
        return m_stData.m_iTotalSign;
    }
    void AddTotalSign(int iTotalSign)
    {
        if (0 == iTotalSign)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalSign(iTotalSign);
        m_stData.m_iTotalSign += iTotalSign;
    }
    void SetTotalSign(int iTotalSign)
    {
        if (m_stData.m_iTotalSign == iTotalSign)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTotalSign - m_stData.m_iTotalSign;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalSign(RealAdd);
        m_stData.m_iTotalSign = iTotalSign;
    }
    const int& GetActiveDoubleDepositCount() const
    {
        return m_stData.m_iActiveDoubleDepositCount;
    }
    void AddActiveDoubleDepositCount(int iActiveDoubleDepositCount)
    {
        if (0 == iActiveDoubleDepositCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActiveDoubleDepositCount(iActiveDoubleDepositCount);
        m_stData.m_iActiveDoubleDepositCount += iActiveDoubleDepositCount;
    }
    void SetActiveDoubleDepositCount(int iActiveDoubleDepositCount)
    {
        if (m_stData.m_iActiveDoubleDepositCount == iActiveDoubleDepositCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iActiveDoubleDepositCount - m_stData.m_iActiveDoubleDepositCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActiveDoubleDepositCount(RealAdd);
        m_stData.m_iActiveDoubleDepositCount = iActiveDoubleDepositCount;
    }
    const int& GetRefreshDoubleDepositTime() const
    {
        return m_stData.m_iRefreshDoubleDepositTime;
    }
    void AddRefreshDoubleDepositTime(int iRefreshDoubleDepositTime)
    {
        if (0 == iRefreshDoubleDepositTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRefreshDoubleDepositTime(iRefreshDoubleDepositTime);
        m_stData.m_iRefreshDoubleDepositTime += iRefreshDoubleDepositTime;
    }
    void SetRefreshDoubleDepositTime(int iRefreshDoubleDepositTime)
    {
        if (m_stData.m_iRefreshDoubleDepositTime == iRefreshDoubleDepositTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iRefreshDoubleDepositTime - m_stData.m_iRefreshDoubleDepositTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRefreshDoubleDepositTime(RealAdd);
        m_stData.m_iRefreshDoubleDepositTime = iRefreshDoubleDepositTime;
    }
    const int& GetRmb() const
    {
        return m_stData.m_iRmb;
    }
    void AddRmb(int iRmb)
    {
        if (0 == iRmb)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRmb(iRmb);
        m_stData.m_iRmb += iRmb;
    }
    void SetRmb(int iRmb)
    {
        if (m_stData.m_iRmb == iRmb)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iRmb - m_stData.m_iRmb;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRmb(RealAdd);
        m_stData.m_iRmb = iRmb;
    }
    const int& GetPayRmb() const
    {
        return m_stData.m_iPayRmb;
    }
    void AddPayRmb(int iPayRmb)
    {
        if (0 == iPayRmb)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayRmb(iPayRmb);
        m_stData.m_iPayRmb += iPayRmb;
    }
    void SetPayRmb(int iPayRmb)
    {
        if (m_stData.m_iPayRmb == iPayRmb)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPayRmb - m_stData.m_iPayRmb;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayRmb(RealAdd);
        m_stData.m_iPayRmb = iPayRmb;
    }
    const int& GetGold() const
    {
        return m_stData.m_iGold;
    }
    void AddGold(int iGold)
    {
        if (0 == iGold)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGold(iGold);
        m_stData.m_iGold += iGold;
    }
    void SetGold(int iGold)
    {
        if (m_stData.m_iGold == iGold)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGold - m_stData.m_iGold;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGold(RealAdd);
        m_stData.m_iGold = iGold;
    }
    const int& GetEvilBrand() const
    {
        return m_stData.m_iEvilBrand;
    }
    void AddEvilBrand(int iEvilBrand)
    {
        if (0 == iEvilBrand)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEvilBrand(iEvilBrand);
        m_stData.m_iEvilBrand += iEvilBrand;
    }
    void SetEvilBrand(int iEvilBrand)
    {
        if (m_stData.m_iEvilBrand == iEvilBrand)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEvilBrand - m_stData.m_iEvilBrand;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEvilBrand(RealAdd);
        m_stData.m_iEvilBrand = iEvilBrand;
    }
    const int& GetCharm() const
    {
        return m_stData.m_iCharm;
    }
    void AddCharm(int iCharm)
    {
        if (0 == iCharm)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCharm(iCharm);
        m_stData.m_iCharm += iCharm;
    }
    void SetCharm(int iCharm)
    {
        if (m_stData.m_iCharm == iCharm)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCharm - m_stData.m_iCharm;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCharm(RealAdd);
        m_stData.m_iCharm = iCharm;
    }
    const int& GetPhyPower() const
    {
        return m_stData.m_iPhyPower;
    }
    void AddPhyPower(int iPhyPower)
    {
        if (0 == iPhyPower)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPhyPower(iPhyPower);
        m_stData.m_iPhyPower += iPhyPower;
    }
    void SetPhyPower(int iPhyPower)
    {
        if (m_stData.m_iPhyPower == iPhyPower)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPhyPower - m_stData.m_iPhyPower;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPhyPower(RealAdd);
        m_stData.m_iPhyPower = iPhyPower;
    }
    const int& GetPhyStore() const
    {
        return m_stData.m_iPhyStore;
    }
    void AddPhyStore(int iPhyStore)
    {
        if (0 == iPhyStore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPhyStore(iPhyStore);
        m_stData.m_iPhyStore += iPhyStore;
    }
    void SetPhyStore(int iPhyStore)
    {
        if (m_stData.m_iPhyStore == iPhyStore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPhyStore - m_stData.m_iPhyStore;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPhyStore(RealAdd);
        m_stData.m_iPhyStore = iPhyStore;
    }
    const int& GetLastAutoAddTime() const
    {
        return m_stData.m_iLastAutoAddTime;
    }
    void AddLastAutoAddTime(int iLastAutoAddTime)
    {
        if (0 == iLastAutoAddTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastAutoAddTime(iLastAutoAddTime);
        m_stData.m_iLastAutoAddTime += iLastAutoAddTime;
    }
    void SetLastAutoAddTime(int iLastAutoAddTime)
    {
        if (m_stData.m_iLastAutoAddTime == iLastAutoAddTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastAutoAddTime - m_stData.m_iLastAutoAddTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastAutoAddTime(RealAdd);
        m_stData.m_iLastAutoAddTime = iLastAutoAddTime;
    }
    const int& GetVitality() const
    {
        return m_stData.m_iVitality;
    }
    void AddVitality(int iVitality)
    {
        if (0 == iVitality)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetVitality(iVitality);
        m_stData.m_iVitality += iVitality;
    }
    void SetVitality(int iVitality)
    {
        if (m_stData.m_iVitality == iVitality)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iVitality - m_stData.m_iVitality;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetVitality(RealAdd);
        m_stData.m_iVitality = iVitality;
    }
    const int& GetAddVitalityTime() const
    {
        return m_stData.m_iAddVitalityTime;
    }
    void AddAddVitalityTime(int iAddVitalityTime)
    {
        if (0 == iAddVitalityTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAddVitalityTime(iAddVitalityTime);
        m_stData.m_iAddVitalityTime += iAddVitalityTime;
    }
    void SetAddVitalityTime(int iAddVitalityTime)
    {
        if (m_stData.m_iAddVitalityTime == iAddVitalityTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAddVitalityTime - m_stData.m_iAddVitalityTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAddVitalityTime(RealAdd);
        m_stData.m_iAddVitalityTime = iAddVitalityTime;
    }
    const int& GetLastMergeRefreshTime() const
    {
        return m_stData.m_iLastMergeRefreshTime;
    }
    void AddLastMergeRefreshTime(int iLastMergeRefreshTime)
    {
        if (0 == iLastMergeRefreshTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastMergeRefreshTime(iLastMergeRefreshTime);
        m_stData.m_iLastMergeRefreshTime += iLastMergeRefreshTime;
    }
    void SetLastMergeRefreshTime(int iLastMergeRefreshTime)
    {
        if (m_stData.m_iLastMergeRefreshTime == iLastMergeRefreshTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastMergeRefreshTime - m_stData.m_iLastMergeRefreshTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastMergeRefreshTime(RealAdd);
        m_stData.m_iLastMergeRefreshTime = iLastMergeRefreshTime;
    }
    const int& GetRoleCombat() const
    {
        return m_stData.m_iRoleCombat;
    }
    void AddRoleCombat(int iRoleCombat)
    {
        if (0 == iRoleCombat)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRoleCombat(iRoleCombat);
        m_stData.m_iRoleCombat += iRoleCombat;
    }
    void SetRoleCombat(int iRoleCombat)
    {
        if (m_stData.m_iRoleCombat == iRoleCombat)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iRoleCombat - m_stData.m_iRoleCombat;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRoleCombat(RealAdd);
        m_stData.m_iRoleCombat = iRoleCombat;
    }
    const int& GetRoleCombatMax() const
    {
        return m_stData.m_iRoleCombatMax;
    }
    void AddRoleCombatMax(int iRoleCombatMax)
    {
        if (0 == iRoleCombatMax)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRoleCombatMax(iRoleCombatMax);
        m_stData.m_iRoleCombatMax += iRoleCombatMax;
    }
    void SetRoleCombatMax(int iRoleCombatMax)
    {
        if (m_stData.m_iRoleCombatMax == iRoleCombatMax)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iRoleCombatMax - m_stData.m_iRoleCombatMax;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRoleCombatMax(RealAdd);
        m_stData.m_iRoleCombatMax = iRoleCombatMax;
    }
    const int& GetDailyDeposit() const
    {
        return m_stData.m_iDailyDeposit;
    }
    void AddDailyDeposit(int iDailyDeposit)
    {
        if (0 == iDailyDeposit)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyDeposit(iDailyDeposit);
        m_stData.m_iDailyDeposit += iDailyDeposit;
    }
    void SetDailyDeposit(int iDailyDeposit)
    {
        if (m_stData.m_iDailyDeposit == iDailyDeposit)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDailyDeposit - m_stData.m_iDailyDeposit;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyDeposit(RealAdd);
        m_stData.m_iDailyDeposit = iDailyDeposit;
    }
    const int& GetDailyDepositBonus() const
    {
        return m_stData.m_iDailyDepositBonus;
    }
    void AddDailyDepositBonus(int iDailyDepositBonus)
    {
        if (0 == iDailyDepositBonus)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyDepositBonus(iDailyDepositBonus);
        m_stData.m_iDailyDepositBonus += iDailyDepositBonus;
    }
    void SetDailyDepositBonus(int iDailyDepositBonus)
    {
        if (m_stData.m_iDailyDepositBonus == iDailyDepositBonus)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDailyDepositBonus - m_stData.m_iDailyDepositBonus;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyDepositBonus(RealAdd);
        m_stData.m_iDailyDepositBonus = iDailyDepositBonus;
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
    const int& GetScoreRankBonusTime() const
    {
        return m_stData.m_iScoreRankBonusTime;
    }
    void AddScoreRankBonusTime(int iScoreRankBonusTime)
    {
        if (0 == iScoreRankBonusTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetScoreRankBonusTime(iScoreRankBonusTime);
        m_stData.m_iScoreRankBonusTime += iScoreRankBonusTime;
    }
    void SetScoreRankBonusTime(int iScoreRankBonusTime)
    {
        if (m_stData.m_iScoreRankBonusTime == iScoreRankBonusTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iScoreRankBonusTime - m_stData.m_iScoreRankBonusTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetScoreRankBonusTime(RealAdd);
        m_stData.m_iScoreRankBonusTime = iScoreRankBonusTime;
    }
    const int& GetCombatRankBonusTime() const
    {
        return m_stData.m_iCombatRankBonusTime;
    }
    void AddCombatRankBonusTime(int iCombatRankBonusTime)
    {
        if (0 == iCombatRankBonusTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCombatRankBonusTime(iCombatRankBonusTime);
        m_stData.m_iCombatRankBonusTime += iCombatRankBonusTime;
    }
    void SetCombatRankBonusTime(int iCombatRankBonusTime)
    {
        if (m_stData.m_iCombatRankBonusTime == iCombatRankBonusTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCombatRankBonusTime - m_stData.m_iCombatRankBonusTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCombatRankBonusTime(RealAdd);
        m_stData.m_iCombatRankBonusTime = iCombatRankBonusTime;
    }
    const int& GetDailyTaskChangeTime() const
    {
        return m_stData.m_iDailyTaskChangeTime;
    }
    void AddDailyTaskChangeTime(int iDailyTaskChangeTime)
    {
        if (0 == iDailyTaskChangeTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyTaskChangeTime(iDailyTaskChangeTime);
        m_stData.m_iDailyTaskChangeTime += iDailyTaskChangeTime;
    }
    void SetDailyTaskChangeTime(int iDailyTaskChangeTime)
    {
        if (m_stData.m_iDailyTaskChangeTime == iDailyTaskChangeTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDailyTaskChangeTime - m_stData.m_iDailyTaskChangeTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyTaskChangeTime(RealAdd);
        m_stData.m_iDailyTaskChangeTime = iDailyTaskChangeTime;
    }
    const int& GetFightPowerHighestRank() const
    {
        return m_stData.m_iFightPowerHighestRank;
    }
    void AddFightPowerHighestRank(int iFightPowerHighestRank)
    {
        if (0 == iFightPowerHighestRank)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightPowerHighestRank(iFightPowerHighestRank);
        m_stData.m_iFightPowerHighestRank += iFightPowerHighestRank;
    }
    void SetFightPowerHighestRank(int iFightPowerHighestRank)
    {
        if (m_stData.m_iFightPowerHighestRank == iFightPowerHighestRank)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFightPowerHighestRank - m_stData.m_iFightPowerHighestRank;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightPowerHighestRank(RealAdd);
        m_stData.m_iFightPowerHighestRank = iFightPowerHighestRank;
    }
    const int& GetBuyGoldTimes() const
    {
        return m_stData.m_iBuyGoldTimes;
    }
    void AddBuyGoldTimes(int iBuyGoldTimes)
    {
        if (0 == iBuyGoldTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyGoldTimes(iBuyGoldTimes);
        m_stData.m_iBuyGoldTimes += iBuyGoldTimes;
    }
    void SetBuyGoldTimes(int iBuyGoldTimes)
    {
        if (m_stData.m_iBuyGoldTimes == iBuyGoldTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyGoldTimes - m_stData.m_iBuyGoldTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyGoldTimes(RealAdd);
        m_stData.m_iBuyGoldTimes = iBuyGoldTimes;
    }
    const int& GetBuyGoldTotalTimes() const
    {
        return m_stData.m_iBuyGoldTotalTimes;
    }
    void AddBuyGoldTotalTimes(int iBuyGoldTotalTimes)
    {
        if (0 == iBuyGoldTotalTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyGoldTotalTimes(iBuyGoldTotalTimes);
        m_stData.m_iBuyGoldTotalTimes += iBuyGoldTotalTimes;
    }
    void SetBuyGoldTotalTimes(int iBuyGoldTotalTimes)
    {
        if (m_stData.m_iBuyGoldTotalTimes == iBuyGoldTotalTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyGoldTotalTimes - m_stData.m_iBuyGoldTotalTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyGoldTotalTimes(RealAdd);
        m_stData.m_iBuyGoldTotalTimes = iBuyGoldTotalTimes;
    }
    const int& GetBuyEnergyTimes() const
    {
        return m_stData.m_iBuyEnergyTimes;
    }
    void AddBuyEnergyTimes(int iBuyEnergyTimes)
    {
        if (0 == iBuyEnergyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyEnergyTimes(iBuyEnergyTimes);
        m_stData.m_iBuyEnergyTimes += iBuyEnergyTimes;
    }
    void SetBuyEnergyTimes(int iBuyEnergyTimes)
    {
        if (m_stData.m_iBuyEnergyTimes == iBuyEnergyTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyEnergyTimes - m_stData.m_iBuyEnergyTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyEnergyTimes(RealAdd);
        m_stData.m_iBuyEnergyTimes = iBuyEnergyTimes;
    }
    const int& GetLastBuyEnergyTime() const
    {
        return m_stData.m_iLastBuyEnergyTime;
    }
    void AddLastBuyEnergyTime(int iLastBuyEnergyTime)
    {
        if (0 == iLastBuyEnergyTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastBuyEnergyTime(iLastBuyEnergyTime);
        m_stData.m_iLastBuyEnergyTime += iLastBuyEnergyTime;
    }
    void SetLastBuyEnergyTime(int iLastBuyEnergyTime)
    {
        if (m_stData.m_iLastBuyEnergyTime == iLastBuyEnergyTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastBuyEnergyTime - m_stData.m_iLastBuyEnergyTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastBuyEnergyTime(RealAdd);
        m_stData.m_iLastBuyEnergyTime = iLastBuyEnergyTime;
    }
    const int& GetBuyVitalityTimes() const
    {
        return m_stData.m_iBuyVitalityTimes;
    }
    void AddBuyVitalityTimes(int iBuyVitalityTimes)
    {
        if (0 == iBuyVitalityTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyVitalityTimes(iBuyVitalityTimes);
        m_stData.m_iBuyVitalityTimes += iBuyVitalityTimes;
    }
    void SetBuyVitalityTimes(int iBuyVitalityTimes)
    {
        if (m_stData.m_iBuyVitalityTimes == iBuyVitalityTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBuyVitalityTimes - m_stData.m_iBuyVitalityTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBuyVitalityTimes(RealAdd);
        m_stData.m_iBuyVitalityTimes = iBuyVitalityTimes;
    }
    const int& GetLastBuyVitalityTime() const
    {
        return m_stData.m_iLastBuyVitalityTime;
    }
    void AddLastBuyVitalityTime(int iLastBuyVitalityTime)
    {
        if (0 == iLastBuyVitalityTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastBuyVitalityTime(iLastBuyVitalityTime);
        m_stData.m_iLastBuyVitalityTime += iLastBuyVitalityTime;
    }
    void SetLastBuyVitalityTime(int iLastBuyVitalityTime)
    {
        if (m_stData.m_iLastBuyVitalityTime == iLastBuyVitalityTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastBuyVitalityTime - m_stData.m_iLastBuyVitalityTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastBuyVitalityTime(RealAdd);
        m_stData.m_iLastBuyVitalityTime = iLastBuyVitalityTime;
    }
    const int& GetTotalOnlineTime() const
    {
        return m_stData.m_iTotalOnlineTime;
    }
    void AddTotalOnlineTime(int iTotalOnlineTime)
    {
        if (0 == iTotalOnlineTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalOnlineTime(iTotalOnlineTime);
        m_stData.m_iTotalOnlineTime += iTotalOnlineTime;
    }
    void SetTotalOnlineTime(int iTotalOnlineTime)
    {
        if (m_stData.m_iTotalOnlineTime == iTotalOnlineTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTotalOnlineTime - m_stData.m_iTotalOnlineTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalOnlineTime(RealAdd);
        m_stData.m_iTotalOnlineTime = iTotalOnlineTime;
    }
    const int& GetRefreshPvpTimes() const
    {
        return m_stData.m_iRefreshPvpTimes;
    }
    void AddRefreshPvpTimes(int iRefreshPvpTimes)
    {
        if (0 == iRefreshPvpTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRefreshPvpTimes(iRefreshPvpTimes);
        m_stData.m_iRefreshPvpTimes += iRefreshPvpTimes;
    }
    void SetRefreshPvpTimes(int iRefreshPvpTimes)
    {
        if (m_stData.m_iRefreshPvpTimes == iRefreshPvpTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iRefreshPvpTimes - m_stData.m_iRefreshPvpTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRefreshPvpTimes(RealAdd);
        m_stData.m_iRefreshPvpTimes = iRefreshPvpTimes;
    }
    const int& GetLastRefreshPvpTime() const
    {
        return m_stData.m_iLastRefreshPvpTime;
    }
    void AddLastRefreshPvpTime(int iLastRefreshPvpTime)
    {
        if (0 == iLastRefreshPvpTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastRefreshPvpTime(iLastRefreshPvpTime);
        m_stData.m_iLastRefreshPvpTime += iLastRefreshPvpTime;
    }
    void SetLastRefreshPvpTime(int iLastRefreshPvpTime)
    {
        if (m_stData.m_iLastRefreshPvpTime == iLastRefreshPvpTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastRefreshPvpTime - m_stData.m_iLastRefreshPvpTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastRefreshPvpTime(RealAdd);
        m_stData.m_iLastRefreshPvpTime = iLastRefreshPvpTime;
    }
    const int& GetCrossFightResetTime() const
    {
        return m_stData.m_iCrossFightResetTime;
    }
    void AddCrossFightResetTime(int iCrossFightResetTime)
    {
        if (0 == iCrossFightResetTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrossFightResetTime(iCrossFightResetTime);
        m_stData.m_iCrossFightResetTime += iCrossFightResetTime;
    }
    void SetCrossFightResetTime(int iCrossFightResetTime)
    {
        if (m_stData.m_iCrossFightResetTime == iCrossFightResetTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCrossFightResetTime - m_stData.m_iCrossFightResetTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrossFightResetTime(RealAdd);
        m_stData.m_iCrossFightResetTime = iCrossFightResetTime;
    }
    const int& GetCrossFightResetTimes() const
    {
        return m_stData.m_iCrossFightResetTimes;
    }
    void AddCrossFightResetTimes(int iCrossFightResetTimes)
    {
        if (0 == iCrossFightResetTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrossFightResetTimes(iCrossFightResetTimes);
        m_stData.m_iCrossFightResetTimes += iCrossFightResetTimes;
    }
    void SetCrossFightResetTimes(int iCrossFightResetTimes)
    {
        if (m_stData.m_iCrossFightResetTimes == iCrossFightResetTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCrossFightResetTimes - m_stData.m_iCrossFightResetTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrossFightResetTimes(RealAdd);
        m_stData.m_iCrossFightResetTimes = iCrossFightResetTimes;
    }
    const int& GetCrossFightRefreshResetTimesTimes() const
    {
        return m_stData.m_iCrossFightRefreshResetTimesTimes;
    }
    void AddCrossFightRefreshResetTimesTimes(int iCrossFightRefreshResetTimesTimes)
    {
        if (0 == iCrossFightRefreshResetTimesTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrossFightRefreshResetTimesTimes(iCrossFightRefreshResetTimesTimes);
        m_stData.m_iCrossFightRefreshResetTimesTimes += iCrossFightRefreshResetTimesTimes;
    }
    void SetCrossFightRefreshResetTimesTimes(int iCrossFightRefreshResetTimesTimes)
    {
        if (m_stData.m_iCrossFightRefreshResetTimesTimes == iCrossFightRefreshResetTimesTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCrossFightRefreshResetTimesTimes - m_stData.m_iCrossFightRefreshResetTimesTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrossFightRefreshResetTimesTimes(RealAdd);
        m_stData.m_iCrossFightRefreshResetTimesTimes = iCrossFightRefreshResetTimesTimes;
    }
    const int& GetCrossFightLastRefreshResetTimesTime() const
    {
        return m_stData.m_iCrossFightLastRefreshResetTimesTime;
    }
    void AddCrossFightLastRefreshResetTimesTime(int iCrossFightLastRefreshResetTimesTime)
    {
        if (0 == iCrossFightLastRefreshResetTimesTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrossFightLastRefreshResetTimesTime(iCrossFightLastRefreshResetTimesTime);
        m_stData.m_iCrossFightLastRefreshResetTimesTime += iCrossFightLastRefreshResetTimesTime;
    }
    void SetCrossFightLastRefreshResetTimesTime(int iCrossFightLastRefreshResetTimesTime)
    {
        if (m_stData.m_iCrossFightLastRefreshResetTimesTime == iCrossFightLastRefreshResetTimesTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCrossFightLastRefreshResetTimesTime - m_stData.m_iCrossFightLastRefreshResetTimesTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrossFightLastRefreshResetTimesTime(RealAdd);
        m_stData.m_iCrossFightLastRefreshResetTimesTime = iCrossFightLastRefreshResetTimesTime;
    }
    const int& GetPvpHighestRank() const
    {
        return m_stData.m_iPvpHighestRank;
    }
    void AddPvpHighestRank(int iPvpHighestRank)
    {
        if (0 == iPvpHighestRank)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpHighestRank(iPvpHighestRank);
        m_stData.m_iPvpHighestRank += iPvpHighestRank;
    }
    void SetPvpHighestRank(int iPvpHighestRank)
    {
        if (m_stData.m_iPvpHighestRank == iPvpHighestRank)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPvpHighestRank - m_stData.m_iPvpHighestRank;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpHighestRank(RealAdd);
        m_stData.m_iPvpHighestRank = iPvpHighestRank;
    }
    const int& GetPvpFightTimes() const
    {
        return m_stData.m_iPvpFightTimes;
    }
    void AddPvpFightTimes(int iPvpFightTimes)
    {
        if (0 == iPvpFightTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpFightTimes(iPvpFightTimes);
        m_stData.m_iPvpFightTimes += iPvpFightTimes;
    }
    void SetPvpFightTimes(int iPvpFightTimes)
    {
        if (m_stData.m_iPvpFightTimes == iPvpFightTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPvpFightTimes - m_stData.m_iPvpFightTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpFightTimes(RealAdd);
        m_stData.m_iPvpFightTimes = iPvpFightTimes;
    }
    const int& GetPvpVictoryTimes() const
    {
        return m_stData.m_iPvpVictoryTimes;
    }
    void AddPvpVictoryTimes(int iPvpVictoryTimes)
    {
        if (0 == iPvpVictoryTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpVictoryTimes(iPvpVictoryTimes);
        m_stData.m_iPvpVictoryTimes += iPvpVictoryTimes;
    }
    void SetPvpVictoryTimes(int iPvpVictoryTimes)
    {
        if (m_stData.m_iPvpVictoryTimes == iPvpVictoryTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPvpVictoryTimes - m_stData.m_iPvpVictoryTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpVictoryTimes(RealAdd);
        m_stData.m_iPvpVictoryTimes = iPvpVictoryTimes;
    }
    const int& GetMatchScore() const
    {
        return m_stData.m_iMatchScore;
    }
    void AddMatchScore(int iMatchScore)
    {
        if (0 == iMatchScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchScore(iMatchScore);
        m_stData.m_iMatchScore += iMatchScore;
    }
    void SetMatchScore(int iMatchScore)
    {
        if (m_stData.m_iMatchScore == iMatchScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMatchScore - m_stData.m_iMatchScore;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchScore(RealAdd);
        m_stData.m_iMatchScore = iMatchScore;
    }
    const int& GetMatchMaxScore() const
    {
        return m_stData.m_iMatchMaxScore;
    }
    void AddMatchMaxScore(int iMatchMaxScore)
    {
        if (0 == iMatchMaxScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchMaxScore(iMatchMaxScore);
        m_stData.m_iMatchMaxScore += iMatchMaxScore;
    }
    void SetMatchMaxScore(int iMatchMaxScore)
    {
        if (m_stData.m_iMatchMaxScore == iMatchMaxScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMatchMaxScore - m_stData.m_iMatchMaxScore;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchMaxScore(RealAdd);
        m_stData.m_iMatchMaxScore = iMatchMaxScore;
    }
    const int& GetMatchWinCount() const
    {
        return m_stData.m_iMatchWinCount;
    }
    void AddMatchWinCount(int iMatchWinCount)
    {
        if (0 == iMatchWinCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchWinCount(iMatchWinCount);
        m_stData.m_iMatchWinCount += iMatchWinCount;
    }
    void SetMatchWinCount(int iMatchWinCount)
    {
        if (m_stData.m_iMatchWinCount == iMatchWinCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMatchWinCount - m_stData.m_iMatchWinCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchWinCount(RealAdd);
        m_stData.m_iMatchWinCount = iMatchWinCount;
    }
    const int& GetMatchLoseCount() const
    {
        return m_stData.m_iMatchLoseCount;
    }
    void AddMatchLoseCount(int iMatchLoseCount)
    {
        if (0 == iMatchLoseCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchLoseCount(iMatchLoseCount);
        m_stData.m_iMatchLoseCount += iMatchLoseCount;
    }
    void SetMatchLoseCount(int iMatchLoseCount)
    {
        if (m_stData.m_iMatchLoseCount == iMatchLoseCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMatchLoseCount - m_stData.m_iMatchLoseCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchLoseCount(RealAdd);
        m_stData.m_iMatchLoseCount = iMatchLoseCount;
    }
    const std::string& GetMatchMaxLevelAwardBitmap() const
    {
        return m_stData.m_strMatchMaxLevelAwardBitmap;
    }
    void SetMatchMaxLevelAwardBitmap(const std::string& strMatchMaxLevelAwardBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchMaxLevelAwardBitmap(strMatchMaxLevelAwardBitmap);
        m_stData.m_strMatchMaxLevelAwardBitmap = strMatchMaxLevelAwardBitmap;
    }
    const int& GetMatchDailyCount() const
    {
        return m_stData.m_iMatchDailyCount;
    }
    void AddMatchDailyCount(int iMatchDailyCount)
    {
        if (0 == iMatchDailyCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchDailyCount(iMatchDailyCount);
        m_stData.m_iMatchDailyCount += iMatchDailyCount;
    }
    void SetMatchDailyCount(int iMatchDailyCount)
    {
        if (m_stData.m_iMatchDailyCount == iMatchDailyCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMatchDailyCount - m_stData.m_iMatchDailyCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchDailyCount(RealAdd);
        m_stData.m_iMatchDailyCount = iMatchDailyCount;
    }
    const CPackMatchStoreBuyCount& GetMatchStoreBuyCount() const
    {
        return m_stData.m_stMatchStoreBuyCount;
    }
    void SetMatchStoreBuyCount(const CPackMatchStoreBuyCount& stMatchStoreBuyCount)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMatchStoreBuyCount(stMatchStoreBuyCount);
        m_stData.m_stMatchStoreBuyCount = stMatchStoreBuyCount;
    }
    const int& GetFairPvpWinCount() const
    {
        return m_stData.m_iFairPvpWinCount;
    }
    void AddFairPvpWinCount(int iFairPvpWinCount)
    {
        if (0 == iFairPvpWinCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFairPvpWinCount(iFairPvpWinCount);
        m_stData.m_iFairPvpWinCount += iFairPvpWinCount;
    }
    void SetFairPvpWinCount(int iFairPvpWinCount)
    {
        if (m_stData.m_iFairPvpWinCount == iFairPvpWinCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFairPvpWinCount - m_stData.m_iFairPvpWinCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFairPvpWinCount(RealAdd);
        m_stData.m_iFairPvpWinCount = iFairPvpWinCount;
    }
    const int& GetFairPvpLoseCount() const
    {
        return m_stData.m_iFairPvpLoseCount;
    }
    void AddFairPvpLoseCount(int iFairPvpLoseCount)
    {
        if (0 == iFairPvpLoseCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFairPvpLoseCount(iFairPvpLoseCount);
        m_stData.m_iFairPvpLoseCount += iFairPvpLoseCount;
    }
    void SetFairPvpLoseCount(int iFairPvpLoseCount)
    {
        if (m_stData.m_iFairPvpLoseCount == iFairPvpLoseCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFairPvpLoseCount - m_stData.m_iFairPvpLoseCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFairPvpLoseCount(RealAdd);
        m_stData.m_iFairPvpLoseCount = iFairPvpLoseCount;
    }
    const std::vector<int32_t>& GetFairPvpStoreCard() const
    {
        return m_stData.m_stFairPvpStoreCard;
    }
    void SetFairPvpStoreCard(const std::vector<int32_t>& stFairPvpStoreCard)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFairPvpStoreCard(stFairPvpStoreCard);
        m_stData.m_stFairPvpStoreCard = stFairPvpStoreCard;
    }
    const std::vector<int32_t>& GetFairPvpTeamCard() const
    {
        return m_stData.m_stFairPvpTeamCard;
    }
    void SetFairPvpTeamCard(const std::vector<int32_t>& stFairPvpTeamCard)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFairPvpTeamCard(stFairPvpTeamCard);
        m_stData.m_stFairPvpTeamCard = stFairPvpTeamCard;
    }
    const uint64_t& GetNormalChapterBonus1() const
    {
        return m_stData.m_ulNormalChapterBonus1;
    }
    void AddNormalChapterBonus1(uint64_t ulNormalChapterBonus1)
    {
        if (0 == ulNormalChapterBonus1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNormalChapterBonus1(ulNormalChapterBonus1);
        m_stData.m_ulNormalChapterBonus1 += ulNormalChapterBonus1;
    }
    void SetNormalChapterBonus1(uint64_t ulNormalChapterBonus1)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulNormalChapterBonus1 > ulNormalChapterBonus1)
        {
            m_pRecord->SetNormalChapterBonus1(m_stData.m_ulNormalChapterBonus1 - ulNormalChapterBonus1, '-');
        }
        else
        {
            m_pRecord->SetNormalChapterBonus1(ulNormalChapterBonus1 - m_stData.m_ulNormalChapterBonus1, '+');
        }
        m_stData.m_ulNormalChapterBonus1 = ulNormalChapterBonus1;
    }
    const uint64_t& GetNormalChapterBonus2() const
    {
        return m_stData.m_ulNormalChapterBonus2;
    }
    void AddNormalChapterBonus2(uint64_t ulNormalChapterBonus2)
    {
        if (0 == ulNormalChapterBonus2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNormalChapterBonus2(ulNormalChapterBonus2);
        m_stData.m_ulNormalChapterBonus2 += ulNormalChapterBonus2;
    }
    void SetNormalChapterBonus2(uint64_t ulNormalChapterBonus2)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulNormalChapterBonus2 > ulNormalChapterBonus2)
        {
            m_pRecord->SetNormalChapterBonus2(m_stData.m_ulNormalChapterBonus2 - ulNormalChapterBonus2, '-');
        }
        else
        {
            m_pRecord->SetNormalChapterBonus2(ulNormalChapterBonus2 - m_stData.m_ulNormalChapterBonus2, '+');
        }
        m_stData.m_ulNormalChapterBonus2 = ulNormalChapterBonus2;
    }
    const uint64_t& GetNormalChapterBonus3() const
    {
        return m_stData.m_ulNormalChapterBonus3;
    }
    void AddNormalChapterBonus3(uint64_t ulNormalChapterBonus3)
    {
        if (0 == ulNormalChapterBonus3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNormalChapterBonus3(ulNormalChapterBonus3);
        m_stData.m_ulNormalChapterBonus3 += ulNormalChapterBonus3;
    }
    void SetNormalChapterBonus3(uint64_t ulNormalChapterBonus3)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulNormalChapterBonus3 > ulNormalChapterBonus3)
        {
            m_pRecord->SetNormalChapterBonus3(m_stData.m_ulNormalChapterBonus3 - ulNormalChapterBonus3, '-');
        }
        else
        {
            m_pRecord->SetNormalChapterBonus3(ulNormalChapterBonus3 - m_stData.m_ulNormalChapterBonus3, '+');
        }
        m_stData.m_ulNormalChapterBonus3 = ulNormalChapterBonus3;
    }
    const uint64_t& GetEliteChapterBonus1() const
    {
        return m_stData.m_ulEliteChapterBonus1;
    }
    void AddEliteChapterBonus1(uint64_t ulEliteChapterBonus1)
    {
        if (0 == ulEliteChapterBonus1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEliteChapterBonus1(ulEliteChapterBonus1);
        m_stData.m_ulEliteChapterBonus1 += ulEliteChapterBonus1;
    }
    void SetEliteChapterBonus1(uint64_t ulEliteChapterBonus1)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulEliteChapterBonus1 > ulEliteChapterBonus1)
        {
            m_pRecord->SetEliteChapterBonus1(m_stData.m_ulEliteChapterBonus1 - ulEliteChapterBonus1, '-');
        }
        else
        {
            m_pRecord->SetEliteChapterBonus1(ulEliteChapterBonus1 - m_stData.m_ulEliteChapterBonus1, '+');
        }
        m_stData.m_ulEliteChapterBonus1 = ulEliteChapterBonus1;
    }
    const uint64_t& GetEliteChapterBonus2() const
    {
        return m_stData.m_ulEliteChapterBonus2;
    }
    void AddEliteChapterBonus2(uint64_t ulEliteChapterBonus2)
    {
        if (0 == ulEliteChapterBonus2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEliteChapterBonus2(ulEliteChapterBonus2);
        m_stData.m_ulEliteChapterBonus2 += ulEliteChapterBonus2;
    }
    void SetEliteChapterBonus2(uint64_t ulEliteChapterBonus2)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulEliteChapterBonus2 > ulEliteChapterBonus2)
        {
            m_pRecord->SetEliteChapterBonus2(m_stData.m_ulEliteChapterBonus2 - ulEliteChapterBonus2, '-');
        }
        else
        {
            m_pRecord->SetEliteChapterBonus2(ulEliteChapterBonus2 - m_stData.m_ulEliteChapterBonus2, '+');
        }
        m_stData.m_ulEliteChapterBonus2 = ulEliteChapterBonus2;
    }
    const uint64_t& GetEliteChapterBonus3() const
    {
        return m_stData.m_ulEliteChapterBonus3;
    }
    void AddEliteChapterBonus3(uint64_t ulEliteChapterBonus3)
    {
        if (0 == ulEliteChapterBonus3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEliteChapterBonus3(ulEliteChapterBonus3);
        m_stData.m_ulEliteChapterBonus3 += ulEliteChapterBonus3;
    }
    void SetEliteChapterBonus3(uint64_t ulEliteChapterBonus3)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulEliteChapterBonus3 > ulEliteChapterBonus3)
        {
            m_pRecord->SetEliteChapterBonus3(m_stData.m_ulEliteChapterBonus3 - ulEliteChapterBonus3, '-');
        }
        else
        {
            m_pRecord->SetEliteChapterBonus3(ulEliteChapterBonus3 - m_stData.m_ulEliteChapterBonus3, '+');
        }
        m_stData.m_ulEliteChapterBonus3 = ulEliteChapterBonus3;
    }
    const uint64_t& GetVipLevelBuyBonus() const
    {
        return m_stData.m_ulVipLevelBuyBonus;
    }
    void AddVipLevelBuyBonus(uint64_t ulVipLevelBuyBonus)
    {
        if (0 == ulVipLevelBuyBonus)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetVipLevelBuyBonus(ulVipLevelBuyBonus);
        m_stData.m_ulVipLevelBuyBonus += ulVipLevelBuyBonus;
    }
    void SetVipLevelBuyBonus(uint64_t ulVipLevelBuyBonus)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulVipLevelBuyBonus > ulVipLevelBuyBonus)
        {
            m_pRecord->SetVipLevelBuyBonus(m_stData.m_ulVipLevelBuyBonus - ulVipLevelBuyBonus, '-');
        }
        else
        {
            m_pRecord->SetVipLevelBuyBonus(ulVipLevelBuyBonus - m_stData.m_ulVipLevelBuyBonus, '+');
        }
        m_stData.m_ulVipLevelBuyBonus = ulVipLevelBuyBonus;
    }
    const uint64_t& GetRecommendedLineupBitmap() const
    {
        return m_stData.m_ulRecommendedLineupBitmap;
    }
    void AddRecommendedLineupBitmap(uint64_t ulRecommendedLineupBitmap)
    {
        if (0 == ulRecommendedLineupBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRecommendedLineupBitmap(ulRecommendedLineupBitmap);
        m_stData.m_ulRecommendedLineupBitmap += ulRecommendedLineupBitmap;
    }
    void SetRecommendedLineupBitmap(uint64_t ulRecommendedLineupBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulRecommendedLineupBitmap > ulRecommendedLineupBitmap)
        {
            m_pRecord->SetRecommendedLineupBitmap(m_stData.m_ulRecommendedLineupBitmap - ulRecommendedLineupBitmap, '-');
        }
        else
        {
            m_pRecord->SetRecommendedLineupBitmap(ulRecommendedLineupBitmap - m_stData.m_ulRecommendedLineupBitmap, '+');
        }
        m_stData.m_ulRecommendedLineupBitmap = ulRecommendedLineupBitmap;
    }
    const uint64_t& GetFightPowerRankActive() const
    {
        return m_stData.m_ulFightPowerRankActive;
    }
    void AddFightPowerRankActive(uint64_t ulFightPowerRankActive)
    {
        if (0 == ulFightPowerRankActive)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightPowerRankActive(ulFightPowerRankActive);
        m_stData.m_ulFightPowerRankActive += ulFightPowerRankActive;
    }
    void SetFightPowerRankActive(uint64_t ulFightPowerRankActive)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulFightPowerRankActive > ulFightPowerRankActive)
        {
            m_pRecord->SetFightPowerRankActive(m_stData.m_ulFightPowerRankActive - ulFightPowerRankActive, '-');
        }
        else
        {
            m_pRecord->SetFightPowerRankActive(ulFightPowerRankActive - m_stData.m_ulFightPowerRankActive, '+');
        }
        m_stData.m_ulFightPowerRankActive = ulFightPowerRankActive;
    }
    const int& GetClimbTowerCurrentOrder() const
    {
        return m_stData.m_iClimbTowerCurrentOrder;
    }
    void AddClimbTowerCurrentOrder(int iClimbTowerCurrentOrder)
    {
        if (0 == iClimbTowerCurrentOrder)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetClimbTowerCurrentOrder(iClimbTowerCurrentOrder);
        m_stData.m_iClimbTowerCurrentOrder += iClimbTowerCurrentOrder;
    }
    void SetClimbTowerCurrentOrder(int iClimbTowerCurrentOrder)
    {
        if (m_stData.m_iClimbTowerCurrentOrder == iClimbTowerCurrentOrder)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iClimbTowerCurrentOrder - m_stData.m_iClimbTowerCurrentOrder;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetClimbTowerCurrentOrder(RealAdd);
        m_stData.m_iClimbTowerCurrentOrder = iClimbTowerCurrentOrder;
    }
    const int& GetClimbTowerMaxOrder() const
    {
        return m_stData.m_iClimbTowerMaxOrder;
    }
    void AddClimbTowerMaxOrder(int iClimbTowerMaxOrder)
    {
        if (0 == iClimbTowerMaxOrder)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetClimbTowerMaxOrder(iClimbTowerMaxOrder);
        m_stData.m_iClimbTowerMaxOrder += iClimbTowerMaxOrder;
    }
    void SetClimbTowerMaxOrder(int iClimbTowerMaxOrder)
    {
        if (m_stData.m_iClimbTowerMaxOrder == iClimbTowerMaxOrder)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iClimbTowerMaxOrder - m_stData.m_iClimbTowerMaxOrder;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetClimbTowerMaxOrder(RealAdd);
        m_stData.m_iClimbTowerMaxOrder = iClimbTowerMaxOrder;
    }
    const int& GetClimbTowerResetCount() const
    {
        return m_stData.m_iClimbTowerResetCount;
    }
    void AddClimbTowerResetCount(int iClimbTowerResetCount)
    {
        if (0 == iClimbTowerResetCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetClimbTowerResetCount(iClimbTowerResetCount);
        m_stData.m_iClimbTowerResetCount += iClimbTowerResetCount;
    }
    void SetClimbTowerResetCount(int iClimbTowerResetCount)
    {
        if (m_stData.m_iClimbTowerResetCount == iClimbTowerResetCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iClimbTowerResetCount - m_stData.m_iClimbTowerResetCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetClimbTowerResetCount(RealAdd);
        m_stData.m_iClimbTowerResetCount = iClimbTowerResetCount;
    }
    const int& GetLastClimbTowerResetTime() const
    {
        return m_stData.m_iLastClimbTowerResetTime;
    }
    void AddLastClimbTowerResetTime(int iLastClimbTowerResetTime)
    {
        if (0 == iLastClimbTowerResetTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastClimbTowerResetTime(iLastClimbTowerResetTime);
        m_stData.m_iLastClimbTowerResetTime += iLastClimbTowerResetTime;
    }
    void SetLastClimbTowerResetTime(int iLastClimbTowerResetTime)
    {
        if (m_stData.m_iLastClimbTowerResetTime == iLastClimbTowerResetTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastClimbTowerResetTime - m_stData.m_iLastClimbTowerResetTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastClimbTowerResetTime(RealAdd);
        m_stData.m_iLastClimbTowerResetTime = iLastClimbTowerResetTime;
    }
    const int& GetClimbTowerSweepEndTime() const
    {
        return m_stData.m_iClimbTowerSweepEndTime;
    }
    void AddClimbTowerSweepEndTime(int iClimbTowerSweepEndTime)
    {
        if (0 == iClimbTowerSweepEndTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetClimbTowerSweepEndTime(iClimbTowerSweepEndTime);
        m_stData.m_iClimbTowerSweepEndTime += iClimbTowerSweepEndTime;
    }
    void SetClimbTowerSweepEndTime(int iClimbTowerSweepEndTime)
    {
        if (m_stData.m_iClimbTowerSweepEndTime == iClimbTowerSweepEndTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iClimbTowerSweepEndTime - m_stData.m_iClimbTowerSweepEndTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetClimbTowerSweepEndTime(RealAdd);
        m_stData.m_iClimbTowerSweepEndTime = iClimbTowerSweepEndTime;
    }
    const int& GetClimbTowerHighestRank() const
    {
        return m_stData.m_iClimbTowerHighestRank;
    }
    void AddClimbTowerHighestRank(int iClimbTowerHighestRank)
    {
        if (0 == iClimbTowerHighestRank)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetClimbTowerHighestRank(iClimbTowerHighestRank);
        m_stData.m_iClimbTowerHighestRank += iClimbTowerHighestRank;
    }
    void SetClimbTowerHighestRank(int iClimbTowerHighestRank)
    {
        if (m_stData.m_iClimbTowerHighestRank == iClimbTowerHighestRank)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iClimbTowerHighestRank - m_stData.m_iClimbTowerHighestRank;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetClimbTowerHighestRank(RealAdd);
        m_stData.m_iClimbTowerHighestRank = iClimbTowerHighestRank;
    }
    const int& GetSerialPayAwarded() const
    {
        return m_stData.m_iSerialPayAwarded;
    }
    void AddSerialPayAwarded(int iSerialPayAwarded)
    {
        if (0 == iSerialPayAwarded)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSerialPayAwarded(iSerialPayAwarded);
        m_stData.m_iSerialPayAwarded += iSerialPayAwarded;
    }
    void SetSerialPayAwarded(int iSerialPayAwarded)
    {
        if (m_stData.m_iSerialPayAwarded == iSerialPayAwarded)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSerialPayAwarded - m_stData.m_iSerialPayAwarded;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSerialPayAwarded(RealAdd);
        m_stData.m_iSerialPayAwarded = iSerialPayAwarded;
    }
    const int& GetCornucopiaProfitYuanbao() const
    {
        return m_stData.m_iCornucopiaProfitYuanbao;
    }
    void AddCornucopiaProfitYuanbao(int iCornucopiaProfitYuanbao)
    {
        if (0 == iCornucopiaProfitYuanbao)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCornucopiaProfitYuanbao(iCornucopiaProfitYuanbao);
        m_stData.m_iCornucopiaProfitYuanbao += iCornucopiaProfitYuanbao;
    }
    void SetCornucopiaProfitYuanbao(int iCornucopiaProfitYuanbao)
    {
        if (m_stData.m_iCornucopiaProfitYuanbao == iCornucopiaProfitYuanbao)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCornucopiaProfitYuanbao - m_stData.m_iCornucopiaProfitYuanbao;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCornucopiaProfitYuanbao(RealAdd);
        m_stData.m_iCornucopiaProfitYuanbao = iCornucopiaProfitYuanbao;
    }
    const int& GetGuildCoin() const
    {
        return m_stData.m_iGuildCoin;
    }
    void AddGuildCoin(int iGuildCoin)
    {
        if (0 == iGuildCoin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildCoin(iGuildCoin);
        m_stData.m_iGuildCoin += iGuildCoin;
    }
    void SetGuildCoin(int iGuildCoin)
    {
        if (m_stData.m_iGuildCoin == iGuildCoin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGuildCoin - m_stData.m_iGuildCoin;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildCoin(RealAdd);
        m_stData.m_iGuildCoin = iGuildCoin;
    }
    const int& GetLastLeaveGuildTime() const
    {
        return m_stData.m_iLastLeaveGuildTime;
    }
    void AddLastLeaveGuildTime(int iLastLeaveGuildTime)
    {
        if (0 == iLastLeaveGuildTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastLeaveGuildTime(iLastLeaveGuildTime);
        m_stData.m_iLastLeaveGuildTime += iLastLeaveGuildTime;
    }
    void SetLastLeaveGuildTime(int iLastLeaveGuildTime)
    {
        if (m_stData.m_iLastLeaveGuildTime == iLastLeaveGuildTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastLeaveGuildTime - m_stData.m_iLastLeaveGuildTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastLeaveGuildTime(RealAdd);
        m_stData.m_iLastLeaveGuildTime = iLastLeaveGuildTime;
    }
    const int& GetJubaoAwardCount() const
    {
        return m_stData.m_iJubaoAwardCount;
    }
    void AddJubaoAwardCount(int iJubaoAwardCount)
    {
        if (0 == iJubaoAwardCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetJubaoAwardCount(iJubaoAwardCount);
        m_stData.m_iJubaoAwardCount += iJubaoAwardCount;
    }
    void SetJubaoAwardCount(int iJubaoAwardCount)
    {
        if (m_stData.m_iJubaoAwardCount == iJubaoAwardCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iJubaoAwardCount - m_stData.m_iJubaoAwardCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetJubaoAwardCount(RealAdd);
        m_stData.m_iJubaoAwardCount = iJubaoAwardCount;
    }
    const int& GetTodayJubaoAwardCount() const
    {
        return m_stData.m_iTodayJubaoAwardCount;
    }
    void AddTodayJubaoAwardCount(int iTodayJubaoAwardCount)
    {
        if (0 == iTodayJubaoAwardCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTodayJubaoAwardCount(iTodayJubaoAwardCount);
        m_stData.m_iTodayJubaoAwardCount += iTodayJubaoAwardCount;
    }
    void SetTodayJubaoAwardCount(int iTodayJubaoAwardCount)
    {
        if (m_stData.m_iTodayJubaoAwardCount == iTodayJubaoAwardCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTodayJubaoAwardCount - m_stData.m_iTodayJubaoAwardCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTodayJubaoAwardCount(RealAdd);
        m_stData.m_iTodayJubaoAwardCount = iTodayJubaoAwardCount;
    }
    const int& GetLastJubaoAwardTime() const
    {
        return m_stData.m_iLastJubaoAwardTime;
    }
    void AddLastJubaoAwardTime(int iLastJubaoAwardTime)
    {
        if (0 == iLastJubaoAwardTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastJubaoAwardTime(iLastJubaoAwardTime);
        m_stData.m_iLastJubaoAwardTime += iLastJubaoAwardTime;
    }
    void SetLastJubaoAwardTime(int iLastJubaoAwardTime)
    {
        if (m_stData.m_iLastJubaoAwardTime == iLastJubaoAwardTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastJubaoAwardTime - m_stData.m_iLastJubaoAwardTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastJubaoAwardTime(RealAdd);
        m_stData.m_iLastJubaoAwardTime = iLastJubaoAwardTime;
    }
    const int& GetWeiBuyNum() const
    {
        return m_stData.m_iWeiBuyNum;
    }
    void AddWeiBuyNum(int iWeiBuyNum)
    {
        if (0 == iWeiBuyNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWeiBuyNum(iWeiBuyNum);
        m_stData.m_iWeiBuyNum += iWeiBuyNum;
    }
    void SetWeiBuyNum(int iWeiBuyNum)
    {
        if (m_stData.m_iWeiBuyNum == iWeiBuyNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWeiBuyNum - m_stData.m_iWeiBuyNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWeiBuyNum(RealAdd);
        m_stData.m_iWeiBuyNum = iWeiBuyNum;
    }
    const int& GetWeiAvgPrice() const
    {
        return m_stData.m_iWeiAvgPrice;
    }
    void AddWeiAvgPrice(int iWeiAvgPrice)
    {
        if (0 == iWeiAvgPrice)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWeiAvgPrice(iWeiAvgPrice);
        m_stData.m_iWeiAvgPrice += iWeiAvgPrice;
    }
    void SetWeiAvgPrice(int iWeiAvgPrice)
    {
        if (m_stData.m_iWeiAvgPrice == iWeiAvgPrice)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWeiAvgPrice - m_stData.m_iWeiAvgPrice;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWeiAvgPrice(RealAdd);
        m_stData.m_iWeiAvgPrice = iWeiAvgPrice;
    }
    const int& GetShuBuyNum() const
    {
        return m_stData.m_iShuBuyNum;
    }
    void AddShuBuyNum(int iShuBuyNum)
    {
        if (0 == iShuBuyNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetShuBuyNum(iShuBuyNum);
        m_stData.m_iShuBuyNum += iShuBuyNum;
    }
    void SetShuBuyNum(int iShuBuyNum)
    {
        if (m_stData.m_iShuBuyNum == iShuBuyNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iShuBuyNum - m_stData.m_iShuBuyNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetShuBuyNum(RealAdd);
        m_stData.m_iShuBuyNum = iShuBuyNum;
    }
    const int& GetShuAvgPrice() const
    {
        return m_stData.m_iShuAvgPrice;
    }
    void AddShuAvgPrice(int iShuAvgPrice)
    {
        if (0 == iShuAvgPrice)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetShuAvgPrice(iShuAvgPrice);
        m_stData.m_iShuAvgPrice += iShuAvgPrice;
    }
    void SetShuAvgPrice(int iShuAvgPrice)
    {
        if (m_stData.m_iShuAvgPrice == iShuAvgPrice)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iShuAvgPrice - m_stData.m_iShuAvgPrice;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetShuAvgPrice(RealAdd);
        m_stData.m_iShuAvgPrice = iShuAvgPrice;
    }
    const int& GetWuBuyNum() const
    {
        return m_stData.m_iWuBuyNum;
    }
    void AddWuBuyNum(int iWuBuyNum)
    {
        if (0 == iWuBuyNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWuBuyNum(iWuBuyNum);
        m_stData.m_iWuBuyNum += iWuBuyNum;
    }
    void SetWuBuyNum(int iWuBuyNum)
    {
        if (m_stData.m_iWuBuyNum == iWuBuyNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWuBuyNum - m_stData.m_iWuBuyNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWuBuyNum(RealAdd);
        m_stData.m_iWuBuyNum = iWuBuyNum;
    }
    const int& GetWuAvgPrice() const
    {
        return m_stData.m_iWuAvgPrice;
    }
    void AddWuAvgPrice(int iWuAvgPrice)
    {
        if (0 == iWuAvgPrice)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWuAvgPrice(iWuAvgPrice);
        m_stData.m_iWuAvgPrice += iWuAvgPrice;
    }
    void SetWuAvgPrice(int iWuAvgPrice)
    {
        if (m_stData.m_iWuAvgPrice == iWuAvgPrice)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWuAvgPrice - m_stData.m_iWuAvgPrice;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWuAvgPrice(RealAdd);
        m_stData.m_iWuAvgPrice = iWuAvgPrice;
    }
    const int& GetEntireWarContribution() const
    {
        return m_stData.m_iEntireWarContribution;
    }
    void AddEntireWarContribution(int iEntireWarContribution)
    {
        if (0 == iEntireWarContribution)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarContribution(iEntireWarContribution);
        m_stData.m_iEntireWarContribution += iEntireWarContribution;
    }
    void SetEntireWarContribution(int iEntireWarContribution)
    {
        if (m_stData.m_iEntireWarContribution == iEntireWarContribution)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEntireWarContribution - m_stData.m_iEntireWarContribution;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarContribution(RealAdd);
        m_stData.m_iEntireWarContribution = iEntireWarContribution;
    }
    const int& GetEntireWarMilitarkLevel() const
    {
        return m_stData.m_iEntireWarMilitarkLevel;
    }
    void AddEntireWarMilitarkLevel(int iEntireWarMilitarkLevel)
    {
        if (0 == iEntireWarMilitarkLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarMilitarkLevel(iEntireWarMilitarkLevel);
        m_stData.m_iEntireWarMilitarkLevel += iEntireWarMilitarkLevel;
    }
    void SetEntireWarMilitarkLevel(int iEntireWarMilitarkLevel)
    {
        if (m_stData.m_iEntireWarMilitarkLevel == iEntireWarMilitarkLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEntireWarMilitarkLevel - m_stData.m_iEntireWarMilitarkLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarMilitarkLevel(RealAdd);
        m_stData.m_iEntireWarMilitarkLevel = iEntireWarMilitarkLevel;
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
    const int& GetLastEntireWarTime() const
    {
        return m_stData.m_iLastEntireWarTime;
    }
    void AddLastEntireWarTime(int iLastEntireWarTime)
    {
        if (0 == iLastEntireWarTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastEntireWarTime(iLastEntireWarTime);
        m_stData.m_iLastEntireWarTime += iLastEntireWarTime;
    }
    void SetLastEntireWarTime(int iLastEntireWarTime)
    {
        if (m_stData.m_iLastEntireWarTime == iLastEntireWarTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastEntireWarTime - m_stData.m_iLastEntireWarTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastEntireWarTime(RealAdd);
        m_stData.m_iLastEntireWarTime = iLastEntireWarTime;
    }
    const int& GetWeekWarContribution() const
    {
        return m_stData.m_iWeekWarContribution;
    }
    void AddWeekWarContribution(int iWeekWarContribution)
    {
        if (0 == iWeekWarContribution)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWeekWarContribution(iWeekWarContribution);
        m_stData.m_iWeekWarContribution += iWeekWarContribution;
    }
    void SetWeekWarContribution(int iWeekWarContribution)
    {
        if (m_stData.m_iWeekWarContribution == iWeekWarContribution)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWeekWarContribution - m_stData.m_iWeekWarContribution;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWeekWarContribution(RealAdd);
        m_stData.m_iWeekWarContribution = iWeekWarContribution;
    }
    const int& GetWeekLimitBossScore() const
    {
        return m_stData.m_iWeekLimitBossScore;
    }
    void AddWeekLimitBossScore(int iWeekLimitBossScore)
    {
        if (0 == iWeekLimitBossScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWeekLimitBossScore(iWeekLimitBossScore);
        m_stData.m_iWeekLimitBossScore += iWeekLimitBossScore;
    }
    void SetWeekLimitBossScore(int iWeekLimitBossScore)
    {
        if (m_stData.m_iWeekLimitBossScore == iWeekLimitBossScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWeekLimitBossScore - m_stData.m_iWeekLimitBossScore;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWeekLimitBossScore(RealAdd);
        m_stData.m_iWeekLimitBossScore = iWeekLimitBossScore;
    }
    const int& GetActionCoin() const
    {
        return m_stData.m_iActionCoin;
    }
    void AddActionCoin(int iActionCoin)
    {
        if (0 == iActionCoin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActionCoin(iActionCoin);
        m_stData.m_iActionCoin += iActionCoin;
    }
    void SetActionCoin(int iActionCoin)
    {
        if (m_stData.m_iActionCoin == iActionCoin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iActionCoin - m_stData.m_iActionCoin;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActionCoin(RealAdd);
        m_stData.m_iActionCoin = iActionCoin;
    }
    const int& GetLastAddActionTime() const
    {
        return m_stData.m_iLastAddActionTime;
    }
    void AddLastAddActionTime(int iLastAddActionTime)
    {
        if (0 == iLastAddActionTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastAddActionTime(iLastAddActionTime);
        m_stData.m_iLastAddActionTime += iLastAddActionTime;
    }
    void SetLastAddActionTime(int iLastAddActionTime)
    {
        if (m_stData.m_iLastAddActionTime == iLastAddActionTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastAddActionTime - m_stData.m_iLastAddActionTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastAddActionTime(RealAdd);
        m_stData.m_iLastAddActionTime = iLastAddActionTime;
    }
    const uint64_t& GetEntireWarAwardBitmap() const
    {
        return m_stData.m_ulEntireWarAwardBitmap;
    }
    void AddEntireWarAwardBitmap(uint64_t ulEntireWarAwardBitmap)
    {
        if (0 == ulEntireWarAwardBitmap)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarAwardBitmap(ulEntireWarAwardBitmap);
        m_stData.m_ulEntireWarAwardBitmap += ulEntireWarAwardBitmap;
    }
    void SetEntireWarAwardBitmap(uint64_t ulEntireWarAwardBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulEntireWarAwardBitmap > ulEntireWarAwardBitmap)
        {
            m_pRecord->SetEntireWarAwardBitmap(m_stData.m_ulEntireWarAwardBitmap - ulEntireWarAwardBitmap, '-');
        }
        else
        {
            m_pRecord->SetEntireWarAwardBitmap(ulEntireWarAwardBitmap - m_stData.m_ulEntireWarAwardBitmap, '+');
        }
        m_stData.m_ulEntireWarAwardBitmap = ulEntireWarAwardBitmap;
    }
    const CVecBitmap& GetEntireWarNodeAwardStat() const
    {
        return m_stData.m_stEntireWarNodeAwardStat;
    }
    void SetEntireWarNodeAwardStat(const CVecBitmap& stEntireWarNodeAwardStat)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarNodeAwardStat(stEntireWarNodeAwardStat);
        m_stData.m_stEntireWarNodeAwardStat = stEntireWarNodeAwardStat;
    }
    const CPackEntireWarExtra& GetEntireWarExtraStatus() const
    {
        return m_stData.m_stEntireWarExtraStatus;
    }
    void SetEntireWarExtraStatus(const CPackEntireWarExtra& stEntireWarExtraStatus)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarExtraStatus(stEntireWarExtraStatus);
        m_stData.m_stEntireWarExtraStatus = stEntireWarExtraStatus;
    }
    const std::map<int32_t, int32_t>& GetEntireWarNodePara() const
    {
        return m_stData.m_stEntireWarNodePara;
    }
    void SetEntireWarNodePara(const std::map<int32_t, int32_t>& stEntireWarNodePara)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEntireWarNodePara(stEntireWarNodePara);
        m_stData.m_stEntireWarNodePara = stEntireWarNodePara;
    }
    const int& GetDayQiyuID() const
    {
        return m_stData.m_iDayQiyuID;
    }
    void AddDayQiyuID(int iDayQiyuID)
    {
        if (0 == iDayQiyuID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDayQiyuID(iDayQiyuID);
        m_stData.m_iDayQiyuID += iDayQiyuID;
    }
    void SetDayQiyuID(int iDayQiyuID)
    {
        if (m_stData.m_iDayQiyuID == iDayQiyuID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDayQiyuID - m_stData.m_iDayQiyuID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDayQiyuID(RealAdd);
        m_stData.m_iDayQiyuID = iDayQiyuID;
    }
    const int& GetCangbaotuQiyuID() const
    {
        return m_stData.m_iCangbaotuQiyuID;
    }
    void AddCangbaotuQiyuID(int iCangbaotuQiyuID)
    {
        if (0 == iCangbaotuQiyuID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCangbaotuQiyuID(iCangbaotuQiyuID);
        m_stData.m_iCangbaotuQiyuID += iCangbaotuQiyuID;
    }
    void SetCangbaotuQiyuID(int iCangbaotuQiyuID)
    {
        if (m_stData.m_iCangbaotuQiyuID == iCangbaotuQiyuID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCangbaotuQiyuID - m_stData.m_iCangbaotuQiyuID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCangbaotuQiyuID(RealAdd);
        m_stData.m_iCangbaotuQiyuID = iCangbaotuQiyuID;
    }
    const int& GetQiyuEndtime() const
    {
        return m_stData.m_iQiyuEndtime;
    }
    void AddQiyuEndtime(int iQiyuEndtime)
    {
        if (0 == iQiyuEndtime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQiyuEndtime(iQiyuEndtime);
        m_stData.m_iQiyuEndtime += iQiyuEndtime;
    }
    void SetQiyuEndtime(int iQiyuEndtime)
    {
        if (m_stData.m_iQiyuEndtime == iQiyuEndtime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iQiyuEndtime - m_stData.m_iQiyuEndtime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQiyuEndtime(RealAdd);
        m_stData.m_iQiyuEndtime = iQiyuEndtime;
    }
    const int& GetCangbaotuEndtime() const
    {
        return m_stData.m_iCangbaotuEndtime;
    }
    void AddCangbaotuEndtime(int iCangbaotuEndtime)
    {
        if (0 == iCangbaotuEndtime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCangbaotuEndtime(iCangbaotuEndtime);
        m_stData.m_iCangbaotuEndtime += iCangbaotuEndtime;
    }
    void SetCangbaotuEndtime(int iCangbaotuEndtime)
    {
        if (m_stData.m_iCangbaotuEndtime == iCangbaotuEndtime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCangbaotuEndtime - m_stData.m_iCangbaotuEndtime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCangbaotuEndtime(RealAdd);
        m_stData.m_iCangbaotuEndtime = iCangbaotuEndtime;
    }
    const CPackEntireWarBuffVector& GetQiyuBuff() const
    {
        return m_stData.m_stQiyuBuff;
    }
    void SetQiyuBuff(const CPackEntireWarBuffVector& stQiyuBuff)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQiyuBuff(stQiyuBuff);
        m_stData.m_stQiyuBuff = stQiyuBuff;
    }
    const CPackEntireWarBuffVector& GetCangbaotuBuff() const
    {
        return m_stData.m_stCangbaotuBuff;
    }
    void SetCangbaotuBuff(const CPackEntireWarBuffVector& stCangbaotuBuff)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCangbaotuBuff(stCangbaotuBuff);
        m_stData.m_stCangbaotuBuff = stCangbaotuBuff;
    }
    const CPackCangbaotuSuipian& GetCangbaotuSuipian() const
    {
        return m_stData.m_stCangbaotuSuipian;
    }
    void SetCangbaotuSuipian(const CPackCangbaotuSuipian& stCangbaotuSuipian)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCangbaotuSuipian(stCangbaotuSuipian);
        m_stData.m_stCangbaotuSuipian = stCangbaotuSuipian;
    }
    const CPackBanquetTeamIds& GetBanquetAllTeamID() const
    {
        return m_stData.m_stBanquetAllTeamID;
    }
    void SetBanquetAllTeamID(const CPackBanquetTeamIds& stBanquetAllTeamID)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBanquetAllTeamID(stBanquetAllTeamID);
        m_stData.m_stBanquetAllTeamID = stBanquetAllTeamID;
    }
    const int& GetDataUpdateVersion() const
    {
        return m_stData.m_iDataUpdateVersion;
    }
    void AddDataUpdateVersion(int iDataUpdateVersion)
    {
        if (0 == iDataUpdateVersion)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDataUpdateVersion(iDataUpdateVersion);
        m_stData.m_iDataUpdateVersion += iDataUpdateVersion;
    }
    void SetDataUpdateVersion(int iDataUpdateVersion)
    {
        if (m_stData.m_iDataUpdateVersion == iDataUpdateVersion)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDataUpdateVersion - m_stData.m_iDataUpdateVersion;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDataUpdateVersion(RealAdd);
        m_stData.m_iDataUpdateVersion = iDataUpdateVersion;
    }
    const int& GetNewPassTotalExp() const
    {
        return m_stData.m_iNewPassTotalExp;
    }
    void AddNewPassTotalExp(int iNewPassTotalExp)
    {
        if (0 == iNewPassTotalExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewPassTotalExp(iNewPassTotalExp);
        m_stData.m_iNewPassTotalExp += iNewPassTotalExp;
    }
    void SetNewPassTotalExp(int iNewPassTotalExp)
    {
        if (m_stData.m_iNewPassTotalExp == iNewPassTotalExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNewPassTotalExp - m_stData.m_iNewPassTotalExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewPassTotalExp(RealAdd);
        m_stData.m_iNewPassTotalExp = iNewPassTotalExp;
    }
    const std::vector<int32_t>& GetNewPassNormalLevel() const
    {
        return m_stData.m_stNewPassNormalLevel;
    }
    void SetNewPassNormalLevel(const std::vector<int32_t>& stNewPassNormalLevel)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewPassNormalLevel(stNewPassNormalLevel);
        m_stData.m_stNewPassNormalLevel = stNewPassNormalLevel;
    }
    const std::vector<int32_t>& GetNewPassAdvancedLevel() const
    {
        return m_stData.m_stNewPassAdvancedLevel;
    }
    void SetNewPassAdvancedLevel(const std::vector<int32_t>& stNewPassAdvancedLevel)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewPassAdvancedLevel(stNewPassAdvancedLevel);
        m_stData.m_stNewPassAdvancedLevel = stNewPassAdvancedLevel;
    }
    const int& GetLimitBossTotalPoint() const
    {
        return m_stData.m_iLimitBossTotalPoint;
    }
    void AddLimitBossTotalPoint(int iLimitBossTotalPoint)
    {
        if (0 == iLimitBossTotalPoint)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBossTotalPoint(iLimitBossTotalPoint);
        m_stData.m_iLimitBossTotalPoint += iLimitBossTotalPoint;
    }
    void SetLimitBossTotalPoint(int iLimitBossTotalPoint)
    {
        if (m_stData.m_iLimitBossTotalPoint == iLimitBossTotalPoint)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLimitBossTotalPoint - m_stData.m_iLimitBossTotalPoint;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBossTotalPoint(RealAdd);
        m_stData.m_iLimitBossTotalPoint = iLimitBossTotalPoint;
    }
    const std::map<int32_t,int32_t>& GetFindLimitBoss() const
    {
        return m_stData.m_stFindLimitBoss;
    }
    void SetFindLimitBoss(const std::map<int32_t,int32_t>& stFindLimitBoss)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFindLimitBoss(stFindLimitBoss);
        m_stData.m_stFindLimitBoss = stFindLimitBoss;
    }
    const int& GetPvpScore() const
    {
        return m_stData.m_iPvpScore;
    }
    void AddPvpScore(int iPvpScore)
    {
        if (0 == iPvpScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpScore(iPvpScore);
        m_stData.m_iPvpScore += iPvpScore;
    }
    void SetPvpScore(int iPvpScore)
    {
        if (m_stData.m_iPvpScore == iPvpScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPvpScore - m_stData.m_iPvpScore;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpScore(RealAdd);
        m_stData.m_iPvpScore = iPvpScore;
    }
    const int& GetPvpHighestScore() const
    {
        return m_stData.m_iPvpHighestScore;
    }
    void AddPvpHighestScore(int iPvpHighestScore)
    {
        if (0 == iPvpHighestScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpHighestScore(iPvpHighestScore);
        m_stData.m_iPvpHighestScore += iPvpHighestScore;
    }
    void SetPvpHighestScore(int iPvpHighestScore)
    {
        if (m_stData.m_iPvpHighestScore == iPvpHighestScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPvpHighestScore - m_stData.m_iPvpHighestScore;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpHighestScore(RealAdd);
        m_stData.m_iPvpHighestScore = iPvpHighestScore;
    }
    const int& GetPvpRefreshCount() const
    {
        return m_stData.m_iPvpRefreshCount;
    }
    void AddPvpRefreshCount(int iPvpRefreshCount)
    {
        if (0 == iPvpRefreshCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpRefreshCount(iPvpRefreshCount);
        m_stData.m_iPvpRefreshCount += iPvpRefreshCount;
    }
    void SetPvpRefreshCount(int iPvpRefreshCount)
    {
        if (m_stData.m_iPvpRefreshCount == iPvpRefreshCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPvpRefreshCount - m_stData.m_iPvpRefreshCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpRefreshCount(RealAdd);
        m_stData.m_iPvpRefreshCount = iPvpRefreshCount;
    }
    const std::vector<int32_t>& GetPvpWeeklyTask() const
    {
        return m_stData.m_stPvpWeeklyTask;
    }
    void SetPvpWeeklyTask(const std::vector<int32_t>& stPvpWeeklyTask)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPvpWeeklyTask(stPvpWeeklyTask);
        m_stData.m_stPvpWeeklyTask = stPvpWeeklyTask;
    }
    const int& GetHomeExp() const
    {
        return m_stData.m_iHomeExp;
    }
    void AddHomeExp(int iHomeExp)
    {
        if (0 == iHomeExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHomeExp(iHomeExp);
        m_stData.m_iHomeExp += iHomeExp;
    }
    void SetHomeExp(int iHomeExp)
    {
        if (m_stData.m_iHomeExp == iHomeExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iHomeExp - m_stData.m_iHomeExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHomeExp(RealAdd);
        m_stData.m_iHomeExp = iHomeExp;
    }
    const std::vector<int32_t>& GetHomeExpReward() const
    {
        return m_stData.m_stHomeExpReward;
    }
    void SetHomeExpReward(const std::vector<int32_t>& stHomeExpReward)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHomeExpReward(stHomeExpReward);
        m_stData.m_stHomeExpReward = stHomeExpReward;
    }
    const char& GetMonthAgeLimit() const
    {
        return m_stData.m_cMonthAgeLimit;
    }
    void AddMonthAgeLimit(char cMonthAgeLimit)
    {
        if (0 == cMonthAgeLimit)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthAgeLimit(cMonthAgeLimit);
        m_stData.m_cMonthAgeLimit += cMonthAgeLimit;
    }
    void SetMonthAgeLimit(char cMonthAgeLimit)
    {
        if (m_stData.m_cMonthAgeLimit == cMonthAgeLimit)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cMonthAgeLimit - m_stData.m_cMonthAgeLimit;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthAgeLimit(RealAdd);
        m_stData.m_cMonthAgeLimit = cMonthAgeLimit;
    }
    const int& GetLastMonthAgeChange() const
    {
        return m_stData.m_iLastMonthAgeChange;
    }
    void AddLastMonthAgeChange(int iLastMonthAgeChange)
    {
        if (0 == iLastMonthAgeChange)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastMonthAgeChange(iLastMonthAgeChange);
        m_stData.m_iLastMonthAgeChange += iLastMonthAgeChange;
    }
    void SetLastMonthAgeChange(int iLastMonthAgeChange)
    {
        if (m_stData.m_iLastMonthAgeChange == iLastMonthAgeChange)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastMonthAgeChange - m_stData.m_iLastMonthAgeChange;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastMonthAgeChange(RealAdd);
        m_stData.m_iLastMonthAgeChange = iLastMonthAgeChange;
    }
    const int& GetLastLimitCommodityID() const
    {
        return m_stData.m_iLastLimitCommodityID;
    }
    void AddLastLimitCommodityID(int iLastLimitCommodityID)
    {
        if (0 == iLastLimitCommodityID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastLimitCommodityID(iLastLimitCommodityID);
        m_stData.m_iLastLimitCommodityID += iLastLimitCommodityID;
    }
    void SetLastLimitCommodityID(int iLastLimitCommodityID)
    {
        if (m_stData.m_iLastLimitCommodityID == iLastLimitCommodityID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastLimitCommodityID - m_stData.m_iLastLimitCommodityID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastLimitCommodityID(RealAdd);
        m_stData.m_iLastLimitCommodityID = iLastLimitCommodityID;
    }
    const char& GetHotSpringRewardEmail() const
    {
        return m_stData.m_cHotSpringRewardEmail;
    }
    void AddHotSpringRewardEmail(char cHotSpringRewardEmail)
    {
        if (0 == cHotSpringRewardEmail)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHotSpringRewardEmail(cHotSpringRewardEmail);
        m_stData.m_cHotSpringRewardEmail += cHotSpringRewardEmail;
    }
    void SetHotSpringRewardEmail(char cHotSpringRewardEmail)
    {
        if (m_stData.m_cHotSpringRewardEmail == cHotSpringRewardEmail)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cHotSpringRewardEmail - m_stData.m_cHotSpringRewardEmail;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHotSpringRewardEmail(RealAdd);
        m_stData.m_cHotSpringRewardEmail = cHotSpringRewardEmail;
    }
    const char& GetHotSpringRewardOther() const
    {
        return m_stData.m_cHotSpringRewardOther;
    }
    void AddHotSpringRewardOther(char cHotSpringRewardOther)
    {
        if (0 == cHotSpringRewardOther)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHotSpringRewardOther(cHotSpringRewardOther);
        m_stData.m_cHotSpringRewardOther += cHotSpringRewardOther;
    }
    void SetHotSpringRewardOther(char cHotSpringRewardOther)
    {
        if (m_stData.m_cHotSpringRewardOther == cHotSpringRewardOther)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cHotSpringRewardOther - m_stData.m_cHotSpringRewardOther;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHotSpringRewardOther(RealAdd);
        m_stData.m_cHotSpringRewardOther = cHotSpringRewardOther;
    }
    const char& GetHotSpringReward() const
    {
        return m_stData.m_cHotSpringReward;
    }
    void AddHotSpringReward(char cHotSpringReward)
    {
        if (0 == cHotSpringReward)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHotSpringReward(cHotSpringReward);
        m_stData.m_cHotSpringReward += cHotSpringReward;
    }
    void SetHotSpringReward(char cHotSpringReward)
    {
        if (m_stData.m_cHotSpringReward == cHotSpringReward)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cHotSpringReward - m_stData.m_cHotSpringReward;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHotSpringReward(RealAdd);
        m_stData.m_cHotSpringReward = cHotSpringReward;
    }
    const char& GetHotSpringInvite() const
    {
        return m_stData.m_cHotSpringInvite;
    }
    void AddHotSpringInvite(char cHotSpringInvite)
    {
        if (0 == cHotSpringInvite)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHotSpringInvite(cHotSpringInvite);
        m_stData.m_cHotSpringInvite += cHotSpringInvite;
    }
    void SetHotSpringInvite(char cHotSpringInvite)
    {
        if (m_stData.m_cHotSpringInvite == cHotSpringInvite)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cHotSpringInvite - m_stData.m_cHotSpringInvite;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHotSpringInvite(RealAdd);
        m_stData.m_cHotSpringInvite = cHotSpringInvite;
    }
    const char& GetHotSpringJoin() const
    {
        return m_stData.m_cHotSpringJoin;
    }
    void AddHotSpringJoin(char cHotSpringJoin)
    {
        if (0 == cHotSpringJoin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHotSpringJoin(cHotSpringJoin);
        m_stData.m_cHotSpringJoin += cHotSpringJoin;
    }
    void SetHotSpringJoin(char cHotSpringJoin)
    {
        if (m_stData.m_cHotSpringJoin == cHotSpringJoin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cHotSpringJoin - m_stData.m_cHotSpringJoin;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHotSpringJoin(RealAdd);
        m_stData.m_cHotSpringJoin = cHotSpringJoin;
    }
    const char& GetIsBindEmail() const
    {
        return m_stData.m_cIsBindEmail;
    }
    void AddIsBindEmail(char cIsBindEmail)
    {
        if (0 == cIsBindEmail)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsBindEmail(cIsBindEmail);
        m_stData.m_cIsBindEmail += cIsBindEmail;
    }
    void SetIsBindEmail(char cIsBindEmail)
    {
        if (m_stData.m_cIsBindEmail == cIsBindEmail)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cIsBindEmail - m_stData.m_cIsBindEmail;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsBindEmail(RealAdd);
        m_stData.m_cIsBindEmail = cIsBindEmail;
    }
    const char& GetDailyGuildDonate() const
    {
        return m_stData.m_cDailyGuildDonate;
    }
    void AddDailyGuildDonate(char cDailyGuildDonate)
    {
        if (0 == cDailyGuildDonate)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildDonate(cDailyGuildDonate);
        m_stData.m_cDailyGuildDonate += cDailyGuildDonate;
    }
    void SetDailyGuildDonate(char cDailyGuildDonate)
    {
        if (m_stData.m_cDailyGuildDonate == cDailyGuildDonate)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cDailyGuildDonate - m_stData.m_cDailyGuildDonate;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildDonate(RealAdd);
        m_stData.m_cDailyGuildDonate = cDailyGuildDonate;
    }
    const int& GetGuildPersonDonate() const
    {
        return m_stData.m_iGuildPersonDonate;
    }
    void AddGuildPersonDonate(int iGuildPersonDonate)
    {
        if (0 == iGuildPersonDonate)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildPersonDonate(iGuildPersonDonate);
        m_stData.m_iGuildPersonDonate += iGuildPersonDonate;
    }
    void SetGuildPersonDonate(int iGuildPersonDonate)
    {
        if (m_stData.m_iGuildPersonDonate == iGuildPersonDonate)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGuildPersonDonate - m_stData.m_iGuildPersonDonate;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildPersonDonate(RealAdd);
        m_stData.m_iGuildPersonDonate = iGuildPersonDonate;
    }
    const int& GetGuildExtraLevel() const
    {
        return m_stData.m_iGuildExtraLevel;
    }
    void AddGuildExtraLevel(int iGuildExtraLevel)
    {
        if (0 == iGuildExtraLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildExtraLevel(iGuildExtraLevel);
        m_stData.m_iGuildExtraLevel += iGuildExtraLevel;
    }
    void SetGuildExtraLevel(int iGuildExtraLevel)
    {
        if (m_stData.m_iGuildExtraLevel == iGuildExtraLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGuildExtraLevel - m_stData.m_iGuildExtraLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildExtraLevel(RealAdd);
        m_stData.m_iGuildExtraLevel = iGuildExtraLevel;
    }
    const char& GetDailySignFlag() const
    {
        return m_stData.m_cDailySignFlag;
    }
    void AddDailySignFlag(char cDailySignFlag)
    {
        if (0 == cDailySignFlag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailySignFlag(cDailySignFlag);
        m_stData.m_cDailySignFlag += cDailySignFlag;
    }
    void SetDailySignFlag(char cDailySignFlag)
    {
        if (m_stData.m_cDailySignFlag == cDailySignFlag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cDailySignFlag - m_stData.m_cDailySignFlag;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailySignFlag(RealAdd);
        m_stData.m_cDailySignFlag = cDailySignFlag;
    }
    const CPackGameItem& GetDailySignReward() const
    {
        return m_stData.m_stDailySignReward;
    }
    void SetDailySignReward(const CPackGameItem& stDailySignReward)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailySignReward(stDailySignReward);
        m_stData.m_stDailySignReward = stDailySignReward;
    }
    const CPackGameItem& GetCompleteSignReward() const
    {
        return m_stData.m_stCompleteSignReward;
    }
    void SetCompleteSignReward(const CPackGameItem& stCompleteSignReward)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCompleteSignReward(stCompleteSignReward);
        m_stData.m_stCompleteSignReward = stCompleteSignReward;
    }
    const int& GetZhuhaiScore() const
    {
        return m_stData.m_iZhuhaiScore;
    }
    void AddZhuhaiScore(int iZhuhaiScore)
    {
        if (0 == iZhuhaiScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuhaiScore(iZhuhaiScore);
        m_stData.m_iZhuhaiScore += iZhuhaiScore;
    }
    void SetZhuhaiScore(int iZhuhaiScore)
    {
        if (m_stData.m_iZhuhaiScore == iZhuhaiScore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iZhuhaiScore - m_stData.m_iZhuhaiScore;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuhaiScore(RealAdd);
        m_stData.m_iZhuhaiScore = iZhuhaiScore;
    }

    CRecordUserInfoTable* m_pRecord;
    TUserInfoTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserInfoTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserInfoTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserInfoTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
