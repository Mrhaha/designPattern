//
// Created by lxhbroser on 2020/3/3.
//

#include "logic_config_card_dialog.h"
#include "Markup.h"
#include "common_datetime.h"
#include "logic_config.h"

std::pair<bool, std::string> CLogicConfigCardDialogParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        if(false == stXML.FindElem("card_dialog"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("times"));
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iMin = atoi(stXML.GetAttrib("min").c_str());
            int32_t iMax = atoi(stXML.GetAttrib("max").c_str());
            CONFIG_ASSERT_EXCEPTION(iMin <= iMax);
            m_stTimesCDList.push_back(std::make_pair(iMin, iMax));
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("reward_datas"));
        stXML.IntoElem();
        while (stXML.FindElem("reward_bag"))
        {
            std::string strBag = stXML.GetAttrib("bag");

            stXML.IntoElem();
            while (stXML.FindElem("level"))
            {
                std::vector<std::string> stBonusBagList;
                int32_t iLevel = atoi(stXML.GetAttrib("lv").c_str());
                for(int i=1; i<=5; ++i)
                {
                    std::stringstream strStream;
                    strStream << "item_bag_" << i;
                    if(stXML.GetAttrib(strStream.str()).empty()) break;

                    std::string itemBag = stXML.GetAttrib(strStream.str());
                    auto stIter = CLogicConfig::GetMallBagConfig().m_stCardBagInfo.find(itemBag);
                    if (stIter == CLogicConfig::GetMallBagConfig().m_stCardBagInfo.end())
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND item_bag_i:" << itemBag << " IN mallbag.xml" << std::endl;
                        throw std::exception();
                    }

                    stBonusBagList.push_back(itemBag);
                }

                m_stRewardBagsMap[strBag].insert(std::make_pair(iLevel, stBonusBagList));
            }
            stXML.OutOfElem();
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("limit_avy_reward"));
        stXML.IntoElem();
        while (stXML.FindElem("avy"))
        {
            int32_t iCardID = atoi(stXML.GetAttrib("card_id").c_str());
            CONFIG_ASSERT_EXCEPTION(iCardID > 0);

            TCardDialogLimitAvyBonus stBonus;
            stBonus.m_iAvyID = atoi(stXML.GetAttrib("avy_id").c_str());
            auto iterRef = CLogicConfig::GetActivityConfig().m_stActivityRef.find(stBonus.m_iAvyID);
            if (iterRef == CLogicConfig::GetActivityConfig().m_stActivityRef.end())
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND avy_id:" << stBonus.m_iAvyID << " IN activity_config.xml" << std::endl;
                throw std::exception();
            }

            for(int i=1; i<=5; ++i)
            {
                CPackGameItem stItem;
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("item_type" + stStream.str()).empty()) break;
                stItem.m_iItemType  = atoi(stXML.GetAttrib("item_type" + stStream.str()).c_str());
                stItem.m_iCardID    = atoi(stXML.GetAttrib("item_id" + stStream.str()).c_str());
                stItem.m_iNum       = atoi(stXML.GetAttrib("item_num" + stStream.str()).c_str());
                stBonus.m_stBonusList.push_back(stItem);
            }

            m_stLimitAvyRewardMap.insert(std::make_pair(iCardID, stBonus));
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("other_avy_reward"));
        stXML.IntoElem();
        while (stXML.FindElem("avy"))
        {
            std::string strBag = stXML.GetAttrib("bag");
            if(m_stRewardBagsMap.find(strBag) == m_stRewardBagsMap.end())
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:can not find other_avy_reward bag:" << strBag << std::endl;
                throw std::exception();
            }

            TCardDialogAvyTime stAvyTime;
            if(!try_time_from_string(stXML.GetAttrib("start_time"), stAvyTime.m_iStartTime))
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT start_time ERROR|bag:" << strBag << std::endl;
                throw std::exception();
            }
            if(!try_time_from_string(stXML.GetAttrib("end_time"), stAvyTime.m_iEndTime))
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT end_time ERROR|bag:" << strBag << std::endl;
                throw std::exception();
            }
            CONFIG_ASSERT_EXCEPTION(stAvyTime.m_iEndTime >= stAvyTime.m_iStartTime);

            stAvyTime.m_iStartDay = atoi(stXML.GetAttrib("start_day").c_str());
            stAvyTime.m_iEndDay = atoi(stXML.GetAttrib("end_day").c_str());
            CONFIG_ASSERT_EXCEPTION(stAvyTime.m_iEndDay >= stAvyTime.m_iStartDay);

            CONFIG_ASSERT_EXCEPTION(((stAvyTime.m_iStartTime != 0 && stAvyTime.m_iEndTime != 0) || (stAvyTime.m_iStartDay != 0 && stAvyTime.m_iEndDay != 0)))
            m_stOtherAvyRewardList.push_back(std::make_pair(strBag, stAvyTime));
        }
        stXML.OutOfElem();

        int32_t iCardIndex = 0;
        while (stXML.FindElem("cards"))
        {
            for(int i=1; i<=100; ++i)
            {
                std::stringstream strStream;
                strStream << "id_" << i;
                if(stXML.GetAttrib(strStream.str()).empty()) break;
                int32_t iCardID = atoi(stXML.GetAttrib(strStream.str()).c_str());

                if(m_stCardIndexMap.find(iCardID) != m_stCardIndexMap.end())
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:cards card_id repeating ERROR|card id:" << iCardID << std::endl;
                    throw std::exception();
                }
                m_stCardIndexMap[iCardID] = iCardIndex;
            }

            stXML.IntoElem();

            std::vector<std::vector<std::string>> stIndexList;
            while (stXML.FindElem("index"))
            {
                std::vector<std::string> stOptionList;
                for(int i=1; i<=30; ++i)
                {
                    std::stringstream strStream;
                    strStream << "bag_" << i;
                    if(stXML.GetAttrib(strStream.str()).empty()) break;
                    std::string strBag = stXML.GetAttrib(strStream.str());
                    if(m_stRewardBagsMap.find(strBag) == m_stRewardBagsMap.end())
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:can not find cards bag:" << strBag << std::endl;
                        throw std::exception();
                    }

                    stOptionList.push_back(strBag);
                }

                stIndexList.push_back(stOptionList);
            }

            m_stDialogRewardList.push_back(stIndexList);

            stXML.OutOfElem();
            ++iCardIndex;
        }
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const std::vector<CPackGameItem> CLogicConfigCardDialogParser::GetBonusByBagName(int32_t iLevel, std::string strBagName) const
{
    std::vector<CPackGameItem> stBonus;
    auto iterBag = m_stRewardBagsMap.find(strBagName);
    if(iterBag == m_stRewardBagsMap.end())
        return stBonus;

    auto iterLevel = iterBag->second.lower_bound(iLevel);
    if(iterLevel == iterBag->second.end())
        return stBonus;

    for(auto iter = iterLevel->second.begin(); iter != iterLevel->second.end(); ++iter)
    {
        auto stBagElem = CLogicConfig::GetMallBagConfig().GetCommodityElem(*iter);
        if(stBagElem.m_stCommodityItem.m_iCardID != 0)
            stBonus.push_back(stBagElem.m_stCommodityItem);
    }

    return stBonus;
}
