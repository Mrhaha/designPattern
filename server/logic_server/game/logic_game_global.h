//
// Created by hzl on 2019/5/28.
//

#pragma once

#include "server_award_protocol.h"
#include "i_logic_connector_sender.h"
#include "logic_game_global_cache.h"
#include "logic_game_cache_table.h"
#include "logic_game_buff.h"
#include "logic_game_user.h"
#include "logic_game_bill.h"

class CLogicGlobal
{
public:
    static bool Initialize();

    static void Refresh();

    static void OnCrossOneHour();

    static void OnCrossOneDay();

    static void OnCrossOneWeek();

    static void OnServerMerge();

    static ILogicConnectorSender* GetConnectorSender();

    static TLogicGlobalCache* GetGlobalCache();

private:
    static void LoadRecoveryUser();

    static TLogicGlobalCache* GetGlobalCacheFromDB();

/************************************************************************************************************/
/******************************************排    行    榜*****************************************************/
/************************************************************************************************************/
public:
    static uint32_t UpdateOrInsertCharmRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

    static uint32_t UpdateOrInsertCountdownRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

    static uint32_t UpdateOrInsertEntireWarRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

    static uint32_t UpdateOrInsertGuildRank(int32_t iGuildID, uint64_t ulRankScore);

    static uint32_t UpdateOrInsertHomeRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

    static uint32_t UpdateOrInsertIbusyRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

    static uint32_t UpdateOrInsertMatchRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

    static uint32_t UpdateOrInsertMilitaryRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

    static uint32_t UpdateOrInsertPowerRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

    static uint32_t UpdateOrInsertPvpRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

    static uint32_t UpdateOrInsertTowerRank(int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

private:
    static void RefreshAwardRankList();

    static int32_t AwardPvpRankList();

    static int32_t AwardFightPowerActiveRankList();

    static int32_t AwardCountdownHeroRankList();

    static int32_t AwardIbusyRankList(int32_t iBusyActivityID);

    template <typename RankContainer>
    static uint32_t UpdateOrInsertUserRank(RankContainer& stRankIns, int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);

    // 通用排行榜
public:
    static uint32_t UpdateOrInsertCommonRank(int32_t iRankID, int32_t iUid, int32_t iGroupID, uint64_t ulRankScore);
    static uint32_t GetRankByCommonRank(int32_t iRankID, int32_t iUid, int32_t iGroupID);

private:
    static void RefreshCommonRank(int32_t iRefreshType);
    static int32_t AwardCommonRank(int32_t iRankID);
    static void RefreshEquipLevelAffix();
    static void RefreshPvpRank();

/************************************************************************************************************/
/******************************************缤  纷  彩  灯*****************************************************/
/************************************************************************************************************/
public:
    static bool RefreshMultiColorOpen();

    static bool MultiColorOpenCardVec();

    static int32_t GetMultiColorNextOpenTime(int32_t iLastOpenTime);

    static int32_t GetGuessMultiColorCount(const std::vector<int32_t>& stCardVec);

    static const std::vector<int32_t>& GetUserSelectLanternVec(int32_t iUin, int32_t iGroupID);

    static void SetUserSelectLanternVec(int32_t iUin, int32_t iGroupID, const std::vector<int32_t>& stCardVec);

private:
    static uint32_t GetMultiColorSeedAssist();

/************************************************************************************************************/
/******************************************招    财    猫*****************************************************/
/************************************************************************************************************/
public:
    static std::list<CPackLuckyCatHistory> GetLuckCatHistory();

    static void AddLuckCatHistory(const CPackLuckyCatHistory& stHistory);

/************************************************************************************************************/
/******************************************诸  葛  钱  庄*****************************************************/
/************************************************************************************************************/
public:
    static int32_t GetLastBankPrice(int32_t iCountry);

    static const std::list<CPackBankPrice>& GetBankPriceList();

    static int32_t GetBankNextRefreshTime();

    static std::string GetBankPriceEvent();

private:
    static bool RefreshBankPrice();

    static void AddBankPriceNode(int32_t iTimeStamp, int32_t iWeiPrice, int32_t iShuPrice, int32_t iWuPrice);

/************************************************************************************************************/
/******************************************聊  天  广  播*****************************************************/
/************************************************************************************************************/
public:
    static void PublishBuyMallBroadcast(int32_t iCommodityID, const std::string &strNick,
                                        const std::vector<CPackGameItem> &rstGameItemList);

    static void PublishGashaponMachinesMallBroadcast(int32_t iCommodityID, const std::string &strNick, const CPackGameItem &stItem, int iNotify);

    static void PublishCrossFightBroadcast(const std::string &strNick, const std::vector<CPackGameItem> &rstGameItemList);

    static void PublishBroadcast(const std::string &strBroadcastID, int32_t iTriggerValue, int32_t iNotifyLevel,
                                 const std::map<std::string, std::string> &rstParamMap,
                                 const std::vector<int> &rstJumpParams = {}, int32_t iPara = 0, int32_t iCmd = 0);

    static void PublishBroadcast(const std::string &strBroadcastID, int32_t iTriggerValue,
                                 const std::map<std::string, std::string> &rstParamMap,
                                 const std::vector<int> &rstJumpParams = {});

    static void PublishMixBroadcast(const std::string &strBroadcastID, int32_t iBroadcastIndex, int32_t iTriggerValue,
                                    int32_t iNotifyLevel, const std::map<std::string, std::string> &rstParamMap,
                                    const std::vector<int> &rstJumpParams = {});

    static void PublishMixBroadcast(const std::string &strBroadcastID, int32_t iBroadcastIndex, int32_t iTriggerValue,
                                    const std::map<std::string, std::string> &rstParamMap,
                                    const std::vector<int> &rstJumpParams = {});

    static void PublishRewardCardBroadcast(const std::string& strNick, int32_t iCardID, int32_t iCmd);
    static void PublishDepositBroadcast(const std::string& strNick, int32_t iDepositID);

    static void PublishWishCoinBroadcast(const std::string& strNick, int32_t iCardID);

    template<typename MSGPACK_T>
    static void BroadcastToClient(int iCmd, MSGPACK_T& stMsgPack, int32_t iChannelID = 0, int iErrcode = 0);

private:
    static void RefreshBroadcast();

    static void PublishSystemBulletins();

    static void PublishActivityBroadcast();

    static bool IsInActiveBroadcastTime(int broadcastID);

    static void PublishBroadcast(TGlobalBroadcastContent stContent);

/************************************************************************************************************/
/******************************************邮  件  系  统*****************************************************/
/************************************************************************************************************/
public:
    static bool AddGlobalMail(int32_t iBeginTime, int32_t iEndTime, int32_t iVipLevel, const std::string& strSender, const std::string& strTitle,
                            const std::string& strContent, const std::vector<CPackGameItem>& stGameItemList, const std::set<int32_t>& stChannelVec = {});

    static void DelGlobalMail(int32_t iBeginTime, int32_t iMailID);

    static void AddAsyncUserMail(int32_t iUin, int32_t iGroupID, const std::string &strSender, const std::string &strTitle, const std::string &strContent,
                             const std::vector<CPackGameItem> &stGameItemList, const std::string &strCmdName, int32_t iAwardMultiple = 1, bool bSync = false,int32_t iExpiredTime = 0);

    static void AddAsyncUserMail(const TGlobalAsyncUserMail& stMail, bool bSync = false);

    static bool AddAsyncEventMail(int32_t iUin, int32_t iGroupID, const std::string& strEventID, const std::map<std::string, std::string>& stValMap, int32_t iAwardMultiple = 1);

    static void AddAsyncNpcMail(int32_t iUin, int32_t iGroupID);

    static void AddAsyncUserGlobalMail(int32_t iUin, int32_t iGroupID);

    static void SendAllAsyncUserMail();

private:
    static void DelMergedGlobalMail();

    static void RefreshGlobalMail();

    static void SendOneAsyncUserMail(bool bForce = false);

    static void CheckNewGlobalMail();

    static void BroadcastHasNewGlobalMail();

/************************************************************************************************************/
/******************************************全  民  战  役*****************************************************/
/************************************************************************************************************/
public:
    static int32_t GetEntireWarThemeID();

    static int32_t GetEntireWarLevel();

    static CPackUserKey GetEntireWarGodKey();

    static const std::vector<CPackEntireWarBuff>& GetEntireWarBuff();

    static void AddNewEntireWarBuff(int32_t iBuffType, int32_t iBuffPercentage, int32_t iBuffTime);

    static bool RefreshEntireWarNodeProgress(int32_t iWarID, uint64_t ulProgress, bool isSet = false);

    static bool IsEntireWarPass(int32_t iWarID);

    static std::pair<std::time_t, std::time_t> GetEntireWarActivityTime() { return m_stEntireWarAvyTime; }

    static std::vector<CPackMarket> GetEntireWarMarketPack(int32_t iWarID);

    static const TGlobalEntireMapTableValueType* GetEntireWarMapInfo(int32_t iWarID);

    static const TGlobalEntireExtraTableValueType* GetEntireWarExtraInfo(int32_t iWarID, int32_t iExtraID);


private:
    static void RefreshEntireWarActivityTime();

    static void RefreshEntireWar();

    static void RefreshWorldEntireWar();

    static void RefreshEntireWarGodData();

    static void BuildEntireWarExtraData(int32_t iWarID);

    static void BuildEntireWarMarketExtra(int32_t iWarID);

    static void BuildEntireWarTreasureExtra(int32_t iWarID);

    static void AwardEntireWarRankList();

    static void BuildNewEntireWar();

    static void BuildMergedEntireWar();

    static TGlobalEntireMapTable* GetEntireWarMapTable(int32_t iWarID);

    static TGlobalEntireExtraTable* GetEntireWarExtraTable(int32_t iWarID, int32_t iExtraID);

/************************************************************************************************************/
/******************************************全  服  战  报*****************************************************/
/************************************************************************************************************/
public:
    static bool AddGlobalFightReport(CPackFightReportItem &stReportItem);

    static bool ClearGlobalFightReport(int32_t iFightReportType);

/************************************************************************************************************/
/******************************************防    沉    迷*****************************************************/
/************************************************************************************************************/
public:
    // 防沉迷更新玩家年龄
    static void AntiAddNotifyUpdateUserAge(int32_t iUin, int32_t iGroupID);

    // 检查防沉迷用户是否在可登陆时间，或者每日在线时间是否已满
    static int AntiAddCheckCanLogin(int32_t iUin, int32_t iGroupID, int32_t iAge, bool bHandleVisitor = false);

    // 检查是否为防沉迷用户
    static void AntiAddCheckUser(int32_t iUin, int32_t iGroupID, int32_t iAge);

    // 防沉迷提示
    static bool AntiAddHandleNotify(int32_t iUin, int32_t iGroupID, int32_t iAge, int32_t iOnlineTime, bool bHandleVisitor = false, int32_t iInterval = 20);

    // 防沉迷检测在线超时踢下线,不做游客模式的检查
    static bool AntiAddHandleOnlineTime(int32_t iUin, int32_t iGroupID, int32_t iOnlineTime);

    // 游客模式防沉迷单独处理
    static bool AntiAddHandleVisitor(int32_t iUin, int32_t iGroupID);

    // 更新防沉迷用户在线时间
    static void AntiAddUpdateOnlineTime(int32_t iInterval);

    // 防沉迷跨天更新
    static void AntiAddCrossDayRefreshData();

/************************************************************************************************************/
/******************************************文  文  日  报*****************************************************/
/************************************************************************************************************/
public:
    static void UpdateWenWenNewsEvent(int32_t iUin, int32_t iGroupID, int32_t iType, std::vector<std::string> iPara);

private:
    static void UpdateWenWenNewsPersonData(int32_t iUin, int32_t iGroupID, int32_t iPara, int32_t iParaEx, int32_t iID, int32_t iType, int32_t iLimitType);
    static void UpdateWenWenNewsServerData(int32_t iUin, int32_t iGroupID, int32_t iIndexID,int32_t iType, std::vector<std::string> iPara, int32_t iID, int8_t iLimitType);

/************************************************************************************************************/
/******************************************公  平  竞  技*****************************************************/
/************************************************************************************************************/
public:
    static std::vector<int32_t> FairPvpRandomOptionCard(user_data_table_ptr_type pUserInfo);

/************************************************************************************************************/
/******************************************伙  伴  传  记*****************************************************/
/************************************************************************************************************/
public:
    static void UpdateHeroHistory(user_data_table_ptr_type pUserInfo, int32_t iCardID);

/************************************************************************************************************/
/******************************************问  卷  调  查*****************************************************/
/************************************************************************************************************/
public:
    static void UpdateQuestionnaire(user_data_table_ptr_type pUserInfo, bool bForceNotify = false);

private:
    static bool CheckQuestionnaireOpenType(user_data_table_ptr_type pUserInfo, int32_t iType, int32_t iPara, int32_t iParaEx);

/************************************************************************************************************/
/******************************************排队服务器通知*****************************************************/
/************************************************************************************************************/
public:
    static void NotifyQueueSvrUserLogin(int32_t iUin, bool bLogin, bool bProtect = true);
    static void NotifyQueueSvrUserListOnline();
    static void NotifyQueueSvrReconnectCheck(int32_t iUin);

/************************************************************************************************************/
/******************************************  其     他  *****************************************************/
/************************************************************************************************************/
public:
    static void NotifyCrossUserBriefUpdate(user_data_table_ptr_type pUserInfo, bool bLogin);

/************************************************************************************************************/
/******************************************  付费礼包  *****************************************************/
/************************************************************************************************************/
public:
    static void RefreshCrossUserEmailReward();
/************************************************************************************************************/
/******************************************  家园拜访  *****************************************************/
/************************************************************************************************************/
public:
    static void NotifyCrossUserVisitorLogout(user_data_table_ptr_type pUserInfo, CRequestHomeVisit visitInfo);


/************************************************************************************************************/
/******************************************静  态  成  员*****************************************************/
/************************************************************************************************************/
private:
    static std::pair<std::time_t, std::time_t> m_stEntireWarAvyTime;
};

template<typename MSGPACK_T>
void CLogicGlobal::BroadcastToClient(int iCmd, MSGPACK_T &stMsgPack, int32_t iChannelID, int iErrcode)
{
    for (auto iter = CLogicUser::GetInstance().Begin(); iter != CLogicUser::GetInstance().End(); ++ iter)
    {
        if (iChannelID == 0 || iChannelID == iter->second.m_iChannelID)
        {
            GetConnectorSender()->NotifyClientUser(iter->second.m_iGroupID
                    , iter->first
                    , iCmd
                    , stMsgPack
                    , iErrcode);
        }
    }
}
