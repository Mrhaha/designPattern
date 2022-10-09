//
// Created by hzl on 2019/8/29.
//

#include <cmath>
#include "logic_game_attribute.h"
#include "logic_config.h"
#include "logic_link_common.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_cache_common.h"

int32_t CLogicAttribute::CalcUserTeamFightPower(user_data_table_ptr_type pUserInfo, int32_t iTeamType)
{
    auto stTeam = CLogicCacheCommon::GetTeam(pUserInfo, iTeamType);
    return CalcUserTotalFightPower(pUserInfo, stTeam);
}

int32_t CLogicAttribute::CalcUserTotalFightPower(user_data_table_ptr_type pUserInfo, const std::vector<int32_t>& vcCardID)
{/*
    RefreshUserFashionAttribute(pUserInfo);
    RefreshUserThemeAttribute(pUserInfo);
    RefreshUserMilitaryAttribute(pUserInfo);
    RefreshUserHeartLevelAttribute(pUserInfo);
    RefreshUserTalentAttribute(pUserInfo);
    RefreshUserHonorAttribute(pUserInfo);
    RefreshUserGuildAttribute(pUserInfo);

    double fight_power = 0.0;
    for (const auto& card_id : vcCardID)
    {
        if (card_id > 0)
        {
            fight_power += CalcUserCardFightPower(pUserInfo, card_id);
        }
    }

    fight_power += CalcUserWushenFightPower(pUserInfo);
    fight_power += CalcUserTacticsFightPower(pUserInfo);

    return std::ceil(fight_power);
    */
return 0;
}

int32_t CLogicAttribute::CalcClientFightPower(user_data_table_ptr_type pUserInfo, int32_t iFightPower1, int32_t iFightPower2)
{
    if ((pUserInfo->m_iUin == iFightPower1 + iFightPower2) && iFightPower1 < 0)
        return (-iFightPower1);

    return -1;
}
/*
int32_t CLogicAttribute::CalcUserCardFightPower(user_data_table_ptr_type pUserInfo, int32_t iCardID)
{
    const auto* pstConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iCardID);
    if (pstConfig == nullptr)
    {
        return 0;
    }

    CLogicCacheCardOperation stOperation(pUserInfo);
    auto iterCard = pUserInfo->m_stHeroCardTableMap.Find(iCardID);
    if (iterCard == pUserInfo->m_stHeroCardTableMap.End() || stOperation.CheckHeroCardNotExist(iterCard->second))
    {
        return 0;
    }

    attribute_elem_t base_attribute = {};
    attribute_elem_t advance_attribute = {};
    const auto& rstCardInfo = iterCard->second->m_stData;

    //卡片升级属性，包括登机，星级，颜色，培养
    base_attribute += CalcCardUpgradeAttribute(pUserInfo, rstCardInfo);

    //卡片羁绊
    base_attribute += CalcCardFateAttribute(pUserInfo, rstCardInfo);

    //奥义加成
    base_attribute += CalcCardMysteryAttribute(pUserInfo, rstCardInfo);

    //天赋加成
    base_attribute += GetCardTypeTalentAttribute(pUserInfo, 0);
    base_attribute += GetCardTypeTalentAttribute(pUserInfo, pstConfig->m_iCardType);

    //称号加成
    advance_attribute += GetCardTypeHonorAttribute(pUserInfo, 0);
    advance_attribute += GetCardTypeHonorAttribute(pUserInfo, pstConfig->m_iCardType);
    advance_attribute += GetCardTypeHonorAttribute(pUserInfo, 10 + pstConfig->m_iSexual);

    //时装加成
    advance_attribute += GetUserFashionAttribute(pUserInfo);

    //主题加成
    advance_attribute += GetUserThemeAttribute(pUserInfo);

    //军衔加成
    advance_attribute += GetUserMilitaryAttribute(pUserInfo);

    //主角好感度等级加成
    advance_attribute += GetUserHeartLevelAttribute(pUserInfo);

    //公会加成
    advance_attribute += GetCardTypeGuildAttribute(pUserInfo, 0);
    advance_attribute += GetCardTypeGuildAttribute(pUserInfo, pstConfig->m_iCardType);

    //装备加成
    advance_attribute += CalcCardEquipAttribute(pUserInfo, rstCardInfo);

    //宠物加成
    advance_attribute += CalcCardPetAttribute(pUserInfo, rstCardInfo);

    //咒印加成
    advance_attribute += CalcCardAmuletAttribute(pUserInfo, rstCardInfo);

    //根性加成
    advance_attribute += CalcCardCharacterAttribute(pUserInfo, rstCardInfo);

    const auto attack_ratio = (base_attribute.m_dPercentAtk + advance_attribute.m_dPercentAtk) / 100.0 + 1;
    const auto defense_ratio = (base_attribute.m_dPercentDef + advance_attribute.m_dPercentDef) / 100.0 + 1;
    const auto hp_ratio = (base_attribute.m_dPercentHp + advance_attribute.m_dPercentHp) / 100.0 +1;
    const auto crit_ratio = (base_attribute.m_dPercentCri + advance_attribute.m_dPercentCri) / 100.0 + 1;
    const int32_t attack = std::ceil(base_attribute.m_dAtk * attack_ratio + advance_attribute.m_dAtk);
    const int32_t defense = std::ceil(base_attribute.m_dDef * defense_ratio + advance_attribute.m_dDef);
    const int32_t hp = std::ceil(base_attribute.m_dHp * hp_ratio + advance_attribute.m_dHp);
    const int32_t crit = std::ceil(base_attribute.m_dCri * crit_ratio + advance_attribute.m_dCri);

    const int32_t crit_damage = std::ceil(base_attribute.m_dCritDamage + advance_attribute.m_dCritDamage);

    const auto& stFightPowerRatio = CLogicConfig::GetCalcConfig().m_stCalcConfig;
    // 技能类型加成属性战力
    const double auto_skill_power = (base_attribute.m_dAutoSkillEffect + advance_attribute.m_dAutoSkillEffect) * stFightPowerRatio.m_fAutoSkillFightPowerRatio;
    const double unique_skill_power = (base_attribute.m_dUniqueSkillEffect + advance_attribute.m_dUniqueSkillEffect) * stFightPowerRatio.m_fUniqueSkillFightPowerRatio;
    const double passive_skill_power = (base_attribute.m_dPassiveSkillEffect + advance_attribute.m_dPassiveSkillEffect) * stFightPowerRatio.m_fPassiveSkillFightPowerRatio;
    std::vector<std::pair<int32_t, int32_t>> stTypeLevelVec = {
        {pstConfig->m_stSkill1.m_iSkillType, iterCard->second->GetCardSkill1Level()},
        {pstConfig->m_stSkill2.m_iSkillType, iterCard->second->GetCardSkill2Level()},
        {pstConfig->m_stSkill3.m_iSkillType, iterCard->second->GetCardSkill3Level()},
        {pstConfig->m_stSkill4.m_iSkillType, iterCard->second->GetCardSkill4Level()},
    };

    double dSkillTypeFightPower = 0.0;
    for(auto& value : stTypeLevelVec)
    {
        switch (value.first)
        {
        case CLogicConfigDefine::EST_PassiveSkill:
            dSkillTypeFightPower += (passive_skill_power * value.second);
            break;
        case CLogicConfigDefine::EST_AutoSkill:
            dSkillTypeFightPower += (auto_skill_power * value.second);
            break;
        case CLogicConfigDefine::EST_UniqueSkill:
            dSkillTypeFightPower += (unique_skill_power * value.second);
            break;
        default:
            break;
        }
    }

    const int32_t fight_power = std::ceil(
            attack * stFightPowerRatio.m_fATKFightPowerRatio +
            defense * stFightPowerRatio.m_fDEFFightPowerRatio +
            hp * stFightPowerRatio.m_fHPFightPowerRatio +
            crit * stFightPowerRatio.m_fCRIFightPowerRatio +
            crit_damage * stFightPowerRatio.m_fCritDamageFightPowerRatio +
            iterCard->second->GetCardSkill1Level() * stFightPowerRatio.m_fSkill1LevelFightPowerRatio +
            iterCard->second->GetCardSkill2Level() * stFightPowerRatio.m_fSkill2LevelFightPowerRatio +
            iterCard->second->GetCardSkill3Level() * stFightPowerRatio.m_fSkill3LevelFightPowerRatio +
            iterCard->second->GetCardSkill4Level() * stFightPowerRatio.m_fSkill4LevelFightPowerRatio +
            base_attribute.m_dFightPower + advance_attribute.m_dFightPower + dSkillTypeFightPower
    );

    iterCard->second->SetFightAttack(attack);
    iterCard->second->SetFightDefense(defense);
    iterCard->second->SetFightHp(hp);
    iterCard->second->SetFightCrit(crit);
    iterCard->second->SetFightPower(fight_power);

    LOGIC_LOG_TRACE_INFO << "GROUP_ID:" << pUserInfo->m_iGroupID << "|UIN:" << pUserInfo->m_iUin << "|STAT_CARD_ID:" << iCardID
         << "|ATK:" << attack << "|DEF:" << defense << "|HP:" << hp << "|CRI:" << crit << "|FIGHT_POWER:" << fight_power << std::endl;

    return fight_power;
}

double CLogicAttribute::CalcUserWushenFightPower(user_data_table_ptr_type pUserInfo)
{
    double skill_fight_power = 0.0;
    double attr_fight_power = 0.0;
    for (auto iterWushen = pUserInfo->m_stWushenInfoMap.Begin(); iterWushen != pUserInfo->m_stWushenInfoMap.End(); ++iterWushen)
    {
        const auto iterConfig = CLogicConfig::GetWushenConfig().m_stWushenMap.find(iterWushen->first);
        const auto pAttrConfig = CLogicConfig::GetWushenAttrConfig().GetWushenAttr(iterWushen->first);
        if (iterConfig != CLogicConfig::GetWushenConfig().m_stWushenMap.end() && pAttrConfig != nullptr)
        {
            std::vector<std::pair<int32_t, int32_t>> skills;
            skills.emplace_back(iterConfig->second.m_iSkill1OpenLevel, iterConfig->second.m_iSkill1);
            skills.emplace_back(iterConfig->second.m_iSkill2OpenLevel, iterConfig->second.m_iSkill2);
            skills.emplace_back(iterConfig->second.m_iSkill3OpenLevel, iterConfig->second.m_iSkill3);
            skills.emplace_back(iterConfig->second.m_iSkill4OpenLevel, iterConfig->second.m_iSkill4);

            for (const auto& skill : skills)
            {
                if (skill.first > 0 && iterWushen->second->GetAdvanceLevel() >= skill.first)
                {
                    skill_fight_power += CLogicConfig::GetCalcConfig().GetSkillPower(skill.second);
                }
            }

            if (iterWushen->first == pUserInfo->m_stUserInfoTable.GetInFightWushen())
            {
                attribute_elem_t attr = {};
                if((int)pAttrConfig->m_stLevelAttrList.size() > iterWushen->second->GetLevel())
                {
                    const auto& stAttrConfig = pAttrConfig->m_stLevelAttrList[iterWushen->second->GetLevel()];
                    attr.m_dAtk += stAttrConfig.m_dAtk;
                    attr.m_dDef += stAttrConfig.m_dDef;
                    attr.m_dHp += stAttrConfig.m_dHp;
                    attr.m_dCri += stAttrConfig.m_dCrt;
                }

                if((int)pAttrConfig->m_stAdvanceAttrList.size() > iterWushen->second->GetAdvanceLevel())
                {
                    const auto& stAttrConfig = pAttrConfig->m_stAdvanceAttrList[iterWushen->second->GetAdvanceLevel()].first;
                    attr.m_dAtk += stAttrConfig.m_dAtk;
                    attr.m_dDef += stAttrConfig.m_dDef;
                    attr.m_dHp += stAttrConfig.m_dHp;
                    attr.m_dCri += stAttrConfig.m_dCrt;
                }

                // 刻印属性
                if(iterWushen->second->GetKeyinTimes() > 0)
                {
                    int times = iterWushen->second->GetKeyinTimes();
                    const auto& kyAttr = CLogicConfig::GetWushenAttrConfig().m_stKeyinAttr;
                    attr.m_dAtk += (kyAttr.m_dAtk * times);
                    attr.m_dDef += (kyAttr.m_dDef * times);
                    attr.m_dHp  += (kyAttr.m_dHp * times);
                    attr.m_dCri += (kyAttr.m_dCrt * times);
                }

                const auto& stFightPowerRatio = CLogicConfig::GetCalcConfig().m_stCalcConfig;
                attr_fight_power = attr.m_dAtk * stFightPowerRatio.m_fATKFightPowerRatio +
                                   attr.m_dDef * stFightPowerRatio.m_fDEFFightPowerRatio +
                                   attr.m_dHp * stFightPowerRatio.m_fHPFightPowerRatio +
                                   attr.m_dCri * stFightPowerRatio.m_fCRIFightPowerRatio;

                attr_fight_power *= 5;
                LOGIC_LOG_TRACE_INFO << "GROUP_ID:" << pUserInfo->m_iGroupID << "|UIN:" << pUserInfo->m_iUin << "|WUSHEN_FIGHT_POWER"
                                     << "|ATK:" << attr.m_dAtk << "|DEF:" << attr.m_dDef << "|HP:" << attr.m_dHp << "|CRT:" << attr.m_dCri << std::endl;
            }
        }
    }

    LOGIC_LOG_TRACE_INFO << "GROUP_ID:" << pUserInfo->m_iGroupID << "|UIN:" << pUserInfo->m_iUin
                        << "|WUSHEN_SKILL_FIGHT_POWER:" << skill_fight_power
                        << "|WUSHEN_ATTR_FIGHT_POWER" << attr_fight_power  << std::endl;

    return skill_fight_power + attr_fight_power;
}

double CLogicAttribute::CalcUserTacticsFightPower(user_data_table_ptr_type pUserInfo)
{
    double fight_power = 0.0;
    attribute_elem_t attribute = {};
    const auto iterTactics = pUserInfo->m_stTacticsInfoMap.Find(pUserInfo->m_stUserInfoTable.GetEnabledTacticsID());
    if (iterTactics != pUserInfo->m_stTacticsInfoMap.End())
    {
        const auto pstTacticsConfig = CLogicConfig::GetTacticsConfig().GetTacticsLevelConfig(pUserInfo->m_stUserInfoTable.GetEnabledTacticsID(), iterTactics->second->GetLevel());
        const auto pStarConfig = CLogicConfig::GetTacticsConfig().GetUpgradeStarConfig(pUserInfo->m_stUserInfoTable.GetEnabledTacticsID(), iterTactics->second->GetStar());
        if (pstTacticsConfig && pStarConfig)
        {
            for (const auto& level : pstTacticsConfig->m_astSlot)
            {
                for (const auto& slot : level.second.m_astAddAttr)
                {
                    AddAttribute(attribute, slot.first, slot.second);
                }
            }

            int32_t iPresent = pStarConfig->m_iPresent;
            double dAttrPresent = 1.0 + iPresent / 100.0;
            const auto& stFightPowerRatio = CLogicConfig::GetCalcConfig().m_stCalcConfig;
            fight_power += stFightPowerRatio.m_fHPFightPowerRatio * attribute.m_dHp * dAttrPresent;
            fight_power += stFightPowerRatio.m_fATKFightPowerRatio * attribute.m_dAtk * dAttrPresent;
            fight_power += stFightPowerRatio.m_fDEFFightPowerRatio * attribute.m_dDef * dAttrPresent;
            fight_power += stFightPowerRatio.m_fCRIFightPowerRatio * attribute.m_dCri * dAttrPresent;
        }
    }
    return fight_power;
}

void CLogicAttribute::AddAttribute(attribute_elem_t &stAttribute, int32_t iType, int32_t iValue)
{
    if(CLogicConfigDefine::IsValidEnumCardAttrType(iType))
    {
        stAttribute.m_stAttrValue[iType] += iValue;
    }
}

attribute_elem_t CLogicAttribute::CalcCardUpgradeAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo)
{
    attribute_elem_t attribute = {};
    const auto* pstConfig = CLogicConfig::GetHeroCardConfig().GetConfig(rstCardInfo.m_iCardID);
    if (pstConfig != nullptr)
    {
        //卡片等级加成
        attribute.m_dAtk += pstConfig->m_stLevelUpAttr.m_fATK * rstCardInfo.m_iCardLevel;
        attribute.m_dDef += pstConfig->m_stLevelUpAttr.m_fDEF * rstCardInfo.m_iCardLevel;
        attribute.m_dHp += pstConfig->m_stLevelUpAttr.m_fHP * rstCardInfo.m_iCardLevel;
        attribute.m_dCri += pstConfig->m_stLevelUpAttr.m_fCRI * rstCardInfo.m_iCardLevel;

        //卡片星级加成
        const auto star = rstCardInfo.m_cCardStar;
        if (star > 0 && star < (char)pstConfig->m_stStarUpAttr.size())
        {
            attribute.m_dAtk += pstConfig->m_stStarUpAttr[star].m_iATK;
            attribute.m_dDef += pstConfig->m_stStarUpAttr[star].m_iDEF;
            attribute.m_dHp += pstConfig->m_stStarUpAttr[star].m_iHP;
            attribute.m_dCri += pstConfig->m_stStarUpAttr[star].m_iCRI;
        }

        //卡片颜色加成
        const auto color = rstCardInfo.m_cCardColor;
        if (color > 0 && color < (char)pstConfig->m_stRankUpAttr.size())
        {
            attribute.m_dAtk += pstConfig->m_stRankUpAttr[color].m_iATK;
            attribute.m_dDef += pstConfig->m_stRankUpAttr[color].m_iDEF;
            attribute.m_dHp += pstConfig->m_stRankUpAttr[color].m_iHP;
            attribute.m_dCri += pstConfig->m_stRankUpAttr[color].m_iCRI;
        }

        const int iGradeIndex = rstCardInfo.m_iGradeLevel - pstConfig->m_iInitGrade - 1;
        double iPro = 1.0;
        if(iGradeIndex >= 0 && (int)pstConfig->m_stGradeLevelUpVec.size() > iGradeIndex)
            iPro = pstConfig->m_stGradeLevelUpVec[iGradeIndex].m_dAttrCoef;

        if(iPro < 1.0) iPro = 1.0;

        attribute.m_dAtk *= iPro;
        attribute.m_dDef *= iPro;
        attribute.m_dHp *= iPro;
        attribute.m_dCri *= iPro;

        //卡片镶嵌孔
        const auto pstColorUpgradeConfig = CLogicConfig::GetUpgradeHeroCardColorConfig().GetConfigElem(pstConfig->m_iCardType, rstCardInfo.m_cCardColor, false);
        if (pstColorUpgradeConfig)
        {
            if (rstCardInfo.m_iUpgradeColor1 > 0 && pstColorUpgradeConfig->m_stAddAttrVec.size() > 1)
            {
                AddAttribute(attribute, pstColorUpgradeConfig->m_stAddAttrVec[1].first, pstColorUpgradeConfig->m_stAddAttrVec[1].second);
            }

            if (rstCardInfo.m_iUpgradeColor2 > 0 && pstColorUpgradeConfig->m_stAddAttrVec.size() > 2)
            {
                AddAttribute(attribute, pstColorUpgradeConfig->m_stAddAttrVec[2].first, pstColorUpgradeConfig->m_stAddAttrVec[2].second);
            }

            if (rstCardInfo.m_iUpgradeColor3 > 0 && pstColorUpgradeConfig->m_stAddAttrVec.size() > 3)
            {
                AddAttribute(attribute, pstColorUpgradeConfig->m_stAddAttrVec[3].first, pstColorUpgradeConfig->m_stAddAttrVec[3].second);
            }

            if (rstCardInfo.m_iUpgradeColor4 > 0 && pstColorUpgradeConfig->m_stAddAttrVec.size() > 4)
            {
                AddAttribute(attribute, pstColorUpgradeConfig->m_stAddAttrVec[4].first, pstColorUpgradeConfig->m_stAddAttrVec[4].second);
            }
        }

        //卡片培养
        attribute.m_dAtk += rstCardInfo.m_iTrainAttr1;
        attribute.m_dDef += rstCardInfo.m_iTrainAttr2;
        attribute.m_dHp += rstCardInfo.m_iTrainAttr3;
        attribute.m_dCri += rstCardInfo.m_iTrainAttr4;
    }

    return attribute;
}

attribute_elem_t CLogicAttribute::CalcCardFateAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo)
{
    attribute_elem_t attribute = {};
    const auto iterFate = CLogicConfig::GetHeroFateConfig().m_stConfigMap.find(rstCardInfo.m_iCardID);
    if (iterFate != CLogicConfig::GetHeroFateConfig().m_stConfigMap.end())
    {
        for (size_t position = 1; position < iterFate->second.size(); ++position)
        {
            uint64_t uiMask = (1ULL << (position - 1));
            if (rstCardInfo.m_ulHeroFate & uiMask)
            {
                attribute.m_dPercentHp += iterFate->second[position].m_fHPRatio * 100;
                attribute.m_dPercentAtk += iterFate->second[position].m_fATKRatio * 100;
                attribute.m_dPercentDef += iterFate->second[position].m_fDEFRatio * 100;
                attribute.m_dPercentCri += iterFate->second[position].m_fCRIRatio * 100;
            }
        }
    }

    return attribute;
}


attribute_elem_t CLogicAttribute::CalcCardMysteryAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo)
{
    attribute_elem_t attribute = {};
    const auto& pstConfig = CLogicConfig::GetCardMysteryConfig().GetCardMysteryConfig(rstCardInfo.m_iCardID);
    if (pstConfig != nullptr)
    {
        for (const auto& open : pstConfig->m_stOpenMap)
        {
            const auto ulFlag = 1ULL << open.first;
            if (rstCardInfo.m_ulMysteryBitmap & ulFlag)
            {
                attribute.m_dFightPower += open.second.m_iFightPower;
            }
        }

        for (const auto& level : pstConfig->m_stLevelMap)
        {
            if (level.first <= rstCardInfo.m_iMysteryFinalLevel)
            {
                attribute.m_dAtk += level.second.m_iAddATK;
                attribute.m_dCri += level.second.m_iAddCRI;
                attribute.m_dDef += level.second.m_iAddDEF;
                attribute.m_dHp += level.second.m_iAddHP;
            }
        }

        for (const auto& final : pstConfig->m_stFinalMap)
        {
            if (final.first.first == rstCardInfo.m_iMysteryFinalType && final.first.second * 5 + 1 <= rstCardInfo.m_iMysteryFinalLevel)
            {
                AddAttribute(attribute, final.second.m_iAttrPerType, final.second.m_iAttrPerValue, true);
                attribute.m_dFightPower += final.second.m_iFightPower;
            }
        }
    }
    return attribute;
}

attribute_elem_t CLogicAttribute::CalcCardEquipAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo)
{
    attribute_elem_t attribute = {};
    const auto* pstConfig = CLogicConfig::GetHeroCardConfig().GetConfig(rstCardInfo.m_iCardID);
    if (pstConfig != nullptr)
    {
        //装备等级，星级，颜色
        auto GetUpgradeConfigFunc = std::bind(&CLogicConfigHeroEquipmentParser::GetConfigElem
                , CLogicConfig::GetHeroEquipmentConfig()
                , pstConfig->m_iCardType
                , std::placeholders::_1
                , std::placeholders::_2
                , std::placeholders::_3
                , false);

        auto GetPersonalUpgradeConfigFunc = std::bind(&CLogicConfigHeroPersonalParser::GetConfigElem
                , CLogicConfig::GetHeroPersonalConfig()
                , rstCardInfo.m_iCardID
                , std::placeholders::_1
                , std::placeholders::_2
                , false);

        std::vector<const TLogicHeroEquipmentBaseElem*> upgrade_configs;
        //装备等级
        upgrade_configs.push_back(GetUpgradeConfigFunc(1, "level", rstCardInfo.m_iEquipmentLevel1));
        upgrade_configs.push_back(GetUpgradeConfigFunc(2, "level", rstCardInfo.m_iEquipmentLevel2));
        upgrade_configs.push_back(GetUpgradeConfigFunc(3, "level", rstCardInfo.m_iEquipmentLevel3));
        upgrade_configs.push_back(GetUpgradeConfigFunc(4, "level", rstCardInfo.m_iEquipmentLevel4));
        upgrade_configs.push_back(GetUpgradeConfigFunc(5, "level", rstCardInfo.m_iEquipmentLevel5));
        upgrade_configs.push_back(GetUpgradeConfigFunc(6, "level", rstCardInfo.m_iEquipmentLevel6));
        //装备颜色
        upgrade_configs.push_back(GetUpgradeConfigFunc(1, "rank", rstCardInfo.m_iEquipmentColor1));
        upgrade_configs.push_back(GetUpgradeConfigFunc(2, "rank", rstCardInfo.m_iEquipmentColor2));
        upgrade_configs.push_back(GetUpgradeConfigFunc(3, "rank", rstCardInfo.m_iEquipmentColor3));
        upgrade_configs.push_back(GetUpgradeConfigFunc(4, "rank", rstCardInfo.m_iEquipmentColor4));
        upgrade_configs.push_back(GetUpgradeConfigFunc(5, "rank", rstCardInfo.m_iEquipmentColor5));
        upgrade_configs.push_back(GetUpgradeConfigFunc(6, "rank", rstCardInfo.m_iEquipmentColor6));
        //装备星级
        upgrade_configs.push_back(GetPersonalUpgradeConfigFunc(1, rstCardInfo.m_iEquipmentStar1));
        upgrade_configs.push_back(GetPersonalUpgradeConfigFunc(2, rstCardInfo.m_iEquipmentStar2));
        upgrade_configs.push_back(GetUpgradeConfigFunc(3, "star", rstCardInfo.m_iEquipmentStar3));
        upgrade_configs.push_back(GetUpgradeConfigFunc(4, "star", rstCardInfo.m_iEquipmentStar4));
        upgrade_configs.push_back(GetUpgradeConfigFunc(5, "star", rstCardInfo.m_iEquipmentStar5));
        upgrade_configs.push_back(GetUpgradeConfigFunc(6, "star", rstCardInfo.m_iEquipmentStar6));

        for (const auto& config : upgrade_configs)
        {
            if (config)
            {
                for (const auto& attr : config->m_stAddAttrVec)
                {
                    AddAttribute(attribute, attr.m_iAttrType, attr.m_iAttrNum);
                }
            }
        }

        //装备宝石
        std::vector<const TLogicEquipmentJewelBaseElem*> jewel_configs;
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(1, 1, rstCardInfo.m_iEquipmentJewel1Slot1));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(2, 1, rstCardInfo.m_iEquipmentJewel2Slot1));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(3, 1, rstCardInfo.m_iEquipmentJewel3Slot1));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(4, 1, rstCardInfo.m_iEquipmentJewel4Slot1));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(5, 1, rstCardInfo.m_iEquipmentJewel5Slot1));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(6, 1, rstCardInfo.m_iEquipmentJewel6Slot1));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(1, 2, rstCardInfo.m_iEquipmentJewel1Slot2));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(2, 2, rstCardInfo.m_iEquipmentJewel2Slot2));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(3, 2, rstCardInfo.m_iEquipmentJewel3Slot2));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(4, 2, rstCardInfo.m_iEquipmentJewel4Slot2));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(5, 2, rstCardInfo.m_iEquipmentJewel5Slot2));
        jewel_configs.push_back(CLogicConfig::GetEquipmentJewelConfig().GetConfigElem(6, 2, rstCardInfo.m_iEquipmentJewel6Slot2));
        for (const auto& config : jewel_configs)
        {
            if (config)
            {
                for (const auto& attr : config->m_stAddAttrVec)
                {
                    AddAttribute(attribute, attr.m_iAttrType, attr.m_dAttrNum);
                }
            }
        }

        // 星纹
        std::vector<int32_t> stConstelID;
        stConstelID.push_back(rstCardInfo.m_iEquipConstellation1);
        stConstelID.push_back(rstCardInfo.m_iEquipConstellation2);
        stConstelID.push_back(rstCardInfo.m_iEquipConstellation3);
        stConstelID.push_back(rstCardInfo.m_iEquipConstellation4);
        stConstelID.push_back(rstCardInfo.m_iEquipConstellation5);
        stConstelID.push_back(rstCardInfo.m_iEquipConstellation6);

        std::map<int32_t, int32_t> stSuit;
        for(auto constelID : stConstelID)
        {
            if(constelID <= 0) continue;

            auto iter = pUserInfo->m_stConstelMap.Find(constelID);
            if(iter == pUserInfo->m_stConstelMap.End()) continue;

            const auto& stValue = iter->second->m_stData;
            if(stValue.m_iAttrType1 > 0) AddAttribute(attribute, stValue.m_iAttrType1, CLogicConfig::GetConstelAttrConfig().GetAttrValue(stValue.m_iAttrType1, stValue.m_iAttrGrade1));
            if(stValue.m_iAttrType2 > 0) AddAttribute(attribute, stValue.m_iAttrType2, CLogicConfig::GetConstelAttrConfig().GetAttrValue(stValue.m_iAttrType2, stValue.m_iAttrGrade2));
            if(stValue.m_iAttrType3 > 0) AddAttribute(attribute, stValue.m_iAttrType3, CLogicConfig::GetConstelAttrConfig().GetAttrValue(stValue.m_iAttrType3, stValue.m_iAttrGrade3));
            if(stValue.m_iAttrType4 > 0) AddAttribute(attribute, stValue.m_iAttrType4, CLogicConfig::GetConstelAttrConfig().GetAttrValue(stValue.m_iAttrType4, stValue.m_iAttrGrade4));
            if(stValue.m_iAttrType5 > 0) AddAttribute(attribute, stValue.m_iAttrType5, CLogicConfig::GetConstelAttrConfig().GetAttrValue(stValue.m_iAttrType5, stValue.m_iAttrGrade5));

            ++stSuit[stValue.m_iSuit];
        }

        // 星纹套装
        const auto& stSuitConfigMap = CLogicConfig::GetConstelAttrConfig().m_stSuitMap;
        for(auto suit : stSuit)
        {
            auto iterConfig = stSuitConfigMap.find(suit.first);
            if(iterConfig == stSuitConfigMap.end()) continue;
            for(auto& config : iterConfig->second)
            {
                if(config.m_iSuitNum > suit.second) continue;

                if(config.m_iEffectType == CLogicConfigDefine::ECSET_NormalEffect)
                {
                    AddAttribute(attribute, config.m_iAttrType, config.m_dAttrValue);
                }
                if(config.m_iEffectType == CLogicConfigDefine::ECSET_SkillEffect)
                {
                    attribute.m_dFightPower += config.m_iFightPower;
                }
            }
        }
    }

    return attribute;
}

attribute_elem_t CLogicAttribute::CalcCardPetAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo)
{
    double xunchong_ratio = 1.0;
    const auto iterXunchongConfig = CLogicConfig::GetXunchongConfig().m_mapXunchongLevel.find(pUserInfo->m_stUserInfoTable.GetXunchongLevel());
    if (iterXunchongConfig != CLogicConfig::GetXunchongConfig().m_mapXunchongLevel.end())
    {
        xunchong_ratio = iterXunchongConfig->second.m_dAttr / 100.0;
    }

    attribute_elem_t attribute = {};
    for (auto iPetID : { rstCardInfo.m_iProtectPetID1, rstCardInfo.m_iProtectPetID2, rstCardInfo.m_iProtectPetID3 })
    {
        const double attr_ratio = (iPetID == rstCardInfo.m_iProtectPetID1 ? 1.0 : 0.5) * xunchong_ratio;
        const double skill_ratio = (iPetID == rstCardInfo.m_iProtectPetID1 ? 1.0 : 0.0);
        const double special_ratio = (iPetID == rstCardInfo.m_iProtectPetID1 ? 1.0 : 0.5);

        const auto iterPet = pUserInfo->m_stPetInfoMap.Find(iPetID);
        const auto pstPetConfig = CLogicConfig::GetPetConfig().GetPetConfig(iPetID);
        if (nullptr != pstPetConfig && iterPet != pUserInfo->m_stPetInfoMap.End())
        {
            const auto iterPetStar = pstPetConfig->m_stStarMap.find(iterPet->second->GetPetStar());
            if (iterPetStar != pstPetConfig->m_stStarMap.end())
            {
                attribute.m_dAtk += iterPetStar->second.m_iATK * attr_ratio;
                attribute.m_dDef += iterPetStar->second.m_iDEF * attr_ratio;
                attribute.m_dHp += iterPetStar->second.m_iHP * attr_ratio;
                attribute.m_dCri += iterPetStar->second.m_iCRI * attr_ratio;

                const auto iterPetSkill = pstPetConfig->m_stSkillPowerMap.find(iterPetStar->second.m_iSkillLevel);
                if (iterPetSkill != pstPetConfig->m_stSkillPowerMap.end())
                {
                    attribute.m_dFightPower += iterPetSkill->second * skill_ratio;
                }
            }

            const auto iterPetLevel = pstPetConfig->m_stLevelMap.find(CLogicConfig::GetPetConfig().GetLevelByExp(iPetID, iterPet->second->GetPetExp()));
            if (iterPetLevel != pstPetConfig->m_stLevelMap.end())
            {
                attribute.m_dAtk += iterPetLevel->second.m_iATK * attr_ratio;
                attribute.m_dDef += iterPetLevel->second.m_iDEF * attr_ratio;
                attribute.m_dHp += iterPetLevel->second.m_iHP * attr_ratio;
                attribute.m_dCri += iterPetLevel->second.m_iCRI * attr_ratio;

                const auto iterPetSpecial = pstPetConfig->m_stSpecialPowerMap.find(iterPetLevel->second.m_iSpecialtyLevel);
                if (iterPetSpecial != pstPetConfig->m_stSpecialPowerMap.end())
                {
                    attribute.m_dFightPower += iterPetSpecial->second * special_ratio;
                }
            }
        }
    }
    return attribute;
}

attribute_elem_t CLogicAttribute::CalcCardAmuletAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo)
{
    attribute_elem_t attribute = {};
    const std::vector<int32_t> amulet_unique_ids = {
            rstCardInfo.m_iAmuletUniqueID1, rstCardInfo.m_iAmuletUniqueID2, rstCardInfo.m_iAmuletUniqueID3, rstCardInfo.m_iAmuletUniqueID4
    };
    for (const auto& unique_id : amulet_unique_ids)
    {
        const auto iterAmulet = pUserInfo->m_stAmuletInfoMap.Find(unique_id);
        if (iterAmulet != pUserInfo->m_stAmuletInfoMap.End())
        {
            const auto* pstLevelConfig = CLogicConfig::GetAmuletConfig().GetAmuletLevelConfig(iterAmulet->second->GetAmuletID(), iterAmulet->second->GetGradeID(), iterAmulet->second->GetLevel());
            if (nullptr != pstLevelConfig)
            {
                for (const auto& attr : pstLevelConfig->m_stAttrVec)
                {
                    AddAttribute(attribute, attr.first, attr.second);
                }
            }

            AddAttribute(attribute, iterAmulet->second->GetDestinyAttrType1(), iterAmulet->second->GetDestinyAttrValue1());
            AddAttribute(attribute, iterAmulet->second->GetDestinyAttrType2(), iterAmulet->second->GetDestinyAttrValue2());
            AddAttribute(attribute, iterAmulet->second->GetDestinyAttrType3(), iterAmulet->second->GetDestinyAttrValue3());

            const std::vector<int32_t> amulet_skill_ids = {
                    iterAmulet->second->GetSkillID1(), iterAmulet->second->GetSkillID2(), iterAmulet->second->GetSkillID3(), iterAmulet->second->GetSkillID4()
            };
            for (const auto& skill_id : amulet_skill_ids)
            {
                const auto* pstSkillConfig = CLogicConfig::GetAmuletConfig().GetAmuletSkillConfig(skill_id);
                if (nullptr != pstSkillConfig)
                {
                    attribute.m_dFightPower += pstSkillConfig->m_iPower;
                }
            }
        }
    }

    return attribute;
}

attribute_elem_t CLogicAttribute::CalcCardCharacterAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo)
{
    attribute_elem_t attribute = {};
    const std::vector<std::pair<int32_t, int32_t>> characters = {
            { rstCardInfo.m_iCharacter1, rstCardInfo.m_iCharacterColor1 },
            { rstCardInfo.m_iCharacter2, rstCardInfo.m_iCharacterColor2 },
            { rstCardInfo.m_iCharacter3, rstCardInfo.m_iCharacterColor3 },
            { rstCardInfo.m_iCharacter4, rstCardInfo.m_iCharacterColor4 },
            { rstCardInfo.m_iCharacter5, rstCardInfo.m_iCharacterColor5 },
    };

    const auto* pstConfig = CLogicConfig::GetHeroCardConfig().GetConfig(rstCardInfo.m_iCardID);
    if (pstConfig != nullptr)
    {
        for (const auto& character : characters)
        {
            const auto iterCharacterAttr = CLogicConfig::GetCardCharacterConfig().m_stCharacterMap.find(character);
            if (iterCharacterAttr != CLogicConfig::GetCardCharacterConfig().m_stCharacterMap.end())
            {
                if (std::find(pstConfig->m_iSpecialCharacterVec.begin(), pstConfig->m_iSpecialCharacterVec.end(), character.first) == pstConfig->m_iSpecialCharacterVec.end())
                {
                    attribute.m_dFightPower += iterCharacterAttr->second.m_iFightPower;
                }
                else
                {
                    attribute.m_dFightPower += iterCharacterAttr->second.m_iFightPower * 2;
                }
            }
        }
    }

    return attribute;
}

void CLogicAttribute::RefreshUserFashionAttribute(user_data_table_ptr_type pUserInfo)
{
    pUserInfo->m_stOnlyInCacheData.m_stFashionAttribute = {};

    for (auto iterFashion = pUserInfo->m_stFashionInfoMap.Begin(); iterFashion != pUserInfo->m_stFashionInfoMap.End(); ++iterFashion)
    {
        if (iterFashion->second->GetExpiredTime() == 0 || iterFashion->second->GetExpiredTime() >= CLogicCommon::GetSec())
        {
            const auto iterFashionConfig = CLogicConfig::GetFashionConfig().m_stFashionMap.find(iterFashion->first);
            if (iterFashionConfig != CLogicConfig::GetFashionConfig().m_stFashionMap.end())
            {
                for (const auto& attr : iterFashionConfig->second.m_stAttrMap)
                {
                    AddAttribute(pUserInfo->m_stOnlyInCacheData.m_stFashionAttribute, attr.first, attr.second, true);
                }
            }
        }
        else if (iterFashion->second->GetEquipCardID())
        {
            iterFashion->second->SetEquipCardID(0);
            CLogicUserNotifySystem::NotifyUpdateFashion(pUserInfo, iterFashion->first, iterFashion->second->GetExpiredTime(), iterFashion->second->GetEquipCardID());
        }
    }

    const auto& attribute = pUserInfo->m_stOnlyInCacheData.m_stFashionAttribute;
    LOGIC_LOG_TRACE_INFO << "GROUP_ID:" << pUserInfo->m_iGroupID << "|UIN:" << pUserInfo->m_iUin << "|STAT_FASHION"
                         << "|ATK:" << attribute.m_dAtk << "|DEF:" << attribute.m_dDef
                         << "|HP:" << attribute.m_dHp << "|CRI:" << attribute.m_dCri << "|ATK_PERCENT:" << attribute.m_dPercentAtk
                         << "|DEF_PERCENT:" << attribute.m_dPercentDef << "|HP_PERCENT:" << attribute.m_dPercentHp
                         << "|CRI_PERCENT:" << attribute.m_dPercentCri << std::endl;
}

const attribute_elem_t& CLogicAttribute::GetUserFashionAttribute(user_data_table_ptr_type pUserInfo)
{
    return pUserInfo->m_stOnlyInCacheData.m_stFashionAttribute;
}

void CLogicAttribute::RefreshUserThemeAttribute(user_data_table_ptr_type pUserInfo)
{
    pUserInfo->m_stOnlyInCacheData.m_stThemeAttribute = {};

    for (auto iterTheme = pUserInfo->m_stThemeInfoMap.Begin(); iterTheme != pUserInfo->m_stThemeInfoMap.End(); ++iterTheme)
    {
        const auto *pstThemeConfig = CLogicConfig::GetThemeConfig().GetThemeConfig(iterTheme->first);
        if (pstThemeConfig)
        {
            for (const auto &attr : pstThemeConfig->m_stAttrMap)
            {
                AddAttribute(pUserInfo->m_stOnlyInCacheData.m_stThemeAttribute, attr.first, attr.second, true);
            }
        }
    }

    const auto &attribute = pUserInfo->m_stOnlyInCacheData.m_stThemeAttribute;
    LOGIC_LOG_TRACE_INFO << "GROUP_ID:" << pUserInfo->m_iGroupID << "|UIN:" << pUserInfo->m_iUin << "|STAT_FASHION"
                         << "|ATK:" << attribute.m_dAtk << "|DEF:" << attribute.m_dDef << "|HP:" << attribute.m_dHp
                         << "|CRI:" << attribute.m_dCri << "|ATK_PERCENT:" << attribute.m_dPercentAtk
                         << "|DEF_PERCENT:" << attribute.m_dPercentDef << "|HP_PERCENT:" << attribute.m_dPercentHp
                         << "|CRI_PERCENT:" << attribute.m_dPercentCri << std::endl;
}

const attribute_elem_t& CLogicAttribute::GetUserThemeAttribute(user_data_table_ptr_type pUserInfo)
{
  return pUserInfo->m_stOnlyInCacheData.m_stThemeAttribute;
}

void CLogicAttribute::RefreshUserMilitaryAttribute(user_data_table_ptr_type pUserInfo)
{
    pUserInfo->m_stOnlyInCacheData.m_stMilitaryAttribute = {};
    const auto iterWarLevel = CLogicConfig::GetEntireWarConfig().m_stMilitaryRankMap.find(pUserInfo->m_stUserInfoTable.GetEntireWarMilitarkLevel());
    if (iterWarLevel != CLogicConfig::GetEntireWarConfig().m_stMilitaryRankMap.end())
    {
        pUserInfo->m_stOnlyInCacheData.m_stMilitaryAttribute.m_dAtk += iterWarLevel->second.m_iATK;
        pUserInfo->m_stOnlyInCacheData.m_stMilitaryAttribute.m_dDef += iterWarLevel->second.m_iDEF;
        pUserInfo->m_stOnlyInCacheData.m_stMilitaryAttribute.m_dCri += iterWarLevel->second.m_iCRI;
        pUserInfo->m_stOnlyInCacheData.m_stMilitaryAttribute.m_dHp += iterWarLevel->second.m_iHP;
    }

    const auto& attribute = pUserInfo->m_stOnlyInCacheData.m_stMilitaryAttribute;
    LOGIC_LOG_TRACE_INFO << "GROUP_ID:" << pUserInfo->m_iGroupID << "|UIN:" << pUserInfo->m_iUin << "|STAT_Military"
                         "|ATK:" << attribute.m_dAtk << "|DEF:" << attribute.m_dDef << "|HP:" << attribute.m_dHp
                         << "|CRI:" << attribute.m_dCri << std::endl;
}

const attribute_elem_t& CLogicAttribute::GetUserMilitaryAttribute(user_data_table_ptr_type pUserInfo)
{
    return pUserInfo->m_stOnlyInCacheData.m_stMilitaryAttribute;
}

void CLogicAttribute::RefreshUserHeartLevelAttribute(user_data_table_ptr_type pUserInfo)
{
    pUserInfo->m_stOnlyInCacheData.m_stHeartLevelAttribute = {};
    auto pConfig = CLogicConfig::GetHeroHeartConfig().GetMainCardLevelConfig(pUserInfo->m_stIBusyData.GetTotalHeartLevel());
    if(nullptr != pConfig)
    {
        for(auto& attr : pConfig->m_stAddAttr)
        {
            AddAttribute(pUserInfo->m_stOnlyInCacheData.m_stHeartLevelAttribute, attr.first, attr.second);
        }
    }

    const auto &attribute = pUserInfo->m_stOnlyInCacheData.m_stHeartLevelAttribute;
    LOGIC_LOG_TRACE_INFO << "GROUP_ID:" << pUserInfo->m_iGroupID << "|UIN:" << pUserInfo->m_iUin
                         << "|ATK:" << attribute.m_dAtk << "|DEF:" << attribute.m_dDef << "|HP:" << attribute.m_dHp
                         << "|CRI:" << attribute.m_dCri << "|ATK_PERCENT:" << attribute.m_dPercentAtk
                         << "|DEF_PERCENT:" << attribute.m_dPercentDef << "|HP_PERCENT:" << attribute.m_dPercentHp
                         << "|CRI_PERCENT:" << attribute.m_dPercentCri << std::endl;
}

const attribute_elem_t& CLogicAttribute::GetUserHeartLevelAttribute(user_data_table_ptr_type pUserInfo)
{
    return pUserInfo->m_stOnlyInCacheData.m_stHeartLevelAttribute;
}

void CLogicAttribute::RefreshUserTalentAttribute(user_data_table_ptr_type pUserInfo)
{
    pUserInfo->m_stOnlyInCacheData.m_stTalentAttributeMap.clear();

    CLogicCacheCardOperation stOperation(pUserInfo);
    for (auto iterCard = pUserInfo->m_stHeroCardTableMap.Begin(); iterCard != pUserInfo->m_stHeroCardTableMap.End(); ++iterCard)
    {
        if (stOperation.CheckHeroCardExist(iterCard->second))
        {
            const auto pstTalent = CLogicConfig::GetTalentConfig().GetHeroTalentConfig(iterCard->first);
            if (pstTalent && pstTalent->m_iHeroCardType >= 0 && pstTalent->m_iHeroCardType < 10)
            {
                const auto isPercentage = (pstTalent->m_iTalentNumType == 1);
                attribute_elem_t& attribute = pUserInfo->m_stOnlyInCacheData.m_stTalentAttributeMap[pstTalent->m_iHeroCardType];
                const auto iterStar = pstTalent->m_astStar.find(iterCard->second->GetCardStar());
                if (iterStar != pstTalent->m_astStar.end())
                {
                    AddAttribute(attribute, pstTalent->m_iType, iterStar->second.m_dTalentNum, isPercentage);
                }
                const auto iterColor = pstTalent->m_astColor.find(iterCard->second->GetCardColor());
                if (iterColor != pstTalent->m_astColor.end())
                {
                    AddAttribute(attribute, pstTalent->m_iType, iterColor->second.m_dTalentNum, isPercentage);
                }
            }
        }
    }

    for (const auto& talent : pUserInfo->m_stOnlyInCacheData.m_stTalentAttributeMap)
    {
        const auto& attribute = talent.second;
        LOGIC_LOG_TRACE_INFO << "GROUP_ID:" << pUserInfo->m_iGroupID << "|UIN:" << pUserInfo->m_iUin << "|STAT_TALENT"
            << "|CARD_TYPE:" << talent.first << "|ATK:" << attribute.m_dAtk << "|DEF:" << attribute.m_dDef
            << "|HP:" << attribute.m_dHp << "|CRI:" << attribute.m_dCri << "|ATK_PERCENT:" << attribute.m_dPercentAtk
            << "|DEF_PERCENT:" << attribute.m_dPercentDef << "|HP_PERCENT:" << attribute.m_dPercentHp
            << "|CRI_PERCENT:" << attribute.m_dPercentCri << std::endl;
    }
}

const attribute_elem_t& CLogicAttribute::GetCardTypeTalentAttribute(user_data_table_ptr_type pUserInfo, int32_t iCardType)
{
    static attribute_elem_t attribute = {};
    const auto iterAttribute = pUserInfo->m_stOnlyInCacheData.m_stTalentAttributeMap.find(iCardType);
    return iterAttribute != pUserInfo->m_stOnlyInCacheData.m_stTalentAttributeMap.end() ? iterAttribute->second : attribute;
}

void CLogicAttribute::RefreshUserHonorAttribute(user_data_table_ptr_type pUserInfo)
{
    pUserInfo->m_stOnlyInCacheData.m_stHonorAttributeMap.clear();

    for (auto iterHonor = pUserInfo->m_stUserHonorMap.Begin(); iterHonor != pUserInfo->m_stUserHonorMap.End(); ++iterHonor)
    {
        if (CLogicConfigDefine::HONOR_STATUS_EQUIP == iterHonor->second->GetHonorStatus() || CLogicConfigDefine::HONOR_STATUS_UNLOCK == iterHonor->second->GetHonorStatus())
        {
            if (0 == iterHonor->second->GetExpireTime() || iterHonor->second->GetExpireTime() > CLogicCommon::GetSec())
            {
                const auto* pstHonorConfig = CLogicConfig::GetHonorConfig().GetHonorConfig(iterHonor->first);
                if (nullptr != pstHonorConfig)
                {
                    for (const auto& attr : pstHonorConfig->m_stAttrVec)
                    {
                        if (attr.m_iCardType >= 0 && attr.m_iCardType < 20)
                        {
                            const auto isPercentage = (attr.m_iNumType == 1);
                            attribute_elem_t& attribute = pUserInfo->m_stOnlyInCacheData.m_stHonorAttributeMap[attr.m_iCardType];
                            AddAttribute(attribute, attr.m_iAttrType, attr.m_dAttrValue, isPercentage);
                        }
                    }
                }
            }
        }
    }

    for (const auto& honor : pUserInfo->m_stOnlyInCacheData.m_stHonorAttributeMap)
    {
        const auto& attribute = honor.second;
        LOGIC_LOG_TRACE_INFO << "GROUP_ID:" << pUserInfo->m_iGroupID << "|UIN:" << pUserInfo->m_iUin << "|STAT_HONOR"
                             << "|CARD_TYPE:" << honor.first << "|ATK:" << attribute.m_dAtk << "|DEF:" << attribute.m_dDef
                             << "|HP:" << attribute.m_dHp << "|CRI:" << attribute.m_dCri << "|ATK_PERCENT:" << attribute.m_dPercentAtk
                             << "|DEF_PERCENT:" << attribute.m_dPercentDef << "|HP_PERCENT:" << attribute.m_dPercentHp
                             << "|CRI_PERCENT:" << attribute.m_dPercentCri << std::endl;
    }
}

const attribute_elem_t& CLogicAttribute::GetCardTypeHonorAttribute(user_data_table_ptr_type pUserInfo, int32_t iCardType)
{
    static attribute_elem_t attribute = {};
    const auto iterAttribute = pUserInfo->m_stOnlyInCacheData.m_stHonorAttributeMap.find(iCardType);
    return iterAttribute != pUserInfo->m_stOnlyInCacheData.m_stHonorAttributeMap.end() ? iterAttribute->second : attribute;
}

void CLogicAttribute::RefreshUserGuildAttribute(user_data_table_ptr_type pUserInfo)
{
    pUserInfo->m_stOnlyInCacheData.m_stGuildAttributeMap.clear();
    for (const auto& skill : CLogicConfig::GetGuildSkillConfig().m_astSkillConfig)
    {
        if (skill.second.m_iHeroCardType >= 0 && skill.second.m_iHeroCardType < 10)
        {
            const auto iterSkill = pUserInfo->m_stUserGuildSkillMap.Find(skill.second.m_iSkillID);
            if (iterSkill != pUserInfo->m_stUserGuildSkillMap.End())
            {
                const auto iterSkillLevel = skill.second.m_astLevel.find(iterSkill->second->GetSkillLevel());
                if (iterSkillLevel != skill.second.m_astLevel.end())
                {
                    auto& attribute = pUserInfo->m_stOnlyInCacheData.m_stGuildAttributeMap[skill.second.m_iHeroCardType];
                    AddAttribute(attribute, skill.second.m_iAddAttrType, iterSkillLevel->second.m_dAddAttrNum);
                }
            }
        }
    }

    for (const auto& guild : pUserInfo->m_stOnlyInCacheData.m_stGuildAttributeMap)
    {
        const auto& attribute = guild.second;
        LOGIC_LOG_TRACE_INFO << "GROUP_ID:" << pUserInfo->m_iGroupID << "|UIN:" << pUserInfo->m_iUin << "|STAT_GUILD"
                             << "|CARD_TYPE:" << guild.first << "|ATK:" << attribute.m_dAtk << "|DEF:" << attribute.m_dDef
                             << "|HP:" << attribute.m_dHp << "|CRI:" << attribute.m_dCri << "|ATK_PERCENT:" << attribute.m_dPercentAtk
                             << "|DEF_PERCENT:" << attribute.m_dPercentDef << "|HP_PERCENT:" << attribute.m_dPercentHp
                             << "|CRI_PERCENT:" << attribute.m_dPercentCri << std::endl;
    }
}

const attribute_elem_t& CLogicAttribute::GetCardTypeGuildAttribute(user_data_table_ptr_type pUserInfo, int32_t iCardType)
{
    static attribute_elem_t attribute = {};
    const auto iterAttribute = pUserInfo->m_stOnlyInCacheData.m_stGuildAttributeMap.find(iCardType);
    return iterAttribute != pUserInfo->m_stOnlyInCacheData.m_stGuildAttributeMap.end() ? iterAttribute->second : attribute;
}

*/