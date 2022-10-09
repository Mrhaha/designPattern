#pragma once

#include "logic_game_processor_base.h"

class CLogicBlackMarketProcessor : public CLogicProcessorBase
{
public:
	CLogicBlackMarketProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
    bool GetRandomStoreInfo(int32_t iStoreID, CRandomStoreInfoPack& stMarketInfo);
};
