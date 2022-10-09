#pragma once

#include "logic_game_processor_base.h"

class CLogicCardOperationProcessor : public CLogicProcessorBase
{
public:
	CLogicCardOperationProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData);
};
