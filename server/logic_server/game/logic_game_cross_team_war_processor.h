#pragma once

#include "logic_game_processor_base.h"

class CLogicCrossTeamWarProcessor : public CLogicProcessorBase
{
public:
	CLogicCrossTeamWarProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

    bool DoUserRunCreateBanquetTeam(const msgpack::unpacked& stFormatData);
    bool DoUserRunJoinBanquetTeam(const msgpack::unpacked& stFormatData);
    bool DoUserRunGetBanquetInfo(const msgpack::unpacked& stFormatData);
    bool DoUserRunBanquetInvite(const msgpack::unpacked& stFormatData);
    bool DoUserRunGetBanquetInviteList(const msgpack::unpacked& stFormatData);
    bool DoUserRunQuitBanquetTeam(const msgpack::unpacked& stFormatData);
    bool DoUserRunGetBanquetOpenTeam(const msgpack::unpacked& stFormatData);


protected:
	bool DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData) override;

    bool DoServerRunCreateBanquetTeam(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGetBanquetInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunBanquetFinishNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunBanquetInfoNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunBanquetUpdateLogin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunBanquetInviteNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunBanquetInvite(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunBanquetQuitTeam(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunJoinBanquetTeam(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGetBanquetOpenTeam(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);



    bool DoServerRunAdminModifyGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunAdminDismissGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunAdminSetGuildChief(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunAdminChangeGuildName(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
};
