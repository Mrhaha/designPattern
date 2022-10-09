#include "logic_config_store.h"
#include "Markup.h"
#include "server_errcode.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_config.h"
#include "common_datetime.h"

////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigLevelDropParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("cardbag_data"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		while (stXML.FindElem("cardbag"))
		{
			std::vector<TLogicRandBonusItem> stBonusVec;
			std::string strCardBagName = stXML.GetAttrib("cardbag_id");

			stXML.IntoElem();
			while (stXML.FindElem("card_id"))
			{
				TLogicRandBonusItem stBonusItem;
				stBonusItem.m_iCardID = ::atoi(stXML.GetAttrib("card_id").c_str());
				stBonusItem.m_iItemType = ::atoi(stXML.GetAttrib("item_type").c_str());
				stBonusItem.m_iWeight = ::atoi(stXML.GetAttrib("weight").c_str());
				stBonusItem.m_iNum = ::atoi(stXML.GetAttrib("num").c_str());
                stBonusItem.m_iNumMax = ::atoi(stXML.GetAttrib("num_max").c_str());
				stBonusVec.push_back(stBonusItem);
			}
			stXML.OutOfElem();
			m_stBonusMap[strCardBagName] = stBonusVec;
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

std::vector<CPackGameItem>
CLogicConfigLevelDropParser::GetRandBonusByName(const std::string& strCardBagName) const
{
	std::vector<CPackGameItem> stRandBonus;
	auto stBonusIT = m_stBonusMap.find(strCardBagName);

	if (stBonusIT != m_stBonusMap.end())
	{
		for (size_t i = 0; i < stBonusIT->second.size(); ++i)
		{
			int iRandNum = CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_RandomWeight;
			if (iRandNum < stBonusIT->second[i].m_iWeight)
			{
                int iRandItemNum = CLogicCommon::GetRandNum(stBonusIT->second[i].m_iNum,stBonusIT->second[i].m_iNumMax);
                CLogicConfigUtil::CombineGameItem(stRandBonus, CPackGameItem(stBonusIT->second[i].m_iItemType,
					stBonusIT->second[i].m_iCardID,iRandItemNum));
			}
		}
	}

	return stRandBonus;
}

void
CLogicConfigLevelDropParser::GetRandBonusByName(const std::string& strCardBagName, std::vector<CPackGameItem>& stRandBonus) const
{
    if(strCardBagName.empty()) return;

	auto stBonusIT = m_stBonusMap.find(strCardBagName);
	if (stBonusIT != m_stBonusMap.end())
	{
		for (size_t i = 0; i < stBonusIT->second.size(); ++i)
		{
			int iRandNum = CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_RandomWeight;
            if (iRandNum < stBonusIT->second[i].m_iWeight)
            {
                int iRandItemNum = stBonusIT->second[i].m_iNum;
                if (stBonusIT->second[i].m_iNumMax != 0 && stBonusIT->second[i].m_iNumMax > stBonusIT->second[i].m_iNum)
                {
                    iRandItemNum = CLogicCommon::GetRandNum(stBonusIT->second[i].m_iNum,stBonusIT->second[i].m_iNumMax);
                }
                CLogicConfigUtil::CombineGameItem(stRandBonus, CPackGameItem(stBonusIT->second[i].m_iItemType,stBonusIT->second[i].m_iCardID,iRandItemNum));
            }
		}
	}
}

const std::vector<TLogicRandBonusItem>* CLogicConfigLevelDropParser::GetConfig(std::string strName) const
{
    auto iter = m_stBonusMap.find(strName);
    return iter == m_stBonusMap.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string> CLogicConfigLevelDropBagParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if (false == stXML.FindElem("bag_list"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        while (stXML.FindElem("bag"))
        {
            TLogicLevelDropBag stBag;
            std::string strBagName = stXML.GetAttrib("bag_id");
            CONFIG_ASSERT_EXCEPTION(m_stBags.end() == m_stBags.find(strBagName));

            stBag.m_iTotalWeight = 0;

            stXML.IntoElem();
            while (stXML.FindElem("index"))
            {
                int32_t iWeight = ::atoi(stXML.GetAttrib("weight").c_str());
                CONFIG_ASSERT_EXCEPTION(iWeight >= 0);

                stBag.m_iTotalWeight += iWeight;

                std::vector<CPackGameItem> stReward;
                for (int i = 1; i <= 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if (stXML.GetAttrib("item_type" + stStream.str()).empty()) break;

                    CPackGameItem stItem;
                    stItem.m_iItemType = atoi(stXML.GetAttrib("item_type" + stStream.str()).c_str());
                    stItem.m_iCardID = atoi(stXML.GetAttrib("item_id" + stStream.str()).c_str());
                    stItem.m_iNum = atoi(stXML.GetAttrib("item_num" + stStream.str()).c_str());

                    stReward.emplace_back(stItem);
                }

                stBag.m_stDrops.emplace_back(std::make_pair(iWeight, stReward));
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION(stBag.m_iTotalWeight > 0);
            m_stBags[strBagName] = std::move(stBag);
        }

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicLevelDropBag* CLogicConfigLevelDropBagParser::GetConfig(const std::string& strBag) const
{
    auto iter = m_stBags.find(strBag);
    return iter == m_stBags.end() ? nullptr : &(iter->second);
}

void CLogicConfigLevelDropBagParser::GetRandReward(const std::string& strBag, std::vector<CPackGameItem>& stBonus) const
{
    if(strBag.empty()) return;

    auto iterBag = m_stBags.find(strBag);
    if (iterBag != m_stBags.end())
    {
        int32_t iRandWeight = CLogicCommon::GetRandNum() % iterBag->second.m_iTotalWeight;
        int32_t iCurWeight = 0;
        for(auto& stOrder : iterBag->second.m_stDrops)
        {
            iCurWeight += stOrder.first;
            if(iCurWeight > iRandWeight)
            {
                stBonus.insert(stBonus.end(), stOrder.second.begin(), stOrder.second.end());
                return;
            }
        }
    }
}

std::pair<bool, std::string> CLogicConfigMallParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("mall_data"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
		}

		stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"))
        m_iBaodiTimes = ::atoi(stXML.GetAttrib("baodi_bag_times").c_str());
        m_strBaodiBag = stXML.GetAttrib("baodi_bag");
        m_iExcludeID = ::atoi(stXML.GetAttrib("exclude_id").c_str());
        m_iRecordGrade = ::atoi(stXML.GetAttrib("record_grade").c_str());
        if(m_iBaodiTimes > 0)
        {
            CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetCardbagConfig().GetConfig(m_strBaodiBag))
            CONFIG_ASSERT_EXCEPTION(m_iExcludeID >= 0)
            CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumCardGradeType(m_iRecordGrade))
        }

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("commodity_list"))

        stXML.IntoElem();
		while (stXML.FindElem("commodity"))
		{
			int32_t iCommodityID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(m_stMallConfig.find(iCommodityID) == m_stMallConfig.end(), iCommodityID)

            TLogicMallConfigElem stMallElem;
            stMallElem.m_iAvyID = ::atoi(stXML.GetAttrib("avy_id").c_str());
            if(stMallElem.m_iAvyID > 0)
            {
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetAvyTimeConfig().GetConfig(stMallElem.m_iAvyID), iCommodityID)
            }

            stMallElem.m_strName = stXML.GetAttrib("name");
            CONFIG_ASSERT_EXCEPTION(!stMallElem.m_strName.empty());

            stMallElem.m_bFirstFree = (bool)::atoi(stXML.GetAttrib("first_free").c_str());
            if(stMallElem.m_bFirstFree)
            {
                stMallElem.m_strFirstBag = stXML.GetAttrib("first_bag");
                if(!stMallElem.m_strFirstBag.empty())
                {
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(stMallElem.m_strFirstBag), iCommodityID)
                }
            }

            stMallElem.m_bIsOnlyPayRmb = (bool)::atoi(stXML.GetAttrib("is_only_pay_rmb").c_str());
            stMallElem.m_iTotalTimes = ::atoi(stXML.GetAttrib("total_times").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stMallElem.m_iTotalTimes >= 0, iCommodityID)
            stMallElem.m_strCardBag = stXML.GetAttrib("card_bag");
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(stMallElem.m_strCardBag), iCommodityID)
            stMallElem.m_iRecordGrade = ::atoi(stXML.GetAttrib("record_grade").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardGradeType(stMallElem.m_iRecordGrade), iCommodityID)
            stMallElem.m_strTenBaodiBag = stXML.GetAttrib("ten_baodi_bag");
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(stMallElem.m_strTenBaodiBag), iCommodityID)
            stMallElem.m_iDailyLimitPayRmb = ::atoi(stXML.GetAttrib("daily_limit_pay_rmb").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stMallElem.m_iDailyLimitPayRmb >= 0, iCommodityID)
            stMallElem.m_iDailyLimitPayTimes = ::atoi(stXML.GetAttrib("daily_limit_pay_times").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stMallElem.m_iDailyLimitPayTimes >= 0, iCommodityID)
            stMallElem.m_iSinglePrice = ::atoi(stXML.GetAttrib("single_price").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stMallElem.m_iSinglePrice >= 0, iCommodityID)
            stMallElem.m_iTenPrice = ::atoi(stXML.GetAttrib("ten_price").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stMallElem.m_iTenPrice >= 0, iCommodityID)

            stMallElem.m_stSingleItem.m_iItemType = ::atoi(stXML.GetAttrib("single_item_type").c_str());
            stMallElem.m_stSingleItem.m_iCardID = ::atoi(stXML.GetAttrib("single_item_id").c_str());
            stMallElem.m_stSingleItem.m_iNum = ::atoi(stXML.GetAttrib("single_item_num").c_str());
            if(stMallElem.m_stSingleItem.m_iItemType > 0)
            {
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stMallElem.m_stSingleItem), iCommodityID)
            }

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("ten_item_type" + stStream.str()).empty()) break;
                CPackGameItem gameItem;
                gameItem.m_iItemType = ::atoi(stXML.GetAttrib("ten_item_type" + stStream.str()).c_str());
                gameItem.m_iCardID = ::atoi(stXML.GetAttrib("ten_item_id" + stStream.str()).c_str());
                gameItem.m_iNum = ::atoi(stXML.GetAttrib("ten_item_num" + stStream.str()).c_str());
                if(gameItem.m_iItemType > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(gameItem), iCommodityID)
                }
                stMallElem.m_stTenItem.emplace_back(gameItem);
            }

            //初始化星级-权重
            stMallElem.m_iStartFixed = ::atoi(stXML.GetAttrib("start_fixed").c_str());
            stMallElem.m_iPerFixedWeight = ::atoi(stXML.GetAttrib("per_fixed_weight").c_str());
            auto initCardConfig = CLogicConfig::GetCardbagConfig().GetConfig(stMallElem.m_strCardBag);
            for(auto iterCardWeight : initCardConfig->m_stCardbagElem)
            {
                auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iterCardWeight.m_iCardID);
                CONFIG_ASSERT_EXCEPTION_EX(pCardConfig != nullptr,pCardConfig->m_iHeroCardID);

                stMallElem.m_initCardWeight[pCardConfig->m_iInitStar] += iterCardWeight.m_iWeight;

                stMallElem.m_iInitGradeCard[pCardConfig->m_iInitStar].m_iTotalWeight += iterCardWeight.m_iWeight;
                stMallElem.m_iInitGradeCard[pCardConfig->m_iInitStar].m_stCardbagElem.push_back(iterCardWeight);
            }


            auto initTenCardConfig = CLogicConfig::GetCardbagConfig().GetConfig(stMallElem.m_strTenBaodiBag);
            for(auto iterCardWeight : initTenCardConfig->m_stCardbagElem)
            {
                auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iterCardWeight.m_iCardID);
                CONFIG_ASSERT_EXCEPTION_EX(pCardConfig != nullptr,pCardConfig->m_iHeroCardID);
                stMallElem.m_initTenCardWeight[pCardConfig->m_iInitStar] += iterCardWeight.m_iWeight;

                stMallElem.m_iInitTenGradeCard[pCardConfig->m_iInitStar].m_iTotalWeight += iterCardWeight.m_iWeight;
                stMallElem.m_iInitTenGradeCard[pCardConfig->m_iInitStar].m_stCardbagElem.push_back(iterCardWeight);
            }

            m_stMallConfig[iCommodityID] = std::move(stMallElem);
		}
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("pt_exchange"))

        stXML.IntoElem();
        while (stXML.FindElem("info"))
        {
            int32_t iCommodityID = ::atoi(stXML.GetAttrib("id").c_str());
            auto iterConfig = m_stMallConfig.find(iCommodityID);
            CONFIG_ASSERT_EXCEPTION_EX(iterConfig != m_stMallConfig.end(), iCommodityID)

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("card_id" + stStream.str()).empty()) break;

                int32_t iCardID = ::atoi(stXML.GetAttrib("card_id" + stStream.str()).c_str());
                int32_t iPt = ::atoi(stXML.GetAttrib("pt" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetHeroCardConfig().GetConfig(iCardID), iCommodityID)
                CONFIG_ASSERT_EXCEPTION_EX(iPt > 0, iCommodityID)

                iterConfig->second.m_stPtCardExchangeMap[iCardID] = iPt;
            }

            CONFIG_ASSERT_EXCEPTION_EX(!iterConfig->second.m_stPtCardExchangeMap.empty(), iCommodityID)
        }
        stXML.OutOfElem();
	}
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicMallConfigElem* CLogicConfigMallParser::GetConfig(int iCommodityID) const
{
    auto iter = m_stMallConfig.find(iCommodityID);
    if(iter == m_stMallConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(BAD_COMMODITY_ID);
        return nullptr;
    }

    return &(iter->second);
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigCardbagParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if (false == stXML.FindElem("cardbag_data"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

		stXML.IntoElem();

		while (stXML.FindElem("cardbag"))
		{
			TLogicCardbagConfigElem stCardbagElem;
			std::string strCardBagName = stXML.GetAttrib("cardbag_id");
            CONFIG_ASSERT_EXCEPTION_EX(!strCardBagName.empty(), strCardBagName)
            CONFIG_ASSERT_EXCEPTION_EX(m_stCardbagConfig.find(strCardBagName) == m_stCardbagConfig.end(), strCardBagName)

            stCardbagElem.m_iConfigTotalWeight = ::atoi(stXML.GetAttrib("total_weight").c_str());

			stXML.IntoElem();
			while (stXML.FindElem("card_id"))
			{
				TLogicRandBonusItem stRandBonusItem;
				stRandBonusItem.m_iOrder = ::atoi(stXML.GetAttrib("order").c_str());
				stRandBonusItem.m_iCardID = ::atoi(stXML.GetAttrib("card_id").c_str());
				stRandBonusItem.m_iItemType = ::atoi(stXML.GetAttrib("item_type").c_str());
				stRandBonusItem.m_iWeight = ::atoi(stXML.GetAttrib("weight").c_str());
				stRandBonusItem.m_iNum = ::atoi(stXML.GetAttrib("num").c_str());
                stRandBonusItem.m_iNumMax = ::atoi(stXML.GetAttrib("num_max").c_str());
				stRandBonusItem.m_iNotify = ::atoi(stXML.GetAttrib("notify").c_str());

                CONFIG_ASSERT_EXCEPTION_EXEX(stRandBonusItem.m_iWeight >= 0, strCardBagName, stRandBonusItem.m_iOrder)
                CONFIG_ASSERT_EXCEPTION_EXEX(stRandBonusItem.m_iNum > 0, strCardBagName, stRandBonusItem.m_iOrder)

                if(stRandBonusItem.m_iNumMax > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EXEX(stRandBonusItem.m_iNum < stRandBonusItem.m_iNumMax, strCardBagName, stRandBonusItem.m_iOrder)
                }

				stCardbagElem.m_iTotalWeight += stRandBonusItem.m_iWeight;
				stCardbagElem.m_stCardbagElem.push_back(stRandBonusItem);
			}
			stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION_EX(stCardbagElem.m_iTotalWeight > 0, strCardBagName)

            if(stCardbagElem.m_iConfigTotalWeight > 0)
            {
                CONFIG_ASSERT_EXCEPTION_EX(stCardbagElem.m_iConfigTotalWeight > stCardbagElem.m_iTotalWeight, strCardBagName)
            }

			m_stCardbagConfig[strCardBagName] = stCardbagElem;
		}
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicCardbagConfigElem* CLogicConfigCardbagParser::GetConfig(std::string strBag) const
{
    auto iter = m_stCardbagConfig.find(strBag);
    return iter == m_stCardbagConfig.end() ? nullptr : &(iter->second);
}

std::vector<TLogicRandBonusItem> CLogicConfigCardbagParser::GetRandBonusByName(const std::string& strCardbagName, int iTimes/* = 1*/) const
{
	std::vector<TLogicRandBonusItem> stBonusVec;

	auto stCardbagElemIT = m_stCardbagConfig.find(strCardbagName);
	if (stCardbagElemIT == m_stCardbagConfig.end())
		return stBonusVec;

	auto& stCardbagElem = stCardbagElemIT->second;
	int iTotalWeight = stCardbagElem.m_iTotalWeight;
    if(stCardbagElem.m_iConfigTotalWeight > 0)
    {
        iTotalWeight = stCardbagElem.m_iConfigTotalWeight;
    }

	for (int iIndex = 0; iIndex < iTimes; ++iIndex)
	{
		int iRandWeight = CLogicCommon::GetRandNum() % iTotalWeight;
        for(auto& stBagElem : stCardbagElem.m_stCardbagElem)
        {
            if (stBagElem.m_iWeight <= 0)
                continue;

            if (iRandWeight < stBagElem.m_iWeight)
            {
                stBonusVec.push_back(stBagElem);
                break;
            }

            iRandWeight -= stBagElem.m_iWeight;
        }
	}

	return stBonusVec;
}

void
CLogicConfigCardbagParser::GetRandBonusByName(const std::string& strCardbagName, std::vector<CPackGameItem>& stBonusVec, int iTimes/* = 1*/) const
{
	std::vector<TLogicRandBonusItem> stRandBonusVec = GetRandBonusByName(strCardbagName, iTimes);
	for (auto rand : stRandBonusVec)
	{
        int iNum = rand.m_iNum;
        if(rand.m_iNumMax > 0) iNum = CLogicCommon::GetRandNum(iNum, rand.m_iNumMax, true);

		stBonusVec.emplace_back(rand.m_iItemType, rand.m_iCardID, iNum);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigBuyResourceParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("data_buylist"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"))
        m_iPVPOnceBuyTimes = ::atoi(stXML.GetAttrib("pvp_once_buy_times").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iPVPOnceBuyTimes > 0)

		while (stXML.FindElem("buy_types"))
		{
			if (0 == stXML.GetAttrib("buy_type").compare("buy_gold"))
			{
				stXML.IntoElem();
				TLogicBuyResourceElem stElem;
				m_stBuyGoldConfig.push_back(stElem);
				while (stXML.FindElem("item"))
				{
					stElem.m_stConsumeItem = CPackGameItem(
						::atoi(stXML.GetAttrib("consume_item_type").c_str()),
						::atoi(stXML.GetAttrib("consume_item_id").c_str()),
						::atoi(stXML.GetAttrib("consume_item_amount").c_str()));

					stElem.m_stGetItem = CPackGameItem(
						::atoi(stXML.GetAttrib("get_item_type").c_str()),
						::atoi(stXML.GetAttrib("get_item_id").c_str()),
						::atoi(stXML.GetAttrib("get_item_amount").c_str()));

					m_stBuyGoldConfig.push_back(stElem);
				}
				stXML.OutOfElem();
			}
			else if (0 == stXML.GetAttrib("buy_type").compare("buy_energy"))
			{
				stXML.IntoElem();
				TLogicBuyResourceElem stElem;
				m_stBuyEnergyConfig.push_back(stElem);
				while (stXML.FindElem("item"))
				{
					stElem.m_stConsumeItem = CPackGameItem(
						::atoi(stXML.GetAttrib("consume_item_type").c_str()),
						::atoi(stXML.GetAttrib("consume_item_id").c_str()),
						::atoi(stXML.GetAttrib("consume_item_amount").c_str()));

					stElem.m_stGetItem = CPackGameItem(
						::atoi(stXML.GetAttrib("get_item_type").c_str()),
						::atoi(stXML.GetAttrib("get_item_id").c_str()),
						::atoi(stXML.GetAttrib("get_item_amount").c_str()));

					m_stBuyEnergyConfig.push_back(stElem);
				}
				stXML.OutOfElem();
			}
			else if (0 == stXML.GetAttrib("buy_type").compare("buy_vitality"))
			{
				stXML.IntoElem();
				TLogicBuyResourceElem stElem;
				m_stBuyVitalityConfig.push_back(stElem);
				while (stXML.FindElem("item"))
				{
					stElem.m_stConsumeItem = CPackGameItem(
						::atoi(stXML.GetAttrib("consume_item_type").c_str()),
						::atoi(stXML.GetAttrib("consume_item_id").c_str()),
						::atoi(stXML.GetAttrib("consume_item_amount").c_str()));

					stElem.m_stGetItem = CPackGameItem(
						::atoi(stXML.GetAttrib("get_item_type").c_str()),
						::atoi(stXML.GetAttrib("get_item_id").c_str()),
						::atoi(stXML.GetAttrib("get_item_amount").c_str()));

					m_stBuyVitalityConfig.push_back(stElem);
				}
				stXML.OutOfElem();
			}
			else if (0 == stXML.GetAttrib("buy_type").compare("refresh_pvp"))
			{
				stXML.IntoElem();
				CPackGameItem stElem;
				m_stRefreshPVP.push_back(stElem);
				while (stXML.FindElem("item"))
				{
					m_stRefreshPVP.push_back(CPackGameItem(
						::atoi(stXML.GetAttrib("consume_item_type").c_str()),
						::atoi(stXML.GetAttrib("consume_item_id").c_str()),
						::atoi(stXML.GetAttrib("consume_item_amount").c_str())));
				}
				stXML.OutOfElem();
			}
            else if (0 == stXML.GetAttrib("buy_type").compare("refresh_level"))
            {
                stXML.IntoElem();
                while(stXML.FindElem("level_type"))
                {
                    int iLevelType = ::atoi(stXML.GetAttrib("type").c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumLevelType(iLevelType), iLevelType)

                    stXML.IntoElem();
                    CPackGameItem stElem;
                    while (stXML.FindElem("item"))
                    {
                        stElem.m_iItemType = ::atoi(stXML.GetAttrib("consume_type").c_str());
                        stElem.m_iCardID = ::atoi(stXML.GetAttrib("consume_id").c_str());
                        stElem.m_iNum = ::atoi(stXML.GetAttrib("consume_num").c_str());
                        CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stElem), iLevelType)

                        m_stRefreshLevelTypeMap[iLevelType].push_back(stElem);
                    }
                    stXML.OutOfElem();
                }
                stXML.OutOfElem();
            }
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicBuyResourceElem*
CLogicConfigBuyResourceParser::GetBuyGoldConfig(int iTimes) const
{
    if(m_stBuyGoldConfig.empty())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(BUY_RESOURCE_IS_OUT_OF_RANGE);
        return (nullptr);
    }

	if ((unsigned int)iTimes >= m_stBuyGoldConfig.size())
	{
        return &(m_stBuyGoldConfig.back());
	}

	return &(m_stBuyGoldConfig[iTimes]);
}

const TLogicBuyResourceElem*
CLogicConfigBuyResourceParser::GetBuyEnergyConfig(int iTimes) const
{
	if ((unsigned int)iTimes >= m_stBuyEnergyConfig.size())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(BUY_RESOURCE_IS_OUT_OF_RANGE);
		return (nullptr);
	}
	return &(m_stBuyEnergyConfig[iTimes]);
}

const TLogicBuyResourceElem*
CLogicConfigBuyResourceParser::GetBuyVitalityConfig(int iTimes) const
{
	if ((unsigned int)iTimes >= m_stBuyVitalityConfig.size())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(BUY_RESOURCE_IS_OUT_OF_RANGE);
		return (nullptr);
	}
	return &(m_stBuyVitalityConfig[iTimes]);
}

const CPackGameItem*
CLogicConfigBuyResourceParser::GetRefreshPVPConfig(int iTimes) const
{
	if ((unsigned int)iTimes >= m_stRefreshPVP.size())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(BUY_RESOURCE_IS_OUT_OF_RANGE);
		return (nullptr);
	}
	return &(m_stRefreshPVP[iTimes]);
}

const CPackGameItem* CLogicConfigBuyResourceParser::GetRefreshLevelTypeConfig(int iLevelType, int iTimes) const
{
    auto iterMap = m_stRefreshLevelTypeMap.find(iLevelType);
    if(iterMap == m_stRefreshLevelTypeMap.end())
        return nullptr;

    if(iTimes < 0 || iTimes >= int(iterMap->second.size()))
        return nullptr;

    return &(iterMap->second[iTimes]);
}

std::pair<bool, std::string>
CLogicConfigMallBagParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("mallbag"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("mallbag_data"))
		{
			std::string strCardBagName = stXML.GetAttrib("cardbag_id");

			stXML.IntoElem();

			while (stXML.FindElem("item_type"))
			{
				TLogicCardbagInfoElem stElem;

				stElem.m_iWeight = atoi(stXML.GetAttrib("weight").c_str());
				stElem.m_iDiscount = atoi(stXML.GetAttrib("discount").c_str());

				stElem.m_stCommodityItem.m_iItemType = atoi(stXML.GetAttrib("item_type").c_str());
				stElem.m_stCommodityItem.m_iCardID = atoi(stXML.GetAttrib("card_id").c_str());
				stElem.m_stCommodityItem.m_iNum = atoi(stXML.GetAttrib("num").c_str());

				stElem.m_stConsumeItem.m_iItemType = atoi(stXML.GetAttrib("price_type").c_str());
				stElem.m_stConsumeItem.m_iCardID = atoi(stXML.GetAttrib("price_id").c_str());
				stElem.m_stConsumeItem.m_iNum = atoi(stXML.GetAttrib("price_num").c_str());

				m_stCardBagInfo[strCardBagName].push_back(stElem);
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

TLogicCardbagInfoElem
CLogicConfigMallBagParser::GetCommodityElem(const std::string& strCardBagName) const
{
	TLogicCardbagInfoElem stElem;

	auto stIter = m_stCardBagInfo.find(strCardBagName);
	if (stIter != m_stCardBagInfo.end())
	{
		int32_t iTotalWeight = 0;
		for (const auto& config : stIter->second)
		{
			iTotalWeight += config.m_iWeight;
		}

		if (iTotalWeight <= 0)
		{
			return stElem;
		}

		int iPos = 0;
		int iRandWeight = CLogicCommon::GetRandNum() % iTotalWeight;
		for (size_t i = 0; i < stIter->second.size(); ++i)
		{
			iPos += stIter->second[i].m_iWeight;
			if (iPos >= iRandWeight)
			{
				return stIter->second[i];
			}
		}
	}

	return stElem;
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string> CLogicConfigShopParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		while (stXML.FindElem("shop"))
		{
            int iShopID = ::atoi(stXML.GetAttrib("id").c_str());

			stXML.IntoElem();

            if (stXML.FindElem("info") && m_stConstelShopConfig.m_iRefreshCount == 0)
            {
                m_stConstelShopConfig.m_iRefreshCount = ::atoi(stXML.GetAttrib("refresh_count").c_str());
                m_stConstelShopConfig.m_iGradePro3 = ::atoi(stXML.GetAttrib("grade_pro_3").c_str());
                m_stConstelShopConfig.m_iGradePro4 = ::atoi(stXML.GetAttrib("grade_pro_4").c_str());
                m_stConstelShopConfig.m_iGradePro5 = ::atoi(stXML.GetAttrib("grade_pro_5").c_str());
                m_stConstelShopConfig.m_iGradePro6 = ::atoi(stXML.GetAttrib("grade_pro_6").c_str());
                m_stConstelShopConfig.m_iRefreshTime = ::atoi(stXML.GetAttrib("refresh_time").c_str());
                m_stConstelShopConfig.m_iSpecialPro = ::atoi(stXML.GetAttrib("special_pro").c_str());

                m_stConstelShopConfig.m_iRefreshConsume.m_iItemType = ::atoi(stXML.GetAttrib("refresh_consume_type").c_str());
                m_stConstelShopConfig.m_iRefreshConsume.m_iCardID = ::atoi(stXML.GetAttrib("refresh_consume_id").c_str());
                m_stConstelShopConfig.m_iRefreshConsume.m_iNum = ::atoi(stXML.GetAttrib("refresh_consume_num").c_str());

            }

            CONFIG_ASSERT_EXCEPTION_EX(stXML.FindElem("item_list"), iShopID)

            stXML.IntoElem();
			while (stXML.FindElem("item"))
			{
				int iIndex = ::atoi(stXML.GetAttrib("index").c_str());
                CONFIG_ASSERT_EXCEPTION_EXEX(m_stShopMap.find(iIndex) == m_stShopMap.end(), iShopID, iIndex)

                int iDepositID = ::atoi(stXML.GetAttrib("deposit_id").c_str());
                if(iDepositID > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::GetDepositConfig().GetDepositConfig(iDepositID), iShopID, iIndex)
                    continue;
                }

				TLogicShopItemElem stElem;
                stElem.m_iAvyID = ::atoi(stXML.GetAttrib("avy_id").c_str());
                if(stElem.m_iAvyID > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::GetAvyTimeConfig().GetConfig(stElem.m_iAvyID), iShopID, iIndex)
                }

                std::string strShopIndex = stXML.GetAttrib("content_shop_index").c_str();
                auto stShopIndexVec = string_split(strShopIndex, "|");
                for(std::size_t i = 0; i < stShopIndexVec.size(); ++i)
                {
                    int shopIndex = ::atoi(stShopIndexVec[i].c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetShopConfig().GetConfig(shopIndex),iIndex);
                    stElem.m_vecContentIndex.push_back(shopIndex);
                }

                if(!stElem.m_vecContentIndex.empty()) //针对集合物品的操作
                {
                    m_stShopMap.emplace(iIndex, stElem);
                    continue;
                }

                if(iDepositID > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::GetDepositConfig().GetDepositConfig(iDepositID), iShopID, iIndex)
                    continue;
                }

                stElem.m_stConsumeItem.m_iItemType = atoi(stXML.GetAttrib("consume_type1").c_str());
                stElem.m_stConsumeItem.m_iCardID = atoi(stXML.GetAttrib("consume_id1").c_str());
                stElem.m_stConsumeItem.m_iNum = atoi(stXML.GetAttrib("consume_num1").c_str());
                if(stElem.m_stConsumeItem.m_iItemType > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::IsValidGameItem(stElem.m_stConsumeItem), iShopID, iIndex)
                }

                stElem.m_stOrConsumeItem.m_iItemType = atoi(stXML.GetAttrib("consume_type2").c_str());
                stElem.m_stOrConsumeItem.m_iCardID = atoi(stXML.GetAttrib("consume_id2").c_str());
                stElem.m_stOrConsumeItem.m_iNum = atoi(stXML.GetAttrib("consume_num2").c_str());
                if(stElem.m_stOrConsumeItem.m_iItemType > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::IsValidGameItem(stElem.m_stOrConsumeItem), iShopID, iIndex)
                }

                stElem.m_stGetItem.m_iItemType = atoi(stXML.GetAttrib("get_type").c_str());
                stElem.m_stGetItem.m_iCardID = atoi(stXML.GetAttrib("get_id").c_str());
                stElem.m_stGetItem.m_iNum = atoi(stXML.GetAttrib("get_num").c_str());
                CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::IsValidGameItem(stElem.m_stGetItem), iShopID, iIndex)

                stElem.m_iLimitType = ::atoi(stXML.GetAttrib("limit_type").c_str());
                stElem.m_iLimitBuyCount = ::atoi(stXML.GetAttrib("limit_buy_count").c_str());
                CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfigDefine::IsValidEnumStoreLimitType(stElem.m_iLimitType), iShopID, iIndex)
                CONFIG_ASSERT_EXCEPTION_EXEX(stElem.m_iLimitBuyCount >= 0, iShopID, iIndex)

                stElem.m_iRuleType = ::atoi(stXML.GetAttrib("rule_type").c_str());
                stElem.m_iRulePara = ::atoi(stXML.GetAttrib("rule_para").c_str());
                CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfigDefine::IsValidEnumShopRuleType(stElem.m_iRuleType), iShopID, iIndex)

                m_stLimitTypeMap[stElem.m_iLimitType].push_back(iIndex);
				m_stShopMap.emplace(iIndex, stElem);

                int32_t iGrade = ::atoi(stXML.GetAttrib("grade").c_str());
                if (iGrade > 0)
                {
                    m_stConstelShop[iGrade].push_back(iIndex);
                }
			}
            stXML.OutOfElem();

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

const TLogicShopItemElem* CLogicConfigShopParser::GetConfig(int32_t iIndex) const
{
    auto iter = m_stShopMap.find(iIndex);
    return iter == m_stShopMap.end() ? nullptr : &(iter->second);
}
