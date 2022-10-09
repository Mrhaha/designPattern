//
// Created by czy on 2022/5/5.
//
#include "logic_config_new_pass.h"
#include "logic_config.h"
#include "Markup.h"

std::pair<bool, std::string> CLogicConfigLimitBossParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("baodi_time"), m_iBaodiTime));
        CONFIG_ASSERT_EXCEPTION_EX(m_iBaodiTime > 0,m_iBaodiTime);
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("avy_id"), m_iAcyID));
        CONFIG_ASSERT_EXCEPTION_EX(m_iAcyID > 0,m_iAcyID);
        CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetAvyTimeConfig().GetConfig(m_iAcyID))

        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("point_item_id"), m_iPointItemID));
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("limit_time"), m_iLimitTime));
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("invite_friend_num"), m_iInviteFriendNum));

        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("boss_number_limit"), m_iBossLimitNumber));
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("daily_get_reward_count"), m_iDailyGetRewardCount));

        std::string strDate;
        strDate = stXML.GetAttrib("start_time");
        if (!strDate.empty())
        {
            struct tm stTM = {};
            CONFIG_ASSERT_EXCEPTION(strptime(strDate.c_str(), "%Y-%m-%d %T", &stTM) != nullptr)
            m_iLevelStartTime = (int)mktime(&stTM);
        }

        strDate.clear();
        strDate = stXML.GetAttrib("end_time");
        if (!strDate.empty())
        {
            struct tm stTM = {};
            CONFIG_ASSERT_EXCEPTION(strptime(strDate.c_str(), "%Y-%m-%d %T", &stTM) != nullptr)
            m_iLevelEndTime = (int)mktime(&stTM);
        }

        m_strSender = stXML.GetAttrib("sender");
        m_strTitle = stXML.GetAttrib("title");
        m_strContent = stXML.GetAttrib("content");

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("refresh_weight"))
        stXML.IntoElem();

        while(stXML.FindElem("leve_id"))
        {
            TLogicLimitBossWeightElem levelElem;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), levelElem.m_iLevelID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("weight"), levelElem.m_iWeight));
            m_mapLeveID2Weight[levelElem.m_iLevelID] = levelElem.m_iWeight;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("limit_boss"))
        stXML.IntoElem();
        while(stXML.FindElem("level_id"))
        {
            TLogicLimitBossElem levelElem;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), levelElem.m_iLevelID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("boss_hp"), levelElem.m_iTotalHP));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("weight"), levelElem.m_iWeight));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("physical_consume"), levelElem.m_iPhysicalConsume));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("hp_reward_score"), levelElem.m_iHpRewardScore));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("find_reward_score"), levelElem.m_iFindRewardScore));

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, levelElem.m_iLevelID);
                levelElem.m_stRewardItem.emplace_back(stItem);
            }
            m_mapLevelID2Boss[levelElem.m_iLevelID] = levelElem;
        }
        stXML.OutOfElem();

        stXML.OutOfElem();
    } catch (const std::exception& stEx) {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
    return (std::pair<bool, std::string>(true, "OK"));
}

int32_t CLogicConfigLimitBossParser::GetLevelRefreshWeight(int32_t levelID) const
{
    auto iterLevelRefresh = m_mapLeveID2Weight.find(levelID);
    return iterLevelRefresh == m_mapLeveID2Weight.end() ? 0 : iterLevelRefresh->second;
}

const TLogicLimitBossElem* CLogicConfigLimitBossParser::GetLimitBossConfigByLevelID(int32_t levelID) const
{
    auto iterLimitBossConfig = m_mapLevelID2Boss.find(levelID);
    return iterLimitBossConfig == m_mapLevelID2Boss.end() ? nullptr : &(iterLimitBossConfig->second);
}
