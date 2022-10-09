//
// Created by lxhbroser on 2020/4/1.
//

#pragma once

#include "logic_game_cache_table.h"
#include "logic_config_base.h"
#include "server_fight_protocol.h"
#include "logic_config.h"

class CLogicBuff
{
public:
    static int32_t AddBuff(user_data_table_ptr_type pUserData, int iBuffID);
    static bool IsHaveMonthCard(user_data_table_ptr_type pUserData);
    static bool IsHaveFatMonthCard(user_data_table_ptr_type pUserData);

    static int32_t GetBuffValue(user_data_table_ptr_type pUserData, int iBuffType, int iPara);

    static void UpdateBuff(user_data_table_ptr_type pUserData);

    static bool HasTargetBuff(user_data_table_ptr_type pUserData,int32_t buffType,int32_t buffParam=0);
    static void ReduceBuffTimes(user_data_table_ptr_type pUserData,int32_t buffType,int32_t buffParam=0);
};

class CLogicBadgeManage
{
public:
    static void UpdateBadgeCond(user_data_table_ptr_type pUserData, int iType, int iPara, const CPackLevelFightPara& stParam);

private:
    static bool CheckLevelFightCond(user_data_table_ptr_type pUserData, TUserBadgeTable* pTable, const TLogicBadgeCondElem& stConfig, int iLevelID, const CPackLevelFightPara& stParam);
};
