//
// Created by czy on 2022/5/7.
//
#pragma once

#include "logic_game_processor_base.h"

class CLogicCrossLimitBossProcessor : public CLogicProcessorBase
{
public:
    CLogicCrossLimitBossProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;
    bool DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData) override;

private:
    bool DoUserRunGetLimitBossHistory(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossBegin(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossEnd(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossInvite(const msgpack::unpacked& stFormatData);
    bool DoUserRunLimitBossGetReward(const msgpack::unpacked& stFormatData);

private:
    bool DoServerRunLimitBossNew(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunLimitBossHistory(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunLimitBossBegin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunLimitBossEnd(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunLimitBossDead(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunLimitBossInviteNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunLimitBossGetReward(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunLimitBossRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunLimitBossGetInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

};
