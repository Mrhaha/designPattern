#include "logic_config_ibusy.h"
#include "Markup.h"
#include "common_datetime.h"
#include "logic_config.h"
#include "logic_link_common.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigIbusyParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("ibusy_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		/////////////////////////////////////////////////////////////////////////////////
		while (stXML.FindElem("activity"))
		{
			TLogicIbusyActivityInfo activityInfo;
			activityInfo.m_iId = atoi(stXML.GetAttrib("id").c_str());

			stXML.IntoElem();

			
			/////////////////////////
			if (false == stXML.FindElem("info"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			if (!try_time_from_string(stXML.GetAttrib("begin_time"), activityInfo.m_iBeginTime))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "begin_time format error" << std::endl;
				throw std::exception();
			}
			if (!try_time_from_string(stXML.GetAttrib("end_time"), activityInfo.m_iEndTime))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "end_time format error" << std::endl;
				throw std::exception();
			}
			activityInfo.m_iDepositID = atoi(stXML.GetAttrib("deposit_id").c_str());
            activityInfo.m_iDepositExp = atoi(stXML.GetAttrib("deposit_exp").c_str());
			activityInfo.m_iDayMaxHuoyue = atoi(stXML.GetAttrib("day_max_huoyue").c_str());
			activityInfo.m_strSender = stXML.GetAttrib("m_strSender");
			activityInfo.m_strSendTitle = stXML.GetAttrib("m_strSendTitle");
			activityInfo.m_strSendContent = stXML.GetAttrib("m_strSendContent");
			
			/////////////////////////
			if (false == stXML.FindElem("level_exp"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}			
			stXML.IntoElem();
			int tmpKey, tmpValue;
			while (stXML.FindElem("level"))
			{
				tmpKey = atoi(stXML.GetAttrib("id").c_str());
				tmpValue = atoi(stXML.GetAttrib("exp").c_str());
				activityInfo.m_mpLevelExp.insert(std::make_pair(tmpKey, tmpValue));
			}
			stXML.OutOfElem();
	
			/////////////////////////
			if (false == stXML.FindElem("event_pool"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			stXML.IntoElem();
			while (stXML.FindElem("event"))
			{
				TLogicIbusyEventElem elem;
				elem.m_iId = atoi(stXML.GetAttrib("id").c_str());
				elem.m_iHeroNum = atoi(stXML.GetAttrib("hero_num").c_str());
				elem.m_iPinzhiMin = atoi(stXML.GetAttrib("pinzhi_min").c_str());
				elem.m_iPinzhiMax = atoi(stXML.GetAttrib("pinzhi_max").c_str());
				
				for (int i = 1; i < 10; i++)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("need" + stStream.str()).empty()) break;

					elem.m_vecNeeds.push_back(atoi(stXML.GetAttrib("need" + stStream.str()).c_str()));
				}
					
				for (int i = 1; i < 10; i++)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("charactor" + stStream.str()).empty()) break;

					elem.m_vecCharactors.push_back(atoi(stXML.GetAttrib("charactor" + stStream.str()).c_str()));
				}

				activityInfo.m_vecEvents.push_back(elem);
			}
			stXML.OutOfElem();

			/////////////////////////
			if (false == stXML.FindElem("pinzhi_pool"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			stXML.IntoElem();
			activityInfo.m_iTotalPinzhiCoef = 0;
			while (stXML.FindElem("pinzhi"))
			{
				TLogicIbusyPinzhiElem elem;
				elem.m_iId = atoi(stXML.GetAttrib("id").c_str());
				elem.m_iScore = atoi(stXML.GetAttrib("score").c_str());
				elem.m_iExp = atoi(stXML.GetAttrib("exp").c_str());
				elem.m_iCoef = atoi(stXML.GetAttrib("coef").c_str());
				elem.m_iHuoyue = atoi(stXML.GetAttrib("huoyue").c_str());
				elem.m_stReward.m_iCardID = atoi(stXML.GetAttrib("item_id").c_str());
				elem.m_stReward.m_iItemType = atoi(stXML.GetAttrib("item_type").c_str());
				elem.m_stReward.m_iNum = atoi(stXML.GetAttrib("item_num").c_str());

				activityInfo.m_vecPinzhis.push_back(elem);
				activityInfo.m_iTotalPinzhiCoef += elem.m_iCoef;
			}
			stXML.OutOfElem();

			/////////////////////////
			if (false == stXML.FindElem("change_cost"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			stXML.IntoElem();
			while (stXML.FindElem("change"))
			{
				tmpKey = atoi(stXML.GetAttrib("id").c_str());
				tmpValue = atoi(stXML.GetAttrib("cost").c_str());
				activityInfo.m_mpChangeCost.insert(std::make_pair(tmpKey, tmpValue));
			}
			stXML.OutOfElem();

			/////////////////////////
			if (false == stXML.FindElem("charactor_add"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			stXML.IntoElem();
			while (stXML.FindElem("charactor"))
			{
				tmpKey = atoi(stXML.GetAttrib("id").c_str());
				tmpValue = atoi(stXML.GetAttrib("add_percent").c_str());
				activityInfo.m_mpCharactorAdd.insert(std::make_pair(tmpKey, tmpValue));
			}
			stXML.OutOfElem();

			/////////////////////////
			if (false == stXML.FindElem("level_add"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			stXML.IntoElem();
			while (stXML.FindElem("level"))
			{
				tmpKey = atoi(stXML.GetAttrib("id").c_str());
				tmpValue = atoi(stXML.GetAttrib("add_percent").c_str());
				activityInfo.m_mpLevelAdd.insert(std::make_pair(tmpKey, tmpValue));
			}
			stXML.OutOfElem();

			/////////////////////////
			if (false == stXML.FindElem("level_reward"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			stXML.IntoElem();
			while (stXML.FindElem("level"))
			{
				TLogicIbusyLevelRewardElem elem;
				CPackGameItem item;

				elem.m_iLevel = atoi(stXML.GetAttrib("level").c_str());

				for (int i = 1; i < 10; i++)
				{
					std::stringstream stStream;
					stStream << "_" << i;

					if (!stXML.GetAttrib("bonus_item_id" + stStream.str()).empty())
                    {
                        item.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
                        item.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
                        item.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());
                        elem.m_vecRewards.push_back(item);
                    }

                    if (!stXML.GetAttrib("deposit_item_id" + stStream.str()).empty())
                    {
                        item.m_iCardID = atoi(stXML.GetAttrib("deposit_item_id" + stStream.str()).c_str());
                        item.m_iItemType = atoi(stXML.GetAttrib("deposit_item_type" + stStream.str()).c_str());
                        item.m_iNum = atoi(stXML.GetAttrib("deposit_item_num" + stStream.str()).c_str());
                        elem.m_stDepositRewards.push_back(item);
                    }
				}	

				activityInfo.m_mpLevelReward.insert(std::make_pair(elem.m_iLevel, elem));
			}
			stXML.OutOfElem();

			/////////////////////////
			if (false == stXML.FindElem("score_rank_reward"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			stXML.IntoElem();
			while (stXML.FindElem("rank"))
			{
				TLogicIbusyRankRewardElem elem;
				CPackGameItem item;

				elem.m_iId = atoi(stXML.GetAttrib("id").c_str());
				elem.m_iBeginLevel = atoi(stXML.GetAttrib("begin").c_str());
				elem.m_iEndLevel = atoi(stXML.GetAttrib("finish").c_str());

				for (int i = 1; i < 10; i++)
				{
					std::stringstream stStream;
					stStream << "_" << i;
					if (stXML.GetAttrib("bonus_item_id" + stStream.str()).empty()) break;

					item.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
					item.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
					item.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());
					elem.m_vecRewards.push_back(item);
				}

				activityInfo.m_vecRankReward.push_back(elem);
			}
			stXML.OutOfElem();
			
			/////////////////////////
			if (false == stXML.FindElem("blackmarket_datas"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND blackmarket_datas" << std::endl;
				throw std::exception();
			}
			stXML.IntoElem();

			if (false == stXML.FindElem("blackmarket_desc"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND blackmarket_desc" << std::endl;
				throw std::exception();
			}
			for (int i = 1; i < 10; i++)
			{
				std::stringstream stStream;
				stStream << "_" << i;
				if (stXML.GetAttrib("refresh_time" + stStream.str()).empty()) break;

				activityInfo.m_stMarketData.m_stFlashTime.push_back(atoi(stXML.GetAttrib("refresh_time" + stStream.str()).c_str()));
			}			

			if (false == stXML.FindElem("blackmarket_cost"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND blackmarket_cost" << std::endl;
				throw std::exception();
			}

			stXML.IntoElem();

			activityInfo.m_stMarketData.m_stRefreshConsume.push_back(CPackGameItem());
			while (stXML.FindElem("cost"))
			{
				CPackGameItem stElem;
				stElem.m_iItemType = atoi(stXML.GetAttrib("restore_item_currency_type").c_str());
				stElem.m_iCardID = atoi(stXML.GetAttrib("restore_item_cost_id").c_str());
				stElem.m_iNum = atoi(stXML.GetAttrib("restore_item_cost_amount").c_str());
				activityInfo.m_stMarketData.m_stRefreshConsume.push_back(stElem);
			}

			stXML.OutOfElem();

			while (stXML.FindElem("blackmarket_data"))
			{
				int m_iLevel = atoi(stXML.GetAttrib("blackmarket_lv").c_str());

				stXML.IntoElem();
				while (stXML.FindElem("item"))
				{
					LiLogicCardbagRateVector stElem;
					for (int i = 1; i < 100; i++)
					{
						TLogicCardbagRateElem stCardBagRateElem;

						std::stringstream stStream;
						stStream << "_" << i;
						if (stXML.GetAttrib("bag" + stStream.str()).empty()) break;

						stCardBagRateElem.m_iRate = atoi(stXML.GetAttrib("bag_rate" + stStream.str()).c_str());
						stCardBagRateElem.m_strCardBag = stXML.GetAttrib("bag" + stStream.str());

						auto stIter = CLogicConfig::GetMallBagConfig().m_stCardBagInfo.find(stCardBagRateElem.m_strCardBag);
						if (stIter == CLogicConfig::GetMallBagConfig().m_stCardBagInfo.end())
						{
							strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND CARDBAG " << stCardBagRateElem.m_strCardBag
								<< " IN mallbag.xml" << std::endl;
							throw std::exception();
						}

						stElem.push_back(stCardBagRateElem);
					}
					activityInfo.m_stMarketData.m_stCardBagInfo[m_iLevel].push_back(stElem);
				}
				stXML.OutOfElem();
			}
			stXML.OutOfElem();
			/////////////////////////
			if (false == stXML.FindElem("task_huoyue"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}
			stXML.IntoElem();
			int tmpKey2;
			while (stXML.FindElem("huoyue"))
			{
				tmpKey = atoi(stXML.GetAttrib("type").c_str());
				tmpKey2 = atoi(stXML.GetAttrib("target").c_str());
				tmpValue = atoi(stXML.GetAttrib("huoyue").c_str());

				activityInfo.m_mpTaskHuoyue.insert(std::make_pair(std::make_tuple(tmpKey, tmpKey2), tmpValue));
			}
			stXML.OutOfElem();

			/////////////////////////

			stXML.OutOfElem();

			m_mpActivities[activityInfo.m_iId] = activityInfo;
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

std::vector<TLogicCardbagInfoElem>
TLogicIbusyMarketData::GetCommodityElem(int iLevel) const
{
	std::vector<TLogicCardbagInfoElem> stElem;

	auto stIter = m_stCardBagInfo.lower_bound(iLevel);
	if (stIter == m_stCardBagInfo.end()) return stElem;

	for (size_t i = 0; i < stIter->second.size(); ++i)
	{
		int iPos = 0;
		int iRandWeight = CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_RandomWeight;
		for (size_t j = 0; j < stIter->second[i].size(); ++j)
		{
			iPos += stIter->second[i][j].m_iRate;
			if (iPos >= iRandWeight)
			{
				stElem.push_back(CLogicConfig::GetMallBagConfig().GetCommodityElem(stIter->second[i][j].m_strCardBag));
				break;
			}
		}
	}

	return stElem;

}

std::pair<bool, std::string> CLogicConfigBuffParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("buff_effect"))
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iIndex = ::atoi(stXML.GetAttrib("effect_index").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(m_stBuffEffectMap.find(iIndex) == m_stBuffEffectMap.end(), iIndex)

            TLogicBuffEffectElem stElem;
            stElem.m_iMutexType = ::atoi(stXML.GetAttrib("mutex_type").c_str());
            stElem.m_iBuffType = ::atoi(stXML.GetAttrib("buff_type").c_str());
            stElem.m_iBuffPara = ::atoi(stXML.GetAttrib("buff_para1").c_str());
            stElem.m_iBuffParaEx = ::atoi(stXML.GetAttrib("buff_para2").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(IsValidMutexType(stElem.m_iMutexType), iIndex)
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumBuffType(stElem.m_iBuffType), iIndex)
            switch (stElem.m_iBuffType)
            {
            case CLogicConfigDefine::EBT_MonthCard:
                m_iMonthCardIndex = iIndex;
                break;
            case CLogicConfigDefine::EBT_AddAttr:
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardAttrType(stElem.m_iBuffPara), iIndex)
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidCardAttrValue(stElem.m_iBuffParaEx), iIndex)
                break;
            case CLogicConfigDefine::EBT_AddLevelTypeExplodeRate:
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumLevelType(stElem.m_iBuffPara), iIndex)
                break;
            case CLogicConfigDefine::EBT_FatMonthCard:
                m_iFatMonthCardIndex = iIndex;
                break;
            }

            // 互斥列表
            std::string strIndexList = stXML.GetAttrib("mutex_index");
            auto stIndexVec = string_split(strIndexList, "|");
            for(auto& strIndex : stIndexVec)
            {
                stElem.m_stMutexVec.insert(atoi(strIndex.c_str()));
            }
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_stMutexVec.find(iIndex) == stElem.m_stMutexVec.end(), iIndex)

            // 互相替代列表
            strIndexList = stXML.GetAttrib("replace_index");
            stIndexVec = string_split(strIndexList, "|");
            for(auto& strIndex : stIndexVec)
            {
                stElem.m_stReplaceVec.insert(atoi(strIndex.c_str()));
            }
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_stReplaceVec.find(iIndex) == stElem.m_stReplaceVec.end(), iIndex)

            m_stBuffEffectMap[iIndex] = std::move(stElem);
        }
        stXML.OutOfElem();


        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_buff_effect"))

        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iID = atoi(stXML.GetAttrib("buff_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(iID > 0, iID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stBuffConfig.find(iID) == m_stBuffConfig.end(), iID)

            TLogicGuildBuffElem stElem;
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("buff_para" + stStream.str()).empty()) break;
                int32_t iBuffParam = ::atoi(stXML.GetAttrib("buff_para" + stStream.str()).c_str());
                stElem.m_iBuffParam.push_back(iBuffParam);
            }
            stElem.m_iBuffID = iID;
            m_stGuildBuffConfig[iID] = stElem;
        }
        stXML.OutOfElem();

        // 检查月卡索引
        auto iter = m_stBuffEffectMap.find(m_iMonthCardIndex);
        CONFIG_ASSERT_EXCEPTION(iter != m_stBuffEffectMap.end())
        CONFIG_ASSERT_EXCEPTION(iter->second.m_iBuffType == CLogicConfigDefine::EBT_MonthCard)

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("buff_list"))

        stXML.IntoElem();
        while (stXML.FindElem("buff"))
        {
            int32_t iID = atoi(stXML.GetAttrib("buff_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(iID > 0, iID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stBuffConfig.find(iID) == m_stBuffConfig.end(), iID)

            TLogicBuffElem stElem;
            stElem.m_iBuffEffectIndex = ::atoi(stXML.GetAttrib("effect_index").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(m_stBuffEffectMap.find(stElem.m_iBuffEffectIndex) != m_stBuffEffectMap.end(), iID)

            stElem.m_iDurTime = ::atoi(stXML.GetAttrib("dur_time").c_str());
            stElem.m_iDurDays = ::atoi(stXML.GetAttrib("dur_days").c_str());
            stElem.m_iEffectTimes = ::atoi(stXML.GetAttrib("effect_times").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iDurDays >= 0 && stElem.m_iDurTime >= 0, iID)
            CONFIG_ASSERT_EXCEPTION_EX(!(stElem.m_iDurDays > 0 && stElem.m_iDurTime > 0), iID)
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iEffectTimes >= 0, iID)

            m_stBuffConfig[iID] = stElem;
        }
        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicBuffElem* CLogicConfigBuffParser::GetConfig(int iBuffID) const
{
    auto iter = m_stBuffConfig.find(iBuffID);
    return iter == m_stBuffConfig.end() ? nullptr : &(iter->second);
}

const TLogicBuffEffectElem* CLogicConfigBuffParser::GetEffectConfig(int iIndex) const
{
    auto iter = m_stBuffEffectMap.find(iIndex);
    return iter == m_stBuffEffectMap.end() ? nullptr : &(iter->second);
}
