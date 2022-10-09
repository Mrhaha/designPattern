#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "server_cross_protocol.h"
#include "logic_game_cross_server_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_global.h"
#include "logic_game_active_info.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"

REGISTER_CMD_FACTORY(MSG_CROSS_FIGHT_MATCH_PVP, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_FIGHT_MATCH_PVP_RESULT, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NOTIFY_BE_MATCHED, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_BUY_MATCH_STORE_ITEM, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GET_MATCH_MAX_AWARD, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NOTIFY_CROSS_BROADCAST, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_REFRESH_FAIR_PVP_CARD, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_SELECT_FAIR_PVP_TEAM, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_FIGHT_FAIR_PVP, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_FIGHT_FAIR_PVP_RESULT, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NOTIFY_FAIR_MATCHED, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_LUCKY_HAMMER_LOTTERY, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GET_LUCKY_HAMMER_RANK, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GET_LUCKY_HAMMER_LOG, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_LUCKY_HAMMER_LOG_SCORE, CLogicCrossServerProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_NOTIFY_LUCKY_HAMMER_RANK_AWARD, CLogicCrossServerProcessor)


CLogicCrossServerProcessor::CLogicCrossServerProcessor(uint16_t usCmd, const std::string& strCmdName)
	: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicCrossServerProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	ReportMatchUserData();

	bool bRet = false;

	switch (m_usCmd)
	{
	case MSG_CROSS_FIGHT_MATCH_PVP:
		bRet = DoUserRunFightMatchPvp(stFormatData);
		break;

	case MSG_CROSS_FIGHT_MATCH_PVP_RESULT:
		bRet = DoUserRunFightMatchPvpResult(stFormatData);
		break;

	case MSG_CROSS_BUY_MATCH_STORE_ITEM:
		bRet = DoUserRunBuyMatchStoreItem(stFormatData);
		break;

	case MSG_CROSS_GET_MATCH_MAX_AWARD:
		bRet = DoUserRunGetMatchMaxAward(stFormatData);
		break;

	case MSG_CROSS_REFRESH_FAIR_PVP_CARD:
		bRet = DoUserRunRefreshFairPvpCard(stFormatData);
		break;

	case MSG_CROSS_SELECT_FAIR_PVP_TEAM:
		bRet = DoUserRunSelectFairPvpTeam(stFormatData);
		break;

	case MSG_CROSS_FIGHT_FAIR_PVP:
		bRet = DoUserRunFightFairPvp(stFormatData);
		break;

	case MSG_CROSS_FIGHT_FAIR_PVP_RESULT:
		bRet = DoUserRunFightFairPvpResult(stFormatData);
		break;

	case MSG_CROSS_LUCKY_HAMMER_LOTTERY:
		bRet = DoUserRunLuckyHammerLottery(stFormatData);
		break;

	case MSG_CROSS_GET_LUCKY_HAMMER_RANK:
		bRet = DoUserRunGetLuckyHammerRank(stFormatData);
		break;

	case MSG_CROSS_GET_LUCKY_HAMMER_LOG:
		bRet = DoUserRunGetLuckyHammerLog(stFormatData);
		break;

	case MSG_CROSS_LUCKY_HAMMER_LOG_SCORE:
		bRet = DoUserRunLuckyHammerLogScore(stFormatData);
		break;

	default:
		break;
	}

	return bRet;
}

bool CLogicCrossServerProcessor::DoUserRunFightMatchPvp(const msgpack::unpacked& stFormatData)
{
	if (abs(CLogicCommon::GetSec() - m_pUserInfo->m_stOnlyInCacheData.m_iLastStartMatchTime) <= 10)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HAVE_ALREADY_MATCH);
	}

	CLogicCacheCommon::ResultMatchPVPScore(m_pUserInfo, false);

	if (!GetCrossConnection()->IsConnected())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MATCH_SERVER_NOT_CONN);
	}

	CSRequestFightMatchPvp stServerReq;
	stServerReq.m_iMatchLevel = CLogicConfig::GetMatchPvpConfig().GetMatchLevelByScore(m_pUserInfo->m_stUserInfoTable.GetMatchScore());
	stServerReq.m_iHadMatchUin = m_pUserInfo->m_stOnlyInCacheData.m_stMatchPvpHadUins;
	stServerReq.m_iBaodiRobotDelta = 0;
	if (stServerReq.m_iMatchLevel >= CLogicConfig::GetMatchPvpConfig().m_iBaodiMatchLevel && m_pUserInfo->m_stOnlyInCacheData.m_iMatchBaodiFailCount >= CLogicConfig::GetMatchPvpConfig().m_iBaodiFailCount)
	{
		stServerReq.m_iBaodiRobotDelta = CLogicConfig::GetMatchPvpConfig().m_iBaodiMatchRobotDelta;
	}

	m_pUserInfo->m_stOnlyInCacheData.m_iLastStartMatchTime = CLogicCommon::GetSec();

	SendToCrossServer(enmCmdType_request, m_usCmd, stServerReq);

	return false;
}

bool CLogicCrossServerProcessor::DoUserRunFightMatchPvpResult(const msgpack::unpacked& stFormatData)
{
	CRequestFightMatchPvpResult stReqBody;
	CResponseFightMatchPvpResult stRspBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	if (0 >= m_pUserInfo->m_stOnlyInCacheData.m_stMatchPvpPlayer.m_stBriefInfo.m_iUid)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NO_MATCH_PVP_PLAYER);
	}

	stRspBody.m_bResult = stReqBody.m_bResult;
	stRspBody.m_iDeltaMatchScore = CLogicCacheCommon::ResultMatchPVPScore(m_pUserInfo, stRspBody.m_bResult);
	m_pUserInfo->m_stUserInfoTable.AddMatchDailyCount(1);
	CLogicUserNotifySystem::NotifyMatchPvpInfo(m_pUserInfo);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoUserRunBuyMatchStoreItem(const msgpack::unpacked& stFormatData)
{
	CRequestBuyMatchStoreItem stReqBody;
	CResponseBuyMatchStoreItem stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicConfig::GetMatchPvpConfig().GetMatchStoreConfig(stReqBody.m_iStoreID);
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::STORE_ITEM_NOT_IN_CONFIG)
	}

	if (CLogicConfig::GetMatchPvpConfig().GetMatchLevelByScore(m_pUserInfo->m_stUserInfoTable.GetMatchScore()) < pstConfig->m_iMatchLevelLimit)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MATCH_LEVEL_NOT_ENOUGH)
	}

	auto stStoreBuyCount = m_pUserInfo->m_stUserInfoTable.GetMatchStoreBuyCount();
	auto iterBuyCount = stStoreBuyCount.m_iBuyCountMap.find(stReqBody.m_iStoreID);
	if (iterBuyCount == stStoreBuyCount.m_iBuyCountMap.end())
	{
		const auto stRet = stStoreBuyCount.m_iBuyCountMap.emplace(stReqBody.m_iStoreID, 0);
		if (!stRet.second)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_HAS_MAX)
		}
		iterBuyCount = stRet.first;
	}

	if (pstConfig->m_iDailyBuyCount > 0 && iterBuyCount->second >= pstConfig->m_iDailyBuyCount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_HAS_MAX)
	}

	if (pstConfig->m_iTotalBuyCount > 0 && iterBuyCount->second >= pstConfig->m_iTotalBuyCount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_HAS_MAX)
	}

	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pstConfig->m_stConsumeItem, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	CLogicCacheCommon::AddGameItem(m_pUserInfo, pstConfig->m_stBuyItem, m_usCmd);

	++iterBuyCount->second;
	m_pUserInfo->m_stUserInfoTable.SetMatchStoreBuyCount(stStoreBuyCount);
	CLogicUserNotifySystem::NotifyMatchPvpInfo(m_pUserInfo);

	stRspBody.m_stBonusItem = pstConfig->m_stBuyItem;
	stRspBody.m_stConsumeItem = pstConfig->m_stConsumeItem;

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoUserRunGetMatchMaxAward(const msgpack::unpacked& stFormatData)
{
	CRequestGetMatchMaxAward stReqBody;
	CResponseGetMatchMaxAward stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const int32_t iMatchMaxLevel = CLogicConfig::GetMatchPvpConfig().GetMatchLevelByScore(m_pUserInfo->m_stUserInfoTable.GetMatchMaxScore());
	if (stReqBody.m_iIndex > iMatchMaxLevel || stReqBody.m_iIndex < 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MATCH_LEVEL_NOT_ENOUGH)
	}

	const auto* pstConfig = CLogicConfig::GetMatchPvpConfig().GetMaxAwardConfig(stReqBody.m_iIndex);
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MATCH_LEVEL_NOT_ENOUGH)
	}

	std::string strMatchMaxLevelAwardBitmap = m_pUserInfo->m_stUserInfoTable.GetMatchMaxLevelAwardBitmap();
	if (int32_t(strMatchMaxLevelAwardBitmap.size()) < stReqBody.m_iIndex + 1)
	{
		strMatchMaxLevelAwardBitmap.resize(stReqBody.m_iIndex + 1, '0');
	}

	if (strMatchMaxLevelAwardBitmap[stReqBody.m_iIndex] != '0')
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED)
	}

	strMatchMaxLevelAwardBitmap[stReqBody.m_iIndex] = '1';
	m_pUserInfo->m_stUserInfoTable.SetMatchMaxLevelAwardBitmap(strMatchMaxLevelAwardBitmap);
	CLogicUserNotifySystem::NotifyMatchPvpInfo(m_pUserInfo);

	CLogicCacheCommon::AddGameItem(m_pUserInfo, *pstConfig, m_usCmd);

	stRspBody.m_iIndex = stReqBody.m_iIndex;
	stRspBody.m_stBonusVec = *pstConfig;

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoUserRunRefreshFairPvpCard(const msgpack::unpacked& stFormatData)
{
	if (!CLogicConfig::GetFairPvpConfig().IsActivityOpen())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
	}

	if (!m_pUserInfo->m_stUserInfoTable.GetFairPvpTeamCard().empty())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HAVE_FAIR_TEAM);
	}

	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfig::GetFairPvpConfig().m_stRefreshItem, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	m_pUserInfo->m_stUserInfoTable.SetFairPvpStoreCard(CLogicGlobal::FairPvpRandomOptionCard(m_pUserInfo));
	CLogicUserNotifySystem::NotifyFairPvpInfo(m_pUserInfo);

	SendToClient(SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoUserRunSelectFairPvpTeam(const msgpack::unpacked& stFormatData)
{
	CRequestSelectFairPvpTeam stReqBody;
	CResponseSelectFairPvpTeam stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	if (!CLogicConfig::GetFairPvpConfig().IsActivityOpen())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
	}

	if (!m_pUserInfo->m_stUserInfoTable.GetFairPvpTeamCard().empty())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HAVE_FAIR_TEAM);
	}

	if (stReqBody.m_iTeamVec.size() != 5)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TEAM_CARD_NOT_ENOUGH);
	}

	const auto& stStoreVec = m_pUserInfo->m_stUserInfoTable.GetFairPvpStoreCard();
	for (auto iter = stReqBody.m_iTeamVec.begin(); iter != stReqBody.m_iTeamVec.end(); ++iter)
	{
		if (std::find(stReqBody.m_iTeamVec.begin(), iter, *iter) != iter)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TEAM_CARD_NOT_UNIQUE);
		}

		if (std::find(stStoreVec.begin(), stStoreVec.end(), *iter) == stStoreVec.end())
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TEAM_CARD_NOT_OPTION);
		}
	}

	int32_t iTotalQuality = 0;
	for (const auto& card_id : stReqBody.m_iTeamVec)
	{
		const auto* pstCard = CLogicConfig::GetHeroCardConfig().GetConfig(card_id);
		if (nullptr == pstCard)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
		}

		iTotalQuality += pstCard->m_iQuality;
	}

	if (iTotalQuality > CLogicConfig::GetFairPvpConfig().m_iTotalQuality)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TEAM_CARD_MAX_QUALITY);
	}

	stRspBody.m_iTeamVec = stReqBody.m_iTeamVec;
	m_pUserInfo->m_stUserInfoTable.SetFairPvpTeamCard(stRspBody.m_iTeamVec);
	CLogicUserNotifySystem::NotifyFairPvpInfo(m_pUserInfo);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoUserRunFightFairPvp(const msgpack::unpacked& stFormatData)
{
	CRequestFightFairPvp stReqBody;
	CResponseFightFairPvp stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	if (!CLogicConfig::GetFairPvpConfig().IsActivityOpen())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
	}

	if (abs(CLogicCommon::GetSec() - m_pUserInfo->m_stOnlyInCacheData.m_iLastStartFairTime) <= 10)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HAVE_ALREADY_MATCH);
	}

	CLogicCacheCommon::ResultFairPVPScore(m_pUserInfo, false);

	if (!GetCrossConnection()->IsConnected())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MATCH_SERVER_NOT_CONN);
	}

	if (m_pUserInfo->m_stUserInfoTable.GetFairPvpWinCount() >= CLogicConfig::GetFairPvpConfig().m_iTotalWinCount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FAIR_PVP_WIN_MAX);
	}

	if (m_pUserInfo->m_stUserInfoTable.GetFairPvpLoseCount() >= CLogicConfig::GetFairPvpConfig().m_iTotalFailCount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FAIR_PVP_LOSE_MAX);
	}

	if (m_pUserInfo->m_stUserInfoTable.GetFairPvpTeamCard().empty())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TEAM_CARD_NOT_ENOUGH);
	}

	if (m_pUserInfo->m_stUserInfoTable.GetFairPvpTeamCard().size() != stReqBody.m_iTeamVec.size())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TEAM_CARD_NOT_ENOUGH);
	}

	for (const auto& card_id : m_pUserInfo->m_stUserInfoTable.GetFairPvpTeamCard())
	{
		if (std::find(stReqBody.m_iTeamVec.begin(), stReqBody.m_iTeamVec.end(), card_id) == stReqBody.m_iTeamVec.end())
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TEAM_CARD_NOT_OPTION);
		}
	}

	const auto* pstWinConfig = CLogicConfig::GetFairPvpConfig().GetFairPvpWinConfig(m_pUserInfo->m_stUserInfoTable.GetFairPvpWinCount(), CLogicCommon::GetSec());
	if (nullptr == pstWinConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
	}

	m_pUserInfo->m_stUserInfoTable.SetFairPvpTeamCard(stReqBody.m_iTeamVec);

	m_pUserInfo->m_stOnlyInCacheData.m_iLastStartFairTime = CLogicCommon::GetSec();

	CSRequestFightFairPvp stCrossBody;
	stCrossBody.m_iTeamVec = stReqBody.m_iTeamVec;
	stCrossBody.m_iNotCardVec = pstWinConfig->m_iNotMatchCardVec;

	SendToCrossServer(enmCmdType_request, m_usCmd, stCrossBody);

	return true;
}

bool CLogicCrossServerProcessor::DoUserRunFightFairPvpResult(const msgpack::unpacked& stFormatData)
{
	CRequestFightFairPvpResult stReqBody;
	CResponseFightFairPvpResult stRspBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	if (!CLogicConfig::GetFairPvpConfig().IsActivityOpen())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
	}

	if (0 >= m_pUserInfo->m_stOnlyInCacheData.m_stFairPvpPlayer.m_stBriefInfo.m_iUid)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NO_MATCH_PVP_PLAYER);
	}

	stRspBody.m_bResult = stReqBody.m_bResult;
	CLogicCacheCommon::ResultFairPVPScore(m_pUserInfo, stRspBody.m_bResult);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoUserRunLuckyHammerLottery(const msgpack::unpacked & stFormatData)
{
	CResponseLuckyHammerLottery stRsp;
	CRequestLuckyHammerLottery stParam;

	if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

	if (stParam.m_iCount < 0 || stParam.m_iCount > 10)
	{
		using namespace SERVER_ERRCODE;
		SEND_ERRCODE_AND_RET(INTERN_ERROR)
	}

	const auto iterActivityRef = CLogicConfig::GetActivityConfig().m_stActivityRef.find(stParam.m_iActivityID);
	if (iterActivityRef == CLogicConfig::GetActivityConfig().m_stActivityRef.end())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << ACTIVE_IS_NOT_IN_CONFIG
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(ACTIVE_IS_NOT_IN_CONFIG)
			<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(ACTIVE_IS_NOT_IN_CONFIG)
	}

	const auto iterActivity = CLogicConfig::GetActivityConfig().m_stLuckyHammerConfig.find(stParam.m_iActivityID);
	if (iterActivity == CLogicConfig::GetActivityConfig().m_stLuckyHammerConfig.end())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << ACTIVE_IS_NOT_IN_CONFIG
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(ACTIVE_IS_NOT_IN_CONFIG)
			<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(ACTIVE_IS_NOT_IN_CONFIG)
	}

	const auto iterCommodity = CLogicConfig::GetActivityConfig().m_stActiveCommodity.find(stParam.m_iActivityID);
	if (iterCommodity == CLogicConfig::GetActivityConfig().m_stActiveCommodity.end())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << ACTIVE_IS_NOT_IN_CONFIG
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(ACTIVE_IS_NOT_IN_CONFIG)
			<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(ACTIVE_IS_NOT_IN_CONFIG)
	}

	auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(m_pUserInfo, stParam.m_iActivityID);
	if (!pActiveObjPtr)
	{
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
			<< "|MSG:" << CLogicError::GetErrMsg()
			<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

	int32_t iHasLotteryScore = 0;
	auto stIter = m_pUserInfo->m_stUserActiveMap.Find(stParam.m_iActivityID);
	if (stIter != m_pUserInfo->m_stUserActiveMap.End())
	{
		iHasLotteryScore = stIter->second->GetActiveTargetNum();
	}

	std::string strScoreMallBag = "";
	for (const auto& active : iterActivity->second)
	{
		if (iHasLotteryScore >= active.m_iCount)
		{
			strScoreMallBag = active.m_strBuyMallBag;
		}
	}

	const auto iLastActivityID = m_pUserInfo->m_stIBusyData.GetLuckyHammerActivityID();
	const auto iLastLotteryCount = m_pUserInfo->m_stIBusyData.GetLuckyHammerLotteryCount();
    const int32_t iHasLotteryCount = (iLastActivityID == stParam.m_iActivityID ? iLastLotteryCount : 0);

    stRsp.m_iCount = stParam.m_iCount;
	stRsp.m_iActivityID = stParam.m_iActivityID;
	stRsp.m_iLotteryIndex = stParam.m_iLotteryIndex;
	std::map<int32_t, std::vector<CPackGameItem>>stNotifyBonusMap;
	for (int32_t i = 1; i <= stParam.m_iCount; ++i)
	{
		const std::string strBagName = CLogicConfig::GetActivityConfig().GetActiveCommodityBuyMallBag(stParam.m_iActivityID, iHasLotteryCount + i);
		auto stTempVec = CLogicConfig::GetCardbagConfig().GetRandBonusByName(strBagName.empty() ? strScoreMallBag : strBagName);
		if (1 != stTempVec.size())
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BAD_COMMODITY_ID);
		}
		if (stTempVec[0].m_iNotify > 0)
		{
			stNotifyBonusMap[stTempVec[0].m_iNotify].emplace_back(stTempVec[0].m_iItemType, stTempVec[0].m_iCardID, stTempVec[0].m_iNum);
		}
		stRsp.m_vtAwardItem.emplace_back(stTempVec[0].m_iItemType, stTempVec[0].m_iCardID, stTempVec[0].m_iNum);
	}

	if (int32_t(stRsp.m_vtAwardItem.size()) != stParam.m_iCount)
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << BAD_COMMODITY_ID
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(BAD_COMMODITY_ID)
			<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(BAD_COMMODITY_ID)
	}

	stRsp.m_stConsume.m_iItemType = iterCommodity->second.m_iBuyItemType;
	stRsp.m_stConsume.m_iCardID = iterCommodity->second.m_iBuyItemID;
	stRsp.m_stConsume.m_iNum = stParam.m_iCount;
	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRsp.m_stConsume, m_usCmd))
	{
		PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
			<< "|MSG:" << CLogicError::GetErrMsg()
			<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
	}

    m_pUserInfo->m_stIBusyData.SetLuckyHammerActivityID(stParam.m_iActivityID);
    m_pUserInfo->m_stIBusyData.SetLuckyHammerLotteryCount(iHasLotteryCount + stParam.m_iCount);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_vtAwardItem, m_usCmd);

	logic_game_user_action_channel_data_t stData;
	stData.m_iTimes = stParam.m_iCount * iterCommodity->second.m_iCountScore;
	stData.m_iAction = CLogicGameChannelEnum::ACTION_LUCKY_HAMMER;
	LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

	SyncLuckyHammerScore(stParam.m_iActivityID);

	for (const auto& notify : stNotifyBonusMap)
	{
		for (const auto& item : notify.second)
		{
			for (auto iter = CLogicConfig::GetBroadcastConfig().m_stConfigMap.lower_bound("lucky_hammer"); iter != CLogicConfig::GetBroadcastConfig().m_stConfigMap.upper_bound("lucky_hammer"); ++iter)
			{
				CSRequestSyncLuckyHammerLog stCrossBody;
				stCrossBody.m_iActivityID = stParam.m_iActivityID;
				stCrossBody.m_iStartTime = iterActivityRef->second.m_iStartTime;
				stCrossBody.m_iEndTime = iterActivityRef->second.m_iEndTime;
				stCrossBody.m_stParamMap = {
					{"nick", m_pUserInfo->m_strNick},
					{"item_name", CLogicConfig::GetItemName(item.m_iItemType, item.m_iCardID)},
                    {"item_count", std::to_string(item.m_iNum)},
					{"notify", std::to_string(notify.first)}
				};
				stCrossBody.m_strContent = iter->second.m_stTalkContentFmt.Replace(stCrossBody.m_stParamMap);
				stCrossBody.m_iContentNotify = notify.first;
				stCrossBody.m_strBroadcastID = iter->first;

				SendToCrossServer(enmCmdType_request, MSG_CROSS_SYNC_LUCKY_HAMMER_LOG, stCrossBody);

				break;
			}
		}
	}

	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

	return true;
}

bool CLogicCrossServerProcessor::DoUserRunGetLuckyHammerRank(const msgpack::unpacked & stFormatData)
{
	CRequestGetLuckyHammerRank stParam;

	if (!DecodeMsgPack(stFormatData, stParam))
	{
		return (false);
	}

	SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
	return true;
}

bool CLogicCrossServerProcessor::DoUserRunGetLuckyHammerLog(const msgpack::unpacked & stFormatData)
{
	CRequestGetLuckyHammerLog stReqBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return (false);
	}

	SendToCrossServer(enmCmdType_request, m_usCmd, stReqBody);
	return true;
}

bool CLogicCrossServerProcessor::DoUserRunLuckyHammerLogScore(const msgpack::unpacked & stFormatData)
{
	CRequestLuckyHammerLogScore stReqBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return (false);
	}

	const auto iterCommodity = CLogicConfig::GetActivityConfig().m_stActiveCommodity.find(stReqBody.m_iActivityID);
	if (iterCommodity == CLogicConfig::GetActivityConfig().m_stActiveCommodity.end())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << ACTIVE_IS_NOT_IN_CONFIG
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(ACTIVE_IS_NOT_IN_CONFIG)
			<< "|ACTIVE_ID:" << stReqBody.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(ACTIVE_IS_NOT_IN_CONFIG)
	}

	if (m_pUserInfo->m_stUserDailyTable.GetLuckyHammerLogCount() >= iterCommodity->second.m_iDailyLuckyCount)
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << COUNT_NOT_ENOUGH
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(COUNT_NOT_ENOUGH)
			<< "|ACTIVE_ID:" << stReqBody.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(COUNT_NOT_ENOUGH)
	}

	SendToCrossServer(enmCmdType_request, m_usCmd, stReqBody);
	return true;
}

bool CLogicCrossServerProcessor::DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
	ReportMatchUserData();

	bool bRet = false;

	switch (stMsgHead.m_usCmd)
	{
	case MSG_CROSS_FIGHT_MATCH_PVP:
		bRet = DoServerRunFightMatchPvp(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_NOTIFY_BE_MATCHED:
		bRet = DoServerRunBeMatchedNotify(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_NOTIFY_CROSS_BROADCAST:
		bRet = DoServerRunBroadcastNotify(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_FIGHT_FAIR_PVP:
		bRet = DoServerRunFightFairPvp(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_NOTIFY_FAIR_MATCHED:
		bRet = DoServerRunFairMatchedNotify(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_GET_LUCKY_HAMMER_RANK:
		bRet = DoServerRunGetLuckyHammerRank(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_GET_LUCKY_HAMMER_LOG:
		bRet = DoServerRunGetLuckyHammerLog(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_LUCKY_HAMMER_LOG_SCORE:
		bRet = DoServerRunLuckyHammerLogScore(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_NOTIFY_LUCKY_HAMMER_RANK_AWARD:
		bRet = DoServerRunLuckyHammerRankNotify(stMsgHead, stFormatData);
		break;

	default:
		break;
	}
	return bRet;
}

bool CLogicCrossServerProcessor::DoServerRunFightMatchPvp(const CMsgHeadCross &stMsgHead,
															const msgpack::unpacked &stFormatData)
{
	CResponseFightMatchPvp stRspBody;
	if (!DecodeMsgPack(stFormatData, stRspBody))
	{
		return false;
	}

	//机器人假战绩
	if (0 == stRspBody.m_stChallengerPlayer.m_stBriefInfo.m_iGroupID)
	{
		// stRspBody.m_stChallengerPlayer.m_iPvpVictoryTimes = CLogicCommon::GetRandNum(m_pUserInfo->m_stUserInfoTable.GetMatchWinCount() * 0.5, m_pUserInfo->m_stUserInfoTable.GetMatchWinCount() * 1.5);
		// stRspBody.m_stChallengerPlayer.m_iPvpFailedTimes = CLogicCommon::GetRandNum(stRspBody.m_stChallengerPlayer.m_iPvpVictoryTimes * 0.8, stRspBody.m_stChallengerPlayer.m_iPvpVictoryTimes * 1.2);
	}
	
	m_pUserInfo->m_stOnlyInCacheData.m_stMatchPvpPlayer = stRspBody.m_stChallengerPlayer;

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoServerRunBeMatchedNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CRequestCrossUserInfo stNotify;
    if (!DecodeMsgPack(stFormatData, stNotify))
    {
        return false;
    }

	CSRequestUpdateMatchUser stUpdateBody;
	stUpdateBody.m_stUserVec.emplace_back(CLogicCacheCommon::GetUserCrossInfo(m_pUserInfo));
	SendToCrossServer(enmCmdType_request, MSG_CROSS_UPDATE_MATCH_USER, stUpdateBody);

	CResponseFightMatchPvp stCrossBody;
	stCrossBody.m_stChallengerPlayer = CLogicCacheCommon::GetPvpChallengerInfo(m_pUserInfo->m_iGroupID, m_pUserInfo->m_iUin);
	// stCrossBody.m_stChallengerPlayer.m_iPvpElo = m_pUserInfo->m_stUserInfoTable.GetMatchScore();
	// stCrossBody.m_stChallengerPlayer.m_iPvpVictoryTimes = m_pUserInfo->m_stUserInfoTable.GetMatchWinCount();
	// stCrossBody.m_stChallengerPlayer.m_iPvpFailedTimes = m_pUserInfo->m_stUserInfoTable.GetMatchLoseCount();
	SendToCrossServer(enmCmdType_cross, MSG_CROSS_FIGHT_MATCH_PVP, stCrossBody, stNotify.m_iDstUin, stNotify.m_iDstGroup);
	return true;
}

bool CLogicCrossServerProcessor::DoServerRunBroadcastNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
	CSNotifyBroadcast stNotify;
	if (!DecodeMsgPack(stFormatData, stNotify))
	{
		return false;
	}

	CLogicGlobal::PublishBroadcast(stNotify.m_strBroadcastID, 0, stNotify.m_stParamMap);
	return true;
}

bool CLogicCrossServerProcessor::DoServerRunFightFairPvp(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
	CResponseFightFairPvp stRspBody;
	if (!DecodeMsgPack(stFormatData, stRspBody))
	{
		return false;
	}

	//没有人,则随机找个机器人
	if (0 == stRspBody.m_stChallengerPlayer.m_stBriefInfo.m_iUid)
	{
		const auto& stRobot = CLogicConfig::GetFairPvpConfig().RandomRobot();

		CPackPvpChallengerInfo stChallenger;
		stChallenger.m_stBriefInfo.m_iUid = stRobot.m_iID;
		stChallenger.m_stBriefInfo.m_iGroupID = 0;
		stChallenger.m_stBriefInfo.m_strNick = stRobot.m_strName;
		stChallenger.m_stBriefInfo.m_iLevel = m_pUserInfo->m_stUserInfoTable.GetLevel() * 0.8;
		stChallenger.m_stBriefInfo.m_iAvatarID = stRobot.m_iAvatarID;
		stChallenger.m_stBriefInfo.m_iAvatarBorder = stRobot.m_iAvatarBorder;

		for (const auto& card_id : stRobot.m_iCardVec)
		{
			CPackPvpChallengerCardInfo stCard;
			stCard.m_iCardID = card_id;
			stCard.m_iColor = CLogicConfig::GetFairPvpConfig().m_iFightCardColor;
			stCard.m_iStar = CLogicConfig::GetFairPvpConfig().m_iFightCardStar;
			stCard.m_iLevel = CLogicConfig::GetFairPvpConfig().m_iFightCardLevel;
			// stCard.m_iCardSkill1Level = CLogicConfig::GetFairPvpConfig().m_iFightSkillLevel1;
			// stCard.m_iCardSkill2Level = CLogicConfig::GetFairPvpConfig().m_iFightSkillLevel2;
			// stCard.m_iCardSkill3Level = CLogicConfig::GetFairPvpConfig().m_iFightSkillLevel3;

			stChallenger.m_stCardInfoList.push_back(std::move(stCard));
		}

		stRspBody.m_stChallengerPlayer = std::move(stChallenger);
	}

	//竞技系数
	const auto* pstConfig = CLogicConfig::GetFairPvpConfig().GetFairPvpWinConfig(m_pUserInfo->m_stUserInfoTable.GetFairPvpWinCount(), CLogicCommon::GetSec());
	if (nullptr != pstConfig)
	{
		// stRspBody.m_stChallengerPlayer.m_dAtk = pstConfig->m_dAtk;
		// stRspBody.m_stChallengerPlayer.m_dCri = pstConfig->m_dCri;
		// stRspBody.m_stChallengerPlayer.m_dDef = pstConfig->m_dDef;
		// stRspBody.m_stChallengerPlayer.m_dHp = pstConfig->m_dHp;
	}
		
	m_pUserInfo->m_stOnlyInCacheData.m_stFairPvpPlayer = stRspBody.m_stChallengerPlayer;

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoServerRunFairMatchedNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
	CRequestSelectFairPvpTeam notify;
	if (!DecodeMsgPack(stFormatData, notify))
	{
		return false;
	}

	CPackPvpChallengerInfo stChallenger;
	stChallenger.m_stBriefInfo.m_iUid = m_pUserInfo->m_iUin;
	stChallenger.m_stBriefInfo.m_iGroupID = m_pUserInfo->m_iGroupID;
	stChallenger.m_stBriefInfo.m_strNick = m_pUserInfo->m_strNick;
	stChallenger.m_stBriefInfo.m_iLevel = m_pUserInfo->m_stUserInfoTable.GetLevel();
	// stChallenger.m_iPvpVictoryTimes = m_pUserInfo->m_stUserInfoTable.GetFairPvpWinCount();
	// stChallenger.m_iPvpFailedTimes = m_pUserInfo->m_stUserInfoTable.GetFairPvpLoseCount();
	stChallenger.m_stBriefInfo.m_iAvatarID = m_pUserInfo->m_stUserInfoTable.GetAvatarID();
	stChallenger.m_stBriefInfo.m_iAvatarBorder = m_pUserInfo->m_stUserInfoTable.GetAvatarBorder();

	for (const auto& card_id : notify.m_iTeamVec)
	{
		CPackPvpChallengerCardInfo stCard;
		stCard.m_iCardID = card_id;
		stCard.m_iColor = CLogicConfig::GetFairPvpConfig().m_iFightCardColor;
		stCard.m_iStar = CLogicConfig::GetFairPvpConfig().m_iFightCardStar;
		stCard.m_iLevel = CLogicConfig::GetFairPvpConfig().m_iFightCardLevel;
		// stCard.m_iCardSkill1Level = CLogicConfig::GetFairPvpConfig().m_iFightSkillLevel1;
		// stCard.m_iCardSkill2Level = CLogicConfig::GetFairPvpConfig().m_iFightSkillLevel2;
		// stCard.m_iCardSkill3Level = CLogicConfig::GetFairPvpConfig().m_iFightSkillLevel3;

		stChallenger.m_stCardInfoList.push_back(std::move(stCard));
	}

	CResponseFightFairPvp stCrossBody;
	stCrossBody.m_stChallengerPlayer = std::move(stChallenger);

	SendToCrossServer(enmCmdType_cross, MSG_CROSS_FIGHT_FAIR_PVP, stCrossBody, notify.m_iDstUin, notify.m_iDstGroup);
	return true;
}

bool CLogicCrossServerProcessor::DoServerRunGetLuckyHammerLog(const CMsgHeadCross &stMsgHead,
																const msgpack::unpacked &stFormatData)
{
	CResponseGetLuckyHammerLog stRspBody;
	if (!DecodeMsgPack(stFormatData, stRspBody))
	{
		return false;
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoServerRunLuckyHammerLogScore(const CMsgHeadCross &stMsgHead,
																  const msgpack::unpacked &stFormatData)
{
	CResponseLuckyHammerLogScore stRspBody;
	if (!DecodeMsgPack(stFormatData, stRspBody))
	{
		return false;
	}

	const auto iterCommodity = CLogicConfig::GetActivityConfig().m_stActiveCommodity.find(stRspBody.m_iActivityID);
	if (iterCommodity == CLogicConfig::GetActivityConfig().m_stActiveCommodity.end())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << ACTIVE_IS_NOT_IN_CONFIG
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(ACTIVE_IS_NOT_IN_CONFIG)
			<< "|ACTIVE_ID:" << stRspBody.m_iActivityID << std::endl;

		SEND_ERRCODE_AND_RET(ACTIVE_IS_NOT_IN_CONFIG)
	}

	if (m_pUserInfo->m_stUserDailyTable.GetLuckyHammerLogCount() < iterCommodity->second.m_iDailyLuckyCount)
	{
		m_pUserInfo->m_stUserDailyTable.AddLuckyHammerLogCount(1);

		CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);

		logic_game_user_action_channel_data_t stData;
		stData.m_iTimes = iterCommodity->second.m_iLuckyScore;
		stData.m_iAction = CLogicGameChannelEnum::ACTION_LUCKY_HAMMER;
		LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

		SyncLuckyHammerScore(stRspBody.m_iActivityID);
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoServerRunGetLuckyHammerRank(const CMsgHeadCross &stMsgHead,
																 const msgpack::unpacked &stFormatData)
{
	CResponseGetLuckyHammerRank stRspBody;
	if (!DecodeMsgPack(stFormatData, stRspBody))
	{
		return false;
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossServerProcessor::DoServerRunLuckyHammerRankNotify(const CMsgHeadCross &stMsgHead,
																	const msgpack::unpacked &stFormatData)
{
	CSNotifyLuckyHammerRank stNotify;
	if (!DecodeMsgPack(stFormatData, stNotify))
	{
		return false;
	}

	const auto iterRank = CLogicConfig::GetActivityConfig().m_stActiveRankConfig.find(stNotify.m_iActivityID);
	if (iterRank == CLogicConfig::GetActivityConfig().m_stActiveRankConfig.end())
	{
		return false;
	}

	for (const auto& award : iterRank->second)
	{
		if (award.m_iMaxRank <= stNotify.m_iRank && award.m_iMinRank >= stNotify.m_iRank)
		{
            CLogicGlobal::AddAsyncUserMail(m_iUin, m_iGroupID, award.m_strSender, award.m_strTitle,
                                        award.m_strContent, award.m_stBonusItemVec, "SEND_LUCKY_HAMMER_RANK_AWARD");
		}
	}

	return true;
}

void CLogicCrossServerProcessor::ReportMatchUserData()
{
	static int32_t s_iLastReportMatchUserTime = 0;
	if (CLogicCommon::GetSec() - s_iLastReportMatchUserTime > 10 * 60 && GetCrossConnection()->IsConnected())
	{
		CSRequestUpdateMatchUser stReqBody;
		int32_t rank = 1;
		for (auto iter = LOGIC_FIGHT_POWER_RANK_INS.begin(); iter != LOGIC_FIGHT_POWER_RANK_INS.end() && rank <= 30; ++iter, ++rank)
		{
			user_info_key_t stKey;
			stKey.m_ulRoleID = iter->first;
			const auto pUserData = CLogicCacheCommon::LoadUserData(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
			if (nullptr != pUserData && pUserData->m_stUserInfoTable.GetLevel() >= CLogicConfig::GetMatchPvpConfig().m_iMatchMinLevel)
			{
				stReqBody.m_stUserVec.emplace_back(CLogicCacheCommon::GetUserCrossInfo(pUserData));
			}
		}
		for (auto iterUser = CLogicUser::GetInstance().Begin(); iterUser != CLogicUser::GetInstance().End(); ++iterUser)
		{
			const auto pUserData = CLogicCacheCommon::LoadUserData(iterUser->second.m_iGroupID, iterUser->first);
			if (nullptr != pUserData && pUserData->m_stUserInfoTable.GetLevel() >= CLogicConfig::GetMatchPvpConfig().m_iMatchMinLevel)
			{
				stReqBody.m_stUserVec.emplace_back(CLogicCacheCommon::GetUserCrossInfo(pUserData));
			}
		}

		SendToCrossServer(enmCmdType_request, MSG_CROSS_UPDATE_MATCH_USER, stReqBody);

		s_iLastReportMatchUserTime = CLogicCommon::GetSec();
	}
}

void CLogicCrossServerProcessor::SyncLuckyHammerScore(int32_t iActivityID)
{
	const auto iterActivityRef = CLogicConfig::GetActivityConfig().m_stActivityRef.find(iActivityID);
	if (iterActivityRef == CLogicConfig::GetActivityConfig().m_stActivityRef.end())
	{
		return;
	}

	if (iterActivityRef->second.m_iStartTime <= 0 || iterActivityRef->second.m_iEndTime <= 0)
	{
		return;
	}

	const auto iterActivityCommodity = CLogicConfig::GetActivityConfig().m_stActiveCommodity.find(iActivityID);
	if (iterActivityCommodity == CLogicConfig::GetActivityConfig().m_stActiveCommodity.end())
	{
		return;
	}

	const auto iterActivity = m_pUserInfo->m_stUserActiveMap.Find(iActivityID);
	if (iterActivity == m_pUserInfo->m_stUserActiveMap.End())
	{
		return;
	}

	if(iterActivity->second->GetActiveTargetNum() < iterActivityCommodity->second.m_iRankScore)
	{
		return;
	}

	CSRequestSyncLuckyHammerScore stReqBody;
	stReqBody.m_iActivityID = iActivityID;
	stReqBody.m_iStartTime = iterActivityRef->second.m_iStartTime;
	stReqBody.m_iEndTime = iterActivityRef->second.m_iEndTime;
	stReqBody.m_iScore = iterActivity->second->GetActiveTargetNum();
	stReqBody.m_strNick = m_pUserInfo->m_strNick;
	SendToCrossServer(enmCmdType_request, MSG_CROSS_SYNC_LUCKY_HAMMER_SCORE, stReqBody);
}

