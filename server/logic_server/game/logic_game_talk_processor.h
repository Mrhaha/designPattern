#pragma once

#include "logic_game_processor_base.h"

class CLogicTalkProcessor : public CLogicProcessorBase
{
public:
	CLogicTalkProcessor(uint16_t usCmd, const std::string& strCmdName);

private:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;
    bool DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData) override;

private:
    bool DoServerRunUserTalkNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
};

class CLogicAvatarProcessor : public CLogicProcessorBase
{
public:
	CLogicAvatarProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;
};
