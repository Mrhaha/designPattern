#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_pvp_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_game_pvp_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_global.h"
#include "logic_game_cache_table.h"
#include "logic_game_common_publisher.h"
#include "logic_record_rank_skiplist.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"
#include "logic_game_user_notify_system.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_SELF_PVP_INFO, CLogicPVPProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_PVP_ENEMY_LIST, CLogicPVPProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CHANGE_PVP_ENEMY, CLogicPVPProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_PVP, CLogicPVPProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_PVP_RESULT, CLogicPVPProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_PVP_SWEEP, CLogicPVPProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_PVP_FIGHT_HISTORY, CLogicPVPProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_PVP_CONTINUE_WIN_REWARD, CLogicPVPProcessor)





CLogicPVPProcessor::CLogicPVPProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

int CLogicPVPProcessor::SendHighestPVPRankMail(int iHighestRank)
{
    const auto& stConfig = CLogicConfig::GetHighestRecordConfig().m_stPVP;
    int32_t iOldRank = m_pUserInfo->m_stUserInfoTable.GetPvpHighestRank();
    int32_t iTotalAwardAmount = 0;
    if (0 == iOldRank || iOldRank > iHighestRank)
    {
        m_pUserInfo->m_stUserInfoTable.SetPvpHighestRank(iHighestRank);

        auto iterBegin = stConfig.m_stRecordMap.lower_bound(iHighestRank);
        auto iterEnd = stConfig.m_stRecordMap.end();
        if(iOldRank > 0)
            iterEnd = stConfig.m_stRecordMap.lower_bound(iOldRank);

        for(; iterBegin != iterEnd; ++iterBegin)
        {
            int iMin = std::max(iHighestRank, iterBegin->second.first);
            iTotalAwardAmount += ((iterBegin->first - iMin + 1) * iterBegin->second.second);
        }

        if(iterEnd != stConfig.m_stRecordMap.end())
        {
            int iMin = std::max(iHighestRank, iterEnd->second.first);
            int iMax = std::min(iOldRank, iterEnd->first);
            iTotalAwardAmount += ((iMax - iMin) * iterEnd->second.second);
        }
    }

    if(iTotalAwardAmount > 0)
    {
        std::map<std::string, std::string> stTemplateMap;
        stTemplateMap["rank"] = std::to_string(iHighestRank);
        //特判第一名替换邮件文本内容
        std::string strMailContent;
        if (iHighestRank == 1) {
            strMailContent = stConfig.m_stFirstMailContentTemplateObj.Replace({});
        } else {
            strMailContent = stConfig.m_stMailContentTemplateObj.Replace(stTemplateMap);
        }
        CPackGameItem stGameItem;
        stGameItem.m_iItemType = stConfig.m_iBonusItemType;
        stGameItem.m_iCardID = stConfig.m_iBonusItemID;
        stGameItem.m_iNum = iTotalAwardAmount;

        CLogicGlobal::AddAsyncUserMail(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, stConfig.m_strMailSender, stConfig.m_strMailTitle,
                                       strMailContent, {stGameItem}, "SEND_HIGHEST_PVP_RANK_MAIL");
    }

    return iTotalAwardAmount;
}

bool 
CLogicPVPProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if((int)m_usCmd == MSG_LOGIC_GET_SELF_PVP_INFO)
    {
        CResponseGetPvpInfo stRsp;
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;
        stKey.m_stRoleIDDetail.m_iUin = m_iUin;

        int curRank = (int) LOGIC_PVP_RANK_INS.GetRank(stKey.m_ulRoleID);
        if(curRank > 0 && m_pUserInfo->m_stUserInfoTable.GetPvpVictoryTimes() > 0)
        {
            stRsp.m_iPVPRank = curRank;
        }
        else
        {
            stRsp.m_iPVPRank = LOGIC_PVP_RANK_INS.max_rank()+1;
        }

        auto questConfig = CLogicConfig::GetQuestConfig().m_stPvpWeeklyQuestList;
        if (m_pUserInfo->m_stUserInfoTable.GetPvpWeeklyTask().empty() && !questConfig.empty())
        {
            std::vector<int32_t> stRandQuest;
            std::set<int32_t> stSelectIndex;
            for (auto i = 0; i != CLogicConfigDefine::ESD_PvpWeeklyTaskNumber;)
            {
                auto index = CLogicCommon::GetRandNum(0,questConfig.size());
                auto iterSet = stSelectIndex.find(index);
                if (iterSet != stSelectIndex.end())
                {
                    continue;
                }
                stSelectIndex.insert(index);
                stRandQuest.emplace_back(questConfig[index]);
                i++;
            }
            m_pUserInfo->m_stUserInfoTable.SetPvpWeeklyTask(stRandQuest);
        }

        stRsp.m_iPVPWeeklyQuestID = m_pUserInfo->m_stUserInfoTable.GetPvpWeeklyTask();
        stRsp.m_iPVPMaxScore = m_pUserInfo->m_stUserInfoTable.GetPvpHighestScore();
        stRsp.m_iPVPScore = m_pUserInfo->m_stUserInfoTable.GetPvpScore();
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_PVP_ENEMY_LIST)
    {
        CResponseGetPvpChallengerList stRsp;
        stRsp.m_stCurrRewardState = m_pUserInfo->m_stOnlyInCacheData.m_stPVPRewardState;

        if (!m_pUserInfo->m_stOnlyInCacheData.m_stPVPUserInfo.empty())
        {
            stRsp.m_stChallengerList = m_pUserInfo->m_stOnlyInCacheData.m_stPVPUserInfo;
            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
            return true;
        } else
        {
            auto ret = FindChallenger(&stRsp.m_stChallengerList);
            if (ret != SERVER_ERRCODE::SUCCESS)
            {
                SEND_ERRCODE_AND_RET(ret)
            } else
            {
                m_pUserInfo->m_stOnlyInCacheData.m_stPVPUserInfo = stRsp.m_stChallengerList;
                SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
                return true;
            }
        }
    }
    else if((int)m_usCmd == MSG_LOGIC_CHANGE_PVP_ENEMY)
    {
        CResponseFlushPvpChallenger stRsp;
        auto ret = FindChallenger(&stRsp.m_stChallengerList);
        if (ret != SERVER_ERRCODE::SUCCESS)
        {
            SEND_ERRCODE_AND_RET(ret)
        } else
        {
            //记录到内存
            m_pUserInfo->m_stOnlyInCacheData.m_stPVPUserInfo = stRsp.m_stChallengerList;
            //清空连胜奖励记录
            m_pUserInfo->m_stOnlyInCacheData.m_stPVPRewardState.clear();
            m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterWinCount = 0;
            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        }
        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_FIGHT_PVP)
    {
        CRequestFightPvp stParam;
        CResponseFightPvp stRsp;
        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        if(stParam.m_iUin == m_iUin)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }


		auto challenge_player = CLogicCacheCommon::GetPvpChallengerInfo(stParam.m_iGroupID, stParam.m_iUin);
        if(challenge_player.m_stBriefInfo.m_iUid <= 0 && stParam.m_iGroupID != 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterGroupID = stParam.m_iGroupID;
        m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterUin = stParam.m_iUin;

		stRsp.m_stChallengerInfo = std::move(challenge_player);
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_FightPvPTimes;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

        //buff次数减少
        CLogicBuff::ReduceBuffTimes(m_pUserInfo,CLogicConfigDefine::EBT_AddAttr);
        CLogicUserNotifySystem::NotifyUserBuffData(m_pUserInfo);
        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_FIGHT_PVP_RESULT)
    {
        CRequestFightPvpResult stParam;
        CResponseFightPvpResult stRsp;
        if(!DecodeMsgPack(stFormatData, stParam)) return (false);

        if(stParam.m_iUin <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        if(stParam.m_iGroupID != m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterGroupID ||
            stParam.m_iUin != m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterUin)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FIGHT_PVP_MISMATCH)
        }

        auto pvpConfig = CLogicConfig::GetPVPConfig();
        std::vector<CPackGameItem> stRewards;
        static std::string strBonus;

        int iOtherGroupID = m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterGroupID;
        int iOtherUin = m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterUin;
        // 胜利
        if(stParam.m_stFightLog.m_bWin)
        {
            // 只有胜利才消耗道具
            int32_t iConsumeItemID = CLogicConfig::GetPVPConfig().m_iConsumeItemID;
            auto itemConfig = CLogicConfig::GetItemConfig().GetConfig(iConsumeItemID);
            if (itemConfig == nullptr)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
            }
            if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM,itemConfig->m_iItemID,1, m_usCmd))
            {
                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }

            int32_t curOtherPvpScore = 0;
            auto pOtherUserData = CLogicCacheCommon::LoadUserData(stParam.m_iGroupID, stParam.m_iUin);
            if (pOtherUserData)
            {
                curOtherPvpScore =  pOtherUserData->m_stUserInfoTable.GetPvpScore();
                if (curOtherPvpScore-pvpConfig.m_iDefendLose <= 0)
                {
                    pOtherUserData->m_stUserInfoTable.SetPvpScore(0);
                } else
                {
                    pOtherUserData->m_stUserInfoTable.SetPvpScore(curOtherPvpScore-pvpConfig.m_iDefendLose);
                }
                LOGIC_RECORD_CACHE_INS->ExecSQL(pOtherUserData->m_iGroupID, pOtherUserData->m_iUin, m_strCmdName);
                auto iOtherResultRank = CLogicGlobal::UpdateOrInsertPvpRank(iOtherUin, iOtherGroupID, pOtherUserData->m_stUserInfoTable.GetPvpScore());

                stParam.m_stFightLog.m_iDefendRankChange = pOtherUserData->m_stUserInfoTable.GetPvpScore() - curOtherPvpScore;
                stParam.m_stFightLog.m_iDefendRank = iOtherResultRank;
                stParam.m_stFightLog.m_iDefendCurrScore = pOtherUserData->m_stUserInfoTable.GetPvpScore();
            } else
            {
                //机器人的话分数直接读表配置
                auto stRobotConfig = CLogicConfig::GetPVPRobotConfig().GetConfig(iOtherUin,iOtherGroupID);
                if (stRobotConfig)
                {
                    curOtherPvpScore = stRobotConfig->m_iPvpScore;
                    CLogicGlobal::UpdateOrInsertPvpRank(iOtherUin, iOtherGroupID, curOtherPvpScore);
                }
            }

            auto selfOldPvpScore = m_pUserInfo->m_stUserInfoTable.GetPvpScore();
            auto selfCurPvpScore = selfOldPvpScore;
            if (selfOldPvpScore < curOtherPvpScore)
            {
                selfCurPvpScore = selfOldPvpScore + pvpConfig.m_iWinUpper;
                m_pUserInfo->m_stUserInfoTable.SetPvpScore(selfCurPvpScore);
                RandomPVPReward(pvpConfig.m_stPvpReward[0].m_stLogicReward,&stRewards);
            } else if (selfOldPvpScore == curOtherPvpScore)
            {
                selfCurPvpScore = selfOldPvpScore + pvpConfig.m_iWinEqual;
                m_pUserInfo->m_stUserInfoTable.SetPvpScore(selfCurPvpScore);
                RandomPVPReward(pvpConfig.m_stPvpReward[1].m_stLogicReward,&stRewards);
            } else
            {
                selfCurPvpScore = selfOldPvpScore + pvpConfig.m_iWinLower;
                m_pUserInfo->m_stUserInfoTable.SetPvpScore(selfCurPvpScore);
                RandomPVPReward(pvpConfig.m_stPvpReward[2].m_stLogicReward,&stRewards);
            }
            auto iSelfResultRank = CLogicGlobal::UpdateOrInsertPvpRank(m_iUin, m_iGroupID, selfCurPvpScore);

            m_pUserInfo->m_stUserInfoTable.AddPvpVictoryTimes(1);
            m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterWinCount += 1;

            //更新历史最高积分
            if (selfCurPvpScore >= m_pUserInfo->m_stUserInfoTable.GetPvpHighestScore())
            {
                m_pUserInfo->m_stUserInfoTable.SetPvpHighestScore(selfCurPvpScore);
            }

            stRsp.m_iNewRank = m_pUserInfo->m_stUserInfoTable.GetPvpScore();
            stRsp.m_iOldRank = selfOldPvpScore;

            // 排名变化
            stParam.m_stFightLog.m_iAttackRankChange = m_pUserInfo->m_stUserInfoTable.GetPvpScore() - selfOldPvpScore;
            stParam.m_stFightLog.m_iAttackRank = iSelfResultRank;
            stParam.m_stFightLog.m_iAttackCurrScore = m_pUserInfo->m_stUserInfoTable.GetPvpScore();

            //修改对手列表
            for (auto& userInfo :m_pUserInfo->m_stOnlyInCacheData.m_stPVPUserInfo)
            {
                if(userInfo.m_stInfo.m_stBriefInfo.m_iUid == stParam.m_iUin && userInfo.m_stInfo.m_stBriefInfo.m_iGroupID == stParam.m_iGroupID)
                {
                    userInfo.m_bHasChallenge = true;
                }
            }
            // 通用事件
            CLogicEventData stWinEventData;
            stWinEventData.m_eType = CLogicConfigDefine::ECET_FightPvPContinueWinTimes;
            stWinEventData.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stWinEventData);

            stWinEventData.m_eType = CLogicConfigDefine::ECET_FightPvPWinTimes;
            stWinEventData.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stWinEventData);

            // 通用事件
            std::map<int32_t,int32_t> stCardTypeNumber;
            int32_t iAliveNum = 0;
            for (auto carLog : stParam.m_stFightLog.m_stAttackLog.m_stCardLogVec)
            {
                auto cardType =  CLogicConfig::GetHeroCardConfig().GetConfig(carLog.m_iCardID)->m_iCardType;
                stCardTypeNumber[cardType]++;

                if (carLog.m_bIsAlive)
                {

                    iAliveNum++;
                }
            }
            for (auto iterCard : stCardTypeNumber)
            {
                if (iterCard.second == (int32_t)stParam.m_stFightLog.m_stAttackLog.m_stCardLogVec.size())
                {
                    CLogicEventData stTypeWinEventData;
                    stTypeWinEventData.m_eType = CLogicConfigDefine::ECET_FightPvpAllSameType;
                    stTypeWinEventData.m_iCmd = m_usCmd;
                    stTypeWinEventData.m_iPara = iterCard.first;
                    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stTypeWinEventData);
                }
            }
            for (auto cardType = CLogicConfigDefine::EnumCardType::ECT_Attack; cardType != CLogicConfigDefine::EnumCardType::ECT_Max; )
            {
                if (stCardTypeNumber.find(cardType) == stCardTypeNumber.end())
                {
                    CLogicEventData stTypeNotInEventData;
                    stTypeNotInEventData.m_eType = CLogicConfigDefine::ECET_FightPvpNotJoinType;
                    stTypeNotInEventData.m_iCmd = m_usCmd;
                    stTypeNotInEventData.m_iPara = cardType;
                    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stTypeNotInEventData);
                }
                cardType = (CLogicConfigDefine::EnumCardType)((int)cardType + 1);
            }


            CLogicEventData stTypeAliveEventData;
            stTypeAliveEventData.m_eType = CLogicConfigDefine::ECET_FightPvpLeftAliveWin;
            stTypeAliveEventData.m_iCmd = m_usCmd;
            stTypeAliveEventData.m_iPara = iAliveNum;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stTypeAliveEventData);

            CLogicEventData stTypeSecondEventData;
            stTypeSecondEventData.m_eType = CLogicConfigDefine::ECET_FightPvpWinInSecond;
            stTypeSecondEventData.m_iCmd = m_usCmd;
            stTypeSecondEventData.m_iPara = stParam.m_stFightLog.m_iDurationTime;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stTypeSecondEventData);

            // 文文日报
            std::vector<std::string> stWenWenParam;
            stWenWenParam.push_back(m_pUserInfo->m_strNick);
            stWenWenParam.push_back(to_string(selfCurPvpScore));
            CLogicGlobal::UpdateWenWenNewsEvent(m_iUin, m_iGroupID, CLogicConfigDefine::EWWNT_PVP_Score, stWenWenParam);
        }
        else
        {
            int32_t curOtherPvpScore = 0;
            auto pOtherUserData = CLogicCacheCommon::LoadUserData(stParam.m_iGroupID, stParam.m_iUin);
            if (pOtherUserData)
            {
                curOtherPvpScore =  pOtherUserData->m_stUserInfoTable.GetPvpScore();
                pOtherUserData->m_stUserInfoTable.SetPvpScore(curOtherPvpScore+pvpConfig.m_iDefendWin);
                LOGIC_RECORD_CACHE_INS->ExecSQL(pOtherUserData->m_iGroupID, pOtherUserData->m_iUin, m_strCmdName);

                auto iOtherResultRank = CLogicGlobal::UpdateOrInsertPvpRank(iOtherUin, iOtherGroupID, pOtherUserData->m_stUserInfoTable.GetPvpScore());
                stParam.m_stFightLog.m_iDefendRankChange = pOtherUserData->m_stUserInfoTable.GetPvpScore() - curOtherPvpScore;
                stParam.m_stFightLog.m_iDefendRank = iOtherResultRank;
                stParam.m_stFightLog.m_iDefendCurrScore = pOtherUserData->m_stUserInfoTable.GetPvpScore();
            } else
            {
                //机器人的话分数直接读表配置
                auto stRobotConfig = CLogicConfig::GetPVPRobotConfig().GetConfig(iOtherUin,iOtherGroupID);
                if (stRobotConfig)
                {
                    CLogicGlobal::UpdateOrInsertPvpRank(iOtherUin, iOtherGroupID, stRobotConfig->m_iPvpScore);
                }
            }
            auto selfOldPvpScore = m_pUserInfo->m_stUserInfoTable.GetPvpScore();
            auto selfCurPvpScore = selfOldPvpScore-pvpConfig.m_iLose;
            selfCurPvpScore = selfCurPvpScore <= 0 ? 0:selfCurPvpScore;
            m_pUserInfo->m_stUserInfoTable.SetPvpScore(selfCurPvpScore);

            auto iResultRank = CLogicGlobal::UpdateOrInsertPvpRank(m_iUin, m_iGroupID, selfCurPvpScore);
            stParam.m_stFightLog.m_iAttackRankChange = selfCurPvpScore - selfOldPvpScore;
            stParam.m_stFightLog.m_iAttackRank = iResultRank;

            RandomPVPReward(pvpConfig.m_stPvpReward[3].m_stLogicReward,&stRewards);
            stRsp.m_iNewRank = m_pUserInfo->m_stUserInfoTable.GetPvpScore();
            stRsp.m_iOldRank = selfOldPvpScore;

            int32_t iAliveNum = 0;
            for (auto carLog : stParam.m_stFightLog.m_stDefendLog.m_stCardLogVec)
            {
                if (carLog.m_bIsAlive)
                {
                    iAliveNum++;
                }
            }

            CLogicEventData stTypeAliveEventData;
            stTypeAliveEventData.m_eType = CLogicConfigDefine::ECET_FightPvpLeftAliveLose;
            stTypeAliveEventData.m_iCmd = m_usCmd;
            stTypeAliveEventData.m_iPara = iAliveNum;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stTypeAliveEventData);
            // 通用事件
            CLogicEventData stWinEventData;
            stWinEventData.m_eType = CLogicConfigDefine::ECET_FightPvPContinueWinTimes;
            stWinEventData.m_iCmd = m_usCmd;
            stWinEventData.m_iCount = 1;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stWinEventData);
        }

        CLogicEventData stTypeSecondEventData;
        stTypeSecondEventData.m_eType = CLogicConfigDefine::ECET_FightPvpNotWinInSecond;
        stTypeSecondEventData.m_iCmd = m_usCmd;
        stTypeSecondEventData.m_iPara = stParam.m_stFightLog.m_iDurationTime;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stTypeSecondEventData);

        m_pUserInfo->m_stUserInfoTable.AddPvpFightTimes(1);

        m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterGroupID = 0;
        m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterUin = 0;

        //添加奖励
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, strBonus, m_usCmd);
        stRsp.m_stPvpReward = m_pUserInfo->m_stCacheBonusData;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        //存储对战日志
        stParam.m_stFightLog.m_iTime = CLogicCommon::GetSec();
        m_pUserInfo->m_stOnlyInCacheData.m_stPVPFightHistory.push_front(stParam.m_stFightLog);
        if (m_pUserInfo->m_stOnlyInCacheData.m_stPVPFightHistory.size() > CLogicConfigDefine::ESD_PvpHistoryMaxNum)
        {
            m_pUserInfo->m_stOnlyInCacheData.m_stPVPFightHistory.pop_back();
        }

        //机器人不用存记录
        if (stParam.m_iGroupID > 0)
        {
            auto pOtherUserData = CLogicCacheCommon::LoadUserData(stParam.m_iGroupID, stParam.m_iUin);
            if (pOtherUserData)
            {
                pOtherUserData->m_stOnlyInCacheData.m_stPVPFightHistory.push_front(stParam.m_stFightLog);
                if(pOtherUserData->m_stOnlyInCacheData.m_stPVPFightHistory.size() > CLogicConfigDefine::ESD_PvpHistoryMaxNum)
                {
                    pOtherUserData->m_stOnlyInCacheData.m_stPVPFightHistory.pop_back();
                }
            }
        }
        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_PVP_FIGHT_HISTORY)
    {
        CResponseGetPvpFightHistory stRsp;
        for(auto stIter = m_pUserInfo->m_stOnlyInCacheData.m_stPVPFightHistory.begin();
             stIter != m_pUserInfo->m_stOnlyInCacheData.m_stPVPFightHistory.end(); ++stIter)
        {
            stRsp.m_stLogList.push_back(*stIter);
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_PVP_CONTINUE_WIN_REWARD)
    {
        CRequestGetPvpFightReward stParam;
        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        //检查需要领取的各个奖励是否条件完成
        std::vector<CPackGameItem> stRewards;
        int32_t currMaxIndex = 0;
        for (auto rewardIndex: stParam.m_stRewardIndex)
        {
            if (rewardIndex > (int32_t)CLogicConfig::GetPVPConfig().m_stRefreshReward.size() || rewardIndex <= 0)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
            }

            auto rewardConfig = CLogicConfig::GetPVPConfig().m_stRefreshReward[rewardIndex-1];
            if (rewardConfig.m_iWinCount > m_pUserInfo->m_stOnlyInCacheData.m_iPVPFighterWinCount)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
            }
            if(rewardIndex > currMaxIndex)
            {
                currMaxIndex = rewardIndex;
            }
        }

        for (auto rewardIndex: stParam.m_stRewardIndex)
        {
            auto rewardConfig = CLogicConfig::GetPVPConfig().m_stRefreshReward[rewardIndex-1];
            auto iterPvpReward = m_pUserInfo->m_stOnlyInCacheData.m_stPVPRewardState.find(rewardIndex);
            if (iterPvpReward != m_pUserInfo->m_stOnlyInCacheData.m_stPVPRewardState.end() &&
                iterPvpReward->second)
            {
                continue;
            }
            stRewards.insert(stRewards.end(),rewardConfig.m_stRefreshReward.begin(),rewardConfig.m_stRefreshReward.end());
            m_pUserInfo->m_stOnlyInCacheData.m_stPVPRewardState.insert(std::make_pair(rewardIndex, true));
        }

        static std::string strBonus;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, strBonus, m_usCmd);
        //根据配置获取连胜奖励
        CResponseGetPvpFightReward stRsp;
        stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    //废弃
    else if((int)m_usCmd == MSG_LOGIC_FIGHT_PVP_SWEEP)
    {
//        CRequestFightPvp stParam;
//        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);
//
//        if(stParam.m_iUin <= 0 || stParam.m_iUin == m_iUin)
//        {
//            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
//        }
//
//
//        user_info_key_t stKey;
//        stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;
//        stKey.m_stRoleIDDetail.m_iUin = m_iUin;
//        int iSelfRank = LOGIC_PVP_RANK_INS.GetRank(stKey.m_ulRoleID);
//        if(iSelfRank == 0)
//        {
//            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
//        }
//
//        stKey.m_stRoleIDDetail.m_iGroupID = stParam.m_iGroupID;
//        stKey.m_stRoleIDDetail.m_iUin = stParam.m_iUin;
//        int iOtherRank = LOGIC_PVP_RANK_INS.GetRank(stKey.m_ulRoleID);
//        if (iOtherRank == 0)
//        {
//            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
//        }
//
//        if(iSelfRank >= iOtherRank)
//        {
//            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PVP_DST_RANK_TOO_HIGHT)
//        }
//
//        m_pUserInfo->m_stUserInfoTable.AddPvpFightTimes(1);
//        m_pUserInfo->m_stUserInfoTable.AddPvpVictoryTimes(1);
//
//        static std::string strBonus;
//        CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfig::GetPVPConfig().m_stVictoryBonusItem, strBonus, m_usCmd);
//
//        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|PREV_RANK:" << iSelfRank << "|BONUS_ITEM:" << strBonus << std::endl;
//
//        SendSuccessCacheItemToClient();
//
//        // 通用事件
//        CLogicEventData stEventData;
//        stEventData.m_eType = CLogicConfigDefine::ECET_FightPvPTimes;
//        stEventData.m_iCmd = m_usCmd;
//        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
//
//        // 通用事件
//        CLogicEventData stWinEventData;
//        stWinEventData.m_eType = CLogicConfigDefine::ECET_FightPvPWinTimes;
//        stWinEventData.m_iCmd = m_usCmd;
//        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stWinEventData);
//        return (true);
    }

    return (true);
}


int CLogicPVPProcessor::FindChallenger(std::vector<CPackPvpUserInfo>* stVecUserInfo)
{
    //待选择的普通组
    std::vector<CPackPvpUserInfo> stVecReadyUserInfo;
    //待选择的强敌组
    std::vector<CPackPvpUserInfo> stVecReadyMoreUserInfo;
    //待选择的机器人普通组
    std::vector<CPackPvpUserInfo> stVecReadyRobotInfo;
    //待选择的机器人强敌组
    std::vector<CPackPvpUserInfo> stVecReadyMoreRobotInfo;

    auto level = m_pUserInfo->m_stUserInfoTable.GetLevel();
    auto curPoint = m_pUserInfo->m_stUserInfoTable.GetPvpScore();
    auto curRefreshCount = m_pUserInfo->m_stUserInfoTable.GetPvpRefreshCount();

    auto matchLevelRuleConfig = CLogicConfig::GetPVPMatchRuleConfig().FindMatchRuleByLevel(level);
    auto matchPointRuleConfig = CLogicConfig::GetPVPMatchRuleConfig().FindMatchRuleByPoint(curPoint);
    auto matchRefreshRuleConfig = CLogicConfig::GetPVPMatchRuleConfig().FindMatchRuleByRefreshCount(curRefreshCount);

    if (matchLevelRuleConfig == nullptr)
    {
        return SERVER_ERRCODE::NEW_PVP_LEVEL_NOT_MATCH;
    }

    //积分比自己低的人数
    int32_t randLowerNumber = CLogicCommon::GetRandNum(matchLevelRuleConfig->m_iLowerNumberMin, matchLevelRuleConfig->m_iLowerNumberMax, true);
    //积分比自己高的人数
    int32_t randUpperNumber = CLogicConfigDefine::ESD_PvpChallengeListNumber - randLowerNumber;

    float robotPro = 0;
    if (matchRefreshRuleConfig != nullptr)
    {
        robotPro = matchRefreshRuleConfig->m_iRobotPro / CLogicConfigDefine::ESD_RandomWeight;
    } else if (matchPointRuleConfig != nullptr)
    {
        robotPro = matchPointRuleConfig->m_iRobotPro / CLogicConfigDefine::ESD_RandomWeight;
    }

    if (randLowerNumber > CLogicConfigDefine::ESD_PvpChallengeListNumber || randLowerNumber < 0 ||
        randUpperNumber > CLogicConfigDefine::ESD_PvpChallengeListNumber || randUpperNumber < 0)
    {
        return SERVER_ERRCODE::NEW_PVP_NUMBER_ERROR;
    }


    int32_t pointRangeMin = curPoint * (1-matchLevelRuleConfig->m_iLowerPointLimit/CLogicConfigDefine::ESD_RandomWeight);
    int32_t pointRangeMax = curPoint * (1+matchLevelRuleConfig->m_iUpperPointLimit/CLogicConfigDefine::ESD_RandomWeight);


    int32_t iRobotNum = robotPro * CLogicConfigDefine::ESD_PvpChallengeListNumber;
    int32_t iUserNum = CLogicConfigDefine::ESD_PvpChallengeListNumber - iRobotNum;

    if (iRobotNum > CLogicConfigDefine::ESD_PvpChallengeListNumber || iRobotNum < 0 ||
        iUserNum > CLogicConfigDefine::ESD_PvpChallengeListNumber || iUserNum < 0)
    {
        return SERVER_ERRCODE::NEW_PVP_NUMBER_ERROR;
    }

    int32_t iFoundUserCount = 0;
    int32_t iFoundRobotCount = 0;

    int32_t iUpperUserNum = 0;
    int32_t iUpperRobotNum = 0;

    int32_t iLevel = 0;
    int32_t iPoint = 0;
    int32_t iCurPoint = m_pUserInfo->m_stUserInfoTable.GetPvpScore();

    int32_t iLevelMin = matchLevelRuleConfig->m_iMinLevel;
    int32_t iLevelMax = matchLevelRuleConfig->m_iMaxLevel;

    auto iCycleCount = 1000;

    std::set<int32_t> m_iExistUserID;
    auto stIter = LOGIC_USER_DATA_HASHMAP_INS->begin();
    for (;stIter != LOGIC_USER_DATA_HASHMAP_INS->end() && iCycleCount != 0;
         stIter++)
    {
        iCycleCount--;
        if (stIter->second == nullptr) continue;
        auto iter = m_iExistUserID.find(stIter->second->m_iUin);
        if (iter == m_iExistUserID.end())
        {
            m_iExistUserID.insert(stIter->second->m_iUin);
        } else
        {
            PROCESSOR_LOG_DEBUG << "tIter->second->m_iUin: " << stIter->second->m_iUin << std::endl;
            continue;
        }

        if (iFoundUserCount == CLogicConfigDefine::ESD_PvpRandomSelectNumber &&
            iUpperUserNum == CLogicConfigDefine::ESD_PvpRandomSelectNumber) break;

        iLevel = stIter->second->m_stUserInfoTable.GetLevel();
        iPoint = stIter->second->m_stUserInfoTable.GetPvpScore();

        if (stIter->second->m_stUserInfoTable.m_stData.m_iUid == m_iUin) continue ;

        if (iLevel < iLevelMax && iLevel > iLevelMin && iPoint > pointRangeMin && iPoint < pointRangeMax)
        {
            CPackPvpUserInfo userInfo;
            userInfo = CLogicCacheCommon::GetPvpUserInfo(stIter->second->m_iGroupID, stIter->second->m_iUin);
            if (iCurPoint < iPoint)
            {
                stVecReadyMoreUserInfo.emplace_back(userInfo);
                iUpperUserNum++;
            } else
            {
                stVecReadyUserInfo.emplace_back(userInfo);
                iFoundUserCount++;
            }
        }
    }

    auto robotConfig = CLogicConfig::GetPVPRobotConfig().m_stPvpRobotList;
    for (auto iterRobot: robotConfig)
    {
        if (iFoundRobotCount == CLogicConfigDefine::ESD_PvpRandomSelectNumber &&
            iUpperRobotNum == CLogicConfigDefine::ESD_PvpRandomSelectNumber)
        {
            break;
        }

        if (iterRobot.first > iLevelMax || iterRobot.first < iLevelMin)
        {
            continue;
        }

        for (auto iterRobotScore: iterRobot.second)
        {
            if (iterRobotScore.first < pointRangeMin && iterRobotScore.first > pointRangeMax)
            {
                continue;
            }

            CPackPvpUserInfo userInfo = CLogicCacheCommon::GetPvpUserInfo(0, iterRobotScore.second.m_iUinKey);
            if  (iterRobotScore.first > curPoint)
            {
                stVecReadyMoreRobotInfo.emplace_back(userInfo);
                iUpperRobotNum++;
            } else if (iterRobotScore.first < curPoint)
            {
                stVecReadyRobotInfo.emplace_back(userInfo);
                iFoundRobotCount++;
            }

            if (iFoundRobotCount == CLogicConfigDefine::ESD_PvpRandomSelectNumber &&
                iUpperRobotNum == CLogicConfigDefine::ESD_PvpRandomSelectNumber)
            {
                break;
            }
        }
    }


    if ((int32_t)stVecReadyUserInfo.size() < iUserNum)
    {
        iUserNum = stVecReadyUserInfo.size();
    }

    std::set<int32_t> selectedUserIndex;
    for (auto userNum = 0; userNum != iUserNum && iUserNum != 0;)
    {
        auto iRandUserIndex = CLogicCommon::GetRandNum(0,stVecReadyUserInfo.size());
        if (selectedUserIndex.find(iRandUserIndex) == selectedUserIndex.end())
        {
            selectedUserIndex.insert(iRandUserIndex);
            stVecUserInfo->emplace_back(*(stVecReadyUserInfo.begin()+iRandUserIndex));
            userNum++;
        }
    }

    iRobotNum = CLogicConfigDefine::ESD_PvpChallengeListNumber - iUserNum;
    //普通机器人不够了
    if (iRobotNum > (int32_t)stVecReadyRobotInfo.size())
    {
        iRobotNum = stVecReadyRobotInfo.size();
    }

    std::set<int32_t> selectedRobotIndex;
    for (auto robotNum = 0; robotNum != iRobotNum && iRobotNum != 0;)
    {
        auto iRandRobotIndex = CLogicCommon::GetRandNum(0,stVecReadyRobotInfo.size());
        if (selectedRobotIndex.find(iRandRobotIndex) == selectedRobotIndex.end())
        {
            selectedRobotIndex.insert(iRandRobotIndex);
            stVecUserInfo->emplace_back(*(stVecReadyRobotInfo.begin()+iRandRobotIndex));
            robotNum++;
        }
    }

    randUpperNumber = CLogicConfigDefine::ESD_PvpChallengeListNumber - iUserNum - iRobotNum;
    //强敌优先是玩家
    std::set<int32_t> selectedMoreUserIndex;
    if (randUpperNumber <= (int32_t)stVecReadyMoreUserInfo.size() && !stVecReadyMoreUserInfo.empty())
    {
      for (;randUpperNumber != 0;)
      {
          auto iRandUserMoreIndex = CLogicCommon::GetRandNum(0,stVecReadyMoreUserInfo.size());
          if (selectedMoreUserIndex.find(iRandUserMoreIndex) == selectedMoreUserIndex.end())
          {
              selectedMoreUserIndex.insert(iRandUserMoreIndex);
              stVecUserInfo->emplace_back(*(stVecReadyMoreUserInfo.begin()+iRandUserMoreIndex));
              randUpperNumber--;
          }
      }
    }
    else if (randUpperNumber > (int32_t)stVecReadyMoreUserInfo.size() && !stVecReadyMoreUserInfo.empty())
    {
        randUpperNumber -= stVecReadyMoreUserInfo.size();
        stVecUserInfo->insert(stVecUserInfo->end(),stVecReadyMoreUserInfo.begin(),stVecReadyMoreUserInfo.end());
    }

    //不够用机器人填充
    if (!stVecReadyMoreRobotInfo.empty() && randUpperNumber <= (int32_t)stVecReadyMoreRobotInfo.size())
    {
        std::set<int32_t> selectedMoreRobotIndex;
        for (;randUpperNumber != 0;)
        {
            auto iRandRobotIndex = CLogicCommon::GetRandNum(0,stVecReadyMoreRobotInfo.size());
            if (selectedMoreRobotIndex.find(iRandRobotIndex) == selectedMoreRobotIndex.end())
            {
                selectedMoreRobotIndex.insert(iRandRobotIndex);
                stVecUserInfo->emplace_back(*(stVecReadyMoreRobotInfo.begin()+iRandRobotIndex));
                randUpperNumber--;
            }
        }
    }

    
    //不够用机器人填充
    if (!stVecReadyRobotInfo.empty())
    {
        std::set<int32_t> selectedMoreRobotIndex;
        for (;randUpperNumber != 0;)
        {
            auto iRandRobotIndex = CLogicCommon::GetRandNum(0,stVecReadyRobotInfo.size());
            if (selectedMoreRobotIndex.find(iRandRobotIndex) == selectedMoreRobotIndex.end())
            {
                selectedMoreRobotIndex.insert(iRandRobotIndex);
                stVecUserInfo->emplace_back(*(stVecReadyRobotInfo.begin()+iRandRobotIndex));
                randUpperNumber--;
            }
        }
    }

    return  SERVER_ERRCODE::SUCCESS;
}


void CLogicPVPProcessor::RandomPVPReward(const std::vector<CPackRandomGameItem>&rewardConfig ,std::vector<CPackGameItem>* pvpReward)
{
    for (auto iterRewardConfig : rewardConfig)
    {
        int randomNum = CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_RandomWeight;
        if (randomNum < iterRewardConfig.m_iWeight)
        {
            if (iterRewardConfig.m_iRandom > 0)
            {
                int randomMax = iterRewardConfig.m_iNum * (1 + iterRewardConfig.m_iRandom / CLogicConfigDefine::ESD_RandomWeight);
                int randomMin = iterRewardConfig.m_iNum * (1 - iterRewardConfig.m_iRandom / CLogicConfigDefine::ESD_RandomWeight);
                auto randCount = CLogicCommon::GetRandNum(randomMin,randomMax);
                pvpReward->emplace_back(CPackGameItem{iterRewardConfig.m_iItemType,iterRewardConfig.m_iCardID,randCount});
            } else
            {
                pvpReward->emplace_back(CPackGameItem{iterRewardConfig.m_iItemType,iterRewardConfig.m_iCardID,iterRewardConfig.m_iNum});
            }
        }
    }
}

