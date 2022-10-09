#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

class CPackFriendInfo
{
public:
	CPackFriendInfo() : m_stBriefInfo() {}

	MSGPACK_DEFINE(m_stBriefInfo);

	CPackUserBrief  m_stBriefInfo;
};

class CPackFriendApply
{
public:
	CPackFriendApply() : m_stBriefInfo(), m_iApplyTime(0) {}

	MSGPACK_DEFINE(m_stBriefInfo, m_iApplyTime);

    CPackUserBrief  m_stBriefInfo;
    int32_t         m_iApplyTime;
};

//GET_FRIEND_LIST = 10223,						//拉取好友列表
class CRequestGetFriendList
{
public:
    struct user_key
    {
        MSGPACK_DEFINE(m_iUin, m_iGroupID);

        int32_t m_iUin;
        int32_t m_iGroupID;
    };

    CRequestGetFriendList() : m_stUserVec() {}

    MSGPACK_DEFINE(m_stUserVec);

    std::vector<user_key> m_stUserVec;
};

class CResponseGetFriendList : public CPackUserSyncData
{
public:
	CResponseGetFriendList() : m_stFriendVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stFriendVec);

	std::vector<CPackFriendInfo> m_stFriendVec;
};

//GET_FRIEND_APPLY_LIST = 10224,				//拉取好友申请
class CRequestGetFriendApplyList
{
public:
    struct user_key
    {
        MSGPACK_DEFINE(m_iUin, m_iGroupID, m_iApplyTime);

        int32_t m_iUin;
        int32_t m_iGroupID;
        int32_t m_iApplyTime;
    };

    CRequestGetFriendApplyList() : m_stUserVec() {}

    MSGPACK_DEFINE(m_stUserVec);

    std::vector<user_key> m_stUserVec;
};

class CResponseGetFriendApplyList : public CPackUserSyncData
{
public:
	CResponseGetFriendApplyList() : m_stApplyVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stApplyVec);

	std::vector<CPackFriendApply> m_stApplyVec;
};

class CResponseGetFriendRecommend : public CPackUserSyncData
{
public:
    CResponseGetFriendRecommend() : m_stRecommendVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stRecommendVec);

    std::vector<CPackUserBrief> m_stRecommendVec;
};

class CRequestDealFriendApply
{
public:
	CRequestDealFriendApply() : m_iDstUid(0), m_iDstGroupID(0), m_bIsAgree(false) {}

	MSGPACK_DEFINE(m_iDstUid, m_iDstGroupID, m_bIsAgree);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
	bool m_bIsAgree;
};

class CCrossFriendDealApplyCheck
{
public:
    CCrossFriendDealApplyCheck() : m_iDstUid(0), m_iDstGroupID(0), m_iRetCode(0) {}

    MSGPACK_DEFINE(m_iDstUid, m_iDstGroupID, m_iRetCode);

    int32_t m_iDstUid;
    int32_t m_iDstGroupID;
    int32_t m_iRetCode;
};

class CRequestAddFriend
{
public:
	CRequestAddFriend() : m_iDstUin(0), m_iDstGroupID(0) {}

	MSGPACK_DEFINE(m_iDstUin, m_iDstGroupID);

	int32_t     m_iDstUin;
    int32_t     m_iDstGroupID;
};

//SEND_FRIEND_ENERGY = 10229,					//赠送好友体力
class CRequestSendFriendEnergy
{
public:
	CRequestSendFriendEnergy() : m_iDstUid(0), m_iDstGroupID(0) {}

	MSGPACK_DEFINE(m_iDstUid, m_iDstGroupID);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
};

class CResponseSendFriendEnergy : public CPackUserSyncData
{
public:
	CResponseSendFriendEnergy() : m_stFriend(), m_iAddFriendEnergyCount(0), m_iAddEnergyValue(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_stFriend, m_iAddFriendEnergyCount, m_iAddEnergyValue);

	CPackFriendInfo m_stFriend;
	int32_t m_iAddFriendEnergyCount;
	int32_t m_iAddEnergyValue;
};

//FASET_SEND_FRIEND_ENERGY = 10230,				//一键赠送好友
class CResponseFastSendFriendEnergy : public CPackUserSyncData
{
public:
	CResponseFastSendFriendEnergy() :m_iAddFriendEnergyCount(0), m_iAddEnergyValue(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iAddFriendEnergyCount, m_vtDstUser, m_iAddEnergyValue);

	int32_t m_iAddFriendEnergyCount;
	std::vector<CPackFriendInfo> m_vtDstUser;
	int32_t m_iAddEnergyValue;
};

//GET_FRIEND_ENERGY = 10231,					//领取好友体力
class CRequestGetFriendEnergy
{
public:
	CRequestGetFriendEnergy() : m_iDstUid(0), m_iDstGroupID(0) {}

	MSGPACK_DEFINE(m_iDstUid, m_iDstGroupID);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
};

class CResponseGetFriendEnergy : public CPackUserSyncData
{
public:
	CResponseGetFriendEnergy() : m_stFriend(), m_iAddFriendEnergyCount(0), m_iAddEnergyValue(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_stFriend, m_iAddFriendEnergyCount, m_iAddEnergyValue);

	CPackFriendInfo m_stFriend;
	int32_t m_iAddFriendEnergyCount;
	int32_t m_iAddEnergyValue;
};

//FAST_GET_FRIEND_ENERGY = 10232,				//一键领取体力
class CResponseFastGetFriendEnergy : public CPackUserSyncData
{
public:
	CResponseFastGetFriendEnergy() :m_iAddFriendEnergyCount(0), m_iAddEnergyValue(0){}

	MSGPACK_DEFINE(m_stSyncData, m_iAddFriendEnergyCount, m_vtDstUser, m_iAddEnergyValue);

	int32_t m_iAddFriendEnergyCount;
	std::vector<CPackFriendInfo> m_vtDstUser;
	int32_t m_iAddEnergyValue;
};

class CNotifyItemChange : public CPackUserSyncData
{
public:
    CNotifyItemChange() : m_stGets(), m_stConsumes(), m_bNotify(false) {}

    MSGPACK_DEFINE(m_stSyncData, m_stGets, m_stConsumes, m_bNotify);

    std::vector<CPackGameItem> m_stGets;            // 获得
    std::vector<CPackGameItem> m_stConsumes;        // 消耗
    bool m_bNotify;     // 是否弹窗通知
};

class CNotifyQuestData : public CPackUserSyncData
{
public:
    CNotifyQuestData() : m_stQuestVec(),m_bIsInit(false) {}

    MSGPACK_DEFINE(m_stSyncData, m_stQuestVec,m_bIsInit);

    std::vector<CPackQuestData> m_stQuestVec;
    bool m_bIsInit;
};

class CNotifyBadgeData : public CPackUserSyncData
{
public:
    CNotifyBadgeData() : m_stBadgeVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stBadgeVec);

    std::vector<CPackBadgeData> m_stBadgeVec;
};

class CNotifyUsePhyData : public CPackUserSyncData
{
public:
    CNotifyUsePhyData() : m_iPhyStore(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iPhyStore);

    int32_t     m_iPhyStore;
};

//SEARCH_FRIEND = 10234,						//查找好友
class CRequestSearchFriend
{
public:
	CRequestSearchFriend() : m_strSearch() {}

	MSGPACK_DEFINE(m_strSearch);

	std::string m_strSearch;
};

class CResponseSearchFriend : public CPackUserSyncData
{
public:
	CResponseSearchFriend() : m_stUserInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stUserInfo);

	CPackUserBrief m_stUserInfo;
};

//CHANGE_NICK = 10285,					//改名
class CRequestChangeNick
{
public:
	CRequestChangeNick() : m_strNick() {}

	MSGPACK_DEFINE(m_strNick);

	std::string m_strNick;
};

class CResponseChangeNick : public CPackUserSyncData
{
public:
	CResponseChangeNick() : m_strNick(), m_iChangeNickCount(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_strNick, m_iChangeNickCount);

	std::string m_strNick;
    int32_t     m_iChangeNickCount;
};
