#include "common_datetime.h"
#include "server_cmd_logic_define.h"
#include "logic_link_common.h"
#include "logic_config.h"
#include "logic_game_day_refresh_system.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_cache_common.h"
#include "logic_game_active_info.h"
#include "logic_game_active.h"
#include "logic_game_rank_list.h"
#include "logic_game_cache_card_operation.h"
#include "logic_record_server_nick_table.h"
#include "logic_game_user_stat.h"
#include "logic_game_global.h"
#include "logic_game_common_publisher.h"
#include "../service/logic_service_logger.h"
#include "server_cmd_data_report_define.h"
#include "server_data_report_protocol.h"



void CLogicDayRefreshSystem::CheckUserDataRefresh(user_data_table_ptr_type pUserInfo)
{
	const auto iLastRefreshTime = pUserInfo->m_stUserDailyTable.GetLastRefreshTime();
	if (LOGIC_CHECK_DAY_REFRESH(iLastRefreshTime))
	{
        //每天刷新
        RefreshUserDailyData(pUserInfo);

        // 周刷新
        if (LOGIC_CHECK_WEEK_REFRESH(iLastRefreshTime))
        {
            RefreshUserWeeklyData(pUserInfo);
        }

        // 月刷新
        if(LOGIC_CHECK_MONTH_REFRESH(iLastRefreshTime))
        {
            RefreshUserMonthlyData(pUserInfo);
        }
	}

	const auto iLastMergeRefreshTime = pUserInfo->m_stUserInfoTable.GetLastMergeRefreshTime();
	if (iLastMergeRefreshTime < CLogicConfig::GetSystemConfig().m_stServerConfig.m_iMergeTime)
	{
		RefreshUserMergeData(pUserInfo);
	}

	//有些实时要求不高的数据， 凌晨两点时开始处理
	if (CLogicCommon::GetSec() > pUserInfo->m_stOnlyInCacheData.m_iNextCheckLazyDataTime)
	{
		pUserInfo->m_stOnlyInCacheData.m_iNextCheckLazyDataTime = get_hour_timestamp(CLogicCommon::GetSec(), 2) + 1_day;

		LazyDeleteActivityInfo(pUserInfo);

		LazyUserTaskStatus(pUserInfo);

        // 刷新特权周、月礼
        RefreshUserSpecialGiftInfo(pUserInfo, CLogicConfigDefine::ESGT_Week);
        RefreshUserSpecialGiftInfo(pUserInfo, CLogicConfigDefine::ESGT_Month);
	}
}

void CLogicDayRefreshSystem::RefreshUserDailyData(user_data_table_ptr_type pUserInfo)
{
    RefreshUserBuyBackInfo(pUserInfo);

    RefreshUserTableData(pUserInfo);

    // 刷新每日任务
    RefreshUserDailyQuest(pUserInfo);

    RefreshUserExploreQuest(pUserInfo);

    RefreshUserShopCount(pUserInfo, CLogicConfigDefine::ESLT_Daily);

    // 刷新特权周、月礼
    RefreshUserSpecialGiftInfo(pUserInfo, CLogicConfigDefine::ESGT_Week);
    RefreshUserSpecialGiftInfo(pUserInfo, CLogicConfigDefine::ESGT_Month);

    RefreshMailActivityAward(pUserInfo);

    RefreshActivityTask(pUserInfo);

    RefreshUserDepositTimes(pUserInfo);

    RefreshActivityLoginInsure(pUserInfo);

    RefreshActivityLoginFund(pUserInfo);

    RefreshActivityFlowerLantern(pUserInfo);

    RefreshFairPvpInfo(pUserInfo);

    RefreshMatchPvpInfo(pUserInfo);

    RefreshUserQiyuInfo(pUserInfo);

    RefreshUserThemeActivityChargeInfo(pUserInfo);

    RefreshUserIBusyInfo(pUserInfo);

    RefreshUserScoreLevelInfo(pUserInfo);

    RefreshUserDaySingleRechargeInfo(pUserInfo);

    //RefreshMonthPassInfo(pUserInfo);
    //发送月卡奖励
    RefreshSendMonthCardDailyMail(pUserInfo);

    //通行证每日刷新
    RefreshUserNewPassInfo(pUserInfo);

    //限定池结束，pt点兑换
    RefreshLimitCommodityPT(pUserInfo);

    //清空限时Boss信息
    RefreshLimitBoss(pUserInfo);

    //社团个人每日刷新信息
    RefreshGuildReward(pUserInfo);

    //刷新家园每日信息
    RefreshHomeInfo(pUserInfo);

    //检测是否发放cbt测试奖励
    RefreshRegisterAccount(pUserInfo);

    //刷新念写卡商店信息
    RefreshConstelShop(pUserInfo);

    //根据活动id控制活动内容刷新
    RefreshAvyInfo(pUserInfo);


    CLogicUserNotifySystem::NotifyUserDailyData(pUserInfo);

    CLogicUserNotifySystem::NotifyDayRefreshEvent(pUserInfo);

    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_Login;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(pUserInfo, stEventData);
}

void CLogicDayRefreshSystem::RefreshUserWeeklyData(user_data_table_ptr_type pUserInfo)
{
    RefreshUserShopCount(pUserInfo, CLogicConfigDefine::ESLT_Weekly);

    RefreshUserZhuHaiData(pUserInfo);

    //刷新通行证每周任务
    RefreshUserNewPassWeeklyInfo(pUserInfo);

    //刷新道馆每周任务
    RefreshPVPWeeklyInfo(pUserInfo);

    //发送道馆奖励
    RefreshPVPWeeklyReward(pUserInfo);

    //刷新报纸奖励和阵营选择
    RefreshWenWenChoice(pUserInfo);



}

void CLogicDayRefreshSystem::RefreshUserMonthlyData(user_data_table_ptr_type pUserInfo)
{
    RefreshUserShopCount(pUserInfo, CLogicConfigDefine::ESLT_Monthly);

    pUserInfo->m_stUserInfoTable.SetMonthAddUpRmb(0);
}

void CLogicDayRefreshSystem::RefreshUserShopCount(user_data_table_ptr_type pUserInfo, CLogicConfigDefine::EnumStoreLimitType eType)
{
    auto iterConfig = CLogicConfig::GetShopConfig().m_stLimitTypeMap.find(eType);
    if(iterConfig == CLogicConfig::GetShopConfig().m_stLimitTypeMap.end())
        return;

    for(auto index : iterConfig->second)
    {
        auto iterShop = pUserInfo->m_stUserShopMap.Find(index);
        if(iterShop != pUserInfo->m_stUserShopMap.End())
        {
            iterShop->second->SetBuyCount(0);
        }
    }
}

void CLogicDayRefreshSystem::RefreshUserZhuHaiData(user_data_table_ptr_type pUserInfo)
{
    pUserInfo->m_stIBusyData.SetZhuHaiSaveTeamFlag(0);
    pUserInfo->m_stIBusyData.SetZhuHaiLevel(0);
    pUserInfo->m_stIBusyData.SetZhuHaiGress(0);
    pUserInfo->m_stIBusyData.SetZhuHaiBuff({});
    pUserInfo->m_stIBusyData.SetZhuHaiCardHp({});
    pUserInfo->m_stIBusyData.SetZhuHaiRoleBuff({});
    pUserInfo->m_stIBusyData.SetZhuHaiBlessGroup({});
    pUserInfo->m_stUserInfoTable.SetZhuhaiScore(0);
}

void CLogicDayRefreshSystem::RefreshUserTableData(user_data_table_ptr_type pUserInfo)
{
	if (pUserInfo->m_stUserDailyTable.GetEntireWarWorshippedCount() > 0)
	{
		const auto& config = CLogicConfig::GetEntireWarConfig();
		std::vector<CPackGameItem> stWarshipItemVec = config.m_stWorshipItemVec;
		for (const auto& item : config.m_stWarGodWorshippedItemVec)
		{
			stWarshipItemVec.emplace_back(item.m_iItemType, item.m_iCardID, item.m_iNum * pUserInfo->m_stUserDailyTable.GetEntireWarWorshippedCount());
		}
		const int32_t iAddCharm = pUserInfo->m_stUserDailyTable.GetEntireWarWorshippedCount() * CLogicConfig::GetEntireWarConfig().m_iWorshipCharm;
		CLogicCacheCommon::AddCharm(pUserInfo, iAddCharm);

		const std::string strContent = config.m_stWarGodWorshippedMailContent.Replace({ { "charm", std::to_string(iAddCharm)} });
        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID,
                                    config.m_strWarGodWorshippedMailSender, config.m_strWarGodWorshippedMailTitle,
                                    strContent,
                                    stWarshipItemVec, "SEND_ENTIRE_WAR_WORSHIPPED_AWARD");
	}

	pUserInfo->m_stUserDailyTable.SetLastRefreshTime(CLogicCommon::GetSec());
	pUserInfo->m_stUserDailyTable.SetFreeTurnTableCount(0);
	pUserInfo->m_stUserDailyTable.SetCostEnergy(0);
	pUserInfo->m_stUserDailyTable.SetLeftTurnCardCount(0);
	pUserInfo->m_stUserDailyTable.SetTurnCardBitmap(0);
	pUserInfo->m_stUserDailyTable.SetTurnCardAwardBitmap(0);
	pUserInfo->m_stUserDailyTable.SetVipDailyAwardBitmap(0);
	pUserInfo->m_stUserDailyTable.SetQuarterVipAwardLevel(0);
	pUserInfo->m_stUserDailyTable.SetFreeCountdownHeroTableCount(0);
	pUserInfo->m_stUserDailyTable.SetWorshipWarGodCount(0);
	pUserInfo->m_stUserDailyTable.SetEntireWarWorshippedCount(0);
    pUserInfo->m_stUserDailyTable.SetEntireWarDailyGameTimes(0);
	pUserInfo->m_stUserDailyTable.SetBuyActionCoinCount(0);
	pUserInfo->m_stUserDailyTable.SetJubaoCount(0);
	pUserInfo->m_stUserDailyTable.SetMergeLevelCount(0);
	pUserInfo->m_stUserDailyTable.SetLuckyHammerLogCount(0);
	pUserInfo->m_stUserDailyTable.SetDailyAwardState(0);
	pUserInfo->m_stUserDailyTable.SetDatingPlaceTimes({});
	pUserInfo->m_stUserDailyTable.SetSimpleGameTimes({});
	pUserInfo->m_stUserDailyTable.SetFlyArrayRewardState(0);
    pUserInfo->m_stUserDailyTable.SetRedBagEarnings({});
    pUserInfo->m_stUserDailyTable.SetLimitBossDailyNum(0);
    pUserInfo->m_stUserDailyTable.SetLimitBoxDailyNum(0);
    pUserInfo->m_stUserDailyTable.SetLimitBossAction(0);
    pUserInfo->m_stUserDailyTable.SetFishingTimes(0);
    pUserInfo->m_stUserDailyTable.SetWishSwapTimes(0);

	// 飞天大炮弹阵列奖励重置
    /*
	auto stFlyArrayReward = pUserInfo->m_stUserDailyTable.GetFlyArrayReward();
	if(int(stFlyArrayReward.size()) != CLogicConfig::GetSimpleGameConfig().m_iFlyArrayRewardMaxNum)
    {
        stFlyArrayReward.resize(CLogicConfig::GetSimpleGameConfig().m_iFlyArrayRewardMaxNum);
        for(size_t i = 0; i < stFlyArrayReward.size(); ++i)
        {
            stFlyArrayReward[i] = i+1;
        }
    }
	std::random_shuffle(stFlyArrayReward.begin(), stFlyArrayReward.end());
	pUserInfo->m_stUserDailyTable.SetFlyArrayReward(stFlyArrayReward);
    */

	pUserInfo->m_stUserInfoTable.SetBuyGoldTimes(0);
    pUserInfo->m_stUserInfoTable.SetBuyEnergyTimes(0);
	pUserInfo->m_stOnlyInCacheData.m_iTodayLongTalkCount = 0;

	pUserInfo->m_stIBusyData.SetCardDialogLimitAvyStat({});

	if(!CLogicConfig::GetCardDialogConfig().m_stTimesCDList.empty())
    {
        const auto iter = CLogicConfig::GetCardDialogConfig().m_stTimesCDList[0];
        pUserInfo->m_stIBusyData.SetCardDialogNextTime(CLogicCommon::GetSec() + CLogicCommon::GetRandNum(iter.first, iter.second));
    }

	pUserInfo->m_stIBusyData.SetCardDialogTimes(0);
}

void CLogicDayRefreshSystem::RefreshUserExploreQuest(user_data_table_ptr_type pUserInfo)
{
    // 未领取的奖励发邮件
    auto& pConfig = CLogicConfig::GetHomeGardenExploreConfig();
    for(auto iter = pUserInfo->m_stHomeExploreMap.Begin(); iter != pUserInfo->m_stHomeExploreMap.End(); ++iter)
    {
        switch (iter->second->GetState())
        {
        case CLogicConfigHomeGardenExploreParser::EQS_CanAccept:
            if(!iter->second->GetIsCbt())
                iter->second->SetState(CLogicConfigHomeGardenExploreParser::EQS_Received);
            break;
        case CLogicConfigHomeGardenExploreParser::EQS_Accepted:
            if(iter->second->GetEndTime() < CLogicCommon::GetSec())
            {
                std::vector<CPackGameItem> stRewards;
                CLogicCacheCommon::GetHomeExploreReward(pUserInfo, iter->first, iter->second->GetTeam(), stRewards);
                CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID, pConfig.m_strExploreMailSender,
                                               pConfig.m_strExploreMailTitle, pConfig.m_strExploreMailContent,
                                               stRewards, "SEND_EXPLORE_QUEST_REWARD");

                iter->second->SetState(CLogicConfigHomeGardenExploreParser::EQS_Received);
                iter->second->SetTeam({});
                iter->second->SetEndTime(0);
            }
            break;
        default:
            break;
        }
    }

    // 刷新任务
    int32_t iQuestNum = CLogicCommon::GetRandNum(pConfig.m_iExploreNumMin, pConfig.m_iExploreNumMax, true);
    if(iQuestNum <= 0) return;

    auto pLevelConfig = pConfig.GetExploreLevelConfig(pUserInfo->m_stIBusyData.GetExploreExp());
    if(nullptr == pLevelConfig) return;

    while (iQuestNum--)
    {
        int32_t iStar = CLogicCacheCommon::RandStlCont(pLevelConfig->m_stWeight, pLevelConfig->m_iTotalWeight);
        if(iStar <= 0) continue;

        auto pQuestRandMap = pConfig.GetExploreStarRandConfig(iStar);
        if(pQuestRandMap == nullptr) continue;

        // 随机10次，如果还是重复就跳过
        for(int m = 0; m < 10; ++m)
        {
            int32_t iQuestID = CLogicCacheCommon::RandStlCont(pQuestRandMap->m_stWeight, pQuestRandMap->m_iTotalWeight);
            if(iQuestID <= 0) continue;

            auto iterData = pUserInfo->m_stHomeExploreMap.Find(iQuestID);
            if(iterData != pUserInfo->m_stHomeExploreMap.End())
            {
                if(iterData->second->GetState() == CLogicConfigHomeGardenExploreParser::EQS_Received)
                {
                    iterData->second->SetState(CLogicConfigHomeGardenExploreParser::EQS_CanAccept);
                    iterData->second->SetTeam({});
                    iterData->second->SetEndTime(0);
                    break;
                }
            }
            else
            {
                TUserHomeExploreTableValueType stValue;
                stValue.m_iUid = pUserInfo->m_iUin;
                stValue.m_iGroupID = pUserInfo->m_iGroupID;
                stValue.m_iExploreQuestID = iQuestID;
                stValue.m_iEndTime = 0;
                stValue.m_cState = CLogicConfigHomeGardenExploreParser::EQS_CanAccept;
                pUserInfo->m_stHomeExploreMap.Insert(std::make_pair(stValue.m_iExploreQuestID, stValue));

                break;
            }
        }
    }
}

void CLogicDayRefreshSystem::RefreshActivityTask(user_data_table_ptr_type pUserInfo)
{
	bool bUpdate = false;

	std::vector<decltype(pUserInfo->m_stActivityTaskMap)::key_type> stDeleteVec;

	for (auto iter = pUserInfo->m_stActivityTaskMap.Begin(); iter != pUserInfo->m_stActivityTaskMap.End(); ++iter)
	{
		const auto iActivityType = iter->second->m_stData.m_iActivityType;
		switch (iActivityType)
		{
		case CLogicConfigDefine::ACTIVITY_TYPE_MULTI_TASK:
			{
				if (!CLogicActive::IsActivityMultiTaskOpen(pUserInfo, iter->second->m_stData.m_iActivityID))
				{
					stDeleteVec.emplace_back(iter->first);
				}
			}
			break;
		}
	}

	while (!stDeleteVec.empty())
	{
		pUserInfo->m_stActivityTaskMap.Erase(stDeleteVec.back());
		stDeleteVec.pop_back();
	}

	if (bUpdate)
	{
		// CLogicUserNotifySystem::NotifyUserAllActivityTask(pUserInfo);
	}
}

void CLogicDayRefreshSystem::RefreshMailActivityAward(user_data_table_ptr_type pUserInfo)
{
	const auto iterEventMail = CLogicConfig::GetMailConfig().m_astEventMail.find("mail_activity_award");
	if (iterEventMail == CLogicConfig::GetMailConfig().m_astEventMail.end())
	{
		return;
	}

	const auto& activity_config = CLogicConfig::GetActivityConfig();
	const auto& type_2_id_map = activity_config.m_stActivityType2IDMap;
	const std::vector<int32_t> activity_type_vec = {
		CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_DEPOSIT,
		CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_CONSUME_YUANBAO,
		CLogicConfigDefine::ACTIVITY_TYPE_LUCKY_BOX,
		CLogicConfigDefine::ACTIVITY_TYPE_TURN_TABLE,
		CLogicConfigDefine::ACTIVITY_TYPE_XUNBAO,
		CLogicConfigDefine::ACTIVITY_TYPE_COUNTDOWN_HERO,
	};
	std::vector<int32_t> activity_id_vec;
	for (const auto& activity_type : activity_type_vec)
	{
		for (auto iterType = type_2_id_map.lower_bound(activity_type); iterType != type_2_id_map.upper_bound(activity_type); ++iterType)
		{
			activity_id_vec.push_back(iterType->second);
		}
	}

	const int32_t iNow = CLogicCommon::GetSec();
	const int32_t iYesterday = iNow - 1_day;
	for (const auto& activity_id : activity_id_vec)
	{
		auto iterRef = CLogicConfig::GetActivityConfig().m_stActivityRef.find(activity_id);
		if (iterRef == CLogicConfig::GetActivityConfig().m_stActivityRef.end())
		{
			continue;
		}

		if (!CLogicActive::IsWithinActivityTime(pUserInfo, iterRef->second.m_iType, iYesterday
			, iterRef->second.m_iTimeType, iterRef->second.m_iStartDay, iterRef->second.m_iEndDay
			, iterRef->second.m_iServerDay, iterRef->second.m_iStartTime, iterRef->second.m_iEndTime))
		{
			continue;
		}

		if (CLogicActive::IsWithinActivityTime(pUserInfo, iterRef->second.m_iType, iNow
			, iterRef->second.m_iTimeType, iterRef->second.m_iStartDay, iterRef->second.m_iEndDay
			, iterRef->second.m_iServerDay, iterRef->second.m_iStartTime, iterRef->second.m_iEndTime))
		{
			continue;
		}

		auto stFuncIter = CLogicActiveBase::CREATE_ACTIVE_FUNC_MAP.find(iterRef->second.m_iType);
		if (stFuncIter == CLogicActiveBase::CREATE_ACTIVE_FUNC_MAP.end())
		{
			continue;
		}

		auto pActiveObjPtr = ((stFuncIter->second)(pUserInfo, activity_id));
		if (pActiveObjPtr.get() == nullptr)
		{
			continue;
		}

		for (size_t i = 0 ; i < 20; ++i)
		{
			std::vector<CPackGameItem> stGameItemList;
			if (pActiveObjPtr->GetActiveGift(i, stGameItemList,MSG_LOGIC_MAIL_ACTIVITY_AWARD))
			{
                CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID, iterEventMail->second.m_strSender,
                                            iterEventMail->second.m_strTitle,
                                            iterEventMail->second.m_stContent.Replace({}), stGameItemList,
                                            "MAIL_ACTIVITY_AWARD_" + std::to_string(i), 1);
			}
		}
	}	
}

void CLogicDayRefreshSystem::RefreshActivityLoginInsure(user_data_table_ptr_type pUserInfo)
{
	for (const auto& active : CLogicConfig::GetActivityConfig().m_stLoginInsureMap)
	{
		auto iterActive = pUserInfo->m_stUserActiveMap.Find(active.first);
		if (iterActive != pUserInfo->m_stUserActiveMap.End() && int32_t(active.second.size()) <= bin1num(iterActive->second->GetActiveReceivedBitmap()))
		{
			iterActive->second->SetActiveTargetNum(0);
			iterActive->second->SetActiveReceivedBitmap(0);
			iterActive->second->SetActiveCompleteBitmap(0);
			iterActive->second->SetActiveTime(CLogicCommon::GetSec());

			// pUserInfo->m_stUserInfoTable.SetLoginInsureOpenTime(0);
		}
	}
}

void CLogicDayRefreshSystem::RefreshActivityLoginFund(user_data_table_ptr_type pUserInfo)
{
    for (const auto& active : CLogicConfig::GetActivityConfig().m_stLoginFundMap)
    {
        auto iterActive = pUserInfo->m_stUserActiveMap.Find(active.first);
        if (iterActive != pUserInfo->m_stUserActiveMap.End() && int32_t(active.second.second.size()) <= bin1num(iterActive->second->GetActiveReceivedBitmap()))
        {
            /*
            const auto* pstDepositConfig = CLogicConfig::GetDepositConfig().GetDepositConfig(active.second.first);
            if (pstDepositConfig && pstDepositConfig->m_iBuyItemType == CLogicConfigDefine::BENEFITS_CARD)
            {
                const std::map<int32_t, std::function<void()>> login_fund_func_map = {
                        {CLogicConfigDefine::LOGIN_FUND_1_CARD_ID, std::bind(&TUserInfoTable::SetLoginFund1OpenTime, &pUserInfo->m_stUserInfoTable, 0)},
                        {CLogicConfigDefine::LOGIN_FUND_2_CARD_ID, std::bind(&TUserInfoTable::SetLoginFund2OpenTime, &pUserInfo->m_stUserInfoTable, 0)},
                        {CLogicConfigDefine::LOGIN_FUND_3_CARD_ID, std::bind(&TUserInfoTable::SetLoginFund3OpenTime, &pUserInfo->m_stUserInfoTable, 0)},
                        {CLogicConfigDefine::LOGIN_FUND_4_CARD_ID, std::bind(&TUserInfoTable::SetLoginFund4OpenTime, &pUserInfo->m_stUserInfoTable, 0)},
                        {CLogicConfigDefine::LOGIN_FUND_5_CARD_ID, std::bind(&TUserInfoTable::SetLoginFund5OpenTime, &pUserInfo->m_stUserInfoTable, 0)},
                };

                auto iterLoginFund = login_fund_func_map.find(pstDepositConfig->m_iBuyItemID);
                if (iterLoginFund != login_fund_func_map.end())
                {
                    iterActive->second->SetActiveTargetNum(0);
                    iterActive->second->SetActiveReceivedBitmap(0);
                    iterActive->second->SetActiveCompleteBitmap(0);
                    iterActive->second->SetActiveTime(CLogicCommon::GetSec());

                    iterLoginFund->second();
                }
            }
             */
        }
    }
}

void CLogicDayRefreshSystem::RefreshUserDepositTimes(user_data_table_ptr_type pUserInfo)
{
	for (auto iter = pUserInfo->m_stUserDepositMap.Begin(); iter != pUserInfo->m_stUserDepositMap.End(); ++iter)
	{
		const auto pstConfig = CLogicConfig::GetDepositConfig().GetDepositConfig(iter->first);
		if (nullptr != pstConfig)
		{
			const int32_t iLastDepositTime = iter->second->GetLastDepositTime();
			if (iter->second->GetTodayBuyTimes() > 0 && LOGIC_CHECK_DAY_REFRESH(iLastDepositTime))
			{
				iter->second->SetTodayBuyTimes(0);
			}
			if (iter->second->GetWeekBuyTimes() > 0 && LOGIC_CHECK_WEEK_REFRESH(iLastDepositTime))
			{
				iter->second->SetWeekBuyTimes(0);
			}
			if(iter->second->GetMonthBuyTimes() > 0 && LOGIC_CHECK_MONTH_REFRESH(iLastDepositTime))
            {
			    iter->second->SetMonthBuyTimes(0);
            }
			if(iter->second->GetDayIntervalBuyTimes() > 0 && day_diff(iLastDepositTime, CLogicCommon::GetSec()) >= pstConfig->m_iDayIntervalNum)
            {
                iter->second->SetDayIntervalBuyTimes(0);
            }
		}
	}
}

void CLogicDayRefreshSystem::RefreshUserBuyBackInfo(user_data_table_ptr_type pUserInfo)
{
	for (const auto& back : CLogicConfig::GetAwardConfig().m_stBuyBackMap)
	{
		auto iter = pUserInfo->m_stBuyBackMap.Find(back.first);
		if (iter == pUserInfo->m_stBuyBackMap.End())
		{
			TUserBuyBackTableValueType stVal;
			stVal.m_iUid = pUserInfo->m_iUin;
			stVal.m_iGroupID = pUserInfo->m_iGroupID;
			stVal.m_iBackType = back.first;
			stVal.m_iBuyBackCount = 0;
			stVal.m_iCanBackCount = 0;
			const auto stRet = pUserInfo->m_stBuyBackMap.Insert(std::make_pair(stVal.m_iBackType, stVal));
			if (!stRet.second)
			{
				return;
			}
			iter = stRet.first;
		}

		iter->second->AddCanBackCount(-iter->second->GetUseBackCount());
		iter->second->SetBuyBackCount(0);
		iter->second->SetUseBackCount(0);

		const int32_t iAddCanBackCount = GetCanBuyBackCount(pUserInfo, static_cast<CEnumBuyBackType>(back.first), back.second.m_iDailyCount, back.second.m_iPara);
		iter->second->AddCanBackCount(iAddCanBackCount);
		if (iter->second->GetCanBackCount() > back.second.m_iMaxCount)
		{
			iter->second->SetCanBackCount(back.second.m_iMaxCount);
		}
	}

	CLogicUserNotifySystem::NotifyUserBuyBackInfo(pUserInfo);
}

void CLogicDayRefreshSystem::RefreshActivityFlowerLantern(user_data_table_ptr_type pUserInfo)
{
	CLogicFlowerLanternActive::AddCandle(pUserInfo, CLogicConfigDefine::FLOWER_LANTERN_EVENT_LOGIN, 1);

	std::vector<int32_t> vtDeleteLogIndex;
	for (const auto& info : CLogicConfig::GetActivityConfig().m_stFlowerLanterInfoConfig)
	{
		if (!CLogicActiveBase::IsActiveOpen(pUserInfo, info.first))
		{
			auto iterActivity = pUserInfo->m_stUserActiveMap.Find(info.first);
			if (iterActivity != pUserInfo->m_stUserActiveMap.End())
			{
				//删掉对应活动ID的日志,先记录，后面删掉
				for (auto iterLantern = pUserInfo->m_stFlowerLanternMap.Begin(); iterLantern != pUserInfo->m_stFlowerLanternMap.End(); ++iterLantern)
				{
					if (iterLantern->second->GetActivityID() == info.first)
					{
						vtDeleteLogIndex.push_back(iterLantern->first);
					}
				}
				while (!vtDeleteLogIndex.empty())
				{
					pUserInfo->m_stFlowerLanternMap.Erase(vtDeleteLogIndex.back());
					vtDeleteLogIndex.pop_back();
				}

				//根据剩余蜡烛数量返回元宝
				if (iterActivity->second->GetActiveTargetNum() > 0)
				{
					const int32_t iNum = iterActivity->second->GetActiveTargetNum();
					const int32_t iYuanbao = iNum * info.second.m_iCandleReturnDiamond;
                    CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID, info.second.m_strReturnSender,
                                                info.second.m_strReturnTitle,
                                                info.second.m_stReturnContent.Replace({{"num",     std::to_string(
                                                        iNum)},
                                                                                       {"yuanbao", std::to_string(
                                                                                               iYuanbao)}}),
                                                {{CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, iYuanbao}},
                                                "FLOWER_LANTERN_RETURN_YUANBAO_MAIL");

					iterActivity->second->SetActiveTargetNum(0);
				}

				//删掉活动数据
				pUserInfo->m_stUserActiveMap.Erase(iterActivity);
			}
		}
	}
}

void CLogicDayRefreshSystem::RefreshFairPvpInfo(user_data_table_ptr_type pUserInfo)
{
	if (!pUserInfo->m_stUserInfoTable.GetFairPvpTeamCard().empty())
	{
		const auto* pstConfig = CLogicConfig::GetFairPvpConfig().GetFairPvpWinConfig(pUserInfo->m_stUserInfoTable.GetFairPvpWinCount(), CLogicCommon::GetSec() - 1_day);
		if (nullptr != pstConfig)
		{
			if (CLogicActive::IsMergeActivityFairPvpMailAwardDouble())
			{
				decltype(pstConfig->m_stBonusItemVec) bonus = pstConfig->m_stBonusItemVec;
				for (auto& item : bonus)
				{
					item.m_iNum *= 2;
				}
                CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID, pstConfig->m_strSender,
                                            pstConfig->m_strTitle, pstConfig->m_strContent, bonus,
                                            "SEND_FAIR_PVP_AWARD_MAIL", 2);
			}
			else
			{
                CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID, pstConfig->m_strSender,
                                            pstConfig->m_strTitle, pstConfig->m_strContent, pstConfig->m_stBonusItemVec,
                                            "SEND_FAIR_PVP_AWARD_MAIL");
			}
		}
	}

	pUserInfo->m_stUserInfoTable.SetFairPvpWinCount(0);
	pUserInfo->m_stUserInfoTable.SetFairPvpLoseCount(0);
	pUserInfo->m_stUserInfoTable.SetFairPvpStoreCard({});
	pUserInfo->m_stUserInfoTable.SetFairPvpTeamCard({});

	pUserInfo->m_stOnlyInCacheData.m_stFairPvpPlayer = {};

	CLogicUserNotifySystem::NotifyFairPvpInfo(pUserInfo);

	//清空战报
    CLogicCacheCommon::ClearUserFightReport(pUserInfo, CLogicConfigDefine::FIGHT_REPORT_FAIR_PVP);
}

void CLogicDayRefreshSystem::RefreshMatchPvpInfo(user_data_table_ptr_type pUserInfo)
{
	const auto& config = CLogicConfig::GetMatchPvpConfig();
	if (pUserInfo->m_stUserInfoTable.GetMatchDailyCount() >= config.m_iDailyAwardCount)
	{
		const auto iLevel = config.GetMatchLevelByScore(pUserInfo->m_stUserInfoTable.GetMatchScore());
		const auto pstAward = config.GetDailyAwardConfig(iLevel);
		if (nullptr != pstAward)
		{
			if (CLogicActive::IsMergeActivityMatchPvpMailAwardDouble())
			{
				decltype(pstAward->m_stBonusVec) bonus = pstAward->m_stBonusVec;
				for (auto& item : bonus)
				{
					item.m_iNum *= 2;
				}
                CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID,
                                            pstAward->m_strSender, pstAward->m_strTitle, pstAward->m_strContent, bonus,
                                            "SEND_MATCH_PVP_DAILY_AWARD", 2);
			}
			else
			{
                CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID,
                                            pstAward->m_strSender, pstAward->m_strTitle, pstAward->m_strContent,
                                            pstAward->m_stBonusVec, "SEND_MATCH_PVP_DAILY_AWARD");
			}			
		}
	}

	pUserInfo->m_stUserInfoTable.SetMatchDailyCount(0);
	auto stStoreBuyCount = pUserInfo->m_stUserInfoTable.GetMatchStoreBuyCount();
	for (auto& store_count : stStoreBuyCount.m_iBuyCountMap)
	{
		const auto* pstConfig = CLogicConfig::GetMatchPvpConfig().GetMatchStoreConfig(store_count.first);
		if (nullptr != pstConfig && pstConfig->m_iDailyBuyCount > 0)
		{
			store_count.second = 0;
		}
	}
	pUserInfo->m_stUserInfoTable.SetMatchStoreBuyCount(stStoreBuyCount);

	CLogicUserNotifySystem::NotifyMatchPvpInfo(pUserInfo);
}

void CLogicDayRefreshSystem::RefreshSendMonthCardDailyMail(user_data_table_ptr_type pUserInfo)
{
    if(!CLogicBuff::IsHaveMonthCard(pUserInfo))
        return;

    auto& stConfig = CLogicConfig::GetAwardConfig().m_stMonthCardConfig;
    if(stConfig.m_stAwardItems.size() > 0)
    {
        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID,
                                       stConfig.m_strSender, stConfig.m_strTitle, stConfig.m_strContent,
                                       stConfig.m_stAwardItems,
                                       "SEND_MONTH_CARD_DAILY_AWARD",1,false,stConfig.m_iExpiredDay);
    }

    if(!CLogicBuff::IsHaveFatMonthCard(pUserInfo))
    {
        return;
    }

    auto& stFatConfig = CLogicConfig::GetAwardConfig().m_stFatMonthCardConfig;
    if(stFatConfig.m_stAwardItems.size() > 0)
    {
        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID,
                                       stFatConfig.m_strSender, stFatConfig.m_strTitle, stFatConfig.m_strContent,
                                       stFatConfig.m_stAwardItems,
                                       "SEND_FAT_MONTH_CARD_DAILY_AWARD",1,false,stFatConfig.m_iExpiredDay);
    }
}

void CLogicDayRefreshSystem::RefreshUserMergeData(user_data_table_ptr_type pUserInfo)
{
	if (pUserInfo->m_iGroupID != CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID)
	{
		const std::string old_nick = pUserInfo->m_strNick;
		if (!MergeChangeRoleNick(pUserInfo))
		{
            LOGIC_SERVICE_API_INS->RequestServiceAlert("change_nick_fail", {
                    {"uin", std::to_string(pUserInfo->m_iUin)},
                    {"group_id", std::to_string(pUserInfo->m_iGroupID)},
                    {"old_nick", old_nick},
            });
			return;
		}
		if (old_nick != pUserInfo->m_strNick)
		{
			CLogicGlobal::AddAsyncEventMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID, "merge_change_nick", { {"nick", old_nick}, {"merge_nick", pUserInfo->m_strNick } });
		}
	}

	//记录刷新时间
	pUserInfo->m_stUserInfoTable.SetLastMergeRefreshTime(CLogicCommon::GetSec());

	//pvp刷新
	pUserInfo->m_stUserInfoTable.SetPvpHighestRank(0);
	pUserInfo->m_stUserInfoTable.SetPvpFightTimes(0);
	pUserInfo->m_stUserInfoTable.SetRefreshPvpTimes(0);

	//全民战役奖励刷新
	pUserInfo->m_stUserInfoTable.SetEntireWarAwardBitmap(0);
	pUserInfo->m_stUserInfoTable.SetEntireWarNodeAwardStat({});
	pUserInfo->m_stUserInfoTable.SetEntireWarExtraStatus({});

	//合服活动刷新
	RefreshMergeActivity(pUserInfo);

	//排行恢复
	CLogicRankList::RestoreUserRank(pUserInfo);

	//通知合服时间及次数
	CLogicUserNotifySystem::NotifyServerConfig(pUserInfo);
}

void CLogicDayRefreshSystem::RefreshUserQiyuInfo(user_data_table_ptr_type pUserInfo)
{
	CLogicUserNotifySystem::NotifyQiyuData(pUserInfo);
}

void CLogicDayRefreshSystem::RefreshUserThemeActivityChargeInfo(user_data_table_ptr_type pUserInfo)
{
	int nowTime = CLogicCommon::GetSec();
	int dayBeginTime = get_day_begin_time(nowTime);

	//1.未领取的奖励，通过邮件方式发放
	for (auto &iter : pUserInfo->m_stThemeActivityChargeMap)
	{
		auto confIter = CLogicConfig::GetThemeActivityChargeConfig().m_stAllActivities.find(iter.first);
		if (confIter == CLogicConfig::GetThemeActivityChargeConfig().m_stAllActivities.end())
		{
			continue;
		}
		auto dayIter = confIter->second.m_stDayActivities.find(dayBeginTime - 86400);
		if (dayIter == confIter->second.m_stDayActivities.end())
		{
			continue;
		}

		std::vector<CPackGameItem> rewards;
		std::vector<int> 	canGetTasks;

		for (auto subIter = iter.second.Begin(); subIter != iter.second.End(); ++subIter)
		{
			if (subIter->second->GetState() == charge_task_can_rewad)
				canGetTasks.push_back(subIter->first);
		}
		for (auto &taskIter : canGetTasks)
		{
			auto taskConfIter = dayIter->second.m_stTask.find(taskIter);
			if (taskConfIter != dayIter->second.m_stTask.end())
			{
				rewards.insert(rewards.end(), taskConfIter->second.m_aReward.begin(), taskConfIter->second.m_aReward.end());
			}
		}

		if (!rewards.empty())
		{
			const std::string sender = confIter->second.m_strSender;
			const std::string title = confIter->second.m_strSendTitle;
			const std::string content = confIter->second.m_strSendContent;

			auto beginIter = rewards.begin();
			auto endIter = beginIter;
			std::vector<CPackGameItem> sendRewards;

			do
			{
				beginIter = endIter;
				int index = 0;
				while ((index++ < 5) && (endIter != rewards.end()))
				{
					++endIter;
				}
				sendRewards.assign(beginIter, endIter);

                CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin,
                                            pUserInfo->m_iGroupID,
                                            sender,
                                            title,
                                            content,
                                            sendRewards,
                                            "SEND_THEME_ACTIVITY_CHARGE_AWARD");
			} while (endIter != rewards.end());
		}
	}

	//1.清空所有任务状态
	for (auto &iter : pUserInfo->m_stThemeActivityChargeMap)
	{
		while (!iter.second.Empty())
		{
			iter.second.Erase(iter.second.Begin());
		}
	}
	pUserInfo->m_stThemeActivityChargeMap.clear();

	//2.遍历配置表，初始化所有任务
    CLogicActive::InitNewThemeActivityCharge(pUserInfo);

	for (auto &iter : pUserInfo->m_stThemeActivityChargeMap)
	{
		CLogicUserNotifySystem::NotifyThemeActivityChargeData(pUserInfo, iter.first);
	}
}

void CLogicDayRefreshSystem::RefreshUserIBusyInfo(user_data_table_ptr_type pUserInfo)
{
	int nowTime = CLogicCommon::GetSec();
	auto confIter = CLogicConfig::GetIBusyConfig().m_mpActivities.begin();
	for (; confIter != CLogicConfig::GetIBusyConfig().m_mpActivities.end(); ++confIter)
	{
		if ((nowTime >= confIter->second.m_iBeginTime) && (nowTime < confIter->second.m_iEndTime))
			break;
	}

	//1.活动结束
	if (confIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
	{
		if (pUserInfo->m_stIBusyData.GetActivityID() != 0)
		{
			pUserInfo->m_stIBusyData.SetActivityID(0);

			CLogicUserNotifySystem::NotifyIBusyBaseData(pUserInfo);			
		}
	}
	else
	{
		//新一期活动
		if (pUserInfo->m_stIBusyData.GetActivityID() != confIter->first)
		{
			pUserInfo->m_stIBusyData.SetActivityID(confIter->first);
			pUserInfo->m_stIBusyData.SetLevel(1);
			pUserInfo->m_stIBusyData.SetExp(0);
            pUserInfo->m_stIBusyData.SetDailyExpAward(0);
			pUserInfo->m_stIBusyData.SetScore(0);
			// pUserInfo->m_stIBusyData.SetHuoyue(0);
			pUserInfo->m_stIBusyData.SetTodayHuoyue(0);
			pUserInfo->m_stIBusyData.SetChangeTaskCount(0);
			pUserInfo->m_stIBusyData.SetFreeChangeTaskCount(0);
			pUserInfo->m_stIBusyData.SetMarketLastRefreshTime(0);
			pUserInfo->m_stIBusyData.SetMarketRefreshTimes(0);
			pUserInfo->m_stIBusyData.SetGettedLevelReward({});
			pUserInfo->m_stIBusyData.SetGettedDepositReward({});
			CLogicActive::RefreshIBusyTaskData(pUserInfo);

			CLogicUserNotifySystem::NotifyIBusyData(pUserInfo);

			CLogicUserStat::ResetRechargeCount(pUserInfo, confIter->second.m_iDepositID);
		}
		//活动内的新一天
		else
		{
		    pUserInfo->m_stIBusyData.SetDailyExpAward(0);
			pUserInfo->m_stIBusyData.SetTodayHuoyue(0);
			pUserInfo->m_stIBusyData.SetChangeTaskCount(0);
			pUserInfo->m_stIBusyData.SetMarketRefreshTimes(0);
			CLogicUserNotifySystem::NotifyIBusyBaseData(pUserInfo);
		}
	}
}

void CLogicDayRefreshSystem::RefreshUserScoreLevelInfo(user_data_table_ptr_type pUserInfo)
{
	const auto open_activity_id = CLogicActive::GetScoreLevelOpenActivityID();
	std::vector<int32_t> delete_activity_vec;
	for (auto iter_activity = pUserInfo->m_stScoreLevelMap.Begin(); iter_activity != pUserInfo->m_stScoreLevelMap.End(); ++iter_activity)
	{
        if (open_activity_id != iter_activity->first)
        {
            delete_activity_vec.push_back(iter_activity->first);
            const auto *config = CLogicConfig::GetScoreLevelConfig().GetActivityConfig(iter_activity->first);
            if (nullptr != config)
            {
                bool have_buy_deposit = CLogicUserStat::HaveBuyDepositID(pUserInfo, config->m_iScoreDepositID);
                for (const auto &award : config->m_stRewardMap)
                {
                    if (award.first > 0 && award.first < 60 &&
                        iter_activity->second->GetScore() >= award.second.m_iTarget)
                    {
                        const uint64_t flag = 1ULL << award.first;
                        if ((iter_activity->second->GetScoreAwardBitmap() & flag) == 0)
                        {
                            iter_activity->second->AddScoreAwardBitmap(flag);
                            CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin,
                                                        pUserInfo->m_iGroupID,
                                                        config->m_strScoreMailSender,
                                                        config->m_strScoreMailTitle,
                                                        config->m_strScoreMailContent,
                                                        award.second.m_stBonusItemVec,
                                                        "SEND_SCORE_LEVEL_SCORE_AWARD");
                        }

                        if (have_buy_deposit && (iter_activity->second->GetDepositAwardBitmap() & flag) == 0)
                        {
                            iter_activity->second->AddDepositAwardBitmap(flag);
                            CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin,
                                                        pUserInfo->m_iGroupID,
                                                        config->m_strScoreMailSender,
                                                        config->m_strScoreMailTitle,
                                                        config->m_strScoreMailContent,
                                                        award.second.m_stDepositItemVec,
                                                        "SEND_SCORE_LEVEL_DEPOSIT_AWARD");
                        }
                    }
                }
            }
        }
        else
        {
            iter_activity->second->SetBuyXingdongliCount(0);
            CLogicUserNotifySystem::NotifyScoreLevelActivityData(pUserInfo);
        }
	}

	while (!delete_activity_vec.empty())
	{
		const int32_t old_activity_id = delete_activity_vec.back();
		delete_activity_vec.pop_back();
		pUserInfo->m_stScoreLevelMap.Erase(old_activity_id);
	}

	if (open_activity_id > 0 && pUserInfo->m_stScoreLevelMap.Find(open_activity_id) == pUserInfo->m_stScoreLevelMap.End())
	{
		CLogicActive::InitNewScoreLevelActivity(pUserInfo);
		CLogicUserNotifySystem::NotifyScoreLevelActivityData(pUserInfo);
	}
}

void CLogicDayRefreshSystem::RefreshUserDaySingleRechargeInfo(user_data_table_ptr_type pUserInfo)
{
	std::vector<decltype(pUserInfo->m_stDaySingleRechargeMap)::key_type> deleteActivity;
	for (auto iter = pUserInfo->m_stDaySingleRechargeMap.Begin(); iter != pUserInfo->m_stDaySingleRechargeMap.End(); ++iter)
	{
		if (iter->second->GetCanRewardCount() > 0)
		{
			const int32_t iActivityID = iter->second->m_stData.m_iActivityID;
			const int32_t iTaskID = iter->second->m_stData.m_iTaskID;
			const auto* pstActivityConfig = CLogicConfig::GetDaySingleRechargeConfig().GetActivityConfig(iActivityID);
			const auto* pstTaskConfig = CLogicConfig::GetDaySingleRechargeConfig().GetActivityTaskConfig(iActivityID, iTaskID);
			if (nullptr != pstActivityConfig && nullptr != pstTaskConfig)
			{
				while (iter->second->GetRewardCount() < pstTaskConfig->m_iTaskNum && iter->second->GetCanRewardCount() > 0)
				{
                    CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID,
                                                pstActivityConfig->m_strSender, pstActivityConfig->m_strSendTitle,
                                                pstActivityConfig->m_strSendContent, pstTaskConfig->m_stBonusItemVec,
                                                "SEND_DAY_SINGLE_RECHARGE_MAIL_" + std::to_string(iActivityID) + "_" +
                                                std::to_string(iTaskID));

					iter->second->AddCanRewardCount(-1);
					iter->second->AddRewardCount(1);
				}
			}
		}

		if (CLogicActive::IsActivityDaySingleRechargeOpen(pUserInfo, iter->second->m_stData.m_iActivityID))
		{
			iter->second->SetCanRewardCount(0);
			iter->second->SetRewardCount(0);
		}
		else
		{
			deleteActivity.push_back(iter->first);
		}
	}

	//旧活动清空
	while (!deleteActivity.empty())
	{
		pUserInfo->m_stDaySingleRechargeMap.Erase(deleteActivity.back());
		deleteActivity.pop_back();
	}

}

void CLogicDayRefreshSystem::RefreshMonthPassInfo(user_data_table_ptr_type pUserInfo)
{
    const auto& config = CLogicConfig::GetMonthPassConfig();

    const int32_t iCreateTime = std::max(pUserInfo->m_stUserInfoTable.GetCreateTime(), config.m_iCustomCreateTime);
    const int32_t iCreateDay = day_diff(iCreateTime, CLogicCommon::GetSec()) + 1;
    if (iCreateDay >= config.m_iStartDay && iCreateDay <= config.m_iEndDay)
    {
        const int32_t iActivityDay = iCreateDay - config.m_iStartDay + 1;
        const int32_t iLastActivityDay = pUserInfo->m_stIBusyData.GetMonthPassDay();

        pUserInfo->m_stIBusyData.SetMonthPassDay(iActivityDay);
        pUserInfo->m_stIBusyData.SetMonthPassWishEventID(0);

        auto& UserMonthPassDailyTask = pUserInfo->m_stMonthPassTaskMap[CLogicConfigDefine::MONTH_PASS_TYPE_DAILY_TASK];
        while (!UserMonthPassDailyTask.Empty())
        {
            UserMonthPassDailyTask.Erase(UserMonthPassDailyTask.Begin());
        }

        if ((iLastActivityDay - 1) / 7 != (iActivityDay - 1) / 7)
        {
            auto& UserMonthPassWeeklyTask = pUserInfo->m_stMonthPassTaskMap[CLogicConfigDefine::MONTH_PASS_TYPE_WEEKLY_TASK];
            while (!UserMonthPassWeeklyTask.Empty())
            {
                UserMonthPassWeeklyTask.Erase(UserMonthPassWeeklyTask.Begin());
            }
        }

        CLogicUserNotifySystem::NotifyMonthPass(pUserInfo);
    }

    if (iCreateDay > config.m_iEndDay && !pUserInfo->m_stIBusyData.GetMonthPassWishItemIndex().empty())
    {
        std::vector<CPackGameItem> stBonusItemVec;
        for (const auto& item_index : pUserInfo->m_stIBusyData.GetMonthPassWishItemIndex())
        {

            const auto* pstItemConfig = config.GetWishItemConfig(item_index);
            if (pstItemConfig)
            {
                const auto& stUserWishTaskMap = pUserInfo->m_stMonthPassTaskMap[CLogicConfigDefine::MONTH_PASS_TYPE_WISH];
                const auto iterWish = stUserWishTaskMap.Find(item_index);
                const auto iExtraItemNum = (iterWish == stUserWishTaskMap.End() ? 0 : iterWish->second->GetProgress());
                stBonusItemVec.emplace_back(pstItemConfig->m_stWishItem.m_iItemType
                        , pstItemConfig->m_stWishItem.m_iCardID
                        , pstItemConfig->m_stWishItem.m_iNum + iExtraItemNum);
            }

        }

        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin
                , pUserInfo->m_iGroupID
                , config.m_strWishMailSender
                , config.m_strWishMailTitle
                , config.m_strWishMailContent
                , stBonusItemVec
                , "SEND_MONTH_PASS_WISH_AWARD");
        pUserInfo->m_stIBusyData.SetMonthPassWishItemIndex({});

        CLogicCacheCommon::ClearUserFightReport(pUserInfo, CLogicConfigDefine::FIGHT_REPORT_MONTH_PASS);
    }
}

void CLogicDayRefreshSystem::RefreshUserDailyQuest(user_data_table_ptr_type pUserInfo)
{
    const auto& stIDList = CLogicConfig::GetQuestConfig().m_stDailyQuestList;
    for(auto id : stIDList)
    {
        auto iter = pUserInfo->m_stQuestMap.Find(id);
        if(iter == pUserInfo->m_stQuestMap.End()) continue;

        iter->second->SetState(CLogicConfigDefine::ECQS_Accepted);
        iter->second->SetParaIndex(0);
        iter->second->SetProgress(0);

        pUserInfo->m_stOnlyInCacheData.m_stCacheQuestIDSet.insert(id);
    }
}

void CLogicDayRefreshSystem::RefreshUserSpecialGiftInfo(user_data_table_ptr_type pUserInfo, int32_t iType)
{
    int32_t curTime = CLogicCommon::GetSec();

    CLogicConfigSpecialGiftParser::const_iterator configIter;
    CLogicConfigSpecialGiftParser::const_iterator endIter;
    int32_t iMyActID = 0;
    int32_t iMyLevel = 0;
    if(iType == CLogicConfigDefine::ESGT_Week)
    {
        configIter = CLogicConfig::GetSpecialGiftConfig().m_stSpecialGiftWeekMap.begin();
        endIter = CLogicConfig::GetSpecialGiftConfig().m_stSpecialGiftWeekMap.end();

        iMyActID = pUserInfo->m_stIBusyData.GetSpecialGiftWeekID();
    }
    else
    {
        configIter = CLogicConfig::GetSpecialGiftConfig().m_stSpecialGiftMonthMap.begin();
        endIter = CLogicConfig::GetSpecialGiftConfig().m_stSpecialGiftMonthMap.end();

        iMyActID = pUserInfo->m_stIBusyData.GetSpecialGiftMonthID();
    }

    for (; configIter != endIter; ++configIter)
    {
        const TLogicActivityRefElem* pRefElem = &configIter->second.m_stActivityRef;
        if(pRefElem->m_iLevel <= pUserInfo->m_stUserInfoTable.GetLevel()
            && CLogicActive::IsWithinActivityTime(pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_NONE, curTime, pRefElem->m_iTimeType
                    , pRefElem->m_iStartDay, pRefElem->m_iEndDay, pRefElem->m_iServerDay, pRefElem->m_iStartTime, pRefElem->m_iEndTime))
            break;
    }

    // 活动结束或者没有活动开启
    if(configIter == endIter)
    {
        if(iMyActID > 0)
        {
            if(iType == CLogicConfigDefine::ESGT_Week)
            {
                pUserInfo->m_stIBusyData.SetSpecialGiftWeekID(0);
                pUserInfo->m_stIBusyData.SetSpecialGiftWeekLevel(0);
            }
            else
            {
                pUserInfo->m_stIBusyData.SetSpecialGiftMonthID(0);
                pUserInfo->m_stIBusyData.SetSpecialGiftMonthLevel(0);
            }
        }
    }
    else
    {
        // 新一期活动
        if(iMyActID != configIter->first)
        {
            auto iter = configIter->second.m_stInfoMap.begin();
            for (; iter != configIter->second.m_stInfoMap.end(); ++iter)
            {
                if(pUserInfo->m_stUserInfoTable.GetLevel() < iter->first)
                    break;
            }

            if(iter == configIter->second.m_stInfoMap.begin())
            {
                iMyActID = 0;
                iMyLevel = 0;
            }
            else
            {
                --iter;
                iMyActID = configIter->first;
                iMyLevel = iter->first;
            }

            if(iType == CLogicConfigDefine::ESGT_Week)
            {
                pUserInfo->m_stIBusyData.SetSpecialGiftWeekID(iMyActID);
                pUserInfo->m_stIBusyData.SetSpecialGiftWeekLevel(iMyLevel);
            }
            else
            {
                pUserInfo->m_stIBusyData.SetSpecialGiftMonthID(iMyActID);
                pUserInfo->m_stIBusyData.SetSpecialGiftMonthLevel(iMyLevel);
            }

            // 刷新Deposit次数
            if(iMyActID > 0)
            {
                for (const auto& deposit_id : iter->second.m_stDepositIDList)
                {
                    CLogicUserStat::ResetRechargeCount(pUserInfo, deposit_id);
                }
            }
        }
    }
}

bool CLogicDayRefreshSystem::MergeChangeRoleNick(user_data_table_ptr_type pUserInfo)
{
	auto vtNickData = CRecordServerNickTable::SelectDataFromSQL(pUserInfo->m_iUin, pUserInfo->m_iGroupID);
	if (!vtNickData.empty())
	{
		return true;
	}

	if (CLogicCacheCommon::ChangeRoleNick(pUserInfo, pUserInfo->m_strNick, CLogicSQLTableRecordBase::INSERT))
	{
		return true;
	}

	int iInputNickDisplayWidth = 0;
	if (LOGIC_SERVICE_API_INS->CheckNickCorrect(pUserInfo->m_strNick, iInputNickDisplayWidth) != 0)
	{
		return false;
	}

	if (iInputNickDisplayWidth < 10 && pUserInfo->m_strNick.find(CLogicConfig::GetGlobalConfig().m_strChangeNickStr) == std::string::npos)
	{
		for (int i = 0; i < 10; ++i)
		{
			const std::string strNick = pUserInfo->m_strNick + char(CLogicCommon::GetRandNum('a', 'z')) + char(CLogicCommon::GetRandNum('a', 'z'));
			if (CLogicCacheCommon::ChangeRoleNick(pUserInfo, strNick, CLogicSQLTableRecordBase::INSERT))
			{
				pUserInfo->m_stUserInfoTable.SetChangeNickCount(0);
				return true;
			}
		}
	}

	const std::string strNick = CLogicConfig::GetGlobalConfig().m_strChangeNickStr + std::to_string(pUserInfo->m_iUin);
	if (CLogicCacheCommon::ChangeRoleNick(pUserInfo, strNick, CLogicSQLTableRecordBase::INSERT))
	{
		pUserInfo->m_stUserInfoTable.SetChangeNickCount(0);
		return true;
	}

	return false;
}

void CLogicDayRefreshSystem::RefreshMergeActivity(user_data_table_ptr_type pUserInfo)
{
	for (auto iter = pUserInfo->m_stMergeActivityMap.Begin(); iter != pUserInfo->m_stMergeActivityMap.End(); ++iter)
	{
		iter->second->SetProgress(0);
		iter->second->SetAward(0);
	}

	for (const auto& deposit : CLogicConfig::GetDepositConfig().m_stDepositConfig)
    {
	    if (deposit.second.m_iType == CLogicConfigDefine::DEPOSIT_TYPE_MERGE_GIFT)
        {
	        CLogicUserStat::ResetRechargeCount(pUserInfo, deposit.first);
        }
    }

	CLogicUserNotifySystem::NotifyMergeActivity(pUserInfo);
}

void CLogicDayRefreshSystem::LazyDeleteActivityInfo(user_data_table_ptr_type pUserInfo)
{
	//删除未配置的活动
	std::vector<int32_t> activity;
	for (auto iter = pUserInfo->m_stUserActiveMap.Begin(); iter != pUserInfo->m_stUserActiveMap.End(); ++iter)
	{
		if (CLogicConfig::GetActivityConfig().m_stActivityRef.find(iter->first) == CLogicConfig::GetActivityConfig().m_stActivityRef.end())
		{
			activity.push_back(iter->first);
		}
	}

	while (!activity.empty())
	{
		pUserInfo->m_stUserActiveMap.Erase(activity.back());
		activity.pop_back();
	}
}

void CLogicDayRefreshSystem::LazyUserTaskStatus(user_data_table_ptr_type pUserInfo)
{
	CLogicDailyTaskSystem::SetProgress(pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_CHARM, pUserInfo->m_stUserInfoTable.GetCharm());

	CLogicDailyTaskSystem::SetProgress(pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_TOWER_MAX, pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder());

	CLogicDailyTaskSystem::SetProgress(pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_MATCH_PVP_LEVEL, CLogicConfig::GetMatchPvpConfig().GetMatchLevelByScore(pUserInfo->m_stUserInfoTable.GetMatchMaxScore()));

	int32_t iHaveFashionCount = 0;
	for (auto iter = pUserInfo->m_stFashionInfoMap.Begin(); iter != pUserInfo->m_stFashionInfoMap.End(); ++iter)
	{
		if (iter->second->GetExpiredTime() == 0)
		{
			++iHaveFashionCount;
		}
	}
	CLogicDailyTaskSystem::SetProgress(pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_FASHION_COUNT, iHaveFashionCount);

	int32_t iHaveHonorCount = 0;
	for (auto iter = pUserInfo->m_stUserHonorMap.Begin(); iter != pUserInfo->m_stUserHonorMap.End(); ++iter)
	{
		if (iter->second->GetExpireTime() == 0 && iter->second->GetHonorStatus() != CLogicConfigDefine::HONOR_STATUS_NONE)
		{
			++iHaveHonorCount;
		}
	}
	CLogicDailyTaskSystem::SetProgress(pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_HONOR_COUNT, iHaveHonorCount);

	CLogicDailyTaskSystem::SetProgress(pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_ENTIRE_MILITARK_LEVEL, pUserInfo->m_stUserInfoTable.GetEntireWarMilitarkLevel());

	int32_t iHaveCardCount = 0;
	CLogicCacheCardOperation stCacheCardOP(pUserInfo);
	for (auto stIter = pUserInfo->m_stHeroCardTableMap.Begin(); stIter != pUserInfo->m_stHeroCardTableMap.End(); ++stIter)
	{
		if (stCacheCardOP.CheckHeroCardExist(stIter->second))
		{
			++iHaveCardCount;
			CLogicDailyTaskSystem::SetProgress(pUserInfo, CLogicDailyTaskSystem::HERO_STAR, stIter->first, stIter->second->GetCardStar());
		}
	}
	CLogicDailyTaskSystem::SetProgress(pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_CARD_COUNT, iHaveCardCount);
}

void CLogicDayRefreshSystem::RefreshUserNewPassInfo(user_data_table_ptr_type pUserInfo)
{
    const auto& newPassConfig = CLogicConfig::GetNewPassConfig();
    if (newPassConfig.m_iActID > 0)
    {
        //活动开启刷新每日任务进度
        if (CLogicActive::IsAvyOpen(newPassConfig.m_iActID, pUserInfo->m_stUserInfoTable.GetCreateTime()))
        {
            const auto& stIDList = CLogicConfig::GetQuestConfig().m_stPassDailyQuestList;
            for (auto id : stIDList)
            {
                auto iter = pUserInfo->m_stQuestMap.Find(id);
                if (iter == pUserInfo->m_stQuestMap.End())
                    continue;

                iter->second->SetState(CLogicConfigDefine::ECQS_Accepted);
                iter->second->SetParaIndex(0);
                iter->second->SetProgress(0);

                pUserInfo->m_stOnlyInCacheData.m_stCacheQuestIDSet.insert(id);
            }
        }
        else//通行证活动结束邮寄奖励，清空活动数据
        {
            if (pUserInfo->m_stUserInfoTable.GetNewPassTotalExp() > 0)
            {
                std::vector<CPackGameItem> stBonusItemVec;
                int32_t currNewPassLevel = CLogicConfig::GetNewPassConfig().GetNewPassLevelByExp(pUserInfo->m_stUserInfoTable.GetNewPassTotalExp());
                auto normalAccepted = pUserInfo->m_stUserInfoTable.GetNewPassNormalLevel();
                auto advancedAccepted = pUserInfo->m_stUserInfoTable.GetNewPassAdvancedLevel();

                auto depositIter = pUserInfo->m_stUserDepositMap.Find(CLogicConfig::GetNewPassConfig().m_iDepositID);
                bool buyNewPass = CLogicNewPassProcessor::BuyWhichMonthPass(pUserInfo) != 0;

                bool hasReceived = false;
                for (int32_t idx = 1; idx <= currNewPassLevel && currNewPassLevel > 0; idx++)
                {
                    hasReceived = false;
                    for (auto acceptedLevel : normalAccepted)
                    {
                        if (idx == acceptedLevel)
                        {
                            hasReceived = true;
                            break;
                        }
                    }
                    if (!hasReceived)
                    {
                        auto rewardConfigIter = CLogicConfig::GetNewPassConfig().GetNormalRewardByLevel(idx);
                        if (nullptr == rewardConfigIter)
                            continue;
                        stBonusItemVec.insert(stBonusItemVec.end(),rewardConfigIter->m_stReward.begin(),rewardConfigIter->m_stReward.end());
                    }

                    if (buyNewPass)
                    {
                        hasReceived = false;
                        for (auto acceptedLevel : advancedAccepted)
                        {
                            if (idx == acceptedLevel)
                            {
                                hasReceived = true;
                                break;
                            }
                        }
                        if (!hasReceived)
                        {
                            auto rewardConfigIter = CLogicConfig::GetNewPassConfig().GetAdvancedRewardByLevel(idx);
                            if (nullptr == rewardConfigIter)
                                continue;
                            stBonusItemVec.insert(stBonusItemVec.end(),rewardConfigIter->m_stReward.begin(),rewardConfigIter->m_stReward.end());
                        }
                    }
                }

                CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin,
                                               pUserInfo->m_iGroupID,
                                               newPassConfig.m_strSender,
                                               newPassConfig.m_strTitle,
                                               newPassConfig.m_strContent,
                                               stBonusItemVec,
                                               "SEND_NEW_PASS_REWARD");

                //清空活动数据
                pUserInfo->m_stUserInfoTable.SetNewPassNormalLevel({});
                pUserInfo->m_stUserInfoTable.SetNewPassAdvancedLevel({});
                pUserInfo->m_stUserInfoTable.SetNewPassTotalExp(0);

                //清空通行证购买数据
                auto upGradeDepositIter = pUserInfo->m_stUserDepositMap.Find(CLogicConfig::GetNewPassConfig().m_iUpDepositID);
                auto advDepositIter = pUserInfo->m_stUserDepositMap.Find(CLogicConfig::GetNewPassConfig().m_iAdvancedDepositID);
                if (depositIter != pUserInfo->m_stUserDepositMap.End())
                {
                    depositIter->second->SetBuyTimes(0);
                }

                if (upGradeDepositIter != pUserInfo->m_stUserDepositMap.End())
                {
                    upGradeDepositIter->second->SetBuyTimes(0);
                }

                if (advDepositIter != pUserInfo->m_stUserDepositMap.End())
                {
                    advDepositIter->second->SetBuyTimes(0);
                }

                //清空相关任务进度
                const auto& stDailyIDList = CLogicConfig::GetQuestConfig().m_stPassDailyQuestList;
                const auto& stWeeklyIDList = CLogicConfig::GetQuestConfig().m_stPassDailyQuestList;
                const auto& stMonthlyIDList = CLogicConfig::GetQuestConfig().m_stPassDailyQuestList;
                auto clearTaskFun = [&pUserInfo](const vector<int>& stIDList) -> void
                {
                    for (auto id : stIDList)
                    {
                        auto iter = pUserInfo->m_stQuestMap.Find(id);
                        if (iter == pUserInfo->m_stQuestMap.End())
                            continue;

                        iter->second->SetState(CLogicConfigDefine::ECQS_Accepted);
                        iter->second->SetParaIndex(0);
                        iter->second->SetProgress(0);

                        pUserInfo->m_stOnlyInCacheData.m_stCacheQuestIDSet.insert(id);
                    }
                };
                clearTaskFun(stDailyIDList);
                clearTaskFun(stWeeklyIDList);
                clearTaskFun(stMonthlyIDList);
            }
        }
    }
}

void CLogicDayRefreshSystem::RefreshUserNewPassWeeklyInfo(user_data_table_ptr_type pUserInfo)
{
    //活动开启刷新每周任务进度
    const auto& newPassConfig = CLogicConfig::GetNewPassConfig();
    if (CLogicActive::IsAvyOpen(newPassConfig.m_iActID, pUserInfo->m_stUserInfoTable.GetCreateTime()))
    {
        const auto& stIDList = CLogicConfig::GetQuestConfig().m_stPassWeeklyQuestList;
        for (auto id : stIDList)
        {
            auto iter = pUserInfo->m_stQuestMap.Find(id);
            if (iter == pUserInfo->m_stQuestMap.End())
                continue;

            iter->second->SetState(CLogicConfigDefine::ECQS_Accepted);
            iter->second->SetParaIndex(0);
            iter->second->SetProgress(0);

            pUserInfo->m_stOnlyInCacheData.m_stCacheQuestIDSet.insert(id);
        }
    }
}

//刷新道馆每周任务
void CLogicDayRefreshSystem::RefreshPVPWeeklyInfo(user_data_table_ptr_type pUserInfo)
{
    const auto& stIDList = CLogicConfig::GetQuestConfig().m_stPvpWeeklyQuestList;
    for (auto id : stIDList)
    {
        auto iter = pUserInfo->m_stQuestMap.Find(id);
        if (iter == pUserInfo->m_stQuestMap.End())
            continue;

        iter->second->SetState(CLogicConfigDefine::ECQS_Accepted);
        iter->second->SetParaIndex(0);
        iter->second->SetProgress(0);

        //清空玩家随机到的每周任务
        pUserInfo->m_stUserInfoTable.SetPvpWeeklyTask({});
        pUserInfo->m_stOnlyInCacheData.m_stCacheQuestIDSet.insert(id);
    }
}

//刷新道馆每周奖励
void CLogicDayRefreshSystem::RefreshPVPWeeklyReward(user_data_table_ptr_type pUserInfo)
{
    //恢复默认头像框
    auto headBorder = pUserInfo->m_stUserInfoTable.GetAvatarBorder();
    if (headBorder == EnumPvpRewardHeadBorder::EnumPvpHeadBorderFirst ||
        headBorder == EnumPvpRewardHeadBorder::EnumPvpHeadBorderSecond ||
        headBorder == EnumPvpRewardHeadBorder::EnumPvpHeadBorderThird)
    {
        pUserInfo->m_stUserInfoTable.SetAvatarBorder(EnumPvpRewardHeadBorder::EnumDefaultBorder);
    }

    //清除指定头像框
    auto clearTargetAvatarFunc = [&](int32_t avatarID) {
        auto targetIter = pUserInfo->m_stUserAvatarMap.Find(avatarID);
        if (targetIter != pUserInfo->m_stUserAvatarMap.End())
        {
            pUserInfo->m_stUserAvatarMap.Erase(targetIter);
        }
    };

    clearTargetAvatarFunc(EnumPvpRewardHeadBorder::EnumPvpHeadBorderFirst);
    clearTargetAvatarFunc(EnumPvpRewardHeadBorder::EnumPvpHeadBorderSecond);
    clearTargetAvatarFunc(EnumPvpRewardHeadBorder::EnumPvpHeadBorderThird);

    if(pUserInfo->m_stUserInfoTable.GetPvpFightTimes() <= 0)
    {
        return;
    }
    int32_t pvpScore = pUserInfo->m_stUserInfoTable.GetPvpScore();
    for(auto pvpReward: CLogicConfig::GetPVPConfig().m_stWeeklyReward)
    {
        if (pvpReward.second.m_iMin <= pvpScore && pvpScore <= pvpReward.second.m_iMax)
        {
            CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID
                                           , pvpReward.second.m_strMailSender, pvpReward.second.m_strMailTitle
                                           , pvpReward.second.m_strMailContent, pvpReward.second.m_stWeeklyReward
                                           , "SEND_RANK_WEEKLY_AWARD");
        }
    }

    //数据打点上报
    CDataReportRewardPVP params;
    params.m_iUin = pUserInfo->m_iUin;
    params.m_iRankScore = pUserInfo->m_stUserInfoTable.GetPvpScore();
    params.m_stCommon.m_strEven = "tga_reward_pvp";
    params.m_stCommon.m_strAccount = pUserInfo->m_strPid;
    params.m_stCommon.m_strUdid = pUserInfo->m_strDeviceID;
    params.m_stCommon.m_strNickName = pUserInfo->m_strNick;

    CMsgHeadCross stMsgHead;
    stMsgHead.m_byCmdType = enmCmdType_request;
    stMsgHead.m_usCmd = SERVER_CMD_DATA_REPORT_DEFINE::MSG_REPORT_REWARD_PVP;
    stMsgHead.m_iUin = pUserInfo->m_iUin;
    stMsgHead.m_nGroupID = pUserInfo->m_iGroupID;
    LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stMsgHead, params);

    pUserInfo->m_stUserInfoTable.SetPvpScore(1200);
    pUserInfo->m_stUserInfoTable.SetPvpFightTimes(0);

}


void CLogicDayRefreshSystem::RefreshLimitBoss(user_data_table_ptr_type pUserInfo)
{
    const auto& limitBossConfig = CLogicConfig::GetLimitBossConfig();
    if (limitBossConfig.m_iAcyID > 0 && !CLogicActive::IsAvyOpen(limitBossConfig.m_iAcyID, pUserInfo->m_stUserInfoTable.GetCreateTime()))
    {
        //清空活动数据
        if (!pUserInfo->m_stUserInfoTable.GetFindLimitBoss().empty())
        {
            pUserInfo->m_stIBusyData.SetNewLimitBossReward({});
            pUserInfo->m_stUserInfoTable.SetFindLimitBoss({});
            pUserInfo->m_stUserInfoTable.SetLimitBossTotalPoint(0);
            pUserInfo->m_stIBusyData.SetNewLimitBossDailyCount(0);
            pUserInfo->m_stIBusyData.SetLimitBossBaodiTime(0);
            pUserInfo->m_stOnlyInCacheData.m_bHasLimitBoss = false;
        }
    }
}

void CLogicDayRefreshSystem::RefreshLimitCommodityPT(user_data_table_ptr_type pUserInfo)
{
    for(auto iterBuy = pUserInfo->m_stUserBuyCommodityMap.Begin(); iterBuy != pUserInfo->m_stUserBuyCommodityMap.End(); iterBuy++)
    {
        auto mallConfig = CLogicConfig::GetMallConfig().GetConfig(iterBuy->first);
        auto ptCount = iterBuy->second->GetPt();

        if (nullptr == mallConfig || mallConfig->m_iAvyID <= 0 ||
            CLogicActive::IsAvyOpen(mallConfig->m_iAvyID, pUserInfo->m_stUserInfoTable.GetCreateTime()) ||
            ptCount == 0)
        {
            continue;
        }

        auto ptRewardConfig = CLogicConfig::GetAwardConfig().m_stPTRewardConfig;
        for (auto rewardConfig = ptRewardConfig.m_stReward.begin(); rewardConfig != ptRewardConfig.m_stReward.end(); rewardConfig++)
        {
            rewardConfig->m_iNum *= ptCount;
        }

        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin,
                                       pUserInfo->m_iGroupID,
                                       ptRewardConfig.m_strSender,
                                       ptRewardConfig.m_strTitle,
                                       ptRewardConfig.m_strContent+mallConfig->m_strName+ptRewardConfig.m_strAfterContent,
                                       ptRewardConfig.m_stReward,
                                       "SEND_LIMIT_PT_REWARD");
        iterBuy->second->SetPt(0);
    }
}


void CLogicDayRefreshSystem::RefreshGuildReward(user_data_table_ptr_type pUserInfo)
{
    pUserInfo->m_stIBusyData.SetDailyGuildDonate(0);
    pUserInfo->m_stIBusyData.SetDailyGuildSign(0);
    pUserInfo->m_stIBusyData.SetDailyGuildRandom({});
    pUserInfo->m_stIBusyData.SetDailyGuildRandomCount(0);
    pUserInfo->m_stIBusyData.SetDailyGuildBar(0);
    pUserInfo->m_stIBusyData.SetConstelAdvTimes(0);
    pUserInfo->m_stIBusyData.SetNewLimitBossDailyCount(0);
    //清空社团祈愿记录
    pUserInfo->m_stIBusyData.SetGuildFragmentHistory({});
    //重置社团工会战次数
    pUserInfo->m_stIBusyData.SetDailyGuildWarTime(0);
    pUserInfo->m_stIBusyData.SetGuildWarUseCard({});

    //检查玩家上次参与的公会战是否结束,发送排名奖励
    //(上次参与过 && 周期结束) && (对应的结算周期到达||下次周期开始)
    int32_t iNowSec = CLogicCommon::GetSec();
    if (pUserInfo->m_stIBusyData.GetGuildWarCalInfo().m_iGuildWarEndTime != 0 &&
        iNowSec > pUserInfo->m_stIBusyData.GetGuildWarCalInfo().m_iGuildWarEndTime &&
         (iNowSec > CLogicConfig::GetGuildConfig().m_iGuildWarCalTime ||
         CLogicConfig::GetGuildConfig().m_iGuildWarStartTime > pUserInfo->m_stIBusyData.GetGuildWarCalInfo().m_iGuildWarEndTime))
    {
        auto stGuildWarRankConfig = CLogicConfig::GetGuildConfig().GetGuildWarRankRewardConfig(pUserInfo->m_stIBusyData.GetGuildWarCalInfo().m_iRank);
        std::string strContent;
        if (pUserInfo->m_stIBusyData.GetGuildWarCalInfo().m_iRank == 0)
        {
            strContent = stGuildWarRankConfig->m_strGuildWarContent;
        } else
        {
            strContent = stGuildWarRankConfig->m_strGuildWarContent+to_string(pUserInfo->m_stIBusyData.GetGuildWarCalInfo().m_iRank)+stGuildWarRankConfig->m_strGuildWarAfterContent;
        }
        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin,
                                       pUserInfo->m_iGroupID,
                                       stGuildWarRankConfig->m_strGuildWarSender,
                                       stGuildWarRankConfig->m_strGuildWarTitle,
                                       strContent,
                                       stGuildWarRankConfig->m_stReward,
                                       "SEND_GUILD_WAR_REWARD");

        //清除玩家上次公会战记录
        pUserInfo->m_stIBusyData.SetGuildWarCalInfo({});
        pUserInfo->m_stIBusyData.SetGuildSelfInfo({});
    }

}


void CLogicDayRefreshSystem::RefreshHomeInfo(user_data_table_ptr_type pUserInfo)
{
    //判断是否存在指定的家园buff
    auto userHomeBuff = pUserInfo->m_stIBusyData.GetHomeBuffID();
    auto hotSpringIter = userHomeBuff.find(EnumHomeGoodBuffType::HomeGoodBuff_HotSpring);
    auto hasHotSpringBuff = (hotSpringIter != userHomeBuff.end());

    //参与温泉但是没有领取奖励
    auto homeFuncConfig = CLogicConfig::GetHomeFuncConfig();
    if (pUserInfo->m_stUserInfoTable.GetHotSpringJoin() == 1 && pUserInfo->m_stUserInfoTable.GetHotSpringReward() == 0)
    {
        CPackGameItem gameItem;
        gameItem.m_iItemType = CLogicConfigDefine::CURRENCY;
        gameItem.m_iCardID = CLogicConfigDefine::ECIT_EnergyID;

        auto baseNum = homeFuncConfig.m_iHotSpringSelfReward;
        gameItem.m_iNum = !hasHotSpringBuff ?  baseNum : (int32_t)(baseNum * (1+hotSpringIter->second.m_fParam1/10000));

        //检查公会buff
        auto guildBuffIter = pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.find(EnumGuildBuffType::GuildBuffHotSpring);
        if (guildBuffIter != pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.end())
        {
            gameItem.m_iNum += guildBuffIter->second.m_iBuffParam[0];
        }

        std::vector<CPackGameItem> stReward;
        stReward.emplace_back(gameItem);
        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID,
                                       homeFuncConfig.m_strHotSpringEmailSenderSelf,
                                       homeFuncConfig.m_strHotSpringEmailTitleSelf,
                                       homeFuncConfig.m_strHotSpringEmailContentSelf,
                                       stReward, "SEND_HOT_SPRINT_JOIN",1,
                                       false,homeFuncConfig.m_iExpiredTimeDay);
    }
    //被他人邀请但是没有领取奖励
    if (pUserInfo->m_stUserInfoTable.GetHotSpringInvite() == 1 && pUserInfo->m_stUserInfoTable.GetHotSpringRewardOther() == 0)
    {
        CPackGameItem gameItem;
        gameItem.m_iItemType = CLogicConfigDefine::CURRENCY;
        gameItem.m_iCardID = CLogicConfigDefine::ECIT_EnergyID;

        auto baseNum =  homeFuncConfig.m_iHotSpringFriendReward;
        gameItem.m_iNum = !hasHotSpringBuff ?  baseNum : (int32_t)(baseNum * (1+hotSpringIter->second.m_fParam1/10000));

        std::vector<CPackGameItem> stReward;
        stReward.emplace_back(gameItem);
        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID,
                                       homeFuncConfig.m_strHotSpringEmailSenderOther,
                                       homeFuncConfig.m_strHotSpringEmailTitleOther,
                                       homeFuncConfig.m_strHotSpringEmailContentOther,
                                       stReward, "SEND_HOT_SPRINT_INVITE",1,
                                       false,homeFuncConfig.m_iExpiredTimeDay);
    }

    pUserInfo->m_stUserInfoTable.SetHotSpringRewardEmail(0);
    pUserInfo->m_stUserInfoTable.SetHotSpringReward(0);
    pUserInfo->m_stUserInfoTable.SetHotSpringRewardOther(0);
    pUserInfo->m_stUserInfoTable.SetHotSpringInvite(0);
    pUserInfo->m_stUserInfoTable.SetHotSpringJoin(0);
}


int32_t CLogicDayRefreshSystem::GetCanBuyBackCount(user_data_table_ptr_type pUserInfo, CEnumBuyBackType type, int32_t iDailyCount, int32_t iPara)
{
	int32_t iAddCount = 0;
    /*
    const int32_t iCreateTime = pUserInfo->m_stUserInfoTable.GetCreateTime();
	if (type == CEnumBuyBackType::DINNER)
	{
		for (const auto& dinner : CLogicConfig::GetActivityConfig().m_stDinnerConfig)
		{
			const auto iterActivity = pUserInfo->m_stUserActiveMap.Find(dinner.first);
			if (iterActivity != pUserInfo->m_stUserActiveMap.End())
			{
				iAddCount = iDailyCount - bin1num(iterActivity->second->GetActiveReceivedBitmap());
				const int32_t iDayDiff = day_diff(std::max(iterActivity->second->GetActiveTime(), iCreateTime), CLogicCommon::GetSec());
				if (iDayDiff >= 1)
				{
					iAddCount += iDailyCount * (iDayDiff - 1);
					iterActivity->second->SetActiveCompleteBitmap(0);
					iterActivity->second->SetActiveReceivedBitmap(0);
					iterActivity->second->SetActiveTargetNum(0);
					iterActivity->second->SetActiveTime(CLogicCommon::GetSec());
				}
			}
		}
	}
	else if (type == CEnumBuyBackType::HORSE)
	{
		auto iterLevel = pUserInfo->m_stLevelInfoTableMap.Find(CLogicConfigDefine::PAO_MA_LEVEL_ID);
		if (iterLevel != pUserInfo->m_stLevelInfoTableMap.End())
		{
			iAddCount = iDailyCount - iterLevel->second->GetPlayTimes();
			const int32_t iDayDiff = day_diff(std::max(iterLevel->second->GetLastPlayTime(), iCreateTime), CLogicCommon::GetSec());
			if (iDayDiff >= 1)
			{
				iAddCount += iDailyCount * (iDayDiff - 1);
				iterLevel->second->SetPlayTimes(0);
				iterLevel->second->SetLastPlayTime(CLogicCommon::GetSec());
			}
		}
	}
	else if (type == CEnumBuyBackType::DIAOCHAN)
	{
		auto iterLevel = pUserInfo->m_stLevelInfoTableMap.Find(CLogicConfigDefine::DIAOCHAN_LEVEL_ID);
		if (iterLevel != pUserInfo->m_stLevelInfoTableMap.End())
		{
			iAddCount = iDailyCount - iterLevel->second->GetPlayTimes();
			const int32_t iDayDiff = day_diff(std::max(iterLevel->second->GetLastPlayTime(), iCreateTime), CLogicCommon::GetSec());
			if (iDayDiff >= 1)
			{
				iAddCount += iDailyCount * (iDayDiff - 1);
				iterLevel->second->SetPlayTimes(0);
				iterLevel->second->SetLastPlayTime(CLogicCommon::GetSec());
			}
		}
	}
	else if (type == CEnumBuyBackType::PVP_TERRITORY)
	{
	    if(iPara > 0)
        {
            auto iterLevel = pUserInfo->m_stLevelInfoTableMap.Find(iPara);
            if (iterLevel != pUserInfo->m_stLevelInfoTableMap.End() && iterLevel->second->GetTotalVictoryTimes() > 0)
            {
                iAddCount = std::max(0, iDailyCount - pUserInfo->m_stUserInfoTable.GetFightPvpTerritoryCount());
                const int32_t iLastTime = (pUserInfo->m_stUserInfoTable.GetPvpTerritoryLastRefreshTime() <= 0 ?
                        iterLevel->second->GetLastPlayTime() : pUserInfo->m_stUserInfoTable.GetPvpTerritoryLastRefreshTime());
                const int32_t iDayDiff = day_diff(iLastTime, CLogicCommon::GetSec());
                if (iDayDiff >= 1)
                {
                    iAddCount += iDailyCount * (iDayDiff - 1);
                    pUserInfo->m_stUserInfoTable.SetFightPvpTerritoryCount(0);
                    pUserInfo->m_stUserInfoTable.SetBuyPvpTerritoryCount(0);
                    pUserInfo->m_stUserInfoTable.SetPvpTerritoryLastRefreshTime(CLogicCommon::GetSec());
                }
            }
        }
	    else
        {
            iAddCount = std::max(0, iDailyCount - pUserInfo->m_stUserInfoTable.GetFightPvpTerritoryCount());
            const int32_t iLastTime = std::max(pUserInfo->m_stUserInfoTable.GetPvpTerritoryLastRefreshTime(), iCreateTime);
            const int32_t iDayDiff = day_diff(iLastTime, CLogicCommon::GetSec());
            if (iDayDiff >= 1)
            {
                iAddCount += iDailyCount * (iDayDiff - 1);
                pUserInfo->m_stUserInfoTable.SetFightPvpTerritoryCount(0);
                pUserInfo->m_stUserInfoTable.SetBuyPvpTerritoryCount(0);
                pUserInfo->m_stUserInfoTable.SetPvpTerritoryLastRefreshTime(CLogicCommon::GetSec());
            }
        }
	}
	else if (type == CEnumBuyBackType::SINGLE_LEVEL)
	{
		auto iterLevel = pUserInfo->m_stLevelInfoTableMap.Find(CLogicConfigDefine::SINGLE_LEVEL_ID);
		if (iterLevel != pUserInfo->m_stLevelInfoTableMap.End())
		{
			iAddCount = iDailyCount - iterLevel->second->GetPlayTimes();
			const int32_t iDayDiff = day_diff(std::max(iterLevel->second->GetLastPlayTime(), iCreateTime), CLogicCommon::GetSec());
			if (iDayDiff >= 1)
			{
				iAddCount += iDailyCount * (iDayDiff - 1);
				iterLevel->second->SetPlayTimes(0);
				iterLevel->second->SetLastPlayTime(CLogicCommon::GetSec());
			}
		}
	}
*/
	return std::max(0, iAddCount);
}

void CLogicDayRefreshSystem::RefreshWenWenChoice(user_data_table_ptr_type pUserInfo)
{
    auto choice = pUserInfo->m_stIBusyData.GetWenwenChoose();
    auto stRewardConfig = CLogicConfig::GetWenWenNewsConfig().GetWenWenNewsChooseConfig(choice);
    if (choice != 0 && stRewardConfig != nullptr)
    {
        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin,
                                       pUserInfo->m_iGroupID,
                                       stRewardConfig->m_iStEmail.m_strSender,
                                       stRewardConfig->m_iStEmail.m_strTitle,
                                       stRewardConfig->m_iStEmail.m_strContent,
                                       stRewardConfig->m_stReward,
                                       "SEND_WEN_WEN_NEWS");
    }

    pUserInfo->m_stIBusyData.SetWenwenChoose(0);
}

void CLogicDayRefreshSystem::RefreshRegisterAccount(user_data_table_ptr_type pUserInfo)
{
    if(pUserInfo->m_stIBusyData.GetCbtRewardTag() == 1) return;
    auto registerConfig = CLogicConfig::GetRegisterRewardConfig();
    auto iter = registerConfig.m_mapAccount.find(pUserInfo->m_strPid);
    if (iter == registerConfig.m_mapAccount.end())
    {
        return;
    }

    if (!CLogicActive::IsAvyOpen(registerConfig.m_iAvyID, pUserInfo->m_stUserInfoTable.GetCreateTime()))
    {
        return;
    }

    pUserInfo->m_stIBusyData.SetCbtRewardTag(1);
    CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin,
                                   pUserInfo->m_iGroupID,
                                   registerConfig.m_strSender,
                                   registerConfig.m_strTitle,
                                   registerConfig.m_strContent,
                                   registerConfig.m_stReward,
                                   "SEND_CBT_REGISTER");
}


void CLogicDayRefreshSystem::RefreshConstelShop(user_data_table_ptr_type pUserInfo)
{
    //刷新念写卡商店
    CLogicCacheCommon::RefreshConstelShop(pUserInfo);
    //清空每日刷新次数
    pUserInfo->m_stIBusyData.SetConstelShopRefreshCount(0);
}

void CLogicDayRefreshSystem::RefreshAvyInfo(user_data_table_ptr_type pUserInfo)
{
    auto gashaponConfig = CLogicConfig::GetGashaponConfig();
    if (!CLogicActive::IsGashaponOpen(pUserInfo))
    {
        return;
    }

    CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin,
                                   pUserInfo->m_iGroupID,
                                   gashaponConfig.m_strMailSender,
                                   gashaponConfig.m_strMailTitle,
                                   gashaponConfig.m_strMailContent,
                                   gashaponConfig.m_stMailReward,
                                   "SEND_AVY_EMAIL");
}

