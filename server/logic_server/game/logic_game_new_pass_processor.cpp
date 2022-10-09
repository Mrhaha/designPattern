//
// Created by czy on 2022/04/23.
//
#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "logic_config.h"
#include "logic_link_error.h"
#include "logic_game_cache_common.h"
#include "logic_game_active.h"
#include "logic_game_new_pass_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "server_new_pass_protocol.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"


REGISTER_CMD_FACTORY(MSG_LOGIC_NEW_PASS_GET_INFO, CLogicNewPassProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_NEW_PASS_GET_REWARD, CLogicNewPassProcessor)

CLogicNewPassProcessor::CLogicNewPassProcessor(uint16_t usCmd, const std::string &strCmdName)
    : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicNewPassProcessor::DoUserRun(const msgpack::unpacked &stFormatData)
{
    bool result = false;

    //检查活动是否开启
    auto& newPassConfig = CLogicConfig::GetNewPassConfig();
    if (!CLogicActive::IsAvyOpen(newPassConfig.m_iActID, m_pUserInfo->m_stUserInfoTable.GetCreateTime()))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }

    switch ((int)m_usCmd)
    {
        case MSG_LOGIC_NEW_PASS_GET_INFO:
            result =  DoUserRunGetNewPassInfo(stFormatData);
            break;
        case MSG_LOGIC_NEW_PASS_GET_REWARD:
            result =  DoUserRunGetNewPassAward(stFormatData);
            break;
        default:
            break;
    }

    return result;
}

bool CLogicNewPassProcessor::DoUserRunGetNewPassInfo(const msgpack::unpacked& stFormatData)
{
    CResponsePassInfo stRsp;
    stRsp.m_iTotalExp = m_pUserInfo->m_stUserInfoTable.GetNewPassTotalExp();
    stRsp.m_buyNewPassDepositID = BuyWhichMonthPass(m_pUserInfo);
    stRsp.m_stNormalRewardLevel = m_pUserInfo->m_stUserInfoTable.GetNewPassNormalLevel();
    stRsp.m_stAdvancedRewardLevel = m_pUserInfo->m_stUserInfoTable.GetNewPassAdvancedLevel();
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicNewPassProcessor::DoUserRunGetNewPassAward(const msgpack::unpacked& stFormatData)
{
    CRequestNewPassGetReward stReq;
    if (DecodeMsgPack(stFormatData, stReq) == false)
        return (false);

    if (stReq.m_stNormalLevels.empty() && stReq.m_stAdvancedLevels.empty())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    //是否购买通行证
    if (!stReq.m_stAdvancedLevels.empty() && BuyWhichMonthPass(m_pUserInfo) == 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NEW_PASS_NOT_BUY);
    }

    //检查是否都满足领取条件
    int32_t currLevel = GetNewPassCurrentLevel();
    PROCESSOR_LOG_DEBUG << "CURRENT LEVEL" << currLevel
                        << "TOTAL_EXP: " << m_pUserInfo->m_stUserInfoTable.GetNewPassTotalExp()
                        << std::endl;

    for (auto targetLevel : stReq.m_stNormalLevels)
    {
        if (currLevel < targetLevel)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NEW_PASS_NOT_MEET);
        }
    }

    for (auto targetLevel : stReq.m_stAdvancedLevels)
    {
        if (currLevel < targetLevel)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NEW_PASS_NOT_MEET);
        }
    }

    //已领取的level
    auto normalAccepted   = m_pUserInfo->m_stUserInfoTable.GetNewPassNormalLevel();
    auto advancedAccepted = m_pUserInfo->m_stUserInfoTable.GetNewPassAdvancedLevel();

    //记录本次成功领取Level
    std::vector<int32_t> stRecordNormal;
    std::vector<int32_t> stRecordAdvanced;

    //发放对应的奖励
    std::vector<CPackGameItem> stRewards;
    bool hasReceived = false;

    for (auto normalLevel : stReq.m_stNormalLevels)
    {
        hasReceived = false;
        for (auto acceptedLevel : normalAccepted)
        {
            if (normalLevel == acceptedLevel) //奖励已经领取过了
            {
                PROCESSOR_LOG_DEBUG << "MSG PROCESS NEW_PASS NORMAL_LEVEL :" << normalLevel << "HAS ACCEPTED" << std::endl;
                hasReceived = true;
                break;
            }
        }
        if (hasReceived)  continue;
        auto rewardConfigIter = CLogicConfig::GetNewPassConfig().GetNormalRewardByLevel(normalLevel);
        if (nullptr == rewardConfigIter)
        {
            PROCESSOR_LOG_DEBUG << "MSG PROCESS NEW_PASS NORMAL_LEVEL :" << normalLevel << "Config Not Found" << std::endl;
            continue;
        }
        stRewards.insert(stRewards.end(), rewardConfigIter->m_stReward.begin(), rewardConfigIter->m_stReward.end());
        stRecordNormal.emplace_back(normalLevel);
    }
    for (auto advancedLevel : stReq.m_stAdvancedLevels)
    {
        hasReceived = false;
        for (auto acceptedLevel : advancedAccepted)
        {
            if (advancedLevel == acceptedLevel) //奖励已经领取过了
            {
                PROCESSOR_LOG_DEBUG << "MSG PROCESS NEW_PASS ADVANCED_LEVEL :" << advancedLevel << "HAS ACCEPTED" << std::endl;
                hasReceived = true;
                break;
            }
        }

        if (hasReceived)  continue;
        auto rewardConfigIter = CLogicConfig::GetNewPassConfig().GetAdvancedRewardByLevel(advancedLevel);
        if (nullptr == rewardConfigIter)
        {
            PROCESSOR_LOG_DEBUG << "MSG PROCESS NEW_PASS NORMAL_LEVEL :" << advancedLevel << "Config Not Found" << std::endl;
            continue;
        }
        stRewards.insert(stRewards.end(),rewardConfigIter->m_stReward.begin(),rewardConfigIter->m_stReward.end());
        stRecordAdvanced.emplace_back(advancedLevel);
    }
    //保存领取记录
    normalAccepted.insert(normalAccepted.end(),stRecordNormal.begin(),stRecordNormal.end());
    advancedAccepted.insert(advancedAccepted.end(),stRecordAdvanced.begin(),stRecordAdvanced.end());

    m_pUserInfo->m_stUserInfoTable.SetNewPassNormalLevel(normalAccepted);
    m_pUserInfo->m_stUserInfoTable.SetNewPassAdvancedLevel(advancedAccepted);

    static std::string strBonus;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, strBonus, m_usCmd);

    CResponseNewPassGetReward stRsp;
    stRsp.m_stGetReward = m_pUserInfo->m_stCacheBonusData;
    stRsp.m_stNormalLevels = stRecordNormal;
    stRsp.m_stAdvancedLevels = stRecordAdvanced;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

//获取当前的通行证等级
int32_t CLogicNewPassProcessor::GetNewPassCurrentLevel()
{
    return  CLogicConfig::GetNewPassConfig().GetNewPassLevelByExp(m_pUserInfo->m_stUserInfoTable.GetNewPassTotalExp());
}

int32_t CLogicNewPassProcessor::BuyWhichMonthPass(user_data_table_ptr_type m_pUserInfo)
{
    bool hasBuyNormal = false;
    bool hasBuyUpGrade = false;
    bool hasBuyAdvanced = false;

    auto iter = m_pUserInfo->m_stUserDepositMap.Find(CLogicConfig::GetNewPassConfig().m_iDepositID);
    if (iter != m_pUserInfo->m_stUserDepositMap.End() && iter->second->GetBuyTimes() > 0)
    {
        hasBuyNormal = true;
    }

    auto UpIter = m_pUserInfo->m_stUserDepositMap.Find(CLogicConfig::GetNewPassConfig().m_iUpDepositID);
    if (UpIter != m_pUserInfo->m_stUserDepositMap.End() && UpIter->second->GetBuyTimes() > 0)
    {
        hasBuyUpGrade = true;
    }

    auto AdvIter = m_pUserInfo->m_stUserDepositMap.Find(CLogicConfig::GetNewPassConfig().m_iAdvancedDepositID);
    if (AdvIter != m_pUserInfo->m_stUserDepositMap.End() && AdvIter->second->GetBuyTimes() > 0)
    {
        hasBuyAdvanced = true;
    }

    if (hasBuyAdvanced) return CLogicConfig::GetNewPassConfig().m_iAdvancedDepositID;
    if (hasBuyNormal && hasBuyUpGrade) return CLogicConfig::GetNewPassConfig().m_iAdvancedDepositID;
    if (hasBuyNormal) return CLogicConfig::GetNewPassConfig().m_iDepositID;

    return 0;
}
