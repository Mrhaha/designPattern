#pragma once

#include <algorithm>
#include "AppCommon.h"
#include "Processor.h"
#include "server_cross_protocol.h"
#include "server_new_limit_boss_protocol.h"
#include "cross_record_banquet_id_table.h"

class CTeamProcessor : public CProcessor
{
public:
	CTeamProcessor(void);
	~CTeamProcessor(void);

public:
    bool Stop() override;
    void Refresh() override;
    bool Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID) override;
    int32_t ProcessMessages(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength) override;

protected:
    //家园宴会
    int32_t ProcessCreateBanquetTeam(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessJoinBanquetTeam(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGetBanquetInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessBanquetUpdateLogin(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessBanquetInvite(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessBanquetQuitTeam(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessBanquetGetOpenTeam(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);


    //限时Boss
    int32_t ProcessNewLimitBoss(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessGetLimitBossHistory(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessLimitBossBegin(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessLimitBossEnd(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessLimitBossInvite(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessLimitBossGetInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

private:
    //家园宴请
    CPackBanquetTeamIds* GetTeamIDByUin(int32_t iUin);
    CPackBanquetTeamInfo* GetTeamInfo(uint64_t iTeamID);
    int32_t CreateOrUpdateTeam(int iUin, int iGroupID, const CCrossRequestCreateBanquetTeam& stParam,uint64_t& teamID);
    int32_t JoinTeam(uint64_t iTeamID, int iShowCard, const CPackUserBrief& stBrief, char cPos,int32_t iSkinID);
    void NotifyTeamInfo(const CPackBanquetTeamInfo* pTeam);
    void RefreshTeam();

    void RefreshLimitBoss();
    uint64_t GetCurBanquetID() const {return m_uiBanquetID;}
    void SetCurBanquetID(uint64_t BanquetID) {m_uiBanquetID = BanquetID;}

    //限时Boss
    CPackLimitBossTeamInfo* GetBossTeamInfoByUin(int32_t iUin);


private:
    int32_t RecoveryDataFromFile();
    int32_t SaveDataToFile();

private:
	time_t m_iLastRefreshTime;
    time_t m_iLastDailyRefreshTime;
    //家园宴会
    std::unordered_map<int32_t, CPackBanquetTeamIds> m_stUinToTeamID;   // [uin, team_ids]         只记录处于领奖倒计时的玩家
    std::unordered_map<uint64_t, CPackBanquetTeamInfo> m_stTeamMap;      // [team_id, team_info]   记录所有未全部完成领奖倒计时的队伍

    //限时Boss
    std::unordered_map<int32_t, CPackLimitBossTeamInfo> m_stUin2LimitBossInfo;
    std::unordered_map<int32_t, std::set<int32_t>> m_stUin2LimitBossTeamID;
    uint64_t m_uiBanquetID;                                             // 当前的宴会ID

};
