#include "common_datetime.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_tactics_protocol.h"
#include "server_fashion_protocol.h"
#include "server_award_protocol.h"
#include "server_home_protocol.h"
#include "server_cross_protocol.h"
#include "server_active_protocol.h"
#include "server_game_item_protocol.h"
#include "server_user_protocol.h"
#include "server_role_protocol.h"
#include "server_ibusy_protocol.h"
#include "server_score_level_protocol.h"
#include "logic_link_common.h"
#include "logic_game_user_notify_system.h"
#include "logic_config.h"
#include "logic_game_active.h"
#include "logic_game_talk_common.h"
#include "logic_game_cache_common.h"
#include "logic_game_global_frame.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_global.h"
#include "logic_manager_limit_boss.h"

ILogicConnectorSender* CLogicUserNotifySystem::GetConnectorSender()
{
	return LOGIC_CONNECTOR_SENDER;
}

void CLogicUserNotifySystem::NotifyUserDataToClient(user_data_table_ptr_type pUserInfo)
{
	NotifyUserDailyData(pUserInfo);

    NotifyUserAllAvyInfo(pUserInfo, {});

	NotifyUserBuyBackInfo(pUserInfo);

	NotifyServerConfig(pUserInfo);

	NotifyShopCount(pUserInfo);

	NotifyUserLoginTalk(pUserInfo);

	NotifyFairPvpInfo(pUserInfo);

	NotifyMatchPvpInfo(pUserInfo);

	NotifyMergeActivity(pUserInfo);

	NotifyQiyuData(pUserInfo);

	if (pUserInfo->m_stThemeActivityChargeMap.empty())
	{
        CLogicActive::InitNewThemeActivityCharge(pUserInfo);
	}
	for (auto &iter : pUserInfo->m_stThemeActivityChargeMap)
	{
		NotifyThemeActivityChargeData(pUserInfo, iter.first);
	}

	if (pUserInfo->m_stIBusyData.GetActivityID() == 0)
	{
		CLogicActive::InitNewIBusyActivity(pUserInfo);
	}
	NotifyIBusyData(pUserInfo);

	NotifyScoreLevelActivityData(pUserInfo);

	NotifyDaySingleRechargeActivityInfo(pUserInfo);

	NotifyMonthPass(pUserInfo);

    // 更新BUFF并发送通知
    CLogicBuff::UpdateBuff(pUserInfo);
    NotifyUserBuffData(pUserInfo);

    // 限时BOSS通知
    for(auto boss_id : pUserInfo->m_stOnlyInCacheData.m_stLimitBossInvite)
    {
        auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(boss_id);
        if(nullptr != pBossData && pBossData->m_iDeathTime <= 0)
        {
            CResponseBody notify;
            GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_LIMIT_BOSS_INVITE, notify);

            break;
        }
    }
}

void CLogicUserNotifySystem::NotifyDayRefreshEvent(user_data_table_ptr_type pUserInfo)
{
	CResponseBody notify;
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_DAY_REFRESH_EVENT, notify);
}

void CLogicUserNotifySystem::NotifyHasNewMail(user_data_table_ptr_type pUserInfo)
{
	CResponseBody stNotify;
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_HAS_NEW_MAIL, stNotify);
}

void CLogicUserNotifySystem::NotifyUserDailyData(user_data_table_ptr_type pUserInfo)
{
	CResponseNotifyUserDailyData notify;
	notify.m_iFreeTurnTableCount = pUserInfo->m_stUserDailyTable.GetFreeTurnTableCount();
	notify.m_iLeftTurnCardCount = pUserInfo->m_stUserDailyTable.GetLeftTurnCardCount();
	notify.m_strTurnCardAwardBitmap = int2bin(pUserInfo->m_stUserDailyTable.GetTurnCardAwardBitmap());
	notify.m_iTurnOpenCardAmount = bin1num(pUserInfo->m_stUserDailyTable.GetTurnCardBitmap());
	notify.m_iMultiColorLanternVec = CLogicGlobal::GetUserSelectLanternVec(pUserInfo->m_iUin, pUserInfo->m_iGroupID);
	notify.m_iLastOpenLaternVec = CLogicGlobal::GetGlobalCache()->m_stRecoveryData.m_iLastMultiColorOpenVec;
	notify.m_strVipDailyAwardBitmap = int2bin(pUserInfo->m_stUserDailyTable.GetVipDailyAwardBitmap());
	notify.m_strRecommendedLineupBitmap = int2bin(pUserInfo->m_stUserInfoTable.GetRecommendedLineupBitmap());
	notify.m_iCountDownHero = pUserInfo->m_stUserDailyTable.GetFreeCountdownHeroTableCount();
	const auto stEntireWarGodKey = CLogicGlobal::GetEntireWarGodKey();
	notify.m_iWorshipWarGodCount = pUserInfo->m_stUserDailyTable.GetWorshipWarGodCount() + (stEntireWarGodKey.m_iUin == pUserInfo->m_iUin) + (stEntireWarGodKey.m_iUin == 0);
	notify.m_iBuyActionCoinCount = pUserInfo->m_stUserDailyTable.GetBuyActionCoinCount();
	notify.m_iCanJoinGuildTime = pUserInfo->m_stUserInfoTable.GetLastLeaveGuildTime() + CLogicConfig::GetGuildConfig().m_iJoinGuildCD;
	notify.m_iCharm = pUserInfo->m_stUserInfoTable.GetCharm();
	notify.m_iMergeLevelCount = pUserInfo->m_stUserDailyTable.GetMergeLevelCount();
	notify.m_iLuckyHammerLogCount = pUserInfo->m_stUserDailyTable.GetLuckyHammerLogCount();

    notify.m_iSpeGiftWeekActID  = pUserInfo->m_stIBusyData.GetSpecialGiftWeekID();
    notify.m_iSpeGiftWeekLevel  = pUserInfo->m_stIBusyData.GetSpecialGiftWeekLevel();
    notify.m_iSpeGiftMonthActID = pUserInfo->m_stIBusyData.GetSpecialGiftMonthID();
    notify.m_iSpeGiftMonthLevel = pUserInfo->m_stIBusyData.GetSpecialGiftMonthLevel();

    notify.m_stCardDialogLimitAvyStat = pUserInfo->m_stIBusyData.GetCardDialogLimitAvyStat();
    notify.m_iCardDialogNextTime = pUserInfo->m_stIBusyData.GetCardDialogNextTime();
    notify.m_iCardDialogTimes = pUserInfo->m_stIBusyData.GetCardDialogTimes();

    notify.m_iGrowFoundationOpenTime = pUserInfo->m_stIBusyData.GetFoundationOpenTime();

    notify.m_iQuarterVipAwardLevel = pUserInfo->m_stUserDailyTable.GetQuarterVipAwardLevel();
    notify.m_strDailyAwardState = int2bin(pUserInfo->m_stUserDailyTable.GetDailyAwardState());
    notify.m_iHeartDialogCard = pUserInfo->m_stUserDailyTable.GetHeartDialogCard();
    notify.m_stDatingPlaceTimes = pUserInfo->m_stUserDailyTable.GetDatingPlaceTimes();
    notify.m_stSimpleGameTimes = pUserInfo->m_stUserDailyTable.GetSimpleGameTimes();
    notify.m_iHeartInsureOpenTime = pUserInfo->m_stIBusyData.GetHeartInsureOpenTime();

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_USER_DAILY_DATA, notify);
}

void CLogicUserNotifySystem::NotifyUserAllAvyInfo(user_data_table_ptr_type pUserInfo, const std::vector<int32_t>& stIDList)
{
    CNotifyAllAvyInfo notify;
    if(stIDList.empty())
    {
        for (auto iter = pUserInfo->m_stUserAvyMap.Begin(); iter != pUserInfo->m_stUserAvyMap.End(); ++iter)
        {
            notify.m_stAvyVec.emplace_back(CLogicCacheCommon::GetUserAvyInfoPack(iter->second->m_stData));
        }
    }
    else
    {
        for(auto id : stIDList)
        {
            auto iter = pUserInfo->m_stUserAvyMap.Find(id);
            if(iter != pUserInfo->m_stUserAvyMap.End())
            {
                notify.m_stAvyVec.emplace_back(CLogicCacheCommon::GetUserAvyInfoPack(iter->second->m_stData));
            }
        }
    }

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_ALL_AVY_INFO, notify);
}

void CLogicUserNotifySystem::NotifyUpdateActivityTask(user_data_table_ptr_type pUserInfo, int32_t iActivityType, int32_t iActivityID, int32_t iTaskID, int32_t iProgress, int32_t iState)
{
	CNotifyUpdateActivityTask notify;
	notify.m_stActivityTask = CPackActivityTask(iActivityType, iActivityID, iTaskID, iProgress, iState);
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_UPDATE_ACTIVITY_TASK, notify);
}

void CLogicUserNotifySystem::NotifyUserLoginTalk(user_data_table_ptr_type pUserInfo)
{
	// 公告
	CResponseBroadcastTalk talkBroadcast;
	talkBroadcast.m_stMsgList.assign(LOGIC_GLOBAL_FRAME_INS->GetBroadcastList().begin(), LOGIC_GLOBAL_FRAME_INS->GetBroadcastList().end());
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_PUSH_BROAD_CAST, talkBroadcast);

    CNotifyOfflineMessage stNotify;
    // 私聊
    stNotify.m_stPrivateTalkMap = pUserInfo->m_stOnlyInCacheData.m_stPrivateTalkCache;

    // 世界聊天
    auto pWorldList = LOGIC_GLOBAL_FRAME_INS->GetWorldTalkList();
    stNotify.m_stMessageVec.assign(pWorldList->begin(), pWorldList->end());
    // 公会聊天
    auto pGuildList = LOGIC_GLOBAL_FRAME_INS->GetGuildTalkList(pUserInfo->m_stUserInfoTable.GetGuildID());
    if(pGuildList)
    {
        stNotify.m_stMessageVec.insert(stNotify.m_stMessageVec.end(), pGuildList->begin(), pGuildList->end());
    }

    auto& stTalkVec = stNotify.m_stMessageVec;
	std::sort(stTalkVec.begin(), stTalkVec.end(),
              [](const decltype(stNotify.m_stMessageVec)::value_type& a, const decltype(stNotify.m_stMessageVec)::value_type& b)
              {
                  return a.m_iTalkTime < b.m_iTalkTime;
              }
              );

    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_TOUHOU_NOTIFY_OFFLINE_MESSAGE, stNotify);
}

void CLogicUserNotifySystem::NotifyUserBuyBackInfo(user_data_table_ptr_type pUserInfo)
{
	CNotifyBuyBackInfo notify;
	for (auto iter = pUserInfo->m_stBuyBackMap.Begin(); iter != pUserInfo->m_stBuyBackMap.End(); ++iter)
	{
		CPackBuyBack stPack;
		stPack.m_iBackType = iter->first;
		stPack.m_iUseBackCount = iter->second->GetUseBackCount();
		stPack.m_iBuyBackCount = iter->second->GetBuyBackCount();
		stPack.m_iCanBackCount = iter->second->GetCanBackCount();
		stPack.m_iLeftBasicCount = GetUserBuyBackLeftBasicCount(pUserInfo, static_cast<CEnumBuyBackType>(iter->first));
		notify.m_stBuyBackVec.push_back(std::move(stPack));
	}
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_BUY_BACK_INFO, notify);
}

void CLogicUserNotifySystem::NotifyUpdateHonor(user_data_table_ptr_type pUserInfo, const TUserHonorTableValueType& stVal)
{
	CNotifyHonorUpdate notify;
	notify.m_stHonorInfo = CLogicCacheCommon::GetHonorInfo(stVal);
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_HONOR_UPDATE, notify);
}

int32_t CLogicUserNotifySystem::GetUserBuyBackLeftBasicCount(user_data_table_ptr_type pUserInfo, CEnumBuyBackType type)
{
	int32_t iCount = 0;
	if (type == CEnumBuyBackType::HORSE || type == CEnumBuyBackType::DIAOCHAN || type == CEnumBuyBackType::SINGLE_LEVEL)
	{
		std::map<CEnumBuyBackType, int32_t> BackTypeLevelMap = {
			{ CEnumBuyBackType::HORSE, CLogicConfigDefine::PAO_MA_LEVEL_ID },
			{ CEnumBuyBackType::DIAOCHAN, CLogicConfigDefine::DIAOCHAN_LEVEL_ID },
			{ CEnumBuyBackType::SINGLE_LEVEL, CLogicConfigDefine::SINGLE_LEVEL_ID },
		};
		
		const auto pstLevelConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(BackTypeLevelMap[type]);
		if (nullptr != pstLevelConfig)
		{
			iCount = pstLevelConfig->m_iEnterTimes;
		}

		const auto iterLevel = pUserInfo->m_stLevelInfoTableMap.Find(BackTypeLevelMap[type]);
		if (iterLevel != pUserInfo->m_stLevelInfoTableMap.End() && !LOGIC_CHECK_DAY_REFRESH(iterLevel->second->GetLastPlayTime()))
		{
			iCount -= iterLevel->second->GetPlayTimes();
		}
	}

	return iCount;
}

void CLogicUserNotifySystem::NotifyUserOtherData(user_data_table_ptr_type pUserInfo)
{
    CNotifyUserOtherData notify;
    for(auto iter = pUserInfo->m_stHeroHistoryMap.begin(); iter != pUserInfo->m_stHeroHistoryMap.end(); ++iter)
    {
        for(auto iterElem = iter->second.Begin(); iterElem != iter->second.End(); ++iterElem)
        {
            CPackHeroHistory history;
            history.m_iIndex = iterElem->first;
            history.m_iUnlockTime = iterElem->second->GetUnlockTime();
            history.m_iContentIndex = iterElem->second->GetContentIndex();
            history.m_iRewardTag = iterElem->second->GetRewardTag();

            notify.m_stHistory[iter->first].emplace_back(history);
        }
    }

    // 编队
    for(auto iter = pUserInfo->m_stUserTeamPresetMap.Begin(); iter != pUserInfo->m_stUserTeamPresetMap.End(); ++iter)
    {
        CPackTeamPreset stTeam;
        stTeam.m_iIndex = iter->first;
        stTeam.m_strName = iter->second->GetName();
        stTeam.m_stTeam = iter->second->GetTeam();
        notify.m_stTeamPresetVec.emplace_back(stTeam);
    }

    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_TOUHOU_NOTIFY_USER_OTHER_DATA, notify);
}

void CLogicUserNotifySystem::NotifyUserNewHeroHistory(user_data_table_ptr_type pUserInfo, int32_t iCardID, int32_t iIndex, int32_t iUnlockTime, int32_t iContentIndex)
{
    CNotifyNewHeroHistory notify;
    notify.m_iCardID = iCardID;
    notify.m_stHistory.m_iIndex = iIndex;
    notify.m_stHistory.m_iUnlockTime = iUnlockTime;
    notify.m_stHistory.m_iContentIndex = iContentIndex;

    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_TOUHOU_NOTIFY_NEW_HERO_HISTORY, notify);
}

void CLogicUserNotifySystem::NotifyUserItemAttr(user_data_table_ptr_type pUserInfo, int32_t iItemID, int32_t iNextTime, int32_t iOpenedCount)
{
    CNotifyItemAttr notify;
    notify.m_stAttr.m_iItemID = iItemID;
    notify.m_stAttr.m_iNextOpenTime = iNextTime;
    notify.m_stAttr.m_iOpenedCount = iOpenedCount;
    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_ITEM_ATTR, notify);
}

void CLogicUserNotifySystem::NotifyShopCount(user_data_table_ptr_type pUserInfo)
{
	CNotifyShopCount notify;
	for(auto iter = pUserInfo->m_stUserShopMap.Begin(); iter != pUserInfo->m_stUserShopMap.End(); ++iter)
    {
        notify.m_stCountMap[iter->first] = iter->second->GetBuyCount();
    }

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_SHOP_COUNT, notify);
}

void CLogicUserNotifySystem::NotifyCandleAdd(user_data_table_ptr_type pUserInfo)
{
	CResponseBody notify;
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_CANDLE_ADD, notify);
}

void CLogicUserNotifySystem::NotifyServerConfig(user_data_table_ptr_type pUserInfo)
{
	CNotifyServerConfig notify;
	notify.m_strNormalService = CLogicConfig::GetZooLogicGameConfig().m_strNormalService;
	notify.m_strSuperService = CLogicConfig::GetZooLogicGameConfig().m_strSuperService;
	const auto& channel = CLogicConfig::GetZooLogicPlatformConfig().GetChannelConfig(pUserInfo->m_iChannelID);
	notify.m_iH5PaySwitch = channel.m_iH5PaySwitch;
	notify.m_strH5PayLink = channel.m_strH5PayLink;
	notify.m_strEvaluateUrl = channel.m_strEvaluateUrl;
	notify.m_iEvaluateLevel = CLogicConfig::GetZooLogicGameConfig().m_iEvaluateLevel;
	notify.m_iEvaluateRatio = CLogicConfig::GetZooLogicGameConfig().m_iEvaluateRatio;
	notify.m_iMergeTime = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iMergeTime;
	notify.m_iMergeCount = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iMergeCount;
	notify.m_iThreePaySwitch = channel.m_iThreePaySwitch;
	notify.m_strThreePayLink = channel.m_strThreePayLink;

	if (channel.m_iThreePaySwitch)
    {
        GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_SERVER_CONFIG, notify);
    }
	else
    {
        CNotifyServerConfigNoThree no_three_notify = notify;
        GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_SERVER_CONFIG, no_three_notify);
    }
}

void CLogicUserNotifySystem::NotifyMergeActivity(user_data_table_ptr_type pUserInfo)
{
	CNotifyMergeActivityInfo notify;
	for (auto iter = pUserInfo->m_stMergeActivityMap.Begin(); iter != pUserInfo->m_stMergeActivityMap.End(); ++iter)
	{
		CPackMergeActivityInfo info;
		info.m_iTaskID = iter->first;
		info.m_iProgress = iter->second->GetProgress();
		info.m_byAward = iter->second->GetAward();
		notify.m_stInfoVec.push_back(info);
	}
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_MERGE_ACTIVITY_INFO, notify);
}

void CLogicUserNotifySystem::NotifyMergeActivityChange(user_data_table_ptr_type pUserInfo, int32_t iTaskID, int32_t iProgress, int8_t byAward)
{
	CNotifyMergeActivityChange notify;
	notify.m_stInfo.m_iTaskID = iTaskID;
	notify.m_stInfo.m_iProgress = iProgress;
	notify.m_stInfo.m_byAward = byAward;
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_MERGE_ACTIVITY_CHANGE, notify);
}

void CLogicUserNotifySystem::NotifyFairPvpInfo(user_data_table_ptr_type pUserInfo)
{
	CNotifyFairPvpInfo notify;
	notify.m_stPvpInfo.m_iWinCount = pUserInfo->m_stUserInfoTable.GetFairPvpWinCount();
	notify.m_stPvpInfo.m_iLoseCount = pUserInfo->m_stUserInfoTable.GetFairPvpLoseCount();
	if (pUserInfo->m_stUserInfoTable.GetFairPvpStoreCard().empty())
	{
		pUserInfo->m_stUserInfoTable.SetFairPvpStoreCard(CLogicGlobal::FairPvpRandomOptionCard(pUserInfo));
	}
	notify.m_stPvpInfo.m_iStoreCardVec = pUserInfo->m_stUserInfoTable.GetFairPvpStoreCard();
	notify.m_stPvpInfo.m_iTeamCardVec = pUserInfo->m_stUserInfoTable.GetFairPvpTeamCard();
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_CROSS_NOTIFY_FAIR_PVP_INFO, notify);
}

void CLogicUserNotifySystem::NotifyMatchPvpInfo(user_data_table_ptr_type pUserInfo)
{
	CNotifyMatchPvpInfo notify;
	notify.m_stPvpInfo.m_iMatchScore = pUserInfo->m_stUserInfoTable.GetMatchScore();	
	notify.m_stPvpInfo.m_iDailyCount = pUserInfo->m_stUserInfoTable.GetMatchDailyCount();
	notify.m_stPvpInfo.m_iMaxScore = pUserInfo->m_stUserInfoTable.GetMatchMaxScore();
	notify.m_stPvpInfo.m_iMaxLevel = CLogicConfig::GetMatchPvpConfig().GetMatchLevelByScore(notify.m_stPvpInfo.m_iMaxScore);
	notify.m_stPvpInfo.m_iWinCount = pUserInfo->m_stUserInfoTable.GetMatchWinCount();
	notify.m_stPvpInfo.m_iLoseCount = pUserInfo->m_stUserInfoTable.GetMatchLoseCount();
	notify.m_stPvpInfo.m_strMaxBonusBitmap = std::string(pUserInfo->m_stUserInfoTable.GetMatchMaxLevelAwardBitmap().rbegin(), pUserInfo->m_stUserInfoTable.GetMatchMaxLevelAwardBitmap().rend());
	notify.m_stPvpInfo.m_iStoreBuyCountMap = pUserInfo->m_stUserInfoTable.GetMatchStoreBuyCount().m_iBuyCountMap;
	notify.m_stPvpInfo.m_iShowGroupID = CLogicConfig::GetSystemConfig().GetGroupShowID(pUserInfo->m_iGroupID);
	notify.m_stPvpInfo.m_strGroupName = CLogicConfig::GetSystemConfig().GetGroupName(pUserInfo->m_iGroupID);
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_CROSS_NOTIFY_MATCH_PVP_INFO, notify);
}

void CLogicUserNotifySystem::NotifyQiyuData(user_data_table_ptr_type pUserInfo)
{
	CNotifyQiyuInfo notice;

	CLogicActive::UpdateUserQiyu(pUserInfo);

	for (auto &iter : pUserInfo->m_stQiyuMap)
	{
		CPackQiyuMapData mapData;
		GenQiyuData(pUserInfo, iter.first, iter.second, mapData);
		if (mapData.m_iID == pUserInfo->m_stUserInfoTable.GetDayQiyuID())
			notice.m_qiyuMap = mapData;
		else
			notice.m_cangbaotuMap = mapData;
	}

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_QIYU_INFO_NOTIFY, notice);
}

void CLogicUserNotifySystem::GenQiyuData(user_data_table_ptr_type pUserInfo, int qiyuID, const user_data_table_s::qiyu_container_type& qiyuData, CPackQiyuMapData &mapData)
{
	mapData.m_iID = qiyuID;
	if (qiyuID == pUserInfo->m_stUserInfoTable.GetDayQiyuID())
	{
		mapData.m_iEndTime = pUserInfo->m_stUserInfoTable.GetQiyuEndtime();
		mapData.m_stBuffVec = pUserInfo->m_stUserInfoTable.GetQiyuBuff();
		mapData.m_mpHeroCostHp.clear();
		for (auto stIter = pUserInfo->m_stHeroCardTableMap.Begin();
			stIter != pUserInfo->m_stHeroCardTableMap.End();
			++stIter)
		{
			CLogicCacheCardOperation cardOperation(pUserInfo);
			if (!cardOperation.CheckHeroCardExist(stIter->first))
				continue;

			mapData.m_mpHeroCostHp.insert(std::make_pair(stIter->first, stIter->second->GetQiyuFightCostHp()));
		}

		auto pConf = CLogicConfig::GetQiyuConfig().m_mpQiyuMap.find(qiyuID);
		if (pConf != CLogicConfig::GetQiyuConfig().m_mpQiyuMap.end())
		{
			mapData.m_iCangbaotuID = pConf->second.m_iCangbaotuID;
			auto suipianBitMapIter = pUserInfo->m_stUserInfoTable.GetCangbaotuSuipian().m_mpCangbaotuSuipian.find(mapData.m_iCangbaotuID);
			if (suipianBitMapIter == pUserInfo->m_stUserInfoTable.GetCangbaotuSuipian().m_mpCangbaotuSuipian.end())
			{
				mapData.m_strCangbaotuSuipianBitMap = int2bin<uint32_t>(0);
			}
			else
			{
				mapData.m_strCangbaotuSuipianBitMap = int2bin<uint32_t>(suipianBitMapIter->second);
			}
		}

        mapData.m_stTeam = CLogicCacheCommon::GetTeam(pUserInfo, CLogicConfigDefine::ETT_QiyuTeam);
	}
	else
	{
		mapData.m_iEndTime = pUserInfo->m_stUserInfoTable.GetCangbaotuEndtime();
		mapData.m_stBuffVec = pUserInfo->m_stUserInfoTable.GetCangbaotuBuff();
		mapData.m_mpHeroCostHp.clear();
		for (auto stIter = pUserInfo->m_stHeroCardTableMap.Begin();
			stIter != pUserInfo->m_stHeroCardTableMap.End();
			++stIter)
		{
			CLogicCacheCardOperation cardOperation(pUserInfo);
			if (!cardOperation.CheckHeroCardExist(stIter->first))
				continue;

			mapData.m_mpHeroCostHp.insert(std::make_pair(stIter->first, stIter->second->GetCangbaotuFightCostHp()));
		}

		auto pConf = CLogicConfig::GetQiyuConfig().m_mpQiyuMap.find(qiyuID);
		if (pConf != CLogicConfig::GetQiyuConfig().m_mpQiyuMap.end())
		{
			mapData.m_iCangbaotuID = pConf->second.m_iCangbaotuID;
			auto suipianBitMapIter = pUserInfo->m_stUserInfoTable.GetCangbaotuSuipian().m_mpCangbaotuSuipian.find(mapData.m_iCangbaotuID);
			if (suipianBitMapIter == pUserInfo->m_stUserInfoTable.GetCangbaotuSuipian().m_mpCangbaotuSuipian.end())
			{
				mapData.m_strCangbaotuSuipianBitMap = int2bin<uint32_t>(0);
			}
			else
			{
				mapData.m_strCangbaotuSuipianBitMap = int2bin<uint32_t>(suipianBitMapIter->second);
			}
		}

        mapData.m_stTeam = CLogicCacheCommon::GetTeam(pUserInfo, CLogicConfigDefine::ETT_CangbaotuTeam);
	}

	for (auto iter = qiyuData.Begin(); iter != qiyuData.End(); ++iter)
	{
		CPackQiyuNodeData nodeData;
		nodeData.m_iNodeID = iter->first;
		nodeData.m_iNodeState = iter->second->GetNodeState();
		nodeData.m_iEventType = iter->second->GetNodeEventType();
		nodeData.m_iEventID = iter->second->GetNodeEventID();
		nodeData.m_aParams = iter->second->GetParamsInt();
		nodeData.m_aStrParams = iter->second->GetParamsStr();
		
		mapData.m_aNodes.emplace_back(nodeData);
	}
}

void CLogicUserNotifySystem::NotifyQiyuEvent(user_data_table_ptr_type pUserInfo, CNotifyQiyuEvent &notice)
{
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_QIYU_EVENT_NOTICE, notice);
}

void CLogicUserNotifySystem::NotifyCangbaotuSuipianData(user_data_table_ptr_type pUserInfo, CNotifyCangbaotuSuipian &notice)
{
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_QIYU_CANGBAOTU_SUIPIAN_NOTIFY, notice);
}

void CLogicUserNotifySystem::NotifyThemeActivityChargeData(user_data_table_ptr_type pUserInfo, int activityID)
{
	CNotifyChargeActivity notice;
	notice.m_iActivityID = activityID;

	auto pActivity = pUserInfo->m_stThemeActivityChargeMap.find(activityID);
	if (pActivity != pUserInfo->m_stThemeActivityChargeMap.end())
	{
		for (auto iter = pActivity->second.Begin(); iter != pActivity->second.End(); ++iter)
		{
			CPackChargeActivityTask taskElem;
			taskElem.m_iID = iter->first;
			taskElem.m_iTaskType = iter->second->GetTaskType();
			taskElem.m_iTarget = iter->second->GetTargetAmount();
			taskElem.m_iProgress = iter->second->GetProgress();
			taskElem.m_cState = iter->second->GetState();

			switch (taskElem.m_iTaskType)
			{
			case charge_task_login:
				notice.m_stLoginTask.push_back(taskElem);
				break;
			case charge_task_recharge:
				notice.m_stChargeTask.push_back(taskElem);
				break;
			case charge_task_buy:
				notice.m_stBuyTask.push_back(taskElem);
				break;
			default:
				break;
			}
		}
	}

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_THEME_ACTIVITY_CHARGE_NOTIFY, notice);
}

void CLogicUserNotifySystem::NotifyThemeActivityTaskChange(user_data_table_ptr_type pUserInfo, int activityID, const CPackChargeActivityTask& task)
{
	CNotifyChargeTaskChange notice;
	notice.m_iActivityID = activityID;
	notice.m_stTask = task;

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_THEME_ACTIVITY_CHARGE_CHANGE_NOTIFY, notice);
}

void CLogicUserNotifySystem::NotifyIBusyData(user_data_table_ptr_type pUserInfo)
{
	if (pUserInfo->m_stIBusyData.GetActivityID() == 0)
		return;

	CNotifyIbusyInfo notice;

	//
	notice.m_stBaseInfo.m_iActivityID = pUserInfo->m_stIBusyData.GetActivityID();
	notice.m_stBaseInfo.m_iLevel = pUserInfo->m_stIBusyData.GetLevel();
	notice.m_stBaseInfo.m_iExp = pUserInfo->m_stIBusyData.GetExp();
	notice.m_stBaseInfo.m_iScore = pUserInfo->m_stIBusyData.GetScore();
	// notice.m_stBaseInfo.m_iHuoyue = pUserInfo->m_stIBusyData.GetHuoyue();
	notice.m_stBaseInfo.m_iTodayHuoyue = pUserInfo->m_stIBusyData.GetTodayHuoyue();
	notice.m_stBaseInfo.m_iChangeTaskCount = pUserInfo->m_stIBusyData.GetChangeTaskCount();
	notice.m_stBaseInfo.m_iMarketLastRefreshTime = pUserInfo->m_stIBusyData.GetMarketLastRefreshTime();
	notice.m_stBaseInfo.m_iMarketRefreshTimes = pUserInfo->m_stIBusyData.GetMarketRefreshTimes();
	notice.m_stBaseInfo.m_iDailyExpAward = pUserInfo->m_stIBusyData.GetDailyExpAward();

	//
	notice.m_stGettedLevelReward = pUserInfo->m_stIBusyData.GetGettedLevelReward();
	notice.m_stGettedDepositReward = pUserInfo->m_stIBusyData.GetGettedDepositReward();

	//
	CLogicActive::UpdateIBusyMarketData(pUserInfo);
	for (auto stIterBeg = pUserInfo->m_stIBusyMarket.Begin(); stIterBeg != pUserInfo->m_stIBusyMarket.End(); ++stIterBeg)
	{
		CPackMarket stElem;

		stElem.m_stCommodity.m_iItemType = stIterBeg->second->GetCommodityItemType();
		stElem.m_stCommodity.m_iCardID = stIterBeg->second->GetCommodityItemID();
		stElem.m_stCommodity.m_iNum = stIterBeg->second->GetCommodityItemNum();

		stElem.m_stConsume.m_iItemType = stIterBeg->second->GetConsumeItemType();
		stElem.m_stConsume.m_iCardID = stIterBeg->second->GetConsumeItemID();
		stElem.m_stConsume.m_iNum = stIterBeg->second->GetConsumeItemNum();

		stElem.m_iDiscount = stIterBeg->second->GetDiscount();

		if (pUserInfo->m_stIBusyData.GetMarketLastRefreshTime() < stIterBeg->second->GetLastBuyTime())
			stElem.m_bCanBuy = false;

		notice.m_stMarketInfo.m_stMarketInfo.push_back(stElem);
	}

	//
	notice.m_stTaskInfo = pUserInfo->m_stIBusyData.GetTaskInfo();

	//
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_IBUSY_INFO_NOTICE, notice);
}

void CLogicUserNotifySystem::NotifyIBusyBaseData(user_data_table_ptr_type pUserInfo)
{
	CNotifyIbusyBase notice;

	notice.m_stBaseInfo.m_iActivityID = pUserInfo->m_stIBusyData.GetActivityID();
	notice.m_stBaseInfo.m_iLevel = pUserInfo->m_stIBusyData.GetLevel();
	notice.m_stBaseInfo.m_iExp = pUserInfo->m_stIBusyData.GetExp();
	notice.m_stBaseInfo.m_iScore = pUserInfo->m_stIBusyData.GetScore();
	// notice.m_stBaseInfo.m_iHuoyue = pUserInfo->m_stIBusyData.GetHuoyue();
	notice.m_stBaseInfo.m_iTodayHuoyue = pUserInfo->m_stIBusyData.GetTodayHuoyue();
	notice.m_stBaseInfo.m_iChangeTaskCount = pUserInfo->m_stIBusyData.GetChangeTaskCount();
	notice.m_stBaseInfo.m_iMarketLastRefreshTime = pUserInfo->m_stIBusyData.GetMarketLastRefreshTime();
	notice.m_stBaseInfo.m_iMarketRefreshTimes = pUserInfo->m_stIBusyData.GetMarketRefreshTimes();
    notice.m_stBaseInfo.m_iDailyExpAward = pUserInfo->m_stIBusyData.GetDailyExpAward();

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_IBUSY_BASE_INFO_NOTICE, notice);
}

void CLogicUserNotifySystem::NotifyIBusyTaskData(user_data_table_ptr_type pUserInfo)
{
	CNotifyIbusyTask notice;

	notice.m_stTaskInfo = pUserInfo->m_stIBusyData.GetTaskInfo();
	
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_IBUSY_TASK_NOTICE, notice);
}

void CLogicUserNotifySystem::NotifyIBusyMarketData(user_data_table_ptr_type pUserInfo)
{
	CLogicActive::UpdateIBusyMarketData(pUserInfo);
	
	CNotifyIbusyMarket notice;

	for (auto stIterBeg = pUserInfo->m_stIBusyMarket.Begin(); stIterBeg != pUserInfo->m_stIBusyMarket.End(); ++stIterBeg)
	{
		CPackMarket stElem;

		stElem.m_stCommodity.m_iItemType = stIterBeg->second->GetCommodityItemType();
		stElem.m_stCommodity.m_iCardID = stIterBeg->second->GetCommodityItemID();
		stElem.m_stCommodity.m_iNum = stIterBeg->second->GetCommodityItemNum();

		stElem.m_stConsume.m_iItemType = stIterBeg->second->GetConsumeItemType();
		stElem.m_stConsume.m_iCardID = stIterBeg->second->GetConsumeItemID();
		stElem.m_stConsume.m_iNum = stIterBeg->second->GetConsumeItemNum();

		stElem.m_iDiscount = stIterBeg->second->GetDiscount();

		if (pUserInfo->m_stIBusyData.GetMarketLastRefreshTime() < stIterBeg->second->GetLastBuyTime())
			stElem.m_bCanBuy = false;

		notice.m_stMarketInfo.m_stMarketInfo.push_back(stElem);
	}

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_IBUSY_MARKET_DATA_NOTICE, notice);
}

void CLogicUserNotifySystem::NotifyIBusyLevelRewardData(user_data_table_ptr_type pUserInfo)
{
	CNotifyIbusyLevelReward notice;

	notice.m_stGettedLevelReward = pUserInfo->m_stIBusyData.GetGettedLevelReward();
    notice.m_stGettedDepositReward = pUserInfo->m_stIBusyData.GetGettedDepositReward();

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_IBUSY_LEVELREWARD_INFO_NOTICE, notice);
}

void CLogicUserNotifySystem::NotifyScoreLevelActivityData(user_data_table_ptr_type pUserInfo)
{
	CNotifyScoreLevelInfo notice;
	const auto iter_activity = pUserInfo->m_stScoreLevelMap.Find(CLogicActive::GetScoreLevelOpenActivityID());
	if (iter_activity != pUserInfo->m_stScoreLevelMap.End())
    {
	    notice.m_iActivityID = iter_activity->first;
	    notice.m_iXingdongli = iter_activity->second->GetXingdongli();
	    notice.m_iLastXingdongliTime = iter_activity->second->GetLastXingdongliTime();
	    notice.m_iBuyXingdongliTime = iter_activity->second->GetBuyXingdongliCount();
	    notice.m_iScore = iter_activity->second->GetScore();
	    notice.m_strScoreAwardBitmap = int2bin(iter_activity->second->GetScoreAwardBitmap());
	    notice.m_strDepositAwardBitmap = int2bin(iter_activity->second->GetDepositAwardBitmap());
	    notice.m_strScoreLevelBitmap = int2bin(iter_activity->second->GetScoreLevelBitmap());
	    notice.m_strChallengeLevelBitmap = int2bin(iter_activity->second->GetChallengeLevelBitmap());
    }

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_SCORE_LEVEL_INFO_NOTICE, notice);
}

void CLogicUserNotifySystem::NotifyMonthPass(user_data_table_ptr_type pUserInfo)
{
    CNotifyMonthPass notify;
    notify.m_iExp = pUserInfo->m_stIBusyData.GetMonthPassExp();
    notify.m_iRecharge = pUserInfo->m_stIBusyData.GetMonthPassRecharge();
    notify.m_strAwardBitmap = pUserInfo->m_stIBusyData.GetMonthPassAwardBitmap();
    notify.m_strDepositAwardBitmap = pUserInfo->m_stIBusyData.GetMonthPassDepositAwardBitmap();
    notify.m_iWishEventID = pUserInfo->m_stIBusyData.GetMonthPassWishEventID();
    notify.m_iWishItemIndexVec = pUserInfo->m_stIBusyData.GetMonthPassWishItemIndex();

    for (const auto& month_pass : pUserInfo->m_stMonthPassTaskMap)
    {
        for (auto iterTask = month_pass.second.Begin(); iterTask != month_pass.second.End(); ++iterTask)
        {
            if (month_pass.first == CLogicConfigDefine::MONTH_PASS_TYPE_DAILY_TASK || month_pass.first == CLogicConfigDefine::MONTH_PASS_TYPE_WEEKLY_TASK)
            {
                CPackMonthPassTask task;
                task.m_iTaskID = iterTask->first;
                task.m_iProgress = iterTask->second->GetProgress();
                task.m_iState = iterTask->second->GetState();
                notify.m_stTaskVec.push_back(task);
            }
            else if (month_pass.first == CLogicConfigDefine::MONTH_PASS_TYPE_EXCHANGE)
            {
                notify.m_iExchangeCount[iterTask->first] = iterTask->second->GetProgress();
            }
            else if (month_pass.first == CLogicConfigDefine::MONTH_PASS_TYPE_WISH)
            {
                notify.m_iWishItemExtraNum[iterTask->first] = iterTask->second->GetProgress();
            }
        }
    }

    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_MONTH_PASS_NOTIFY, notify);
}

void CLogicUserNotifySystem::NotifyMonthPassTask(user_data_table_ptr_type pUserInfo, int32_t iTaskID, int32_t iProgress, int32_t iState)
{
    CNotifyMonthPassTask notify;
    notify.m_stTask.m_iTaskID = iTaskID;
    notify.m_stTask.m_iProgress = iProgress;
    notify.m_stTask.m_iState = iState;
    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_MONTH_PASS_TASK_NOTIFY, notify);
}

void 
CLogicUserNotifySystem::NotifyDaySingleRechargeActivityInfo(user_data_table_ptr_type pUserInfo)
{
	CNotifyDaySingleRecharge notify;
	for (auto iter = pUserInfo->m_stDaySingleRechargeMap.Begin(); iter != pUserInfo->m_stDaySingleRechargeMap.End(); ++iter)
	{
		CPackDaySingleRechargeTask task;
		task.m_iActivityID = iter->second->m_stData.m_iActivityID;
		task.m_iTaskID = iter->second->m_stData.m_iTaskID;
		task.m_iRewardCount = iter->second->GetRewardCount();
		task.m_iCanRewardCount = iter->second->GetCanRewardCount();
		notify.m_stTaskVec.push_back(task);
	}

	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_DAY_SINGLE_RECHARGE_INFO, notify);
}

void CLogicUserNotifySystem::NotifyDaySingleRechargeTaskChange(user_data_table_ptr_type pUserInfo, int32_t iActivityID, int32_t iTaskID, int32_t iRewardCount, int32_t iCanRewardCount)
{
	CNotifyDaySingleRechargeTaskChange notify;
	notify.m_stTask.m_iActivityID = iActivityID;
	notify.m_stTask.m_iTaskID = iTaskID;
	notify.m_stTask.m_iRewardCount = iRewardCount;
	notify.m_stTask.m_iCanRewardCount = iCanRewardCount;
	GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_DAY_SINGLE_RECHARGE_TASK_CHANGE, notify);
}

void CLogicUserNotifySystem::NotifyUserBuffData(user_data_table_ptr_type pUserInfo)
{
    CNotifyUserBuffData notify;
    for(auto iter = pUserInfo->m_stUserBuffMap.Begin(); iter != pUserInfo->m_stUserBuffMap.End(); ++iter)
    {
        CPackUserBuff elem;
        elem.m_iEffectIndex = iter->first;
        elem.m_iBuffType = iter->second->GetBuffType();
        elem.m_iBuffPara = iter->second->GetBuffPara();
        elem.m_iBuffParaEx = iter->second->GetBuffParaEx();
        elem.m_iEndTime = iter->second->GetEndTime();
        elem.m_iEffectTimes = iter->second->GetEffectTimes();
        notify.m_stBuffMap.emplace_back(elem);
    }

    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_TOUHOU_NOTIFY_BUFF, notify);
}

void CLogicUserNotifySystem::NotifyQuestionnaire(user_data_table_ptr_type pUserInfo)
{
    if(pUserInfo->m_stIBusyData.GetQuestionnaireDurationTime().empty())
        return;

    CNotifyQuestionnaire notify;
    for(auto& data : pUserInfo->m_stIBusyData.GetQuestionnaireDurationTime())
    {
        const auto pConfig = CLogicConfig::GetQuestionnaireConfig().GetConfig(data.first);
        if(nullptr == pConfig) continue;

        CPackQuestionnaireElem stElem;
        stElem.m_iID = data.first;
        stElem.m_iDurationTime = data.second;
        stElem.m_stRewards = pConfig->m_stRewards;
        stElem.m_strName = pConfig->m_strName;
        stElem.m_strContent = pConfig->m_strContent;
        for(auto iter = pConfig->m_stQuestionList.begin(); iter != pConfig->m_stQuestionList.end(); ++iter)
        {
            CPackQuestionnaireProblemElem stProblem;
            stProblem.m_iType = iter->m_iType;
            stProblem.m_iOtherIndex = iter->m_iOtherIndex;
            stProblem.m_iMaxChoice = iter->m_iMaxChoice;
            stProblem.m_strTitle = iter->m_strTitle;
            stProblem.m_stAnswerList = iter->m_stAnswerList;
            stProblem.m_bNotMustAnswer = iter->m_bNotMustAnswer;
            stElem.m_stProblemList.push_back(stProblem);
        }

        notify.m_stQuestionnaireList.push_back(std::move(stElem));
    }

    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_TOUHOU_NOTIFY_QUESTIONNAIRE, notify);
}

void CLogicUserNotifySystem::NotifyItemChange(user_data_table_ptr_type pUserInfo, const std::vector<CPackGameItem>& stGets,
                                              const std::vector<CPackGameItem>& stConsumes/* = {}*/, bool bNotify/* = false*/)
{
    CNotifyItemChange notify;
    notify.m_stGets = stGets;
    notify.m_stConsumes = stConsumes;
    notify.m_bNotify = bNotify;
    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_ITEM_CHANGE, notify);
}

void CLogicUserNotifySystem::NotifyQuestData(user_data_table_ptr_type pUserInfo, const std::set<int32_t>& stQuestIDSet)
{
    CNotifyQuestData stNotify;
    CPackQuestData stData;
    if(stQuestIDSet.empty())
    {
        for(auto iter = pUserInfo->m_stQuestMap.Begin(); iter != pUserInfo->m_stQuestMap.End(); ++iter)
        {
            stData.m_iQuestID = iter->first;
            stData.m_iIndex = iter->second->GetParaIndex();
            stData.m_iProgress = iter->second->GetProgress();
            stData.m_cState = iter->second->GetState();
            stNotify.m_stQuestVec.push_back(stData);
        }
        stNotify.m_bIsInit = true;
    }
    else
    {
        for(auto& id : stQuestIDSet)
        {
            auto iter = pUserInfo->m_stQuestMap.Find(id);
            if(iter == pUserInfo->m_stQuestMap.End()) continue;

            stData.m_iQuestID = iter->first;
            stData.m_iIndex = iter->second->GetParaIndex();
            stData.m_iProgress = iter->second->GetProgress();
            stData.m_cState = iter->second->GetState();
            stNotify.m_stQuestVec.push_back(stData);
        }
        stNotify.m_bIsInit = false;
    }

    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_QUEST_DATA, stNotify);
}

void CLogicUserNotifySystem::NotifyBadgeData(user_data_table_ptr_type pUserInfo, const std::set<int32_t>& stBadgeIDSet)
{
    CNotifyBadgeData stNotify;
    CPackBadgeData stData;
    if(stBadgeIDSet.empty())
    {
        for(auto iter = pUserInfo->m_stUserBadgeMap.Begin(); iter != pUserInfo->m_stUserBadgeMap.End(); ++iter)
        {
            stData.m_iBadgeID = iter->first;
            stData.m_iProgress = iter->second->GetProgress();
            stData.m_iOwnedTime = iter->second->GetOwnedTime();
            stData.m_stProSet = iter->second->GetProSet();
            stData.m_bIsOwned = iter->second->GetIsOwned();
            stNotify.m_stBadgeVec.push_back(stData);
        }
    }
    else
    {
        for(auto& id : stBadgeIDSet)
        {
            auto iter = pUserInfo->m_stUserBadgeMap.Find(id);
            if(iter == pUserInfo->m_stUserBadgeMap.End()) continue;

            stData.m_iBadgeID = iter->first;
            stData.m_iProgress = iter->second->GetProgress();
            stData.m_iOwnedTime = iter->second->GetOwnedTime();
            stData.m_stProSet = iter->second->GetProSet();
            stData.m_bIsOwned = iter->second->GetIsOwned();
            stNotify.m_stBadgeVec.push_back(stData);
        }
    }

    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_BADGE_DATA, stNotify);
}

void CLogicUserNotifySystem::NotifyUserPhyData(user_data_table_ptr_type pUserInfo)
{
    CNotifyUsePhyData notify;
    notify.m_iPhyStore = pUserInfo->m_stUserInfoTable.GetPhyStore();
    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_PHY_DATA, notify);
}

void CLogicUserNotifySystem::NotifyUserExploreQuest(user_data_table_ptr_type pUserInfo, const TUserHomeExploreTableValueType& stValue)
{
    CNotifyHomeExploreQuest notify;
    notify.m_stQuestInfo = CLogicCacheCommon::GetExploreQuestInfo(stValue);
    GetConnectorSender()->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_NOTIFY_EXPLORE_QUEST, notify);
}


void CLogicUserNotifySystem::RefreshDataChange(user_data_table_ptr_type pUserInfo)
{
    if (pUserInfo->m_stIBusyData.GetDataChangeTag() != 0) return;

    auto awardConfig = CLogicConfig::GetAwardConfig().m_stDataChangeConfig;
    pUserInfo->m_stIBusyData.SetDataChangeTag(1);

    auto stIter = pUserInfo->m_stHeroCardTableMap.Find(awardConfig.m_iCardID);
    if (stIter == pUserInfo->m_stHeroCardTableMap.End())
    {
        return;
    }

    if (stIter->second->GetCardColor() >= awardConfig.m_iNeedGrade)
    {
        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin,
                                       pUserInfo->m_iGroupID,
                                       awardConfig.m_strSender,
                                       awardConfig.m_strTitle,
                                       awardConfig.m_strContent,
                                       awardConfig.m_stReward,
                                       "SEND_DATA_CHANGE_REWARD");
    }
}