#include "logic_config_card.h"
#include "Markup.h"
#include "server_errcode.h"
#include "logic_link_error.h"
#include "server_base_protocol.h"
#include "logic_config.h"
#include "logic_link_common.h"

////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigHeroCardParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	do
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			break;
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		if (false == stXML.FindElem("card_data"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

		while (stXML.FindElem("hero"))
		{
			TLogicHeroCardConfigElem stConfigElem;
			stConfigElem.m_iHeroCardID = atoi(stXML.GetAttrib("card_id").c_str());

			if (!CLogicConfigDefine::IsValidHeroCardItemID(stConfigElem.m_iHeroCardID)
                || m_stHeroCardMap.find(stConfigElem.m_iHeroCardID) != m_stHeroCardMap.end())
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:HERO_CARD_ID INVALID|HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << std::endl;
				break;
			}

			stXML.IntoElem();
			while (stXML.FindElem("ranks"))
			{
				std::string strTag = stXML.GetAttrib("tag");

				if ("main" == strTag)
				{
					stConfigElem.m_strHeroCardName = stXML.GetAttrib("card_name");
					stConfigElem.m_iQuality = atoi(stXML.GetAttrib("quality").c_str());
					stConfigElem.m_iInitStar = atoi(stXML.GetAttrib("init_star").c_str());
					stConfigElem.m_iInitColor = atoi(stXML.GetAttrib("init_color").c_str());
					stConfigElem.m_iInitGrade = atoi(stXML.GetAttrib("init_grade").c_str());
					stConfigElem.m_iCardType = ::atoi(stXML.GetAttrib("card_type").c_str());
                    if(!CLogicConfigDefine::IsValidEnumCardType(stConfigElem.m_iCardType))
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: Card Type Error! |HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << std::endl;
                        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
                    }

                    if(!CLogicConfigDefine::IsValidEnumCardGradeType(stConfigElem.m_iInitGrade))
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: Card grade Error! |HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << std::endl;
                        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
                    }

                    stConfigElem.m_iSoulID = ::atoi(stXML.GetAttrib("card_soul_id").c_str());
                    if(CLogicConfig::GetItemConfig().GetConfig(stConfigElem.m_iSoulID) == nullptr)
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: Card Soul Error! |HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << std::endl;
                        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
                    }

					stConfigElem.m_iSexual = ::atoi(stXML.GetAttrib("sexual").c_str());
					stConfigElem.m_iIsShow = atoi(stXML.GetAttrib("is_show").c_str());

					stConfigElem.m_stLevelUpAttr = TAddAttrFloatElem(
						atof(stXML.GetAttrib("hp_lv_up").c_str()),
						atof(stXML.GetAttrib("atk_lv_up").c_str()),
						atof(stXML.GetAttrib("def_lv_up").c_str()));

					continue;
				}
				// main 不能 IntoElem

				stXML.IntoElem();
				if ("star" == strTag)
				{
					stConfigElem.m_stStarUpAttr.push_back(TAddAttrElem());
					while (true == stXML.FindElem("star"))
					{
                        int32_t iHp = atoi(stXML.GetAttrib("hp").c_str());
                        int32_t iAtk = atoi(stXML.GetAttrib("atk").c_str());
                        int32_t iDef = atoi(stXML.GetAttrib("def").c_str());
                        if(!CLogicConfigDefine::IsValidCardAttrValue(iHp) ||
                            !CLogicConfigDefine::IsValidCardAttrValue(iAtk) ||
                            !CLogicConfigDefine::IsValidCardAttrValue(iDef))
                        {
                            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: Card Star Attr Error! |HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << std::endl;
                            return (std::pair<bool, std::string>(false, strErrorInfo.str()));
                        }

                        TAddAttrElem stStarElem;
                        stStarElem.m_stAttrVec.emplace_back(CLogicConfigDefine::ECAT_HP, iHp);
                        stStarElem.m_stAttrVec.emplace_back(CLogicConfigDefine::ECAT_Attack, iAtk);
                        stStarElem.m_stAttrVec.emplace_back(CLogicConfigDefine::ECAT_Defense, iDef);

						stConfigElem.m_stStarUpAttr.emplace_back(stStarElem);
					}
				}
				else if ("color" == strTag)
				{
                    int iCurColor = 1;
					while (true == stXML.FindElem("color"))
					{
                        int iColor = ::atoi(stXML.GetAttrib("color").c_str());
                        if(iColor != iCurColor)
                        {
                            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: Card Color Error! |HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << std::endl;
                            return (std::pair<bool, std::string>(false, strErrorInfo.str()));
                        }

                        ++iCurColor;

                        int32_t iAttrType = ::atoi(stXML.GetAttrib("attr_type").c_str());
                        int32_t iAttrValue = ::atoi(stXML.GetAttrib("attr_value").c_str());
                        if(!CLogicConfigDefine::IsValidEnumCardAttrType(iAttrType) || !CLogicConfigDefine::IsValidCardAttrValue(iAttrValue))
                        {
                            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: Card Color Attr Error! |HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << std::endl;
                            return (std::pair<bool, std::string>(false, strErrorInfo.str()));
                        }

                        stConfigElem.m_stRankUpAttr[iColor] = std::make_pair(iAttrType, iAttrValue);
					}
				}
                else if ("talent" == strTag)
                {
                    while (stXML.FindElem("index"))
                    {
                        int iTalentID = ::atoi(stXML.GetAttrib("talent_id").c_str());
                        if(stConfigElem.m_stTalentMap.find(iTalentID) != stConfigElem.m_stTalentMap.end() || iTalentID <= 0)
                        {
                            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: talent_id Error! |HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << "|TALENT_ID:" << iTalentID << std::endl;
                            return (std::pair<bool, std::string>(false, strErrorInfo.str()));
                        }

                        TCardTalentElem stTalent;
                        stTalent.m_iMaxLevel = ::atoi(stXML.GetAttrib("max_level").c_str());
                        stTalent.m_iConsumePoint = ::atoi(stXML.GetAttrib("consume_point").c_str());
                        stTalent.m_iConsumePoint1 = ::atoi(stXML.GetAttrib("consume_point_1").c_str());
                        stTalent.m_iNeedPoint = ::atoi(stXML.GetAttrib("need_point").c_str());
                        stTalent.m_iPreID1 = ::atoi(stXML.GetAttrib("pre_id1").c_str());
                        stTalent.m_iPreID2 = ::atoi(stXML.GetAttrib("pre_id2").c_str());

                        if(stTalent.m_iMaxLevel <= 0 || stTalent.m_iConsumePoint < 0 || stTalent.m_iConsumePoint1 < 0)
                        {
                            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: max_level or consume_point Error! |HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << "|TALENT_ID:" << iTalentID << std::endl;
                            return (std::pair<bool, std::string>(false, strErrorInfo.str()));
                        }

                        for(int i = 1; i <= 5; ++i)
                        {
                            std::stringstream stStream;
                            stStream << i;
                            if(stXML.GetAttrib("attr_type_" + stStream.str()).empty()) break;

                            int iAttrType = ::atoi(stXML.GetAttrib("attr_type_" + stStream.str()).c_str());
                            int iAttrValue = ::atoi(stXML.GetAttrib("attr_value_" + stStream.str()).c_str());
                            if(!CLogicConfigDefine::IsValidEnumCardAttrType(iAttrType) || !CLogicConfigDefine::IsValidCardAttrValue(iAttrValue))
                            {
                                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: Card talent Attr Error! |HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << "|TALENT_ID:" << iTalentID << std::endl;
                                return (std::pair<bool, std::string>(false, strErrorInfo.str()));
                            }

                            stTalent.m_stAttrVec.emplace_back(iAttrType, iAttrValue);
                        }

                        stConfigElem.m_stTalentMap[iTalentID] = std::move(stTalent);
                    }

                    if(stConfigElem.m_stTalentMap.size() > E_MaxTalentNum)
                    {
                        strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: Card talent too much! |HERO_CARD_ID:" << stConfigElem.m_iHeroCardID << std::endl;
                        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
                    }
                }
				stXML.OutOfElem();
			}
            
			stXML.OutOfElem();

            m_stHeroCardMap[stConfigElem.m_iHeroCardID] = std::move(stConfigElem);
		}

	} while (0);

	if (strErrorInfo.str().empty())
	{
		return (std::pair<bool, std::string>(true, "OK"));
	}
	else
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
}

const TLogicHeroCardConfigElem* CLogicConfigHeroCardParser::GetConfig(int iID) const
{
	if (CLogicConfigDefine::IsValidHeroCardItemID(iID))
	{
		auto iter = m_stHeroCardMap.find(iID);
		if(iter != m_stHeroCardMap.end())
		{
			return (&(iter->second));
		}
	}

	using namespace SERVER_ERRCODE;
	SET_ERR_INFO(HERO_CARD_IS_NOT_CONFIGED);
	return (nullptr);
}

const TCardTalentElem* CLogicConfigHeroCardParser::GetTalentConfig(int iCardID, int iTalentID) const
{
    auto pConfig = GetConfig(iCardID);
    if(nullptr == pConfig) return nullptr;

    auto iterTalent = pConfig->m_stTalentMap.find(iTalentID);
    return (iterTalent == pConfig->m_stTalentMap.end() ? nullptr : &(iterTalent->second));
}

///////////////////////////////////////////////////////////////////////////////////////////////
int CLogicConfigUpgradeHeroCardStarParser::UpgradeStarNeedSoul(int iStar) const
{
	if(iStar >= 0 && iStar < (int)m_stHeroCardStarUpgradeConfig.size())
	{
		return (m_stHeroCardStarUpgradeConfig[iStar].m_iNeedHeroSoul);
	}

	return (-1);
}

int CLogicConfigUpgradeHeroCardStarParser::UpgradeStarNeedGold(int iStar) const
{
    if(iStar >= 0 && iStar < (int)m_stHeroCardStarUpgradeConfig.size())
	{
		return (m_stHeroCardStarUpgradeConfig[iStar].m_iConsumeGold);
	}

	return (-1);
}

int CLogicConfigUpgradeHeroCardStarParser::GetHeroRepeatExchange(int iGrade) const
{
    if(CLogicConfigDefine::IsValidEnumCardGradeType(iGrade))
        return m_stSoulExchange[iGrade].m_iRepeatExchange;

    return 0;
}

std::string CLogicConfigUpgradeHeroCardStarParser::GetGradeName(int iGrade) const
{
    if(iGrade <= 0 || iGrade > (int)m_stGradeNameList.size())
        return "";

    return m_stGradeNameList[iGrade-1];
}

int CLogicConfigUpgradeHeroCardStarParser::GetSoulExchangeNum(int iGrade) const
{
    if(CLogicConfigDefine::IsValidEnumCardGradeType(iGrade))
        return m_stSoulExchange[iGrade].m_iExchangeNum;

    return 0;
}

std::pair<bool, std::string> CLogicConfigUpgradeHeroCardStarParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"))
        m_iSoulExchangeItem = ::atoi(stXML.GetAttrib("soul_exchange_item").c_str());
        CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetItemConfig().GetConfig(m_iSoulExchangeItem))

        for(int i = 1; i <= 10; ++i)
        {
            std::stringstream stStream;
            stStream << "grade" << i;
            if (stXML.GetAttrib(stStream.str()).empty()) break;

            m_stGradeNameList.push_back(stXML.GetAttrib(stStream.str()));
        }

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("card_soul"))
		stXML.IntoElem();
        int iNeedHeroSoul = 1;
        while (stXML.FindElem("star"))
        {
            TLogicUpgradeHeroCardStarElem stUpgradeHeroCardStarElem;
            stUpgradeHeroCardStarElem.m_iNeedHeroSoul = ::atoi(stXML.GetAttrib("card_soul").c_str());
            stUpgradeHeroCardStarElem.m_iConsumeGold = ::atoi(stXML.GetAttrib("star_up_consume_gold").c_str());
            CONFIG_ASSERT_EXCEPTION(stUpgradeHeroCardStarElem.m_iNeedHeroSoul >= 0 && stUpgradeHeroCardStarElem.m_iConsumeGold >= 0)

            m_stHeroCardStarUpgradeConfig.push_back(stUpgradeHeroCardStarElem);
            iNeedHeroSoul = stUpgradeHeroCardStarElem.m_iNeedHeroSoul;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(iNeedHeroSoul == 0)

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("soul_exchange"))
        stXML.IntoElem();
        m_stSoulExchange.resize(CLogicConfigDefine::ECGT_Max);
        while (stXML.FindElem("grade"))
        {
            int iGrade = ::atoi(stXML.GetAttrib("grade").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardGradeType(iGrade), iGrade)

            TLogicCardGradeSoulElem stElem;
            stElem.m_iExchangeNum = ::atoi(stXML.GetAttrib("exchange_num").c_str());
            stElem.m_iRepeatExchange = ::atoi(stXML.GetAttrib("repeat_exchange").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iExchangeNum > 0 && stElem.m_iRepeatExchange > 0, iGrade)

            m_stSoulExchange[iGrade] = stElem;
        }
        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigUpgradeHeroCardLevelParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
	CMarkup stXML;
	std::stringstream strErrorInfo;

	do
	{
		if (true == strXMLContent.empty())
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
			break;
		}

		if (false == stXML.SetDoc(strXMLContent))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		if (false == stXML.FindElem("card_exp"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			break;
		}

		stXML.IntoElem();

        int iCurLevel = 1;
		m_stConfig.push_back(TLogicCardLevelElem());
		while (stXML.FindElem("exp"))
		{
            int iLevel = atoi(stXML.GetAttrib("level").c_str());
            if(iLevel != iCurLevel++)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:UPGRADE CARD LEVEL INVALID|LEVEL:" << iLevel << std::endl;
                break;
            }

            TLogicCardLevelElem stElem;
			stElem.m_iCardExp = atoi(stXML.GetAttrib("card_exp").c_str());
			if (stElem.m_iCardExp < 0)
			{
				strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:UPGRADE CARD LEVEL CONFIG EXP INVALID|CARD_EXP:"
					<< stElem.m_iCardExp << "|LEVEL:" << iLevel << std::endl;
				break;
			}

			m_stConfig.push_back(stElem);
		}

	} while (0);

	if (strErrorInfo.str().empty())
	{
		return (std::pair<bool, std::string>(true, "OK"));
	}
	else
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}
}

const TLogicCardLevelElem* CLogicConfigUpgradeHeroCardLevelParser::GetConfig(int32_t iLevel) const
{
    if(iLevel <= 0) return nullptr;

    if(iLevel >= (int)m_stConfig.size()) return &(m_stConfig[m_stConfig.size() - 1]);

    return &(m_stConfig[iLevel]);
}

std::pair<int, int>
CLogicConfigUpgradeHeroCardLevelParser::UpgradeCardLevel(int iCurrentRoleLevel, int iCurrentCardLevel, int iCurrentCardExp, int iAddExp) const
{
	if (iCurrentRoleLevel >= (int)CLogicConfig::GetUserExpConfig().m_stUserExpConfig.size())
	{
		return std::make_pair(iCurrentCardLevel, iCurrentCardExp);
	}

	int iCardLevelLimit = CLogicConfig::GetUserExpConfig().m_stUserExpConfig[iCurrentRoleLevel].m_iCardLevelLimit;

	if (iCurrentCardLevel <= 0 || iCurrentCardLevel > iCardLevelLimit)
        return std::make_pair(iCurrentCardLevel, iCurrentCardExp);

    int iMaxLimitLevel = std::min(((int)m_stConfig.size() - 1), iCardLevelLimit);
    int iLevel = iCurrentCardLevel;
    int iExp = iCurrentCardExp + iAddExp;
    while (iLevel < iMaxLimitLevel && m_stConfig[iLevel].m_iCardExp <= iExp)
    {
        iExp -= m_stConfig[iLevel++].m_iCardExp;
    }

    if ((iLevel == iMaxLimitLevel) && (iExp > m_stConfig[iLevel].m_iCardExp))
    {
        iExp = m_stConfig[iLevel].m_iCardExp;
    }

	return std::make_pair(iLevel, iExp);
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string>
CLogicConfigUpgradeHeroCardColorParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

		if (false == stXML.FindElem("hero_upgrade_color"))
		{
			strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
			throw std::exception();
		}

        stXML.IntoElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"))

        m_iUpgradeTalentNum = ::atoi(stXML.GetAttrib("upgrade_talent_num").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iUpgradeTalentNum > 0)


        m_iUpgradeTalentNum1 = ::atoi(stXML.GetAttrib("upgrade_talent_num1").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iUpgradeTalentNum1 > 0)

        m_stCommonTalent.m_iItemType = CLogicConfigDefine::EXCHANGE_ITEM;
        m_stCommonTalent.m_iCardID = ::atoi(stXML.GetAttrib("common_talent_id").c_str());
        m_stCommonTalent.m_iNum = 1;
        CONFIG_ASSERT_EXCEPTION(CLogicConfig::IsValidGameItem(m_stCommonTalent))


        while (stXML.FindElem("card_type"))
        {
            int32_t iCardType = ::atoi(stXML.GetAttrib("tag").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardType(iCardType), iCardType)
            CONFIG_ASSERT_EXCEPTION_EX(m_stConfig.find(iCardType) == m_stConfig.end(), iCardType)

            int iCurColor = 1;
            stXML.IntoElem();
            while (stXML.FindElem("color"))
            {
                int32_t iColor = ::atoi(stXML.GetAttrib("color").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iCurColor == iColor, iCardType)
                ++iCurColor;

                TLogicUpgradeHeroCardColorConfigElem stData;
                stData.m_iLevelLimit = ::atoi(stXML.GetAttrib("level_limit").c_str());
                CONFIG_ASSERT_EXCEPTION_EXEX(stData.m_iLevelLimit >= 0, iCardType, iColor)
                stData.m_iConsumeGold = ::atoi(stXML.GetAttrib("consume_gold").c_str());
                CONFIG_ASSERT_EXCEPTION_EXEX(stData.m_iConsumeGold >= 0, iCardType, iColor)

                stData.m_stColorItemVec.push_back(std::make_pair(0, 0));
                for(int i = 1; i <= CLogicConfigDefine::ESD_CardColorUpPosNum; ++i)
                {
                    int32_t iItemID = ::atoi(stXML.GetAttrib("item" + to_string(i)).c_str());
                    int32_t iNum = ::atoi(stXML.GetAttrib("num" + to_string(i)).c_str());
                    CONFIG_ASSERT_EXCEPTION_EXEX(iItemID > 0 && iNum > 0, iCardType, iColor)

                    stData.m_stColorItemVec.emplace_back(iItemID, iNum);
                }

                stData.m_stItemAttrVec.push_back(std::make_pair(0, 0));
                for(int i = 1; i < CLogicConfigDefine::ESD_CardColorUpPosNum; ++i)
                {
                    int32_t iAttrType = ::atoi(stXML.GetAttrib("item_attr_type" + to_string(i)).c_str());
                    int32_t iAttrValue = ::atoi(stXML.GetAttrib("item_attr_value" + to_string(i)).c_str());
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfigDefine::IsValidEnumCardAttrType(iAttrType), iCardType, iColor)
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfigDefine::IsValidCardAttrValue(iAttrValue), iCardType, iColor)

                    stData.m_stItemAttrVec.emplace_back(iAttrType, iAttrValue);
                }

                for(int i = 1; i < 10; ++i)
                {
                    if(stXML.GetAttrib("attr_type" + to_string(i)).empty()) break;

                    int32_t iAttrType = ::atoi(stXML.GetAttrib("attr_type" + to_string(i)).c_str());
                    int32_t iAttrValue = ::atoi(stXML.GetAttrib("attr_value" + to_string(i)).c_str());
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfigDefine::IsValidEnumCardAttrType(iAttrType), iCardType, iColor)
                    CONFIG_ASSERT_EXCEPTION_EXEX(CLogicConfigDefine::IsValidCardAttrValue(iAttrValue), iCardType, iColor)

                    stData.m_stAttrVec.emplace_back(iAttrType, iAttrValue);
                }

                m_stConfig[iCardType][iColor] = std::move(stData);
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION_EX(!(m_stConfig[iCardType].empty()), iCardType)
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(!m_stConfig.empty())
	}
	catch (const std::exception& stEx)
	{
		return (std::pair<bool, std::string>(false, strErrorInfo.str()));
	}

	return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicUpgradeHeroCardColorConfigElem* CLogicConfigUpgradeHeroCardColorParser::GetConfig(int iCardType, int iColor) const
{
	auto iterCardType = m_stConfig.find(iCardType);
	if (iterCardType != m_stConfig.end())
	{
        auto iterColor = iterCardType->second.find(iColor);
        if(iterColor != iterCardType->second.end())
        {
            return &(iterColor->second);
        }

        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(HERO_CARD_COLOR_REACH_CONFIG_MAX);
        return (nullptr);
	}

	using namespace SERVER_ERRCODE;
	SET_ERR_INFO(CARD_TYPE_COLOR_NOT_CONFIGED);
	return (nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::string> CLogicConfigHeroHeartParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if (false == stXML.FindElem("hero_heart"))
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

        m_iDialogCardRandomNum = ::atoi(stXML.GetAttrib("dialog_card_random_num").c_str());

        if (false == stXML.FindElem("heart_exp"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }
        stXML.IntoElem();

        m_stLevelList.push_back(std::make_pair(0, 0));
        int32_t curLevel = 1;
        while (stXML.FindElem("exp"))
        {
            int32_t iLevel = (int32_t)atoi(stXML.GetAttrib("level").c_str());
            int32_t iExpValue = (int32_t)atoi(stXML.GetAttrib("exp_value").c_str());
            int32_t iGiftNum = (int32_t)atoi(stXML.GetAttrib("gift_num").c_str());

            if(iLevel != curLevel++)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: Level not find. Level:" << curLevel << std::endl;
                throw std::exception();
            }

            TLogicHeartLevelReward stAward;
            for (int32_t i = 1; i <= 10; ++i)
            {
                CPackGameItem stGameItem;
                std::stringstream stStream;
                stStream << "" << i;
                if (stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                stGameItem.m_iItemType = atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stGameItem.m_iCardID = atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stGameItem.m_iNum = atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());

                stAward.m_stDialogAward.emplace_back(stGameItem);
            }

            for(int32_t i = 1; i <= 10; ++i)
            {
                CPackGameItem stGameItem;
                std::stringstream stStream;
                stStream << "" << i;
                if (stXML.GetAttrib("levelup_type" + stStream.str()).empty()) break;

                stGameItem.m_iItemType = atoi(stXML.GetAttrib("levelup_type" + stStream.str()).c_str());
                stGameItem.m_iCardID = atoi(stXML.GetAttrib("levelup_id" + stStream.str()).c_str());
                stGameItem.m_iNum = atoi(stXML.GetAttrib("levelup_num" + stStream.str()).c_str());

                stAward.m_stLevelUpAward.emplace_back(stGameItem);
            }

            m_stLevelAward.insert(std::make_pair(iLevel, stAward));

            m_stLevelList.push_back(std::make_pair(iExpValue, iGiftNum));
        }
        stXML.OutOfElem();

        if(false == stXML.FindElem("main_card_heart_exp"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        curLevel = 1;
        m_stMainCardLevelList.push_back(TLogicMainCardLevelAttr());
        while (stXML.FindElem("exp"))
        {
            int32_t iLevel = ::atoi(stXML.GetAttrib("level").c_str());
            if(iLevel != curLevel++)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: main card Level not find. Level:" << curLevel << std::endl;
                throw std::exception();
            }

            TLogicMainCardLevelAttr stAttr;
            stAttr.m_iExpValue = ::atoi(stXML.GetAttrib("exp_value").c_str());
            for(int32_t i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << "" << i;
                if (stXML.GetAttrib("attr_type" + stStream.str()).empty()) break;

                int32_t iAttrType = atoi(stXML.GetAttrib("attr_type" + stStream.str()).c_str());
                double dAttrNum = atof(stXML.GetAttrib("attr_num" + stStream.str()).c_str());

                stAttr.m_stAddAttr.push_back(std::make_pair(iAttrType, dAttrNum));
            }

            m_stMainCardLevelList.push_back(stAttr);
        }
        stXML.OutOfElem();

        if (false == stXML.FindElem("heart_gift"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }
        stXML.IntoElem();
        while(stXML.FindElem("gift"))
        {
            int32_t iItemID = (int32_t)atoi(stXML.GetAttrib("item_id").c_str());

            TLogicHeartGiftElem stGift;
            stGift.m_iHeartExp = (int32_t)atoi(stXML.GetAttrib("heart_exp").c_str());
            CONFIG_ASSERT_EXCEPTION(stGift.m_iHeartExp >= 0);
            m_stGifts.emplace(iItemID, stGift);
        }
        stXML.OutOfElem();

        if (false == stXML.FindElem("history"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iIndex = atoi(stXML.GetAttrib("index").c_str());

            TLogicHeartHistoryElem stHistory;
            stHistory.m_iUnlockHeartLevel = atoi(stXML.GetAttrib("unlock_heart_level").c_str());
            stHistory.m_iUnlockStar = atoi(stXML.GetAttrib("unlock_star").c_str());
            stHistory.m_iUnlockDatingTimes = atoi(stXML.GetAttrib("unlock_dating_times").c_str());
            stHistory.m_iContentNum = atoi(stXML.GetAttrib("content_num").c_str());
            CONFIG_ASSERT_EXCEPTION(stHistory.m_iContentNum > 0);

            for (int32_t i = 1; i <= 10; ++i)
            {
                CPackGameItem stGameItem;
                std::stringstream stStream;
                stStream << "" << i;
                if (stXML.GetAttrib("reward_type" + stStream.str()).empty()) break;

                stGameItem.m_iItemType = atoi(stXML.GetAttrib("reward_type" + stStream.str()).c_str());
                stGameItem.m_iCardID = atoi(stXML.GetAttrib("reward_id" + stStream.str()).c_str());
                stGameItem.m_iNum = atoi(stXML.GetAttrib("reward_num" + stStream.str()).c_str());

                stHistory.m_stRewards.emplace_back(stGameItem);
            }

            m_stHistoryMap.insert(std::make_pair(iIndex, stHistory));
        }
        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicHeartGiftElem* CLogicConfigHeroHeartParser::GetGiftItemInfo(int32_t iItemID) const
{
    auto iter = m_stGifts.find(iItemID);
    return iter == m_stGifts.end() ? nullptr : &iter->second;
}

const TLogicMainCardLevelAttr* CLogicConfigHeroHeartParser::GetMainCardLevelConfig(int32_t iLevel) const
{
    if(iLevel <= 0) return nullptr;
    if(iLevel >= (int)m_stMainCardLevelList.size()) return &(m_stMainCardLevelList[m_stMainCardLevelList.size() - 1]);

    return &(m_stMainCardLevelList[iLevel]);
}

void CLogicConfigHeroHeartParser::ReCalcHeartLevelExp(int32_t& iLevel, int32_t& iExp) const
{
    while ((unsigned int)iLevel < (m_stLevelList.size()-1) && m_stLevelList[iLevel].first <= iExp)
    {
        iExp -= m_stLevelList[iLevel++].first;
    }

    if ((iLevel == (int)m_stLevelList.size() - 1) && (iExp > m_stLevelList[iLevel].first))
    {
        iExp = m_stLevelList[iLevel].first;
    }
}

void CLogicConfigHeroHeartParser::ReCalcMainCardHeartLevelExp(int32_t& iLevel, int32_t& iExp) const
{
    while ((unsigned int)iLevel < (m_stMainCardLevelList.size() - 1) && m_stMainCardLevelList[iLevel].m_iExpValue <= iExp)
    {
        iExp -= m_stMainCardLevelList[iLevel++].m_iExpValue;
    }

    if((iLevel == (int)m_stMainCardLevelList.size() - 1) && (iExp > m_stMainCardLevelList[iLevel].m_iExpValue))
    {
        iExp = m_stMainCardLevelList[iLevel].m_iExpValue;
    }
}

int32_t CLogicConfigHeroHeartParser::GetHeartGiftLimitNumByHeartLevel(int32_t iHeartLevel) const
{
    return iHeartLevel < (int32_t)m_stLevelList.size() ? m_stLevelList[iHeartLevel].second : 0;
}
