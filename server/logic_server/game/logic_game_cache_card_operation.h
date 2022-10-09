#pragma once

#include "logic_config_card.h"
#include "logic_game_cache_table.h"

class CLogicCacheCardOperation
{
public:
    CLogicCacheCardOperation(user_data_table_ptr_type pUserData);

    bool CheckHeroCardExist(int iCardID);

    bool CheckRealHeroCardExist(int iCardID);
    
    bool CheckHeroCardExist(const THeroCardInfoTable* pHeroCard);
    
    const TLogicHeroCardConfigElem* GetHeroCardConfig (int iCardID);
    
    bool UpgradeCardStar(THeroCardInfoTable& stHeroCard, int32_t iCmd);
    
    bool CombineCard(int iCardID, int32_t iCmd);
    
    bool UpgradeCardLevel(int iCardID, int iAddCardExp);

    bool UpgradeTeamCardLevelExp(int iBonusExp, std::vector<std::pair<int, int>>& stRspCardLevelExp, int iTeamType = 0);

    bool UpgradeCardStar(int iCardID, int32_t iCmd);
    
    bool UpgradeCardColor(int iCardID, int32_t iCmd);

private:
    user_data_table_ptr_type     m_pUserData;
};
