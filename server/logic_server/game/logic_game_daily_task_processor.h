#pragma once

#include "logic_game_processor_base.h"

class CLogicDailyTaskProcessor : public CLogicProcessorBase
{
public:
	CLogicDailyTaskProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData);

private:
    int32_t HandleQuestReward(int32_t iQuestID, std::vector<CPackGameItem>& stRewards, CPackQuestData& stRetData);
    void UpdateSpecialQuestProgress(TUserQuestTable* stQuestData, int32_t iEventType, const TLogicQuestEventPara& stNewConfig, const TLogicQuestEventPara& stOldConfig);
};
