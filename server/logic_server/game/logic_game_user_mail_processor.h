#pragma once

#include "logic_game_processor_base.h"

class CLogicUserMailProcessor : public CLogicProcessorBase
{
public:
	CLogicUserMailProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;
};
