#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
class CRequestCombineGameItem
{
public:
	CRequestCombineGameItem() : m_iItemID(0), m_iTimes(0) {}

	MSGPACK_DEFINE(m_iItemID, m_iTimes);

	int32_t     m_iItemID;
	int32_t     m_iTimes;
};

class CResponseCombineGameItem : public CPackUserSyncData
{
public:
	CResponseCombineGameItem() : m_stCombineItem(), m_stConsumeItems() {}

	MSGPACK_DEFINE(m_stSyncData, m_stCombineItem, m_stConsumeItems);

	CPackGameItem               m_stCombineItem;        // 合成的道具
	std::vector<CPackGameItem>  m_stConsumeItems;       // 消耗的道具
};

////////////////////////////////////////////////////////////////////////////////////////
// 使用万能将魂
class CRequestChangeVariableSoul
{
public:
	CRequestChangeVariableSoul() {}

	MSGPACK_DEFINE(m_iCardID, m_iNum);

	int                 m_iCardID;
	int                 m_iNum;
};

class CResponseChangeVariableSoul : public CPackUserSyncData
{
public:
	CResponseChangeVariableSoul() {}

	MSGPACK_DEFINE(m_stSyncData, m_iCardID, m_iNum);

	int                 m_iCardID;
	int                 m_iNum;
};

////////////////////////////////////////////////////////////////////////////////////////
// 购买刷新令
class CRequestBuyRefreshToken
{
public:
	CRequestBuyRefreshToken() : m_iNum(0) {}

	MSGPACK_DEFINE(m_iNum);

	int             m_iNum;
};

class CResponseBuyRefreshToken : public CPackUserSyncData
{
public:
	CResponseBuyRefreshToken() {}

	MSGPACK_DEFINE(m_stSyncData, m_iNum);

	int             m_iNum;
};

//SHOP_ITEM = 10250, //购买物品
class CRequestShopItem
{
public:
	CRequestShopItem() : m_iID(0), m_iNum(0), m_bOrConsume(false),m_bConstelShop(false) {}

	MSGPACK_DEFINE(m_iID, m_iNum, m_bOrConsume,m_bConstelShop);

	int32_t         m_iID;
	int32_t         m_iNum;
    bool            m_bOrConsume;           // 默认用物品1购买，true 表示用物品2购买
    bool            m_bConstelShop;         // 是否是念写卡商店
};

class CResponseShopItem : public CPackUserSyncData
{
public:
	CResponseShopItem() : m_iID(0), m_iNum(0), m_stItemData(),m_vecSucBuyIndex() {}

	MSGPACK_DEFINE(m_stSyncData, m_iID, m_iNum, m_stItemData,m_vecSucBuyIndex);

	int32_t             m_iID;
    int32_t             m_iNum;
	CPackTotalItemData  m_stItemData;
    std::vector<int32_t> m_vecSucBuyIndex;
};

//NOTIFY_SHOP_COUNT = 10318,					//通知已购买次数
class CNotifyShopCount : public CPackUserSyncData
{
public:
	CNotifyShopCount() : m_stCountMap() {}

	MSGPACK_DEFINE(m_stSyncData, m_stCountMap);

	std::map<int32_t, int32_t> m_stCountMap;
};

class CNotifyItemAttr : public CPackUserSyncData
{
public:
    CNotifyItemAttr() : m_stAttr() {}

    MSGPACK_DEFINE(m_stSyncData, m_stAttr);

    CPackItemAttr   m_stAttr;
};

class CRequestUseItem
{
public:
    CRequestUseItem() : m_iItemID(0), m_iNum(0) {}

	MSGPACK_DEFINE(m_iItemID, m_iNum);

	int32_t m_iItemID;
	int32_t m_iNum;
};

class CResponseUseItem : public CPackUserSyncData
{
public:
    CResponseUseItem() : m_stBonus() {}

	MSGPACK_DEFINE(m_stSyncData, m_stBonus);

    CPackTotalItemData      m_stBonus;
};

class CResponseRefreshConstelShop : public CPackUserSyncData
{
public:
    CResponseRefreshConstelShop() : m_vecShopInfo(),m_vecConsumeItem() {}

    MSGPACK_DEFINE(m_stSyncData, m_vecShopInfo,m_vecConsumeItem);

    std::vector<CPackConstelShopInfo> m_vecShopInfo;
    CPackTotalItemData m_vecConsumeItem;
};

class CResponseConstelShopInfo : public CPackUserSyncData
{
public:
    CResponseConstelShopInfo() : m_vecShopInfo(),m_iRefreshCount(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_vecShopInfo,m_iRefreshCount);

    std::vector<CPackConstelShopInfo> m_vecShopInfo;
    int32_t m_iRefreshCount;            //念写卡商店今日刷新次数
};
