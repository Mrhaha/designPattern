//
// Created by czy on 2022/4/25.
//
#include "logic_config_rune.h"
#include "logic_config.h"
#include "Markup.h"
#include "common_tools.h"

std::pair<bool, std::string> CLogicConfigRuneParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("rune"));
        stXML.IntoElem();

        while(stXML.FindElem("rune"))
        {
            stXML.IntoElem();
            TLogicRuneElem runeElem;
            CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("unlock_item"), runeElem.m_iLockItemID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("upgrade_id"), runeElem.m_iUpgradeID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("item_id"), runeElem.m_iRuneID));
            stXML.OutOfElem();
            m_stRuneMap[runeElem.m_iRuneID] = runeElem;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("upgrade_id"))
        stXML.IntoElem();
        while(stXML.FindElem("upgrade_index"))
        {
            TLogicRuneUpGradeElem upGradeElem;
            upGradeElem.m_iUpGradeID = ::atoi(stXML.GetAttrib("id").c_str());
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("unlock_type" + stStream.str()).empty()) break;

                TLogicUnLockElem conditionItem;
                conditionItem.m_iUnlockType = ::atoi(stXML.GetAttrib("unlock_type" + stStream.str()).c_str());

                std::string strParaList = stXML.GetAttrib("unlock_para" + stStream.str());
                auto stParaVec = string_split(strParaList, "|");
                for(std::size_t j = 0; j < stParaVec.size(); ++j)
                {
                    int iPara = ::atoi(stParaVec[j].c_str());
                    conditionItem.m_iUnlockPara.push_back(iPara);
                }

                std::string strParaExList = stXML.GetAttrib("unlock_para_ex" + stStream.str());
                auto stParaExVec = string_split(strParaExList, "|");
                for(std::size_t j = 0; j < stParaExVec.size(); ++j)
                {
                    int iParaEx = ::atoi(stParaExVec[j].c_str());
                    conditionItem.m_iUnlockParaEx.push_back(iParaEx);
                }
                upGradeElem.m_stUnlock[i] = conditionItem;
            }
            m_stRuneUpgradeMap[upGradeElem.m_iUpGradeID] = upGradeElem;
        }
        stXML.OutOfElem();

        stXML.OutOfElem();
    } catch (const std::exception& stEx) {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicRuneElem* CLogicConfigRuneParser::GetRuneElemByID(int32_t runeID) const
{
    auto iter = m_stRuneMap.find(runeID);
    return iter == m_stRuneMap.end() ? nullptr : &(iter->second);
}


const TLogicRuneUpGradeElem* CLogicConfigRuneParser::GetRuneUpgradeByID(int32_t runeID) const
{
    auto iter = m_stRuneUpgradeMap.find(runeID);
    return iter == m_stRuneUpgradeMap.end() ? nullptr : &(iter->second);
}