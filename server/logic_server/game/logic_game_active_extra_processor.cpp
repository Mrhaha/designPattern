#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_active_info.h"
#include "logic_config.h"
#include "logic_game_currency_check.h"
#include "logic_game_cache_common.h"
#include "logic_game_item_factory.h"
#include "logic_game_cache_level_operation.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_active_extra_processor.h"
#include "logic_game_global.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_bill.h"
#include "logic_game_common_publisher.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_BUY_FOUNDATION, CLogicActiveExtraProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_THEME_LEVEL, CLogicActiveExtraProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FIGHT_THEME_LEVEL_BONUS, CLogicActiveExtraProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SWEEP_FIGHT_THEME_LEVEL, CLogicActiveExtraProcessor)
//REGISTER_CMD_FACTORY(MSG_LOGIC_GET_LEVEL_RACE_RANK, CLogicActiveExtraProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LUCKY_BOX_LOTTERY, CLogicActiveExtraProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TURN_TABLE_LOTTERY, CLogicActiveExtraProcessor)

CLogicActiveExtraProcessor::CLogicActiveExtraProcessor(uint16_t usCmd, const std::string& strCmdName): CLogicProcessorBase(usCmd, strCmdName)
{
}

bool 
CLogicActiveExtraProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if((int)m_usCmd == MSG_LOGIC_BUY_FOUNDATION)
    {
        return (true);
        /*
		CResponseGetActiveInfo stRsp;
		CRequestGetActiveInfo stParam;

        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        auto stConfigIter = CLogicConfig::GetActivityConfig().m_stFoundationConfig.find(stParam.m_iActiveID);
        if (stConfigIter == CLogicConfig::GetActivityConfig().m_stFoundationConfig.end())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << ACTIVE_IS_NOT_IN_CONFIG
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(ACTIVE_IS_NOT_IN_CONFIG)
                                << "|ACTIVE_ID:" << stParam.m_iActiveID << std::endl;

            SEND_ERRCODE_AND_RET(ACTIVE_IS_NOT_IN_CONFIG)
        }

        auto stIter = m_pUserInfo->m_stUserActiveMap.Find(stParam.m_iActiveID);
        if(stIter == m_pUserInfo->m_stUserActiveMap.End())
        {
            TActiveTableValueType stVal;
            stVal.m_iActiveID = stParam.m_iActiveID;
            stVal.m_iUid = m_pUserInfo->m_stUserInfoTable.m_stData.m_iUid;
            stVal.m_iGroupID = m_pUserInfo->m_stUserInfoTable.m_stData.m_iGroupID;
            stVal.m_iActiveTime = 0;
            stVal.m_iActiveTargetNum = 0;
            stVal.m_ulActiveReceivedBitmap = 0;
            stVal.m_ulActiveCompleteBitmap = 0;

            auto stRet = m_pUserInfo->m_stUserActiveMap.Insert(decltype(m_pUserInfo->m_stUserActiveMap)::table_value_type(stVal.m_iActiveID, stVal));
            stIter = stRet.first;
        }

        if (stIter->second->GetActiveTargetNum() != 0)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << HAS_BUY_FOUNDATION << "|MSG:" << GET_ERRMSG_BY_ERRCODE(HAS_BUY_FOUNDATION)
                                << "|ACTIVE_ID:" << stParam.m_iActiveID << std::endl;

            SEND_ERRCODE_AND_RET(HAS_BUY_FOUNDATION)
        }

        if (m_pUserInfo->m_stUserInfoTable.GetVipLevel() < stConfigIter->second.m_iOpenVip)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_VIP_LEVEL << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_VIP_LEVEL)
                                << "|ACTIVE_ID:" << stParam.m_iActiveID << std::endl;

            SEND_ERRCODE_AND_RET(NOT_ENOUGH_VIP_LEVEL)
        }

        if (false == CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stConfigIter->second.m_stConsumeItem, m_usCmd))
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|ACTIVE_ID:" << stParam.m_iActiveID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        stIter->second->SetActiveTargetNum(1);

        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(m_pUserInfo, stParam.m_iActiveID);
        if(pActiveObjPtr.get() == nullptr)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode() << "|MSG:"
                                << CLogicError::GetErrMsg() << "|ACTIVE_ID:"
                                << stParam.m_iActiveID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        pActiveObjPtr->UpdateActiveInfo(0);

        CLogicActiveBase::active_info_t stInfo = pActiveObjPtr->GetActiveInfo();

        stRsp.m_iActiveID = stParam.m_iActiveID;
        stRsp.m_iTargetNum = stInfo.m_iTargetNum;
        stRsp.m_iLastReceiveAwardTime = stInfo.m_iLastReceiveAwardTime;
        stRsp.m_strReceivedBitmap = int2bin<uint64_t>(stInfo.m_ulReceivedBitmap);
        stRsp.m_strCompleteBitmap = int2bin<uint64_t>(stInfo.m_ulCompleteBitmap);

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        */
    }
    else if((int)m_usCmd == MSG_LOGIC_FIGHT_THEME_LEVEL)
    {
        CResponseFightLevel stRsp;
        CRequestFightLevel stParam;

        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        if(!CLogicConfigLevelParser::IsFightThemeLevel(stParam.m_iLevelID))
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(LEVEL_INFO_NOT_FOUND_IN_CONFIG);

            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        CLogicCacheCommon::AutoAddVitality(m_pUserInfo);

        auto pstConfigElem = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stParam.m_iLevelID);
        if (!pstConfigElem)
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        int iConsumeVitality = pstConfigElem->m_iBonusTeamExp;
        if (!CLogicCurrencyCheck::CheckEnoughVitality(m_pUserInfo, iConsumeVitality))
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        m_pUserInfo->m_stUserInfoTable.SetFightLevelID(stParam.m_iLevelID);
        m_pUserInfo->m_stCacheFightLevelDropBonus.clear();

        CLogicConfig::GetLevelDropConfig().GetRandBonusByName(
                pstConfigElem->m_strBonusCardBag,
                m_pUserInfo->m_stCacheFightLevelDropBonus);

        stRsp.m_iPhyPower = m_pUserInfo->m_stUserInfoTable.GetVitality();
        stRsp.m_iLastAutoAddEnergyTime = m_pUserInfo->m_stUserInfoTable.GetAddVitalityTime();
        stRsp.m_stBonusList = m_pUserInfo->m_stCacheFightLevelDropBonus;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        std::string strBonusList = CLogicCacheCommon::GetGameItemString(stRsp.m_stBonusList);
        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << stParam.m_iLevelID
                            << "|RAND_BONUS_LIST:" << strBonusList << std::endl;

        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_FIGHT_THEME_LEVEL_BONUS)
    {
        CResponseGetFightGift stRsp;
        CRequestGetFightGift stParam;

        if (!DecodeMsgPack(stFormatData, stParam)) return (false);

        if(!CLogicConfigLevelParser::IsFightThemeLevel(stParam.m_iLevelID))
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(LEVEL_INFO_NOT_FOUND_IN_CONFIG);

            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        CLogicCacheCommon::AutoAddVitality(m_pUserInfo);

        auto pstConfigElem = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stParam.m_iLevelID);
        if (!pstConfigElem)
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        auto stLevelRet = CLogicCacheLevelOperation::GetUserLevelInfo(m_pUserInfo, stParam.m_iLevelID);
        if (!stLevelRet.second)
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        auto stUserLevelIter = stLevelRet.first;

		if (m_pUserInfo->m_stUserInfoTable.GetRoleCombat() < pstConfigElem->m_iPassFightPower)
		{
			CLogicBill::SaveAntiCheatingBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, stParam.m_iLevelID
					, m_pUserInfo->m_stUserInfoTable.GetRoleCombat(), pstConfigElem->m_iPassFightPower, m_pUserInfo->m_stUserTeamInfoTable.GetTeam1(), "",1);
		}

        int iConsumeVitality = pstConfigElem->m_iBonusTeamExp;
        if (!CLogicCurrencyCheck::CheckEnoughVitality(m_pUserInfo, iConsumeVitality))
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        int iScore = 10000;
        int iStarNum = 3;
        int iCoinNum = std::min(stParam.m_iGetCoinNum, pstConfigElem->m_iMaxCoinNum);
        static std::string strBonusItem;
        CLogicCacheLevelOperation stOperation(m_pUserInfo);
        if (!stOperation.CheckRspLevelDrop(stParam.m_stBonusList))
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            stParam.m_stBonusList = m_pUserInfo->m_stCacheFightLevelDropBonus;
            // BAD_CODE
            // SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        // Evaluate
        //int iDeadCard = 0;
        //for (size_t i = 0; i < stParam.m_stCardStatus.size(); ++i)
        //{
        //    if (stParam.m_stCardStatus[i].m_iCardID == 0) continue;
		//
        //    if (stParam.m_stCardStatus[i].m_iHP <= 0) ++iDeadCard;
        //}
        //iStarNum = 3;
        //iDeadCard = iDeadCard > 2 ? 2 : iDeadCard;
        //iStarNum -= iDeadCard;

        // Three Star
        {
            CLogicConfig::GetLevelDropConfig().GetRandBonusByName(
                    pstConfigElem->m_strPerfectCardBag,
                    stRsp.m_stPerfectBonusList);

            for (const auto& stItem : stRsp.m_stPerfectBonusList)
            {
                stParam.m_stBonusList.push_back(stItem);
            }
        }

		stParam.m_stBonusList.push_back(CPackGameItem(
			CLogicConfigDefine::CURRENCY,
			CLogicConfigDefine::ECIT_GoldID,
			iCoinNum));

        ///////////////////////////////////////////////////////////////////////////
        //////////////////// Add User Game Item
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stParam.m_stBonusList, strBonusItem, m_usCmd);

        ///////////////////////////////////////////////////////////////////////////
        //////////////////// Consume Vitality
        CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_VitalityID, iConsumeVitality, m_usCmd);

        m_pUserInfo->m_stUserInfoTable.SetFightLevelID(0);

        /////////////////// Update Play Times
        if (LOGIC_CHECK_DAY_REFRESH(stUserLevelIter->second->GetLastPlayTime()))
            stUserLevelIter->second->SetPlayTimes(1);
        else
            stUserLevelIter->second->AddPlayTimes(1);

        stUserLevelIter->second->SetLastPlayTime(CLogicCommon::GetSec());

        for (int i = 0; i < iStarNum; ++i) stRsp.m_stStarNum.push_back(1);

        stRsp.m_iScore = iScore;
        stRsp.m_iCoinNum = iCoinNum;
        stRsp.m_iTeamLevel = 0;
        stRsp.m_iTeamExp = 0;
        stRsp.m_iPhyPower = m_pUserInfo->m_stUserInfoTable.GetVitality();
        stRsp.m_iLastAutoAddEnergyTime = m_pUserInfo->m_stUserInfoTable.GetAddVitalityTime();

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << stParam.m_iLevelID
                            << "|STAR:" << iStarNum << "|CONSUME_VITALITY:" << iConsumeVitality
                            << "|CURRENT_VITALITY:" << stRsp.m_iPhyPower
                            << "|BONUS:" << strBonusItem << std::endl;

        CLogicGameUserActionChannel::data_type stData;
        stData.m_iTimes = 1;
        stData.m_iData = stParam.m_iLevelID;
        stData.m_iAction = CLogicGameChannelEnum::ACTION_FIGHT_THEME_ACTIVE_LEVEL;
        LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_SWEEP_FIGHT_THEME_LEVEL)
    {
        CResponseSweepLevel stRsp;
        CRequestSweepLevel stParam;

        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);
        /*
        if(stParam.m_iTimes != 1)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(PARAMETER_ERROR);

            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if(!CLogicConfigLevelParser::IsFightThemeLevel(stParam.m_iLevelID))
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(LEVEL_INFO_NOT_FOUND_IN_CONFIG);

            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        // 是否可扫荡判断
        auto stUserLevelIter = m_pUserInfo->m_stLevelInfoTableMap.Find(stParam.m_iLevelID);
        if (stUserLevelIter == m_pUserInfo->m_stLevelInfoTableMap.End())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << FIGHT_LEVEL_NOT_RECORD
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(FIGHT_LEVEL_NOT_RECORD)
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if (LOGIC_CHECK_DAY_REFRESH(stUserLevelIter->second->GetLastPlayTime()))
        {
            stUserLevelIter->second->SetPlayTimes(0);
            stUserLevelIter->second->SetLastPlayTime(CLogicCommon::GetSec());
        }

        // 每日挑战3次后才能扫荡
        if(stUserLevelIter->second->GetPlayTimes() < 3)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << FIGHT_LEVEL_NOT_RECORD
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(FIGHT_LEVEL_NOT_RECORD)
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        CLogicCacheCommon::AutoAddVitality(m_pUserInfo);

        auto pstConfigElem = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stParam.m_iLevelID);
        if (nullptr == pstConfigElem)
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrCode()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        int iConsumeVitality = pstConfigElem->m_iBonusTeamExp * stParam.m_iTimes;
        if (!CLogicCurrencyCheck::CheckEnoughVitality(m_pUserInfo, iConsumeVitality))
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        //////////////////// Total Sweep Bonus
        static std::string strBonusItemTotal, strBonusItem;
        strBonusItemTotal = " ";
        std::vector<CPackGameItem> stBonusVec;
        CLogicCacheLevelOperation stOperation(m_pUserInfo);
        for (int i = 0; i < stParam.m_iTimes; ++i)
        {
            std::vector<CPackGameItem> helpItems;
            CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pstConfigElem->m_strBonusCardBag, helpItems);

            stRsp.m_stBonusListVec.push_back(helpItems);
            CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pstConfigElem->m_strPerfectCardBag, stRsp.m_stBonusListVec[i]);

            stBonusVec.insert(stBonusVec.end(), stRsp.m_stBonusListVec[i].begin(), stRsp.m_stBonusListVec[i].end());
        }

        CLogicCacheCommon::AddGameItem(m_pUserInfo, stBonusVec, strBonusItem, m_usCmd, {
                {"level_id", std::to_string( stParam.m_iLevelID)}, {"count", std::to_string(stParam.m_iTimes)}
        });

        strBonusItemTotal += strBonusItem + " ";

        //////////////////// Consume Vitality
        CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_VitalityID, iConsumeVitality, m_usCmd);

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << stParam.m_iLevelID << "|SWEEP_TIMES:" << stParam.m_iTimes
                            << "|LEVEL_TOTAL_BONUS:" << strBonusItemTotal << std::endl;

        CLogicGameUserActionChannel::data_type stData;
        stData.m_iTimes = stParam.m_iTimes;
        stData.m_iData = stParam.m_iLevelID;
        stData.m_iAction = CLogicGameChannelEnum::ACTION_FIGHT_THEME_ACTIVE_LEVEL;
        LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);
*/
        return (true);
    }
	else if((int)m_usCmd == MSG_LOGIC_LUCKY_BOX_LOTTERY)
	{
		CResponseLuckyBoxLottery stRsp;
		CRequestLuckyBoxLottery stParam;

		if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

		if (stParam.m_iCount <= 0 || stParam.m_iCount > 10)
		{
			using namespace SERVER_ERRCODE;
			SEND_ERRCODE_AND_RET(INTERN_ERROR)
		}

		auto iterActivity = CLogicConfig::GetActivityConfig().m_stLuckyBoxConfig.find(stParam.m_iActivityID);
		if (iterActivity == CLogicConfig::GetActivityConfig().m_stLuckyBoxConfig.end())
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

		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(m_pUserInfo, stParam.m_iActivityID);
		if (!pActiveObjPtr)
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		int32_t iHasLotteryCount = 0;
		auto stIter = m_pUserInfo->m_stUserActiveMap.Find(stParam.m_iActivityID);
		if (stIter != m_pUserInfo->m_stUserActiveMap.End())
		{
			iHasLotteryCount = stIter->second->GetActiveTargetNum();
		}

		stRsp.m_iCount = stParam.m_iCount;
		stRsp.m_iActivityID = stParam.m_iActivityID;
		std::vector<TLogicRandBonusItem> stRandBonusVec;
		for (int32_t i = 1; i <= stParam.m_iCount; ++i)
		{
			const std::string strBagName = CLogicConfig::GetActivityConfig().GetActiveCommodityBuyMallBag(stParam.m_iActivityID, iHasLotteryCount + i);
			auto stTempVec = CLogicConfig::GetCardbagConfig().GetRandBonusByName(strBagName);
			if (1 != stTempVec.size())
			{
				SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BAD_COMMODITY_ID);
			}
			stRandBonusVec.insert(stRandBonusVec.end(), stTempVec.begin(), stTempVec.end());			
		}

		for (const auto& bonus : stRandBonusVec)
		{
			stRsp.m_vtAwardItem.emplace_back(bonus.m_iItemType, bonus.m_iCardID, bonus.m_iNum);
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
		stRsp.m_stConsume.m_iNum = stParam.m_iCount == 10 ? 9 : stParam.m_iCount;
		if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRsp.m_stConsume, m_usCmd))
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_vtAwardItem, m_usCmd);

        //公告
        bool bHasBigPrize = false;
		for (const auto& bonus : stRandBonusVec)
        {
            if(bonus.m_bIsBigPrize) bHasBigPrize = true;

		    CLogicGlobal::PublishBroadcast("lucky_box", 0, bonus.m_iNotify, {
                    { "nick", m_pUserInfo->m_strNick },
                    { "item_name", CLogicConfig::GetItemName(bonus.m_iItemType, bonus.m_iCardID) },
                    { "item_count", std::to_string(bonus.m_iNum) }
		    });
        }

        // 大奖
        if(bHasBigPrize)
        {
            // 通用事件
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_LuckyHammerFinalAward;
            stEventData.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
        }

		logic_game_user_action_channel_data_t stData;
		stData.m_iTimes = stParam.m_iCount;
		stData.m_iAction = CLogicGameChannelEnum::ACTION_LUCKY_BOX;
		LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_LuckyHammerTimes;
        stEventData.m_iTimes = stParam.m_iCount;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	}
	else if ((int)m_usCmd == MSG_LOGIC_TURN_TABLE_LOTTERY)
	{
		CResponseTurnTableLottery stRsp;
		CRequestTurnTableLottery stParam;

		if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

		if (stParam.m_iCount <= 0 || stParam.m_iCount > 10)
		{
			using namespace SERVER_ERRCODE;
			SEND_ERRCODE_AND_RET(INTERN_ERROR)
		}

		auto iterActivity = CLogicConfig::GetActivityConfig().m_stTurnTableScoreConfig.find(stParam.m_iActivityID);
		if (iterActivity == CLogicConfig::GetActivityConfig().m_stTurnTableScoreConfig.end())
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

		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(m_pUserInfo, stParam.m_iActivityID);
		if (!pActiveObjPtr)
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
				<< "|MSG:" << CLogicError::GetErrMsg()
				<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

		int32_t iHasLotteryCount = 0;
		auto stIter = m_pUserInfo->m_stUserActiveMap.Find(stParam.m_iActivityID);
		if (stIter != m_pUserInfo->m_stUserActiveMap.End())
		{
			iHasLotteryCount = stIter->second->GetActiveTargetNum();
		}
		if (iterCommodity->second.m_iCountScore > 0)
		{
			iHasLotteryCount /= iterCommodity->second.m_iCountScore;
		}

		stRsp.m_iCount = stParam.m_iCount;
		stRsp.m_iActivityID = stParam.m_iActivityID;
		std::vector<TLogicRandBonusItem> stRandBonusVec;
		for (int32_t i = 1; i <= stParam.m_iCount; ++i)
		{
			const std::string strBagName = CLogicConfig::GetActivityConfig().GetActiveCommodityBuyMallBag(stParam.m_iActivityID, iHasLotteryCount + i);
			auto stTempVec = CLogicConfig::GetCardbagConfig().GetRandBonusByName(strBagName);
			if (1 != stTempVec.size())
			{
				SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BAD_COMMODITY_ID);
			}
			stRandBonusVec.insert(stRandBonusVec.end(), stTempVec.begin(), stTempVec.end());
		}

		for (const auto& bonus : stRandBonusVec)
		{
			stRsp.m_vtAwardItem.emplace_back(bonus.m_iItemType, bonus.m_iCardID, bonus.m_iNum);
		}

		if (int32_t(stRsp.m_vtAwardItem.size()) != stParam.m_iCount)
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << BAD_COMMODITY_ID
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(BAD_COMMODITY_ID)
				<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

			SEND_ERRCODE_AND_RET(BAD_COMMODITY_ID)
		}

		if (m_pUserInfo->m_stUserDailyTable.GetFreeTurnTableCount() + stParam.m_iCount <= iterCommodity->second.m_iDailyFreeCount)
		{
			//免费次数
			m_pUserInfo->m_stUserDailyTable.AddFreeTurnTableCount(stParam.m_iCount);
			CLogicUserNotifySystem::NotifyUserDailyData(m_pUserInfo);
		}
		else
		{
			stRsp.m_stConsume.m_iItemType = iterCommodity->second.m_iBuyItemType;
			stRsp.m_stConsume.m_iCardID = iterCommodity->second.m_iBuyItemID;
			stRsp.m_stConsume.m_iNum = stParam.m_iCount == 10 ? 9 : stParam.m_iCount;
			if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRsp.m_stConsume, m_usCmd))
			{
				PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
					<< "|MSG:" << CLogicError::GetErrMsg()
					<< "|ACTIVE_ID:" << stParam.m_iActivityID << std::endl;

				SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
			}
		}

		CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_vtAwardItem, m_usCmd);

		//公告
        bool bHasBigPrize = false;
		for (const auto& bonus : stRandBonusVec)
        {
            if(bonus.m_bIsBigPrize) bHasBigPrize = true;

		    CLogicGlobal::PublishBroadcast("turn_table", 0, bonus.m_iNotify, {
                    { "nick", m_pUserInfo->m_strNick },
                    { "item_name", CLogicConfig::GetItemName(bonus.m_iItemType, bonus.m_iCardID) },
                    { "item_count", std::to_string(bonus.m_iNum) }
		    });
        }

        // 大奖
        if(bHasBigPrize)
        {
            // 通用事件
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_LuckyTurnTableFinalAward;
            stEventData.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
        }

		logic_game_user_action_channel_data_t stData;
		stData.m_iTimes = stParam.m_iCount * iterCommodity->second.m_iCountScore;
		stData.m_iAction = CLogicGameChannelEnum::ACTION_TURN_TABLE;
		LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_LuckyTurnTableTimes;
        stEventData.m_iTimes = stParam.m_iCount;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	}
	/*
    else if((int)m_usCmd == MSG_LOGIC_GET_LEVEL_RACE_RANK)
    {
        CResponseGetActiveLevelRaceRank stRsp;
        CRequestGetActiveLevelRaceRank stParam;

        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        if (stParam.m_iBeginRank <= 0 || stParam.m_iNum <= 0)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << PARAMETER_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                                << "|ACTIVE_ID:" << stParam.m_iActiveID << "|BEGIN_RANK:" << stParam.m_iBeginRank
                                << "|NUM:" << stParam.m_iNum << std::endl;

            SEND_ERRCODE_AND_RET(PARAMETER_ERROR)
        }

        if (stParam.m_iNum > CLogicConfigDefine::MAX_GET_LEVEL_RACE_RANK_NUM) stParam.m_iNum = CLogicConfigDefine::MAX_GET_LEVEL_RACE_RANK_NUM;

        auto stIter = CLogicLevelRaceActive::s_stLevelRaceActive.find(stParam.m_iActiveID);
        if (stIter == CLogicLevelRaceActive::s_stLevelRaceActive.end())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << ACTIVE_IS_NOT_IN_CONFIG
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(ACTIVE_IS_NOT_IN_CONFIG)
                                << "|ACTIVE_ID:" << stParam.m_iActiveID << std::endl;

            SEND_ERRCODE_AND_RET(ACTIVE_IS_NOT_IN_CONFIG)
        }

        stRsp.m_iActiveID = stParam.m_iActiveID;
        stRsp.m_iTotalAchieve = stIter->second.m_iTotalAchieve;

        int iRequireMaxRank = stParam.m_iNum + stParam.m_iBeginRank - 1;
        for (size_t i = stParam.m_iBeginRank - 1; i < stIter->second.m_stLevelRaceRank.size() && i < (size_t)iRequireMaxRank; ++i)
        {
            CPackLevelRaceRank stElem;
            stElem.m_iTime = stIter->second.m_stLevelRaceRank[i].m_iTime;
            stElem.m_strName = stIter->second.m_stLevelRaceRank[i].m_strName;

            stRsp.m_stLevelRaceRankInfo.push_back(stElem);
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    */

    return (true);
}
