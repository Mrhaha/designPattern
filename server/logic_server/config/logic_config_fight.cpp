#include "logic_config_fight.h"
#include "server_errcode.h"
#include "logic_link_error.h"
#include "Markup.h"
#include "logic_config.h"
#include "logic_link_common.h"

////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigBarrierMapParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("map_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		m_stBarrierMapDetailRef.m_iCoinNum += (::atoi(stXML.GetAttrib("coin").c_str()) + CLogicConfig::GetGlobalConfig().m_iSettlementBonusGold);
		m_stBarrierMapDetailRef.m_iBoxNum += ::atoi(stXML.GetAttrib("box").c_str());

		stXML.IntoElem();

		std::string strMonsterID, strBossID;

		if (false == stXML.FindElem("monster_list"))
		{
			strErrorInfo << "CAN NOT FOUND monster_list" << std::endl;
			break;
		}

		stXML.IntoElem();
		while (stXML.FindElem("enemy"))
		{
			strMonsterID = stXML.GetAttrib("id");
			auto stNoCoinMonsterParser = CLogicConfig::GetNoCoinMonsterConfig();
			auto stNoCoinMonsterIT = stNoCoinMonsterParser.m_stNoCoinMonsterSet.find(strMonsterID);
			if (stNoCoinMonsterIT == stNoCoinMonsterParser.m_stNoCoinMonsterSet.end())
            {
                int iNum = ::atoi(stXML.GetAttrib("number").c_str());
                m_stBarrierMapDetailRef.m_stMonsterMap[strMonsterID] += iNum;
                m_stBarrierMapDetailRef.m_iMaxMonsterNum += iNum;
            }
		}
		stXML.OutOfElem();

		if (stXML.FindElem("boss_list"))
		{
			stXML.IntoElem();
			while (stXML.FindElem("boss"))
			{
				strBossID = stXML.GetAttrib("id");
				auto stNoCoinMonsterParser = CLogicConfig::GetNoCoinMonsterConfig();
				auto stNoCoinBossIT = stNoCoinMonsterParser.m_stNoCoinBossSet.find(strBossID);
				if (stNoCoinBossIT == stNoCoinMonsterParser.m_stNoCoinBossSet.end())
                {
                    m_stBarrierMapDetailRef.m_stBossMap[strBossID].m_iNum += ::atoi(stXML.GetAttrib("number").c_str());
                    m_stBarrierMapDetailRef.m_stBossMap[strBossID].m_iHp += ::atoi(stXML.GetAttrib("hp").c_str());
                }
			}
			stXML.OutOfElem();
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

////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigChapterParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("data_chapter"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		while (true == stXML.FindElem("chapter_types"))
		{
			int iChapterType = ::atoi(stXML.GetAttrib("chapter_type").c_str());

			stXML.IntoElem();
			while (true == stXML.FindElem("chapter"))
			{
				int iChapterID = ::atoi(stXML.GetAttrib("chapter_id").c_str());
				TLogicGameChapterElem stGameChapterElem;
				stGameChapterElem.m_iChapterType = iChapterType;
				stGameChapterElem.m_iPefectScore = ::atoi(stXML.GetAttrib("pefect_point").c_str());
				stGameChapterElem.m_iMaxFightHelperNum = ::atoi(stXML.GetAttrib("help_fight_num").c_str());
				char szStarLimit[64], szItemType[64], szAward[64], szAmount[64];
				CPackGameItem stBonus;
				std::vector<CPackGameItem> stBonusVec;

				stGameChapterElem.m_stStarLimit.push_back(0);
				stGameChapterElem.m_stChapterBonus.push_back(stBonusVec);
				for (int iStarPos = 1;
					iStarPos <= CLogicConfigDefine::MAX_NORMAL_CHAPTER_BONUS_AMOUNT;
					iStarPos++)
				{
					snprintf(szStarLimit, sizeof(szStarLimit), "%s_%d", "star_limit", iStarPos);
					if (stXML.GetAttrib(szStarLimit).empty()) break;

					stGameChapterElem.m_stStarLimit.push_back(
						::atoi(stXML.GetAttrib(szStarLimit).c_str()));

					stBonusVec.clear();
					for (int iBonusPos = 1; iBonusPos < 10; ++iBonusPos)
					{
						snprintf(szItemType, sizeof(szItemType), "%s_%d_%d", "item_type", iStarPos, iBonusPos);
						if (stXML.GetAttrib(szItemType).empty()) break;

						snprintf(szAward, sizeof(szAward), "%s_%d_%d", "item_id", iStarPos, iBonusPos);
						snprintf(szAmount, sizeof(szAmount), "%s_%d_%d", "item_num", iStarPos, iBonusPos);

						stBonus.m_iItemType = ::atoi(stXML.GetAttrib(szItemType).c_str());
						stBonus.m_iCardID = ::atoi(stXML.GetAttrib(szAward).c_str());
						stBonus.m_iNum = ::atoi(stXML.GetAttrib(szAmount).c_str());
						if (stBonus.m_iItemType <= 0) break;

						stBonusVec.push_back(stBonus);
					}
					stGameChapterElem.m_stChapterBonus.push_back(stBonusVec);
				}

				m_stChapterMap[iChapterID] = stGameChapterElem;
			}
			stXML.OutOfElem();
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

const TLogicGameChapterElem*
CLogicConfigChapterParser::GetChapterConfigElem(int iChapterID) const
{
	auto stChapterConfigIT = m_stChapterMap.find(iChapterID);
	if (stChapterConfigIT == m_stChapterMap.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(CHAPTER_ID_NOT_FOUND_IN_CONFIG);
		return nullptr;
	}

	return (&(stChapterConfigIT->second));
}

////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigLevelParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("level_list"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (true == stXML.FindElem("chapter"))
		{
			int iChapterID = atoi(stXML.GetAttrib("chapter_id").c_str());
			stXML.IntoElem();
			while (true == stXML.FindElem("level_id"))
			{
				int iLevelID = ::atoi(stXML.GetAttrib("level_id").c_str());
				CONFIG_ASSERT_EXCEPTION_EX(m_stLevelConfig.find(iLevelID) == m_stLevelConfig.end(), iLevelID);

				TLogicLevelElem stLevelElem;
                stLevelElem.m_iLevelType = ::atoi(stXML.GetAttrib("level_type").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumLevelType(stLevelElem.m_iLevelType), iLevelID)

                stLevelElem.m_iChapterID = iChapterID;
                stLevelElem.m_iLevelTypePara = ::atoi(stXML.GetAttrib("level_type_para").c_str());
				stLevelElem.m_iNeedLevel = ::atoi(stXML.GetAttrib("player_rank").c_str());
				stLevelElem.m_iNeedHeartLevel = ::atoi(stXML.GetAttrib("need_heart_level").c_str());
                stLevelElem.m_iSharedLevel = ::atoi(stXML.GetAttrib("shared_level").c_str());
                stLevelElem.m_iSharedTimes = ::atoi(stXML.GetAttrib("shared_times").c_str());
                stLevelElem.m_iAdvRewardTimes = ::atoi(stXML.GetAttrib("adv_reward_times").c_str());
				stLevelElem.m_iPreLevel = ::atoi(stXML.GetAttrib("pre_level").c_str());
                stLevelElem.m_iTeamType = ::atoi(stXML.GetAttrib("team_type").c_str());
				stLevelElem.m_iEnterTimes = ::atoi(stXML.GetAttrib("enter_times").c_str());
                stLevelElem.m_bFirstPassFree = (bool)::atoi(stXML.GetAttrib("first_pass_free").c_str());
				stLevelElem.m_iPassFightPower = ::atoi(stXML.GetAttrib("pass_fight_power").c_str());
				stLevelElem.m_iBonusExp = ::atoi(stXML.GetAttrib("bonus_exp").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stLevelElem.m_iBonusExp >= 0, iLevelID)

				stLevelElem.m_iBonusTeamExp = ::atoi(stXML.GetAttrib("consume").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stLevelElem.m_iBonusTeamExp >= 0, iLevelID)

                stLevelElem.m_strBonusCardBag = stXML.GetAttrib("cardbag_id");
				stLevelElem.m_strPerfectCardBag = stXML.GetAttrib("perfect_bag_id");
                stLevelElem.m_strDropBag = stXML.GetAttrib("drop_bag");
                stLevelElem.m_iTimeLimit = ::atoi(stXML.GetAttrib("time_limit").c_str());

                if(!stLevelElem.m_strDropBag.empty())
                    CONFIG_ASSERT_EXCEPTION_EX(nullptr != CLogicConfig::GetLevelDropBagConfig().GetConfig(stLevelElem.m_strDropBag),iLevelID);

				if(stLevelElem.m_iLevelType == CLogicConfigDefine::ELT_HeartLevel)
                {
				    CONFIG_ASSERT_EXCEPTION(stLevelElem.m_iLevelTypePara > 0);
				    CONFIG_ASSERT_EXCEPTION(stLevelElem.m_iNeedHeartLevel >= 0);
                }

                if(stLevelElem.m_iSharedLevel > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(stLevelElem.m_iSharedLevel != iLevelID, iLevelID)
                    auto iterShared = m_stLevelConfig.find(stLevelElem.m_iSharedLevel);
                    CONFIG_ASSERT_EXCEPTION_EX(iterShared != m_stLevelConfig.end(), iLevelID)

                    // 将指向的那个副本的共享副本设置为他自己，以便统一处理
                    if(iterShared->second.m_iSharedLevel == 0) iterShared->second.m_iSharedLevel = iterShared->first;

                    CONFIG_ASSERT_EXCEPTION_EX(iterShared->second.m_iSharedLevel == iterShared->first, iLevelID)
                    CONFIG_ASSERT_EXCEPTION_EX(iterShared->second.m_iSharedTimes > 0, iLevelID)
                    CONFIG_ASSERT_EXCEPTION_EX(iterShared->second.m_iLevelType == stLevelElem.m_iLevelType, iLevelID)
                }

                if(stLevelElem.m_iTeamType == 0) stLevelElem.m_iTeamType = CLogicConfigDefine::ETT_MainTeam;
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumTeamType(stLevelElem.m_iTeamType), iLevelID)

                stLevelElem.m_stSweepItem.m_iItemType = atoi(stXML.GetAttrib("sweep_item_type").c_str());
                stLevelElem.m_stSweepItem.m_iCardID = atoi(stXML.GetAttrib("sweep_item_id").c_str());
                stLevelElem.m_stSweepItem.m_iNum = 1;
                if(stLevelElem.m_stSweepItem.m_iItemType > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stLevelElem.m_stSweepItem), iLevelID)
                }

				TLogicEvaluateElem stEvaluateElem;
				for(int i = 1; i <= CLogicConfigDefine::ESD_LevelStarEvaluateNum; ++i)
				{
                    std::stringstream stStream;
                    stStream << i;
                    if (stXML.GetAttrib("pingjia_type" + stStream.str()).empty()) break;

					stEvaluateElem.m_iType = atoi(stXML.GetAttrib("pingjia_type" + stStream.str()).c_str());
                    if(stEvaluateElem.m_iType <= 0) break;

					stEvaluateElem.m_iAmount = atoi(stXML.GetAttrib("pingjia_amount" + stStream.str()).c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumLevelEvaluateType(stEvaluateElem.m_iType), iLevelID)
					stLevelElem.m_stEvaluate.push_back(stEvaluateElem);
				}

                for (int i = 1; i < 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if (stXML.GetAttrib("first_bonus_item_type" + stStream.str()).empty()) break;

                    int iItemType = atoi(stXML.GetAttrib("first_bonus_item_type" + stStream.str()).c_str());
                    if(iItemType <= 0) break;

                    int iCardID = atoi(stXML.GetAttrib("first_bonus_id" + stStream.str()).c_str());
                    int iNum = atoi(stXML.GetAttrib("bonus_num" + stStream.str()).c_str());
                    stLevelElem.m_stFirstBonusVec.emplace_back(iItemType, iCardID, iNum);
                }

                for(int i = 1; i < CLogicConfigDefine::ELAT_Max; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;

                    int iNum = atoi(stXML.GetAttrib("achieve_rmb_num" + stStream.str()).c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(iNum >= 0, iLevelID)
                    stLevelElem.m_stAchieveBonus.push_back(iNum);
                }

				std::string strWeekDays = stXML.GetAttrib("weekdays");
				std::string strOpenDays = stXML.GetAttrib("opendays");
				std::string strOpenTimes = stXML.GetAttrib("opentimes");

				if (!strWeekDays.empty())
                {
                    CLogicConfig::ParseWeekDay(strWeekDays, stLevelElem.m_stLevelOpenTime.m_cWeekDays);
                }
				if (!strOpenDays.empty())
                {
                    CLogicConfig::ParseDay(strOpenDays, stLevelElem.m_stLevelOpenTime.m_iStartDayTimestamp, stLevelElem.m_stLevelOpenTime.m_iEndDayTimestamp);
                    stLevelElem.m_stLevelOpenTime.m_bHasOpenDate = true;
                }
				if (!strOpenTimes.empty())
                {
                    CLogicConfig::ParseHour(strOpenTimes, stLevelElem.m_stLevelOpenTime.m_iStartHour,
                                            stLevelElem.m_stLevelOpenTime.m_iStartMinute,
                                            stLevelElem.m_stLevelOpenTime.m_iStartSecond,
                                            stLevelElem.m_stLevelOpenTime.m_iEndHour,
                                            stLevelElem.m_stLevelOpenTime.m_iEndMinute,
                                            stLevelElem.m_stLevelOpenTime.m_iEndSecond);
                    stLevelElem.m_stLevelOpenTime.m_bHasOpenHour = true;
                }

				std::string strMapNameList = stXML.GetAttrib("config");
                auto stMapList = string_split(strMapNameList, "|");
                CLogicConfigBarrierMapParser stBarrierDetail(stLevelElem.m_stBarrierMapDetail);
                stBarrierDetail.ResetData();
                for(auto& strMapName : stMapList)
                {
                    auto stRet = stBarrierDetail.Load(strMapName, m_stGetXMLContentCallback("map/" + strMapName));
                    if (stRet.first == false)
                    {
                        strErrorInfo << "LOAD MAP CONFIG:" << strMapNameList << ".xml FAILD" << std::endl;
                        throw std::exception();
                    }
                }

                // 计算BOSS血量
                double dBossHpRatio = ::atof(stXML.GetAttrib("boss_hp_ratio").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(dBossHpRatio >= 0.0f, iLevelID)
                for (const auto& boss : stLevelElem.m_stBarrierMapDetail.m_stBossMap)
                {
                    const auto pBossConfig = CLogicConfig::GetBossConfig().GetBossConfig(boss.first);
                    if (pBossConfig)
                    {
                        uint64_t lhp = pBossConfig->m_iBaseHp * dBossHpRatio + boss.second.m_iHp;
                        stLevelElem.m_ulBossHp += (lhp * boss.second.m_iNum);
                    }
                }

//				stLevelElem.m_iMaxCoinNum = stLevelElem.m_stBarrierMapDetail.m_iCoinNum +
//					stLevelElem.m_stBarrierMapDetail.m_iBoxNum * CLogicConfig::GetGlobalConfig().m_iOpenBoxGetCoinNum +
//                    CLogicConfig::GetGlobalConfig().m_iSettlementBonusGold;
                //修改单场能够获取的金币上限
                stLevelElem.m_iMaxCoinNum = CLogicConfig::GetGlobalConfig().m_iSettlementBonusGold;

                CONFIG_ASSERT_EXCEPTION_EX(stLevelElem.m_iMaxCoinNum >= 0, iLevelID)

				m_stLevelConfig[iLevelID] = stLevelElem;

				auto stChapterIter = CLogicConfig::GetConfig()->CLogicConfig::GetChapterConfigParser()->m_stChapterMap.find(iChapterID);
				if (stChapterIter != CLogicConfig::GetConfig()->CLogicConfig::GetChapterConfigParser()->m_stChapterMap.end())
				{
					stChapterIter->second.m_stLevelIDList.insert(iLevelID);
				}
			}
			stXML.OutOfElem();
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicLevelElem*
CLogicConfigLevelParser::GetLevelConfigElem(int iLevelID) const
{
	auto stConfigLevelInfoIT = m_stLevelConfig.find(iLevelID);
	if (stConfigLevelInfoIT == m_stLevelConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(LEVEL_INFO_NOT_FOUND_IN_CONFIG);
		return nullptr;
	}
	else
	{
		return (&(stConfigLevelInfoIT->second));
	}
}

uint64_t CLogicConfigLevelParser::GetLevelBossHp(int32_t iLevelID) const
{
	auto pstLevelConfig = GetLevelConfigElem(iLevelID);
	if (pstLevelConfig)
	{
        return pstLevelConfig->m_ulBossHp;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigBossParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("boss_list"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		while (true == stXML.FindElem("boss"))
		{
			TLogicBossElem stElem;
			stElem.m_strBossID = stXML.GetAttrib("id");
			stElem.m_iBaseHp = ::atoi(stXML.GetAttrib("base_hp").c_str());
			m_stBossConfig[stElem.m_strBossID] = stElem;
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

const TLogicBossElem* CLogicConfigBossParser::GetBossConfig(const std::string strBossID) const
{
	const auto iter = m_stBossConfig.find(strBossID);
	return iter == m_stBossConfig.end() ? nullptr : &iter->second;
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigNoCoinMonsterParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("monster_list"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("monster"))
		{
			m_stNoCoinMonsterSet.insert(stXML.GetAttrib("id"));
		}

		while (stXML.FindElem("boss"))
		{
			m_stNoCoinBossSet.insert(stXML.GetAttrib("id"));
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string> CLogicConfigZhuHaiParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if (false == stXML.FindElem("root"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("level_reward"))
        stXML.IntoElem();
        while (stXML.FindElem("level"))
        {
            int iLevelID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetLevelConfig().GetLevelConfigElem(iLevelID), iLevelID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stLevelRewardMap.find(iLevelID) == m_stLevelRewardMap.end(), iLevelID)

            auto& stNumMap = m_stLevelRewardMap[iLevelID];

            stXML.IntoElem();
            while (stXML.FindElem("index"))
            {
                int iNum = ::atoi(stXML.GetAttrib("num").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iNum == int(stNumMap.size() + 1), iLevelID)

                TLogicZhuHaiRewardElem stElem;
                for(int i = 1; i <= 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                    CPackGameItem stItem;
                    stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                    stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                    stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), iLevelID)
                    stElem.m_stRewards.emplace_back(stItem);
                }

                for(int i = 1; i <= 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if(stXML.GetAttrib("bag" + stStream.str()).empty()) break;

                    std::string strBag = stXML.GetAttrib("bag" + stStream.str());
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(strBag), iLevelID)
                    stElem.m_stBagVec.push_back(strBag);
                }

                for(int i = 1; i <= 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if(stXML.GetAttrib("level_bag" + stStream.str()).empty()) break;

                    std::string strBag = stXML.GetAttrib("level_bag" + stStream.str());
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetLevelDropConfig().GetConfig(strBag), iLevelID)
                    stElem.m_stLevelDropVec.push_back(strBag);
                }

                stNumMap.push_back(stElem);
            }
            stXML.OutOfElem();
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("zhuhai_store"))
        stXML.IntoElem();
        while(stXML.FindElem("store"))
        {
            int32_t index = ::atoi(stXML.GetAttrib("index").c_str());
            stXML.IntoElem();
            while(stXML.FindElem("item"))
            {
                TLogicZhuHaiStoreElem storeElem;
                storeElem.m_iStoreID = ::atoi(stXML.GetAttrib("id").c_str());
                storeElem.m_iStoreType = ::atoi(stXML.GetAttrib("type").c_str());
                storeElem.m_iGetBuffID = ::atoi(stXML.GetAttrib("get_buff_id").c_str());

                for(int i = 1; i <= 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if(stXML.GetAttrib("consume_type" + stStream.str()).empty()) break;

                    CPackGameItem stItem;
                    stItem.m_iItemType = ::atoi(stXML.GetAttrib("consume_type" + stStream.str()).c_str());
                    stItem.m_iCardID = ::atoi(stXML.GetAttrib("consume_id" + stStream.str()).c_str());
                    stItem.m_iNum = ::atoi(stXML.GetAttrib("consume_num" + stStream.str()).c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), i)
                    storeElem.m_stCostItem.emplace_back(stItem);
                }
                m_stStoreMap[index][storeElem.m_iStoreID] = storeElem;
            }
            stXML.OutOfElem();
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("refresh_store"))
        stXML.IntoElem();
        while(stXML.FindElem("index"))
        {
            TLogicZhuHaiRefreshStoreElem elem;
            elem.m_iCount = ::atoi(stXML.GetAttrib("count").c_str());
            CONFIG_ASSERT_EXCEPTION(elem.m_iCount > 0)
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("consume_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("consume_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("consume_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("consume_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), i)
                elem.m_iConsumeItem.emplace_back(stItem);
            }
            m_stRefreshStoreMap[elem.m_iCount] = elem;
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

const ZhuHaiRewardVec* CLogicConfigZhuHaiParser::GetConfig(int iLevel) const
{
    auto iter = m_stLevelRewardMap.find(iLevel);
    return iter == m_stLevelRewardMap.end() ? nullptr : &(iter->second);
}

CPackItemVec CLogicConfigZhuHaiParser::GetRewards(const TLogicZhuHaiRewardElem& stRewardConfig)
{
    CPackItemVec stRewards(stRewardConfig.m_stRewards);
    for(auto& strBagName : stRewardConfig.m_stBagVec)
    {
        CLogicConfig::GetCardbagConfig().GetRandBonusByName(strBagName, stRewards);
    }

    for(auto& strBagName : stRewardConfig.m_stLevelDropVec)
    {
        CLogicConfig::GetLevelDropConfig().GetRandBonusByName(strBagName, stRewards);
    }

    return stRewards;
}

const TLogicZhuHaiStoreElem* CLogicConfigZhuHaiParser::GetStoreItem(int index,int storeID) const
{
    auto iter = m_stStoreMap.find(index);
    if (iter == m_stStoreMap.end())
    {
        return nullptr;
    }

    auto storeIter = iter->second.find(storeID);
    if (storeIter == iter->second.end())
    {
        return nullptr;
    }
    return &(storeIter->second);
}

const TLogicZhuHaiRefreshStoreElem* CLogicConfigZhuHaiParser::GetRefreshStore(int count) const
{
    auto iter = m_stRefreshStoreMap.find(count);
    return iter == m_stRefreshStoreMap.end() ? nullptr:&(iter->second);
}

////////////////////////////////////////////////////////////////////////////////////////////////
union rk_logic_key_gen_t
{
	rk_logic_key_gen_t() : m_ulKey(0) {}

	uint64_t                m_ulKey;

	struct key_group_t
	{
		int         m_iUin;
		int         m_iGroupID;
	} m_stKeyGroup;
};

std::pair<bool, std::string>
CLogicConfigPvpRobotParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("rankbot"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

        if (false == stXML.FindElem("basebots"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        m_stBaseRobot.m_strNick = stXML.GetAttrib("name");
        m_stBotsNameList = string_split(m_stBaseRobot.m_strNick, "|");
        m_stBaseRobot.m_strNick = "";
        if (m_stBotsNameList.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:BaseBots NameList is empty!" << std::endl;
            throw std::exception();
        }

        m_stBaseRobot.m_iLevel = atoi(stXML.GetAttrib("team_level").c_str());
        m_stBaseRobot.m_iTotalCombat = atoi(stXML.GetAttrib("fight_power").c_str());
        m_stBaseRobot.m_iAvatarID = atoi(stXML.GetAttrib("avatar_id").c_str());
        m_stBaseRobot.m_iAvatarBorder = atoi(stXML.GetAttrib("avatar_border").c_str());
        m_stBaseRobot.m_iPvpScore = atoi(stXML.GetAttrib("pvp_score").c_str());

        for (size_t i = 1; i <= CLogicConfigDefine::ESD_TeamMaxCardNum; ++i)
        {
            auto strIndex = std::to_string(i);
            if (stXML.GetAttrib("card_id" + strIndex).empty()) break;

            TLogicPvpCardInfo stCardInfo;
            stCardInfo.m_iCardID = atoi(stXML.GetAttrib("card_id" + strIndex).c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetHeroCardConfig().GetConfig(stCardInfo.m_iCardID), stCardInfo.m_iCardID)
            stCardInfo.m_iAtkPara = atoi(stXML.GetAttrib("atk_para" + strIndex).c_str());
            stCardInfo.m_iHpPara = atoi(stXML.GetAttrib("hp_para" + strIndex).c_str());
            stCardInfo.m_iDefPara = atoi(stXML.GetAttrib("def_para" + strIndex).c_str());
            stCardInfo.m_iDiffculty = atoi(stXML.GetAttrib("diffculty" + strIndex).c_str());
            stCardInfo.m_iSkillLevelIndex = atoi(stXML.GetAttrib("skillLevelIndex" + strIndex).c_str());
            m_stBaseRobot.m_stCardInfoList.push_back(stCardInfo);
        }

        CONFIG_ASSERT_EXCEPTION(!m_stBaseRobot.m_stCardInfoList.empty())

		int iInitUin = 1;
		while (stXML.FindElem("bots"))
		{
			TLogicPvpRobotElem stRobotElem;
			stRobotElem.m_strNick = stXML.GetAttrib("name");
			stRobotElem.m_iAvatarID = atoi(stXML.GetAttrib("avatar_id").c_str());
			stRobotElem.m_iAvatarBorder = atoi(stXML.GetAttrib("avatar_border").c_str());
            stRobotElem.m_iTotalCombat = atoi(stXML.GetAttrib("fight_power").c_str());
            stRobotElem.m_iLevel = atoi(stXML.GetAttrib("team_level").c_str());
            stRobotElem.m_iPvpScore = atoi(stXML.GetAttrib("pvp_score").c_str());

            for (size_t i = 1; i <= CLogicConfigDefine::ESD_TeamMaxCardNum; ++i)
			{
                auto strIndex = std::to_string(i);
                if (stXML.GetAttrib("card_id" + strIndex).empty()) break;

                TLogicPvpCardInfo stCardInfo;
                stCardInfo.m_iCardID = atoi(stXML.GetAttrib("card_id" + strIndex).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetHeroCardConfig().GetConfig(stCardInfo.m_iCardID), stCardInfo.m_iCardID)
                stCardInfo.m_iAtkPara = atoi(stXML.GetAttrib("atk_para" + strIndex).c_str());
                stCardInfo.m_iHpPara = atoi(stXML.GetAttrib("hp_para" + strIndex).c_str());
                stCardInfo.m_iDefPara = atoi(stXML.GetAttrib("def_para" + strIndex).c_str());
                stCardInfo.m_iDiffculty = atoi(stXML.GetAttrib("diffculty" + strIndex).c_str());
                stCardInfo.m_iSkillLevelIndex = atoi(stXML.GetAttrib("skillLevelIndex" + strIndex).c_str());
				stRobotElem.m_stCardInfoList.push_back(stCardInfo);
			}

            CONFIG_ASSERT_EXCEPTION_EX(!stRobotElem.m_stCardInfoList.empty(), iInitUin)

            //不允许出现相同等级相同积分的机器人
            rk_logic_key_gen_t stKeyGen;
            stKeyGen.m_stKeyGroup.m_iUin = iInitUin;
            stRobotElem.m_iUinKey = iInitUin++;
            m_stRobotList[stKeyGen.m_ulKey] = stRobotElem;
			m_stPvpRobotList[stRobotElem.m_iLevel][stRobotElem.m_iPvpScore] = stRobotElem;
		}

	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicPvpRobotElem* CLogicConfigPvpRobotParser::GetConfig(int iUin, int iGroupID) const
{
    rk_logic_key_gen_t stKeyGen;
    stKeyGen.m_stKeyGroup.m_iUin = iUin;
    stKeyGen.m_stKeyGroup.m_iGroupID = iGroupID;
    auto iter = m_stRobotList.find(stKeyGen.m_ulKey);
    return iter == m_stRobotList.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string>
CLogicConfigPvpParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("pvp_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (false == stXML.FindElem("main"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

        m_iWinLower =  atoi(stXML.GetAttrib("win_lower").c_str());
        m_iWinUpper =  atoi(stXML.GetAttrib("win_equal").c_str());
        m_iWinEqual =  atoi(stXML.GetAttrib("win_upper").c_str());
        m_iLose = atoi(stXML.GetAttrib("lose").c_str());
        m_iDefendWin =  atoi(stXML.GetAttrib("defend_win").c_str());
        m_iDefendLose =  atoi(stXML.GetAttrib("defend_lose").c_str());
        m_iConsumeItemID =  atoi(stXML.GetAttrib("consume_item_id").c_str());

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("weekly_reward"))
        stXML.IntoElem();
        while(stXML.FindElem("rank"))
        {
            TLogicPvpWeeklyRewardElem weeklyElem;
            weeklyElem.m_iIndex =  atoi(stXML.GetAttrib("number").c_str());

            weeklyElem.m_iMin = atoi(stXML.GetAttrib("min").c_str());
            weeklyElem.m_iMax = atoi(stXML.GetAttrib("max").c_str());

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, weeklyElem.m_iIndex);
                weeklyElem.m_stWeeklyReward.emplace_back(stItem);
            }

            weeklyElem.m_strMailSender = stXML.GetAttrib("mail_sender");
            weeklyElem.m_strMailTitle = stXML.GetAttrib("mail_title");
            weeklyElem.m_strMailContent = stXML.GetAttrib("mail_content");

            m_stWeeklyReward.insert(std::make_pair(weeklyElem.m_iIndex,weeklyElem));
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("refresh_reward"))
        stXML.IntoElem();
        while(stXML.FindElem("rank"))
        {
            TLogicPvpRefreshRewardElem refreshElem;
            refreshElem.m_iWinCount =  atoi(stXML.GetAttrib("win_count").c_str());

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, refreshElem.m_iWinCount);
                refreshElem.m_stRefreshReward.emplace_back(stItem);
            }
            m_stRefreshReward.emplace_back(refreshElem);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("pvp_reward"))
        stXML.IntoElem();
        while(stXML.FindElem("reward"))
        {
            TLogicPvpRewardElem stPvpElem;
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackRandomGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                stItem.m_iWeight = ::atoi(stXML.GetAttrib("reward_weight" + stStream.str()).c_str());
                stItem.m_iRandom = ::atoi(stXML.GetAttrib("reward_random" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iCardID);
                stPvpElem.m_stLogicReward.emplace_back(stItem);
            }
            m_stPvpReward.emplace_back(stPvpElem);
        }
        CONFIG_ASSERT_EXCEPTION(m_stPvpReward.size() == 4);

        stXML.OutOfElem();

        stXML.OutOfElem();
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}



std::pair<bool, std::string>
CLogicConfigPvpMatchRuleParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("arena_match_rules"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("levelRules"))
        stXML.IntoElem();

		while (stXML.FindElem("levelRange"))
		{
            TLogicPvpMatchLevelRuleElem elem;

            elem.m_iMinLevel = atoi(stXML.GetAttrib("min").c_str());
            elem.m_iMaxLevel = atoi(stXML.GetAttrib("max").c_str());
            elem.m_iLowerNumberMin = atoi(stXML.GetAttrib("lower_number_min").c_str());
            elem.m_iLowerNumberMax = atoi(stXML.GetAttrib("lower_number_max").c_str());
            elem.m_iLowerPointLimit = atoi(stXML.GetAttrib("lower_point_limit").c_str());
            elem.m_iUpperPointLimit = atoi(stXML.GetAttrib("upper_point_limit").c_str());

            CONFIG_ASSERT_EXCEPTION_EX(elem.m_iMinLevel <= elem.m_iMaxLevel, elem.m_iMinLevel)
            CONFIG_ASSERT_EXCEPTION_EX(elem.m_iLowerNumberMin <= elem.m_iLowerNumberMax, elem.m_iMinLevel)

            m_stLevelMatchRules.insert(std::make_pair(elem.m_iMinLevel,elem));
		}
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("pointRules"))
        stXML.IntoElem();
        while (stXML.FindElem("pointRange"))
        {
            TLogicPvpMatchPointRuleElem elem;

            elem.m_iMinPoint = atoi(stXML.GetAttrib("min").c_str());
            elem.m_iMaxPoint = atoi(stXML.GetAttrib("max").c_str());
            elem.m_iRobotPro = atoi(stXML.GetAttrib("robotPro").c_str());

            CONFIG_ASSERT_EXCEPTION_EX(elem.m_iMinPoint <= elem.m_iMaxPoint, elem.m_iMinPoint)
            m_stPointMatchRules.insert(std::make_pair(elem.m_iMinPoint,elem));
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("refreshRules"))
        stXML.IntoElem();
        while (stXML.FindElem("refresh"))
        {
            TLogicPvpMatchRefreshRuleElem elem;

            elem.m_iRefreshCount = atoi(stXML.GetAttrib("count").c_str());
            elem.m_iRobotPro = atoi(stXML.GetAttrib("robotPro").c_str());

            m_stRefreshMatchRules.insert(std::make_pair(elem.m_iRefreshCount,elem));
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



const TLogicPvpMatchLevelRuleElem* CLogicConfigPvpMatchRuleParser::FindMatchRuleByLevel(int32_t level) const
{
    if(level <= 0) return nullptr;
    auto iterLevel = m_stLevelMatchRules.begin();
    for (; iterLevel != m_stLevelMatchRules.end(); iterLevel++)
    {
        if (level <= iterLevel->second.m_iMaxLevel) break ;
    }
    return iterLevel == m_stLevelMatchRules.end() ? nullptr : &(iterLevel->second);
}

const TLogicPvpMatchPointRuleElem* CLogicConfigPvpMatchRuleParser::FindMatchRuleByPoint(int32_t point) const
{
    if(point <= 0) return nullptr;
    auto iterPoint = m_stPointMatchRules.begin();
    for (; iterPoint != m_stPointMatchRules.end(); iterPoint++)
    {
        if (point <= iterPoint->second.m_iMaxPoint) break ;
    }
    return iterPoint == m_stPointMatchRules.end() ? nullptr : &(iterPoint->second);
}

const TLogicPvpMatchRefreshRuleElem* CLogicConfigPvpMatchRuleParser::FindMatchRuleByRefreshCount(int32_t refreshCount) const
{
    if(refreshCount <= 0) return nullptr;
    auto iter = m_stRefreshMatchRules.lower_bound(refreshCount);
    return iter == m_stRefreshMatchRules.end() ? nullptr : &(iter->second);
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigCrossFightParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("cross_fight"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (stXML.FindElem("cross_fight_info"))
		{
			m_stConfig.m_iPlayTimesPerDay = ::atoi(stXML.GetAttrib("play_times_per_day").c_str());
			m_stConfig.m_iAutoFightLevel = ::atoi(stXML.GetAttrib("auto_fight_level").c_str());
			m_stConfig.m_iAutoFightTeamCount = ::atoi(stXML.GetAttrib("auto_fight_team_count").c_str());
		}

		if (stXML.FindElem("cross_fight_cost"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("cost"))
			{
				int32_t icount = ::atoi(stXML.GetAttrib("count").c_str());
				int32_t icost = ::atoi(stXML.GetAttrib("cost").c_str());
				if (icount <= 0 || icost <= 0)
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << " cross_fight_cost one of [count,cost] <= 0" << std::endl;
					throw std::exception();
				}

				m_stConfig.m_aiRefreshResetCost[icount] = icost;
			}

			stXML.OutOfElem();
		}

        int iPVPLevelNum = 0;
		if (stXML.FindElem("cross_fight_award"))
		{
			stXML.IntoElem();
			TLogicCrossFightBonusElem stElem;
			while (stXML.FindElem("award"))
			{
				int iTeamID = ::atoi(stXML.GetAttrib("order").c_str());
                stElem.m_iType = ::atoi(stXML.GetAttrib("type").c_str());
                CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumCrossFightLevelType(stElem.m_iType));

                if(stElem.m_iType == CLogicConfigDefine::ECFLT_PVP)
                    ++iPVPLevelNum;

				stElem.m_strCardBag = stXML.GetAttrib("bonus_cardbag");
                stElem.m_strBigPrizeBag = stXML.GetAttrib("big_prize_bag");
				stElem.m_strFirstCardBag = stXML.GetAttrib("first_bonus_cardbag");
				stElem.m_stGameItem.m_iItemType = ::atoi(stXML.GetAttrib("bonus_item_type").c_str());
				stElem.m_stGameItem.m_iCardID = ::atoi(stXML.GetAttrib("bonus_item_id").c_str());
				stElem.m_stGameItem.m_iNum = ::atoi(stXML.GetAttrib("bonus_item_num").c_str());

				m_stConfig.m_stBonusMap[iTeamID] = stElem;
			}
			stXML.OutOfElem();
		}

		if (stXML.FindElem("cross_fight_match"))
		{
			stXML.IntoElem();
			while (stXML.FindElem("range"))
			{
				int iRange = ::atoi(stXML.GetAttrib("tag").c_str());
				std::vector<TLogicCrossFightMatchElem> stMatchElem;
				stXML.IntoElem();
				int iIndex = 0;
				while (stXML.FindElem("level"))
				{
					TLogicCrossFightMatchElem stElem;

					stElem.m_iMaxRank = ::atoi(stXML.GetAttrib("max_rank").c_str());
					stElem.m_iMinRank = ::atoi(stXML.GetAttrib("min_rank").c_str());

					iIndex++;
					if (stElem.m_iMaxRank <= 0 ||
						stElem.m_iMinRank <= 0 ||
						stElem.m_iMinRank > stElem.m_iMaxRank)
					{
						strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:INVALID RANK RANGE|Range:"
							<< iRange << "|Index:" << iIndex << std::endl;

						throw std::exception();
					}

					stMatchElem.push_back(stElem);
				}
				stXML.OutOfElem();

                CONFIG_ASSERT_EXCEPTION(iIndex >= iPVPLevelNum);

				m_stConfig.m_stMatchMap[iRange] = stMatchElem;
			}
			stXML.OutOfElem();
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

std::vector<int>
CLogicConfigCrossFightParser::GetMatchRank(int iRank) const
{
	std::vector<int> stRank;
	iRank = iRank > 0 ? iRank : 1;

	auto stIter = m_stConfig.m_stMatchMap.lower_bound(iRank);
	const auto& stMatchRule = stIter->second;

	int iRandRank;
	for (const auto& stRole : stMatchRule)
	{
		iRandRank = (CLogicCommon::GetRandNum() % (stRole.m_iMaxRank - stRole.m_iMinRank)) + stRole.m_iMinRank;
		if (iRandRank == iRank)
			iRandRank = ((iRandRank - 1) < stRole.m_iMinRank) ? (iRandRank + 1) : (iRandRank - 1);

		stRank.push_back(iRandRank);
	}

	return stRank;
}

bool
CLogicConfigCrossFightParser::GetFightBonus(int iPosition, bool bFirst, bool bBigPrize, std::vector<CPackGameItem>& stBonusVec) const
{
	auto stIter = m_stConfig.m_stBonusMap.find(iPosition);
	if (stIter == m_stConfig.m_stBonusMap.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(CROSS_FIGHT_BONUS_NOT_CONFIG);
		return false;
	}

    if(stIter->second.m_stGameItem.m_iItemType > 0 && stIter->second.m_stGameItem.m_iNum > 0)
        stBonusVec.push_back(stIter->second.m_stGameItem);

    auto strCardBag = stIter->second.m_strCardBag;
    if(bFirst) strCardBag = stIter->second.m_strFirstCardBag;
    else if(bBigPrize) strCardBag = stIter->second.m_strBigPrizeBag;

	CLogicConfig::GetCardbagConfig().GetRandBonusByName(strCardBag, stBonusVec);

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigClimbTowerParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("root"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (false == stXML.FindElem("towerinfo"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		m_iSweepCostYuanbao = atoi(stXML.GetAttrib("cost").c_str());
		m_iSweepCostTime = atoi(stXML.GetAttrib("time").c_str());

		if (m_iSweepCostTime <= 0 || m_iSweepCostYuanbao <= 0)
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "COST OR TIME NOT MORE THAN ZERO" << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("tower_list"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("tower"))
		{
			TLogicClimbTowerOrderElem stConfig;
			stConfig.m_iOrder = atoi(stXML.GetAttrib("order").c_str());
			stConfig.m_iMapID = atoi(stXML.GetAttrib("mapid").c_str());
			stConfig.m_iPVP = atoi(stXML.GetAttrib("pvp").c_str());

			m_stConfig[stConfig.m_iOrder] = stConfig;
		}

		stXML.OutOfElem();

		if (false == stXML.FindElem("tower_refresh"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("refresh"))
		{
			int32_t count = atoi(stXML.GetAttrib("count").c_str());
			int32_t cost = atoi(stXML.GetAttrib("cost").c_str());

			m_iResetConst[count] = cost;
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicClimbTowerOrderElem* CLogicConfigClimbTowerParser::GetConfigElem(int order)const
{
	auto iter = m_stConfig.find(order);
	if (iter == m_stConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(FIGHT_CLIMB_TOWER_ORDER_NOT_CONFIG);
		return nullptr;
	}
	return &(iter->second);
}

const TLogicClimbTowerOrderElem* CLogicConfigClimbTowerParser::GetConfigElemByLevel(int level)const
{
	for (const auto& iter : m_stConfig)
	{
		if (iter.second.m_iMapID == level)
		{
			return &(iter.second);
		}
	}
	using namespace SERVER_ERRCODE;
	SET_ERR_INFO(FIGHT_CLIMB_TOWER_ORDER_NOT_CONFIG);
	return nullptr;
}

int32_t CLogicConfigClimbTowerParser::GetResetCost(int32_t count)const
{
	auto iter = m_iResetConst.find(count);
	if (iter == m_iResetConst.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(NOT_ENOUGH_RMB);
		return 99999999;
	}
	return iter->second;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string> CLogicConfigConstelLevelDropParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if (false == stXML.FindElem("root"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("level_drop"))
        stXML.IntoElem();
        while (stXML.FindElem("drop"))
        {
            int32_t iLevelID = ::atoi(stXML.GetAttrib("level_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetLevelConfig().GetLevelConfigElem(iLevelID), iLevelID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stConstelDropMap.find(iLevelID) == m_stConstelDropMap.end(), iLevelID)

            TLogicConstelLevelDrop stElem;
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("constel_num" + stStream.str()).empty()) break;

                int32_t iNum = ::atoi(stXML.GetAttrib("constel_num" + stStream.str()).c_str());
                int32_t iWeight = ::atoi(stXML.GetAttrib("weight" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iNum > 0 && iWeight > 0, iLevelID)

                stElem.m_iTotalWeight += iWeight;
                stElem.m_stConstelNumRand.emplace_back(iNum, iWeight);
            }
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iTotalWeight > 0, iLevelID)

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("constel_num_extra" + stStream.str()).empty()) break;

                int32_t iNum = ::atoi(stXML.GetAttrib("constel_num_extra" + stStream.str()).c_str());
                int32_t iWeight = ::atoi(stXML.GetAttrib("extra_weight" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iNum > 0 && iWeight > 0, iLevelID)

                stElem.m_iExtraTotalWeight += iWeight;
                stElem.m_stConstelExtraNumRand.emplace_back(iNum, iWeight);
            }
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iExtraTotalWeight > 0, iLevelID)


            stElem.m_strCardBag = stXML.GetAttrib("cardbag");
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(stElem.m_strCardBag), iLevelID)

            m_stConstelDropMap[iLevelID] = std::move(stElem);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("equip_drop"))
        stXML.IntoElem();
        while (stXML.FindElem("drop"))
        {
            int32_t iLevelID = ::atoi(stXML.GetAttrib("level_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetLevelConfig().GetLevelConfigElem(iLevelID), iLevelID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stConstelDropMap.find(iLevelID) == m_stConstelDropMap.end(), iLevelID)

            TLogicEquipLevelDrop stElem;
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("cardbag" + stStream.str()).empty()) break;

                std::string strBagName = stXML.GetAttrib("cardbag" + stStream.str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(strBagName), iLevelID)
                stElem.m_stCardBagVec.push_back(strBagName);
            }

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("cardbag_vip" + stStream.str()).empty()) break;

                std::string strBagName = stXML.GetAttrib("cardbag_vip" + stStream.str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(strBagName), iLevelID)
                stElem.m_stAdvCardBagVec.push_back(strBagName);
            }

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("cardbag_extra" + stStream.str()).empty()) break;

                std::string strBagName = stXML.GetAttrib("cardbag_extra" + stStream.str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(strBagName), iLevelID)
                stElem.m_stExtraCardBagVec.push_back(strBagName);
            }

            m_stEquipDropMap[iLevelID] = std::move(stElem);
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

const TLogicConstelLevelDrop* CLogicConfigConstelLevelDropParser::GetConstelDropConfig(int32_t iLevelID) const
{
    auto iter = m_stConstelDropMap.find(iLevelID);
    return iter == m_stConstelDropMap.end() ? nullptr : &(iter->second);
}

const TLogicEquipLevelDrop* CLogicConfigConstelLevelDropParser::GetEquipDropConfig(int32_t iLevelID) const
{
    auto iter = m_stEquipDropMap.find(iLevelID);
    return iter == m_stEquipDropMap.end() ? nullptr : &(iter->second);
}


const TLogicPvpWeeklyRewardElem* CLogicConfigPvpParser::GetPVPWeeklyRewardElemByRank(int32_t rank) const
{
    auto iter = m_stWeeklyReward.find(rank);
    return iter == m_stWeeklyReward.end() ? nullptr : &(iter->second);
}

//const TLogicPvpRefreshRewardElem* CLogicConfigPvpParser::GetPVPRefreshRewardElemByWinCount(int32_t count) const
//{
//    auto iter = m_stRefreshReward.find(count);
//    return iter == m_stRefreshReward.end() ? nullptr : &(iter->second);
//}