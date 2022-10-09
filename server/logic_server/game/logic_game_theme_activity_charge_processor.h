#pragma once

#include "logic_game_processor_base.h"

class CLogicThemeActivityChargeProcessor : public CLogicProcessorBase
{
public:
	CLogicThemeActivityChargeProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
	bool DoTHEME_ACTIVITY_CHARGE_GET_INFO_REQ(const msgpack::unpacked& stFormatData);
	bool DoTHEME_ACTIVITY_CHARGE_GET_REWARD_REQ(const msgpack::unpacked& stFormatData);
	bool DoTHEME_ACTIVITY_CHARGE_BUY_REQ(const msgpack::unpacked& stFormatData);
	bool DoTHEME_ACTIVITY_CHARGE_CHARGE_REQ(const msgpack::unpacked& stFormatData);
};
