#include "logic_config_active.h"
#include "Markup.h"
#include "common_datetime.h"
#include "logic_config.h"

std::pair<bool, std::string>
CLogicConfigActivityMultiTaskParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (!stXML.FindElem("activity_multi_task"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("activity"))
		{
			TLogicActivityMultiTaskElem stElem;
			stElem.m_iActivityID = ::atoi(stXML.GetAttrib("id").c_str());
			if (stElem.m_iActivityID <= 0)
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "activity id <= 0" << std::endl;
				throw std::exception();
			}

			stXML.IntoElem();

			if (!stXML.FindElem("desc"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "activity desc not found" << std::endl;
				throw std::exception();
			}

			stElem.m_iTimeType = ::atoi(stXML.GetAttrib("time_type").c_str());
			stElem.m_iStartDay = ::atoi(stXML.GetAttrib("start_day").c_str());
			stElem.m_iEndDay = ::atoi(stXML.GetAttrib("end_day").c_str());
			stElem.m_iServerDay = ::atoi(stXML.GetAttrib("server_day").c_str());
			if (!try_time_from_string(stXML.GetAttrib("start_date"), stElem.m_iStartTime))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "start_date format error" << std::endl;
				throw std::exception();
			}
			if (!try_time_from_string(stXML.GetAttrib("end_date"), stElem.m_iEndTime))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "start_date format error" << std::endl;
				throw std::exception();
			}

			if(stElem.m_iStartDay != 0 && stElem.m_iEndDay != 0 && stElem.m_iStartTime != 0 && stElem.m_iEndTime != 0)
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "StartDay and StartTime can't both be valid! id:" << stElem.m_iActivityID << std::endl;
				throw std::exception();
			}

			if (!stXML.FindElem("tasks"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "activity tasks not found" << std::endl;
				throw std::exception();
			}

			stXML.IntoElem();

			while (stXML.FindElem("task"))
			{
				TLogicActivityTaskElem stTask;
				stTask.m_iTaskID = ::atoi(stXML.GetAttrib("task_id").c_str());
				stTask.m_iType = ::atoi(stXML.GetAttrib("type").c_str());
				stTask.m_iTarget = ::atoi(stXML.GetAttrib("target").c_str());
				stTask.m_iTargetAmount = ::atoi(stXML.GetAttrib("target_amount").c_str());
				stTask.m_iBonusScore = 0;
				if (stTask.m_iTaskID <= 0 || stTask.m_iType <= 0 || stTask.m_iTargetAmount <= 0)
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "activity task error in id:" << stElem.m_iActivityID << std::endl;
					throw std::exception();
				}

				for (int i = 1; i < 20; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

					int32_t iItemType = ::atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
					int32_t iCardID = ::atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
					int32_t iNum = ::atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());
					if (iItemType > 0 && iCardID > 0 && iNum > 0)
					{
						stTask.m_stBonusItemVec.emplace_back(iItemType, iCardID, iNum);
					}
				}

				stElem.m_stTaskMultiMap.emplace(stTask.m_iType, stTask);
			}

			stXML.OutOfElem();

			stXML.OutOfElem();

			m_stMultiTaskMap[stElem.m_iActivityID] = stElem;
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string>
CLogicConfigActivityTaskParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

		CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("activity_messy_task"));

		stXML.IntoElem();

		while (stXML.FindElem("activity"))
		{
			TLogicActivityMultiTaskElem stElem;
			stElem.m_iActivityID = ::atoi(stXML.GetAttrib("id").c_str());
			CONFIG_ASSERT_EXCEPTION(stElem.m_iActivityID > 0);

			stXML.IntoElem();

			CONFIG_ASSERT_EXCEPTION(stXML.FindElem("desc"));
			stElem.m_iActivityType = ::atoi(stXML.GetAttrib("activity_type").c_str());
			stElem.m_iTimeType = ::atoi(stXML.GetAttrib("time_type").c_str());
			stElem.m_iStartDay = ::atoi(stXML.GetAttrib("start_day").c_str());
			stElem.m_iEndDay = ::atoi(stXML.GetAttrib("end_day").c_str());
			CONFIG_ASSERT_EXCEPTION(stElem.m_iActivityType > 0);
			CONFIG_ASSERT_EXCEPTION(stElem.m_iTimeType > 0);
			CONFIG_ASSERT_EXCEPTION(stElem.m_iStartDay > 0);
			CONFIG_ASSERT_EXCEPTION(stElem.m_iEndDay > 0);

            std::vector<int32_t> stTaskIDList;

			CONFIG_ASSERT_EXCEPTION(stXML.FindElem("tasks"));
			stXML.IntoElem();
			while (stXML.FindElem("task"))
			{
				TLogicActivityTaskElem stTask;
				stTask.m_iTaskID = ::atoi(stXML.GetAttrib("task_id").c_str());
				stTask.m_iType = ::atoi(stXML.GetAttrib("type").c_str());
				stTask.m_iTarget = ::atoi(stXML.GetAttrib("target").c_str());
				stTask.m_iTargetAmount = ::atoi(stXML.GetAttrib("target_amount").c_str());
				stTask.m_iBonusScore = 0;
				CONFIG_ASSERT_EXCEPTION(stTask.m_iTaskID > 0);
				CONFIG_ASSERT_EXCEPTION(stTask.m_iType > 0);
				CONFIG_ASSERT_EXCEPTION(stTask.m_iTargetAmount > 0);

				for (int i = 1; i < 20; ++i)
				{
					int32_t iItemType = ::atoi(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).c_str());
					int32_t iCardID = ::atoi(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)).c_str());
					int32_t iNum = ::atoi(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)).c_str());
					if (iItemType > 0 && iCardID > 0 && iNum > 0)
					{
						stTask.m_stBonusItemVec.emplace_back(iItemType, iCardID, iNum);
					}
				}

				stElem.m_stTaskMultiMap.emplace(stTask.m_iType, stTask);
                stTaskIDList.push_back(stTask.m_iTaskID);
			}

			stXML.OutOfElem();

            if(stXML.FindElem("sequence_task_reward"))
            {
                stXML.IntoElem();
                while (stXML.FindElem("reward"))
                {
                    TLogicActivitySequenceTask stSeqTask;
                    stSeqTask.m_iTaskID = ::atoi(stXML.GetAttrib("task_id").c_str());
                    CONFIG_ASSERT_EXCEPTION(stSeqTask.m_iTaskID > 0);
                    CONFIG_ASSERT_EXCEPTION(std::find(stTaskIDList.begin(), stTaskIDList.end(), stSeqTask.m_iTaskID) == stTaskIDList.end());

                    for (int i = 1; i <= 10; ++i)
                    {
                        int32_t iTargetTaskID = ::atoi(stXML.GetAttrib("target_task_id" + std::to_string(i)).c_str());
                        if(iTargetTaskID <= 0) break;

                        CONFIG_ASSERT_EXCEPTION(std::find(stTaskIDList.begin(), stTaskIDList.end(), iTargetTaskID) != stTaskIDList.end());
                        stSeqTask.m_stTargetTaskList.push_back(iTargetTaskID);
                    }

                    CONFIG_ASSERT_EXCEPTION(!stSeqTask.m_stTargetTaskList.empty());

                    for (int i = 1; i <= 10; ++i)
                    {
                        int32_t iItemType = ::atoi(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).c_str());
                        int32_t iCardID = ::atoi(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)).c_str());
                        int32_t iNum = ::atoi(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)).c_str());
                        if (iItemType > 0 && iCardID > 0 && iNum > 0)
                        {
                            stSeqTask.m_stRewards.emplace_back(iItemType, iCardID, iNum);
                        }
                    }

                    stElem.m_stSequenceTaskMap.insert(std::make_pair(stSeqTask.m_iTaskID, stSeqTask));
                }
                stXML.OutOfElem();
            }

			stXML.OutOfElem();

			m_stActiveTaskMap.emplace(stElem.m_iActivityType, stElem);
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigActivityParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("activity_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("activity"))
		{
			TLogicActivityRefElem stActivityElem;

			int iActivityID = atoi(stXML.GetAttrib("activity_id").c_str());

			stXML.IntoElem();

			if (stXML.FindElem("desc"))
			{
				stActivityElem.m_iType = atoi(stXML.GetAttrib("type").c_str());
				stActivityElem.m_iTimeType = atoi(stXML.GetAttrib("time_type").c_str());
				stActivityElem.m_iStartDay = atoi(stXML.GetAttrib("start_day").c_str());
				stActivityElem.m_iEndDay = atoi(stXML.GetAttrib("end_day").c_str());
				stActivityElem.m_iLevel = atoi(stXML.GetAttrib("level").c_str());
				stActivityElem.m_iServerDay = atoi(stXML.GetAttrib("server_day").c_str());
				stActivityElem.m_iCreateDay = atoi(stXML.GetAttrib("create_day").c_str());

				std::string strDate;
				strDate = stXML.GetAttrib("start_date");
				if (!strDate.empty())
				{
					struct tm stTM = {};
					if (strptime(strDate.c_str(), "%Y-%m-%d %T", &stTM) == nullptr)
					{
						strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT TIME ERROR|ACTIVE ID:" << iActivityID << std::endl;
						throw std::exception();
					}
					stActivityElem.m_iStartTime = (int)mktime(&stTM);
				}

				strDate = stXML.GetAttrib("end_date");
				if (!strDate.empty())
				{
					struct tm stTM = {};
					if (strptime(strDate.c_str(), "%Y-%m-%d %T", &stTM) == nullptr)
					{
						strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT TIME ERROR|ACTIVE ID:" << iActivityID << std::endl;
						throw std::exception();
					}
					stActivityElem.m_iEndTime = (int)mktime(&stTM);
				}

				if(stActivityElem.m_iStartDay != 0 && stActivityElem.m_iEndDay != 0 && stActivityElem.m_iStartTime != 0 && stActivityElem.m_iEndTime != 0)
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:StartDay and StartTime can't both be valid! ID:" << iActivityID << std::endl;
					throw std::exception();
				}
			}

			m_stActivityRef[iActivityID] = stActivityElem;
			m_stActivityType2IDMap.emplace(stActivityElem.m_iType, iActivityID);

			if (stXML.FindElem("commodity"))
			{
				TLogicActiveCommodityElem stCommodity;
				stCommodity.m_iDailyFreeCount = atoi(stXML.GetAttrib("daily_free_count").c_str());
				stCommodity.m_iDailyLuckyCount = atoi(stXML.GetAttrib("daily_lucky_count").c_str());
				stCommodity.m_iLuckyScore = atoi(stXML.GetAttrib("lucky_score").c_str());
				stCommodity.m_iCountScore = atoi(stXML.GetAttrib("count_score").c_str());
				stCommodity.m_iRankScore = atoi(stXML.GetAttrib("rank_score").c_str());
				stCommodity.m_iBuyItemType = atoi(stXML.GetAttrib("buy_item_type").c_str());
				stCommodity.m_iBuyItemID = atoi(stXML.GetAttrib("buy_item").c_str());
				stCommodity.m_iBuyItemNum = atoi(stXML.GetAttrib("buy_item_num").c_str());
				stCommodity.m_iBuyItemNum2 = atoi(stXML.GetAttrib("buy_item_num_2").c_str());
				stCommodity.m_iOpenLevel = atoi(stXML.GetAttrib("open_level").c_str());
				stCommodity.m_iCondition = atoi(stXML.GetAttrib("condition").c_str());
				stCommodity.m_iDiscount = atoi(stXML.GetAttrib("discount").c_str());

				stCommodity.m_stConsumeItem.m_iItemType = atoi(stXML.GetAttrib("consume_item_type").c_str());
				stCommodity.m_stConsumeItem.m_iCardID = atoi(stXML.GetAttrib("consume_item_id").c_str());
				stCommodity.m_stConsumeItem.m_iNum = atoi(stXML.GetAttrib("consume_item_num").c_str());
				if(stCommodity.m_stConsumeItem.m_iItemType > 0)
                {
				    CONFIG_ASSERT_EXCEPTION(stCommodity.m_stConsumeItem.m_iCardID > 0);
				    CONFIG_ASSERT_EXCEPTION(stCommodity.m_stConsumeItem.m_iNum > 0);
                }

				const int32_t iMallBaodiCount = atoi(stXML.GetAttrib("mall_baodi_count").c_str());
				CONFIG_ASSERT_EXCEPTION(iMallBaodiCount > 0);
				CONFIG_ASSERT_EXCEPTION(stXML.GetAttrib("buy_mall_count_" + std::to_string(iMallBaodiCount + 1)).empty());
				CONFIG_ASSERT_EXCEPTION(stXML.GetAttrib("buy_mall_bag_" + std::to_string(iMallBaodiCount + 1)).empty());

				for (int i = 1; i <= iMallBaodiCount; ++i)
				{
					CONFIG_ASSERT_EXCEPTION(!stXML.GetAttrib("buy_mall_count_" + std::to_string(i)).empty());
					CONFIG_ASSERT_EXCEPTION(!stXML.GetAttrib("buy_mall_bag_" + std::to_string(i)).empty());

					TLogicActiveCommodityElem::TMallBag stMallBag;
					stMallBag.m_iBuyCount = atoi(stXML.GetAttrib("buy_mall_count_" + std::to_string(i)).c_str());
					stMallBag.m_iBuyCycle = atoi(stXML.GetAttrib("buy_mall_cycle_" + std::to_string(i)).c_str());
					stMallBag.m_strBuyBag = stXML.GetAttrib("buy_mall_bag_" + std::to_string(i));

					stCommodity.m_astMallBagVec.push_back(stMallBag);
				}
				m_stActiveCommodity[iActivityID] = stCommodity;
			}

			if (stXML.FindElem("info"))
			{
				if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_FLOWER_LANTERN)
				{
					TLogicActiveFlowerLanternInfoElem stInfo;
					stInfo.m_iDailyLoginCandle = atoi(stXML.GetAttrib("daily_login_candle").c_str());
					stInfo.m_iUnitRecharge = atoi(stXML.GetAttrib("unit_recharge").c_str());
					stInfo.m_iUnitRechargeCandle = atoi(stXML.GetAttrib("unit_recharge_candle").c_str());
					stInfo.m_iCandleReturnDiamond = atoi(stXML.GetAttrib("candle_return_diamond").c_str());

					if (stInfo.m_iDailyLoginCandle <= 0 || stInfo.m_iUnitRecharge <= 0 || stInfo.m_iUnitRechargeCandle <= 0 || stInfo.m_iCandleReturnDiamond <= 0)
					{
						strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:[daily_login_candle,unit_recharge,unit_recharge_candle,candle_return_diamond] <= 0 in " << iActivityID << std::endl;
						throw std::exception();
					}

					stInfo.m_strReturnSender = stXML.GetAttrib("return_sender");
					stInfo.m_strReturnTitle = stXML.GetAttrib("return_title");
					stInfo.m_stReturnContent = CFormatString::Compile(stXML.GetAttrib("return_content"));

					m_stFlowerLanterInfoConfig[iActivityID] = std::move(stInfo);
				}

				if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_1
				    || stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_2
				    || stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_3
				    || stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_4
				    || stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_5)
                {
                    m_stLoginFundMap[iActivityID].first = atoi(stXML.GetAttrib("deposit_id").c_str());
                }
			}

			if (false == stXML.FindElem("data"))
			{
				stXML.OutOfElem();
				continue;
			}

			stXML.IntoElem();

			if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_DINNER)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveDinnerElem stElem;
					stElem.m_iStartTime = atoi(stXML.GetAttrib("effect_dinner_start_time").c_str());
					stElem.m_iEndTime = atoi(stXML.GetAttrib("effect_dinner_end_time").c_str());
					stElem.m_iEnergyAmount = atoi(stXML.GetAttrib("energy_amount").c_str());

					m_stDinnerConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_LOGIN)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveNewServerElem stElem;
					for (int i = 1; i < 100; i++)
					{
						CPackGameItem stGameItem;
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("reward_amount" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusVec.push_back(stGameItem);
					}

					stElem.m_iSignDay = atoi(stXML.GetAttrib("sign_in_days_count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_iSignDay > 0);

					m_stNewServerConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_DEPOSIT)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveDepositElem stElem;
					for (int i = 1; i < 100; i++)
					{
						CPackGameItem stGameItem;
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusVec.push_back(stGameItem);
					}
					stElem.m_iAddUpRmb = atoi(stXML.GetAttrib("add_up_rmb").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_iAddUpRmb > 0);

					m_stDepositConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_CORNUCOPIA)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveCornucopiaElem stElem;

					stElem.m_iNeedYuanBao = atoi(stXML.GetAttrib("need_yuanbao").c_str());
					stElem.m_iVipLevel = atoi(stXML.GetAttrib("vip_level").c_str());
					stElem.m_iRealRebateMin = atoi(stXML.GetAttrib("real_rebate_min").c_str());
					stElem.m_iRealRebateMax = atoi(stXML.GetAttrib("real_rebate_max").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_iNeedYuanBao > 0);

					m_stCornucopiaConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_FOUNDATION)
			{
				TLogicActiveFoundationElem stElem;
                while (stXML.FindElem("active"))
                {
                    stElem.m_stUserLevel.push_back(atoi(stXML.GetAttrib("user_level").c_str()));

                    CPackGameItem stGameItem;
                    stGameItem.m_iItemType = atoi(stXML.GetAttrib("award_type").c_str());
                    stGameItem.m_iCardID = atoi(stXML.GetAttrib("award_id").c_str());
                    stGameItem.m_iNum = atoi(stXML.GetAttrib("award_amount").c_str());

                    CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
                    CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
                    CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

                    stElem.m_stRewards.push_back(stGameItem);
                }
				m_stFoundationConfig[iActivityID] = stElem;
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LEVEL_RACE)
			{
				TLogicActiveLevelRaceElem stElem;

				if (stXML.FindElem("extra"))
				{
					stElem.m_iExtraAwardLevel = atoi(stXML.GetAttrib("user_level").c_str());
					stElem.m_iExtraAwardMemberLimit = atoi(stXML.GetAttrib("member_limit").c_str());
					stElem.m_stExtraAwardItem.m_iItemType = atoi(stXML.GetAttrib("award_type").c_str());
					stElem.m_stExtraAwardItem.m_iCardID = atoi(stXML.GetAttrib("award_id").c_str());
					stElem.m_stExtraAwardItem.m_iNum = atoi(stXML.GetAttrib("award_amount").c_str());
					if (stElem.m_iExtraAwardMemberLimit == -1)
						stElem.m_iExtraAwardMemberLimit = std::numeric_limits<int>::max();
				}

				if (stXML.FindElem("actives"))
				{
					stXML.IntoElem();
					while (stXML.FindElem("active"))
					{
						CPackGameItem stGameItem;
						TLogicActiveLevelRaceAwardElem stAwardElem;
						stAwardElem.m_iUserLevel = atoi(stXML.GetAttrib("need_user_level").c_str());
						CONFIG_ASSERT_EXCEPTION(stAwardElem.m_iUserLevel > 0);

						for (int i = 1; i < 20; ++i)
						{
							std::stringstream stStream;
							stStream << "_" << i;
							if (stXML.GetAttrib("award_type" + stStream.str()).empty()) break;

							stGameItem.m_iItemType = atoi(stXML.GetAttrib("award_type" + stStream.str()).c_str());
							stGameItem.m_iCardID = atoi(stXML.GetAttrib("award_id" + stStream.str()).c_str());
							stGameItem.m_iNum = atoi(stXML.GetAttrib("award_amount" + stStream.str()).c_str());

							CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
							CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
							CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

							stAwardElem.m_stBonusItemVec.push_back(stGameItem);
						}

						stAwardElem.m_stQuarterReward.m_iItemType = atoi(stXML.GetAttrib("yueka_award_type").c_str());
						stAwardElem.m_stQuarterReward.m_iCardID = atoi(stXML.GetAttrib("yueka_award_id").c_str());
						stAwardElem.m_stQuarterReward.m_iNum = atoi(stXML.GetAttrib("yueka_award_amount").c_str());

						stElem.m_stAwardVec.push_back(stAwardElem);
					}
					stXML.OutOfElem();
				}

				m_stLevelRaceConfig[iActivityID] = stElem;
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HERO_FLASH_SALE)
			{
				TLogicHeroFlashSaleElem stElem;

				if (stXML.FindElem("active"))
				{
					stElem.m_iLimitLevelID = atoi(stXML.GetAttrib("limit_level_id").c_str());
					CONFIG_ASSERT_EXCEPTION(stElem.m_iLimitLevelID > 0);

					stElem.m_stAwardItem.m_iItemType = atoi(stXML.GetAttrib("award_type").c_str());
					stElem.m_stAwardItem.m_iCardID = atoi(stXML.GetAttrib("award_id").c_str());
					stElem.m_stAwardItem.m_iNum = atoi(stXML.GetAttrib("award_amount").c_str());
					CONFIG_ASSERT_EXCEPTION(stElem.m_stAwardItem.m_iItemType > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stAwardItem.m_iCardID > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stAwardItem.m_iNum > 0);

					stElem.m_stLimitConsumeItem.m_iItemType = atoi(stXML.GetAttrib("limit_consume_type").c_str());
					stElem.m_stLimitConsumeItem.m_iCardID = atoi(stXML.GetAttrib("limit_consume_id").c_str());
					stElem.m_stLimitConsumeItem.m_iNum = atoi(stXML.GetAttrib("limit_consume_amount").c_str());
					CONFIG_ASSERT_EXCEPTION(stElem.m_stLimitConsumeItem.m_iItemType > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stLimitConsumeItem.m_iCardID > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stLimitConsumeItem.m_iNum > 0);

					stElem.m_stConsumeItem.m_iItemType = atoi(stXML.GetAttrib("consume_type").c_str());
					stElem.m_stConsumeItem.m_iCardID = atoi(stXML.GetAttrib("consume_id").c_str());
					stElem.m_stConsumeItem.m_iNum = atoi(stXML.GetAttrib("consume_amount").c_str());
					CONFIG_ASSERT_EXCEPTION(stElem.m_stConsumeItem.m_iItemType > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stConsumeItem.m_iCardID > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stConsumeItem.m_iNum > 0);
				}

				m_stHeroFlashSaleConfig[iActivityID] = stElem;
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_OPEN_SERVER_LOGIN)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;
					TLogicActiveOpenServerLoginElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					m_stOpenServerLoginConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_OPEN_SERVER_BUY)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveOpenServerBuyElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("consume_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("consume_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("consume_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("consume_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stConsumeItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stConsumeItemVec.size() > 0);

					m_stOpenServerBuyConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_NORMAL_LEVEL)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveNormalLevelElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					
					stElem.m_iLevelID = atoi(stXML.GetAttrib("level_id").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iLevelID > 0);

					m_stNormalLevelConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_ELITE_LEVEL)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveEliteLevelElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iLevelID = atoi(stXML.GetAttrib("level_id").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iLevelID > 0);

					m_stEliteLevelConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_PVP_RANK)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActivePvpRankElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iRank = atoi(stXML.GetAttrib("rank").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iRank > 0);

					m_stPVPRankConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_UNLIMIT_LEVEL_SCORE)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveUnlimitLevelScoreLevelElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iScore = atoi(stXML.GetAttrib("score").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iScore > 0);

					m_stUnlimitLevelScoreConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_PASS_LEVEL_STAR)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActivePassLevelStarElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iStar = atoi(stXML.GetAttrib("star").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iStar > 0);

					m_stPassLevelStarConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_PASS_LEVEL_TIMES)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActivePassLevelTimesElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stPassLevelTimesConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_LEVEL)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveHeroEquipmentUpgradeLevelElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iHeroNum = atoi(stXML.GetAttrib("hero_num").c_str());
					stElem.m_iEquipmentLevel = atoi(stXML.GetAttrib("equipment_level").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iHeroNum > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iEquipmentLevel > 0);

					m_stHeroEquipmentUpgradeLevelConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_JEWEL)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveHeroEquipmentUpgradeJewelElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iJewelNum = atoi(stXML.GetAttrib("jewel_num").c_str());
					stElem.m_iJewelLevel = atoi(stXML.GetAttrib("jewel_level").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iJewelNum > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iJewelLevel > 0);

					m_stHeroEquipmentUpgradeJewelConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_STAR)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveHeroEquipmentUpgradeStarElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iEquipmentNum = atoi(stXML.GetAttrib("equipment_num").c_str());
					stElem.m_iEquipmentStar = atoi(stXML.GetAttrib("equipment_star").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iEquipmentNum > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iEquipmentStar > 0);

					m_stHeroEquipmentUpgradeStarConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_COLOR)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveHeroUpgradeColorElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iHeroNum = atoi(stXML.GetAttrib("hero_num").c_str());
					stElem.m_iColor = atoi(stXML.GetAttrib("color").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iHeroNum > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iColor > 0);

					m_stHeroUpgradeColorConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_LEVEL)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveHeroUpgradeLevelElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iHeroNum = atoi(stXML.GetAttrib("hero_num").c_str());
					stElem.m_iLevel = atoi(stXML.GetAttrib("level").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iHeroNum > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iLevel > 0);

					m_stHeroUpgradeLevelConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_STAR)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveHeroUpgradeStarElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iHeroNum = atoi(stXML.GetAttrib("hero_num").c_str());
					stElem.m_iStar = atoi(stXML.GetAttrib("star").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iHeroNum > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iStar > 0);

					m_stHeroUpgradeStarConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_WUSHEN_UPGRADE_LEVEL)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveWushenLevelElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iLevel = atoi(stXML.GetAttrib("level").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iLevel > 0);

					m_stWushenLevelConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_TEAM_LEVEL_UPGRADE)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveTeamLevelUpgradeElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iLevel = atoi(stXML.GetAttrib("level").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iLevel > 0);

					m_stTeamLevelUpgradeConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_UPGRADE_SKILL)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveUpgradeSkillElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stUpgradeSkillConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_FIGHT_POWER)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveFightPowerElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iFightPower = atoi(stXML.GetAttrib("fight_power").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iFightPower > 0);

					m_stFightPowerConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HERO_ACHIEVE)
			{
				TLogicActiveHeroAchieveElem stElem;
				while (stXML.FindElem("active"))
				{
					stElem.m_iCardID = atoi(stXML.GetAttrib("card_id").c_str());

					stElem.m_stBonusItem.m_iItemType = atoi(stXML.GetAttrib("reward_type").c_str());
					stElem.m_stBonusItem.m_iCardID = atoi(stXML.GetAttrib("reward_id").c_str());
					stElem.m_stBonusItem.m_iNum = atoi(stXML.GetAttrib("reward_amount").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_iCardID > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iItemType > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iCardID > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iNum > 0);

					m_stHeroAchieveConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_BENEFIT_STORE)
			{
				TLogicActiveBenefitStoreElem stElem;
				while (stXML.FindElem("active"))
				{
					stElem.m_stBonusItem.m_iItemType = atoi(stXML.GetAttrib("award_type").c_str());
					stElem.m_stBonusItem.m_iCardID = atoi(stXML.GetAttrib("award_id").c_str());
					stElem.m_stBonusItem.m_iNum = atoi(stXML.GetAttrib("award_num").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iItemType > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iCardID > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iNum > 0);

					stElem.m_stConsumeItem.m_iItemType = atoi(stXML.GetAttrib("consume_type").c_str());
					stElem.m_stConsumeItem.m_iCardID = atoi(stXML.GetAttrib("consume_id").c_str());
					stElem.m_stConsumeItem.m_iNum = atoi(stXML.GetAttrib("consume_num").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stConsumeItem.m_iItemType > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stConsumeItem.m_iCardID > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stConsumeItem.m_iNum > 0);

					m_stBenefitStoreConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_EXCHANGE_STORE)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveExchangeStoreElem stElem;

					stElem.m_stBonusItem.m_iItemType = atoi(stXML.GetAttrib("award_type").c_str());
					stElem.m_stBonusItem.m_iCardID = atoi(stXML.GetAttrib("award_id").c_str());
					stElem.m_stBonusItem.m_iNum = atoi(stXML.GetAttrib("award_num").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iItemType > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iCardID > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iNum > 0);

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("consume_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("consume_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("consume_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("consume_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stConsumeItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stConsumeItemVec.size() > 0);

					m_stExchangeStoreConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_SINGLE_DEPOSIT)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveSingleDepositElem stElem;

					stElem.m_iRmb = atoi(stXML.GetAttrib("rmb").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("award_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("award_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("award_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("award_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iRmb > 0);

					m_stSingleDepositConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_DAILY_CUMULATE_DEPOSIT)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveDailyCumulateDepositElem stElem;

					stElem.m_iRmb = atoi(stXML.GetAttrib("add_up_rmb").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iRmb > 0);

					m_stDailyCumulateDepositConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_DEADLINE_LOGIN)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveDeadlineLoginElem stElem;

					stElem.m_iSignDay = atoi(stXML.GetAttrib("sign_in_days_count").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("reward_amount" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iSignDay > 0);

					m_stDeadlineLoginConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_BUY_COMMODITY)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveBuyCommodityElem stElem;

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("reward_amount" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stBuyCommodityConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_FIGHT_THEME_ACTIVE_LEVEL)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveFightThemeActiveLevelElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stFightThemeActiveLevelConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_PVP_FIGHT_TIMES)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActivePvpFightTimesElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stPVPFightTimesConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_ELITE_LEVEL_FIGHT_TIMES)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveEliteLevelFightTimesElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stEliteLevelFightTimesConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_NORMAL_LEVEL_FIGHT_TIMES)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveNormalLevelFightTimesElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stNormalLevelFightTimesConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_BUY_ENERGY_TIMES)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveBuyEnergyTimesElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stBuyEnergyTimesConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_BUY_GOLD_TIMES)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveBuyGoldTimesElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stBuyGoldTimesConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_BUY_BLACK_STORE_TIMES)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveBuyBlackStoreTimesElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stBuyBlackStoreTimesConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_CROSS_FIGHT_PASS_LEVEL_TIMES)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCrossFightPassLevelTimesElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stCrossFightPassLevelTimesConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_UNLIMIT_LEVEL_FIGHT_TIMES)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveUnlimitLevelFightTimesElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stUnlimitLevelFightTimesConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_CONSUME_YUANBAO)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stCumulateConsumeYuanBaoConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_SINGLE_ITEM_EXCHANGE)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveSingleItemExchangeElem stElem;

					stElem.m_stBonusItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type").c_str());
					stElem.m_stBonusItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id").c_str());
					stElem.m_stBonusItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iItemType > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iCardID > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItem.m_iNum > 0);

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("consume_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("consume_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("consume_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("consume_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stConsumeItemVec.push_back(stGameItem);
					}

					stElem.m_iTimes = atoi(stXML.GetAttrib("times").c_str());
					stElem.m_iVipLevel = atoi(stXML.GetAttrib("vip_level").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stConsumeItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTimes > 0);

					m_stSingleItemExchangeConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_UPGRADE_TACTICS)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveUpgradeTacticsElem stElem;

					stElem.m_iUpgradeTacticsCount = atoi(stXML.GetAttrib("upgrade_tactics_count").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iUpgradeTacticsCount > 0);

					m_stUpgradeTacticsConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HERO_TRAIN)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveHeroTrainElem stElem;

					stElem.m_iHeroTrainCount = atoi(stXML.GetAttrib("hero_train_count").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iHeroTrainCount > 0);

					m_stHeroTrainConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_BREAK_EGG)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveBreakEggElem stElem;

					stElem.m_iBreakEggCount = atoi(stXML.GetAttrib("break_egg_count").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iBreakEggCount > 0);

					m_stBreakEggConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_YUANBAO_RECRUIT)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveYuanbaoRecruitElem stElem;

					stElem.m_iYuanbaoRecruitCount = atoi(stXML.GetAttrib("yuanbao_recruit_count").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iYuanbaoRecruitCount > 0);

					m_stYuanbaoRecruitConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_CONSUME_ENERGY)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveConsumeEnergyElem stElem;

					stElem.m_iConsumeEnergyValue = atoi(stXML.GetAttrib("consume_energy_value").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iConsumeEnergyValue > 0);

					m_stConsumeEnergyConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_CLIMB_TOWER_ORDER)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveClimbTowerElem stElem;

					stElem.m_iClimbTowerOrder = atoi(stXML.GetAttrib("climb_tower_order").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iClimbTowerOrder > 0);

					m_stClimbTowerConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_PVP_TERRITORY_ORDER)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActivePvpTerritoryElem stElem;

					stElem.m_iPvpTerritoryOrder = atoi(stXML.GetAttrib("pvp_territory_order").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iPvpTerritoryOrder > 0);

					m_stPvpTerritoryConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_TACTICS_COUNT)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveTacticsCountElem stElem;

					stElem.m_iTacticsCount = atoi(stXML.GetAttrib("tactics_count").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iTacticsCount > 0);

					m_stTacticsCountConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_GUILD_DONATE)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveGuildDonateElem stElem;

					stElem.m_iGuildDonateCount = atoi(stXML.GetAttrib("guild_donate_count").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iGuildDonateCount > 0);

					m_stGuildDonateConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_GUILD_TASK)
			{
				while (stXML.FindElem("active"))
				{
					TLogicActiveGuildTaskElem stElem;

					stElem.m_iGuildTaskCount = atoi(stXML.GetAttrib("guild_task_count").c_str());

					CPackGameItem stGameItem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iGuildTaskCount > 0);

					m_stGuildTaskConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_DAILY_CONSUME_YUANBAO)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stDailyConsumeYuanBaoConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LUCKY_BOX)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					int notify;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());
						notify = atoi(stXML.GetAttrib("notify" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
						stElem.m_stNotify.push_back(notify);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());
                    stElem.m_bIsBigPrize = (bool)atoi(stXML.GetAttrib("is_big_prize").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stLuckyBoxConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_TALENT_SCORE)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("score").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stTalentScoreConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_TOTAL_HERO_TRAIN)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stTotalHeroTrainConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_TURN_TABLE)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					int notify;

					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());
						notify = atoi(stXML.GetAttrib("notify" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
						stElem.m_stNotify.push_back(notify);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());
                    stElem.m_bIsBigPrize = (bool)atoi(stXML.GetAttrib("is_big_prize").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stTurnTableScoreConfig[iActivityID].push_back(stElem);
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_INSURE)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stLoginInsureMap[iActivityID].push_back(std::move(stElem));
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HERO_INSURE)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stHeroInsureMap[iActivityID].push_back(std::move(stElem));
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HOME_INSURE)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stHomeInsureMap[iActivityID].push_back(std::move(stElem));
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_WISH_CHARACTER)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

                    TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stWishCharacterConfig[iActivityID].push_back(std::move(stElem));
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_COUNTDOWN_HERO)
			{//限时神将
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					int notify;
					for (int i = 1; i < 100; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());
						notify = atoi(stXML.GetAttrib("notify" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
						stElem.m_stNotify.push_back(notify);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_mapCountDownHeroConfig[iActivityID].push_back(std::move(stElem));
				}

				stXML.OutOfElem();

				//rank
				if (stXML.FindElem("rank"))
				{
					stXML.IntoElem();
					std::vector<TLogicActiveRankElem> vecRank;
					while (stXML.FindElem("reward"))
					{
						TLogicActiveRankElem stRank;
						stRank.m_iIndex = atoi(stXML.GetAttrib("index").c_str());
						stRank.m_iMinRank = atoi(stXML.GetAttrib("rank_min").c_str());
						stRank.m_iMaxRank = atoi(stXML.GetAttrib("rank_max").c_str());
						stRank.m_iNeedPoint = atoi(stXML.GetAttrib("need_point").c_str());
						stRank.m_strSender = stXML.GetAttrib("sender").c_str();
						stRank.m_strTitle = stXML.GetAttrib("title").c_str();
						stRank.m_strContent = stXML.GetAttrib("content").c_str();

						CONFIG_ASSERT_EXCEPTION(stRank.m_iMinRank > 0);
						CONFIG_ASSERT_EXCEPTION(stRank.m_iMaxRank > 0);
						CONFIG_ASSERT_EXCEPTION(stRank.m_iNeedPoint > 0);

						CPackGameItem stGameItem;
						for (int i = 1; i < 100; ++i)
						{
							std::stringstream stStream;
							stStream << "_" << i;
							if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

							stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
							stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
							stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

							CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
							CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
							CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

							stRank.m_stBonusItemVec.push_back(stGameItem);
						}

						CONFIG_ASSERT_EXCEPTION(stRank.m_stBonusItemVec.size() > 0);

						vecRank.push_back(stRank);
					}
					m_stActiveRankConfig[iActivityID] = vecRank;
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_FLOWER_LANTERN)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stFlowerLanterConfig[iActivityID].push_back(std::move(stElem));
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_PET_INSURE)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stPetInsureMap[iActivityID].push_back(std::move(stElem));
				}
			}
			else if(stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_HEART_INSURE)
            {
                while (stXML.FindElem("active"))
                {
                    CPackGameItem stGameItem;

                    TLogicActiveHeartInsureElem stElem;
                    stElem.m_iHeartLevel = atoi(stXML.GetAttrib("heart_level").c_str());
                    stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());
                    CONFIG_ASSERT_EXCEPTION(stElem.m_iHeartLevel > 0);
                    CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

                    for (int i = 1; i < 20; ++i)
                    {
                        std::stringstream stStream;
                        stStream << "_" << i;
                        if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

                        stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
                        stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
                        stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

                        CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
                        CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
                        CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

                        stElem.m_stBonusItemVec.push_back(stGameItem);
                    }

                    CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);

                    m_stHeartInsureMap[iActivityID].push_back(std::move(stElem));
                }
            }
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_SEND_FLOWER)
			{
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
					CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

					m_stSendFlowerConfig[iActivityID].push_back(std::move(stElem));
				}
			}
			else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LUCKY_HAMMER)
			{//限时神将
				while (stXML.FindElem("active"))
				{
					CPackGameItem stGameItem;

					TLogicActiveCalculateCountElem stElem;
					for (int i = 1; i < 100; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

						stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
						stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
						stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
						CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

						stElem.m_stBonusItemVec.push_back(stGameItem);
					}

					stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());
					stElem.m_strBuyMallBag = stXML.GetAttrib("buy_mall_bag");

					CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);

					m_stLuckyHammerConfig[iActivityID].push_back(std::move(stElem));
				}

				stXML.OutOfElem();

				//rank
				if (stXML.FindElem("rank"))
				{
					stXML.IntoElem();
					std::vector<TLogicActiveRankElem> vecRank;
					while (stXML.FindElem("reward"))
					{
						TLogicActiveRankElem stRank;
						stRank.m_iIndex = atoi(stXML.GetAttrib("index").c_str());
						stRank.m_iMinRank = atoi(stXML.GetAttrib("rank_min").c_str());
						stRank.m_iMaxRank = atoi(stXML.GetAttrib("rank_max").c_str());
						stRank.m_strSender = stXML.GetAttrib("sender").c_str();
						stRank.m_strTitle = stXML.GetAttrib("title").c_str();
						stRank.m_strContent = stXML.GetAttrib("content").c_str();

						CONFIG_ASSERT_EXCEPTION(stRank.m_iMinRank > 0);
						CONFIG_ASSERT_EXCEPTION(stRank.m_iMaxRank > 0);

						CPackGameItem stGameItem;
						for (int i = 1; i < 100; ++i)
						{
							std::stringstream stStream;
							stStream << "_" << i;
							if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

							stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
							stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
							stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

							CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
							CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
							CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

							stRank.m_stBonusItemVec.push_back(stGameItem);
						}

						CONFIG_ASSERT_EXCEPTION(stRank.m_stBonusItemVec.size() > 0);

						vecRank.push_back(stRank);
					}
					m_stActiveRankConfig[iActivityID] = vecRank;
				}
			}
            else if (stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_1
                || stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_2
                || stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_3
                || stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_4
                || stActivityElem.m_iType == CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_5)
            {
                while (stXML.FindElem("active"))
                {
                    CPackGameItem stGameItem;

                    TLogicActiveCalculateCountElem stElem;
                    for (int i = 1; i < 20; ++i)
                    {
                        std::stringstream stStream;
                        stStream << "_" << i;
                        if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

                        stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
                        stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
                        stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

                        CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
                        CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
                        CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

                        stElem.m_stBonusItemVec.push_back(stGameItem);
                    }

                    stElem.m_iCount = atoi(stXML.GetAttrib("count").c_str());

                    CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);
                    CONFIG_ASSERT_EXCEPTION(stElem.m_iCount > 0);

                    m_stLoginFundMap[iActivityID].second.push_back(std::move(stElem));
                }
            }
			else
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:UNKNOW_ACTIVITY_TYPE|TYPE:" << stActivityElem.m_iType << std::endl;
				throw std::exception();
			}

			stXML.OutOfElem();

			stXML.OutOfElem();
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

std::string CLogicConfigActivityParser::GetActiveCommodityBuyMallBag(int32_t iActivityID, int32_t iBuyCount) const
{
	std::string bag = "";
	auto iterCommodity = m_stActiveCommodity.find(iActivityID);
	if (iterCommodity == m_stActiveCommodity.end())
	{
		return bag;
	}

	for (const auto& buy : iterCommodity->second.m_astMallBagVec)
	{
		if (0 == buy.m_iBuyCount && 0 == buy.m_iBuyCycle)
		{
			bag = buy.m_strBuyBag;
		}
		else if (iBuyCount >= buy.m_iBuyCount)
		{
			const int32_t iLeftCount = iBuyCount - buy.m_iBuyCount;
			if (0 == buy.m_iBuyCycle && 0 == iLeftCount)
			{
				bag = buy.m_strBuyBag;
			}
			else if (buy.m_iBuyCycle > 0 && 0 == (iLeftCount % buy.m_iBuyCycle))
			{
				bag = buy.m_strBuyBag;
			}
		}
	}
	return bag;
}

std::pair<bool, std::string>
CLogicConfigMergeActivityParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

		CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("merge_activity"));

		stXML.IntoElem();

		while (stXML.FindElem("merge"))
		{
			int32_t iMergeCount = 0;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("count"), iMergeCount));

			stXML.IntoElem();

			TLogicMergeActivityElem stMerge;
			CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("time_type"), stMerge.m_iTimeType));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("start_day"), stMerge.m_iStartDay));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("end_day"), stMerge.m_iEndDay));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("daily_level_count"), stMerge.m_iDailyLevelCount));
			
			while (stXML.FindElem("activity"))
			{
				stXML.IntoElem();

				while(stXML.FindElem("task"))
				{
					int32_t iTaskID = 0;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), iTaskID));
					TLogicMergeActivityTaskElem stElem;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("type"), stElem.m_iType));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("target"), stElem.m_iTarget));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("target_amount"), stElem.m_iTargetAmount));
					for (int32_t i = 1; i < 20; ++i)
					{
						if (!stXML.GetAttrib("reward_type_" + std::to_string(i)).empty())
						{
							CPackGameItem stItem;
							CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("reward_type_" + std::to_string(i)), stItem.m_iItemType));
							CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("reward_id_" + std::to_string(i)), stItem.m_iCardID));
							CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("reward_amount_" + std::to_string(i)), stItem.m_iNum));
							stElem.m_stBonusItemVec.push_back(stItem);
						}
					}
					stMerge.m_stTaskMap[iTaskID] = std::move(stElem);
				}

				while (stXML.FindElem("pay"))
				{
					int32_t iDepositID = 0;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("deposit_id"), iDepositID));
					stMerge.m_stDepositIDSet.insert(iDepositID);
				}

				while (stXML.FindElem("exchange"))
				{
					int32_t iExchangeID = 0;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), iExchangeID));
					TLogicMergeActivityExchangeElem stElem;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("exchange_count"), stElem.m_iExchangeCount));
					for (int32_t i = 1; i < 20; ++i)
					{
						if (!stXML.GetAttrib("consume_iterm_type_" + std::to_string(i)).empty())
						{
							CPackGameItem stItem;
							CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("consume_iterm_type_" + std::to_string(i)), stItem.m_iItemType));
							CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("consume_item_id_" + std::to_string(i)), stItem.m_iCardID));
							CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("consume_item_num_" + std::to_string(i)), stItem.m_iNum));
							stElem.m_stConsumeVec.push_back(stItem);
						}
					}
					CONFIG_ASSERT_EXCEPTION(!stElem.m_stConsumeVec.empty());
					for (int32_t i = 1; i < 20; ++i)
					{
						if (!stXML.GetAttrib("reward_type_" + std::to_string(i)).empty())
						{
							CPackGameItem stItem;
							CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("reward_type_" + std::to_string(i)), stItem.m_iItemType));
							CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("reward_id_" + std::to_string(i)), stItem.m_iCardID));
							CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("reward_amount_" + std::to_string(i)), stItem.m_iNum));
							stElem.m_stBonusVec.push_back(stItem);
						}
					}
					stMerge.m_stExchangeMap[iExchangeID] = std::move(stElem);
				}

				while (stXML.FindElem("double"))
				{
					int32_t iDoubleType = 0;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("type"), iDoubleType));
					stMerge.m_stDoubleTypeSet.insert(iDoubleType);
				}

				while (stXML.FindElem("level"))
				{
					int32_t iLevelID = 0, iNeedVip = 0;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("need_vip"), iNeedVip));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("level_id"), iLevelID));
					stMerge.m_stLevelMap.emplace(iLevelID, iNeedVip);
				}

				stXML.OutOfElem();	//activity
			}

			m_stMergeActivityMap[iMergeCount] = std::move(stMerge);

			stXML.OutOfElem();	//merge
		}

		stXML.OutOfElem();//merge_activity

	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string>
CLogicConfigDaySingleRechargeActivityParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (!stXML.FindElem("day_single_recharge_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "day_single_recharge_config not found!" << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("activity"))
		{
			TLogicDaySingleRechargeActivityInfo activityInfo;
			activityInfo.m_iID = atoi(stXML.GetAttrib("activity_id").c_str());
			stXML.IntoElem();

			TLogicActivityRefElem &stActivityElem = activityInfo.m_stActivityRef;
			if (!stXML.FindElem("desc"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}

			stActivityElem.m_iType = atoi(stXML.GetAttrib("type").c_str());
			stActivityElem.m_iTimeType = atoi(stXML.GetAttrib("time_type").c_str());
			stActivityElem.m_iStartDay = atoi(stXML.GetAttrib("start_day").c_str());
			stActivityElem.m_iEndDay = atoi(stXML.GetAttrib("end_day").c_str());
			stActivityElem.m_iLevel = atoi(stXML.GetAttrib("level").c_str());
			stActivityElem.m_iServerDay = atoi(stXML.GetAttrib("server_day").c_str());
			if (!try_time_from_string(stXML.GetAttrib("start_date"), stActivityElem.m_iStartTime))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT start_date ERROR|ACTIVE ID:" << activityInfo.m_iID << std::endl;
				throw std::exception();
			}
			if (!try_time_from_string(stXML.GetAttrib("end_date"), stActivityElem.m_iEndTime))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT end_date ERROR|ACTIVE ID:" << activityInfo.m_iID << std::endl;
				throw std::exception();
			}

			if(stActivityElem.m_iStartDay != 0 && stActivityElem.m_iEndDay != 0 && stActivityElem.m_iStartTime != 0 && stActivityElem.m_iEndTime != 0)
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:StartDay and StartTime can't both be valid! ID:" << activityInfo.m_iID << std::endl;
				throw std::exception();
			}

			activityInfo.m_strSender = stXML.GetAttrib("m_strSender");
			activityInfo.m_strSendTitle = stXML.GetAttrib("m_strSendTitle");
			activityInfo.m_strSendContent = stXML.GetAttrib("m_strSendContent");			

			//////////////////////////////////////////////////
			if (!stXML.FindElem("data"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			stXML.IntoElem();
			while (stXML.FindElem("active"))
			{
				TLogicDaySingleRechargeTaskElem stElem;

				stElem.m_iTaskID = atoi(stXML.GetAttrib("index").c_str());
				stElem.m_iTaskNum = atoi(stXML.GetAttrib("task_num").c_str());
				const int32_t iRmb = atoi(stXML.GetAttrib("rmb").c_str());

				CPackGameItem stGameItem;
				for (int i = 1; i < 20; ++i)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("award_type" + stStream.str()).empty()) break;

					stGameItem.m_iItemType = atoi(stXML.GetAttrib("award_type" + stStream.str()).c_str());
					stGameItem.m_iCardID = atoi(stXML.GetAttrib("award_id" + stStream.str()).c_str());
					stGameItem.m_iNum = atoi(stXML.GetAttrib("award_num" + stStream.str()).c_str());

					CONFIG_ASSERT_EXCEPTION(stGameItem.m_iItemType > 0);
					CONFIG_ASSERT_EXCEPTION(stGameItem.m_iCardID > 0);
					CONFIG_ASSERT_EXCEPTION(stGameItem.m_iNum > 0);

					stElem.m_stBonusItemVec.push_back(stGameItem);
				}

				CONFIG_ASSERT_EXCEPTION(iRmb > 0);
				CONFIG_ASSERT_EXCEPTION(stElem.m_iTaskNum > 0);
				CONFIG_ASSERT_EXCEPTION(stElem.m_iTaskNum < 20);
				CONFIG_ASSERT_EXCEPTION(stElem.m_stBonusItemVec.size() > 0);

				activityInfo.m_stTaskMap[iRmb] = stElem;
			}
			stXML.OutOfElem();
			//////////////////////////////////////////////////
			m_stActivityMap[activityInfo.m_iID] = activityInfo;

			stXML.OutOfElem();
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicDaySingleRechargeActivityInfo* CLogicConfigDaySingleRechargeActivityParser::GetActivityConfig(int32_t iActivityID) const
{
	const auto iter = m_stActivityMap.find(iActivityID);
	return iter == m_stActivityMap.end() ? nullptr : &iter->second;
}

const TLogicDaySingleRechargeTaskElem* CLogicConfigDaySingleRechargeActivityParser::GetActivityTaskConfig(int32_t iActivityID, int32_t iTaskID) const
{
	const auto iter = m_stActivityMap.find(iActivityID);
	if (iter != m_stActivityMap.end())
	{
		for (const auto& task : iter->second.m_stTaskMap)
		{
			if (task.second.m_iTaskID == iTaskID)
			{
				return &task.second;
			}
		}
	}
	return nullptr;
}

std::pair<bool, std::string> CLogicConfigAvyTimeParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"))
        stXML.IntoElem();

        while (stXML.FindElem("avy"))
        {
            int iAvyID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(iAvyID > 0, iAvyID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stAvyTimeMap.find(iAvyID) == m_stAvyTimeMap.end(), iAvyID)

            TLogicAvyTimeInfo stInfo;
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigCommonFunc::ParserAvyOpeningElem(stXML, stInfo.m_stAvyTime), iAvyID)

            m_stAvyTimeMap[iAvyID] = std::move(stInfo);
        }

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicAvyTimeInfo* CLogicConfigAvyTimeParser::GetConfig(int iAvyID) const
{
    auto iter = m_stAvyTimeMap.find(iAvyID);
    return iter == m_stAvyTimeMap.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string> CLogicConfigAvyScoreAwardParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"))
        stXML.IntoElem();

        while (stXML.FindElem("avy"))
        {
            int iAvyID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetAvyTimeConfig().GetConfig(iAvyID), iAvyID)

            int iNum = 1;
            stXML.IntoElem();
            while (stXML.FindElem("index"))
            {
                int iIndex = ::atoi(stXML.GetAttrib("index").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iIndex == iNum, iAvyID)
                ++iNum;

                TLogicAvyScoreAwardInfo stInfo;
                stInfo.m_stNeedItem.m_iItemType = ::atoi(stXML.GetAttrib("need_item_type").c_str());
                stInfo.m_stNeedItem.m_iCardID = ::atoi(stXML.GetAttrib("need_item_id").c_str());
                stInfo.m_stNeedItem.m_iNum = ::atoi(stXML.GetAttrib("need_item_num").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stInfo.m_stNeedItem), iAvyID)

                for(int i = 1; i <= 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                    CPackGameItem stItem;
                    stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                    stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                    stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), iAvyID)

                    stInfo.m_stAwards.emplace_back(stItem);
                }
                CONFIG_ASSERT_EXCEPTION_EX(!stInfo.m_stAwards.empty(), iAvyID)

                m_stAwardMap[iAvyID].push_back(stInfo);
            }
            stXML.OutOfElem();
        }

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicAvyScoreAwardInfo* CLogicConfigAvyScoreAwardParser::GetAwardConfig(int iAvyID, int iIndex) const
{
    auto iter = m_stAwardMap.find(iAvyID);
    if(iter == m_stAwardMap.end())
        return nullptr;

    if(iIndex <= 0 || iIndex > int(iter->second.size()))
        return nullptr;

    return &(iter->second[iIndex - 1]);
}
