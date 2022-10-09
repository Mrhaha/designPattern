#pragma once

#include "logic_game_processor_base.h"

class CLogicErrorProcessor : public CLogicProcessorBase
{
public:
    CLogicErrorProcessor(uint16_t usCmd, const std::string& strCmdName);

	bool DoUserRun(const msgpack::unpacked& stFormatData) override;
};
