#pragma once

#include "logic_game_processor_base.h"

class CLogicMultiStepTaskProcessor : public CLogicProcessorBase
{
public:
	CLogicMultiStepTaskProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;
};
