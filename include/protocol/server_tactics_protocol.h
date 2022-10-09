#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

////////////////////////////////////////////////////////////////////////////////////////////
class CResponseAvatarInfo : public CPackUserSyncData
{
public:
	CResponseAvatarInfo() : m_stAvatarIconIDList(), m_stAvatarBorderIDList() {}

	MSGPACK_DEFINE(m_stSyncData, m_stAvatarIconIDList, m_stAvatarBorderIDList);

	std::vector<int>        m_stAvatarIconIDList;
	std::vector<int>        m_stAvatarBorderIDList;
};

class CRequestSetAvatar
{
public:
	CRequestSetAvatar() : m_iAvatarID(0) {}

	MSGPACK_DEFINE(m_iAvatarID);

	int         m_iAvatarID;
};

class CResponseSetAvatar : public CPackUserSyncData
{
public:
	CResponseSetAvatar() : m_iAvatarID(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iAvatarID);

	int         m_iAvatarID;
};

class CRequestAddBGM
{
public:
    enum
    {
        pos_main = 0,       // 主城
        pos_home = 1,       // 家园
    };
    CRequestAddBGM() : m_stBgmIDs(), m_iPos(0) {}

    MSGPACK_DEFINE(m_stBgmIDs, m_iPos);

    std::list<int32_t>    m_stBgmIDs;     // 如果是删除消息，填空，表示清空播放列表
    int32_t                 m_iPos;
};

class CResponseAddBGM : public CPackUserSyncData
{
public:
    CResponseAddBGM() : m_stBgmIDs(), m_iPos(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_stBgmIDs, m_iPos);

    std::list<int32_t>    m_stBgmIDs;     // 如果是删除消息，填空，表示清空播放列表
    int32_t                 m_iPos;
};

class CRequestSetThemeSkin
{
public:
    CRequestSetThemeSkin() : m_iSkinID(0),isDefault(false) {}

    MSGPACK_DEFINE(m_iSkinID,isDefault);

    int32_t     m_iSkinID;
    bool isDefault;
};

class CResponseSetThemeSkin : public CPackUserSyncData
{
public:
    CResponseSetThemeSkin() : m_iSkinID(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iSkinID);

    int32_t     m_iSkinID;
};

class CResponseGetBadgeList : public CPackUserSyncData
{
public:
    CResponseGetBadgeList() : m_stList() {}

    MSGPACK_DEFINE(m_stSyncData, m_stList);

    std::vector<int>    m_stList;
};

class CRequestSetBadgeList
{
public:
    CRequestSetBadgeList() : m_stList() {}

    MSGPACK_DEFINE(m_stList);

    std::vector<int>    m_stList;
};

