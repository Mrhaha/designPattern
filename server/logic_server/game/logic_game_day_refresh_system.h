#pragma once

#include "logic_game_cache_table.h"
#include "logic_game_new_pass_processor.h"

class CLogicDayRefreshSystem
{
public:
	static void CheckUserDataRefresh(user_data_table_ptr_type pUserInfo);

	// 刷新特权周、月礼
    static void RefreshUserSpecialGiftInfo(user_data_table_ptr_type pUserInfo, int32_t iType);

private:
    static void RefreshUserDailyData(user_data_table_ptr_type pUserInfo);
	static void RefreshUserWeeklyData(user_data_table_ptr_type pUserInfo);
    static void RefreshUserMonthlyData(user_data_table_ptr_type pUserInfo);

private:
    static void RefreshUserShopCount(user_data_table_ptr_type pUserInfo, CLogicConfigDefine::EnumStoreLimitType eType);

    static void RefreshUserZhuHaiData(user_data_table_ptr_type pUserInfo);

	static void RefreshUserTableData(user_data_table_ptr_type pUserInfo);

    static void RefreshUserExploreQuest(user_data_table_ptr_type pUserInfo);

	static void RefreshActivityTask(user_data_table_ptr_type pUserInfo);

	static void RefreshMailActivityAward(user_data_table_ptr_type pUserInfo);

	static void RefreshActivityLoginInsure(user_data_table_ptr_type pUserInfo);

	static void RefreshActivityLoginFund(user_data_table_ptr_type pUserInfo);

	static void RefreshUserDepositTimes(user_data_table_ptr_type pUserInfo);

	static void RefreshUserBuyBackInfo(user_data_table_ptr_type pUserInfo);

	static void RefreshActivityFlowerLantern(user_data_table_ptr_type pUserInfo);

	static void RefreshFairPvpInfo(user_data_table_ptr_type pUserInfo);

	static void RefreshMatchPvpInfo(user_data_table_ptr_type pUserInfo);

    static void RefreshSendMonthCardDailyMail(user_data_table_ptr_type pUserInfo);

	static void RefreshUserQiyuInfo(user_data_table_ptr_type pUserInfo);

	static void RefreshUserThemeActivityChargeInfo(user_data_table_ptr_type pUserInfo);

	static void RefreshUserIBusyInfo(user_data_table_ptr_type pUserInfo);

	static void RefreshUserScoreLevelInfo(user_data_table_ptr_type pUserInfo);

	static void RefreshUserDaySingleRechargeInfo(user_data_table_ptr_type pUserInfo);

	static void RefreshMonthPassInfo(user_data_table_ptr_type pUserInfo);

    static void RefreshUserDailyQuest(user_data_table_ptr_type pUserInfo);

    static void RefreshUserNewPassInfo(user_data_table_ptr_type pUserInfo);

    static void RefreshUserNewPassWeeklyInfo(user_data_table_ptr_type pUserInfo);

    static void RefreshPVPWeeklyInfo(user_data_table_ptr_type pUserInfo);

    static void RefreshPVPWeeklyReward(user_data_table_ptr_type pUserInfo);

    static void RefreshLimitBoss(user_data_table_ptr_type pUserInfo);

    static void RefreshLimitCommodityPT(user_data_table_ptr_type pUserInfo);

	static void RefreshGuildReward(user_data_table_ptr_type pUserInfo);

    static void RefreshHomeInfo(user_data_table_ptr_type pUserInfo);

    static void RefreshWenWenChoice(user_data_table_ptr_type pUserInfo);

    static void RefreshRegisterAccount(user_data_table_ptr_type pUserInfo);

    static void RefreshConstelShop(user_data_table_ptr_type pUserInfo);

    static void RefreshAvyInfo(user_data_table_ptr_type pUserInfo);

private:
	static void RefreshUserMergeData(user_data_table_ptr_type pUserInfo);

	static bool MergeChangeRoleNick(user_data_table_ptr_type pUserInfo);

	static void RefreshMergeActivity(user_data_table_ptr_type pUserInfo);

private:
	static void LazyDeleteActivityInfo(user_data_table_ptr_type pUserInfo);

	static void LazyUserTaskStatus(user_data_table_ptr_type pUserInfo);

private:
	static int32_t GetCanBuyBackCount(user_data_table_ptr_type pUserInfo, CEnumBuyBackType type, int32_t iDailyCount, int32_t iPara);
};
