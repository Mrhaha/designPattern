#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

class CRequestLevelUpAtlas
{
public:
    CRequestLevelUpAtlas() : m_iAreaID(0) {}

    MSGPACK_DEFINE(m_iAreaID);

    int32_t m_iAreaID;          //升级的地区id
};

class CResponseLevelUpAtlas: public CPackUserSyncData
{
public:
    CResponseLevelUpAtlas() : m_iAreaID(0),m_iCurLevel(0) {}

    MSGPACK_DEFINE(m_stSyncData,m_iAreaID,m_iCurLevel);

    int32_t m_iAreaID;          //升级的地区id
    int32_t m_iCurLevel;        //当前等级
};

class CResponseGetAtlasInfo : public CPackUserSyncData
{
public:
    CResponseGetAtlasInfo() : m_mapBuffID2Level(){}

    MSGPACK_DEFINE(m_stSyncData, m_mapBuffID2Level);

    std::map<int32_t,int32_t> m_mapBuffID2Level; //<buffID,Level>;

};