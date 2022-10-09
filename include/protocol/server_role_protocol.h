#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

class CResponseGetUserDepositInfo : public CPackUserSyncData
{
public:
	CResponseGetUserDepositInfo() : m_stLeftDoubleCount(), m_stLeftBuyCount(), m_stLastDepositTime() {}

	MSGPACK_DEFINE(m_stSyncData, m_stLeftDoubleCount, m_stLeftBuyCount, m_stLastDepositTime);

	std::map<int32_t, int32_t> m_stLeftDoubleCount;
	std::map<int32_t, int32_t> m_stLeftBuyCount;
	std::map<int32_t, int32_t> m_stLastDepositTime;
};

class CPackHeroHistory
{
public:
    CPackHeroHistory() : m_iIndex(0), m_iUnlockTime(0), m_iContentIndex(0), m_iRewardTag(0) {}

    MSGPACK_DEFINE(m_iIndex, m_iUnlockTime, m_iContentIndex, m_iRewardTag);

    int32_t     m_iIndex;
    int32_t     m_iUnlockTime;
    int32_t     m_iContentIndex;
    int8_t      m_iRewardTag;
};

class CNotifyUserOtherData : public CPackUserSyncData
{
public:
    CNotifyUserOtherData() : m_stHistory(), m_stTeamPresetVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stHistory, m_stTeamPresetVec);

    std::map<int32_t, std::vector<CPackHeroHistory>>    m_stHistory;            // 伙伴传记
    std::vector<CPackTeamPreset>                        m_stTeamPresetVec;      // 编队
};

class CNotifyNewHeroHistory : public CPackUserSyncData
{
public:
    CNotifyNewHeroHistory() : m_iCardID(0), m_stHistory() {}

    MSGPACK_DEFINE(m_stSyncData, m_iCardID, m_stHistory);

    int32_t             m_iCardID;
    CPackHeroHistory    m_stHistory;
};
