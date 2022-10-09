#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_active_info.h"
#include "logic_config.h"
#include "logic_game_active.h"
#include "logic_game_active_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_global.h"
#include "logic_game_global_frame.h"
#include "logic_game_cache_level_operation.h"
#include "logic_game_global.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_cache_common.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ACTIVE_INFO, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ALL_ACTIVE_INFO, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_RECEIVE_ACTIVE_BONUS, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_AWARD_ACTIVITY_MULTI_TASK, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ACTIVITY_TASK_AWARD, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_GET_SEQUENCE_TASK_REWARD, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ACTIVITY_COUNTDOWN_HERO, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_RANK_COUNTDOWN_HERO, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_LUCKY_CAT_HISTORY, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FLOWER_LANTER_LOG, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_MERGE_ACTIVITY_TASK_AWARD, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_MERGE_ACTIVITY_EXCHANGE, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_MERGE_ACTIVITY, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_MERGE_ACTIVITY_RESULT, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_DAY_SINGLE_RECHARGE_INFO, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_DAY_SINGLE_RECHARGE_REWARD, CLogicActiveProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_GET_WORLD_QUEST_INFO, CLogicActiveProcessor)

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_AVY_REWARD, CLogicActiveProcessor)

CLogicActiveProcessor::CLogicActiveProcessor(uint16_t usCmd, const std::string& strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicActiveProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	bool bRet = false;

	switch (m_usCmd)
	{
	case MSG_LOGIC_GET_ACTIVE_INFO:
		bRet = DoUserRunGetActiveInfo(stFormatData);
		break;

	case MSG_LOGIC_GET_ALL_ACTIVE_INFO:
		bRet = DoUserRunGetAllActiveInfo(stFormatData);
		break;

	case MSG_LOGIC_RECEIVE_ACTIVE_BONUS:
		bRet = DoUserRunReceiveActiveBonus(stFormatData);
		break;

	case MSG_LOGIC_AWARD_ACTIVITY_MULTI_TASK:
		bRet = DoUserRunAwardActivityMultiTask(stFormatData);
		break;

	case MSG_LOGIC_GET_ACTIVITY_TASK_AWARD:
		bRet = DoUserRunGetActivityTaskAward(stFormatData);
		break;

    case MSG_LOGIC_TOUHOU_GET_SEQUENCE_TASK_REWARD:
        bRet = DoUserRunGetSequenceTaskReward(stFormatData);
        break;
	
	case MSG_LOGIC_GET_ACTIVITY_COUNTDOWN_HERO://抽取限时武将
		bRet = DoUserRunGetActivityCountDownHero(stFormatData);
		break;

	case MSG_LOGIC_GET_RANK_COUNTDOWN_HERO:
		bRet = DoUserRunGetRankCountDownHero(stFormatData);//获取限时武将排行
		break;

	case MSG_LOGIC_GET_LUCKY_CAT_HISTORY:
		bRet = DoUserRunGetLuckyCatHistory(stFormatData);//获取招财猫历史记录
		break;	

	case MSG_LOGIC_GET_FLOWER_LANTER_LOG:
		bRet = DoUserRunGetFlowerLanternLog(stFormatData);
		break;

	case MSG_LOGIC_MERGE_ACTIVITY_TASK_AWARD:
		bRet = DoUserRunMergeActivityTaskAward(stFormatData);
		break;

	case MSG_LOGIC_MERGE_ACTIVITY_EXCHANGE:
		bRet = DoUserRunMergeActivityExchange(stFormatData);
		break;

	case MSG_LOGIC_FIGHT_MERGE_ACTIVITY:
		bRet = DoUserRunFightMergeActivity(stFormatData);
		break;

	case MSG_LOGIC_FIGHT_MERGE_ACTIVITY_RESULT:
		bRet = DoUserRunFightMergeActivityResult(stFormatData);
		break;

	case MSG_LOGIC_GET_DAY_SINGLE_RECHARGE_INFO:
		bRet = DoUserRunGetDaySingleRechargeInfo(stFormatData);
		break;

	case MSG_LOGIC_GET_DAY_SINGLE_RECHARGE_REWARD:
		bRet = DoUserRunGetDaySingleRechargeReward(stFormatData);
		break;

    case MSG_LOGIC_TOUHOU_GET_WORLD_QUEST_INFO:
        bRet = DoUserRunGetWorldQuestInfo(stFormatData);
        break;

    case MSG_LOGIC_GET_AVY_REWARD:
        bRet = DoUserRunGetAvyReward(stFormatData);
        break;

	default:
		PROCESSOR_LOG_ERROR << "PROCESS FUNCTION NOT FOUND!"  << std::endl;
		break;
	}

	return bRet;
}

bool CLogicActiveProcessor::DoUserRunGetActiveInfo(const msgpack::unpacked & stFormatData)
{
	CRequestGetActiveInfo stParam;
	if (DecodeMsgPack(stFormatData, stParam) == false) return (false);
	auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(m_pUserInfo, stParam.m_iActiveID);
	if (pActiveObjPtr.get() == nullptr)
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
			<< "|MSG:" << CLogicError::GetErrMsg()
			<< "|ACTIVE_ID:" << stParam.m_iActiveID << std::endl;

		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	CLogicActiveBase::active_info_t stInfo = pActiveObjPtr->GetActiveInfo();

	CResponseGetActiveInfo stRsp;
	stRsp.m_iActiveID = stParam.m_iActiveID;
	stRsp.m_iTargetNum = stInfo.m_iTargetNum;
	stRsp.m_iLastReceiveAwardTime = stInfo.m_iLastReceiveAwardTime;
	stRsp.m_strReceivedBitmap = int2bin<uint64_t>(stInfo.m_ulReceivedBitmap);
	stRsp.m_strCompleteBitmap = int2bin<uint64_t>(stInfo.m_ulCompleteBitmap);
	stRsp.m_stParamMap = pActiveObjPtr->GetParamMap();
	stRsp.m_strParaBitmap = int2bin<uint64_t>(stInfo.m_ulParaEx);

	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

	PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|ACTIVE_ID:" << stParam.m_iActiveID << std::endl;

	return true;
}

bool CLogicActiveProcessor::DoUserRunGetAllActiveInfo(const msgpack::unpacked & stFormatData)
{
	CResponseGetAllActiveInfo stRsp;

	CLogicActiveBase::GetAllActiveInfo(m_pUserInfo, stRsp.m_stActiveInfo);

	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

	PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

	return true;
}

bool CLogicActiveProcessor::DoUserRunReceiveActiveBonus(const msgpack::unpacked & stFormatData)
{
	CResponseGetActiveGift stRsp;
	CRequestGetActiveGift stParam;

	if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

    if(stParam.m_iTimes < 1) stParam.m_iTimes = 1;
    if(stParam.m_iTimes > 999) stParam.m_iTimes = 999;

	auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(m_pUserInfo, stParam.m_iActiveID);
	if (pActiveObjPtr.get() == nullptr)
	{
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
			<< "|MSG:" << CLogicError::GetErrMsg()
			<< "|ACTIVE_ID:" << stParam.m_iActiveID << std::endl;

		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	if (pActiveObjPtr->GetActiveGift(stParam.m_iEntityID, stRsp.m_stGameItemList, m_usCmd, stParam.m_iTimes))
	{
		CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stGameItemList, m_usCmd, {
                {"id", std::to_string(stParam.m_iActiveID)}, { "index", std::to_string(stParam.m_iEntityID) }
		});

		CLogicActiveBase::active_info_t stInfo = pActiveObjPtr->GetActiveInfo();

		stRsp.m_iActiveID = stParam.m_iActiveID;
		stRsp.m_iLastReceiveAwardTime = stInfo.m_iLastReceiveAwardTime;
		stRsp.m_strReceivedBitmap = int2bin<uint64_t>(stInfo.m_ulReceivedBitmap);
		stRsp.m_strCompleteBitmap = int2bin<uint64_t>(stInfo.m_ulCompleteBitmap);
		stRsp.m_stParamMap = pActiveObjPtr->GetParamMap();
		stRsp.m_strParaBitmap = int2bin<uint64_t>(stInfo.m_ulParaEx);
        stRsp.m_iEntityID = stParam.m_iEntityID;
        stRsp.m_iTimes = stParam.m_iTimes;
        stRsp.m_stConstelVec = m_pUserInfo->m_stCacheBonusData.m_stBonus.m_stConstelVec;

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|ACTIVE_ID:" << stParam.m_iActiveID << std::endl;
	}
	else
	{
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
			<< "|MSG:" << CLogicError::GetErrMsg()
			<< "|ACTIVE_ID:" << stParam.m_iActiveID << std::endl;

		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	return true;
}

bool CLogicActiveProcessor::DoUserRunAwardActivityMultiTask(const msgpack::unpacked & stFormatData)
{
	CRequestAwardActivityMultiTask stReqBody;
	CResponseAwardActivityMultiTask stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstActivityConfig = CLogicActive::GetOpenMultiTaskConfig(m_pUserInfo);
	const auto* pstTaskConfig = CLogicActive::GetOpenMultiTaskSingleConfig(m_pUserInfo,
																				   stReqBody.m_iTaskID);
	if (nullptr == pstActivityConfig || nullptr == pstTaskConfig)
	{
		PROCESSOR_LOG_ERROR << "ERROR:TASK ID NOT FOUND"  << "|TASK_ID:" << stReqBody.m_iTaskID << std::endl;

		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MULTI_TASK_NOT_FOUND);
	}

	const auto stTupleKey = std::make_tuple(CLogicConfigDefine::ACTIVITY_TYPE_MULTI_TASK, pstActivityConfig->m_iActivityID, pstTaskConfig->m_iTaskID);
	auto iterActivityTask = m_pUserInfo->m_stActivityTaskMap.Find(stTupleKey);
	if (iterActivityTask == m_pUserInfo->m_stActivityTaskMap.End() || iterActivityTask->second->GetProgress() < pstTaskConfig->m_iTargetAmount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MULTI_TASK_NOT_ENOUGH);
	}

	if (iterActivityTask->second->GetState() == CLogicDailyTaskSystem::TASK_HAS_ACHIEVE_GIFT)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MULTI_TASK_HAVE_AWARD);
	}

	iterActivityTask->second->SetState(CLogicDailyTaskSystem::TASK_HAS_ACHIEVE_GIFT);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, pstTaskConfig->m_stBonusItemVec, m_usCmd);

	CLogicUserNotifySystem::NotifyUpdateActivityTask(m_pUserInfo, CLogicConfigDefine::ACTIVITY_TYPE_MULTI_TASK, pstActivityConfig->m_iActivityID, pstTaskConfig->m_iTaskID, iterActivityTask->second->GetProgress(), iterActivityTask->second->GetState());

	logic_award_activity_multi_task_channel_data_t stActionData;
	stActionData.m_iActivityID = pstActivityConfig->m_iActivityID;
	stActionData.m_iAwardNum = 1;
	LOGIC_PUBLISHER_INS->PublishAwardActivityMultiTaskInfo(m_pUserInfo, stActionData);

	stRspBody.m_iTaskID = stReqBody.m_iTaskID;
	stRspBody.m_stBonusItemVec = pstTaskConfig->m_stBonusItemVec;
	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicActiveProcessor::DoUserRunGetActivityTaskAward(const msgpack::unpacked& stFormatData)
{
	CRequestGetActivityTaskAward stReqBody;
	CResponseGetActivityTaskAward stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicActive::GetOpenActivityTaskConfig(m_pUserInfo, stReqBody.m_iActivityType,
																			stReqBody.m_iActivityID,
																			stReqBody.m_iTaskID);
	if (nullptr == pstConfig)
	{
		PROCESSOR_LOG_ERROR << "ERROR:TASK ID NOT FOUND" << "|TASK_ID:" << stReqBody.m_iTaskID << std::endl;

		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MULTI_TASK_NOT_FOUND);
	}

	const auto stTupleKey = std::make_tuple(stReqBody.m_iActivityType, stReqBody.m_iActivityID, stReqBody.m_iTaskID);
	auto iterActivityTask = m_pUserInfo->m_stActivityTaskMap.Find(stTupleKey);
	if (iterActivityTask == m_pUserInfo->m_stActivityTaskMap.End() || iterActivityTask->second->GetProgress() < pstConfig->m_iTargetAmount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MULTI_TASK_NOT_ENOUGH);
	}

	if (iterActivityTask->second->GetState() == CLogicDailyTaskSystem::TASK_HAS_ACHIEVE_GIFT)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MULTI_TASK_HAVE_AWARD);
	}

	iterActivityTask->second->SetState(CLogicDailyTaskSystem::TASK_HAS_ACHIEVE_GIFT);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, pstConfig->m_stBonusItemVec, m_usCmd);

	CLogicUserNotifySystem::NotifyUpdateActivityTask(m_pUserInfo, stReqBody.m_iActivityType, stReqBody.m_iActivityID, stReqBody.m_iTaskID, iterActivityTask->second->GetProgress(), iterActivityTask->second->GetState());

    if(stReqBody.m_iActivityType == CLogicConfigDefine::ACTIVITY_TYPE_QUN_LIAO_CARD)
    {
        logic_game_user_action_channel_data_t stData;
        stData.m_iTimes = 1;
        stData.m_iAction = CLogicGameChannelEnum::ACTION_QUN_LIAO_CARD_TASK_COUNT;
        LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);
    }

	stRspBody.m_iActivityType = stReqBody.m_iActivityType;
	stRspBody.m_iActivityID = stReqBody.m_iActivityID;
	stRspBody.m_iTaskID = stReqBody.m_iTaskID;
	stRspBody.m_stBonusItemVec = pstConfig->m_stBonusItemVec;
	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicActiveProcessor::DoUserRunGetSequenceTaskReward(const msgpack::unpacked& stFormatData)
{
    CRequestGetActivityTaskAward stReqBody;
    CResponseGetActivityTaskAward stRspBody;

    if (!DecodeMsgPack(stFormatData, stReqBody))
    {
        return false;
    }

    const auto* pstConfig = CLogicActive::GetOpenActivitySequenceTaskConfig(m_pUserInfo, stReqBody.m_iActivityType, stReqBody.m_iActivityID, stReqBody.m_iTaskID);
    if (nullptr == pstConfig)
    {
        PROCESSOR_LOG_ERROR << "ERROR:TASK ID NOT FOUND" << "|TASK_ID:" << stReqBody.m_iTaskID << std::endl;

        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MULTI_TASK_NOT_FOUND);
    }

    const auto stTupleKey = std::make_tuple(stReqBody.m_iActivityType, stReqBody.m_iActivityID, stReqBody.m_iTaskID);
    auto iterActivityTask = m_pUserInfo->m_stActivityTaskMap.Find(stTupleKey);
    if (iterActivityTask != m_pUserInfo->m_stActivityTaskMap.End() && iterActivityTask->second->GetState() == CLogicDailyTaskSystem::TASK_HAS_ACHIEVE_GIFT)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MULTI_TASK_HAVE_AWARD);
    }

    for(auto iTargetTaskID : pstConfig->m_stTargetTaskList)
    {
        const auto stTargetTupleKey = std::make_tuple(stReqBody.m_iActivityType, stReqBody.m_iActivityID, iTargetTaskID);
        auto iterTaskData = m_pUserInfo->m_stActivityTaskMap.Find(stTargetTupleKey);
        if (iterTaskData == m_pUserInfo->m_stActivityTaskMap.End() || iterTaskData->second->GetState() != CLogicDailyTaskSystem::TASK_HAS_ACHIEVE_GIFT)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MULTI_TASK_NOT_ENOUGH);
        }
    }

    if(iterActivityTask == m_pUserInfo->m_stActivityTaskMap.End())
    {
        TActivityTaskTableValueType stVal;
        stVal.m_iUid = m_pUserInfo->m_iUin;
        stVal.m_iGroupID = m_pUserInfo->m_iGroupID;
        stVal.m_iActivityType = stReqBody.m_iActivityType;
        stVal.m_iActivityID = stReqBody.m_iActivityID;
        stVal.m_iTaskID = stReqBody.m_iTaskID;
        stVal.m_iProgress = 0;
        stVal.m_cState = CLogicDailyTaskSystem::TASK_UNDER_WAY;

        auto stRet = m_pUserInfo->m_stActivityTaskMap.Insert(std::make_pair(stTupleKey, stVal));
        if (stRet.second == false)
        {
            using namespace SERVER_ERRCODE;
            LOGIC_LOG_TRACE_ERROR << "ERROR:" << INTERN_ERROR << "|MSG:" << GET_MACRO_STRING(INTERN_ERROR)
                                  << "|ACTIVITY_TYPE:" << stReqBody.m_iActivityType << "|ACTIVITY_ID:" << stReqBody.m_iActivityID
                                  << "|TASK_ID:" << stReqBody.m_iTaskID << "|INSERT_INTO_CACHE_FAILD " << std::endl;
            SEND_ERRCODE_AND_RET(INTERN_ERROR);
        }

        iterActivityTask = stRet.first;
    }

    iterActivityTask->second->SetState(CLogicDailyTaskSystem::TASK_HAS_ACHIEVE_GIFT);

    static std::string strBonus;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, pstConfig->m_stRewards, strBonus, m_usCmd);

    CLogicUserNotifySystem::NotifyUpdateActivityTask(m_pUserInfo, stReqBody.m_iActivityType, stReqBody.m_iActivityID, stReqBody.m_iTaskID,
                                                     iterActivityTask->second->GetProgress(), iterActivityTask->second->GetState());

    stRspBody.m_iActivityType = stReqBody.m_iActivityType;
    stRspBody.m_iActivityID = stReqBody.m_iActivityID;
    stRspBody.m_iTaskID = stReqBody.m_iTaskID;
    stRspBody.m_stBonusItemVec = pstConfig->m_stRewards;
    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|ACTIVITY_TYPE:" << stReqBody.m_iActivityType << "|ACTIVITY_ID:" << stReqBody.m_iActivityID
                        << "|TASK_ID:" << stReqBody.m_iTaskID << "|BONUS_ITEM:" << strBonus << std::endl;

    return (true);
}

//抽取限时武将
bool CLogicActiveProcessor::DoUserRunGetActivityCountDownHero(const msgpack::unpacked& stFormatData)
{
	//抽奖

	CResponseActivityCountdownHero stRsp;
	CRequsetActivityCountdownHero stParam;

	if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

	if (stParam.m_iActivityID < 0)
	{
		using namespace SERVER_ERRCODE;
		SEND_ERRCODE_AND_RET(INTERN_ERROR)
	}

	auto iterActivity = CLogicConfig::GetActivityConfig().m_mapCountDownHeroConfig.find(stParam.m_iActivityID);
	if (iterActivity == CLogicConfig::GetActivityConfig().m_mapCountDownHeroConfig.end())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << ACTIVE_IS_NOT_IN_CONFIG
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(ACTIVE_IS_NOT_IN_CONFIG)
			<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(ACTIVE_IS_NOT_IN_CONFIG)
	}

	auto iterCommodity = CLogicConfig::GetActivityConfig().m_stActiveCommodity.find(stParam.m_iActivityID);
	if (iterCommodity == CLogicConfig::GetActivityConfig().m_stActiveCommodity.end())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << ACTIVE_IS_NOT_IN_CONFIG
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(ACTIVE_IS_NOT_IN_CONFIG)
			<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(ACTIVE_IS_NOT_IN_CONFIG)
	}

	if(stParam.m_bIsConsumeItem && iterCommodity->second.m_stConsumeItem.m_iItemType <= 0)
    {
        using namespace SERVER_ERRCODE;
        PROCESSOR_LOG_ERROR << "ERRCODE:" << PARAMETER_ERROR
                            << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                            << "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

        SEND_ERRCODE_AND_RET(PARAMETER_ERROR)
    }

    if (iterCommodity->second.m_iOpenLevel > m_pUserInfo->m_stUserInfoTable.GetLevel())
    {
        PROCESSOR_LOG_ERROR << "ERRCODE:" << SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL
                            << "|MSG:" << GET_ERRMSG_BY_ERRCODE(SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL)
                            << "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL)
    }

	auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(m_pUserInfo, stParam.m_iActivityID);
	if (!pActiveObjPtr)
	{
        PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                            << "|MSG:" << CLogicError::GetErrMsg()
			                << "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	int32_t iHasScore = 0;
	int32_t iHasNum = 0;
	auto stIter = m_pUserInfo->m_stUserActiveMap.Find(stParam.m_iActivityID);
	if (stIter != m_pUserInfo->m_stUserActiveMap.End())
	{
		iHasScore = stIter->second->GetActiveTargetNum();
	}
	if (iterCommodity->second.m_iCountScore > 0)
	{
		iHasNum = iHasScore / iterCommodity->second.m_iCountScore;
	}
	//随机宝箱
	int32_t iNum = 0;
	if (0 == stParam.m_iTenExtract)
	{
		iNum = 1;
	}
	else
	{
		iNum = 10;
	}
	iHasScore += iNum * iterCommodity->second.m_iCountScore;
	stRsp.m_iTenExtract = stParam.m_iTenExtract;
	stRsp.m_iActivityID = stParam.m_iActivityID;

	std::vector<TLogicRandBonusItem> stRandBonusVec;
	for (int32_t i = 1; i <= iNum; ++i)
	{
		const std::string strBagName = CLogicConfig::GetActivityConfig().GetActiveCommodityBuyMallBag(stParam.m_iActivityID, iHasNum + i);
		auto stTempVec = CLogicConfig::GetCardbagConfig().GetRandBonusByName(strBagName);
		if (1 != stTempVec.size())
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BAD_COMMODITY_ID);
		}
		stRandBonusVec.insert(stRandBonusVec.end(), stTempVec.begin(), stTempVec.end());
	}

	for (const auto& bonus : stRandBonusVec)
	{
		stRsp.m_stBonusItemVec.emplace_back(bonus.m_iItemType, bonus.m_iCardID, bonus.m_iNum);
	}

	if (int32_t(stRsp.m_stBonusItemVec.size()) != iNum)
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << BAD_COMMODITY_ID
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(BAD_COMMODITY_ID)
			<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(BAD_COMMODITY_ID)
	}

	if (m_pUserInfo->m_stUserDailyTable.GetFreeCountdownHeroTableCount() + iNum <= iterCommodity->second.m_iDailyFreeCount)
	{
		//免费次数
		m_pUserInfo->m_stUserDailyTable.AddFreeCountdownHeroTableCount(iNum);
		CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);
	}
	else
	{
	    if(stParam.m_bIsConsumeItem)
        {
	        stRsp.m_stConsume = iterCommodity->second.m_stConsumeItem;
            if (0 != stParam.m_iTenExtract)
            {
                stRsp.m_stConsume.m_iNum *= iNum;
            }
        }
	    else
        {
            stRsp.m_stConsume.m_iItemType = iterCommodity->second.m_iBuyItemType;
            stRsp.m_stConsume.m_iCardID = iterCommodity->second.m_iBuyItemID;
            stRsp.m_stConsume.m_iNum = iterCommodity->second.m_iBuyItemNum;
            if (0 != stParam.m_iTenExtract)
            {
                stRsp.m_stConsume.m_iNum = iterCommodity->second.m_iBuyItemNum2;
            }
        }

		if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRsp.m_stConsume, m_usCmd))
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}
	}

	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stBonusItemVec, m_usCmd);

	logic_game_user_action_channel_data_t stData;
	stData.m_iTimes = iNum * iterCommodity->second.m_iCountScore;
	stData.m_iAction = CLogicGameChannelEnum::ACTION_COUNTDOWN_HERO;
	LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

	//排行
	CLogicGlobal::UpdateOrInsertCountdownRank(m_iUin, m_iGroupID, iHasScore);

	//公告
    for (const auto& bonus : stRandBonusVec)
    {
        CLogicGlobal::PublishBroadcast("countdownhero", 0, bonus.m_iNotify, {
                { "nick", m_pUserInfo->m_strNick },
                { "item_name", CLogicConfig::GetItemName(bonus.m_iItemType, bonus.m_iCardID) },
                { "item_count", std::to_string(bonus.m_iNum) }
        });
    }

	return (true);
}

//获取限时武将排行
bool CLogicActiveProcessor::DoUserRunGetRankCountDownHero(const msgpack::unpacked& stFormatData)
{
	 
	CResponseActivityCountdownHeroRank stRsp;
	CRequsetActivityCountdownHeroRank stParam;

	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	auto stItBeg = LOGIC_COUNTDOWN_HERO_RANK_INS.begin();
	auto stItEnd = LOGIC_COUNTDOWN_HERO_RANK_INS.end();

	int iRankAmount = 0;
	user_info_key_t stKey;
	CPackRankCountdownHero stRankDataElem;
	for (; (iRankAmount < CLogicConfigDefine::MAX_COUNTDOWN_HERO_RANK_AMOUNT) && (stItBeg != stItEnd); ++stItBeg, ++iRankAmount)
	{
		stKey.m_ulRoleID = stItBeg->first;

		stRankDataElem = GetCountdownHeroPlayerInfo(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin, stItBeg->second, iRankAmount);

		stRsp.m_vecRankInfo.push_back(stRankDataElem);
	}

	stKey.m_stRoleIDDetail.m_iUin = m_iUin;
	stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;
	stRsp.m_iRank = LOGIC_COUNTDOWN_HERO_RANK_INS.GetRank(stKey.m_ulRoleID);
	stRsp.m_iActivityID = stParam.m_iActivityID;
	stRsp.m_iScrore = 0;
	auto stIter = m_pUserInfo->m_stUserActiveMap.Find(stRsp.m_iActivityID);
	if (stIter != m_pUserInfo->m_stUserActiveMap.End())
	{
		stRsp.m_iScrore = stIter->second->GetActiveTargetNum();
	}

	PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|RANK_AMOUNT:" << iRankAmount << std::endl;

	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return (true);
}

//获取招财猫历史记录
bool CLogicActiveProcessor::DoUserRunGetLuckyCatHistory(const msgpack::unpacked& stFormatData)
{
	CResponseActivityLuckyCatHistory stRsp;
	stRsp.m_listHistory = CLogicGlobal::GetLuckCatHistory();
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicActiveProcessor::DoUserRunGetFlowerLanternLog(const msgpack::unpacked& stFormatData)
{
	CRequestFlowerLanternLog stReqBody;
	CResponseFlowerLanternLog stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	stRspBody.m_iActivityID = stReqBody.m_iActivityID;
	for (auto iter = m_pUserInfo->m_stFlowerLanternMap.Begin(); iter != m_pUserInfo->m_stFlowerLanternMap.End(); ++iter)
	{
		if (iter->second->GetActivityID() == stReqBody.m_iActivityID)
		{
			CPackFlowerLanternLog stLog;
			stLog.m_iLogTime = iter->second->GetLogTime();
			stLog.m_iEvent = iter->second->GetEvent();
			stLog.m_iEventValue = iter->second->GetEventValue();
			stLog.m_iCandle = iter->second->GetCandle();
			stLog.m_iTotalValue = iter->second->GetTotalValue();
			stLog.m_iTotalCandle = iter->second->GetTotalCandle();

			stRspBody.m_stLogVec.push_back(std::move(stLog));
		}
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicActiveProcessor::DoUserRunMergeActivityTaskAward(const msgpack::unpacked & stFormatData)
{
	CRequestMergeActivityTaskAward stReqBody;
	CResponseMergeActivityTaskAward stRspBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicActive::GetOpenMergeActivityConfig(CLogicCommon::GetSec());
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_OVER);
	}

	const auto iterTaskConfig = pstConfig->m_stTaskMap.find(stReqBody.m_iTaskID);
	if (iterTaskConfig == pstConfig->m_stTaskMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DAILY_TASK_ID_NOT_FOUND_IN_CONFIG);
	}

	auto* pstTaskTable = FetchMergeActivityTable(stReqBody.m_iTaskID);
	if (nullptr == pstTaskTable)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DAILY_TASK_NOT_RECORD);
	}

	if (pstTaskTable->GetAward())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DAILY_TASK_HAS_ACHIEVED_TODAY);
	}

	if (pstTaskTable->GetProgress() < iterTaskConfig->second.m_iTargetAmount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::DAILY_TASK_NOT_COMPLETED);
	}

	pstTaskTable->SetAward(1);

	stRspBody.m_iTaskID = stReqBody.m_iTaskID;
	stRspBody.m_stBonusVec = iterTaskConfig->second.m_stBonusItemVec;
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusVec, m_usCmd);

	CLogicUserNotifySystem::NotifyMergeActivityChange(m_pUserInfo, stReqBody.m_iTaskID, pstTaskTable->GetProgress(), pstTaskTable->GetAward());


	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicActiveProcessor::DoUserRunMergeActivityExchange(const msgpack::unpacked & stFormatData)
{
	CRequestMergeActivityExchange stReqBody;
	CResponseMergeActivityExchange stRspBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicActive::GetOpenMergeActivityConfig(CLogicCommon::GetSec());
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_OVER);
	}

	const auto iterExchangeConfig = pstConfig->m_stExchangeMap.find(stReqBody.m_iTaskID);
	if (iterExchangeConfig == pstConfig->m_stExchangeMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BAD_COMMODITY_ID);
	}

	auto* pstExchangeTable = FetchMergeActivityTable(stReqBody.m_iTaskID);
	if (nullptr == pstExchangeTable)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GAME_ITEM_NOT_EXIST);
	}

	if (iterExchangeConfig->second.m_iExchangeCount > 0 && pstExchangeTable->GetProgress() >= iterExchangeConfig->second.m_iExchangeCount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_BUY_ITEM_TIMES);
	}

	stRspBody.m_iTaskID = stReqBody.m_iTaskID;
	stRspBody.m_stConsumeVec = iterExchangeConfig->second.m_stConsumeVec;
	stRspBody.m_stBonusVec = iterExchangeConfig->second.m_stBonusVec;

	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRspBody.m_stConsumeVec, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	if (iterExchangeConfig->second.m_iExchangeCount > 0)
	{
		pstExchangeTable->AddProgress(1);
	}
	
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusVec, m_usCmd);

	CLogicUserNotifySystem::NotifyMergeActivityChange(m_pUserInfo, stReqBody.m_iTaskID, pstExchangeTable->GetProgress(), pstExchangeTable->GetAward());

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicActiveProcessor::DoUserRunFightMergeActivity(const msgpack::unpacked & stFormatData)
{
	CRequestFightMergeActivity stReqBody;
	CResponseFightMergeActivity stRspBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	CLogicCacheCommon::AutoAddPhyPower(m_pUserInfo);

	const auto* pstConfig = CLogicActive::GetOpenMergeActivityConfig(CLogicCommon::GetSec());
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_OVER);
	}

	if (m_pUserInfo->m_stUserDailyTable.GetMergeLevelCount() >= pstConfig->m_iDailyLevelCount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
	}

	const auto iterLevel = pstConfig->m_stLevelMap.find(stReqBody.m_iLevelID);
	if (iterLevel == pstConfig->m_stLevelMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LEVEL_IS_NOT_IN_OPEN_TIME);
	}

	if (iterLevel->second > m_pUserInfo->m_stUserInfoTable.GetVipLevel())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::VIP_LEVEL_TOO_LOW);
	}

	auto pstConfigElem = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stReqBody.m_iLevelID);
	if (nullptr == pstConfigElem)
	{
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
			<< "|MSG:" << CLogicError::GetErrMsg()
			<< "|LEVEL_ID:" << stReqBody.m_iLevelID << std::endl;
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	CLogicCacheLevelOperation stOperation(m_pUserInfo);
	stOperation.FlushLevelTimes(stReqBody.m_iLevelID);
	if (false == stOperation.CheckCanFightLevel(stReqBody.m_iLevelID, pstConfigElem))
	{
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
			<< "|MSG:" << CLogicError::GetErrMsg()
			<< "|LEVEL_ID:" << stReqBody.m_iLevelID << std::endl;
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	m_pUserInfo->m_stUserInfoTable.SetFightLevelID(stReqBody.m_iLevelID);
    m_pUserInfo->m_stCacheFightLevelDropBonus.clear();
    CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pstConfigElem->m_strBonusCardBag, m_pUserInfo->m_stCacheFightLevelDropBonus);

	stRspBody.m_iLevelID = stReqBody.m_iLevelID;
	stRspBody.m_stDropVec = m_pUserInfo->m_stCacheFightLevelDropBonus;

	std::string strBonusList = CLogicCacheCommon::GetGameItemString(stRspBody.m_stDropVec);
	PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << stReqBody.m_iLevelID
		<< "|RAND_BONUS_LIST:" << strBonusList << std::endl;

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicActiveProcessor::DoUserRunFightMergeActivityResult(const msgpack::unpacked& stFormatData)
{
	CRequestFightMergeActivityResult stReqBody;
	CResponseFightMergeActivityResult stRspBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	CLogicCacheCommon::AutoAddPhyPower(m_pUserInfo);

	CLogicCacheLevelOperation stOperation(m_pUserInfo);
	if (!stOperation.CheckFightLevelID(stReqBody.m_iLevelID))
	{
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
			<< "|MSG:" << CLogicError::GetErrMsg()
			<< "|LEVEL_ID:" << stReqBody.m_iLevelID << std::endl;
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	auto pstConfigElem = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stReqBody.m_iLevelID);
	if (nullptr == pstConfigElem)
	{
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
			<< "|MSG:" << CLogicError::GetErrCode()
			<< "|LEVEL_ID:" << stReqBody.m_iLevelID << std::endl;

		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	stRspBody.m_iLevelID = stReqBody.m_iLevelID;
	stRspBody.m_iGetCoinNum = std::min(stReqBody.m_iGetCoinNum, pstConfigElem->m_iMaxCoinNum);

	if (!stOperation.CheckRspLevelDrop(stReqBody.m_stDropVec))
	{
		stReqBody.m_stDropVec = m_pUserInfo->m_stCacheFightLevelDropBonus;
	}

	stRspBody.m_stDropVec = stReqBody.m_stDropVec;
	CLogicConfig::GetLevelDropConfig().GetRandBonusByName(
		pstConfigElem->m_strPerfectCardBag,
		stRspBody.m_stPerfectBonusList);

	m_pUserInfo->m_stUserInfoTable.SetFightLevelID(0);
	m_pUserInfo->m_stUserDailyTable.AddMergeLevelCount(1);
	CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);

	CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GoldID, stRspBody.m_iGetCoinNum, m_usCmd);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stDropVec, m_usCmd);
	CLogicCacheCommon::AddGameItem(m_pUserInfo,stRspBody.m_stPerfectBonusList, m_usCmd);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

CPackRankCountdownHero CLogicActiveProcessor::GetCountdownHeroPlayerInfo(int iGroupID, int iUin, int iScore, int iRankAmount)
{
	CPackRankCountdownHero stPlayerInfo;
	stPlayerInfo.m_iUID = iUin;
	stPlayerInfo.m_iGroupID = iGroupID;

	auto pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
	if (pUserInfo)
	{
		stPlayerInfo.m_strName = pUserInfo->m_strNick;
	}
	stPlayerInfo.m_iRank = iRankAmount;
	stPlayerInfo.m_iScrore = iScore;
	return (stPlayerInfo);
}

TMergeActivityTable* CLogicActiveProcessor::FetchMergeActivityTable(int32_t iTaskID)
{
	auto iter = m_pUserInfo->m_stMergeActivityMap.Find(iTaskID);
	if (iter == m_pUserInfo->m_stMergeActivityMap.End())
	{
		TMergeActivityTableValueType value;
		value.m_iUid = m_pUserInfo->m_iUin;
		value.m_iGroupID = m_pUserInfo->m_iGroupID;
		value.m_iTaskID = iTaskID;
		value.m_iProgress = 0;
		value.m_cAward = 0;
		const auto ret = m_pUserInfo->m_stMergeActivityMap.Insert(std::make_pair(value.m_iTaskID, value));
		if (!ret.second)
		{
			return nullptr;
		}
		iter = ret.first;
	}
	return iter->second;
}

bool 
CLogicActiveProcessor::DoUserRunGetDaySingleRechargeInfo(const msgpack::unpacked& stFormatData)
{
	CLogicUserNotifySystem::NotifyDaySingleRechargeActivityInfo(m_pUserInfo);

	SendToClient(SERVER_ERRCODE::SUCCESS);
	return true;
}

bool
CLogicActiveProcessor::DoUserRunGetDaySingleRechargeReward(const msgpack::unpacked& stFormatData)
{
	CRequestGetDaySingleRechargeReward stParam;
	CResponseGetDaySingleRechargeReward stRsp;
	if (!DecodeMsgPack(stFormatData, stParam))
	{
		return (false);
	}

	const auto* pstConfig = CLogicConfig::GetDaySingleRechargeConfig().GetActivityConfig(stParam.m_iActivityID);
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_OVER);
	}

	const auto* pstTaskConfig = CLogicConfig::GetDaySingleRechargeConfig().GetActivityTaskConfig(stParam.m_iActivityID, stParam.m_iTaskID);
	if (nullptr == pstTaskConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	auto iterActivity = m_pUserInfo->m_stDaySingleRechargeMap.Find({ stParam.m_iActivityID, stParam.m_iTaskID });
	if (iterActivity == m_pUserInfo->m_stDaySingleRechargeMap.End() || iterActivity->second->GetCanRewardCount() <= 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_CONDITION_IS_NOT_REACHED);
	}

	if (iterActivity->second->GetRewardCount() >= pstTaskConfig->m_iTaskNum)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_CONDITION_IS_NOT_REACHED);
	}

	iterActivity->second->AddRewardCount(1);
	iterActivity->second->AddCanRewardCount(-1);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, pstTaskConfig->m_stBonusItemVec, m_usCmd);
	CLogicUserNotifySystem::NotifyDaySingleRechargeTaskChange(m_pUserInfo
		, stParam.m_iActivityID, stParam.m_iTaskID, iterActivity->second->GetRewardCount(), iterActivity->second->GetCanRewardCount());

	stRsp.m_iActivityID = stParam.m_iActivityID;
	stRsp.m_iTaskID = stParam.m_iTaskID;
	stRsp.m_stBonusItemVec = pstTaskConfig->m_stBonusItemVec;
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicActiveProcessor::DoUserRunGetWorldQuestInfo(const msgpack::unpacked& stFormatData)
{
    CResponseGetWorldQuestInfo stRsp;

    auto iterBegin = LOGIC_GLOBAL_CACHE_INS->m_stWorldQuestMap.Begin();
    auto iterEnd = LOGIC_GLOBAL_CACHE_INS->m_stWorldQuestMap.End();
    for(; iterBegin != iterEnd; ++iterBegin)
    {
        stRsp.m_stQuestInfo[iterBegin->first] = iterBegin->second->GetProgress();
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicActiveProcessor::DoUserRunGetAvyReward(const msgpack::unpacked& stFormatData)
{
    CRequestGetAvyReward stParam;
    CResponseGetAvyReward stRsp;
    if (!DecodeMsgPack(stFormatData, stParam))
    {
        return (false);
    }

    auto pTimeConfig = CLogicConfig::GetAvyTimeConfig().GetConfig(stParam.m_iAvyID);
    if(nullptr == pTimeConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }

    if(!CLogicActive::IsAvyOpen(pTimeConfig->m_stAvyTime, m_pUserInfo->m_stUserInfoTable.GetCreateTime()))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_OVER)
    }

    auto pAwardConfig = CLogicConfig::GetAvyScoreAwardConfig().GetAwardConfig(stParam.m_iAvyID, stParam.m_iIndex);
    if(nullptr == pAwardConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(!CLogicCacheCommon::HasEnoughItem(m_pUserInfo, pAwardConfig->m_stNeedItem))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_CONDITION_IS_NOT_REACHED)
    }

    auto iterData = m_pUserInfo->m_stUserAvyMap.Find(stParam.m_iAvyID);
    if(iterData == m_pUserInfo->m_stUserAvyMap.End())
    {
        TUserAvyTableValueType stValue;
        stValue.m_iUid = m_iUin;
        stValue.m_iGroupID = m_iGroupID;
        stValue.m_iAvyID = stParam.m_iAvyID;
        stValue.m_ulCompleteBitmap = 0;
        auto stRet = m_pUserInfo->m_stUserAvyMap.Insert(std::make_pair(stValue.m_iAvyID, stValue));
        if (stRet.second == false)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
        }

        iterData = stRet.first;
    }

    auto ulBitmap = iterData->second->GetCompleteBitmap();
    if(ulBitmap & (1ULL << stParam.m_iIndex))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::UNLIMIT_LEVEL_DAILY_PHASE_BONUS_HAS_BEING_GET)
    }

    iterData->second->SetCompleteBitmap(ulBitmap | (1ULL << stParam.m_iIndex));
    CLogicCacheCommon::AddGameItem(m_pUserInfo, pAwardConfig->m_stAwards, m_usCmd);

    stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
    stRsp.m_stAvyInfo = CLogicCacheCommon::GetUserAvyInfoPack(iterData->second->m_stData);
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}
