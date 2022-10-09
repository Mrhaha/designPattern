#pragma once

#include "msgpack.hpp"
#include "server_base_protocol.h"
#include "server_item_protocol.h"
#include "server_common_protocol.h"

////////////////////////////////////////////////////////////////////////////////////////
//IBUSY_GET_INFO_REQ = 10800,						//获取信息请求
class CRequestIbusyGetInfo
{
public:
	CRequestIbusyGetInfo()
		: m_iActivityID(0)
	{
	}

	MSGPACK_DEFINE(m_iActivityID);

	int             m_iActivityID;
};

//基础信息
class CPackIbusyBaseInfo
{
public:
	CPackIbusyBaseInfo()
		:m_iActivityID(0), m_iLevel(0), m_iExp(0), m_iScore(0), m_iHuoyue(0)
		, m_iTodayHuoyue(0), m_iChangeTaskCount(0), m_iMarketLastRefreshTime(0), m_iMarketRefreshTimes(0), m_iDailyExpAward(0) {}

	MSGPACK_DEFINE(m_iActivityID, m_iLevel, m_iExp, m_iScore, m_iHuoyue, m_iTodayHuoyue,
	        m_iChangeTaskCount, m_iMarketLastRefreshTime, m_iMarketRefreshTimes, m_iDailyExpAward);

	int				m_iActivityID;
	int				m_iLevel;
	int				m_iExp;
	int				m_iScore;
	int				m_iHuoyue;
	int				m_iTodayHuoyue;
	int				m_iChangeTaskCount;
	int				m_iMarketLastRefreshTime;
	int				m_iMarketRefreshTimes;
	int             m_iDailyExpAward;           //充值后每日经验是否领取
};

//宝库信息
class CPackIbusyMarketInfo
{
public:
	CPackIbusyMarketInfo() : m_stMarketInfo() {}

	MSGPACK_DEFINE(m_stMarketInfo);

	std::vector<CPackMarket>   m_stMarketInfo;
};

//MSG_LOGIC_IBUSY_INFO_NOTICE = 10801,						//信息通知
class CNotifyIbusyInfo : public CPackUserSyncData
{
public:
	CNotifyIbusyInfo() :m_stBaseInfo(), m_stMarketInfo(), m_stGettedLevelReward(), m_stTaskInfo(), m_stGettedDepositReward() {}

	MSGPACK_DEFINE(m_stSyncData, m_stBaseInfo, m_stMarketInfo, m_stGettedLevelReward, m_stTaskInfo, m_stGettedDepositReward);

	CPackIbusyBaseInfo				m_stBaseInfo;
	CPackIbusyMarketInfo			m_stMarketInfo;

	std::set<int>					m_stGettedLevelReward;
	CPackIbusyTaskData				m_stTaskInfo;
	std::set<int>                   m_stGettedDepositReward;    //充值后官邸奖励领取状态
};

//MSG_LOGIC_IBUSY_BASE_INFO_NOTICE = 10802,					//基础信息通知
class CNotifyIbusyBase : public CPackUserSyncData
{
public:
	CNotifyIbusyBase() : m_stBaseInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stBaseInfo);

	CPackIbusyBaseInfo	m_stBaseInfo;
};

//任务信息通知
class CNotifyIbusyTask : public CPackUserSyncData
{
public:
	CNotifyIbusyTask() : m_stTaskInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stTaskInfo);

	CPackIbusyTaskData	m_stTaskInfo;
};

//商场信息通知
class CNotifyIbusyMarket : public CPackUserSyncData
{
public:
	CNotifyIbusyMarket() : m_stMarketInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stMarketInfo);

	CPackIbusyMarketInfo	m_stMarketInfo;
};

//MSG_LOGIC_IBUSY_LEVELREWARD_INFO_NOTICE = 10806,			//官邸奖励领取信息通知
class CNotifyIbusyLevelReward : public CPackUserSyncData
{
public:
	CNotifyIbusyLevelReward() : m_stGettedLevelReward(), m_stGettedDepositReward() {}

	MSGPACK_DEFINE(m_stSyncData, m_stGettedLevelReward, m_stGettedDepositReward);

	std::set<int> 	m_stGettedLevelReward;
    std::set<int> 	m_stGettedDepositReward;    //充值后官邸奖励领取状态
};

//IBUSY_GET_LEVEL_REWARD_REQ = 10804,				//领取官邸奖励请求
class CRequestIbusyGetLevelReward
{
public:
	CRequestIbusyGetLevelReward()
		: m_iLevelID(0)
	{
	}

	MSGPACK_DEFINE(m_iLevelID);

	int             m_iLevelID;
};

class CResponseIbusyGetLevelReward : public CPackUserSyncData
{
public:
	CResponseIbusyGetLevelReward()
		: m_vtAwardItem()
	{
	}

	MSGPACK_DEFINE(m_stSyncData, m_vtAwardItem);

	std::vector<CPackGameItem> m_vtAwardItem;
};

//IBUSY_CHANGE_TASK_REQ = 10803,					//换一批任务请求
class CRequestIbusyChangeTask
{
public:
	CRequestIbusyChangeTask()
		: m_bFree(false)
	{
	}

	MSGPACK_DEFINE(m_bFree);

	bool             m_bFree;
};

//IBUSY_PAIQIAN_REQ = 10810,						//派遣请求
class CRequestIbusyPaiqian
{
public:
	CRequestIbusyPaiqian()
		:m_iTaskID(0)
		, m_iTeamMember1(0)
		, m_iTeamMember2(0)
		, m_iTeamMember3(0)
		, m_iTeamMember4(0)
		, m_iTeamMember5(0)
	{
	}

	MSGPACK_DEFINE(m_iTaskID, m_iTeamMember1, m_iTeamMember2, m_iTeamMember3, m_iTeamMember4, m_iTeamMember5);

	int									m_iTaskID;

	int									m_iTeamMember1;
	int									m_iTeamMember2;
	int									m_iTeamMember3;
	int									m_iTeamMember4;
	int									m_iTeamMember5;
};

class CResponseBuyIbusyPaiqian : public CPackUserSyncData
{
public:
	CResponseBuyIbusyPaiqian() : m_iScore(0), m_iExp(0), m_stReward(){}

	MSGPACK_DEFINE(m_stSyncData, m_iScore, m_iExp, m_stReward);

	int                                 m_iScore;
	int									m_iExp;
	CPackGameItem   m_stReward;
};

//IBUSY_MARKET_BUY_REQ = 10811,					//宝物库房购买请求
class CRequestBuyIbusyMarketCommodity
{
public:
	CRequestBuyIbusyMarketCommodity() : m_iCommodityID(0) {}

	MSGPACK_DEFINE(m_iCommodityID);

	int         m_iCommodityID;
};

class CResponseBuyIbusyMarketCommodity : public CPackUserSyncData
{
public:
	CResponseBuyIbusyMarketCommodity() : m_iCommodityID(0), m_stCommodity(), m_stConsume(){}

	MSGPACK_DEFINE(m_stSyncData, m_iCommodityID, m_stCommodity, m_stConsume);

	int                                 m_iCommodityID;
	CPackGameItem   m_stCommodity;
	CPackGameItem   m_stConsume;
};

//MSG_LOGIC_IBUSY_GET_DEPOSIT_LEVEL_AWARD = 10814,			//领取充值官邸奖励请求
class CRequestIbusyGetDepositLevelAward
{
public:
    CRequestIbusyGetDepositLevelAward()
            : m_iLevelID(0)
    {
    }

    MSGPACK_DEFINE(m_iLevelID);

    int             m_iLevelID;
};

class CResponseIbusyGetDepositLevelAward : public CPackUserSyncData
{
public:
    CResponseIbusyGetDepositLevelAward()
            : m_vtAwardItem()
    {
    }

    MSGPACK_DEFINE(m_stSyncData, m_vtAwardItem);

    std::vector<CPackGameItem> m_vtAwardItem;
};


//MSG_LOGIC_IBUSY_GET_DEPOSIT_EXP_AWARD = 10815,				//领取充值每日经验
class CResponseIbusyGetDepositExpAward : public CPackUserSyncData
{
public:
    CResponseIbusyGetDepositExpAward()
            : m_iAddExp(0)
    {
    }

    MSGPACK_DEFINE(m_stSyncData, m_iAddExp);

    int32_t m_iAddExp;
};

