//
// Created by luoxiaohu on 2020/9/25.
//

#include "common_datetime.h"
#include "logic_config.h"
#include "logic_link_common.h"
#include "logic_game_global.h"
#include "logic_game_active.h"
#include "logic_game_common_observer.h"
#include "logic_game_cache_common.h"
#include "logic_game_user_notify_system.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"

void CLogicCommonObserver::NotifyWorldQuestEvent(user_data_table_ptr_type pUserInfo, const CLogicEventData& stData)
{
    if(!CLogicConfigDefine::IsValidEnumCommonEventType(stData.m_eType)) return;

    auto iterConfig = CLogicConfig::GetWorldQuestConfig().m_stQuestEventMap.lower_bound(stData.m_eType);
    auto iterConfigEnd = CLogicConfig::GetWorldQuestConfig().m_stQuestEventMap.upper_bound(stData.m_eType);
    auto& stWorldQuest = LOGIC_GLOBAL_CACHE_INS->m_stWorldQuestMap;
    for(; iterConfig != iterConfigEnd; ++iterConfig)
    {
        auto iterQuestData = stWorldQuest.Find(iterConfig->second.m_iQuestID);
        if(iterQuestData == stWorldQuest.End())
        {
            TGlobalWorldQuestTableValueType stValue;
            stValue.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
            stValue.m_iQuestID = iterConfig->second.m_iQuestID;
            stValue.m_iProgress = 0;
            stValue.m_cState = CLogicConfigDefine::ECQS_Accepted;
            stValue.m_iOpenTime = 0;
            stValue.m_iEndTime = 0;

            auto iterResult = stWorldQuest.Insert(std::make_pair(stValue.m_iQuestID, stValue));
            if(!iterResult.second)
            {
                LOGIC_LOG_TRACE_ERROR << "WorldQuest Insert Error! |QuestID:" << stValue.m_iQuestID << std::endl;
                continue;
            }

            iterQuestData = iterResult.first;
        }

        if(iterQuestData->second->GetState() != CLogicConfigDefine::ECQS_Accepted)
            continue;

        switch (stData.m_eType)
        {
        case CLogicConfigDefine::ECET_RoleLevelUp:
        case CLogicConfigDefine::ECET_RoleCombatUp:
        case CLogicConfigDefine::ECET_Tower:
            if(stData.m_iPara < iterConfig->second.m_iEventPara && iterConfig->second.m_iEventPara <= stData.m_iParaEx)
            {
                iterQuestData->second->AddProgress(stData.m_iTimes);
            }
            break;

        case CLogicConfigDefine::ECET_PvELevelFirstFight:
            if(stData.m_iPara == iterConfig->second.m_iEventPara)
            {
                iterQuestData->second->AddProgress(stData.m_iTimes);
            }
            break;

        case CLogicConfigDefine::ECET_CardCommodityGradeNum:
            if(stData.m_iPara == iterConfig->second.m_iEventPara)
            {
                iterQuestData->second->AddProgress(stData.m_iCount);
            }
            break;

        case CLogicConfigDefine::ECET_GoldEggTimes:
        case CLogicConfigDefine::ECET_XunBaoTimes:
        case CLogicConfigDefine::ECET_LuckyTurnTableTimes:
        case CLogicConfigDefine::ECET_LuckyHammerTimes:
            iterQuestData->second->AddProgress(stData.m_iTimes);

            break;

        default:
            LOGIC_LOG_TRACE_ERROR << "WorldQuest EventType Error! |QuestID:" << iterConfig->second.m_iQuestID << "|EventType:" << stData.m_eType << std::endl;
            continue;
        }

        // 已完成
        if(iterQuestData->second->GetProgress() >= iterConfig->second.m_iTargetProgress)
        {
            iterQuestData->second->SetState(CLogicConfigDefine::ECQS_Reached);
            // 检查是否到了可领取时间
            const int32_t iOpenServerDay = day_diff(CLogicConfig::GetSystemConfig().GetOpenSvrTime(), CLogicCommon::GetSec()) + 1;
            if(iterConfig->second.m_iOpenServerDay <= iOpenServerDay)
            {
                iterQuestData->second->SetOpenTime(CLogicCommon::GetSec());
                iterQuestData->second->SetEndTime(CLogicCommon::GetSec() + iterConfig->second.m_iContinueTime);

                // 公告
                if(!iterConfig->second.m_strBroadcastID.empty())
                {
                    CLogicGlobal::PublishBroadcast(iterConfig->second.m_strBroadcastID, 0, {{"bag_name", iterConfig->second.m_strName}});
                }
            }
        }
    }
}

void CLogicCommonObserver::NotifyNewRedBagEvent(user_data_table_ptr_type pUserInfo, const CLogicEventData& stData)
{
    if(!CLogicConfigDefine::IsValidEnumCommonEventType(stData.m_eType)) return;

    auto stUserData = pUserInfo->m_stIBusyData.GetRedBagUniqueIndex();
    auto iterBegin = CLogicConfig::GetRedBagConfig().m_stRedBagEventMap.lower_bound(stData.m_eType);
    auto iterEnd = CLogicConfig::GetRedBagConfig().m_stRedBagEventMap.upper_bound(stData.m_eType);
    for(; iterBegin != iterEnd; ++iterBegin)
    {
        if(iterBegin->second.m_iUniqueIndex > 0 && CLogicCacheCommon::VecBitmapCheckState(stUserData, iterBegin->second.m_iUniqueIndex))
            continue;

        bool bReach = false;
        switch (stData.m_eType)
        {
        case CLogicConfigDefine::ECET_LuckyHammerFinalAward:
        case CLogicConfigDefine::ECET_LuckyTurnTableFinalAward:
        case CLogicConfigDefine::ECET_XunBaoFinalAward:
        case CLogicConfigDefine::ECET_MultiColorSpecialAward:
            bReach = true;

            break;

        case CLogicConfigDefine::ECET_CardCommodityGradeNum:
            if(iterBegin->second.m_iEventPara1 == stData.m_iPara && stData.m_iCount >= iterBegin->second.m_iEventPara2)
                bReach = true;

            break;

        default:
            break;
        }

        // 条件未达成
        if(!bReach) continue;

        // 发放红包道具
        if(iterBegin->second.m_iUniqueIndex > 0)
        {
            CLogicCacheCommon::VecBitmapSetState(stUserData, iterBegin->second.m_iUniqueIndex);
            pUserInfo->m_stIBusyData.SetRedBagUniqueIndex(stUserData);
        }

        CLogicGlobal::AddAsyncUserMail(pUserInfo->m_iUin, pUserInfo->m_iGroupID, iterBegin->second.m_strMailSender,
                                       iterBegin->second.m_strMailTitle, iterBegin->second.m_strMailContent,
                                       {CPackGameItem(CLogicConfigDefine::EXCHANGE_ITEM, iterBegin->second.m_iItemID, 1)},
                                       "SEND_RED_BAG_EVENT");
    }
}

void CLogicCommonObserver::NotifyQuestionnaireEvent(user_data_table_ptr_type pUserInfo, const CLogicEventData& stData)
{
    if(stData.m_eType == CLogicConfigDefine::ECET_RoleLevelUp)
    {
        // 检查问卷调查
        CLogicGlobal::UpdateQuestionnaire(pUserInfo);
    }
}

void CLogicCommonObserver::NotifyQuestEvent(user_data_table_ptr_type pUserInfo, const CLogicEventData& stData)
{
    auto iterConfig = CLogicConfig::GetQuestConfig().m_stEventMap.lower_bound(stData.m_eType);
    auto iterConfigEnd = CLogicConfig::GetQuestConfig().m_stEventMap.upper_bound(stData.m_eType);
    for(; iterConfig != iterConfigEnd; ++iterConfig)
    {
        if(iterConfig->second->m_bNoEarlyRecord)
        {
            if(pUserInfo->m_stUserInfoTable.GetLevel() < iterConfig->second->m_iLevelLimit)
                continue;

            // 判断任务是否开启
            if(iterConfig->second->m_iAvyID > 0 && !CLogicActive::IsAvyOpen(iterConfig->second->m_iAvyID, pUserInfo->m_stUserInfoTable.GetCreateTime()))
            {
                continue;
            }
        }

        auto iterData = pUserInfo->m_stQuestMap.Find(iterConfig->second->m_iQuestID);
        if(iterData == pUserInfo->m_stQuestMap.End())
        {
            TUserQuestTableValueType stValue;
            stValue.m_iUid = pUserInfo->m_iUin;
            stValue.m_iGroupID = pUserInfo->m_iGroupID;
            stValue.m_iQuestID = iterConfig->second->m_iQuestID;
            stValue.m_cState = CLogicConfigDefine::ECQS_Accepted;
            stValue.m_iParaIndex = 0;
            stValue.m_iProgress = 0;
            auto stRet = pUserInfo->m_stQuestMap.Insert(std::make_pair(stValue.m_iQuestID, stValue));
            if (!stRet.second)
            {
                LOGIC_LOG_TRACE_ERROR << "UserQuestMap Insert Error!|UIN:" << pUserInfo->m_iUin << "|GROUP_ID:"
                                      << pUserInfo->m_iGroupID << "|QUEST_ID:" << stValue.m_iQuestID << std::endl;
                continue;
            }

            iterData = stRet.first;
        }

        if(iterData->second->GetState() == CLogicConfigDefine::ECQS_Received)
            continue;

        if(iterData->second->GetState() == CLogicConfigDefine::ECQS_Reached && !iterConfig->second->m_bIsMult)
            continue;

        if(int(iterConfig->second->m_stEventVec.size()) <= iterData->second->GetParaIndex())
        {
            LOGIC_LOG_TRACE_ERROR << "Quest Event Config Error!|UIN:" << pUserInfo->m_iUin << "|GROUP_ID:"
                                  << pUserInfo->m_iGroupID << "|QUEST_ID:" << iterConfig->second->m_iQuestID << std::endl;
            continue;
        }

        auto& stEventConfig = iterConfig->second->m_stEventVec[iterData->second->GetParaIndex()];
        switch (stData.m_eType)
        {
        case CLogicConfigDefine::ECET_CardCommodityGradeNum:
            if(stEventConfig.m_iEventPara1 == stData.m_iPara && stData.m_iCount >= stEventConfig.m_iEventPara2)
                iterData->second->AddProgress(stData.m_iTimes);

            break;

        case CLogicConfigDefine::ECET_CardCommoditySameCardNum:
        case CLogicConfigDefine::ECET_WipeFightOnceNum:
            if(stData.m_iCount >= stEventConfig.m_iEventPara1)
                iterData->second->AddProgress(stData.m_iTimes);

            break;

        case CLogicConfigDefine::ECET_WipeFightTotalNum:
        case CLogicConfigDefine::ECET_UseTalentPoint:
        case CLogicConfigDefine::ECET_ConsumeGoldCoin:
        case CLogicConfigDefine::ECET_ConsumeDiamond:
        case CLogicConfigDefine::ECET_TotalMonster:
            iterData->second->AddProgress(stData.m_iCount);
            break;

        case CLogicConfigDefine::ECET_Login:
        case CLogicConfigDefine::ECET_FightPvPTimes:
        case CLogicConfigDefine::ECET_FightPvPWinTimes:
        case CLogicConfigDefine::ECET_HomeExploreTimes:
        case CLogicConfigDefine::ECET_HomeExploreCompleteTimes:
        case CLogicConfigDefine::ECET_HomeVisitTimes:
        case CLogicConfigDefine::ECET_UpgradeCardTimes:
        case CLogicConfigDefine::ECET_BuyEnergyTimes:
        case CLogicConfigDefine::ECET_FishingSuccessTimes:
        case CLogicConfigDefine::ECET_DivConstelTimes:
        case CLogicConfigDefine::ECET_RuneCardNum:
        case CLogicConfigDefine::ECET_CardCommodityNum:
        case CLogicConfigDefine::ECET_HomeOrnamentNum:
        case CLogicConfigDefine::ECET_JoinGuild:
        case CLogicConfigDefine::ECET_JoinFriendBanquet:
        case CLogicConfigDefine::ECET_GuildWish:
        case CLogicConfigDefine::ECET_CreateBanquet:
        case CLogicConfigDefine::ECET_FishingTotalBigFish:
        case CLogicConfigDefine::ECET_FishingTotalKingFish:
        case CLogicConfigDefine::ECET_UserBindEmail:
        case CLogicConfigDefine::ECET_HarvestCount:
        case CLogicConfigDefine::ECET_HotSpringCount:
            iterData->second->AddProgress(stData.m_iTimes);
            break;

        case CLogicConfigDefine::ECET_PvELevelFirstFight:
        case CLogicConfigDefine::ECET_QuestCondIndexTimes:
            if(stEventConfig.m_iEventPara1 > 0)
            {
                if(stEventConfig.m_iEventPara1 == stData.m_iPara)
                    iterData->second->AddProgress(stData.m_iTimes);
            }
            else
            {
                if(stEventConfig.m_stParaSet.find(stData.m_iPara) != stEventConfig.m_stParaSet.end())
                    iterData->second->AddProgress(stData.m_iTimes);
            }

            break;

        case CLogicConfigDefine::ECET_EquipGradeNum:
        case CLogicConfigDefine::ECET_ConstelGradeNum:
        case CLogicConfigDefine::ECET_LevelTypeTimes:
        case CLogicConfigDefine::ECET_UpgradeCardColor:
        case CLogicConfigDefine::ECET_CardGradeNum:
        case CLogicConfigDefine::ECET_UpgradeCardStar:
        case CLogicConfigDefine::ECET_ChapterAchieveCompleteNum:
        case CLogicConfigDefine::ECET_RefreshLevelTimes:
        case CLogicConfigDefine::ECET_PresentGuildWish:
        case CLogicConfigDefine::ECET_CreateBanquetUseItem:
        case CLogicConfigDefine::ECET_FishingTargetFish:
        case CLogicConfigDefine::ECET_FightPvpNotJoinType:
        case CLogicConfigDefine::ECET_FightPvpLeftAliveWin:
        case CLogicConfigDefine::ECET_FightPvpAllSameType:
        case CLogicConfigDefine::ECET_FightPvpLeftAliveLose:
        case CLogicConfigDefine::ECET_GetTargetHeroCard:
            if(stEventConfig.m_iEventPara1 == stData.m_iPara)
                iterData->second->AddProgress(stData.m_iTimes);
            break;

        case CLogicConfigDefine::ECET_RoleLevelUp:
        case CLogicConfigDefine::ECET_PvPRank:
            iterData->second->SetProgress(stData.m_iParaEx);
            break;

        case CLogicConfigDefine::ECET_UpgradeCardLevel:
        case CLogicConfigDefine::ECET_CardHeartLevel:
        case CLogicConfigDefine::ECET_ConstelLevelUp:
            if(stData.m_iPara < stEventConfig.m_iEventPara1 && stEventConfig.m_iEventPara1 <= stData.m_iParaEx)
                iterData->second->AddProgress(stData.m_iTimes);

            break;

        case CLogicConfigDefine::ECET_UninterruptedCardCommodityNoGrade:
            if(stData.m_iPara == stEventConfig.m_iEventPara1)
                iterData->second->SetProgress(0);
            else
                iterData->second->AddProgress(stData.m_iCount);

            break;

        case CLogicConfigDefine::ECET_ConstelURSuitNum:
        {
            std::map<int, std::map<int, int>> stSuitStat;
            for(auto iter = pUserInfo->m_stConstelMap.Begin(); iter != pUserInfo->m_stConstelMap.End(); ++iter)
            {
                if(iter->second->GetGrade() != CLogicConfigDefine::ECGT_UR) continue;

                ++stSuitStat[iter->second->GetSuit()][iter->second->GetEquipmentTag()];
            }

            int iProgress = 0;
            for(auto& iter : stSuitStat)
            {
                if(iter.second.size() == CLogicConfigDefine::EHET_Max - 1)
                    ++iProgress;
            }

            if(iterData->second->GetProgress() < iProgress)
                iterData->second->SetProgress(iProgress);

            break;
        }
        case CLogicConfigDefine::ECET_CurGoldNum:
        case CLogicConfigDefine::ECET_ExploreLevel:
            if(iterData->second->GetProgress() < stData.m_iPara)
                iterData->second->SetProgress(stData.m_iPara);

            break;
        case CLogicConfigDefine::ECET_FishingBigFish:
        case CLogicConfigDefine::ECET_FishingKingFish:
        {
            auto stSet = iterData->second->GetProgressSet();
            if(stSet.find(stData.m_iPara) == stSet.end())
            {
                stSet.insert(stData.m_iPara);
                iterData->second->SetProgressSet(stSet);
                iterData->second->AddProgress(1);
            }

            break;
        }
        case CLogicConfigDefine::ECET_UpLevelConstelMainAtr:
        case CLogicConfigDefine::ECET_UpStarsHeroCard:
        case CLogicConfigDefine::ECET_UpGradeHeroCard:
        {
            if (stEventConfig.m_iEventPara1 == stData.m_iPara && stData.m_iParaEx >= stEventConfig.m_iEventPara2)
            {
                iterData->second->AddProgress(stData.m_iTimes);
            }
            break;
        }
        case CLogicConfigDefine::ECET_FightPvPContinueWinTimes:
        {
            if (stData.m_iCount != 0)
            {
                iterData->second->SetProgress(0);
            } else
            {
                iterData->second->AddProgress(stData.m_iTimes);
            }
            break;
        }
        case CLogicConfigDefine::ECET_FightPvpWinInSecond:
        {
            if (stData.m_iPara < stEventConfig.m_iEventPara1)
            {
                iterData->second->AddProgress(stData.m_iTimes);
            }
            break;
        }
        case CLogicConfigDefine::ECET_FightPvpNotWinInSecond:
        {
            if (stData.m_iPara > stEventConfig.m_iEventPara1)
            {
                iterData->second->AddProgress(stData.m_iTimes);
            }
            break;
        }
        case CLogicConfigDefine::ECET_PutOnConstelSuit:
        {
            auto constelPutInfo = pUserInfo->m_stIBusyData.GetConstelPutInfo();
            if (constelPutInfo.find(stData.m_iPara) != constelPutInfo.end() && constelPutInfo.find(stData.m_iPara)->second.size() == 4)
            {
                iterData->second->AddProgress(1);
            }
            break;
        }

        default:
            continue;
        }

        if(iterData->second->GetState() == CLogicConfigDefine::ECQS_Accepted)
        {
            // 降序判断
            if(iterConfig->second->m_bIsDesc)
            {
                if(iterData->second->GetProgress() <= stEventConfig.m_iEventTarget)
                    iterData->second->SetState(CLogicConfigDefine::ECQS_Reached);
            }
            else
            {
                if(iterData->second->GetProgress() >= stEventConfig.m_iEventTarget)
                    iterData->second->SetState(CLogicConfigDefine::ECQS_Reached);
            }

            pUserInfo->m_stOnlyInCacheData.m_stCacheQuestIDSet.insert(iterData->first);

            // 如果任务完成，触发通用事件
            if(iterData->second->GetState() == CLogicConfigDefine::ECQS_Reached)
            {
                if(iterConfig->second->m_iCondIndex > 0)
                {
                    CLogicEventData stEventData;
                    stEventData.m_eType = CLogicConfigDefine::ECET_QuestCondIndexTimes;
                    stEventData.m_iPara = iterConfig->second->m_iCondIndex;
                    stEventData.m_iCmd = stData.m_iCmd;
                    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(pUserInfo, stEventData);
                }
            }
        }
    }
}

void CLogicCommonObserver::NotifyHomeLevelChangeEvent(user_data_table_ptr_type pUserInfo, const CLogicEventData& stData)
{
    auto homeLevelTag = pUserInfo->m_stIBusyData.GetHomeExpTag();
    if (homeLevelTag.find(stData.m_iPara) != homeLevelTag.end())
    {
        return;
    }
    auto homeLevelConfig = CLogicConfig::GetHomeLevelExpConfig();
    auto homeBuffConfig = CLogicConfig::GetHomeGoodsConfig().m_stHomeGoodBuffEffectMap;
    int32_t iHomeExp = 0;
    if (stData.m_eType == CLogicConfigDefine::ECET_GetHomeLevelItem)
    {
        auto itemExp = homeLevelConfig.m_stLevel2ItemExp.find(stData.m_iPara);
        if (itemExp == homeLevelConfig.m_stLevel2ItemExp.end())
        {
            return;
        }

        iHomeExp = itemExp->second.m_iHomeExp;
    } else if (stData.m_eType == CLogicConfigDefine::ECET_GetHomeLevelFish)
    {
        auto fishExp = homeLevelConfig.m_stLevel2FishExp.find(stData.m_iPara);
        if (fishExp == homeLevelConfig.m_stLevel2FishExp.end() || stData.m_iParaEx < fishExp->second.m_iNeedGrade)
        {
            return;
        }
        iHomeExp = fishExp->second.m_iHomeExp;
    } else if (stData.m_eType == CLogicConfigDefine::ECET_GetHomeBuffItem)
    {
        auto homeBuffIter = homeBuffConfig.find(stData.m_iPara);
        auto userBuffMap = pUserInfo->m_stIBusyData.GetHomeBuffID();
        if (homeBuffIter == homeBuffConfig.end())
        {
            return;
        }
        auto userBuffIter = userBuffMap.find(homeBuffIter->second.m_iBuffID);
        if (userBuffIter != userBuffMap.end())
        {
            return;
        }

        userBuffMap.insert(std::make_pair(homeBuffIter->second.m_iBuffID,homeBuffIter->second));
        pUserInfo->m_stIBusyData.SetHomeBuffID(userBuffMap);

        CNotifyHomeBuff homeBuffNotify;
        homeBuffNotify.m_iHomeItem.m_iCardID = stData.m_iPara;
        homeBuffNotify.m_iHomeItem.m_iNum = stData.m_iCount;
        LOGIC_CONNECTOR_SENDER->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_HOME_BUFF_CHANGE_NOTIFY, homeBuffNotify);
        return;
    }

    //标记该种物品已经获取过经验
    homeLevelTag.insert(std::make_pair(stData.m_iPara, true));
    pUserInfo->m_stIBusyData.SetHomeExpTag(homeLevelTag);

    CNotifyHomeLevelExp notify;
    notify.m_iExpLevel = iHomeExp;
    notify.m_iHomeItemID = stData.m_iPara;
    pUserInfo->m_stUserInfoTable.AddHomeExp(notify.m_iExpLevel);
    LOGIC_CONNECTOR_SENDER->NotifyClientUser(pUserInfo->m_iGroupID, pUserInfo->m_iUin, MSG_LOGIC_HOME_EXP_CHANGE_NOTIFY, notify);
}


