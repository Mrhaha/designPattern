//
// Created by czy on 2022/8/8.
//
#include "logic_config_gashapon.h"
#include "logic_config.h"
#include "Markup.h"
#include "common_tools.h"

std::pair<bool, std::string> CLogicConfigGashaponParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("avy_id"), m_iAvyID));
        CONFIG_ASSERT_EXCEPTION_EX(m_iAvyID > 0,m_iAvyID);
        CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetAvyTimeConfig().GetConfig(m_iAvyID))

        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("crush_boss_count"), m_iCrushBossCount));

        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("chapter_avy_id"), m_iChapterAvyID));
        CONFIG_ASSERT_EXCEPTION_EX(m_iChapterAvyID > 0,m_iChapterAvyID);
        CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetAvyTimeConfig().GetConfig(m_iChapterAvyID))

        m_strMailSender = stXML.GetAttrib("email_sender");
        m_strMailContent = stXML.GetAttrib("email_content");
        m_strMailTitle = stXML.GetAttrib("email_title");

        for(int i = 1; i <= 10; ++i)
        {
            std::stringstream stStream;
            stStream << i;
            if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), stItem.m_iCardID)

            m_stMailReward.emplace_back(stItem);
        }


        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("gash_info"))
        stXML.IntoElem();
        while(stXML.FindElem("gash"))
        {
            TLogicGashInfo gashInfo;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), gashInfo.m_iGashID));

            stXML.IntoElem();
            CONFIG_ASSERT_EXCEPTION (stXML.FindElem("info"))
            for(int i = 1; i <= 15; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("consume_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("consume_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("consume_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("consume_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, gashInfo.m_iGashID);
                gashInfo.m_vecConsumeItem.emplace_back(stItem);
            }

            CONFIG_ASSERT_EXCEPTION (stXML.FindElem("reward_list"))
            stXML.IntoElem();
            while(stXML.FindElem("index"))
            {
                TLogicGashRewardInfo gashRewardInfo;
                CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("index"), gashRewardInfo.m_iIndex))
                CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("type"), gashRewardInfo.m_stItem.m_iItemType))
                CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), gashRewardInfo.m_stItem.m_iCardID))
                CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("num"), gashRewardInfo.m_stItem.m_iNum))
                CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("weight"), gashRewardInfo.m_iWeight))
                CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("total_num"), gashRewardInfo.m_iTotalNum))
                gashRewardInfo.m_strGrade = stXML.GetAttrib("tag");
                gashInfo.m_mapGashRewardInfo[gashRewardInfo.m_iIndex] = gashRewardInfo;
                if (gashRewardInfo.m_strGrade == "A")
                {
                    m_uMapGashapon2Index[gashInfo.m_iGashID].push_back(gashRewardInfo.m_iIndex);
                }
            }
            stXML.OutOfElem();

            m_uMapGashInfo[gashInfo.m_iGashID] = gashInfo;
            stXML.OutOfElem();
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("refresh_weight"))
        stXML.IntoElem();
        while(stXML.FindElem("leve_id"))
        {
            TLogicLevelBossPro levelElem;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), levelElem.m_iLevelID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("pro"), levelElem.m_iCrushBossPro));
            CONFIG_ASSERT_EXCEPTION(levelElem.m_iCrushBossPro >= 0 && levelElem.m_iCrushBossPro <= CLogicConfigDefine::ESD_RandomWeight)

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("weight" + stStream.str()).empty()) break;
                int32_t weight = 0;
                int32_t grade = 0;
                CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("grade" + stStream.str()), grade));
                CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("weight" + stStream.str()), weight));
                CONFIG_ASSERT_EXCEPTION(weight >= 0 && weight <= CLogicConfigDefine::ESD_RandomWeight)
                CONFIG_ASSERT_EXCEPTION(grade >= 0 && levelElem.m_uMapGradeWeight.find(grade) == levelElem.m_uMapGradeWeight.end())
                levelElem.m_uMapGradeWeight[grade] = weight;
            }
            CONFIG_ASSERT_EXCEPTION_EX(m_mapLeveID2Weight.find(levelElem.m_iLevelID) == m_mapLeveID2Weight.end(),levelElem.m_iLevelID)
            m_mapLeveID2Weight.insert(std::make_pair(levelElem.m_iLevelID,levelElem));
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("crush_boss"))
        stXML.IntoElem();
        while(stXML.FindElem("level_id"))
        {
            TLogicBossInfo bossInfo;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), bossInfo.m_iLevelID))
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("boss_hp"), bossInfo.m_iBossHp))
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("grade"), bossInfo.m_iGrade))
            CONFIG_ASSERT_EXCEPTION(m_uMapBossLevelToBossInfo.find(bossInfo.m_iLevelID) == m_uMapBossLevelToBossInfo.end())

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItemRandomNum stItem;
                stItem.m_stGameItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_stGameItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_stGameItem.m_iNum = 1;
                std::string strStarList = (stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                auto stStarVec = string_split(strStarList, "|");
                for(std::size_t j = 0; j < stStarVec.size(); ++j)
                {
                    int num = ::atoi(stStarVec[j].c_str());
                    stItem.m_iNumRange.push_back(num);
                }

                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem.m_stGameItem), bossInfo.m_iLevelID)
                bossInfo.m_stRewardItem.emplace_back(stItem);
                m_uMapBossReward[bossInfo.m_iLevelID].emplace_back(stItem);
            }
            if (m_uMapBossInfo.find(bossInfo.m_iGrade) == m_uMapBossInfo.end())
            {
                std::vector<TLogicBossInfo> bossInfos;
                bossInfos.emplace_back(bossInfo);
                m_uMapBossInfo.insert(std::make_pair(bossInfo.m_iGrade,bossInfos));
            } else
            {
                m_uMapBossInfo[bossInfo.m_iGrade].emplace_back(bossInfo);
            }
            m_uMapBossLevelToBossInfo[bossInfo.m_iLevelID] = bossInfo;
        }
        stXML.OutOfElem();

        stXML.OutOfElem();
    } catch (const std::exception& stEx) {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
    return (std::pair<bool, std::string>(true, "OK"));
}

