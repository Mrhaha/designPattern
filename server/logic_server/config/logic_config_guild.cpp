#include "logic_config_guild.h"
#include "logic_config.h"
#include "logic_link_common.h"
#include "Markup.h"
#include "common_datetime.h"

std::pair<bool, std::string>
CLogicConfigGuildParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (!stXML.FindElem("root"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (!stXML.FindElem("guild_info"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

        m_iRankID = ::atoi(stXML.GetAttrib("rank_id").c_str());

        std::string strCalDate;
        strCalDate = stXML.GetAttrib("calc_time");
        if (!strCalDate.empty())
        {
            struct tm stTM = {};
            CONFIG_ASSERT_EXCEPTION(strptime(strCalDate.c_str(), "%Y-%m-%d %T", &stTM) != nullptr)
            m_iGuildWarCalTime = (int)mktime(&stTM);
        }


        std::string strStartDate;
        strStartDate = stXML.GetAttrib("opening_para1");
        if (!strStartDate.empty())
        {
            struct tm stTM = {};
            CONFIG_ASSERT_EXCEPTION(strptime(strStartDate.c_str(), "%Y-%m-%d %T", &stTM) != nullptr)
            m_iGuildWarStartTime = (int)mktime(&stTM);
        }

        std::string strEndDate;
        strEndDate = stXML.GetAttrib("opening_para2");
        if (!strEndDate.empty())
        {
            struct tm stTM = {};
            CONFIG_ASSERT_EXCEPTION(strptime(strEndDate.c_str(), "%Y-%m-%d %T", &stTM) != nullptr)
            m_iGuildWarEndTime = (int)mktime(&stTM);
        }

        m_iDailyGuildWarCount = atoi(stXML.GetAttrib("daily_guild_war_count").c_str());
        m_iDailyRandomCount = atoi(stXML.GetAttrib("daily_random_count").c_str());
		m_iCreateCost = atoi(stXML.GetAttrib("create_cost").c_str());
		m_iCreateLevel = atoi(stXML.GetAttrib("create_level").c_str());
		m_iUpdateBulletinLevel = atoi(stXML.GetAttrib("update_bulletin_level").c_str());
		if (m_iUpdateBulletinLevel <= 0)
		{
			m_iUpdateBulletinLevel = 30;
		}

		m_iJoinGuildCD = atoi(stXML.GetAttrib("join_guild_cd").c_str());
		if (m_iJoinGuildCD <= 0) m_iJoinGuildCD = 3600;
        m_iChangeNameCost = atoi(stXML.GetAttrib("change_name_cost").c_str());

        m_strBossMailSender = stXML.GetAttrib("boss_mail_sender");
        m_strBossMailTitle = stXML.GetAttrib("boss_mail_title");
        m_strBossMailContent = stXML.GetAttrib("boss_mail_content");

        m_iWishSwapTimes = atoi(stXML.GetAttrib("wish_swap_times").c_str());

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_level"))
        stXML.IntoElem();
        while (stXML.FindElem("level"))
        {
            TLogicGuildLevelNum stLevelNum;
            stLevelNum.m_iLevel = atoi(stXML.GetAttrib("level").c_str());
            stLevelNum.m_iNextExp = atoi(stXML.GetAttrib("nextexp").c_str());
            stLevelNum.m_iMember = atoi(stXML.GetAttrib("member").c_str());
            stLevelNum.m_iAdminMember = atoi(stXML.GetAttrib("admin_count").c_str());
            m_stGuildNum[stLevelNum.m_iLevel] = stLevelNum;
        }
        stXML.OutOfElem();

        for(int i = 1; i <= 10; ++i)
        {
            std::stringstream stStream;
            stStream << i;
            if(stXML.GetAttrib("first_award_type" + stStream.str()).empty()) break;

            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("first_award_type" + stStream.str()).c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("first_award_id" + stStream.str()).c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("first_award_num" + stStream.str()).c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfig::IsValidGameItem(stItem))

            m_stBossFirstAwards.emplace_back(stItem);
        }

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_wish"))
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int iGrade = atoi(stXML.GetAttrib("card_grade").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardGradeType(iGrade), iGrade)
            CONFIG_ASSERT_EXCEPTION_EX(m_stWishMap.find(iGrade) == m_stWishMap.end(), iGrade)

            TLogicGuildWish stElem;
            stElem.m_iWishCount = atoi(stXML.GetAttrib("wish_count").c_str());
            stElem.m_iSwapCount = atoi(stXML.GetAttrib("swap_count").c_str());
            stElem.m_iRewardGuildCoin = atoi(stXML.GetAttrib("reward_guild_coin").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iWishCount > 0 && stElem.m_iWishCount <= E_MaxWishCount && stElem.m_iSwapCount >= 0, iGrade)
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iRewardGuildCoin >= 0, iGrade)

            m_stWishMap[iGrade] = stElem;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_boss_person"))
        stXML.IntoElem();
        while (stXML.FindElem("week"))
        {
            int iWeek = atoi(stXML.GetAttrib("value").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(0 < iWeek && iWeek < 8, iWeek)

            if(iWeek == 7) iWeek = 0;

            stXML.IntoElem();
            while (stXML.FindElem("index"))
            {
                uint64_t ulHurt = atoll(stXML.GetAttrib("hurt").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(ulHurt > 0, iWeek)

                stXML.IntoElem();
                while (stXML.FindElem("level"))
                {
                    int iLevel = atoi(stXML.GetAttrib("role_level").c_str());
                    TLogicGuildBossPersonReward stReward;
                    stReward.m_strCardBag = stXML.GetAttrib("card_bag");
                    if(!stReward.m_strCardBag.empty())
                    {
                        CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(stReward.m_strCardBag), ulHurt)
                    }

                    for(int i = 1; i <= 10; ++i)
                    {
                        auto strIndex = std::to_string(i);
                        if(stXML.GetAttrib("type" + strIndex).empty()) break;

                        TLogicGuildBossItem stItem;
                        stItem.m_iType = atoi(stXML.GetAttrib("type" + strIndex).c_str());
                        stItem.m_iID = atoi(stXML.GetAttrib("id" + strIndex).c_str());
                        stItem.m_iNumMin = atoi(stXML.GetAttrib("num_min" + strIndex).c_str());
                        stItem.m_iNumMax = atoi(stXML.GetAttrib("num_max" + strIndex).c_str());
                        CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem.m_iType, stItem.m_iID, stItem.m_iNumMin), ulHurt)
                        CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iNumMin <= stItem.m_iNumMax, ulHurt)

                        stReward.m_stRewards.push_back(stItem);
                    }
                    stReward.m_iRewardTimes = atoi(stXML.GetAttrib("reward_times").c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(stReward.m_iRewardTimes >= 0, iLevel)

                    m_stBossPerson[iWeek][ulHurt][iLevel] = std::move(stReward);
                }
                stXML.OutOfElem();

                CONFIG_ASSERT_EXCEPTION_EX(!m_stBossPerson[iWeek][ulHurt].empty(), iWeek)
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION_EX(!m_stBossPerson[iWeek].empty(), iWeek)
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_boss_all"))
        stXML.IntoElem();
        while (stXML.FindElem("week"))
        {
            int iWeek = atoi(stXML.GetAttrib("value").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(0 < iWeek && iWeek < 8, iWeek)

            if(iWeek == 7) iWeek = 0;

            stXML.IntoElem();
            while (stXML.FindElem("index"))
            {
                uint64_t ulHurt = atoll(stXML.GetAttrib("hurt").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(ulHurt > 0, iWeek)

                TLogicGuildBossReward stElem;
                for(int i = 1; i <= 10; ++i)
                {
                    auto strIndex = std::to_string(i);
                    if(stXML.GetAttrib("reward_type" + strIndex).empty()) break;

                    CPackGameItem stItem;
                    stItem.m_iItemType = atoi(stXML.GetAttrib("reward_type" + strIndex).c_str());
                    stItem.m_iCardID = atoi(stXML.GetAttrib("reward_id" + strIndex).c_str());
                    stItem.m_iNum = atoi(stXML.GetAttrib("reward_num" + strIndex).c_str());
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), iWeek)

                    stElem.m_stRewards.emplace_back(stItem);
                }

                m_stBossAll[iWeek][ulHurt] = std::move(stElem);
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION_EX(!m_stBossAll[iWeek].empty(), iWeek)
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_donate"))
        stXML.IntoElem();
        while(stXML.FindElem("donate_type"))
        {
            TLogicGuildDonateElem donateElem;
            donateElem.m_iDonateType = atoi(stXML.GetAttrib("type").c_str());
            donateElem.m_iPrivateDonate = atoi(stXML.GetAttrib("private_donate").c_str());
            donateElem.m_iGuildDonate = atoi(stXML.GetAttrib("guild_donate").c_str());

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("consume_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("consume_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("consume_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("consume_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iCardID);
                donateElem.m_stConsumeItem.emplace_back(stItem);
            }

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iCardID);
                donateElem.m_stRewardItem.emplace_back(stItem);
            }
            m_stGuildDonate[donateElem.m_iDonateType] = donateElem;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_sign"))
        stXML.IntoElem();
        while(stXML.FindElem("sign_reward"))
        {
            TLogicGuildSignRewardElem signRewardElem;
            signRewardElem.m_iLevel = atoi(stXML.GetAttrib("level").c_str());

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("card_bag" + stStream.str()).empty()) break;

                TLogicGuildSignBag stItem;
                stItem.m_strBagName = stXML.GetAttrib("card_bag" + stStream.str()).c_str();
                stItem.m_iNum = ::atoi(stXML.GetAttrib("card_bag_num" + stStream.str()).c_str());
                signRewardElem.m_stReward.emplace_back(stItem);
            }
            m_stSignReward[signRewardElem.m_iLevel] = signRewardElem;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_random_chat"))
        stXML.IntoElem();
        while(stXML.FindElem("random_chat"))
        {
            TLogicGuildRandomChatElem randomChatElem;
            auto index = ::atoi(stXML.GetAttrib("index").c_str());
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iCardID);
                randomChatElem.m_stReward.emplace_back(stItem);
            }
            m_stChatReward[index] = randomChatElem;
        }
        stXML.OutOfElem();


        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_random_pk"))
        stXML.IntoElem();
        while(stXML.FindElem("random_pk"))
        {
            TLogicGuildRandomPkElem randomPkElem;
            auto index = ::atoi(stXML.GetAttrib("index").c_str());
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iCardID);
                randomPkElem.m_stWinReward.emplace_back(stItem);
            }

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("lose_reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("lose_reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("lose_reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("lose_reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iCardID);
                randomPkElem.m_stLoseReward.emplace_back(stItem);
            }
            m_stPkReward[index] = randomPkElem;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_exchange_pool"))
        stXML.IntoElem();
        while(stXML.FindElem("random_exchange"))
        {
            TLogicGuildRandomExchangeElem randomExchangeElem;
            auto index = ::atoi(stXML.GetAttrib("index").c_str());
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iCardID);
                randomExchangeElem.m_stReward.emplace_back(stItem);
            }

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("consume_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("consume_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("consume_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("consume_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iCardID);
                randomExchangeElem.m_stConsume.emplace_back(stItem);
            }
            m_stExchangeReward[index] = randomExchangeElem;
        }
        stXML.OutOfElem();


        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_collection"))
        stXML.IntoElem();
        while(stXML.FindElem("collection_item"))
        {
            TLogicGuildCollection guildCollection;

            guildCollection.m_iItemID = ::atoi(stXML.GetAttrib("id").c_str());
            guildCollection.m_iBuffID = ::atoi(stXML.GetAttrib("guild_buff_id").c_str());

            m_stGuildUnlockItemBuff[guildCollection.m_iItemID] = guildCollection;
        }
        stXML.OutOfElem();


        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_bar_reward"))
        m_stGuildBarConfig.m_iSpecialCardID = ::atoi(stXML.GetAttrib("special_card_id").c_str());
        m_stGuildBarConfig.m_iCostTime = ::atoi(stXML.GetAttrib("cost_time").c_str());
        for(int i = 1; i <= 10; ++i)
        {
            std::stringstream stStream;
            stStream << i;
            if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iCardID);
            m_stGuildBarConfig.m_stReward.emplace_back(stItem);
        }

        for(int i = 1; i <= 10; ++i)
        {
            std::stringstream stStream;
            stStream << i;
            if(stXML.GetAttrib("special_reward_type" + stStream.str()).empty()) break;

            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("special_reward_type" + stStream.str()).c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("special_reward_id" + stStream.str()).c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("special_reward_num" + stStream.str()).c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iCardID);
            m_stGuildBarConfig.m_stSpecialReward.emplace_back(stItem);
        }

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_war_reward"))
        stXML.IntoElem();
        while (stXML.FindElem("rank"))
        {
            TLogicGuildWarRankReward guildWarRankReward;
            guildWarRankReward.m_id = atoi(stXML.GetAttrib("number").c_str());
            guildWarRankReward.m_iMin = atoi(stXML.GetAttrib("min").c_str());
            guildWarRankReward.m_iMax = atoi(stXML.GetAttrib("max").c_str());
            guildWarRankReward.m_strGuildWarSender = stXML.GetAttrib("mail_sender").c_str();
            guildWarRankReward.m_strGuildWarTitle = stXML.GetAttrib("mail_title").c_str();

            std::string strMailContentList = stXML.GetAttrib("mail_content");
            auto stContentVec = string_split(strMailContentList, "|");

            guildWarRankReward.m_strGuildWarContent = stContentVec[0];
            guildWarRankReward.m_strGuildWarAfterContent = stContentVec.size() == 2 ? stContentVec[1] : "";

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, guildWarRankReward.m_id);
                guildWarRankReward.m_stReward.emplace_back(stItem);
            }
            m_stGuildWarRankReward[guildWarRankReward.m_id] = guildWarRankReward;
        }
        CONFIG_ASSERT_EXCEPTION(!m_stGuildWarRankReward.empty())
        stXML.OutOfElem();

	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicGuildBossPersonReward* CLogicConfigGuildParser::GetBossPersonReward(int iWeek, uint64_t ulHurt, int iLevel) const
{
    auto iter = m_stBossPerson.find(iWeek);
    if(iter == m_stBossPerson.end()) return nullptr;

    auto iterReward = iter->second.upper_bound(ulHurt);
    // 伤害量小于第一条配置，没有奖励
    if(iterReward == iter->second.begin()) return nullptr;

    --iterReward;

    auto iterConfig = iterReward->second.upper_bound(iLevel);
    if(iterConfig != iterReward->second.begin())
        --iterConfig;

    return &(iterConfig->second);
}

const TLogicGuildBossReward* CLogicConfigGuildParser::GetBossAllReward(int iWeek, uint64_t ulHurt) const
{
    auto iter = m_stBossAll.find(iWeek);
    if(iter == m_stBossAll.end()) return nullptr;

    auto iterReward = iter->second.upper_bound(ulHurt);
    if(iterReward != iter->second.begin())
        --iterReward;

    return &(iterReward->second);
}

const TLogicGuildWish* CLogicConfigGuildParser::GetWishConfig(int iGrade) const
{
    auto iter = m_stWishMap.find(iGrade);
    return iter == m_stWishMap.end() ? nullptr : &(iter->second);
}

const TLogicGuildDonateElem* CLogicConfigGuildParser::GetDonateConfig(int32_t donateType) const
{
    auto iter = m_stGuildDonate.find(donateType);
    return iter == m_stGuildDonate.end() ? nullptr : &(iter->second);
}

const TLogicGuildSignRewardElem* CLogicConfigGuildParser::GetSignRewardConfig(int32_t level) const
{
    auto iter = m_stSignReward.find(level);
    return iter == m_stSignReward.end() ? nullptr : &(iter->second);
}

const TLogicGuildRandomChatElem* CLogicConfigGuildParser::GetChatRewardConfig(int32_t index) const
{
    auto iter = m_stChatReward.find(index);
    return iter == m_stChatReward.end() ? nullptr : &(iter->second);
}

const TLogicGuildRandomPkElem* CLogicConfigGuildParser::GetPkConfig(int32_t index) const
{
    auto iter = m_stPkReward.find(index);
    return iter == m_stPkReward.end() ? nullptr : &(iter->second);
}

const TLogicGuildRandomExchangeElem* CLogicConfigGuildParser::GetExchangeRewardConfig(int32_t index) const
{
    auto iter = m_stExchangeReward.find(index);
    return iter == m_stExchangeReward.end() ? nullptr : &(iter->second);
}


const TLogicGuildWarRankReward* CLogicConfigGuildParser::GetGuildWarRankRewardConfig(int32_t rank) const
{
    auto iterRankRewardConfig = m_stGuildWarRankReward.begin();
    for(; iterRankRewardConfig != m_stGuildWarRankReward.end(); iterRankRewardConfig++)
    {
        if (iterRankRewardConfig->second.m_iMax >= rank &&
            iterRankRewardConfig->second.m_iMin <= rank)
        {
            break;
        }
    }
    //没找到匹配的排名，默认就是最后一段排名
    if (iterRankRewardConfig == m_stGuildWarRankReward.end())
    {
        iterRankRewardConfig--;
    }

    return &(iterRankRewardConfig->second);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
///公会战
std::pair<bool, std::string>
CLogicConfigGuildWarParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

		CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_war"));

		stXML.IntoElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("guild_war_time"));
		stXML.IntoElem();
		while (stXML.FindElem("time"))
		{
			const std::string war = stXML.GetAttrib("tag");
			for (int32_t i = 1; i < 10; ++i)
			{
				if (!stXML.GetAttrib("start_week_day_" + std::to_string(i)).empty())
				{
					TLogicGuildWarTimeElem stElem;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("start_week_day_" + std::to_string(i)), stElem.m_iStartWeekDay));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("start_hour_" + std::to_string(i)), stElem.m_iStartHour));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("start_minute_" + std::to_string(i)), stElem.m_iStartMinute));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("end_week_day_" + std::to_string(i)), stElem.m_iEndWeekDay));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("end_hour_" + std::to_string(i)), stElem.m_iEndHour));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("end_minute_" + std::to_string(i)), stElem.m_iEndMinute));
					m_stTimeMap.emplace(war, stElem);
				}
			}
		}
		stXML.OutOfElem();	//guild_war_time

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("attack_defense_info"));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("init_army_point"), m_stAtkDefInfo.m_iInitArmyPoint));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("max_army_point"), m_stAtkDefInfo.m_iMaxArmyPoint));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("army_point_interval"), m_stAtkDefInfo.m_iArmyPointInterval));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fight_fail_cd"), m_stAtkDefInfo.m_iFightFailCD));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("remove_cd_cost"), m_stAtkDefInfo.m_iRemoveCDCost));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fight_cost_army_point"), m_stAtkDefInfo.m_iFightCostArmyPoint));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("repair_cost_army_point"), m_stAtkDefInfo.m_iRepairCostArmyPoint));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fight_guild_level"), m_stAtkDefInfo.m_iFightGuildLevel));
		CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fight_guild_member"), m_stAtkDefInfo.m_iFightGuildMember));
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("avy_prepare_time"), m_stAtkDefInfo.m_iAvyPrepareTime));
        int32_t bAvyPrepareEndBegin;
        CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("avy_prepare_end_begin"), bAvyPrepareEndBegin));
        m_stAtkDefInfo.m_bAvyPrepareEndBegin = (bool)bAvyPrepareEndBegin;
		CONFIG_ASSERT_EXCEPTION(m_stAtkDefInfo.m_iArmyPointInterval > 0);
		CONFIG_ASSERT_EXCEPTION(m_stAtkDefInfo.m_iRemoveCDCost > 0);
		CONFIG_ASSERT_EXCEPTION(m_stAtkDefInfo.m_iFightCostArmyPoint > 0);
		CONFIG_ASSERT_EXCEPTION(m_stAtkDefInfo.m_iRepairCostArmyPoint > 0);
		CONFIG_ASSERT_EXCEPTION(m_stAtkDefInfo.m_iFightGuildLevel > 0);
		CONFIG_ASSERT_EXCEPTION(m_stAtkDefInfo.m_iFightGuildMember > 0);
        CONFIG_ASSERT_EXCEPTION(m_stAtkDefInfo.m_iAvyPrepareTime >= 300);

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("attack_defense_fight"));
		stXML.IntoElem();
		while (stXML.FindElem("fight"))
		{
			int32_t iType = 0, iDifficult = 0;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("type"), iType));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("difficult"), iDifficult));

			TLogicAttackDefenseFightElem stElem;
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fhp"), stElem.m_dHp));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fatk"), stElem.m_dAtk));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fdef"), stElem.m_dDef));
			CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("fcri"), stElem.m_dCri));
			stElem.m_strFightAwardBag = stXML.GetAttrib("fight_award_bag");
			stElem.m_strActivityFightAwardBag = stXML.GetAttrib("activity_fight_award_bag");
			stElem.m_strRepairAwardBag = stXML.GetAttrib("repair_award_bag");
			stElem.m_strActivityRepairAwardBag = stXML.GetAttrib("activity_repair_award_bag");

			m_stAtkDefFightMap.emplace(std::make_pair(iType, iDifficult), stElem);
		}
		stXML.OutOfElem();	//attack_defense_fight
		
		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("attack_defense_award"));
		stXML.IntoElem();
		while (stXML.FindElem("award"))
		{
			const std::string result = stXML.GetAttrib("tag");

			TLogicMailElem stElem;
			stElem.m_strTitle = stXML.GetAttrib("title");
			stElem.m_stContent = CFormatString::Compile(stXML.GetAttrib("content"));
			for (int32_t i = 1; i <= 5 ; ++i)
			{
				if (!stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
				{
					CPackGameItem stItem;
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)), stItem.m_iItemType));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)), stItem.m_iCardID));
					CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)), stItem.m_iNum));
					stElem.m_astItem.push_back(std::move(stItem));
				}
			}

			m_stAtkDefAwardMap.emplace(result, stElem);
		}
		stXML.OutOfElem();	//attack_defense_award

		if (stXML.FindElem("activity_attack_defense_award"))
		{
			stXML.IntoElem();
			while (stXML.FindElem("award"))
			{
				const std::string result = stXML.GetAttrib("tag");

				TLogicMailElem stElem;
				stElem.m_strTitle = stXML.GetAttrib("title");
				stElem.m_stContent = CFormatString::Compile(stXML.GetAttrib("content"));
				for (int32_t i = 1; i <= 5; ++i)
				{
					if (!stXML.GetAttrib("bonus_item_type_" + std::to_string(i)).empty())
					{
						CPackGameItem stItem;
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_type_" + std::to_string(i)), stItem.m_iItemType));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_id_" + std::to_string(i)), stItem.m_iCardID));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("bonus_item_num_" + std::to_string(i)), stItem.m_iNum));
						stElem.m_astItem.push_back(std::move(stItem));
					}
				}

				m_stActivityAtkDefAwardMap.emplace(result, stElem);
			}
			stXML.OutOfElem();	//activity_attack_defense_award
		}

		if (stXML.FindElem("attack_defense_match"))
		{
			stXML.IntoElem();
			while (stXML.FindElem("match"))
			{
				int32_t iWinCount = 0;
				CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("win_count"), iWinCount));
				for (int32_t i = 1 ; i < 10; ++i)
				{
					if (!stXML.GetAttrib("multiple_" + std::to_string(i)).empty())
					{
						double dMultiple = 0.0;
						int32_t iRate = 0;
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("multiple_" + std::to_string(i)), dMultiple));
						CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("rate_" + std::to_string(i)), iRate));
						m_stAtkDefMatchMap[iWinCount].emplace_back(dMultiple, iRate);
					}		
				}
			}
			stXML.OutOfElem();	//attack_defense_match
		}

		stXML.OutOfElem();//guild_war

	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

//战斗前10分钟为攻防预备时间
bool CLogicConfigGuildWarParser::IsAttackDefenseReady(int32_t iTime)const
{
	return !IsAttackDefenseFighting(iTime) && IsAttackDefenseFighting(iTime + m_stAtkDefInfo.m_iAvyPrepareTime);
}

bool CLogicConfigGuildWarParser::IsAttackDefenseFighting(int32_t iTime)const
{
	const int32_t iMondayTime = get_monday_time(CLogicCommon::GetSec());
	for (auto iter = m_stTimeMap.lower_bound("attack_defense"); iter != m_stTimeMap.upper_bound("attack_defense"); ++iter)
	{
		const int32_t iStartTime = iMondayTime + (iter->second.m_iStartWeekDay - 1) * 24 * 60 * 60
                                    + iter->second.m_iStartHour * 60 * 60 + iter->second.m_iStartMinute * 60;

        const int32_t iEndTime = iMondayTime + (iter->second.m_iEndWeekDay - 1) * 24 * 60 * 60
                                 + iter->second.m_iEndHour * 60 * 60 + iter->second.m_iEndMinute * 60;

        if (iTime >= iStartTime && iTime <= iEndTime)
        {
            return true;
        }
	}
	return false;
}

const TLogicAttackDefenseFightElem * CLogicConfigGuildWarParser::GetAttackDefenseFightConfig(int32_t iType, int32_t iDifficult) const
{
	const auto iter = m_stAtkDefFightMap.find(std::make_pair(iType, iDifficult));
	return iter == m_stAtkDefFightMap.end() ? nullptr : &iter->second;
}

double CLogicConfigGuildWarParser::GetAttackDefenseMatchMultiple(int32_t iWinCount) const
{
	if (m_stAtkDefMatchMap.empty())
	{
		return 1.0;
	}

	const auto iter = m_stAtkDefMatchMap.lower_bound(iWinCount);
	const auto& rstVector = iter != m_stAtkDefMatchMap.end() ? iter->second : m_stAtkDefMatchMap.rbegin()->second;

	int32_t iTotal = 0;
	for (const auto& elem : rstVector)
	{
		iTotal += elem.second;
	}

	if (iTotal <= 0)
	{
		return 1.0;
	}

	int32_t iRand = CLogicCommon::GetRandNum() % iTotal;
	for (const auto& elem : rstVector)
	{
		if (iRand < elem.second)
		{
			return elem.first;
		}
		iRand -= elem.second;
	}
	return 1.0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
///好友
std::pair<bool, std::string>
CLogicConfigFriendParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		CONFIG_ASSERT_EXCEPTION(!strXMLContent.empty());

		CONFIG_ASSERT_EXCEPTION(stXML.SetDoc(strXMLContent));

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("friend"));

		stXML.IntoElem();

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
		m_iMaxFriendCount = atoi(stXML.GetAttrib("max_friend_count").c_str());
		m_iDailyPraiseCount = atoi(stXML.GetAttrib("daily_praise_count").c_str());
		CONFIG_ASSERT_EXCEPTION(m_iMaxFriendCount > 0 && m_iDailyPraiseCount > 0);

		CONFIG_ASSERT_EXCEPTION(stXML.FindElem("intimate"));
		stXML.IntoElem();

		while (stXML.FindElem("intimate_level"))
		{
			const int32_t iLevel = atoi(stXML.GetAttrib("level").c_str());
			TLogicIntimateLevelElem stElem;
			stElem.m_iIntimacy = atoi(stXML.GetAttrib("intimacy").c_str());
			stElem.m_iPraiseIntimacy = atoi(stXML.GetAttrib("praise_intimacy").c_str());
			stElem.m_iPraiseEnergy = atoi(stXML.GetAttrib("praise_energy").c_str());
			CONFIG_ASSERT_EXCEPTION(iLevel > 0);
			CONFIG_ASSERT_EXCEPTION(stElem.m_iPraiseIntimacy > 0);
			CONFIG_ASSERT_EXCEPTION(stElem.m_iPraiseEnergy > 0);

			m_stIntimateLevelMap[iLevel] = std::move(stElem);
		}

		stXML.OutOfElem();
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

int32_t CLogicConfigFriendParser::GetIntimateLevel(int32_t iIntimacy) const
{
	int32_t iLevel = 0;
	for (const auto & config : m_stIntimateLevelMap)
	{
		if (iIntimacy < config.second.m_iIntimacy)
		{
			break;
		}
		iLevel = config.first;
	}
	return iLevel;
}

