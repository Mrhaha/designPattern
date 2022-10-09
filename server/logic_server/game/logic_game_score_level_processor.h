#pragma once

#include "logic_game_processor_base.h"

class CLogicScoreLevelProcessor : public CLogicProcessorBase
{
public:
	CLogicScoreLevelProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
	bool DoUserRunGetActiveInfo(const msgpack::unpacked &stFormatData);
	bool DoUserRunFightStart(const msgpack::unpacked &stFormatData);
	bool DoUserRunFightResult(const msgpack::unpacked &stFormatData);
	bool DoUserRunFightSweep(const msgpack::unpacked &stFormatData);
	bool DoUserRunGetScoreAward(const msgpack::unpacked &stFormatData);
	bool DoUserRunBuyXingdongli(const msgpack::unpacked &stFormatData);
};
