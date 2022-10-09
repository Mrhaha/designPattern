#pragma once

#include "logic_game_processor_base.h"


class CLogicCrossServerProcessor : public CLogicProcessorBase
{
public:
	CLogicCrossServerProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

	bool DoUserRunFightMatchPvp(const msgpack::unpacked& stFormatData);

	bool DoUserRunFightMatchPvpResult(const msgpack::unpacked& stFormatData);

	bool DoUserRunBuyMatchStoreItem(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetMatchMaxAward(const msgpack::unpacked& stFormatData);

	bool DoUserRunRefreshFairPvpCard(const msgpack::unpacked& stFormatData);

	bool DoUserRunSelectFairPvpTeam(const msgpack::unpacked& stFormatData);

	bool DoUserRunFightFairPvp(const msgpack::unpacked& stFormatData);

	bool DoUserRunFightFairPvpResult(const msgpack::unpacked& stFormatData);

	bool DoUserRunLuckyHammerLottery(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetLuckyHammerRank(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetLuckyHammerLog(const msgpack::unpacked& stFormatData);

	bool DoUserRunLuckyHammerLogScore(const msgpack::unpacked& stFormatData);

protected:
	bool DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData) override;

	bool DoServerRunFightMatchPvp(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunBeMatchedNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunBroadcastNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunFightFairPvp(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunFairMatchedNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunGetLuckyHammerLog(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunLuckyHammerLogScore(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunGetLuckyHammerRank(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunLuckyHammerRankNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

private:
	void ReportMatchUserData();

	void SyncLuckyHammerScore(int32_t iActivityID);
};


