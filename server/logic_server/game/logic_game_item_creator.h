//
// Created by hzl on 2019/2/20.
//

#pragma once

#include "logic_config.h"
#include "logic_game_item.h"
#include "logic_game_cache_common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CLogicGameItemCreator
{
public:
    template<class T>
    static CLogicGameItemI* CreateGameItemImp(int iAmount, int iGameItemID, user_data_table_ptr_type pUser)
    {
        return CreateGameItemImpHelper<T>(iAmount, iGameItemID, pUser, typename T::type());
    }

protected:
    template<class T>
    static CLogicGameItemI* CreateGameItemImpHelper(int iAmount, int iGameItemID, user_data_table_ptr_type pUser, currency_item_tag)
    {
        auto pConfig = CLogicConfig::GetCurrencyConfig().GetConfig(iGameItemID);
        if(pConfig) return (new T(iAmount, pConfig, pUser));
        return nullptr;
    }

    template<class T>
    static CLogicGameItemI* CreateGameItemImpHelper(int iAmount, int iGameItemID, user_data_table_ptr_type pUser, fashion_item_tag)
    {
        auto pConfig = CLogicConfig::GetFashionConfig().GetFashionConfig(iGameItemID);
        if(pConfig) return (new T(iAmount, pConfig, pUser));
        return nullptr;
    }

    template<class T>
    static CLogicGameItemI* CreateGameItemImpHelper(int iAmount, int iGameItemID, user_data_table_ptr_type pUser, item_tag)
    {
        auto pConfigElem = CLogicConfig::GetItemConfig().GetConfig(iGameItemID);
        if (pConfigElem) return (new T(iAmount, pConfigElem, pUser));
        return nullptr;
    }

    template<class T>
    static CLogicGameItemI* CreateGameItemImpHelper(int iAmount, int iGameItemID, user_data_table_ptr_type pUser, hero_card_item_tag)
    {
        auto pConfigElem = CLogicConfig::GetHeroCardConfig().GetConfig(iGameItemID);
        if (pConfigElem) return (new T(iAmount, pConfigElem, pUser));
        return nullptr;
    }

    template<class T>
    static CLogicGameItemI* CreateGameItemImpHelper(int iAmount, int iGameItemID, user_data_table_ptr_type pUser, avatar_item_tag)
    {
        auto pConfig = CLogicConfig::GetAvatarConfig().GetConfig(iGameItemID);
        if (pConfig)
        {
            return (new T(iAmount, pConfig, pUser));
        }
        else
        {
            return (nullptr);
        }
    }

    template<class T>
    static CLogicGameItemI* CreateGameItemImpHelper(int iAmount, int iGameItemID, user_data_table_ptr_type pUser, equip_item_tag)
    {
        auto pConfig = CLogicConfig::GetEquipItemConfig().GetConfig(iGameItemID);
        if(nullptr != pConfig)
        {
            return (new T(iAmount, pConfig, pUser));
        }
        else
        {
            return nullptr;
        }
    }

    template<class T>
    static CLogicGameItemI* CreateGameItemImpHelper(int iAmount, int iGameItemID, user_data_table_ptr_type pUser, constellation_tag)
    {
        auto pConfig = CLogicConfig::GetConstelConfig().GetConfig(iGameItemID);
        if(nullptr != pConfig)
        {
            return (new T(iAmount, pConfig, pUser));
        }
        else
        {
            return nullptr;
        }
    }
};
