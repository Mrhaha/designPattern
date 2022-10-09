#include <logic_link_common.h>
#include "logic_config.h"
#include "logic_config_entire_war.h"
#include "Markup.h"

std::pair<bool, std::string>
CLogicConfigEntireWarParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

		CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("entire_war"));

		stXML.IntoElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
		m_iInitActionCoin = atoi(stXML.GetAttrib("init_action_coin").c_str());
		m_iMaxActionCoin = atoi(stXML.GetAttrib("max_action_coin").c_str());
		m_iActionCoinInterval = atoi(stXML.GetAttrib("action_coin_interval").c_str());
		CONFIG_ASSERT_EXCEPTION(m_iMaxActionCoin > 0 && m_iActionCoinInterval > 0);

		m_iDefaultTreasureCount = atoi(stXML.GetAttrib("default_treasure_count").c_str());
		m_iMonthAddTreasureCount = atoi(stXML.GetAttrib("month_add_treasure_count").c_str());
		m_iSilverAddTreasureCount = atoi(stXML.GetAttrib("silver_month_add_treasure_count").c_str());
		m_iGoldAddTreasureCount = atoi(stXML.GetAttrib("gold_month_add_treasure_count").c_str());

		m_strStartMailSender = stXML.GetAttrib("strat_mail_sender");
		m_strStartMailTitle = stXML.GetAttrib("start_mail_title");
		m_strStartMailContent = stXML.GetAttrib("start_mail_content");

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("world_level_ratio"));
		stXML.IntoElem();
		while (stXML.FindElem("ratio"))
		{
			const int32_t iLevel = atoi(stXML.GetAttrib("level").c_str());
			const double dRatio = atof(stXML.GetAttrib("ratio").c_str());
			m_stWorldLevelRatio[iLevel] = dRatio;
		}
		stXML.OutOfElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("marketlist"));
		stXML.IntoElem();

		while (stXML.FindElem("market"))
		{
			const int32_t iMarketID = atoi(stXML.GetAttrib("id").c_str());

			stXML.IntoElem();

			while (stXML.FindElem("market_lv"))
			{
				const int32_t iMarketLevel = atoi(stXML.GetAttrib("market_lv").c_str());

				stXML.IntoElem();

				while (stXML.FindElem("item"))
				{
					LiLogicEntireWarMarketBagVectorElem stBagElem;
					for (int32_t i = 1; i < 10; ++i)
					{
						if (stXML.GetAttrib("bag_" + std::to_string(i)).empty())
							continue;

						const std::string strBag = stXML.GetAttrib("bag_" + std::to_string(i));
						const int32_t iBagRatio = atoi(stXML.GetAttrib("bag_rate_" + std::to_string(i)).c_str());
						stBagElem.emplace_back(strBag, iBagRatio);
					}
					m_stMarketVec[iMarketID][iMarketLevel].push_back(std::move(stBagElem));
				}

				stXML.OutOfElem();
			}

			stXML.OutOfElem();

		}

		stXML.OutOfElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("treasurelist"));
		stXML.IntoElem();
		while (stXML.FindElem("treasure"))
		{
			const int32_t iTreasureID = atoi(stXML.GetAttrib("id").c_str());

			stXML.IntoElem();

			while (stXML.FindElem("treasure_lv"))
			{
				const int32_t iLevel = atoi(stXML.GetAttrib("level").c_str());

				for (int32_t i = 1; i < 10; ++i)
				{
					if (stXML.GetAttrib("bag_" + std::to_string(i)).empty())
						continue;

					const std::string strBag = stXML.GetAttrib("bag_" + std::to_string(i));
                    const auto iterBag = CLogicConfig::GetCardbagConfig().m_stCardbagConfig.find(strBag);
                    CONFIG_ASSERT_EXCEPTION(iterBag != CLogicConfig::GetCardbagConfig().m_stCardbagConfig.end());
                    CONFIG_ASSERT_EXCEPTION(iterBag->second.m_stCardbagElem.size() == CLogicConfigDefine::ENTIRE_WAR_TREASURE_COUNT);

					const int32_t iBagRatio = atoi(stXML.GetAttrib("bag_rate_" + std::to_string(i)).c_str());
					m_stTreasureVec[iTreasureID][iLevel].emplace_back(strBag, iBagRatio);
				}
			}

			stXML.OutOfElem();
		}
		stXML.OutOfElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("military_rank"));
		stXML.IntoElem();
		while (stXML.FindElem("rank"))
		{
			const int32_t id = atoi(stXML.GetAttrib("id").c_str());
			
			TLogicEntireWarMilitaryRankElem stElem;
			stElem.m_iATK = atoi(stXML.GetAttrib("atk").c_str());
			stElem.m_iDEF = atoi(stXML.GetAttrib("def").c_str());
			stElem.m_iHP = atoi(stXML.GetAttrib("hp").c_str());
			stElem.m_iCRI = atoi(stXML.GetAttrib("cri").c_str());
			stElem.m_iCostContribution = atoi(stXML.GetAttrib("cost_contribution").c_str());

			m_stMilitaryRankMap[id] = std::move(stElem);
		}

		stXML.OutOfElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("final_reward"));
		stXML.IntoElem();

		while (stXML.FindElem("reward"))
		{
			const int32_t id = atoi(stXML.GetAttrib("id").c_str());

			TLogicEntireWarFinalAwardElem stElem;
			stElem.m_iLevelCount = atoi(stXML.GetAttrib("level_count").c_str());
			for (int32_t i = 1 ; i < 10; ++i)
			{
				if (stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
					continue;

				CPackGameItem stItem;
				stItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).c_str());
				stItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)).c_str());
				stItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)).c_str());
				stElem.m_stBonusItemVec.push_back(std::move(stItem));
			}

			m_stFinalAwardMap[id] = std::move(stElem);
		}

		stXML.OutOfElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("rank_reward"));

		stXML.IntoElem();

		while (stXML.FindElem("rank"))
		{
			TLogicEntireWarRankAwardElem stElem;
			stElem.m_iMinRank = atoi(stXML.GetAttrib("min").c_str());
			stElem.m_iMaxRank = atoi(stXML.GetAttrib("max").c_str());
			stElem.m_strMailTitle = stXML.GetAttrib("mail_title");
			stElem.m_strMailSender = stXML.GetAttrib("mail_sender");
			stElem.m_strMailContent = stXML.GetAttrib("mail_content");
			for (int32_t i = 1; i < 10; ++i)
			{
				if (stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
					continue;

				CPackGameItem stItem;
				stItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).c_str());
				stItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)).c_str());
				stItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)).c_str());
				stElem.m_stBonusItemVec.push_back(std::move(stItem));
			}
			m_stRankAwardMap[stElem.m_iMaxRank] = stElem;
		}

		stXML.OutOfElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("worship"));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("worship_charm"), m_iWorshipCharm));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("double_probability"), m_iDoubleProbability));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("three_probability"), m_iThreeProbability));
		CONFIG_ASSERT_EXCEPTION(m_iWorshipCharm > 0);

		for (int32_t i = 1; i < 10; ++i)
		{
			if (stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
				continue;

			CPackGameItem stItem;
			stItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).c_str());
			stItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)).c_str());
			stItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)).c_str());
			m_stWorshipItemVec.push_back(std::move(stItem));
		}

		for (int32_t i = 1; i < 10; ++i)
		{
			if (stXML.GetAttrib("worship_item_type_" + std::to_string(i)).empty())
				continue;

			CPackGameItem stItem;
			stItem.m_iItemType = atoi(stXML.GetAttrib("worship_item_type_" + std::to_string(i)).c_str());
			stItem.m_iCardID = atoi(stXML.GetAttrib("worship_item_id_" + std::to_string(i)).c_str());
			stItem.m_iNum = atoi(stXML.GetAttrib("worship_item_num_" + std::to_string(i)).c_str());
			m_stWarGodWorshippedItemVec.push_back(std::move(stItem));
		}

		m_strWarGodWorshippedMailTitle = stXML.GetAttrib("mail_title");
		m_strWarGodWorshippedMailSender = stXML.GetAttrib("mail_sender");
		m_stWarGodWorshippedMailContent = CFormatString::Compile(stXML.GetAttrib("mail_content"));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("buy_action_coin"));
		stXML.IntoElem();
		while (stXML.FindElem("action"))
		{
			const int32_t iCount = atoi(stXML.GetAttrib("count").c_str());
			const int32_t iCost = atoi(stXML.GetAttrib("cost").c_str());
			m_stBuyActionCoinCost[iCount] = iCost;
		}
		stXML.OutOfElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("win_count_type"));
		stXML.IntoElem();
		while (stXML.FindElem("type"))
		{
			const int32_t id = atoi(stXML.GetAttrib("id").c_str());

			stXML.IntoElem();

			while (stXML.FindElem("wincount"))
			{
				const int32_t iLevel = atoi(stXML.GetAttrib("level").c_str());
				const int32_t iCount = atoi(stXML.GetAttrib("count").c_str());

				CONFIG_ASSERT_EXCEPTION(id > 0);
				CONFIG_ASSERT_EXCEPTION(iLevel > 0);

				m_iWinCountMap[id][iLevel] = iCount;
			}

			stXML.OutOfElem();
		}
		stXML.OutOfElem();

		stXML.OutOfElem();
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

double CLogicConfigEntireWarParser::GetWorldLevelRatio(int32_t iLevel) const
{
	auto iter = m_stWorldLevelRatio.upper_bound(iLevel);
	if (iter != m_stWorldLevelRatio.begin())
	{
		--iter;
	}
	return iter == m_stWorldLevelRatio.end() ? 1.0 : iter->second;
}

const LiLogicEntireWarMarketElem* CLogicConfigEntireWarParser::GetEntireWarMarketConfig(int32_t iWarLevelID, int32_t iMarketLevel) const
{
	const auto iterMarketWar = m_stMarketVec.find(iWarLevelID);
	if (iterMarketWar == m_stMarketVec.end())
	{
		return nullptr;
	}

	auto iterMarket = iterMarketWar->second.upper_bound(iMarketLevel);
	if (iterMarket != iterMarketWar->second.begin())
	{
		--iterMarket;
	}
	return iterMarket == iterMarketWar->second.end() ? nullptr : &iterMarket->second;
}

const LiLogicEntireWarTreasureElem* CLogicConfigEntireWarParser::GetEntireWarTreasureConfig(int32_t iWarLevelID, int32_t iWorldLevel) const
{
	const auto iterTreasureWar = m_stTreasureVec.find(iWarLevelID);
	if (iterTreasureWar == m_stTreasureVec.end())
	{
		return nullptr;
	}

	auto iterTreasure = iterTreasureWar->second.upper_bound(iWorldLevel);
	if (iterTreasure != iterTreasureWar->second.begin())
	{
		--iterTreasure;
	}
	return iterTreasure == iterTreasureWar->second.end() ? nullptr : &iterTreasure->second;
}

const TLogicEntireWarRankAwardElem* CLogicConfigEntireWarParser::GetEntireRankAwardConfig(int32_t iRank) const
{
	const auto iter = m_stRankAwardMap.lower_bound(iRank);
	return iter != m_stRankAwardMap.end() && iRank >= iter->second.m_iMinRank && iRank <= iter->second.m_iMaxRank ?
		&iter->second : nullptr;
}

int32_t CLogicConfigEntireWarParser::GetWinCount(int32_t iCountID, int32_t iWarLevel)const
{
	const auto iterType = m_iWinCountMap.find(iCountID);
	if (iterType == m_iWinCountMap.end())
	{
		return 0;
	}

	auto iterLevel = iterType->second.upper_bound(iWarLevel);
	if (iterLevel != iterType->second.begin())
	{
		--iterLevel;
	}

	return iterLevel == iterType->second.end() ? 0 : iterLevel->second;
}

std::string CLogicConfigEntireWarParser::RandEntireWarMarketBag(int32_t iWarLevelID, int32_t iMarketLevel, int32_t iMarketIndex)const
{
    std::string empty;
    const auto *pstMarket = GetEntireWarMarketConfig(iWarLevelID, iMarketLevel);
    if (nullptr == pstMarket)
    {
        return empty;
    }
    const int32_t index = iMarketIndex - 1;
    if (index < 0 || index >= int32_t(pstMarket->size()))
    {
        return empty;
    }

    int32_t iTotalRatio = 0;
    for (const auto& ratio : pstMarket->at(index))
    {
        iTotalRatio += ratio.second;
    }

    if (iTotalRatio <= 0)
    {
        return empty;
    }

    int32_t iRand = CLogicCommon::GetRandNum() % iTotalRatio;
    for (const auto& ratio : pstMarket->at(index))
    {
        if (iRand < ratio.second)
        {
            return ratio.first;
        }

        iRand -= ratio.second;
    }

    return empty;
}

std::string CLogicConfigEntireWarParser::RandEntireWarTreasureBag(int32_t iWarLevelID, int32_t iMarketLevel)const
{
    std::string empty;
    const auto *pstTreasure = GetEntireWarTreasureConfig(iWarLevelID, iMarketLevel);
    if (nullptr == pstTreasure)
    {
        return empty;
    }

    int32_t iTotalRatio = 0;
    for (const auto& ratio : *pstTreasure)
    {
        iTotalRatio += ratio.second;
    }

    if (iTotalRatio <= 0)
    {
        return empty;
    }

    int32_t iRand = CLogicCommon::GetRandNum() % iTotalRatio;
    for (const auto& ratio : *pstTreasure)
    {
        if (iRand < ratio.second)
        {
            return ratio.first;
        }

        iRand -= ratio.second;
    }

    return empty;
}

std::pair<bool, std::string> CLogicConfigEntireWarMapParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

        CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"));

        stXML.IntoElem();

        while (stXML.FindElem("theme"))
        {
            int32_t iThemeID = atoi(stXML.GetAttrib("index").c_str());
            CONFIG_ASSERT_EXCEPTION(iThemeID > 0);
            CONFIG_ASSERT_EXCEPTION(m_stThemeMap.find(iThemeID) == m_stThemeMap.end());

            stXML.IntoElem();

            TLogicEntireWarThemeElem stThemeInfo;
            CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
            stThemeInfo.m_iSimpleGameTimes = atoi(stXML.GetAttrib("simple_game_times").c_str());
            CONFIG_ASSERT_EXCEPTION(stThemeInfo.m_iSimpleGameTimes >= 0);

            CONFIG_ASSERT_EXCEPTION(stXML.FindElem("war"));

            stXML.IntoElem();
            while (stXML.FindElem("map"))
            {
                TLogicEntireWarMapElem stWar;
                stWar.m_iWarID = atoi(stXML.GetAttrib("id").c_str());
                CONFIG_ASSERT_EXCEPTION(0 < stWar.m_iWarID && stWar.m_iWarID < 300);
                CONFIG_ASSERT_EXCEPTION(stThemeInfo.m_stWarMap.find(stWar.m_iWarID) == stThemeInfo.m_stWarMap.end());

                stWar.m_iPreWarID = atoi(stXML.GetAttrib("pre_id").c_str());
                CONFIG_ASSERT_EXCEPTION(stWar.m_iPreWarID != stWar.m_iWarID);

                stWar.m_strName = stXML.GetAttrib("name");
                stWar.m_iIconType = atoi(stXML.GetAttrib("icon_type").c_str());
                stWar.m_iNotify = atoi(stXML.GetAttrib("notify").c_str());
                stWar.m_iNodeType = atoi(stXML.GetAttrib("node_type").c_str());
                CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumEntireWarNodeType(stWar.m_iNodeType));

                stWar.m_iNodePara1 = atoi(stXML.GetAttrib("node_para1").c_str());
                stWar.m_iNodePara2 = atoi(stXML.GetAttrib("node_para2").c_str());
                stWar.m_iWinCountID = atoi(stXML.GetAttrib("win_count_id").c_str());
                stWar.m_iPower = atoi(stXML.GetAttrib("power").c_str());

                // 检查
                if(stWar.m_iNodeType == CLogicConfigDefine::EEWNT_Boss)
                {
                    CONFIG_ASSERT_EXCEPTION(nullptr != CLogicConfig::GetLevelConfig().GetLevelConfigElem(stWar.m_iNodePara1));

                    // 检查BOSS血量
                    /*if(CLogicConfig::GetLevelConfig().GetLevelBossHp(stWar.m_iNodePara1) <= 10000000)
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:boss hp is invalid. m_iWarID:" << stWar.m_iWarID << std::endl;
                        throw std::exception();
                    }*/
                }
                else if(stWar.m_iNodeType == CLogicConfigDefine::EEWNT_PVE || stWar.m_iNodeType == CLogicConfigDefine::EEWNT_Creeps)
                {
                    CONFIG_ASSERT_EXCEPTION(stWar.m_iNodePara1 <= stWar.m_iNodePara2);

                    for(int iLevelID = stWar.m_iNodePara1; iLevelID <= stWar.m_iNodePara2; ++iLevelID)
                        CONFIG_ASSERT_EXCEPTION(nullptr != CLogicConfig::GetLevelConfig().GetLevelConfigElem(iLevelID));

                    if(stWar.m_iNodeType == CLogicConfigDefine::EEWNT_PVE)
                        CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetEntireWarConfig().m_iWinCountMap.find(stWar.m_iWinCountID) != CLogicConfig::GetEntireWarConfig().m_iWinCountMap.end());
                }
                else if(stWar.m_iNodeType == CLogicConfigDefine::EEWNT_PVP || stWar.m_iNodeType == CLogicConfigDefine::EEWNT_Question)
                {
                    if(stWar.m_iNodeType == CLogicConfigDefine::EEWNT_PVP)
                        CONFIG_ASSERT_EXCEPTION(stWar.m_iNodePara1 <= stWar.m_iNodePara2);

                    CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetEntireWarConfig().m_iWinCountMap.find(stWar.m_iWinCountID) != CLogicConfig::GetEntireWarConfig().m_iWinCountMap.end());
                }
                else if(stWar.m_iNodeType == CLogicConfigDefine::EEWNT_Market)
                {
                    CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetEntireWarConfig().m_stMarketVec.find(stWar.m_iNodePara1) != CLogicConfig::GetEntireWarConfig().m_stMarketVec.end());
                }
                else if(stWar.m_iNodeType == CLogicConfigDefine::EEWNT_Treasure)
                {
                    CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetEntireWarConfig().m_stTreasureVec.find(stWar.m_iNodePara1) != CLogicConfig::GetEntireWarConfig().m_stTreasureVec.end());
                }
                else if(stWar.m_iNodeType == CLogicConfigDefine::EEWNT_SimpleGame)
                {
                    CONFIG_ASSERT_EXCEPTION(stWar.m_iNodePara1 > 0);
                    CONFIG_ASSERT_EXCEPTION(nullptr != CLogicConfig::GetRankConfig().GetConfig(stWar.m_iNodePara2));
                }

                for (int32_t i = 1 ; i <= 5; ++i)
                {
                    if (stXML.GetAttrib("limit_type_" + std::to_string(i)).empty())
                        break;

                    stWar.m_stLevelLimitVec.emplace_back(
                        atoi(stXML.GetAttrib("limit_type_" + std::to_string(i)).c_str()),
                        atoi(stXML.GetAttrib("limit_num_" + std::to_string(i)).c_str())
                    );
                }

                stWar.m_iBonusBuffType = atoi(stXML.GetAttrib("bonus_buff_type").c_str());
                stWar.m_iBonusBuffPercentage = atoi(stXML.GetAttrib("bonus_buff_percentage").c_str());
                stWar.m_iBonusBuffTime = atoi(stXML.GetAttrib("bonus_buff_time").c_str());
                stWar.m_iBonusContribution = atoi(stXML.GetAttrib("bonus_contribution").c_str());
                stWar.m_iBonusActionCoin = atoi(stXML.GetAttrib("bonus_action_coin").c_str());
                for (int32_t i = 1; i <= 10; ++i)
                {
                    if (stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty() || stXML.GetAttrib("bonus_item_type_" + std::to_string(i)) == "0")
                        break;

                    stWar.m_stBonusItemVec.emplace_back(
                        atoi(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).c_str()),
                        atoi(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)).c_str()),
                        atoi(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)).c_str())
                    );
                }

                stThemeInfo.m_stWarMap[stWar.m_iWarID] = std::move(stWar);
            }
            stXML.OutOfElem();

            for(auto iter = stThemeInfo.m_stWarMap.begin(); iter != stThemeInfo.m_stWarMap.end(); ++iter)
            {
                if(iter->second.m_iPreWarID > 0)
                {
                    auto iterPreWar = stThemeInfo.m_stWarMap.find(iter->second.m_iPreWarID);
                    if(iterPreWar == stThemeInfo.m_stWarMap.end())
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:m_iPreWarID is invalid. m_iPreWarID:" << iter->second.m_iPreWarID << std::endl;
                        throw std::exception();
                    }

                    // 前置关卡不能为集市，宝藏，刷怪点等类型关卡
                    if(iterPreWar->second.m_iNodeType != CLogicConfigDefine::EEWNT_PVE
                        && iterPreWar->second.m_iNodeType != CLogicConfigDefine::EEWNT_PVP
                        && iterPreWar->second.m_iNodeType != CLogicConfigDefine::EEWNT_Question
                        && iterPreWar->second.m_iNodeType != CLogicConfigDefine::EEWNT_Boss)
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:node pre_id`s type is invalid. node_id:" << iter->first << std::endl;
                        throw std::exception();
                    }

                    iterPreWar->second.m_stUnlockList.emplace_back(iter->first);
                }
            }

            m_stThemeMap[iThemeID] = std::move(stThemeInfo);
            m_stThemeIDVec.emplace_back(iThemeID);

            stXML.OutOfElem();
        }
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

int32_t CLogicConfigEntireWarMapParser::GetNextThemeID(int32_t iThemeID) const
{
    if(m_stThemeIDVec.empty()) return 0;

    size_t iIndex = 0;
    for(; iIndex < m_stThemeIDVec.size(); ++iIndex)
    {
        if(iThemeID == m_stThemeIDVec[iIndex])
            break;
    }

    ++iIndex;

    if(iIndex < m_stThemeIDVec.size())
        return m_stThemeIDVec[iIndex];
    else
        return m_stThemeIDVec[0];
}

const TLogicEntireWarMapElem* CLogicConfigEntireWarMapParser::GetConfig(int32_t iThemeID, int32_t iWarID) const
{
    auto iterTheme = m_stThemeMap.find(iThemeID);
    if(iterTheme == m_stThemeMap.end())
        return nullptr;

    auto iterWar = iterTheme->second.m_stWarMap.find(iWarID);
    return iterWar == iterTheme->second.m_stWarMap.end() ? nullptr : &iterWar->second;
}

std::pair<bool, std::string>
CLogicConfigQuestionParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

		CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("question"));

		stXML.IntoElem();

		while (stXML.FindElem("problem"))
		{
			m_stQuestionVec.push_back(atoi(stXML.GetAttrib("id").c_str()));
		}

		stXML.OutOfElem();
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string> CLogicConfigRewardBagParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

        CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"));

        stXML.IntoElem();

        while (stXML.FindElem("bag"))
        {
            std::string strBagName = stXML.GetAttrib("id");
            CONFIG_ASSERT_EXCEPTION(m_stBagMap.find(strBagName) == m_stBagMap.end());

            auto& stKeyMap = m_stBagMap[strBagName];

            stXML.IntoElem();
            while (stXML.FindElem("key_para"))
            {
                int32_t iKeyPara = ::atoi(stXML.GetAttrib("key").c_str());
                CONFIG_ASSERT_EXCEPTION(stKeyMap.find(iKeyPara) == stKeyMap.end());

                auto& stKeyData = stKeyMap[iKeyPara];

                stXML.IntoElem();
                while (stXML.FindElem("key_para_ex"))
                {
                    int32_t iKeyParaEx = ::atoi(stXML.GetAttrib("key_ex").c_str());
                    CONFIG_ASSERT_EXCEPTION(stKeyData.find(iKeyParaEx) == stKeyData.end());

                    auto& stBagList = stKeyData[iKeyParaEx];

                    stXML.IntoElem();
                    while (stXML.FindElem("rate"))
                    {
                        TLogicRewardBagList stRate;
                        stRate.m_iWeight = ::atoi(stXML.GetAttrib("rate").c_str());
                        CONFIG_ASSERT_EXCEPTION(stRate.m_iWeight > 0);
                        stBagList.m_iTotalWeight += stRate.m_iWeight;

                        stXML.IntoElem();
                        while(stXML.FindElem("index"))
                        {
                            TLogicRewardBagElem stElem;
                            stElem.m_iWeight = ::atoi(stXML.GetAttrib("bag_rate").c_str());
                            CONFIG_ASSERT_EXCEPTION(stElem.m_iWeight > 0);
                            stRate.m_iTotalWeight += stElem.m_iWeight;

                            stElem.m_strTotalBag = stXML.GetAttrib("total_bag");
                            if(!stElem.m_strTotalBag.empty())
                            {
                                if(CLogicConfig::GetCardbagConfig().m_stCardbagConfig.find(stElem.m_strTotalBag) == CLogicConfig::GetCardbagConfig().m_stCardbagConfig.end())
                                {
                                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "invalid total_bag:" << stElem.m_strTotalBag << std::endl;
                                    throw std::exception();
                                }
                            }

                            for(int i = 1; i <= 10; ++i)
                            {
                                if(stXML.GetAttrib("card_bag" + std::to_string(i)).empty()) break;

                                std::string strCardBag = stXML.GetAttrib("card_bag" + std::to_string(i));
                                if(CLogicConfig::GetCardbagConfig().m_stCardbagConfig.find(strCardBag) == CLogicConfig::GetCardbagConfig().m_stCardbagConfig.end())
                                {
                                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "invalid card_bag:" << strCardBag << std::endl;
                                    throw std::exception();
                                }
                                stElem.m_stCardBagVec.push_back(strCardBag);
                            }

                            stRate.m_stBagList.emplace_back(std::move(stElem));
                        }

                        CONFIG_ASSERT_EXCEPTION(stRate.m_iTotalWeight > 0);
                        stBagList.m_stRateList.emplace_back(std::move(stRate));

                        stXML.OutOfElem();
                    }

                    CONFIG_ASSERT_EXCEPTION(stBagList.m_iTotalWeight > 0);

                    stXML.OutOfElem();
                }

                CONFIG_ASSERT_EXCEPTION(!stKeyData.empty());

                stXML.OutOfElem();
            }

            CONFIG_ASSERT_EXCEPTION(!stKeyMap.empty());

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

std::vector<CPackGameItem> CLogicConfigRewardBagParser::GetReward(const std::string& strName, const int32_t& iKey, const int32_t& iKeyEx) const
{
    std::vector<CPackGameItem> stReward;
    auto iterBag = m_stBagMap.find(strName);
    if(iterBag == m_stBagMap.end()) return stReward;

    auto iterKey = iterBag->second.lower_bound(iKey);
    if(iterKey == iterBag->second.end()) return stReward;

    auto iterKeyEx = iterKey->second.lower_bound(iKeyEx);
    if(iterKeyEx == iterKey->second.end()) return stReward;

    const TLogicRewardBagElem* pElem = nullptr;
    int32_t iRandWeight = CLogicCommon::GetRandNum() % iterKeyEx->second.m_iTotalWeight;
    for(auto& rate : iterKeyEx->second.m_stRateList)
    {
        if(iRandWeight < rate.m_iWeight)
        {
            iRandWeight = CLogicCommon::GetRandNum() % rate.m_iTotalWeight;
            for(auto& bag : rate.m_stBagList)
            {
                if(iRandWeight < bag.m_iWeight)
                {
                    pElem = &bag;
                    break;
                }

                iRandWeight -= bag.m_iWeight;
            }

            break;
        }

        iRandWeight -= rate.m_iWeight;
    }

    if(nullptr == pElem) return stReward;

    // 固定奖励
    if(!pElem->m_strTotalBag.empty())
    {
        auto iterCardBag = CLogicConfig::GetCardbagConfig().m_stCardbagConfig.find(pElem->m_strTotalBag);
        if(iterCardBag != CLogicConfig::GetCardbagConfig().m_stCardbagConfig.end())
        {
            for(auto& item : iterCardBag->second.m_stCardbagElem)
            {
                stReward.emplace_back(item.m_iItemType, item.m_iCardID, item.m_iNum);
            }
        }
    }

    // 随机奖励
    for(auto& cardBagName : pElem->m_stCardBagVec)
    {
        CLogicConfig::GetCardbagConfig().GetRandBonusByName(cardBagName, stReward);
    }

    return stReward;
}

std::pair<bool, std::string> CLogicConfigEntireWarLimitBossParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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
        m_iScoreRankID = ::atoi(stXML.GetAttrib("score_rank_id").c_str());
        CONFIG_ASSERT_EXCEPTION(nullptr != CLogicConfig::GetRankConfig().GetConfig(m_iScoreRankID));
        m_iBossActionCoin = ::atoi(stXML.GetAttrib("boss_action_coin").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iBossActionCoin > 0);
        m_iFightConsumeCoin = ::atoi(stXML.GetAttrib("fight_consume_coin").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iFightConsumeCoin > 0);
        m_iDailyBossTimes = ::atoi(stXML.GetAttrib("daily_boss_times").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iDailyBossTimes >= 0);
        m_iDailyBoxTimes = ::atoi(stXML.GetAttrib("daily_box_times").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iDailyBoxTimes >= 0);

        m_iEmptyRate = ::atoi(stXML.GetAttrib("empty_rate").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iEmptyRate > 0);
        m_iBoxRate = ::atoi(stXML.GetAttrib("box_rate").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iBoxRate >= 0);
        m_iBossRate = ::atoi(stXML.GetAttrib("boss_rate").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iBossRate >= 0);

        m_iBossDeathWaitTime = ::atoi(stXML.GetAttrib("boss_death_wait_time").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iBossDeathWaitTime > 0 && m_iBossDeathWaitTime < 300);
        m_iBaseScore = ::atoi(stXML.GetAttrib("base_score").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iBaseScore > 0);
        m_strBoxRewardBagID = stXML.GetAttrib("box_reward_bag_id");
        CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetRewardBagConfig().m_stBagMap.find(m_strBoxRewardBagID) != CLogicConfig::GetRewardBagConfig().m_stBagMap.end());
        m_iBoxDurTime = ::atoi(stXML.GetAttrib("box_dur_time").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iBoxDurTime > 0);

        m_iAcceptBossNum = ::atoi(stXML.GetAttrib("accept_boss_num").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iAcceptBossNum > 0 && m_iAcceptBossNum <= E_MaxAcceptBossNum);
        m_iAttackerNum = ::atoi(stXML.GetAttrib("attacker_num").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iAttackerNum > 0 && m_iAttackerNum <= E_MaxAttackerNum);
        m_iRandBossNum = ::atoi(stXML.GetAttrib("rand_boss_num").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iRandBossNum >= 0 && m_iRandBossNum < E_MaxAcceptBossNum);
        m_iLimitRefreshNewBossTime = ::atoi(stXML.GetAttrib("limit_refresh_new_boss_time").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iLimitRefreshNewBossTime >= 5400);
        m_iLastBossEndTime = ::atoi(stXML.GetAttrib("last_boss_end_time").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iLastBossEndTime > 1800 && m_iLastBossEndTime < m_iLimitRefreshNewBossTime);

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("mail"));
        m_strMailSender = stXML.GetAttrib("mail_sender");
        m_strKillerMailTitle = stXML.GetAttrib("killer_mail_title");
        m_strKillerMailContent = stXML.GetAttrib("killer_mail_content");
        m_strOwnerMailTitle = stXML.GetAttrib("owner_mail_title");
        m_strOwnerMailContent = stXML.GetAttrib("owner_mail_content");

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("boss_fight_reward"));
        for(int i = 1; i <= 10; ++i)
        {
            if(stXML.GetAttrib("reward_type" + std::to_string(i)).empty()) break;

            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + std::to_string(i)).c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + std::to_string(i)).c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + std::to_string(i)).c_str());
            m_stBossFightReward.emplace_back(stItem);
        }

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("boss_hp_reward_pro"));
        stXML.IntoElem();
        while (stXML.FindElem("hp"))
        {
            int32_t iHp = ::atoi(stXML.GetAttrib("max_hp").c_str());
            CONFIG_ASSERT_EXCEPTION(iHp > 0);
            int32_t iRewardPro = ::atoi(stXML.GetAttrib("reward_pro").c_str());
            CONFIG_ASSERT_EXCEPTION(iRewardPro > 0);
            m_stBossHpRewardPro.emplace(iHp, iRewardPro);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("boss"));

        stXML.IntoElem();
        while (stXML.FindElem("hard"))
        {
            TLogicLimitBossHard stHard;
            stHard.m_iHard = ::atoi(stXML.GetAttrib("hard").c_str());
            CONFIG_ASSERT_EXCEPTION(stHard.m_iHard > 0 && m_stHardInfo.find(stHard.m_iHard) == m_stHardInfo.end());
            stHard.m_iDurTime = ::atoi(stXML.GetAttrib("dur_time").c_str());
            CONFIG_ASSERT_EXCEPTION(stHard.m_iDurTime > 0);
            stHard.m_iOpenTime = ::atoi(stXML.GetAttrib("open_time").c_str());
            CONFIG_ASSERT_EXCEPTION(stHard.m_iOpenTime >= 0 && stHard.m_iOpenTime <= stHard.m_iDurTime);
            stHard.m_iKillRewardCoin = ::atoi(stXML.GetAttrib("kill_reward_coin").c_str());
            CONFIG_ASSERT_EXCEPTION(stHard.m_iKillRewardCoin >= 0);
            stHard.m_iHardScore = ::atoi(stXML.GetAttrib("hard_score").c_str());
            CONFIG_ASSERT_EXCEPTION(stHard.m_iHardScore > 0);
            stHard.m_strKillRewardBagID = stXML.GetAttrib("kill_reward_bag_id");
            CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetRewardBagConfig().m_stBagMap.find(stHard.m_strKillRewardBagID) != CLogicConfig::GetRewardBagConfig().m_stBagMap.end());
            stHard.m_strFinderRewardBagID = stXML.GetAttrib("finder_reward_bag_id");
            CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetRewardBagConfig().m_stBagMap.find(stHard.m_strFinderRewardBagID) != CLogicConfig::GetRewardBagConfig().m_stBagMap.end());

            for(int i = 1; i <= 10; ++i)
            {
                if(stXML.GetAttrib("bag" + std::to_string(i)).empty()) break;

                std::string strCardBag = stXML.GetAttrib("bag" + std::to_string(i));
                if(CLogicConfig::GetCardbagConfig().m_stCardbagConfig.find(strCardBag) == CLogicConfig::GetCardbagConfig().m_stCardbagConfig.end())
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "invalid bag:" << strCardBag << std::endl;
                    throw std::exception();
                }
                stHard.m_stBagVec.push_back(strCardBag);
            }

            m_stHardInfo[stHard.m_iHard] = std::move(stHard);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("hard_rate"));
        stXML.IntoElem();
        while (stXML.FindElem("level"))
        {
            int32_t iLevel = ::atoi(stXML.GetAttrib("level").c_str());
            CONFIG_ASSERT_EXCEPTION(iLevel > 0);
            CONFIG_ASSERT_EXCEPTION(m_stHardRateMap.find(iLevel) == m_stHardRateMap.end());

            TLogicLimitBossHardRate stHard;
            stHard.m_iTotalWeight = 0;
            for(int i = 1; i <= 10; ++i)
            {
                if(m_stHardInfo.find(i) == m_stHardInfo.end()) continue;

                int32_t iRate = ::atoi(stXML.GetAttrib("hard" + std::to_string(i)).c_str());
                if(iRate > 0)
                {
                    stHard.m_iTotalWeight += iRate;
                    stHard.m_stHardRate[i] = iRate;
                }
            }

            CONFIG_ASSERT_EXCEPTION(stHard.m_iTotalWeight > 0);

            m_stHardRateMap[iLevel] = std::move(stHard);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("theme_list"));
        stXML.IntoElem();
        while (stXML.FindElem("theme"))
        {
            int32_t iThemeID = ::atoi(stXML.GetAttrib("index").c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetEntireWarMapConfig().m_stThemeMap.find(iThemeID) != CLogicConfig::GetEntireWarMapConfig().m_stThemeMap.end());

            stXML.IntoElem();
            while (stXML.FindElem("hard"))
            {
                int32_t iHard = ::atoi(stXML.GetAttrib("hard").c_str());
                CONFIG_ASSERT_EXCEPTION(m_stHardInfo.find(iHard) != m_stHardInfo.end());
                int32_t iLevelID = ::atoi(stXML.GetAttrib("level_id").c_str());
                if(nullptr == CLogicConfig::GetLevelConfig().GetLevelConfigElem(iLevelID))
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "invalid level_id:" << iLevelID << std::endl;
                    throw std::exception();
                }

                // 检查BOSS血量
                /*if(CLogicConfig::GetLevelConfig().GetLevelBossHp(iLevelID) <= 1000000)
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:boss hp is invalid. levelID:" << iLevelID << std::endl;
                    throw std::exception();
                }*/

                m_stThemeHardLevelIDMap[iThemeID][iHard] = iLevelID;
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION(m_stThemeHardLevelIDMap[iThemeID].size() == m_stHardInfo.size());
        }
        stXML.OutOfElem();

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicLimitBossHard* CLogicConfigEntireWarLimitBossParser::GetHardConfig(int32_t iHard) const
{
    auto iter = m_stHardInfo.find(iHard);
    return (iter == m_stHardInfo.end() ? nullptr : &(iter->second));
}

const TLogicLimitBossHard* CLogicConfigEntireWarLimitBossParser::GetRandHardConfig(int32_t iLevel) const
{
    auto iter = m_stHardRateMap.lower_bound(iLevel);
    if(iter == m_stHardRateMap.end()) return nullptr;

    int32_t iRandWeight = CLogicCommon::GetRandNum() % (iter->second.m_iTotalWeight);
    for(auto& hard : iter->second.m_stHardRate)
    {
        if(iRandWeight < hard.second)
        {
            return GetHardConfig(hard.first);
        }

        iRandWeight -= hard.second;
    }

    return nullptr;
}

int32_t CLogicConfigEntireWarLimitBossParser::GetLevelID(int32_t iThemeID, int32_t iHard) const
{
    auto iterTheme = m_stThemeHardLevelIDMap.find(iThemeID);
    if(iterTheme == m_stThemeHardLevelIDMap.end())
        return 0;

    auto iterLevel = iterTheme->second.find(iHard);
    if(iterLevel == iterTheme->second.end())
        return 0;

    return iterLevel->second;
}

int32_t CLogicConfigEntireWarLimitBossParser::GetHurtRewardPro(int32_t iHurtPrecent) const
{
    auto iter = m_stBossHpRewardPro.lower_bound(iHurtPrecent);
    if(iter == m_stBossHpRewardPro.end())
        return 100;

    return iter->second;
}
