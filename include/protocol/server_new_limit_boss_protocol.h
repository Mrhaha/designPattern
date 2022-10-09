//
// Created by czy on 2022/5/2.
//

#pragma once

#include "msgpack.hpp"
#include "server_base_protocol.h"
#include "server_common_protocol.h"

class CCrossNotifyNewLimitBoss
{
public:
    CCrossNotifyNewLimitBoss() :m_strFinderName(""),m_iIconHead(0),m_iIconImage(0),m_stBossInfo(){}

    MSGPACK_DEFINE(m_strFinderName,m_iIconHead,m_iIconImage,m_stBossInfo);

    std::string m_strFinderName;
    int32_t m_iIconHead;
    int32_t m_iIconImage;
    CPackNewLimitBossInfo m_stBossInfo;
};



class CResponseGetLimitBossHistory: public CPackUserSyncData
{
public:
    CResponseGetLimitBossHistory() :  m_stHistoryInfo(),m_stFoundLevelID(0),m_iTotalLimitBossPoint(0),m_iDailyRewardCount(0),m_iBaoDiTime(0){}

    MSGPACK_DEFINE(m_stSyncData,m_stHistoryInfo,m_stFoundLevelID,m_iTotalLimitBossPoint,m_iDailyRewardCount,m_iBaoDiTime);

    std::vector<CPackLimitBossTeamInfo> m_stHistoryInfo;
    std::vector<int32_t> m_stFoundLevelID;
    int32_t m_iTotalLimitBossPoint;
    int32_t m_iDailyRewardCount;
    int32_t m_iBaoDiTime;
};

class CRequestLimitBossBegin
{
public:
    CRequestLimitBossBegin():m_iOwnerUin(0),m_iOwnerGroupID(0),m_iLevelID(0){}

    MSGPACK_DEFINE(m_iOwnerUin,m_iOwnerGroupID,m_iLevelID);

    int32_t     m_iOwnerUin;
    int32_t     m_iOwnerGroupID;
    int32_t     m_iLevelID;
};

class CRequestLimitBossEnd
{

public:
    CRequestLimitBossEnd():m_iOwnerUin(0),m_iOwnerGroupID(0),m_iLevelID(0),m_ulHurt(0){}

    MSGPACK_DEFINE(m_iOwnerUin,m_iOwnerGroupID,m_iLevelID,m_ulHurt);

    int32_t     m_iOwnerUin;
    int32_t     m_iOwnerGroupID;
    int32_t     m_iLevelID;             // 对应的关卡ID
    int64_t     m_ulHurt;               // 单次伤害
};

class CCrossLimitBossDeadNotify:public CPackUserSyncData
{
public:
    CCrossLimitBossDeadNotify():m_iOwnerUin(0),m_iOwnerGroupID(0),m_iLevelID(0),m_stTeamInfo(){}

    MSGPACK_DEFINE(m_stSyncData,m_iOwnerUin,m_iOwnerGroupID,m_iLevelID,m_stTeamInfo);

    int32_t m_iOwnerUin;
    int32_t m_iOwnerGroupID;
    int32_t m_iLevelID;                         // 对应的关卡ID
    CPackLimitBossTeamInfo m_stTeamInfo;
};

class CRequestNewLimitBossInvite
{

public:

    CRequestNewLimitBossInvite():m_stInviteUid(),m_stUid2Name(){}

    MSGPACK_DEFINE(m_stInviteUid,m_stUid2Name);

    std::map<int32_t,std::vector<int32_t>> m_stInviteUid;        //<uin,std::vector<int32_t>groupId,headIcon,ImageIcon>//头像，头像框
    std::map<int32_t,std::string> m_stUid2Name;                  //<uin,strName>
};


class CCrossLimitBossInviteNotify:public CPackUserSyncData
{
public:

    CCrossLimitBossInviteNotify():m_stBossInfo(){}

    MSGPACK_DEFINE(m_stSyncData,m_stBossInfo);

    CPackNewLimitBossInfo m_stBossInfo;
};

class CPackGetRewardInfo
{
public:
    CPackGetRewardInfo() : m_iOwnerUin(0),m_iOwnerGroupID(0),m_iLevelID(0),m_iIndex(0) {}

    MSGPACK_DEFINE(m_iOwnerUin,m_iOwnerGroupID,m_iLevelID,m_iIndex);

    int32_t m_iOwnerUin;
    int32_t m_iOwnerGroupID;
    int32_t m_iLevelID;
    int32_t m_iIndex;
};


class CRequestLimitBossGetReward
{
public:
    CRequestLimitBossGetReward():m_stReward() {}

    MSGPACK_DEFINE(m_stReward);

    std::vector<CPackGetRewardInfo> m_stReward;
};

class CResponseLimitBossGetReward:public CPackUserSyncData
{
public:
    CResponseLimitBossGetReward():m_stGetReward(){}

    MSGPACK_DEFINE(m_stSyncData,m_stGetReward);

    CPackTotalItemData      m_stGetReward;
};

class CNotifyLimitBossRunaway: public CPackUserSyncData
{
public:
    CNotifyLimitBossRunaway():m_stBossTeamInfo(){}

    MSGPACK_DEFINE(m_stSyncData,m_stBossTeamInfo);

    CPackLimitBossTeamInfo m_stBossTeamInfo;
};

class CCrossLimitBossLevelEnd
{
public:
    CCrossLimitBossLevelEnd():m_iOwnerUin(0),m_iOwnerGroupID(0),m_iLevelID(0){}

    MSGPACK_DEFINE(m_iOwnerUin,m_iOwnerGroupID,m_iLevelID);

    int32_t m_iOwnerUin;
    int32_t m_iOwnerGroupID;
    int32_t m_iLevelID;
};


