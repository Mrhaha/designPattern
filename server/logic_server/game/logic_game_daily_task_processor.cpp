#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "server_task_protocol.h"
#include "logic_link_common.h"
#include "logic_config.h"
#include "logic_game_item.h"
#include "logic_game_active.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_cache_common.h"
#include "logic_game_daily_task_processor.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_active_info.h"
#include "logic_game_global.h"
#include "logic_game_common_publisher.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_COMMON_DAILY_AWARD, CLogicDailyTaskProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_HERO_DATING, CLogicDailyTaskProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_QUEST_REWARD, CLogicDailyTaskProcessor)

CLogicDailyTaskProcessor::CLogicDailyTaskProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicDailyTaskProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if((int)m_usCmd == MSG_LOGIC_TOUHOU_COMMON_DAILY_AWARD)
    {
        CRequestCommonDailyAward stReq;
        CResponseCommonDailyAward stRsp;
        if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

        if(!CLogicConfigDefine::IsValidEnumDailyAwardStateType(stReq.m_iDailyAwardStatType))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        const uint64_t ulAwardState = m_pUserInfo->m_stUserDailyTable.GetDailyAwardState();
        const uint64_t ulFlag = 1ULL << stReq.m_iDailyAwardStatType;
        if (ulAwardState & ulFlag)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
        }

        switch (stReq.m_iDailyAwardStatType)
        {
        case CLogicConfigDefine::EDAST_HeartDialogAward:
            {
                // 好感度伙伴互动奖励
                auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(m_pUserInfo->m_stUserDailyTable.GetHeartDialogCard());
                if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
                {
                    using namespace SERVER_ERRCODE;
                    PROCESSOR_LOG_ERROR << "ERROR:" << HERO_HEART_LEVEL_LIMIT
                                        << "|MSG:" << GET_ERRMSG_BY_ERRCODE(HERO_HEART_LEVEL_LIMIT)
                                        << std::endl;

                    SEND_ERRCODE_AND_RET(HERO_HEART_LEVEL_LIMIT)
                }

                const auto iterConfig = CLogicConfig::GetHeroHeartConfig().m_stLevelAward.find(iterCard->second->GetHeartLevel());
                if(iterConfig == CLogicConfig::GetHeroHeartConfig().m_stLevelAward.end())
                {
                    using namespace SERVER_ERRCODE;
                    PROCESSOR_LOG_ERROR << "ERROR:" << HERO_HEART_LEVEL_LIMIT
                                        << "|MSG:" << GET_ERRMSG_BY_ERRCODE(HERO_HEART_LEVEL_LIMIT)
                                        << std::endl;

                    SEND_ERRCODE_AND_RET(HERO_HEART_LEVEL_LIMIT)
                }

                stRsp.m_stAwardItems = iterConfig->second.m_stDialogAward;
            }
            break;
        default:
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
            }
        }

        m_pUserInfo->m_stUserDailyTable.SetDailyAwardState(ulAwardState | ulFlag);

        stRsp.m_strDailyAwardState = int2bin(m_pUserInfo->m_stUserDailyTable.GetDailyAwardState());

        static std::string strBonus;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stAwardItems, strBonus, m_usCmd);

        PROCESSOR_LOG_DEBUG << "SUCCESS|BONUS_LIST:" << strBonus << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_TOUHOU_HERO_DATING)
    {
        /*
        CRequestHeroDating stReq;
        CResponseHeroDating stRsp;
        if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

        auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iCardID);
        if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End() || iterCard->second->GetCardLevel() <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED)
        }

        const auto pPlaceConfig = CLogicConfig::GetHeroDatingConfig().GetDatingPlaceConfig(stReq.m_iPlaceID);
        if(nullptr == pPlaceConfig || pPlaceConfig->m_ucDailyDatingTimes <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
        }

        auto stPlaceTimes = m_pUserInfo->m_stUserDailyTable.GetDatingPlaceTimes();
        if(stPlaceTimes.find(stReq.m_iPlaceID) != stPlaceTimes.end() && stPlaceTimes[stReq.m_iPlaceID] >= pPlaceConfig->m_ucDailyDatingTimes)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH)
        }

        if(iterCard->second->GetHeartLevel() < pPlaceConfig->m_iHeartLevelLimitMin || iterCard->second->GetHeartLevel() > pPlaceConfig->m_iHeartLevelLimitMax)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_HEART_LEVEL_LIMIT)
        }

        const auto pDialogConfig = CLogicConfig::GetHeroDatingDialogConfig().GetConfig(stReq.m_iCardID, stReq.m_iPlaceID, stReq.m_iDialogID);
        if(nullptr == pDialogConfig)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
        }

        stRsp.m_iCardID = stReq.m_iCardID;
        stRsp.m_iPlaceID = stReq.m_iPlaceID;
        stRsp.m_iDialogID = stReq.m_iDialogID;
        stRsp.m_iConsumeItemID = stReq.m_iConsumeItemID;

        int32_t iTotalAddExp = CLogicCommon::GetRandNum(pPlaceConfig->m_iHeartExpMin, pPlaceConfig->m_iHeartExpMax);
        std::vector<TLogicHeroImpression> stImpressionReward;

        // 特殊对话
        if(stReq.m_iConsumeItemID > 0)
        {
            if(std::find(pPlaceConfig->m_stDatingItems.begin(), pPlaceConfig->m_stDatingItems.end(), stReq.m_iConsumeItemID) == pPlaceConfig->m_stDatingItems.end())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
            }

            const auto pItemConfig = CLogicConfig::GetHeroDatingConfig().GetDatingItemConfig(stReq.m_iConsumeItemID);
            if(nullptr == pItemConfig)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
            }

            // 消耗特殊道具
            if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, stReq.m_iConsumeItemID, 1, m_usCmd))
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode() << "|MSG:" << CLogicError::GetErrMsg() << std::endl;

                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }

            iTotalAddExp += pItemConfig->m_iHeartExp;
            stRsp.m_stReward = pItemConfig->m_stRewards;
            stImpressionReward = pItemConfig->m_stImpression;
        }
        else
        {
            // 普通对话
            const auto iterOptionConfig = pDialogConfig->m_stOptionImpression.find(stReq.m_iAnswerPara);
            if(iterOptionConfig == pDialogConfig->m_stOptionImpression.end())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
            }

            stImpressionReward = iterOptionConfig->second;
        }

        // 增加印象分
        auto stImpressionData = iterCard->second->GetImpression();
        for(auto imp : stImpressionReward)
        {
            if(!CLogicConfigDefine::IsValidEnumHeroImpressionType(imp.m_iImpressionType))
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
            }

            if(imp.m_iImpressionType > int(stImpressionData.size()))
            {
                stImpressionData.resize(imp.m_iImpressionType);
            }

            stImpressionData[imp.m_iImpressionType - 1] += CLogicCommon::GetRandNum(imp.m_iImpressionMinNum, imp.m_iImpressionMaxNum, true);
            if(stImpressionData[imp.m_iImpressionType - 1] > CLogicConfig::GetHeroDatingConfig().m_iImpressionMaxNum)
                stImpressionData[imp.m_iImpressionType - 1] = CLogicConfig::GetHeroDatingConfig().m_iImpressionMaxNum;
        }

        iterCard->second->SetImpression(stImpressionData);

        if(stPlaceTimes.find(stReq.m_iPlaceID) == stPlaceTimes.end())
        {
            stPlaceTimes[stReq.m_iPlaceID] = 1;
        }
        else
        {
            stPlaceTimes[stReq.m_iPlaceID] += 1;
        }

        m_pUserInfo->m_stUserDailyTable.SetDatingPlaceTimes(stPlaceTimes);

        int32_t iCurLevel = iterCard->second->GetHeartLevel();
        int32_t iNewLevel = iCurLevel;
        int32_t iNewExp = iterCard->second->GetHeartExp() + iTotalAddExp;
        CLogicConfig::GetHeroHeartConfig().ReCalcHeartLevelExp(iNewLevel, iNewExp);
        iterCard->second->SetHeartLevel(iNewLevel);
        iterCard->second->SetHeartExp(iNewExp);
        iterCard->second->AddDatingTimes(1);

        auto pTimesReward = CLogicConfig::GetHeroDatingConfig().GetDatingTimesReward(stReq.m_iCardID, iterCard->second->GetDatingTimes());
        if(nullptr != pTimesReward)
        {
            stRsp.m_stReward.insert(stRsp.m_stReward.end(), pTimesReward->begin(), pTimesReward->end());
        }

        static std::string strBonus;
        if(!stRsp.m_stReward.empty()) CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stReward, strBonus, m_usCmd);

        stRsp.m_iPlaceDailyTimes = stPlaceTimes[stReq.m_iPlaceID];
        stRsp.m_iHeartLevel = iterCard->second->GetHeartLevel();
        stRsp.m_iHeartExp = iterCard->second->GetHeartExp();
        stRsp.m_stImpression = iterCard->second->GetImpression();

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        // 更新伙伴传记
        CLogicGlobal::UpdateHeroHistory(m_pUserInfo, stReq.m_iCardID);

        CLogicDailyTaskSystem::NotifyTask(m_pUserInfo, CLogicDailyTaskSystem::HERO_HEART_DATING, stReq.m_iCardID, 1);

        if(iCurLevel != iNewLevel)
        {
            CLogicDailyTaskSystem::SetProgress(m_pUserInfo, CLogicDailyTaskSystem::HERO_HEART_LEVEL, stReq.m_iCardID, iNewLevel);

            // 好感度基金
            CLogicActiveBase::Notify(m_pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_HEART_INSURE, 1);
        }

        PROCESSOR_LOG_DEBUG << "SUCCESS|BONUS_LIST:" << strBonus << "|HEART_LEVEL:" << stRsp.m_iHeartLevel << "|HEART_EXP:" << stRsp.m_iHeartExp << std::endl;

        return (true);
         */
    }
    else if((int)m_usCmd == MSG_LOGIC_QUEST_REWARD)
    {
        CRequestQuestReward stReq;
        CResponseQuestReward stRsp;
        if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

        int32_t iRetCode = SERVER_ERRCODE::SUCCESS;
        std::string strIDVec = "{";
        CPackQuestData stPack;
        std::vector<CPackGameItem> stRewards;

        for(auto& id : stReq.m_stQuestIDVec)
        {
            iRetCode = HandleQuestReward(id, stRewards, stPack);
            if(SERVER_ERRCODE::SUCCESS == iRetCode)
            {
                stRsp.m_stQuestVec.push_back(stPack);
                strIDVec += std::to_string(id) + "-";
            }
        }

        strIDVec[strIDVec.length() - 1] = '}';

        if(stRsp.m_stQuestVec.empty())
        {
            SEND_ERRCODE_AND_RET(iRetCode)
        }

        static std::string strBonus;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, strBonus, m_usCmd);

        PROCESSOR_LOG_DEBUG << "SUCCESS|QUEST_ID_LIST:" << strIDVec << "|BONUS_LIST:" << strBonus << std::endl;

        stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }

    return (true);
}

int32_t CLogicDailyTaskProcessor::HandleQuestReward(int32_t iQuestID, std::vector<CPackGameItem>& stRewards, CPackQuestData& stRetData)
{
    auto pConfig = CLogicConfig::GetQuestConfig().GetConfig(iQuestID);
    if(nullptr == pConfig)
    {
        return SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST;
    }

    if(pConfig->m_iLevelLimit > m_pUserInfo->m_stUserInfoTable.GetLevel())
    {
        return SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL;
    }

    if(pConfig->m_iAvyID > 0 && !CLogicActive::IsAvyOpen(pConfig->m_iAvyID, m_pUserInfo->m_stUserInfoTable.GetCreateTime()))
    {
        return SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN;
    }

    // 临时限制
    if(pConfig->m_iOpenHour > CLogicCommon::GetDate().tm_hour)
    {
        return SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN;
    }

    auto iterData = m_pUserInfo->m_stQuestMap.Find(iQuestID);
    if(iterData == m_pUserInfo->m_stQuestMap.End())
    {
        // 空事件可直接达成领奖
        if(pConfig->m_iEventType == CLogicConfigDefine::ECET_Empty)
        {
            TUserQuestTableValueType stValue;
            stValue.m_iUid = m_pUserInfo->m_iUin;
            stValue.m_iGroupID = m_pUserInfo->m_iGroupID;
            stValue.m_iQuestID = iQuestID;
            stValue.m_cState = CLogicConfigDefine::ECQS_Accepted;
            stValue.m_iParaIndex = 0;
            stValue.m_iProgress = 0;
            auto stRet = m_pUserInfo->m_stQuestMap.Insert(std::make_pair(stValue.m_iQuestID, stValue));
            if (stRet.second == false)
            {
                return SERVER_ERRCODE::INTERN_ERROR;
            }

            iterData = stRet.first;
        }
        else
        {
            return SERVER_ERRCODE::DAILY_TASK_NOT_COMPLETED;
        }
    }

    // 已领取
    if(iterData->second->GetState() == CLogicConfigDefine::ECQS_Received)
    {
        return SERVER_ERRCODE::DAILY_TASK_HAS_ACHIEVED_TODAY;
    }

    // 空事件任务可直接完成
    if(pConfig->m_iEventType == CLogicConfigDefine::ECET_Empty && iterData->second->GetState() == CLogicConfigDefine::ECQS_Accepted)
    {
        iterData->second->AddProgress(1);
        iterData->second->SetState(CLogicConfigDefine::ECQS_Reached);
    }

    if(iterData->second->GetState() != CLogicConfigDefine::ECQS_Reached)
    {
        return SERVER_ERRCODE::DAILY_TASK_NOT_COMPLETED;
    }

    if(iterData->second->GetParaIndex() >= int(pConfig->m_stEventVec.size()))
    {
        return SERVER_ERRCODE::INTERN_ERROR;
    }

    auto& stEventConfig = pConfig->m_stEventVec[iterData->second->GetParaIndex()];
    stRewards.insert(stRewards.end(), stEventConfig.m_stReward.begin(), stEventConfig.m_stReward.end());

    // 多阶段性任务
    if(pConfig->m_bIsMult && iterData->second->GetParaIndex() + 1 < int(pConfig->m_stEventVec.size()))
    {
        // 新阶段
        iterData->second->AddParaIndex(1);
        auto& stNewEventConfig = pConfig->m_stEventVec[iterData->second->GetParaIndex()];

        // 特殊任务事件更新
        UpdateSpecialQuestProgress(iterData->second, pConfig->m_iEventType, stNewEventConfig, stEventConfig);

        // 更新任务状态
        iterData->second->SetState(CLogicConfigDefine::ECQS_Accepted);
        // 降序判断
        if(pConfig->m_bIsDesc)
        {
            if(iterData->second->GetProgress() <= stNewEventConfig.m_iEventTarget)
                iterData->second->SetState(CLogicConfigDefine::ECQS_Reached);
        }
        else
        {
            if(iterData->second->GetProgress() >= stNewEventConfig.m_iEventTarget)
                iterData->second->SetState(CLogicConfigDefine::ECQS_Reached);
        }
    }
    else
    {
        // 设置已领取
        iterData->second->SetState(CLogicConfigDefine::ECQS_Received);
    }

    // 返回最新任务数据
    stRetData.m_iQuestID = iQuestID;
    stRetData.m_iIndex = iterData->second->GetParaIndex();
    stRetData.m_iProgress = iterData->second->GetProgress();
    stRetData.m_cState = iterData->second->GetState();

    return SERVER_ERRCODE::SUCCESS;
}

void CLogicDailyTaskProcessor::UpdateSpecialQuestProgress(TUserQuestTable* stQuestData, int32_t iEventType, const TLogicQuestEventPara& stNewConfig, const TLogicQuestEventPara& stOldConfig)
{
    switch (iEventType)
    {
    case CLogicConfigDefine::ECET_PvELevelFirstFight:
    {
        if(stNewConfig.m_iEventPara1 > 0)
        {
            auto iter = m_pUserInfo->m_stLevelInfoTableMap.Find(stNewConfig.m_iEventPara1);
            if(iter == m_pUserInfo->m_stLevelInfoTableMap.End() || iter->second->GetTotalVictoryTimes() <= 0)
                stQuestData->SetProgress(0);
        }
        else
        {
            stQuestData->SetProgress(0);
            for(auto level_id : stNewConfig.m_stParaSet)
            {
                auto iter = m_pUserInfo->m_stLevelInfoTableMap.Find(level_id);
                if(iter != m_pUserInfo->m_stLevelInfoTableMap.End() && iter->second->GetTotalVictoryTimes() > 0)
                {
                    stQuestData->SetProgress(1);
                    break;
                }
            }
        }

        break;
    }
    case CLogicConfigDefine::ECET_UpgradeCardColor:
    {
        if(stNewConfig.m_iEventPara1 != stOldConfig.m_iEventPara1)
        {
            int iNewProgress = 0;
            for(auto iter = m_pUserInfo->m_stHeroCardTableMap.Begin(); iter != m_pUserInfo->m_stHeroCardTableMap.End(); ++iter)
            {
                if(iter->second->GetCardColor() >= stNewConfig.m_iEventPara1)
                    ++iNewProgress;
            }

            stQuestData->SetProgress(iNewProgress);
        }
        break;
    }
    case CLogicConfigDefine::ECET_UpgradeCardLevel:
    {
        if(stNewConfig.m_iEventPara1 != stOldConfig.m_iEventPara1)
        {
            int iNewProgress = 0;
            for(auto iter = m_pUserInfo->m_stHeroCardTableMap.Begin(); iter != m_pUserInfo->m_stHeroCardTableMap.End(); ++iter)
            {
                if(iter->second->GetCardLevel() >= stNewConfig.m_iEventPara1)
                    ++iNewProgress;
            }

            stQuestData->SetProgress(iNewProgress);
        }
        break;
    }
    case CLogicConfigDefine::ECET_CardHeartLevel:
    {
        if(stNewConfig.m_iEventPara1 != stOldConfig.m_iEventPara1)
        {
            int iNewProgress = 0;
            for(auto iter = m_pUserInfo->m_stHeroCardTableMap.Begin(); iter != m_pUserInfo->m_stHeroCardTableMap.End(); ++iter)
            {
                if(iter->second->GetHeartLevel() >= stNewConfig.m_iEventPara1)
                    ++iNewProgress;
            }

            stQuestData->SetProgress(iNewProgress);
        }
        break;
    }
    case CLogicConfigDefine::ECET_ConstelLevelUp:
    {
        if(stNewConfig.m_iEventPara1 != stOldConfig.m_iEventPara1)
        {
            int iNewProgress = 0;
            for(auto iter = m_pUserInfo->m_stConstelMap.Begin(); iter != m_pUserInfo->m_stConstelMap.End(); ++iter)
            {
                if(iter->second->GetLevel() >= stNewConfig.m_iEventPara1)
                    ++iNewProgress;
            }

            stQuestData->SetProgress(iNewProgress);
        }
        break;
    }
    case CLogicConfigDefine::ECET_CardGradeNum:
    {
        if(stNewConfig.m_iEventPara1 != stOldConfig.m_iEventPara1)
        {
            int iNewProgress = 0;
            for(auto iter = m_pUserInfo->m_stHeroCardTableMap.Begin(); iter != m_pUserInfo->m_stHeroCardTableMap.End(); ++iter)
            {
                if(iter->second->GetGradeLevel() >= stNewConfig.m_iEventPara1)
                    ++iNewProgress;
            }

            stQuestData->SetProgress(iNewProgress);
        }
        break;
    }
    case CLogicConfigDefine::ECET_UpgradeCardStar:
    {
        if(stNewConfig.m_iEventPara1 != stOldConfig.m_iEventPara1)
        {
            int iNewProgress = 0;
            for(auto iter = m_pUserInfo->m_stHeroCardTableMap.Begin(); iter != m_pUserInfo->m_stHeroCardTableMap.End(); ++iter)
            {
                if(iter->second->GetCardStar() >= stNewConfig.m_iEventPara1)
                    ++iNewProgress;
            }

            stQuestData->SetProgress(iNewProgress);
        }
        break;
    }
    default:
    {
        break;
    }
    }
}
