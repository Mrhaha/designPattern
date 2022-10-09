//
// Created by lxhbroser on 2020/6/6.
//

#include "Markup.h"
#include "logic_config.h"
#include "logic_config_random_store.h"
#include "logic_link_common.h"

std::pair<bool, std::string> CLogicConfigRandomStoreParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if(true == strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if(false == stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        if(false == stXML.FindElem("random_store"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        while (stXML.FindElem("store_info"))
        {
            int32_t iStoreID = ::atoi(stXML.GetAttrib("id").c_str());

            stXML.IntoElem();

            TLogicRandomStoreInfo stStoreInfo;

            if(false == stXML.FindElem("info"))
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << "|StoreID:" << iStoreID << std::endl;
                throw std::exception();
            }

            stStoreInfo.m_iStoreType = ::atoi(stXML.GetAttrib("store_type").c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumRandomStoreType(stStoreInfo.m_iStoreType));

            stStoreInfo.m_iOpenLevel = ::atoi(stXML.GetAttrib("open_level").c_str());
            stStoreInfo.m_iRefreshMaxTimes = ::atoi(stXML.GetAttrib("refresh_max_times").c_str());
            if(stStoreInfo.m_iRefreshMaxTimes <= 0) stStoreInfo.m_iRefreshMaxTimes = 99999;

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << "_" << i;
                if(stXML.GetAttrib("refresh_time" + stStream.str()).empty()) break;

                int32_t iFlashTime = ::atoi(stXML.GetAttrib("refresh_time" + stStream.str()).c_str());
                stStoreInfo.m_stFlashTime.push_back(iFlashTime);
            }

            if(false == stXML.FindElem("refresh_cost"))
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << "|StoreID:" << iStoreID << std::endl;
                throw std::exception();
            }

            stXML.IntoElem();
            while (stXML.FindElem("cost"))
            {
                CPackGameItem stCost;
                stCost.m_iItemType = ::atoi(stXML.GetAttrib("cost_type").c_str());
                stCost.m_iCardID = ::atoi(stXML.GetAttrib("cost_id").c_str());
                stCost.m_iNum = ::atoi(stXML.GetAttrib("cost_num").c_str());

                stStoreInfo.m_stRefreshCost.emplace_back(stCost);
            }
            stXML.OutOfElem();

            if(false == stXML.FindElem("store_data"))
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << "|StoreID:" << iStoreID << std::endl;
                throw std::exception();
            }

            stXML.IntoElem();
            while (stXML.FindElem("level_data"))
            {
                TLogicRandomStoreLimit stStoreLimit;
                stStoreLimit.m_iLevel = ::atoi(stXML.GetAttrib("level_max").c_str());
                stStoreLimit.m_iHeartLevel = ::atoi(stXML.GetAttrib("heart_level_max").c_str());
                if(stStoreLimit.m_iLevel <= 0) stStoreLimit.m_iLevel = 999;
                if(stStoreLimit.m_iHeartLevel <= 0) stStoreLimit.m_iHeartLevel = 999;

                stXML.IntoElem();
                while (stXML.FindElem("item"))
                {
                    LiLogicCardbagRateVector stBagList;
                    int32_t iTotalWeight = 0;
                    for(int i = 1; i <= 10; ++i)
                    {
                        TLogicCardbagRateElem stBag;
                        std::stringstream stStream;
                        stStream << "_" << i;
                        if(stXML.GetAttrib("bag" + stStream.str()).empty()) break;

                        stBag.m_strCardBag = stXML.GetAttrib("bag" + stStream.str());
                        stBag.m_iRate = ::atoi(stXML.GetAttrib("bag_rate" + stStream.str()).c_str());

                        auto stIter = CLogicConfig::GetMallBagConfig().m_stCardBagInfo.find(stBag.m_strCardBag);
                        if(stIter == CLogicConfig::GetMallBagConfig().m_stCardBagInfo.end())
                        {
                            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND CARDBAG " << stBag.m_strCardBag << std::endl;
                            throw std::exception();
                        }

                        iTotalWeight += stBag.m_iRate;
                        stBagList.emplace_back(stBag);
                    }

                    if(iTotalWeight != CLogicConfigDefine::ESD_RandomWeight)
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: iTotalWeight != 10000 |StoreID:" << iStoreID << "|Level:"
                                     << stStoreLimit.m_iLevel << "|HeartLevel:" << stStoreLimit.m_iHeartLevel << std::endl;
                        throw std::exception();
                    }

                    stStoreLimit.m_stStore.emplace_back(stBagList);
                }
                stXML.OutOfElem();

                stStoreInfo.m_stStoreLimit.emplace_back(std::move(stStoreLimit));
            }
            stXML.OutOfElem();

            m_stRandomStoreInfoMap.insert(std::make_pair(iStoreID, stStoreInfo));

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

const TLogicRandomStoreInfo* CLogicConfigRandomStoreParser::GetStoreInfo(int32_t iStoreID) const
{
    const auto iter = m_stRandomStoreInfoMap.find(iStoreID);
    return iter == m_stRandomStoreInfoMap.end() ? nullptr : &(iter->second);
}

std::vector<TLogicCardbagInfoElem> TLogicRandomStoreInfo::GetCommodityElem(int32_t iLevel, int32_t iHeartLevel) const
{
    std::vector<TLogicCardbagInfoElem> stElem;

    auto stIter = m_stStoreLimit.end();
    for(auto iter = m_stStoreLimit.begin(); iter != m_stStoreLimit.end(); ++iter)
    {
        if(iter->m_iLevel >= iLevel && iter->m_iHeartLevel >= iHeartLevel)
        {
            stIter = iter;
            break;
        }
    }
    if (stIter == m_stStoreLimit.end()) return stElem;

    for (size_t i = 0; i < stIter->m_stStore.size(); ++i)
    {
        int iPos = 0;
        int iRandWeight = CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_RandomWeight;
        for (size_t j = 0; j < stIter->m_stStore[i].size(); ++j)
        {
            iPos += stIter->m_stStore[i][j].m_iRate;
            if (iPos >= iRandWeight)
            {
                stElem.push_back(CLogicConfig::GetMallBagConfig().GetCommodityElem(stIter->m_stStore[i][j].m_strCardBag));
                break;
            }
        }
    }

    return stElem;
}

std::pair<bool, std::string> CLogicConfigHeartStoreDiscountParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if(true == strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if(false == stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        if(false == stXML.FindElem("questions"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        if(false == stXML.FindElem("info"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        m_iDialogExp = ::atoi(stXML.GetAttrib("dialog_exp").c_str());
        m_iDailyDialogExpTimes = ::atoi(stXML.GetAttrib("daily_dialog_exp_times").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iDialogExp > 0);

        if(false == stXML.FindElem("favorshop_time"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        m_stLevelList.push_back(TLogicHeartStoreLevel());
        int32_t curLevel = 1;
        while (stXML.FindElem("times"))
        {
            TLogicHeartStoreLevel stLevelInfo;
            int32_t iLevel = ::atoi(stXML.GetAttrib("level").c_str());
            if(iLevel != curLevel++)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: Level not find. Level:" << curLevel << std::endl;
                throw std::exception();
            }

            stLevelInfo.m_iExpValue = ::atoi(stXML.GetAttrib("exp_value").c_str());
            CONFIG_ASSERT_EXCEPTION(stLevelInfo.m_iExpValue >= 0);

            stXML.IntoElem();
            while (stXML.FindElem("shop"))
            {
                int32_t iShopID = ::atoi(stXML.GetAttrib("shop_id").c_str());
                std::vector<int32_t> stDiscountList;
                for (int32_t i = 1; i <= 20; ++i)
                {
                    std::stringstream stStream;
                    stStream << "_" << i;
                    if (stXML.GetAttrib("discount" + stStream.str()).empty()) break;

                    int32_t iDiscount = atoi(stXML.GetAttrib("discount" + stStream.str()).c_str());
                    CONFIG_ASSERT_EXCEPTION(iDiscount > 0);
                    stDiscountList.push_back(iDiscount);
                }

                stLevelInfo.m_stDiscount.insert(std::make_pair(iShopID, stDiscountList));
            }
            stXML.OutOfElem();

            m_stLevelList.emplace_back(std::move(stLevelInfo));
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

void CLogicConfigHeartStoreDiscountParser::ReCalcHeartLevelExp(int32_t& iLevel, int32_t& iExp) const
{
    while ((unsigned int)iLevel < (m_stLevelList.size()-1) && m_stLevelList[iLevel].m_iExpValue <= iExp)
    {
        iExp -= m_stLevelList[iLevel++].m_iExpValue;
    }

    if ((iLevel == (int)m_stLevelList.size() - 1) && (iExp > m_stLevelList[iLevel].m_iExpValue))
    {
        iExp = m_stLevelList[iLevel].m_iExpValue;
    }
}

const TLogicHeartStoreLevel* CLogicConfigHeartStoreDiscountParser::GetConfig(int32_t iLevel) const
{
    if(iLevel <= 0 || iLevel >= (int32_t)m_stLevelList.size())
        return nullptr;

    return &(m_stLevelList[iLevel]);
}
