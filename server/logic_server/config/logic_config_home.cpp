#include "logic_config_home.h"
#include "Markup.h"
#include "logic_link_common.h"
#include "logic_config.h"

std::pair<bool, std::string> CLogicConfigHomeGoodsParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if (!stXML.SetDoc(strXMLContent) || !stXML.FindElem("home"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"))
        m_iDefaultFishingRod = ::atoi(stXML.GetAttrib("default_fishing_rod").c_str());
        auto pRod = CLogicConfig::GetItemConfig().GetConfig(m_iDefaultFishingRod);
        CONFIG_ASSERT_EXCEPTION((pRod && pRod->m_iSubType == CLogicConfigDefine::EIST_HomeFishingRod))

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("init_items"))
        stXML.IntoElem();
        while (stXML.FindElem("init_goods"))
        {
            int32_t iID = ::atoi(stXML.GetAttrib("id").c_str());

            TLogicHomeInitGoodsElem stElem;
            stElem.m_iItemId = ::atoi(stXML.GetAttrib("item_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetItemConfig().GetConfig(stElem.m_iItemId), stElem.m_iItemId)

            stElem.m_iInitPosX = ::atoi(stXML.GetAttrib("init_pos_x").c_str());

            stElem.m_iInitPosY = ::atoi(stXML.GetAttrib("init_pos_y").c_str());
            CONFIG_ASSERT_EXCEPTION(IsValidPos(stElem.m_iInitPosX,stElem.m_iInitPosY));

            stElem.m_cStat = ::atoi(stXML.GetAttrib("state").c_str());
            CONFIG_ASSERT_EXCEPTION(IsValidStat(stElem.m_cStat));

            stElem.m_cQuadrant = ::atoi(stXML.GetAttrib("quadrant").c_str());
            CONFIG_ASSERT_EXCEPTION(IsValidStat(stElem.m_cQuadrant));

            stElem.m_iSeatId = ::atoi(stXML.GetAttrib("seat_index").c_str());

            m_stHomeInitGoodsMap[iID] = std::move(stElem);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("home_type"))
        stXML.IntoElem();
        std::set<int32_t> stPosID;
        while (stXML.FindElem("index"))
        {
            int32_t iHomeType = ::atoi(stXML.GetAttrib("home_type").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumHomeType(iHomeType), iHomeType)

            TLogicHomeElem stElem;
            stElem.m_bIsInit = (bool)::atoi(stXML.GetAttrib("is_init").c_str());
            stElem.m_bIsExtend = (bool)::atoi(stXML.GetAttrib("is_extend").c_str());
            stElem.m_bIsCanDress = (bool)::atoi(stXML.GetAttrib("is_can_dress").c_str());
            stElem.m_iLimitHeroNum = ::atoi(stXML.GetAttrib("limit_hero_num").c_str());
            CONFIG_ASSERT_EXCEPTION_EX((stElem.m_iLimitHeroNum >= 0 && stElem.m_iLimitHeroNum <= E_MaxHomeCardNum), iHomeType)

            if(stElem.m_bIsInit)
            {
                stElem.m_iInitID = ::atoi(stXML.GetAttrib("init_pos").c_str());
                if(iHomeType == CLogicConfigDefine::EHT_Farmland)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iInitID == 1, iHomeType)
                }
                else
                {
                    int x = stElem.m_iInitID / E_UniqueIDBase;
                    int y = stElem.m_iInitID % E_UniqueIDBase;
                    CONFIG_ASSERT_EXCEPTION_EX((x > 0 && x < E_UniqueIDBase), iHomeType)
                    CONFIG_ASSERT_EXCEPTION_EX((y > 0 && y < E_UniqueIDBase), iHomeType)
                    CONFIG_ASSERT_EXCEPTION_EX(stPosID.find(stElem.m_iInitID) == stPosID.end(), iHomeType)
                    stPosID.insert(stElem.m_iInitID);
                }
            }

            stElem.m_iInitPageID = ::atoi(stXML.GetAttrib("init_wall").c_str());
            auto pPage = CLogicConfig::GetItemConfig().GetConfig(stElem.m_iInitPageID);
            CONFIG_ASSERT_EXCEPTION_EX((pPage && pPage->m_iSubType == CLogicConfigDefine::EIST_HomePage), iHomeType)

            stElem.m_iInitFloorPageID = ::atoi(stXML.GetAttrib("init_floor").c_str());
            auto pFloor = CLogicConfig::GetItemConfig().GetConfig(stElem.m_iInitFloorPageID);
            CONFIG_ASSERT_EXCEPTION_EX((pFloor && pFloor->m_iSubType == CLogicConfigDefine::EIST_HomeFloorBG), iHomeType)

            stElem.m_stBuildConsume.m_iItemType = CLogicConfigDefine::EXCHANGE_ITEM;
            stElem.m_stBuildConsume.m_iCardID = ::atoi(stXML.GetAttrib("build_consume_id").c_str());
            stElem.m_stBuildConsume.m_iNum = 1;
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stElem.m_stBuildConsume), iHomeType)

            std::string strStarList = stXML.GetAttrib("init_items");
            auto stStarVec = string_split(strStarList, "|");
            for(std::size_t i = 0; i < stStarVec.size(); ++i)
            {
                int iStar = ::atoi(stStarVec[i].c_str());
                CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::GetHomeGoodsConfig().GetHomeInitGoodsConfig(iStar), iStar,iHomeType);
                stElem.m_iInitGoodsVec.push_back(iStar);
            }
            m_stHomeMap[iHomeType] = std::move(stElem);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("items"))
        stXML.IntoElem();
        while (stXML.FindElem("goods"))
        {
            int32_t iID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetItemConfig().GetConfig(iID), iID)

            TLogicHomeGoodsElem stElem;
            stElem.m_iLimitHomeType = ::atoi(stXML.GetAttrib("limit_home_type").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iLimitHomeType >= 0, iID)
            if(stElem.m_iLimitHomeType > 0)
            {
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumHomeType(stElem.m_iLimitHomeType), iID)
            }

            stElem.m_iFuncType = ::atoi(stXML.GetAttrib("func_type").c_str());
            if(stElem.m_iFuncType > 0)
            {
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumHomeFuncGoodsType(stElem.m_iFuncType), iID)
            }

            stElem.m_iFuncPara = ::atoi(stXML.GetAttrib("func_para").c_str());
            if(stElem.m_iFuncType == CLogicConfigDefine::EHFGT_DiningTable)
            {
                CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iFuncPara > 0 && stElem.m_iFuncPara <= 20, iID)
            }

            m_stHomeGoodsMap[iID] = std::move(stElem);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("home_buff"))
        stXML.IntoElem();
        while(stXML.FindElem("home_buff"))
        {
            TLogicHomeGoodBuff buff;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("id"), buff.m_iBuffID));
            CONFIG_ASSERT_EXCEPTION(m_stHomeGoodBuffMap.find(buff.m_iBuffID) == m_stHomeGoodBuffMap.end());
            m_stHomeGoodBuffMap[buff.m_iBuffID] = buff;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("home_good_buff"))
        stXML.IntoElem();
        while(stXML.FindElem("good_buff"))
        {
            CPackHomeGoodsBuffEffect buffEffect;
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("good_id"), buffEffect.m_iGoodID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("buff_id"), buffEffect.m_iBuffID));
            CONFIG_ASSERT_EXCEPTION(try_str2num(stXML.GetAttrib("param1"), buffEffect.m_fParam1));
            CONFIG_ASSERT_EXCEPTION(m_stHomeGoodBuffMap.find(buffEffect.m_iGoodID) == m_stHomeGoodBuffMap.end());
            CONFIG_ASSERT_EXCEPTION(buffEffect.m_fParam1 > 0 && buffEffect.m_iGoodID > 0 && buffEffect.m_iBuffID > 0);
            m_stHomeGoodBuffEffectMap[buffEffect.m_iGoodID] = buffEffect;
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

const TLogicHomeElem* CLogicConfigHomeGoodsParser::GetHomeConfig(int32_t iHomeType) const
{
    auto iter = m_stHomeMap.find(iHomeType);
    return iter == m_stHomeMap.end() ? nullptr : &(iter->second);
}

const TLogicHomeGoodsElem* CLogicConfigHomeGoodsParser::GetHomeGoodsConfig(int32_t iID) const
{
    auto iter = m_stHomeGoodsMap.find(iID);
    return iter == m_stHomeGoodsMap.end() ? nullptr : &(iter->second);
}

const TLogicHomeInitGoodsElem* CLogicConfigHomeGoodsParser::GetHomeInitGoodsConfig(int32_t iID) const
{
    auto iter = m_stHomeInitGoodsMap.find(iID);
    return iter == m_stHomeInitGoodsMap.end() ? nullptr : &(iter->second);
}


std::pair<bool, std::string> CLogicConfigHomeFuncParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if (!stXML.SetDoc(strXMLContent) || !stXML.FindElem("home"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"))
        m_strBanquetMailSender = stXML.GetAttrib("banquet_mail_send");
        m_strBanquetMailTitle = stXML.GetAttrib("banquet_mail_title");
        m_strBanquetMailContent = stXML.GetAttrib("banquet_mail_content");
        m_strBanquetMailOtherContent = stXML.GetAttrib("banquet_mail_content_other");

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("fishing"))
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iFishID = ::atoi(stXML.GetAttrib("fish_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX((iFishID > 0 && m_stFishMap.find(iFishID) == m_stFishMap.end()), iFishID)

            TLogicHomeFishElem stElem;
            stElem.m_iValueMin = ::atoi(stXML.GetAttrib("value_min").c_str());
            stElem.m_iValueMax = ::atoi(stXML.GetAttrib("value_max").c_str());
            stElem.m_iItemID = ::atoi(stXML.GetAttrib("item_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX((stElem.m_iValueMin > 0 && stElem.m_iValueMax >= stElem.m_iValueMin), iFishID)
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetItemConfig().GetConfig(stElem.m_iItemID), iFishID)

            for(int i = 1; i < 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("grade_num" + stStream.str()).empty()) break;

                char cGrade = ::atoi(stXML.GetAttrib("grade_num" + stStream.str()).c_str());
                int iMaxValue = ::atoi(stXML.GetAttrib("grade_max_value" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardGradeType(cGrade), iFishID)
                CONFIG_ASSERT_EXCEPTION_EX(iMaxValue >= stElem.m_iValueMin, iFishID)
                stElem.m_stValueToGradeMap[iMaxValue] = cGrade;
            }

            CONFIG_ASSERT_EXCEPTION_EX(!stElem.m_stValueToGradeMap.empty(), iFishID)

            stElem.m_stSellPrice.resize(CLogicConfigDefine::ECGT_Max);
            for(int i = CLogicConfigDefine::ECGT_Invalid + 1; i < CLogicConfigDefine::ECGT_Max; ++i)
            {
                std::stringstream stStream;
                stStream << i;

                int iPrice = ::atoi(stXML.GetAttrib("sell_grade" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iPrice >= 0, iFishID)

                stElem.m_stSellPrice[i] = iPrice;
            }

            m_stFishMap[iFishID] = std::move(stElem);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("fishpond"))
        stXML.IntoElem();
        while (stXML.FindElem("room"))
        {
            int32_t iPos = ::atoi(stXML.GetAttrib("pos").c_str());

            stXML.IntoElem();

            CONFIG_ASSERT_EXCEPTION_EX(stXML.FindElem("info"), iPos)
            TLogicHomeFishpond stFishpond;
            stFishpond.m_strBag = stXML.GetAttrib("bag");
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(stFishpond.m_strBag), iPos)
            stFishpond.m_iBagTimes = ::atoi(stXML.GetAttrib("bag_times").c_str());

            CONFIG_ASSERT_EXCEPTION_EX(stXML.FindElem("fish_list"), iPos)
            stXML.IntoElem();
            while (stXML.FindElem("index"))
            {
                int32_t iFishID = ::atoi(stXML.GetAttrib("fish_id").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(m_stFishMap.find(iFishID) != m_stFishMap.end(), iPos)
                CONFIG_ASSERT_EXCEPTION_EX(stFishpond.m_stBigWeight.find(iFishID) == stFishpond.m_stBigWeight.end(), iPos)

                int iWeight = ::atoi(stXML.GetAttrib("weight").c_str());
                int iBigWeight = ::atoi(stXML.GetAttrib("big_weight").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iWeight > 0, iPos)
                CONFIG_ASSERT_EXCEPTION_EX(iBigWeight >= 0, iPos)

                stFishpond.m_stFishWeight.m_stWeight[iFishID] = iWeight;
                stFishpond.m_stFishWeight.m_iTotalWeight += iWeight;
                stFishpond.m_stBigWeight[iFishID] = iBigWeight;
            }
            stXML.OutOfElem();

            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION_EX(!stFishpond.m_stBigWeight.empty(), iPos)

            m_stFishpondMap[iPos] = std::move(stFishpond);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("fish_rod"))
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int iRodID = ::atoi(stXML.GetAttrib("rod_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetItemConfig().GetConfig(iRodID), iRodID)

            TLogicFishRodElem stElem;
            for(int i = 1; i <= 10; ++i)
            {
                auto strTag = std::to_string(i);
                if(stXML.GetAttrib("func_type" + strTag).empty()) break;

                int iType = ::atoi(stXML.GetAttrib("func_type" + strTag).c_str());
                int iWeight = ::atoi(stXML.GetAttrib("weight" + strTag).c_str());
                int iFishID = ::atoi(stXML.GetAttrib("fish_id" + strTag).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(IsValidEnumRodFunType(iType), iRodID)
                CONFIG_ASSERT_EXCEPTION_EX(iWeight > 0, iRodID)
                if(iFishID > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(m_stFishMap.find(iFishID) != m_stFishMap.end(), iRodID)
                }

                stElem.m_stRodFuncMap.emplace(iType, std::make_pair(iWeight, iFishID));
            }

            m_stFishRodMap[iRodID] = ::move(stElem);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("cookbook"))
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iCookID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(m_stCookBookMap.find(iCookID) == m_stCookBookMap.end(), iCookID)

            TLogicHomeCookBookElem stElem;
            stElem.m_iUnlockItem = ::atoi(stXML.GetAttrib("unlock_item").c_str());
            if(stElem.m_iUnlockItem > 0)
            {
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetItemConfig().GetConfig(stElem.m_iUnlockItem), iCookID)
            }

            for(int i = 1; i <= 5; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("fish" + stStream.str()).empty()) break;

                int32_t iFishID = ::atoi(stXML.GetAttrib("fish" + stStream.str()).c_str());
                char cGrade = ::atoi(stXML.GetAttrib("fish_grade" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EXEX(m_stFishMap.find(iFishID) != m_stFishMap.end(), iCookID, iFishID)
                if(cGrade > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfigDefine::IsValidEnumCardGradeType(cGrade), iCookID, iFishID)
                }

                stElem.m_stNeedFish.push_back(std::make_pair(iFishID, cGrade));
            }

            for(int i = 1; i <= 5; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("food_type" + stStream.str()).empty()) break;

                int iFoodType = ::atoi(stXML.GetAttrib("food_type" + stStream.str()).c_str());
                char cGrade = ::atoi(stXML.GetAttrib("food_grade" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iFoodType > 0, iCookID)
                if(cGrade > 0)
                {
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfigDefine::IsValidEnumCardGradeType(cGrade), iCookID, iFoodType)
                }
                stElem.m_stFoodTypeVec.push_back(std::make_pair(iFoodType, cGrade));
            }

            auto strList = stXML.GetAttrib("reward_list");
            auto stDataList = string_split(strList, "|");
            for(auto& para : stDataList)
            {
                int iItemID = ::atoi(para.c_str());
                auto pItemConfig = CLogicConfig::GetItemConfig().GetConfig(iItemID);
                CONFIG_ASSERT_EXCEPTION_EX(pItemConfig, iCookID)

                stElem.m_stRewardGradeToItemID[pItemConfig->m_cGrade] = iItemID;
            }
            CONFIG_ASSERT_EXCEPTION_EX(!stElem.m_stRewardGradeToItemID.empty(), iCookID)

            m_stCookBookMap[iCookID] = std::move(stElem);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("banquet_item"))
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int iItemID = ::atoi(stXML.GetAttrib("item_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(m_stBanquetItemMap.find(iItemID) == m_stBanquetItemMap.end(), iItemID)
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetItemConfig().GetConfig(iItemID), iItemID)

            TLogicBanquetItemElem stElem;
            stElem.m_iBanquetTime = ::atoi(stXML.GetAttrib("banquet_time").c_str());
            stElem.m_iRewardTime = ::atoi(stXML.GetAttrib("reward_time").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iBanquetTime > 0, iItemID)
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iRewardTime > 0, iItemID)

            for(int i = 1; i < 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("owner_reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("owner_reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("owner_reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("owner_reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), iItemID)

                stElem.m_stOwnerRewards.emplace_back(stItem);
            }

            for(int i = 1; i < 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::IsValidGameItem(stItem), iItemID)

                stElem.m_stRewards.emplace_back(stItem);
            }

            m_stBanquetItemMap[iItemID] = std::move(stElem);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("hot_spring"))
        m_iHotSpringDuration = ::atoi(stXML.GetAttrib("duration").c_str());
        m_iFriendLimit = ::atoi(stXML.GetAttrib("friend_limit").c_str());
        m_iHotSpringSelfReward = ::atoi(stXML.GetAttrib("self_reward").c_str());
        m_iHotSpringFriendReward = ::atoi(stXML.GetAttrib("friend_reward").c_str());
        m_iHotSpringInvitedFriendReward = ::atoi(stXML.GetAttrib("invite_friend_reward").c_str());

        m_strHotSpringEmailSender = stXML.GetAttrib("email_sender");
        m_strHotSpringEmailTitle = stXML.GetAttrib("email_title");
        m_strHotSpringEmailContent = stXML.GetAttrib("email_content");

        m_strHotSpringEmailSenderOther = stXML.GetAttrib("email_sender_other");
        m_strHotSpringEmailTitleOther = stXML.GetAttrib("email_title_other");
        m_strHotSpringEmailContentOther = stXML.GetAttrib("email_content_other");

        m_strHotSpringEmailSenderSelf = stXML.GetAttrib("email_sender_self");
        m_strHotSpringEmailTitleSelf = stXML.GetAttrib("email_title_self");
        m_strHotSpringEmailContentSelf = stXML.GetAttrib("email_content_self");

        m_iHotSpringEmailReward =  ::atoi(stXML.GetAttrib("email_reward").c_str());
        m_fStartTime = ::atof(stXML.GetAttrib("start_time").c_str());
        m_fEndTime = ::atof(stXML.GetAttrib("end_time").c_str());
        m_iExpiredTimeDay = ::atoi(stXML.GetAttrib("email_expired").c_str());
        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicHomeFishElem* CLogicConfigHomeFuncParser::GetFishConfig(int iFishID) const
{
    auto iter = m_stFishMap.find(iFishID);
    return iter == m_stFishMap.end() ? nullptr : &(iter->second);
}

int CLogicConfigHomeFuncParser::GetFishSellPrice(int iFishID, int iGrade) const
{
    if(CLogicConfigDefine::IsValidEnumCardGradeType(iGrade))
    {
        auto pConfig = GetFishConfig(iFishID);
        if(pConfig)
        {
            return pConfig->m_stSellPrice[iGrade];
        }
    }

    return 0;
}

const TLogicHomeFishpond* CLogicConfigHomeFuncParser::GetFishpondConfig(int iPos) const
{
    auto iter = m_stFishpondMap.find(iPos);
    return iter == m_stFishpondMap.end() ? nullptr : &(iter->second);
}

const TLogicFishRodElem* CLogicConfigHomeFuncParser::GetFishRodConfig(int iRodID) const
{
    auto iter = m_stFishRodMap.find(iRodID);
    return iter == m_stFishRodMap.end() ? nullptr : &(iter->second);
}

const TLogicHomeCookBookElem* CLogicConfigHomeFuncParser::GetCookBookConfig(int iCookID) const
{
    auto iter = m_stCookBookMap.find(iCookID);
    return iter == m_stCookBookMap.end() ? nullptr : &(iter->second);
}

const TLogicBanquetItemElem* CLogicConfigHomeFuncParser::GetBanquetItemConfig(int iItemID) const
{
    auto iter = m_stBanquetItemMap.find(iItemID);
    return iter == m_stBanquetItemMap.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string> CLogicConfigHomeGardenExploreParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if (!stXML.SetDoc(strXMLContent) || !stXML.FindElem("home"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
        m_strGardenOpBag = stXML.GetAttrib("garden_op_bag");
        m_iFarmlandNum = ::atoi(stXML.GetAttrib("farmland_num").c_str());
        m_iOpConsumeAction = ::atoi(stXML.GetAttrib("op_consume_action").c_str());
        m_iOpRewardPoint = ::atoi(stXML.GetAttrib("op_reward_point").c_str());
        m_iExploreNumMin = ::atoi(stXML.GetAttrib("explore_num_min").c_str());
        m_iExploreNumMax = ::atoi(stXML.GetAttrib("explore_num_max").c_str());
        m_strExploreMailSender = stXML.GetAttrib("send");
        m_strExploreMailTitle = stXML.GetAttrib("title");
        m_strExploreMailContent = stXML.GetAttrib("content");
        CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetCardbagConfig().GetConfig(m_strGardenOpBag));
        CONFIG_ASSERT_EXCEPTION(m_iFarmlandNum > 0)
        CONFIG_ASSERT_EXCEPTION(m_iOpConsumeAction > 0)
        CONFIG_ASSERT_EXCEPTION(m_iOpRewardPoint > 0)
        CONFIG_ASSERT_EXCEPTION(0 <= m_iExploreNumMin && m_iExploreNumMin <= m_iExploreNumMax)

        // 探索等级
        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("explore_level"))
        std::set<int> stAllQuestStarVec;
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int iExp = ::atoi(stXML.GetAttrib("total_exp").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(iExp >= 0, iExp)
            CONFIG_ASSERT_EXCEPTION_EX(m_stExploreLevelMap.find(iExp) == m_stExploreLevelMap.end(), iExp)

            if(m_iMaxExploreExp < iExp) m_iMaxExploreExp = iExp;

            TLogicWeightElem stLevel;
            std::string strStarList = stXML.GetAttrib("star_list");
            std::string strWeightList = stXML.GetAttrib("weight_list");
            auto stStarVec = string_split(strStarList, "|");
            auto stWeightVec = string_split(strWeightList, "|");
            CONFIG_ASSERT_EXCEPTION_EX(stStarVec.size() == stWeightVec.size() && !stStarVec.empty(), iExp)
            for(std::size_t i = 0; i < stStarVec.size(); ++i)
            {
                int iStar = ::atoi(stStarVec[i].c_str());
                int iWeight = ::atoi(stWeightVec[i].c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iWeight > 0, iExp)
                CONFIG_ASSERT_EXCEPTION_EX(stLevel.m_stWeight.find(iStar) == stLevel.m_stWeight.end(), iExp)

                stLevel.m_iTotalWeight += iWeight;
                stLevel.m_stWeight[iStar] = iWeight;

                stAllQuestStarVec.insert(iStar);
            }

            m_stExploreLevelMap[iExp] = stLevel;
        }
        stXML.OutOfElem();

        // 探索任务
        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("explore"))
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int iQuestID = ::atoi(stXML.GetAttrib("id").c_str());
            int iStar = ::atoi(stXML.GetAttrib("star").c_str());
            int iWeight = ::atoi(stXML.GetAttrib("weight").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(iQuestID > 0, iQuestID)
            CONFIG_ASSERT_EXCEPTION_EX(m_stExploreMap.find(iQuestID) == m_stExploreMap.end(), iQuestID)
            CONFIG_ASSERT_EXCEPTION_EX(stAllQuestStarVec.find(iStar) != stAllQuestStarVec.end(), iQuestID)

            TLogicExploreQuestElem stElem;
            stElem.m_bIsCbt = (bool)::atoi(stXML.GetAttrib("is_cbt").c_str());

            if(stElem.m_bIsCbt)
            {
                // 藏宝图任务
                CONFIG_ASSERT_EXCEPTION_EX(iWeight <= 0, iQuestID)
            }
            else
            {
                // 普通可刷新任务
                CONFIG_ASSERT_EXCEPTION_EX(iWeight > 0, iQuestID)

                m_stExploreStarQuestMap[iStar].m_iTotalWeight += iWeight;
                m_stExploreStarQuestMap[iStar].m_stWeight[iQuestID] = iWeight;
            }

            stElem.m_iTime = ::atoi(stXML.GetAttrib("time").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iTime > 0, iQuestID)

            stElem.m_iCardLevel = ::atoi(stXML.GetAttrib("card_level").c_str());
            stElem.m_iCardGrade = ::atoi(stXML.GetAttrib("card_grade").c_str());
            stElem.m_iCardColor = ::atoi(stXML.GetAttrib("card_color").c_str());

            // 阵容限制
            for(int i = 1; i <= 5; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("limit_type" + stStream.str()).empty()) break;

                TLogicExploreCardLimit stLimit;
                stLimit.m_iLimitType = ::atoi(stXML.GetAttrib("limit_type" + stStream.str()).c_str());
                stLimit.m_iLimitPara = ::atoi(stXML.GetAttrib("limit_para" + stStream.str()).c_str());
                stLimit.m_iLimitParaEx = ::atoi(stXML.GetAttrib("limit_para_ex" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(IsValidEnumExploreLimit(stLimit.m_iLimitType), iQuestID)
                CONFIG_ASSERT_EXCEPTION_EX(stLimit.m_iLimitPara > 0, iQuestID)
                if(stLimit.m_iLimitType == EEL_CardTypeNum)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardType(stLimit.m_iLimitPara), iQuestID)
                    CONFIG_ASSERT_EXCEPTION_EX(stLimit.m_iLimitParaEx > 0, iQuestID)
                }
                else if(stLimit.m_iLimitType == EEL_CardID)
                {
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetHeroCardConfig().GetConfig(stLimit.m_iLimitPara), iQuestID)
                }

                stElem.m_stCardLimitVec.push_back(stLimit);
            }

            // 推荐伙伴
            for(int i = 1; i <= 5; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("recommend_card" + stStream.str()).empty()) break;

                TLogicExploreRecommend stRecommend;
                stRecommend.m_iCardID = ::atoi(stXML.GetAttrib("recommend_card" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetHeroCardConfig().GetConfig(stRecommend.m_iCardID), iQuestID)

                for(int j = 1; j < 10; ++j)
                {
                    std::stringstream stStreamEx;
                    stStreamEx << i << "_" << j;
                    if(stXML.GetAttrib("recommend_reward_type" + stStreamEx.str()).empty()) break;

                    CPackGameItem stItem;
                    stItem.m_iItemType = ::atoi(stXML.GetAttrib("recommend_reward_type" + stStreamEx.str()).c_str());
                    stItem.m_iCardID = ::atoi(stXML.GetAttrib("recommend_reward_id" + stStreamEx.str()).c_str());
                    stItem.m_iNum = ::atoi(stXML.GetAttrib("recommend_reward_num" + stStreamEx.str()).c_str());
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::IsValidGameItem(stItem), iQuestID, stItem.m_iCardID)

                    stRecommend.m_stRewards.emplace_back(stItem);
                }

                stRecommend.m_strCardBag = stXML.GetAttrib("recommend_cardbag" + stStream.str());
                if(!stRecommend.m_strCardBag.empty())
                {
                    CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(stRecommend.m_strCardBag), iQuestID)
                }

                CONFIG_ASSERT_EXCEPTION_EX(!stRecommend.m_stRewards.empty() || !stRecommend.m_strCardBag.empty(), iQuestID)

                stElem.m_stRecommendVec.push_back(stRecommend);
            }

            for(int i = 1; i < 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfig::IsValidGameItem(stItem), iQuestID, stItem.m_iCardID)

                stElem.m_stRewards.emplace_back(stItem);
            }

            stElem.m_strCardBag = stXML.GetAttrib("cardbag");
            if(!stElem.m_strCardBag.empty())
            {
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetCardbagConfig().GetConfig(stElem.m_strCardBag), iQuestID)
            }

            CONFIG_ASSERT_EXCEPTION_EX(!stElem.m_stRewards.empty() || !stElem.m_strCardBag.empty(), iQuestID)

            m_stExploreMap[iQuestID] = std::move(stElem);
        }
        stXML.OutOfElem();

        // 判断能随机到的所有星数是否都有任务配置
        for(auto& star_id : stAllQuestStarVec)
        {
            CONFIG_ASSERT_EXCEPTION_EX(m_stExploreStarQuestMap.find(star_id) != m_stExploreStarQuestMap.end(), star_id)
        }

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("garden"));
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iSeedID = ::atoi(stXML.GetAttrib("seed_id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetItemConfig().GetConfig(iSeedID), iSeedID);

            TLogicGardenElem stElem;
            stElem.m_stStatVec.push_back(TLogicSeedStatElem());
            int iLastOp = 0;
            for(int i = 1; i <= 20; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("time" + stStream.str()).empty()) break;

                TLogicSeedStatElem stStat;
                stStat.m_iTime = ::atoi(stXML.GetAttrib("time" + stStream.str()).c_str());
                stStat.m_iNeedOp = ::atoi(stXML.GetAttrib("need_op" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stStat.m_iTime > 0, iSeedID);
                CONFIG_ASSERT_EXCEPTION_EX(IsValidEnumGardenOp(stStat.m_iNeedOp), iSeedID);

                iLastOp = stStat.m_iNeedOp;
                stElem.m_stStatVec.emplace_back(stStat);
            }
            CONFIG_ASSERT_EXCEPTION_EX(iLastOp == EOT_Harvest, iSeedID);

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX((stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0), iSeedID);

                stElem.m_stRewards.emplace_back(stItem);
            }
            CONFIG_ASSERT_EXCEPTION_EX(!stElem.m_stRewards.empty(), iSeedID);

            m_stGardenMap[iSeedID] = std::move(stElem);
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

const TLogicExploreQuestElem* CLogicConfigHomeGardenExploreParser::GetExploreQuestConfig(int32_t iID) const
{
    auto iter = m_stExploreMap.find(iID);
    return iter == m_stExploreMap.end() ? nullptr : &(iter->second);
}

const TLogicWeightElem* CLogicConfigHomeGardenExploreParser::GetExploreLevelConfig(int32_t iExp) const
{
    if(m_stExploreLevelMap.empty()) return nullptr;

    auto iter = m_stExploreLevelMap.upper_bound(iExp);
    if(iter != m_stExploreLevelMap.begin())
        --iter;

    return &(iter->second);
}

const TLogicWeightElem* CLogicConfigHomeGardenExploreParser::GetExploreStarRandConfig(int32_t iStar) const
{
    auto iter = m_stExploreStarQuestMap.find(iStar);
    return iter == m_stExploreStarQuestMap.end() ? nullptr : &(iter->second);
}

const TLogicGardenElem* CLogicConfigHomeGardenExploreParser::GetGardenConfig(int32_t iID) const
{
    auto iter = m_stGardenMap.find(iID);
    return iter == m_stGardenMap.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string> CLogicConfigHeroDatingParser::Load(const std::string& strFileName, const std::string& strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if(strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if(false == stXML.SetDoc(strXMLContent) || false == stXML.FindElem("dating"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        if(false == stXML.FindElem("info"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        m_iImpressionMaxNum = ::atoi(stXML.GetAttrib("impression_max_num").c_str());

        if(false == stXML.FindElem("dating_item"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("item"))
        {
            TLogicDatingItem stDatingItem;
            int32_t iItemID = ::atoi(stXML.GetAttrib("id").c_str());
            stDatingItem.m_iHeartExp = ::atoi(stXML.GetAttrib("heart_exp").c_str());

            for (int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if (stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());

                stDatingItem.m_stRewards.push_back(stItem);
            }

            for (int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if (stXML.GetAttrib("impression_type" + stStream.str()).empty()) break;

                TLogicHeroImpression stImpression;
                stImpression.m_iImpressionType = ::atoi(stXML.GetAttrib("impression_type" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumHeroImpressionType(stImpression.m_iImpressionType));

                std::string strValue = stXML.GetAttrib("impression_num" + stStream.str());
                auto stValueList = string_split(strValue, "_");
                CONFIG_ASSERT_EXCEPTION(stValueList.size() == 2 || stValueList.size() == 1);
                stImpression.m_iImpressionMinNum = ::atoi(stValueList[0].c_str());
                CONFIG_ASSERT_EXCEPTION(stImpression.m_iImpressionMinNum > 0);

                if(stValueList.size() == 2)
                {
                    stImpression.m_iImpressionMaxNum = ::atoi(stValueList[1].c_str());
                    CONFIG_ASSERT_EXCEPTION(stImpression.m_iImpressionMinNum <= stImpression.m_iImpressionMaxNum);
                }
                else
                {
                    stImpression.m_iImpressionMaxNum = stImpression.m_iImpressionMinNum;
                }

                stDatingItem.m_stImpression.emplace_back(stImpression);
            }

            m_stDatingItemMap[iItemID] = stDatingItem;
        }
        stXML.OutOfElem();

        if(false == stXML.FindElem("dating_place"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("place"))
        {
            TLogicDatingPlace stPlace;
            uint8_t ucPlaceID = (uint8_t)::atoi(stXML.GetAttrib("id").c_str());
            stPlace.m_ucDailyDatingTimes = (uint8_t)::atoi(stXML.GetAttrib("daily_dating_times").c_str());
            stPlace.m_iHeartLevelLimitMin = ::atoi(stXML.GetAttrib("heart_level_limit_min").c_str());
            stPlace.m_iHeartLevelLimitMax = ::atoi(stXML.GetAttrib("heart_level_limit_max").c_str());
            CONFIG_ASSERT_EXCEPTION(stPlace.m_iHeartLevelLimitMin <= stPlace.m_iHeartLevelLimitMax);

            stPlace.m_iHeartExpMin = ::atoi(stXML.GetAttrib("heart_exp_min").c_str());
            stPlace.m_iHeartExpMax = ::atoi(stXML.GetAttrib("heart_exp_max").c_str());
            CONFIG_ASSERT_EXCEPTION(stPlace.m_iHeartExpMin <= stPlace.m_iHeartExpMax);

            for (int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if (stXML.GetAttrib("item_id" + stStream.str()).empty()) break;

                int32_t iItemID = ::atoi(stXML.GetAttrib("item_id" + stStream.str()).c_str());
                if(m_stDatingItemMap.find(iItemID) == m_stDatingItemMap.end())
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:Invalid ItemID:" << iItemID << "|PlaceID:" << ucPlaceID << std::endl;
                    throw std::exception();
                }

                stPlace.m_stDatingItems.push_back(iItemID);
            }

            m_stDatingPlaceMap[ucPlaceID] = stPlace;
        }
        stXML.OutOfElem();

        if(false == stXML.FindElem("dating_times_reward"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iCardID = ::atoi(stXML.GetAttrib("card_id").c_str());
            int32_t iTimes = ::atoi(stXML.GetAttrib("times").c_str());
            CONFIG_ASSERT_EXCEPTION(nullptr != CLogicConfig::GetHeroCardConfig().GetConfig(iCardID));
            CONFIG_ASSERT_EXCEPTION(iTimes > 0);

            std::vector<CPackGameItem> stRewardVec;
            for (int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if (stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());

                stRewardVec.push_back(stItem);
            }

            m_stDatingTimesReward[iCardID].insert(std::make_pair(iTimes, stRewardVec));
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

const TLogicDatingPlace* CLogicConfigHeroDatingParser::GetDatingPlaceConfig(uint8_t ucPlaceID) const
{
    auto iter = m_stDatingPlaceMap.find(ucPlaceID);
    return iter == m_stDatingPlaceMap.end() ? nullptr : &(iter->second);
}

const TLogicDatingItem* CLogicConfigHeroDatingParser::GetDatingItemConfig(int32_t iItemID) const
{
    auto iter = m_stDatingItemMap.find(iItemID);
    return iter == m_stDatingItemMap.end() ? nullptr : &(iter->second);
}

const std::vector<CPackGameItem>* CLogicConfigHeroDatingParser::GetDatingTimesReward(int32_t iCardID, int32_t iTimes) const
{
    auto iterCard = m_stDatingTimesReward.find(iCardID);
    if(iterCard != m_stDatingTimesReward.end())
    {
        auto iterTimes = iterCard->second.find(iTimes);
        if(iterTimes != iterCard->second.end())
            return &(iterTimes->second);
    }

    return nullptr;
}

std::pair<bool, std::string> CLogicConfigHeroDatingDialogParser::Load(const std::string& strFileName, const std::string& strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if(strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if(false == stXML.SetDoc(strXMLContent) || false == stXML.FindElem("questions"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("hero"))
        {
            int32_t iCardID = ::atoi(stXML.GetAttrib("hero_id").c_str());
            CONFIG_ASSERT_EXCEPTION(nullptr != CLogicConfig::GetHeroCardConfig().GetConfig(iCardID));

            stXML.IntoElem();
            while (stXML.FindElem("dating_place"))
            {
                int32_t iPlaceID = ::atoi(stXML.GetAttrib("place_id").c_str());
                CONFIG_ASSERT_EXCEPTION(nullptr != CLogicConfig::GetHeroDatingConfig().GetDatingPlaceConfig(iPlaceID));

                stXML.IntoElem();
                while (stXML.FindElem("dialog"))
                {
                    int32_t iDialogID = ::atoi(stXML.GetAttrib("dialog_id").c_str());

                    stXML.IntoElem();
                    if(false == stXML.FindElem("info"))
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << "|CARD_ID:" << iCardID << std::endl;
                        throw std::exception();
                    }

                    stXML.IntoElem();
                    TLogicDatingDialog stDialog;
                    while (stXML.FindElem("select_answer"))
                    {
                        int32_t iIndex = ::atoi(stXML.GetAttrib("index").c_str());

                        for (int i = 1; i <= 10; ++i)
                        {
                            std::stringstream stStream;
                            stStream << i;
                            if (stXML.GetAttrib("impression_type" + stStream.str()).empty()) break;

                            TLogicHeroImpression stImpression;
                            stImpression.m_iImpressionType = ::atoi(stXML.GetAttrib("impression_type" + stStream.str()).c_str());
                            CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumHeroImpressionType(stImpression.m_iImpressionType));
                            stImpression.m_iImpressionMinNum = ::atoi(stXML.GetAttrib("impression_value" + stStream.str()).c_str());
                            CONFIG_ASSERT_EXCEPTION(stImpression.m_iImpressionMinNum > 0);
                            stImpression.m_iImpressionMaxNum = stImpression.m_iImpressionMinNum;

                            stDialog.m_stOptionImpression[iIndex].emplace_back(stImpression);
                        }
                    }
                    stXML.OutOfElem();

                    m_stDatingDialogMap[iCardID][iPlaceID][iDialogID] = stDialog;

                    stXML.OutOfElem();
                }
                stXML.OutOfElem();
            }
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

const TLogicDatingDialog* CLogicConfigHeroDatingDialogParser::GetConfig(int32_t iHeroID, int32_t iPlaceID, int32_t iDialogID) const
{
    auto iterCard = m_stDatingDialogMap.find(iHeroID);
    if(iterCard != m_stDatingDialogMap.end())
    {
        auto iterPlace = iterCard->second.find(iPlaceID);
        if(iterPlace != iterCard->second.end())
        {
            auto iterDialog = iterPlace->second.find(iDialogID);
            if(iterDialog != iterPlace->second.end())
            {
                return &(iterDialog->second);
            }
        }
    }

    return nullptr;
}

std::pair<bool, std::string> CLogicConfigSimpleGameParser::Load(const std::string& strFileName, const std::string& strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if(strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if(false == stXML.SetDoc(strXMLContent) || false == stXML.FindElem("root"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        if(false == stXML.FindElem("info"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        m_iFlyArrayRewardMaxNum = ::atoi(stXML.GetAttrib("fly_array_reward_max_num").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iFlyArrayRewardMaxNum <= 60);

        if(false == stXML.FindElem("simple_game"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("info"))
        {
            TLogicSimpleGameInfo stInfo;
            int32_t iID = ::atoi(stXML.GetAttrib("id").c_str());
            // CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumSimpleGameType(iID));

            stInfo.m_iLevelLimit = ::atoi(stXML.GetAttrib("limit_level").c_str());
            stInfo.m_iFreeTimes = ::atoi(stXML.GetAttrib("free_times").c_str());
            stInfo.m_iRewardGroupType = ::atoi(stXML.GetAttrib("reward_group_type").c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumSimpleGameRewardType(stInfo.m_iRewardGroupType));

            stInfo.m_iRewardGroupID = ::atoi(stXML.GetAttrib("reward_group_id").c_str());
            stInfo.m_bIsAddupReward = (bool)::atoi(stXML.GetAttrib("is_addup_reward").c_str());
            stInfo.m_stConsumeItem.m_iItemType = ::atoi(stXML.GetAttrib("consume_type").c_str());
            stInfo.m_stConsumeItem.m_iCardID = ::atoi(stXML.GetAttrib("consume_id").c_str());
            stInfo.m_stConsumeItem.m_iNum = ::atoi(stXML.GetAttrib("consume_num").c_str());

            m_stSimpleGameMap[iID] = stInfo;
        }
        stXML.OutOfElem();

        if(false == stXML.FindElem("point_reward"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("group"))
        {
            std::vector<TLogicSimpleGamePointReward> stRewardVec;
            int32_t iID = ::atoi(stXML.GetAttrib("id").c_str());
            stXML.IntoElem();
            while(stXML.FindElem("point"))
            {
                TLogicSimpleGamePointReward stReward;
                stReward.m_iPointMin = ::atoi(stXML.GetAttrib("point_min").c_str());
                stReward.m_iPointMax = ::atoi(stXML.GetAttrib("point_max").c_str());
                CONFIG_ASSERT_EXCEPTION(stReward.m_iPointMin <= stReward.m_iPointMax);

                for (int i = 1; i <= 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << i;
                    if (stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                    CPackGameItem stItem;
                    stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                    stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                    stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());

                    stReward.m_stRewards.push_back(stItem);
                }

                stRewardVec.emplace_back(stReward);
            }
            stXML.OutOfElem();

            m_stPointRewardMap[iID] = stRewardVec;
        }
        stXML.OutOfElem();

        if(false == stXML.FindElem("victory_reward"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("group"))
        {
            TLogicSimpleGameVictoryReward stRewards;
            int32_t iID = ::atoi(stXML.GetAttrib("id").c_str());

            for (int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if (stXML.GetAttrib("victory_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("victory_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("victory_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("victory_num" + stStream.str()).c_str());

                stRewards.m_stVictoryRewards.push_back(stItem);
            }

            for (int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if (stXML.GetAttrib("failed_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("failed_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("failed_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("failed_num" + stStream.str()).c_str());

                stRewards.m_stFailedRewards.push_back(stItem);
            }

            m_stVictoryRewardMap[iID] = stRewards;
        }
        stXML.OutOfElem();

        if(false == stXML.FindElem("fly_reward_bag"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("bag"))
        {
            std::vector<CPackGameItem> stRewardVec;
            stXML.IntoElem();
            while (stXML.FindElem("index"))
            {
                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type").c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id").c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num").c_str());

                stRewardVec.push_back(stItem);
            }
            stXML.OutOfElem();

            m_stFlyArrayRewardBag.emplace_back(stRewardVec);
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

const TLogicSimpleGameInfo* CLogicConfigSimpleGameParser::GetSimpleGameConfig(int32_t iGameID) const
{
    auto iter = m_stSimpleGameMap.find(iGameID);
    return iter == m_stSimpleGameMap.end() ? nullptr : &(iter->second);
}

const TLogicSimpleGameVictoryReward* CLogicConfigSimpleGameParser::GetVictoryRewardConfig(int32_t iGroupID) const
{
    auto iter = m_stVictoryRewardMap.find(iGroupID);
    return iter == m_stVictoryRewardMap.end() ? nullptr : &(iter->second);
}

std::vector<CPackGameItem> CLogicConfigSimpleGameParser::GetGameReward(int32_t iGameID, int32_t iPara) const
{
    std::vector<CPackGameItem> stRewardVec;

    const auto pGameConfig = GetSimpleGameConfig(iGameID);
    if(nullptr == pGameConfig) return stRewardVec;

    // 积分类奖励
    if(pGameConfig->m_iRewardGroupType == CLogicConfigDefine::ESGRT_PointReward)
    {
        auto iterPoint = m_stPointRewardMap.find(pGameConfig->m_iRewardGroupID);
        if(iterPoint == m_stPointRewardMap.end())
            return stRewardVec;

        for(auto& stReward : iterPoint->second)
        {
            if(stReward.m_iPointMin > iPara)
                break;

            if(pGameConfig->m_bIsAddupReward)
            {
                stRewardVec.insert(stRewardVec.end(), stReward.m_stRewards.begin(), stReward.m_stRewards.end());
            }
            else if(stReward.m_iPointMin <= iPara && iPara <= stReward.m_iPointMax)
            {
                stRewardVec = stReward.m_stRewards;
                break;
            }
        }
    }
    else if(pGameConfig->m_iRewardGroupType == CLogicConfigDefine::ESGRT_VictoryReward)
    {
        auto iterVic = m_stVictoryRewardMap.find(pGameConfig->m_iRewardGroupID);
        if(iterVic == m_stVictoryRewardMap.end())
            return stRewardVec;

        if(iPara)
        {
            stRewardVec = iterVic->second.m_stVictoryRewards;
        }
        else
        {
            stRewardVec = iterVic->second.m_stFailedRewards;
        }
    }

    return stRewardVec;
}

std::pair<bool, std::string> CLogicConfigHomeLevelExpParser::Load(const std::string& strFileName, const std::string& strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if(strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if(false == stXML.SetDoc(strXMLContent) || false == stXML.FindElem("home"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("home_level_reward"))
        stXML.IntoElem();
        while (stXML.FindElem("reward"))
        {
            TLogicHomeLeveReward levelReward;
            levelReward.m_iLevel = ::atoi(stXML.GetAttrib("level").c_str());
            levelReward.m_iNeedExp = ::atoi(stXML.GetAttrib("need_total_exp").c_str());

            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                CPackGameItem stItem;
                stItem.m_iItemType = ::atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stItem.m_iCardID = ::atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stItem.m_iNum = ::atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stItem.m_iItemType > 0 && stItem.m_iCardID > 0 && stItem.m_iNum > 0, levelReward.m_iLevel);
                levelReward.m_levelReward.emplace_back(stItem);
            }
            m_stLevel2Reward[levelReward.m_iLevel] = levelReward;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("exp_item"))
        stXML.IntoElem();
        while (stXML.FindElem("item"))
        {
            TLogicHomeItemExp itemExp;
            itemExp.m_itemID = ::atoi(stXML.GetAttrib("id").c_str());
            itemExp.m_iHomeExp = ::atoi(stXML.GetAttrib("home_exp").c_str());
            m_stLevel2ItemExp[itemExp.m_itemID] = itemExp;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION (stXML.FindElem("exp_fish"))
        stXML.IntoElem();
        while (stXML.FindElem("fish"))
        {
            TLogicHomeFishExp fishExp;
            fishExp.m_iFishID = ::atoi(stXML.GetAttrib("id").c_str());
            fishExp.m_iNeedGrade = ::atoi(stXML.GetAttrib("need_grade").c_str());
            fishExp.m_iHomeExp = ::atoi(stXML.GetAttrib("home_exp").c_str());
            m_stLevel2FishExp[fishExp.m_iFishID] = fishExp;
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


const TLogicHomeLeveReward* CLogicConfigHomeLevelExpParser::GetHomeLevelRewardConfig(int32_t level) const
{
    auto iterHomeLevelReward = m_stLevel2Reward.find(level);
    return (iterHomeLevelReward == m_stLevel2Reward.end()) ? nullptr : &(iterHomeLevelReward->second);
}
