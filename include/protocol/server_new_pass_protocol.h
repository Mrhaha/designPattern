//
// Created by czy on 2022/4/26.
//

#pragma once

#include "msgpack.hpp"
#include "server_base_protocol.h"

class CResponsePassInfo: public CPackUserSyncData
{
public:
    CResponsePassInfo() : m_iTotalExp(),m_buyNewPassDepositID(0), m_stNormalRewardLevel(), m_stAdvancedRewardLevel(){}

    MSGPACK_DEFINE(m_stSyncData, m_iTotalExp,m_buyNewPassDepositID,m_stNormalRewardLevel, m_stAdvancedRewardLevel);

    int32_t m_iTotalExp;                                //通行证总经验
    int32_t m_buyNewPassDepositID;                      //是否购买高级通行证
    std::vector<int32_t> m_stNormalRewardLevel;         //已经领取的普通奖励等级
    std::vector<int32_t> m_stAdvancedRewardLevel;       //已经领取的高级奖励等级
};

class CRequestNewPassGetReward
{
public:
    CRequestNewPassGetReward(): m_stNormalLevels(),m_stAdvancedLevels(){}
    MSGPACK_DEFINE(m_stNormalLevels,m_stAdvancedLevels);

    std::vector<int32_t> m_stNormalLevels;          //领取普通奖励的目标等级
    std::vector<int32_t> m_stAdvancedLevels;        //领取高级奖励的目标等级
};


class CResponseNewPassGetReward: public CPackUserSyncData
{
public:
    CResponseNewPassGetReward() : m_stGetReward(),m_stNormalLevels(),m_stAdvancedLevels(){}

    MSGPACK_DEFINE(m_stSyncData, m_stGetReward,m_stNormalLevels,m_stAdvancedLevels);

    CPackTotalItemData      m_stGetReward;
    std::vector<int32_t>    m_stNormalLevels;          //领取成功普通奖励的目标等级
    std::vector<int32_t>    m_stAdvancedLevels;        //领取成功高级奖励的目标等级
};