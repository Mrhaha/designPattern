#pragma once

#include "logic_game_processor_base.h"

class CLogicReconnectProcessor : public CLogicProcessorBase
{
public:
	CLogicReconnectProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

    bool DoServerRun(const CMsgHeadQueue &stMsgHead, const msgpack::unpacked &stFormatData) override;

private:
    bool DoServerRunUserReconnectCheck(const CMsgHeadQueue &stMsgHead, const msgpack::unpacked &stFormatData);

    void HandleReconnect();
};
