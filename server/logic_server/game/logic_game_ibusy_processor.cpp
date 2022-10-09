#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_ibusy_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_config.h"
#include "logic_game_ibusy_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_cache_common.h"
#include "logic_game_global.h"
#include "logic_game_active.h"
#include "logic_game_cache_card_operation.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_GET_INFO_REQ, CLogicIBusyProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_CHANGE_TASK_REQ, CLogicIBusyProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_GET_LEVEL_REWARD_REQ, CLogicIBusyProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_GET_MARKET_DATA_REQ, CLogicIBusyProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_REFRESH_MARKET_REQ, CLogicIBusyProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_PAIQIAN_REQ, CLogicIBusyProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_MARKET_BUY_REQ, CLogicIBusyProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_GET_DEPOSIT_LEVEL_AWARD, CLogicIBusyProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_GET_DEPOSIT_EXP_AWARD, CLogicIBusyProcessor)

CLogicIBusyProcessor::CLogicIBusyProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicIBusyProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	switch ((int)m_usCmd)
	{
	case MSG_LOGIC_IBUSY_GET_INFO_REQ:
	{
		return DoIBUSY_GET_INFO_REQ(stFormatData);
	}
	case MSG_LOGIC_IBUSY_CHANGE_TASK_REQ:
	{
		return DoIBUSY_CHANGE_TASK_REQ(stFormatData);
	}
	case MSG_LOGIC_IBUSY_GET_LEVEL_REWARD_REQ:
	{
		return DoIBUSY_GET_LEVEL_REWARD_REQ(stFormatData);
	}
	case MSG_LOGIC_IBUSY_GET_MARKET_DATA_REQ:
	{
		return DoIBUSY_GET_MARKET_DATA_REQ(stFormatData);
	}
	case MSG_LOGIC_IBUSY_REFRESH_MARKET_REQ:
	{
		return DoIBUSY_REFRESH_MARKET_REQ(stFormatData);
	}
	case MSG_LOGIC_IBUSY_PAIQIAN_REQ:
	{
		return DoIBUSY_PAIQIAN_REQ(stFormatData);
	}
	case MSG_LOGIC_IBUSY_MARKET_BUY_REQ:
	{
		return DoIBUSY_MARKET_BUY_REQ(stFormatData);
	}
    case MSG_LOGIC_IBUSY_GET_DEPOSIT_LEVEL_AWARD:
    {
        return DoIBUSY_GET_DEPOSIT_LEVEL_AWARD(stFormatData);
    }
    case MSG_LOGIC_IBUSY_GET_DEPOSIT_EXP_AWARD:
    {
        return DoIBUSY_GET_DEPOSIT_EXP_AWARD(stFormatData);
    }
	}
    return (true);
}

bool 
CLogicIBusyProcessor::DoIBUSY_GET_INFO_REQ(const msgpack::unpacked& stFormatData)
{
	//信息通知
	CLogicUserNotifySystem::NotifyIBusyData(m_pUserInfo);

	//返回
	SendToClient(SERVER_ERRCODE::SUCCESS);

	return (true);
}

bool CLogicIBusyProcessor::DoIBUSY_CHANGE_TASK_REQ(const msgpack::unpacked& stFormatData)
{
	CRequestIbusyChangeTask stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	int activityID = m_pUserInfo->m_stIBusyData.GetActivityID();
	auto activityIter = CLogicConfig::GetIBusyConfig().m_mpActivities.find(activityID);
	if (activityIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}
	
	int needCash = 0;
	if (stParam.m_bFree == false)
	{
		//1.计算需要花费的元宝
		if (activityIter->second.m_mpChangeCost.empty())
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
		}

		auto costIter = activityIter->second.m_mpChangeCost.find(m_pUserInfo->m_stIBusyData.GetChangeTaskCount() + 1);
		if (costIter == activityIter->second.m_mpChangeCost.end())
			costIter = --activityIter->second.m_mpChangeCost.end();

		needCash = costIter->second;

		//2.判断元宝是否足够
		if (false == CLogicCacheCommon::CanConsume(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, needCash, m_usCmd))
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_RMB);
		}
	}
	else
	{
		if (m_pUserInfo->m_stIBusyData.GetFreeChangeTaskCount() + 1 >= m_pUserInfo->m_stIBusyData.GetLevel() + 100)
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);

		m_pUserInfo->m_stIBusyData.AddFreeChangeTaskCount(1);
	}

	//3.刷新任务，消耗元宝，增加刷新次数
    CLogicActive::RefreshIBusyTaskData(m_pUserInfo);
	CLogicUserNotifySystem::NotifyIBusyTaskData(m_pUserInfo);	

	if (stParam.m_bFree == false)
	{
		CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, needCash, m_usCmd);

		m_pUserInfo->m_stIBusyData.AddChangeTaskCount(1);
		CLogicUserNotifySystem::NotifyIBusyBaseData(m_pUserInfo);
	}

	SendToClient(SERVER_ERRCODE::SUCCESS);

	return (true);
}

bool CLogicIBusyProcessor::DoIBUSY_GET_LEVEL_REWARD_REQ(const msgpack::unpacked& stFormatData)
{
	CRequestIbusyGetLevelReward stParam;
	CResponseIbusyGetLevelReward stRsp;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	//1.判断是否达成条件
	int activityID = m_pUserInfo->m_stIBusyData.GetActivityID();
	auto activityIter = CLogicConfig::GetIBusyConfig().m_mpActivities.find(activityID);
	if (activityIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}
	auto rewardIter = activityIter->second.m_mpLevelReward.find(stParam.m_iLevelID);
	if (rewardIter == activityIter->second.m_mpLevelReward.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	if (stParam.m_iLevelID > m_pUserInfo->m_stIBusyData.GetLevel())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::IBUSY_LEVEL_NOT_MATCH);
	}

	//2.判断是否已领取过
	std::set<int> gettedLevelReward = m_pUserInfo->m_stIBusyData.GetGettedLevelReward();
	if (gettedLevelReward.find(stParam.m_iLevelID) != gettedLevelReward.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
	}

	//3.领取奖励，置任务状态为已领取
	CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardIter->second.m_vecRewards, m_usCmd);
	gettedLevelReward.insert(stParam.m_iLevelID);
	m_pUserInfo->m_stIBusyData.SetGettedLevelReward(gettedLevelReward);

	CLogicUserNotifySystem::NotifyIBusyLevelRewardData(m_pUserInfo);

	//4.返回
	stRsp.m_vtAwardItem = rewardIter->second.m_vecRewards;
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicIBusyProcessor::DoIBUSY_GET_MARKET_DATA_REQ(const msgpack::unpacked& stFormatData)
{
	CLogicUserNotifySystem::NotifyIBusyMarketData(m_pUserInfo);
	SendToClient(SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicIBusyProcessor::DoIBUSY_REFRESH_MARKET_REQ(const msgpack::unpacked& stFormatData)
{
	int activityID = m_pUserInfo->m_stIBusyData.GetActivityID();
	auto activityIter = CLogicConfig::GetIBusyConfig().m_mpActivities.find(activityID);
	if (activityIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	int iNowRefresheTimes = 1;
	if (!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stIBusyData.GetMarketLastRefreshTime()))
		iNowRefresheTimes = m_pUserInfo->m_stIBusyData.GetMarketRefreshTimes() + 1;

	if (iNowRefresheTimes > (int)activityIter->second.m_stMarketData.m_stRefreshConsume.size() - 1)
		iNowRefresheTimes = (int)activityIter->second.m_stMarketData.m_stRefreshConsume.size() - 1;

	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, activityIter->second.m_stMarketData.m_stRefreshConsume[iNowRefresheTimes], m_usCmd))
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
			<< "|MSG:" << CLogicError::GetErrMsg()
			<< std::endl;

		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	m_pUserInfo->m_stIBusyData.SetMarketRefreshTimes(iNowRefresheTimes);

    CLogicActive::RefreshIBusyMarketData(m_pUserInfo);
	CLogicUserNotifySystem::NotifyIBusyMarketData(m_pUserInfo);

	SendToClient(SERVER_ERRCODE::SUCCESS);

	return (true);
}

bool CLogicIBusyProcessor::DoIBUSY_PAIQIAN_REQ(const msgpack::unpacked& stFormatData)
{
	CRequestIbusyPaiqian stParam;
	CResponseBuyIbusyPaiqian stRsp;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	int activityID = m_pUserInfo->m_stIBusyData.GetActivityID();
	auto activityIter = CLogicConfig::GetIBusyConfig().m_mpActivities.find(activityID);
	if (activityIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	//1.判断该任务是否已完成
	auto taskInfo = m_pUserInfo->m_stIBusyData.GetTaskInfo();
	auto taskIter = taskInfo.m_mpTasks.find(stParam.m_iTaskID);
	if (taskIter == taskInfo.m_mpTasks.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
	}
	if (taskIter->second.m_bFinished == true)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::IBUSY_TASK_FINISHED);
	}

	//2.判断条件是否满足
	//2.1配置
	auto eventIter = activityIter->second.m_vecEvents.begin();
	for (; eventIter != activityIter->second.m_vecEvents.end(); ++eventIter)
	{
		if (eventIter->m_iId == taskIter->second.m_iTaskID)
			break;
	}

	auto pinzhiIter = activityIter->second.m_vecPinzhis.begin();
	for (; pinzhiIter != activityIter->second.m_vecPinzhis.end(); ++pinzhiIter)
	{
		if (pinzhiIter->m_iId == taskIter->second.m_iStar)
			break;
	}

	if ((eventIter == activityIter->second.m_vecEvents.end()) || \
		(pinzhiIter == activityIter->second.m_vecPinzhis.end()))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	//2.2伙伴满足需求
	CLogicCacheCardOperation stOperation(m_pUserInfo);
	if (stParam.m_iTeamMember1 > 0 && !stOperation.CheckHeroCardExist(stParam.m_iTeamMember1))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	if (stParam.m_iTeamMember2 > 0 && !stOperation.CheckHeroCardExist(stParam.m_iTeamMember2))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	if (stParam.m_iTeamMember3 > 0 && !stOperation.CheckHeroCardExist(stParam.m_iTeamMember3))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	if (stParam.m_iTeamMember4 > 0 && !stOperation.CheckHeroCardExist(stParam.m_iTeamMember4))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	if (stParam.m_iTeamMember5 > 0 && !stOperation.CheckHeroCardExist(stParam.m_iTeamMember5))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	std::map<int, THeroCardInfoTable*> heros;
	auto heroIter1 = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iTeamMember1);
	if (heroIter1 != m_pUserInfo->m_stHeroCardTableMap.End())
	{
		heros[heroIter1->first] = heroIter1->second;
	}
	auto heroIter2 = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iTeamMember2);
	if (heroIter2 != m_pUserInfo->m_stHeroCardTableMap.End())
	{
		heros[heroIter2->first] = heroIter2->second;
	}
	auto heroIter3 = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iTeamMember3);
	if (heroIter3 != m_pUserInfo->m_stHeroCardTableMap.End())
	{
		heros[heroIter3->first] = heroIter3->second;
	}
	auto heroIter4 = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iTeamMember4);
	if (heroIter4 != m_pUserInfo->m_stHeroCardTableMap.End())
	{
		heros[heroIter4->first] = heroIter4->second;
	}
	auto heroIter5 = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iTeamMember5);
	if (heroIter5 != m_pUserInfo->m_stHeroCardTableMap.End())
	{
		heros[heroIter5->first] = heroIter5->second;
	}
	
	std::set<int> matachedNeeds;
	std::vector<int> matchedCharators;
	for (auto &heroIter : heros)
	{		
		auto pHeroConf = CLogicConfig::GetHeroCardConfig().GetConfig(heroIter.first);
		if (pHeroConf)
		{
			matachedNeeds.insert(pHeroConf->m_iCardType);
			if (pHeroConf->m_iSexual == 1)
				matachedNeeds.insert(4);
			else if (pHeroConf->m_iSexual == 2)
				matachedNeeds.insert(5);

            /*
            for (auto &charactorIter : eventIter->m_vecCharactors)
            {
                if (heroIter.second->GetCharacter1() == charactorIter)
                    matchedCharators.push_back(heroIter.second->GetCharacterColor1());
                if (heroIter.second->GetCharacter2() == charactorIter)
                    matchedCharators.push_back(heroIter.second->GetCharacterColor2());
                if (heroIter.second->GetCharacter3() == charactorIter)
                    matchedCharators.push_back(heroIter.second->GetCharacterColor3());
                if (heroIter.second->GetCharacter4() == charactorIter)
                    matchedCharators.push_back(heroIter.second->GetCharacterColor4());
                if (heroIter.second->GetCharacter5() == charactorIter)
                    matchedCharators.push_back(heroIter.second->GetCharacterColor5());
			}
            */
		}
	}	

	for (auto &needIter : eventIter->m_vecNeeds)
	{
		if (matachedNeeds.find(needIter) == matachedNeeds.end())
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::IBUSY_TASK_NEED_NOT_MATCH);
		}
	}

	//3.活跃点是否足够
	// int huoyue = m_pUserInfo->m_stIBusyData.GetHuoyue();
	// if (huoyue < pinzhiIter->m_iHuoyue)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::IBUSY_TASK_HUOYUE_NOT_ENOUGH);
	}

	//4.成功，计算并发放奖励，标志该任务为已完成,消耗活跃点
	stRsp.m_iExp = pinzhiIter->m_iExp;
	stRsp.m_iScore = pinzhiIter->m_iScore;
	stRsp.m_stReward = pinzhiIter->m_stReward;
	
	int percent = heros.size() * 100 / eventIter->m_iHeroNum;
	for (auto &charactorIter : matchedCharators)
	{
		auto findIter = activityIter->second.m_mpCharactorAdd.find(charactorIter);
		if (findIter != activityIter->second.m_mpCharactorAdd.end())
			percent += findIter->second;
	}
	{
		auto findIter = activityIter->second.m_mpLevelAdd.find(m_pUserInfo->m_stIBusyData.GetLevel());
		if (findIter != activityIter->second.m_mpLevelAdd.end())
			percent += findIter->second;
	}

	stRsp.m_iScore = stRsp.m_iScore * percent / 100;
	stRsp.m_stReward.m_iNum = stRsp.m_stReward.m_iNum * percent / 100;

	m_pUserInfo->m_stIBusyData.AddScore(stRsp.m_iScore);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stReward, m_usCmd);
    CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_IbusyExpID, stRsp.m_iExp, m_usCmd);

	taskIter->second.m_bFinished = true;
	m_pUserInfo->m_stIBusyData.SetTaskInfo(taskInfo);

	// huoyue -= pinzhiIter->m_iHuoyue;
	// m_pUserInfo->m_stIBusyData.SetHuoyue(huoyue);

	CLogicUserNotifySystem::NotifyIBusyBaseData(m_pUserInfo);
	CLogicUserNotifySystem::NotifyIBusyTaskData(m_pUserInfo);

	//积分排行榜更新
	CLogicGlobal::UpdateOrInsertIbusyRank(m_iUin, m_iGroupID, m_pUserInfo->m_stIBusyData.GetScore());

	//5.返回
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicIBusyProcessor::DoIBUSY_MARKET_BUY_REQ(const msgpack::unpacked& stFormatData)
{
	CResponseBuyIbusyMarketCommodity stRsp;
	CRequestBuyIbusyMarketCommodity stParam;

	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	auto stIter = m_pUserInfo->m_stIBusyMarket.Find(stParam.m_iCommodityID);
	if (stIter == m_pUserInfo->m_stIBusyMarket.End())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << UNKNOW_COMMODITY_ID << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
			<< "|COMMODITY_ID:" << stParam.m_iCommodityID << std::endl;

		SEND_ERRCODE_AND_RET(UNKNOW_COMMODITY_ID)
	}

	if (stIter->second->GetLastBuyTime() > m_pUserInfo->m_stIBusyData.GetMarketLastRefreshTime())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_BUY_ITEM_TIMES << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_BUY_ITEM_TIMES)
			<< "|COMMODITY_ID:" << stParam.m_iCommodityID << std::endl;

		SEND_ERRCODE_AND_RET(NOT_ENOUGH_BUY_ITEM_TIMES)
	}

	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,
                                            (CLogicConfigDefine::GAME_ITEM_COLLECTION) stIter->second->GetConsumeItemType(),
                                            stIter->second->GetConsumeItemID(),
                                            stIter->second->GetConsumeItemNum(), m_usCmd))
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode() << "|MSG:" << CLogicError::GetErrMsg()
			<< "|COMMODITY_ID:" << stParam.m_iCommodityID << std::endl;

		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	static std::string strBonus;

	CPackGameItem stBonusItem;

	stBonusItem.m_iItemType = stIter->second->GetCommodityItemType();
	stBonusItem.m_iCardID = stIter->second->GetCommodityItemID();
	stBonusItem.m_iNum = stIter->second->GetCommodityItemNum();

	stRsp.m_iCommodityID = stParam.m_iCommodityID;
	stRsp.m_stCommodity = stBonusItem;
	stRsp.m_stConsume.m_iItemType = (CLogicConfigDefine::GAME_ITEM_COLLECTION)stIter->second->GetConsumeItemType();
	stRsp.m_stConsume.m_iCardID = stIter->second->GetConsumeItemID();
	stRsp.m_stConsume.m_iNum = stIter->second->GetConsumeItemNum();

	CLogicCacheCommon::AddGameItem(m_pUserInfo, stBonusItem, strBonus, m_usCmd);

	stIter->second->SetLastBuyTime(CLogicCommon::GetSec());
	CLogicUserNotifySystem::NotifyIBusyMarketData(m_pUserInfo);
	
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

	return true;
}

bool CLogicIBusyProcessor::DoIBUSY_GET_DEPOSIT_LEVEL_AWARD(const msgpack::unpacked& stFormatData)
{
    CRequestIbusyGetDepositLevelAward stParam;
    CResponseIbusyGetDepositLevelAward stRsp;
    if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    //1.判断是否达成条件
    int activityID = m_pUserInfo->m_stIBusyData.GetActivityID();
    auto activityIter = CLogicConfig::GetIBusyConfig().m_mpActivities.find(activityID);
    if (activityIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }
    auto rewardIter = activityIter->second.m_mpLevelReward.find(stParam.m_iLevelID);
    if (rewardIter == activityIter->second.m_mpLevelReward.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    if (stParam.m_iLevelID > m_pUserInfo->m_stIBusyData.GetLevel())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::IBUSY_LEVEL_NOT_MATCH);
    }

    const auto iterDeposit = m_pUserInfo->m_stUserDepositMap.Find(activityIter->second.m_iDepositID);
    if (iterDeposit == m_pUserInfo->m_stUserDepositMap.End() || iterDeposit->second->GetBuyTimes() <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_CONDITION_IS_NOT_REACHED);
    }

    //2.判断是否已领取过
    std::set<int> gettedLevelReward = m_pUserInfo->m_stIBusyData.GetGettedDepositReward();
    if (gettedLevelReward.find(stParam.m_iLevelID) != gettedLevelReward.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
    }

    //3.领取奖励，置任务状态为已领取
    CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardIter->second.m_stDepositRewards, m_usCmd);
    gettedLevelReward.insert(stParam.m_iLevelID);
    m_pUserInfo->m_stIBusyData.SetGettedDepositReward(gettedLevelReward);

    CLogicUserNotifySystem::NotifyIBusyLevelRewardData(m_pUserInfo);

    //4.返回
    stRsp.m_vtAwardItem = rewardIter->second.m_stDepositRewards;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return (true);
}

bool CLogicIBusyProcessor::DoIBUSY_GET_DEPOSIT_EXP_AWARD(const msgpack::unpacked& stFormatData)
{
    CResponseIbusyGetDepositExpAward stRsp;

    //1.判断是否达成条件
    int activityID = m_pUserInfo->m_stIBusyData.GetActivityID();
    auto activityIter = CLogicConfig::GetIBusyConfig().m_mpActivities.find(activityID);
    if (activityIter == CLogicConfig::GetIBusyConfig().m_mpActivities.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    const auto iterDeposit = m_pUserInfo->m_stUserDepositMap.Find(activityIter->second.m_iDepositID);
    if (iterDeposit == m_pUserInfo->m_stUserDepositMap.End() || iterDeposit->second->GetBuyTimes() <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_CONDITION_IS_NOT_REACHED);
    }

    //2.判断是否已领取过
    if (m_pUserInfo->m_stIBusyData.GetDailyExpAward())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
    }

    //3.领取奖励，置任务状态为已领取
    stRsp.m_iAddExp = activityIter->second.m_iDepositExp;
    m_pUserInfo->m_stIBusyData.SetDailyExpAward(1);
    CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_IbusyExpID, stRsp.m_iAddExp, m_usCmd);

    //4.返回
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return (true);
}

