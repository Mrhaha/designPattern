#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CPackMonthPassTask
{
public:
    MSGPACK_DEFINE(m_iTaskID, m_iProgress, m_iState);

    int32_t m_iTaskID {0};
    int32_t m_iProgress {0};
    int32_t m_iState {0};
};

//MSG_LOGIC_MONTH_PASS_NOTIFY = 10851,                //月通行证数据通知
class CNotifyMonthPass : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_iExp, m_iRecharge, m_strAwardBitmap, m_strDepositAwardBitmap, m_stTaskVec, m_iExchangeCount, m_iWishEventID, m_iWishItemIndexVec, m_iWishItemExtraNum);

    int32_t m_iExp;
    int32_t m_iRecharge;
    std::string m_strAwardBitmap;
    std::string m_strDepositAwardBitmap;
    std::vector<CPackMonthPassTask> m_stTaskVec;    //任务
    std::map<int32_t, int32_t> m_iExchangeCount;    //已兑换的次数
    int32_t m_iWishEventID;                         //今日许愿出现的事件ID
    std::vector<int32_t> m_iWishItemIndexVec;       //许愿九宫格选择的道具
    std::map<int32_t, int32_t> m_iWishItemExtraNum;   //许愿道具额外增加的数量
};


//MSG_LOGIC_MONTH_PASS_TASK_NOTIFY = 10852,           //月通行证任务数据通知
class CNotifyMonthPassTask : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_stTask);

    CPackMonthPassTask m_stTask;
};

//MSG_LOGIC_GET_MONTH_PASS_AWARD = 10853,             //获取月通行证奖励
class CRequestGetMonthPassAward
{
public:
    MSGPACK_DEFINE(m_iIndex);

    int32_t m_iIndex;
};

class CResponseGetMonthPassAward : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_iIndex, m_stItemVec);

    int32_t m_iIndex;
    std::vector<CPackGameItem> m_stItemVec;
};

//MSG_LOGIC_GET_MONTH_PASS_TASK_AWARD = 10854,        //获取月通行证任务奖励
class CRequestGetMonthPassTaskAward
{
public:
    MSGPACK_DEFINE(m_iTaskID);

    int32_t m_iTaskID;
};

class CResponseGetMonthPassTaskAward : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_iTaskID, m_stItemVec);

    int32_t m_iTaskID;
    std::vector<CPackGameItem> m_stItemVec;
};


//MSG_LOGIC_MONTH_PASS_EXCHANGE = 10855,              //请求月通行证道具兑换
class CRequestMonthPassExchange
{
public:
    MSGPACK_DEFINE(m_iExchangeID);

    int32_t m_iExchangeID;
};

class CResponseMonthPassExchange : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_iExchangeID, m_stConsumeItem, m_stBonusItem);

    int32_t m_iExchangeID;
    CPackGameItem m_stConsumeItem;
    CPackGameItem m_stBonusItem;
};


//MSG_LOGIC_MONTH_PASS_WISH_ITEM = 10856,             //请求月通行证许愿池道具
class CRequestMonthPassWishItem
{
public:
    MSGPACK_DEFINE(m_iPoolIndex, m_iItemIndex);

    int32_t m_iPoolIndex;
    int32_t m_iItemIndex;
};

class CResponseMonthPassWishItem : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_iPoolIndex, m_iItemIndex);

    int32_t m_iPoolIndex;
    int32_t m_iItemIndex;
};


//MSG_LOGIC_MONTH_PASS_DAILY_WISH = 10857,            //请求月通行证每日许愿
class CResponseMonthPassDailyWish : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_iEventID);

    int32_t m_iEventID;
};
