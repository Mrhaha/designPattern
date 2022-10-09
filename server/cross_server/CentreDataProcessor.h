#pragma once

#include "Processor.h"
#include "server_base_protocol.h"
#include "CentreDataDefine.h"

class CCentreDataProcessor : public CProcessor
{
public:
    CCentreDataProcessor(void);
    ~CCentreDataProcessor(void);

    bool Stop() override;
    void Refresh() override;
    bool Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID) override;
    int32_t ProcessMessages(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength) override;

protected:
    int32_t ProcessUserBriefUpdate(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    // 好友
    int32_t ProcessGetFriendList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGetFriendApplyList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessSearchFriend(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    // 公会
    int32_t ProcessGetGuildList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGetSelfGuildInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGetGuildMembers(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessCreateGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessLeaveGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessUpdateGuildBulletin(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessUpdateGuildPost(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessTransferGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessExpelGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessApplyJoinGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessDealGuildApply(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGetGuildApplyList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessSearchGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessChangeGuildName(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessAdminModifyGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessAdminDismissGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessAdminSetGuildChief(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessAdminChangeGuildName(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildBossHurtNotify(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildBossHurtList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildGetLog(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildWishNotify(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildWishGetList(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    // 聊天
    int32_t ProcessUserTalk(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

    //公会新加功能
    int32_t ProcessGuildDonate(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildSignIn(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildSignUpGrade(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildDecorateSave(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildGetBossHurtRank(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildSitBar(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildGetBarReward(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildUnlockCollection(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildSetDecorate(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

    //公会战
    int32_t ProcessGuildWarInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildWarBegin(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildWarEnd(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildWarGetRankInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGuildWarUpdateRankInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

private:
    int32_t RecoveryDataFromFile();
    int32_t SaveDataToFile();



private:
    // 内部数据
    time_t m_iLastRefreshTime;
    time_t m_iLastLazyRefreshTime;

private:
    // 玩家数据
    CCrossSQLDataManager m_stSQLDataManager;            // 玩家数据库数据管理
};
