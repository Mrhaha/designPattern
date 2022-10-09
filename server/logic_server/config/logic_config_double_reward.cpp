//
// Created by czy on 2022/7/19
//
#include "logic_config_double_reward.h"
#include "Markup.h"
#include "common_tools.h"

std::pair<bool, std::string> CLogicConfigDoubleRewardParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("double_reward"))
        stXML.IntoElem();
        while(stXML.FindElem("double_index"))
        {
            TLogicDoubleReward doubleReward;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), doubleReward.m_iIndex));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("level_type"), doubleReward.m_iLevelType));
            std::string strParaList = stXML.GetAttrib("avy_time");
            auto stParaVec = string_split(strParaList, "|");
            for(std::size_t j = 0; j < stParaVec.size(); ++j)
            {
                int iAvyTimeID = ::atoi(stParaVec[j].c_str());
                doubleReward.m_iAvy_time.push_back(iAvyTimeID);
            }
            m_stDoubleRewardConfig[doubleReward.m_iLevelType] = doubleReward;
        }
        stXML.OutOfElem();

        stXML.OutOfElem();
    } catch (const std::exception& stEx) {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicDoubleReward* CLogicConfigDoubleRewardParser::GetDoubleRewardConfig(int32_t iLevelType) const
{
    auto iter = m_stDoubleRewardConfig.find(iLevelType);
    return iter == m_stDoubleRewardConfig.end() ? nullptr : &(iter->second);
}