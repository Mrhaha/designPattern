#include "common_datetime.h"
#include "logic_link_common.h"
#include "logic_game_user_stat.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_active.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_item_factory.h"
#include "logic_game_bill.h"

void CLogicUserStat::SubscribeGameUserLevelUpInfoCallBack(user_data_table_ptr_type pUserData)
{
    LOGIC_STATISTICS_INS->UpdatePlayerInfo(pUserData->m_iUin, pUserData->m_iGroupID, pUserData->m_iChannelID,
                                          pUserData->m_stUserInfoTable.GetCreateTime(), pUserData->m_strNick,
                                          pUserData->m_stUserInfoTable.GetLevel(),
                                          pUserData->m_stUserInfoTable.GetRmb(),
                                          pUserData->m_stUserInfoTable.GetAddUpRmb(),
                                          pUserData->m_stUserInfoTable.GetRoleCombatMax());
}

void CLogicUserStat::SubscribeFunctionalGameItemInfoCallBack(user_data_table_ptr_type pUserData, const logic_functional_game_item_channel_data_t& stData)
{
	CLogicBill::SaveGameItemBill(pUserData->m_iUin, pUserData->m_iGroupID, pUserData->m_iChannelID, stData.m_iCmd,
                                          stData.m_iItemType, stData.m_iItemID, stData.m_iCurrentNum, stData.m_iDeltaNum, stData.m_strInfo);
}

int CLogicUserStat::GetDailyRechargeThresholdCount(user_data_table_ptr_type pUserData, int iBeginTime, int iEndTime, int iThreshold)
{
    int iCount = 0;
	int iTimestamp = iBeginTime;
	int iDateInt = get_date_integer(iTimestamp);
	int iDateIntEnd = get_date_integer(iEndTime);
	while (iDateInt <= iDateIntEnd)
	{
		auto stIter = pUserData->m_stUserStatTableMap.Find(iDateInt);
		if (stIter != pUserData->m_stUserStatTableMap.End() && stIter->second->GetPayMoney() >= iThreshold)
		{
			++iCount;
		}
		
		iTimestamp += 86400;
		iDateInt = get_date_integer(iTimestamp);
	}

    return iCount;
}

void 
CLogicUserStat::OnRecharge(user_data_table_ptr_type pUserData, int iPayMoney)
{
	OnRechargeSerialPay(pUserData, iPayMoney);

	OnRechargeDaySingle(pUserData, iPayMoney);

	OnRechargeMonthPass(pUserData, iPayMoney);
}

bool CLogicUserStat::isTodayRechargeThreshold(user_data_table_ptr_type pUserData, int iThreshold)
{
	int iDateInt = get_date_integer(CLogicCommon::GetSec());
	auto stIter = pUserData->m_stUserStatTableMap.Find(iDateInt);
    return stIter != pUserData->m_stUserStatTableMap.End() && stIter->second->GetPayMoney() >= iThreshold;
}

void CLogicUserStat::ResetRechargeCount(user_data_table_ptr_type pUserData, int32_t iDepositID)
{
    auto iter = pUserData->m_stUserDepositMap.Find(iDepositID);
    if (iter != pUserData->m_stUserDepositMap.End())
    {
        iter->second->SetBuyTimes(0);
        iter->second->SetDoubleTimes(0);
        iter->second->SetTodayBuyTimes(0);
        iter->second->SetWeekBuyTimes(0);
        iter->second->SetMonthBuyTimes(0);
        iter->second->SetDayIntervalBuyTimes(0);
        iter->second->SetLastDepositTime(CLogicCommon::GetSec());
    }
}

int32_t CLogicUserStat::GetLeftRechargeDoubleCount(user_data_table_ptr_type pUserData, int32_t iDepositID)
{
	int32_t iHaveCount = 0;
	auto iter = pUserData->m_stUserDepositMap.Find(iDepositID);
	if (iter != pUserData->m_stUserDepositMap.End())
	{
		iHaveCount = iter->second->GetDoubleTimes();
	}

	int32_t iTotalCount = pUserData->m_stUserInfoTable.GetActiveDoubleDepositCount();
	const auto* pstVipConfig = CLogicConfig::GetVipConfig().GetVipInfo(pUserData->m_stUserInfoTable.GetVipLevel());
	if (nullptr != pstVipConfig)
	{
		iTotalCount += pstVipConfig->m_iDoubleRechargeCount;
	}

	return iTotalCount - iHaveCount;
}

int32_t CLogicUserStat::GetLastRechargeTime(user_data_table_ptr_type pUserData, int32_t iDepositID)
{
    auto iter = pUserData->m_stUserDepositMap.Find(iDepositID);
    if (iter != pUserData->m_stUserDepositMap.End())
    {
        return iter->second->GetLastDepositTime();
    }

    return 0;
}

int32_t CLogicUserStat::GetLeftRechargeBuyCount(user_data_table_ptr_type pUserData, int32_t iDepositID)
{
	int32_t iTodayCount = 0;
	int32_t iTotalCount = 0;
	int32_t iWeekCount = 0;
	int32_t iMonthCount = 0;
	int32_t iDayIntervalCount = 0;
	int32_t iLeftCount = 0;
	const auto iter = pUserData->m_stUserDepositMap.Find(iDepositID);
	if (iter != pUserData->m_stUserDepositMap.End())
	{
		iTodayCount = iter->second->GetTodayBuyTimes();
		iTotalCount = iter->second->GetBuyTimes();
		iWeekCount = iter->second->GetWeekBuyTimes();
		iMonthCount = iter->second->GetMonthBuyTimes();
		iDayIntervalCount = iter->second->GetDayIntervalBuyTimes();
	}

	const auto pstDeposit = CLogicConfig::GetDepositConfig().GetDepositConfig(iDepositID);
	if (nullptr == pstDeposit)
        return 0;
	{
	}

	bool bCantNotBuy = false;
	bCantNotBuy += (pUserData->m_stUserInfoTable.GetLevel() < pstDeposit->m_iShowLevel || pUserData->m_stUserInfoTable.GetLevel() > pstDeposit->m_iShowMaxLevel);
    bCantNotBuy += (pUserData->m_stUserInfoTable.GetVipLevel() < pstDeposit->m_iVipLvMin || pUserData->m_stUserInfoTable.GetVipLevel() > pstDeposit->m_iVipLvMax);
    // TODO
	// bCantNotBuy += (pstDeposit->m_iAvyID > 0 && ??);
	bCantNotBuy += pstDeposit->m_iBuyItemID > 0 && !LOGIC_GAME_ITEM_FACTORY_INS->CheckGameItem((CLogicConfigDefine::GAME_ITEM_COLLECTION)pstDeposit->m_iBuyItemType, pstDeposit->m_iBuyItemID);
	bCantNotBuy += pstDeposit->m_iType == CLogicConfigDefine::DEPOSIT_TYPE_MERGE_GIFT && !CanBuyMergeGiftDepositID(pUserData, iDepositID);
	bCantNotBuy += pstDeposit->m_iType == CLogicConfigDefine::DEPOSIT_TYPE_THEME_CHARGE && !CanBuyThemeRechargeDepositID(pUserData, iDepositID);
	bCantNotBuy += pstDeposit->m_iType == CLogicConfigDefine::DEPOSIT_TYPE_IBUSY && !CanBuyIbusyDepositID(pUserData, iDepositID);
    bCantNotBuy += pstDeposit->m_iType == CLogicConfigDefine::DEPOSIT_TYPE_SPECIAL_WEEK && !CanBuySpecialGiftDepositID(pUserData, pstDeposit->m_iType, iDepositID);
    bCantNotBuy += pstDeposit->m_iType == CLogicConfigDefine::DEPOSIT_TYPE_SPECIAL_MONTH && !CanBuySpecialGiftDepositID(pUserData, pstDeposit->m_iType, iDepositID);
    bCantNotBuy += pstDeposit->m_iType == CLogicConfigDefine::DEPOSIT_TYPE_SCORE_LEVEL && !CanBuyScoreLevelDepositID(pUserData, iDepositID);
    bCantNotBuy += pstDeposit->m_iType == CLogicConfigDefine::DEPOSIT_TYPE_MONTH_PASS && !CanBuyMonthPassDepositID(pUserData, iDepositID);

	if (bCantNotBuy)
	{
		return 0;
	}

	iLeftCount = 999;
	if (pstDeposit->m_iMaxBuyCount > 0)
	{
		iLeftCount = std::min(iLeftCount, pstDeposit->m_iMaxBuyCount - iTotalCount);
	}
	if (pstDeposit->m_iDailyBuyCount > 0)
	{
		iLeftCount = std::min(iLeftCount, pstDeposit->m_iDailyBuyCount - iTodayCount);
	}
	if (pstDeposit->m_iWeeklyBuyCount > 0)
	{
		iLeftCount = std::min(iLeftCount, pstDeposit->m_iWeeklyBuyCount - iWeekCount);
	}
	if(pstDeposit->m_iMonthBuyCount > 0)
    {
	    iLeftCount = std::min(iLeftCount, pstDeposit->m_iMonthBuyCount - iMonthCount);
    }
	if(pstDeposit->m_iDayIntervalBuyCount > 0)
    {
	    iLeftCount = std::min(iLeftCount, pstDeposit->m_iDayIntervalBuyCount - iDayIntervalCount);
    }

	return iLeftCount;
}

bool CLogicUserStat::HaveBuyDepositID(user_data_table_ptr_type pUserData, int32_t iDepositID)
{
	const auto iter = pUserData->m_stUserDepositMap.Find(iDepositID);
	return iter != pUserData->m_stUserDepositMap.End() && iter->second->GetBuyTimes() > 0;
}

void CLogicUserStat::OnRechargeSerialPay(user_data_table_ptr_type pUserData, int iPayMoney)
{
	int iDateInt = get_date_integer(CLogicCommon::GetSec());
	auto stIter = pUserData->m_stUserStatTableMap.Find(iDateInt);
	if (stIter != pUserData->m_stUserStatTableMap.End())
	{
		stIter->second->AddPayMoney(iPayMoney);
	}
	else
	{
		TUserStatTableValueType stNodeValue;
		stNodeValue.m_iUid = pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stNodeValue.m_iGroupID = pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
		stNodeValue.m_iStatDate = iDateInt;
		stNodeValue.m_iPayMoney = iPayMoney;

		typename decltype(pUserData->m_stUserStatTableMap)::table_value_type stTableValue(stNodeValue.m_iStatDate, stNodeValue);
		pUserData->m_stUserStatTableMap.Insert(stTableValue);
	}
}

void CLogicUserStat::OnRechargeDaySingle(user_data_table_ptr_type pUserData, int32_t iPayMoney)
{
	for (const auto& activity : CLogicConfig::GetDaySingleRechargeConfig().m_stActivityMap)
	{
		if (CLogicActive::IsActivityDaySingleRechargeOpen(pUserData, activity.first))
		{
			const auto iterTask = activity.second.m_stTaskMap.upper_bound(iPayMoney);
			const decltype(activity.second.m_stTaskMap.rbegin()) riterTask(iterTask);
			if (riterTask != activity.second.m_stTaskMap.rend())
			{
				const auto key = std::make_pair(activity.first, riterTask->second.m_iTaskID);
				auto iterUserTask = pUserData->m_stDaySingleRechargeMap.Find(key);
				if (iterUserTask == pUserData->m_stDaySingleRechargeMap.End())
				{
					decltype(pUserData->m_stDaySingleRechargeMap)::table_mapped_type value;
					value.m_iUid = pUserData->m_iUin;
					value.m_iGroupID = pUserData->m_iGroupID;
					value.m_iActivityID = activity.first;
					value.m_iTaskID = riterTask->second.m_iTaskID;
					value.m_iRewardCount = 0;
					value.m_iCanRewardCount = 0;
					const auto stRet = pUserData->m_stDaySingleRechargeMap.Insert(std::make_pair(key, value));
					if (!stRet.second)
					{
						return;
					}
					iterUserTask = stRet.first;
				}
				if (iterUserTask->second->GetRewardCount() + iterUserTask->second->GetCanRewardCount() < riterTask->second.m_iTaskNum)
				{
					iterUserTask->second->AddCanRewardCount(1);
					CLogicUserNotifySystem::NotifyDaySingleRechargeTaskChange(pUserData, activity.first, riterTask->second.m_iTaskID
						, iterUserTask->second->GetRewardCount(), iterUserTask->second->GetCanRewardCount());
				}
			}
		}
	}
}

void CLogicUserStat::OnRechargeMonthPass(user_data_table_ptr_type pUserData, int iPayMoney)
{
    if (CLogicActive::IsMonthPassOpen(pUserData))
    {
        pUserData->m_stIBusyData.AddMonthPassRecharge(iPayMoney);
        CLogicUserNotifySystem::NotifyMonthPass(pUserData);
    }
}

bool CLogicUserStat::CanBuyMergeGiftDepositID(user_data_table_ptr_type pUserInfo, int32_t iDepositID)
{
    //没有合服配置，或不在合服时间，不能买
    const auto* config = CLogicActive::GetOpenMergeActivityConfig(CLogicCommon::GetSec());
    if (nullptr == config)
    {
        return false;
    }

    //当前合服配置是否有该物品，拥有则能购买
    return config->m_stDepositIDSet.find(iDepositID) != config->m_stDepositIDSet.end();
}

bool CLogicUserStat::CanBuyThemeRechargeDepositID(user_data_table_ptr_type pUserInfo, int32_t iDepositID)
{
    const int32_t iNowTime = CLogicCommon::GetSec();
    const int32_t iBeginTime = get_day_begin_time(iNowTime);
    for (auto &iterActive : CLogicConfig::GetThemeActivityChargeConfig().m_stAllActivities)
    {
        if ((iterActive.second.m_iStartTime <= iNowTime) && (iterActive.second.m_iEndTime > iNowTime))
        {
            auto iterDayActivity = iterActive.second.m_stDayActivities.find(iBeginTime);
            if (iterDayActivity != iterActive.second.m_stDayActivities.end())
            {
                for (const auto& charge : iterDayActivity->second.m_stChargeItem)
                {
                    if (charge.second.m_iDepositID == iDepositID)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool CLogicUserStat::CanBuyIbusyDepositID(user_data_table_ptr_type pUserInfo, int32_t iDepositID)
{
    //已经购买过，不能购买
    const auto iterDeposit = pUserInfo->m_stUserDepositMap.Find(iDepositID);
    if (iterDeposit != pUserInfo->m_stUserDepositMap.End() && iterDeposit->second->GetBuyTimes() > 0)
    {
        return false;
    }

    //没有开启 不能购买
    const auto iterConfig = CLogicConfig::GetIBusyConfig().m_mpActivities.find(pUserInfo->m_stIBusyData.GetActivityID());
    return iterConfig != CLogicConfig::GetIBusyConfig().m_mpActivities.end() && iterConfig->second.m_iDepositID == iDepositID;
}

bool CLogicUserStat::CanBuyScoreLevelDepositID(user_data_table_ptr_type pUserInfo, int32_t iDepositID)
{
    const auto activity_id = CLogicActive::GetScoreLevelOpenActivityID();
    const auto* config = CLogicConfig::GetScoreLevelConfig().GetActivityConfig(activity_id);
    if (nullptr == config)
    {
        return false;
    }

    if (pUserInfo->m_stScoreLevelMap.Find(activity_id) == pUserInfo->m_stScoreLevelMap.End())
    {
        return false;
    }

    const auto iterDeposit = pUserInfo->m_stUserDepositMap.Find(iDepositID);
    if (iterDeposit != pUserInfo->m_stUserDepositMap.End() && iterDeposit->second->GetBuyTimes() > 0)
    {
        return false;
    }

    return config->m_iScoreDepositID == iDepositID;
}

bool CLogicUserStat::CanBuySpecialGiftDepositID(user_data_table_ptr_type pUserData, int32_t iType, int32_t iDepositID)
{
    if(iType != CLogicConfigDefine::DEPOSIT_TYPE_SPECIAL_WEEK && iType != CLogicConfigDefine::DEPOSIT_TYPE_SPECIAL_MONTH)
        return false;

    int32_t iMyActID;
    int32_t iMyLevel;
    if(iType == CLogicConfigDefine::DEPOSIT_TYPE_SPECIAL_WEEK)
    {
        iMyActID = pUserData->m_stIBusyData.GetSpecialGiftWeekID();
        iMyLevel = pUserData->m_stIBusyData.GetSpecialGiftWeekLevel();
    }
    else
    {
        iMyActID = pUserData->m_stIBusyData.GetSpecialGiftMonthID();
        iMyLevel = pUserData->m_stIBusyData.GetSpecialGiftMonthLevel();
    }

    if(iMyActID <= 0)
        return false;

    return CLogicConfig::GetSpecialGiftConfig().IsValidDepositID(
            (iType == CLogicConfigDefine::DEPOSIT_TYPE_SPECIAL_WEEK ? CLogicConfigDefine::ESGT_Week : CLogicConfigDefine::ESGT_Month)
            , iMyActID, iMyLevel, iDepositID);
}

bool CLogicUserStat::CanBuyMonthPassDepositID(user_data_table_ptr_type pUserInfo, int32_t iDepositID)
{
    if (!CLogicActive::IsNewPassOpen(pUserInfo))
    {
        return false;
    }

    if (CLogicConfig::GetNewPassConfig().m_iDepositID != iDepositID &&
        CLogicConfig::GetNewPassConfig().m_iAdvancedDepositID != iDepositID &&
        CLogicConfig::GetNewPassConfig().m_iUpDepositID != iDepositID)
    {
        return false;
    }

    auto normalIter = pUserInfo->m_stUserDepositMap.Find(CLogicConfig::GetNewPassConfig().m_iDepositID);
    auto upIter = pUserInfo->m_stUserDepositMap.Find(CLogicConfig::GetNewPassConfig().m_iUpDepositID);
    auto advIter = pUserInfo->m_stUserDepositMap.Find(CLogicConfig::GetNewPassConfig().m_iAdvancedDepositID);

    //买普通版本
    if (iDepositID == CLogicConfig::GetNewPassConfig().m_iDepositID)
    {
        if (normalIter != pUserInfo->m_stUserDepositMap.End() && normalIter->second->GetBuyTimes() > 0)
        {
            return false;
        }

        if (upIter != pUserInfo->m_stUserDepositMap.End() && upIter->second->GetBuyTimes() > 0)
        {
            return false;
        }

        if (advIter != pUserInfo->m_stUserDepositMap.End() && advIter->second->GetBuyTimes() > 0)
        {
            return false;
        }
    }

    if (iDepositID == CLogicConfig::GetNewPassConfig().m_iUpDepositID)
    {
        if (normalIter == pUserInfo->m_stUserDepositMap.End() || normalIter->second->GetBuyTimes() == 0)
        {
            return false;
        }

        if (advIter != pUserInfo->m_stUserDepositMap.End() && advIter->second->GetBuyTimes() > 0)
        {
            return false;
        }
    }

    if (iDepositID == CLogicConfig::GetNewPassConfig().m_iAdvancedDepositID)
    {
        if (normalIter != pUserInfo->m_stUserDepositMap.End() && normalIter->second->GetBuyTimes() > 0)
        {
            return false;
        }

        if (upIter != pUserInfo->m_stUserDepositMap.End() && upIter->second->GetBuyTimes() > 0)
        {
            return false;
        }

        if (advIter != pUserInfo->m_stUserDepositMap.End() && advIter->second->GetBuyTimes() > 0)
        {
            return false;
        }
    }
    return true;
}
