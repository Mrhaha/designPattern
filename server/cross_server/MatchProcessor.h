#pragma once

#include <algorithm>
#include "AppCommon.h"
#include "Processor.h"
#include "MatchPlayer.h"
#include "FairPlayer.h"
#include "AttackDefense.h"
#include "AttackDefenseChallenge.h"

enum enmFightReport
{
    enmFightReport_AttackDefense = 5,

    enmFightReport_AttackDefense_Occupy = 1,	//我方攻占
    enmFightReport_AttackDefense_Repair = 2,	//我方修复
    enmFightReport_AttackDefense_Occupied = 3,	//敌方攻占
    enmFightReport_AttackDefense_Repaired = 4,	//敌方修复
};

class CMatchProcessor : public CProcessor
{
public:
	CMatchProcessor(void);
	~CMatchProcessor(void);

public:
    bool Stop() override;
    void Refresh() override;
    bool Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID) override;
    int32_t ProcessMessages(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength) override;

protected:
	int32_t ProcessUpdateMatchUser(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessFightMatchPvp(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessFightFairPvp(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessGetAttackDefenseInfo(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessFightAttackDefense(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessFightAttackDefenseResult(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessRepairAttackDefense(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessAttackDefenseSync(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessAttackDefenseSyncDone(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessGetLuckyHammerRank(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessGetLuckyHammerLog(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessLuckyHammerLogScore(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessSyncLuckyHammerScore(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

	int32_t ProcessSyncLuckyHammerLog(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

protected:
	int32_t UpdateMatchPlayer(int32_t iUin, int16_t nGroupID, int32_t iMatchLevel, int32_t iPower);

	int32_t RankLuckyHammer();

private:
    int32_t RecoveryDataFromFile();

    int32_t SaveDataToFile();

protected:
	void AttackDefenseRefresh();

	void AttackDefensePrePare();

	int32_t AttackDefenseToBegin();

	int32_t AttackDefenseChallenging();

	int32_t AttackDefenseToEnd();

	bool IsAttackDefenseFighting();
	bool IsAttackDefenseFightingExt();
	bool IsAttackDefensePrepare();

private:
	CAttackDefense m_stAtkDef;
    CAttackDefenseChallenge m_stAtkDefChallenge;			//进行挑战的玩家列表
    std::map<int32_t, std::pair<int32_t, bool>> m_stAtkDefPrepareStat;      // 公会攻防战准备阶段，各服务器数据同步状态 <server_id, <上次发送数据请求时间, 是否同步成功>>

    std::vector<CMatchPlayer> m_stMatchPlayerVec;

    CFairPlayer m_stFairPlayer;

	std::map<std::pair<int32_t, int32_t>, CPackLuckyHammer> m_stLuckyHammerMap;
};

