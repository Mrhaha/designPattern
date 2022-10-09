#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

enum EChargeActivityTaskType
{
	charge_task_login = 1,
	charge_task_recharge = 2,
	charge_task_buy = 3,
};

enum EChargeActivityTaskState
{
	charge_task_cannot_rewad = 0,
	charge_task_can_rewad = 1,
	charge_task_get_rewad = 2,
};

////////////////////////////////////////////////////////////////////////////////////////
class CRequestGetChargeActivityInfo
{
public:
	CRequestGetChargeActivityInfo() : m_iActivityID(0)
	{}

	MSGPACK_DEFINE(m_iActivityID);

	int								m_iActivityID;	//活动id（双十二狂欢）
};

class CPackChargeActivityTask
{
public:
	CPackChargeActivityTask() : m_iTaskType(0), m_iID(0), m_iTarget(0), m_iProgress(0), m_cState(0)
	{}

	MSGPACK_DEFINE(m_iTaskType, m_iID, m_iTarget, m_iProgress, m_cState);

	int             m_iTaskType;
	int             m_iID;
	int				m_iTarget;
	int				m_iProgress;
	char			m_cState;
};

class CNotifyChargeTaskChange : public CPackUserSyncData
{
public:
	CNotifyChargeTaskChange() : m_stTask()
	{}

	MSGPACK_DEFINE(m_stSyncData, m_stTask);

	int								m_iActivityID;	//活动id（双十二狂欢）
	CPackChargeActivityTask			m_stTask;
};

class CNotifyChargeActivity : public CPackUserSyncData
{
public:
	CNotifyChargeActivity() : m_iActivityID(0), m_stLoginTask(), m_stChargeTask(), m_stBuyTask()
	{
	}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_stLoginTask, m_stChargeTask, m_stBuyTask);

	int										m_iActivityID;
	std::vector<CPackChargeActivityTask>	m_stLoginTask;
	std::vector<CPackChargeActivityTask>	m_stChargeTask;
	std::vector<CPackChargeActivityTask>	m_stBuyTask;
};

//领取奖励
class CRequestChargeActivityGetReward
{
public:
	CRequestChargeActivityGetReward()
		: m_iActivityID(0)
		, m_iTaskID(0)
	{
	}

	MSGPACK_DEFINE(m_iActivityID, m_iTaskID);

	int             m_iActivityID;
	int             m_iTaskID;
};

class CResponseChargeActivityGetReward : public CPackUserSyncData
{
public:
	CResponseChargeActivityGetReward() : m_vtAwardItem() {}

	MSGPACK_DEFINE(m_stSyncData, m_vtAwardItem);

	std::vector<CPackGameItem> m_vtAwardItem;
};

//礼包购买
class CRequestChargeActivityBuy
{
public:
	CRequestChargeActivityBuy()
		: m_iActivityID(0)
		, m_iTaskID(0)
	{
	}

	MSGPACK_DEFINE(m_iActivityID, m_iTaskID);

	int             m_iActivityID;
	int             m_iTaskID;
};

class CResponseChargeActivityBuy : public CPackUserSyncData
{
public:
	CResponseChargeActivityBuy() : m_vtAwardItem() {}

	MSGPACK_DEFINE(m_stSyncData, m_vtAwardItem);

	std::vector<CPackGameItem> m_vtAwardItem;
};

//充值预判断
class CRequestChargeActivityCharge
{
public:
	CRequestChargeActivityCharge()
		: m_iActivityID(0)
		, m_iTaskID(0)
	{
	}

	MSGPACK_DEFINE(m_iActivityID, m_iTaskID);

	int             m_iActivityID;
	int             m_iTaskID;
};
