//
// Created by czy on 2022/9/6.
//
#include "logic_config_atlas.h"
#include "logic_config_base.h"
#include "logic_config.h"
#include "common_tools.h"
#include "Markup.h"

std::pair<bool, std::string> CLogicConfigCompleteAtlasParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("root"));
        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));

        m_strCardBag = stXML.GetAttrib("card_bag");
        CONFIG_ASSERT_EXCEPTION(!m_strCardBag.empty());

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("area_info"))
        stXML.IntoElem();
        while(stXML.FindElem("buff"))
        {
            TLogicAreaInfo areaInfo;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), areaInfo.m_iBuffID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("level_up_param"), areaInfo.m_iLevelParam));

            std::string strCardList = stXML.GetAttrib("card_id");
            auto stCardVec = string_split(strCardList, "|");
            for(std::size_t i = 0; i < stCardVec.size(); ++i)
            {
                int iCardID = ::atoi(stCardVec[i].c_str());
                CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::GetHeroCardConfig().GetConfig(iCardID),areaInfo.m_iBuffID,iCardID);
                areaInfo.m_vecCardID.push_back(iCardID);
                CONFIG_ASSERT_EXCEPTION_EXEX(!areaInfo.m_vecCardID.empty(),areaInfo.m_iBuffID,iCardID);
            }

            std::string strBuffList = stXML.GetAttrib("buff_param");
            auto stBuffVec = string_split(strBuffList, "|");
            for(std::size_t i = 0; i < stBuffVec.size(); ++i)
            {
                float iBuffParam = ::atof(stBuffVec[i].c_str());
                areaInfo.m_vecBuffParam.push_back(iBuffParam);
                CONFIG_ASSERT_EXCEPTION_EXEX(!areaInfo.m_vecBuffParam.empty(),areaInfo.m_iBuffID,iBuffParam);
            }
            CONFIG_ASSERT_EXCEPTION(m_mapBuffID2Config.find(areaInfo.m_iBuffID) == m_mapBuffID2Config.end());
            m_mapBuffID2Config[areaInfo.m_iBuffID] = areaInfo;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("area_level"))
        stXML.IntoElem();
        while(stXML.FindElem("level_up"))
        {
            TLogicAreaLevelup levelUpInfo;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("level"), levelUpInfo.m_iLevel));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("need_star"), levelUpInfo.m_iNeedStar));
            m_mapLevelUpInfo[levelUpInfo.m_iLevel] = levelUpInfo;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("complete_atlas"))
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("avy_id"), m_iAvyID))

        stXML.IntoElem();
        while(stXML.FindElem("hero_card"))
        {
            int32_t iCardID;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), iCardID));
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetHeroCardConfig().GetConfig(iCardID),iCardID);
            m_vecCompleteCard.emplace_back(iCardID);
        }
        stXML.OutOfElem();

        stXML.OutOfElem();
    } catch (const std::exception& stEx) {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
    return (std::pair<bool, std::string>(true, "OK"));
}

