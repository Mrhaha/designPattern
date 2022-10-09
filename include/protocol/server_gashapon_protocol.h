//
// Created by czy on 2022/8/8.
//

#pragma once

#include "msgpack.hpp"
#include "server_base_protocol.h"

class CResponseGashapon: public CPackUserSyncData
{
public:
    CResponseGashapon() : m_mapGashaponInfo(){}

    MSGPACK_DEFINE(m_stSyncData, m_mapGashaponInfo);

    std::map<int32_t,std::map<int32_t,CPackGashaponInfo>> m_mapGashaponInfo; //key1 gashaponID, key2=index


};

class CRequestPlayGashapon
{
public:
    CRequestPlayGashapon():m_iGashID(0),m_iCount(0){}

    MSGPACK_DEFINE(m_iGashID,m_iCount);

    int32_t m_iGashID;
    int32_t m_iCount;
};

class CResponsePlayGashaponGameItem: public CPackUserSyncData
{

public:
    CResponsePlayGashaponGameItem():m_stRewardItem(),m_vecIndex(),m_iGashID(0){}

    MSGPACK_DEFINE(m_stSyncData, m_stRewardItem,m_vecIndex,m_iGashID);

    CPackTotalItemData m_stRewardItem;
    std::vector<int32_t> m_vecIndex;
    int32_t m_iGashID;
};


class CResponseGetCrushBossInfo: public CPackUserSyncData
{

public:
    CResponseGetCrushBossInfo():m_stBossInfo(),m_iCrushBossCount(0){}

    MSGPACK_DEFINE(m_stSyncData, m_stBossInfo,m_iCrushBossCount);

    CPackCrushBossInfo m_stBossInfo;
    int32_t m_iCrushBossCount;

};

class CRequestPlayCrushBoss
{
public:
    CRequestPlayCrushBoss():m_iCrushBossLevelID(0){}

    MSGPACK_DEFINE(m_iCrushBossLevelID);

    int32_t m_iCrushBossLevelID;
};

class CRequestEndCrushBoss
{
public:
    CRequestEndCrushBoss():m_iLevelID(0),m_ulDamage(0){}

    MSGPACK_DEFINE(m_iLevelID,m_ulDamage);

    int32_t m_iLevelID;
    uint64_t m_ulDamage;
};

class CResponseEndCrushBoss: public CPackUserSyncData
{
public:
    CResponseEndCrushBoss():m_stCrushReward(){}

    MSGPACK_DEFINE(m_stSyncData,m_stCrushReward);

    CPackTotalItemData m_stCrushReward;
};

class CRequestGiveUpCrushBoss
{

public:
    CRequestGiveUpCrushBoss():m_iLevelID(0){}

    MSGPACK_DEFINE(m_iLevelID);

    int32_t m_iLevelID;
};

class CResponseGiveUpCrushBoss: public CPackUserSyncData
{
public:
    CResponseGiveUpCrushBoss():m_stBossInfo(){}

    MSGPACK_DEFINE(m_stSyncData,m_stBossInfo);

    CPackCrushBossInfo m_stBossInfo;
};


















