#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "logic_link_common.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_active.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_global.h"

bool CLogicDailyTaskSystem::NotifyTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
    if (nullptr == pUserData) return (true);

	NotifyActivityTask(pUserData, iTaskType, iTargetID, iNum);

	NotifyActivityMultiTask(pUserData, iTaskType, iTargetID, iNum);

	NotifyMergeActivityTask(pUserData, iTaskType, iTargetID, iNum);

	NotifyHonorTaskProgress(pUserData, false, iTaskType, iTargetID, iNum);

	NotifyThemeActivityChargeTask(pUserData, iTaskType, iTargetID, iNum);

	NotifyIBusyTask(pUserData, iTaskType, iTargetID, iNum);

	NotifyMonthPassTask(pUserData, iTaskType, iTargetID, iNum);

	return true;
}

bool CLogicDailyTaskSystem::SetProgress(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
	if (nullptr == pUserData) return (true);

//	SetDailyTask(pUserData, iTaskType, iTargetID, iNum);

	SetActivityTask(pUserData, iTaskType, iTargetID, iNum);

//	SetActivityRevelyTask(pUserData, iTaskType, iTargetID, iNum);

//	SetActivityMultiTask(pUserData, iTaskType, iTargetID, iNum);

	SetMergeActivityTask(pUserData, iTaskType, iTargetID, iNum);

	NotifyHonorTaskProgress(pUserData, true, iTaskType, iTargetID, iNum);

	SetThemeActivityChargeTask(pUserData, iTaskType, iTargetID, iNum);

    SetMonthPassTask(pUserData, iTaskType, iTargetID, iNum);

	return true;
}

bool CLogicDailyTaskSystem::NotifyActivityTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
	const auto rstTaskMap = CLogicConfig::GetActivityTaskConfig().m_stActiveTaskMap;
	for (const auto& activity : rstTaskMap)
    {
        if (CLogicActive::IsWithinActivityTime(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_NONE, CLogicCommon::GetSec(),
                                               activity.second.m_iTimeType, activity.second.m_iStartDay, activity.second.m_iEndDay,
                                               activity.second.m_iServerDay, activity.second.m_iStartTime, activity.second.m_iEndTime))
        {
            for (auto iterTask = activity.second.m_stTaskMultiMap.lower_bound(iTaskType); iterTask != activity.second.m_stTaskMultiMap.upper_bound(iTaskType); ++iterTask)
            {
                if (iterTask->second.m_iTarget == 0 || iTargetID == 0 || iterTask->second.m_iTarget == iTargetID)
                {
                    AddActivityTaskProgress(pUserData, activity.first, activity.second.m_iActivityID, iterTask->second.m_iTaskID, iNum, iterTask->second);
                }
            }
        }
    }

	return true;
}

bool CLogicDailyTaskSystem::SetThemeActivityChargeTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
	for (auto &iter : pUserData->m_stThemeActivityChargeMap)
	{
		for (auto taskIter = iter.second.Begin(); taskIter != iter.second.End(); ++taskIter)
		{
			if ((taskIter->second->GetTargetType() == iTaskType) && (taskIter->second->GetTargetID() == iTargetID) && (taskIter->second->GetState() == charge_task_cannot_rewad))
			{
				int progress = taskIter->second->GetProgress();
				if (iNum > progress)
				{
					progress = iNum;
					if (progress >= taskIter->second->GetTargetAmount())
					{
						progress = taskIter->second->GetTargetAmount();

						taskIter->second->SetProgress(progress);
						taskIter->second->SetState(charge_task_can_rewad);

						//只在有任务变成可领取时，才主动通知任务变化，方便前端红点
						CPackChargeActivityTask notice;
						notice.m_iID = taskIter->first;
						notice.m_iTaskType = taskIter->second->GetTaskType();
						notice.m_iTarget = taskIter->second->GetTargetAmount();
						notice.m_iProgress = taskIter->second->GetProgress();
						notice.m_cState = taskIter->second->GetState();
						CLogicUserNotifySystem::NotifyThemeActivityTaskChange(pUserData, iter.first, notice);
					}
					else
					{
						taskIter->second->SetProgress(progress);
					}
				}
			}
		}
	}

	return true;
}

bool CLogicDailyTaskSystem::NotifyThemeActivityChargeTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
	for (auto &iter : pUserData->m_stThemeActivityChargeMap)
	{
		for (auto taskIter = iter.second.Begin(); taskIter != iter.second.End(); ++taskIter)
		{
			if ((taskIter->second->GetTargetType() == iTaskType) && (taskIter->second->GetTargetID() == iTargetID) && (taskIter->second->GetState() == charge_task_cannot_rewad))
			{
				int progress = taskIter->second->GetProgress();
				progress += iNum;
				if (progress >= taskIter->second->GetTargetAmount())
				{
					progress = taskIter->second->GetTargetAmount();

					taskIter->second->SetProgress(progress);
					taskIter->second->SetState(charge_task_can_rewad);

					//只在有任务变成可领取时，才主动通知任务变化，方便前端红点
					CPackChargeActivityTask notice;
					notice.m_iID = taskIter->first;
					notice.m_iTaskType = taskIter->second->GetTaskType();
					notice.m_iTarget = taskIter->second->GetTargetAmount();
					notice.m_iProgress = taskIter->second->GetProgress();
					notice.m_cState = taskIter->second->GetState();
					CLogicUserNotifySystem::NotifyThemeActivityTaskChange(pUserData, iter.first, notice);
				}
				else
				{
					taskIter->second->SetProgress(progress);
				}
			}
		}
	}

	return true;
}

bool CLogicDailyTaskSystem::NotifyIBusyTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
	int activityID = pUserData->m_stIBusyData.GetActivityID();
	if (activityID == 0)
		return true;

	auto activityIter = CLogicConfig::GetIBusyConfig().m_mpActivities.find(activityID);
	if (activityIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
		return true;

	auto taskIter = activityIter->second.m_mpTaskHuoyue.find(std::make_tuple(iTaskType, iTargetID));
	if (taskIter != activityIter->second.m_mpTaskHuoyue.end())
	{
		int huoyue = pUserData->m_stIBusyData.GetTodayHuoyue();
		int addHuoyue = taskIter->second * iNum;
		if (huoyue + addHuoyue > activityIter->second.m_iDayMaxHuoyue)
		{
			addHuoyue = activityIter->second.m_iDayMaxHuoyue - huoyue;
		}

		if (addHuoyue > 0)
		{
			pUserData->m_stIBusyData.AddTodayHuoyue(addHuoyue);
			// pUserData->m_stIBusyData.AddHuoyue(addHuoyue);
			CLogicUserNotifySystem::NotifyIBusyBaseData(pUserData);
		}
	}

	return true;
}

bool CLogicDailyTaskSystem::SetMonthPassTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
    if (CLogicActive::IsMonthPassOpen(pUserData))
    {
        int32_t iActivityDay = pUserData->m_stIBusyData.GetMonthPassDay();
        int32_t iActivityWeek = (iActivityDay - 1) / 7 + 1;
        std::vector<std::pair<int32_t, int32_t>> iTaskIDVec;
        const auto& config = CLogicConfig::GetMonthPassConfig();
        for (auto iter = config.m_stDailyTask.lower_bound(iActivityDay); iter != config.m_stDailyTask.upper_bound(iActivityDay); ++iter)
        {
            iTaskIDVec.emplace_back(iter->second, CLogicConfigDefine::MONTH_PASS_TYPE_DAILY_TASK);
        }
        for (auto iter = config.m_stWeeklyTask.lower_bound(iActivityWeek); iter != config.m_stWeeklyTask.upper_bound(iActivityWeek); ++iter)
        {
            iTaskIDVec.emplace_back(iter->second, CLogicConfigDefine::MONTH_PASS_TYPE_WEEKLY_TASK);
        }
        for (const auto& task : iTaskIDVec)
        {
            const auto* pstTaskConfig = config.GetTaskConfig(task.first);
            if (pstTaskConfig && pstTaskConfig->m_iType == iTaskType)
            {
                if (pstTaskConfig->m_iTarget == 0 || iTargetID == 0 || pstTaskConfig->m_iTarget == iTargetID)
                {
                    auto iterTask = pUserData->m_stMonthPassTaskMap[task.second].Find(task.first);
                    if (iterTask == pUserData->m_stMonthPassTaskMap[task.second].End())
                    {
                        TMonthPassTaskTableValueType stVal;
                        stVal.m_iUid = pUserData->m_iUin;
                        stVal.m_iGroupID = pUserData->m_iGroupID;
                        stVal.m_iTaskType = task.second;
                        stVal.m_iTaskID = task.first;
                        stVal.m_iProgress = 0;
                        stVal.m_cState = 0;

                        const auto stRet = pUserData->m_stMonthPassTaskMap[stVal.m_iTaskType].Insert(std::make_pair(stVal.m_iTaskID, stVal));
                        iterTask = stRet.first;
                    }
                    iterTask->second->SetProgress(iNum);
                    CLogicUserNotifySystem::NotifyMonthPassTask(pUserData, iterTask->first, iterTask->second->GetProgress(), iterTask->second->GetState());
                }
            }
        }
    }
    return true;
}

bool CLogicDailyTaskSystem::NotifyMonthPassTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
    if (CLogicActive::IsMonthPassOpen(pUserData))
    {
        int32_t iActivityDay = pUserData->m_stIBusyData.GetMonthPassDay();
        int32_t iActivityWeek = (iActivityDay - 1) / 7 + 1;
        std::vector<std::pair<int32_t, int32_t>> iTaskIDVec;
        const auto& config = CLogicConfig::GetMonthPassConfig();
        for (auto iter = config.m_stDailyTask.lower_bound(iActivityDay); iter != config.m_stDailyTask.upper_bound(iActivityDay); ++iter)
        {
            iTaskIDVec.emplace_back(iter->second, CLogicConfigDefine::MONTH_PASS_TYPE_DAILY_TASK);
        }
        for (auto iter = config.m_stWeeklyTask.lower_bound(iActivityWeek); iter != config.m_stWeeklyTask.upper_bound(iActivityWeek); ++iter)
        {
            iTaskIDVec.emplace_back(iter->second, CLogicConfigDefine::MONTH_PASS_TYPE_WEEKLY_TASK);
        }
        for (const auto& task : iTaskIDVec)
        {
            const auto* pstTaskConfig = config.GetTaskConfig(task.first);
            if (pstTaskConfig && pstTaskConfig->m_iType == iTaskType)
            {
                if (pstTaskConfig->m_iTarget == 0 || iTargetID == 0 || pstTaskConfig->m_iTarget == iTargetID)
                {
                    auto iterTask = pUserData->m_stMonthPassTaskMap[task.second].Find(task.first);
                    if (iterTask == pUserData->m_stMonthPassTaskMap[task.second].End())
                    {
                        TMonthPassTaskTableValueType stVal;
                        stVal.m_iUid = pUserData->m_iUin;
                        stVal.m_iGroupID = pUserData->m_iGroupID;
                        stVal.m_iTaskType = task.second;
                        stVal.m_iTaskID = task.first;
                        stVal.m_iProgress = 0;
                        stVal.m_cState = 0;

                        const auto stRet = pUserData->m_stMonthPassTaskMap[stVal.m_iTaskType].Insert(std::make_pair(stVal.m_iTaskID, stVal));
                        iterTask = stRet.first;
                    }
                    iterTask->second->AddProgress(iNum);
                    CLogicUserNotifySystem::NotifyMonthPassTask(pUserData, iterTask->first, iterTask->second->GetProgress(), iterTask->second->GetState());
                }
            }
        }
    }
    return true;
}

bool CLogicDailyTaskSystem::NotifyMergeActivityTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
	const auto* pstConfig = CLogicActive::GetOpenMergeActivityConfig(CLogicCommon::GetSec());
	if (nullptr != pstConfig)
	{
		for (const auto& task : pstConfig->m_stTaskMap)
		{
			if (task.second.m_iType == iTaskType)
			{
				if (task.second.m_iTarget == 0 || iTargetID == 0 || task.second.m_iTarget == iTargetID)
				{
					auto iterTask = pUserData->m_stMergeActivityMap.Find(task.first);
					if (iterTask == pUserData->m_stMergeActivityMap.End())
					{
						TMergeActivityTableValueType stVal;
						stVal.m_iUid = pUserData->m_iUin;
						stVal.m_iGroupID = pUserData->m_iGroupID;
						stVal.m_iTaskID = task.first;
						stVal.m_iProgress = 0;
						stVal.m_cAward = 0;

						const auto stRet = pUserData->m_stMergeActivityMap.Insert(std::make_pair(stVal.m_iTaskID, stVal));
						if (stRet.second == false)
						{
							return false;
						}
						iterTask = stRet.first;
					}
					iterTask->second->AddProgress(iNum);
					CLogicUserNotifySystem::NotifyMergeActivityChange(pUserData, iterTask->first, iterTask->second->GetProgress(), iterTask->second->GetAward());
				}
			}
		}
	}
	return true;
}

bool CLogicDailyTaskSystem::SetMergeActivityTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
	const auto* pstConfig = CLogicActive::GetOpenMergeActivityConfig(CLogicCommon::GetSec());
	if (nullptr != pstConfig)
	{
		for (const auto& task : pstConfig->m_stTaskMap)
		{
			if (task.second.m_iType == iTaskType)
			{
				if (task.second.m_iTarget == 0 || iTargetID == 0 || task.second.m_iTarget == iTargetID)
				{
					auto iterTask = pUserData->m_stMergeActivityMap.Find(task.first);
					if (iterTask == pUserData->m_stMergeActivityMap.End())
					{
						TMergeActivityTableValueType stVal;
						stVal.m_iUid = pUserData->m_iUin;
						stVal.m_iGroupID = pUserData->m_iGroupID;
						stVal.m_iTaskID = task.first;
						stVal.m_iProgress = 0;
						stVal.m_cAward = 0;

						const auto stRet = pUserData->m_stMergeActivityMap.Insert(std::make_pair(stVal.m_iTaskID, stVal));
						if (stRet.second == false)
						{
							return false;
						}
						iterTask = stRet.first;
					}

					if (iterTask->second->GetProgress() != iNum)
					{
						iterTask->second->SetProgress(iNum);
						CLogicUserNotifySystem::NotifyMergeActivityChange(pUserData, iterTask->first, iterTask->second->GetProgress(), iterTask->second->GetAward());
					}					
				}
			}
		}
	}
	return true;
}

bool CLogicDailyTaskSystem::NotifyActivityMultiTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
	const auto* pstConfig = CLogicActive::GetOpenMultiTaskConfig(pUserData);
	if (nullptr == pstConfig)
	{
		return false;
	}

	for (auto iter = pstConfig->m_stTaskMultiMap.lower_bound(iTaskType); iter != pstConfig->m_stTaskMultiMap.upper_bound(iTaskType); ++iter)
	{
		if (iter->second.m_iTarget == 0 || iTargetID == 0 || iter->second.m_iTarget == iTargetID)
		{
			AddActivityTaskProgress(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_MULTI_TASK, pstConfig->m_iActivityID, iter->second.m_iTaskID, iNum, iter->second);
		}
	}
	return true;
}

bool CLogicDailyTaskSystem::SetActivityTask(user_data_table_ptr_type pUserData, int iTaskType, int iTargetID, int iNum)
{
	const auto rstTaskMap = CLogicConfig::GetActivityTaskConfig().m_stActiveTaskMap;
	for (const auto& activity : rstTaskMap)
	{
		if (CLogicActive::IsWithinActivityTime(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_NONE, CLogicCommon::GetSec()
			, activity.second.m_iTimeType, activity.second.m_iStartDay, activity.second.m_iEndDay
			, activity.second.m_iServerDay, activity.second.m_iStartTime, activity.second.m_iEndTime))
		{
			for (auto iterTask = activity.second.m_stTaskMultiMap.lower_bound(iTaskType); iterTask != activity.second.m_stTaskMultiMap.upper_bound(iTaskType); ++iterTask)
			{
				if (iterTask->second.m_iTarget == 0 || iTargetID == 0 || iterTask->second.m_iTarget == iTargetID)
				{
					const auto tupleKey = std::make_tuple(activity.first, activity.second.m_iActivityID, iterTask->second.m_iTaskID);
					int32_t iCurProgress = 0;
					const auto myTask = pUserData->m_stActivityTaskMap.Find(tupleKey);
					if (myTask != pUserData->m_stActivityTaskMap.End())
					{
						iCurProgress = myTask->second->GetProgress();
					}

					if (iNum > iCurProgress)
					{
						AddActivityTaskProgress(pUserData, activity.first, activity.second.m_iActivityID, iterTask->second.m_iTaskID, iNum - iCurProgress, iterTask->second);
					}
				}
			}
		}
	}
	return true;
}

bool CLogicDailyTaskSystem::NotifyHonorTaskProgress(user_data_table_ptr_type pUserData, bool bIsSetProgress, int iTaskType, int iTargetID, int iNum)
{
	std::vector<std::function<int32_t(TUserHonorTable*)>> get_progress_func_vec = {
		std::bind(&TUserHonorTable::GetTaskProgress1, std::placeholders::_1),
		std::bind(&TUserHonorTable::GetTaskProgress2, std::placeholders::_1),
		std::bind(&TUserHonorTable::GetTaskProgress3, std::placeholders::_1),
		std::bind(&TUserHonorTable::GetTaskProgress4, std::placeholders::_1),
		std::bind(&TUserHonorTable::GetTaskProgress5, std::placeholders::_1)
	};

	std::vector<std::function<void(TUserHonorTable*, int32_t)>> add_progress_func_vec = {
		std::bind(&TUserHonorTable::AddTaskProgress1, std::placeholders::_1, std::placeholders::_2),
		std::bind(&TUserHonorTable::AddTaskProgress2, std::placeholders::_1, std::placeholders::_2),
		std::bind(&TUserHonorTable::AddTaskProgress3, std::placeholders::_1, std::placeholders::_2),
		std::bind(&TUserHonorTable::AddTaskProgress4, std::placeholders::_1, std::placeholders::_2),
		std::bind(&TUserHonorTable::AddTaskProgress5, std::placeholders::_1, std::placeholders::_2)
	};

	const auto& config = CLogicConfig::GetHonorConfig();
	for (auto iter = config.m_stTaskType2HonorIDMap.lower_bound(iTaskType); iter != config.m_stTaskType2HonorIDMap.upper_bound(iTaskType); ++iter)
	{
		const auto* pstConfig = config.GetHonorConfig(iter->second);
		if (nullptr != pstConfig && pstConfig->m_stTaskVec.size() <= get_progress_func_vec.size())
		{
			auto iterHonor = pUserData->m_stUserHonorMap.Find(iter->second);
			if (iterHonor == pUserData->m_stUserHonorMap.End())
			{
				TUserHonorTableValueType value;
				value.m_iUid = pUserData->m_iUin;
				value.m_iGroupID = pUserData->m_iGroupID;
				value.m_iHonorID = iter->second;
				const auto ret = pUserData->m_stUserHonorMap.Insert(std::make_pair(value.m_iHonorID, value));
				if (!ret.second)
				{
					continue;
				}
				iterHonor = ret.first;
			}

			if (iterHonor->second->GetHonorStatus() != CLogicConfigDefine::HONOR_STATUS_NONE)
			{
				continue;
			}

			bool bUpdate = false;
			for (size_t index = 0 ; index < pstConfig->m_stTaskVec.size(); ++index)
			{
				if (pstConfig->m_stTaskVec[index].m_iType == iTaskType)
				{
					if (pstConfig->m_stTaskVec[index].m_iTarget == 0 || iTargetID == 0 || pstConfig->m_stTaskVec[index].m_iTarget == iTargetID)
					{
						const int32_t iAddNum = bIsSetProgress ? std::max(0, iNum - get_progress_func_vec[index](iterHonor->second)) : iNum;
						if (0 != iAddNum)
						{
							add_progress_func_vec[index](iterHonor->second, iAddNum);
							bUpdate = true;
						}
					}
				}
			}

			if (bUpdate)
			{
				CLogicUserNotifySystem::NotifyUpdateHonor(pUserData, iterHonor->second->m_stData);
			}
		}
	}
	return true;
}

void CLogicDailyTaskSystem::AddActivityTaskProgress(user_data_table_ptr_type pUserData, int32_t iActivityType, int32_t iActivityID, int32_t iTaskID, int32_t iAddNum, const TLogicActivityTaskElem& stTaskElem)
{
	const auto tupleKey = std::make_tuple(iActivityType, iActivityID, iTaskID);

	auto iterTask = pUserData->m_stActivityTaskMap.Find(tupleKey);
	if (iterTask == pUserData->m_stActivityTaskMap.End())
	{
		TActivityTaskTableValueType stVal;
		stVal.m_iUid = pUserData->m_iUin;
		stVal.m_iGroupID = pUserData->m_iGroupID;
		stVal.m_iActivityType = iActivityType;
		stVal.m_iActivityID = iActivityID;
		stVal.m_iTaskID = iTaskID;
		stVal.m_iProgress = 0;
		stVal.m_cState = TASK_UNDER_WAY;

		auto stRet = pUserData->m_stActivityTaskMap.Insert(std::make_pair(tupleKey, stVal));
		if (stRet.second == false)
		{
			using namespace SERVER_ERRCODE;
			LOGIC_LOG_TRACE_ERROR << "ERROR:" << INTERN_ERROR << "|MSG:" << GET_MACRO_STRING(INTERN_ERROR)
				<< "|ACTIVITY_TYPE:" << iActivityType << "|ACTIVITY_ID:" << iActivityID
				<< "|TASK_ID:" << iTaskID << "|INSERT_INTO_CACHE_FAILD " << std::endl;
			return;
		}
		iterTask = stRet.first;
	}

	if (iterTask->second->GetState() == TASK_HAS_ACHIEVE_GIFT)
		return;
	if (iterTask->second->GetProgress() >= stTaskElem.m_iTargetAmount)
		return;

	iterTask->second->AddProgress(iAddNum);

	CLogicUserNotifySystem::NotifyUpdateActivityTask(pUserData, iActivityType, iActivityID, iTaskID, iterTask->second->GetProgress(), iterTask->second->GetState());
}

////////////////////////////////////////////////////////////////////////
void
CLogicDailyTaskSystem::SubscribeGameLevelInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_level_channel_data_t& stData)
{
    int iChapterType = CLogicConfig::GetChapterTypeByLevelID(stData.m_iLevelID);

    CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::CHAPTER_TYPE_TASK, iChapterType, stData.m_iTimes);
    CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::FIGHT_LEVEL_TASK, stData.m_iLevelID, stData.m_iTimes);

	if (iChapterType == CLogicConfigDefine::NORMAL_CHAPTER || iChapterType == CLogicConfigDefine::ELITE_CHAPTER)
	{
		CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::USER_GAME_ACTION_TASK, CLogicGameChannelEnum::ACTION_LEVEL_PASS_COUNT, stData.m_iTimes);
	}
}

void
CLogicDailyTaskSystem::SubscribeGameCardInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_card_channel_data_t& stData)
{
    if (stData.m_iCardOption == CLogicGameChannelEnum::UPGRADE_CARD_SKILL_LEVEL_OPTION)
    {
        CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::UPGRADE_SKILL_TASK, 0, 1);
    }
    else if (stData.m_iCardOption == CLogicGameChannelEnum::UPGRADE_CARD_USE_EXP_DRINK)
    {
        CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::UPGRADE_CARD_TASK, 0, 1);
    }
    else if (stData.m_iCardOption == CLogicGameChannelEnum::UPGRADE_CARD_COLOR_OPTION)
    {
        CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::UPGRADE_CARD_COLOR_TASK, 0, 1);
    }
    else if(stData.m_iCardOption == CLogicGameChannelEnum::UPGRADE_CARD_LEVEL_OPTION)
    {
        CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::UPGRADE_CARD_LEVEL, 0, 1);
    }
}

void
CLogicDailyTaskSystem::SubscribeGameMallInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_mall_channel_data_t& stData)
{
    CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::BUY_CARD_TASK, 0, stData.m_iCommodityNum);
}

void
CLogicDailyTaskSystem::SubscribeGameUserActionInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_user_action_channel_data_t& stData)
{
    CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::USER_GAME_ACTION_TASK, stData.m_iAction, stData.m_iTimes);
}

void
CLogicDailyTaskSystem::SubscribeGameHeroEquipmentCallBack(user_data_table_ptr_type pUserData, const logic_game_hero_equipment_channel_data_t& stData)
{
    if (CLogicGameChannelEnum::ACTION_UPGRADE_EQUIPMENT_LEVEL == stData.m_enmActionType)
    {
        CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::EQUIPMENT_UPGRADE_LEVEL_TASK, stData.m_enmActionType, stData.m_iNum);
    }
}

void
CLogicDailyTaskSystem::SubscribeFunctionalGameItemInfoCallBack(user_data_table_ptr_type pUserData, const logic_functional_game_item_channel_data_t& stData)
{
	if (stData.m_iDeltaNum > 0)
	{
		//卖出钱庄金券不算获得金币
		if (stData.m_iCmd != MSG_LOGIC_SELL_BANK_GOLD)
		{
			CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::GET_GAME_ITEM_TASK, stData.m_iItemType, stData.m_iDeltaNum);

			if(CLogicConfigDefine::CURRENCY == stData.m_iItemType)
            {
                CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::GET_VIRTUAL_ITEM, stData.m_iItemID, stData.m_iDeltaNum);
            }
		}
	}
	else
	{
		//买进钱庄金券不算消耗金币
		if (stData.m_iCmd != MSG_LOGIC_BUY_BANK_GOLD)
		{
			CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::CONSUME_GAME_ITEM_TASK, stData.m_iItemType, -stData.m_iDeltaNum);
		}

		if (CLogicConfigDefine::EXCHANGE_ITEM == stData.m_iItemType)
		{
			CLogicDailyTaskSystem::NotifyTask(pUserData, CLogicDailyTaskSystem::CONSUME_TARGET_ITEM_TASK, stData.m_iItemID, -stData.m_iDeltaNum);
		}
	}

	if (CLogicConfigDefine::HERO_CARD == stData.m_iItemType)
	{
		int32_t iHaveCardCount = 0;
		CLogicCacheCardOperation stCacheCardOP(pUserData);
		for (auto stIter = pUserData->m_stHeroCardTableMap.Begin(); stIter != pUserData->m_stHeroCardTableMap.End(); ++stIter)
		{
			if (stCacheCardOP.CheckHeroCardExist(stIter->second))
			{
				++iHaveCardCount;
				CLogicDailyTaskSystem::SetProgress(pUserData, CLogicDailyTaskSystem::HERO_STAR, stIter->first, stIter->second->GetCardStar());
			}
		}
		CLogicDailyTaskSystem::SetProgress(pUserData, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_CARD_COUNT, iHaveCardCount);

	}
}

void CLogicDailyTaskSystem::SubscribeAwardActivityMultiTaskCallBack(user_data_table_ptr_type pUserData, const logic_award_activity_multi_task_channel_data_t& stData)
{
	CLogicDailyTaskSystem::NotifyActivityMultiTask(pUserData, CLogicDailyTaskSystem::ACTIVITY_MULTI_TASK_AWARD, 0, stData.m_iAwardNum);
}
