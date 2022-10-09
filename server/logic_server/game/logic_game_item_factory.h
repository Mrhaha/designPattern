//
// Created by hzl on 2019/2/20.
//

#pragma once

#include "logic_config_base.h"
#include "logic_game_item_base.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
class CLogicGameItemFactory
{
public:
    typedef CLogicGameItemI* (*game_item_creator_func_type)(int, int, user_data_table_ptr_type);

    typedef shared_ptr<CLogicGameItemI> CRkLogicGameItemBasePtr;

    static CLogicGameItemFactory* GetInstance();

public:
    void RegisterGameItemFactory(CLogicConfigDefine::GAME_ITEM_COLLECTION enuGameItemType, game_item_creator_func_type pCreateFunc);

    bool CheckGameItem(CLogicConfigDefine::GAME_ITEM_COLLECTION enuGameItemType, int iGameItemID);

    CRkLogicGameItemBasePtr CreateGameItem(CLogicConfigDefine::GAME_ITEM_COLLECTION enuGameItemType, int iGameItemID, int iNum, int32_t iCmd, user_data_table_ptr_type pUser, const map<string, string>& info = {});
    CRkLogicGameItemBasePtr CreateGameItem(int iGameItemID, int iNum, int32_t iCmd, user_data_table_ptr_type pUser, const map<string, string>& info = {});

private:
    CLogicGameItemFactory() = default;

    game_item_creator_func_type GetGameItemCreatorFunc(CLogicConfigDefine::GAME_ITEM_COLLECTION enuGameItemType);

    game_item_creator_func_type m_pGameItemCreatorList[CLogicConfigDefine::GAME_ITEM_COLLECTION_END - CLogicConfigDefine::GAME_ITEM_COLLECTION_START - 1];

    static CLogicGameItemFactory* m_pInstance;
};

#define LOGIC_GAME_ITEM_FACTORY_INS CLogicGameItemFactory::GetInstance()
