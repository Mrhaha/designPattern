#pragma once

#include "logic_game_processor_base.h"
#include "server_common_protocol.h"

class CLogicLevelProcessor : public CLogicProcessorBase
{
public:
	CLogicLevelProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;
private:
    bool AntiCheatingCheck(const TLogicLevelElem* pConfig,const CPackLevelFightPara& stParam,int64_t* lDps,int64_t* lMaxDps);

    bool IsRefreshCrushBoss(int32_t iLevelID,CPackCrushBossInfo& crushBossInfo);
};
