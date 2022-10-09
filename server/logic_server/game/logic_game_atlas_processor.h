#pragma once

#include "logic_game_processor_base.h"

class CLogicAtlasProcessor : public CLogicProcessorBase
{
public:
    CLogicAtlasProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
	bool DoUserRunGetAtlasInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunLevelUpAtlasBuff(const msgpack::unpacked& stFormatData);


    //根据当前地区buff等级，得出已经消耗的总星级,iLevelUpParam（角色星级转换地区星级比例）
    static int32_t CalcAreaTotalStar(int32_t iAreaID,int32_t iLevel,int32_t iLevelUpParam);

};


