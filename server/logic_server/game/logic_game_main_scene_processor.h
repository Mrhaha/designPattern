#pragma once

#include "logic_game_processor_base.h"

class CLogicMainSceneProcessor : public CLogicProcessorBase
{
public:
	CLogicMainSceneProcessor(uint16_t usCmd, const std::string& strCmdName);

private:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
    bool GetWenWenNewsOneServerData(const TLogicNewsDynamicElem& stConfig, CPackWenWenNewsDynamicData& stData);
};
