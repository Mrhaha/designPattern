//
// Created by hzl on 2019/10/24.
//

#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_xuyuanqingdian_protocol.h"
#include "logic_config.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_cache_common.h"
#include "logic_game_active.h"
#include "logic_game_global.h"
#include "logic_game_month_pass_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_daily_task_system.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_MONTH_PASS_AWARD, CLogicMonthPassProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_MONTH_PASS_TASK_AWARD, CLogicMonthPassProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_MONTH_PASS_EXCHANGE, CLogicMonthPassProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_MONTH_PASS_WISH_ITEM, CLogicMonthPassProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_MONTH_PASS_DAILY_WISH, CLogicMonthPassProcessor)

CLogicMonthPassProcessor::CLogicMonthPassProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicMonthPassProcessor::DoUserRun(const msgpack::unpacked &stFormatData)
{
    bool result = false;

    if (!CLogicActive::IsMonthPassOpen(m_pUserInfo))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }

    switch ((int)m_usCmd)
    {
        case MSG_LOGIC_GET_MONTH_PASS_AWARD:
            result =  DoUserRunGetMonthPassAward(stFormatData);
            break;

        case MSG_LOGIC_GET_MONTH_PASS_TASK_AWARD:
            result =  DoUserRunGetMonthPassTaskAward(stFormatData);
            break;

        case MSG_LOGIC_MONTH_PASS_EXCHANGE:
            result =  DoUserRunMonthPassExchange(stFormatData);
            break;

        case MSG_LOGIC_MONTH_PASS_WISH_ITEM:
            result =  DoUserRunMonthPassWishItem(stFormatData);
            break;

        case MSG_LOGIC_MONTH_PASS_DAILY_WISH:
            result =  DoUserRunMonthPassDailyWish(stFormatData);
            break;

        default:
            break;
    }

    return result;
}

bool CLogicMonthPassProcessor::DoUserRunGetMonthPassAward(const msgpack::unpacked& stFormatData)
{
    CRequestGetMonthPassAward stReqBody;
    CResponseGetMonthPassAward stRspBody;
    if (!DecodeMsgPack(stFormatData, stReqBody))
    {
        return false;
    }

    if (stReqBody.m_iIndex < 0 || stReqBody.m_iIndex > 200 || stReqBody.m_iIndex > GetMonthPassLevel())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL);
    }

    const auto* pstConfig = CLogicConfig::GetMonthPassConfig().GetLevelExpConfig(stReqBody.m_iIndex);
    if (pstConfig == nullptr)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL);
    }

    std::string strBitmap = m_pUserInfo->m_stIBusyData.GetMonthPassAwardBitmap();
    if (int32_t (strBitmap.length()) <= stReqBody.m_iIndex)
    {
        strBitmap.resize(stReqBody.m_iIndex + 1, '0');
    }
    if (strBitmap.at(stReqBody.m_iIndex) != '1')
    {
        strBitmap.at(stReqBody.m_iIndex) = '1';
        m_pUserInfo->m_stIBusyData.SetMonthPassAwardBitmap(strBitmap);
        CLogicCacheCommon::AddGameItem(m_pUserInfo, pstConfig->m_stBonusVec, m_usCmd);
        stRspBody.m_stItemVec = pstConfig->m_stBonusVec;
    }

    if (IsBuyMonthPass())
    {
        strBitmap = m_pUserInfo->m_stIBusyData.GetMonthPassDepositAwardBitmap();
        if (int32_t (strBitmap.length()) <= stReqBody.m_iIndex)
        {
            strBitmap.resize(stReqBody.m_iIndex + 1, '0');
        }
        if (strBitmap.at(stReqBody.m_iIndex) != '1')
        {
            strBitmap.at(stReqBody.m_iIndex) = '1';
            m_pUserInfo->m_stIBusyData.SetMonthPassDepositAwardBitmap(strBitmap);
            CLogicCacheCommon::AddGameItem(m_pUserInfo, pstConfig->m_stDepositVec, m_usCmd);
            stRspBody.m_stItemVec.insert(stRspBody.m_stItemVec.end(), pstConfig->m_stDepositVec.begin(), pstConfig->m_stDepositVec.end());
        }
    }

    CLogicUserNotifySystem::NotifyMonthPass(m_pUserInfo);

    stRspBody.m_iIndex = stReqBody.m_iIndex;
    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicMonthPassProcessor::DoUserRunGetMonthPassTaskAward(const msgpack::unpacked& stFormatData)
{
    CRequestGetMonthPassTaskAward stReqBody;
    CResponseGetMonthPassTaskAward stRspBody;
    if (!DecodeMsgPack(stFormatData, stReqBody))
    {
        return false;
    }

    const auto* pstConfig = CLogicConfig::GetMonthPassConfig().GetTaskConfig(stReqBody.m_iTaskID);
    if (pstConfig == nullptr)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DAILY_TASK_NOT_RECORD);
    }

    auto iterTask = m_pUserInfo->m_stMonthPassTaskMap[CLogicConfigDefine::MONTH_PASS_TYPE_DAILY_TASK].Find(stReqBody.m_iTaskID);
    if (iterTask == m_pUserInfo->m_stMonthPassTaskMap[CLogicConfigDefine::MONTH_PASS_TYPE_DAILY_TASK].End())
    {
        iterTask = m_pUserInfo->m_stMonthPassTaskMap[CLogicConfigDefine::MONTH_PASS_TYPE_WEEKLY_TASK].Find(stReqBody.m_iTaskID);
        if (iterTask == m_pUserInfo->m_stMonthPassTaskMap[CLogicConfigDefine::MONTH_PASS_TYPE_WEEKLY_TASK].End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DAILY_TASK_NOT_RECORD);
        }
    }

    if (iterTask->second->GetProgress() < pstConfig->m_iTargetAmount)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DAILY_TASK_NOT_COMPLETED)
    }

    if (iterTask->second->GetState() == CLogicDailyTaskSystem::TASK_HAS_ACHIEVE_GIFT)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DAILY_TASK_HAS_ACHIEVED_TODAY)
    }

    iterTask->second->SetState(CLogicDailyTaskSystem::TASK_HAS_ACHIEVE_GIFT);
    CLogicCacheCommon::AddGameItem(m_pUserInfo, pstConfig->m_stRewardItemVec, m_usCmd);
    CLogicUserNotifySystem::NotifyMonthPassTask(m_pUserInfo, stReqBody.m_iTaskID, iterTask->second->GetProgress(), iterTask->second->GetState());

    stRspBody.m_iTaskID = stReqBody.m_iTaskID;
    stRspBody.m_stItemVec = pstConfig->m_stRewardItemVec;
    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicMonthPassProcessor::DoUserRunMonthPassExchange(const msgpack::unpacked& stFormatData)
{
    CRequestMonthPassExchange stReqBody;
    CResponseMonthPassExchange stRspBody;
    if (!DecodeMsgPack(stFormatData, stReqBody))
    {
        return false;
    }

    const auto* pstConfig = CLogicConfig::GetMonthPassConfig().GetExchangeConfig(stReqBody.m_iExchangeID);
    if (pstConfig == nullptr)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DEPOSIT_ID_NOT_FOUND);
    }

    if (pstConfig->m_iNeedPass && !IsBuyMonthPass())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::UNKNOW_GAME_ITEM_NOT_IN_CONFIG);
    }

    auto& stUserExchangeMap = m_pUserInfo->m_stMonthPassTaskMap[CLogicConfigDefine::MONTH_PASS_TYPE_EXCHANGE];
    auto iterExchange = stUserExchangeMap.Find(stReqBody.m_iExchangeID);
    if (iterExchange == stUserExchangeMap.End())
    {
        TMonthPassTaskTableValueType value;
        value.m_iUid = m_pUserInfo->m_iUin;
        value.m_iGroupID = m_pUserInfo->m_iGroupID;
        value.m_iTaskType = CLogicConfigDefine::MONTH_PASS_TYPE_EXCHANGE;
        value.m_iTaskID = stReqBody.m_iExchangeID;
        value.m_iProgress = 0;
        auto result = stUserExchangeMap.Insert(std::make_pair(stReqBody.m_iExchangeID, value));
        iterExchange = result.first;
    }

    if (iterExchange->second->GetProgress() >= pstConfig->m_iMaxCount)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::UNKNOW_GAME_ITEM_NOT_IN_CONFIG);
    }

    if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pstConfig->m_stConsumeItem, m_usCmd))
    {
        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
    }

    iterExchange->second->AddProgress(1);
    CLogicCacheCommon::AddGameItem(m_pUserInfo, pstConfig->m_stBonusItem, m_usCmd);
    CLogicUserNotifySystem::NotifyMonthPass(m_pUserInfo);

    stRspBody.m_iExchangeID = stReqBody.m_iExchangeID;
    stRspBody.m_stConsumeItem = pstConfig->m_stConsumeItem;
    stRspBody.m_stBonusItem = pstConfig->m_stBonusItem;
    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicMonthPassProcessor::DoUserRunMonthPassWishItem(const msgpack::unpacked& stFormatData)
{
    CRequestMonthPassWishItem stReqBody;
    CResponseMonthPassWishItem stRspBody;
    if (!DecodeMsgPack(stFormatData, stReqBody))
    {
        return false;
    }

    if (stReqBody.m_iPoolIndex < 1 || stReqBody.m_iPoolIndex > 20)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    const auto* pstPoolConfig = CLogicConfig::GetMonthPassConfig().GetWishPoolConfig(stReqBody.m_iPoolIndex);
    if (pstPoolConfig == nullptr)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if (GetMonthPassLevel() < pstPoolConfig->m_iNeedLevel)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL);
    }

    if (m_pUserInfo->m_stIBusyData.GetMonthPassRecharge() < pstPoolConfig->m_iNeedCharge)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ADD_UP_RMB);
    }

    auto stItemIndexVec = m_pUserInfo->m_stIBusyData.GetMonthPassWishItemIndex();
    if (int32_t(stItemIndexVec.size()) < stReqBody.m_iPoolIndex)
    {
        stItemIndexVec.resize(stReqBody.m_iPoolIndex, 0);
    }

    if (stReqBody.m_iItemIndex > 0)
    {
        const auto* pstItemConfig = CLogicConfig::GetMonthPassConfig().GetWishItemConfig(stReqBody.m_iItemIndex);
        if (pstItemConfig == nullptr)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        if (pstItemConfig->m_iNeedPass && !IsBuyMonthPass())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::UNKNOW_GAME_ITEM_NOT_IN_CONFIG);
        }

        if (std::find(stItemIndexVec.begin(), stItemIndexVec.end(), stReqBody.m_iItemIndex) != stItemIndexVec.end())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::XYQD_XUYUAN_DOING);
        }
    }

    stItemIndexVec.at(stReqBody.m_iPoolIndex - 1) = stReqBody.m_iItemIndex;
    m_pUserInfo->m_stIBusyData.SetMonthPassWishItemIndex(stItemIndexVec);

    CLogicUserNotifySystem::NotifyMonthPass(m_pUserInfo);

    stRspBody.m_iPoolIndex = stReqBody.m_iPoolIndex;
    stRspBody.m_iItemIndex = stReqBody.m_iItemIndex;
    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicMonthPassProcessor::DoUserRunMonthPassDailyWish(const msgpack::unpacked& stFormatData)
{
    if (m_pUserInfo->m_stIBusyData.GetMonthPassWishEventID() > 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
    }

    int32_t iTotalWeight = 0;
    std::map<int32_t, int32_t> stEventWeight;
    for (const auto& event : CLogicConfig::GetMonthPassConfig().m_stWishEventMap)
    {
        if (m_pUserInfo->m_stIBusyData.GetMonthPassDay() >= event.second.m_iStartDay && m_pUserInfo->m_stIBusyData.GetMonthPassDay() <= event.second.m_iEndDay)
        {
            if (event.first > 0)
            {
                iTotalWeight += event.second.m_iWeight;
                stEventWeight.emplace(event.first, event.second.m_iWeight);
            }
        }
    }

    int32_t iEventID = 0;
    if (iTotalWeight > 0 && !stEventWeight.empty())
    {
        int32_t iRand = CLogicCommon::GetRandNum() % iTotalWeight;
        for (const auto& weight : stEventWeight)
        {
            if (iRand < weight.second)
            {
                iEventID = weight.first;
                break;
            }
            iRand -= weight.second;
        }
    }

    const auto* pstEventConfig = CLogicConfig::GetMonthPassConfig().GetWishEventConfig(iEventID);
    if (pstEventConfig != nullptr)
    {
        auto& stUserWishMap = m_pUserInfo->m_stMonthPassTaskMap[CLogicConfigDefine::MONTH_PASS_TYPE_WISH];
        auto iterWish = stUserWishMap.Find(pstEventConfig->m_iBonusIndex);
        if (iterWish == stUserWishMap.End())
        {
            TMonthPassTaskTableValueType value;
            value.m_iUid = m_pUserInfo->m_iUin;
            value.m_iGroupID = m_pUserInfo->m_iGroupID;
            value.m_iTaskType = CLogicConfigDefine::MONTH_PASS_TYPE_WISH;
            value.m_iTaskID = pstEventConfig->m_iBonusIndex;
            value.m_iProgress = 0;
            auto result = stUserWishMap.Insert(std::make_pair(pstEventConfig->m_iBonusIndex, value));
            iterWish = result.first;
        }
        iterWish->second->AddProgress(pstEventConfig->m_iBonusNum);

        m_pUserInfo->m_stIBusyData.SetMonthPassWishEventID(iEventID);

        CLogicUserNotifySystem::NotifyMonthPass(m_pUserInfo);

        CPackFightReportItem reportItem;
        reportItem.m_iReportType = CLogicConfigDefine::FIGHT_REPORT_MONTH_PASS;
        reportItem.m_iReportID = CLogicCommon::GetSec();
        reportItem.m_strReportContent = std::to_string(iEventID);
        reportItem.m_cFightResult = 0;
        CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);
    }

    CResponseMonthPassDailyWish stRspBody;
    stRspBody.m_iEventID = m_pUserInfo->m_stIBusyData.GetMonthPassWishEventID();
    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
    return true;
}

int32_t CLogicMonthPassProcessor::GetMonthPassLevel()
{
    return CLogicConfig::GetMonthPassConfig().GetLevelByExp(m_pUserInfo->m_stIBusyData.GetMonthPassExp());
}

bool CLogicMonthPassProcessor::IsBuyMonthPass()
{
    auto iter = m_pUserInfo->m_stUserDepositMap.Find(CLogicConfig::GetMonthPassConfig().m_iDepositID);
    return iter != m_pUserInfo->m_stUserDepositMap.End() && iter->second->GetBuyTimes() > 0;
}
