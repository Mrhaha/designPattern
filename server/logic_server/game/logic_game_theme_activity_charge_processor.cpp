#include "common_datetime.h"
#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_theme_activity_charge_protocol.h"
#include "logic_link_common.h"
#include "logic_game_theme_activity_charge_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_user_notify_system.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_THEME_ACTIVITY_CHARGE_GET_INFO_REQ, CLogicThemeActivityChargeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_THEME_ACTIVITY_CHARGE_GET_REWARD_REQ, CLogicThemeActivityChargeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_THEME_ACTIVITY_CHARGE_BUY_REQ, CLogicThemeActivityChargeProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_THEME_ACTIVITY_CHARGE_CHARGE_REQ, CLogicThemeActivityChargeProcessor)

CLogicThemeActivityChargeProcessor::CLogicThemeActivityChargeProcessor(uint16_t usCmd,
																		   const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool
CLogicThemeActivityChargeProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	switch ((int)m_usCmd)
	{
	case MSG_LOGIC_THEME_ACTIVITY_CHARGE_GET_INFO_REQ:
	{
		return DoTHEME_ACTIVITY_CHARGE_GET_INFO_REQ(stFormatData);
	}
	case MSG_LOGIC_THEME_ACTIVITY_CHARGE_GET_REWARD_REQ:
	{
		return DoTHEME_ACTIVITY_CHARGE_GET_REWARD_REQ(stFormatData);
	}
	case MSG_LOGIC_THEME_ACTIVITY_CHARGE_BUY_REQ:
	{
		return DoTHEME_ACTIVITY_CHARGE_BUY_REQ(stFormatData);
	}
	case MSG_LOGIC_THEME_ACTIVITY_CHARGE_CHARGE_REQ:
	{
		return DoTHEME_ACTIVITY_CHARGE_CHARGE_REQ(stFormatData);
	}
	}
    return (true);
}

bool 
CLogicThemeActivityChargeProcessor::DoTHEME_ACTIVITY_CHARGE_GET_INFO_REQ(const msgpack::unpacked& stFormatData)
{
	//1.解析请求协议
	CRequestGetChargeActivityInfo stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	//2.信息通知
	CLogicUserNotifySystem::NotifyThemeActivityChargeData(m_pUserInfo, stParam.m_iActivityID);

	//3.返回
	SendToClient(SERVER_ERRCODE::SUCCESS);

	return true;
}

bool
CLogicThemeActivityChargeProcessor::DoTHEME_ACTIVITY_CHARGE_GET_REWARD_REQ(const msgpack::unpacked& stFormatData)
{
	//1.解析请求协议
	CRequestChargeActivityGetReward stParam;
	CResponseChargeActivityGetReward stRsp;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	//2.找到对应的任务
	auto pActivity = m_pUserInfo->m_stThemeActivityChargeMap.find(stParam.m_iActivityID);
	if (pActivity == m_pUserInfo->m_stThemeActivityChargeMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::THEME_ACTIVITY_CHARGE_TASK_NOT_FOUND);
	}
	auto pTask = pActivity->second.Find(stParam.m_iTaskID);
	if (pTask == pActivity->second.End())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::THEME_ACTIVITY_CHARGE_TASK_NOT_FOUND);
	}

	//3.如果任务状态为不可领取或者已领取，返回错误
	if (pTask->second->GetState() == charge_task_get_rewad)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_BONUS_IS_RECEIVED);
	}
	else if (pTask->second->GetState() == charge_task_cannot_rewad)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_CONDITION_IS_NOT_REACHED);
	}

	//3.找到配置
	auto pConf = CLogicConfig::GetThemeActivityChargeConfig().m_stAllActivities.find(stParam.m_iActivityID);
	if (pConf == CLogicConfig::GetThemeActivityChargeConfig().m_stAllActivities.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	int dayBeginTime = get_day_begin_time(CLogicCommon::GetSec());
	auto pDayConf = pConf->second.m_stDayActivities.find(dayBeginTime);
	if (pDayConf == pConf->second.m_stDayActivities.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	auto pTaskConf = pDayConf->second.m_stTask.find(stParam.m_iTaskID);
	if (pTaskConf == pDayConf->second.m_stTask.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	//4.领取奖励，置任务状态为已领取
	CLogicCacheCommon::AddGameItem(m_pUserInfo, pTaskConf->second.m_aReward, m_usCmd);
	pTask->second->SetState(charge_task_get_rewad);

	//5.任务信息通知
	CPackChargeActivityTask taskElem;
	taskElem.m_iID = pTask->first;
	taskElem.m_iTaskType = pTask->second->GetTaskType();
	taskElem.m_iTarget = pTask->second->GetTargetAmount();
	taskElem.m_iProgress = pTask->second->GetProgress();
	taskElem.m_cState = pTask->second->GetState();
	CLogicUserNotifySystem::NotifyThemeActivityTaskChange(m_pUserInfo, stParam.m_iActivityID, taskElem);

	//6.返回
	stRsp.m_vtAwardItem = pTaskConf->second.m_aReward;
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

	return true;
}

bool
CLogicThemeActivityChargeProcessor::DoTHEME_ACTIVITY_CHARGE_BUY_REQ(const msgpack::unpacked& stFormatData)
{
	//1.解析请求协议
	CRequestChargeActivityBuy stParam;
	CResponseChargeActivityBuy stRsp;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	//2.找到对应的任务
	auto pActivity = m_pUserInfo->m_stThemeActivityChargeMap.find(stParam.m_iActivityID);
	if (pActivity == m_pUserInfo->m_stThemeActivityChargeMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::THEME_ACTIVITY_CHARGE_TASK_NOT_FOUND);
	}
	auto pTask = pActivity->second.Find(stParam.m_iTaskID);
	if (pTask == pActivity->second.End())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::THEME_ACTIVITY_CHARGE_TASK_NOT_FOUND);
	}

	//3.如果任务状态为已领取，返回错误
	//对于购买任务，只有不可领取和已领取两种状态，默认为不可领取，购买次数用完，则状态变为已领取
	if (pTask->second->GetState() == charge_task_get_rewad)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_BONUS_IS_RECEIVED);
	}

	//3.找到配置
	auto pConf = CLogicConfig::GetThemeActivityChargeConfig().m_stAllActivities.find(stParam.m_iActivityID);
	if (pConf == CLogicConfig::GetThemeActivityChargeConfig().m_stAllActivities.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	int dayBeginTime = get_day_begin_time(CLogicCommon::GetSec());
	auto pDayConf = pConf->second.m_stDayActivities.find(dayBeginTime);
	if (pDayConf == pConf->second.m_stDayActivities.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	auto pTaskConf = pDayConf->second.m_stBuyItem.find(stParam.m_iTaskID);
	if (pTaskConf == pDayConf->second.m_stBuyItem.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	//4.判断原材料是否足够
	for (auto &confIter : pTaskConf->second.m_aConsume)
	{
		if (false == CLogicCacheCommon::CanConsume(m_pUserInfo, confIter.m_iItemType, confIter.m_iCardID, confIter.m_iNum, m_usCmd))
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_RMB);
		}
	}

	//5.购买成功，消耗原材料，获取奖励；任务进度加一，如果达到最大次数，则置任务状态为已领取
	CLogicCacheCommon::AddGameItem(m_pUserInfo, pTaskConf->second.m_aReward, m_usCmd);
	CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pTaskConf->second.m_aConsume, m_usCmd);

	pTask->second->AddProgress(1);
	if (pTask->second->GetProgress() >= pTask->second->GetTargetAmount())
	{
		pTask->second->SetState(charge_task_get_rewad);
	}

	//6.任务信息通知
	CPackChargeActivityTask taskElem;
	taskElem.m_iID = pTask->first;
	taskElem.m_iTaskType = pTask->second->GetTaskType();
	taskElem.m_iTarget = pTask->second->GetTargetAmount();
	taskElem.m_iProgress = pTask->second->GetProgress();
	taskElem.m_cState = pTask->second->GetState();
	CLogicUserNotifySystem::NotifyThemeActivityTaskChange(m_pUserInfo, stParam.m_iActivityID, taskElem);

	//6.返回
	stRsp.m_vtAwardItem = pTaskConf->second.m_aReward;
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

	return true;
}

bool
CLogicThemeActivityChargeProcessor::DoTHEME_ACTIVITY_CHARGE_CHARGE_REQ(const msgpack::unpacked& stFormatData)
{
	//1.解析请求协议
	CRequestChargeActivityCharge stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	//2.找到对应的任务
	auto pActivity = m_pUserInfo->m_stThemeActivityChargeMap.find(stParam.m_iActivityID);
	if (pActivity == m_pUserInfo->m_stThemeActivityChargeMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::THEME_ACTIVITY_CHARGE_TASK_NOT_FOUND);
	}
	auto pTask = pActivity->second.Find(stParam.m_iTaskID);
	if (pTask == pActivity->second.End())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::THEME_ACTIVITY_CHARGE_TASK_NOT_FOUND);
	}

	//3.如果任务状态为已领取，返回错误
	//对于充值任务，只有不可领取和已领取两种状态，默认为不可领取，充值次数用完，则状态变为已领取
	if (pTask->second->GetState() == charge_task_get_rewad)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_BUY_RESOURCE_TIMES);
	}

	//3.找到配置
	auto pConf = CLogicConfig::GetThemeActivityChargeConfig().m_stAllActivities.find(stParam.m_iActivityID);
	if (pConf == CLogicConfig::GetThemeActivityChargeConfig().m_stAllActivities.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	int dayBeginTime = get_day_begin_time(CLogicCommon::GetSec());
	auto pDayConf = pConf->second.m_stDayActivities.find(dayBeginTime);
	if (pDayConf == pConf->second.m_stDayActivities.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	auto pTaskConf = pDayConf->second.m_stChargeItem.find(stParam.m_iTaskID);
	if (pTaskConf == pDayConf->second.m_stChargeItem.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	//6.返回
	SendToClient(SERVER_ERRCODE::SUCCESS);

	return true;
}


