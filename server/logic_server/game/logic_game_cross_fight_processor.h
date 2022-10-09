#pragma once

#include "logic_game_processor_base.h"
#include "server_fight_protocol.h"

class CLogicCrossFightProcessor : public CLogicProcessorBase
{
    enum {
        CROSS_FIGHT_LEVEL_CAN_FIGHT = 0,
        CROSS_FIGHT_LEVEL_PASSED = 1,
        CROSS_FIGHT_LEVEL_RECEIVED = 2,
        CROSS_FIGHT_DEFAULT_RANK = 5000,
    };

public:
	CLogicCrossFightProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

protected:

	std::map<int32_t, CPackPvpChallengerInfo> ResetChallengerInfo();
};

//镇魂之塔
class CLogicClimbTowerProcessor : public CLogicProcessorBase
{
public:
	CLogicClimbTowerProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;

	int32_t GetSweepCostYuanbao();
};
