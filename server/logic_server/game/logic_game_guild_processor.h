#pragma once

#include "logic_game_processor_base.h"

class CLogicGuildProcessor : public CLogicProcessorBase
{
public:
	CLogicGuildProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;
    bool DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData) override;

protected:
	bool DoUserRunGetGuildList(const msgpack::unpacked& stFormatData);
	bool DoUserRunGetSelfGuildInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunGetGuildMembers(const msgpack::unpacked& stFormatData);
	bool DoUserRunCreateGuild(const msgpack::unpacked& stFormatData);
	bool DoUserRunLeaveGuild(const msgpack::unpacked& stFormatData);
	bool DoUserRunUpdateGuildBulletin(const msgpack::unpacked& stFormatData);
	bool DoUserRunUpdateGuildPost(const msgpack::unpacked& stFormatData);
	bool DoUserRunTransferGuild(const msgpack::unpacked& stFormatData);
	bool DoUserRunExpelGuild(const msgpack::unpacked& stFormatData);
	bool DoUserRunApplyJoinGuild(const msgpack::unpacked& stFormatData);
	bool DoUserRunDealGuildApply(const msgpack::unpacked& stFormatData);
	bool DoUserRunGetGuildApplyList(const msgpack::unpacked& stFormatData);
	bool DoUserRunGetOtherPlayerDetail(const msgpack::unpacked& stFormatData);
	bool DoUserRunSearchGuild(const msgpack::unpacked& stFormatData);
	bool DoUserRunChangeGuildName(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildBossGetHurt(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildBossHurtList(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildGetLog(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildWishSetCard(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildWishSetSwapCard(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildWishGetList(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildWishGetOtherSwapInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildWishPresent(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildWishSwap(const msgpack::unpacked& stFormatData);

    bool DoUserRunGuildDonate(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildSignIn(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildSignUpGrade(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildRandom(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildGetRandomReward(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildSaveDecorate(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildGetBossHurtRank(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildSitBar(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildGetBarReward(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildUnlockCollection(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildSetDecorate(const msgpack::unpacked& stFormatData);
    bool DoUserRunGetGuildFragmentHistory(const msgpack::unpacked& stFormatData);
    bool DoUserRunGetGuildRewardInfo(const msgpack::unpacked& stFormatData);

    //公会战
    bool DoUserRunGetGuildWarInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildWarBegin(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildWarEnd(const msgpack::unpacked& stFormatData);
    bool DoUserRunGuildWarGetRankInfo(const msgpack::unpacked& stFormatData);



protected:
    bool DoServerRunGetGuildList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGetSelfGuildInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGetGuildMembers(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunCreateGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunLeaveGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunUpdateGuildBulletin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunUpdateGUildPost(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunTransferGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunExpelGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunApplyJoinGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunDealGuildApply(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGetGuildApplyList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunSearchGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGetUserDetail(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGetOtherPlayerDetail(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunChangeGuildName(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildBossRewardNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildBossHurtList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildChangeNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildGetLog(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildWishGetList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildWishGetSwapInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildWishGetOtherSwapInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

    bool DoServerRunGuildDonate(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildSign(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildSignUpgrade(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGetBossHurtRank(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunBossSitBar(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGetBossSitBarReward(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunUnlockCollection(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunSetDecorate(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunSaveGuildDecorate(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

private:
    bool DoServerRunGuildWishCrossCheck(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildWishCrossCheckResult(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildWishCrossDstGet(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

    //公会战
    bool DoServerRunGuildWarInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildBegin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildEnd(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildGetRankInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGuildUpdateGuildWar(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

private:
    void GuildWishNotifyCross(user_data_table_ptr_type pUser,bool isFull = false);
    bool GuildWishGetSwapInfo(int iUin, int iGroupID, CResponseGuildWishGetOtherSwapInfo& stRsp);

    bool GuildWishHandleCheck(CRequestGuildWishSwap& stReq);
    bool GuildWishDstCheck(user_data_table_ptr_type pDstUser, const CCrossGuildWishOp& stPara);
    bool GuildWishDstGetCard(user_data_table_ptr_type pDstUser, const CCrossGuildWishOp& stPara);
    bool GuildWishSelfConsume(const CCrossGuildWishOp& stPara);

    void GuildWishCheckAcceptCard();

    //检查公会战开始和结束时间
    bool CheckGuildWarOpen();
};
