//
// Created by lxhbroser on 2019/7/2.
//

#pragma once

#include "logic_game_cache_table.h"

class CLogicMonitor
{
public:
    // 协议监控
    static void ProtoMonitor(user_data_table_ptr_type pUserData, uint16_t usCmd, std::string strCmdName);

    //  物品监控
    static void ItemMonitor(user_data_table_ptr_type pUserData, int32_t iItemType, int32_t iItemID, int32_t iNum, uint16_t usCmd);

    // 协议开关
    static bool ProtoSwitch(int32_t iCmd);
};
