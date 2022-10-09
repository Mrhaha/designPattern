#include "common_datetime.h"
#include "server_errcode.h"
#include "server_item_protocol.h"
#include "server_theme_activity_charge_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_config.h"
#include "logic_game_active_info.h"
#include "logic_game_active.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_cache_common.h"
#include "logic_game_user_stat.h"

bool CLogicActive::IsAvyOpen(const int32_t iAvyID, const int32_t iCreateTime)
{
    auto pConfig = CLogicConfig::GetAvyTimeConfig().GetConfig(iAvyID);
    if(nullptr == pConfig)
        return false;

    return IsAvyOpen(pConfig->m_stAvyTime, iCreateTime);
}

bool CLogicActive::IsAvyOpen(const TLogicAvyOpeningElem& stConfig, const int32_t iCreateTime)
{
    const time_t iNowTime = CLogicCommon::GetSec();
    switch (stConfig.m_iOpeningType)
    {
    case CLogicConfigDefine::EAOT_Always:
        return true;

    case CLogicConfigDefine::EAOT_Server:
    {
        int32_t iOpenServerDay = day_diff(CLogicConfig::GetSystemConfig().GetOpenSvrTime(), iNowTime) + 1;
        return stConfig.m_iOpeningPara1 <= iOpenServerDay && iOpenServerDay <= stConfig.m_iOpeningPara2;
    }
    case CLogicConfigDefine::EAOT_Create:
    {
        int32_t iCreateDay = day_diff(iCreateTime, iNowTime) + 1;
        tm tmNowTime, tmCreateTime;
        const time_t stCreateTime = iCreateTime;
        localtime_r(&iNowTime, &tmNowTime);
        localtime_r(&stCreateTime, &tmCreateTime);
        if (tmCreateTime.tm_hour < EVERYDAY_REFRESH_HOUR && tmNowTime.tm_hour >= EVERYDAY_REFRESH_HOUR) iCreateDay += 1;
        //if (tmNowTime.tm_hour < EVERYDAY_REFRESH_HOUR && iCreateDay > 1) iCreateDay -= 1;
        return stConfig.m_iOpeningPara1 <= iCreateDay && iCreateDay <= stConfig.m_iOpeningPara2;
    }
    case CLogicConfigDefine::EAOT_CustomCreate:
    {
        int32_t iCustomCreateDay = day_diff(std::max(iCreateTime, stConfig.m_iCustomCreateTime), iNowTime) + 1;
        return stConfig.m_iOpeningPara1 <= iCustomCreateDay && iCustomCreateDay <= stConfig.m_iOpeningPara2;
    }
    case CLogicConfigDefine::EAOT_MergeServer:
    {
        int32_t iMergeServerDay = day_diff(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iMergeTime, iNowTime) + 1;
        return stConfig.m_iOpeningPara1 <= iMergeServerDay && iMergeServerDay <= stConfig.m_iOpeningPara2;
    }
    case CLogicConfigDefine::EAOT_AbsoluteDate:
    {
        return stConfig.m_iOpeningPara1 <= iNowTime && iNowTime <= stConfig.m_iOpeningPara2;
    }
    default:
        break;
    }

    return false;
}

bool CLogicActive::IsActivityNormalTimeOpen(user_data_table_ptr_type pUserData, int32_t iActivityType, int32_t iNowTime)
{
	if (iActivityType == CLogicConfigDefine::ACTIVITY_TYPE_LIMIT_DEPOSIT_GIFT)
	{
		return false;
	}
	else
	{
		const auto& activity_config = CLogicConfig::GetActivityConfig();
		for (auto iter = activity_config.m_stActivityType2IDMap.lower_bound(iActivityType); iter != activity_config.m_stActivityType2IDMap.upper_bound(iActivityType); ++iter)
		{
			const auto& iterRef = activity_config.m_stActivityRef.find(iter->second);
			if (iterRef != activity_config.m_stActivityRef.end() && iterRef->second.m_iServerDay > 0 && iterRef->second.m_iStartTime > 0 && iterRef->second.m_iEndTime)
			{
				if (IsWithinActivityTime(pUserData, iActivityType, iNowTime
						, iterRef->second.m_iTimeType, iterRef->second.m_iStartDay, iterRef->second.m_iEndDay
						, iterRef->second.m_iServerDay, iterRef->second.m_iStartTime, iterRef->second.m_iEndTime))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CLogicActive::IsWithinActivityTime(user_data_table_ptr_type pUserData, int32_t iActivityType, int32_t iNowTime
		, int32_t iActivityTimeType, int32_t iActivityStartDay, int32_t iActivityEndDay
		, int32_t iActivityServerDay, int32_t iActivityStartTime, int32_t iActivityEndTime
		, int32_t iCustomCreateTime/* = 0*/, int32_t iActivityCreateDay/* = 0*/)
{
    //是相对时间的活动
    if (iActivityStartDay != 0 && iActivityEndDay != 0)
    {
        if (iActivityTimeType == CLogicConfigDefine::TIME_TYPE_NORMAL_SERVER)
        {
            return iActivityType != CLogicConfigDefine::ACTIVITY_TYPE_NONE
                   && IsWithinRangeDay(pUserData, iNowTime, CLogicConfigDefine::TIME_TYPE_SERVER, iActivityStartDay, iActivityEndDay, iCustomCreateTime)
                   && !IsActivityNormalTimeOpen(pUserData, iActivityType, iNowTime);
        }

        return IsWithinRangeDay(pUserData, iNowTime, iActivityTimeType, iActivityStartDay, iActivityEndDay, iCustomCreateTime);
    }

    //绝对时间
    if (iActivityStartTime != 0 && iActivityEndTime != 0)
    {
        const int32_t iNormalServerDay = day_diff(CLogicConfig::GetSystemConfig().GetOpenSvrTime(), iActivityStartTime) + 1;
        if (iActivityServerDay > 0 && iNormalServerDay < iActivityServerDay)
        {
            return false;
        }

        const int32_t iNormalCreateDay = nullptr == pUserData ? -1 : day_diff(pUserData->m_stUserInfoTable.GetCreateTime(), iActivityStartTime) + 1;
        if(iActivityCreateDay > 0 && iNormalCreateDay < iActivityCreateDay)
        {
            return false;
        }

        return IsWithinRangeTime(iNowTime, iActivityStartTime, iActivityEndTime);
    }

    //没有配时间， 默认开启
    return true;
}


bool CLogicActive::IsWithinRangeDay(user_data_table_ptr_type pUserData, int32_t iTimeType, int32_t iStartDay, int32_t iEndDay, int32_t iCustomCreateTime)
{
	return IsWithinRangeDay(pUserData, CLogicCommon::GetSec(), iTimeType, iStartDay, iEndDay, iCustomCreateTime);
}

bool CLogicActive::IsWithinRangeDay(user_data_table_ptr_type pUserData, int32_t iNowTime, int32_t iTimeType, int32_t iStartDay, int32_t iEndDay, int32_t iCustomCreateTime)
{
	const int32_t iOpenServerDay = day_diff(CLogicConfig::GetSystemConfig().GetOpenSvrTime(), iNowTime) + 1;
	const int32_t iCreateDay = nullptr == pUserData ? -1 : day_diff(pUserData->m_stUserInfoTable.GetCreateTime(), iNowTime) + 1;
	const int32_t iCustomCreateDay = std::min(iCreateDay, day_diff(iCustomCreateTime, iNowTime) + 1);
	const int32_t iMergeServerDay = day_diff(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iMergeTime, iNowTime) + 1;
	int32_t iDiffDay = 0;
	if (CLogicConfigDefine::TIME_TYPE_SERVER == iTimeType)
	{
		iDiffDay = iOpenServerDay;
	}
	if (CLogicConfigDefine::TIME_TYPE_CREATE == iTimeType)
	{
		iDiffDay = iCreateDay;
	}
	if (CLogicConfigDefine::TIME_TYPE_CUSTOM == iTimeType)
	{
		iDiffDay = iCustomCreateDay;
	}
	if (CLogicConfigDefine::TIME_TYPE_MERGE == iTimeType)
	{
		iDiffDay = iMergeServerDay;
	}

	return iDiffDay >= iStartDay && iDiffDay <= iEndDay;
}

bool CLogicActive::IsWithinRangeTime(int32_t iNowTime, int32_t iStartTime, int32_t iEndTime)
{
	return iNowTime >= iStartTime && iNowTime <= iEndTime;
}

//获取当前的限时武将活动ID
int32_t CLogicActive::GetOpenCountdownHeroActivityID()
{
    const auto& config = CLogicConfig::GetActivityConfig().m_mapCountDownHeroConfig;
    for (auto pairActivity = config.begin(); pairActivity != config.end(); ++pairActivity)
    {
        if (CLogicActiveBase::IsActiveOpen(nullptr, pairActivity->first))
        {
            return pairActivity->first;
        }
    }
    return 0;
}

const TLogicActivityMultiTaskElem* CLogicActive::GetOpenMultiTaskConfig(
		user_data_table_ptr_type pUserInfo)
{
	for (const auto& task : CLogicConfig::GetActivityMultiTaskConfig().m_stMultiTaskMap)
	{
		if (IsActivityMultiTaskOpen(pUserInfo, task.first))
		{
			return &task.second;
		}
	}
	return nullptr;
}

const TLogicActivityTaskElem* CLogicActive::GetOpenMultiTaskSingleConfig(
		user_data_table_ptr_type pUserInfo, int32_t iTaskID)
{
	const auto *pstConfig = GetOpenMultiTaskConfig(pUserInfo);
	if (nullptr == pstConfig)
	{
		return nullptr;;
	}

	for (const auto& task : pstConfig->m_stTaskMultiMap)
	{
		if (task.second.m_iTaskID == iTaskID)
		{
			return &task.second;
		}
	}
	return nullptr;
}

bool CLogicActive::IsActivityMultiTaskOpen(user_data_table_ptr_type pUserInfo, int32_t iActivityID)
{
	const auto iter = CLogicConfig::GetActivityMultiTaskConfig().m_stMultiTaskMap.find(iActivityID);
	if (iter == CLogicConfig::GetActivityMultiTaskConfig().m_stMultiTaskMap.end())
	{
		return false;
	}

	return IsWithinActivityTime(pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_NONE, CLogicCommon::GetSec()
		, iter->second.m_iTimeType, iter->second.m_iStartDay, iter->second.m_iEndDay
		, iter->second.m_iServerDay, iter->second.m_iStartTime, iter->second.m_iEndTime);
}

const TLogicActivityTaskElem * CLogicActive::GetOpenActivityTaskConfig(
		user_data_table_ptr_type pUserInfo, int32_t iActivityType, int32_t iActivityID, int32_t iTaskID)
{
	const auto& tasks = CLogicConfig::GetActivityTaskConfig().m_stActiveTaskMap;
	for (auto iterActivity = tasks.lower_bound(iActivityType); iterActivity != tasks.upper_bound(iActivityType); ++iterActivity)
	{
		if (iterActivity->second.m_iActivityID == iActivityID && IsWithinActivityTime(pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_NONE, CLogicCommon::GetSec()
			, iterActivity->second.m_iTimeType, iterActivity->second.m_iStartDay, iterActivity->second.m_iEndDay
			, iterActivity->second.m_iServerDay, iterActivity->second.m_iStartTime, iterActivity->second.m_iEndTime))
		{
			for (const auto& task : iterActivity->second.m_stTaskMultiMap)
			{
				if (task.second.m_iTaskID == iTaskID)
				{
					return &task.second;
				}
			}
		}
	}
	return nullptr;
}

const TLogicActivitySequenceTask* CLogicActive::GetOpenActivitySequenceTaskConfig(user_data_table_ptr_type pUserInfo, int32_t iActivityType, int32_t iActivityID, int32_t iTaskID)
{
    const auto& tasks = CLogicConfig::GetActivityTaskConfig().m_stActiveTaskMap;
    for (auto iterActivity = tasks.lower_bound(iActivityType); iterActivity != tasks.upper_bound(iActivityType); ++iterActivity)
    {
        if (iterActivity->second.m_iActivityID == iActivityID && IsWithinActivityTime(pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_NONE, CLogicCommon::GetSec()
            , iterActivity->second.m_iTimeType, iterActivity->second.m_iStartDay, iterActivity->second.m_iEndDay
            , iterActivity->second.m_iServerDay, iterActivity->second.m_iStartTime, iterActivity->second.m_iEndTime))
        {
            auto iterTask = iterActivity->second.m_stSequenceTaskMap.find(iTaskID);
            return iterTask == iterActivity->second.m_stSequenceTaskMap.end() ? nullptr : &(iterTask->second);
        }
    }

    return nullptr;
}

const TLogicMergeActivityElem* CLogicActive::GetOpenMergeActivityConfig(int32_t iNowTime)
{
	const int32_t iMergeCount = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iMergeCount;
	const auto& merge_activity_map = CLogicConfig::GetMergeActivityConfig().m_stMergeActivityMap;
	const auto iter = merge_activity_map.upper_bound(iMergeCount);
	const decltype(merge_activity_map.rbegin()) riter(iter);
	if (riter != merge_activity_map.rend() && IsWithinRangeDay(nullptr, iNowTime, riter->second.m_iTimeType, riter->second.m_iStartDay, riter->second.m_iEndDay, 0))
	{
		return &riter->second;
	}
	return nullptr;
}

bool CLogicActive::IsMergeActivityMatchPvpMailAwardDouble()
{
	//巅峰对决次日发送邮件，所以要将当前时间减去一天
	const auto* config = GetOpenMergeActivityConfig(CLogicCommon::GetSec() - 1_day);
	if (nullptr == config)
	{
		return false;
	}

	return config->m_stDoubleTypeSet.find(1) != config->m_stDoubleTypeSet.end();
}

bool CLogicActive::IsMergeActivityAttackDefenseMailAwardDouble()
{
	const auto* config = GetOpenMergeActivityConfig(CLogicCommon::GetSec());
	if (nullptr == config)
	{
		return false;
	}

	return config->m_stDoubleTypeSet.find(2) != config->m_stDoubleTypeSet.end();
}

bool CLogicActive::IsMergeActivityFairPvpMailAwardDouble()
{
	//公平竞技次日发送邮件，所以要将当前时间减去一天
	const auto* config = GetOpenMergeActivityConfig(CLogicCommon::GetSec() - 1_day);
	if (nullptr == config)
	{
		return false;
	}

	return config->m_stDoubleTypeSet.find(3) != config->m_stDoubleTypeSet.end();
}

bool CLogicActive::IsActivityDaySingleRechargeOpen(user_data_table_ptr_type pUserInfo, int32_t iActivityID)
{
	const auto* pstConfig = CLogicConfig::GetDaySingleRechargeConfig().GetActivityConfig(iActivityID);
	if (nullptr == pstConfig)
	{
		return false;
	}

	const auto& ref = pstConfig->m_stActivityRef;
	return IsWithinActivityTime(pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_NONE, CLogicCommon::GetSec()
		, ref.m_iTimeType, ref.m_iStartDay, ref.m_iEndDay, ref.m_iServerDay, ref.m_iStartTime, ref.m_iEndTime);
}

void CLogicActive::InitNewThemeActivityCharge(user_data_table_ptr_type pUserInfo)
{
	if (!pUserInfo->m_stThemeActivityChargeMap.empty())
		return;

	int nowTime = CLogicCommon::GetSec();
	int dayBeginTime = get_day_begin_time(nowTime);
	for (auto &confIter : CLogicConfig::GetThemeActivityChargeConfig().m_stAllActivities)
	{
		if ((confIter.second.m_iStartTime <= nowTime) && (confIter.second.m_iEndTime > nowTime))
		{
			auto subConfIter = confIter.second.m_stDayActivities.find(dayBeginTime);
			if (subConfIter != confIter.second.m_stDayActivities.end())
			{
				for (auto &taskIter : subConfIter->second.m_stTask)
				{
					TUserThemeActivityChargeTableValueType newElem;
					newElem.m_iGroupID = pUserInfo->m_iGroupID;
					newElem.m_iUid = pUserInfo->m_iUin;
					newElem.m_iActivityID = confIter.first;
					newElem.m_iTaskID = taskIter.first;
					newElem.m_iTaskType = taskIter.second.m_iTaskType;
					newElem.m_iTargetType = taskIter.second.m_iType;
					newElem.m_iTargetID = taskIter.second.m_iTarget;
					newElem.m_iTargetAmount = taskIter.second.m_iTargetAmount;
					newElem.m_iProgress = 0;
					newElem.m_cState = charge_task_cannot_rewad;

					pUserInfo->m_stThemeActivityChargeMap[confIter.first].Insert(make_pair(newElem.m_iTaskID, newElem));
				}

				for (auto &taskIter : subConfIter->second.m_stBuyItem)
				{
					TUserThemeActivityChargeTableValueType newElem;
					newElem.m_iGroupID = pUserInfo->m_iGroupID;
					newElem.m_iUid = pUserInfo->m_iUin;
					newElem.m_iActivityID = confIter.first;
					newElem.m_iTaskID = taskIter.first;
					newElem.m_iTaskType = charge_task_buy;
					newElem.m_iTargetType = 0;
					newElem.m_iTargetID = 0;
					newElem.m_iTargetAmount = taskIter.second.m_iExchangeCount;
					newElem.m_iProgress = 0;
					newElem.m_cState = charge_task_cannot_rewad;

					pUserInfo->m_stThemeActivityChargeMap[confIter.first].Insert(make_pair(newElem.m_iTaskID, newElem));
				}

				for (auto &taskIter : subConfIter->second.m_stChargeItem)
				{
					TUserThemeActivityChargeTableValueType newElem;
					newElem.m_iGroupID = pUserInfo->m_iGroupID;
					newElem.m_iUid = pUserInfo->m_iUin;
					newElem.m_iActivityID = confIter.first;
					newElem.m_iTaskID = taskIter.second.m_id;
					newElem.m_iTaskType = charge_task_recharge;
					newElem.m_iTargetType = 0;
					newElem.m_iTargetID = 0;
					newElem.m_iTargetAmount = taskIter.second.m_iDayBuyNum;
					newElem.m_iProgress = 0;
					newElem.m_cState = charge_task_cannot_rewad;

					pUserInfo->m_stThemeActivityChargeMap[confIter.first].Insert(make_pair(newElem.m_iTaskID, newElem));

                    CLogicUserStat::ResetRechargeCount(pUserInfo, taskIter.second.m_iDepositID);
				}
			}
		}
	}
}

void CLogicActive::InitNewIBusyActivity(user_data_table_ptr_type pUserInfo)
{
	if (pUserInfo->m_stIBusyData.GetActivityID() != 0)
		return;

	int nowTime = CLogicCommon::GetSec();
	auto confIter = CLogicConfig::GetIBusyConfig().m_mpActivities.begin();
	for (; confIter != CLogicConfig::GetIBusyConfig().m_mpActivities.end(); ++confIter)
	{
		if ((nowTime >= confIter->second.m_iBeginTime) && (nowTime < confIter->second.m_iEndTime))
			break;
	}

	if (confIter != CLogicConfig::GetIBusyConfig().m_mpActivities.end())
	{
		//新一期活动
		if (pUserInfo->m_stIBusyData.GetActivityID() != confIter->first)
		{
			pUserInfo->m_stIBusyData.SetActivityID(confIter->first);
			pUserInfo->m_stIBusyData.SetLevel(1);
			pUserInfo->m_stIBusyData.SetExp(0);
			pUserInfo->m_stIBusyData.SetScore(0);
			// pUserInfo->m_stIBusyData.SetHuoyue(0);
			pUserInfo->m_stIBusyData.SetTodayHuoyue(0);
			pUserInfo->m_stIBusyData.SetChangeTaskCount(0);
			pUserInfo->m_stIBusyData.SetFreeChangeTaskCount(0);
			pUserInfo->m_stIBusyData.SetMarketLastRefreshTime(0);
			pUserInfo->m_stIBusyData.SetMarketRefreshTimes(0);
			pUserInfo->m_stIBusyData.SetGettedLevelReward({});
			CLogicActive::RefreshIBusyTaskData(pUserInfo);
		}
	}
}

void CLogicActive::UpdateIBusyMarketData(user_data_table_ptr_type pUserInfo)
{
	int activityID = pUserInfo->m_stIBusyData.GetActivityID();
	auto activityIter = CLogicConfig::GetIBusyConfig().m_mpActivities.find(activityID);
	if (activityIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
		return;

	int iLastRefreshTime = pUserInfo->m_stIBusyData.GetMarketLastRefreshTime();
	if (iLastRefreshTime == 0 || is_pass_some_hour(iLastRefreshTime, CLogicCommon::GetSec(), activityIter->second.m_stMarketData.m_stFlashTime))
	{
		RefreshIBusyMarketData(pUserInfo);
	}
}

void CLogicActive::RefreshIBusyMarketData(user_data_table_ptr_type pUserInfo)
{
	int activityID = pUserInfo->m_stIBusyData.GetActivityID();
	auto activityIter = CLogicConfig::GetIBusyConfig().m_mpActivities.find(activityID);
	if (activityIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
		return;

	int iLevel = pUserInfo->m_stUserInfoTable.GetLevel();
	auto stItemVec = activityIter->second.m_stMarketData.GetCommodityElem(iLevel);
	for (size_t i = 0; i < stItemVec.size(); ++i)
	{
		CPackMarket stElem;

		stElem.m_stCommodity = stItemVec[i].m_stCommodityItem;
		stElem.m_stConsume = stItemVec[i].m_stConsumeItem;
		stElem.m_iDiscount = stItemVec[i].m_iDiscount;

		int iCommodityID = i + 1;

		auto stIter = pUserInfo->m_stIBusyMarket.Find(iCommodityID);
		if (stIter == pUserInfo->m_stIBusyMarket.End())
		{
			TUserIbusyMarketTableValueType stVal;

			stVal.m_iCommodityID = iCommodityID;
			stVal.m_iGroupID = pUserInfo->m_stUserInfoTable.m_stData.m_iGroupID;
			stVal.m_iUid = pUserInfo->m_stUserInfoTable.m_stData.m_iUid;
			stVal.m_iCommodityItemType = stItemVec[i].m_stCommodityItem.m_iItemType;
			stVal.m_iCommodityItemID = stItemVec[i].m_stCommodityItem.m_iCardID;
			stVal.m_iCommodityItemNum = stItemVec[i].m_stCommodityItem.m_iNum;

			stVal.m_iConsumeItemType = stItemVec[i].m_stConsumeItem.m_iItemType;
			stVal.m_iConsumeItemID = stItemVec[i].m_stConsumeItem.m_iCardID;
			stVal.m_iConsumeItemNum = stItemVec[i].m_stConsumeItem.m_iNum;
			stVal.m_iDiscount = stItemVec[i].m_iDiscount;
			stVal.m_iLastBuyTime = 0;

			pUserInfo->m_stIBusyMarket.Insert(make_pair(iCommodityID, stVal));
		}
		else
		{
			stIter->second->SetCommodityItemType(stElem.m_stCommodity.m_iItemType);
			stIter->second->SetCommodityItemID(stElem.m_stCommodity.m_iCardID);
			stIter->second->SetCommodityItemNum(stElem.m_stCommodity.m_iNum);

			stIter->second->SetConsumeItemType(stElem.m_stConsume.m_iItemType);
			stIter->second->SetConsumeItemID(stElem.m_stConsume.m_iCardID);
			stIter->second->SetConsumeItemNum(stElem.m_stConsume.m_iNum);

			stIter->second->SetDiscount(stElem.m_iDiscount);
		}
	}
	pUserInfo->m_stIBusyData.SetMarketLastRefreshTime(CLogicCommon::GetSec());
}

void CLogicActive::RefreshIBusyTaskData(user_data_table_ptr_type pUserInfo)
{
	if (pUserInfo->m_stIBusyData.GetActivityID() == 0)
		return;

	CPackIbusyTaskData task_data;
	auto pConfIter = CLogicConfig::GetIBusyConfig().m_mpActivities.find(pUserInfo->m_stIBusyData.GetActivityID());
	if (pConfIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
		return;

	for (int i = 0 ; i < 6; ++i)
	{
		CPackIbusyTaskElem taskElem;
		taskElem.m_bFinished = false;

		int randomNum = CLogicCommon::GetRandNum(0, pConfIter->second.m_iTotalPinzhiCoef);
		int coef = 0;
		for (auto &pinzhiIter : pConfIter->second.m_vecPinzhis)
		{
			coef += pinzhiIter.m_iCoef;
			if (coef >= randomNum)
			{
				taskElem.m_iStar = pinzhiIter.m_iId;
				break;
			}
		}

		vector<int> matchedEvents;
		for (auto &eventIter : pConfIter->second.m_vecEvents)
		{
			if ((taskElem.m_iStar >= eventIter.m_iPinzhiMin) && (taskElem.m_iStar <= eventIter.m_iPinzhiMax))
			{
				matchedEvents.push_back(eventIter.m_iId);
			}
		}
		if (!matchedEvents.empty())
		{
			randomNum = CLogicCommon::GetRandNum(0, matchedEvents.size());
			taskElem.m_iTaskID = matchedEvents[randomNum];

			task_data.m_mpTasks.insert(make_pair(i + 1, taskElem));
		}
	}

	pUserInfo->m_stIBusyData.SetTaskInfo(task_data);
}

int32_t CLogicActive::GetScoreLevelOpenActivityID()
{
	const int32_t iNow = CLogicCommon::GetSec();
	for (const auto& info : CLogicConfig::GetScoreLevelConfig().m_stActivityMap)
	{
		if (iNow >= info.second.m_iBeginTime && iNow <= info.second.m_iEndTime)
		{
			return info.first;
		}
	}
	return 0;
}

void CLogicActive::InitNewScoreLevelActivity(user_data_table_ptr_type pUserInfo)
{
	const auto activity_id = GetScoreLevelOpenActivityID();
	const auto* config = CLogicConfig::GetScoreLevelConfig().GetActivityConfig(activity_id);
	if (nullptr == config)
	{
		return;
	}

	if (pUserInfo->m_stScoreLevelMap.Find(activity_id) != pUserInfo->m_stScoreLevelMap.End())
	{
		return;
	}

	TScoreLevelInfoTableValueType value;
	value.m_iUid = pUserInfo->m_iUin;
	value.m_iGroupID = pUserInfo->m_iGroupID;
	value.m_iActivityID = activity_id;
	value.m_iXingdongli = config->m_iInitXingdongli;
	value.m_iLastXingdongliTime = config->m_iBeginTime;
	pUserInfo->m_stScoreLevelMap.Insert(std::make_pair(activity_id, value));

	AutoAddScoreLevelXingdongli(pUserInfo);

	CLogicUserStat::ResetRechargeCount(pUserInfo, config->m_iScoreDepositID);
}

bool CLogicActive::AutoAddScoreLevelXingdongli(user_data_table_ptr_type pUserInfo)
{
	const auto activity_id = GetScoreLevelOpenActivityID();
	const auto* config = CLogicConfig::GetScoreLevelConfig().GetActivityConfig(activity_id);
	if (nullptr == config || config->m_iXingdongliSpeed < 0)
	{
		return false;
	}

	auto activity_iter = pUserInfo->m_stScoreLevelMap.Find(activity_id);
	if (activity_iter == pUserInfo->m_stScoreLevelMap.End())
	{
		return false;
	}

	const auto iNow = CLogicCommon::GetSec();
	const auto dis_time = iNow - activity_iter->second->GetLastXingdongliTime();
	if (dis_time > 0 && activity_iter->second->GetXingdongli() < config->m_iMaxXingdongli)
	{
		const int32_t iAdd = dis_time / config->m_iXingdongliSpeed;
		activity_iter->second->AddXingdongli(iAdd);
		activity_iter->second->AddLastXingdongliTime(iAdd * config->m_iXingdongliSpeed);
		if (activity_iter->second->GetXingdongli() >= config->m_iMaxXingdongli)
		{
			activity_iter->second->SetXingdongli(config->m_iMaxXingdongli);
			activity_iter->second->SetLastXingdongliTime(iNow);
		}
		return true;
	}
	else if (dis_time > 30 && activity_iter->second->GetXingdongli() >= config->m_iMaxXingdongli)
	{
		activity_iter->second->SetLastXingdongliTime(iNow);
		return true;
	}

	return false;
}

bool CLogicActive::UpdateUserQiyu(user_data_table_ptr_type pUserInfo)
{
	int nowTime = CLogicCommon::GetSec();

	//1.如果奇遇时间已到，则删除奇遇
	if ((pUserInfo->m_stUserInfoTable.GetDayQiyuID() != 0) && (pUserInfo->m_stUserInfoTable.GetQiyuEndtime() < nowTime))
	{
		ClearQiyuMap(pUserInfo, true);
	}

	if ((pUserInfo->m_stUserInfoTable.GetCangbaotuQiyuID() != 0) && (pUserInfo->m_stUserInfoTable.GetCangbaotuEndtime() < nowTime))
	{
		ClearQiyuMap(pUserInfo, false);
	}

	//2.如果没有每日奇遇，则初始化每日奇遇
	if (pUserInfo->m_stUserInfoTable.GetDayQiyuID() == 0)
	{
		int nowDay = weekday(now_sec());
		auto findIter = CLogicConfig::GetQiyuConfig().m_mpDayQiyu.find(nowDay);
		if ((findIter != CLogicConfig::GetQiyuConfig().m_mpDayQiyu.end()) && (!findIter->second.empty()))
		{
			int randomNum = CLogicCommon::GetRandNum(0, findIter->second.size());
			int mapID = findIter->second[randomNum];
			InitNewQiyuMap(pUserInfo, true, mapID);
		}
	}

	return true;
}

bool CLogicActive::ClearQiyuMap(user_data_table_ptr_type pUserInfo, bool dayQiyuOrCangbaotu)
{
	if (dayQiyuOrCangbaotu == true)
	{
		if (pUserInfo->m_stUserInfoTable.GetDayQiyuID() != 0)
		{
			//结点信息清除
			auto findIter = pUserInfo->m_stQiyuMap.find(pUserInfo->m_stUserInfoTable.GetDayQiyuID());
			if (findIter != pUserInfo->m_stQiyuMap.end())
			{
				auto &mapNode = findIter->second;

				while (mapNode.Size() > 0)
				{
					mapNode.Erase(mapNode.Begin());
				}
			}
			pUserInfo->m_stQiyuMap.erase(findIter);

			//全局信息清理
			pUserInfo->m_stUserInfoTable.SetDayQiyuID(0);
			pUserInfo->m_stUserInfoTable.SetQiyuEndtime(0);
			CPackEntireWarBuffVector qiyuBuff;
			pUserInfo->m_stUserInfoTable.SetQiyuBuff(qiyuBuff);

			//伙伴的血量消耗重置
			for (auto stIter = pUserInfo->m_stHeroCardTableMap.Begin();
				stIter != pUserInfo->m_stHeroCardTableMap.End();
				++stIter)
			{
				CLogicCacheCardOperation cardOperation(pUserInfo);
				if (!cardOperation.CheckHeroCardExist(stIter->first))
					continue;

				stIter->second->SetQiyuFightCostHp(0);
			}

			//上阵信息重置
            pUserInfo->m_stUserTeamInfoTable.SetTeam9({});
		}
	}
	else
	{
		if (pUserInfo->m_stUserInfoTable.GetCangbaotuQiyuID() != 0)
		{
			//结点信息清除
			auto findIter = pUserInfo->m_stQiyuMap.find(pUserInfo->m_stUserInfoTable.GetCangbaotuQiyuID());
			if (findIter != pUserInfo->m_stQiyuMap.end())
			{
				auto &mapNode = findIter->second;

				while (mapNode.Size() > 0)
				{
					mapNode.Erase(mapNode.Begin());
				}
			}
			pUserInfo->m_stQiyuMap.erase(findIter);

			//全局信息清理
			pUserInfo->m_stUserInfoTable.SetCangbaotuQiyuID(0);
			pUserInfo->m_stUserInfoTable.SetCangbaotuEndtime(0);
			CPackEntireWarBuffVector qiyuBuff;
			pUserInfo->m_stUserInfoTable.SetCangbaotuBuff(qiyuBuff);

			//伙伴的血量消耗重置
			for (auto stIter = pUserInfo->m_stHeroCardTableMap.Begin();
				stIter != pUserInfo->m_stHeroCardTableMap.End();
				++stIter)
			{
				CLogicCacheCardOperation cardOperation(pUserInfo);
				if (!cardOperation.CheckHeroCardExist(stIter->first))
					continue;

				stIter->second->SetCangbaotuFightCostHp(0);
			}

			//上阵信息重置
            pUserInfo->m_stUserTeamInfoTable.SetTeam10({});
		}
	}

	return true;
}

bool CLogicActive::InitNewQiyuMap(user_data_table_ptr_type pUserInfo, bool dayQiyuOrCangbaotu, int mapID)
{
	//判断等级
	if (pUserInfo->m_stUserInfoTable.GetLevel() < CLogicConfig::GetQiyuConfig().m_iOpenLevel)
		return false;

	//如果有老的设置，先清除老的设置
	ClearQiyuMap(pUserInfo, dayQiyuOrCangbaotu);

	//1.找到对应的奇遇地图配置
	auto mapIter = CLogicConfig::GetQiyuConfig().m_mpQiyuMap.find(mapID);
	if (mapIter == CLogicConfig::GetQiyuConfig().m_mpQiyuMap.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(COMMON_CONFIG_NOT_EXIST);
		return false;
	}

	//2.找到对应的等级事件池配置
	const vector<stQiyuEventNode> *pEventVec = nullptr;
	for (auto &levelIter : mapIter->second.m_mpEventNodes)
	{
		if (pUserInfo->m_stUserInfoTable.GetLevel() <= levelIter.first)
		{
			pEventVec = &(levelIter.second);
			break;
		}
	}
	if (pEventVec == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(COMMON_CONFIG_NOT_EXIST);
		return false;
	}

	//3.开始配置
	//3.1全局信息配置
	if (dayQiyuOrCangbaotu == true)
	{
		pUserInfo->m_stUserInfoTable.SetDayQiyuID(mapID);
		pUserInfo->m_stUserInfoTable.SetQiyuEndtime(get_day_begin_time(CLogicCommon::GetSec()) + 86400);
		CPackEntireWarBuffVector qiyuBuff;
		pUserInfo->m_stUserInfoTable.SetQiyuBuff(qiyuBuff);

		//3.2伙伴的血量消耗重置
		for (auto stIter = pUserInfo->m_stHeroCardTableMap.Begin();
			stIter != pUserInfo->m_stHeroCardTableMap.End();
			++stIter)
		{
			CLogicCacheCardOperation cardOperation(pUserInfo);
			if (!cardOperation.CheckHeroCardExist(stIter->first))
				continue;

			stIter->second->SetQiyuFightCostHp(0);
		}
	}
	else
	{
		pUserInfo->m_stUserInfoTable.SetCangbaotuQiyuID(mapID);
		pUserInfo->m_stUserInfoTable.SetCangbaotuEndtime(mapIter->second.m_iLifeTime + CLogicCommon::GetSec());
		CPackEntireWarBuffVector qiyuBuff;
		pUserInfo->m_stUserInfoTable.SetCangbaotuBuff(qiyuBuff);

		//3.2伙伴的血量消耗重置
		for (auto stIter = pUserInfo->m_stHeroCardTableMap.Begin();
			stIter != pUserInfo->m_stHeroCardTableMap.End();
			++stIter)
		{
			CLogicCacheCardOperation cardOperation(pUserInfo);
			if (!cardOperation.CheckHeroCardExist(stIter->first))
				continue;

			stIter->second->SetCangbaotuFightCostHp(0);
		}
	}

	//3.2先初始化所有节点，没有事件
	set<int> emptyNode;
	user_data_table_s::qiyu_container_type &qiyuMap = pUserInfo->m_stQiyuMap[mapID];
	for (auto &nodeIter : mapIter->second.m_mpMapNodes)
	{
		TUserQiyuTableValueType newNode;
		newNode.m_iGroupID = pUserInfo->m_iGroupID;
		newNode.m_iUid = pUserInfo->m_iUin;
		newNode.m_iMapID = mapID;
		newNode.m_iNodeID = nodeIter.first;
		newNode.m_iNodeState = 0;
		newNode.m_iNodeEventType = 0;
		newNode.m_iNodeEventID = 0;

		qiyuMap.Insert(make_pair(newNode.m_iNodeID, newNode));

		emptyNode.insert(nodeIter.first);
	}

	//3.3给节点赋予事件
	multimap<int, stQiyuEventNode> mpEventNode;
	for (auto &eventIter : *pEventVec)
	{
		if (eventIter.m_aMapNodes.size() > 0)
			mpEventNode.insert(make_pair(eventIter.m_aMapNodes.size(), eventIter));
		else
			mpEventNode.insert(make_pair(999, eventIter));
	}

	vector<stQiyuEventNode> leftEvent;//剩余未放置的事件
	for (auto &eventIter : mpEventNode)
	{
		int finishCount = 0;
		for (int i = 0; i < eventIter.second.m_iCount; ++i)
		{
			while (!eventIter.second.m_aMapNodes.empty())
			{
				int randomIndex = CLogicCommon::GetRandNum(0, eventIter.second.m_aMapNodes.size());
				if (emptyNode.find(eventIter.second.m_aMapNodes[randomIndex]) != emptyNode.end())
				{
					auto nodeIter = qiyuMap.Find(eventIter.second.m_aMapNodes[randomIndex]);
					nodeIter->second->SetNodeEventType(eventIter.second.m_cType);
					nodeIter->second->SetNodeEventID(eventIter.second.m_iEventID);

					emptyNode.erase(eventIter.second.m_aMapNodes[randomIndex]);
					eventIter.second.m_aMapNodes.erase(
                            find(eventIter.second.m_aMapNodes.begin(), eventIter.second.m_aMapNodes.end(), eventIter.second.m_aMapNodes[randomIndex]));

					++finishCount;
					break;
				}
				else
				{
					eventIter.second.m_aMapNodes.erase(
                            find(eventIter.second.m_aMapNodes.begin(), eventIter.second.m_aMapNodes.end(), eventIter.second.m_aMapNodes[randomIndex]));
				}
			}
		}

		if (finishCount < eventIter.second.m_iCount)
		{
			stQiyuEventNode tmpNode = eventIter.second;
			tmpNode.m_iCount = eventIter.second.m_iCount - finishCount;
			leftEvent.push_back(tmpNode);
		}
	}

	//剩余事件直接放置到空节点中
	for (auto &leftIter : leftEvent)
	{
		for (int i = 0; i < leftIter.m_iCount; ++i)
		{
			if (!emptyNode.empty())
			{
				int randomIndex = CLogicCommon::GetRandNum(0, emptyNode.size());
				auto emptyNodeIter = emptyNode.begin();
				advance(emptyNodeIter, randomIndex);

				auto nodeIter = qiyuMap.Find(*emptyNodeIter);
				nodeIter->second->SetNodeEventType(leftIter.m_cType);
				nodeIter->second->SetNodeEventID(leftIter.m_iEventID);

				emptyNode.erase(emptyNodeIter);
			}
		}
	}

	//战报清空
	if (dayQiyuOrCangbaotu)
        CLogicCacheCommon::ClearUserFightReport(pUserInfo, CLogicConfigDefine::FIGHT_REPORT_QIYU);
	else
        CLogicCacheCommon::ClearUserFightReport(pUserInfo, CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU);


	return true;
}

bool CLogicActive::RefreshQiyuMap(user_data_table_ptr_type pUserInfo, bool dayQiyuOrCangbaotu, int mapID)
{
	if (mapID == pUserInfo->m_stUserInfoTable.GetDayQiyuID())
	{
		ClearQiyuMap(pUserInfo, true);
		UpdateUserQiyu(pUserInfo);
	}
	else
	{
		//测试用函数，方便测试初始化奇遇地图
		InitNewQiyuMap(pUserInfo, dayQiyuOrCangbaotu, mapID);
	}

	return true;
}

bool CLogicActive::IsMonthPassOpen(user_data_table_ptr_type pUserInfo)
{
    const auto& config = CLogicConfig::GetMonthPassConfig();
    return CLogicActive::IsWithinRangeDay(pUserInfo, config.m_iTimeType, config.m_iStartDay, config.m_iEndDay, config.m_iCustomCreateTime);
}

bool CLogicActive::IsNewPassOpen(user_data_table_ptr_type pUserInfo)
{
    const auto& config = CLogicConfig::GetNewPassConfig();
    return (CLogicActive::IsAvyOpen(config.m_iActID, pUserInfo->m_stUserInfoTable.GetCreateTime()));
}

bool CLogicActive::IsLimitBossLevelOpen(user_data_table_ptr_type pUserInfo)
{
    const int32_t iNowTime = CLogicCommon::GetSec();
    const auto& config = CLogicConfig::GetLimitBossConfig();
    return config.m_iLevelStartTime <= iNowTime && iNowTime <= config.m_iLevelEndTime;
}

bool CLogicActive::IsGashaponOpen(user_data_table_ptr_type pUserInfo)
{
    const auto& config = CLogicConfig::GetGashaponConfig();
    return (CLogicActive::IsAvyOpen(config.m_iAvyID, pUserInfo->m_stUserInfoTable.GetCreateTime()));
}

bool CLogicActive::IsGashaponChapterOpen(user_data_table_ptr_type pUserInfo)
{
    const auto& config = CLogicConfig::GetGashaponConfig();
    return (CLogicActive::IsAvyOpen(config.m_iChapterAvyID, pUserInfo->m_stUserInfoTable.GetCreateTime()));
}