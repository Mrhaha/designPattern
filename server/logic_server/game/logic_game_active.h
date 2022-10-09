#pragma once

#include "logic_game_cache_table.h"
#include "logic_config_active.h"

class CLogicActive
{
public:
    static bool IsAvyOpen(const int32_t iAvyID, const int32_t iCreateTime);
    static bool IsAvyOpen(const TLogicAvyOpeningElem& stConfig, const int32_t iCreateTime);

	static bool IsActivityNormalTimeOpen(user_data_table_ptr_type pUserData, int32_t iActivityType, int32_t iNowTime);

	static bool IsWithinActivityTime(user_data_table_ptr_type pUserData, int32_t iActivityType, int32_t iNowTime, int32_t iActivityTimeType, int32_t iActivityStartDay,
	        int32_t iActivityEndDay, int32_t iActivityServerDay, int32_t iActivityStartTime, int32_t iActivityEndTime, int32_t iCustomCreateTime = 0, int32_t iActivityCreateDay = 0);

	static bool IsWithinRangeDay(user_data_table_ptr_type pUserData, int32_t iTimeType, int32_t iStartDay, int32_t iEndDay, int32_t iCustomCreateTime);

	static bool IsWithinRangeDay(user_data_table_ptr_type pUserData, int32_t iNowTime, int32_t iTimeType, int32_t iStartDay, int32_t iEndDay, int32_t iCustomCreateTime);

    static bool IsWithinRangeTime(int32_t iNowTime, int32_t iStartTime, int32_t iEndTime);

    //限时神将
    static int32_t GetOpenCountdownHeroActivityID();

    //多重活动任务(皇叔悬赏)
    static const TLogicActivityMultiTaskElem* GetOpenMultiTaskConfig(user_data_table_ptr_type pUserInfo);

    static const TLogicActivityTaskElem* GetOpenMultiTaskSingleConfig(user_data_table_ptr_type pUserInfo,
                                                                             int32_t iTaskID);

    static bool IsActivityMultiTaskOpen(user_data_table_ptr_type pUserInfo, int32_t iActivityID);

    //通用活动任务
    static const TLogicActivityTaskElem* GetOpenActivityTaskConfig(user_data_table_ptr_type pUserInfo, int32_t iActivityType, int32_t iActivityID, int32_t iTaskID);
    //通用活动任务
    static const TLogicActivitySequenceTask* GetOpenActivitySequenceTaskConfig(user_data_table_ptr_type pUserInfo, int32_t iActivityType, int32_t iActivityID, int32_t iTaskID);

    //合服活动
    static const TLogicMergeActivityElem* GetOpenMergeActivityConfig(int32_t iNowTime);

    static bool IsMergeActivityMatchPvpMailAwardDouble();

    static bool IsMergeActivityAttackDefenseMailAwardDouble();

    static bool IsMergeActivityFairPvpMailAwardDouble();

    //单笔充值
    static bool IsActivityDaySingleRechargeOpen(user_data_table_ptr_type pUserInfo, int32_t iActivityID);

    //主题活动[充值]
    static void InitNewThemeActivityCharge(user_data_table_ptr_type pUserInfo);

    //主公很忙
    static void InitNewIBusyActivity(user_data_table_ptr_type pUserInfo);

    static void UpdateIBusyMarketData(user_data_table_ptr_type pUserInfo);

    static void RefreshIBusyMarketData(user_data_table_ptr_type pUserInfo);

    static void RefreshIBusyTaskData(user_data_table_ptr_type pUserInfo);

    //主题活动[积分]
    static int32_t GetScoreLevelOpenActivityID();

    static void InitNewScoreLevelActivity(user_data_table_ptr_type pUserInfo);

    static bool AutoAddScoreLevelXingdongli(user_data_table_ptr_type pUserInfo);

    //奇遇
    static bool UpdateUserQiyu(user_data_table_ptr_type pUserInfo);

    static bool ClearQiyuMap(user_data_table_ptr_type pUserInfo, bool dayQiyuOrCangbaotu);

    static bool InitNewQiyuMap(user_data_table_ptr_type pUserInfo, bool dayQiyuOrCangbaotu, int mapID);

    static bool RefreshQiyuMap(user_data_table_ptr_type pUserInfo, bool dayQiyuOrCangbaotu, int mapID);

    //月通行证
    static bool IsMonthPassOpen(user_data_table_ptr_type pUserInfo);

    static bool IsNewPassOpen(user_data_table_ptr_type pUserInfo);

    //限时boss
    static bool IsLimitBossLevelOpen(user_data_table_ptr_type pUserInfo);

    //扭蛋机活动
    static bool IsGashaponOpen(user_data_table_ptr_type pUserInfo);
    static bool IsGashaponChapterOpen(user_data_table_ptr_type pUserInfo);

};

