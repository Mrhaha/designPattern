#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "logic_config.h"
#include "logic_game_item.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_cross_fight_processor.h"
#include "logic_game_cache_level_operation.h"
#include "logic_game_global.h"
#include "logic_game_cache_common.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_currency_check.h"
#include "logic_game_publisher.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_item_factory.h"
#include "logic_game_bill.h"
#include "logic_game_common_publisher.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_CROSS_FIGHT_RESULT, CLogicCrossFightProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_CROSS_FIGHT_CHALLENGER, CLogicCrossFightProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_RESET_CROSS_FIGHT_CHALLENGER, CLogicCrossFightProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CROSS_FIGHT_AUTO_FIGHT, CLogicCrossFightProcessor)

REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_CLIMB_TOWER_RESULT, CLogicClimbTowerProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_RESET_CLIMB_TOWER_ORDER, CLogicClimbTowerProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_CLIMB_TOWER_START, CLogicClimbTowerProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SWEEP_CLIMB_TOWER_START, CLogicClimbTowerProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SWEEP_CLIMB_TOWER_AWARD, CLogicClimbTowerProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SWEEP_CLIMB_TOWER_YUANBAO, CLogicClimbTowerProcessor)

CLogicCrossFightProcessor::CLogicCrossFightProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicCrossFightProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if ((int)m_usCmd == MSG_LOGIC_GET_CROSS_FIGHT_RESULT)
    {
        CResponseGetCrossFightResult stRsp;
        CRequestGetCrossFightResult stParam;

        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        const auto iterConfig = CLogicConfig::GetCrossFightConfig().m_stConfig.m_stBonusMap.find(stParam.m_iTeamID);
        if(iterConfig == CLogicConfig::GetCrossFightConfig().m_stConfig.m_stBonusMap.end())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
        }

        auto stIter = m_pUserInfo->m_stCrossFightInfoMap.Find(stParam.m_iTeamID);
        if (stIter == m_pUserInfo->m_stCrossFightInfoMap.End() || stIter->second->GetPassed() != CROSS_FIGHT_LEVEL_CAN_FIGHT)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CROSS_FIGHT_LEVEL_HAS_PASSED
                                << "|MSG:" <<  GET_ERRMSG_BY_ERRCODE(CROSS_FIGHT_LEVEL_HAS_PASSED) 
                                << "|TEAM_ID:" << stParam.m_iTeamID << std::endl;

            SEND_ERRCODE_AND_RET(CROSS_FIGHT_LEVEL_HAS_PASSED)
        }

		if (stParam.m_iTeamID > 1)
		{
			const auto iterPre = m_pUserInfo->m_stCrossFightInfoMap.Find(stParam.m_iTeamID - 1);
			if (iterPre == m_pUserInfo->m_stCrossFightInfoMap.End() || iterPre->second->GetPassed() == CROSS_FIGHT_LEVEL_CAN_FIGHT)
			{
				using namespace SERVER_ERRCODE;
				PROCESSOR_LOG_ERROR << "ERRCODE:" << CROSS_FIGHT_PRE_NOT_PASSED
					<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(CROSS_FIGHT_PRE_NOT_PASSED)
					<< "|TEAM_ID:" << stParam.m_iTeamID << std::endl;

				SEND_ERRCODE_AND_RET(CROSS_FIGHT_PRE_NOT_PASSED)
			}
		}

        if(iterConfig->second.m_iType == CLogicConfigDefine::ECFLT_PVP)
        {
            auto player = stIter->second->GetPlayer();
            size_t indexCost = 0, indexCard = 0;
            while (indexCost < stParam.m_stCostHP.size() && indexCard < player.m_stCardInfoList.size())
            {
                /*if (player.m_stCardInfoList[indexCard].m_iCrossFightCostHP < 100)
                {
                    player.m_stCardInfoList[indexCard].m_iCrossFightCostHP = stParam.m_stCostHP[indexCost];
                    ++indexCost;
                }*/
                ++indexCard;
            }

            stIter->second->SetPlayer(player);

            int iCardID, iCostHP;
            for (size_t i = 0; i < 5 && i < stParam.m_stUserCostHP.size(); ++i )
            {
                iCardID = stParam.m_stUserCostHP[i].first;
                iCostHP = stParam.m_stUserCostHP[i].second;
                auto stCardIter = m_pUserInfo->m_stHeroCardTableMap.Find(iCardID);
                if (stCardIter != m_pUserInfo->m_stHeroCardTableMap.End())
                    stCardIter->second->SetCrossFightCostHp(iCostHP);
            }
        }

        // 奖励
        static std::string strBonus;
        if((iterConfig->second.m_iType == CLogicConfigDefine::ECFLT_PVP && stParam.m_iResult) || iterConfig->second.m_iType == CLogicConfigDefine::ECFLT_Box)
        {
            bool bFirst = (stIter->second->GetFirstPass() == 0) ? true : false;
            CLogicConfig::GetCrossFightConfig().GetFightBonus(stParam.m_iTeamID, bFirst, false, stRsp.m_stRewards);

            CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stRewards, strBonus, m_usCmd);

            if (bFirst) stIter->second->SetFirstPass(1);
            stIter->second->SetPassed(CROSS_FIGHT_LEVEL_RECEIVED);

            CLogicGlobal::PublishCrossFightBroadcast(m_pUserInfo->m_strNick, stRsp.m_stRewards);

            // 任务
            CLogicGameUserActionChannel::data_type stChannelData;
            stChannelData.m_iTimes = 1;
            stChannelData.m_iAction = CLogicGameChannelEnum::ACTION_CROSS_FIGHT;
            LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stChannelData);
        }

        stRsp.m_iTeamID = stParam.m_iTeamID;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|TEAM_ID:" << stParam.m_iTeamID << "|BONUS_ITEM:" << strBonus << std::endl;

        return true;
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_CROSS_FIGHT_CHALLENGER)
    {
        CResponseGetCrossFightChallenger stRsp;

        if(m_pUserInfo->m_stCrossFightInfoMap.Empty())
        {
            stRsp.m_iCurProgress = 0;
            stRsp.m_stChallengerList = ResetChallengerInfo();
        }
        else
        {
            for(auto iter = m_pUserInfo->m_stCrossFightInfoMap.Begin(); iter != m_pUserInfo->m_stCrossFightInfoMap.End(); ++iter)
            {
                if(iter->second->GetPassed())
                {
                    stRsp.m_iCurProgress = std::max(stRsp.m_iCurProgress, iter->first);
                }
                else if(iter->second->GetPlayer().m_stBriefInfo.m_iUid > 0)
                {
                    stRsp.m_stChallengerList.emplace(iter->first, iter->second->GetPlayer());
                }
            }
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return true;
    }
    else if ((int)m_usCmd == MSG_LOGIC_RESET_CROSS_FIGHT_CHALLENGER)
    {
        int iResetTime = m_pUserInfo->m_stUserInfoTable.GetCrossFightResetTime();
        int iResetTimes = m_pUserInfo->m_stUserInfoTable.GetCrossFightResetTimes();

        if (LOGIC_CHECK_DAY_REFRESH(iResetTime))
            iResetTimes = 0;

        if (iResetTimes >= CLogicConfig::GetCrossFightConfig().m_stConfig.m_iPlayTimesPerDay)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CROSS_FIGHT_NOT_ENOUGH_RESET_TIMES
                                << "|MSG:" <<  GET_ERRMSG_BY_ERRCODE(CROSS_FIGHT_NOT_ENOUGH_RESET_TIMES) 
                                << "|RESET_TIMES:" << iResetTimes << std::endl;

            SEND_ERRCODE_AND_RET(CROSS_FIGHT_NOT_ENOUGH_RESET_TIMES)
        }

        m_pUserInfo->m_stUserInfoTable.SetCrossFightResetTime(CLogicCommon::GetSec());
        m_pUserInfo->m_stUserInfoTable.SetCrossFightResetTimes(iResetTimes + 1);

        for (auto iter = m_pUserInfo->m_stHeroCardTableMap.Begin(); iter != m_pUserInfo->m_stHeroCardTableMap.End(); ++iter)
        {
            iter->second->SetCrossFightCostHp(0);
        }

        CResponseGetCrossFightChallenger stRsp;
        stRsp.m_iCurProgress = 0;
		stRsp.m_stChallengerList = ResetChallengerInfo();

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

        return true;
    }
	else if (m_usCmd == MSG_LOGIC_CROSS_FIGHT_AUTO_FIGHT)
	{
		CResponseCrossFightAutoFight stRspBody;

		if (m_pUserInfo->m_stUserInfoTable.GetLevel() < CLogicConfig::GetCrossFightConfig().m_stConfig.m_iAutoFightLevel)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL);
		}

        int32_t iTimes = 0;
		for (int i = 1 ; i <= CLogicConfig::GetCrossFightConfig().m_stConfig.m_iAutoFightTeamCount; ++i)
		{
			auto iter = m_pUserInfo->m_stCrossFightInfoMap.Find(i);
			if (iter != m_pUserInfo->m_stCrossFightInfoMap.End() && iter->second->GetPassed() != CROSS_FIGHT_LEVEL_RECEIVED)
			{
				CLogicConfig::GetCrossFightConfig().GetFightBonus(i, false, false, stRspBody.m_stBonusItemVec);

				iter->second->SetPassed(CROSS_FIGHT_LEVEL_RECEIVED);
                ++iTimes;
			}
		}

        if(iTimes <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

		static std::string strBonus;
		CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusItemVec, strBonus, m_usCmd);

		CLogicGlobal::PublishCrossFightBroadcast(m_pUserInfo->m_strNick, stRspBody.m_stBonusItemVec);

        stRspBody.m_iCurProgress = CLogicConfig::GetCrossFightConfig().m_stConfig.m_iAutoFightTeamCount;
		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|BONUS_ITEM:" << strBonus << std::endl;

        // 事件
        CLogicGameUserActionChannel::data_type stChannelData;
        stChannelData.m_iTimes = iTimes;
        stChannelData.m_iAction = CLogicGameChannelEnum::ACTION_CROSS_FIGHT;
        LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stChannelData);
	}

    return true;
}

std::map<int32_t, CPackPvpChallengerInfo> CLogicCrossFightProcessor::ResetChallengerInfo()
{
	user_info_key_t stKey;
	stKey.m_stRoleIDDetail.m_iUin = m_pUserInfo->m_iUin;
	stKey.m_stRoleIDDetail.m_iGroupID = m_pUserInfo->m_iGroupID;

	int iRank = (int32_t)LOGIC_PVP_RANK_INS.GetRank(stKey.m_ulRoleID);
	if (iRank == 0) iRank = CROSS_FIGHT_DEFAULT_RANK;

	std::vector<int> stRank = CLogicConfig::GetCrossFightConfig().GetMatchRank(iRank);

    std::map<int32_t, CPackPvpChallengerInfo> players;
    size_t index = 0;
    for(const auto& iterConfig : CLogicConfig::GetCrossFightConfig().m_stConfig.m_stBonusMap)
    {
        auto iterFight = m_pUserInfo->m_stCrossFightInfoMap.Find(iterConfig.first);
        if (iterFight == m_pUserInfo->m_stCrossFightInfoMap.End())
        {
            TCrossFightInfoTableValueType value;
            value.m_iUid = m_pUserInfo->m_iUin;
            value.m_iGroupID = m_pUserInfo->m_iGroupID;
            value.m_iTeamID = iterConfig.first;
            value.m_iPassed = 0;
            value.m_iFirstPass = 0;
            value.m_stPlayer = {};

            const auto stRet = m_pUserInfo->m_stCrossFightInfoMap.Insert(std::make_pair(value.m_iTeamID, value));
            if (!stRet.second)
            {
                LOGIC_LOG_TRACE_ERROR << "CANNOT INSERT CROSS FIGHT:|UIN:" << m_pUserInfo->m_iUin
                                      << "|GROUP:" << m_pUserInfo->m_iGroupID << "|TEAM:" << iterConfig.first << std::endl;
                continue;
            }
            iterFight = stRet.first;
        }

        iterFight->second->SetPassed(0);
        iterFight->second->SetPlayer({});

        if(iterConfig.second.m_iType == CLogicConfigDefine::ECFLT_PVP && index < stRank.size())
        {
            iterFight->second->SetPlayer(CLogicCacheCommon::GetPvpChallengerInfo(stRank[index++]));

            if (iterFight->second->GetPlayer().m_stBriefInfo.m_iUid <= 0)
            {
                LOGIC_LOG_TRACE_ERROR << "CANNOT FOUND RANK:" << stRank[index - 1] << " IN PVP RANK" << std::endl;
            }

            players.emplace(iterConfig.first, iterFight->second->GetPlayer());
        }
    }

	return players;
}

CLogicClimbTowerProcessor::CLogicClimbTowerProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicClimbTowerProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{	
	if (m_pUserInfo->m_stUserInfoTable.GetClimbTowerSweepEndTime() > 0)
	{//在扫荡中,请先领奖
		if ((int)m_usCmd != MSG_LOGIC_SWEEP_CLIMB_TOWER_AWARD
			&& (int)m_usCmd != MSG_LOGIC_SWEEP_CLIMB_TOWER_YUANBAO)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLIMB_TOWER_IS_SWEEPING
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(CLIMB_TOWER_IS_SWEEPING)
				<< "|CMD:" << m_usCmd << std::endl;
			SEND_ERRCODE_AND_RET(CLIMB_TOWER_IS_SWEEPING)
		}
	}

	if ((int)m_usCmd == MSG_LOGIC_FIGHT_CLIMB_TOWER_RESULT)
	{
		CRequestFightClimbTowerResult stParam;
		CResponseFightClimbTowerResult stRsp;

		if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

		CLogicCacheCommon::AutoAddPhyPower(m_pUserInfo);

		if (m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder() + 1 != stParam.m_iOrder)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << INVALID_CLIMB_TOWER_ORDER
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(INVALID_CLIMB_TOWER_ORDER)
				<< "|TEAM_ID:" << stParam.m_iOrder << std::endl;

			SEND_ERRCODE_AND_RET(INVALID_CLIMB_TOWER_ORDER)
		}

		auto pstTowerConfig = CLogicConfig::GetClimbTowerConfig().GetConfigElem(stParam.m_iOrder);
		if (nullptr == pstTowerConfig)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|TEAM_ID:" << stParam.m_iOrder << std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		auto pstLevelConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(pstTowerConfig->m_iMapID);
		if (nullptr == pstLevelConfig)
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrCode()
				<< "|LEVEL_ID:" << pstTowerConfig->m_iMapID << std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		if (m_pUserInfo->m_stUserInfoTable.GetRoleCombat() < pstLevelConfig->m_iPassFightPower)
		{
			CLogicBill::SaveAntiCheatingBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, pstTowerConfig->m_iMapID
					, m_pUserInfo->m_stUserInfoTable.GetRoleCombat(), pstLevelConfig->m_iPassFightPower, m_pUserInfo->m_stUserTeamInfoTable.GetTeam1(), "",1);
		}

		CLogicCacheLevelOperation stOperation(m_pUserInfo);
		if (!stOperation.CheckFightLevelID(pstTowerConfig->m_iMapID))
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|LEVEL_ID:" << pstTowerConfig->m_iMapID << std::endl;
			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		if (!stOperation.CheckRspLevelDrop(stParam.m_stBonusList))
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|LEVEL_ID:" << pstTowerConfig->m_iMapID << std::endl;

			stParam.m_stBonusList = m_pUserInfo->m_stCacheFightLevelDropBonus;
			// BAD_CODE
			// SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		if (m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder() < stParam.m_iOrder)
		{
            stParam.m_stBonusList.insert(stParam.m_stBonusList.end(), pstLevelConfig->m_stFirstBonusVec.begin(), pstLevelConfig->m_stFirstBonusVec.end());
            stRsp.m_stFirstBonusList.insert(stRsp.m_stFirstBonusList.end(), pstLevelConfig->m_stFirstBonusVec.begin(), pstLevelConfig->m_stFirstBonusVec.end());
		}

		CLogicConfig::GetLevelDropConfig().GetRandBonusByName(
			pstLevelConfig->m_strPerfectCardBag,
			stRsp.m_stPerfectBonusList);
		stParam.m_stBonusList.insert(stParam.m_stBonusList.end(), stRsp.m_stPerfectBonusList.begin(), stRsp.m_stPerfectBonusList.end());

		int iCoinNum = std::min(stParam.m_iGetCoinNum, pstLevelConfig->m_iMaxCoinNum);
		stParam.m_stBonusList.push_back(CPackGameItem(
			CLogicConfigDefine::CURRENCY,
			CLogicConfigDefine::ECIT_GoldID,
			iCoinNum));

		m_pUserInfo->m_stUserInfoTable.SetFightLevelID(0);

		static std::string strBonusItem;

		///////////////////////////////////////////////////////////////////////////
		//////////////////// Calc User Team Level
		stParam.m_stBonusList.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_RoleExpID, pstLevelConfig->m_iBonusTeamExp);
		///////////////////////////////////////////////////////////////////////////
		//////////////////// Add User Game Item
		CLogicCacheCommon::AddGameItem(m_pUserInfo, stParam.m_stBonusList, strBonusItem, m_usCmd);
		///////////////////////////////////////////////////////////////////////////
		//////////////////// Consume Energy
		int iEnergy = pstLevelConfig->m_iBonusTeamExp;
        CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_EnergyID, iEnergy, m_usCmd);

		///////////////////////////////////////////////////////////////////////////
		//////////////////// Upgrade Card Exp
		CLogicCacheCardOperation stCardOP(m_pUserInfo);
        stCardOP.UpgradeTeamCardLevelExp(pstLevelConfig->m_iBonusExp, stRsp.m_stCardLevelExp);

		stRsp.m_iOrder = stParam.m_iOrder;
		stRsp.m_iCoinNum = iCoinNum;
		stRsp.m_iTeamLevel = m_pUserInfo->m_stUserInfoTable.GetLevel();
		stRsp.m_iTeamExp = m_pUserInfo->m_stUserInfoTable.GetExp();

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << pstTowerConfig->m_iMapID << "|LEVEL_STAR:" << 0
			<< "|BONUS_CARD_EXP:" << pstLevelConfig->m_iBonusExp << "|GET_COIN_NUM:" << iCoinNum
			<< "|CONSUME_PHYPOWER:" << iEnergy
			<< "|LEVEL_TOTAL_BONUS:" << strBonusItem << std::endl;

		// CLogicCacheCommon::CalcCombat(m_pUserInfo);

		m_pUserInfo->m_stUserInfoTable.SetClimbTowerCurrentOrder(stParam.m_iOrder);
		if (stParam.m_iOrder > m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder())
		{
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_Tower;
            stEventData.m_iPara = m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder();
            stEventData.m_iParaEx = stParam.m_iOrder;
            stEventData.m_iCmd = m_usCmd;

			m_pUserInfo->m_stUserInfoTable.SetClimbTowerMaxOrder(stParam.m_iOrder);

			CLogicDailyTaskSystem::SetProgress(m_pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_TOWER_MAX, m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder());

			CLogicBill::SaveFirstFightBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, pstTowerConfig->m_iMapID, 1, m_pUserInfo->m_stUserInfoTable.GetRoleCombat());

            // 通用事件
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
		}

		CLogicBill::SaveFightPassBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, pstTowerConfig->m_iMapID, m_pUserInfo->m_stUserInfoTable.GetRoleCombat()
				, m_pUserInfo->m_stUserTeamInfoTable.GetTeam1(), "");


		CLogicGlobal::UpdateOrInsertTowerRank(m_iUin, m_iGroupID, m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder());

        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iUin = m_iUin;
        stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;
		int iRank = LOGIC_CLIMB_TOWER_RANK_INS.GetRank(stKey.m_ulRoleID);
		int iHighestRank = m_pUserInfo->m_stUserInfoTable.GetClimbTowerHighestRank();
		if (!iHighestRank || (iHighestRank > iRank))
		{
			m_pUserInfo->m_stUserInfoTable.SetClimbTowerHighestRank(iRank);
		}

		CLogicClimbTowerOrderChannel::data_type stOrder;
		stOrder.m_iOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder();
		LOGIC_PUBLISHER_INS->PublishClimbTowerOrderInfo(m_pUserInfo, stOrder);

		stRsp.m_stClimbTowerInfo.m_iCurrentOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder();
		stRsp.m_stClimbTowerInfo.m_iMaxOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder();
		if (!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetLastClimbTowerResetTime()))
		{
			stRsp.m_stClimbTowerInfo.m_iResetCount = m_pUserInfo->m_stUserInfoTable.GetClimbTowerResetCount();
		}
		stRsp.m_stClimbTowerInfo.m_iSweepEndTime = m_pUserInfo->m_stUserInfoTable.GetClimbTowerSweepEndTime();

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

	}
	else if ((int)m_usCmd == MSG_LOGIC_RESET_CLIMB_TOWER_ORDER)
	{
		CResponseResetClimbTowerOrder stRsp;

		if (0 == m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder())
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << INVALID_CLIMB_TOWER_ORDER
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(INVALID_CLIMB_TOWER_ORDER)
				<< std::endl;

			SEND_ERRCODE_AND_RET(INVALID_CLIMB_TOWER_ORDER)
		}

		auto pstVipInfo = CLogicConfig::GetVipConfig().GetVipInfo(m_pUserInfo->m_stUserInfoTable.GetVipLevel());
		if (!pstVipInfo)
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|VIP:" << m_pUserInfo->m_stUserInfoTable.GetVipLevel()
				<< std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		int iResetCount = 1;
		if (!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetLastClimbTowerResetTime()))
			iResetCount = m_pUserInfo->m_stUserInfoTable.GetClimbTowerResetCount() + 1;

		int iCanResetCount = pstVipInfo->m_iCanRefreshClimbTowerCount;
		if (iResetCount > iCanResetCount)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_RESET_CLIMB_TOWER_COUNT
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_RESET_CLIMB_TOWER_COUNT)
				<< "|RESET COUNT:" << m_pUserInfo->m_stUserInfoTable.GetClimbTowerResetCount()
				<< "|CAN RESET COUNT:" << iCanResetCount
				<< std::endl;

			SEND_ERRCODE_AND_RET(NOT_ENOUGH_RESET_CLIMB_TOWER_COUNT)
		}

		int32_t iCost = CLogicConfig::GetClimbTowerConfig().GetResetCost(iResetCount);
		if (iCost >= 99999999)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,
			CLogicConfigDefine::CURRENCY,
			CLogicConfigDefine::ECIT_YuanBaoID,
			iCost, m_usCmd))
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		m_pUserInfo->m_stUserInfoTable.SetClimbTowerResetCount(iResetCount);
		m_pUserInfo->m_stUserInfoTable.SetLastClimbTowerResetTime(CLogicCommon::GetSec());
		m_pUserInfo->m_stUserInfoTable.SetClimbTowerCurrentOrder(0);

		stRsp.m_iClimbTowerCurrentOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder();
		stRsp.m_iResetClimbTowerCount = m_pUserInfo->m_stUserInfoTable.GetClimbTowerResetCount();

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	}
	else if ((int)m_usCmd == MSG_LOGIC_FIGHT_CLIMB_TOWER_START)
	{
		CRequestFightClimbTowerStart stParam;
		CResponseFightClimbTowerStart stRsp;

		if (DecodeMsgPack(stFormatData, stParam) == false)  return false;

		CLogicCacheCommon::AutoAddPhyPower(m_pUserInfo);

		if (m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder() + 1 != stParam.m_iOrder)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << INVALID_CLIMB_TOWER_ORDER
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(INVALID_CLIMB_TOWER_ORDER)
				<< "|ORDER_ID:" << stParam.m_iOrder << std::endl;

			SEND_ERRCODE_AND_RET(INVALID_CLIMB_TOWER_ORDER)
		}

		auto pstTowerConfig = CLogicConfig::GetClimbTowerConfig().GetConfigElem(stParam.m_iOrder);
		if (nullptr == pstTowerConfig)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|ORDER_ID:" << stParam.m_iOrder << std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		auto pstLevelConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(pstTowerConfig->m_iMapID);
		if (nullptr == pstLevelConfig)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|ORDER_ID:" << stParam.m_iOrder
				<< "|MAP_ID:" << pstTowerConfig->m_iMapID << std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		CLogicCacheLevelOperation stOperation(m_pUserInfo);

		stOperation.FlushLevelTimes(pstTowerConfig->m_iMapID);
		if (false == stOperation.CheckCanFightLevel(pstTowerConfig->m_iMapID, pstLevelConfig))
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|LEVEL_ID:" << pstTowerConfig->m_iMapID << std::endl;
			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		m_pUserInfo->m_stUserInfoTable.SetFightLevelID(pstTowerConfig->m_iMapID);
        m_pUserInfo->m_stCacheFightLevelDropBonus.clear();
        CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pstLevelConfig->m_strBonusCardBag, m_pUserInfo->m_stCacheFightLevelDropBonus);

		stRsp.m_stBonusList = m_pUserInfo->m_stCacheFightLevelDropBonus;

		std::string strBonusList = CLogicCacheCommon::GetGameItemString(stRsp.m_stBonusList);
		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << pstTowerConfig->m_iMapID
			<< "|RAND_BONUS_LIST:" << strBonusList << std::endl;

		if (stParam.m_iOrder > m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder())
		{
			CLogicBill::SaveFirstFightBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, pstTowerConfig->m_iMapID, 0, m_pUserInfo->m_stUserInfoTable.GetRoleCombat());
		}

		CLogicGameLevelChannel::data_type stData;
		stData.m_iLevelID = pstTowerConfig->m_iMapID;
		stData.m_iTimes = 1;
		LOGIC_PUBLISHER_INS->PublishGameLevelInfo(m_pUserInfo, stData);

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	}
	else if ((int)m_usCmd == MSG_LOGIC_SWEEP_CLIMB_TOWER_START)
	{
		CResponseSweepClimbTowerStart stRsp;

		int32_t iDeltaOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder() - m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder();

		if (iDeltaOrder > 0)
		{
			m_pUserInfo->m_stUserInfoTable.SetClimbTowerSweepEndTime(CLogicCommon::GetSec() + iDeltaOrder * CLogicConfig::GetClimbTowerConfig().m_iSweepCostTime);

			stRsp.m_iSweepEndTime = m_pUserInfo->m_stUserInfoTable.GetClimbTowerSweepEndTime();
		}

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;
	}
	else if ((int)m_usCmd == MSG_LOGIC_SWEEP_CLIMB_TOWER_AWARD)
	{
		CRequestSweepClimbTowerAward stParam;
		CResponseSweepClimbTowerAward stRsp;

		if (DecodeMsgPack(stFormatData, stParam) == false)   return false;

		if (m_pUserInfo->m_stUserInfoTable.GetClimbTowerSweepEndTime() <= 0)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLIMB_TOWER_SWEEP_NOT_START
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(CLIMB_TOWER_SWEEP_NOT_START)
				<< "|CMD:" << m_usCmd << std::endl;
			SEND_ERRCODE_AND_RET(CLIMB_TOWER_SWEEP_NOT_START)
		}

		int32_t iCostYuanBao = GetSweepCostYuanbao();
		if (iCostYuanBao > stParam.m_iCostYuanBao)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLIMB_TOWER_SWEEP_NOT_ENOUGH_TIME
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(CLIMB_TOWER_SWEEP_NOT_ENOUGH_TIME)
				<< "|CMD:" << m_usCmd << std::endl;
			SEND_ERRCODE_AND_RET(CLIMB_TOWER_SWEEP_NOT_ENOUGH_TIME)
		}

		stParam.m_iCostYuanBao = iCostYuanBao;
		if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,
			CLogicConfigDefine::CURRENCY,
			CLogicConfigDefine::ECIT_YuanBaoID,
			stParam.m_iCostYuanBao, m_usCmd))
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		int32_t iCardLevelExp = 0, iRoleExp = 0;
        int32_t iBaseMapID = 0;
        int32_t iLastOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder();
		while (m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder() < m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder())
		{
			int iOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder() + 1;

			auto pstTowerConfig = CLogicConfig::GetClimbTowerConfig().GetConfigElem(iOrder);
			if (nullptr == pstTowerConfig)
			{
				using namespace SERVER_ERRCODE;
				PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
					<< "|MSG:" << CLogicError::GetErrMsg()
					<< "|ORDER:" << iOrder << std::endl;

				break;
			}

			auto pstLevelConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(pstTowerConfig->m_iMapID);
			if (nullptr == pstLevelConfig)
			{
				PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
					<< "|MSG:" << CLogicError::GetErrCode()
					<< "|LEVEL_ID:" << pstTowerConfig->m_iMapID << std::endl;

				break;
			}

            if(0 == iBaseMapID) iBaseMapID = pstTowerConfig->m_iMapID;

			CLogicCacheLevelOperation stOperation(m_pUserInfo);
            std::vector<CPackGameItem> stRandBonus;
            CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pstLevelConfig->m_strBonusCardBag, stRandBonus);
			stRsp.m_stBonusList.insert(stRsp.m_stBonusList.end(), stRandBonus.begin(), stRandBonus.end());
			CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pstLevelConfig->m_strPerfectCardBag, stRsp.m_stBonusList);

			iCardLevelExp += pstLevelConfig->m_iBonusExp;
			iRoleExp += pstLevelConfig->m_iBonusTeamExp;

			m_pUserInfo->m_stUserInfoTable.AddClimbTowerCurrentOrder(1);
		}

        if(iBaseMapID)
        {
            CLogicGameLevelChannel::data_type stData;
            stData.m_iLevelID = iBaseMapID;
            stData.m_iTimes = m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder() - iLastOrder;
            LOGIC_PUBLISHER_INS->PublishGameLevelInfo(m_pUserInfo, stData);
        }
		
		m_pUserInfo->m_stUserInfoTable.SetClimbTowerSweepEndTime(0);

        stRsp.m_stBonusList.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_RoleExpID, iRoleExp);

		static std::string strBonusItem;
		CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stBonusList, strBonusItem, m_usCmd);

		///////////////////////////////////////////////////////////////////////////
		//////////////////// Upgrade Card Exp
		CLogicCacheCardOperation stCardOP(m_pUserInfo);
        stCardOP.UpgradeTeamCardLevelExp(iCardLevelExp, stRsp.m_stCardLevelExp);

		stRsp.m_iTeamLevel = m_pUserInfo->m_stUserInfoTable.GetLevel();
		stRsp.m_iTeamExp = m_pUserInfo->m_stUserInfoTable.GetExp();

		stRsp.m_stClimbTowerInfo.m_iCurrentOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerCurrentOrder();
		stRsp.m_stClimbTowerInfo.m_iMaxOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder();
		if (!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetLastClimbTowerResetTime()))
		{
			stRsp.m_stClimbTowerInfo.m_iResetCount = m_pUserInfo->m_stUserInfoTable.GetClimbTowerResetCount();
		}
		stRsp.m_stClimbTowerInfo.m_iSweepEndTime = m_pUserInfo->m_stUserInfoTable.GetClimbTowerSweepEndTime();

		CLogicGlobal::UpdateOrInsertTowerRank(m_iUin, m_iGroupID, m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder());

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|SWEEP CLIMB TOWER"
			<< "|BONUS_TEAM_EXP:" << iCardLevelExp << "|BONUS_CARD_EXP:" << iRoleExp
			<< "|TOTAL_BONUS:" << strBonusItem << std::endl;

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;
	}
	else if ((int)m_usCmd == MSG_LOGIC_SWEEP_CLIMB_TOWER_YUANBAO)
	{
		CResponseSweepClimbTowerYuanbao stRsp;

		stRsp.m_iCostYuanBao = GetSweepCostYuanbao();

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|SWEEP CLIMB TOWER"
			<< "|COST YUANBAO:" << stRsp.m_iCostYuanBao << std::endl;

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;
	}

	return true;
}

int32_t CLogicClimbTowerProcessor::GetSweepCostYuanbao()
{
	int32_t iEndTime = m_pUserInfo->m_stUserInfoTable.GetClimbTowerSweepEndTime();
	int32_t iNow = CLogicCommon::GetSec();
	int32_t cost = CLogicConfig::GetClimbTowerConfig().m_iSweepCostYuanbao;
	int32_t time = CLogicConfig::GetClimbTowerConfig().m_iSweepCostTime;
	int32_t iCostYuanBao = 0;
	if (iEndTime > iNow)
	{
		iCostYuanBao = ceil(1.0f * (iEndTime - iNow) / time * cost);
	}
	return iCostYuanBao;
}
