//
// Created by hzl on 2019/8/29.
//

#pragma once

#include "logic_game_cache_table.h"

class CLogicAttribute
{
public:
    static int32_t CalcUserTeamFightPower(user_data_table_ptr_type pUserInfo, int32_t iTeamType);

    static int32_t CalcUserTotalFightPower(user_data_table_ptr_type pUserInfo, const std::vector<int32_t>& vcCardID);

    static int32_t CalcClientFightPower(user_data_table_ptr_type pUserInfo, int32_t iFightPower1, int32_t iFightPower2);

    static int32_t CalcUserCardFightPower(user_data_table_ptr_type pUserInfo, int32_t iCardID);

    static double CalcUserWushenFightPower(user_data_table_ptr_type pUserInfo);

    static double CalcUserTacticsFightPower(user_data_table_ptr_type pUserInfo);

private:
    static void AddAttribute(attribute_elem_t &stAttribute, int32_t iType, int32_t iValue);

    static attribute_elem_t CalcCardUpgradeAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo);

    static attribute_elem_t CalcCardFateAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo);

    static attribute_elem_t CalcCardMysteryAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo);

    static attribute_elem_t CalcCardEquipAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo);

    static attribute_elem_t CalcCardPetAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo);

    static attribute_elem_t CalcCardAmuletAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo);

    static attribute_elem_t CalcCardCharacterAttribute(user_data_table_ptr_type pUserInfo, const THeroCardInfoTableValueType &rstCardInfo);

private:
    static void RefreshUserFashionAttribute(user_data_table_ptr_type pUserInfo);

    static const attribute_elem_t& GetUserFashionAttribute(user_data_table_ptr_type pUserInfo);

    static void RefreshUserThemeAttribute(user_data_table_ptr_type pUserInfo);

    static const attribute_elem_t& GetUserThemeAttribute(user_data_table_ptr_type pUserInfo);

    static void RefreshUserMilitaryAttribute(user_data_table_ptr_type pUserInfo);

    static const attribute_elem_t& GetUserMilitaryAttribute(user_data_table_ptr_type pUserInfo);

    static void RefreshUserHeartLevelAttribute(user_data_table_ptr_type pUserInfo);

    static const attribute_elem_t& GetUserHeartLevelAttribute(user_data_table_ptr_type pUserInfo);

    static void RefreshUserTalentAttribute(user_data_table_ptr_type pUserInfo);

    static const attribute_elem_t& GetCardTypeTalentAttribute(user_data_table_ptr_type pUserInfo, int32_t iCardType);

    static void RefreshUserHonorAttribute(user_data_table_ptr_type pUserInfo);

    static const attribute_elem_t& GetCardTypeHonorAttribute(user_data_table_ptr_type pUserInfo, int32_t iCardType);

    static void RefreshUserGuildAttribute(user_data_table_ptr_type pUserInfo);

    static const attribute_elem_t& GetCardTypeGuildAttribute(user_data_table_ptr_type pUserInfo, int32_t iCardType);
};

