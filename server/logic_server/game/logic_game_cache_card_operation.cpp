#include <set>
#include <cassert>
#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "logic_link_common.h"
#include "logic_config.h"
#include "logic_game_item.h"
#include "logic_game_cache_common.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_publisher.h"
#include "logic_game_global.h"
#include "logic_game_item_factory.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_common_publisher.h"

CLogicCacheCardOperation::CLogicCacheCardOperation(user_data_table_ptr_type pUserData)
: m_pUserData(pUserData) { assert(nullptr != m_pUserData && "USERDATA CAN NOT INIT nullptr"); }

bool
CLogicCacheCardOperation::CheckHeroCardExist(int iCardID)
{
	if (CLogicCacheCommon::IsMainRoleCardID(iCardID))
	{
		iCardID = CLogicCacheCommon::GetMainRoleCardID(m_pUserData);
	}

    return CheckRealHeroCardExist(iCardID);
}

bool CLogicCacheCardOperation::CheckRealHeroCardExist(int iCardID)
{
    auto stIter = m_pUserData->m_stHeroCardTableMap.Find(iCardID);
    if (stIter != m_pUserData->m_stHeroCardTableMap.End())
    {
        return CheckHeroCardExist(stIter->second);
    }

    using namespace SERVER_ERRCODE;
    SET_ERR_INFO(HERO_CARD_IS_NOT_OWNED);
    return (false);
}

bool    
CLogicCacheCardOperation::CheckHeroCardExist(const THeroCardInfoTable* pHeroCard)
{
    if (!pHeroCard || (pHeroCard->GetCardLevel() <= 0))
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(HERO_CARD_IS_NOT_OWNED);
        return (false);
    }
    
    return (true);
}

const TLogicHeroCardConfigElem*
CLogicCacheCardOperation::GetHeroCardConfig(int iCardID)
{
    auto pHeroCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iCardID);
    if(!pHeroCardConfig)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(HERO_CARD_IS_NOT_CONFIGED);
        return (nullptr);
    }
    
    return (pHeroCardConfig);
}

bool    
CLogicCacheCardOperation::UpgradeCardStar(THeroCardInfoTable& stHeroCard, int32_t iCmd)
{
    int iStar = 1;
    if(stHeroCard.GetCardLevel()) iStar = (int)stHeroCard.GetCardStar();
    auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stHeroCard.m_stData.m_iCardID);
    if(nullptr == pConfig)
    {
        return false;
    }
    
    int iConsumeHeroSoulNum = CLogicConfig::GetUpgradeHeroCardStarConfig().UpgradeStarNeedSoul(iStar);
    int iConsumeGoldNum = CLogicConfig::GetUpgradeHeroCardStarConfig().UpgradeStarNeedGold(iStar);
    if((iConsumeHeroSoulNum <= 0) || (iConsumeGoldNum < 0))
    {
        using namespace SERVER_ERRCODE;
        CLogicError::SetErrCode(HERO_CARD_STAR_IS_REACH_CONFIG_MAX, GET_ERRMSG_BY_ERRCODE(HERO_CARD_STAR_IS_REACH_CONFIG_MAX));
        return (false);
    }

    std::vector<CPackGameItem> stConsumeVec;
    stConsumeVec.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GoldID, iConsumeGoldNum);
    stConsumeVec.emplace_back(CLogicConfigDefine::EXCHANGE_ITEM, pConfig->m_iSoulID, iConsumeHeroSoulNum);
    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserData, stConsumeVec, iCmd)) return (false);
    
    stHeroCard.AddCardStar(1);

    CLogicGameCardChannel::data_type stData;
    stData.m_iCardOption = CLogicGameChannelEnum::UPGRADE_CARD_STAR_OPTION;
    LOGIC_PUBLISHER_INS->PublishGameCardInfo(m_pUserData, stData);
    
    return (true);
}

bool    
CLogicCacheCardOperation::CombineCard(int iCardID, int32_t iCmd)
{
    auto stIter = m_pUserData->m_stHeroCardTableMap.Find(iCardID);
    if (stIter != m_pUserData->m_stHeroCardTableMap.End())
    {
        SET_ERR_INFO(SERVER_ERRCODE::HERO_CARD_IS_EXIST);
        return (false);
    }

    auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iCardID);
    if(nullptr == pConfig)
    {
        return false;
    }
    
    int iNeedHeroSoul = 0;
    for (int i = 0; i < pConfig->m_iInitStar; ++i)
    {
        iNeedHeroSoul += CLogicConfig::GetUpgradeHeroCardStarConfig().UpgradeStarNeedSoul(i);
    }
    
    auto stHeroCardSoul = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(CLogicConfigDefine::EXCHANGE_ITEM, pConfig->m_iSoulID, iNeedHeroSoul, iCmd, m_pUserData);
    if (!stHeroCardSoul.get() || !stHeroCardSoul->CanConsume()) return (false);

    stHeroCardSoul->Consume();

    CLogicCacheCommon::AddGameItem(m_pUserData, CLogicConfigDefine::HERO_CARD, iCardID, 1, iCmd);
    
    return (true);
}

bool CLogicCacheCardOperation::UpgradeCardLevel(int iCardID, int iAddCardExp)
{
    auto stIter = m_pUserData->m_stHeroCardTableMap.Find(iCardID);
    if (stIter == m_pUserData->m_stHeroCardTableMap.End())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(HERO_CARD_IS_NOT_OWNED);
        return (false);
    }

    auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iCardID);
    if (pCardConfig == nullptr)
    {
        return (false);
    }
    
    if(iAddCardExp <= 0) return (true);

    std::pair<int, int> stRet = CLogicConfig::GetUpgradeHeroCardLevelConfig().UpgradeCardLevel(m_pUserData->m_stUserInfoTable.GetLevel(),
                                                                                                 stIter->second->GetCardLevel(), 
                                                                                                 stIter->second->GetCardExp(), 
                                                                                                 iAddCardExp);
    int iOldLevel = stIter->second->GetCardLevel();
    stIter->second->SetCardLevel(stRet.first);
    stIter->second->SetCardExp(stRet.second);

    if(iOldLevel != stRet.first)
    {
        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_UpgradeCardLevel;
        stEventData.m_iPara = iOldLevel;
        stEventData.m_iParaEx = stRet.first;
        stEventData.m_iCmd = m_pUserData->m_iCommand;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserData, stEventData);
    }
    
    return (true);
}

bool
CLogicCacheCardOperation::UpgradeTeamCardLevelExp(int iBonusExp, std::vector<std::pair<int, int>>& stRspCardLevelExp, int iTeamType/* = 0*/)
{
    if(iTeamType == 0) iTeamType = CLogicConfigDefine::ETT_MainTeam;

    int iCardLevel, iCardExp;
    auto stTeam = CLogicCacheCommon::GetTeam(m_pUserData, iTeamType);
    for(auto cardID : stTeam)
    {
        iCardLevel = 0, iCardExp = 0;
        if(cardID > 0)
        {
            UpgradeCardLevel(cardID, iBonusExp);
            auto stHeroCardIT = m_pUserData->m_stHeroCardTableMap.Find(cardID);
            if (stHeroCardIT != m_pUserData->m_stHeroCardTableMap.End())
            {
                iCardLevel = stHeroCardIT->second->GetCardLevel();
                iCardExp = stHeroCardIT->second->GetCardExp();
            }
        }

        stRspCardLevelExp.push_back(std::make_pair(iCardLevel, iCardExp));
    }

    return (true);
}

bool    
CLogicCacheCardOperation::UpgradeCardStar(int iCardID, int32_t iCmd)
{
    auto stIter = m_pUserData->m_stHeroCardTableMap.Find(iCardID);
    if (stIter == m_pUserData->m_stHeroCardTableMap.End())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(HERO_CARD_IS_NOT_OWNED);
        return (false);
    }
    
    return (UpgradeCardStar(*(stIter->second), iCmd));
}

bool CLogicCacheCardOperation::UpgradeCardColor(int iCardID, int32_t iCmd)
{
    auto stIter = m_pUserData->m_stHeroCardTableMap.Find(iCardID);
    if (stIter == m_pUserData->m_stHeroCardTableMap.End())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(HERO_CARD_IS_NOT_OWNED);
        return (false);
    }

    if (stIter->second->GetUpgradeColor1() == 0 ||
        stIter->second->GetUpgradeColor2() == 0 ||
        stIter->second->GetUpgradeColor3() == 0 ||
        stIter->second->GetUpgradeColor4() == 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(NOT_ALL_MATERIAL_COLLECTED);
        return (false);
    }

    auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iCardID);
    if (pCardConfig == nullptr) return (false);

    int iColor = stIter->second->GetCardColor();
    auto pCardUpgradeConfig = CLogicConfig::GetUpgradeHeroCardColorConfig().GetConfig(pCardConfig->m_iCardType, iColor);
    if (pCardUpgradeConfig == nullptr) return (false);

    if (stIter->second->GetCardLevel() < pCardUpgradeConfig->m_iLevelLimit)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(NOT_ENOUGH_CARD_LEVEL);
        return (false);
    }

    if (CLogicConfig::GetUpgradeHeroCardColorConfig().GetConfig(pCardConfig->m_iCardType, iColor + 1) == nullptr)
    {
        return (false);
    }

    if (!CLogicCacheCommon::ConsumeGameItem(m_pUserData, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GoldID, pCardUpgradeConfig->m_iConsumeGold, iCmd))
        return (false);

    stIter->second->AddCardColor(1);
    stIter->second->SetUpgradeColor1(0);
    stIter->second->SetUpgradeColor2(0);
    stIter->second->SetUpgradeColor3(0);
    stIter->second->SetUpgradeColor4(0);

    stIter->second->AddTalentPoint(CLogicConfig::GetUpgradeHeroCardColorConfig().m_iUpgradeTalentNum);
    stIter->second->AddTalentPoint1(CLogicConfig::GetUpgradeHeroCardColorConfig().m_iUpgradeTalentNum1);

    CLogicGameCardChannel::data_type stData;
    stData.m_iCardOption = CLogicGameChannelEnum::UPGRADE_CARD_COLOR_OPTION;
    stData.m_iPara = iCardID;
    LOGIC_PUBLISHER_INS->PublishGameCardInfo(m_pUserData, stData);

	CLogicGlobal::PublishBroadcast("upgrade_card_color", stIter->second->GetCardColor(), {
            {"nick", m_pUserData->m_strNick},
            {"card_id", std::to_string(iCardID) },
            {"card_name", CLogicConfig::GetItemName(CLogicConfigDefine::HERO_CARD, iCardID)},
    });

    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_UpgradeCardColor;
    stEventData.m_iPara = stIter->second->GetCardColor();
    stEventData.m_iCmd = iCmd;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserData, stEventData);

    // 通用事件
    CLogicEventData stEventTargetData;
    stEventTargetData.m_eType = CLogicConfigDefine::ECET_UpGradeHeroCard;
    stEventTargetData.m_iPara = iCardID;
    stEventTargetData.m_iParaEx = stIter->second->GetCardColor();
    stEventTargetData.m_iCmd = iCmd;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserData, stEventTargetData);

    return (true);
}
