#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
#include "server_common_protocol.h"

////////////////////////////////////////////////////////////////////////////////////////
class CPackQiyuNodeData
{
public:
	CPackQiyuNodeData()
		:m_iNodeID(0), m_iNodeState(0), m_iEventType(0), m_iEventID(0), m_aParams(), m_aStrParams()
	{
	}

	MSGPACK_DEFINE(m_iNodeID, m_iNodeState, m_iEventType, m_iEventID, m_aParams, m_aStrParams);

	int							m_iNodeID;
	int							m_iNodeState;	//0-未打开，1-已打开，2-已完成
	int							m_iEventType;
	int							m_iEventID;
	std::vector<int>			m_aParams;
	std::vector<std::string>	m_aStrParams;
};

//单个奇遇信息
class CPackQiyuMapData
{
public:
	CPackQiyuMapData() : m_iID(0), m_iEndTime(0), m_aNodes(), m_stBuffVec(), m_mpHeroCostHp()
	    , m_iCangbaotuID(), m_strCangbaotuSuipianBitMap(), m_stTeam() {}

	MSGPACK_DEFINE(m_iID, m_iEndTime, m_aNodes, m_stBuffVec, m_mpHeroCostHp, m_iCangbaotuID, m_strCangbaotuSuipianBitMap, m_stTeam);

	int											m_iID;
	int											m_iEndTime;

	std::vector<CPackQiyuNodeData>		m_aNodes;
	CPackEntireWarBuffVector			m_stBuffVec;
	std::unordered_map<int, int>				m_mpHeroCostHp;
	int											m_iCangbaotuID;
	std::string									m_strCangbaotuSuipianBitMap;

    CPackCardTeam                       m_stTeam;
};

class CNotifyQiyuInfo : public CPackUserSyncData
{
public:
	CNotifyQiyuInfo() : m_qiyuMap(), m_cangbaotuMap()
	{}

	MSGPACK_DEFINE(m_stSyncData, m_qiyuMap, m_cangbaotuMap);

	CPackQiyuMapData			m_qiyuMap;
	CPackQiyuMapData			m_cangbaotuMap;
};

////////////////////////////////////////////////////////////////////////////////////////
//QIYU_REFRESH_REQ = 10746,				//奇遇刷新请求（测试用）
class CRequestQiyuRefresh
{
public:
	CRequestQiyuRefresh()
		:m_bDayQiyuOrCangbaotu(false), m_iMapID(0)
	{
	}

	MSGPACK_DEFINE(m_bDayQiyuOrCangbaotu, m_iMapID);

	bool				m_bDayQiyuOrCangbaotu;
	int					m_iMapID;
};

//QIYU_MOVE_REQ = 10741,					//跳转到某个新位置
class CRequestQiyuMove
{
public:
	CRequestQiyuMove()
		:m_iMapID(0), m_iNodeID(0), m_aParams()
	{
	}

	MSGPACK_DEFINE(m_iMapID, m_iNodeID, m_aParams);

	int					m_iMapID;
	int					m_iNodeID;
	std::vector<int>	m_aParams;
};

//QIYU_EVENT_NOTICE = 10743,				//奇遇事件通知
class CNotifyQiyuEvent : public CPackUserSyncData
{
public:
	CNotifyQiyuEvent() : m_iEventType(0), m_iEventID(0), m_aParams(), m_aStrParams()
	{}

	MSGPACK_DEFINE(m_stSyncData, m_iEventType, m_iEventID, m_aParams, m_aStrParams);

	int								m_iEventType;
	int								m_iEventID;
	std::vector<int>				m_aParams;
	std::vector<std::string>		m_aStrParams;
};

//QIYU_FINISH_REQ = 10744,				//完成奇遇请求
class CRequestQiyuFinish
{
public:
	CRequestQiyuFinish()
		:m_iMapID(0), m_iNodeID(0), m_aParams()
	{
	}

	MSGPACK_DEFINE(m_iMapID, m_iNodeID, m_aParams);

	int					m_iMapID;
	int					m_iNodeID;
	std::vector<int>				m_aParams;
};

class CNotifyQiyuReward : public CPackUserSyncData
{
public:
	CNotifyQiyuReward() : m_stItemVec(), m_stBuff(), m_iSuipianIndex(0), m_iMapID(0), m_iNodeID(0), m_stUserCostHP(){}

	MSGPACK_DEFINE(m_stSyncData, m_stItemVec, m_stBuff, m_iSuipianIndex, m_iMapID, m_iNodeID, m_stUserCostHP);

	std::vector<CPackGameItem> m_stItemVec;
	CPackEntireWarBuff						m_stBuff;
	int												m_iSuipianIndex;
	int												m_iMapID;
	int												m_iNodeID;
	std::vector<std::pair<int, int>>				m_stUserCostHP;
};

//QIYU_RONGLIAN_REQ = 10747,				//熔炼请求
class CRequestQiyuRonglian
{
public:
	CRequestQiyuRonglian()
		:m_iMapID(0), m_iNodeID(0), m_stSourceItem()
	{
	}

	MSGPACK_DEFINE(m_iMapID, m_iNodeID, m_stSourceItem);

	int												m_iMapID;
	int												m_iNodeID;
	CPackGameItem				m_stSourceItem;
};

//QIYU_FIGHT_START = 10749,				//战斗开始
class CPackQiyuFightStart
{
public:
	CPackQiyuFightStart() :m_iMapID(0), m_iNodeID(0), m_stTeam() {}

	MSGPACK_DEFINE(m_iMapID, m_iNodeID, m_stTeam);

	int									m_iMapID;
	int									m_iNodeID;

    CPackCardTeam                       m_stTeam;
};

class CResponseQiyuFightStart : public CPackUserSyncData
{
public:
	CResponseQiyuFightStart()
		:m_iMapID(0), m_iNodeID(0)
	{
	}

	MSGPACK_DEFINE(m_stSyncData, m_iMapID, m_iNodeID);

	int									m_iMapID;
	int									m_iNodeID;
};

//QIYU_FIGHT_RESULT = 10748,				//战斗结果通知
class CRequestQiyuFightResult
{
public:
	CRequestQiyuFightResult()
		:m_iMapID(0), m_iNodeID(0), m_stCostHP(), m_stUserCostHP()
	{
	}

	MSGPACK_DEFINE(m_iMapID, m_iNodeID, m_stCostHP, m_stUserCostHP);

	int									m_iMapID;
	int									m_iNodeID;
	std::vector<int>                    m_stCostHP;
	std::vector<std::pair<int, int>>    m_stUserCostHP;
};

//QIYU_CANGBAOTU_DUIHUAN_REQ = 10750,		//藏宝图兑换请求
class CRequestQiyuCangbaotuDuihuan
{
public:
	CRequestQiyuCangbaotuDuihuan()
		:m_iCangbaotuID(0)
	{
	}

	MSGPACK_DEFINE(m_iCangbaotuID);

	int									m_iCangbaotuID;	
};

class CResponseQiyuCangbaotuDuihuan : public CPackUserSyncData
{
public:
	CResponseQiyuCangbaotuDuihuan()
		:m_stDuihuanItem()
	{
	}

	MSGPACK_DEFINE(m_stSyncData, m_stDuihuanItem);

	CPackGameItem						m_stDuihuanItem;
};

class CResponseQiyuCangbaotuHeCheng : public CPackUserSyncData
{
public:
    CResponseQiyuCangbaotuHeCheng() : m_stItems() {}

    MSGPACK_DEFINE(m_stSyncData, m_stItems);

    std::vector<CPackGameItem>	m_stItems;
};

//QIYU_CANGBAOTU_USE_REQ = 10751,			//藏宝图使用
class CRequestQiyuCangbaotuUse
{
public:
	CRequestQiyuCangbaotuUse()
		:m_iCangbaotuID(0)
	{
	}

	MSGPACK_DEFINE(m_iCangbaotuID);

	int									m_iCangbaotuID;
};
