#pragma once

#include "Processor.h"

class CHttpProcessor : public CProcessor
{
public:
    int32_t ProcessMessages(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength) override;

protected:
    int32_t ProcessServiceAlert(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportSetMusic(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportGetEquip(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportUpgradeSkill(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportUpgradeConstel(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportGetConstel(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportStartFightPVE(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportGainEmail(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportAchieveMission(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportGetMissionReward(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportGetPassReward(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportPVPFight(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportRefreshPVP(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportRewardPVP(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportChangeTeam(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportCreateGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportJoinGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportLeaveGuild(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportChangeGuildName(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportGetGuildInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportDonateCard(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportSetWishCard(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportBuyGuildItem(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportChoosePay(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportPay(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportItemMonitor(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessServiceDataReportOffline(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);


};
