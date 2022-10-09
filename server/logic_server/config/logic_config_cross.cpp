#include "logic_config_cross.h"
#include "Markup.h"
#include "logic_config.h"
#include "logic_link_common.h"

std::pair<bool, std::string>
CLogicConfigMatchPvpParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

		CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"));

		stXML.IntoElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("match_info"));

		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("match_min_level"), m_iMatchMinLevel));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("daily_award_count"), m_iDailyAwardCount));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("baodi_match_level"), m_iBaodiMatchLevel));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("baodi_fail_count"), m_iBaodiFailCount));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("baodi_match_robot_delta"), m_iBaodiMatchRobotDelta));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("match_level"));

		stXML.IntoElem();

		while (stXML.FindElem("level"))
		{
			int32_t iLevel = 0;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("level"), iLevel));

			TLogicMatchLevelElem stElem;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("score"), stElem.m_iScore));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("win_score"), stElem.m_iWinScore));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("lose_score"), stElem.m_iLoseScore));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("match_min_level"), stElem.m_iMatchMinLevel));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("match_max_level"), stElem.m_iMatchMaxLevel));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("match_power_min_ratio"), stElem.m_dMatchPowerMinRatio));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("match_power_max_ratio"), stElem.m_dMatchPowerMaxRatio));

			m_stLevelMap[iLevel] = std::move(stElem);
		}

		stXML.OutOfElem();//match_level

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("match_store"));

		stXML.IntoElem();

		while (stXML.FindElem("store"))
		{
			int32_t id = 0;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), id));

			TLogicMatchStoreElem stElem;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("item_type"), stElem.m_stBuyItem.m_iItemType));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("card_id"), stElem.m_stBuyItem.m_iCardID));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("num"), stElem.m_stBuyItem.m_iNum));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("price_type"), stElem.m_stConsumeItem.m_iItemType));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("price_id"), stElem.m_stConsumeItem.m_iCardID));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("price_num"), stElem.m_stConsumeItem.m_iNum));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("match_level_limit"), stElem.m_iMatchLevelLimit));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("daily_count"), stElem.m_iDailyBuyCount));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("total_count"), stElem.m_iTotalBuyCount));

			m_stStoreMap[id] = std::move(stElem);
		}

		stXML.OutOfElem();//match_store

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("daily_award"));

		stXML.IntoElem();

		while (stXML.FindElem("award"))
		{
			int32_t level = 0;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("level"), level));

			TLogicMatchDailyAwardElem stElem;
			stElem.m_strSender = stXML.GetAttrib("sender");
			stElem.m_strTitle = stXML.GetAttrib("title");
			stElem.m_strContent = stXML.GetAttrib("conetnt");

			for (int32_t i = 1; i < 10; ++i)
			{
				if (!stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
				{
					CPackGameItem item;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)), item.m_iItemType));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)), item.m_iCardID));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)), item.m_iNum));
					stElem.m_stBonusVec.push_back(std::move(item));
				}
			}

			m_stDailyAwardMap[level] = std::move(stElem);
		}

		stXML.OutOfElem();//daily_award

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("max_award"));

		stXML.IntoElem();

		while (stXML.FindElem("award"))
		{
			int32_t level = 0;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("level"), level));

			for (int32_t i = 1; i < 10; ++i)
			{
				if (!stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
				{
					CPackGameItem item;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)), item.m_iItemType));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)), item.m_iCardID));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)), item.m_iNum));

					m_stMaxAwardMap[level].push_back(std::move(item));
				}
			}
		}

		stXML.OutOfElem();//max_award

		stXML.OutOfElem();//root

	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}


int32_t CLogicConfigMatchPvpParser::GetMatchLevelByScore(int32_t iScore) const
{
	auto iLevel = 0;
	for (const auto& level : m_stLevelMap)
	{
		if (iScore >= level.second.m_iScore)
		{
			iLevel = level.first;
		}
	}
	return iLevel;
}

const TLogicMatchLevelElem* CLogicConfigMatchPvpParser::GetMatchLevelConfig(int32_t iLevel) const
{
	const auto iter = m_stLevelMap.find(iLevel);
	return iter == m_stLevelMap.end() ? nullptr : &iter->second;
}

const TLogicMatchStoreElem* CLogicConfigMatchPvpParser::GetMatchStoreConfig(int32_t iStoreID) const
{
	const auto iter = m_stStoreMap.find(iStoreID);
	return iter == m_stStoreMap.end() ? nullptr : &iter->second;
}

const TLogicMatchDailyAwardElem* CLogicConfigMatchPvpParser::GetDailyAwardConfig(int32_t iLevel) const
{
	const auto iter = m_stDailyAwardMap.find(iLevel);
	return iter == m_stDailyAwardMap.end() ? nullptr : &iter->second;
}

const std::vector<CPackGameItem>* CLogicConfigMatchPvpParser::GetMaxAwardConfig(int32_t iLevel) const
{
	const auto iter = m_stMaxAwardMap.find(iLevel);
	return iter == m_stMaxAwardMap.end() ? nullptr : &iter->second;
}

std::pair<bool, std::string>
CLogicConfigFairPvpParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

		CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"));

		stXML.IntoElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("fair_info"));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("total_quality"), m_iTotalQuality));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("total_win_count"), m_iTotalWinCount));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("total_fail_count"), m_iTotalFailCount));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("option_card_count"), m_iOptionCardCount));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("refresh_item_type"), m_stRefreshItem.m_iItemType));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("refresh_item_id"), m_stRefreshItem.m_iCardID));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("refresh_item_amount"), m_stRefreshItem.m_iNum));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("fair_open"));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("duration_second"), m_iOpenDurationSecond));
		for (int32_t i = 1; i < 10; ++i)
		{
			if (!stXML.GetAttrib("start_week_day_" + std::to_string(i)).empty())
			{
				int32_t iStartWeekDay = 0, iStartHour = 0;
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("start_week_day_" + std::to_string(i)), iStartWeekDay));
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("start_hour_" + std::to_string(i)), iStartHour));
				m_stOpenWeekDayVec.emplace_back(iStartWeekDay, iStartHour);
			}
		}
				
		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("fair_attr"));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("card_color"), m_iFightCardColor));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("card_star"), m_iFightCardStar));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("card_level"), m_iFightCardLevel));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("skill_level1"), m_iFightSkillLevel1));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("skill_level2"), m_iFightSkillLevel2));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("skill_level3"), m_iFightSkillLevel3));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("skill_level4"), m_iFightSkillLevel4));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("win_data"));
		stXML.IntoElem();
		while (stXML.FindElem("count"))
		{
			int32_t iCount = 0;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("count"), iCount));

			TLogicFairPvpWinElem stElem;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fhp"), stElem.m_dHp));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fatk"), stElem.m_dAtk));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fdef"), stElem.m_dDef));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fcri"), stElem.m_dCri));
			stElem.m_strSender = stXML.GetAttrib("sender");
			stElem.m_strTitle = stXML.GetAttrib("title");
			stElem.m_strContent = stXML.GetAttrib("content");
			stElem.m_strBroadCastID = stXML.GetAttrib("broadcast");
			for (int32_t i = 1; i < 10; ++i)
			{
				if (!stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
				{
					CPackGameItem stItem;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)), stItem.m_iItemType));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)), stItem.m_iCardID));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)), stItem.m_iNum));
					stElem.m_stBonusItemVec.push_back(std::move(stItem));
				}

				if (!stXML.GetAttrib("not_match_card_" + std::to_string(i)).empty())
				{
					int32_t iNotCardID = 0;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("not_match_card_" + std::to_string(i)), iNotCardID));
					stElem.m_iNotMatchCardVec.push_back(iNotCardID);
				}
			}
			m_stWinMap[iCount] = std::move(stElem);
		}
		stXML.OutOfElem();//win_data

		if (stXML.FindElem("activity_win_data"))
		{
			stXML.IntoElem();
			while (stXML.FindElem("count"))
			{
				int32_t iCount = 0;
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("count"), iCount));

				TLogicFairPvpWinElem stElem;
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fhp"), stElem.m_dHp));
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fatk"), stElem.m_dAtk));
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fdef"), stElem.m_dDef));
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fcri"), stElem.m_dCri));
				stElem.m_strSender = stXML.GetAttrib("sender");
				stElem.m_strTitle = stXML.GetAttrib("title");
				stElem.m_strContent = stXML.GetAttrib("content");
				stElem.m_strBroadCastID = stXML.GetAttrib("broadcast");
				for (int32_t i = 1; i < 10; ++i)
				{
					if (!stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
					{
						CPackGameItem stItem;
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)), stItem.m_iItemType));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)), stItem.m_iCardID));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)), stItem.m_iNum));
						stElem.m_stBonusItemVec.push_back(std::move(stItem));
					}

					if (!stXML.GetAttrib("not_match_card_" + std::to_string(i)).empty())
					{
						int32_t iNotCardID = 0;
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("not_match_card_" + std::to_string(i)), iNotCardID));
						stElem.m_iNotMatchCardVec.push_back(iNotCardID);
					}
				}
				m_stActivityWinMap[iCount] = std::move(stElem);
			}
			stXML.OutOfElem();//activity_win_data
		}		

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("fair_robot"));
		stXML.IntoElem();
		while (stXML.FindElem("robot"))
		{
			TLogicFairPvpRobotElem stElem;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), stElem.m_iID));
			stElem.m_strName = stXML.GetAttrib("name");
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("avatar_id"), stElem.m_iAvatarID));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("avatar_border"), stElem.m_iAvatarBorder));
			for (int32_t i = 1; i <= 5; ++i)
			{
				stElem.m_iCardVec.push_back(atoi(stXML.GetAttrib("card_id_" + std::to_string(i)).c_str()));
			}
			m_stRobotVec.push_back(std::move(stElem));
		}
		stXML.OutOfElem();//fair_robot

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("random_option_card"));
		stXML.IntoElem();
		while (stXML.FindElem("random"))
		{
			int32_t iQuality = 0, iMaxCount = 0, iMinCount = 0;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("hero_quality"), iQuality));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("min_count"), iMinCount));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("max_count"), iMaxCount));
			m_stRandomCardMap[iQuality] = { iMinCount, iMaxCount };
		}
		stXML.OutOfElem();//fair_robot

		stXML.OutOfElem();//root

	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

TLogicFairPvpRobotElem CLogicConfigFairPvpParser::RandomRobot() const
{
	TLogicFairPvpRobotElem stRobot;
	if (!m_stRobotVec.empty())
	{
		stRobot = m_stRobotVec[CLogicCommon::GetRandNum() % m_stRobotVec.size()];
	}
	return stRobot;
}

const TLogicFairPvpWinElem* CLogicConfigFairPvpParser::GetFairPvpWinConfig(int32_t iWinCount, int32_t iTime) const
{
	const auto isInActivityTime = CLogicConfig::GetAwardConfig().IsInActivityTime(CEnumActivityTimeType::FAIR_PVP, iTime);
	const auto& winMap = isInActivityTime ? m_stActivityWinMap : m_stWinMap;
	const auto iter = winMap.find(iWinCount);
	return iter == winMap.end() ? nullptr : &iter->second;
}

bool CLogicConfigFairPvpParser::IsActivityOpen() const
{
	int32_t wday = CLogicCommon::GetDate().tm_wday;
	if (0 == wday)
	{
		wday = 7;
	}

	for (const auto& open : m_stOpenWeekDayVec)
	{
		if (wday == open.first)
		{
			return true;
		}
	}
	return false;
}
