//
// Created by czy on 2022/5/7.
//

#include "server_errcode.h"
#include "server_cmd_cross_define.h"
#include "server_new_limit_boss_protocol.h"
#include "logic_game_cross_limit_boss_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_active.h"
#include "logic_game_cache_common.h"
#include "logic_config_base.h"
#include "logic_game_buff.h"
#include "logic_game_user_notify_system.h"
#include "server_cross_protocol.h"

REGISTER_CMD_FACTORY(MSG_CROSS_NEW_LIMIT_BOSS_HISTORY, CLogicCrossLimitBossProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NEW_LIMIT_BOSS_BEGIN, CLogicCrossLimitBossProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NEW_LIMIT_BOSS_END, CLogicCrossLimitBossProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NEW_LIMIT_BOSS_INVITE, CLogicCrossLimitBossProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NEW_LIMIT_BOSS_GET_REWARD, CLogicCrossLimitBossProcessor)


REGISTER_CMD_FACTORY(MSG_CROSS_NEW_LIMIT_BOSS_NOTIFY, CLogicCrossLimitBossProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NEW_LIMIT_BOSS_DEAD_NOTIFY, CLogicCrossLimitBossProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NEW_LIMIT_BOSS_INVITE_NOTIFY, CLogicCrossLimitBossProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NEW_LIMIT_BOSS_RUN_NOTIFY, CLogicCrossLimitBossProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NEW_LIMIT_BOSS_GET_INFO, CLogicCrossLimitBossProcessor)



CLogicCrossLimitBossProcessor::CLogicCrossLimitBossProcessor(uint16_t usCmd, const std::string &strCmdName)
    : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicCrossLimitBossProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    bool bRet = false;

    //是否连接cross服务器
    if (!GetCrossConnection()->IsConnected())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MATCH_SERVER_NOT_CONN);
    }

    if (!CLogicActive::IsLimitBossLevelOpen(m_pUserInfo))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }

    switch (m_usCmd)
    {
    case MSG_CROSS_NEW_LIMIT_BOSS_HISTORY:
        bRet = DoUserRunGetLimitBossHistory(stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_BEGIN:
        bRet = DoUserRunLimitBossBegin(stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_END:
        bRet = DoUserRunLimitBossEnd(stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_INVITE:
        bRet = DoUserRunLimitBossInvite(stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_GET_REWARD:
        bRet = DoUserRunLimitBossGetReward(stFormatData);
        break;
    default:
        break;
    }

    return bRet;
}

bool CLogicCrossLimitBossProcessor::DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    bool bRet = false;

    switch (stMsgHead.m_usCmd)
    {
    case MSG_CROSS_NEW_LIMIT_BOSS_NOTIFY:
        bRet = DoServerRunLimitBossNew(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_HISTORY:
        bRet = DoServerRunLimitBossHistory(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_BEGIN:
        bRet = DoServerRunLimitBossBegin(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_END:
        bRet = DoServerRunLimitBossEnd(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_DEAD_NOTIFY:
        bRet = DoServerRunLimitBossDead(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_INVITE_NOTIFY:
        bRet = DoServerRunLimitBossInviteNotify(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_RUN_NOTIFY:
        bRet = DoServerRunLimitBossRun(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_NEW_LIMIT_BOSS_GET_INFO:
        bRet = DoServerRunLimitBossGetInfo(stMsgHead, stFormatData);
        break;


    default:
        break;
    }
    return bRet;
}


bool CLogicCrossLimitBossProcessor::DoUserRunGetLimitBossHistory(const msgpack::unpacked& stFormatData)
{
    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}



bool CLogicCrossLimitBossProcessor::DoUserRunLimitBossBegin(const msgpack::unpacked& stFormatData)
{
    CRequestLimitBossBegin stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    if(stReq.m_iOwnerUin == m_pUserInfo->m_iUin && !m_pUserInfo->m_stOnlyInCacheData.m_bHasLimitBoss)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_NOT_EXIST);
    }

    //体力判断
    auto limitBossConfig = CLogicConfig::GetLimitBossConfig().GetLimitBossConfigByLevelID(stReq.m_iLevelID);
    if (limitBossConfig == nullptr)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(!CLogicCacheCommon::CanConsume(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_EnergyID,
                                            limitBossConfig->m_iPhysicalConsume, m_usCmd))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ENERGY)
    }

    //buff次数减少
    CLogicBuff::ReduceBuffTimes(m_pUserInfo,CLogicConfigDefine::EBT_AddAttr);
    CLogicUserNotifySystem::NotifyUserBuffData(m_pUserInfo);

    SendToCrossServer(enmCmdType_request,m_usCmd,stReq);
    return true;
}

bool CLogicCrossLimitBossProcessor::DoUserRunLimitBossEnd(const msgpack::unpacked& stFormatData)
{
    CRequestLimitBossEnd stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    if (stReq.m_iLevelID <= 0 || m_pUserInfo->m_stUserInfoTable.GetFightLevelID() != stReq.m_iLevelID)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FIGHT_LEVEL_MISMATCH)
    }

    SendToCrossServer(enmCmdType_request,m_usCmd,stReq);
    return success;
}

bool CLogicCrossLimitBossProcessor::DoUserRunLimitBossGetReward(const msgpack::unpacked& stFormatData)
{
    CRequestLimitBossGetReward stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    std::vector<CPackGameItem> stRewards;
    for (auto& iterGetReward: stReq.m_stReward)
    {
        auto limitBossLevelConfig = CLogicConfig::GetLimitBossConfig().GetLimitBossConfigByLevelID(iterGetReward.m_iLevelID);
        if (nullptr == limitBossLevelConfig || iterGetReward.m_iIndex < 0)
        {
            PROCESSOR_LOG_DEBUG << "MSG PROCESS LIMIT_BOSS LEVEL :" << iterGetReward.m_iLevelID << "Config Not Found" << std::endl;
            continue;
        }

        if (m_pUserInfo->m_iUin != iterGetReward.m_iOwnerUin && m_pUserInfo->m_stIBusyData.GetNewLimitBossDailyCount() >= CLogicConfig::GetLimitBossConfig().m_iDailyGetRewardCount)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_DAILY_COUNT_LIMIT)
        }

        auto limitBossRewardList = m_pUserInfo->m_stIBusyData.GetNewLimitBossReward();
        auto iterReward = limitBossRewardList.find(iterGetReward.m_iOwnerUin);
        if (iterReward == limitBossRewardList.end())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_CANT_GET_REWARD)
        }

        if (iterGetReward.m_iIndex >= (int32_t)iterReward->second.size())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_CANT_GET_REWARD)
        }

        //不管是否逃逸发现人都有发现积分
        if (m_pUserInfo->m_iUin == iterReward->first)
        {
            CPackGameItem rewardPoint;
            rewardPoint.m_iItemType = CLogicConfigDefine::EXCHANGE_ITEM;
            rewardPoint.m_iCardID = CLogicConfig::GetLimitBossConfig().m_iPointItemID;
            rewardPoint.m_iNum = limitBossLevelConfig->m_iFindRewardScore;
            stRewards.push_back(rewardPoint);
        } else //不是发现人需要累加领奖次数
        {
            m_pUserInfo->m_stIBusyData.SetNewLimitBossDailyCount(m_pUserInfo->m_stIBusyData.GetNewLimitBossDailyCount()+1);
        }

        //不管是否逃逸都有血量积分
        auto iterInviteUser = iterReward->second[iterGetReward.m_iIndex].m_stInviteUser.find(m_pUserInfo->m_iUin);
        if (iterInviteUser != iterReward->second[iterGetReward.m_iIndex].m_stInviteUser.end())
        {
            CPackGameItem rewardPoint;
            rewardPoint.m_iItemType = CLogicConfigDefine::EXCHANGE_ITEM;
            rewardPoint.m_iCardID = CLogicConfig::GetLimitBossConfig().m_iPointItemID;
            rewardPoint.m_iNum =   (float)iterInviteUser->second.m_ulHurt / limitBossLevelConfig->m_iTotalHP * limitBossLevelConfig->m_iHpRewardScore;
            stRewards.push_back(rewardPoint);
        } else
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_NOT_INVITE)
        }

        //只有击杀才有击杀奖励奖励
        if (!iterReward->second[iterGetReward.m_iIndex].m_stBossInfo.m_bIsRun) {
            stRewards.insert(stRewards.end(), limitBossLevelConfig->m_stRewardItem.begin(), limitBossLevelConfig->m_stRewardItem.end());
        }

        //删除领奖记录
        int32_t index = 0;
        for(auto recordIter = iterReward->second.begin();  recordIter != iterReward->second.end();)
        {
            if (index != iterGetReward.m_iIndex)
            {
                recordIter++;
                index++;
                continue;
            }
            iterReward->second.erase(recordIter);
            break;
        }
        if (iterReward->second.empty())
        {
            limitBossRewardList.erase(iterReward);
        }
        m_pUserInfo->m_stIBusyData.SetNewLimitBossReward(limitBossRewardList);
    }

    for (auto rewardIter: stRewards)
    {
        if (rewardIter.m_iCardID != CLogicConfig::GetLimitBossConfig().m_iPointItemID)
        {
            continue;
        }
        m_pUserInfo->m_stUserInfoTable.AddLimitBossTotalPoint(rewardIter.m_iNum);
    }

    static std::string strBonus;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, strBonus, m_usCmd);

    CResponseLimitBossGetReward stRsp;
    stRsp.m_stGetReward = m_pUserInfo->m_stCacheBonusData;

    SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    return success;
}

bool CLogicCrossLimitBossProcessor::DoUserRunLimitBossInvite(const msgpack::unpacked& stFormatData)
{
    CRequestNewLimitBossInvite stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    if (!m_pUserInfo->m_stOnlyInCacheData.m_bHasLimitBoss)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_NOT_EXIST)
    }

    if ((int32_t)stReq.m_stInviteUid.size() > CLogicConfig::GetLimitBossConfig().m_iInviteFriendNum)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_INVITE_LIMIT)
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);

    SendToClient(SERVER_ERRCODE::SUCCESS);
    return true;
}

/***************************************SERVER_RUN*********************************************************/

bool CLogicCrossLimitBossProcessor::DoServerRunLimitBossNew(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    m_pUserInfo->m_stOnlyInCacheData.m_bHasLimitBoss = true;
    return true;
}


bool CLogicCrossLimitBossProcessor::DoServerRunLimitBossHistory(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGetLimitBossHistory stHistory;
    if (!DecodeMsgPack(stFormatData, stHistory)) return false;

    auto rewardList =  m_pUserInfo->m_stIBusyData.GetNewLimitBossReward();
    for (const auto& iter: rewardList)
    {
        for(const auto& rewardItem : iter.second)
        {
            stHistory.m_stHistoryInfo.emplace_back(rewardItem);
        }
    }

    for (const auto& iter: m_pUserInfo->m_stUserInfoTable.GetFindLimitBoss())
    {
        stHistory.m_stFoundLevelID.push_back(iter.first);
    }

    stHistory.m_iTotalLimitBossPoint = m_pUserInfo->m_stUserInfoTable.GetLimitBossTotalPoint();
    stHistory.m_iDailyRewardCount = m_pUserInfo->m_stIBusyData.GetNewLimitBossDailyCount();
    stHistory.m_iBaoDiTime = m_pUserInfo->m_stIBusyData.GetLimitBossBaodiTime();

    SendToClient(stHistory,SERVER_ERRCODE::SUCCESS);
    return success;
}

bool CLogicCrossLimitBossProcessor::DoServerRunLimitBossBegin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CRequestLimitBossBegin stCross;
    if (!DecodeMsgPack(stFormatData, stCross)) return false;
    m_pUserInfo->m_stUserInfoTable.SetFightLevelID(stCross.m_iLevelID);
    SendToClient(SERVER_ERRCODE::SUCCESS);
    return success;
}

bool CLogicCrossLimitBossProcessor::DoServerRunLimitBossEnd(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{

    CCrossLimitBossLevelEnd stCross;
    if (!DecodeMsgPack(stFormatData, stCross)) return false;
    //挑战结束成功,扣除体力
    auto limitBossConfig = CLogicConfig::GetLimitBossConfig().GetLimitBossConfigByLevelID(stCross.m_iLevelID);
    if (limitBossConfig == nullptr)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_EnergyID,
                                            limitBossConfig->m_iPhysicalConsume, m_usCmd))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ENERGY)
    }

    m_pUserInfo->m_stUserInfoTable.SetFightLevelID(0);
    SendToClient(SERVER_ERRCODE::SUCCESS);

    return success;
}

bool CLogicCrossLimitBossProcessor::DoServerRunLimitBossDead(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossLimitBossDeadNotify stNotify;
    if (!DecodeMsgPack(stFormatData, stNotify)) return false;

    //如果击杀的是自己的boss，则可以继续刷新出限时boss
    if (stNotify.m_iOwnerUin == m_pUserInfo->m_iUin && stNotify.m_iOwnerGroupID == m_pUserInfo->m_iGroupID)
    {
        m_pUserInfo->m_stOnlyInCacheData.m_bHasLimitBoss = false;
    }

    PROCESSOR_LOG_DEBUG << "HISTORY bossInfo:" << stNotify.m_stTeamInfo.m_stBossInfo.m_iOwnerUin
                        << "|" << stNotify.m_stTeamInfo.m_stBossInfo.m_iOwnerGroupID
                        << "|" << stNotify.m_stTeamInfo.m_stBossInfo.m_iAppearLeveID
                        << "|" << stNotify.m_stTeamInfo.m_stBossInfo.m_iLevelID
                        << std::endl;
    for (auto user : stNotify.m_stTeamInfo.m_stInviteUser)
    {

        PROCESSOR_LOG_DEBUG << "damageInfo: " << user.second.m_ulHurt
                            << "userid: " << user.second.m_iUin
                            << std::endl;
    }

    //同时将奖励记录下来
    auto newLimitBoss =  m_pUserInfo->m_stIBusyData.GetNewLimitBossReward();
    int32_t iOtherCount = 0;
    int32_t iSelfCount = 0;
    for(auto bossReward: newLimitBoss)
    {
        if (bossReward.first == m_pUserInfo->m_iUin)
        {
            iSelfCount += bossReward.second.size();
        } else
        {
            iOtherCount += bossReward.second.size();
        }
    }

    //插入别人的奖励，但容量满了
    if (iOtherCount >= UNRECEIVED_REWARD && stNotify.m_iOwnerUin != m_pUserInfo->m_iUin)
    {
        return success;
    }

    //插入自己的奖励，但容量满了
    if (iSelfCount >= 23 - UNRECEIVED_REWARD && stNotify.m_iOwnerUin == m_pUserInfo->m_iUin)
    {
        return success;
    }

    auto newLimitBossIter = newLimitBoss.find(stNotify.m_iOwnerUin);
    if (newLimitBossIter == newLimitBoss.end())
    {
        std::vector<CPackLimitBossTeamInfo> stRewardItem;
        stRewardItem.emplace_back(stNotify.m_stTeamInfo);
        newLimitBoss.insert(std::make_pair(stNotify.m_iOwnerUin,stRewardItem));
    } else
    {
        newLimitBossIter->second.emplace_back(stNotify.m_stTeamInfo);
    }

    m_pUserInfo->m_stIBusyData.SetNewLimitBossReward(newLimitBoss);

    SendToClient(stNotify,SERVER_ERRCODE::SUCCESS);
    return success;
}

bool CLogicCrossLimitBossProcessor::DoServerRunLimitBossInviteNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossLimitBossInviteNotify stNotify;
    if (!DecodeMsgPack(stFormatData, stNotify)) return false;

    SendToClient(stNotify,SERVER_ERRCODE::SUCCESS);
    return success;
}


bool CLogicCrossLimitBossProcessor::DoServerRunLimitBossRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CNotifyLimitBossRunaway stNotify;
    if (!DecodeMsgPack(stFormatData, stNotify)) return false;

    //如果是自己的boss，则可以继续刷新出限时boss
    if (stNotify.m_stBossTeamInfo.m_stBossInfo.m_iOwnerUin == m_pUserInfo->m_iUin
        && stNotify.m_stBossTeamInfo.m_stBossInfo.m_iOwnerGroupID == m_pUserInfo->m_iGroupID)
    {
        m_pUserInfo->m_stOnlyInCacheData.m_bHasLimitBoss = false;
    }
    //同时将奖励记录下来
    auto newLimitBoss =  m_pUserInfo->m_stIBusyData.GetNewLimitBossReward();

    int32_t iOtherCount = 0;
    int32_t iSelfCount = 0;
    for(auto bossReward: newLimitBoss)
    {
        if (bossReward.first == m_pUserInfo->m_iUin)
        {
            iSelfCount += bossReward.second.size();
        } else
        {
            iOtherCount += bossReward.second.size();
        }
    }

    //插入别人的奖励，但容量满了
    if (iOtherCount >= UNRECEIVED_REWARD && stNotify.m_stBossTeamInfo.m_stBossInfo.m_iOwnerUin != m_pUserInfo->m_iUin)
    {
        return success;
    }

    //插入自己的奖励，但容量满了
    if (iSelfCount >= 23 - UNRECEIVED_REWARD && stNotify.m_stBossTeamInfo.m_stBossInfo.m_iOwnerUin == m_pUserInfo->m_iUin)
    {
        return success;
    }

    auto newLimitBossRewardIter = newLimitBoss.find(stNotify.m_stBossTeamInfo.m_stBossInfo.m_iOwnerUin);
    if (newLimitBossRewardIter == newLimitBoss.end())
    {
        std::vector<CPackLimitBossTeamInfo> limitReward;
        limitReward.emplace_back(stNotify.m_stBossTeamInfo);
        newLimitBoss.insert(std::make_pair(stNotify.m_stBossTeamInfo.m_stBossInfo.m_iOwnerUin,limitReward));
    } else
    {
        newLimitBossRewardIter->second.emplace_back(stNotify.m_stBossTeamInfo);
    }

    m_pUserInfo->m_stIBusyData.SetNewLimitBossReward(newLimitBoss);

    return success;
}

bool CLogicCrossLimitBossProcessor::DoServerRunLimitBossGetInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CNotifyLimitBossInfo stNotify;
    if (!DecodeMsgPack(stFormatData, stNotify)) return false;
    m_pUserInfo->m_stOnlyInCacheData.m_bHasLimitBoss = stNotify.m_bIsExist;
    return success;
}

