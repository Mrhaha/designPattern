//
// Created by lxhbroser on 2019/7/2.
//

#pragma once

//*********************************！！！注意！！！*******************************************
// 该枚举只作为协议开关功能的扩展功能使用，它并不是协议，它只是协议开关操作 SERVER_CMD_ADMIN_DEFINE
// 和 SERVER_CMD_LOGIC_DEFINE 之外的其他非协议功能的开关枚举(比如：协议监控功能，物品监控功能)
//******************************************************************************************
enum SERVER_CMD_EXTEND_DEFINE
{//特别注意:ID范围: [1 -- 50]
    CMD_EXTEND_START = 0,

    CMD_EXTEND_PROTO_MONITOR    = 1,        // 协议监控
    CMD_EXTEND_ITEM_MONITOR     = 2,        // 物品监控

    CMD_EXTEND_END,

};//特别注意:ID范围: [1 -- 50]
