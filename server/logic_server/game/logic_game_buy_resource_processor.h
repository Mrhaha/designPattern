#pragma once

#include "logic_game_processor_base.h"

class CLogicBuyResourceProcessor : public CLogicProcessorBase
{
public:
	CLogicBuyResourceProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

    bool CheckVipLevel(unsigned short iCommand);

	bool CheckShopRuleType(int32_t iRuleType, int32_t iNeedGrade);
};
