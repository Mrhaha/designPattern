#pragma once

#include "logic_game_processor_base.h"

class CLogicCrossGuildWarProcessor : public CLogicProcessorBase
{
public:
	CLogicCrossGuildWarProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;

	bool DoUserRunGetAttackDefenseInfo(const msgpack::unpacked& stFormatData);

	bool DoUserRunFightAttackDefense(const msgpack::unpacked& stFormatData);

	bool DoUserRunFightAttackDefenseResult(const msgpack::unpacked& stFormatData);

	bool DoUserRunRepairAttackDefense(const msgpack::unpacked& stFormatData);

	bool DoUserRunRemoveFightFailCD(const msgpack::unpacked& stFormatData);

protected:
	bool DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData) override;

	bool DoServerRunGetAttackDefenseInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunFightAttackDefense(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunFightAttackDefenseResult(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunRepairAttackDefense(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunCrossAttackDefenseAward(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunCrossAttackDefenseStart(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunCrossGuildWarReportNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

	bool DoServerRunCrossAttackDefensePrepare(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

private:
	bool CheckCanAttackDefenseAction(int32_t iGuildID, int32_t iUid, int32_t iGroupID, int32_t iNeedArmyPoint);

	void AddGuildWarFightReport(int32_t iGuildID, int32_t iReportType, int32_t iReportValue, const std::map<std::string, std::string>& stParamMap, const std::vector<int32_t>& stParamVec);
};


