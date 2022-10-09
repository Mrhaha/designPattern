#include "logic_conifg_score_level.h"
#include "Markup.h"
#include "common_datetime.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
CLogicConfigScoreLevelParser::CLogicConfigScoreLevelParser()
		: CLogicConfigParserBase()
{}


std::pair<bool, std::string>
CLogicConfigScoreLevelParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());
		CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));
		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("scorelevel_config"));

		stXML.IntoElem();

		/////////////////////////////////////////////////////////////////////////////////
		while (stXML.FindElem("activity"))
		{
			const auto activity_id = atoi(stXML.GetAttrib("id").c_str());
			CONFIG_ASSERT_EXCEPTION(activity_id > 0);

			stXML.IntoElem();

			TLogicScoreLevelElem activity_info;
			CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
			CONFIG_ASSERT_EXCEPTION(try_time_from_string(stXML.GetAttrib("begin_time"), activity_info.m_iBeginTime))
			CONFIG_ASSERT_EXCEPTION(try_time_from_string(stXML.GetAttrib("end_time"), activity_info.m_iEndTime))
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("single_buy_xingdongli"), activity_info.m_iSingleBuyXingdongli));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("max_xingdongli"), activity_info.m_iMaxXingdongli));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("init_xingdongli"), activity_info.m_iInitXingdongli));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("xingdongli_speed"), activity_info.m_iXingdongliSpeed));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("xingdongli_item_id"), activity_info.m_iXingdongliItemID));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("score_deposit_id"), activity_info.m_iScoreDepositID));
			activity_info.m_strScoreMailTitle = stXML.GetAttrib("score_mail_title");
			activity_info.m_strScoreMailContent = stXML.GetAttrib("score_mail_content");
			activity_info.m_strScoreMailSender = stXML.GetAttrib("score_mail_sender");

			//////////////////////////////////////////////////////
			//score_reward
			CONFIG_ASSERT_EXCEPTION(stXML.FindElem("score_reward"));
			stXML.IntoElem();
			while (stXML.FindElem("reward"))
			{
				const auto index = atoi(stXML.GetAttrib("index").c_str());
				CONFIG_ASSERT_EXCEPTION(index > 0);

				TLogicScoreLevelRewardElem reward;
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("target"), reward.m_iTarget));
				CONFIG_ASSERT_EXCEPTION(reward.m_iTarget > 0);

				for (int32_t i = 1; i < 10; ++i)
				{
					if (!stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
					{
						CPackGameItem item;
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)), item.m_iItemType));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)), item.m_iCardID));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)), item.m_iNum));

						reward.m_stBonusItemVec.push_back(item);
					}

					if (!stXML.GetAttrib("deposit_item_type_" + std::to_string(i)).empty())
					{
						CPackGameItem item;
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("deposit_item_type_" + std::to_string(i)), item.m_iItemType));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("deposit_item_id_" + std::to_string(i)), item.m_iCardID));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("deposit_item_num_" + std::to_string(i)), item.m_iNum));

						reward.m_stDepositItemVec.push_back(item);
					}
				}

				CONFIG_ASSERT_EXCEPTION(activity_info.m_stRewardMap.emplace(index, std::move(reward)).second);
			}
			stXML.OutOfElem();

			//////////////////////////////////////////////////////
			//score_level
			CONFIG_ASSERT_EXCEPTION(stXML.FindElem("score_level"));
			stXML.IntoElem();
			while (stXML.FindElem("level"))
			{
				const auto index = atoi(stXML.GetAttrib("index").c_str());
				CONFIG_ASSERT_EXCEPTION(index > 0);

				TLogicScoreLevelFightElem level;
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("level_id"), level.m_iLevelID))
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("xingdongli"), level.m_iXingdongli))
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("score"), level.m_iScore))
				for (int32_t i = 1; i < 10; ++i)
				{
					if (!stXML.GetAttrib("need_type_" + std::to_string(i)).empty())
					{
						int32_t need_type = 0, need_param = 0;
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("need_type_" + std::to_string(i)), need_type));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("need_param_" + std::to_string(i)), need_param));
						level.m_stNeedParams.emplace_back(need_type, need_param);
					}
				}

				CONFIG_ASSERT_EXCEPTION(activity_info.m_stScoreLevelMap.emplace(index, level).second);
			}
			stXML.OutOfElem();

			//////////////////////////////////////////////////////
			//fight_level
			CONFIG_ASSERT_EXCEPTION(stXML.FindElem("fight_level"));
			stXML.IntoElem();
			while (stXML.FindElem("level"))
			{
				const auto index = atoi(stXML.GetAttrib("index").c_str());
				CONFIG_ASSERT_EXCEPTION(index > 0);

				TLogicScoreLevelFightElem level;
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("level_id"), level.m_iLevelID))
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("xingdongli"), level.m_iXingdongli))
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("score"), level.m_iScore))
				for (int32_t i = 1; i < 10; ++i)
				{
					if (!stXML.GetAttrib("need_type_" + std::to_string(i)).empty())
					{
						int32_t need_type = 0, need_param = 0;
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("need_type_" + std::to_string(i)), need_type));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("need_param_" + std::to_string(i)), need_param));
						level.m_stNeedParams.emplace_back(need_type, need_param);
					}
				}

				CONFIG_ASSERT_EXCEPTION(activity_info.m_stChallengeLevelMap.emplace(index, level).second);
			}
			stXML.OutOfElem();

			//////////////////////////////////////////////////////
			//buy_xingdongli_cost
			CONFIG_ASSERT_EXCEPTION(stXML.FindElem("buy_xingdongli_cost"));
			stXML.IntoElem();
			while (stXML.FindElem("cost"))
			{
				const auto count = atoi(stXML.GetAttrib("count").c_str());
				TLogicScoreLevelBuyElem buy{};
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("cost"), buy.m_iCost))
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("need_vip_level"), buy.m_iNeedVipLevel))

				CONFIG_ASSERT_EXCEPTION(activity_info.m_stBuyCostMap.emplace(count, buy).second);
			}
			stXML.OutOfElem();

			/////////////////////////

			stXML.OutOfElem();

			CONFIG_ASSERT_EXCEPTION(m_stActivityMap.emplace(activity_id, activity_info).second);
		}
		stXML.OutOfElem();
		
		/////////////////////////////////////////////////////////////////////////////////
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicScoreLevelElem* CLogicConfigScoreLevelParser::GetActivityConfig(int32_t iActivityID) const
{
	const auto iter = m_stActivityMap.find(iActivityID);
	return iter == m_stActivityMap.end() ? nullptr : &iter->second;
}

std::pair<bool, std::string> CLogicConfigZooQueueParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());
        CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));
        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"));

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));

        m_bSwitch = (bool)::atoi(stXML.GetAttrib("switch").c_str());
        m_iLogoutProtectTime = ::atoi(stXML.GetAttrib("logout_protect_time").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iLogoutProtectTime > 0);

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}
