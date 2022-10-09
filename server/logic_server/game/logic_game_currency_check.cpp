#include "server_errcode.h"
#include "logic_link_error.h"
#include "logic_game_currency_check.h"

bool
CLogicCurrencyCheck::CheckEnoughRoleLevel(const user_data_table_ptr_type pUser, int iRequireNum)
{
    assert(nullptr != pUser && "pUser CAN NOT nullptr");
    if (pUser->m_stUserInfoTable.GetLevel() < iRequireNum)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(NOT_ENOUGH_ROLE_LEVEL);
        return (false);
    }
    return true;
}

bool
CLogicCurrencyCheck::CheckEnoughGold(const user_data_table_ptr_type pUser, int iRequireNum)
{
    assert(nullptr != pUser && "pUser CAN NOT nullptr");
    if (pUser->m_stUserInfoTable.GetGold() < iRequireNum)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(NOT_ENOUGH_GOLD);
        return (false);
    }
    return true;
}

bool
CLogicCurrencyCheck::CheckEnoughPhyPower(const user_data_table_ptr_type pUser, int iRequireNum)
{
    assert(nullptr != pUser && "pUser CAN NOT nullptr");
    if (pUser->m_stUserInfoTable.GetPhyPower() < iRequireNum)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(NOT_ENOUGH_PHYPOWER);
        return (false);
    }
    return true;
}

bool
CLogicCurrencyCheck::CheckEnoughVitality(const user_data_table_ptr_type pUser, int iRequireNum)
{
    assert(nullptr != pUser && "pUser CAN NOT nullptr");
    if (pUser->m_stUserInfoTable.GetVitality() < iRequireNum)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(NOT_ENOUGH_VITALITY);
        return (false);
    }
    return true;
}

bool CLogicCurrencyCheck::CheckEnoughHeroHeartLevel(const user_data_table_ptr_type pUser, int iCardID, int iRequireNum)
{
    assert(nullptr != pUser && "pUser CAN NOT nullptr");
    auto iter = pUser->m_stHeroCardTableMap.Find(iCardID);
    if(iter == pUser->m_stHeroCardTableMap.End() || iter->second->GetCardLevel() <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(HERO_CARD_IS_NOT_OWNED);
        return (false);
    }

    if(iter->second->GetHeartLevel() < iRequireNum)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(HERO_HEART_LEVEL_LIMIT);
        return (false);
    }

    return true;
}
