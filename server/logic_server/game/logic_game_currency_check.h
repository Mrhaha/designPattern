#pragma once

#include "logic_game_cache_table.h"

class CLogicCurrencyCheck
{
public:
    static bool CheckEnoughRoleLevel(const user_data_table_ptr_type pUser, int iRequireNum);
    static bool CheckEnoughGold(const user_data_table_ptr_type pUser, int iRequireNum);
    static bool CheckEnoughPhyPower(const user_data_table_ptr_type pUser, int iRequireNum);
    static bool CheckEnoughVitality(const user_data_table_ptr_type pUser, int iRequireNum);
	static bool CheckEnoughHeroHeartLevel(const user_data_table_ptr_type pUser, int iCardID, int iRequireNum);
};
