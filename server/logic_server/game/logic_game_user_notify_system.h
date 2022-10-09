#pragma once

#include "server_qiyu_protocol.h"
#include "server_task_protocol.h"
#include "server_xuyuanqingdian_protocol.h"
#include "server_theme_activity_charge_protocol.h"
#include "logic_config_base.h"
#include "logic_game_cache_table.h"

class CLogicUserNotifySystem
{
public:
	static ILogicConnectorSender* GetConnectorSender();

	static void NotifyUserDataToClient(user_data_table_ptr_type pUserInfo);

	static void NotifyDayRefreshEvent(user_data_table_ptr_type pUserInfo);

	static void NotifyHasNewMail(user_data_table_ptr_type pUserInfo);

public:
	static void NotifyUserDailyData(user_data_table_ptr_type pUserInfo);

	static void NotifyUserAllAvyInfo(user_data_table_ptr_type pUserInfo, const std::vector<int32_t>& stIDList);

	static void NotifyUpdateActivityTask(user_data_table_ptr_type pUserInfo, int32_t iActivityType, int32_t iActivityID, int32_t iTaskID, int32_t iProgress, int32_t iState);

	static void NotifyUserLoginTalk(user_data_table_ptr_type pUserInfo);

	static void NotifyUserBuyBackInfo(user_data_table_ptr_type pUserInfo);

	static void NotifyUpdateHonor(user_data_table_ptr_type pUserInfo, const TUserHonorTableValueType& stVal);

	static void NotifyCandleAdd(user_data_table_ptr_type pUserInfo);

	static void NotifyServerConfig(user_data_table_ptr_type pUserInfo);

	static void NotifyShopCount(user_data_table_ptr_type pUserInfo);

	static void NotifyMergeActivity(user_data_table_ptr_type pUserInfo);

	static void NotifyMergeActivityChange(user_data_table_ptr_type pUserInfo, int32_t iTaskID, int32_t iProgress, int8_t byAward);

	static void NotifyFairPvpInfo(user_data_table_ptr_type pUserInfo);

	static void NotifyMatchPvpInfo(user_data_table_ptr_type pUserInfo);

	static void NotifyUserOtherData(user_data_table_ptr_type pUserInfo);

    static void NotifyUserNewHeroHistory(user_data_table_ptr_type pUserInfo, int32_t iCardID, int32_t iIndex, int32_t iUnlockTime, int32_t iContentIndex);

    static void NotifyUserItemAttr(user_data_table_ptr_type pUserInfo, int32_t iItemID, int32_t iNextTime, int32_t iOpenedCount);

	static void NotifyQiyuData(user_data_table_ptr_type pUserInfo);
	static void GenQiyuData(user_data_table_ptr_type pUserInfo, int qiyuID, const user_data_table_s::qiyu_container_type& qiyuData, CPackQiyuMapData &mapData);
	static void NotifyQiyuEvent(user_data_table_ptr_type pUserInfo, CNotifyQiyuEvent &notice);
	static void NotifyCangbaotuSuipianData(user_data_table_ptr_type pUserInfo, CNotifyCangbaotuSuipian &notice);

	static void NotifyThemeActivityChargeData(user_data_table_ptr_type pUserInfo, int activityID);
	static void NotifyThemeActivityTaskChange(user_data_table_ptr_type pUserInfo, int activityID, const CPackChargeActivityTask& task);

	static void NotifyIBusyData(user_data_table_ptr_type pUserInfo);
	static void NotifyIBusyBaseData(user_data_table_ptr_type pUserInfo);
	static void NotifyIBusyTaskData(user_data_table_ptr_type pUserInfo);
	static void NotifyIBusyMarketData(user_data_table_ptr_type pUserInfo);
	static void NotifyIBusyLevelRewardData(user_data_table_ptr_type pUserInfo);

	static void NotifyScoreLevelActivityData(user_data_table_ptr_type pUserInfo);

	static void NotifyMonthPass(user_data_table_ptr_type pUserInfo);

	static void NotifyMonthPassTask(user_data_table_ptr_type pUserInfo, int32_t iTaskID, int32_t iProgress, int32_t iState);

	static void NotifyDaySingleRechargeActivityInfo(user_data_table_ptr_type pUserInfo);
	static void NotifyDaySingleRechargeTaskChange(user_data_table_ptr_type pUserInfo, int32_t iActivityID, int32_t iTaskID, int32_t iRewardCount, int32_t iCanRewardCount);
    static void NotifyUserBuffData(user_data_table_ptr_type pUserInfo);

    static void NotifyQuestionnaire(user_data_table_ptr_type pUserInfo);

    static void NotifyItemChange(user_data_table_ptr_type pUserInfo, const std::vector<CPackGameItem>& stGets, const std::vector<CPackGameItem>& stConsumes = {}, bool bNotify = false);

    static void NotifyQuestData(user_data_table_ptr_type pUserInfo, const std::set<int32_t>& stQuestIDSet);
    static void NotifyBadgeData(user_data_table_ptr_type pUserInfo, const std::set<int32_t>& stBadgeIDSet);
    static void NotifyUserPhyData(user_data_table_ptr_type pUserInfo);
    static void NotifyUserExploreQuest(user_data_table_ptr_type pUserInfo, const TUserHomeExploreTableValueType& stValue);

    static void RefreshDataChange(user_data_table_ptr_type pUserInfo);

private:
	static int32_t GetUserBuyBackLeftBasicCount(user_data_table_ptr_type pUserInfo, CEnumBuyBackType type);
};
