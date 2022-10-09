//
// Created by luoxiaohu on 2020/10/10.
//

#pragma once

#include "logic_game_cache_table.h"

class CLogicVersionDataUpdateSystem
{
public:
    enum EnumVersion
    {
        EV_Invalid = 0,             //
        EV_CardGradeMod = 1,        // 伙伴品阶修改
        // TODO
        EV_AddNewMusic = 2,         // 添加新的CD音乐

        EV_Max,                     // 最大版本值标记位
        EV_CurVersion = EV_Max - 1, // 当前版本值，总是取最大有效版本值
    };

public:
    static void CheckVersionDataUpdate(user_data_table_ptr_type pUserInfo);

private:
    static void UpdateCardGradeData(user_data_table_ptr_type pUserInfo);
};
