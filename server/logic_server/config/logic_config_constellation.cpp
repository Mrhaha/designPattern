//
// Created by luoxiaohu on 2020/12/31.
//

#include "Markup.h"
#include "logic_config_constellation.h"
#include "common_tools.h"
#include "logic_config.h"
#include "logic_link_common.h"

std::pair<bool, std::string> CLogicConfigConstelParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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
        while (stXML.FindElem("constellation"))
        {
            TLogicConstelElem stElem;
            stElem.m_iItemID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidConstelItemID(stElem.m_iItemID), stElem.m_iItemID);
            CONFIG_ASSERT_EXCEPTION(m_stConfig.find(stElem.m_iItemID) == m_stConfig.end());

            stElem.m_iGrade = ::atoi(stXML.GetAttrib("grade").c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfig::GetConstelAttrConfig().GetGradeConfig(stElem.m_iGrade) != nullptr);

            // 装备位随机规则
            stElem.m_iEquipmentSelectType = ::atoi(stXML.GetAttrib("equipment_select_type").c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumConstelESRandomType(stElem.m_iEquipmentSelectType));
            std::string strValueList = stXML.GetAttrib("equipment_value");
            auto stEquipTagList = string_split(strValueList, "|");
            stElem.m_stEquipmentValue.clear();
            for(auto& tag : stEquipTagList)
            {
                int iTag = ::atoi(tag.c_str());
                if(!CLogicConfigDefine::IsValidEnumHeroEquipType(iTag))
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: equipment_value Error! ItemID:" << stElem.m_iItemID << std::endl;
                    throw std::exception();
                }

                stElem.m_stEquipmentValue.push_back(iTag);
            }

            if(stElem.m_iEquipmentSelectType == CLogicConfigDefine::ECESRT_Given)
            {
                CONFIG_ASSERT_EXCEPTION(1 == stElem.m_stEquipmentValue.size());
            }

            // 套装随机规则
            stElem.m_iSuitSelectType = ::atoi(stXML.GetAttrib("suit_select_type").c_str());
            CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumConstelESRandomType(stElem.m_iSuitSelectType));
            strValueList = stXML.GetAttrib("suit_value");
            auto stSuitList = string_split(strValueList, "|");
            stElem.m_stSuitValue.clear();
            for(auto& suit : stSuitList)
            {
                int iSuit = ::atoi(suit.c_str());
                if(CLogicConfig::GetConstelAttrConfig().m_stSuitMap.find(iSuit) == CLogicConfig::GetConstelAttrConfig().m_stSuitMap.end())
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG: suit_value Error! ItemID:" << stElem.m_iItemID << std::endl;
                    throw std::exception();
                }

                stElem.m_stSuitValue.push_back(iSuit);
            }

            if(stElem.m_iSuitSelectType == CLogicConfigDefine::ECESRT_Given)
            {
                CONFIG_ASSERT_EXCEPTION(1 == stElem.m_stSuitValue.size());
            }

            m_stConfig[stElem.m_iItemID] = std::move(stElem);
        }
        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicConstelElem* CLogicConfigConstelParser::GetConfig(int32_t iItemID) const
{
    auto iter = m_stConfig.find(iItemID);
    return (iter == m_stConfig.end() ? nullptr : &iter->second);
}

std::pair<bool, std::string> CLogicConfigConstelAttrParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"))

        m_iBagMaxSize = ::atoi(stXML.GetAttrib("bag_max").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iBagMaxSize > 0 && m_iBagMaxSize <= E_MaxBagSize)

        m_iOnceNeedBagSpace = ::atoi(stXML.GetAttrib("once_need_bag_space").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iOnceNeedBagSpace > 0)

        m_dExpInheritPct = ::atof(stXML.GetAttrib("exp_inherit_pct").c_str());
        m_dConsumeGoldPct = ::atof(stXML.GetAttrib("consume_gold_pct").c_str());
        CONFIG_ASSERT_EXCEPTION(m_dExpInheritPct > 0)
        CONFIG_ASSERT_EXCEPTION(m_dConsumeGoldPct > 0)

        std::string strList = stXML.GetAttrib("level_add_attr");
        auto stLevelList = string_split(strList, "|");
        for(auto& level : stLevelList)
        {
            int iLevel = ::atoi(level.c_str());
            CONFIG_ASSERT_EXCEPTION(iLevel > 0)

            m_stLevelAddAttr.push_back(iLevel);
        }

        std::set<int32_t> stAllMainAttrType;
        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("equip_main_attr_weight"))
        stXML.IntoElem();
        while (stXML.FindElem("equip"))
        {
            int32_t iType = ::atoi(stXML.GetAttrib("equip_type").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumHeroEquipType(iType), iType)

            TLogicEquipMainAttr stData;
            std::string strAttrList = stXML.GetAttrib("attr_list");
            std::string strWeightList = stXML.GetAttrib("weight_list");
            auto stAttrLs = string_split(strAttrList, "|");
            auto stWeightLs = string_split(strWeightList, "|");
            CONFIG_ASSERT_EXCEPTION_EX(stAttrLs.size() == stWeightLs.size(), iType)
            for(int i = 0; i < int(stAttrLs.size()); ++i)
            {
                int32_t iAttrType = ::atoi(stAttrLs[i].c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardAttrType(iAttrType), iType)
                int32_t iWeight = ::atoi(stWeightLs[i].c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iWeight > 0, iType)

                stData.m_stAttrWeight.emplace_back(iAttrType, iWeight);
                stData.m_iTotalWeight += iWeight;

                stAllMainAttrType.insert(iAttrType);
            }

            CONFIG_ASSERT_EXCEPTION_EX(stData.m_iTotalWeight > 0, iType)
            m_stMainAttrRand[iType] = std::move(stData);
        }
        stXML.OutOfElem();
        CONFIG_ASSERT_EXCEPTION(!m_stMainAttrRand.empty())

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("grade_attr"))
        stXML.IntoElem();
        while (stXML.FindElem("grade"))
        {
            int32_t iGrade = ::atoi(stXML.GetAttrib("grade").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(m_stGradeAttrRand.find(iGrade) == m_stGradeAttrRand.end(), iGrade)
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardGradeType(iGrade), iGrade)

            TLogicConstelGradeAttr stData;
            stXML.IntoElem();

            CONFIG_ASSERT_EXCEPTION_EX(stXML.FindElem("main_attr"), iGrade)
            stXML.IntoElem();
            while (stXML.FindElem("attr"))
            {
                int32_t iAttr = ::atoi(stXML.GetAttrib("attr").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardAttrType(iAttr), iGrade)

                TLogicConstelGradeAttr::main_attr stMainAttr;
                stMainAttr.m_iBaseValue = ::atoi(stXML.GetAttrib("x").c_str());
                stMainAttr.m_iParaValue = ::atoi(stXML.GetAttrib("y").c_str());
                stMainAttr.m_iEquationType = ::atoi(stXML.GetAttrib("equation_type").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stMainAttr.m_iBaseValue > 0, iGrade)
                CONFIG_ASSERT_EXCEPTION_EX(stMainAttr.m_iParaValue >= 0, iGrade)
                CONFIG_ASSERT_EXCEPTION_EX(IsValidEquationType(stMainAttr.m_iEquationType), iGrade)

                stData.m_stMainAttr[iAttr] = std::move(stMainAttr);
            }
            stXML.OutOfElem();

            // 检查主属性
            for(auto type : stAllMainAttrType)
            {
                CONFIG_ASSERT_EXCEPTION_EX(stData.m_stMainAttr.find(type) != stData.m_stMainAttr.end(), iGrade)
            }

            CONFIG_ASSERT_EXCEPTION_EX(stXML.FindElem("ex_attr"), iGrade)
            stXML.IntoElem();

            CONFIG_ASSERT_EXCEPTION_EX(stXML.FindElem("num_weight"), iGrade)
            stData.m_iBaseExp = ::atoi(stXML.GetAttrib("base_exp").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stData.m_iBaseExp > 0, iGrade)

            std::string strNumList = stXML.GetAttrib("num_list");
            std::string strWeightList = stXML.GetAttrib("weight_list");
            auto stNumLs = string_split(strNumList, "|");
            auto stWeightLs = string_split(strWeightList, "|");
            CONFIG_ASSERT_EXCEPTION_EX(stNumLs.size() == stWeightLs.size(), iGrade)
            for(int i = 0; i < int(stNumLs.size()); ++i)
            {
                int32_t iNum = ::atoi(stNumLs[i].c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iNum > 0 && iNum <= E_ExAttrMaxNum, iGrade)
                int32_t iWeight = ::atoi(stWeightLs[i].c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iWeight > 0, iGrade)

                stData.m_stAttrNumWeight.m_iTotalWeight += iWeight;
                stData.m_stAttrNumWeight.m_stAttrWeight.emplace_back(iNum, iWeight);
            }

            CONFIG_ASSERT_EXCEPTION_EX(stData.m_stAttrNumWeight.m_iTotalWeight > 0, iGrade)

            CONFIG_ASSERT_EXCEPTION_EX(stXML.FindElem("attr_list"), iGrade)
            stXML.IntoElem();
            while (stXML.FindElem("attr"))
            {
                int32_t iAttr = ::atoi(stXML.GetAttrib("attr").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardAttrType(iAttr), iGrade)

                TLogicConstelGradeAttr::ex_attr stExAttrData;
                stExAttrData.m_iWeight = ::atoi(stXML.GetAttrib("weight").c_str());
                stExAttrData.m_iValueMin = ::atoi(stXML.GetAttrib("value_min").c_str());
                stExAttrData.m_iValueMax = ::atoi(stXML.GetAttrib("value_max").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stExAttrData.m_iWeight > 0, iGrade)
                CONFIG_ASSERT_EXCEPTION_EX((stExAttrData.m_iValueMin > 0 && stExAttrData.m_iValueMin <= stExAttrData.m_iValueMax), iGrade)

                stData.m_stExAttr[iAttr] = std::move(stExAttrData);
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION_EX((stData.m_stExAttr.size() > E_ExAttrMaxNum), iGrade)

            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION_EX(stXML.FindElem("exp"), iGrade)
            stXML.IntoElem();
            int32_t iLevel = 0;
            while (stXML.FindElem("level"))
            {
                int32_t iIndex = ::atoi(stXML.GetAttrib("level").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iLevel == iIndex, iGrade)
                ++iLevel;

                int32_t iExp = ::atoi(stXML.GetAttrib("exp").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iExp > 0, iGrade)
                stData.m_stLevelExp.push_back(iExp);
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION_EX((!stData.m_stLevelExp.empty()), iGrade)

            stXML.OutOfElem();

            m_stGradeAttrRand[iGrade] = std::move(stData);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(!m_stGradeAttrRand.empty())

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("resolve_reward"))
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iGrade = ::atoi(stXML.GetAttrib("grade").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardGradeType(iGrade), iGrade)

            int iExp = ::atoi(stXML.GetAttrib("exp").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(iExp >= 0, iGrade)
            std::string strBag = stXML.GetAttrib("level_drop");
            if(!strBag.empty())
            {
                // 表的读取顺序问题，此时不能判断包的有效性
                // CONFIG_ASSERT_EXCEPTION_EX(CLogicConfig::GetLevelDropConfig().GetConfig(strBag), iGrade)
            }

            m_stGradeResolveReward[iGrade] = std::make_pair(iExp, strBag);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(m_stGradeAttrRand.size() == m_stGradeResolveReward.size())

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("suit_list"))
        stXML.IntoElem();
        while (stXML.FindElem("suit"))
        {
            int32_t iSuitType = ::atoi(stXML.GetAttrib("suit_type").c_str());
            CONFIG_ASSERT_EXCEPTION(m_stSuitMap.find(iSuitType) == m_stSuitMap.end())

            std::vector<TLogicConstelSuitEffectElem> stEffectList;
            stXML.IntoElem();
            while (stXML.FindElem("index"))
            {
                TLogicConstelSuitEffectElem stElem;
                stElem.m_iSuitNum = ::atoi(stXML.GetAttrib("suit_num").c_str());
                CONFIG_ASSERT_EXCEPTION(stElem.m_iSuitNum > 0)

                stElem.m_iEffectType = ::atoi(stXML.GetAttrib("effect_type").c_str());
                CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumConstelSuitEffectType(stElem.m_iEffectType))

                stElem.m_iAttrType = ::atoi(stXML.GetAttrib("attr_type").c_str());
                stElem.m_iAttrValue = ::atoi(stXML.GetAttrib("attr_value").c_str());
                if(stElem.m_iEffectType == CLogicConfigDefine::ECSET_NormalEffect)
                {
                    CONFIG_ASSERT_EXCEPTION(CLogicConfigDefine::IsValidEnumCardAttrType(stElem.m_iAttrType))
                    CONFIG_ASSERT_EXCEPTION(stElem.m_iAttrValue > 0)
                }

                stElem.m_iFightPower = ::atoi(stXML.GetAttrib("fight_power").c_str());

                stEffectList.push_back(std::move(stElem));
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION(!stEffectList.empty())

            m_stSuitMap[iSuitType] = std::move(stEffectList);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(!m_stSuitMap.empty())

        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicConstelGradeAttr* CLogicConfigConstelAttrParser::GetGradeConfig(int32_t iGrade) const
{
    auto iter = m_stGradeAttrRand.find(iGrade);
    return iter == m_stGradeAttrRand.end() ? nullptr : &(iter->second);
}

const std::pair<int, std::string>* CLogicConfigConstelAttrParser::GetGradeResolveConfig(int32_t iGrade) const
{
    auto iter = m_stGradeResolveReward.find(iGrade);
    return iter == m_stGradeResolveReward.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string> CLogicConfigEquipAttrParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("info"));
        m_iEquipBagMax = ::atoi(stXML.GetAttrib("bag_max").c_str());
        m_iOnceNeedBagSpace = ::atoi(stXML.GetAttrib("once_need_bag_space").c_str());
        m_iTokenItemType = ::atoi(stXML.GetAttrib("token_item_type").c_str());
        m_iTokenItemID = ::atoi(stXML.GetAttrib("token_item_id").c_str());
        CONFIG_ASSERT_EXCEPTION(m_iEquipBagMax > 0);
        CONFIG_ASSERT_EXCEPTION(m_iOnceNeedBagSpace > 0);
        CONFIG_ASSERT_EXCEPTION(m_iTokenItemType == CLogicConfigDefine::EXCHANGE_ITEM);
        CONFIG_ASSERT_EXCEPTION(nullptr != CLogicConfig::GetItemConfig().GetConfig(m_iTokenItemID));

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("resolve_reward"));
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iGrade = ::atoi(stXML.GetAttrib("grade").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardGradeType(iGrade), iGrade);
            CONFIG_ASSERT_EXCEPTION_EX(m_stGradeResolveReward.find(iGrade) == m_stGradeResolveReward.end(), iGrade);

            TLogicConstelResolveReward stReward;
            stReward.m_iGold = ::atoi(stXML.GetAttrib("gold").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(stReward.m_iGold >= 0, iGrade);

            for(int i = 1; i < 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("num" + stStream.str()).empty()) break;

                int iNum = ::atoi(stXML.GetAttrib("num" + stStream.str()).c_str());
                int iWeight = ::atoi(stXML.GetAttrib("weight" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION_EX(iNum >= 0 && iWeight > 0, iGrade);
                stReward.m_stNumWeight.emplace_back(iNum, iWeight);
                stReward.m_iTotalWeight += iWeight;
            }

            m_stGradeResolveReward[iGrade] = std::move(stReward);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("attr_list"));
        stXML.IntoElem();
        while (stXML.FindElem("attr"))
        {
            int32_t iIndex = ::atoi(stXML.GetAttrib("index").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(m_stAttrMap.find(iIndex) == m_stAttrMap.end(), iIndex);

            TLogicEquipAttrElem stAttrList;
            int32_t iAttrType = ::atoi(stXML.GetAttrib("main_attr").c_str());
            int32_t iAttrValue = ::atoi(stXML.GetAttrib("main_value").c_str());
            if(iAttrType > 0)
            {
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardAttrType(iAttrType), iIndex);
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidCardAttrValue(iAttrValue), iIndex);
                stAttrList.m_stAttrList.emplace_back(iAttrType, iAttrValue);
            }

            stAttrList.m_iMainSkill = ::atoi(stXML.GetAttrib("main_skill").c_str());
            stAttrList.m_iExtSkill = ::atoi(stXML.GetAttrib("ex_skill").c_str());
            //洗练属性索引
            stAttrList.m_iRefineIndex = ::atoi(stXML.GetAttrib("attr_index").c_str());

            iAttrType = ::atoi(stXML.GetAttrib("ex_attr").c_str());
            iAttrValue = ::atoi(stXML.GetAttrib("ex_value").c_str());
            if(iAttrType > 0)
            {
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardAttrType(iAttrType), iIndex);
                CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidCardAttrValue(iAttrValue), iIndex);
                stAttrList.m_stAttrList.emplace_back(iAttrType, iAttrValue);
            }

            m_stAttrMap[iIndex] = std::move(stAttrList);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("rand_affix"));
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            std::vector<int32_t> stAffix;
            for(int i = 1; i <= 10; ++i)
            {
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("affix" + stStream.str()).empty()) break;

                int iNum = ::atoi(stXML.GetAttrib("affix" + stStream.str()).c_str());
                CONFIG_ASSERT_EXCEPTION(iNum > 0);
                stAffix.push_back(iNum);
            }
            CONFIG_ASSERT_EXCEPTION(!stAffix.empty());

            m_stAffixVec.emplace_back(stAffix);
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("attr_trans"));
        CPackGameItem gameItem;
        gameItem.m_iNum = ::atoi(stXML.GetAttrib("cost_item_num").c_str());
        gameItem.m_iItemType = ::atoi(stXML.GetAttrib("cost_item_type").c_str());
        gameItem.m_iCardID = ::atoi(stXML.GetAttrib("cost_item_id").c_str());
        m_iRefineItem = gameItem;

        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iIndex = ::atoi(stXML.GetAttrib("index").c_str());
            auto strList = stXML.GetAttrib("attr_type");
            auto stDataList = string_split(strList, "|");
            TLogicRefineRandAttrElem randAttrElem;
            for(auto& para : stDataList)
            {
                randAttrElem.m_iRandom.push_back(::atoi(para.c_str()));
            }
            auto strWeightList = stXML.GetAttrib("weight");
            auto stWeightDataList = string_split(strWeightList, "|");
            for(auto& para : stWeightDataList)
            {
                randAttrElem.m_iWeight.push_back(::atoi(para.c_str()));
            }
            m_stRandRefine[iIndex] = randAttrElem;
        }
        stXML.OutOfElem();

        CONFIG_ASSERT_EXCEPTION(stXML.FindElem("equip_rank"));
        stXML.IntoElem();
        while (stXML.FindElem("index"))
        {
            int32_t iRank = ::atoi(stXML.GetAttrib("equip_rank").c_str());
            TLogicEquipRank equipRank;
            equipRank.m_iNeedLevel = ::atoi(stXML.GetAttrib("need_level").c_str());
            for(int i = 1; i <= 10; ++i)
            {
                CPackGameItem costItem;
                std::stringstream stStream;
                stStream << i;
                if(stXML.GetAttrib("cost_item_id" + stStream.str()).empty()) break;

                costItem.m_iCardID = ::atoi(stXML.GetAttrib("cost_item_id" + stStream.str()).c_str());
                costItem.m_iNum = ::atoi(stXML.GetAttrib("cost_item_num" + stStream.str()).c_str());
                costItem.m_iItemType = ::atoi(stXML.GetAttrib("cost_item_type" + stStream.str()).c_str());

                CONFIG_ASSERT_EXCEPTION(costItem.m_iNum > 0);
                equipRank.m_stCostItem.emplace_back(costItem);
            }
            equipRank.m_iCostGold = ::atoi(stXML.GetAttrib("cost_gold").c_str());
            m_stEquipUpGrade[iRank] = equipRank;
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

const TLogicEquipAttrElem* CLogicConfigEquipAttrParser::GetAttrConfig(int32_t iIndex) const
{
    auto iter = m_stAttrMap.find(iIndex);
    return iter == m_stAttrMap.end() ? nullptr : &(iter->second);
}

const TLogicConstelResolveReward* CLogicConfigEquipAttrParser::GetGradeResolveConfig(int32_t iGrade) const
{
    auto iter = m_stGradeResolveReward.find(iGrade);
    return iter == m_stGradeResolveReward.end() ? nullptr : &(iter->second);
}

std::vector<int32_t> CLogicConfigEquipAttrParser::GetRandAffix() const
{
    std::vector<int32_t> stVec;
    for(auto& vec : m_stAffixVec)
    {
        stVec.push_back(vec[CLogicCommon::GetRandNum() % vec.size()]);
    }

    return stVec;
}

std::pair<bool, std::string> CLogicConfigEquipAttrRandParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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
        while (stXML.FindElem("bag"))
        {
            std::string strName = stXML.GetAttrib("name");
            CONFIG_ASSERT_EXCEPTION_EX(m_stConfig.find(strName) == m_stConfig.end(), strName);

            TLogicEquipAttrRandBag stElem;
            stXML.IntoElem();
            while (stXML.FindElem("index"))
            {
                int32_t iIndex = ::atoi(stXML.GetAttrib("attr_index").c_str());
                int32_t iWeight = ::atoi(stXML.GetAttrib("weight").c_str());
                CONFIG_ASSERT_EXCEPTION_EX(stElem.m_stIndexRandMap.find(iIndex) == stElem.m_stIndexRandMap.end(), strName);
                CONFIG_ASSERT_EXCEPTION_EX(nullptr != CLogicConfig::GetEquipAttrConfig().GetAttrConfig(iIndex), strName);
                CONFIG_ASSERT_EXCEPTION_EX(iWeight > 0, strName);
                stElem.m_stIndexRandMap.emplace(iIndex, iWeight);
                stElem.m_iTotalWeight += iWeight;
            }
            stXML.OutOfElem();

            CONFIG_ASSERT_EXCEPTION_EX(stElem.m_iTotalWeight > 0, strName);
            m_stConfig[strName] = std::move(stElem);
        }
        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicEquipAttrRandBag* CLogicConfigEquipAttrRandParser::GetConfig(std::string strName) const
{
    auto iter = m_stConfig.find(strName);
    return iter == m_stConfig.end() ? nullptr : &(iter->second);
}

std::pair<bool, std::string> CLogicConfigEquipItemParser::Load(const std::string& strFileName, const std::string& strXMLContent)
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
        while (stXML.FindElem("equip"))
        {
            TLogicEquipElem stElem;
            stElem.m_iItemID = ::atoi(stXML.GetAttrib("id").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEquipItemID(stElem.m_iItemID), stElem.m_iItemID);
            CONFIG_ASSERT_EXCEPTION_EX(m_stConfig.find(stElem.m_iItemID) == m_stConfig.end(), stElem.m_iItemID);

            stElem.m_iGrade = ::atoi(stXML.GetAttrib("grade").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumCardGradeType(stElem.m_iGrade), stElem.m_iItemID);
            stElem.m_iEquipType = ::atoi(stXML.GetAttrib("equip_type").c_str());
            CONFIG_ASSERT_EXCEPTION_EX(CLogicConfigDefine::IsValidEnumHeroEquipType(stElem.m_iEquipType), stElem.m_iItemID);
            stElem.m_strAttrBag = stXML.GetAttrib("attr_bag");
            CONFIG_ASSERT_EXCEPTION_EX(nullptr != CLogicConfig::GetEquipAttrRandConfig().GetConfig(stElem.m_strAttrBag), stElem.m_iItemID);

            m_stConfig[stElem.m_iItemID] = std::move(stElem);
        }
        stXML.OutOfElem();
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicEquipElem* CLogicConfigEquipItemParser::GetConfig(int32_t iItemID) const
{
    auto iter = m_stConfig.find(iItemID);
    return iter == m_stConfig.end() ? nullptr : &(iter->second);
}
