#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserIbusyInfoTable;
struct TUserIbusyInfoTableValueType;

class CRecordUserIBusyInfoTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserIBusyInfoTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserIBusyInfoTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();

    static std::vector<TUserIbusyInfoTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strActivityID;
    static const char* s_strLevel;
    static const char* s_strExp;
    static const char* s_strDailyExpAward;
    static const char* s_strScore;
    static const char* s_strExploreExp;
    static const char* s_strTodayHuoyue;
    static const char* s_strChangeTaskCount;
    static const char* s_strFreeChangeTaskCount;
    static const char* s_strMarketLastRefreshTime;
    static const char* s_strMarketRefreshTimes;
    static const char* s_strGettedLevelReward;
    static const char* s_strGettedDepositReward;
    static const char* s_strTaskInfo;
    static const char* s_strSpecialGiftWeekID;
    static const char* s_strSpecialGiftWeekLevel;
    static const char* s_strSpecialGiftMonthID;
    static const char* s_strSpecialGiftMonthLevel;
    static const char* s_strLuckyHammerActivityID;
    static const char* s_strLuckyHammerLotteryCount;
    static const char* s_strMonthPassExp;
    static const char* s_strMonthPassDay;
    static const char* s_strMonthPassRecharge;
    static const char* s_strMonthPassAwardBitmap;
    static const char* s_strMonthPassDepositAwardBitmap;
    static const char* s_strMonthPassWishEventID;
    static const char* s_strMonthPassWishItemIndex;
    static const char* s_strCardDialogLimitAvyStat;
    static const char* s_strCardDialogNextTime;
    static const char* s_strCardDialogTimes;
    static const char* s_strNewsLastTime;
    static const char* s_strNewsLastData;
    static const char* s_strAddupDepositBonus;
    static const char* s_strFoundationOpenTime;
    static const char* s_strOnceEventStat;
    static const char* s_strRandomStoreRefreshData;
    static const char* s_strSimpleGamePoint;
    static const char* s_strHeartInsureOpenTime;
    static const char* s_strMoonMapSkillLevel;
    static const char* s_strRedBagUniqueIndex;
    static const char* s_strWorldQuestRewardState;
    static const char* s_strWorldQuestRewardTimes;
    static const char* s_strQuestionnaireState;
    static const char* s_strQuestionnaireDurationTime;
    static const char* s_strLimitBossAccepted;
    static const char* s_strLimitBossRand;
    static const char* s_strLimitBoxTime;
    static const char* s_strTotalHeartLevel;
    static const char* s_strTotalHeartExp;
    static const char* s_strConstelExpPool;
    static const char* s_strFishingRod;
    static const char* s_strMainBgm;
    static const char* s_strHomeBgm;
    static const char* s_strMallBaodiTimes;
    static const char* s_strZhuHaiLevel;
    static const char* s_strZhuHaiGress;
    static const char* s_strZhuHaiBuff;
    static const char* s_strZhuHaiCardHp;
    static const char* s_strZhuHaiBlessGroup;
    static const char* s_strZhuHaiRoleBuff;
    static const char* s_strZhuHaiRefreshStore;
    static const char* s_strZhuHaiSaveTeamFlag;
    static const char* s_strGuildBossHurt;
    static const char* s_strWishLastTime;
    static const char* s_strWishCardID;
    static const char* s_strWishNum;
    static const char* s_strWishRewardVec;
    static const char* s_strWishSwapCard;
    static const char* s_strBadgeList;
    static const char* s_strNewLimitBossReward;
    static const char* s_strNewLimitBossDailyCount;
    static const char* s_strDailyGuildDonate;
    static const char* s_strDailyGuildSign;
    static const char* s_strDailyGuildRandomCount;
    static const char* s_strDailyGuildRandom;
    static const char* s_strDailyGuildBar;
    static const char* s_strWenwenChoose;
    static const char* s_strConstelAdvTimes;
    static const char* s_strGuildFragmentHistory;
    static const char* s_strHomeExpTag;
    static const char* s_strRuneInfo;
    static const char* s_strConstelPutInfo;
    static const char* s_strDataChangeTag;
    static const char* s_strCbtRewardTag;
    static const char* s_strConstelShop;
    static const char* s_strConstelShopRefreshCount;
    static const char* s_strConstelShopLastTime;
    static const char* s_strCheatingCount;
    static const char* s_strGashaponInfo;
    static const char* s_strCrushBossLevel;
    static const char* s_strCrushBossInfo;
    static const char* s_strCrushBossCount;
    static const char* s_strHomeBuffID;
    static const char* s_strStartLevelTime;
    static const char* s_strAtlasBuffInfo;
    static const char* s_strDailyGuildWarTime;
    static const char* s_strGuildWarUseCard;
    static const char* s_strGuildSelfInfo;
    static const char* s_strGuildWarCalInfo;
    static const char* s_strLimitBossBaodiTime;
    static const char* s_strUid;
    static const char* s_strGroupID;

    TUserIbusyInfoTable* m_pCache;

    void SetActivityID(int iActivityID)
    {
        m_stIntData[s_strActivityID] += iActivityID;
    }
    void SetLevel(int iLevel)
    {
        m_stIntData[s_strLevel] += iLevel;
    }
    void SetExp(int iExp)
    {
        m_stIntData[s_strExp] += iExp;
    }
    void SetDailyExpAward(int iDailyExpAward)
    {
        m_stIntData[s_strDailyExpAward] += iDailyExpAward;
    }
    void SetScore(int iScore)
    {
        m_stIntData[s_strScore] += iScore;
    }
    void SetExploreExp(int iExploreExp)
    {
        m_stIntData[s_strExploreExp] += iExploreExp;
    }
    void SetTodayHuoyue(int iTodayHuoyue)
    {
        m_stIntData[s_strTodayHuoyue] += iTodayHuoyue;
    }
    void SetChangeTaskCount(int iChangeTaskCount)
    {
        m_stIntData[s_strChangeTaskCount] += iChangeTaskCount;
    }
    void SetFreeChangeTaskCount(int iFreeChangeTaskCount)
    {
        m_stIntData[s_strFreeChangeTaskCount] += iFreeChangeTaskCount;
    }
    void SetMarketLastRefreshTime(int iMarketLastRefreshTime)
    {
        m_stIntData[s_strMarketLastRefreshTime] += iMarketLastRefreshTime;
    }
    void SetMarketRefreshTimes(int iMarketRefreshTimes)
    {
        m_stIntData[s_strMarketRefreshTimes] += iMarketRefreshTimes;
    }
    void SetGettedLevelReward(const std::set<int>& stGettedLevelReward)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGettedLevelReward);
        m_stVarData[s_strGettedLevelReward] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetGettedDepositReward(const std::set<int>& stGettedDepositReward)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGettedDepositReward);
        m_stVarData[s_strGettedDepositReward] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTaskInfo(const CPackIbusyTaskData& stTaskInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTaskInfo);
        m_stVarData[s_strTaskInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetSpecialGiftWeekID(int iSpecialGiftWeekID)
    {
        m_stIntData[s_strSpecialGiftWeekID] += iSpecialGiftWeekID;
    }
    void SetSpecialGiftWeekLevel(int iSpecialGiftWeekLevel)
    {
        m_stIntData[s_strSpecialGiftWeekLevel] += iSpecialGiftWeekLevel;
    }
    void SetSpecialGiftMonthID(int iSpecialGiftMonthID)
    {
        m_stIntData[s_strSpecialGiftMonthID] += iSpecialGiftMonthID;
    }
    void SetSpecialGiftMonthLevel(int iSpecialGiftMonthLevel)
    {
        m_stIntData[s_strSpecialGiftMonthLevel] += iSpecialGiftMonthLevel;
    }
    void SetLuckyHammerActivityID(int iLuckyHammerActivityID)
    {
        m_stIntData[s_strLuckyHammerActivityID] += iLuckyHammerActivityID;
    }
    void SetLuckyHammerLotteryCount(int iLuckyHammerLotteryCount)
    {
        m_stIntData[s_strLuckyHammerLotteryCount] += iLuckyHammerLotteryCount;
    }
    void SetMonthPassExp(int iMonthPassExp)
    {
        m_stIntData[s_strMonthPassExp] += iMonthPassExp;
    }
    void SetMonthPassDay(int iMonthPassDay)
    {
        m_stIntData[s_strMonthPassDay] += iMonthPassDay;
    }
    void SetMonthPassRecharge(int iMonthPassRecharge)
    {
        m_stIntData[s_strMonthPassRecharge] += iMonthPassRecharge;
    }
    void SetMonthPassAwardBitmap(const std::string& strMonthPassAwardBitmap)
    {
        m_stStringData[s_strMonthPassAwardBitmap] = strMonthPassAwardBitmap;
    }
    void SetMonthPassDepositAwardBitmap(const std::string& strMonthPassDepositAwardBitmap)
    {
        m_stStringData[s_strMonthPassDepositAwardBitmap] = strMonthPassDepositAwardBitmap;
    }
    void SetMonthPassWishEventID(int iMonthPassWishEventID)
    {
        m_stIntData[s_strMonthPassWishEventID] += iMonthPassWishEventID;
    }
    void SetMonthPassWishItemIndex(const std::vector<int>& stMonthPassWishItemIndex)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stMonthPassWishItemIndex);
        m_stVarData[s_strMonthPassWishItemIndex] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetCardDialogLimitAvyStat(const std::vector<int32_t>& stCardDialogLimitAvyStat)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stCardDialogLimitAvyStat);
        m_stVarData[s_strCardDialogLimitAvyStat] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetCardDialogNextTime(int iCardDialogNextTime)
    {
        m_stIntData[s_strCardDialogNextTime] += iCardDialogNextTime;
    }
    void SetCardDialogTimes(int iCardDialogTimes)
    {
        m_stIntData[s_strCardDialogTimes] += iCardDialogTimes;
    }
    void SetNewsLastTime(int iNewsLastTime)
    {
        m_stIntData[s_strNewsLastTime] += iNewsLastTime;
    }
    void SetNewsLastData(int iNewsLastData)
    {
        m_stIntData[s_strNewsLastData] += iNewsLastData;
    }
    void SetAddupDepositBonus(int iAddupDepositBonus)
    {
        m_stIntData[s_strAddupDepositBonus] += iAddupDepositBonus;
    }
    void SetFoundationOpenTime(int iFoundationOpenTime)
    {
        m_stIntData[s_strFoundationOpenTime] += iFoundationOpenTime;
    }
    void SetOnceEventStat(uint64_t ulOnceEventStat, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strOnceEventStat];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulOnceEventStat;
        }
        else
        {
            if (stDataRef.m_ulVal > ulOnceEventStat)
            {
                stDataRef.m_ulVal -= ulOnceEventStat;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulOnceEventStat - stDataRef.m_ulVal;
            }
        }
    }
    void SetRandomStoreRefreshData(const std::map<int32_t, std::pair<int32_t, int32_t>>& stRandomStoreRefreshData)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stRandomStoreRefreshData);
        m_stVarData[s_strRandomStoreRefreshData] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetSimpleGamePoint(const std::map<uint8_t, int32_t>& stSimpleGamePoint)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stSimpleGamePoint);
        m_stVarData[s_strSimpleGamePoint] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetHeartInsureOpenTime(int iHeartInsureOpenTime)
    {
        m_stIntData[s_strHeartInsureOpenTime] += iHeartInsureOpenTime;
    }
    void SetMoonMapSkillLevel(const std::map<int32_t, int32_t>& stMoonMapSkillLevel)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stMoonMapSkillLevel);
        m_stVarData[s_strMoonMapSkillLevel] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetRedBagUniqueIndex(const CVecBitmap& stRedBagUniqueIndex)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stRedBagUniqueIndex);
        m_stVarData[s_strRedBagUniqueIndex] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetWorldQuestRewardState(const CVecBitmap& stWorldQuestRewardState)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stWorldQuestRewardState);
        m_stVarData[s_strWorldQuestRewardState] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetWorldQuestRewardTimes(int iWorldQuestRewardTimes)
    {
        m_stIntData[s_strWorldQuestRewardTimes] += iWorldQuestRewardTimes;
    }
    void SetQuestionnaireState(const CVecBitmap& stQuestionnaireState)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stQuestionnaireState);
        m_stVarData[s_strQuestionnaireState] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetQuestionnaireDurationTime(const std::map<int32_t, int32_t>& stQuestionnaireDurationTime)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stQuestionnaireDurationTime);
        m_stVarData[s_strQuestionnaireDurationTime] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetLimitBossAccepted(const std::map<int32_t, int32_t>& stLimitBossAccepted)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stLimitBossAccepted);
        m_stVarData[s_strLimitBossAccepted] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetLimitBossRand(const std::map<int32_t, int32_t>& stLimitBossRand)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stLimitBossRand);
        m_stVarData[s_strLimitBossRand] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetLimitBoxTime(int iLimitBoxTime)
    {
        m_stIntData[s_strLimitBoxTime] += iLimitBoxTime;
    }
    void SetTotalHeartLevel(int iTotalHeartLevel)
    {
        m_stIntData[s_strTotalHeartLevel] += iTotalHeartLevel;
    }
    void SetTotalHeartExp(int iTotalHeartExp)
    {
        m_stIntData[s_strTotalHeartExp] += iTotalHeartExp;
    }
    void SetConstelExpPool(int iConstelExpPool)
    {
        m_stIntData[s_strConstelExpPool] += iConstelExpPool;
    }
    void SetFishingRod(int iFishingRod)
    {
        m_stIntData[s_strFishingRod] += iFishingRod;
    }
    void SetMainBgm(const std::list<int32_t>& stMainBgm)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stMainBgm);
        m_stVarData[s_strMainBgm] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetHomeBgm(const std::list<int32_t>& stHomeBgm)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stHomeBgm);
        m_stVarData[s_strHomeBgm] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetMallBaodiTimes(int iMallBaodiTimes)
    {
        m_stIntData[s_strMallBaodiTimes] += iMallBaodiTimes;
    }
    void SetZhuHaiLevel(int iZhuHaiLevel)
    {
        m_stIntData[s_strZhuHaiLevel] += iZhuHaiLevel;
    }
    void SetZhuHaiGress(int iZhuHaiGress)
    {
        m_stIntData[s_strZhuHaiGress] += iZhuHaiGress;
    }
    void SetZhuHaiBuff(const std::vector<int>& stZhuHaiBuff)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stZhuHaiBuff);
        m_stVarData[s_strZhuHaiBuff] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetZhuHaiCardHp(const std::map<int, int>& stZhuHaiCardHp)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stZhuHaiCardHp);
        m_stVarData[s_strZhuHaiCardHp] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetZhuHaiBlessGroup(const std::map<int, CPackZhuHaiBlessInfo>& stZhuHaiBlessGroup)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stZhuHaiBlessGroup);
        m_stVarData[s_strZhuHaiBlessGroup] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetZhuHaiRoleBuff(const std::map<int,std::vector<int>>& stZhuHaiRoleBuff)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stZhuHaiRoleBuff);
        m_stVarData[s_strZhuHaiRoleBuff] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetZhuHaiRefreshStore(int iZhuHaiRefreshStore)
    {
        m_stIntData[s_strZhuHaiRefreshStore] += iZhuHaiRefreshStore;
    }
    void SetZhuHaiSaveTeamFlag(char cZhuHaiSaveTeamFlag)
    {
        m_stCharData[s_strZhuHaiSaveTeamFlag] += cZhuHaiSaveTeamFlag;
    }
    void SetGuildBossHurt(const std::map<int, uint64_t>& stGuildBossHurt)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildBossHurt);
        m_stVarData[s_strGuildBossHurt] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetWishLastTime(int iWishLastTime)
    {
        m_stIntData[s_strWishLastTime] += iWishLastTime;
    }
    void SetWishCardID(int iWishCardID)
    {
        m_stIntData[s_strWishCardID] += iWishCardID;
    }
    void SetWishNum(int iWishNum)
    {
        m_stIntData[s_strWishNum] += iWishNum;
    }
    void SetWishRewardVec(const std::vector<std::pair<std::string, int>>& stWishRewardVec)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stWishRewardVec);
        m_stVarData[s_strWishRewardVec] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetWishSwapCard(const std::vector<int>& stWishSwapCard)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stWishSwapCard);
        m_stVarData[s_strWishSwapCard] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetBadgeList(const std::vector<int>& stBadgeList)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stBadgeList);
        m_stVarData[s_strBadgeList] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetNewLimitBossReward(const std::map<int32_t,std::vector<CPackLimitBossTeamInfo>>& stNewLimitBossReward)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stNewLimitBossReward);
        m_stVarData[s_strNewLimitBossReward] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetNewLimitBossDailyCount(int iNewLimitBossDailyCount)
    {
        m_stIntData[s_strNewLimitBossDailyCount] += iNewLimitBossDailyCount;
    }
    void SetDailyGuildDonate(char cDailyGuildDonate)
    {
        m_stCharData[s_strDailyGuildDonate] += cDailyGuildDonate;
    }
    void SetDailyGuildSign(char cDailyGuildSign)
    {
        m_stCharData[s_strDailyGuildSign] += cDailyGuildSign;
    }
    void SetDailyGuildRandomCount(int iDailyGuildRandomCount)
    {
        m_stIntData[s_strDailyGuildRandomCount] += iDailyGuildRandomCount;
    }
    void SetDailyGuildRandom(const CPackGuildRandomInfo& stDailyGuildRandom)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stDailyGuildRandom);
        m_stVarData[s_strDailyGuildRandom] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetDailyGuildBar(char cDailyGuildBar)
    {
        m_stCharData[s_strDailyGuildBar] += cDailyGuildBar;
    }
    void SetWenwenChoose(char cWenwenChoose)
    {
        m_stCharData[s_strWenwenChoose] += cWenwenChoose;
    }
    void SetConstelAdvTimes(int iConstelAdvTimes)
    {
        m_stIntData[s_strConstelAdvTimes] += iConstelAdvTimes;
    }
    void SetGuildFragmentHistory(const std::vector<CPackGuildFragmentsHistory>& stGuildFragmentHistory)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildFragmentHistory);
        m_stVarData[s_strGuildFragmentHistory] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetHomeExpTag(const std::map<int,bool>& stHomeExpTag)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stHomeExpTag);
        m_stVarData[s_strHomeExpTag] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetRuneInfo(const std::map<int,CPackRuneInfo>& stRuneInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stRuneInfo);
        m_stVarData[s_strRuneInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetConstelPutInfo(const std::map<int,map<int,char>>& stConstelPutInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stConstelPutInfo);
        m_stVarData[s_strConstelPutInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetDataChangeTag(char cDataChangeTag)
    {
        m_stCharData[s_strDataChangeTag] += cDataChangeTag;
    }
    void SetCbtRewardTag(char cCbtRewardTag)
    {
        m_stCharData[s_strCbtRewardTag] += cCbtRewardTag;
    }
    void SetConstelShop(const std::vector<CPackConstelShopInfo>& stConstelShop)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stConstelShop);
        m_stVarData[s_strConstelShop] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetConstelShopRefreshCount(int iConstelShopRefreshCount)
    {
        m_stIntData[s_strConstelShopRefreshCount] += iConstelShopRefreshCount;
    }
    void SetConstelShopLastTime(int iConstelShopLastTime)
    {
        m_stIntData[s_strConstelShopLastTime] += iConstelShopLastTime;
    }
    void SetCheatingCount(int iCheatingCount)
    {
        m_stIntData[s_strCheatingCount] += iCheatingCount;
    }
    void SetGashaponInfo(const std::map<int32_t,std::map<int32_t,CPackGashaponInfo>>& stGashaponInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGashaponInfo);
        m_stVarData[s_strGashaponInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetCrushBossLevel(int iCrushBossLevel)
    {
        m_stIntData[s_strCrushBossLevel] += iCrushBossLevel;
    }
    void SetCrushBossInfo(const CPackCrushBossInfo& stCrushBossInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stCrushBossInfo);
        m_stVarData[s_strCrushBossInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetCrushBossCount(int iCrushBossCount)
    {
        m_stIntData[s_strCrushBossCount] += iCrushBossCount;
    }
    void SetHomeBuffID(const std::map<int32_t,CPackHomeGoodsBuffEffect>& stHomeBuffID)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stHomeBuffID);
        m_stVarData[s_strHomeBuffID] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetStartLevelTime(int iStartLevelTime)
    {
        m_stIntData[s_strStartLevelTime] += iStartLevelTime;
    }
    void SetAtlasBuffInfo(const std::map<int32_t,int32_t>& stAtlasBuffInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stAtlasBuffInfo);
        m_stVarData[s_strAtlasBuffInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetDailyGuildWarTime(char cDailyGuildWarTime)
    {
        m_stCharData[s_strDailyGuildWarTime] += cDailyGuildWarTime;
    }
    void SetGuildWarUseCard(const std::map<int32_t,char>& stGuildWarUseCard)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildWarUseCard);
        m_stVarData[s_strGuildWarUseCard] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetGuildSelfInfo(const std::map<int32_t,CPackGuildWarSelfInfo>& stGuildSelfInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildSelfInfo);
        m_stVarData[s_strGuildSelfInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetGuildWarCalInfo(const CPackGuildWarSelfRankInfo& stGuildWarCalInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stGuildWarCalInfo);
        m_stVarData[s_strGuildWarCalInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetLimitBossBaodiTime(int iLimitBossBaodiTime)
    {
        m_stIntData[s_strLimitBossBaodiTime] += iLimitBossBaodiTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TUserIbusyInfoTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
};

struct TUserIbusyInfoTableValueType
{
    TUserIbusyInfoTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iActivityID(0)
    , m_iLevel(0)
    , m_iExp(0)
    , m_iDailyExpAward(0)
    , m_iScore(0)
    , m_iExploreExp(0)
    , m_iTodayHuoyue(0)
    , m_iChangeTaskCount(0)
    , m_iFreeChangeTaskCount(0)
    , m_iMarketLastRefreshTime(0)
    , m_iMarketRefreshTimes(0)
    , m_stGettedLevelReward()
    , m_stGettedDepositReward()
    , m_stTaskInfo()
    , m_iSpecialGiftWeekID(0)
    , m_iSpecialGiftWeekLevel(0)
    , m_iSpecialGiftMonthID(0)
    , m_iSpecialGiftMonthLevel(0)
    , m_iLuckyHammerActivityID(0)
    , m_iLuckyHammerLotteryCount(0)
    , m_iMonthPassExp(0)
    , m_iMonthPassDay(0)
    , m_iMonthPassRecharge(0)
    , m_strMonthPassAwardBitmap("")
    , m_strMonthPassDepositAwardBitmap("")
    , m_iMonthPassWishEventID(0)
    , m_stMonthPassWishItemIndex()
    , m_stCardDialogLimitAvyStat()
    , m_iCardDialogNextTime(0)
    , m_iCardDialogTimes(0)
    , m_iNewsLastTime(0)
    , m_iNewsLastData(0)
    , m_iAddupDepositBonus(0)
    , m_iFoundationOpenTime(0)
    , m_ulOnceEventStat(0)
    , m_stRandomStoreRefreshData()
    , m_stSimpleGamePoint()
    , m_iHeartInsureOpenTime(0)
    , m_stMoonMapSkillLevel()
    , m_stRedBagUniqueIndex()
    , m_stWorldQuestRewardState()
    , m_iWorldQuestRewardTimes(0)
    , m_stQuestionnaireState()
    , m_stQuestionnaireDurationTime()
    , m_stLimitBossAccepted()
    , m_stLimitBossRand()
    , m_iLimitBoxTime(0)
    , m_iTotalHeartLevel(1)
    , m_iTotalHeartExp(0)
    , m_iConstelExpPool(0)
    , m_iFishingRod(0)
    , m_stMainBgm()
    , m_stHomeBgm()
    , m_iMallBaodiTimes(0)
    , m_iZhuHaiLevel(0)
    , m_iZhuHaiGress(0)
    , m_stZhuHaiBuff()
    , m_stZhuHaiCardHp()
    , m_stZhuHaiBlessGroup()
    , m_stZhuHaiRoleBuff()
    , m_iZhuHaiRefreshStore(0)
    , m_cZhuHaiSaveTeamFlag(0)
    , m_stGuildBossHurt()
    , m_iWishLastTime(0)
    , m_iWishCardID(0)
    , m_iWishNum(0)
    , m_stWishRewardVec()
    , m_stWishSwapCard()
    , m_stBadgeList()
    , m_stNewLimitBossReward()
    , m_iNewLimitBossDailyCount(0)
    , m_cDailyGuildDonate(0)
    , m_cDailyGuildSign(0)
    , m_iDailyGuildRandomCount(0)
    , m_stDailyGuildRandom()
    , m_cDailyGuildBar(0)
    , m_cWenwenChoose(0)
    , m_iConstelAdvTimes(0)
    , m_stGuildFragmentHistory()
    , m_stHomeExpTag()
    , m_stRuneInfo()
    , m_stConstelPutInfo()
    , m_cDataChangeTag(0)
    , m_cCbtRewardTag(0)
    , m_stConstelShop()
    , m_iConstelShopRefreshCount(0)
    , m_iConstelShopLastTime(0)
    , m_iCheatingCount(0)
    , m_stGashaponInfo()
    , m_iCrushBossLevel(0)
    , m_stCrushBossInfo()
    , m_iCrushBossCount(0)
    , m_stHomeBuffID()
    , m_iStartLevelTime(0)
    , m_stAtlasBuffInfo()
    , m_cDailyGuildWarTime(0)
    , m_stGuildWarUseCard()
    , m_stGuildSelfInfo()
    , m_stGuildWarCalInfo()
    , m_iLimitBossBaodiTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iActivityID;
    int                     m_iLevel;
    int                     m_iExp;
    int                     m_iDailyExpAward;
    int                     m_iScore;
    int                     m_iExploreExp;
    int                     m_iTodayHuoyue;
    int                     m_iChangeTaskCount;
    int                     m_iFreeChangeTaskCount;
    int                     m_iMarketLastRefreshTime;
    int                     m_iMarketRefreshTimes;
    std::set<int>           m_stGettedLevelReward;
    std::set<int>           m_stGettedDepositReward;
    CPackIbusyTaskData      m_stTaskInfo;
    int                     m_iSpecialGiftWeekID;
    int                     m_iSpecialGiftWeekLevel;
    int                     m_iSpecialGiftMonthID;
    int                     m_iSpecialGiftMonthLevel;
    int                     m_iLuckyHammerActivityID;
    int                     m_iLuckyHammerLotteryCount;
    int                     m_iMonthPassExp;
    int                     m_iMonthPassDay;
    int                     m_iMonthPassRecharge;
    std::string             m_strMonthPassAwardBitmap;
    std::string             m_strMonthPassDepositAwardBitmap;
    int                     m_iMonthPassWishEventID;
    std::vector<int>        m_stMonthPassWishItemIndex;
    std::vector<int32_t>    m_stCardDialogLimitAvyStat;
    int                     m_iCardDialogNextTime;
    int                     m_iCardDialogTimes;
    int                     m_iNewsLastTime;
    int                     m_iNewsLastData;
    int                     m_iAddupDepositBonus;
    int                     m_iFoundationOpenTime;
    uint64_t                m_ulOnceEventStat;
    std::map<int32_t, std::pair<int32_t, int32_t>>    m_stRandomStoreRefreshData;
    std::map<uint8_t, int32_t>    m_stSimpleGamePoint;
    int                     m_iHeartInsureOpenTime;
    std::map<int32_t, int32_t>    m_stMoonMapSkillLevel;
    CVecBitmap              m_stRedBagUniqueIndex;
    CVecBitmap              m_stWorldQuestRewardState;
    int                     m_iWorldQuestRewardTimes;
    CVecBitmap              m_stQuestionnaireState;
    std::map<int32_t, int32_t>    m_stQuestionnaireDurationTime;
    std::map<int32_t, int32_t>    m_stLimitBossAccepted;
    std::map<int32_t, int32_t>    m_stLimitBossRand;
    int                     m_iLimitBoxTime;
    int                     m_iTotalHeartLevel;
    int                     m_iTotalHeartExp;
    int                     m_iConstelExpPool;
    int                     m_iFishingRod;
    std::list<int32_t>      m_stMainBgm;
    std::list<int32_t>      m_stHomeBgm;
    int                     m_iMallBaodiTimes;
    int                     m_iZhuHaiLevel;
    int                     m_iZhuHaiGress;
    std::vector<int>        m_stZhuHaiBuff;
    std::map<int, int>      m_stZhuHaiCardHp;
    std::map<int, CPackZhuHaiBlessInfo>    m_stZhuHaiBlessGroup;
    std::map<int,std::vector<int>>    m_stZhuHaiRoleBuff;
    int                     m_iZhuHaiRefreshStore;
    char                    m_cZhuHaiSaveTeamFlag;
    std::map<int, uint64_t> m_stGuildBossHurt;
    int                     m_iWishLastTime;
    int                     m_iWishCardID;
    int                     m_iWishNum;
    std::vector<std::pair<std::string, int>>    m_stWishRewardVec;
    std::vector<int>        m_stWishSwapCard;
    std::vector<int>        m_stBadgeList;
    std::map<int32_t,std::vector<CPackLimitBossTeamInfo>>    m_stNewLimitBossReward;
    int                     m_iNewLimitBossDailyCount;
    char                    m_cDailyGuildDonate;
    char                    m_cDailyGuildSign;
    int                     m_iDailyGuildRandomCount;
    CPackGuildRandomInfo    m_stDailyGuildRandom;
    char                    m_cDailyGuildBar;
    char                    m_cWenwenChoose;
    int                     m_iConstelAdvTimes;
    std::vector<CPackGuildFragmentsHistory>    m_stGuildFragmentHistory;
    std::map<int,bool>      m_stHomeExpTag;
    std::map<int,CPackRuneInfo>    m_stRuneInfo;
    std::map<int,map<int,char>>    m_stConstelPutInfo;
    char                    m_cDataChangeTag;
    char                    m_cCbtRewardTag;
    std::vector<CPackConstelShopInfo>    m_stConstelShop;
    int                     m_iConstelShopRefreshCount;
    int                     m_iConstelShopLastTime;
    int                     m_iCheatingCount;
    std::map<int32_t,std::map<int32_t,CPackGashaponInfo>>    m_stGashaponInfo;
    int                     m_iCrushBossLevel;
    CPackCrushBossInfo      m_stCrushBossInfo;
    int                     m_iCrushBossCount;
    std::map<int32_t,CPackHomeGoodsBuffEffect>    m_stHomeBuffID;
    int                     m_iStartLevelTime;
    std::map<int32_t,int32_t>    m_stAtlasBuffInfo;
    char                    m_cDailyGuildWarTime;
    std::map<int32_t,char>  m_stGuildWarUseCard;
    std::map<int32_t,CPackGuildWarSelfInfo>    m_stGuildSelfInfo;
    CPackGuildWarSelfRankInfo    m_stGuildWarCalInfo;
    int                     m_iLimitBossBaodiTime;
};

struct TUserIbusyInfoTable
{
    TUserIbusyInfoTable() : m_pRecord(nullptr) { }
    ~TUserIbusyInfoTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserIbusyInfoTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserIbusyInfoTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID);
        m_pRecord->SetActivityID(stData.m_iActivityID);
        m_pRecord->SetLevel(stData.m_iLevel);
        m_pRecord->SetExp(stData.m_iExp);
        m_pRecord->SetDailyExpAward(stData.m_iDailyExpAward);
        m_pRecord->SetScore(stData.m_iScore);
        m_pRecord->SetExploreExp(stData.m_iExploreExp);
        m_pRecord->SetTodayHuoyue(stData.m_iTodayHuoyue);
        m_pRecord->SetChangeTaskCount(stData.m_iChangeTaskCount);
        m_pRecord->SetFreeChangeTaskCount(stData.m_iFreeChangeTaskCount);
        m_pRecord->SetMarketLastRefreshTime(stData.m_iMarketLastRefreshTime);
        m_pRecord->SetMarketRefreshTimes(stData.m_iMarketRefreshTimes);
        m_pRecord->SetGettedLevelReward(stData.m_stGettedLevelReward);
        m_pRecord->SetGettedDepositReward(stData.m_stGettedDepositReward);
        m_pRecord->SetTaskInfo(stData.m_stTaskInfo);
        m_pRecord->SetSpecialGiftWeekID(stData.m_iSpecialGiftWeekID);
        m_pRecord->SetSpecialGiftWeekLevel(stData.m_iSpecialGiftWeekLevel);
        m_pRecord->SetSpecialGiftMonthID(stData.m_iSpecialGiftMonthID);
        m_pRecord->SetSpecialGiftMonthLevel(stData.m_iSpecialGiftMonthLevel);
        m_pRecord->SetLuckyHammerActivityID(stData.m_iLuckyHammerActivityID);
        m_pRecord->SetLuckyHammerLotteryCount(stData.m_iLuckyHammerLotteryCount);
        m_pRecord->SetMonthPassExp(stData.m_iMonthPassExp);
        m_pRecord->SetMonthPassDay(stData.m_iMonthPassDay);
        m_pRecord->SetMonthPassRecharge(stData.m_iMonthPassRecharge);
        m_pRecord->SetMonthPassAwardBitmap(stData.m_strMonthPassAwardBitmap);
        m_pRecord->SetMonthPassDepositAwardBitmap(stData.m_strMonthPassDepositAwardBitmap);
        m_pRecord->SetMonthPassWishEventID(stData.m_iMonthPassWishEventID);
        m_pRecord->SetMonthPassWishItemIndex(stData.m_stMonthPassWishItemIndex);
        m_pRecord->SetCardDialogLimitAvyStat(stData.m_stCardDialogLimitAvyStat);
        m_pRecord->SetCardDialogNextTime(stData.m_iCardDialogNextTime);
        m_pRecord->SetCardDialogTimes(stData.m_iCardDialogTimes);
        m_pRecord->SetNewsLastTime(stData.m_iNewsLastTime);
        m_pRecord->SetNewsLastData(stData.m_iNewsLastData);
        m_pRecord->SetAddupDepositBonus(stData.m_iAddupDepositBonus);
        m_pRecord->SetFoundationOpenTime(stData.m_iFoundationOpenTime);
        m_pRecord->SetOnceEventStat(stData.m_ulOnceEventStat);
        m_pRecord->SetRandomStoreRefreshData(stData.m_stRandomStoreRefreshData);
        m_pRecord->SetSimpleGamePoint(stData.m_stSimpleGamePoint);
        m_pRecord->SetHeartInsureOpenTime(stData.m_iHeartInsureOpenTime);
        m_pRecord->SetMoonMapSkillLevel(stData.m_stMoonMapSkillLevel);
        m_pRecord->SetRedBagUniqueIndex(stData.m_stRedBagUniqueIndex);
        m_pRecord->SetWorldQuestRewardState(stData.m_stWorldQuestRewardState);
        m_pRecord->SetWorldQuestRewardTimes(stData.m_iWorldQuestRewardTimes);
        m_pRecord->SetQuestionnaireState(stData.m_stQuestionnaireState);
        m_pRecord->SetQuestionnaireDurationTime(stData.m_stQuestionnaireDurationTime);
        m_pRecord->SetLimitBossAccepted(stData.m_stLimitBossAccepted);
        m_pRecord->SetLimitBossRand(stData.m_stLimitBossRand);
        m_pRecord->SetLimitBoxTime(stData.m_iLimitBoxTime);
        m_pRecord->SetTotalHeartLevel(stData.m_iTotalHeartLevel);
        m_pRecord->SetTotalHeartExp(stData.m_iTotalHeartExp);
        m_pRecord->SetConstelExpPool(stData.m_iConstelExpPool);
        m_pRecord->SetFishingRod(stData.m_iFishingRod);
        m_pRecord->SetMainBgm(stData.m_stMainBgm);
        m_pRecord->SetHomeBgm(stData.m_stHomeBgm);
        m_pRecord->SetMallBaodiTimes(stData.m_iMallBaodiTimes);
        m_pRecord->SetZhuHaiLevel(stData.m_iZhuHaiLevel);
        m_pRecord->SetZhuHaiGress(stData.m_iZhuHaiGress);
        m_pRecord->SetZhuHaiBuff(stData.m_stZhuHaiBuff);
        m_pRecord->SetZhuHaiCardHp(stData.m_stZhuHaiCardHp);
        m_pRecord->SetZhuHaiBlessGroup(stData.m_stZhuHaiBlessGroup);
        m_pRecord->SetZhuHaiRoleBuff(stData.m_stZhuHaiRoleBuff);
        m_pRecord->SetZhuHaiRefreshStore(stData.m_iZhuHaiRefreshStore);
        m_pRecord->SetZhuHaiSaveTeamFlag(stData.m_cZhuHaiSaveTeamFlag);
        m_pRecord->SetGuildBossHurt(stData.m_stGuildBossHurt);
        m_pRecord->SetWishLastTime(stData.m_iWishLastTime);
        m_pRecord->SetWishCardID(stData.m_iWishCardID);
        m_pRecord->SetWishNum(stData.m_iWishNum);
        m_pRecord->SetWishRewardVec(stData.m_stWishRewardVec);
        m_pRecord->SetWishSwapCard(stData.m_stWishSwapCard);
        m_pRecord->SetBadgeList(stData.m_stBadgeList);
        m_pRecord->SetNewLimitBossReward(stData.m_stNewLimitBossReward);
        m_pRecord->SetNewLimitBossDailyCount(stData.m_iNewLimitBossDailyCount);
        m_pRecord->SetDailyGuildDonate(stData.m_cDailyGuildDonate);
        m_pRecord->SetDailyGuildSign(stData.m_cDailyGuildSign);
        m_pRecord->SetDailyGuildRandomCount(stData.m_iDailyGuildRandomCount);
        m_pRecord->SetDailyGuildRandom(stData.m_stDailyGuildRandom);
        m_pRecord->SetDailyGuildBar(stData.m_cDailyGuildBar);
        m_pRecord->SetWenwenChoose(stData.m_cWenwenChoose);
        m_pRecord->SetConstelAdvTimes(stData.m_iConstelAdvTimes);
        m_pRecord->SetGuildFragmentHistory(stData.m_stGuildFragmentHistory);
        m_pRecord->SetHomeExpTag(stData.m_stHomeExpTag);
        m_pRecord->SetRuneInfo(stData.m_stRuneInfo);
        m_pRecord->SetConstelPutInfo(stData.m_stConstelPutInfo);
        m_pRecord->SetDataChangeTag(stData.m_cDataChangeTag);
        m_pRecord->SetCbtRewardTag(stData.m_cCbtRewardTag);
        m_pRecord->SetConstelShop(stData.m_stConstelShop);
        m_pRecord->SetConstelShopRefreshCount(stData.m_iConstelShopRefreshCount);
        m_pRecord->SetConstelShopLastTime(stData.m_iConstelShopLastTime);
        m_pRecord->SetCheatingCount(stData.m_iCheatingCount);
        m_pRecord->SetGashaponInfo(stData.m_stGashaponInfo);
        m_pRecord->SetCrushBossLevel(stData.m_iCrushBossLevel);
        m_pRecord->SetCrushBossInfo(stData.m_stCrushBossInfo);
        m_pRecord->SetCrushBossCount(stData.m_iCrushBossCount);
        m_pRecord->SetHomeBuffID(stData.m_stHomeBuffID);
        m_pRecord->SetStartLevelTime(stData.m_iStartLevelTime);
        m_pRecord->SetAtlasBuffInfo(stData.m_stAtlasBuffInfo);
        m_pRecord->SetDailyGuildWarTime(stData.m_cDailyGuildWarTime);
        m_pRecord->SetGuildWarUseCard(stData.m_stGuildWarUseCard);
        m_pRecord->SetGuildSelfInfo(stData.m_stGuildSelfInfo);
        m_pRecord->SetGuildWarCalInfo(stData.m_stGuildWarCalInfo);
        m_pRecord->SetLimitBossBaodiTime(stData.m_iLimitBossBaodiTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetActivityID() const
    {
        return m_stData.m_iActivityID;
    }
    void AddActivityID(int iActivityID)
    {
        if (0 == iActivityID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActivityID(iActivityID);
        m_stData.m_iActivityID += iActivityID;
    }
    void SetActivityID(int iActivityID)
    {
        if (m_stData.m_iActivityID == iActivityID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iActivityID - m_stData.m_iActivityID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetActivityID(RealAdd);
        m_stData.m_iActivityID = iActivityID;
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
    const int& GetDailyExpAward() const
    {
        return m_stData.m_iDailyExpAward;
    }
    void AddDailyExpAward(int iDailyExpAward)
    {
        if (0 == iDailyExpAward)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyExpAward(iDailyExpAward);
        m_stData.m_iDailyExpAward += iDailyExpAward;
    }
    void SetDailyExpAward(int iDailyExpAward)
    {
        if (m_stData.m_iDailyExpAward == iDailyExpAward)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDailyExpAward - m_stData.m_iDailyExpAward;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyExpAward(RealAdd);
        m_stData.m_iDailyExpAward = iDailyExpAward;
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
    const int& GetExploreExp() const
    {
        return m_stData.m_iExploreExp;
    }
    void AddExploreExp(int iExploreExp)
    {
        if (0 == iExploreExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExploreExp(iExploreExp);
        m_stData.m_iExploreExp += iExploreExp;
    }
    void SetExploreExp(int iExploreExp)
    {
        if (m_stData.m_iExploreExp == iExploreExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iExploreExp - m_stData.m_iExploreExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExploreExp(RealAdd);
        m_stData.m_iExploreExp = iExploreExp;
    }
    const int& GetTodayHuoyue() const
    {
        return m_stData.m_iTodayHuoyue;
    }
    void AddTodayHuoyue(int iTodayHuoyue)
    {
        if (0 == iTodayHuoyue)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTodayHuoyue(iTodayHuoyue);
        m_stData.m_iTodayHuoyue += iTodayHuoyue;
    }
    void SetTodayHuoyue(int iTodayHuoyue)
    {
        if (m_stData.m_iTodayHuoyue == iTodayHuoyue)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTodayHuoyue - m_stData.m_iTodayHuoyue;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTodayHuoyue(RealAdd);
        m_stData.m_iTodayHuoyue = iTodayHuoyue;
    }
    const int& GetChangeTaskCount() const
    {
        return m_stData.m_iChangeTaskCount;
    }
    void AddChangeTaskCount(int iChangeTaskCount)
    {
        if (0 == iChangeTaskCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChangeTaskCount(iChangeTaskCount);
        m_stData.m_iChangeTaskCount += iChangeTaskCount;
    }
    void SetChangeTaskCount(int iChangeTaskCount)
    {
        if (m_stData.m_iChangeTaskCount == iChangeTaskCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iChangeTaskCount - m_stData.m_iChangeTaskCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChangeTaskCount(RealAdd);
        m_stData.m_iChangeTaskCount = iChangeTaskCount;
    }
    const int& GetFreeChangeTaskCount() const
    {
        return m_stData.m_iFreeChangeTaskCount;
    }
    void AddFreeChangeTaskCount(int iFreeChangeTaskCount)
    {
        if (0 == iFreeChangeTaskCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFreeChangeTaskCount(iFreeChangeTaskCount);
        m_stData.m_iFreeChangeTaskCount += iFreeChangeTaskCount;
    }
    void SetFreeChangeTaskCount(int iFreeChangeTaskCount)
    {
        if (m_stData.m_iFreeChangeTaskCount == iFreeChangeTaskCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFreeChangeTaskCount - m_stData.m_iFreeChangeTaskCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFreeChangeTaskCount(RealAdd);
        m_stData.m_iFreeChangeTaskCount = iFreeChangeTaskCount;
    }
    const int& GetMarketLastRefreshTime() const
    {
        return m_stData.m_iMarketLastRefreshTime;
    }
    void AddMarketLastRefreshTime(int iMarketLastRefreshTime)
    {
        if (0 == iMarketLastRefreshTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMarketLastRefreshTime(iMarketLastRefreshTime);
        m_stData.m_iMarketLastRefreshTime += iMarketLastRefreshTime;
    }
    void SetMarketLastRefreshTime(int iMarketLastRefreshTime)
    {
        if (m_stData.m_iMarketLastRefreshTime == iMarketLastRefreshTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMarketLastRefreshTime - m_stData.m_iMarketLastRefreshTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMarketLastRefreshTime(RealAdd);
        m_stData.m_iMarketLastRefreshTime = iMarketLastRefreshTime;
    }
    const int& GetMarketRefreshTimes() const
    {
        return m_stData.m_iMarketRefreshTimes;
    }
    void AddMarketRefreshTimes(int iMarketRefreshTimes)
    {
        if (0 == iMarketRefreshTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMarketRefreshTimes(iMarketRefreshTimes);
        m_stData.m_iMarketRefreshTimes += iMarketRefreshTimes;
    }
    void SetMarketRefreshTimes(int iMarketRefreshTimes)
    {
        if (m_stData.m_iMarketRefreshTimes == iMarketRefreshTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMarketRefreshTimes - m_stData.m_iMarketRefreshTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMarketRefreshTimes(RealAdd);
        m_stData.m_iMarketRefreshTimes = iMarketRefreshTimes;
    }
    const std::set<int>& GetGettedLevelReward() const
    {
        return m_stData.m_stGettedLevelReward;
    }
    void SetGettedLevelReward(const std::set<int>& stGettedLevelReward)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGettedLevelReward(stGettedLevelReward);
        m_stData.m_stGettedLevelReward = stGettedLevelReward;
    }
    const std::set<int>& GetGettedDepositReward() const
    {
        return m_stData.m_stGettedDepositReward;
    }
    void SetGettedDepositReward(const std::set<int>& stGettedDepositReward)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGettedDepositReward(stGettedDepositReward);
        m_stData.m_stGettedDepositReward = stGettedDepositReward;
    }
    const CPackIbusyTaskData& GetTaskInfo() const
    {
        return m_stData.m_stTaskInfo;
    }
    void SetTaskInfo(const CPackIbusyTaskData& stTaskInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTaskInfo(stTaskInfo);
        m_stData.m_stTaskInfo = stTaskInfo;
    }
    const int& GetSpecialGiftWeekID() const
    {
        return m_stData.m_iSpecialGiftWeekID;
    }
    void AddSpecialGiftWeekID(int iSpecialGiftWeekID)
    {
        if (0 == iSpecialGiftWeekID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSpecialGiftWeekID(iSpecialGiftWeekID);
        m_stData.m_iSpecialGiftWeekID += iSpecialGiftWeekID;
    }
    void SetSpecialGiftWeekID(int iSpecialGiftWeekID)
    {
        if (m_stData.m_iSpecialGiftWeekID == iSpecialGiftWeekID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSpecialGiftWeekID - m_stData.m_iSpecialGiftWeekID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSpecialGiftWeekID(RealAdd);
        m_stData.m_iSpecialGiftWeekID = iSpecialGiftWeekID;
    }
    const int& GetSpecialGiftWeekLevel() const
    {
        return m_stData.m_iSpecialGiftWeekLevel;
    }
    void AddSpecialGiftWeekLevel(int iSpecialGiftWeekLevel)
    {
        if (0 == iSpecialGiftWeekLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSpecialGiftWeekLevel(iSpecialGiftWeekLevel);
        m_stData.m_iSpecialGiftWeekLevel += iSpecialGiftWeekLevel;
    }
    void SetSpecialGiftWeekLevel(int iSpecialGiftWeekLevel)
    {
        if (m_stData.m_iSpecialGiftWeekLevel == iSpecialGiftWeekLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSpecialGiftWeekLevel - m_stData.m_iSpecialGiftWeekLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSpecialGiftWeekLevel(RealAdd);
        m_stData.m_iSpecialGiftWeekLevel = iSpecialGiftWeekLevel;
    }
    const int& GetSpecialGiftMonthID() const
    {
        return m_stData.m_iSpecialGiftMonthID;
    }
    void AddSpecialGiftMonthID(int iSpecialGiftMonthID)
    {
        if (0 == iSpecialGiftMonthID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSpecialGiftMonthID(iSpecialGiftMonthID);
        m_stData.m_iSpecialGiftMonthID += iSpecialGiftMonthID;
    }
    void SetSpecialGiftMonthID(int iSpecialGiftMonthID)
    {
        if (m_stData.m_iSpecialGiftMonthID == iSpecialGiftMonthID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSpecialGiftMonthID - m_stData.m_iSpecialGiftMonthID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSpecialGiftMonthID(RealAdd);
        m_stData.m_iSpecialGiftMonthID = iSpecialGiftMonthID;
    }
    const int& GetSpecialGiftMonthLevel() const
    {
        return m_stData.m_iSpecialGiftMonthLevel;
    }
    void AddSpecialGiftMonthLevel(int iSpecialGiftMonthLevel)
    {
        if (0 == iSpecialGiftMonthLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSpecialGiftMonthLevel(iSpecialGiftMonthLevel);
        m_stData.m_iSpecialGiftMonthLevel += iSpecialGiftMonthLevel;
    }
    void SetSpecialGiftMonthLevel(int iSpecialGiftMonthLevel)
    {
        if (m_stData.m_iSpecialGiftMonthLevel == iSpecialGiftMonthLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSpecialGiftMonthLevel - m_stData.m_iSpecialGiftMonthLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSpecialGiftMonthLevel(RealAdd);
        m_stData.m_iSpecialGiftMonthLevel = iSpecialGiftMonthLevel;
    }
    const int& GetLuckyHammerActivityID() const
    {
        return m_stData.m_iLuckyHammerActivityID;
    }
    void AddLuckyHammerActivityID(int iLuckyHammerActivityID)
    {
        if (0 == iLuckyHammerActivityID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLuckyHammerActivityID(iLuckyHammerActivityID);
        m_stData.m_iLuckyHammerActivityID += iLuckyHammerActivityID;
    }
    void SetLuckyHammerActivityID(int iLuckyHammerActivityID)
    {
        if (m_stData.m_iLuckyHammerActivityID == iLuckyHammerActivityID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLuckyHammerActivityID - m_stData.m_iLuckyHammerActivityID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLuckyHammerActivityID(RealAdd);
        m_stData.m_iLuckyHammerActivityID = iLuckyHammerActivityID;
    }
    const int& GetLuckyHammerLotteryCount() const
    {
        return m_stData.m_iLuckyHammerLotteryCount;
    }
    void AddLuckyHammerLotteryCount(int iLuckyHammerLotteryCount)
    {
        if (0 == iLuckyHammerLotteryCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLuckyHammerLotteryCount(iLuckyHammerLotteryCount);
        m_stData.m_iLuckyHammerLotteryCount += iLuckyHammerLotteryCount;
    }
    void SetLuckyHammerLotteryCount(int iLuckyHammerLotteryCount)
    {
        if (m_stData.m_iLuckyHammerLotteryCount == iLuckyHammerLotteryCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLuckyHammerLotteryCount - m_stData.m_iLuckyHammerLotteryCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLuckyHammerLotteryCount(RealAdd);
        m_stData.m_iLuckyHammerLotteryCount = iLuckyHammerLotteryCount;
    }
    const int& GetMonthPassExp() const
    {
        return m_stData.m_iMonthPassExp;
    }
    void AddMonthPassExp(int iMonthPassExp)
    {
        if (0 == iMonthPassExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassExp(iMonthPassExp);
        m_stData.m_iMonthPassExp += iMonthPassExp;
    }
    void SetMonthPassExp(int iMonthPassExp)
    {
        if (m_stData.m_iMonthPassExp == iMonthPassExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMonthPassExp - m_stData.m_iMonthPassExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassExp(RealAdd);
        m_stData.m_iMonthPassExp = iMonthPassExp;
    }
    const int& GetMonthPassDay() const
    {
        return m_stData.m_iMonthPassDay;
    }
    void AddMonthPassDay(int iMonthPassDay)
    {
        if (0 == iMonthPassDay)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassDay(iMonthPassDay);
        m_stData.m_iMonthPassDay += iMonthPassDay;
    }
    void SetMonthPassDay(int iMonthPassDay)
    {
        if (m_stData.m_iMonthPassDay == iMonthPassDay)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMonthPassDay - m_stData.m_iMonthPassDay;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassDay(RealAdd);
        m_stData.m_iMonthPassDay = iMonthPassDay;
    }
    const int& GetMonthPassRecharge() const
    {
        return m_stData.m_iMonthPassRecharge;
    }
    void AddMonthPassRecharge(int iMonthPassRecharge)
    {
        if (0 == iMonthPassRecharge)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassRecharge(iMonthPassRecharge);
        m_stData.m_iMonthPassRecharge += iMonthPassRecharge;
    }
    void SetMonthPassRecharge(int iMonthPassRecharge)
    {
        if (m_stData.m_iMonthPassRecharge == iMonthPassRecharge)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMonthPassRecharge - m_stData.m_iMonthPassRecharge;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassRecharge(RealAdd);
        m_stData.m_iMonthPassRecharge = iMonthPassRecharge;
    }
    const std::string& GetMonthPassAwardBitmap() const
    {
        return m_stData.m_strMonthPassAwardBitmap;
    }
    void SetMonthPassAwardBitmap(const std::string& strMonthPassAwardBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassAwardBitmap(strMonthPassAwardBitmap);
        m_stData.m_strMonthPassAwardBitmap = strMonthPassAwardBitmap;
    }
    const std::string& GetMonthPassDepositAwardBitmap() const
    {
        return m_stData.m_strMonthPassDepositAwardBitmap;
    }
    void SetMonthPassDepositAwardBitmap(const std::string& strMonthPassDepositAwardBitmap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassDepositAwardBitmap(strMonthPassDepositAwardBitmap);
        m_stData.m_strMonthPassDepositAwardBitmap = strMonthPassDepositAwardBitmap;
    }
    const int& GetMonthPassWishEventID() const
    {
        return m_stData.m_iMonthPassWishEventID;
    }
    void AddMonthPassWishEventID(int iMonthPassWishEventID)
    {
        if (0 == iMonthPassWishEventID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassWishEventID(iMonthPassWishEventID);
        m_stData.m_iMonthPassWishEventID += iMonthPassWishEventID;
    }
    void SetMonthPassWishEventID(int iMonthPassWishEventID)
    {
        if (m_stData.m_iMonthPassWishEventID == iMonthPassWishEventID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMonthPassWishEventID - m_stData.m_iMonthPassWishEventID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassWishEventID(RealAdd);
        m_stData.m_iMonthPassWishEventID = iMonthPassWishEventID;
    }
    const std::vector<int>& GetMonthPassWishItemIndex() const
    {
        return m_stData.m_stMonthPassWishItemIndex;
    }
    void SetMonthPassWishItemIndex(const std::vector<int>& stMonthPassWishItemIndex)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMonthPassWishItemIndex(stMonthPassWishItemIndex);
        m_stData.m_stMonthPassWishItemIndex = stMonthPassWishItemIndex;
    }
    const std::vector<int32_t>& GetCardDialogLimitAvyStat() const
    {
        return m_stData.m_stCardDialogLimitAvyStat;
    }
    void SetCardDialogLimitAvyStat(const std::vector<int32_t>& stCardDialogLimitAvyStat)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardDialogLimitAvyStat(stCardDialogLimitAvyStat);
        m_stData.m_stCardDialogLimitAvyStat = stCardDialogLimitAvyStat;
    }
    const int& GetCardDialogNextTime() const
    {
        return m_stData.m_iCardDialogNextTime;
    }
    void AddCardDialogNextTime(int iCardDialogNextTime)
    {
        if (0 == iCardDialogNextTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardDialogNextTime(iCardDialogNextTime);
        m_stData.m_iCardDialogNextTime += iCardDialogNextTime;
    }
    void SetCardDialogNextTime(int iCardDialogNextTime)
    {
        if (m_stData.m_iCardDialogNextTime == iCardDialogNextTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCardDialogNextTime - m_stData.m_iCardDialogNextTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardDialogNextTime(RealAdd);
        m_stData.m_iCardDialogNextTime = iCardDialogNextTime;
    }
    const int& GetCardDialogTimes() const
    {
        return m_stData.m_iCardDialogTimes;
    }
    void AddCardDialogTimes(int iCardDialogTimes)
    {
        if (0 == iCardDialogTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardDialogTimes(iCardDialogTimes);
        m_stData.m_iCardDialogTimes += iCardDialogTimes;
    }
    void SetCardDialogTimes(int iCardDialogTimes)
    {
        if (m_stData.m_iCardDialogTimes == iCardDialogTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCardDialogTimes - m_stData.m_iCardDialogTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardDialogTimes(RealAdd);
        m_stData.m_iCardDialogTimes = iCardDialogTimes;
    }
    const int& GetNewsLastTime() const
    {
        return m_stData.m_iNewsLastTime;
    }
    void AddNewsLastTime(int iNewsLastTime)
    {
        if (0 == iNewsLastTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewsLastTime(iNewsLastTime);
        m_stData.m_iNewsLastTime += iNewsLastTime;
    }
    void SetNewsLastTime(int iNewsLastTime)
    {
        if (m_stData.m_iNewsLastTime == iNewsLastTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNewsLastTime - m_stData.m_iNewsLastTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewsLastTime(RealAdd);
        m_stData.m_iNewsLastTime = iNewsLastTime;
    }
    const int& GetNewsLastData() const
    {
        return m_stData.m_iNewsLastData;
    }
    void AddNewsLastData(int iNewsLastData)
    {
        if (0 == iNewsLastData)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewsLastData(iNewsLastData);
        m_stData.m_iNewsLastData += iNewsLastData;
    }
    void SetNewsLastData(int iNewsLastData)
    {
        if (m_stData.m_iNewsLastData == iNewsLastData)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNewsLastData - m_stData.m_iNewsLastData;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewsLastData(RealAdd);
        m_stData.m_iNewsLastData = iNewsLastData;
    }
    const int& GetAddupDepositBonus() const
    {
        return m_stData.m_iAddupDepositBonus;
    }
    void AddAddupDepositBonus(int iAddupDepositBonus)
    {
        if (0 == iAddupDepositBonus)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAddupDepositBonus(iAddupDepositBonus);
        m_stData.m_iAddupDepositBonus += iAddupDepositBonus;
    }
    void SetAddupDepositBonus(int iAddupDepositBonus)
    {
        if (m_stData.m_iAddupDepositBonus == iAddupDepositBonus)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAddupDepositBonus - m_stData.m_iAddupDepositBonus;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAddupDepositBonus(RealAdd);
        m_stData.m_iAddupDepositBonus = iAddupDepositBonus;
    }
    const int& GetFoundationOpenTime() const
    {
        return m_stData.m_iFoundationOpenTime;
    }
    void AddFoundationOpenTime(int iFoundationOpenTime)
    {
        if (0 == iFoundationOpenTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFoundationOpenTime(iFoundationOpenTime);
        m_stData.m_iFoundationOpenTime += iFoundationOpenTime;
    }
    void SetFoundationOpenTime(int iFoundationOpenTime)
    {
        if (m_stData.m_iFoundationOpenTime == iFoundationOpenTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFoundationOpenTime - m_stData.m_iFoundationOpenTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFoundationOpenTime(RealAdd);
        m_stData.m_iFoundationOpenTime = iFoundationOpenTime;
    }
    const uint64_t& GetOnceEventStat() const
    {
        return m_stData.m_ulOnceEventStat;
    }
    void AddOnceEventStat(uint64_t ulOnceEventStat)
    {
        if (0 == ulOnceEventStat)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetOnceEventStat(ulOnceEventStat);
        m_stData.m_ulOnceEventStat += ulOnceEventStat;
    }
    void SetOnceEventStat(uint64_t ulOnceEventStat)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulOnceEventStat > ulOnceEventStat)
        {
            m_pRecord->SetOnceEventStat(m_stData.m_ulOnceEventStat - ulOnceEventStat, '-');
        }
        else
        {
            m_pRecord->SetOnceEventStat(ulOnceEventStat - m_stData.m_ulOnceEventStat, '+');
        }
        m_stData.m_ulOnceEventStat = ulOnceEventStat;
    }
    const std::map<int32_t, std::pair<int32_t, int32_t>>& GetRandomStoreRefreshData() const
    {
        return m_stData.m_stRandomStoreRefreshData;
    }
    void SetRandomStoreRefreshData(const std::map<int32_t, std::pair<int32_t, int32_t>>& stRandomStoreRefreshData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRandomStoreRefreshData(stRandomStoreRefreshData);
        m_stData.m_stRandomStoreRefreshData = stRandomStoreRefreshData;
    }
    const std::map<uint8_t, int32_t>& GetSimpleGamePoint() const
    {
        return m_stData.m_stSimpleGamePoint;
    }
    void SetSimpleGamePoint(const std::map<uint8_t, int32_t>& stSimpleGamePoint)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSimpleGamePoint(stSimpleGamePoint);
        m_stData.m_stSimpleGamePoint = stSimpleGamePoint;
    }
    const int& GetHeartInsureOpenTime() const
    {
        return m_stData.m_iHeartInsureOpenTime;
    }
    void AddHeartInsureOpenTime(int iHeartInsureOpenTime)
    {
        if (0 == iHeartInsureOpenTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeartInsureOpenTime(iHeartInsureOpenTime);
        m_stData.m_iHeartInsureOpenTime += iHeartInsureOpenTime;
    }
    void SetHeartInsureOpenTime(int iHeartInsureOpenTime)
    {
        if (m_stData.m_iHeartInsureOpenTime == iHeartInsureOpenTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iHeartInsureOpenTime - m_stData.m_iHeartInsureOpenTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeartInsureOpenTime(RealAdd);
        m_stData.m_iHeartInsureOpenTime = iHeartInsureOpenTime;
    }
    const std::map<int32_t, int32_t>& GetMoonMapSkillLevel() const
    {
        return m_stData.m_stMoonMapSkillLevel;
    }
    void SetMoonMapSkillLevel(const std::map<int32_t, int32_t>& stMoonMapSkillLevel)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMoonMapSkillLevel(stMoonMapSkillLevel);
        m_stData.m_stMoonMapSkillLevel = stMoonMapSkillLevel;
    }
    const CVecBitmap& GetRedBagUniqueIndex() const
    {
        return m_stData.m_stRedBagUniqueIndex;
    }
    void SetRedBagUniqueIndex(const CVecBitmap& stRedBagUniqueIndex)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRedBagUniqueIndex(stRedBagUniqueIndex);
        m_stData.m_stRedBagUniqueIndex = stRedBagUniqueIndex;
    }
    const CVecBitmap& GetWorldQuestRewardState() const
    {
        return m_stData.m_stWorldQuestRewardState;
    }
    void SetWorldQuestRewardState(const CVecBitmap& stWorldQuestRewardState)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWorldQuestRewardState(stWorldQuestRewardState);
        m_stData.m_stWorldQuestRewardState = stWorldQuestRewardState;
    }
    const int& GetWorldQuestRewardTimes() const
    {
        return m_stData.m_iWorldQuestRewardTimes;
    }
    void AddWorldQuestRewardTimes(int iWorldQuestRewardTimes)
    {
        if (0 == iWorldQuestRewardTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWorldQuestRewardTimes(iWorldQuestRewardTimes);
        m_stData.m_iWorldQuestRewardTimes += iWorldQuestRewardTimes;
    }
    void SetWorldQuestRewardTimes(int iWorldQuestRewardTimes)
    {
        if (m_stData.m_iWorldQuestRewardTimes == iWorldQuestRewardTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWorldQuestRewardTimes - m_stData.m_iWorldQuestRewardTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWorldQuestRewardTimes(RealAdd);
        m_stData.m_iWorldQuestRewardTimes = iWorldQuestRewardTimes;
    }
    const CVecBitmap& GetQuestionnaireState() const
    {
        return m_stData.m_stQuestionnaireState;
    }
    void SetQuestionnaireState(const CVecBitmap& stQuestionnaireState)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQuestionnaireState(stQuestionnaireState);
        m_stData.m_stQuestionnaireState = stQuestionnaireState;
    }
    const std::map<int32_t, int32_t>& GetQuestionnaireDurationTime() const
    {
        return m_stData.m_stQuestionnaireDurationTime;
    }
    void SetQuestionnaireDurationTime(const std::map<int32_t, int32_t>& stQuestionnaireDurationTime)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQuestionnaireDurationTime(stQuestionnaireDurationTime);
        m_stData.m_stQuestionnaireDurationTime = stQuestionnaireDurationTime;
    }
    const std::map<int32_t, int32_t>& GetLimitBossAccepted() const
    {
        return m_stData.m_stLimitBossAccepted;
    }
    void SetLimitBossAccepted(const std::map<int32_t, int32_t>& stLimitBossAccepted)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBossAccepted(stLimitBossAccepted);
        m_stData.m_stLimitBossAccepted = stLimitBossAccepted;
    }
    const std::map<int32_t, int32_t>& GetLimitBossRand() const
    {
        return m_stData.m_stLimitBossRand;
    }
    void SetLimitBossRand(const std::map<int32_t, int32_t>& stLimitBossRand)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBossRand(stLimitBossRand);
        m_stData.m_stLimitBossRand = stLimitBossRand;
    }
    const int& GetLimitBoxTime() const
    {
        return m_stData.m_iLimitBoxTime;
    }
    void AddLimitBoxTime(int iLimitBoxTime)
    {
        if (0 == iLimitBoxTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBoxTime(iLimitBoxTime);
        m_stData.m_iLimitBoxTime += iLimitBoxTime;
    }
    void SetLimitBoxTime(int iLimitBoxTime)
    {
        if (m_stData.m_iLimitBoxTime == iLimitBoxTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLimitBoxTime - m_stData.m_iLimitBoxTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBoxTime(RealAdd);
        m_stData.m_iLimitBoxTime = iLimitBoxTime;
    }
    const int& GetTotalHeartLevel() const
    {
        return m_stData.m_iTotalHeartLevel;
    }
    void AddTotalHeartLevel(int iTotalHeartLevel)
    {
        if (0 == iTotalHeartLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalHeartLevel(iTotalHeartLevel);
        m_stData.m_iTotalHeartLevel += iTotalHeartLevel;
    }
    void SetTotalHeartLevel(int iTotalHeartLevel)
    {
        if (m_stData.m_iTotalHeartLevel == iTotalHeartLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTotalHeartLevel - m_stData.m_iTotalHeartLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalHeartLevel(RealAdd);
        m_stData.m_iTotalHeartLevel = iTotalHeartLevel;
    }
    const int& GetTotalHeartExp() const
    {
        return m_stData.m_iTotalHeartExp;
    }
    void AddTotalHeartExp(int iTotalHeartExp)
    {
        if (0 == iTotalHeartExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalHeartExp(iTotalHeartExp);
        m_stData.m_iTotalHeartExp += iTotalHeartExp;
    }
    void SetTotalHeartExp(int iTotalHeartExp)
    {
        if (m_stData.m_iTotalHeartExp == iTotalHeartExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTotalHeartExp - m_stData.m_iTotalHeartExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalHeartExp(RealAdd);
        m_stData.m_iTotalHeartExp = iTotalHeartExp;
    }
    const int& GetConstelExpPool() const
    {
        return m_stData.m_iConstelExpPool;
    }
    void AddConstelExpPool(int iConstelExpPool)
    {
        if (0 == iConstelExpPool)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelExpPool(iConstelExpPool);
        m_stData.m_iConstelExpPool += iConstelExpPool;
    }
    void SetConstelExpPool(int iConstelExpPool)
    {
        if (m_stData.m_iConstelExpPool == iConstelExpPool)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConstelExpPool - m_stData.m_iConstelExpPool;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelExpPool(RealAdd);
        m_stData.m_iConstelExpPool = iConstelExpPool;
    }
    const int& GetFishingRod() const
    {
        return m_stData.m_iFishingRod;
    }
    void AddFishingRod(int iFishingRod)
    {
        if (0 == iFishingRod)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFishingRod(iFishingRod);
        m_stData.m_iFishingRod += iFishingRod;
    }
    void SetFishingRod(int iFishingRod)
    {
        if (m_stData.m_iFishingRod == iFishingRod)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFishingRod - m_stData.m_iFishingRod;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFishingRod(RealAdd);
        m_stData.m_iFishingRod = iFishingRod;
    }
    const std::list<int32_t>& GetMainBgm() const
    {
        return m_stData.m_stMainBgm;
    }
    void SetMainBgm(const std::list<int32_t>& stMainBgm)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMainBgm(stMainBgm);
        m_stData.m_stMainBgm = stMainBgm;
    }
    const std::list<int32_t>& GetHomeBgm() const
    {
        return m_stData.m_stHomeBgm;
    }
    void SetHomeBgm(const std::list<int32_t>& stHomeBgm)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHomeBgm(stHomeBgm);
        m_stData.m_stHomeBgm = stHomeBgm;
    }
    const int& GetMallBaodiTimes() const
    {
        return m_stData.m_iMallBaodiTimes;
    }
    void AddMallBaodiTimes(int iMallBaodiTimes)
    {
        if (0 == iMallBaodiTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMallBaodiTimes(iMallBaodiTimes);
        m_stData.m_iMallBaodiTimes += iMallBaodiTimes;
    }
    void SetMallBaodiTimes(int iMallBaodiTimes)
    {
        if (m_stData.m_iMallBaodiTimes == iMallBaodiTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMallBaodiTimes - m_stData.m_iMallBaodiTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMallBaodiTimes(RealAdd);
        m_stData.m_iMallBaodiTimes = iMallBaodiTimes;
    }
    const int& GetZhuHaiLevel() const
    {
        return m_stData.m_iZhuHaiLevel;
    }
    void AddZhuHaiLevel(int iZhuHaiLevel)
    {
        if (0 == iZhuHaiLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiLevel(iZhuHaiLevel);
        m_stData.m_iZhuHaiLevel += iZhuHaiLevel;
    }
    void SetZhuHaiLevel(int iZhuHaiLevel)
    {
        if (m_stData.m_iZhuHaiLevel == iZhuHaiLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iZhuHaiLevel - m_stData.m_iZhuHaiLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiLevel(RealAdd);
        m_stData.m_iZhuHaiLevel = iZhuHaiLevel;
    }
    const int& GetZhuHaiGress() const
    {
        return m_stData.m_iZhuHaiGress;
    }
    void AddZhuHaiGress(int iZhuHaiGress)
    {
        if (0 == iZhuHaiGress)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiGress(iZhuHaiGress);
        m_stData.m_iZhuHaiGress += iZhuHaiGress;
    }
    void SetZhuHaiGress(int iZhuHaiGress)
    {
        if (m_stData.m_iZhuHaiGress == iZhuHaiGress)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iZhuHaiGress - m_stData.m_iZhuHaiGress;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiGress(RealAdd);
        m_stData.m_iZhuHaiGress = iZhuHaiGress;
    }
    const std::vector<int>& GetZhuHaiBuff() const
    {
        return m_stData.m_stZhuHaiBuff;
    }
    void SetZhuHaiBuff(const std::vector<int>& stZhuHaiBuff)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiBuff(stZhuHaiBuff);
        m_stData.m_stZhuHaiBuff = stZhuHaiBuff;
    }
    const std::map<int, int>& GetZhuHaiCardHp() const
    {
        return m_stData.m_stZhuHaiCardHp;
    }
    void SetZhuHaiCardHp(const std::map<int, int>& stZhuHaiCardHp)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiCardHp(stZhuHaiCardHp);
        m_stData.m_stZhuHaiCardHp = stZhuHaiCardHp;
    }
    const std::map<int, CPackZhuHaiBlessInfo>& GetZhuHaiBlessGroup() const
    {
        return m_stData.m_stZhuHaiBlessGroup;
    }
    void SetZhuHaiBlessGroup(const std::map<int, CPackZhuHaiBlessInfo>& stZhuHaiBlessGroup)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiBlessGroup(stZhuHaiBlessGroup);
        m_stData.m_stZhuHaiBlessGroup = stZhuHaiBlessGroup;
    }
    const std::map<int,std::vector<int>>& GetZhuHaiRoleBuff() const
    {
        return m_stData.m_stZhuHaiRoleBuff;
    }
    void SetZhuHaiRoleBuff(const std::map<int,std::vector<int>>& stZhuHaiRoleBuff)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiRoleBuff(stZhuHaiRoleBuff);
        m_stData.m_stZhuHaiRoleBuff = stZhuHaiRoleBuff;
    }
    const int& GetZhuHaiRefreshStore() const
    {
        return m_stData.m_iZhuHaiRefreshStore;
    }
    void AddZhuHaiRefreshStore(int iZhuHaiRefreshStore)
    {
        if (0 == iZhuHaiRefreshStore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiRefreshStore(iZhuHaiRefreshStore);
        m_stData.m_iZhuHaiRefreshStore += iZhuHaiRefreshStore;
    }
    void SetZhuHaiRefreshStore(int iZhuHaiRefreshStore)
    {
        if (m_stData.m_iZhuHaiRefreshStore == iZhuHaiRefreshStore)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iZhuHaiRefreshStore - m_stData.m_iZhuHaiRefreshStore;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiRefreshStore(RealAdd);
        m_stData.m_iZhuHaiRefreshStore = iZhuHaiRefreshStore;
    }
    const char& GetZhuHaiSaveTeamFlag() const
    {
        return m_stData.m_cZhuHaiSaveTeamFlag;
    }
    void AddZhuHaiSaveTeamFlag(char cZhuHaiSaveTeamFlag)
    {
        if (0 == cZhuHaiSaveTeamFlag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiSaveTeamFlag(cZhuHaiSaveTeamFlag);
        m_stData.m_cZhuHaiSaveTeamFlag += cZhuHaiSaveTeamFlag;
    }
    void SetZhuHaiSaveTeamFlag(char cZhuHaiSaveTeamFlag)
    {
        if (m_stData.m_cZhuHaiSaveTeamFlag == cZhuHaiSaveTeamFlag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cZhuHaiSaveTeamFlag - m_stData.m_cZhuHaiSaveTeamFlag;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetZhuHaiSaveTeamFlag(RealAdd);
        m_stData.m_cZhuHaiSaveTeamFlag = cZhuHaiSaveTeamFlag;
    }
    const std::map<int, uint64_t>& GetGuildBossHurt() const
    {
        return m_stData.m_stGuildBossHurt;
    }
    void SetGuildBossHurt(const std::map<int, uint64_t>& stGuildBossHurt)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildBossHurt(stGuildBossHurt);
        m_stData.m_stGuildBossHurt = stGuildBossHurt;
    }
    const int& GetWishLastTime() const
    {
        return m_stData.m_iWishLastTime;
    }
    void AddWishLastTime(int iWishLastTime)
    {
        if (0 == iWishLastTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWishLastTime(iWishLastTime);
        m_stData.m_iWishLastTime += iWishLastTime;
    }
    void SetWishLastTime(int iWishLastTime)
    {
        if (m_stData.m_iWishLastTime == iWishLastTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWishLastTime - m_stData.m_iWishLastTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWishLastTime(RealAdd);
        m_stData.m_iWishLastTime = iWishLastTime;
    }
    const int& GetWishCardID() const
    {
        return m_stData.m_iWishCardID;
    }
    void AddWishCardID(int iWishCardID)
    {
        if (0 == iWishCardID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWishCardID(iWishCardID);
        m_stData.m_iWishCardID += iWishCardID;
    }
    void SetWishCardID(int iWishCardID)
    {
        if (m_stData.m_iWishCardID == iWishCardID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWishCardID - m_stData.m_iWishCardID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWishCardID(RealAdd);
        m_stData.m_iWishCardID = iWishCardID;
    }
    const int& GetWishNum() const
    {
        return m_stData.m_iWishNum;
    }
    void AddWishNum(int iWishNum)
    {
        if (0 == iWishNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWishNum(iWishNum);
        m_stData.m_iWishNum += iWishNum;
    }
    void SetWishNum(int iWishNum)
    {
        if (m_stData.m_iWishNum == iWishNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iWishNum - m_stData.m_iWishNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWishNum(RealAdd);
        m_stData.m_iWishNum = iWishNum;
    }
    const std::vector<std::pair<std::string, int>>& GetWishRewardVec() const
    {
        return m_stData.m_stWishRewardVec;
    }
    void SetWishRewardVec(const std::vector<std::pair<std::string, int>>& stWishRewardVec)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWishRewardVec(stWishRewardVec);
        m_stData.m_stWishRewardVec = stWishRewardVec;
    }
    const std::vector<int>& GetWishSwapCard() const
    {
        return m_stData.m_stWishSwapCard;
    }
    void SetWishSwapCard(const std::vector<int>& stWishSwapCard)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWishSwapCard(stWishSwapCard);
        m_stData.m_stWishSwapCard = stWishSwapCard;
    }
    const std::vector<int>& GetBadgeList() const
    {
        return m_stData.m_stBadgeList;
    }
    void SetBadgeList(const std::vector<int>& stBadgeList)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBadgeList(stBadgeList);
        m_stData.m_stBadgeList = stBadgeList;
    }
    const std::map<int32_t,std::vector<CPackLimitBossTeamInfo>>& GetNewLimitBossReward() const
    {
        return m_stData.m_stNewLimitBossReward;
    }
    void SetNewLimitBossReward(const std::map<int32_t,std::vector<CPackLimitBossTeamInfo>>& stNewLimitBossReward)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewLimitBossReward(stNewLimitBossReward);
        m_stData.m_stNewLimitBossReward = stNewLimitBossReward;
    }
    const int& GetNewLimitBossDailyCount() const
    {
        return m_stData.m_iNewLimitBossDailyCount;
    }
    void AddNewLimitBossDailyCount(int iNewLimitBossDailyCount)
    {
        if (0 == iNewLimitBossDailyCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewLimitBossDailyCount(iNewLimitBossDailyCount);
        m_stData.m_iNewLimitBossDailyCount += iNewLimitBossDailyCount;
    }
    void SetNewLimitBossDailyCount(int iNewLimitBossDailyCount)
    {
        if (m_stData.m_iNewLimitBossDailyCount == iNewLimitBossDailyCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNewLimitBossDailyCount - m_stData.m_iNewLimitBossDailyCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNewLimitBossDailyCount(RealAdd);
        m_stData.m_iNewLimitBossDailyCount = iNewLimitBossDailyCount;
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
    const char& GetDailyGuildSign() const
    {
        return m_stData.m_cDailyGuildSign;
    }
    void AddDailyGuildSign(char cDailyGuildSign)
    {
        if (0 == cDailyGuildSign)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildSign(cDailyGuildSign);
        m_stData.m_cDailyGuildSign += cDailyGuildSign;
    }
    void SetDailyGuildSign(char cDailyGuildSign)
    {
        if (m_stData.m_cDailyGuildSign == cDailyGuildSign)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cDailyGuildSign - m_stData.m_cDailyGuildSign;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildSign(RealAdd);
        m_stData.m_cDailyGuildSign = cDailyGuildSign;
    }
    const int& GetDailyGuildRandomCount() const
    {
        return m_stData.m_iDailyGuildRandomCount;
    }
    void AddDailyGuildRandomCount(int iDailyGuildRandomCount)
    {
        if (0 == iDailyGuildRandomCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildRandomCount(iDailyGuildRandomCount);
        m_stData.m_iDailyGuildRandomCount += iDailyGuildRandomCount;
    }
    void SetDailyGuildRandomCount(int iDailyGuildRandomCount)
    {
        if (m_stData.m_iDailyGuildRandomCount == iDailyGuildRandomCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDailyGuildRandomCount - m_stData.m_iDailyGuildRandomCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildRandomCount(RealAdd);
        m_stData.m_iDailyGuildRandomCount = iDailyGuildRandomCount;
    }
    const CPackGuildRandomInfo& GetDailyGuildRandom() const
    {
        return m_stData.m_stDailyGuildRandom;
    }
    void SetDailyGuildRandom(const CPackGuildRandomInfo& stDailyGuildRandom)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildRandom(stDailyGuildRandom);
        m_stData.m_stDailyGuildRandom = stDailyGuildRandom;
    }
    const char& GetDailyGuildBar() const
    {
        return m_stData.m_cDailyGuildBar;
    }
    void AddDailyGuildBar(char cDailyGuildBar)
    {
        if (0 == cDailyGuildBar)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildBar(cDailyGuildBar);
        m_stData.m_cDailyGuildBar += cDailyGuildBar;
    }
    void SetDailyGuildBar(char cDailyGuildBar)
    {
        if (m_stData.m_cDailyGuildBar == cDailyGuildBar)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cDailyGuildBar - m_stData.m_cDailyGuildBar;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildBar(RealAdd);
        m_stData.m_cDailyGuildBar = cDailyGuildBar;
    }
    const char& GetWenwenChoose() const
    {
        return m_stData.m_cWenwenChoose;
    }
    void AddWenwenChoose(char cWenwenChoose)
    {
        if (0 == cWenwenChoose)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWenwenChoose(cWenwenChoose);
        m_stData.m_cWenwenChoose += cWenwenChoose;
    }
    void SetWenwenChoose(char cWenwenChoose)
    {
        if (m_stData.m_cWenwenChoose == cWenwenChoose)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cWenwenChoose - m_stData.m_cWenwenChoose;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetWenwenChoose(RealAdd);
        m_stData.m_cWenwenChoose = cWenwenChoose;
    }
    const int& GetConstelAdvTimes() const
    {
        return m_stData.m_iConstelAdvTimes;
    }
    void AddConstelAdvTimes(int iConstelAdvTimes)
    {
        if (0 == iConstelAdvTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelAdvTimes(iConstelAdvTimes);
        m_stData.m_iConstelAdvTimes += iConstelAdvTimes;
    }
    void SetConstelAdvTimes(int iConstelAdvTimes)
    {
        if (m_stData.m_iConstelAdvTimes == iConstelAdvTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConstelAdvTimes - m_stData.m_iConstelAdvTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelAdvTimes(RealAdd);
        m_stData.m_iConstelAdvTimes = iConstelAdvTimes;
    }
    const std::vector<CPackGuildFragmentsHistory>& GetGuildFragmentHistory() const
    {
        return m_stData.m_stGuildFragmentHistory;
    }
    void SetGuildFragmentHistory(const std::vector<CPackGuildFragmentsHistory>& stGuildFragmentHistory)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildFragmentHistory(stGuildFragmentHistory);
        m_stData.m_stGuildFragmentHistory = stGuildFragmentHistory;
    }
    const std::map<int,bool>& GetHomeExpTag() const
    {
        return m_stData.m_stHomeExpTag;
    }
    void SetHomeExpTag(const std::map<int,bool>& stHomeExpTag)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHomeExpTag(stHomeExpTag);
        m_stData.m_stHomeExpTag = stHomeExpTag;
    }
    const std::map<int,CPackRuneInfo>& GetRuneInfo() const
    {
        return m_stData.m_stRuneInfo;
    }
    void SetRuneInfo(const std::map<int,CPackRuneInfo>& stRuneInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetRuneInfo(stRuneInfo);
        m_stData.m_stRuneInfo = stRuneInfo;
    }
    const std::map<int,map<int,char>>& GetConstelPutInfo() const
    {
        return m_stData.m_stConstelPutInfo;
    }
    void SetConstelPutInfo(const std::map<int,map<int,char>>& stConstelPutInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelPutInfo(stConstelPutInfo);
        m_stData.m_stConstelPutInfo = stConstelPutInfo;
    }
    const char& GetDataChangeTag() const
    {
        return m_stData.m_cDataChangeTag;
    }
    void AddDataChangeTag(char cDataChangeTag)
    {
        if (0 == cDataChangeTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDataChangeTag(cDataChangeTag);
        m_stData.m_cDataChangeTag += cDataChangeTag;
    }
    void SetDataChangeTag(char cDataChangeTag)
    {
        if (m_stData.m_cDataChangeTag == cDataChangeTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cDataChangeTag - m_stData.m_cDataChangeTag;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDataChangeTag(RealAdd);
        m_stData.m_cDataChangeTag = cDataChangeTag;
    }
    const char& GetCbtRewardTag() const
    {
        return m_stData.m_cCbtRewardTag;
    }
    void AddCbtRewardTag(char cCbtRewardTag)
    {
        if (0 == cCbtRewardTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCbtRewardTag(cCbtRewardTag);
        m_stData.m_cCbtRewardTag += cCbtRewardTag;
    }
    void SetCbtRewardTag(char cCbtRewardTag)
    {
        if (m_stData.m_cCbtRewardTag == cCbtRewardTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cCbtRewardTag - m_stData.m_cCbtRewardTag;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCbtRewardTag(RealAdd);
        m_stData.m_cCbtRewardTag = cCbtRewardTag;
    }
    const std::vector<CPackConstelShopInfo>& GetConstelShop() const
    {
        return m_stData.m_stConstelShop;
    }
    void SetConstelShop(const std::vector<CPackConstelShopInfo>& stConstelShop)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelShop(stConstelShop);
        m_stData.m_stConstelShop = stConstelShop;
    }
    const int& GetConstelShopRefreshCount() const
    {
        return m_stData.m_iConstelShopRefreshCount;
    }
    void AddConstelShopRefreshCount(int iConstelShopRefreshCount)
    {
        if (0 == iConstelShopRefreshCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelShopRefreshCount(iConstelShopRefreshCount);
        m_stData.m_iConstelShopRefreshCount += iConstelShopRefreshCount;
    }
    void SetConstelShopRefreshCount(int iConstelShopRefreshCount)
    {
        if (m_stData.m_iConstelShopRefreshCount == iConstelShopRefreshCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConstelShopRefreshCount - m_stData.m_iConstelShopRefreshCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelShopRefreshCount(RealAdd);
        m_stData.m_iConstelShopRefreshCount = iConstelShopRefreshCount;
    }
    const int& GetConstelShopLastTime() const
    {
        return m_stData.m_iConstelShopLastTime;
    }
    void AddConstelShopLastTime(int iConstelShopLastTime)
    {
        if (0 == iConstelShopLastTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelShopLastTime(iConstelShopLastTime);
        m_stData.m_iConstelShopLastTime += iConstelShopLastTime;
    }
    void SetConstelShopLastTime(int iConstelShopLastTime)
    {
        if (m_stData.m_iConstelShopLastTime == iConstelShopLastTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConstelShopLastTime - m_stData.m_iConstelShopLastTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelShopLastTime(RealAdd);
        m_stData.m_iConstelShopLastTime = iConstelShopLastTime;
    }
    const int& GetCheatingCount() const
    {
        return m_stData.m_iCheatingCount;
    }
    void AddCheatingCount(int iCheatingCount)
    {
        if (0 == iCheatingCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCheatingCount(iCheatingCount);
        m_stData.m_iCheatingCount += iCheatingCount;
    }
    void SetCheatingCount(int iCheatingCount)
    {
        if (m_stData.m_iCheatingCount == iCheatingCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCheatingCount - m_stData.m_iCheatingCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCheatingCount(RealAdd);
        m_stData.m_iCheatingCount = iCheatingCount;
    }
    const std::map<int32_t,std::map<int32_t,CPackGashaponInfo>>& GetGashaponInfo() const
    {
        return m_stData.m_stGashaponInfo;
    }
    void SetGashaponInfo(const std::map<int32_t,std::map<int32_t,CPackGashaponInfo>>& stGashaponInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGashaponInfo(stGashaponInfo);
        m_stData.m_stGashaponInfo = stGashaponInfo;
    }
    const int& GetCrushBossLevel() const
    {
        return m_stData.m_iCrushBossLevel;
    }
    void AddCrushBossLevel(int iCrushBossLevel)
    {
        if (0 == iCrushBossLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrushBossLevel(iCrushBossLevel);
        m_stData.m_iCrushBossLevel += iCrushBossLevel;
    }
    void SetCrushBossLevel(int iCrushBossLevel)
    {
        if (m_stData.m_iCrushBossLevel == iCrushBossLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCrushBossLevel - m_stData.m_iCrushBossLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrushBossLevel(RealAdd);
        m_stData.m_iCrushBossLevel = iCrushBossLevel;
    }
    const CPackCrushBossInfo& GetCrushBossInfo() const
    {
        return m_stData.m_stCrushBossInfo;
    }
    void SetCrushBossInfo(const CPackCrushBossInfo& stCrushBossInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrushBossInfo(stCrushBossInfo);
        m_stData.m_stCrushBossInfo = stCrushBossInfo;
    }
    const int& GetCrushBossCount() const
    {
        return m_stData.m_iCrushBossCount;
    }
    void AddCrushBossCount(int iCrushBossCount)
    {
        if (0 == iCrushBossCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrushBossCount(iCrushBossCount);
        m_stData.m_iCrushBossCount += iCrushBossCount;
    }
    void SetCrushBossCount(int iCrushBossCount)
    {
        if (m_stData.m_iCrushBossCount == iCrushBossCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCrushBossCount - m_stData.m_iCrushBossCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrushBossCount(RealAdd);
        m_stData.m_iCrushBossCount = iCrushBossCount;
    }
    const std::map<int32_t,CPackHomeGoodsBuffEffect>& GetHomeBuffID() const
    {
        return m_stData.m_stHomeBuffID;
    }
    void SetHomeBuffID(const std::map<int32_t,CPackHomeGoodsBuffEffect>& stHomeBuffID)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHomeBuffID(stHomeBuffID);
        m_stData.m_stHomeBuffID = stHomeBuffID;
    }
    const int& GetStartLevelTime() const
    {
        return m_stData.m_iStartLevelTime;
    }
    void AddStartLevelTime(int iStartLevelTime)
    {
        if (0 == iStartLevelTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetStartLevelTime(iStartLevelTime);
        m_stData.m_iStartLevelTime += iStartLevelTime;
    }
    void SetStartLevelTime(int iStartLevelTime)
    {
        if (m_stData.m_iStartLevelTime == iStartLevelTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iStartLevelTime - m_stData.m_iStartLevelTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetStartLevelTime(RealAdd);
        m_stData.m_iStartLevelTime = iStartLevelTime;
    }
    const std::map<int32_t,int32_t>& GetAtlasBuffInfo() const
    {
        return m_stData.m_stAtlasBuffInfo;
    }
    void SetAtlasBuffInfo(const std::map<int32_t,int32_t>& stAtlasBuffInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAtlasBuffInfo(stAtlasBuffInfo);
        m_stData.m_stAtlasBuffInfo = stAtlasBuffInfo;
    }
    const char& GetDailyGuildWarTime() const
    {
        return m_stData.m_cDailyGuildWarTime;
    }
    void AddDailyGuildWarTime(char cDailyGuildWarTime)
    {
        if (0 == cDailyGuildWarTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildWarTime(cDailyGuildWarTime);
        m_stData.m_cDailyGuildWarTime += cDailyGuildWarTime;
    }
    void SetDailyGuildWarTime(char cDailyGuildWarTime)
    {
        if (m_stData.m_cDailyGuildWarTime == cDailyGuildWarTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cDailyGuildWarTime - m_stData.m_cDailyGuildWarTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGuildWarTime(RealAdd);
        m_stData.m_cDailyGuildWarTime = cDailyGuildWarTime;
    }
    const std::map<int32_t,char>& GetGuildWarUseCard() const
    {
        return m_stData.m_stGuildWarUseCard;
    }
    void SetGuildWarUseCard(const std::map<int32_t,char>& stGuildWarUseCard)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildWarUseCard(stGuildWarUseCard);
        m_stData.m_stGuildWarUseCard = stGuildWarUseCard;
    }
    const std::map<int32_t,CPackGuildWarSelfInfo>& GetGuildSelfInfo() const
    {
        return m_stData.m_stGuildSelfInfo;
    }
    void SetGuildSelfInfo(const std::map<int32_t,CPackGuildWarSelfInfo>& stGuildSelfInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildSelfInfo(stGuildSelfInfo);
        m_stData.m_stGuildSelfInfo = stGuildSelfInfo;
    }
    const CPackGuildWarSelfRankInfo& GetGuildWarCalInfo() const
    {
        return m_stData.m_stGuildWarCalInfo;
    }
    void SetGuildWarCalInfo(const CPackGuildWarSelfRankInfo& stGuildWarCalInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGuildWarCalInfo(stGuildWarCalInfo);
        m_stData.m_stGuildWarCalInfo = stGuildWarCalInfo;
    }
    const int& GetLimitBossBaodiTime() const
    {
        return m_stData.m_iLimitBossBaodiTime;
    }
    void AddLimitBossBaodiTime(int iLimitBossBaodiTime)
    {
        if (0 == iLimitBossBaodiTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBossBaodiTime(iLimitBossBaodiTime);
        m_stData.m_iLimitBossBaodiTime += iLimitBossBaodiTime;
    }
    void SetLimitBossBaodiTime(int iLimitBossBaodiTime)
    {
        if (m_stData.m_iLimitBossBaodiTime == iLimitBossBaodiTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLimitBossBaodiTime - m_stData.m_iLimitBossBaodiTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLimitBossBaodiTime(RealAdd);
        m_stData.m_iLimitBossBaodiTime = iLimitBossBaodiTime;
    }

    CRecordUserIBusyInfoTable* m_pRecord;
    TUserIbusyInfoTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserIBusyInfoTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserIBusyInfoTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserIBusyInfoTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
