//
// Created by lxhbroser on 2020/3/10.
//

#include "logic_config_wenwen_news.h"
#include "Markup.h"
#include "server_base_protocol.h"

std::pair<bool, std::string> CLogicConfigWenWenNewsParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        if(false == stXML.FindElem("news"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        if(false == stXML.FindElem("personal_theme"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("theme"))
        {
            TLogicNewsThemeInfo info;
            info.m_iIndex = atoi(stXML.GetAttrib("index").c_str());
            info.m_iType = atoi(stXML.GetAttrib("type").c_str());
            CONFIG_ASSERT_EXCEPTION(IsValidType(info.m_iType));

            info.m_iLimitType = (int8_t)atoi(stXML.GetAttrib("limit_type").c_str());
            CONFIG_ASSERT_EXCEPTION(IsValidLimitType(info.m_iLimitType));

            info.m_iLevelMin   = atoi(stXML.GetAttrib("lv_min").c_str());
            info.m_iLevelMax   = atoi(stXML.GetAttrib("lv_max").c_str());

            stXML.IntoElem();
            while (stXML.FindElem("info"))
            {
                TLogicNewsThemeElem elem;
                elem.m_iID      = atoi(stXML.GetAttrib("id").c_str());
                elem.m_iPara1   = atoi(stXML.GetAttrib("para1").c_str());
                elem.m_iPara2   = atoi(stXML.GetAttrib("para2").c_str());

                CONFIG_ASSERT_EXCEPTION(elem.m_iPara1 >= 0);
                CONFIG_ASSERT_EXCEPTION(elem.m_iPara2 >= 0);

                info.m_stElemList.push_back(std::move(elem));
            }
            stXML.OutOfElem();

            m_stNewsThemeMap[info.m_iType] = info;
        }
        stXML.OutOfElem();

        if(false == stXML.FindElem("server_dynamic"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("dynamic"))
        {
            TLogicNewsDynamicElem elem;
            elem.m_iIndex   = atoi(stXML.GetAttrib("index").c_str());
            elem.m_iType    = atoi(stXML.GetAttrib("type").c_str());
            CONFIG_ASSERT_EXCEPTION(IsValidType(elem.m_iType));

            elem.m_iLimitType = (int8_t)atoi(stXML.GetAttrib("limit_type").c_str());
            CONFIG_ASSERT_EXCEPTION(IsValidLimitType(elem.m_iLimitType));

            elem.m_iLvMin   = atoi(stXML.GetAttrib("lv_min").c_str());
            elem.m_iLvMax   = atoi(stXML.GetAttrib("lv_max").c_str());
            elem.m_iStartDay= atoi(stXML.GetAttrib("start_day").c_str());
            elem.m_iEndDay  = atoi(stXML.GetAttrib("end_day").c_str());

            stXML.IntoElem();
            while (stXML.FindElem("info"))
            {
                int32_t id = atoi(stXML.GetAttrib("id").c_str());
                for(int i = 1; i <= 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if(stXML.GetAttrib("para" + stStream.str()).empty()) break;

                    std::string iPara = stXML.GetAttrib("para" + stStream.str());

                    elem.m_iInfo[id].push_back(iPara);
                }
            }
            stXML.OutOfElem();
            m_stNewsDynamicMap[elem.m_iIndex] = elem;
        }
        stXML.OutOfElem();


        if(false == stXML.FindElem("choose_reward"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("choose_id"))
        {
            TLogicNewsChooseReward elem;
            elem.m_iChooseID = atoi(stXML.GetAttrib("id").c_str());

            for(int i = 1; i <= 10; ++i)
            {
                CPackGameItem stGameItem;
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;
                stGameItem.m_iItemType = atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stGameItem.m_iCardID = atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stGameItem.m_iNum = atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());

                elem.m_stReward.push_back(stGameItem);
            }

            elem.m_iStEmail.m_strTitle = stXML.GetAttrib("title");
            elem.m_iStEmail.m_strContent = stXML.GetAttrib("content");
            elem.m_iStEmail.m_strSender = stXML.GetAttrib("sender");

            m_stNewsChooseMap[elem.m_iChooseID] = elem;
        }
        stXML.OutOfElem();

    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicNewsChooseReward *CLogicConfigWenWenNewsParser::GetWenWenNewsChooseConfig(int32_t m_iChoseID) const
{
    auto iterChooseConfig = m_stNewsChooseMap.find(m_iChoseID);
    return iterChooseConfig == m_stNewsChooseMap.end() ? nullptr : &(iterChooseConfig->second);
}