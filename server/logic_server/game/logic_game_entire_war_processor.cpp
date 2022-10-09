#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_entire_war_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_day_refresh_system.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_global.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_global.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_bill.h"
#include "logic_manager_limit_boss.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ENTIRE_WAR_INFO, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_ENTIRE_WAR_START, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_ENTIRE_WAR_RESULT, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SWEEP_ENTIRE_WAR_CREEPS, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ENTIRE_WAR_MARKET, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_BUY_ENTIRE_WAR_MARKET, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ENTIRE_WAR_TREASURE, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_DIG_ENTIRE_WAR_TREASURE, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ENTIRE_WAR_QUESTION, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CHANGE_ENTIRE_WAR_QUESTION, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ANSWER_ENTIRE_WAR_QUESTION, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ENTIRE_WAR_MAP_AWARD, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_UPGRADE_ENTIRE_MILITARK_LEVEL, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ENTIRE_WAR_FINAL_AWARD, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_ENTIRE_WAR_RANK, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_WORSHIP_ENTIRE_WAR_GOD, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_BUY_ACTION_COIN, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SIMPLE_GAME_ENTIRE_WAR, CLogicEntireWarProcessor)

REGISTER_CMD_FACTORY(MSG_LOGIC_LIMIT_BOSS_LEVEL_BEGIN, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LIMIT_BOSS_LEVEL_RESULT, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LIMIT_BOSS_INVITE, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LIMIT_BOSS_OPEN_INVITE_LIST, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LIMIT_BOSS_ACCEPT, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LIMIT_BOSS_OPEN_LIST, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LIMIT_BOSS_GET_BOX, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LIMIT_BOSS_OPEN_FRIEND_LIST, CLogicEntireWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LIMIT_BOSS_REFRESH_OPEN_BOSS, CLogicEntireWarProcessor)

CLogicEntireWarProcessor::CLogicEntireWarProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicEntireWarProcessor::DoUserRun(const msgpack::unpacked & stFormatData)
{
	bool bRet = false;

    // 检查限时宝箱
    if(m_pUserInfo->m_stIBusyData.GetLimitBoxTime() > 0 && m_pUserInfo->m_stIBusyData.GetLimitBoxTime() < CLogicCommon::GetSec())
    {
        m_pUserInfo->m_stIBusyData.SetLimitBoxTime(0);
    }

	RefreshSelfEntireWar();

	AutoAddActionCoin();

	if (m_usCmd != MSG_LOGIC_GET_ENTIRE_WAR_INFO)
	{
		const auto activity_time = CLogicGlobal::GetEntireWarActivityTime();
		const time_t this_time = CLogicCommon::GetSec();
		if (this_time < activity_time.first || this_time > activity_time.second)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
		}

        if(m_usCmd == MSG_LOGIC_FIGHT_ENTIRE_WAR_START
            || m_usCmd == MSG_LOGIC_SWEEP_ENTIRE_WAR_CREEPS
            || m_usCmd == MSG_LOGIC_GET_ENTIRE_WAR_QUESTION
            || m_usCmd == MSG_LOGIC_CHANGE_ENTIRE_WAR_QUESTION
            || m_usCmd == MSG_LOGIC_LIMIT_BOSS_ACCEPT
            || m_usCmd == MSG_LOGIC_LIMIT_BOSS_INVITE
            || m_usCmd == MSG_LOGIC_LIMIT_BOSS_OPEN_INVITE_LIST
            || m_usCmd == MSG_LOGIC_LIMIT_BOSS_LEVEL_BEGIN)
        {
            if (activity_time.second - this_time < 3600)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ENTIRE_WAR_RESET);
            }
        }
	}

	switch (m_usCmd)
	{
	case MSG_LOGIC_GET_ENTIRE_WAR_INFO:
		bRet = DoUserRunGetEntireWarInfo(stFormatData);
		break;

	case MSG_LOGIC_FIGHT_ENTIRE_WAR_START:
		bRet = DoUserRunFightEntireWarStart(stFormatData);
		break;

	case MSG_LOGIC_FIGHT_ENTIRE_WAR_RESULT:
		bRet = DoUserRunFightEntireWarResult(stFormatData);
		break;

	case MSG_LOGIC_SWEEP_ENTIRE_WAR_CREEPS:
		bRet = DoUserRunSweepEntireWar(stFormatData);
		break;

	case MSG_LOGIC_GET_ENTIRE_WAR_MARKET:
    case MSG_LOGIC_GET_ENTIRE_WAR_TREASURE:
		bRet = DoUserRunGetEntireWarMarket(stFormatData);
		break;

	case MSG_LOGIC_BUY_ENTIRE_WAR_MARKET:
		bRet = DoUserRunBuyEntireWarMarket(stFormatData);
		break;

	case MSG_LOGIC_DIG_ENTIRE_WAR_TREASURE:
		bRet = DoUserRunDigEntireWarTreasure(stFormatData);
		break;

	case MSG_LOGIC_GET_ENTIRE_WAR_QUESTION:
	case MSG_LOGIC_CHANGE_ENTIRE_WAR_QUESTION:
		bRet = DoUserRunGetEntireWarQuestion(stFormatData);
		break;

	case MSG_LOGIC_ANSWER_ENTIRE_WAR_QUESTION:
		bRet = DoUserRunAnswerEntireWarQuestion(stFormatData);
		break;

	case MSG_LOGIC_GET_ENTIRE_WAR_MAP_AWARD:
		bRet = DoUserRunGetEntireWarMapAward(stFormatData);
		break;

	case MSG_LOGIC_UPGRADE_ENTIRE_MILITARK_LEVEL:
		bRet = DoUserRunUpgradeEntireMilitaryLevel(stFormatData);
		break;

	case MSG_LOGIC_GET_ENTIRE_WAR_FINAL_AWARD:
		bRet = DoUserRunGetEntireWarFinalAward(stFormatData);
		break;

	case MSG_LOGIC_GET_ENTIRE_WAR_RANK:
		bRet = DoUserRunGetEntireWarRank(stFormatData);
		break;

	case MSG_LOGIC_WORSHIP_ENTIRE_WAR_GOD:
		bRet = DoUserRunWorshipEntireWarGod(stFormatData);
		break;

	case MSG_LOGIC_BUY_ACTION_COIN:
		bRet = DoUserRunBuyActionCoin(stFormatData);
		break;

    case MSG_LOGIC_SIMPLE_GAME_ENTIRE_WAR:
        bRet = DoUserSimpleGameEntireWar(stFormatData);
        break;

    case MSG_LOGIC_LIMIT_BOSS_LEVEL_BEGIN:
        bRet = DoUserRunLimitBossLevelBegin(stFormatData);
        break;

    case MSG_LOGIC_LIMIT_BOSS_LEVEL_RESULT:
        bRet = DoUserRunLimitBossLevelResult(stFormatData);
        break;

    case MSG_LOGIC_LIMIT_BOSS_INVITE:
        bRet = DoUserRunLimitBossInvite(stFormatData);
        break;

    case MSG_LOGIC_LIMIT_BOSS_OPEN_INVITE_LIST:
        bRet = DoUserRunLimitBossOpenInviteList(stFormatData);
        break;

    case MSG_LOGIC_LIMIT_BOSS_ACCEPT:
        bRet = DoUserRunLimitBossAccept(stFormatData);
        break;

    case MSG_LOGIC_LIMIT_BOSS_OPEN_LIST:
        bRet = DoUserRunLimitBossOpenList(stFormatData);
        break;

    case MSG_LOGIC_LIMIT_BOSS_GET_BOX:
        bRet = DoUserRunLimitBossGetBox(stFormatData);
        break;

    case MSG_LOGIC_LIMIT_BOSS_OPEN_FRIEND_LIST:
        bRet = DoUserRunLimitBossOpenFriendList(stFormatData);
        break;

    case MSG_LOGIC_LIMIT_BOSS_REFRESH_OPEN_BOSS:
        bRet = DoUserRunLimitBossRefreshOpenBoss(stFormatData);
        break;

	default:
		PROCESSOR_LOG_ERROR << "NOT FOUND PROCESSOR WITH CMD:" << m_usCmd << std::endl;
		break;
	}

	return bRet;
}

bool CLogicEntireWarProcessor::DoUserRunGetEntireWarInfo(const msgpack::unpacked & stFormatData)
{
	CResponseGetEntireWarInfo stRspBody;
	stRspBody.m_stInfo = GetUserEntireWarInfoPack();

    auto iterConfig = CLogicConfig::GetEntireWarMapConfig().m_stThemeMap.find(stRspBody.m_stInfo.m_iThemeID);
    if(iterConfig == CLogicConfig::GetEntireWarMapConfig().m_stThemeMap.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ENTIRE_WAR_RESET);
    }

	for (const auto& war : iterConfig->second.m_stWarMap)
	{
		stRspBody.m_astMapVec.emplace_back(GetUserEntireWarMapPack(war.first));
	}

    auto pMyBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(m_iUin);
    if(nullptr != pMyBossData)
    {
        if(pMyBossData->m_strNick.compare(m_pUserInfo->m_strNick) != 0)
        {
            CLogicManagerLimitBoss::UpdateOwnerName(m_iUin, m_pUserInfo->m_strNick);
        }

        stRspBody.m_stLimitBoss = CLogicManagerLimitBoss::GetPackBossInfo(*pMyBossData, m_iUin);
    }
    /*
    else
    {
        // 检查自己接受的BOSS
        auto stAcptList = m_pUserInfo->m_stIBusyData.GetLimitBossAccepted();
        for(auto iterApt = stAcptList.begin(); iterApt != stAcptList.end(); ++iterApt)
        {
            auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(*iterApt);
            if(nullptr != pBossData)
            {
                stRspBody.m_stLimitBoss = CLogicManagerLimitBoss::GetPackBossInfo(*pBossData, m_iUin);
                break;
            }
        }
    }
     */

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicEntireWarProcessor::DoUserRunFightEntireWarStart(const msgpack::unpacked & stFormatData)
{
	CRequestFightEntireWarStart stReqBody;
	CResponseFightEntireWarStart stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

    if (!CheckUserEntireWarTeamMember(stReqBody.m_iWarID, stReqBody.m_iCardIDVec))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TEAM_MEMBER_ERROR);
    }

	if (m_pUserInfo->m_stUserInfoTable.GetActionCoin() <= 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ACTION_COIN);
	}

	const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), stReqBody.m_iWarID);
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_CONFIG_WAR_ID);
	}

    if (!CLogicCacheCommon::SetTeam(m_pUserInfo, CLogicConfigDefine::ETT_EntireWarTeam, stReqBody.m_iCardIDVec))
    {
        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
    }

	if (!CLogicGlobal::IsEntireWarPass(pstConfig->m_iPreWarID))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PRE_WAR_NOT_PASS);
	}

    int32_t iRandLevelID = m_pUserInfo->m_stUserInfoTable.GetFightLevelID();
    switch (pstConfig->m_iNodeType)
    {
    case CLogicConfigDefine::EEWNT_PVE:
    case CLogicConfigDefine::EEWNT_PVP:
    case CLogicConfigDefine::EEWNT_Creeps:
        if(iRandLevelID < pstConfig->m_iNodePara1 || iRandLevelID > pstConfig->m_iNodePara2)
            iRandLevelID = CLogicCommon::GetRandNum(pstConfig->m_iNodePara1, pstConfig->m_iNodePara2, true);

        if(pstConfig->m_iNodeType != CLogicConfigDefine::EEWNT_Creeps && CLogicGlobal::IsEntireWarPass(stReqBody.m_iWarID))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::THIS_WAR_HAS_PASS);
        }

        break;
    case CLogicConfigDefine::EEWNT_Boss:
        iRandLevelID = pstConfig->m_iNodePara1;
        if(CLogicGlobal::IsEntireWarPass(stReqBody.m_iWarID))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::THIS_WAR_HAS_PASS);
        }

        break;
    default:
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
    }

	const auto* pstWarValue = CLogicGlobal::GetEntireWarMapInfo(stReqBody.m_iWarID);
	if (nullptr == pstWarValue)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_FOUND_WAR_ID);
	}

	stRspBody.m_iWarID = stReqBody.m_iWarID;
	stRspBody.m_iCardIDVec = stReqBody.m_iCardIDVec;
	stRspBody.m_iLevelID = iRandLevelID;
    stRspBody.m_ulTargetProgress = pstWarValue->m_ulTargetProgress;
    stRspBody.m_ulCurProgress = pstWarValue->m_ulCurProgress;

	m_pUserInfo->m_stUserInfoTable.SetFightLevelID(iRandLevelID);
	
	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicEntireWarProcessor::DoUserRunFightEntireWarResult(const msgpack::unpacked & stFormatData)
{
	CRequestFightEntireWarResult stReqBody;
	CResponseFightEntireWarResult stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return fail;
	}

	if (m_pUserInfo->m_stUserInfoTable.GetActionCoin() <= 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ACTION_COIN);
	}

	const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), stReqBody.m_iWarID);
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_CONFIG_WAR_ID);
	}

	if (!CLogicGlobal::IsEntireWarPass(pstConfig->m_iPreWarID))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PRE_WAR_NOT_PASS);
	}

	if (stReqBody.m_iLevelID != m_pUserInfo->m_stUserInfoTable.GetFightLevelID())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FIGHT_WAR_LEVEL_ERROR);
	}

	const auto pstWarValue = CLogicGlobal::GetEntireWarMapInfo(stReqBody.m_iWarID);
	if (nullptr == pstWarValue)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_FOUND_WAR_ID);
    }

	if (pstConfig->m_iNodeType == CLogicConfigDefine::EEWNT_Boss)
	{
		CLogicBill::SaveAntiScoreBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, pstConfig->m_iNodePara1
				, stReqBody.m_iHurtBossHp, 0, m_pUserInfo->m_stUserInfoTable.GetRoleCombat()
				, m_pUserInfo->m_stUserTeamInfoTable.GetTeam1(), "");

        if (pstWarValue->m_ulCurProgress < pstWarValue->m_ulTargetProgress)
        {
            if (uint64_t(stReqBody.m_iHurtBossHp) > pstWarValue->m_ulTargetProgress - pstWarValue->m_ulCurProgress)
            {
                stReqBody.m_iHurtBossHp = pstWarValue->m_ulTargetProgress - pstWarValue->m_ulCurProgress;
            }
        }
    }
    else
    {
        stReqBody.m_iHurtBossHp = 1;
    }

    // 更新进度
    if(CLogicGlobal::RefreshEntireWarNodeProgress(stReqBody.m_iWarID, stReqBody.m_iHurtBossHp))
    {
        std::vector<int> params;
        if (pstConfig->m_iBonusBuffType > 0 && pstConfig->m_iBonusBuffPercentage > 0 && pstConfig->m_iBonusBuffTime > 0)
        {
            params.push_back(pstConfig->m_iBonusBuffType);
            params.push_back(pstConfig->m_iBonusBuffPercentage);

            CLogicGlobal::AddNewEntireWarBuff(pstConfig->m_iBonusBuffType, pstConfig->m_iBonusBuffPercentage, pstConfig->m_iBonusBuffTime);
        }

        AddReport(stReqBody.m_iWarID, params);

        CLogicGlobal::PublishBroadcast("pass_entire_war", 0, pstConfig->m_iNotify
                                       , {{"nick", m_pUserInfo->m_strNick}, {"war_name", pstConfig->m_strName}});
    }

	m_pUserInfo->m_stUserInfoTable.SetFightLevelID(0);
	m_pUserInfo->m_stUserInfoTable.AddActionCoin(-1);

    stRspBody.m_stBonusItemVec.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_WarContributionID, pstConfig->m_iBonusContribution);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusItemVec, m_usCmd);

	stRspBody.m_iWarID = stReqBody.m_iWarID;
	stRspBody.m_iLevelID = stReqBody.m_iLevelID;
	stRspBody.m_stWarInfo = GetUserEntireWarInfoPack();
	stRspBody.m_stMapInfo = GetUserEntireWarMapPack(stReqBody.m_iWarID);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	logic_game_user_action_channel_data_t stData;
	stData.m_iTimes = 1;
	stData.m_iAction = CLogicGameChannelEnum::ACTION_ENTIRE_WAR_COUNT;
	LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

    // 随机限时BOSS
    RandLimitBoss();

	return (true);
}

bool CLogicEntireWarProcessor::DoUserRunSweepEntireWar(const msgpack::unpacked& stFormatData)
{
	CRequestSweepEntireWar stReqBody;
	CResponseFightEntireWarResult stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	int iSweepCount = 1;

	if (m_pUserInfo->m_stUserInfoTable.GetActionCoin() <= 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ACTION_COIN);
	}

	const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), stReqBody.m_iWarID);
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_CONFIG_WAR_ID);
	}

	if (!CLogicGlobal::IsEntireWarPass(pstConfig->m_iPreWarID))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PRE_WAR_NOT_PASS);
	}

	// 修改1- 只有野怪关可以扫荡
	if (pstConfig->m_iNodeType != CLogicConfigDefine::EEWNT_Creeps)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	// 只有所有关卡都通关，才可以扫荡
    const auto& stEntireMap = CLogicGlobal::GetGlobalCache()->m_stEntireWarMap;
    for(auto iter = stEntireMap.Begin(); iter != stEntireMap.End(); ++iter)
    {
        if(!iter->second->GetIsPassed())
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ENTIRE_WAR_ALL_MAP_NEED_PASS);
    }

	//1.3只有战力大于推荐战力，才可以扫荡
	int needCombat = pstConfig->m_iPower * CLogicConfig::GetEntireWarConfig().GetWorldLevelRatio(CLogicGlobal::GetEntireWarLevel());
	if (m_pUserInfo->m_stUserInfoTable.GetRoleCombat() < needCombat)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ENTIRE_WAR_SWEEP_COMBAT_NOT_ENOUGH);
	}

	//2.复制DoUserRunFightEntireWarResult的内容
	m_pUserInfo->m_stUserInfoTable.AddActionCoin(-iSweepCount);

    stRspBody.m_stBonusItemVec.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_WarContributionID, pstConfig->m_iBonusContribution * iSweepCount);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusItemVec, m_usCmd);

	stRspBody.m_iWarID = stReqBody.m_iWarID;
	stRspBody.m_iLevelID = 0;
	stRspBody.m_stWarInfo = GetUserEntireWarInfoPack();
	stRspBody.m_stMapInfo = GetUserEntireWarMapPack(stReqBody.m_iWarID);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	logic_game_user_action_channel_data_t stData;
	stData.m_iTimes = iSweepCount;
	stData.m_iAction = CLogicGameChannelEnum::ACTION_ENTIRE_WAR_COUNT;
	LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

    // 随机限时BOSS
    RandLimitBoss();

	return true;
}

bool CLogicEntireWarProcessor::DoUserRunGetEntireWarMarket(const msgpack::unpacked& stFormatData)
{
	CRequestGetEntireWarMarket stReqBody;
	CResponseGetEntireWarMarket stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), stReqBody.m_iWarID);
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_MARKET_WAR);
	}

    if (pstConfig->m_iNodeType != CLogicConfigDefine::EEWNT_Market && pstConfig->m_iNodeType != CLogicConfigDefine::EEWNT_Treasure)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_MARKET_WAR);
    }

	if (!CLogicGlobal::IsEntireWarPass(pstConfig->m_iPreWarID))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PRE_WAR_NOT_PASS);
	}

	stRspBody.m_iWarID = stReqBody.m_iWarID;
	stRspBody.m_stMarketVec = GetUserEntireWarMarketPack(stReqBody.m_iWarID);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	return (true);
}

bool CLogicEntireWarProcessor::DoUserRunBuyEntireWarMarket(const msgpack::unpacked& stFormatData)
{
	CRequestBuyEntireWarMarket stReqBody;
	CResponseBuyEntireWarMarket stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), stReqBody.m_iWarID);
	if (nullptr == pstConfig || pstConfig->m_iNodeType != CLogicConfigDefine::EEWNT_Market)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_MARKET_WAR);
	}

	if (!CLogicGlobal::IsEntireWarPass(pstConfig->m_iPreWarID))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PRE_WAR_NOT_PASS);
	}

	const auto* pstMarketValue = CLogicGlobal::GetEntireWarExtraInfo(stReqBody.m_iWarID, stReqBody.m_iMarketIndex);
	if (nullptr == pstMarketValue)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MARKET_REFRESH_ERROR);
    }

	if (IsUserEntireWarExtraAward(stReqBody.m_iWarID, stReqBody.m_iMarketIndex))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ITEM_HAS_SOLD_OUT);
	}

	stRspBody.m_iWarID = stReqBody.m_iWarID;
	stRspBody.m_iMarketIndex = stReqBody.m_iMarketIndex;
	stRspBody.m_stCommodity = { pstMarketValue->m_iCommodityItemType, pstMarketValue->m_iCommodityItemID, pstMarketValue->m_iCommodityItemNum };
	stRspBody.m_stConsume = { pstMarketValue->m_iConsumeItemType, pstMarketValue->m_iConsumeItemID, pstMarketValue->m_iConsumeItemNum };

	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRspBody.m_stConsume, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	SetUserEntireWarExtraAward(stReqBody.m_iWarID, stReqBody.m_iMarketIndex);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stCommodity, m_usCmd);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	return (true);
}

bool CLogicEntireWarProcessor::DoUserRunDigEntireWarTreasure(const msgpack::unpacked& stFormatData)
{
	CRequestDigEntireWarTreasure stReqBody;
	CResponseDigEntireWarTreasure stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), stReqBody.m_iWarID);
	if (nullptr == pstConfig || pstConfig->m_iNodeType != CLogicConfigDefine::EEWNT_Treasure)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_TREASURE_WAR);
	}

	if (!CLogicGlobal::IsEntireWarPass(pstConfig->m_iPreWarID))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PRE_WAR_NOT_PASS);
	}

	int32_t iHaveDigCount = 0;
	std::map<int32_t, int32_t> dig_weight;
	int32_t iTotalWeight = 0;
	for (int32_t i = 1; i <= CLogicConfigDefine::ENTIRE_WAR_TREASURE_COUNT; ++i)
	{
		if (IsUserEntireWarExtraAward(stReqBody.m_iWarID, i))
		{
			++iHaveDigCount;
		}
		else
		{
		    const auto* pstExtraValue = CLogicGlobal::GetEntireWarExtraInfo(stReqBody.m_iWarID, i);
		    if (pstExtraValue)
            {
                dig_weight[i] = pstExtraValue->m_iDiscount;
                iTotalWeight += pstExtraValue->m_iDiscount;
            }
		}
	}

	const int32_t iTotalDigCount = CLogicConfig::GetEntireWarConfig().m_iDefaultTreasureCount;
	if (iHaveDigCount >= iTotalDigCount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
	}

	if (iTotalWeight <= 0 || dig_weight.empty())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TREASURE_WEIGHT_ERROR);
	}

	//一键获取所有奖励
	if ((iTotalDigCount == CLogicConfigDefine::ENTIRE_WAR_TREASURE_COUNT) && (iHaveDigCount == 0))
	{
		int32_t dig_index = -1;
		stRspBody.m_iWarID = stReqBody.m_iWarID;
		stRspBody.m_iDigIndex = 0;
		for (const auto& dig : dig_weight)
		{
			dig_index = dig.first;
			const auto* pstExtraValue = CLogicGlobal::GetEntireWarExtraInfo(stReqBody.m_iWarID, dig_index);
			if (nullptr == pstExtraValue)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TREASURE_REFRESH_ERROR);
            }

			stRspBody.m_stBonusItems.emplace_back(pstExtraValue->m_iCommodityItemType, pstExtraValue->m_iCommodityItemID, pstExtraValue->m_iCommodityItemNum);

			SetUserEntireWarExtraAward(stReqBody.m_iWarID, dig_index);
		}

        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusItems, m_usCmd);
	}
	else
	{
		int32_t iRand = CLogicCommon::GetRandNum() % iTotalWeight;
		int32_t dig_index = -1;
		for (const auto& dig : dig_weight)
		{
			if (iRand < dig.second)
			{
				dig_index = dig.first;
				break;
			}
			iRand -= dig.second;
		}

        const auto* pstExtraValue = CLogicGlobal::GetEntireWarExtraInfo(stReqBody.m_iWarID, dig_index);
        if (nullptr == pstExtraValue)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TREASURE_REFRESH_ERROR);
        }

		stRspBody.m_iWarID = stReqBody.m_iWarID;
        stRspBody.m_stBonusItems.emplace_back(pstExtraValue->m_iCommodityItemType, pstExtraValue->m_iCommodityItemID, pstExtraValue->m_iCommodityItemNum);
		stRspBody.m_iDigIndex = dig_index;

		SetUserEntireWarExtraAward(stReqBody.m_iWarID, dig_index);
		CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusItems, m_usCmd);
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	return (true);
}

bool CLogicEntireWarProcessor::DoUserRunGetEntireWarQuestion(const msgpack::unpacked& stFormatData)
{
	CRequestGetEntireWarQuestion stReqBody;
	CResponseGetEntireWarQuestion stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), stReqBody.m_iWarID);
	if (nullptr == pstConfig || pstConfig->m_iNodeType != CLogicConfigDefine::EEWNT_Question)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_QUESTION_WAR);
	}

	if (!CLogicGlobal::IsEntireWarPass(pstConfig->m_iPreWarID))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PRE_WAR_NOT_PASS);
	}

	if (m_pUserInfo->m_stUserInfoTable.GetActionCoin() <= 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ACTION_COIN);
	}

	const auto& question_map = m_pUserInfo->m_stUserInfoTable.GetEntireWarNodePara();
	const auto iterQuestion = question_map.find(stReqBody.m_iWarID);
	if (iterQuestion == question_map.end() || m_usCmd == MSG_LOGIC_CHANGE_ENTIRE_WAR_QUESTION)
	{
		
		stRspBody.m_iQuestionID = GetNewUserEntireWarQuestion(stReqBody.m_iWarID);
	}
	else
	{
		stRspBody.m_iQuestionID = iterQuestion->second;
	}

	stRspBody.m_iWarID = stReqBody.m_iWarID;
	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	return (true);
}

bool CLogicEntireWarProcessor::DoUserRunAnswerEntireWarQuestion(const msgpack::unpacked& stFormatData)
{
	CRequestAnswerEntireWarQuestion stReqBody;
	CResponseAnswerEntireWarQuestion stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), stReqBody.m_iWarID);
	if (nullptr == pstConfig || pstConfig->m_iNodeType != CLogicConfigDefine::EEWNT_Question)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_QUESTION_WAR);
	}

	if (!CLogicGlobal::IsEntireWarPass(pstConfig->m_iPreWarID))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PRE_WAR_NOT_PASS);
	}

	if (m_pUserInfo->m_stUserInfoTable.GetActionCoin() <= 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ACTION_COIN);
	}

	const auto* pstWarValue = CLogicGlobal::GetEntireWarMapInfo(stReqBody.m_iWarID);
	if (nullptr == pstWarValue)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_FOUND_WAR_ID);
	}

    if(CLogicGlobal::RefreshEntireWarNodeProgress(stReqBody.m_iWarID, 1))
    {
        AddReport(stReqBody.m_iWarID, {});
    }

    GetNewUserEntireWarQuestion(stReqBody.m_iWarID);

	m_pUserInfo->m_stUserInfoTable.AddActionCoin(-1);

    stRspBody.m_stBonusItemVec.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_WarContributionID, pstConfig->m_iBonusContribution);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusItemVec, m_usCmd);

	stRspBody.m_iWarID = stReqBody.m_iWarID;
	stRspBody.m_stWarInfo = GetUserEntireWarInfoPack();
	stRspBody.m_stMapInfo = GetUserEntireWarMapPack(stReqBody.m_iWarID);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	logic_game_user_action_channel_data_t stData;
	stData.m_iTimes = 1;
	stData.m_iAction = CLogicGameChannelEnum::ACTION_ENTIRE_WAR_COUNT;
	LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

    // 随机限时BOSS
    RandLimitBoss();

	return true;
}

bool CLogicEntireWarProcessor::DoUserRunGetEntireWarMapAward(const msgpack::unpacked& stFormatData)
{
	CRequestGetEntireWarMapAward stReqBody;
	CResponseGetEntireWarMapAward stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), stReqBody.m_iWarID);
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_CONFIG_WAR_ID);
	}

	if (!CLogicGlobal::IsEntireWarPass(stReqBody.m_iWarID))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PRE_WAR_NOT_PASS);
	}

    auto stAwardStat = m_pUserInfo->m_stUserInfoTable.GetEntireWarNodeAwardStat();
	if (CLogicCacheCommon::VecBitmapCheckState(stAwardStat, stReqBody.m_iWarID))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
	}

	stRspBody.m_iWarID = stReqBody.m_iWarID;
	stRspBody.m_iAddActionCoin = pstConfig->m_iBonusActionCoin;
	stRspBody.m_stBonusItemVec = pstConfig->m_stBonusItemVec;

    CLogicCacheCommon::VecBitmapSetState(stAwardStat, stReqBody.m_iWarID);
    m_pUserInfo->m_stUserInfoTable.SetEntireWarNodeAwardStat(stAwardStat);

	m_pUserInfo->m_stUserInfoTable.AddActionCoin(stRspBody.m_iAddActionCoin);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusItemVec, m_usCmd);

	stRspBody.m_stWarInfo = GetUserEntireWarInfoPack();
	stRspBody.m_stMapInfo = GetUserEntireWarMapPack(stReqBody.m_iWarID);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	return (true);
}

bool CLogicEntireWarProcessor::DoUserRunUpgradeEntireMilitaryLevel(const msgpack::unpacked &stFormatData)
{
	CResponseUpgradeEntireMilitarkLevel stRspBody;

	const auto iter = CLogicConfig::GetEntireWarConfig().m_stMilitaryRankMap.find(m_pUserInfo->m_stUserInfoTable.GetEntireWarMilitarkLevel());
	if (iter == CLogicConfig::GetEntireWarConfig().m_stMilitaryRankMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_CONFIG_MILILTARY);
	}

	if (std::next(iter) == CLogicConfig::GetEntireWarConfig().m_stMilitaryRankMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MILITARY_LEVEL_MAX);
	}

	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_WarContributionID, iter->second.m_iCostContribution, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	m_pUserInfo->m_stUserInfoTable.AddEntireWarMilitarkLevel(1);

	CLogicGlobal::UpdateOrInsertMilitaryRank(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_stUserInfoTable.GetEntireWarMilitarkLevel());

	stRspBody.m_iMilitarkLevel = m_pUserInfo->m_stUserInfoTable.GetEntireWarMilitarkLevel();
	stRspBody.m_iEntireWarContribution = m_pUserInfo->m_stUserInfoTable.GetEntireWarContribution();

	CLogicDailyTaskSystem::SetProgress(m_pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_ENTIRE_MILITARK_LEVEL, m_pUserInfo->m_stUserInfoTable.GetEntireWarMilitarkLevel());

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	return (true);
}

bool CLogicEntireWarProcessor::DoUserRunGetEntireWarFinalAward(const msgpack::unpacked& stFormatData)
{
	CRequestGetEntireWarFinalAward stReqBody;
	CResponseGetEntireWarFinalAward stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto iter = CLogicConfig::GetEntireWarConfig().m_stFinalAwardMap.find(stReqBody.m_iFinalID);
	if (iter == CLogicConfig::GetEntireWarConfig().m_stFinalAwardMap.end() || stReqBody.m_iFinalID < 0 || stReqBody.m_iFinalID > 60)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_CONFIG_FINAL_AWARD);
	}

    int32_t iTotalWinCount = 0;
    const auto& stEntireMap = CLogicGlobal::GetGlobalCache()->m_stEntireWarMap;
    for(auto iterMap = stEntireMap.Begin(); iterMap != stEntireMap.End(); ++iterMap)
    {
        if(iterMap->second->GetTargetProgress() > 0 && iterMap->second->GetIsPassed())
            ++iTotalWinCount;
    }

	if (iTotalWinCount < iter->second.m_iLevelCount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::WAR_WIN_COUNT_NOT_ENOUGH);
	}

	const uint64_t ulFlag = 1ULL << stReqBody.m_iFinalID;
	if (m_pUserInfo->m_stUserInfoTable.GetEntireWarAwardBitmap() & ulFlag)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
	}

	m_pUserInfo->m_stUserInfoTable.AddEntireWarAwardBitmap(ulFlag);

	stRspBody.m_iFinalID = stReqBody.m_iFinalID;
	stRspBody.m_strEntireWarAwardBitmap = int2bin(m_pUserInfo->m_stUserInfoTable.GetEntireWarAwardBitmap());
	stRspBody.m_stBonusItemVec = iter->second.m_stBonusItemVec;

	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusItemVec, m_usCmd);
	
	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicEntireWarProcessor::DoUserRunGetEntireWarRank(const msgpack::unpacked& stFormatData)
{
	CResponseGetEntireWarRank stRspBody;

	const auto stWarGodKey = CLogicGlobal::GetEntireWarGodKey();
	const auto pstWarGod = CLogicCacheCommon::LoadUserData(stWarGodKey.m_iGroupID, stWarGodKey.m_iUin);
	if (nullptr != pstWarGod)
	{
		stRspBody.m_stWarGod = CLogicCacheCommon::GetUserBriefInfo(pstWarGod);
		stRspBody.m_iTotalWorshippedCount = pstWarGod->m_stUserInfoTable.GetEntireWarWorshippedCount();
		stRspBody.m_iTodayWorshippedCount = pstWarGod->m_stUserDailyTable.GetEntireWarWorshippedCount();
	}
	stRspBody.m_iSelfWeeklyContribution = m_pUserInfo->m_stUserInfoTable.GetWeekWarContribution();
	const user_info_key_t stKey(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
	stRspBody.m_iSelfRank = LOGIC_ENTIRE_WAR_RANK_INS.GetRank(stKey.m_ulRoleID);
	
	int32_t iRank = 1;
	for (auto iter = LOGIC_ENTIRE_WAR_RANK_INS.begin(); iter != LOGIC_ENTIRE_WAR_RANK_INS.end() && iRank <= 30; ++iter, ++iRank)
	{
		const user_info_key_t stRankKey(iter->first);
		const auto pstRankUser = CLogicCacheCommon::LoadUserData(stRankKey.m_stRoleIDDetail.m_iGroupID, stRankKey.m_stRoleIDDetail.m_iUin);
		if (nullptr != pstRankUser)
		{
			stRspBody.m_stRank.emplace_back(CLogicCacheCommon::GetUserRankInfo(pstRankUser, iRank, iter->second));
		}
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicEntireWarProcessor::DoUserRunWorshipEntireWarGod(const msgpack::unpacked& stFormatData)
{
	CResponseWorshipEntireWarGod stRspBody;

    const auto stWarGodKey = CLogicGlobal::GetEntireWarGodKey();
    if (stWarGodKey.m_iUin == m_pUserInfo->m_iUin)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::CANT_WORSHIP_SELF);
	}
	
	auto pstWarGod = CLogicCacheCommon::LoadUserData(stWarGodKey.m_iGroupID, stWarGodKey.m_iUin);
	if (nullptr == pstWarGod)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::USER_ROLE_NOT_EXIST);
	}
	
	if (m_pUserInfo->m_stUserDailyTable.GetWorshipWarGodCount() > 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HAS_WORSHIP_WAR_GOD);
	}
		
	pstWarGod->m_stUserInfoTable.AddEntireWarWorshippedCount(1);
	pstWarGod->m_stUserDailyTable.AddEntireWarWorshippedCount(1);

	m_pUserInfo->m_stUserDailyTable.SetWorshipWarGodCount(1);
	CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);

	stRspBody.m_iTotalWorshippedCount = pstWarGod->m_stUserInfoTable.GetEntireWarWorshippedCount();
	stRspBody.m_iTodayWorshippedCount = pstWarGod->m_stUserDailyTable.GetEntireWarWorshippedCount();
	stRspBody.m_stBonusItemVec = CLogicConfig::GetEntireWarConfig().m_stWorshipItemVec;

	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusItemVec, m_usCmd);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	const int32_t iRand = CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_RandomWeight;
	if (iRand < CLogicConfig::GetEntireWarConfig().m_iDoubleProbability)
	{
		pstWarGod->m_stUserInfoTable.AddEntireWarWorshippedCount(1);
		pstWarGod->m_stUserDailyTable.AddEntireWarWorshippedCount(1);
	}
	else if (iRand < CLogicConfig::GetEntireWarConfig().m_iDoubleProbability + CLogicConfig::GetEntireWarConfig().m_iThreeProbability)
	{
		pstWarGod->m_stUserInfoTable.AddEntireWarWorshippedCount(2);
		pstWarGod->m_stUserDailyTable.AddEntireWarWorshippedCount(2);
	}

	return (true);
}

bool CLogicEntireWarProcessor::DoUserRunBuyActionCoin(const msgpack::unpacked& stFormatData)
{
	CResponseBuyActionCoin stRspBody;

	const auto* pstVipConfig = CLogicConfig::GetVipConfig().GetVipInfo(m_pUserInfo->m_stUserInfoTable.GetVipLevel());
	if (nullptr == pstVipConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::VIP_INFO_IS_NOT_CONFIGED);
	}

	if (m_pUserInfo->m_stUserDailyTable.GetBuyActionCoinCount() >= pstVipConfig->m_iCanBuyActionCoinCount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_VIP_LEVEL);
	}

	const auto iterCost = CLogicConfig::GetEntireWarConfig().m_stBuyActionCoinCost.find(m_pUserInfo->m_stUserDailyTable.GetBuyActionCoinCount() + 1);
	if (iterCost == CLogicConfig::GetEntireWarConfig().m_stBuyActionCoinCost.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_HAS_MAX);
	}

	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, iterCost->second, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	m_pUserInfo->m_stUserDailyTable.AddBuyActionCoinCount(1);
	CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);

	m_pUserInfo->m_stUserInfoTable.AddActionCoin(1);

	stRspBody.m_iActionCoin = m_pUserInfo->m_stUserInfoTable.GetActionCoin();
	stRspBody.m_iAddActionTime = m_pUserInfo->m_stUserInfoTable.GetLastAddActionTime();

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return (true);
}

bool CLogicEntireWarProcessor::DoUserSimpleGameEntireWar(const msgpack::unpacked& stFormatData)
{
    CRequestEntireWarSimpleGame stReq;
    CResponseEntireWarSimpleGame stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    const auto pThemeConfig = CLogicConfig::GetEntireWarMapConfig().m_stThemeMap.find(CLogicGlobal::GetEntireWarThemeID());
    if(pThemeConfig == CLogicConfig::GetEntireWarMapConfig().m_stThemeMap.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    const auto pConfig = pThemeConfig->second.m_stWarMap.find(stReq.m_iWarID);
    if (pConfig == pThemeConfig->second.m_stWarMap.end() || pConfig->second.m_iNodeType != CLogicConfigDefine::EEWNT_SimpleGame || stReq.m_iScore <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if (!CLogicGlobal::IsEntireWarPass(pConfig->second.m_iPreWarID))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PRE_WAR_NOT_PASS);
    }

    if (m_pUserInfo->m_stUserDailyTable.GetEntireWarDailyGameTimes() >= pThemeConfig->second.m_iSimpleGameTimes)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::REACH_ENTER_LEVEL_TIMES_PER_DAY_LIMIT);
    }

    auto stData = m_pUserInfo->m_stUserInfoTable.GetEntireWarNodePara();
    if(stData[stReq.m_iWarID] < stReq.m_iScore)
    {
        stData[stReq.m_iWarID] = stReq.m_iScore;
        CLogicGlobal::UpdateOrInsertCommonRank(pConfig->second.m_iNodePara2, m_iUin, m_iGroupID, stReq.m_iScore);

        m_pUserInfo->m_stUserInfoTable.SetEntireWarNodePara(stData);
    }

    m_pUserInfo->m_stUserDailyTable.AddEntireWarDailyGameTimes(1);

    stRsp.m_iWarID = stReq.m_iWarID;
    stRsp.m_iScore = stReq.m_iScore;
    stRsp.m_iMaxScore = stData[stReq.m_iWarID];
    stRsp.m_iSimpleGameDailyTimes = m_pUserInfo->m_stUserDailyTable.GetEntireWarDailyGameTimes();

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return (true);
}

bool CLogicEntireWarProcessor::DoUserRunLimitBossLevelBegin(const msgpack::unpacked& stFormatData)
{
    CRequestLimitBossLevelBegin stReq;
    CResponseLimitBossLevelBegin stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(stReq.m_iOwnerUin);
    if(nullptr == pBossData)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_EXPIRED);
    }

    if(pBossData->m_iDeathTime)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_DEATHING);
    }

    const auto& stConfig = CLogicConfig::GetEntireWarLimitBossConfig();
    if(m_pUserInfo->m_stUserDailyTable.GetLimitBossAction() + stConfig.m_iFightConsumeCoin > stConfig.m_iBossActionCoin)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ACTION_COIN);
    }

    int32_t iLevelID = stConfig.GetLevelID(CLogicGlobal::GetEntireWarThemeID(), pBossData->m_cHard);
    if(iLevelID <= 0 || iLevelID != stReq.m_iLevelID)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FIGHT_WAR_LEVEL_ERROR);
    }

    m_pUserInfo->m_stUserInfoTable.SetFightLevelID(iLevelID);

    stRsp.m_iOwnerUin = stReq.m_iOwnerUin;
    stRsp.m_iLevelID = iLevelID;
    stRsp.m_ulMaxHp = pBossData->m_ulMaxHp;
    stRsp.m_ulCurHurt = pBossData->m_ulCurHurt;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return (true);
}

bool CLogicEntireWarProcessor::DoUserRunLimitBossLevelResult(const msgpack::unpacked& stFormatData)
{
    CRequestLimitBossLevelResult stReq;
    CResponseLimitBossLevelResult stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    if(stReq.m_iHurt <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(stReq.m_iOwnerUin);
    if(nullptr == pBossData)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_EXPIRED);
    }

    const auto& stConfig = CLogicConfig::GetEntireWarLimitBossConfig();
    auto pHardConfig = stConfig.GetHardConfig(pBossData->m_cHard);
    if(nullptr == pHardConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    if(m_pUserInfo->m_stUserDailyTable.GetLimitBossAction() + stConfig.m_iFightConsumeCoin > stConfig.m_iBossActionCoin)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ACTION_COIN);
    }

    int32_t iLevelID = stConfig.GetLevelID(CLogicGlobal::GetEntireWarThemeID(), pBossData->m_cHard);
    if(iLevelID <= 0 || iLevelID != stReq.m_iLevelID || iLevelID != m_pUserInfo->m_stUserInfoTable.GetFightLevelID())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FIGHT_WAR_LEVEL_ERROR);
    }

    CLogicBill::SaveAntiScoreBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, iLevelID
        , stReq.m_iHurt, 0, m_pUserInfo->m_stUserInfoTable.GetRoleCombat()
        , m_pUserInfo->m_stUserTeamInfoTable.GetTeam1(), "");

    m_pUserInfo->m_stUserDailyTable.AddLimitBossAction(stConfig.m_iFightConsumeCoin);
    m_pUserInfo->m_stUserInfoTable.SetFightLevelID(0);

    if((uint64_t)stReq.m_iHurt > pBossData->m_ulMaxHp) stReq.m_iHurt = pBossData->m_ulMaxHp;

    if(pBossData->m_iDeathTime == 0)
    {
        CLogicManagerLimitBoss::SetBossHurt(stReq.m_iOwnerUin, stReq.m_iHurt, m_iUin, m_iGroupID);
    }

    // 奖励
    double dHpPrecent = stReq.m_iHurt * 1.0 / pBossData->m_ulMaxHp;
    float fRewardPro = stConfig.GetHurtRewardPro(dHpPrecent * 100) / 100.0;

    for(auto& reward : stConfig.m_stBossFightReward)
    {
        stRsp.m_stReward.emplace_back(reward.m_iItemType, reward.m_iCardID, std::ceil(reward.m_iNum * fRewardPro));
    }

    for(auto& bag : pHardConfig->m_stBagVec)
    {
        CLogicConfig::GetCardbagConfig().GetRandBonusByName(bag, stRsp.m_stReward);
    }

    stRsp.m_stBossInfo = CLogicManagerLimitBoss::GetPackBossInfo(*pBossData, m_iUin);
    stRsp.m_iScore = stConfig.m_iBaseScore * pHardConfig->m_iHardScore * dHpPrecent;
    stRsp.m_iHurt = stReq.m_iHurt;

    static std::string strBonus;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stReward, strBonus, m_usCmd);

    // 更新排行榜
    m_pUserInfo->m_stUserInfoTable.AddWeekLimitBossScore(stRsp.m_iScore);
    CLogicGlobal::UpdateOrInsertCommonRank(stConfig.m_iScoreRankID, m_iUin, m_iGroupID, m_pUserInfo->m_stUserInfoTable.GetWeekLimitBossScore());

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << iLevelID << "|BONUS:" << strBonus << std::endl;

    return (true);
}

bool CLogicEntireWarProcessor::DoUserRunLimitBossInvite(const msgpack::unpacked& stFormatData)
{
    CRequestLimitBossInvite stReq;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(m_iUin);
    if(nullptr == pBossData)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_EXPIRED);
    }

    if(pBossData->m_iDeathTime)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_DEATHING);
    }

    // 如果是主动公开BOSS
    if(stReq.m_bOpen)
    {
        CLogicManagerLimitBoss::SetBossOpen(m_iUin);

        SendToClient(SERVER_ERRCODE::SUCCESS);
        return (true);
    }

    if(stReq.m_stInviteList.empty())
    {
        SendToClient(SERVER_ERRCODE::SUCCESS);
        return (true);
    }

    if(int(pBossData->m_stAttackerList.size()) >= CLogicConfig::GetEntireWarLimitBossConfig().m_iAttackerNum)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_ATTACKER_LIMIT);
    }

    int32_t iIndex = 0;
    for(auto& iter : stReq.m_stInviteList)
    {
        if(iter.first == m_iUin) continue;

        auto pFriendInfo = CLogicCacheCommon::GetInCacheUserData(iter.second, iter.first);
        if (pFriendInfo)
        {
            if(pFriendInfo->m_stOnlyInCacheData.m_stLimitBossInvite.size() >= 30)
                pFriendInfo->m_stOnlyInCacheData.m_stLimitBossInvite.erase(pFriendInfo->m_stOnlyInCacheData.m_stLimitBossInvite.begin());

            pFriendInfo->m_stOnlyInCacheData.m_stLimitBossInvite.emplace(m_iUin);

            // 发送通知
            if(CLogicUser::GetInstance().IsUserOnline(iter.first, iter.second))
            {
                CResponseBody notify;
                GetConnectorSender()->NotifyClientUser(iter.second, iter.first, MSG_LOGIC_NOTIFY_LIMIT_BOSS_INVITE, notify);
            }
        }

        if(++iIndex > 100) break;
    }

    SendToClient(SERVER_ERRCODE::SUCCESS);
    return (true);
}

bool CLogicEntireWarProcessor::DoUserRunLimitBossOpenInviteList(const msgpack::unpacked& stFormatData)
{
    CResponseLimitBossOpenInviteList stRsp;

    // 检查自己接受的BOSS
    bool bSave = false;
    auto stAcptList = m_pUserInfo->m_stIBusyData.GetLimitBossAccepted();
    for(auto iterApt = stAcptList.begin(); iterApt != stAcptList.end(); )
    {
        auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(iterApt->first);
        if(nullptr == pBossData || pBossData->m_iEndTime != iterApt->second)
        {
            bSave = true;
            iterApt = stAcptList.erase(iterApt);
            continue;
        }

        ++iterApt;
    }

    if(bSave) m_pUserInfo->m_stIBusyData.SetLimitBossAccepted(stAcptList);

    auto& stInviteList = m_pUserInfo->m_stOnlyInCacheData.m_stLimitBossInvite;
    for(auto iter = stInviteList.begin(); iter != stInviteList.end(); )
    {
        auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(*iter);
        if(nullptr == pBossData || pBossData->m_iDeathTime)
        {
            iter = stInviteList.erase(iter);
            continue;
        }

        stRsp.m_stInviteBossList.emplace_back(CLogicManagerLimitBoss::GetPackBossInfo(*pBossData, m_iUin));

        ++iter;
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return (true);
}

bool CLogicEntireWarProcessor::DoUserRunLimitBossAccept(const msgpack::unpacked& stFormatData)
{
    CRequestLimitBossAccept stReq;
    CResponseLimitBossAccept stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    if(stReq.m_iOwnerUin == m_iUin)
    {
        m_pUserInfo->m_stOnlyInCacheData.m_stLimitBossInvite.erase(stReq.m_iOwnerUin);

        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if(stReq.m_iAcceptType == EAPT_Refuse)
    {
        m_pUserInfo->m_stOnlyInCacheData.m_stLimitBossInvite.erase(stReq.m_iOwnerUin);
    }
    else if(stReq.m_iAcceptType == EAPT_OneKeyRefuse)
    {
        m_pUserInfo->m_stOnlyInCacheData.m_stLimitBossInvite.clear();
    }
    else if(stReq.m_iAcceptType == EAPT_Accept)
    {
        m_pUserInfo->m_stOnlyInCacheData.m_stLimitBossInvite.erase(stReq.m_iOwnerUin);
        auto stAcceptData = m_pUserInfo->m_stIBusyData.GetLimitBossAccepted();
        if(int(stAcceptData.size()) >= CLogicConfig::GetEntireWarLimitBossConfig().m_iAcceptBossNum)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_ACCEPT_LIMIT);
        }

        auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(stReq.m_iOwnerUin);
        if(nullptr == pBossData || pBossData->m_iDeathTime)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_EXPIRED);
        }

        auto stData = pBossData->m_stAttackerList;
        if(stData.find(m_iUin) == stData.end())
        {
            // 检查BOSS攻击者是否达上限
            if(int(stData.size()) >= CLogicConfig::GetEntireWarLimitBossConfig().m_iAttackerNum)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_ATTACKER_LIMIT);
            }

            stAcceptData[stReq.m_iOwnerUin] = pBossData->m_iEndTime;
            m_pUserInfo->m_stIBusyData.SetLimitBossAccepted(stAcceptData);

            stData[m_iUin] = m_iGroupID;
            CLogicManagerLimitBoss::SetBossAttackerList(stReq.m_iOwnerUin, stData);
        }
    }
    else if(stReq.m_iAcceptType == EAPT_OneKeyAccept)
    {
        auto stAcceptData = m_pUserInfo->m_stIBusyData.GetLimitBossAccepted();
        for(auto& id : m_pUserInfo->m_stOnlyInCacheData.m_stLimitBossInvite)
        {
            if(int(stAcceptData.size()) >= CLogicConfig::GetEntireWarLimitBossConfig().m_iAcceptBossNum)
            {
                break;
            }

            auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(id);
            if(nullptr == pBossData || pBossData->m_iDeathTime)
            {
                continue;
            }

            auto stData = pBossData->m_stAttackerList;
            if(stData.find(m_iUin) == stData.end())
            {
                // 检查BOSS攻击者是否达上限
                if(int(stData.size()) >= CLogicConfig::GetEntireWarLimitBossConfig().m_iAttackerNum)
                {
                    continue;
                }

                stAcceptData[id] = pBossData->m_iEndTime;

                stData[m_iUin] = m_iGroupID;
                CLogicManagerLimitBoss::SetBossAttackerList(id, stData);
            }
        }

        m_pUserInfo->m_stIBusyData.SetLimitBossAccepted(stAcceptData);
        m_pUserInfo->m_stOnlyInCacheData.m_stLimitBossInvite.clear();
    }
    else
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    stRsp.m_iAcceptType = stReq.m_iAcceptType;
    stRsp.m_iOwnerUin = stReq.m_iOwnerUin;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return (true);
}

bool CLogicEntireWarProcessor::DoUserRunLimitBossOpenList(const msgpack::unpacked& stFormatData)
{
    CResponseLimitBossOpenBossList stRsp;

    std::set<int32_t> stExclude;
    // 自己的BOSS
    auto pMyBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(m_iUin);
    if(nullptr != pMyBossData)
    {
        stRsp.m_stMyBossInfo = CLogicManagerLimitBoss::GetPackBossInfo(*pMyBossData, m_iUin);
        stExclude.emplace(m_iUin);
    }

    // 检查自己接受的BOSS
    auto& stAcptList = m_pUserInfo->m_stIBusyData.GetLimitBossAccepted();
    for(auto iterApt = stAcptList.begin(); iterApt != stAcptList.end(); ++iterApt)
    {
        auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(iterApt->first);
        if(nullptr == pBossData || pBossData->m_iEndTime != iterApt->second)
            continue;

        stRsp.m_stBossList.emplace_back(CLogicManagerLimitBoss::GetPackBossInfo(*pBossData, m_iUin));
        stExclude.emplace(iterApt->first);
    }

    // 随机到的公开BOSS
    auto& stRandList = m_pUserInfo->m_stIBusyData.GetLimitBossRand();
    for(auto iter = stRandList.begin(); iter != stRandList.end(); ++iter)
    {
        auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(iter->first);
        if(nullptr == pBossData || pBossData->m_iEndTime != iter->second)
            continue;

        stRsp.m_stRandBossList.emplace_back(CLogicManagerLimitBoss::GetPackBossInfo(*pBossData, m_iUin));
    }

    // 如果公开列表为空，随机几个
    if(stRsp.m_stRandBossList.empty())
    {
        CLogicManagerLimitBoss::GetSomeRandBoss(stRsp.m_stRandBossList, stExclude, CLogicConfig::GetEntireWarLimitBossConfig().m_iRandBossNum, m_iUin);
        std::map<int32_t, int32_t> stNewRand;
        for(auto& stBoss : stRsp.m_stRandBossList)
        {
            stNewRand[stBoss.m_iOwnerUin] = stBoss.m_iEndTime;
        }

        m_pUserInfo->m_stIBusyData.SetLimitBossRand(stNewRand);
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return (true);
}

bool CLogicEntireWarProcessor::DoUserRunLimitBossGetBox(const msgpack::unpacked& stFormatData)
{
    CResponseLimitBossGetBox stRsp;

    if(m_pUserInfo->m_stIBusyData.GetLimitBoxTime() <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOX_EXPIRED);
    }

    stRsp.m_stReward = CLogicConfig::GetRewardBagConfig().GetReward(CLogicConfig::GetEntireWarLimitBossConfig().m_strBoxRewardBagID,
                                                                    m_pUserInfo->m_stUserInfoTable.GetLevel(), 0);

    static std::string strBonus;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stReward, strBonus, m_usCmd);

    m_pUserInfo->m_stIBusyData.SetLimitBoxTime(0);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS |ITEMS:" << strBonus << std::endl;

    return (true);
}

bool CLogicEntireWarProcessor::DoUserRunLimitBossOpenFriendList(const msgpack::unpacked& stFormatData)
{
    CRequestLimitBossOpenFriendList stReq;
    CResponseLimitBossOpenFirendList stRsp;

    if (!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    stRsp.m_bFriend = stReq.m_bFriend;

    auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(m_iUin);
    if(pBossData == nullptr || pBossData->m_iDeathTime)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LIMIT_BOSS_EXPIRED);
    }

    // 好友
    if(stReq.m_bFriend)
    {
        /*
        for (auto iter = m_pUserInfo->m_stFriendMap.Begin(); iter != m_pUserInfo->m_stFriendMap.End(); ++iter)
        {
            if(CLogicUser::GetInstance().IsUserOnline(iter->second->m_stData.m_iFriendUid, iter->second->m_stData.m_iFriendGroupID))
            {
                if(pBossData->m_stAttackerList.find(iter->second->m_stData.m_iFriendUid) == pBossData->m_stAttackerList.end())
                {
                    stRsp.m_stFriendList.push_back(CLogicFriend::GetFriendInfoPack(m_pUserInfo, iter->second->m_stData.m_iFriendUid, iter->second->m_stData.m_iFriendGroupID));
                }
            }
        }
         */
    }
    else
    {
        /*
        auto* pstGuild = CLogicGuild::GetGuild(m_pUserInfo);
        if (pstGuild)
        {
            const auto stGuildMemberKeyList = pstGuild->GetGuildMemberKeyList();
            for (const auto& stMemberKey : stGuildMemberKeyList)
            {
                if(stMemberKey.m_stRoleIDDetail.m_iUin == m_iUin)
                    continue;

                if(!CLogicUser::GetInstance().IsUserOnline(stMemberKey.m_stRoleIDDetail.m_iUin, stMemberKey.m_stRoleIDDetail.m_iGroupID))
                    continue;

                if(pBossData->m_stAttackerList.find(stMemberKey.m_stRoleIDDetail.m_iUin) != pBossData->m_stAttackerList.end())
                    continue;

                const auto pDstUser = CLogicCacheCommon::GetInCacheUserData(stMemberKey.m_stRoleIDDetail.m_iGroupID, stMemberKey.m_stRoleIDDetail.m_iUin);
                if(pDstUser == nullptr)
                    continue;

                CPackFriendInfo stUser;
                stUser.m_iUid = pDstUser->m_iUin;
                stUser.m_iGroupID = pDstUser->m_iGroupID;
                stUser.m_strNick = pDstUser->m_strNick;
                stUser.m_iLevel = pDstUser->m_stUserInfoTable.GetLevel();
                stUser.m_iVipLevel = pDstUser->m_stUserInfoTable.GetVipLevel();
                stUser.m_iAvatarID = pDstUser->m_stUserInfoTable.GetAvatarID();
                stUser.m_iAvatarBorder = pDstUser->m_stUserInfoTable.GetAvatarBorder();
                stUser.m_iFightPower = pDstUser->m_stUserInfoTable.GetRoleCombat();
                stUser.m_bIsOnline = true;

                stRsp.m_stFriendList.emplace_back(std::move(stUser));
            }
        }
        */
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return (true);
}

bool CLogicEntireWarProcessor::DoUserRunLimitBossRefreshOpenBoss(const msgpack::unpacked& stFormatData)
{
    CResponseLimitBossRefreshOpenBoss stRsp;

    if(CLogicCommon::GetSec() - m_pUserInfo->m_stOnlyInCacheData.m_iLimitBossLastRefreshOpenTime < 5)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::REFRESH_TIMELIMIT);
    }

    m_pUserInfo->m_stOnlyInCacheData.m_iLimitBossLastRefreshOpenTime = CLogicCommon::GetSec();

    std::set<int32_t> stExclude;
    // 自己的BOSS
    stExclude.emplace(m_iUin);

    // 检查自己接受的BOSS
    auto& stAcptList = m_pUserInfo->m_stIBusyData.GetLimitBossAccepted();
    for(auto iterApt = stAcptList.begin(); iterApt != stAcptList.end(); ++iterApt)
    {
        auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(iterApt->first);
        if(nullptr == pBossData || pBossData->m_iEndTime != iterApt->second)
            continue;

        stExclude.emplace(iterApt->first);
    }

    // 随机
    CLogicManagerLimitBoss::GetSomeRandBoss(stRsp.m_stRandBossList, stExclude, CLogicConfig::GetEntireWarLimitBossConfig().m_iRandBossNum, m_iUin);
    std::map<int32_t, int32_t> stNewRand;
    for(auto& stBoss : stRsp.m_stRandBossList)
    {
        stNewRand[stBoss.m_iOwnerUin] = stBoss.m_iEndTime;
    }

    m_pUserInfo->m_stIBusyData.SetLimitBossRand(stNewRand);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return (true);
}

void CLogicEntireWarProcessor::RefreshSelfEntireWar()
{
    const auto activity_time = CLogicGlobal::GetEntireWarActivityTime();
    const time_t this_time = CLogicCommon::GetSec();
    const time_t last_time = m_pUserInfo->m_stUserInfoTable.GetLastEntireWarTime();
    if (this_time >= activity_time.first && this_time < activity_time.second)
    {
        if (last_time < activity_time.first || last_time >= activity_time.second)
        {
            m_pUserInfo->m_stUserInfoTable.SetLastEntireWarTime(this_time);
            m_pUserInfo->m_stUserInfoTable.SetWeekWarContribution(0);
            m_pUserInfo->m_stUserInfoTable.SetWeekLimitBossScore(0);
            m_pUserInfo->m_stUserInfoTable.SetEntireWarAwardBitmap(0);
            m_pUserInfo->m_stUserInfoTable.SetEntireWarNodeAwardStat({});
            m_pUserInfo->m_stUserInfoTable.SetEntireWarExtraStatus({});
            m_pUserInfo->m_stUserInfoTable.SetEntireWarNodePara({});
        }
    }
}

void CLogicEntireWarProcessor::AutoAddActionCoin()
{
    const auto activity_time = CLogicGlobal::GetEntireWarActivityTime();
    const time_t this_time = CLogicCommon::GetSec();
    const auto& config = CLogicConfig::GetEntireWarConfig();
    time_t add_time = m_pUserInfo->m_stUserInfoTable.GetLastAddActionTime();
    int32_t action_coin = m_pUserInfo->m_stUserInfoTable.GetActionCoin();

    if (this_time >= activity_time.first && this_time < activity_time.second)
    {
        if (add_time < activity_time.first || add_time >= activity_time.second)
        {
            action_coin = add_time > 0 ? config.m_iMaxActionCoin : config.m_iInitActionCoin;
            add_time = activity_time.first;
        }

        if (action_coin < config.m_iMaxActionCoin && config.m_iActionCoinInterval > 0)
        {
            action_coin = std::min (action_coin + int32_t(this_time - add_time) / config.m_iActionCoinInterval, config.m_iMaxActionCoin);
            add_time = add_time + (this_time - add_time) / config.m_iActionCoinInterval * config.m_iActionCoinInterval;
        }

        if (action_coin >= config.m_iMaxActionCoin && this_time - add_time > 5)
        {
            add_time = this_time;
        }
    }

    if (add_time != m_pUserInfo->m_stUserInfoTable.GetLastAddActionTime() || action_coin != m_pUserInfo->m_stUserInfoTable.GetActionCoin())
    {
        m_pUserInfo->m_stUserInfoTable.SetActionCoin(action_coin);
        m_pUserInfo->m_stUserInfoTable.SetLastAddActionTime(add_time);
    }
}

bool CLogicEntireWarProcessor::IsUserEntireWarTreasureDigOut(int32_t iWarID)
{
    const auto& extra = m_pUserInfo->m_stUserInfoTable.GetEntireWarExtraStatus();
    const auto iterExtra = extra.m_stStatus.find(iWarID);
    if (iterExtra == extra.m_stStatus.end())
    {
        return false;
    }

    const int32_t iTotalDigCount = CLogicConfig::GetEntireWarConfig().m_iDefaultTreasureCount;
    int32_t iHaveDigCount = 0;
    for (int32_t i = 1; i <= CLogicConfigDefine::ENTIRE_WAR_TREASURE_COUNT; ++i)
    {
        if(i < int32_t(iterExtra->second.size()) && iterExtra->second[i])
            ++iHaveDigCount;
    }

    return iHaveDigCount >= iTotalDigCount;
}

bool CLogicEntireWarProcessor::IsUserEntireWarExtraAward(int32_t iWarID, int32_t iExtraID)
{
    const auto& extra = m_pUserInfo->m_stUserInfoTable.GetEntireWarExtraStatus();
    const auto iterExtra = extra.m_stStatus.find(iWarID);
    if (iterExtra == extra.m_stStatus.end() || iExtraID >= int32_t(iterExtra->second.size()))
    {
        return false;
    }
    return iterExtra->second[iExtraID];
}

void CLogicEntireWarProcessor::SetUserEntireWarExtraAward(int32_t iWarID, int32_t iExtraID)
{
    auto extra = m_pUserInfo->m_stUserInfoTable.GetEntireWarExtraStatus();
    auto& status = extra.m_stStatus[iWarID];
    if (iExtraID >= int32_t(status.size()))
    {
        status.resize(iExtraID + 1);
    }
    status[iExtraID] = true;
    m_pUserInfo->m_stUserInfoTable.SetEntireWarExtraStatus(extra);
}

int32_t CLogicEntireWarProcessor::GetNewUserEntireWarQuestion(int32_t iWarID)
{
    const auto& config = CLogicConfig::GetQuestionConfig().m_stQuestionVec;
    if (config.empty())
    {
        return 0;
    }

    const int32_t rand = CLogicCommon::GetRandNum() % config.size();
    const int32_t id = config[rand];

    auto questions = m_pUserInfo->m_stUserInfoTable.GetEntireWarNodePara();
    questions[iWarID] = id;
    m_pUserInfo->m_stUserInfoTable.SetEntireWarNodePara(questions);
    return id;
}

bool CLogicEntireWarProcessor::CheckUserEntireWarTeamMember(int32_t iWarID, const std::vector<int32_t>& iTeamVec)
{
    const auto* pstWarConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), iWarID);
    if (nullptr == pstWarConfig)
    {
        return false;
    }

    std::vector<int32_t> iValidTeamVec;
    for (const auto& id : iTeamVec)
    {
        if (id > 0)
        {
            iValidTeamVec.push_back(id);
        }
    }

    if (iValidTeamVec.empty())
    {
        return false;
    }

    for (const auto& limit : pstWarConfig->m_stLevelLimitVec)
    {
        const bool ret = CheckUserEntireWarTeamMember(limit.first, limit.second, iValidTeamVec);
        if (false == ret)
        {
            return false;
        }
    }

    return true;
}

bool CLogicEntireWarProcessor::CheckUserEntireWarTeamMember(int32_t iType, int32_t iValue, const std::vector<int32_t>& iValidTeamVec)
{
    if (iType == CLogicConfigDefine::ENTIRE_WAR_LIMIT_NULL)
    {
        return true;
    }
    else if (iType == CLogicConfigDefine::ENTIRE_WAR_LIMIT_TEAN_MEMBER)
    {
        return (int32_t(iValidTeamVec.size()) <= iValue);
    }
    else if (iType == CLogicConfigDefine::ENTIRE_WAR_LIMIT_HERO_TYPE)
    {
        char szValue[10] = { 0 };
        snprintf(szValue, sizeof szValue, "%04d", iValue);
        for (const auto& id : iValidTeamVec)
        {
            const auto* pstConfig = CLogicConfig::GetHeroCardConfig().GetConfig(id);
            if (nullptr == pstConfig)
            {
                return false;
            }

            if (pstConfig->m_iCardType <= 0 || pstConfig->m_iCardType >= int32_t(sizeof szValue) || szValue[pstConfig->m_iCardType] != '1')
            {
                return false;
            }
        }
    }
    else if (iType == CLogicConfigDefine::ENTIRE_WAR_LIMIT_HERO_ID)
    {
        if (CLogicCacheCommon::IsMainRoleCardID(iValue))
        {
            return (std::find(iValidTeamVec.begin(), iValidTeamVec.end(), CLogicConfigDefine::MALE_LEADING_HERO_ID) != iValidTeamVec.end())
                   || (std::find(iValidTeamVec.begin(), iValidTeamVec.end(), CLogicConfigDefine::FEMALE_LEADING_HERO_ID) != iValidTeamVec.end());
        }

        return (std::find(iValidTeamVec.begin(), iValidTeamVec.end(), iValue) != iValidTeamVec.end());
    }
    else if (iType == CLogicConfigDefine::ENTIRE_WAR_LIMIT_HERO_SEX)
    {
        for (const auto& id : iValidTeamVec)
        {
            const auto* pstConfig = CLogicConfig::GetHeroCardConfig().GetConfig(id);
            if (nullptr == pstConfig)
            {
                return false;
            }

            if (pstConfig->m_iSexual != iValue)
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

CPackEntireWarInfo CLogicEntireWarProcessor::GetUserEntireWarInfoPack()
{
    CPackEntireWarInfo stInfo;
    stInfo.m_iActionCoin = m_pUserInfo->m_stUserInfoTable.GetActionCoin();
    stInfo.m_iAddActionTime = m_pUserInfo->m_stUserInfoTable.GetLastAddActionTime();
    stInfo.m_iWarLevel = CLogicGlobal::GetEntireWarLevel();
    stInfo.m_strAwardBitmap = int2bin(m_pUserInfo->m_stUserInfoTable.GetEntireWarAwardBitmap());
    stInfo.m_iWarContribution = m_pUserInfo->m_stUserInfoTable.GetEntireWarContribution();
    stInfo.m_iWeekWarContribution = m_pUserInfo->m_stUserInfoTable.GetWeekWarContribution();
    stInfo.m_stBuffVec = CLogicGlobal::GetEntireWarBuff();
    stInfo.m_iThemeID = CLogicGlobal::GetEntireWarThemeID();
    stInfo.m_iSimpleGameDailyTimes = m_pUserInfo->m_stUserDailyTable.GetEntireWarDailyGameTimes();
    stInfo.m_iLimitBossAction = m_pUserInfo->m_stUserDailyTable.GetLimitBossAction();
    stInfo.m_iLimitBoxTime = m_pUserInfo->m_stIBusyData.GetLimitBoxTime();

    return stInfo;
}

CPackEntireWarMap CLogicEntireWarProcessor::GetUserEntireWarMapPack(int32_t iWarID)
{
    CPackEntireWarMap stMap;
    const auto* pstWarValue = CLogicGlobal::GetEntireWarMapInfo(iWarID);
    if (pstWarValue)
    {
        stMap.m_iWarID = iWarID;
        stMap.m_ulTargetProgress = pstWarValue->m_ulTargetProgress;
        stMap.m_ulCurProgress = pstWarValue->m_ulCurProgress;
        stMap.m_bIsAward = CLogicCacheCommon::VecBitmapCheckState(m_pUserInfo->m_stUserInfoTable.GetEntireWarNodeAwardStat(), iWarID)
            + IsUserEntireWarTreasureDigOut(iWarID);
    }

    return stMap;
}

std::vector<CPackMarket> CLogicEntireWarProcessor::GetUserEntireWarMarketPack(int32_t iWarID)
{
    auto stMarketVec = CLogicGlobal::GetEntireWarMarketPack(iWarID);
    for (size_t i = 0 ; i < stMarketVec.size(); ++i)
    {
        stMarketVec[i].m_bCanBuy = !IsUserEntireWarExtraAward(iWarID, i + 1);
    }
    return stMarketVec;
}

//加日志
void CLogicEntireWarProcessor::AddReport(int32_t iWarID, const std::vector<int> &params)
{
	// 如果刚好完成了全民战役关卡，则开始加入日志
    const auto* pstWarValue = CLogicGlobal::GetEntireWarMapInfo(iWarID);
	if (!pstWarValue || !pstWarValue->m_iIsPassed)
		return;

	// 读取全民战役表，和战报表
	auto entireWarConf = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), iWarID);
	if (entireWarConf == nullptr)
		return;

	// 战报
	for (auto iter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("entirewar"); iter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("entirewar"); ++iter)
	{
		if (iter->second.m_iTriggerValue == entireWarConf->m_iIconType)
		{
            CPackFightReportItem reportItem;
            reportItem.m_iReportType = CLogicConfigDefine::FIGHT_REPORT_ENTIRE_WAR;
            reportItem.m_iReportID = CLogicCommon::GetSec();
            std::map<std::string, std::string> stParamMap;
            stParamMap["nick"] = m_pUserInfo->m_strNick;
            stParamMap["mapName"] = entireWarConf->m_strName;

            if(params.size() >= 2)
            {
                stParamMap["bufPercent"] = std::to_string(params[1]);
            }

            reportItem.m_strReportContent = iter->second.m_stContentObj.Replace(stParamMap);
            reportItem.m_cFightResult = 0;
            CLogicGlobal::AddGlobalFightReport(reportItem);

            break;
		}
	}

	// 解锁下一关战报
	for(auto& unlockID : entireWarConf->m_stUnlockList)
    {
        auto newEntireWarConf = CLogicConfig::GetEntireWarMapConfig().GetConfig(CLogicGlobal::GetEntireWarThemeID(), unlockID);
        if (newEntireWarConf == nullptr)
            continue;

        if(newEntireWarConf->m_iNodeType != CLogicConfigDefine::EEWNT_Market
            && newEntireWarConf->m_iNodeType != CLogicConfigDefine::EEWNT_Treasure
            && newEntireWarConf->m_iNodeType != CLogicConfigDefine::EEWNT_Creeps
            && newEntireWarConf->m_iNodeType != CLogicConfigDefine::EEWNT_SimpleGame)
            continue;

        for (auto iter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("entirewar"); iter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("entirewar"); ++iter)
        {
            if (iter->second.m_iTriggerValue == newEntireWarConf->m_iIconType)
            {
                CPackFightReportItem reportItem;
                reportItem.m_iReportType = CLogicConfigDefine::FIGHT_REPORT_ENTIRE_WAR;
                reportItem.m_iReportID = CLogicCommon::GetSec();
                std::map<std::string, std::string> stParamMap;
                stParamMap["nick"] = m_pUserInfo->m_strNick;
                stParamMap["mapName"] = entireWarConf->m_strName;
                stParamMap["newMapName"] = newEntireWarConf->m_strName;

                reportItem.m_strReportContent = iter->second.m_stContentObj.Replace(stParamMap);
                reportItem.m_cFightResult = 0;
                reportItem.m_stParams.push_back(unlockID);
                CLogicGlobal::AddGlobalFightReport(reportItem);

                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CLogicEntireWarProcessor::RandLimitBoss()
{
    const auto activity_time = CLogicGlobal::GetEntireWarActivityTime();
    const time_t this_time = CLogicCommon::GetSec();
    if (this_time < activity_time.first || this_time > activity_time.second)
    {
        return;
    }

    const auto& stConfig = CLogicConfig::GetEntireWarLimitBossConfig();

    // 最后2小时不再刷新出新BOSS
    if (activity_time.second - this_time <= stConfig.m_iLimitRefreshNewBossTime)
    {
        return;
    }

    auto pBossData = CLogicManagerLimitBoss::GetAndRefreshBossData(m_iUin);

    bool bCheckBoss = true;
    if(pBossData || CLogicManagerLimitBoss::GetBossNum() >= CLogicConfigEntireWarLimitBossParser::E_MaxBossNum)
    {
        bCheckBoss = false;
    }

    // 计算BOSS和宝箱的随机概率
    std::map<int32_t, int32_t> stWeight;
    int32_t iTotalWeight = 0;
    if(bCheckBoss && m_pUserInfo->m_stUserDailyTable.GetLimitBossDailyNum() < stConfig.m_iDailyBossTimes)
    {
        stWeight[EPRO_Boss] = stConfig.m_iBossRate;
        iTotalWeight += stConfig.m_iBossRate;
    }

    if(m_pUserInfo->m_stIBusyData.GetLimitBoxTime() == 0 && m_pUserInfo->m_stUserDailyTable.GetLimitBoxDailyNum() < stConfig.m_iDailyBoxTimes)
    {
        stWeight[EPRO_Box] = stConfig.m_iBoxRate;
        iTotalWeight += stConfig.m_iBoxRate;
    }

    if(stWeight.empty() || iTotalWeight == 0)
        return;

    stWeight[EPRO_Empty] = stConfig.m_iEmptyRate;
    iTotalWeight += stConfig.m_iEmptyRate;

    int32_t iRandWeight = CLogicCommon::GetRandNum() % iTotalWeight;
    int32_t iType = EPRO_Invalid;
    for(auto& iter : stWeight)
    {
        if(iRandWeight < iter.second)
        {
            iType = iter.first;
            break;
        }

        iRandWeight -= iter.second;
    }

    // 如果随机到BOSS
    if(iType == EPRO_Boss)
    {
        auto pHardConfig = stConfig.GetRandHardConfig(m_pUserInfo->m_stUserInfoTable.GetLevel());
        if(nullptr == pHardConfig)
        {
            PROCESSOR_LOG_ERROR << "Create Limit Boss rand hard Error!" << std::endl;
            return;
        }

        int32_t iLevelID = stConfig.GetLevelID(CLogicGlobal::GetEntireWarThemeID(), pHardConfig->m_iHard);
        if(iLevelID <= 0)
        {
            PROCESSOR_LOG_ERROR << "Create Limit Boss level_id Error! ThemeID:" << CLogicGlobal::GetEntireWarThemeID()
                                << "|Hard:" << pHardConfig->m_iHard << std::endl;
            return;
        }

        int32_t iEndTime = this_time + pHardConfig->m_iDurTime;
        if(iEndTime > activity_time.second - stConfig.m_iLastBossEndTime) iEndTime = activity_time.second - stConfig.m_iLastBossEndTime;

        TGlobalLimitBossTableValueType stMyBossData;
        stMyBossData.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
        stMyBossData.m_iOwnerUin = m_iUin;
        stMyBossData.m_iOwnerGroup = m_iGroupID;
        stMyBossData.m_strNick = m_pUserInfo->m_strNick;
        stMyBossData.m_cHard = pHardConfig->m_iHard;
        stMyBossData.m_cIsOpen = 0;
        stMyBossData.m_iEndTime = iEndTime;
        stMyBossData.m_iOpenTime = this_time + pHardConfig->m_iOpenTime;
        if(stMyBossData.m_iOpenTime > iEndTime) stMyBossData.m_iOpenTime = iEndTime;
        stMyBossData.m_iDeathTime = 0;
        stMyBossData.m_ulCurHurt = 0;

        // 计算BOSS血量
        stMyBossData.m_ulMaxHp = CLogicConfig::GetLevelConfig().GetLevelBossHp(iLevelID)
                                 * CLogicConfig::GetEntireWarConfig().GetWorldLevelRatio(CLogicGlobal::GetEntireWarLevel());
        if(stMyBossData.m_ulMaxHp <= 0)
        {
            PROCESSOR_LOG_ERROR << "Create Limit Boss HP Error! LevelID:" << iLevelID << std::endl;
            return;
        }

        if(false == CLogicManagerLimitBoss::AddBoss(stMyBossData))
        {
            PROCESSOR_LOG_ERROR << "Create Limit Boss Add Boss Error!" << std::endl;
            return;
        }

        m_pUserInfo->m_stUserDailyTable.AddLimitBossDailyNum(1);

        // 发送通知
        CNotifyNewLimitBoss notify;
        notify.m_stBossInfo = CLogicManagerLimitBoss::GetPackBossInfo(stMyBossData, m_iUin);
        GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, MSG_LOGIC_TOUHOU_NOTIFY_LIMIT_BOSS, notify);
    }
    else if(iType == EPRO_Box)
    {
        // 随机到宝箱
        m_pUserInfo->m_stIBusyData.SetLimitBoxTime(this_time + stConfig.m_iBoxDurTime);
        m_pUserInfo->m_stUserDailyTable.AddLimitBoxDailyNum(1);

        // 发送通知
        CNotifyNewLimitBox notify;
        notify.m_iLimitBoxTime = m_pUserInfo->m_stIBusyData.GetLimitBoxTime();
        GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, MSG_LOGIC_TOUHOU_NOTIFY_LIMIT_BOX, notify);
    }
}
