#include "logic_config_award.h"
#include "Markup.h"
#include "server_errcode.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "common_datetime.h"
#include "logic_config.h"

std::pair<bool, std::string>
CLogicConfigBankPriceParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		m_stBankPriceConfigVec.clear();
		while (stXML.FindElem("gold_price"))
		{
			TLogicBankPriceElem stElem;
			stElem.m_iDay = ::atoi(stXML.GetAttrib("day").c_str());
			stElem.m_iHour = ::atoi(stXML.GetAttrib("hour").c_str());
			stElem.m_iWeiPrice = ::atoi(stXML.GetAttrib("wei_price").c_str());
			stElem.m_iShuPrice = ::atoi(stXML.GetAttrib("shu_price").c_str());
			stElem.m_iWuPrice = ::atoi(stXML.GetAttrib("wu_price").c_str());
			stElem.m_strEvent = stXML.GetAttrib("event");

			if (stElem.m_iWeiPrice <= 0 || stElem.m_iShuPrice <= 0 || stElem.m_iWuPrice <= 0)
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "Wei,Shu,Wu price less than 0" << std::endl;
				throw std::exception();
			}

			m_stBankPriceConfigVec.push_back(stElem);
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string>
CLogicConfigAwardParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (!stXML.FindElem("bank"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "bank not found" << std::endl;
			throw std::exception();
		}
		m_iBankDefaultCapacity = ::atoi(stXML.GetAttrib("default_capacity").c_str());
		m_iBankMonthCardAddCapacity = ::atoi(stXML.GetAttrib("month_card_add_capacity").c_str());
		m_iBankSilverMonthCardAddCapacity = ::atoi(stXML.GetAttrib("silver_month_card_add_capacity").c_str());
		m_iBankGoldMonthCardAddCapacity = ::atoi(stXML.GetAttrib("gold_month_card_add_capacity").c_str());
		m_iBankQuarterCardAddCapacity = ::atoi(stXML.GetAttrib("quarter_card_add_capacity").c_str());
		if (m_iBankDefaultCapacity <= 0 || m_iBankMonthCardAddCapacity < 0 || m_iBankQuarterCardAddCapacity < 0)
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "bank capacity less than 0" << std::endl;
			throw std::exception();
		}

		if (!stXML.FindElem("multicolor_lantern"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "multicolor_lantern not found" << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (!stXML.FindElem("info"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "multicolor_lantern info not found" << std::endl;
			throw std::exception();
		}

		std::string strDate = stXML.GetAttrib("start_datetime");
		if (strDate.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "multicolor_lantern  start_datetime not found" << std::endl;
			throw std::exception();
		}

		struct tm stTM = {};
		if (strptime(strDate.c_str(), "%Y-%m-%d %T", &stTM) == nullptr)
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT multicolor_lantern  start_datetime TIME ERROR" << std::endl;
			throw std::exception();
		}

		m_stMultiColorLantern.m_iStartDatetime = mktime(&stTM);
		m_stMultiColorLantern.m_iStillDay = ::atoi(stXML.GetAttrib("still_day").c_str());
		m_stMultiColorLantern.m_iStopDay = ::atoi(stXML.GetAttrib("stop_day").c_str());
		for (int32_t i = 1; i < 10; ++i)
		{
			const std::string attr_open_hour = "open_hour_" + std::to_string(i);
			const int32_t open_hour = ::atoi(stXML.GetAttrib(attr_open_hour).c_str());
			m_stMultiColorLantern.m_iOpenHourVec.push_back(open_hour);
		}
		std::sort(m_stMultiColorLantern.m_iOpenHourVec.begin(), m_stMultiColorLantern.m_iOpenHourVec.end());
		m_stMultiColorLantern.m_iOpenHourVec.erase(std::unique(m_stMultiColorLantern.m_iOpenHourVec.begin(), m_stMultiColorLantern.m_iOpenHourVec.end()), m_stMultiColorLantern.m_iOpenHourVec.end());
		m_stMultiColorLantern.m_iDefaultAmount = ::atoi(stXML.GetAttrib("default_amount").c_str());
		m_stMultiColorLantern.m_iMonthCardAddAmount = ::atoi(stXML.GetAttrib("month_card_add_amount").c_str());
		m_stMultiColorLantern.m_iSilverMonthCardAddAmount = ::atoi(stXML.GetAttrib("silver_month_card_add_amount").c_str());
		m_stMultiColorLantern.m_iGoldMonthCardAddAmount = ::atoi(stXML.GetAttrib("gold_month_card_add_amount").c_str());
		m_stMultiColorLantern.m_iQuarterCardAddAmount = ::atoi(stXML.GetAttrib("quarter_card_add_amount").c_str());
		if (m_stMultiColorLantern.m_iStartDatetime <= 0 || m_stMultiColorLantern.m_iStillDay < 0 || m_stMultiColorLantern.m_iStopDay < 0 || m_stMultiColorLantern.m_iOpenHourVec.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "multicolor_lantern day <= 0 or open hour empty" << std::endl;
			throw std::exception();
		}

		if (m_stMultiColorLantern.m_iDefaultAmount <= 0)
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "multicolor_lantern amount <= 0" << std::endl;
			throw std::exception();
		}

		if (!stXML.FindElem("award"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "multicolor_lantern award not found" << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("card"))
		{
			TLogicMulticolorLanternAwardElem stAward;
			stAward.m_strSender = stXML.GetAttrib("send");
			stAward.m_strTitle = stXML.GetAttrib("title");
			stAward.m_strDoubleTitle = stXML.GetAttrib("double_title");
			stAward.m_strConetnt = stXML.GetAttrib("content");
            stAward.m_strDoubleContent = stXML.GetAttrib("double_content");
			for (int32_t i = 1 ; i < 10; ++i)
			{
				std::string attr_item_type = "bonus_item_type_" + std::to_string(i);
				std::string attr_item_id = "bonus_item_id_" + std::to_string(i);
				std::string attr_item_count = "bonus_item_count_" + std::to_string(i);

				CPackGameItem stItem;
				stItem.m_iItemType = ::atoi(stXML.GetAttrib(attr_item_type).c_str());
				stItem.m_iCardID = ::atoi(stXML.GetAttrib(attr_item_id).c_str());
				stItem.m_iNum = ::atoi(stXML.GetAttrib(attr_item_count).c_str());
				if (stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0)
				{
					stAward.m_stBonusVec.emplace_back(std::move(stItem));
				}
			}

			const int32_t iAward = ::atoi(stXML.GetAttrib("award").c_str());
			m_stMultiColorLantern.m_stAwardMap[iAward] = std::move(stAward);
		}

		stXML.OutOfElem();

		stXML.OutOfElem();

		if (!stXML.FindElem("happy_turn_card"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "happy_turn_card not found" << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (!stXML.FindElem("info"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "happy_turn_card info not found" << std::endl;
			throw std::exception();
		}

		strDate = stXML.GetAttrib("start_datetime");
		if (strDate.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "multicolor_lantern  start_datetime not found" << std::endl;
			throw std::exception();
		}

		if (strptime(strDate.c_str(), "%Y-%m-%d %T", &stTM) == nullptr)
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT multicolor_lantern  start_datetime TIME ERROR" << std::endl;
			throw std::exception();
		}

		m_stHappyTurnCard.m_iStartDatetime = mktime(&stTM);
		m_stHappyTurnCard.m_iStillDay = ::atoi(stXML.GetAttrib("still_day").c_str());
		m_stHappyTurnCard.m_iStopDay = ::atoi(stXML.GetAttrib("stop_day").c_str());
		m_stHappyTurnCard.m_iCostEnergy = ::atoi(stXML.GetAttrib("cost_energy").c_str());
		m_stHappyTurnCard.m_strRandBag = stXML.GetAttrib("rand_bag");

		if (!stXML.FindElem("award"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "happy_turn_card award not found" << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("card"))
		{
			const int32_t iCount = ::atoi(stXML.GetAttrib("count").c_str());
			CPackGameItem stPack;
			stPack.m_iItemType = ::atoi(stXML.GetAttrib("bonus_item_type").c_str());
			stPack.m_iCardID = ::atoi(stXML.GetAttrib("bonus_item_id").c_str());
			stPack.m_iNum = ::atoi(stXML.GetAttrib("bonus_item_count").c_str());
			if (iCount <= 0 || stPack.m_iItemType <= 0 || stPack.m_iCardID <= 0 || stPack.m_iNum <= 0)
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "happy_turn_card item <= 0" << std::endl;
				throw std::exception();
			}
			m_stHappyTurnCard.m_stBonusItemMap[iCount] = std::move(stPack);
		}

		stXML.OutOfElem();

		stXML.OutOfElem();

		if (!stXML.FindElem("recommended_lineup"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "recommended_lineup not found" << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("lineup"))
		{
			const int32_t is_open = ::atoi(stXML.GetAttrib("isopen").c_str());
			if (is_open)
			{
				TLogicRecommendedLineupElem stElem;
				stElem.m_iIndex = ::atoi(stXML.GetAttrib("id").c_str());
				for (int32_t i = 0; i < 10; ++i)
				{
					int32_t iHeroID = ::atoi(stXML.GetAttrib("heroid_" + std::to_string(i)).c_str());
					if (iHeroID > 0)
					{
						stElem.m_iHeroVec.push_back(iHeroID);
					}

					CPackGameItem stItem;
					stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_item_type_" + std::to_string(i)).c_str());
					stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_item_id_" + std::to_string(i)).c_str());
					stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_item_count_" + std::to_string(i)).c_str());
					if (stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0)
					{
						stElem.m_stBonusVec.emplace_back(std::move(stItem));
					}
				}

				m_stRecommendedLineupMap[stElem.m_iIndex] = std::move(stElem);
			}
		}

		stXML.OutOfElem();

		if (!stXML.FindElem("buyback"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "buyback not found" << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("back"))
		{
			int32_t iType = ::atoi(stXML.GetAttrib("back_type").c_str());

			TLogicBuyBackElem stElem;
			stElem.m_iPara = ::atoi(stXML.GetAttrib("para").c_str());
			stElem.m_iDailyCount = ::atoi(stXML.GetAttrib("daily_count").c_str());
			stElem.m_iMaxCount = ::atoi(stXML.GetAttrib("max_count").c_str());
			stElem.m_iPrice = ::atoi(stXML.GetAttrib("price").c_str());
			m_stBuyBackMap[iType] = std::move(stElem);
		}

		stXML.OutOfElem();

		if (!stXML.FindElem("activity_time"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "activity_time not found" << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("time"))
		{
			const int32_t iType = ::atoi(stXML.GetAttrib("type").c_str());
			const int32_t iStarTime = get_time_from_string(stXML.GetAttrib("start_date"));
			const int32_t iEndTime = get_time_from_string(stXML.GetAttrib("end_date"));
			if (iStarTime <= 0 || iEndTime <= 0)
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "activity_time date error!" << std::endl;
				throw std::exception();
			}

			m_stActivityTimeMap[iType] = { iStarTime , iEndTime };
		}

		stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("month_card"))
        stXML.IntoElem();
        CONFIG_ASSERT_EXCEPTION_EX(stXML.FindElem("info"), "month_card info")
        m_stMonthCardConfig.m_iExpiredDay = ::atoi(stXML.GetAttrib("expired_day").c_str());
        CONFIG_ASSERT_EXCEPTION(m_stMonthCardConfig.m_iExpiredDay > 0)

        for (int i = 1; i <= 10; ++i)
        {
            std::stringstream stStream;
            stStream << i;
            if (stXML.GetAttrib("level_type" + stStream.str()).empty()) break;

            int iLevelType = ::atoi(stXML.GetAttrib("level_type" + stStream.str()).c_str());
            int iAddTimes = ::atoi(stXML.GetAttrib("add_times" + stStream.str()).c_str());
            int iRewardTimes = ::atoi(stXML.GetAttrib("add_award_times" + stStream.str()).c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumLevelType(iLevelType))
            CONFIG_ASSERT_EXCEPTION(iAddTimes >= 0)
            m_stMonthCardConfig.m_stLevelAwardTimesMap[iLevelType] = iRewardTimes;
            m_stMonthCardConfig.m_stLevelTimesMap[iLevelType] = iAddTimes;
        }

        for(int i = 1; i <= 10; ++i)
        {
            std::stringstream stStream;
            stStream << i;
            if(stXML.GetAttrib("award_item_type" + stStream.str()).empty()) break;

            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("award_item_type" + stStream.str()).c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("award_item_id" + stStream.str()).c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("award_item_num" + stStream.str()).c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iItemType);
            m_stMonthCardConfig.m_stAwardItems.emplace_back(stItem);
        }


        m_stMonthCardConfig.m_iPhyStorePctInc = ::atoi(stXML.GetAttrib("phy_store_pct_inc").c_str());
        m_stMonthCardConfig.m_strSender = stXML.GetAttrib("send").c_str();
        m_stMonthCardConfig.m_strTitle = stXML.GetAttrib("title").c_str();
        m_stMonthCardConfig.m_strContent = stXML.GetAttrib("content").c_str();
        CONFIG_ASSERT_EXCEPTION(m_stMonthCardConfig.m_iPhyStorePctInc >= 0)
        stXML.OutOfElem();


        //体力月卡
        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("fat_month_card"))
        stXML.IntoElem();
        CONFIG_ASSERT_EXCEPTION_EX(stXML.FindElem("info"), "fat_month_card info")
        m_stFatMonthCardConfig.m_iExpiredDay = ::atoi(stXML.GetAttrib("expired_day").c_str());
        CONFIG_ASSERT_EXCEPTION(m_stFatMonthCardConfig.m_iExpiredDay > 0)

        for(int i = 1; i <= 10; ++i)
        {
            std::stringstream stStream;
            stStream << i;
            if(stXML.GetAttrib("award_item_type" + stStream.str()).empty()) break;

            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("award_item_type" + stStream.str()).c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("award_item_id" + stStream.str()).c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("award_item_num" + stStream.str()).c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iItemType);
            m_stFatMonthCardConfig.m_stAwardItems.emplace_back(stItem);
        }
        m_stFatMonthCardConfig.m_strSender = stXML.GetAttrib("send").c_str();
        m_stFatMonthCardConfig.m_strTitle = stXML.GetAttrib("title").c_str();
        m_stFatMonthCardConfig.m_strContent = stXML.GetAttrib("content").c_str();
        stXML.OutOfElem();


        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("deposit_bag"))
        stXML.IntoElem();
        while (stXML.FindElem("deposit"))
        {
            TLogicAwardDepositElem elem;
            auto index = ::atoi(stXML.GetAttrib("index").c_str());
            auto shop_index = ::atoi(stXML.GetAttrib("shop_index").c_str());
            auto deposit_id = ::atoi(stXML.GetAttrib("deposit_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX((deposit_id != 0 || shop_index != 0), index);

            elem.m_iAwardID = shop_index == 0 ?  deposit_id : shop_index;
            elem.m_iDuration = ::atoi(stXML.GetAttrib("duration_day").c_str());
            elem.m_strEmailTitle = stXML.GetAttrib("email_title").c_str();
            elem.m_strEmailContent = stXML.GetAttrib("email_content").c_str();
            elem.m_strEmailSender = stXML.GetAttrib("email_sender").c_str();
            elem.m_iExpired_day = ::atoi(stXML.GetAttrib("expired_day").c_str());
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iItemType);
                elem.m_stBonusVec.emplace_back(stItem);
            }
            m_stAwardDepositMap[elem.m_iAwardID] = elem;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("pt_reward"))

        m_stPTRewardConfig.m_strTitle = stXML.GetAttrib("email_title").c_str();
        std::string strContent = stXML.GetAttrib("email_content").c_str();
        auto stStarVec = string_split(strContent, "|");

        CONFIG_ASSERT_EXCEPTION(stStarVec.size() == 2)
        m_stPTRewardConfig.m_strContent = stStarVec[0];
        m_stPTRewardConfig.m_strAfterContent = stStarVec[1];

        m_stPTRewardConfig.m_strSender = stXML.GetAttrib("email_sender").c_str();

        for(int i = 1; i <= 10; ++i)
        {
            std::stringstream stStream;
            stStream << i;
            if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iItemType);
            m_stPTRewardConfig.m_stReward.emplace_back(stItem);
        }

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("create_role"))
        stXML.IntoElem();
        while (stXML.FindElem("item"))
        {
            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type").c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id").c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iItemType);
            m_stInitReward.emplace_back(stItem);
        }
        stXML.OutOfElem();


        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("add_item"))
        stXML.IntoElem();
        while (stXML.FindElem("item"))
        {
            CPackGameItem stItem;
            stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type").c_str());
            stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id").c_str());
            stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iItemType);
            m_stAddReward.emplace_back(stItem);
        }
        stXML.OutOfElem();


        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("date_change"))
        m_stDataChangeConfig.m_strTitle = stXML.GetAttrib("title").c_str();
        m_stDataChangeConfig.m_strContent = stXML.GetAttrib("content").c_str();
        m_stDataChangeConfig.m_strSender = stXML.GetAttrib("sender").c_str();
        m_stDataChangeConfig.m_iNeedGrade = ::atoi(stXML.GetAttrib("need_grade").c_str());
        m_stDataChangeConfig.m_iCardID = ::atoi(stXML.GetAttrib("hero_card_id").c_str());
        m_stDataChangeConfig.m_iAvyTime = ::atoi(stXML.GetAttrib("avy_time_id").c_str());

        CPackGameItem stItem;
        stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type").c_str());
        stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id").c_str());
        stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num").c_str());
        CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, stItem.m_iItemType);
        m_stDataChangeConfig.m_stReward.emplace_back(stItem);

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("anti_cheating_check"))
        iAntiCheatingRatio = ::atoi(stXML.GetAttrib("ration").c_str());

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("anti_cheating_time"))
        iPassLevelTime = ::atoi(stXML.GetAttrib("limit_time").c_str());

        stXML.OutOfElem();
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}

bool CLogicConfigAwardParser::IsInActivityTime(CEnumActivityTimeType iType, int32_t iTime) const
{
	const auto iter = m_stActivityTimeMap.find(static_cast<int32_t>(iType));
	return iter != m_stActivityTimeMap.end() && iter->second.first <= iTime && iter->second.second >= iTime;
}

int CLogicConfigAwardParser::GetMonthCardAddLevelTimes(int iLevelType) const
{
    auto iter = m_stMonthCardConfig.m_stLevelTimesMap.find(iLevelType);
    return iter != m_stMonthCardConfig.m_stLevelTimesMap.end() ? iter->second : 0;
}

int CLogicConfigAwardParser::GetMonthCardAdvLevelTimes(int iLevelType) const
{
    auto iter = m_stMonthCardConfig.m_stLevelAwardTimesMap.find(iLevelType);
    return iter != m_stMonthCardConfig.m_stLevelAwardTimesMap.end() ? iter->second : 0;
}


const TLogicAwardDepositElem* CLogicConfigAwardParser::GetAwardConfig(int32_t index) const
{
    auto iter = m_stAwardDepositMap.find(index);
    return iter == m_stAwardDepositMap.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string> CLogicConfigRedBagParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if (!stXML.FindElem("red_bag"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        std::vector<int32_t> stUniqueIndex;
        while (stXML.FindElem("item"))
        {
            const int32_t id = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION(nullptr != CLogicConfig::GetItemConfig().GetConfig(id));

            TLogicRedBagElem stElem;
            stElem.m_iTime = ::atoi(stXML.GetAttrib("time").c_str());
            CONFIG_ASSERT_EXCEPTION(stElem.m_iTime > 0);

            // 新红包，事件触发类型
            int32_t iEventType = ::atoi(stXML.GetAttrib("event_type").c_str());
            if(iEventType != CLogicConfigDefine::ECET_Invalid)
            {
                CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumCommonEventType(iEventType));

                TLogicRedBagEventElem stEvent;
                stEvent.m_iItemID = id;
                stEvent.m_iUniqueIndex = ::atoi(stXML.GetAttrib("unique_index").c_str());
                stEvent.m_iEventPara1 = ::atoi(stXML.GetAttrib("event_para1").c_str());
                stEvent.m_iEventPara2 = ::atoi(stXML.GetAttrib("event_para2").c_str());
                stEvent.m_strMailSender = stXML.GetAttrib("mail_sender");
                stEvent.m_strMailTitle = stXML.GetAttrib("mail_title");
                stEvent.m_strMailContent = stXML.GetAttrib("mail_content");

                CONFIG_ASSERT_EXCEPTION(0 <= stEvent.m_iUniqueIndex && stEvent.m_iUniqueIndex <= 700);
                if(stEvent.m_iUniqueIndex > 0)      // stEvent.m_iUniqueIndex = 0 表示可重复获得的红包
                {
                    CONFIG_ASSERT_EXCEPTION(std::find(stUniqueIndex.begin(), stUniqueIndex.end(), stEvent.m_iUniqueIndex) == stUniqueIndex.end());
                    stUniqueIndex.push_back(stEvent.m_iUniqueIndex);
                }

                CONFIG_ASSERT_EXCEPTION(stEvent.m_iEventPara1 >= 0);
                CONFIG_ASSERT_EXCEPTION(stEvent.m_iEventPara2 >= 0);
                CONFIG_ASSERT_EXCEPTION(!stEvent.m_strMailSender.empty());
                CONFIG_ASSERT_EXCEPTION(!stEvent.m_strMailTitle.empty());

                m_stRedBagEventMap.emplace(iEventType, stEvent);
            }

            // 旧红包规则
            stElem.m_iOldCount = ::atoi(stXML.GetAttrib("old_count").c_str());
            stElem.m_iOldTotalYuanbao = ::atoi(stXML.GetAttrib("old_total_yuanbao").c_str());
            stElem.m_iOldMinYuanbao = ::atoi(stXML.GetAttrib("old_min_yuanbao").c_str());
            stElem.m_iOldMaxYuanbao = ::atoi(stXML.GetAttrib("old_max_yuanbao").c_str());
            if (stElem.m_iOldCount > 0)
            {
                CONFIG_ASSERT_EXCEPTION(stElem.m_iOldTotalYuanbao > 0);
                CONFIG_ASSERT_EXCEPTION(stElem.m_iOldMinYuanbao > 0);
                CONFIG_ASSERT_EXCEPTION(stElem.m_iOldMaxYuanbao > 0);
            }

            // 新红包
            stElem.m_strNewRedBag = stXML.GetAttrib("new_red_bag");
            if(stElem.m_iOldCount <= 0)
            {
                CONFIG_ASSERT_EXCEPTION(!stElem.m_strNewRedBag.empty());
            }
            else
            {
                CONFIG_ASSERT_EXCEPTION(stElem.m_strNewRedBag.empty());
            }

            stElem.m_iSendChannel = ::atoi(stXML.GetAttrib("send_channel").c_str());
            stElem.m_strSendBroadcast = stXML.GetAttrib("send_broadcast");
            stElem.m_strSendTalk = stXML.GetAttrib("send_talk");

            m_stRedBagMap[id] = std::move(stElem);
        }

        stXML.OutOfElem();

        if (stXML.FindElem("red_bag_receive_talk"))
        {
            stXML.IntoElem();

            while (stXML.FindElem("talk"))
            {
                m_stRedBagReceiveTalkVec.push_back(CFormatString::Compile(stXML.GetAttrib("talk")));
            }

            stXML.OutOfElem();
        }

        if (!stXML.FindElem("new_red_bag_reward"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("bag"))
        {
            std::string strBagName = stXML.GetAttrib("name");
            CONFIG_ASSERT_EXCEPTION(m_stNewRedBagMap.find(strBagName) == m_stNewRedBagMap.end());

            stXML.IntoElem();
            while (stXML.FindElem("item"))
            {
                TLogicNewRedBagReward stReward;
                int32_t iIndex = ::atoi(stXML.GetAttrib("index").c_str());
                stReward.m_iItemType = ::atoi(stXML.GetAttrib("item_type").c_str());
                stReward.m_iItemID = ::atoi(stXML.GetAttrib("item_id").c_str());
                stReward.m_iItemNumMin = ::atoi(stXML.GetAttrib("item_num_min").c_str());
                stReward.m_iItemNumMax = ::atoi(stXML.GetAttrib("item_num_max").c_str());
                CONFIG_ASSERT_EXCEPTION(stReward.m_iItemType > 0);
                CONFIG_ASSERT_EXCEPTION(stReward.m_iItemID > 0);
                CONFIG_ASSERT_EXCEPTION(stReward.m_iItemNumMin > 0);
                CONFIG_ASSERT_EXCEPTION(stReward.m_iItemNumMax > 0);

                stReward.m_iWeight = ::atoi(stXML.GetAttrib("weight").c_str());
                CONFIG_ASSERT_EXCEPTION(stReward.m_iWeight > 0);

                stReward.m_iCount = ::atoi(stXML.GetAttrib("count").c_str());
                CONFIG_ASSERT_EXCEPTION(stReward.m_iCount > 0);

                stReward.m_bIsBest = (bool)::atoi(stXML.GetAttrib("is_best").c_str());

                m_stNewRedBagMap[strBagName][iIndex] = std::move(stReward);
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION(!m_stNewRedBagMap[strBagName].empty());
        }
        stXML.OutOfElem();

        for(auto elem : m_stRedBagMap)
        {
            if(elem.second.m_strNewRedBag.empty()) continue;

            auto iter = m_stNewRedBagMap.find(elem.second.m_strNewRedBag);
            if(iter == m_stNewRedBagMap.end())
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: new_red_bag not found! " << elem.second.m_strNewRedBag << std::endl;
                throw std::exception();
            }

            // 公会红包最多只能有两种物品
            if(elem.second.m_iSendChannel == CLogicConfigDefine::talk_channel_guild && iter->second.size() > 2)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: guild red_bag too large! " << elem.second.m_strNewRedBag << std::endl;
                throw std::exception();
            }
        }

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicRedBagElem* CLogicConfigRedBagParser::GetRedBagConfig(int32_t iItemID) const
{
    auto iter = m_stRedBagMap.find(iItemID);
    return iter == m_stRedBagMap.end() ? nullptr : &(iter->second);
}

const TLogicNewRedBagReward* CLogicConfigRedBagParser::GetNewRedBagReward(std::string strBagName, int32_t iIndex) const
{
    auto iterBag = m_stNewRedBagMap.find(strBagName);
    if(iterBag != m_stNewRedBagMap.end())
    {
        auto iterReward = iterBag->second.find(iIndex);
        return iterReward == iterBag->second.end() ? nullptr : &(iterReward->second);
    }

    return nullptr;
}

std::pair<bool, std::string> CLogicConfigWorldQuestParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if (!stXML.FindElem("quest_list"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("info"))
        {
            TLogicWorldQuestElem stElem;
            stElem.m_iQuestID = ::atoi(stXML.GetAttrib("quest_id").c_str());
            CONFIG_ASSERT_EXCEPTION(m_stQuestMap.find(stElem.m_iQuestID) == m_stQuestMap.end());
            CONFIG_ASSERT_EXCEPTION(0 < stElem.m_iQuestID && stElem.m_iQuestID < 400);

            stElem.m_strName = stXML.GetAttrib("bag_name");
            stElem.m_iOpenServerDay = ::atoi(stXML.GetAttrib("open_server_day").c_str());
            CONFIG_ASSERT_EXCEPTION(stElem.m_iOpenServerDay >= 0);
            stElem.m_iContinueTime = ::atoi(stXML.GetAttrib("continue_time").c_str());
            CONFIG_ASSERT_EXCEPTION(stElem.m_iContinueTime >= 0);

            int32_t iEventType = ::atoi(stXML.GetAttrib("event_type").c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumCommonEventType(iEventType));
            stElem.m_eEventType = (CLogicConfigDefine::EnumCommonEventType)iEventType;
            stElem.m_iEventPara = ::atoi(stXML.GetAttrib("event_para1").c_str());
            stElem.m_iEventParaEx = ::atoi(stXML.GetAttrib("event_para2").c_str());
            stElem.m_iTargetProgress = ::atoi(stXML.GetAttrib("event_target").c_str());
            CONFIG_ASSERT_EXCEPTION(stElem.m_iEventPara >= 0);
            CONFIG_ASSERT_EXCEPTION(stElem.m_iEventParaEx >= 0);
            CONFIG_ASSERT_EXCEPTION(stElem.m_iTargetProgress > 0);

            stElem.m_bAfterCreateCanReceive = (bool)::atoi(stXML.GetAttrib("after_create_can_receive").c_str());
            stElem.m_strBagName = stXML.GetAttrib("reward_bag");
            CONFIG_ASSERT_EXCEPTION(!stElem.m_strBagName.empty());

            stElem.m_strBroadcastID = stXML.GetAttrib("broadcast_id");

            m_stQuestMap[stElem.m_iQuestID] = stElem;
            m_stQuestEventMap.emplace(stElem.m_eEventType, stElem);
        }
        stXML.OutOfElem();

        if (!stXML.FindElem("info_list"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        if (!stXML.FindElem("info"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        m_iBigPrizeTimes = ::atoi(stXML.GetAttrib("big_prize_times").c_str());
        stXML.OutOfElem();

        if (!stXML.FindElem("reward_bag"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("bag"))
        {
            std::string strBagName = stXML.GetAttrib("name");
            CONFIG_ASSERT_EXCEPTION(m_stRewardBagMap.find(strBagName) == m_stRewardBagMap.end());

            TLogicWorldQuestRewardBag stBag;

            stXML.IntoElem();
            while (stXML.FindElem("item"))
            {
                TLogicNewRedBagReward stReward;
                int32_t iIndex = ::atoi(stXML.GetAttrib("index").c_str());
                CONFIG_ASSERT_EXCEPTION(stBag.m_stRewardBag.find(iIndex) == stBag.m_stRewardBag.end());

                stReward.m_iItemType = ::atoi(stXML.GetAttrib("item_type").c_str());
                stReward.m_iItemID = ::atoi(stXML.GetAttrib("item_id").c_str());
                stReward.m_iItemNumMin = ::atoi(stXML.GetAttrib("item_num_min").c_str());
                stReward.m_iItemNumMax = ::atoi(stXML.GetAttrib("item_num_max").c_str());
                CONFIG_ASSERT_EXCEPTION(stReward.m_iItemType > 0);
                CONFIG_ASSERT_EXCEPTION(stReward.m_iItemID > 0);
                CONFIG_ASSERT_EXCEPTION(stReward.m_iItemNumMin > 0);
                CONFIG_ASSERT_EXCEPTION(stReward.m_iItemNumMax > 0);

                stReward.m_iWeight = ::atoi(stXML.GetAttrib("weight").c_str());
                CONFIG_ASSERT_EXCEPTION(stReward.m_iWeight > 0);
                stBag.m_iTotalWeight += stReward.m_iWeight;

                stReward.m_bIsBest = (bool)::atoi(stXML.GetAttrib("is_best").c_str());

                stBag.m_stRewardBag[iIndex] = stReward;
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION(!stBag.m_stRewardBag.empty());

            m_stRewardBagMap[strBagName] = std::move(stBag);
        }
        stXML.OutOfElem();

        for(auto elem : m_stQuestMap)
        {
            auto iter = m_stRewardBagMap.find(elem.second.m_strBagName);
            if(iter == m_stRewardBagMap.end())
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: new_red_bag not found! " << elem.second.m_strBagName << std::endl;
                throw std::exception();
            }
        }

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicWorldQuestElem* CLogicConfigWorldQuestParser::GetQuestConfig(int32_t iQuestID) const
{
    auto iter = m_stQuestMap.find(iQuestID);
    return iter == m_stQuestMap.end() ? nullptr : &(iter->second);
}

const TLogicNewRedBagReward* CLogicConfigWorldQuestParser::GetRewardBagRandReward(std::string strBagName, bool bBigPrize) const
{
    auto iterBag = m_stRewardBagMap.find(strBagName);
    if(iterBag == m_stRewardBagMap.end())
        return nullptr;

    if(bBigPrize)
    {
        for(auto& item : iterBag->second.m_stRewardBag)
        {
            if(item.second.m_bIsBest) return &(item.second);
        }
    }

    int32_t iRandWeight = CLogicCommon::GetRandNum() % iterBag->second.m_iTotalWeight + 1;
    int32_t iCurWeight = 0;
    for(auto& item : iterBag->second.m_stRewardBag)
    {
        iCurWeight += item.second.m_iWeight;
        if(iCurWeight >= iRandWeight)
        {
            return &(item.second);
        }
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigRankListBonusParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("data_ranklist"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;

			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("rank_types"))
		{
			std::string strTag = stXML.GetAttrib("rank_type");
			stXML.IntoElem();

			TLogicRankListBonusDesc stDesc;
			if (false == stXML.FindElem("award_mail"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}

			stDesc.m_strMailTitle = stXML.GetAttrib("title");
			stDesc.m_strMailSender = stXML.GetAttrib("sender");
			stDesc.m_strMailContent1 = stXML.GetAttrib("content1");
			stDesc.m_strMailContent2 = stXML.GetAttrib("content2");

			if (false == stXML.FindElem("award_list"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}

			stXML.IntoElem();
			int iPreMax = 0, iNowMin, iNowMax;
			while (stXML.FindElem("award"))
			{
				TLogicRankListBonusElem stElem;
				iNowMin = ::atoi(stXML.GetAttrib("min").c_str());
				iNowMax = ::atoi(stXML.GetAttrib("max").c_str());
				if (iPreMax != (iNowMin - 1))
				{
					strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: PRE_MAX " << iPreMax << "NOT EQUAL NOW_MIN " << iNowMin << "MINUS 1" << std::endl;
					throw std::exception();
				}

				iPreMax = iNowMax;
				stElem.m_iBonusNum = 1 + iNowMax - iNowMin;

				int iPos = 1;
				char szItemType[64], szAward[64], szAmount[64];
				CPackGameItem stBonus;
				while (true)
				{
					snprintf(szItemType, sizeof(szItemType), "%s%d", "award_type_", iPos);
					if (true == stXML.GetAttrib(szItemType).empty())
						break;
					snprintf(szAward, sizeof(szAward), "%s%d", "award_id_", iPos);
					snprintf(szAmount, sizeof(szAmount), "%s%d", "award_amount_", iPos);
					stBonus.m_iItemType = ::atoi(stXML.GetAttrib(szItemType).c_str());
					stBonus.m_iCardID = ::atoi(stXML.GetAttrib(szAward).c_str());
					stBonus.m_iNum = ::atoi(stXML.GetAttrib(szAmount).c_str());
					stElem.m_stBonusList.push_back(stBonus);
					iPos++;
				}

				stDesc.m_stRankListBonusVec.push_back(stElem);
			}
			stXML.OutOfElem();

			if (strTag == "pvp_day_reward")
			{
				m_stPVPRanklistBonus = stDesc;
			}

			stXML.OutOfElem();
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
	return (std::pair<bool, std::string>(true, "OK"));
}


std::pair<bool, std::string>
CLogicConfigFirstDepositParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

        if (false == stXML.FindElem("root"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

		if (false == stXML.FindElem("first_deposit"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();
		while (stXML.FindElem("reward"))
		{
			CPackGameItem stElem;

			stElem.m_iItemType = atoi(stXML.GetAttrib("reward_type").c_str());
			stElem.m_iCardID = atoi(stXML.GetAttrib("reward_id").c_str());
			stElem.m_iNum = atoi(stXML.GetAttrib("reward_num").c_str());

			m_stBonusVec.push_back(stElem);
		}
		stXML.OutOfElem();

        if (false == stXML.FindElem("addup_deposit"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("addup_index"))
        {
            stXML.IntoElem();

            TLogicAddupDepositBonus info;
            CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
            info.m_iDepositNum = atoi(stXML.GetAttrib("num").c_str());
            CONFIG_ASSERT_EXCEPTION(info.m_iDepositNum > 0);

            while (stXML.FindElem("reward"))
            {
                CPackGameItem stElem;

                stElem.m_iItemType = atoi(stXML.GetAttrib("reward_type").c_str());
                stElem.m_iCardID = atoi(stXML.GetAttrib("reward_id").c_str());
                stElem.m_iNum = atoi(stXML.GetAttrib("reward_num").c_str());

                info.m_stBonus.push_back(stElem);
            }

            m_stAddUpDepositBonus.push_back(info);

            stXML.OutOfElem();
        }
        stXML.OutOfElem();
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string>
CLogicConfigDailyDepositParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("first_deposit"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("reward"))
		{
			CPackGameItem stElem;

			stElem.m_iItemType = atoi(stXML.GetAttrib("reward_type").c_str());
			stElem.m_iCardID = atoi(stXML.GetAttrib("reward_id").c_str());
			stElem.m_iNum = atoi(stXML.GetAttrib("reward_num").c_str());

			m_stBonusVec.push_back(stElem);
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

std::pair<bool, std::string>
CLogicConfigFightPowerRankActiveParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("fight_power_rank_active"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (false == stXML.FindElem("desc"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND desc TAG" << std::endl;
			throw std::exception();
		}

		m_iRankBonusHour = atoi(stXML.GetAttrib("rank_bonus_time").c_str());
		m_iRankBonusDay = atoi(stXML.GetAttrib("rank_bonus_day").c_str());
		m_iRankBonusMin = atoi(stXML.GetAttrib("rank_bonus_min").c_str());

		m_strTitle = stXML.GetAttrib("title");
		m_strSender = stXML.GetAttrib("sender");
		m_strContent1 = stXML.GetAttrib("content1");
		m_strContent2 = stXML.GetAttrib("content2");

		if (false == stXML.FindElem("rank_bonus"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND rank_bonus TAG" << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("index"))
		{
			TLogicFightPowerRankActiveRankElem stElem;
			for (int i = 1; i < 10; i++)
			{
				CPackGameItem stGameItem;
				std::stringstream stStream;
				stStream << "_" << i;
				if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

				stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
				stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
				stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

				stElem.m_stBonusVec.push_back(stGameItem);
			}

			m_stRankInfo.push_back(stElem);
		}

		stXML.OutOfElem();

		if (false == stXML.FindElem("stage_bonus"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:CAN NOT FOUND stage_bonus TAG" << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		while (stXML.FindElem("index"))
		{
			TLogicFightPowerRankActiveStageElem stElem;
			for (int i = 1; i < 10; i++)
			{
				CPackGameItem stGameItem;
				std::stringstream stStream;
				stStream << "_" << i;
				if (stXML.GetAttrib("bonus_item_type" + stStream.str()).empty()) break;

				stGameItem.m_iItemType = atoi(stXML.GetAttrib("bonus_item_type" + stStream.str()).c_str());
				stGameItem.m_iCardID = atoi(stXML.GetAttrib("bonus_item_id" + stStream.str()).c_str());
				stGameItem.m_iNum = atoi(stXML.GetAttrib("bonus_item_num" + stStream.str()).c_str());

				stElem.m_stBonusVec.push_back(stGameItem);
			}

			stElem.m_iFightPower = atoi(stXML.GetAttrib("fight_power").c_str());

			m_stStageInfo.push_back(stElem);
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicFightPowerRankActiveRankElem*
CLogicConfigFightPowerRankActiveParser::GetRankInfo(int iIndex) const
{
	if (iIndex < 0 || iIndex >(int) m_stRankInfo.size())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(FIGHT_POWER_RANK_ACTIVE_NOT_CONFIG);
		return (nullptr);
	}

	return (&m_stRankInfo[iIndex - 1]);
}

const TLogicFightPowerRankActiveStageElem*
CLogicConfigFightPowerRankActiveParser::GetStageInfo(int iIndex) const
{
	if (iIndex < 0 || iIndex >(int) m_stStageInfo.size())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(FIGHT_POWER_RANK_ACTIVE_NOT_CONFIG);
		return (nullptr);
	}

	return (&m_stStageInfo[iIndex - 1]);
}


///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigHighestRecordParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML Content IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("data_rankrecord"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();
		while (stXML.FindElem("rank_types"))
		{
			TLogicHighestRecordConfigElem stElem;
			std::string strTag = stXML.GetAttrib("rank_type");

			stXML.IntoElem();
			if (false == stXML.FindElem("record_award_mail"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}

			stElem.m_strMailTitle = stXML.GetAttrib("title");
			stElem.m_strMailSender = stXML.GetAttrib("sender");
			stElem.m_stMailContentTemplateObj = CFormatString::Compile(stXML.GetAttrib("content"));
            stElem.m_stFirstMailContentTemplateObj = CFormatString::Compile(stXML.GetAttrib("first_content"));
			stElem.m_iBonusItemType = ::atoi(stXML.GetAttrib("bonus_item_type").c_str());
			stElem.m_iBonusItemID = ::atoi(stXML.GetAttrib("bonus_item_id").c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfig::IsValidGameItem(stElem.m_iBonusItemType, stElem.m_iBonusItemID))

			if (false == stXML.FindElem("record_award_list"))
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
				throw std::exception();
			}

			stXML.IntoElem();
			while (stXML.FindElem("award"))
			{
				int iMinRank = atoi(stXML.GetAttrib("min").c_str());
                int iMaxRank = atoi(stXML.GetAttrib("max").c_str());
				int iAmount = atoi(stXML.GetAttrib("amount").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iMinRank > 0 && iMinRank <= iMaxRank, iMinRank)
                CONFIG_ASSERT_EXCEPTION_EX(stElem.m_stRecordMap.find(iMinRank) == stElem.m_stRecordMap.end(), iMinRank)
                CONFIG_ASSERT_EXCEPTION_EX(iAmount > 0, iMinRank)

                stElem.m_stRecordMap[iMaxRank] = std::make_pair(iMinRank, iAmount);
			}

			stXML.OutOfElem();

			stXML.OutOfElem();

            if (strTag == "pvp_rank_record_reward")
			{
				m_stPVP = stElem;
			}
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigVipDailyAwardParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("vip_daily_award"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

		if (stXML.FindElem("mail_info"))
		{
			stXML.IntoElem();

			if (stXML.FindElem("mail"))
			{
				m_strTitle = stXML.GetAttrib("title");
				m_strSender = stXML.GetAttrib("sender");
				m_strContent = stXML.GetAttrib("content");
			}

			stXML.OutOfElem();
		}

		if (stXML.FindElem("awards"))
		{
			stXML.IntoElem();

			while (stXML.FindElem("award"))
			{
				int iVip = atoi(stXML.GetAttrib("vip_id").c_str());

				CPackGameItem stElem;
				stElem.m_iItemType = atoi(stXML.GetAttrib("award_type").c_str());
				stElem.m_iCardID = atoi(stXML.GetAttrib("award_id").c_str());
				stElem.m_iNum = atoi(stXML.GetAttrib("award_amount").c_str());

				m_stVipDailyAward[iVip] = stElem;
			}

			stXML.OutOfElem();
		}
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigVipBagParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	try
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			throw std::exception();
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		if (false == stXML.FindElem("vip_bag"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

		stXML.IntoElem();

        if (false == stXML.FindElem("info"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        m_strDailyPrayBag = stXML.GetAttrib("pray_bag");
        if (!m_strDailyPrayBag.empty())
        {
            auto stCardbagIT = CLogicConfig::GetCardbagConfig().m_stCardbagConfig.find(m_strDailyPrayBag);
            if (stCardbagIT == CLogicConfig::GetCardbagConfig().m_stCardbagConfig.end())
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG pray_bag:" << m_strDailyPrayBag << " NOT FOUND IN cardbag.xml" << std::endl;
                throw std::exception();
            }
        }

        if (false == stXML.FindElem("common_bag"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
		while (stXML.FindElem("award"))
		{
			TLogicVipBagElem stElem;

			int iVipLevel = atoi(stXML.GetAttrib("vip_level").c_str());
			stElem.m_iPrice = atoi(stXML.GetAttrib("now_price").c_str());

			int iPos = 1;
			char szAwardType[64], szAwardID[64], szAwardAmount[64];
			while (true)
			{
				snprintf(szAwardType, sizeof(szAwardType), "award_type_%d", iPos);
				if (true == stXML.GetAttrib(szAwardType).empty())
					break;
				snprintf(szAwardID, sizeof(szAwardID), "award_id_%d", iPos);
				snprintf(szAwardAmount, sizeof(szAwardAmount), "award_amount_%d", iPos);

				CPackGameItem stBonus;
				stBonus.m_iItemType = ::atoi(stXML.GetAttrib(szAwardType).c_str());
				stBonus.m_iCardID = ::atoi(stXML.GetAttrib(szAwardID).c_str());
				stBonus.m_iNum = ::atoi(stXML.GetAttrib(szAwardAmount).c_str());
				stElem.m_stBonusList.push_back(stBonus);
				iPos++;
			}
			m_stVipBagMap[iVipLevel] = stElem;
		}
		stXML.OutOfElem();

        if (false == stXML.FindElem("quarter_bag"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("award"))
        {
            std::vector<CPackGameItem> stElem;

            int iVipLevel = atoi(stXML.GetAttrib("vip_level").c_str());

            int iPos = 1;
            char szAwardType[64], szAwardID[64], szAwardAmount[64];
            while (true)
            {
                snprintf(szAwardType, sizeof(szAwardType), "award_type_%d", iPos);
                if (true == stXML.GetAttrib(szAwardType).empty())
                    break;
                snprintf(szAwardID, sizeof(szAwardID), "award_id_%d", iPos);
                snprintf(szAwardAmount, sizeof(szAwardAmount), "award_amount_%d", iPos);

                CPackGameItem stBonus;
                stBonus.m_iItemType = ::atoi(stXML.GetAttrib(szAwardType).c_str());
                stBonus.m_iCardID = ::atoi(stXML.GetAttrib(szAwardID).c_str());
                stBonus.m_iNum = ::atoi(stXML.GetAttrib(szAwardAmount).c_str());
                stElem.push_back(stBonus);
                ++iPos;
            }
            m_stVipQuarterBagMap[iVipLevel] = stElem;
        }
        stXML.OutOfElem();
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicVipBagElem*
CLogicConfigVipBagParser::GetVipBagConfigElem(int iVipLevel) const
{
	auto stIter = m_stVipBagMap.find(iVipLevel);
	if (stIter == m_stVipBagMap.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INVALID_VIP_LEVEL_BAG);
		return nullptr;
	}
	return (&(stIter->second));
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string> CLogicConfigQuestionnaireParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if (true == strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if (false == stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        if (false == stXML.FindElem("root"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("questionnaire"))
        {
            int32_t iID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION(0 < iID && iID < 400);

            stXML.IntoElem();

            TLogicQuestionnaireElem stElem;
            CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
            if (!try_time_from_string(stXML.GetAttrib("expired_date"), stElem.m_iExpiredDateTime))
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "expired_date format error" << std::endl;
                throw std::exception();
            }

            stElem.m_strName = stXML.GetAttrib("name");
            CONFIG_ASSERT_EXCEPTION(!stElem.m_strName.empty());

            stElem.m_strContent = stXML.GetAttrib("content");
            CONFIG_ASSERT_EXCEPTION(!stElem.m_strContent.empty());

            for (int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if (stXML.GetAttrib("open_type" + stStream.str()).empty()) break;

                TLogicQuestionnaireOpenType stOpenType;
                stOpenType.m_iOpenType = ::atoi(stXML.GetAttrib("open_type" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumQuestionnaireOpenType(stOpenType.m_iOpenType));

                if(stOpenType.m_iOpenType == CLogicConfigDefine::EQOT_Date)
                {
                    if (!try_time_from_string(stXML.GetAttrib("open_para" + stStream.str()), stOpenType.m_iOpenPara))
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "open_para format error" << std::endl;
                        throw std::exception();
                    }

                    std::string strParaEx = stXML.GetAttrib("open_para_ex" + stStream.str());
                    if (!strParaEx.empty() && !try_time_from_string(strParaEx, stOpenType.m_iOpenParaEx))
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "open_para_ex format error" << std::endl;
                        throw std::exception();
                    }
                }
                else
                {
                    stOpenType.m_iOpenPara = ::atoi(stXML.GetAttrib("open_para" + stStream.str()).c_str());
                    stOpenType.m_iOpenParaEx = ::atoi(stXML.GetAttrib("open_para_ex" + stStream.str()).c_str());
                }

                stElem.m_stOpenTypeList.push_back(stOpenType);
            }

            stElem.m_iDurationDays = ::atoi(stXML.GetAttrib("duration_days").c_str());
            CONFIG_ASSERT_EXCEPTION(stElem.m_iDurationDays >= 0);

            for (int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if (stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stGameItem;
                stGameItem.m_iItemType = atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stGameItem.m_iCardID = atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stGameItem.m_iNum = atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());

                stElem.m_stRewards.emplace_back(stGameItem);
            }

            CONFIG_ASSERT_EXCEPTION(stXML.FindElem("question_list"));
            stXML.IntoElem();
            while (stXML.FindElem("problem"))
            {
                TLogicQuestionnaireProblem stProblem;
                stProblem.m_iType = ::atoi(stXML.GetAttrib("type").c_str());
                CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumQuestionnaireProblemType(stProblem.m_iType));

                stProblem.m_bNotMustAnswer = (bool)::atoi(stXML.GetAttrib("not_must_answer").c_str());
                stProblem.m_iOtherIndex = ::atoi(stXML.GetAttrib("other_index").c_str());
                stProblem.m_iMaxChoice = ::atoi(stXML.GetAttrib("max_choice").c_str());
                stProblem.m_strTitle = stXML.GetAttrib("title");
                CONFIG_ASSERT_EXCEPTION(!stProblem.m_strTitle.empty());
                for (int i = 1; i <= E_SelectMaxNum; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if (stXML.GetAttrib("answer_" + stStream.str()).empty()) break;

                    std::string strAnswer = stXML.GetAttrib("answer_" + stStream.str());

                    stProblem.m_stAnswerList.push_back(strAnswer);
                }

                if(stProblem.m_iType == CLogicConfigDefine::EQPT_MultipleChoice)
                {
                    CONFIG_ASSERT_EXCEPTION(stProblem.m_iMaxChoice > 0);
                    CONFIG_ASSERT_EXCEPTION(stProblem.m_iMaxChoice <= (int32_t)stProblem.m_stAnswerList.size());
                }

                if(stProblem.m_iType == CLogicConfigDefine::EQPT_SingleChoice || stProblem.m_iType == CLogicConfigDefine::EQPT_MultipleChoice)
                    CONFIG_ASSERT_EXCEPTION(stProblem.m_stAnswerList.size() > 0);

                stElem.m_stQuestionList.push_back(stProblem);
            }
            stXML.OutOfElem();

            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION(stElem.m_stQuestionList.size() > 0);
            CONFIG_ASSERT_EXCEPTION(stElem.m_stQuestionList.size() <= E_ProblemMaxNum);

            m_stQuestionnaireMap[iID] = std::move(stElem);
        }
        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicQuestionnaireElem* CLogicConfigQuestionnaireParser::GetConfig(int32_t iID) const
{
    auto iter = m_stQuestionnaireMap.find(iID);
    return iter == m_stQuestionnaireMap.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string> CLogicConfigSignParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if (true == strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if (false == stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        if (false == stXML.FindElem("root"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("daily_sign"))
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            TLogicTodaySignReward stSignReward;

            int32_t iIndex = ::atoi(stXML.GetAttrib("index").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(iIndex == int(m_stDailySignVec.size() + 1), iIndex)

            stSignReward.m_strCardBag = stXML.GetAttrib("card_bag");
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(stSignReward.m_strCardBag), iIndex)

            stSignReward.m_iWeight = ::atoi(stXML.GetAttrib("weight").c_str());
            stSignReward.m_iNotify = ::atoi(stXML.GetAttrib("notify").c_str());
            m_stDailySignVec.push_back(stSignReward);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("total_sign"))
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int iTimes = ::atoi(stXML.GetAttrib("times").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(iTimes > 0, iTimes)
            CONFIG_ASSERT_EXCEPTION_EX(m_stTotalSignRewardMap.find(iTimes) == m_stTotalSignRewardMap.end(), iTimes)

            TLogicTotalSignReward stRewards;
            for(int i = 1; i < 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), iTimes)

                stRewards.m_stRewards.emplace_back(stItem);
            }

            m_stTotalSignRewardMap[iTimes] = stRewards;
        }
        stXML.OutOfElem();

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicTotalSignReward* CLogicConfigSignParser::GetTotalSignReward(int iTotalSign) const
{
    auto iter = m_stTotalSignRewardMap.find(iTotalSign);
    return iter == m_stTotalSignRewardMap.end() ? nullptr : &(iter->second);
}
