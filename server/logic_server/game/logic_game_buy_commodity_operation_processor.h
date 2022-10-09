#pragma once

#include "logic_game_processor_base.h"

class CLogicBuyCommodityOperationProcessor : public CLogicProcessorBase
{
public:
	CLogicBuyCommodityOperationProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;


    //特殊卡池抽卡机制(抽取次数>指定值，三星会递增指定权重，相应的其他星级会降低)
    std::vector<TLogicRandBonusItem> GetFixedCardRand(int32_t iCommodityID,bool useTenBaodi) const;

};
