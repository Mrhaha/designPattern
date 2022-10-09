#pragma once

#include "logic_game_processor_base.h"

class CLogicRuneProcessor : public CLogicProcessorBase
{
public:
    CLogicRuneProcessor(uint16_t usCmd, const std::string& strCmdName);



protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;


private:
    int32_t DoUserRunSetRune(const msgpack::unpacked& stFormatData);

    int32_t DoUserRunGetRuneInfo(const msgpack::unpacked& stFormatData);

    int32_t DoUserRunUnlockRune(const msgpack::unpacked& stFormatData);

    int32_t DoUserRunRuneLevelUp(const msgpack::unpacked& stFormatData);


};

