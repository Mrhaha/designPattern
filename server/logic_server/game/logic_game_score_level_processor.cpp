#include "common_datetime.h"
#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_score_level_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_game_score_level_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_active.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_cache_level_operation.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_user_stat.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_SCORE_LEVEL_GET_ACTIVE_INFO, CLogicScoreLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SCORE_LEVEL_FIGHT_START, CLogicScoreLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SCORE_LEVEL_FIGHT_RESULT, CLogicScoreLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SCORE_LEVEL_FIGHT_SWEEP, CLogicScoreLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SCORE_LEVEL_GET_SCORE_AWARD, CLogicScoreLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SCORE_LEVEL_BUY_XINGDONGLI, CLogicScoreLevelProcessor)

CLogicScoreLevelProcessor::CLogicScoreLevelProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicScoreLevelProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	bool result = false;

    CLogicActive::AutoAddScoreLevelXingdongli(m_pUserInfo);

	switch ((int)m_usCmd)
    {
        case MSG_LOGIC_SCORE_LEVEL_GET_ACTIVE_INFO:
            result =  DoUserRunGetActiveInfo(stFormatData);
            break;

        case MSG_LOGIC_SCORE_LEVEL_FIGHT_START:
            result =  DoUserRunFightStart(stFormatData);
            break;

        case MSG_LOGIC_SCORE_LEVEL_FIGHT_RESULT:
            result =  DoUserRunFightResult(stFormatData);
            break;

        case MSG_LOGIC_SCORE_LEVEL_FIGHT_SWEEP:
            result =  DoUserRunFightSweep(stFormatData);
            break;

        case MSG_LOGIC_SCORE_LEVEL_GET_SCORE_AWARD:
            result =  DoUserRunGetScoreAward(stFormatData);
            break;

        case MSG_LOGIC_SCORE_LEVEL_BUY_XINGDONGLI:
            result =  DoUserRunBuyXingdongli(stFormatData);
            break;

        default:
            break;
    }


    return result;
}

bool CLogicScoreLevelProcessor::DoUserRunGetActiveInfo(const msgpack::unpacked &stFormatData)
{
	CLogicUserNotifySystem::NotifyScoreLevelActivityData(m_pUserInfo);

	SendToClient(SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicScoreLevelProcessor::DoUserRunFightStart(const msgpack::unpacked &stFormatData)
{
    CRequestScoreLevelFightStart stReqBody;
    CResponseScoreLevelFightStart stRspBody;

    if (!DecodeMsgPack(stFormatData, stReqBody)) return (false);

    if (stReqBody.m_iIndex <= 0 || stReqBody.m_iIndex >= 60)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if (stReqBody.m_iCount <= 0 || stReqBody.m_iCount > 99)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
    }

    if (stReqBody.m_bChallenge && stReqBody.m_iCount != 1)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    const auto activity_id = CLogicActive::GetScoreLevelOpenActivityID();
    auto iter_activity = m_pUserInfo->m_stScoreLevelMap.Find(activity_id);
    if (iter_activity == m_pUserInfo->m_stScoreLevelMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }

    const auto* config = CLogicConfig::GetScoreLevelConfig().GetActivityConfig(activity_id);
    if (nullptr == config)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    auto get_bitmap_func = stReqBody.m_bChallenge
           ? std::bind(&TScoreLevelInfoTable::GetChallengeLevelBitmap, iter_activity->second)
           : std::bind(&TScoreLevelInfoTable::GetScoreLevelBitmap, iter_activity->second);
    const auto& fight_level_map = stReqBody.m_bChallenge ? config->m_stChallengeLevelMap : config->m_stScoreLevelMap;
    const auto iter_fight_level = fight_level_map.find(stReqBody.m_iIndex);
    if (iter_fight_level == fight_level_map.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LEVEL_IS_NOT_IN_OPEN_TIME);
    }
    if (iter_activity->second->GetXingdongli() < iter_fight_level->second.m_iXingdongli * stReqBody.m_iCount)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ACTION_COIN);
    }

    const auto flag = 1ULL << stReqBody.m_iIndex;
    if (stReqBody.m_bChallenge && (get_bitmap_func() & flag))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SCORELEVEL_LEVEL_CANNOT_FIGHT);
    }

    for (const auto& param : iter_fight_level->second.m_stNeedParams)
    {
        if (param.first == 1 && day_diff(config->m_iBeginTime, CLogicCommon::GetSec()) + 1 < param.second)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LEVEL_IS_NOT_IN_OPEN_TIME);
        }
        if (param.first == 2 && m_pUserInfo->m_stUserInfoTable.GetLevel() < param.second)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_TEAM_LEVEL);
        }
        if (param.first == 3 && iter_activity->second->GetScore() < param.second)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SCORELEVEL_LEVEL_CANNOT_FIGHT);
        }
        if (param.first == 4 && param.second > 0 && param.second < 60 && (get_bitmap_func() & (1ULL << param.second)) == 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SCORELEVEL_LEVEL_CANNOT_FIGHT);
        }
        if (param.first == 5 && bin1num(iter_activity->second->GetScoreAwardBitmap()) < param.second)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SCORELEVEL_LEVEL_CANNOT_FIGHT);
        }
        if (param.first < 1 || param.first > 5)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_CONDITION_IS_NOT_REACHED);
        }
    }

	m_pUserInfo->m_stUserInfoTable.SetFightLevelID(iter_fight_level->second.m_iLevelID);

	stRspBody.m_iIndex = stReqBody.m_iIndex;
	stRspBody.m_iCount = stReqBody.m_iCount;
	stRspBody.m_bChallenge = stReqBody.m_bChallenge;
	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicScoreLevelProcessor::DoUserRunFightResult(const msgpack::unpacked &stFormatData)
{
    CRequestScoreLevelFightResult stReqBody;
    CResponseScoreLevelFightResult stRspBody;

    if (!DecodeMsgPack(stFormatData, stReqBody)) return (false);

    if (stReqBody.m_iIndex <= 0 || stReqBody.m_iIndex >= 60)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if (stReqBody.m_iCount <= 0 || stReqBody.m_iCount > 99)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
    }

    if (stReqBody.m_bChallenge && stReqBody.m_iCount != 1)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    const auto activity_id = CLogicActive::GetScoreLevelOpenActivityID();
    auto iter_activity = m_pUserInfo->m_stScoreLevelMap.Find(activity_id);
    if (iter_activity == m_pUserInfo->m_stScoreLevelMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }

    const auto* activity_config = CLogicConfig::GetScoreLevelConfig().GetActivityConfig(activity_id);
    if (nullptr == activity_config)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    auto get_bitmap_func = stReqBody.m_bChallenge
                           ? std::bind(&TScoreLevelInfoTable::GetChallengeLevelBitmap, iter_activity->second)
                           : std::bind(&TScoreLevelInfoTable::GetScoreLevelBitmap, iter_activity->second);
    auto set_bitmap_func = stReqBody.m_bChallenge
                           ? std::bind(&TScoreLevelInfoTable::SetChallengeLevelBitmap, iter_activity->second, std::placeholders::_1)
                           : std::bind(&TScoreLevelInfoTable::SetScoreLevelBitmap, iter_activity->second, std::placeholders::_1);

    const auto& fight_level_map = stReqBody.m_bChallenge ? activity_config->m_stChallengeLevelMap : activity_config->m_stScoreLevelMap;
    const auto iter_fight_level = fight_level_map.find(stReqBody.m_iIndex);
    if (iter_fight_level == fight_level_map.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LEVEL_IS_NOT_IN_OPEN_TIME);
    }
    if (iter_fight_level->second.m_iLevelID != m_pUserInfo->m_stUserInfoTable.GetFightLevelID())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
    }
    if (iter_activity->second->GetXingdongli() < iter_fight_level->second.m_iXingdongli * stReqBody.m_iCount)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ACTION_COIN);
    }

    const auto flag = 1ULL << stReqBody.m_iIndex;
    if (stReqBody.m_bChallenge && (get_bitmap_func() & flag))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SCORELEVEL_LEVEL_CANNOT_FIGHT);
    }

    const auto* level_config = CLogicConfig::GetLevelConfig().GetLevelConfigElem(iter_fight_level->second.m_iLevelID);
    if (nullptr == level_config)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LEVEL_INFO_NOT_FOUND_IN_CONFIG);
    }

    stRspBody.m_iIndex = stReqBody.m_iIndex;
    stRspBody.m_iCount = stReqBody.m_iCount;
    stRspBody.m_bChallenge = stReqBody.m_bChallenge;
    stRspBody.m_iAddScore = iter_fight_level->second.m_iScore * stReqBody.m_iCount;
    stRspBody.m_iCostXingdongli = iter_fight_level->second.m_iXingdongli * stReqBody.m_iCount;
    for (auto index = 0; index < stReqBody.m_iCount; ++index)
    {
        CLogicConfig::GetLevelDropConfig().GetRandBonusByName(level_config->m_strPerfectCardBag, stRspBody.m_stBonusItemVec);
    }

    set_bitmap_func(get_bitmap_func() | flag);
    iter_activity->second->AddXingdongli(-stRspBody.m_iCostXingdongli);
    iter_activity->second->AddScore(stRspBody.m_iAddScore);
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusItemVec, m_usCmd);

    CLogicUserNotifySystem::NotifyScoreLevelActivityData(m_pUserInfo);
    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

    return (true);
}

bool CLogicScoreLevelProcessor::DoUserRunFightSweep(const msgpack::unpacked &stFormatData)
{
	return (true);
}

bool CLogicScoreLevelProcessor::DoUserRunGetScoreAward(const msgpack::unpacked &stFormatData)
{
    CRequestScoreLevelScoreAward stReqBody;
    CResponseScoreLevelScoreAward stRspBody;

    if (!DecodeMsgPack(stFormatData, stReqBody)) return (false);

    if (stReqBody.m_iIndex <= 0 || stReqBody.m_iIndex >= 60)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    const auto activity_id = CLogicActive::GetScoreLevelOpenActivityID();
    auto iter_activity = m_pUserInfo->m_stScoreLevelMap.Find(activity_id);
    if (iter_activity == m_pUserInfo->m_stScoreLevelMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }

    const auto* config = CLogicConfig::GetScoreLevelConfig().GetActivityConfig(activity_id);
    if (nullptr == config)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    const auto iter_award = config->m_stRewardMap.find(stReqBody.m_iIndex);
    if (iter_award == config->m_stRewardMap.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    if (iter_activity->second->GetScore() < iter_award->second.m_iTarget)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_CONDITION_IS_NOT_REACHED);
    }

    if (stReqBody.m_bDeposit && !CLogicUserStat::HaveBuyDepositID(m_pUserInfo, config->m_iScoreDepositID))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DEPOSIT_ID_NOT_FOUND);
    }

    const auto flag = 1ULL << stReqBody.m_iIndex;
    auto get_bitmap_func = stReqBody.m_bDeposit
            ? std::bind(&TScoreLevelInfoTable::GetDepositAwardBitmap, iter_activity->second)
            : std::bind(&TScoreLevelInfoTable::GetScoreAwardBitmap, iter_activity->second);
    auto set_bitmap_func = stReqBody.m_bDeposit
           ? std::bind(&TScoreLevelInfoTable::SetDepositAwardBitmap, iter_activity->second, std::placeholders::_1)
           : std::bind(&TScoreLevelInfoTable::SetScoreAwardBitmap, iter_activity->second, std::placeholders::_1);
    const auto& bonus_item_vec = stReqBody.m_bDeposit ? iter_award->second.m_stDepositItemVec : iter_award->second.m_stBonusItemVec;

    if (get_bitmap_func() & flag)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
    }

    set_bitmap_func(get_bitmap_func() | flag);
    CLogicCacheCommon::AddGameItem(m_pUserInfo, bonus_item_vec, m_usCmd);
    CLogicUserNotifySystem::NotifyScoreLevelActivityData(m_pUserInfo);

    stRspBody.m_iIndex = stReqBody.m_iIndex;
    stRspBody.m_bDeposit = stReqBody.m_bDeposit;
    stRspBody.m_stBonusItemVec = bonus_item_vec;

    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

    return (true);
}

bool CLogicScoreLevelProcessor::DoUserRunBuyXingdongli(const msgpack::unpacked &stFormatData)
{
    CResponseScoreLevelBuyXingdongli stRspBody;

    const auto activity_id = CLogicActive::GetScoreLevelOpenActivityID();
    auto iter_activity = m_pUserInfo->m_stScoreLevelMap.Find(activity_id);
    if (iter_activity == m_pUserInfo->m_stScoreLevelMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }

    const auto* config = CLogicConfig::GetScoreLevelConfig().GetActivityConfig(activity_id);
    if (nullptr == config)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    const auto have_buy_count = iter_activity->second->GetBuyXingdongliCount();
    const auto iter_buy_cost = config->m_stBuyCostMap.find(have_buy_count + 1);
    if (iter_buy_cost == config->m_stBuyCostMap.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
    }

    if (m_pUserInfo->m_stUserInfoTable.GetVipLevel() < iter_buy_cost->second.m_iNeedVipLevel)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::VIP_LEVEL_TOO_LOW);
    }

    if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo
            , CLogicConfigDefine::CURRENCY
            , CLogicConfigDefine::ECIT_YuanBaoID
            , iter_buy_cost->second.m_iCost
            , m_usCmd))
    {
        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
    }

    iter_activity->second->AddBuyXingdongliCount(1);
    iter_activity->second->AddXingdongli(config->m_iSingleBuyXingdongli);
    CLogicUserNotifySystem::NotifyScoreLevelActivityData(m_pUserInfo);

    stRspBody.m_iCostYuanbao = iter_buy_cost->second.m_iCost;
    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

    return (true);
}


