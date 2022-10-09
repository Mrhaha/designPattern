//
// Created by hzl on 2019/10/24.
//

#pragma once

#include "logic_game_processor_base.h"

class CLogicMonthPassProcessor : public CLogicProcessorBase
{
public:
    CLogicMonthPassProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

    bool DoUserRunGetMonthPassAward(const msgpack::unpacked& stFormatData);

    bool DoUserRunGetMonthPassTaskAward(const msgpack::unpacked& stFormatData);

    bool DoUserRunMonthPassExchange(const msgpack::unpacked& stFormatData);

    bool DoUserRunMonthPassWishItem(const msgpack::unpacked& stFormatData);

    bool DoUserRunMonthPassDailyWish(const msgpack::unpacked& stFormatData);

private:
    int32_t GetMonthPassLevel();

    bool IsBuyMonthPass();
};
