//
// Created by czy on 2022/4/25.
//
#include "logic_config_new_pass.h"
#include "logic_config.h"
#include "Markup.h"

std::pair<bool, std::string> CLogicConfigNewPassParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("avy_id"), m_iActID));
        CONFIG_ASSERT_EXCEPTION_EX(m_iActID >=0,m_iActID);
        CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetAvyTimeConfig().GetConfig(m_iActID))

        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("deposit_id"), m_iDepositID));
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("up_deposit_id"), m_iUpDepositID));
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("advanced_deposit_id"), m_iAdvancedDepositID));

        m_strSender = stXML.GetAttrib("sender");
        m_strTitle = stXML.GetAttrib("title");
        m_strContent = stXML.GetAttrib("content");

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("level_up_exp"))
        stXML.IntoElem();

        while(stXML.FindElem("exp"))
        {
            TLogicLevelExpElem expElem;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("level"), expElem.m_iLevel));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("exp_value"), expElem.m_iNeedExp));
            m_mapLevel2Exp[expElem.m_iLevel] = expElem.m_iNeedExp;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("normal_reward"))
        stXML.IntoElem();
        while(stXML.FindElem("reward"))
        {
            TLogicNewPassRewardElem rewardElem;
            rewardElem.m_iLevel = ::atoi(stXML.GetAttrib("level").c_str());
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, rewardElem.m_iLevel);
                rewardElem.m_stReward.push_back(stItem);

                m_mapLevel2Normal[rewardElem.m_iLevel] = rewardElem;
            }
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("advanced_reward"))
        stXML.IntoElem();
        while(stXML.FindElem("reward"))
        {
            TLogicNewPassRewardElem rewardElem;
            rewardElem.m_iLevel = ::atoi(stXML.GetAttrib("level").c_str());
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, rewardElem.m_iLevel);
                rewardElem.m_stReward.push_back(stItem);


                m_mapLevel2Advanced[rewardElem.m_iLevel] = rewardElem;
            }
        }
        stXML.OutOfElem();

        stXML.OutOfElem();
    } catch (const std::exception& stEx) {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
    return (std::pair<bool, std::string>(true, "OK"));
}

//获取玩家通行证经验对应的等级
int32_t CLogicConfigNewPassParser::GetNewPassLevelByExp(int32_t currExp) const
{
    int32_t totalNeedExp = 0;
    int32_t currNewPassLevel = 0;

    auto iterLevelNeedExp = m_mapLevel2Exp.begin();
    for(; iterLevelNeedExp != m_mapLevel2Exp.end(); iterLevelNeedExp++)
    {
        totalNeedExp += iterLevelNeedExp->second;
        if (totalNeedExp > currExp)
        {
            currNewPassLevel = iterLevelNeedExp->first - 1;
            break;
        }
    }

    if (iterLevelNeedExp == m_mapLevel2Exp.end())
    {
        currNewPassLevel = (--m_mapLevel2Exp.end())->first;
    }

    return currNewPassLevel;
}

const TLogicNewPassRewardElem* CLogicConfigNewPassParser::GetNormalRewardByLevel(int32_t level) const
{
    auto iterReward = m_mapLevel2Normal.find(level);
    return iterReward == m_mapLevel2Normal.end() ? nullptr : &(iterReward->second);
}

const TLogicNewPassRewardElem* CLogicConfigNewPassParser::GetAdvancedRewardByLevel(int32_t level) const
{
    auto iterReward = m_mapLevel2Advanced.find(level);
    return iterReward == m_mapLevel2Advanced.end() ? nullptr : &(iterReward->second);
}