#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "common_datetime.h"
#include "server_award_protocol.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_active_info.h"
#include "logic_config.h"
#include "logic_game_award_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_global_frame.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_talk_common.h"
#include "logic_game_cache_common.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_global.h"
#include "server_cmd_data_report_define.h"
#include "server_data_report_protocol.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_BANK_INFO, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_BUY_BANK_GOLD, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SELL_BANK_GOLD, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_HAPPY_TURN_CARD, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_TURN_CARD_AWARD, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TURNING_HAPPY_CARD, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SELECT_MULTI_COLOR_LANTERN, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_VIP_DAILY_AWARD, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_BUY_BACK_COUNT, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_RECOMMENDED_LINEUP_AWARD, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SEND_RED_BAG, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_RECEIVE_RED_BAG, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_RED_BAG_LIST, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CHECK_RED_BAG_VALID, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SYS_OTHER_BUY, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_GET_VIP_QUARTER_DAILY_REWARD, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_WORLD_QUEST_REWARD, CLogicAwardProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CHANGE_AGE_LIMIT, CLogicAwardProcessor)




CLogicAwardProcessor::CLogicAwardProcessor(uint16_t usCmd, const std::string& strCmdName)
	: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicAwardProcessor::DoUserRun(const msgpack::unpacked & stFormatData)
{
	bool bRet = false;
	switch (m_usCmd)
	{
	case MSG_LOGIC_GET_BANK_INFO:
		bRet = DoUserRunGetBankInfo(stFormatData);
		break;

	case MSG_LOGIC_BUY_BANK_GOLD:
		bRet = DoUserRunBuyBankGold(stFormatData);
		break;

	case MSG_LOGIC_SELL_BANK_GOLD:
		bRet = DoUserRunSellBankGold(stFormatData);
		break;

	case MSG_LOGIC_GET_HAPPY_TURN_CARD:
		bRet = DoUserRunGetHappyTurnCard(stFormatData);
		break;

	case MSG_LOGIC_GET_TURN_CARD_AWARD:
		bRet = DoUserRunGetTurnCardAward(stFormatData);
		break;

	case MSG_LOGIC_TURNING_HAPPY_CARD:
		bRet = DoUserRunTurningHappyCard(stFormatData);
		break;

	case MSG_LOGIC_SELECT_MULTI_COLOR_LANTERN:
		bRet = DoUserRunSelectMultiColorLantern(stFormatData);
		break;

	case MSG_LOGIC_GET_VIP_DAILY_AWARD:
		bRet = DoUserRunGetVipDailyAward(stFormatData);
		break;

	case MSG_LOGIC_TOUHOU_GET_VIP_QUARTER_DAILY_REWARD:
	    bRet = DoUserRunGetVipQuarterDailyAward(stFormatData);
	    break;

	case MSG_LOGIC_GET_BUY_BACK_COUNT:
		bRet = DoUserRunGetBuyBackCount(stFormatData);
		break;

	case MSG_LOGIC_GET_RECOMMENDED_LINEUP_AWARD:
		bRet = DoUserRunGetRecommendedLineupAward(stFormatData);
		break;

	case MSG_LOGIC_SEND_RED_BAG:
		bRet = DoUserRunSendRedBag(stFormatData);
		break;

	case MSG_LOGIC_RECEIVE_RED_BAG:
		bRet = DoUserRunReceiveRedBag(stFormatData);
		break;

    case MSG_LOGIC_TOUHOU_WORLD_QUEST_REWARD:
        bRet = DoUserRunWorldQuestReward(stFormatData);
        break;

	case MSG_LOGIC_GET_RED_BAG_LIST:
		bRet = DoUserRunGetRedBagList(stFormatData);
		break;

	case MSG_LOGIC_CHECK_RED_BAG_VALID:
		bRet = DoUserRunCheckRedBagValid(stFormatData);
		break;

	case MSG_LOGIC_SYS_OTHER_BUY:
	    bRet = DoUserRunSysOtherBuy(stFormatData);
	    break;

    case MSG_LOGIC_CHANGE_AGE_LIMIT:
        bRet = DoUserRunChangeAgeLimit(stFormatData);
        break;

	default:
		break;
	}

	return bRet;
}

bool CLogicAwardProcessor::DoUserRunGetBankInfo(const msgpack::unpacked& stFormatData)
{
	const auto& rstBankPrice = CLogicGlobal::GetBankPriceList();

	CResponseGetBankInfo stRsp;
	stRsp.m_iRefreshTimeStamp = CLogicGlobal::GetBankNextRefreshTime();
	stRsp.m_strEvent = CLogicGlobal::GetBankPriceEvent();
	stRsp.m_stBankPriceVec.assign(rstBankPrice.begin(), rstBankPrice.end());

	stRsp.m_stBankCountryVec.emplace_back(CLogicConfigDefine::COUNTRY_WEI,
		m_pUserInfo->m_stUserInfoTable.GetWeiBuyNum(),
		m_pUserInfo->m_stUserInfoTable.GetWeiAvgPrice());

	stRsp.m_stBankCountryVec.emplace_back(CLogicConfigDefine::COUNTRY_SHU,
		m_pUserInfo->m_stUserInfoTable.GetShuBuyNum(),
		m_pUserInfo->m_stUserInfoTable.GetShuAvgPrice());

	stRsp.m_stBankCountryVec.emplace_back(CLogicConfigDefine::COUNTRY_WU,
		m_pUserInfo->m_stUserInfoTable.GetWuBuyNum(),
		m_pUserInfo->m_stUserInfoTable.GetWuAvgPrice());

	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunBuyBankGold(const msgpack::unpacked& stFormatData)
{
	CRequestBuyBankGold stReqBody;
	CResponseBuyBankGold stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

    int iMaxCapacity = 0;

	const auto iNowCapacity = m_pUserInfo->m_stUserInfoTable.GetWeiBuyNum() + m_pUserInfo->m_stUserInfoTable.GetShuBuyNum() + m_pUserInfo->m_stUserInfoTable.GetWuBuyNum();

	if (stReqBody.m_iBuyNum <= 0 || iNowCapacity + stReqBody.m_iBuyNum > iMaxCapacity)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BANK_CAPACITY_NOT_ENOUGH);
	}

	const auto iLastPrice = CLogicGlobal::GetLastBankPrice(stReqBody.m_iCountry);
	if (iLastPrice <= 0 || iLastPrice != stReqBody.m_iPrice)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BANK_PRICE_CHANGE);
	}

	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GoldID, stReqBody.m_iBuyNum * iLastPrice, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	stRspBody.m_iBuyNum = stReqBody.m_iBuyNum;
	stRspBody.m_iPrice = stReqBody.m_iPrice;
	if (stReqBody.m_iCountry == CLogicConfigDefine::COUNTRY_WEI)
	{
		const auto iAvgPrice = m_pUserInfo->m_stUserInfoTable.GetWeiAvgPrice();
		const auto iBuyNum = m_pUserInfo->m_stUserInfoTable.GetWeiBuyNum();

		m_pUserInfo->m_stUserInfoTable.SetWeiAvgPrice((iAvgPrice * iBuyNum + stReqBody.m_iBuyNum * iLastPrice) / (iBuyNum + stReqBody.m_iBuyNum));
		m_pUserInfo->m_stUserInfoTable.AddWeiBuyNum(stReqBody.m_iBuyNum);

		stRspBody.m_stBankCountry = { CLogicConfigDefine::COUNTRY_WEI, m_pUserInfo->m_stUserInfoTable.GetWeiBuyNum(), m_pUserInfo->m_stUserInfoTable.GetWeiAvgPrice() };
	}
	else if (stReqBody.m_iCountry == CLogicConfigDefine::COUNTRY_SHU)
	{
		const auto iAvgPrice = m_pUserInfo->m_stUserInfoTable.GetShuAvgPrice();
		const auto iBuyNum = m_pUserInfo->m_stUserInfoTable.GetShuBuyNum();

		m_pUserInfo->m_stUserInfoTable.SetShuAvgPrice((iAvgPrice * iBuyNum + stReqBody.m_iBuyNum * iLastPrice) / (iBuyNum + stReqBody.m_iBuyNum));
		m_pUserInfo->m_stUserInfoTable.AddShuBuyNum(stReqBody.m_iBuyNum);

		stRspBody.m_stBankCountry = { CLogicConfigDefine::COUNTRY_SHU, m_pUserInfo->m_stUserInfoTable.GetShuBuyNum(), m_pUserInfo->m_stUserInfoTable.GetShuAvgPrice() };
	}
	else if (stReqBody.m_iCountry == CLogicConfigDefine::COUNTRY_WU)
	{
		const auto iAvgPrice = m_pUserInfo->m_stUserInfoTable.GetWuAvgPrice();
		const auto iBuyNum = m_pUserInfo->m_stUserInfoTable.GetWuBuyNum();

		m_pUserInfo->m_stUserInfoTable.SetWuAvgPrice((iAvgPrice * iBuyNum + stReqBody.m_iBuyNum * iLastPrice) / (iBuyNum + stReqBody.m_iBuyNum));
		m_pUserInfo->m_stUserInfoTable.AddWuBuyNum(stReqBody.m_iBuyNum);

		stRspBody.m_stBankCountry = { CLogicConfigDefine::COUNTRY_WU, m_pUserInfo->m_stUserInfoTable.GetWuBuyNum(), m_pUserInfo->m_stUserInfoTable.GetWuAvgPrice() };
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunSellBankGold(const msgpack::unpacked& stFormatData)
{
	CRequestSellBankGold stReqBody;
	CResponseSellBankGold stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto iLastPrice = CLogicGlobal::GetLastBankPrice(stReqBody.m_iCountry);
	if (iLastPrice <= 0 || iLastPrice != stReqBody.m_iPrice)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BANK_PRICE_CHANGE);
	}

	std::function<const int32_t&()> getBuyNumFunc(nullptr);
	std::function<const int32_t&()> getAvgPriceFunc(nullptr);
	std::function<void(int32_t)> setBuyNumFunc(nullptr);

	if (stReqBody.m_iCountry == CLogicConfigDefine::COUNTRY_WEI)
	{
		getBuyNumFunc = std::bind(&decltype(m_pUserInfo->m_stUserInfoTable)::GetWeiBuyNum, &m_pUserInfo->m_stUserInfoTable);
		getAvgPriceFunc = std::bind(&decltype(m_pUserInfo->m_stUserInfoTable)::GetWeiAvgPrice, &m_pUserInfo->m_stUserInfoTable);
		setBuyNumFunc = std::bind(&decltype(m_pUserInfo->m_stUserInfoTable)::SetWeiBuyNum, &m_pUserInfo->m_stUserInfoTable, std::placeholders::_1);
	}
	else if (stReqBody.m_iCountry == CLogicConfigDefine::COUNTRY_SHU)
	{
		getBuyNumFunc = std::bind(&decltype(m_pUserInfo->m_stUserInfoTable)::GetShuBuyNum, &m_pUserInfo->m_stUserInfoTable);
		getAvgPriceFunc = std::bind(&decltype(m_pUserInfo->m_stUserInfoTable)::GetShuAvgPrice, &m_pUserInfo->m_stUserInfoTable);
		setBuyNumFunc = std::bind(&decltype(m_pUserInfo->m_stUserInfoTable)::SetShuBuyNum, &m_pUserInfo->m_stUserInfoTable, std::placeholders::_1);
	}
	else if (stReqBody.m_iCountry == CLogicConfigDefine::COUNTRY_WU)
	{
		getBuyNumFunc = std::bind(&decltype(m_pUserInfo->m_stUserInfoTable)::GetWuBuyNum, &m_pUserInfo->m_stUserInfoTable);
		getAvgPriceFunc = std::bind(&decltype(m_pUserInfo->m_stUserInfoTable)::GetWuAvgPrice, &m_pUserInfo->m_stUserInfoTable);
		setBuyNumFunc = std::bind(&decltype(m_pUserInfo->m_stUserInfoTable)::SetWuBuyNum, &m_pUserInfo->m_stUserInfoTable, std::placeholders::_1);
	}

	if (!getBuyNumFunc || !getAvgPriceFunc || !setBuyNumFunc)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
	}

	const auto iBuyNum = getBuyNumFunc();
	if (iBuyNum < stReqBody.m_iSellNum)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BANK_GOLD_BUY_NOT_ENOUGH);
	}

	setBuyNumFunc(iBuyNum - stReqBody.m_iSellNum);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GoldID, iLastPrice * stReqBody.m_iSellNum, m_usCmd);

	stRspBody.m_iPrice = stReqBody.m_iPrice;
	stRspBody.m_iSellNum = stReqBody.m_iSellNum;
	stRspBody.m_stBankCountry = { stReqBody.m_iCountry, getBuyNumFunc(), getAvgPriceFunc() };

	if (iLastPrice > getAvgPriceFunc())
	{
		logic_game_user_action_channel_data_t stActionData;
		stActionData.m_iTimes = (iLastPrice - getAvgPriceFunc()) * stReqBody.m_iSellNum;
		stActionData.m_iAction = CLogicGameChannelEnum::ACTION_BANK_EARN_GOLD;
		LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stActionData);

		// CLogicDailyTaskSystem::NotifyTask(m_pUserInfo, CLogicDailyTaskSystem::GET_GAME_ITEM_TASK, CLogicConfigDefine::GOLD, (iLastPrice - getAvgPriceFunc()) * stReqBody.m_iSellNum);
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunGetHappyTurnCard(const msgpack::unpacked& stFormatData)
{
	CResponseGetHappyTurnCard stRspBody;

	const auto& stTurnCardVec = LOGIC_GLOBAL_FRAME_INS->GetTurnCardVec();

	for (uint32_t i = 0 ; i < stTurnCardVec.size(); ++i)
	{
		const auto iFlag = 1ULL << i;
		if (m_pUserInfo->m_stUserDailyTable.GetTurnCardBitmap() & iFlag)
		{
			stRspBody.m_stItemVec.emplace_back(stTurnCardVec[i]);
		}
		else
		{
			stRspBody.m_stItemVec.emplace_back();
		}
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunGetTurnCardAward(const msgpack::unpacked& stFormatData)
{
	CRequestGetTurnCardAward stReqBody;
	CResponseGetTurnCardAward stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto iterAward = CLogicConfig::GetAwardConfig().m_stHappyTurnCard.m_stBonusItemMap.find(stReqBody.m_iCount);
	if (iterAward == CLogicConfig::GetAwardConfig().m_stHappyTurnCard.m_stBonusItemMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	auto ulBitmap = m_pUserInfo->m_stUserDailyTable.GetTurnCardBitmap();
	auto iTurnCount = 0;
	while (ulBitmap)
	{
		ulBitmap = ulBitmap & (ulBitmap - 1);
		++iTurnCount;
	}
	iTurnCount /= 2;
	if (iTurnCount < stReqBody.m_iCount)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
	}

	const auto ulFlag = 1ULL << stReqBody.m_iCount;
	if (m_pUserInfo->m_stUserDailyTable.GetTurnCardAwardBitmap() & ulFlag)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
	}

    stRspBody.m_stBonus = iterAward->second;
    stRspBody.m_iCount = stReqBody.m_iCount;

	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonus, m_usCmd);

	m_pUserInfo->m_stUserDailyTable.SetTurnCardAwardBitmap(m_pUserInfo->m_stUserDailyTable.GetTurnCardAwardBitmap() | ulFlag);
	CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunTurningHappyCard(const msgpack::unpacked& stFormatData)
{
	CRequestTurningHappyCard stReqBody;
	CResponseTurningHappyCard stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto& stTurnConfig = CLogicConfig::GetAwardConfig().m_stHappyTurnCard;
	const auto& stTurnCardVec = LOGIC_GLOBAL_FRAME_INS->GetTurnCardVec();
	if (!CLogicCommon::IsWithinCycleDay(CLogicCommon::GetSec(), stTurnConfig.m_iStartDatetime, stTurnConfig.m_iStillDay, stTurnConfig.m_iStopDay))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
	}

	if (m_pUserInfo->m_stUserDailyTable.GetLeftTurnCardCount() <= 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
	}

	const uint32_t iOrder = stReqBody.m_iIndex - 1;
	if (iOrder >= stTurnCardVec.size())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	const auto ulFlag = 1ULL << iOrder;
	if (m_pUserInfo->m_stUserDailyTable.GetTurnCardBitmap() & ulFlag)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::CARD_HAVE_TURN);
	}

	m_pUserInfo->m_stUserDailyTable.AddLeftTurnCardCount(-1);
	m_pUserInfo->m_stUserDailyTable.SetTurnCardBitmap(m_pUserInfo->m_stUserDailyTable.GetTurnCardBitmap() | ulFlag);
	
	std::map<int32_t, int32_t> indexMap;
	for (uint32_t i = 0 ; i < stTurnCardVec.size(); ++i)
	{
		const auto ulTemp = 1ULL << i;
		if (m_pUserInfo->m_stUserDailyTable.GetTurnCardBitmap() & ulTemp)
		{
			const auto iterIndex = indexMap.find(stTurnCardVec[i].m_iCardID);
			if (iterIndex == indexMap.end())
			{
				indexMap.insert(std::make_pair(stTurnCardVec[i].m_iCardID, i));
			}
			else
			{
				indexMap.erase(iterIndex);
			}
		}
	}

	stRspBody.m_iIndex = stReqBody.m_iIndex;
	stRspBody.m_bIsPair = false;
	stRspBody.m_stCard = stTurnCardVec[iOrder];

	if (indexMap.empty())
	{
		stRspBody.m_bIsPair = true;
        auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stRspBody.m_stCard.m_iCardID);
        if(pConfig)
        {
            stRspBody.m_stCard.m_iItemType = CLogicConfigDefine::EXCHANGE_ITEM;
            stRspBody.m_stCard.m_iCardID = pConfig->m_iSoulID;
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stCard, m_usCmd);
        }
	}
	else if (indexMap.size() > 1)
	{
		for (const auto& index : indexMap)
		{
			const auto ulTemp = ~(1ULL << index.second);
			m_pUserInfo->m_stUserDailyTable.SetTurnCardBitmap(m_pUserInfo->m_stUserDailyTable.GetTurnCardBitmap() & ulTemp);
		}
	}

	const int32_t iHaveTurnCount = bin1num(m_pUserInfo->m_stUserDailyTable.GetTurnCardBitmap());
	if (iHaveTurnCount == int32_t(stTurnCardVec.size()))
	{
		logic_game_user_action_channel_data_t stData;
		stData.m_iTimes = 1;
		stData.m_iAction = CLogicGameChannelEnum::ACTION_COMPLETE_TURN_CARD_COUNT;
		LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);
	}
	
	CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);
	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunSelectMultiColorLantern(const msgpack::unpacked& stFormatData)
{
	CRequestSelectMultiColorLantern stReqBody;
	CResponseSelectMultiColorLantern stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	std::sort(stReqBody.m_iLanternVec.begin(), stReqBody.m_iLanternVec.end());
	stReqBody.m_iLanternVec.erase(std::unique(stReqBody.m_iLanternVec.begin(), stReqBody.m_iLanternVec.end()), stReqBody.m_iLanternVec.end());

	const auto& selectVec = CLogicGlobal::GetUserSelectLanternVec(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
	for (const auto& have : selectVec)
	{
		if (std::find(stReqBody.m_iLanternVec.begin(), stReqBody.m_iLanternVec.end(), have) == stReqBody.m_iLanternVec.end())
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::CANT_CANCEL_SELECT_LANTERN);
		}
	}

	uint32_t iSize = CLogicConfig::GetAwardConfig().m_stMultiColorLantern.m_iDefaultAmount;
	if (iSize < stReqBody.m_iLanternVec.size())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
	}

	CLogicGlobal::SetUserSelectLanternVec(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, stReqBody.m_iLanternVec);

	CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);
	stRspBody.m_iLanternVec = stReqBody.m_iLanternVec;
	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunGetVipDailyAward(const msgpack::unpacked& stFormatData)
{
	CRequestGetVipDailyAward stReqBody;
	CResponseGetVipDailyAward stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	if (stReqBody.m_iVipLevel < 0 || stReqBody.m_iVipLevel > m_pUserInfo->m_stUserInfoTable.GetVipLevel())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::VIP_LEVEL_TOO_LOW);
	}

	const uint64_t ulFlag = 1ULL << stReqBody.m_iVipLevel;
	const uint64_t ulAwardBitmap = m_pUserInfo->m_stUserDailyTable.GetVipDailyAwardBitmap();
	if (ulAwardBitmap & ulFlag)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
	}

	const auto* pstVipConfig = CLogicConfig::GetVipConfig().GetVipInfo(stReqBody.m_iVipLevel);
	if (nullptr == pstVipConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_VIP_LEVEL);
	}

	stRspBody.m_iVipLevel = stReqBody.m_iVipLevel;
	stRspBody.m_stBonusVec = pstVipConfig->m_stDailyAwardVec;

	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusVec, m_usCmd);
	m_pUserInfo->m_stUserDailyTable.SetVipDailyAwardBitmap(ulAwardBitmap | ulFlag);
	CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunGetVipQuarterDailyAward(const msgpack::unpacked& stFormatData)
{
    CRequestGetVipDailyAward stReqBody;
    CResponseGetVipDailyAward stRspBody;

    if (!DecodeMsgPack(stFormatData, stReqBody))
    {
        return false;
    }

    // if(stReqBody.m_bIsQuarterAward && !CLogicCacheCommon::isQuarterCardUser(m_pUserInfo))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_MEET_CONDITION);
    }

    if (stReqBody.m_iVipLevel != m_pUserInfo->m_stUserInfoTable.GetVipLevel())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    // 默认取 61 位作为每日祈祷的状态位
    int iPos = 61;
    const uint64_t ulFlag = 1ULL << iPos;
    const uint64_t ulAwardBitmap = m_pUserInfo->m_stUserDailyTable.GetVipDailyAwardBitmap();

    stRspBody.m_iVipLevel = stReqBody.m_iVipLevel;
    if(stReqBody.m_bIsQuarterAward)
    {
        int32_t iVipAwardLevel = m_pUserInfo->m_stUserDailyTable.GetQuarterVipAwardLevel();
        if(iVipAwardLevel >= m_pUserInfo->m_stUserInfoTable.GetVipLevel())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
        }

        for(++iVipAwardLevel; iVipAwardLevel <= stReqBody.m_iVipLevel; ++iVipAwardLevel)
        {
            auto iter = CLogicConfig::GetVipBagConfig().m_stVipQuarterBagMap.find(iVipAwardLevel);
            if (iter != CLogicConfig::GetVipBagConfig().m_stVipQuarterBagMap.end())
            {
                stRspBody.m_stBonusVec.insert(stRspBody.m_stBonusVec.end(), iter->second.begin(), iter->second.end());
            }
        }
    }
    else
    {
        if ((ulAwardBitmap & ulFlag))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
        }

        CLogicConfig::GetCardbagConfig().GetRandBonusByName(CLogicConfig::GetVipBagConfig().m_strDailyPrayBag, stRspBody.m_stBonusVec, 1);
    }

    if(stRspBody.m_stBonusVec.empty())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusVec, m_usCmd);

    if(stReqBody.m_bIsQuarterAward)
    {
        m_pUserInfo->m_stUserDailyTable.SetQuarterVipAwardLevel(stReqBody.m_iVipLevel);
    }
    else
    {
        m_pUserInfo->m_stUserDailyTable.SetVipDailyAwardBitmap(ulAwardBitmap | ulFlag);
    }

    CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);

    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicAwardProcessor::DoUserRunGetBuyBackCount(const msgpack::unpacked& stFormatData)
{
	CRequestGetBuyBackCount stReqBody;
	CResponseGetBuyBackCount stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	stRspBody.m_iBuyBackType = stReqBody.m_iBuyBackType;
	stRspBody.m_iBuyBackCount = stReqBody.m_iBuyBackCount;

	auto iterBack = m_pUserInfo->m_stBuyBackMap.Find(stReqBody.m_iBuyBackType);
	if (iterBack == m_pUserInfo->m_stBuyBackMap.End())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	if (stReqBody.m_iBuyBackCount <= 0 || iterBack->second->GetBuyBackCount() + stReqBody.m_iBuyBackCount > iterBack->second->GetCanBackCount())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
	}

	const auto iterConfig = CLogicConfig::GetAwardConfig().m_stBuyBackMap.find(stReqBody.m_iBuyBackType);
	if (iterConfig == CLogicConfig::GetAwardConfig().m_stBuyBackMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	stRspBody.m_stConsumeVec.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, iterConfig->second.m_iPrice * stReqBody.m_iBuyBackCount);
	if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRspBody.m_stConsumeVec, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	iterBack->second->AddBuyBackCount(stReqBody.m_iBuyBackCount);

	if (CEnumBuyBackType::DINNER == static_cast<CEnumBuyBackType>(stReqBody.m_iBuyBackType))
	{
		iterBack->second->SetUseBackCount(iterBack->second->GetBuyBackCount());

		int32_t iAddEnergy = 60;
		stRspBody.m_stBonusVec.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_EnergyID, iAddEnergy * stReqBody.m_iBuyBackCount);
	}

	if (!stRspBody.m_stBonusVec.empty())
	{
		CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusVec, m_usCmd);
	}

	CLogicUserNotifySystem::NotifyUserBuyBackInfo(m_pUserInfo);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunGetRecommendedLineupAward(const msgpack::unpacked& stFormatData)
{
	CRequestGetRecommendedLineupAward stReqBody;
	CResponseGetRecommendedLineupAward stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	if (stReqBody.m_iIndex < 0 || stReqBody.m_iIndex > 62)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	const auto iterConfig = CLogicConfig::GetAwardConfig().m_stRecommendedLineupMap.find(stReqBody.m_iIndex);
	if (iterConfig == CLogicConfig::GetAwardConfig().m_stRecommendedLineupMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	const uint64_t ulFlag = 1ULL << stReqBody.m_iIndex;
	const uint64_t ulBitmap = m_pUserInfo->m_stUserInfoTable.GetRecommendedLineupBitmap();
	if (ulBitmap & ulFlag)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
	}

	if (!HaveAllHero(iterConfig->second.m_iHeroVec))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED);
	}

	stRspBody.m_iIndex = stReqBody.m_iIndex;
	stRspBody.m_stBonusVec = iterConfig->second.m_stBonusVec;

	m_pUserInfo->m_stUserInfoTable.SetRecommendedLineupBitmap(ulBitmap | ulFlag);
	CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);

	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusVec, m_usCmd);
	
	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunSendRedBag(const msgpack::unpacked& stFormatData)
{
	CRequestSendRedBag stReqBody;
	CResponseSendRedBag stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicConfig::GetRedBagConfig().GetRedBagConfig(stReqBody.m_iRedBagItemID);
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::RED_BAG_NOT_EXIST);
	}

	if (pstConfig->m_iSendChannel == CLogicConfigDefine::talk_channel_guild && m_pUserInfo->m_stUserInfoTable.GetGuildID() <= 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND);
	}

	stRspBody.m_stConsumeItem.m_iItemType = CLogicConfigDefine::EXCHANGE_ITEM;
	stRspBody.m_stConsumeItem.m_iCardID = stReqBody.m_iRedBagItemID;
	stRspBody.m_stConsumeItem.m_iNum = 1;
	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRspBody.m_stConsumeItem, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	LOGIC_GLOBAL_FRAME_INS->SendRedBag(m_pUserInfo, stReqBody.m_iRedBagItemID);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunReceiveRedBag(const msgpack::unpacked& stFormatData)
{
	CRequestReceiveRedBag stReqBody;
	CResponseReceiveRedBag stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstRedBag = LOGIC_GLOBAL_FRAME_INS->GetRedBagInfo(stReqBody.m_iRedBagID);
	if (nullptr == pstRedBag)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::RED_BAG_NOT_EXIST);
	}

    if(pstRedBag->m_iGuildID != 0 && pstRedBag->m_iGuildID != m_pUserInfo->m_stUserInfoTable.GetGuildID())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::RED_BAG_NOT_EXIST);
    }

	stRspBody.m_stRedBagInfo = LOGIC_GLOBAL_FRAME_INS->GetRedBagBrief(stReqBody.m_iRedBagID);
    if (LOGIC_GLOBAL_FRAME_INS->ReceiveRedBag(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, stReqBody.m_iRedBagID, stRspBody.m_stReward, stRspBody.m_bIsBest))
    {
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stReward, m_usCmd);

        const auto& talks = CLogicConfig::GetRedBagConfig().m_stRedBagReceiveTalkVec;
        if (!talks.empty())
        {
            const auto& content = talks[CLogicCommon::GetRandNum() % talks.size()].Replace({ {"nick", stRspBody.m_stRedBagInfo.m_strNick} });
            CLogicTalk::SendWordBroadcastTalk(m_pUserInfo, pstRedBag->m_iTalkChannel, content, std::vector<int32_t>{});
        }

        // 每日收益记录
        auto stEarnings = m_pUserInfo->m_stUserDailyTable.GetRedBagEarnings();
        CLogicCacheCommon::PushGameItem(stEarnings, stRspBody.m_stReward);
        if(stEarnings.size() > 20) stEarnings.resize(20);
        m_pUserInfo->m_stUserDailyTable.SetRedBagEarnings(stEarnings);
    }

	for (auto& player : pstRedBag->m_stReceivePlayer)
	{
		auto* pUserInfo = CLogicCacheCommon::LoadUserData(player.m_iGroupID, player.m_iUid);
		if (nullptr != pUserInfo)
		{
			CPackRedBagReceiver stUser;
			stUser.m_iUid = pUserInfo->m_iUin;
			stUser.m_iGroupID = pUserInfo->m_iGroupID;
			stUser.m_strNick = pUserInfo->m_strNick;
			stUser.m_iAvatarID = pUserInfo->m_stUserInfoTable.GetAvatarID();
			stUser.m_iAvatarBorder = pUserInfo->m_stUserInfoTable.GetAvatarBorder();
			stUser.m_stReward = player.m_stReward;
            stUser.m_bIsBest = player.m_bIsBest;
			stRspBody.m_stReceiverVec.push_back(stUser);
		}
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

	return true;
}

bool CLogicAwardProcessor::DoUserRunWorldQuestReward(const msgpack::unpacked& stFormatData)
{
    CRequestReceiveRedBag stReqBody;
    CResponseReceiveRedBag stRspBody;

    if (!DecodeMsgPack(stFormatData, stReqBody))
    {
        return false;
    }

    auto iterQuest = LOGIC_GLOBAL_CACHE_INS->m_stWorldQuestMap.Find(stReqBody.m_iRedBagID);
    if(iterQuest == LOGIC_GLOBAL_CACHE_INS->m_stWorldQuestMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::RED_BAG_NOT_EXIST);
    }

    auto pConfig = CLogicConfig::GetWorldQuestConfig().GetQuestConfig(stReqBody.m_iRedBagID);
    if(nullptr == pConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
    }

    if(iterQuest->second->GetState() != CLogicConfigDefine::ECQS_Reached || iterQuest->second->GetOpenTime() <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_MEET_CONDITION);
    }

    if(iterQuest->second->GetEndTime() <= CLogicCommon::GetSec())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::RED_BAG_NOT_EXIST);
    }

    // 判断创角时间
    if(!pConfig->m_bAfterCreateCanReceive && iterQuest->second->GetOpenTime() < m_pUserInfo->m_stUserInfoTable.GetCreateTime())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::RED_BAG_NOT_EXIST);
    }

    auto stQuestState = m_pUserInfo->m_stIBusyData.GetWorldQuestRewardState();
    if(CLogicCacheCommon::VecBitmapCheckState(stQuestState, stReqBody.m_iRedBagID))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
    }

    bool bBigPrize = false;
    if(CLogicConfig::GetWorldQuestConfig().m_iBigPrizeTimes > 0
        && m_pUserInfo->m_stIBusyData.GetWorldQuestRewardTimes() + 1 >= CLogicConfig::GetWorldQuestConfig().m_iBigPrizeTimes)
    {
        bBigPrize = true;
    }

    // 领奖
    auto pRewardRandData = CLogicConfig::GetWorldQuestConfig().GetRewardBagRandReward(pConfig->m_strBagName, bBigPrize);
    if(nullptr == pRewardRandData)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
    }

    stRspBody.m_stReward.m_iItemType = pRewardRandData->m_iItemType;
    stRspBody.m_stReward.m_iCardID = pRewardRandData->m_iItemID;
    stRspBody.m_stReward.m_iNum = CLogicCommon::GetRandNum(pRewardRandData->m_iItemNumMin, pRewardRandData->m_iItemNumMax, true);
    stRspBody.m_bIsBest = pRewardRandData->m_bIsBest;
    stRspBody.m_stRedBagInfo.m_iRedBagID = stReqBody.m_iRedBagID;

    CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stReward, m_usCmd);

    CLogicCacheCommon::VecBitmapSetState(stQuestState, stReqBody.m_iRedBagID);
    m_pUserInfo->m_stIBusyData.SetWorldQuestRewardState(stQuestState);

    if(pRewardRandData->m_bIsBest)
        m_pUserInfo->m_stIBusyData.SetWorldQuestRewardTimes(0);
    else
        m_pUserInfo->m_stIBusyData.AddWorldQuestRewardTimes(1);

    // 每日收益记录
    auto stEarnings = m_pUserInfo->m_stUserDailyTable.GetRedBagEarnings();
    CLogicCacheCommon::PushGameItem(stEarnings, stRspBody.m_stReward);
    if(stEarnings.size() > 20) stEarnings.resize(20);
    m_pUserInfo->m_stUserDailyTable.SetRedBagEarnings(stEarnings);

    auto stAwardInfo = iterQuest->second->GetAwardInfo();
    if(stAwardInfo.size() >= 20)
    {
        int32_t iIndex = CLogicCommon::GetRandNum() % stAwardInfo.size();
        stAwardInfo[iIndex].m_iUid = m_pUserInfo->m_iUin;
        stAwardInfo[iIndex].m_iGroupID = m_pUserInfo->m_iGroupID;
        stAwardInfo[iIndex].m_stReward = stRspBody.m_stReward;
        stAwardInfo[iIndex].m_bIsBest = stRspBody.m_bIsBest;
    }
    else
    {
        CPackWorldQuestAwardInfo stNewUser;
        stNewUser.m_iUid = m_pUserInfo->m_iUin;
        stNewUser.m_iGroupID = m_pUserInfo->m_iGroupID;
        stNewUser.m_stReward = stRspBody.m_stReward;
        stNewUser.m_bIsBest = stRspBody.m_bIsBest;
        stAwardInfo.push_back(stNewUser);
    }

    iterQuest->second->SetAwardInfo(stAwardInfo);

    for (auto& player : stAwardInfo)
    {
        auto* pUserInfo = CLogicCacheCommon::LoadUserData(player.m_iGroupID, player.m_iUid);
        if (nullptr != pUserInfo)
        {
            CPackRedBagReceiver stUser;
            stUser.m_iUid = pUserInfo->m_iUin;
            stUser.m_iGroupID = pUserInfo->m_iGroupID;
            stUser.m_strNick = pUserInfo->m_strNick;
            stUser.m_iAvatarID = pUserInfo->m_stUserInfoTable.GetAvatarID();
            stUser.m_iAvatarBorder = pUserInfo->m_stUserInfoTable.GetAvatarBorder();
            stUser.m_stReward = player.m_stReward;
            stUser.m_bIsBest = player.m_bIsBest;
            stRspBody.m_stReceiverVec.push_back(stUser);
        }
    }

    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

    // 公告
    if(pRewardRandData->m_bIsBest)
    {
        CLogicGlobal::PublishBroadcast("world_quest_big_prize", 0, {
                {"nick", m_pUserInfo->m_strNick},
                {"bag_name", pConfig->m_strName},
                {"item_name", CLogicConfig::GetItemName(pRewardRandData->m_iItemType, pRewardRandData->m_iItemID)},
                {"item_num", std::to_string(stRspBody.m_stReward.m_iNum)}
        });
    }

    return true;
}

bool CLogicAwardProcessor::DoUserRunGetRedBagList(const msgpack::unpacked& stFormatData)
{
	auto stRspBody = LOGIC_GLOBAL_FRAME_INS->GetRedBagList(m_pUserInfo);
    stRspBody.m_stRedBagEarnings = m_pUserInfo->m_stUserDailyTable.GetRedBagEarnings();

    // 世界任务，全民红包雨
    auto iNowSec = CLogicCommon::GetSec();
    auto& stQuestState = m_pUserInfo->m_stIBusyData.GetWorldQuestRewardState();
    auto iterQuestBegin = LOGIC_GLOBAL_CACHE_INS->m_stWorldQuestMap.Begin();
    auto iterQuestEnd = LOGIC_GLOBAL_CACHE_INS->m_stWorldQuestMap.End();
    for(; iterQuestBegin != iterQuestEnd; ++iterQuestBegin)
    {
        if(iterQuestBegin->second->GetState() != CLogicConfigDefine::ECQS_Reached)
            continue;

        // 已过期
        if(iterQuestBegin->second->GetEndTime() > 0 && iterQuestBegin->second->GetEndTime() <= iNowSec)
            continue;

        auto pConfig = CLogicConfig::GetWorldQuestConfig().GetQuestConfig(iterQuestBegin->first);
        if(nullptr == pConfig) continue;

        // 未到开启时间
        if(iterQuestBegin->second->GetOpenTime() <= 0)
        {
            // 检查是否到了可领取时间
            const int32_t iOpenServerDay = day_diff(CLogicConfig::GetSystemConfig().GetOpenSvrTime(), iNowSec) + 1;
            if(pConfig->m_iOpenServerDay > iOpenServerDay)
                continue;

            iterQuestBegin->second->SetOpenTime(iNowSec);
            iterQuestBegin->second->SetEndTime(iNowSec + pConfig->m_iContinueTime);

            // 公告
            if(!pConfig->m_strBroadcastID.empty())
            {
                CLogicGlobal::PublishBroadcast(pConfig->m_strBroadcastID, 0, {{"bag_name", pConfig->m_strName}});
            }
        }

        // 判断创角时间
        if(!pConfig->m_bAfterCreateCanReceive && iterQuestBegin->second->GetOpenTime() < m_pUserInfo->m_stUserInfoTable.GetCreateTime())
        {
            continue;
        }

        if(CLogicCacheCommon::VecBitmapCheckState(stQuestState, iterQuestBegin->first))
        {
            continue;
        }

        stRspBody.m_stWorldQuestRedBag.push_back(iterQuestBegin->first);
    }

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunCheckRedBagValid(const msgpack::unpacked& stFormatData)
{
	CRequestCheckRedBagValid stReqBody;
	CResponseCheckRedBagValid stRspBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	stRspBody.m_iRedBagID = stReqBody.m_iRedBagID;
	stRspBody.m_iStatus = LOGIC_GLOBAL_FRAME_INS->GetRedBagStatus(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, stReqBody.m_iRedBagID);
	stRspBody.m_stRedBagInfo = LOGIC_GLOBAL_FRAME_INS->GetRedBagBrief(stReqBody.m_iRedBagID);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicAwardProcessor::DoUserRunSysOtherBuy(const msgpack::unpacked& stFormatData)
{
  CRequestSysOtherBuy stReq;
  CResponseSysOtherBuy stRsp;
  if (!DecodeMsgPack(stFormatData, stReq))
  {
    return false;
  }

  stRsp.m_iType = stReq.m_iType;
  stRsp.m_iPara1 = stReq.m_iPara1;
  stRsp.m_iPara2 = stReq.m_iPara2;

  if(stReq.m_iType == (int32_t)CESysOtherBuyType::CESOBT_DINNER)
  {
    // 豪华大餐补吃
    // 获取豪华大餐回购的价格
    const auto iterConfig = CLogicConfig::GetAwardConfig().m_stBuyBackMap.find(static_cast<int32_t>(CEnumBuyBackType::DINNER));
    if (iterConfig == CLogicConfig::GetAwardConfig().m_stBuyBackMap.end())
    {
      SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if(m_pUserInfo->m_stUserInfoTable.GetRmb() < iterConfig->second.m_iPrice)
    {
      SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_RMB);
    }

    if(!DinnerFixBuy(m_pUserInfo, stReq.m_iPara1, stReq.m_iPara2, stRsp.m_stBonusVec))
    {
      PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                          << "|MSG:" << CLogicError::GetErrMsg()
                          << "|ACTIVE_ID:" << stReq.m_iPara1
                          << "|INIDEX:" << stReq.m_iPara2 << std::endl;

      SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
    }

    stRsp.m_stConsumeVec.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, iterConfig->second.m_iPrice);
    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRsp.m_stConsumeVec, m_usCmd))
    {
      PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                          << "|MSG:Energy Add Success But Consume Failed!!!"
                          << "|ACTIVE_ID:" << stReq.m_iPara1
                          << "|INIDEX:" << stReq.m_iPara2 << std::endl;
      SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
    }

    if (!stRsp.m_stBonusVec.empty())
    {
      CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stBonusVec, m_usCmd);
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
  }
  else
  {
    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
  }

  return true;
}

bool CLogicAwardProcessor::DoUserRunChangeAgeLimit(const msgpack::unpacked& stFormatData)
{
    CRequestChangeAgeLimit stReq;
    CResponseChangeAgeLimit stRsp;

    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    auto iLastChangeTime = m_pUserInfo->m_stUserInfoTable.GetLastMonthAgeChange();

    if (iLastChangeTime != 0 && !LOGIC_CHECK_MONTH_REFRESH(iLastChangeTime))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AGE_LIMIT_CANT_CHANGE_INDEX);
    }


    auto ageLimitConfig = CLogicConfig::GetDepositConfig().m_stAgeLimitConfig;

    if (stReq.m_iAgeLimitIndex < 0 && stReq.m_iAgeLimitIndex > (int32_t)ageLimitConfig.size())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AGE_LIMIT_CANT_FOUND_INDEX);
    }

    m_pUserInfo->m_stUserInfoTable.SetMonthAgeLimit(stReq.m_iAgeLimitIndex);
    m_pUserInfo->m_stUserInfoTable.SetLastMonthAgeChange(CLogicCommon::GetSec());

    stRsp.m_iAgeLimitIndex = stReq.m_iAgeLimitIndex;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}


bool CLogicAwardProcessor::HaveAllHero(const std::vector<int32_t>& iHeroVec) const
{
	CLogicCacheCardOperation stOperation(m_pUserInfo);
	for (const auto iHeroID : iHeroVec)
	{
		if (iHeroID != 10 && iHeroID != 11 && !stOperation.CheckHeroCardExist(iHeroID))
		{
			return false;
		}
	}
	return true;
}

bool CLogicAwardProcessor::DinnerFixBuy(user_data_table_ptr_type pUserInfo, int32_t iActivityID, int32_t iIndex, std::vector<CPackGameItem>& stBonusList)
{
  auto stConfigIter = CLogicConfig::GetActivityConfig().m_stDinnerConfig.find(iActivityID);
  if (stConfigIter == CLogicConfig::GetActivityConfig().m_stDinnerConfig.end())
  {
    using namespace SERVER_ERRCODE;
    SET_ERR_INFO(PARAMETER_ERROR);
    return (false);
  }

  if(iIndex < 0 || iIndex >= (int32_t)stConfigIter->second.size())
  {
    using namespace SERVER_ERRCODE;
    SET_ERR_INFO(PARAMETER_ERROR);
    return (false);
  }

  const auto &stDinnerConfig = stConfigIter->second[iIndex];

  auto stIter = pUserInfo->m_stUserActiveMap.Find(iActivityID);
  if (stIter == pUserInfo->m_stUserActiveMap.End())
  {
    TActiveTableValueType stVal;
    stVal.m_iActiveID = iActivityID;
    stVal.m_iUid = pUserInfo->m_stUserInfoTable.m_stData.m_iUid;
    stVal.m_iGroupID = pUserInfo->m_stUserInfoTable.m_stData.m_iGroupID;

    auto stRet = pUserInfo->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
    stIter = stRet.first;
  }

  if(CLogicCommon::IsCurrentDay(stIter->second->GetActiveTime()) == false)
  {
    stIter->second->SetActiveReceivedBitmap(0);
    stIter->second->SetActiveTime(CLogicCommon::GetSec());
  }

  uint64_t ulBitmapMask = 0x1, ulActiveBitmap = stIter->second->GetActiveReceivedBitmap();
  ulBitmapMask = ulBitmapMask << iIndex;

  if((ulBitmapMask & ulActiveBitmap) != 0)
  {
    using namespace SERVER_ERRCODE;
    SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
    return (false);
  }

  ulActiveBitmap = ulBitmapMask | ulActiveBitmap;
  stIter->second->SetActiveReceivedBitmap(ulActiveBitmap);
  stIter->second->SetActiveTime(CLogicCommon::GetSec());

  CPackGameItem stItem;
  stItem.m_iItemType = CLogicConfigDefine::CURRENCY;
  stItem.m_iCardID = CLogicConfigDefine::ECIT_EnergyID;
  stItem.m_iNum = stDinnerConfig.m_iEnergyAmount;

  stBonusList.push_back(stItem);

  return (true);
}
