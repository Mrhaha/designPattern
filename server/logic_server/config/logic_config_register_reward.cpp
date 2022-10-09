//
// Created by czy on 2022/7/19
//
#include "logic_config_register_reward.h"
#include "Markup.h"
#include "common_tools.h"

std::pair<bool, std::string> CLogicConfigRegisterRewardParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"))
        stXML.IntoElem();


        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("info"))

        m_iAvyID =  atoi(stXML.GetAttrib("avy_id").c_str());
        m_strSender = stXML.GetAttrib("sender");
        m_strTitle = stXML.GetAttrib("title");
        m_strContent = stXML.GetAttrib("content");
        for(int i = 1; i <= 10; ++i)
        {
            std::stringstream stStream;
            stStream << i;
            if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, m_iAvyID);
            m_stReward.emplace_back(stItem);
        }

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("cbt_reward"))
        stXML.IntoElem();

        while(stXML.FindElem("account"))
        {
            std::string registerAcc = stXML.GetData();
            if (!registerAcc.empty())
            {
                m_mapAccount.insert(std::make_pair(registerAcc, true));
            }
        }
        stXML.OutOfElem();
    } catch (const std::exception& stEx) {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
    return (std::pair<bool, std::string>(true, "OK"));
}

