//
// Created by hzl on 2019/2/20.
//

#include "logic_game_item_factory.h"
#include "logic_config.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CLogicGameItemFactory* CLogicGameItemFactory::m_pInstance = nullptr;

CLogicGameItemFactory *CLogicGameItemFactory::GetInstance()
{
    if(nullptr == m_pInstance) m_pInstance = new CLogicGameItemFactory();

    return (m_pInstance);
}


void
CLogicGameItemFactory::RegisterGameItemFactory(CLogicConfigDefine::GAME_ITEM_COLLECTION enuGameItemType, game_item_creator_func_type pCreateFunc)
{
    if((enuGameItemType > CLogicConfigDefine::GAME_ITEM_COLLECTION_START) && (enuGameItemType < CLogicConfigDefine::GAME_ITEM_COLLECTION_END))
    {
        m_pGameItemCreatorList[enuGameItemType - CLogicConfigDefine::GAME_ITEM_COLLECTION_START - 1] = pCreateFunc;
    }
}

CLogicGameItemFactory::game_item_creator_func_type
CLogicGameItemFactory::GetGameItemCreatorFunc(CLogicConfigDefine::GAME_ITEM_COLLECTION enuGameItemType)
{
    if((enuGameItemType > CLogicConfigDefine::GAME_ITEM_COLLECTION_START) && (enuGameItemType < CLogicConfigDefine::GAME_ITEM_COLLECTION_END))
    {
        return (m_pGameItemCreatorList[enuGameItemType - CLogicConfigDefine::GAME_ITEM_COLLECTION_START - 1]);
    }

    return (nullptr);
}

CLogicGameItemFactory::CRkLogicGameItemBasePtr
CLogicGameItemFactory::CreateGameItem(CLogicConfigDefine::GAME_ITEM_COLLECTION enuGameItemType, int iGameItemID, int iNum, int32_t iCmd, user_data_table_ptr_type pUser, const map<string, string>& info)
{
    CLogicGameItemI* pGameItem = nullptr;

    CLogicGameItemFactory::game_item_creator_func_type pCreateFunc = GetGameItemCreatorFunc(enuGameItemType);
    if(pCreateFunc != nullptr)
    {
        pGameItem = pCreateFunc(iNum, iGameItemID, pUser);
        if (pGameItem)
        {
            pGameItem->SetItemInfo(enuGameItemType, iGameItemID, iNum);
			pGameItem->SetCmd(iCmd);
            pGameItem->SetUserPtr(pUser);
			pGameItem->SetInfo(info);
        }
    }

    if(nullptr == pGameItem && 0 != enuGameItemType && nullptr != pUser)
    {
		LOGIC_LOG_TRACE_ERROR << "SYSTEM FAULT|UIN:" << pUser->m_iUin << "|GROUP_ID:" << pUser->m_iGroupID << "|CMD:" << iCmd
    		<< "|GAME_ITEM_TYPE:" << enuGameItemType << "|GAME_ITEM_ID:" << iGameItemID << "|IS NOT REGISTER CREATOR FUNCTION!" << endl;

		char szBuffer[64] = { 0 };
		snprintf(szBuffer, sizeof(szBuffer), "|UIN:%d|GROUP_ID:%d|CMD:%d|UNKNOW_GAME_ITEM:[%d, %d, %d]",
			pUser->m_iUin, pUser->m_iGroupID, iCmd,
			enuGameItemType,
			iGameItemID,
			iNum);

        LOGIC_SERVICE_API_INS->RequestServiceAlert("unknown_game_item", {
                { "uin" , std::to_string(pUser->m_iUin)},
                { "group_id", std::to_string(pUser->m_iGroupID)},
                { "cmd", std::to_string(iCmd) },
                { "cmd_name", "NO_CMD_NAME"},
                { "item_type", std::to_string(enuGameItemType)},
                { "item_id", std::to_string(iGameItemID)},
                { "item_num", std::to_string(iNum)}
        });
    }

    return (CRkLogicGameItemBasePtr(pGameItem));
}

CLogicGameItemFactory::CRkLogicGameItemBasePtr
CLogicGameItemFactory::CreateGameItem(int iGameItemID, int iNum, int32_t iCmd, user_data_table_ptr_type pUser, const map<string, string>& info)
{
    CLogicGameItemI* pGameItem = nullptr;

    CLogicGameItemFactory::game_item_creator_func_type pCreateFunc = GetGameItemCreatorFunc(CLogicConfigDefine::EXCHANGE_ITEM);
    if(pCreateFunc != nullptr)
    {
        pGameItem = pCreateFunc(iNum, iGameItemID, pUser);
        if (pGameItem)
        {
            pGameItem->SetItemInfo(CLogicConfigDefine::EXCHANGE_ITEM, iGameItemID, iNum);
            pGameItem->SetCmd(iCmd);
            pGameItem->SetUserPtr(pUser);
            pGameItem->SetInfo(info);
        }
    }

    if(nullptr == pGameItem && nullptr != pUser)
    {
        LOGIC_LOG_TRACE_ERROR << "SYSTEM FAULT|UIN:" << pUser->m_iUin << "|GROUP_ID:" << pUser->m_iGroupID << "|CMD:" << iCmd
                              << "|GAME_ITEM_ID:" << iGameItemID << "|IS NOT REGISTER CREATOR FUNCTION!" << endl;

        char szBuffer[64] = { 0 };
        snprintf(szBuffer, sizeof(szBuffer), "|UIN:%d|GROUP_ID:%d|CMD:%d|UNKNOW_GAME_ITEM:[%d, %d, %d]",
                 pUser->m_iUin, pUser->m_iGroupID, iCmd,
                 CLogicConfigDefine::EXCHANGE_ITEM,
                 iGameItemID,
                 iNum);

        LOGIC_SERVICE_API_INS->RequestServiceAlert("unknown_game_item", {
            { "uin" , std::to_string(pUser->m_iUin)},
            { "group_id", std::to_string(pUser->m_iGroupID)},
            { "cmd", std::to_string(iCmd) },
            { "cmd_name", "NO_CMD_NAME"},
            { "item_type", std::to_string(CLogicConfigDefine::EXCHANGE_ITEM)},
            { "item_id", std::to_string(iGameItemID)},
            { "item_num", std::to_string(iNum)}
        });
    }

    return (CRkLogicGameItemBasePtr(pGameItem));
}

bool
CLogicGameItemFactory::CheckGameItem(CLogicConfigDefine::GAME_ITEM_COLLECTION enuGameItemType, int iGameItemID)
{
    CLogicGameItemFactory::game_item_creator_func_type pCreateFunc = GetGameItemCreatorFunc(enuGameItemType);

    if(pCreateFunc == nullptr) return (false);

    auto pGameItem = pCreateFunc(1, iGameItemID, nullptr);

    if (pGameItem == nullptr) return (false);

    delete pGameItem;

    return (true);
}


