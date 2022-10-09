#pragma once

#include "logic_game_cache_table.h"
#include "logic_game_publisher.h"

class CLogicUserStat
{
public:
	static void SubscribeGameUserLevelUpInfoCallBack(user_data_table_ptr_type pUserData);

	static void SubscribeFunctionalGameItemInfoCallBack(user_data_table_ptr_type pUserData, const logic_functional_game_item_channel_data_t& stData);

public:
    static int GetDailyRechargeThresholdCount(user_data_table_ptr_type pUserData, int iBeginTime, int iEndTime, int iThreshold);

	static void OnRecharge(user_data_table_ptr_type pUserData, int iPayMoney);

	static bool isTodayRechargeThreshold(user_data_table_ptr_type pUserData, int iThreshold);

	static void ResetRechargeCount(user_data_table_ptr_type pUserData, int32_t iDepositID);

	static int32_t GetLeftRechargeDoubleCount(user_data_table_ptr_type pUserData, int32_t iDepositID);

	static int32_t GetLastRechargeTime(user_data_table_ptr_type pUserData, int32_t iDepositID);

	static int32_t GetLeftRechargeBuyCount(user_data_table_ptr_type pUserData, int32_t iDepositID);

	static bool HaveBuyDepositID(user_data_table_ptr_type pUserData, int32_t iDepositID);

private:
	static void OnRechargeSerialPay(user_data_table_ptr_type pUserData, int iPayMoney);

	static void OnRechargeDaySingle(user_data_table_ptr_type pUserData, int iPayMoney);

	static void OnRechargeMonthPass(user_data_table_ptr_type pUserData, int iPayMoney);

    static bool CanBuyMergeGiftDepositID(user_data_table_ptr_type pUserInfo, int32_t iDepositID);

    static bool CanBuyThemeRechargeDepositID(user_data_table_ptr_type pUserInfo, int32_t iDepositID);

    static bool CanBuyIbusyDepositID(user_data_table_ptr_type pUserInfo, int32_t iDepositID);

    static bool CanBuyScoreLevelDepositID(user_data_table_ptr_type pUserInfo, int32_t iDepositID);

    static bool CanBuySpecialGiftDepositID(user_data_table_ptr_type pUserData, int32_t iType, int32_t iDepositID);

    static bool CanBuyMonthPassDepositID(user_data_table_ptr_type pUserInfo, int32_t iDepositID);
};
