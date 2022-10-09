#pragma once

#include "logic_game_processor_base.h"

class CLogicFriendProcessor : public CLogicProcessorBase
{
public:
	CLogicFriendProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
	bool DoUserRun(const msgpack::unpacked& stFormatData) override;
    bool DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData) override;

private:
	bool DoUserRunGetFriendList(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetFriendApplyList(const msgpack::unpacked& stFormatData);

	bool DoUserRunDealFriendApply(const msgpack::unpacked& stFormatData);

	bool DoUserRunAddFriend(const msgpack::unpacked& stFormatData);

	bool DoUserRunDeleteFriend(const msgpack::unpacked& stFormatData);

	bool DoUserRunSendFriendEnergy(const msgpack::unpacked& stFormatData);

	bool DoUserRunFastSendFriendEnergy(const msgpack::unpacked& stFormatData);

	bool DoUserRunGetFriendRecommend(const msgpack::unpacked& stFormatData);

	bool DoUserRunSearchFriend(const msgpack::unpacked& stFormatData);

private:
    bool DoServerRunGetFriendList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunGetFriendApplyList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunNotifyFriendApply(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunSearchFriend(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunDealFriendApply(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

private:
    bool DoServerRunFriendDealApplyCrossCheck(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunFriendCrossDirectAdd(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);
    bool DoServerRunFriendCrossDirectDel(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData);

private:
    bool AddFriendApply(int iFriendUin, int iFriendGroupID);
    bool AddFriend(user_data_table_ptr_type pUser, int iFriendUin, int iFriendGroupID);
};
