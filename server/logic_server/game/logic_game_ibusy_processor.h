#pragma once

#include "logic_game_processor_base.h"

class CLogicIBusyProcessor : public CLogicProcessorBase
{
public:
	CLogicIBusyProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
	bool DoIBUSY_GET_INFO_REQ(const msgpack::unpacked& stFormatData);
	bool DoIBUSY_CHANGE_TASK_REQ(const msgpack::unpacked& stFormatData);
	bool DoIBUSY_GET_LEVEL_REWARD_REQ(const msgpack::unpacked& stFormatData);
	bool DoIBUSY_GET_MARKET_DATA_REQ(const msgpack::unpacked& stFormatData);
	bool DoIBUSY_REFRESH_MARKET_REQ(const msgpack::unpacked& stFormatData);
	bool DoIBUSY_PAIQIAN_REQ(const msgpack::unpacked& stFormatData);
	bool DoIBUSY_MARKET_BUY_REQ(const msgpack::unpacked& stFormatData);
    bool DoIBUSY_GET_DEPOSIT_LEVEL_AWARD(const msgpack::unpacked& stFormatData);
    bool DoIBUSY_GET_DEPOSIT_EXP_AWARD(const msgpack::unpacked& stFormatData);

};
