#include "logic_config_game_item.h"
#include "Markup.h"
#include "common_datetime.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "server_errcode.h"
#include "logic_config.h"

std::pair<bool, std::string> CLogicConfigItemParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("items"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
		}

		stXML.IntoElem();
		while (stXML.FindElem("item"))
		{
            TLogicItemConfig stElem;
			stElem.m_iItemID = atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidExchangeItemID(stElem.m_iItemID), stElem.m_iItemID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stConfigMap.find(stElem.m_iItemID) == m_stConfigMap.end(), stElem.m_iItemID)

			stElem.m_strName = stXML.GetAttrib("name");
            stElem.m_cGrade = atoi(stXML.GetAttrib("grade").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardGradeType(stElem.m_cGrade), stElem.m_iItemID)

            stElem.m_iSubType = atoi(stXML.GetAttrib("sub_type").c_str());
            stElem.m_iTypePara1 = atoi(stXML.GetAttrib("type_para1").c_str());
            stElem.m_iTypePara2 = atoi(stXML.GetAttrib("type_para2").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumItemSubType(stElem.m_iSubType), stElem.m_iItemID)
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iTypePara1 >= 0 && stElem.m_iTypePara2 >= 0, stElem.m_iItemID)

			stElem.m_iUseType = atoi(stXML.GetAttrib("use_type").c_str());
            stElem.m_iUsePara1 = atoi(stXML.GetAttrib("use_para1").c_str());
            stElem.m_iUsePara2 = atoi(stXML.GetAttrib("use_para2").c_str());
            stElem.m_bAutoUse = (bool)atoi(stXML.GetAttrib("auto_use").c_str());
            stElem.m_iFoodType = atoi(stXML.GetAttrib("food_type").c_str());
            stElem.m_iUniqueCount = atoi(stXML.GetAttrib("unique_count").c_str());
            stElem.m_iHasCountLimit = atoi(stXML.GetAttrib("has_count_limit").c_str());
            stElem.m_iHomeBagCountLimit = atoi(stXML.GetAttrib("home_bag_count").c_str());
            stElem.m_iSellPrice = atoi(stXML.GetAttrib("sell_price").c_str());
            stElem.m_bIsNotShowInFight = (bool)atoi(stXML.GetAttrib("not_show_in_fight").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iUniqueCount >= 0, stElem.m_iItemID)
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iSellPrice >= 0, stElem.m_iItemID)

            if(stElem.m_iUniqueCount > 0 || stElem.m_iHomeBagCountLimit > 0)
            {
                stElem.m_stRepeatExchangeItem.m_iItemType = atoi(stXML.GetAttrib("repeat_item_type").c_str());
                stElem.m_stRepeatExchangeItem.m_iCardID = atoi(stXML.GetAttrib("repeat_item_id").c_str());
                stElem.m_stRepeatExchangeItem.m_iNum = atoi(stXML.GetAttrib("repeat_item_num").c_str());
                if(stElem.m_stRepeatExchangeItem.m_iItemType > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stElem.m_stRepeatExchangeItem), stElem.m_iItemID)
                }
            }

            if(stElem.m_iUseType > 0)
            {
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumItemUseType(stElem.m_iUseType), stElem.m_iItemID)
                CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iUsePara1 >= 0 && stElem.m_iUsePara2 >= 0, stElem.m_iItemID)
            }

			m_stConfigMap.insert(std::make_pair(stElem.m_iItemID, stElem));
		}
	}
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicItemConfig* CLogicConfigItemParser::GetConfig(int iID) const
{
    auto stIter = m_stConfigMap.find(iID);
    if (stIter != m_stConfigMap.end()) return (&(stIter->second));

	return (nullptr);
}

std::pair<bool, std::string>
CLogicConfigFashionParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (!stXML.FindElem("root"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (!stXML.FindElem("info"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		m_strSendTitle = stXML.GetAttrib("m_strSendTitle");
		m_stSendContent = CFormatString::Compile(stXML.GetAttrib("m_strSendContent"));

		while (stXML.FindElem("data"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("fashion"))
			{
				TLogicFashionConfigElem stFashion;
				stFashion.m_iFashionID = atoi(stXML.GetAttrib("id").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stFashion.m_iFashionID > 0, stFashion.m_iFashionID)
                CONFIG_ASSERT_EXCEPTION_EX(m_stFashionMap.find(stFashion.m_iFashionID) == m_stFashionMap.end(), stFashion.m_iFashionID)
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidFashionItemID(stFashion.m_iFashionID), stFashion.m_iFashionID)

				stFashion.m_strName = stXML.GetAttrib("name");
				stFashion.m_iBelongCardID = atoi(stXML.GetAttrib("belong_card_id").c_str());
                stFashion.m_iYuanBao = atoi(stXML.GetAttrib("yuanbao").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stFashion.m_iYuanBao >= 0, stFashion.m_iFashionID)
				CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetHeroCardConfig().GetConfig(stFashion.m_iBelongCardID), stFashion.m_iFashionID)

				m_stFashionMap[stFashion.m_iFashionID] = std::move(stFashion);
			}

			while (stXML.FindElem("item"))
			{
				TLogicFashionItemConfigElem stItem;
				stItem.m_iItemID = atoi(stXML.GetAttrib("id").c_str());
				stItem.m_iFashionID = atoi(stXML.GetAttrib("fashion").c_str());
				stItem.m_iYuanbao = atoi(stXML.GetAttrib("yuanbao").c_str());
				stItem.m_iDay = atoi(stXML.GetAttrib("day").c_str());

                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetItemConfig().GetConfig(stItem.m_iItemID), stItem.m_iItemID)
                CONFIG_ASSERT_EXCEPTION_EX(m_stFashionMap.find(stItem.m_iFashionID) != m_stFashionMap.end(), stItem.m_iItemID)
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iYuanbao > 0 && stItem.m_iDay >= 0, stItem.m_iItemID)

				m_stFashionItemMap[stItem.m_iItemID] = std::move(stItem);
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

const TLogicFashionConfigElem* CLogicConfigFashionParser::GetFashionConfig(int32_t iFashionID) const
{
    auto iter = m_stFashionMap.find(iFashionID);
    return iter == m_stFashionMap.end() ? nullptr : &(iter->second);
}

////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigVipParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("vip_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		char szBuffer[128];
		while (stXML.FindElem("vip"))
		{
			TLogicVipConfigElem stElem;

			stElem.m_iNeedChargeAmount = ::atoi(stXML.GetAttrib("need_charge_amount").c_str());
			stElem.m_iDoubleRechargeCount = ::atoi(stXML.GetAttrib("double_recharge_count").c_str());
			stElem.m_iCanBuyActionCoinCount = ::atoi(stXML.GetAttrib("buy_action_coin_count").c_str());
			stElem.m_iCanBuyGoldTimes = ::atoi(stXML.GetAttrib("buy_gold_times").c_str());
			stElem.m_iCanBuyEnergyTimes = ::atoi(stXML.GetAttrib("buy_energy_times").c_str());
			stElem.m_iCanBuyVitalityTimes = ::atoi(stXML.GetAttrib("buy_vitality_times").c_str());
			stElem.m_iCanBuyPVPTimes = ::atoi(stXML.GetAttrib("refresh_pvp_times").c_str());
			stElem.m_iCanBuyRefreshResetCrossFightTimesTimes = ::atoi(stXML.GetAttrib("refresh_corss_fight_times").c_str());
			stElem.m_iCanRefreshClimbTowerCount = ::atoi(stXML.GetAttrib("refresh_climb_tower_count").c_str());

			for (int i = 2; i <= 10; i++)
			{
				snprintf(szBuffer, sizeof(szBuffer), "shake_gold_%d_times_rate", i);
				stElem.m_stShakeGoldRate.push_back(::atoi(stXML.GetAttrib(szBuffer).c_str()));
			}

            for(int i = 1; i < 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("refresh_level_type" + stStream.str()).empty()) break;

                int iLevelType = ::atoi(stXML.GetAttrib("refresh_level_type" + stStream.str()).c_str());
                int iTimes = ::atoi(stXML.GetAttrib("refresh_level_times" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumLevelType(iLevelType))
                CONFIG_ASSERT_EXCEPTION(iTimes >= 0)

                stElem.m_stRefreshLevelMap[iLevelType] = iTimes;
            }

			for (int32_t i = 0; i < 10; ++i)
			{
				CPackGameItem stItem;
				stItem.m_iItemType = ::atoi(stXML.GetAttrib("day_reward_type_" + std::to_string(i)).c_str());
				stItem.m_iCardID = ::atoi(stXML.GetAttrib("day_reward_id_" + std::to_string(i)).c_str());
				stItem.m_iNum = ::atoi(stXML.GetAttrib("day_reward_num_" + std::to_string(i)).c_str());
				if (stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0)
				{
					stElem.m_stDailyAwardVec.push_back(std::move(stItem));
				}
			}

			m_stVipConfig.push_back(stElem);
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

int
CLogicConfigVipParser::GetShakeGoldRate(int iVip) const
{
	if (iVip < 0 || (unsigned int)iVip >= m_stVipConfig.size())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(VIP_INFO_IS_NOT_CONFIGED);
		return (-1);
	}

	int iRet = 1, iAnt = 0;
	int iRandNum = CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_RandomWeight;
	const auto& stShakeGoldRate = m_stVipConfig[iVip].m_stShakeGoldRate;
	for (size_t i = 0; i < stShakeGoldRate.size(); ++i)
	{
		if (0 == stShakeGoldRate[i])
			continue;
		if (stShakeGoldRate[i] + iAnt >= iRandNum)
		{
			iRet = i + 2;
			break;
		}
		iAnt += stShakeGoldRate[i];
	}
	return iRet;
}

int
CLogicConfigVipParser::GetMaxVipLevel() const
{
	return (m_stVipConfig.size() - 1);
}

int
CLogicConfigVipParser::GetVipLevel(int iVipExp) const
{
	for (size_t i = 1; i < m_stVipConfig.size(); ++i)
	{
		if (iVipExp < m_stVipConfig[i].m_iNeedChargeAmount)
		{
			return (i - 1);
		}
	}

	return (m_stVipConfig.size() - 1);
}

int
CLogicConfigVipParser::GetVipExp(int iVipLevel) const
{
	int iMaxVipLevel = GetMaxVipLevel();

	iVipLevel = iVipLevel > iMaxVipLevel ? iMaxVipLevel : iVipLevel;

	return m_stVipConfig[iVipLevel].m_iNeedChargeAmount;
}

const TLogicVipConfigElem*
CLogicConfigVipParser::GetVipInfo(int iVip) const
{
	if (iVip < 0 || (unsigned int)iVip >= m_stVipConfig.size())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(VIP_INFO_IS_NOT_CONFIGED);
		return (nullptr);
	}

	return &(m_stVipConfig[iVip]);
}

int CLogicConfigVipParser::GetRefreshLevelTimes(int iVipLevel, int iLevelType) const
{
    if (iVipLevel < 0 || (unsigned int)iVipLevel >= m_stVipConfig.size())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(VIP_INFO_IS_NOT_CONFIGED);
        return 0;
    }

    auto iter = m_stVipConfig[iVipLevel].m_stRefreshLevelMap.find(iLevelType);
    if(iter != m_stVipConfig[iVipLevel].m_stRefreshLevelMap.end())
        return iter->second;

    return 0;
}

std::pair<bool, std::string>
CLogicConfigVipUnlockParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("unlock"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("vip"))
		{
			std::string strType = stXML.GetAttrib("function_type");
			if (strType.compare("buy_energy") == 0)
			{
				m_iBuyEnergy = atoi(stXML.GetAttrib("vip_level").c_str());
			}
			else if (strType.compare("buy_vitality") == 0)
			{
				m_iBuyVitality = atoi(stXML.GetAttrib("vip_level").c_str());
			}
			else if (strType.compare("buy_gold") == 0)
			{
				m_iBuyGold = atoi(stXML.GetAttrib("vip_level").c_str());
			}
			else if (strType.compare("refresh_elite_refresh") == 0)
			{
				m_iRefreshEliteRefresh = atoi(stXML.GetAttrib("vip_level").c_str());
			}
			else if (strType.compare("refresh_pvp") == 0)
			{
				m_iRefreshPVP = atoi(stXML.GetAttrib("vip_level").c_str());
			}
			else if (strType.compare("ruhua_bonus") == 0)
			{
				m_iRuhuaBonus = atoi(stXML.GetAttrib("vip_level").c_str());
			}
			else if (strType.compare("paoma_bonus") == 0)
			{
				m_iPaomaBonus = atoi(stXML.GetAttrib("vip_level").c_str());
			}
			else if (strType.compare("add_ruhua_time") == 0)
			{
				m_iAddRuhuaTime = atoi(stXML.GetAttrib("vip_level").c_str());
			}
			else if (strType.compare("add_paoma_time") == 0)
			{
				m_iAddPaomaTime = atoi(stXML.GetAttrib("vip_level").c_str());
			}
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
CLogicConfigFragmentCombineParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("items"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();
		while (stXML.FindElem("item"))
		{
            int32_t iID = ::atoi(stXML.GetAttrib("item_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(m_stConfigMap.find(iID) == m_stConfigMap.end(), iID)

            TLogicFragmentCombineConfigElem stElem;
			stElem.m_iItemType = ::atoi(stXML.GetAttrib("item_type").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iItemType == CLogicConfigDefine::EXCHANGE_ITEM, iID)
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stElem.m_iItemType, iID), iID)

            for(int i = 1; i < 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("consume_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("consume_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("consume_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("consume_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), iID)
                stElem.m_stConsumeItem.emplace_back(stItem);
            }

            CONFIG_ASSERT_EXCEPTION_EX(!stElem.m_stConsumeItem.empty(), iID)

            m_stConfigMap[iID] = std::move(stElem);
		}

        // 检查
        for(auto& iter : m_stConfigMap)
        {
            if(CheckCombineLoop(iter.first, {}))
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "Combine loop true! ItemID:" << iter.first << std::endl;
                throw std::exception();
            }
        }
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicFragmentCombineConfigElem* CLogicConfigFragmentCombineParser::GetConfig(int iID) const
{
	auto stIter = m_stConfigMap.find(iID);
	if (stIter != m_stConfigMap.end())
	{
		return &(stIter->second);
	}

	using namespace SERVER_ERRCODE;
	SET_ERR_INFO(COMMON_CONFIG_NOT_EXIST);
	return nullptr;
}

bool CLogicConfigFragmentCombineParser::CheckCombineLoop(int32_t iItemID, std::set<int32_t> stIDList) const
{
    auto iter = m_stConfigMap.find(iItemID);
    if(iter == m_stConfigMap.end())
        return false;

    stIDList.insert(iItemID);
    for(auto& item : iter->second.m_stConsumeItem)
    {
        if(stIDList.find(item.m_iCardID) != stIDList.end())
            return true;

        if(CheckCombineLoop(item.m_iCardID, stIDList))
            return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigAvatarParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("avatar_config"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (false == stXML.FindElem("avatar_icon"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();
		while (stXML.FindElem("avatar_icon"))
		{
			TLogicAvatarElem stElem;
			stElem.m_iAvatarID = atoi(stXML.GetAttrib("avatar_icon_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidAvatarItemID(stElem.m_iAvatarID), stElem.m_iAvatarID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stAvatarList.find(stElem.m_iAvatarID) == m_stAvatarList.end(), stElem.m_iAvatarID)

			stElem.m_iAvatarType = (int)CLogicConfigDefine::AVATAR_DEFINE::ICON_TYPE;
			stElem.m_iUnlockValue = atoi(stXML.GetAttrib("unlock_value").c_str());
			stElem.m_bIsFree = atoi(stXML.GetAttrib("is_activity").c_str()) == 0;
			m_stAvatarList[stElem.m_iAvatarID] = stElem;
			if (!m_iDefaultIconID)
			{
				m_iDefaultIconID = stElem.m_iAvatarID;
			}
		}
		stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(m_iDefaultIconID > 0)

		if (false == stXML.FindElem("avatar_border"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();
		while (stXML.FindElem("border_img"))
		{
			TLogicAvatarElem stElem;
			stElem.m_iAvatarID = atoi(stXML.GetAttrib("border_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidAvatarItemID(stElem.m_iAvatarID), stElem.m_iAvatarID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stAvatarList.find(stElem.m_iAvatarID) == m_stAvatarList.end(), stElem.m_iAvatarID)

			stElem.m_iAvatarType = (int)CLogicConfigDefine::AVATAR_DEFINE::BORDER_TYPE;
			stElem.m_iUnlockValue = atoi(stXML.GetAttrib("unlock_value").c_str());
			stElem.m_bIsFree = atoi(stXML.GetAttrib("is_activity").c_str()) == 0;
			m_stAvatarList[stElem.m_iAvatarID] = stElem;
			if (!m_iDefaultBorderID)
			{
				m_iDefaultBorderID = stElem.m_iAvatarID;
			}
		}
		stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(m_iDefaultBorderID > 0)
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicAvatarElem* CLogicConfigAvatarParser::GetConfig(int32_t iID) const
{
    auto iter = m_stAvatarList.find(iID);
    return iter == m_stAvatarList.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string>
CLogicConfigHonorParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

		CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"));

		stXML.IntoElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("honor"));
		stXML.IntoElem();
		while (stXML.FindElem("data"))
		{
			int32_t id = 0;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), id));

			TLogicHonorElem stElem;
			stElem.m_strName = stXML.GetAttrib("name");
			stElem.m_strBroadcast = stXML.GetAttrib("broadcast");
			stElem.m_stConsumeItem.m_iItemType = atoi(stXML.GetAttrib("consume_item_type").c_str());
			stElem.m_stConsumeItem.m_iCardID = atoi(stXML.GetAttrib("consume_item_id").c_str());
			stElem.m_stConsumeItem.m_iNum = atoi(stXML.GetAttrib("consume_item_num").c_str());
			for (int32_t i = 1; i <= 5; ++i)
			{
				if (!stXML.GetAttrib("task_type_" + std::to_string(i)).empty())
				{
					TLogicHonorElem::honor_task task;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("task_type_" + std::to_string(i)), task.m_iType));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("task_target_" + std::to_string(i)), task.m_iTarget));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("task_target_amount_" + std::to_string(i)), task.m_iTargetAmount));
					CONFIG_ASSERT_EXCEPTION(task.m_iTargetAmount > 0);
					stElem.m_stTaskVec.push_back(task);
					m_stTaskType2HonorIDMap.emplace(task.m_iType, id);
				}

				if (!stXML.GetAttrib("attr_type_" + std::to_string(i)).empty())
				{
					TLogicHonorElem::honor_attr attr;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("card_type"), attr.m_iCardType));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("num_type_" + std::to_string(i)), attr.m_iNumType));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("attr_type_" + std::to_string(i)), attr.m_iAttrType));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("attr_value_" + std::to_string(i)), attr.m_dAttrValue));
					stElem.m_stAttrVec.push_back(attr);
				}
			}

			m_stConfigMap[id] = stElem;			
		}
		stXML.OutOfElem(); //honor
		
		stXML.OutOfElem();//root

	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicHonorElem* CLogicConfigHonorParser::GetHonorConfig(int32_t iHonorID) const
{
	const auto iter = m_stConfigMap.find(iHonorID);
	return iter != m_stConfigMap.end() ? &iter->second : nullptr;
}

std::pair<bool, std::string> CLogicConfigCurrencyParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty())

        CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent))

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"))

        stXML.IntoElem();

        while (stXML.FindElem("item"))
        {
            TLogicCurrencyItem stItem;
            stItem.m_iItemID = ::atoi(stXML.GetAttrib("item_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidCurrencyItemID(stItem.m_iItemID), stItem.m_iItemID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stConfigMap.find(stItem.m_iItemID) == m_stConfigMap.end(), stItem.m_iItemID)

            stItem.m_strName = stXML.GetAttrib("name");
            m_stConfigMap[stItem.m_iItemID] = stItem;
        }

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicCurrencyItem* CLogicConfigCurrencyParser::GetConfig(int32_t iID) const
{
    auto iter = m_stConfigMap.find(iID);
    return iter == m_stConfigMap.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string> CLogicConfigGiftBagParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty())

        CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent))

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"))

        stXML.IntoElem();

        while (stXML.FindElem("gift_bag"))
        {
            int32_t iItemID = ::atoi(stXML.GetAttrib("id").c_str());
            auto pItemConfig = CLogicConfig::GetItemConfig().GetConfig(iItemID);
            CONFIG_ASSERT_EXCEPTION_EX(pItemConfig, iItemID)

            stXML.IntoElem();

            TLogicGiftBagElem stElem;
            if(stXML.FindElem("info"))
            {
                stElem.m_iLevel = ::atoi(stXML.GetAttrib("level").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(try_time_from_string(stXML.GetAttrib("open_date_time"), stElem.m_iOpenDate), iItemID)
                stElem.m_bIsPerDay = (bool)::atoi(stXML.GetAttrib("is_per_day").c_str());
                stElem.m_iIntervalTime = ::atoi(stXML.GetAttrib("interval_time").c_str());
                stElem.m_iOpenCount = ::atoi(stXML.GetAttrib("open_count").c_str());

                CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iIntervalTime >= 0, iItemID)
                CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iOpenCount >= -1, iItemID)
                if(stElem.m_iOpenCount == 1) stElem.m_iOpenCount = 0;
                if(stElem.m_iOpenCount != 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(pItemConfig->m_iUniqueCount == 1, iItemID)
                    CONFIG_ASSERT_EXCEPTION_EX((stElem.m_bIsPerDay || stElem.m_iIntervalTime > 0), iItemID)
                }
            }

            if(stXML.FindElem("item_list"))
            {
                stXML.IntoElem();
                while (stXML.FindElem("item"))
                {
                    CPackGameItem item;
                    item.m_iItemType = ::atoi(stXML.GetAttrib("item_type").c_str());
                    item.m_iCardID = ::atoi(stXML.GetAttrib("item_id").c_str());
                    item.m_iNum = ::atoi(stXML.GetAttrib("item_num").c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(item.m_iCardID != iItemID, iItemID)
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::IsValidGameItem(item), iItemID, item.m_iCardID)

                    stElem.m_stItems.emplace_back(item);
                }
                stXML.OutOfElem();
            }

            if(stXML.FindElem("bag"))
            {
                stElem.m_strCardBag = stXML.GetAttrib("cardbag");
                stElem.m_strLevelDrop = stXML.GetAttrib("level_drop");
                stElem.m_strLevelDropBag = stXML.GetAttrib("level_drop_bag");
                if(!stElem.m_strCardBag.empty())
                {
                    auto pBag = CLogicConfig::GetCardbagConfig().GetConfig(stElem.m_strCardBag);
                    CONFIG_ASSERT_EXCEPTION_EX(pBag, iItemID)
                    for(auto& item : pBag->m_stCardbagElem)
                    {
                        CONFIG_ASSERT_EXCEPTION_EXEX(item.m_iCardID != iItemID, iItemID, stElem.m_strCardBag)
                    }
                }
                if(!stElem.m_strLevelDrop.empty())
                {
                    auto pBag = CLogicConfig::GetLevelDropConfig().GetConfig(stElem.m_strLevelDrop);
                    CONFIG_ASSERT_EXCEPTION_EX(pBag, iItemID)
                    for(auto& item : *pBag)
                    {
                        CONFIG_ASSERT_EXCEPTION_EXEX(item.m_iCardID != iItemID, iItemID, stElem.m_strLevelDrop)
                    }
                }
                if(!stElem.m_strLevelDropBag.empty())
                {
                    auto pBag = CLogicConfig::GetLevelDropBagConfig().GetConfig(stElem.m_strLevelDropBag);
                    CONFIG_ASSERT_EXCEPTION_EX(pBag, iItemID)
                    for(auto& stVec : pBag->m_stDrops)
                    {
                        for(auto& item : stVec.second)
                        {
                            CONFIG_ASSERT_EXCEPTION_EXEX(item.m_iCardID != iItemID, iItemID, stElem.m_strLevelDropBag)
                        }
                    }
                }
            }

            CONFIG_ASSERT_EXCEPTION_EX((!stElem.m_stItems.empty() || !stElem.m_strCardBag.empty() || !stElem.m_strLevelDrop.empty() || !stElem.m_strLevelDropBag.empty()), iItemID)

            m_stConfigMap[iItemID] = std::move(stElem);

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

const TLogicGiftBagElem* CLogicConfigGiftBagParser::GetConfig(int32_t iItemID) const
{
    auto iter = m_stConfigMap.find(iItemID);
    return iter == m_stConfigMap.end() ? nullptr : &(iter->second);
}

bool CLogicConfigGiftBagParser::OpenGift(int32_t iItemID, std::vector<CPackGameItem>& stBonusVec, int32_t iNum/* = 1*/) const
{
    if(iNum < 1) return false;

    auto pConfig = GetConfig(iItemID);
    if(nullptr == pConfig)
        return false;

    stBonusVec = pConfig->m_stItems;
    if(iNum > 1)
    {
        for(auto& item : stBonusVec)
        {
            item.m_iNum *= iNum;
        }
    }

    if(!pConfig->m_strCardBag.empty())
    {
        CLogicConfig::GetCardbagConfig().GetRandBonusByName(pConfig->m_strCardBag, stBonusVec, iNum);
    }

    if(!pConfig->m_strLevelDrop.empty() || !pConfig->m_strLevelDropBag.empty())
    {
        while (iNum--)
        {
            if(!pConfig->m_strLevelDrop.empty())
            {
                CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pConfig->m_strLevelDrop, stBonusVec);
            }

            if(!pConfig->m_strLevelDropBag.empty())
            {
                CLogicConfig::GetLevelDropBagConfig().GetRandReward(pConfig->m_strLevelDropBag, stBonusVec);
            }
        }
    }

    return true;
}
