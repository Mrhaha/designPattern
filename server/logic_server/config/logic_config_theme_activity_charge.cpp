#include "logic_config_theme_activity_charge.h"
#include "Markup.h"
#include "common_datetime.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigThemeActivityChargeParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		while (true == stXML.FindElem("activity_config"))
		{
			TLogicThemeActivityItem activityItem;

			stXML.IntoElem();

			/////////////////////////////////////////////////////////////////////////////////
			if (!stXML.FindElem("info"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			activityItem.m_id = atoi(stXML.GetAttrib("id").c_str());
			if (!try_time_from_string(stXML.GetAttrib("start_time"), activityItem.m_iStartTime))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "start_time format error" << std::endl;
				throw std::exception();
			}
			if (!try_time_from_string(stXML.GetAttrib("end_time"), activityItem.m_iEndTime))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "end_time format error" << std::endl;
				throw std::exception();
			}

			activityItem.m_strSender = stXML.GetAttrib("m_strSender");
			activityItem.m_strSendTitle = stXML.GetAttrib("m_strSendTitle");
			activityItem.m_strSendContent = stXML.GetAttrib("m_strSendContent");

			/////////////////////////////////////////////////////////////////////////////////		
			while (stXML.FindElem("daily_task"))
			{
				TLogicThemeActivityOneday  oneday;
				stXML.IntoElem();

				if (false == stXML.FindElem("time"))
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
					throw std::exception();
				}

				if (!try_time_from_string(stXML.GetAttrib("start_time"), oneday.m_iStartTime))
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "daily_task start_time format error" << std::endl;
					throw std::exception();
				}
				if (!try_time_from_string(stXML.GetAttrib("end_time"), oneday.m_iEndTime))
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "daily_task end_time format error" << std::endl;
					throw std::exception();
				}

				/////////////////////////////////////////////////////////////////////////////////	
				if (false == stXML.FindElem("login"))
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
					throw std::exception();
				}
				stXML.IntoElem();				

				while (stXML.FindElem("task"))
				{
					TLogicThemeActivityChargeTaskItem task_item;

					task_item.m_iTaskType = 1;
					task_item.m_id = atoi(stXML.GetAttrib("id").c_str());
					task_item.m_iType = atoi(stXML.GetAttrib("type").c_str());
					task_item.m_iTarget = atoi(stXML.GetAttrib("target").c_str());
					task_item.m_iTargetAmount = atoi(stXML.GetAttrib("target_amount").c_str());

					//奖励
					CPackGameItem game_item;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("reward_id" + stStream.str()).empty()) break;

						game_item.m_iCardID = atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
						game_item.m_iItemType = atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
						game_item.m_iNum = atoi(stXML.GetAttrib("reward_amount" + stStream.str()).c_str());

						task_item.m_aReward.push_back(game_item);
					}

					oneday.m_stTask[task_item.m_id] = task_item;
					oneday.m_stTypeTask.insert(std::make_pair(std::make_tuple(task_item.m_iType, task_item.m_iTarget), task_item));
				}

				stXML.OutOfElem();

				/////////////////////////////////////////////////////////////////////////////////
				if (false == stXML.FindElem("recharge"))
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
					throw std::exception();
				}
				stXML.IntoElem();

				if (stXML.FindElem("pay"))
				{
					TLogicThemeActivityChargeItem charge_item;
					charge_item.m_id = atoi(stXML.GetAttrib("id").c_str());
					charge_item.m_iDepositID = atoi(stXML.GetAttrib("deposit_id").c_str());
					charge_item.m_iDayBuyNum = atoi(stXML.GetAttrib("day_buy_num").c_str());
					oneday.m_stChargeItem[charge_item.m_iDepositID] = charge_item;
				}

				while (stXML.FindElem("task"))
				{
					TLogicThemeActivityChargeTaskItem task_item;

					task_item.m_iTaskType = 2;
					task_item.m_id = atoi(stXML.GetAttrib("id").c_str());
					task_item.m_iType = atoi(stXML.GetAttrib("type").c_str());
					task_item.m_iTarget = atoi(stXML.GetAttrib("target").c_str());
					task_item.m_iTargetAmount = atoi(stXML.GetAttrib("target_amount").c_str());

					//奖励
					CPackGameItem game_item;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("reward_id" + stStream.str()).empty()) break;

						game_item.m_iCardID = atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
						game_item.m_iItemType = atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
						game_item.m_iNum = atoi(stXML.GetAttrib("reward_amount" + stStream.str()).c_str());

						task_item.m_aReward.push_back(game_item);
					}

					oneday.m_stTask[task_item.m_id] = task_item;
					oneday.m_stTypeTask.insert(std::make_pair(std::make_tuple(task_item.m_iType, task_item.m_iTarget), task_item));
				}

				stXML.OutOfElem();

				/////////////////////////////////////////////////////////////////////////////////	
				if (false == stXML.FindElem("buy"))
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
					throw std::exception();
				}
				stXML.IntoElem();

				while (stXML.FindElem("exchange"))
				{
					TLogicThemeActivityBuyItem buy_item;

					buy_item.m_id = atoi(stXML.GetAttrib("id").c_str());
					buy_item.m_iExchangeCount = atoi(stXML.GetAttrib("exchange_count").c_str());					

					//奖励
					CPackGameItem game_item;
					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("reward_id" + stStream.str()).empty()) break;

						game_item.m_iCardID = atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
						game_item.m_iItemType = atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
						game_item.m_iNum = atoi(stXML.GetAttrib("reward_amount" + stStream.str()).c_str());

						buy_item.m_aReward.push_back(game_item);
					}

					for (int i = 1; i < 20; ++i)
					{
						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("consume_item_id" + stStream.str()).empty()) break;

						game_item.m_iCardID = atoi(stXML.GetAttrib("consume_item_id" + stStream.str()).c_str());
						game_item.m_iItemType = atoi(stXML.GetAttrib("consume_iterm_type" + stStream.str()).c_str());
						game_item.m_iNum = atoi(stXML.GetAttrib("consume_item_num" + stStream.str()).c_str());

						buy_item.m_aConsume.push_back(game_item);
					}

					oneday.m_stBuyItem[buy_item.m_id] = buy_item;				
				}

				stXML.OutOfElem();

				/////////////////////////////////////////////////////////////////////////////////	
				stXML.OutOfElem();

				activityItem.m_stDayActivities[oneday.m_iStartTime] = oneday;
			}

			m_stAllActivities.insert(std::make_pair(activityItem.m_id, activityItem));
		}		
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}
