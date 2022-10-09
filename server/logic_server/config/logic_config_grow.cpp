#include "logic_config_grow.h"
#include "Markup.h"
#include "server_errcode.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_config.h"

////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigUserExpParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	do
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			break;
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		if (false == stXML.FindElem("user_exp"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		m_stUserExpConfig.push_back(TLogicUserExpElem());
		while (true == stXML.FindElem("exp"))
		{
			TLogicUserExpElem stUserExpElem;

			stUserExpElem.m_iExpValue = ::atoi(stXML.GetAttrib("exp_value").c_str());
			stUserExpElem.m_iAddPhyPower = ::atoi(stXML.GetAttrib("add_stamina").c_str());
			stUserExpElem.m_iCardLevelLimit = ::atoi(stXML.GetAttrib("card_level_limit").c_str());

			m_stUserExpConfig.push_back(stUserExpElem);
		}

	} while (0);

	if (strErrorInfo.str().empty())
	{
		return (std::pair<bool, std::string>(true, "OK"));
	}
	else
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
}

std::pair<int, int>
CLogicConfigUserExpParser::ReCalcUserLevelExp(int iLevel, int iExp) const
{
	while ((unsigned int)iLevel < m_stUserExpConfig.size() - 1 && m_stUserExpConfig[iLevel].m_iExpValue <= iExp)
	{
		iExp -= m_stUserExpConfig[iLevel++].m_iExpValue;
	}

	if ((iLevel == (int)m_stUserExpConfig.size() - 1) && (iExp > m_stUserExpConfig[iLevel].m_iExpValue))
	{
		iExp = m_stUserExpConfig[iLevel].m_iExpValue;
	}

	return (std::make_pair(iLevel, iExp));
}

///////////////////////////////////////////////////////////////////////////////////////////////
static void ParserMutiStepTaskXML(enum CLogicConfigDefine::TASK_TYPE m_eumTaskType,
	CMarkup& stXML, MapLogicMutiStepTaskConfig& stConfig,
	std::stringstream& strErrorInfo)
{
	do
	{
		while (stXML.FindElem("quests"))
		{
			TLogicMutiStepTaskConfigElem stMutiStepTaskElem;
			stMutiStepTaskElem.m_eumTaskType = m_eumTaskType;
			stMutiStepTaskElem.m_iTaskID = atoi(stXML.GetAttrib("quest_type").c_str());

			stXML.IntoElem();
			while (stXML.FindElem("tags"))
			{
				if (stXML.GetAttrib("tag") == "main")
				{
					/*
					stXML.IntoElem();
					stMutiStepTaskElem.m_iOpenTaskUserLevelLimit = 1;
					if(stXML.FindElem("main"))
					{
					*/
					stMutiStepTaskElem.m_iOpenTaskUserLevelLimit = atoi(stXML.GetAttrib("player_level").c_str());
					stMutiStepTaskElem.m_bIsCheckAfterAccept = atoi(stXML.GetAttrib("check_after_accept").c_str()) == 1;
					/*
					}
					stXML.OutOfElem();
					*/
				}
				else
				{
					stXML.IntoElem();
					while (stXML.FindElem("step"))
					{
                        TLogicMutiStepTaskConfigStepDetail stStepElem;
                        stStepElem.m_iTargetID = atoi(stXML.GetAttrib("target").c_str());
                        stStepElem.m_iTargetAmount = atoi(stXML.GetAttrib("target_amount").c_str());

                        char szItemType[64], szAward[64], szAmount[64];
                        for(int iPos = 1; iPos <= 5; ++iPos)
                        {
                            snprintf(szItemType, sizeof(szItemType), "item_type%d", iPos);
                            if (true == stXML.GetAttrib(szItemType).empty())
                                break;
                            snprintf(szAward, sizeof(szAward), "award_%d", iPos);
                            snprintf(szAmount, sizeof(szAmount), "amount_%d", iPos);
                            CPackGameItem stBonus;
                            stBonus.m_iItemType = ::atoi(stXML.GetAttrib(szItemType).c_str());
                            stBonus.m_iCardID = ::atoi(stXML.GetAttrib(szAward).c_str());
                            stBonus.m_iNum = ::atoi(stXML.GetAttrib(szAmount).c_str());
                            stStepElem.m_stBonusItemList.push_back(stBonus);
                        }

						stMutiStepTaskElem.m_stStepDetail.push_back(stStepElem);
					}
					stXML.OutOfElem();
				}
			}
			stXML.OutOfElem();

			stConfig[stMutiStepTaskElem.m_iTaskID] = stMutiStepTaskElem;
		}
	} while (0);
}

std::pair<bool, std::string>
CLogicConfigMainTaskParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	do
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			break;
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		if (false == stXML.FindElem("main_line_data"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();
		ParserMutiStepTaskXML(CLogicConfigDefine::MAIN_TASK, stXML, m_stConfig, strErrorInfo);
	} while (0);

	if (strErrorInfo.str().empty())
	{
		return (std::pair<bool, std::string>(true, "OK"));
	}
	else
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
const TLogicSerialPayAwardConfigElem*
TLogicSerialPayConfigElem::GetAwardConfigByID(int iBoxID) const
{
	for (auto pstAwardConfig = m_stAwardElem.begin(); pstAwardConfig != m_stAwardElem.end(); ++pstAwardConfig)
	{
		if (pstAwardConfig->m_iBoxID == iBoxID)
		{
			return &(*pstAwardConfig);
		}
	}

	return nullptr;
}

const TLogicSerialPayConfigElem*
CLogicConfigActivitySerialPayParser::GetPayConfigByID(int iSerialID) const
{
	for (auto pstPayConfig = m_stSerialPayConfig.begin(); pstPayConfig != m_stSerialPayConfig.end(); ++pstPayConfig)
	{
		if (pstPayConfig->m_iSerialID == iSerialID)
		{
			return &(*pstPayConfig);
		}
	}

	return nullptr;
}

std::pair<bool, std::string>
CLogicConfigActivitySerialPayParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	do
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			break;
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		if (false == stXML.FindElem("serial_pay"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		while (stXML.FindElem("serial"))
		{
			TLogicSerialPayConfigElem stSerialPayElem;
			stSerialPayElem.m_iSerialID = ::atoi(stXML.GetAttrib("id").c_str());

			stXML.IntoElem();

			if (false == stXML.FindElem("serial_info"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				break;
			}

			stSerialPayElem.m_iStartDay = ::atoi(stXML.GetAttrib("start_time").c_str());
			stSerialPayElem.m_iActivityDay = ::atoi(stXML.GetAttrib("activity_day").c_str());
			stSerialPayElem.m_iPayMoney = ::atoi(stXML.GetAttrib("pay_money").c_str());
			if (stSerialPayElem.m_iPayMoney <= 0)
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "serial_pay pay_money <= 0" << std::endl;
				break;
			}


			if (false == stXML.FindElem("reward"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				break;
			}

			stXML.IntoElem();
			while (stXML.FindElem("box"))
			{
				TLogicSerialPayAwardConfigElem stPayAwardElem;
				stPayAwardElem.m_iBoxID = ::atoi(stXML.GetAttrib("id").c_str());

				stXML.IntoElem();

				if (true == stXML.FindElem("info"))
				{
					stPayAwardElem.m_iPayDay = ::atoi(stXML.GetAttrib("pay_day").c_str());
					if (stPayAwardElem.m_iPayDay <= 0)
					{
						strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "box info pay_day <= 0" << std::endl;
						break;
					}
				}
				else
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
					break;
				}

				if (false == stXML.FindElem("items"))
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
					break;
				}

				stXML.IntoElem();
				while (stXML.FindElem("item_id"))
				{
					CPackGameItem stGameItemElem;

					stGameItemElem.m_iCardID = ::atoi(stXML.GetAttrib("item_id").c_str());
					stGameItemElem.m_iItemType = ::atoi(stXML.GetAttrib("item_type").c_str());
					stGameItemElem.m_iNum = ::atoi(stXML.GetAttrib("num").c_str());

					if (stGameItemElem.m_iCardID <= 0 || stGameItemElem.m_iItemType <= 0 || stGameItemElem.m_iNum <= 0)
					{
						strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "item id or type or num <= 0" << std::endl;
						break;
					}

					stPayAwardElem.m_stPackageElem.push_back(stGameItemElem);
				}
				stXML.OutOfElem();

				stSerialPayElem.m_stAwardElem.push_back(stPayAwardElem);
				stXML.OutOfElem();
			}
			stXML.OutOfElem();

			m_stSerialPayConfig.push_back(stSerialPayElem);

			stXML.OutOfElem();
		}

		stXML.OutOfElem();
	} while (0);

	if (strErrorInfo.str().empty())
	{
		return (std::pair<bool, std::string>(true, "OK"));
	}
	else
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
}

std::pair<bool, std::string> CLogicConfigMoneyTreeExpParser::Load(const std::string& strFileName, const std::string& strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if (strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if (!stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        if (!stXML.FindElem("level_exp"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        while (stXML.FindElem("exp"))
        {
            int32_t iTotalExp = atoi(stXML.GetAttrib("total_exp").c_str());
            CONFIG_ASSERT_EXCEPTION(iTotalExp >= 0);
            int32_t iBaseMoney = atoi(stXML.GetAttrib("base_money").c_str());
            CONFIG_ASSERT_EXCEPTION(iBaseMoney > 0);

            m_stExpMap[iTotalExp] = iBaseMoney;
        }
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
    return (std::pair<bool, std::string>(true, "OK"));
}

int32_t CLogicConfigMoneyTreeExpParser::GetBaseMoney(int32_t iTotalExp) const
{
    auto iterUpper = m_stExpMap.upper_bound(iTotalExp);
    if(iterUpper == m_stExpMap.begin())
        return iterUpper->second;

    --iterUpper;
    return iterUpper->second;
}

std::pair<bool, std::string> CLogicConfigQuestParser::Load(const std::string& strFileName, const std::string& strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if (strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if (!stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"));

        stXML.IntoElem();

        while (stXML.FindElem("quest"))
        {
            stXML.IntoElem();
            while (stXML.FindElem("info"))
            {
                TLogicQuestElem stElem;
                stElem.m_iQuestID = ::atoi(stXML.GetAttrib("id").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(m_stQuestMap.find(stElem.m_iQuestID) == m_stQuestMap.end(), stElem.m_iQuestID);

                stElem.m_iType = ::atoi(stXML.GetAttrib("type").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumQuestType(stElem.m_iType), stElem.m_iQuestID);

                stElem.m_iLevelLimit = ::atoi(stXML.GetAttrib("level_limit").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iLevelLimit >= 0, stElem.m_iQuestID);
                stElem.m_iAvyID = ::atoi(stXML.GetAttrib("avy_id").c_str());
                if(stElem.m_iAvyID > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetAvyTimeConfig().GetConfig(stElem.m_iAvyID), stElem.m_iQuestID)
                }

                stElem.m_bNoEarlyRecord = (bool)::atoi(stXML.GetAttrib("no_early_record").c_str());
                stElem.m_iOpenHour = ::atoi(stXML.GetAttrib("open_hour").c_str());
                stElem.m_bIsMult = (bool)::atoi(stXML.GetAttrib("is_mult").c_str());
                stElem.m_bIsDesc = (bool)::atoi(stXML.GetAttrib("is_desc").c_str());
                stElem.m_iCondIndex = ::atoi(stXML.GetAttrib("cond_index").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iCondIndex >= 0, stElem.m_iQuestID)
                stElem.m_iEventType = ::atoi(stXML.GetAttrib("event_type").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCommonEventType(stElem.m_iEventType), stElem.m_iQuestID)

                // 编组任务
                if(stElem.m_iEventType == CLogicConfigDefine::ECET_QuestCondIndexTimes)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iCondIndex == 0, stElem.m_iQuestID)
                    CONFIG_ASSERT_EXCEPTION_EX(stElem.m_bIsMult == false, stElem.m_iQuestID)
                }

                if(false == stElem.m_bIsMult)
                {
                    TLogicQuestEventPara stPara;
                    stPara.m_iEventPara1 = ::atoi(stXML.GetAttrib("event_para1").c_str());
                    stPara.m_iEventPara2 = ::atoi(stXML.GetAttrib("event_para2").c_str());
                    stPara.m_iEventTarget = ::atoi(stXML.GetAttrib("event_target").c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(stPara.m_iEventPara1 >= 0, stElem.m_iQuestID)
                    CONFIG_ASSERT_EXCEPTION_EX(stPara.m_iEventPara2 >= 0, stElem.m_iQuestID)
                    CONFIG_ASSERT_EXCEPTION_EX(stPara.m_iEventTarget > 0, stElem.m_iQuestID)

                    std::string strList = stXML.GetAttrib("event_para_list");
                    auto stDataList = string_split(strList, "|");
                    for(auto& para : stDataList)
                    {
                        int iPara = ::atoi(para.c_str());
                        CONFIG_ASSERT_EXCEPTION_EX(iPara > 0, stElem.m_iQuestID)
                        stPara.m_stParaSet.insert(iPara);
                    }
                    CONFIG_ASSERT_EXCEPTION_EX(stPara.m_stParaSet.size() < 30, stElem.m_iQuestID)

                    for(int i = 1; i <= 10; ++i)
                    {
                        std::stringstream stStream;
                        stStream << i;
                        if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                        CPackGameItem stItem;
                        stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                        stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                        stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                        CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), stElem.m_iQuestID);

                        stPara.m_stReward.push_back(stItem);
                    }

                    stElem.m_stEventVec.emplace_back(std::move(stPara));
                }

                m_stQuestMap[stElem.m_iQuestID] = stElem;
                m_stEventMap.insert(std::make_pair(stElem.m_iEventType, &(m_stQuestMap[stElem.m_iQuestID])));

                if(stElem.m_iType == CLogicConfigDefine::EQT_DailyQuest)
                {
                    m_stDailyQuestList.push_back(stElem.m_iQuestID);
                }
                else if (stElem.m_iType == CLogicConfigDefine::EQT_NEWPass_DAILY)
                {
                    m_stPassDailyQuestList.push_back(stElem.m_iQuestID);
                }
                else if (stElem.m_iType == CLogicConfigDefine::EQT_NewPass_Weekly)
                {
                    m_stPassWeeklyQuestList.push_back(stElem.m_iQuestID);
                }
                else if (stElem.m_iType == CLogicConfigDefine::EQT_NewPass_Monthly)
                {
                    m_stPassMonthlyQuestList.push_back(stElem.m_iQuestID);
                }
                else if (stElem.m_iType == CLogicConfigDefine::EQT_NewPvp_Weekly)
                {
                    m_stPvpWeeklyQuestList.push_back(stElem.m_iQuestID);
                }
            }
            stXML.OutOfElem();
        }

        while (stXML.FindElem("mult"))
        {
            int iQuestID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(m_stQuestMap.find(iQuestID) != m_stQuestMap.end(), iQuestID);

            auto& stQuestElem = m_stQuestMap[iQuestID];
            CONFIG_ASSERT_EXCEPTION_EX(stQuestElem.m_bIsMult, iQuestID);

            stXML.IntoElem();
            while (stXML.FindElem("index"))
            {
                TLogicQuestEventPara stPara;
                stPara.m_iEventPara1 = ::atoi(stXML.GetAttrib("event_para1").c_str());
                stPara.m_iEventPara2 = ::atoi(stXML.GetAttrib("event_para2").c_str());
                stPara.m_iEventTarget = ::atoi(stXML.GetAttrib("event_target").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stPara.m_iEventPara1 >= 0, iQuestID)
                CONFIG_ASSERT_EXCEPTION_EX(stPara.m_iEventPara2 >= 0, iQuestID)
                CONFIG_ASSERT_EXCEPTION_EX(stPara.m_iEventTarget > 0, iQuestID)

                std::string strList = stXML.GetAttrib("event_para_list");
                auto stDataList = string_split(strList, "|");
                for(auto& para : stDataList)
                {
                    int iPara = ::atoi(para.c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(iPara > 0, iQuestID)
                    stPara.m_stParaSet.insert(iPara);
                }
                CONFIG_ASSERT_EXCEPTION_EX(stPara.m_stParaSet.size() < 30, iQuestID)

                for(int i = 1; i <= 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                    CPackGameItem stItem;
                    stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                    stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                    stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, iQuestID);

                    stPara.m_stReward.push_back(stItem);
                }

                stQuestElem.m_stEventVec.emplace_back(std::move(stPara));
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION_EX(false == stQuestElem.m_stEventVec.empty(), iQuestID);
        }

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicQuestElem* CLogicConfigQuestParser::GetConfig(int32_t iID) const
{
    auto iter = m_stQuestMap.find(iID);
    return iter == m_stQuestMap.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string> CLogicConfigBadgeParser::Load(const std::string& strFileName, const std::string& strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if (strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if (!stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"));

        stXML.IntoElem();

        while (stXML.FindElem("badge"))
        {
            TLogicBadgeCondElem stElem;
            stElem.m_iBadgeID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iBadgeID > 0, stElem.m_iBadgeID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stBadgeMap.find(stElem.m_iBadgeID) == m_stBadgeMap.end(), stElem.m_iBadgeID)

            stElem.m_iType = ::atoi(stXML.GetAttrib("cond_type").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumBadgeCondType(stElem.m_iType), stElem.m_iBadgeID)

            std::string strPara = stXML.GetAttrib("para");
            auto stList = string_split(strPara, "|");
            if(stList.size() > 1)
            {
                for(auto& para : stList)
                {
                    int iPara = ::atoi(para.c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(iPara > 0, stElem.m_iBadgeID)
                    stElem.m_stParaSet.insert(iPara);
                }
            }
            else
            {
                stElem.m_iPara = ::atoi(strPara.c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iPara >= 0, stElem.m_iBadgeID)
            }

            stElem.m_bIsAll = (bool)::atoi(stXML.GetAttrib("is_all").c_str());

            strPara = stXML.GetAttrib("team_card");
            if(!strPara.empty())
            {
                stList = string_split(strPara, "|");
                for(auto& card : stList)
                {
                    int iCardID = ::atoi(card.c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetHeroCardConfig().GetConfig(iCardID), stElem.m_iBadgeID)
                    stElem.m_stTeamCard.insert(iCardID);
                }
            }

            stElem.m_iBuffID = ::atoi(stXML.GetAttrib("buff").c_str());
            stElem.m_strMissBoss = stXML.GetAttrib("miss_boss");
            stElem.m_strKillBoss = stXML.GetAttrib("kill_boss");
            stElem.m_bAllGold = (bool)::atoi(stXML.GetAttrib("all_gold").c_str());
            stElem.m_bNoDeath = (bool)::atoi(stXML.GetAttrib("no_death").c_str());
            stElem.m_iNoCardType = ::atoi(stXML.GetAttrib("no_card_type").c_str());

            m_stBadgeMap[stElem.m_iBadgeID] = stElem;
            m_stCondTypeMap.insert(std::make_pair(stElem.m_iType, &(m_stBadgeMap[stElem.m_iBadgeID])));
        }

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}
