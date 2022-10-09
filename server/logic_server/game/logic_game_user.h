#pragma once

#include <set>
#include <unordered_map>
#include "logicso_define.h"

//default 40K concurrent user
class CLogicUser
{
public:
	typedef std::unordered_map<int32_t, stLogicSoClientInfo> container_type;

	typedef container_type::iterator iterator;

	typedef container_type::const_iterator const_iterator;

	typedef container_type::size_type size_type;

public:
	static CLogicUser&        GetInstance();

	const_iterator              Begin() const;

	const_iterator              End() const;

	size_type                   GetUserNum() const;

	const_iterator              GetUserInfo(int iUin) const;

	stLogicSoClientInfo 		GetClientInfo(int32_t iUin) const;

    bool                        SetUserInfo(int iUin, const stLogicSoClientInfo& stUserInfo);

	std::pair<iterator, bool>   AddUser(int iGroupID, int iUin, stLogicSoClientInfo& stUserInfo);

    std::pair<iterator, bool>   GetUser(int iUin);

	bool                        DelUser(int iUin);

	void OnUserLogin(int32_t iUin, int32_t iGroupID, int32_t iChannelID);

	void OnUserLogout(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iOnlineTime);

	bool IsUserOnline(int32_t iUin, int32_t iGroupID);

	void UpdateUserOnlineTime();

    void AddQueueTag(int32_t iUin);

private:
	CLogicUser() = default;

    bool CheckQueueTag(int32_t iUin);

private:
	container_type              m_stUserContainer;
    std::set<int32_t>           m_stUserQueueTag;       // 下线不能加入保护期的玩家UIN，重连机制使用

	static CLogicUser*        m_pLogicUser;
};
