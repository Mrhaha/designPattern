#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

////////////////////////////////////////////////////////////////////////////////////////
//物品出售
class CRequestSellGameItem
{
public:
	CRequestSellGameItem() : m_stItem(), m_stFishIndex() {}

	MSGPACK_DEFINE(m_stItem, m_stFishIndex);

	CPackGameItem       m_stItem;
    std::vector<int>    m_stFishIndex;
};

class CResponseSellGameItem : public CPackUserSyncData
{
public:
	CResponseSellGameItem() : m_stItemData(), m_stFishIndex() {}

	MSGPACK_DEFINE(m_stSyncData, m_stItemData, m_stFishIndex);

	CPackTotalItemData  m_stItemData;
    std::vector<int>    m_stFishIndex;
};

class CPackCommodityInfo
{
public:
    CPackCommodityInfo() : m_iCommodityID(0), m_iDailyLimitPayTimes(0), m_iLastPayTime(0), m_iPt(0), m_iTotalTimes(0),m_iBaoDiTimes(0) {}

    MSGPACK_DEFINE(m_iCommodityID, m_iDailyLimitPayTimes, m_iLastPayTime, m_iPt, m_iTotalTimes,m_iBaoDiTimes);

    int         m_iCommodityID;
    int         m_iDailyLimitPayTimes;
    int         m_iLastPayTime;
    int         m_iPt;
    int         m_iTotalTimes;
    int         m_iBaoDiTimes;          //距离上次三星抽出三星的抽数（初始是0）
};

class CResponseGetCommodityInfo : public CPackUserSyncData
{
public:
    CResponseGetCommodityInfo() : m_stInfoVec(), m_iMallBaodiTimes(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_stInfoVec, m_iMallBaodiTimes);

    std::vector<CPackCommodityInfo> m_stInfoVec;
    int m_iMallBaodiTimes;
};

class CRequestBuyCommodity
{
public:
    enum
    {
        E_LimitPay = 0,     // 付费钻石
        E_Single = 1,       // 单抽
        E_Ten = 2,          // 十连
    };

    CRequestBuyCommodity() : m_iCommodityID(0), m_iBuyType(0), m_bExchangeItem(false),m_iConsumeIndex(0) {}

    MSGPACK_DEFINE(m_iCommodityID, m_iBuyType, m_bExchangeItem,m_iConsumeIndex);

    int32_t     m_iCommodityID;
    int32_t     m_iBuyType;         // 自定义枚举
    bool        m_bExchangeItem;    // 是否是消耗道具
    int32_t     m_iConsumeIndex;    // 消耗十连道具index
};

class CResponseBuyCommodity : public CPackUserSyncData
{
public:
    CResponseBuyCommodity() : m_stCommodityInfo(), m_stReward(), m_stItemData(), m_iMallBaodiTimes(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_stCommodityInfo, m_stReward, m_stItemData, m_iMallBaodiTimes);

    CPackCommodityInfo  m_stCommodityInfo;
    std::vector<CPackGameItem> m_stReward;          // 仅做表现用
    CPackTotalItemData  m_stItemData;
    int m_iMallBaodiTimes;
};

class CRequestCommodityPtExchange
{
public:
    CRequestCommodityPtExchange() : m_iCommodityID(0), m_iCardID(0) {}

    MSGPACK_DEFINE(m_iCommodityID, m_iCardID);

    int32_t             m_iCommodityID;
    int32_t             m_iCardID;
};

class CResponseCommodityPtExchange : public CPackUserSyncData
{
public:
    CResponseCommodityPtExchange() : m_iCommodityID(0), m_iPt(0), m_stReward() {}

    MSGPACK_DEFINE(m_stSyncData, m_iCommodityID, m_iPt, m_stReward);

    int32_t             m_iCommodityID;
    int32_t             m_iPt;
    CPackGameItem       m_stReward;
};

////////////////////////////////////////////////////////////////////////////////////////
// vip 礼包
class CRequestBuyVipBag
{
public:
	CRequestBuyVipBag() : m_iVipLevel(0) {}

	MSGPACK_DEFINE(m_iVipLevel);

	int         m_iVipLevel;
};

class CResponseBuyVipBag : public CPackUserSyncData
{
public:
	CResponseBuyVipBag() : m_strVipBagBitmap(""), m_stItemList() {}

	MSGPACK_DEFINE(m_stSyncData, m_strVipBagBitmap, m_stItemList);

	std::string                                     m_strVipBagBitmap;
	std::vector<CPackGameItem>  m_stItemList;
};

////////////////////////////////////////////////////////////////////////////////
//邮件系统
class CResponseGetUserMail : public CPackUserSyncData
{
public:
	CResponseGetUserMail() : m_stUserMailVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stUserMailVec);

	struct user_mail_info_t
	{
		user_mail_info_t()
			: m_iMailID(0), m_cIsNew(0), m_cIsReceived(0),
			m_strTitle(""), m_strSender(""), m_strContent(""),
			m_stAttachment(), m_iAwardMultiple(1),m_iExpiredTime(0) { }

		MSGPACK_DEFINE(m_iMailID,
			m_cIsNew,
			m_cIsReceived,
			m_iCreateTime,
			m_strTitle,
			m_strSender,
			m_strContent,
			m_stAttachment,
			m_iAwardMultiple,m_iExpiredTime);

		int32_t                                     m_iMailID;
		char                                        m_cIsNew;
		char                                        m_cIsReceived;
		int                                         m_iCreateTime;
		std::string                                 m_strTitle;
		std::string                                 m_strSender;
		std::string                                 m_strContent;
		std::vector<CPackGameItem>   m_stAttachment;
		int32_t										m_iAwardMultiple;
        int32_t                                     m_iExpiredTime;
	};

	std::vector<user_mail_info_t>       m_stUserMailVec;
};

class CRequestGetUserMailBonus
{
public:
	CRequestGetUserMailBonus() : m_iMailIDVec() {}

	MSGPACK_DEFINE(m_iMailIDVec);

    std::vector<int32_t>     m_iMailIDVec;
};

class CResponseGetUserMailBonus : public CPackUserSyncData
{
public:
	CResponseGetUserMailBonus() : m_iMailIDVec(), m_stAttachment() {}

	MSGPACK_DEFINE(m_stSyncData, m_iMailIDVec, m_stAttachment);

	std::vector<int32_t>     m_iMailIDVec;
	std::vector<CPackGameItem>   m_stAttachment;
};

class CRequestReadUserMail
{
public:
	CRequestReadUserMail() : m_iMailID(0) {}

	MSGPACK_DEFINE(m_iMailID);

	int32_t         m_iMailID;
};

class CResponseReadUserMail : public CPackUserSyncData
{
public:
	CResponseReadUserMail() : m_iMailID(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iMailID);

    int32_t         m_iMailID;
};

class CRequestDelUserMail
{
public:
    CRequestDelUserMail() : m_iMailIDList() {}

    MSGPACK_DEFINE(m_iMailIDList);

    std::vector<int32_t>         m_iMailIDList;
};

class CResponseDelUserMail : public CPackUserSyncData
{
public:
    CResponseDelUserMail() : m_iMailIDList() {}

    MSGPACK_DEFINE(m_stSyncData, m_iMailIDList);

    std::vector<int32_t>         m_iMailIDList;
};

////////////////////////////////////////////////////////////////////////////////
//购买资源
class CPackBuyGold
{
public:
	CPackBuyGold() : m_iRate(1), m_stGetItem() { }

	MSGPACK_DEFINE(m_iRate, m_stGetItem);

	int                                     m_iRate;
	CPackGameItem       m_stGetItem;
};

class CResponseBuyGold : public CPackUserSyncData
{
public:
	CResponseBuyGold() : m_iRate(), m_stConsumeItem(), m_stGetItem() {}

	MSGPACK_DEFINE(m_stSyncData, m_iRate, m_stConsumeItem, m_stGetItem);

	int                                     m_iRate;
	CPackGameItem       m_stConsumeItem;
	CPackGameItem       m_stGetItem;
};

class CResponseSequentialBuyGold : public CPackUserSyncData
{
public:
	CResponseSequentialBuyGold() : m_stConsumeItem(), m_stSequentialBuyGoldRsp() {}

	MSGPACK_DEFINE(m_stSyncData, m_stConsumeItem, m_stSequentialBuyGoldRsp);

	CPackGameItem               m_stConsumeItem;
	std::vector<CPackBuyGold>       m_stSequentialBuyGoldRsp;
};

class CResponseBuyResource : public CPackUserSyncData
{
public:
	CResponseBuyResource() : m_stConsumeItem(), m_stGetItem() {}

	MSGPACK_DEFINE(m_stSyncData, m_stConsumeItem, m_stGetItem);

	CPackGameItem   m_stConsumeItem;
	CPackGameItem   m_stGetItem;
};

class CRequestRefreshLevelTimes
{
public:
    CRequestRefreshLevelTimes() : m_iLevelID(0) { }

	MSGPACK_DEFINE(m_iLevelID);

	int     m_iLevelID;
};

class CResponseRefreshLevelTimes : public CPackUserSyncData
{
public:
    CResponseRefreshLevelTimes() : m_stLevelInfo(), m_stConsume() {}

    MSGPACK_DEFINE(m_stSyncData, m_stLevelInfo, m_stConsume);

    CPackLevelInfo      m_stLevelInfo;
    CPackGameItem       m_stConsume;
};

class CResponseBuyRefresh : public CPackUserSyncData
{
public:
	CResponseBuyRefresh() : m_iLeftTimes() {}

	MSGPACK_DEFINE(m_stSyncData, m_iLeftTimes);

	int                 m_iLeftTimes;
};

////////////////////////////////////////////////////////////////////////
class CRequestRandomStoreGetInfo
{
public:
    CRequestRandomStoreGetInfo() : m_iStoreID(0) {}

    MSGPACK_DEFINE(m_iStoreID);

    int32_t     m_iStoreID;
};

class CResponseRandomStoreGetInfo : public CPackUserSyncData
{
public:
    CResponseRandomStoreGetInfo() : m_iStoreID(0), m_stStoreInfo() {}

    MSGPACK_DEFINE(m_stSyncData, m_iStoreID, m_stStoreInfo);

    int32_t     m_iStoreID;
    std::vector<CPackMarket>    m_stStoreInfo;
};

class CRandomStoreInfoPack
{
public:
    CRandomStoreInfoPack() : m_iStoreID(0), m_stStoreInfo(), m_bSoldOut(false) {}

    MSGPACK_DEFINE(m_iStoreID, m_stStoreInfo, m_bSoldOut);

    int32_t     m_iStoreID;
    std::vector<CPackMarket>    m_stStoreInfo;
    bool        m_bSoldOut;
};

class CRequestRandomStoreInfoList
{
public:
    CRequestRandomStoreInfoList() : m_stStoreIDList() {}

    MSGPACK_DEFINE(m_stStoreIDList);

    std::vector<int32_t>    m_stStoreIDList;
};

class CResponseRandomStoreInfoList : public CPackUserSyncData
{
public:
    CResponseRandomStoreInfoList() : m_stInfoList() {}

    MSGPACK_DEFINE(m_stSyncData, m_stInfoList);

    std::vector<CRandomStoreInfoPack> m_stInfoList;
};

class CRequestRandomStoreBuyItem
{
public:
    CRequestRandomStoreBuyItem() : m_iStoreID(0), m_iCommodityID(0) {}

    MSGPACK_DEFINE(m_iStoreID, m_iCommodityID);

    int32_t     m_iStoreID;
    int32_t     m_iCommodityID;
};

class CResponseRandomStoreBuyItem : public CPackUserSyncData
{
public:
    CResponseRandomStoreBuyItem() : m_iStoreID(0), m_iCommodityID(0), m_stCommodity(), m_stConsume() {}

    MSGPACK_DEFINE(m_stSyncData, m_iStoreID, m_iCommodityID, m_stCommodity, m_stConsume);

    int32_t     m_iStoreID;
    int32_t     m_iCommodityID;
    CPackGameItem   m_stCommodity;
    CPackGameItem   m_stConsume;
};

// 好感度商店互动
class CResponseHeartStoreDialog : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_iHeroCardID, m_iHeartLevel, m_iHeartExp, m_iDailyGiftNum, m_iLastGiftTime);

    int32_t                 m_iHeroCardID;
    int32_t                 m_iHeartLevel;
    int32_t                 m_iHeartExp;
    int32_t                 m_iDailyGiftNum;
    int32_t                 m_iLastGiftTime;
};

