//
// Created by czy on 2022/8/8.
//
#pragma once

#include "logic_game_processor_base.h"

class CLogicGashaponProcessor : public CLogicProcessorBase
{
public:
    CLogicGashaponProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
    bool DoUserRunGetGashaponInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunPlayGashapon(const msgpack::unpacked& stFormatData);
    bool DoUserRunGetCrushBossInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunPlayCrushBoss(const msgpack::unpacked& stFormatData);
    bool DoUserGetCrushBossReward(const msgpack::unpacked& stFormatData);
    bool DoUserRunEndCrushBoss(const msgpack::unpacked& stFormatData);
    bool DoUserRunGiveUpCrushBoss(const msgpack::unpacked& stFormatData);

};
