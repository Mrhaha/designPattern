#pragma once

#include "logic_config_store.h"

class CLogicActiveExtraProcessor : public CLogicProcessorBase
{
public:
	CLogicActiveExtraProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;
};
