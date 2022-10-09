#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_active_protocol.h"
#include "server_user_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_game_user_stat_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_user_stat.h"
#include "common_datetime.h"
#include "logic_game_buff.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_talk_common.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_USER_STAT_LIST, CLogicUserStatProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CHECK_SERIAL_PAY_AWARD, CLogicUserStatProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_SERIAL_PAY_AWARD, CLogicUserStatProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CHECK_CAN_DEPOSIT, CLogicUserStatProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_OTHER_DEPOSIT, CLogicUserStatProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_GET_HERO_HISTORY_REWARD, CLogicUserStatProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_QUESTIONNAIRE, CLogicUserStatProcessor)

CLogicUserStatProcessor::CLogicUserStatProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicUserStatProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	if ((int)m_usCmd == MSG_LOGIC_GET_USER_STAT_LIST)
	{
		CResponseGetUserStatList stRsp;

		auto stIter = m_pUserInfo->m_stUserStatTableMap.Begin();
		for (; stIter != m_pUserInfo->m_stUserStatTableMap.End(); ++stIter)
		{
			CPackUserStatInfoElem stStatElem;
			stStatElem.m_iStatDate = stIter->second->m_stData.m_iStatDate;
			stStatElem.m_iPayMoney = stIter->second->GetPayMoney();
			stRsp.m_stUserStatVec.push_back(stStatElem);
		}

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
		return true;
	}
	else if ((int)m_usCmd == MSG_LOGIC_CHECK_SERIAL_PAY_AWARD)
	{
		CResponseCheckSerialPayAward stRsp;
		CRequestCheckSerialPayAward stParam;

		if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

		const auto pstSerialPayConfig = CLogicConfig::GetActivitySerialPayConfig().GetPayConfigByID(stParam.m_iSerialID);
		if (!pstSerialPayConfig)
		{
			stRsp.m_iAwardeStatus = 0;
			SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
			return (true);
		}

		const auto pstSerialPayAwardConfig = pstSerialPayConfig->GetAwardConfigByID(stParam.m_iBoxID);
		if (!pstSerialPayAwardConfig)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
		}

		int iAwardInt = stParam.m_iSerialID * 1000 + stParam.m_iBoxID;
		if (iAwardInt <= m_pUserInfo->m_stUserInfoTable.GetSerialPayAwarded())
		{
			//已经领取奖励
			stRsp.m_iAwardeStatus = 2;
		}
		else
		{
			//可以领取奖励
			stRsp.m_iAwardeStatus = 1;
		}

		int iBeginTime = m_pUserInfo->m_stUserInfoTable.GetCreateTime() + (pstSerialPayConfig->m_iStartDay - 1) * 86400;
		int iEndTime = m_pUserInfo->m_stUserInfoTable.GetCreateTime() + (pstSerialPayConfig->m_iStartDay + pstSerialPayConfig->m_iActivityDay - 1) * 86400;
		int iCount = CLogicUserStat::GetDailyRechargeThresholdCount(m_pUserInfo, iBeginTime, iEndTime, pstSerialPayConfig->m_iPayMoney);
		if (iCount < pstSerialPayAwardConfig->m_iPayDay)
		{
			//不能领取奖励
			stRsp.m_iAwardeStatus = 0;
		}

		for (auto iterBoxConfig = pstSerialPayConfig->m_stAwardElem.begin(); iterBoxConfig != pstSerialPayConfig->m_stAwardElem.end(); ++iterBoxConfig)
		{
			int32_t iStatus = 0;
			iAwardInt = stParam.m_iSerialID * 1000 + iterBoxConfig->m_iBoxID;
			if (iAwardInt <= m_pUserInfo->m_stUserInfoTable.GetSerialPayAwarded())
			{
				//已经领取奖励
				iStatus = 2;
			}
			else
			{
				//可以领取奖励
				iStatus = 1;
			}
			if (iCount < iterBoxConfig->m_iPayDay)
			{
				//不能领取奖励
				iStatus = 0;
			}
			stRsp.m_aiBoxStatus.push_back(iStatus);
		}

		stRsp.m_bIsTodayPay = CLogicUserStat::isTodayRechargeThreshold(m_pUserInfo, pstSerialPayConfig->m_iPayMoney);

		stRsp.m_iSerialID = stParam.m_iSerialID;
		stRsp.m_iBoxID = stParam.m_iBoxID;

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
		return (true);
	}
	else if ((int)m_usCmd == MSG_LOGIC_GET_SERIAL_PAY_AWARD)
	{
		CResponseGetSerialPayAward stRsp;
		CRequestGetSerialPayAward stParam;

		if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

		const auto pstSerialPayConfig = CLogicConfig::GetActivitySerialPayConfig().GetPayConfigByID(stParam.m_iSerialID);
		if (!pstSerialPayConfig)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
		}

		const auto pstSerialPayAwardConfig = pstSerialPayConfig->GetAwardConfigByID(stParam.m_iBoxID);
		if (!pstSerialPayAwardConfig)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
		}

		int iBeginTime = m_pUserInfo->m_stUserInfoTable.GetCreateTime() + (pstSerialPayConfig->m_iStartDay - 1) * 86400;
		int iEndTime = m_pUserInfo->m_stUserInfoTable.GetCreateTime() + (pstSerialPayConfig->m_iStartDay + pstSerialPayConfig->m_iActivityDay - 1) * 86400;
		int iCount = CLogicUserStat::GetDailyRechargeThresholdCount(m_pUserInfo, iBeginTime, iEndTime, pstSerialPayConfig->m_iPayMoney);
		if (iCount < pstSerialPayAwardConfig->m_iPayDay)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
		}

		int iAwardInt = stParam.m_iSerialID * 1000 + stParam.m_iBoxID;
		if (iAwardInt <= m_pUserInfo->m_stUserInfoTable.GetSerialPayAwarded())
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
		}

		int32_t iAwardSerialID = m_pUserInfo->m_stUserInfoTable.GetSerialPayAwarded() / 1000;
		if (stParam.m_iSerialID == iAwardSerialID && m_pUserInfo->m_stUserInfoTable.GetSerialPayAwarded() + 1 != iAwardInt)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_CONDITION_IS_NOT_REACHED);
        }

        if (stParam.m_iSerialID != iAwardSerialID && stParam.m_iBoxID != 1)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_CONDITION_IS_NOT_REACHED);
        }

		stRsp.m_iSerialID = stParam.m_iSerialID;
		stRsp.m_iBoxID = stParam.m_iBoxID;
		stRsp.m_stBonusVec = pstSerialPayAwardConfig->m_stPackageElem;
		stRsp.m_iSerialPayAwarded = iAwardInt;

		static std::string strBonus;
		CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stBonusVec, strBonus, m_usCmd);

		m_pUserInfo->m_stUserInfoTable.SetSerialPayAwarded(iAwardInt);

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|GET_SERIAL_PAY_AWARD:" << "|BONUS_ITEM:" << strBonus << std::endl;

		return (true);
	}
	else if (m_usCmd == MSG_LOGIC_CHECK_CAN_DEPOSIT)
	{
		CRequestCheckCanDeposit stReqBody;
		CResponseCheckCanDeposit stRspBody;

		if (!DecodeMsgPack(stFormatData, stReqBody))
		{
			return false;
		}

		stRspBody.m_iDepositID = stReqBody.m_iDepositID;

        const auto &attrConfig = CLogicConfig::GetAntiAddictionConfig().m_stAntiAddictionAttr;
        if(attrConfig.m_byMainSwitch && attrConfig.m_nAgeLimit > m_pUserInfo->m_stUserInfoTable.GetAge())
        {
            auto serverConfig = CLogicConfig::GetAntiAddictionConfig().GetServerAntiAddictionConfig(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID);
            if(serverConfig && serverConfig->m_byTimeLimitSwitch)
            {
                // 防沉迷判断
                for(auto& config : CLogicConfig::GetAntiAddictionConfig().m_stAntiAddRechargeNotify)
                {
                    if(config.m_iAgeMin <= m_pUserInfo->m_stUserInfoTable.GetAge() && m_pUserInfo->m_stUserInfoTable.GetAge() <= config.m_iAgeMax)
                    {
                        const auto pstDeposit = CLogicConfig::GetDepositConfig().GetDepositConfig(stReqBody.m_iDepositID);
                        if (nullptr != pstDeposit)
                        {
                            if(pstDeposit->m_dPrice > config.m_iSingleRecharge)
                            {
                                // 发送提示
                                CCommonNotify proto;
                                proto.m_iType = (int)CCommonNotify::ECCN_AntiAddiction;
                                proto.m_strNotify = config.m_strSingleNotify;
                                GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, MSG_LOGIC_COMMON_NOTIFY, proto);

                                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::EMPTY_NOTICE);
                            }

                            // 每月累计充值
                            int iMonthAddUpRmb = 0;
                            const int iLastDepositTime = m_pUserInfo->m_stUserInfoTable.GetLastDepositTime();
                            if(!LOGIC_CHECK_MONTH_REFRESH(iLastDepositTime))
                            {
                                iMonthAddUpRmb = m_pUserInfo->m_stUserInfoTable.GetMonthAddUpRmb();
                            }

                            if(pstDeposit->m_dPrice + iMonthAddUpRmb > config.m_iMonthRecharge)
                            {
                                // 发送提示
                                CCommonNotify proto;
                                proto.m_iType = (int)CCommonNotify::ECCN_AntiAddiction;
                                proto.m_strNotify = config.m_strMonthNotify;
                                GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, MSG_LOGIC_COMMON_NOTIFY, proto);

                                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::EMPTY_NOTICE);
                            }
                        }

                        break;
                    }
                }
            }
        }

		stRspBody.m_bCanBuy = CLogicUserStat::GetLeftRechargeBuyCount(m_pUserInfo, stReqBody.m_iDepositID) > 0;

		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);

		return (true);
	}
	else if(m_usCmd == MSG_LOGIC_TOUHOU_OTHER_DEPOSIT)
    {
	    CRequestFreeDeposit stReq;
	    CResponseFreeDeposit stRsp;

        if (!DecodeMsgPack(stFormatData, stReq))
        {
            return false;
        }

        const auto pConfig = CLogicConfig::GetDepositConfig().GetDepositConfig(stReq.m_iDepositID);
        if(pConfig == nullptr)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
        }

        if(pConfig->m_iType != CLogicConfigDefine::DEPOSIT_TYPE_OTHER_DEPOSIT)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        const int32_t iLeftCount = CLogicUserStat::GetLeftRechargeBuyCount(m_pUserInfo, stReq.m_iDepositID);
        if(iLeftCount <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
        }

        if(pConfig->m_stConsumeItem.m_iItemType > 0)
        {
            if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pConfig->m_stConsumeItem, m_usCmd))
            {
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                    << "|MSG:" << CLogicError::GetErrMsg()
                                    << "|DEPOSIT_ID:" << stReq.m_iDepositID << std::endl;

                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }
        }

        auto stDepositIter = m_pUserInfo->m_stUserDepositMap.Find(stReq.m_iDepositID);
        if (stDepositIter == m_pUserInfo->m_stUserDepositMap.End())
        {
            TUserDepositTableValueType stVal;

            stVal.m_iOrderID = stReq.m_iDepositID;
            stVal.m_iUid = m_iUin;
            stVal.m_iGroupID = m_iGroupID;
            stVal.m_iBuyTimes = 0;
            stVal.m_iTodayBuyTimes = 0;
            stVal.m_iWeekBuyTimes = 0;
            stVal.m_iMonthBuyTimes = 0;
            stVal.m_iDayIntervalBuyTimes = 0;
            stVal.m_iDoubleTimes = 0;
            stVal.m_iLastDepositTime = 0;
            const auto stRet = m_pUserInfo->m_stUserDepositMap.Insert(std::make_pair(stReq.m_iDepositID, stVal));
            if (!stRet.second)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
            }

            stDepositIter = stRet.first;
        }

        stDepositIter->second->AddBuyTimes(1);
        stDepositIter->second->AddTodayBuyTimes(1);
        stDepositIter->second->AddWeekBuyTimes(1);
        stDepositIter->second->AddMonthBuyTimes(1);
        stDepositIter->second->AddDayIntervalBuyTimes(1);
        if(pConfig->m_iDayIntervalNum <= 0 || day_diff(stDepositIter->second->GetLastDepositTime(), CLogicCommon::GetSec()) >= pConfig->m_iDayIntervalNum)
        {
            stDepositIter->second->SetLastDepositTime(CLogicCommon::GetSec());
        }

        stRsp.m_stItemList.emplace_back(pConfig->m_iBuyItemType, pConfig->m_iBuyItemID, 1);

        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stItemList, m_usCmd);

        CLogicCacheCommon::AddGameItem(m_pUserInfo,
                                       CLogicConfigDefine::CURRENCY,
                                       CLogicConfigDefine::ECIT_YuanBaoID,
                                       pConfig->m_iNormalBuyAmount,
                                       m_usCmd);

        CLogicCacheCommon::AddGameItem(m_pUserInfo,
                                       CLogicConfigDefine::CURRENCY,
                                       CLogicConfigDefine::ECIT_VIPExpID,
                                       pConfig->m_iVipExp,
                                       m_usCmd);

        stRsp.m_iDepositID = stReq.m_iDepositID;
        stRsp.m_iVipExp = m_pUserInfo->m_stUserInfoTable.GetVipExp();

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	    return (true);
    }
	else if(m_usCmd == MSG_LOGIC_TOUHOU_GET_HERO_HISTORY_REWARD)
    {
	    CRequestGetHeroHistoryReward stReq;
	    CResponseGetHeroHistoryReward stRsp;

        if (!DecodeMsgPack(stFormatData, stReq))
        {
            return false;
        }

        auto iter = m_pUserInfo->m_stHeroHistoryMap.find(stReq.m_iCardID);
        if(iter == m_pUserInfo->m_stHeroHistoryMap.end())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_MEET_CONDITION);
        }

        auto iterHistory = iter->second.Find(stReq.m_iIndex);
        if(iterHistory == iter->second.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_MEET_CONDITION);
        }

        const auto iterConfig = CLogicConfig::GetHeroHeartConfig().m_stHistoryMap.find(stReq.m_iIndex);
        if(iterConfig == CLogicConfig::GetHeroHeartConfig().m_stHistoryMap.end() || iterConfig->second.m_stRewards.empty())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
        }

        if(iterHistory->second->GetRewardTag())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::AWARD_HAVE_RECEIVED);
        }

        stRsp.m_stReward = iterConfig->second.m_stRewards;
        stRsp.m_iCardID = stReq.m_iCardID;

        std::string strBonus;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stReward, strBonus, m_usCmd);

        iterHistory->second->SetRewardTag(1);

        stRsp.m_stHistory.m_iIndex = iterHistory->first;
        stRsp.m_stHistory.m_iContentIndex = iterHistory->second->GetContentIndex();
        stRsp.m_stHistory.m_iUnlockTime = iterHistory->second->GetUnlockTime();
        stRsp.m_stHistory.m_iRewardTag = iterHistory->second->GetRewardTag();

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|CARD_ID:" << stReq.m_iCardID << "|INDEX:" << stReq.m_iIndex << "|BONUS_ITEM:" << strBonus << std::endl;

	    return true;
    }
    else if(m_usCmd == MSG_LOGIC_TOUHOU_QUESTIONNAIRE)
    {
        CRequestQuestionnaire stReq;
        CResponseQuestionnaire stRsp;

        if (!DecodeMsgPack(stFormatData, stReq))
        {
            return false;
        }

        auto pConfig = CLogicConfig::GetQuestionnaireConfig().GetConfig(stReq.m_iID);
        if(nullptr == pConfig)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
        }

        auto stState = m_pUserInfo->m_stIBusyData.GetQuestionnaireState();
        if(CLogicCacheCommon::VecBitmapCheckState(stState, stReq.m_iID))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QUESTIONNAIRE_TIME_OUT);
        }

        if(pConfig->m_stQuestionList.size() != stReq.m_stAnswers.size())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QUESTIONNAIRE_PROBLEM_NOT_ENOUGH);
        }

        std::vector<std::pair<std::string, std::string>> stAnswers;
        for(std::size_t index = 0; index < pConfig->m_stQuestionList.size(); ++index)
        {
            const auto& stConfigProblem = pConfig->m_stQuestionList[index];
            auto& stUserAnswer = stReq.m_stAnswers[index];

            // 检查输入内容长度
            if(!stUserAnswer.m_strContent.empty() && !CLogicTalk::CheckUTF8ContentLength(stUserAnswer.m_strContent, CLogicConfigQuestionnaireParser::E_ContentMaxLength))
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TALK_WORD_COUNT_REACH_LIMIT);
            }
            //检查非法字符
            if (!stUserAnswer.m_strContent.empty() && LOGIC_SERVICE_API_INS->CheckNickHasDirtyWord(stUserAnswer.m_strContent))
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::CONTENT_HAS_ILLEGAL_CHAR);
            }
            std::string strSelect = "";
            std::string strContent = "";
            if(stConfigProblem.m_iType == CLogicConfigDefine::EQPT_SingleChoice || stConfigProblem.m_iType == CLogicConfigDefine::EQPT_MultipleChoice)
            {
                // 选择题
                if(!stConfigProblem.m_bNotMustAnswer && stUserAnswer.m_stIndexVec.empty())
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QUESTIONNAIRE_PROBLEM_NOT_ENOUGH);
                }

                if(stUserAnswer.m_stIndexVec.size() > stConfigProblem.m_stAnswerList.size())
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
                }

                if(stConfigProblem.m_iType == CLogicConfigDefine::EQPT_SingleChoice && stUserAnswer.m_stIndexVec.size() > 1)
                {
                    stUserAnswer.m_stIndexVec.resize(1);
                }

                for(auto answer : stUserAnswer.m_stIndexVec)
                {
                    if(answer < 1 || answer > (int32_t)stConfigProblem.m_stAnswerList.size())
                    {
                        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
                    }

                    strSelect += 'A' + answer - 1;

                    // 带'其他'选项的选择题
                    if(answer == stConfigProblem.m_iOtherIndex)
                    {
                        strContent = stUserAnswer.m_strContent;
                    }
                }
            }
            else if(stConfigProblem.m_iType == CLogicConfigDefine::EQPT_Text)
            {
                // 自由输入题
                if(!stConfigProblem.m_bNotMustAnswer && stUserAnswer.m_strContent.empty())
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QUESTIONNAIRE_PROBLEM_NOT_ENOUGH);
                }

                strContent = stUserAnswer.m_strContent;
            }
            else if(stConfigProblem.m_iType == CLogicConfigDefine::EQPT_BirthYear)
            {
                // 年份选择题
                if(!stConfigProblem.m_bNotMustAnswer && stUserAnswer.m_strContent.empty())
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QUESTIONNAIRE_PROBLEM_NOT_ENOUGH);
                }

                int32_t iYear = ::atoi(stUserAnswer.m_strContent.c_str());
                if(!stUserAnswer.m_strContent.empty())
                {
                    if(iYear < 1900 || iYear > 9999)
                    {
                        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QUESTIONNAIRE_PROBLEM_NOT_ENOUGH);
                    }
                }

                strContent = std::to_string(iYear);
            }
            else
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
            }

            stAnswers.emplace_back(strSelect, strContent);
        }

        // 保存到统计库
        CLogicBill::SaveGameQuestionnaire(m_iUin, m_iGroupID, m_pUserInfo->m_iChannelID, stReq.m_iID, m_pUserInfo->m_stUserInfoTable.GetLevel()
                , m_pUserInfo->m_stUserInfoTable.GetVipLevel(), m_pUserInfo->m_stUserInfoTable.GetRoleCombatMax(), m_pUserInfo->m_strNick, stAnswers);

        stRsp.m_iID = stReq.m_iID;
        stRsp.m_stRewards = pConfig->m_stRewards;

        CLogicCacheCommon::VecBitmapSetState(stState, stReq.m_iID);
        m_pUserInfo->m_stIBusyData.SetQuestionnaireState(stState);

        std::string strBonus;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stRewards, strBonus, m_usCmd);

        auto stDuration = m_pUserInfo->m_stIBusyData.GetQuestionnaireDurationTime();
        auto iterDuration = stDuration.find(stReq.m_iID);
        if(iterDuration != stDuration.end())
        {
            stDuration.erase(iterDuration);
            m_pUserInfo->m_stIBusyData.SetQuestionnaireDurationTime(stDuration);
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|QUESTIONNAIRE_ID:" << stReq.m_iID << "|BONUS_ITEM:" << strBonus << std::endl;

        return true;
    }

    return (true);
}
