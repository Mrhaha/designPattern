#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
#include "server_common_protocol.h"

class CResponseGetRuneInfo : public CPackUserSyncData
{
public:
    CResponseGetRuneInfo() : m_stRuneInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stRuneInfo);

    std::map<int32_t,CPackRuneInfo> m_stRuneInfo;
};


class CRequestUnlockRune
{
public:
    CRequestUnlockRune() : m_iRuneID(0) {}

    MSGPACK_DEFINE(m_iRuneID);

    int32_t m_iRuneID;
};

class CResponseUnlockRune : public CPackUserSyncData
{
public:
    CResponseUnlockRune() : m_iRuneID(),m_iUnlockItemID(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iRuneID,m_iUnlockItemID);

    int32_t m_iRuneID;
    int32_t m_iUnlockItemID;
};

class CRequestRuneLevelUp
{
public:
    CRequestRuneLevelUp():m_iRuneID(0){}

    MSGPACK_DEFINE(m_iRuneID);

    int32_t m_iRuneID;
};

class CResponseRuneLevelUp: public CPackUserSyncData
{
public:
    CResponseRuneLevelUp():m_iRuneID() {}

    MSGPACK_DEFINE(m_stSyncData, m_iRuneID);

    int32_t m_iRuneID;
};