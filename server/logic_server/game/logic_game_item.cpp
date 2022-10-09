#include "server_errcode.h"
#include "logic_game_item.h"
#include "logic_game_item_factory.h"
#include "logic_game_item_creator.h"
#include "logic_game_global.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_currency_check.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_global.h"
#include "logic_game_active.h"
#include "logic_game_active_info.h"
#include "logic_game_bill.h"
#include "logic_game_common_publisher.h"

#define REGISTER_GAME_ITEM(GAME_ITEM_ID, ITEM_CLASS_NAME) \
struct _static_GAME_##ITEM_CLASS_NAME##_TEMP123456789 \
{\
    _static_GAME_##ITEM_CLASS_NAME##_TEMP123456789() \
    {\
        CLogicGameItemFactory::GetInstance()->RegisterGameItemFactory(GAME_ITEM_ID, CLogicGameItemCreator::CreateGameItemImp<ITEM_CLASS_NAME>); \
    }\
}; \
static _static_GAME_##ITEM_CLASS_NAME##_TEMP123456789 g_stInstance##ITEM_CLASS_NAME##_TEMP123456789;
////////////////////////////////////////////////////////////////////////////////////////////////////////
REGISTER_GAME_ITEM(CLogicConfigDefine::HERO_CARD, CLogicHeroCardGameItem)
REGISTER_GAME_ITEM(CLogicConfigDefine::CURRENCY, CLogicCurrencyGameItem)
REGISTER_GAME_ITEM(CLogicConfigDefine::FASHION, CLogicFashionGameItem)
REGISTER_GAME_ITEM(CLogicConfigDefine::AVATAR, CLogicAvatarGameItem)
REGISTER_GAME_ITEM(CLogicConfigDefine::EXCHANGE_ITEM, CLogicItemGameItem)
REGISTER_GAME_ITEM(CLogicConfigDefine::EQUIPMENT, CLogicEquipGameItem)
REGISTER_GAME_ITEM(CLogicConfigDefine::CONSTELLATION, CLogicConstellationGameItem)

/////////////////////////////////////////////////////////////////////////////////////////////////////
int CLogicCurrencyGameItem::RealAdd()
{
    bool bRecord = true;
    int32_t iCurNum = 0;
    auto iMaxValue = std::numeric_limits<int32_t>::max();
    switch (m_pConfig->m_iItemID)
    {
    case CLogicConfigDefine::ECIT_GoldID:
    {
        if(iMaxValue - m_iAmount < m_pUser->m_stUserInfoTable.GetGold())
        {
            m_pUser->m_stUserInfoTable.SetGold(iMaxValue);
        }
        else
        {
            m_pUser->m_stUserInfoTable.AddGold(m_iAmount);
        }

        iCurNum = m_pUser->m_stUserInfoTable.GetGold();
        break;
    }
    case CLogicConfigDefine::ECIT_EnergyID:
    {
        CLogicGameEnergyChannel::data_type stData;
        stData.m_iNum = m_iAmount;
        LOGIC_PUBLISHER_INS->PublishGameEnergyInfo(m_pUser, stData);

        m_pUser->m_stUserInfoTable.AddPhyPower(m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetPhyPower();
        break;
    }
    case CLogicConfigDefine::ECIT_YuanBaoID:
    {
        if(iMaxValue - m_iAmount < m_pUser->m_stUserInfoTable.GetRmb())
        {
            m_pUser->m_stUserInfoTable.SetRmb(iMaxValue);
        }
        else
        {
            m_pUser->m_stUserInfoTable.AddRmb(m_iAmount);
        }

        iCurNum = m_pUser->m_stUserInfoTable.GetRmb();
        break;
    }
    case CLogicConfigDefine::ECIT_PayYuanBaoID:
    {
        if(iMaxValue - m_iAmount < m_pUser->m_stUserInfoTable.GetPayRmb())
        {
            m_pUser->m_stUserInfoTable.SetPayRmb(iMaxValue);
        }
        else
        {
            m_pUser->m_stUserInfoTable.AddPayRmb(m_iAmount);
        }

        iCurNum = m_pUser->m_stUserInfoTable.GetPayRmb();
        break;
    }
    case CLogicConfigDefine::ECIT_RoleExpID:
    {
        int iPreExp = m_pUser->m_stUserInfoTable.GetExp();
        int iPreLevel = m_pUser->m_stUserInfoTable.GetLevel();
        std::pair<int, int> stLevelExpPair = CLogicConfig::GetUserExpConfig().ReCalcUserLevelExp(iPreLevel, iPreExp + m_iAmount);

        int iAddLevel = stLevelExpPair.first - iPreLevel;
        if(iAddLevel > 0)
        {
            CLogicCacheCommon::AddGameItem(m_pUser, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_RoleLevelID, iAddLevel, GetCmd());
        }
        m_pUser->m_stUserInfoTable.SetExp(stLevelExpPair.second);

        bRecord = false;
        break;
    }
    case CLogicConfigDefine::ECIT_RoleLevelID:
    {
        int iPreLevel = m_pUser->m_stUserInfoTable.GetLevel();
        m_pUser->m_stUserInfoTable.AddLevel(m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetLevel();

        LevelUpHandle(iPreLevel, iCurNum);
        break;
    }
    case CLogicConfigDefine::ECIT_VIPExpID:
    {
        if(iMaxValue - m_iAmount < m_pUser->m_stUserInfoTable.GetVipExp())
        {
            m_pUser->m_stUserInfoTable.SetVipExp(iMaxValue);
        }
        else
        {
            m_pUser->m_stUserInfoTable.AddVipExp(m_iAmount);
        }

        // 更新VIP等级
        m_pUser->m_stUserInfoTable.SetVipLevel(CLogicConfig::GetVipConfig().GetVipLevel(m_pUser->m_stUserInfoTable.GetVipExp()));

        iCurNum = m_pUser->m_stUserInfoTable.GetVipExp();
        break;
    }
    case CLogicConfigDefine::ECIT_EvilBrandID:
    {
        if(iMaxValue - m_iAmount < m_pUser->m_stUserInfoTable.GetEvilBrand())
        {
            m_pUser->m_stUserInfoTable.SetEvilBrand(iMaxValue);
        }
        else
        {
            m_pUser->m_stUserInfoTable.AddEvilBrand(m_iAmount);
        }

        iCurNum = m_pUser->m_stUserInfoTable.GetEvilBrand();
        break;
    }
    case CLogicConfigDefine::ECIT_VitalityID:
    {
        m_pUser->m_stUserInfoTable.AddVitality(m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetVitality();
        break;
    }
    case CLogicConfigDefine::ECIT_GuildCoinID:
    {
        if(iMaxValue - m_iAmount < m_pUser->m_stUserInfoTable.GetGuildCoin())
        {
            m_pUser->m_stUserInfoTable.SetGuildCoin(iMaxValue);
        }
        else
        {
            m_pUser->m_stUserInfoTable.AddGuildCoin(m_iAmount);
        }

        iCurNum = m_pUser->m_stUserInfoTable.GetGuildCoin();
        break;
    }
    case CLogicConfigDefine::ECIT_WarContributionID:
    {
        if(iMaxValue - m_iAmount < m_pUser->m_stUserInfoTable.GetEntireWarContribution())
        {
            m_pUser->m_stUserInfoTable.SetEntireWarContribution(iMaxValue);
        }
        else
        {
            m_pUser->m_stUserInfoTable.AddEntireWarContribution(m_iAmount);
        }

        m_pUser->m_stUserInfoTable.AddWeekWarContribution(m_iAmount);
        CLogicGlobal::UpdateOrInsertEntireWarRank(m_pUser->m_iUin, m_pUser->m_iGroupID, m_pUser->m_stUserInfoTable.GetWeekWarContribution());

        iCurNum = m_pUser->m_stUserInfoTable.GetEntireWarContribution();
        break;
    }
    case CLogicConfigDefine::ECIT_IbusyExpID:
    {
        m_pUser->m_stIBusyData.AddExp(m_iAmount);

        const auto iterActivity = CLogicConfig::GetIBusyConfig().m_mpActivities.find(m_pUser->m_stIBusyData.GetActivityID());
        if (iterActivity != CLogicConfig::GetIBusyConfig().m_mpActivities.end())
        {
            while (true)
            {
                auto findIter = iterActivity->second.m_mpLevelExp.find(m_pUser->m_stIBusyData.GetLevel());
                if (findIter != iterActivity->second.m_mpLevelExp.end() && m_pUser->m_stIBusyData.GetExp() >= findIter->second)
                {
                    m_pUser->m_stIBusyData.AddLevel(1);
                    m_pUser->m_stIBusyData.AddExp(-findIter->second);
                }
                else
                {
                    break;
                }
            }
        }

        CLogicUserNotifySystem::NotifyIBusyBaseData(m_pUser);
        iCurNum = m_pUser->m_stIBusyData.GetExp();
        break;
    }
    case CLogicConfigDefine::ECIT_ExploreExp:
    {
        iMaxValue = CLogicConfig::GetHomeGardenExploreConfig().m_iMaxExploreExp;
        if(iMaxValue - m_iAmount < m_pUser->m_stIBusyData.GetExploreExp())
        {
            if(iMaxValue > m_pUser->m_stIBusyData.GetExploreExp())
                m_pUser->m_stIBusyData.SetExploreExp(iMaxValue);
        }
        else
        {
            m_pUser->m_stIBusyData.AddExploreExp(m_iAmount);
        }

        iCurNum = m_pUser->m_stIBusyData.GetExploreExp();
        break;
    }
    case CLogicConfigDefine::ECIT_ScoreLevelID:
    {
        const auto activity_id = CLogicActive::GetScoreLevelOpenActivityID();
        auto iterActivity = m_pUser->m_stScoreLevelMap.Find(activity_id);
        if (iterActivity != m_pUser->m_stScoreLevelMap.End())
        {
            iterActivity->second->AddScore(m_iAmount);
            CLogicUserNotifySystem::NotifyScoreLevelActivityData(m_pUser);
            return iterActivity->second->GetScore();
        }

        bRecord = false;
        break;
    }
    case CLogicConfigDefine::ECIT_ScoreLevelXingDongLiID:
    {
        const auto activity_id = CLogicActive::GetScoreLevelOpenActivityID();
        auto iterActivity = m_pUser->m_stScoreLevelMap.Find(activity_id);
        if (iterActivity != m_pUser->m_stScoreLevelMap.End())
        {
            iterActivity->second->AddXingdongli(m_iAmount);
            CLogicUserNotifySystem::NotifyScoreLevelActivityData(m_pUser);
            return iterActivity->second->GetXingdongli();
        }

        bRecord = false;
        break;
    }
    case CLogicConfigDefine::ECIT_NewPassExpID:
    {
        m_pUser->m_stUserInfoTable.AddNewPassTotalExp(m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetNewPassTotalExp();
        break;
    }
    case CLogicConfigDefine::ECIT_WarActionCoinID:
    {
        m_pUser->m_stUserInfoTable.AddActionCoin(m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetActionCoin();
        break;
    }
    case CLogicConfigDefine::ECIT_ConstelExpID:
    {
        if(iMaxValue - m_iAmount < m_pUser->m_stIBusyData.GetConstelExpPool())
        {
            m_pUser->m_stIBusyData.SetConstelExpPool(iMaxValue);
        }
        else
        {
            m_pUser->m_stIBusyData.AddConstelExpPool(m_iAmount);
        }

        iCurNum = m_pUser->m_stIBusyData.GetConstelExpPool();
        break;
    }
    case CLogicConfigDefine::ECIT_HomeEXP:
        m_pUser->m_stUserInfoTable.AddHomeExp(m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetHomeExp();
        break;
    case CLogicConfigDefine::ECIT_ZhuHaiScore:
        m_pUser->m_stUserInfoTable.AddZhuhaiScore(m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetZhuhaiScore();
        break;
    default:
        return (-1);
    }

    if(bRecord)
    {
        CLogicFunctionalGameItemChannel::data_type stData;
        stData.m_iItemType = GetItemType();
        stData.m_iItemID = m_pConfig->m_iItemID;
        stData.m_iCurrentNum = iCurNum;
        stData.m_iDeltaNum = m_iAmount;
        stData.m_iCmd = GetCmd();
        stData.m_strInfo = GetInfo();
        LOGIC_PUBLISHER_INS->PublishFunctionalGameItemInfo(m_pUser, stData);
    }

    // 缓存(屏蔽某些渠道奖励)
    if(GetCmd() != MSG_LOGIC_PHY_ADD_BY_LEVEL_UP)
        m_pUser->m_stCacheBonusData.m_stBonus.m_stItems.emplace_back(GetItemType(), m_pConfig->m_iItemID, m_iAmount);

    if(m_pConfig->m_iItemID == CLogicConfigDefine::ECIT_GoldID)
    {
        // 通用事件
        CLogicEventData stEvent;
        stEvent.m_eType = CLogicConfigDefine::ECET_CurGoldNum;
        stEvent.m_iPara = m_pUser->m_stUserInfoTable.GetGold();
        stEvent.m_iCmd = GetCmd();
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEvent);

        // 徽章
        CLogicBadgeManage::UpdateBadgeCond(m_pUser, CLogicConfigDefine::EBCT_CollectGold, m_iAmount, {});
    }

    if(m_pConfig->m_iItemID == CLogicConfigDefine::ECIT_ExploreExp)
    {
        // 通用事件
        CLogicEventData stEvent;
        stEvent.m_eType = CLogicConfigDefine::ECET_ExploreLevel;
        stEvent.m_iPara = m_pUser->m_stIBusyData.GetExploreExp();
        stEvent.m_iCmd = GetCmd();
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEvent);
    }

    return m_iAmount;
}

int CLogicCurrencyGameItem::RealConsume()
{
    bool bRecord = true;
    int32_t iCurNum = 0;
    switch (m_pConfig->m_iItemID)
    {
    case CLogicConfigDefine::ECIT_GoldID:
    {
        m_pUser->m_stUserInfoTable.AddGold(-m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetGold();

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_ConsumeGoldCoin;
        stEventData.m_iCount = m_iAmount;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventData);
        break;
    }
    case CLogicConfigDefine::ECIT_EnergyID:
    {
        CLogicGameEnergyChannel::data_type stData;
        stData.m_iNum = -m_iAmount;
        LOGIC_PUBLISHER_INS->PublishGameEnergyInfo(m_pUser, stData);

        logic_game_user_action_channel_data_t stActionData;
        stActionData.m_iTimes = m_iAmount;
        stActionData.m_iAction = CLogicGameChannelEnum::ACTION_CONSUME_ENERGY;
        LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUser, stActionData);

        if (CLogicConfig::GetAwardConfig().m_stHappyTurnCard.m_iCostEnergy > 0)
        {
            const int32_t iHaveCount = m_pUser->m_stUserDailyTable.GetCostEnergy() / CLogicConfig::GetAwardConfig().m_stHappyTurnCard.m_iCostEnergy;
            m_pUser->m_stUserDailyTable.AddCostEnergy(m_iAmount);
            const int32_t iNewCount = m_pUser->m_stUserDailyTable.GetCostEnergy() / CLogicConfig::GetAwardConfig().m_stHappyTurnCard.m_iCostEnergy;
            m_pUser->m_stUserDailyTable.AddLeftTurnCardCount(iNewCount - iHaveCount);
            CLogicUserNotifySystem::NotifyUserDailyData(m_pUser);
        }

        m_pUser->m_stUserInfoTable.AddPhyPower(-m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetPhyPower();

        break;
    }
    case CLogicConfigDefine::ECIT_YuanBaoID:
    {
        // 优先消耗免费钻石，不够的话消耗付费钻石
        if(m_iAmount > m_pUser->m_stUserInfoTable.GetRmb())
        {
            CLogicCacheCommon::ConsumeGameItem(m_pUser,
                                               CLogicConfigDefine::CURRENCY,
                                               CLogicConfigDefine::ECIT_PayYuanBaoID,
                                               m_iAmount - m_pUser->m_stUserInfoTable.GetRmb(),
                                               GetCmd());

            m_pUser->m_stUserInfoTable.SetRmb(0);
        }
        else
        {
            m_pUser->m_stUserInfoTable.AddRmb(-m_iAmount);
        }

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_ConsumeDiamond;
        stEventData.m_iCount = m_iAmount;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventData);
        iCurNum = m_pUser->m_stUserInfoTable.GetRmb();
        break;
    }
    case CLogicConfigDefine::ECIT_PayYuanBaoID:
    {
        m_pUser->m_stUserInfoTable.AddPayRmb(-m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetPayRmb();

        //付费钻石平账记录
        auto payDiamondLeft = m_pUser->m_stUserPayDiamondTable.GetPayDiamondLeft();
        auto consumePayDiamond = m_iAmount;
        for(auto leftIter = payDiamondLeft.begin(); (leftIter != payDiamondLeft.end() && consumePayDiamond != 0);)
        {
            if (leftIter->m_iNum <= consumePayDiamond)
            {
                consumePayDiamond -= leftIter->m_iNum;
                leftIter = payDiamondLeft.erase(leftIter);
                continue;
            } else
            {
                leftIter->m_iNum -= consumePayDiamond;
                consumePayDiamond = 0;
            }
            leftIter++;
        }
        m_pUser->m_stUserPayDiamondTable.SetPayDiamondLeft(payDiamondLeft);

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_ConsumeDiamond;
        stEventData.m_iCount = m_iAmount;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventData);
        break;
    }
    case CLogicConfigDefine::ECIT_RoleLevelID:
    {
        m_pUser->m_stUserInfoTable.AddLevel(-m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetLevel();
        break;
    }
    case CLogicConfigDefine::ECIT_EvilBrandID:
    {
        m_pUser->m_stUserInfoTable.AddEvilBrand(-m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetEvilBrand();
        break;
    }
    case CLogicConfigDefine::ECIT_VitalityID:
    {
        m_pUser->m_stUserInfoTable.AddVitality(-m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetVitality();
        break;
    }
    case CLogicConfigDefine::ECIT_GuildCoinID:
    {
        m_pUser->m_stUserInfoTable.AddGuildCoin(-m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetGuildCoin();
        break;
    }
    case CLogicConfigDefine::ECIT_WarContributionID:
    {
        m_pUser->m_stUserInfoTable.AddEntireWarContribution(-m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetEntireWarContribution();
        break;
    }
    case CLogicConfigDefine::ECIT_ConstelExpID:
    {
        m_pUser->m_stIBusyData.AddConstelExpPool(-m_iAmount);
        iCurNum = m_pUser->m_stIBusyData.GetConstelExpPool();
        break;
    }
    case CLogicConfigDefine::ECIT_ZhuHaiScore:
    {
        m_pUser->m_stUserInfoTable.AddZhuhaiScore(-m_iAmount);
        iCurNum = m_pUser->m_stUserInfoTable.GetZhuhaiScore();
        break;
    }
    default:
        return (-1);
    }

    if(bRecord)
    {
        CLogicFunctionalGameItemChannel::data_type stData;
        stData.m_iItemType = GetItemType();
        stData.m_iItemID = m_pConfig->m_iItemID;
        stData.m_iCurrentNum = iCurNum;
        stData.m_iDeltaNum = -m_iAmount;
        stData.m_iCmd = GetCmd();
        stData.m_strInfo = GetInfo();
        LOGIC_PUBLISHER_INS->PublishFunctionalGameItemInfo(m_pUser, stData);
    }

    // 缓存
    m_pUser->m_stCacheBonusData.m_stConsume.m_stItems.emplace_back(GetItemType(), m_pConfig->m_iItemID, m_iAmount);

    return m_iAmount;
}

bool CLogicCurrencyGameItem::CanConsume() const
{
    switch (m_pConfig->m_iItemID)
    {
    case CLogicConfigDefine::ECIT_GoldID:
    {
        if (m_pUser->m_stUserInfoTable.GetGold() < m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_GOLD);
            return false;
        }
        break;
    }
    case CLogicConfigDefine::ECIT_EnergyID:
    {
        if(m_pUser->m_stUserInfoTable.GetPhyPower() < m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_PHYPOWER);
            return false;
        }
        break;
    }
    case CLogicConfigDefine::ECIT_YuanBaoID:
    {
        if(m_pUser->m_stUserInfoTable.GetRmb() + m_pUser->m_stUserInfoTable.GetPayRmb() < m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_RMB);
            return false;
        }
        break;
    }
    case CLogicConfigDefine::ECIT_PayYuanBaoID:
    {
        if(m_pUser->m_stUserInfoTable.GetPayRmb() < m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_PAY_RMB);
            return false;
        }
        break;
    }
    case CLogicConfigDefine::ECIT_RoleLevelID:
    {
        if(m_pUser->m_stUserInfoTable.GetLevel() <= m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL);
            return false;
        }
        break;
    }
    case CLogicConfigDefine::ECIT_EvilBrandID:
    {
        if (m_pUser->m_stUserInfoTable.GetEvilBrand() < m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_EVIL_BRAND);
            return false;
        }
        break;
    }
    case CLogicConfigDefine::ECIT_VitalityID:
    {
        if (m_pUser->m_stUserInfoTable.GetVitality() < m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_VITALITY);
            return false;
        }
        break;
    }
    case CLogicConfigDefine::ECIT_GuildCoinID:
    {
        if (m_pUser->m_stUserInfoTable.GetGuildCoin() < m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_GUILD_COIN);
            return false;
        }
        break;
    }
    case CLogicConfigDefine::ECIT_WarContributionID:
    {
        if (m_pUser->m_stUserInfoTable.GetEntireWarContribution() < m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::WAR_CONTRIBUTION_NOT_ENOUGH);
            return false;
        }
        break;
    }
    case CLogicConfigDefine::ECIT_ConstelExpID:
    {
        if(m_pUser->m_stIBusyData.GetConstelExpPool() < m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
            return false;
        }
        break;
    }
    case CLogicConfigDefine::ECIT_ZhuHaiScore:
    {
        if(m_pUser->m_stUserInfoTable.GetZhuhaiScore() < m_iAmount)
        {
            SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
            return false;
        }
        break;
    }
    default:
    {
        SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
        return false;
    }
    }

    return true;
}

void CLogicCurrencyGameItem::LevelUpHandle(int iPreLevel, int iCurLevel)
{
    int iAddEnergy = 0;
    const auto& stUserExpConfig = CLogicConfig::GetUserExpConfig().m_stUserExpConfig;
    while (iPreLevel < iCurLevel)
    {
        if(iPreLevel < int(stUserExpConfig.size())) iAddEnergy += stUserExpConfig[iPreLevel].m_iAddPhyPower;
        ++iPreLevel;
    }

    CLogicCacheCommon::AddGameItem(m_pUser, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_EnergyID, iAddEnergy, MSG_LOGIC_PHY_ADD_BY_LEVEL_UP);

    CLogicGlobal::AddAsyncNpcMail(m_pUser->m_iUin, m_pUser->m_iGroupID);

    LOGIC_PUBLISHER_INS->PublishGameUserLevelUpInfo(m_pUser);

    if (m_pUser->m_stUserInfoTable.GetLevel() == CLogicConfig::GetQiyuConfig().m_iOpenLevel)
    {
        CLogicUserNotifySystem::NotifyQiyuData(m_pUser);
    }

    // 文文日报
    std::vector<std::string> stWenWenParam;
    stWenWenParam.push_back(m_pUser->m_stUserInfoTable.GetNick());
    stWenWenParam.push_back(to_string(m_pUser->m_stUserInfoTable.GetLevel()));
    CLogicGlobal::UpdateWenWenNewsEvent(m_pUser->m_iUin, m_pUser->m_iGroupID, CLogicConfigDefine::EWWNT_Role_Level,stWenWenParam);

    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_RoleLevelUp;
    stEventData.m_iPara = m_pUser->m_stUserInfoTable.GetLevel() - m_iAmount;
    stEventData.m_iParaEx = m_pUser->m_stUserInfoTable.GetLevel();
    stEventData.m_iCmd = GetCmd();
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventData);
}

int CLogicFashionGameItem::RealAdd()
{
    auto iterFashion = m_pUser->m_stFashionInfoMap.Find(m_pConfig->m_iFashionID);
    if (iterFashion == m_pUser->m_stFashionInfoMap.End())
    {
        TFashionInfoTableValueType stVal;
        stVal.m_iUid = m_pUser->m_iUin;
        stVal.m_iGroupID = m_pUser->m_iGroupID;
        stVal.m_iFashionID = m_pConfig->m_iFashionID;
        stVal.m_iExpiredTime = 0;
        const auto stRet = m_pUser->m_stFashionInfoMap.Insert(std::make_pair(stVal.m_iFashionID, stVal));
        if (!stRet.second) return (-1);
        iterFashion = stRet.first;

        CLogicFunctionalGameItemChannel::data_type stData;
        stData.m_iItemType = GetItemType();
        stData.m_iItemID = m_pConfig->m_iFashionID;
        stData.m_iCurrentNum = 1;
        stData.m_iDeltaNum = 1;
        stData.m_iCmd = GetCmd();
        stData.m_strInfo = GetInfo();
        LOGIC_PUBLISHER_INS->PublishFunctionalGameItemInfo(m_pUser, stData);

        m_pUser->m_stCacheBonusData.m_stBonus.m_stFashionVec.emplace_back(CLogicCacheCommon::GetFashionInfo(iterFashion->second->m_stData));
    }
    else
    {
        if(m_pConfig->m_iYuanBao > 0)
            CLogicCacheCommon::AddGameItem(m_pUser, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, m_pConfig->m_iYuanBao, GetCmd());
    }

    return m_iAmount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CLogicItemGameItem::RealAdd()
{
    // 自动使用的道具
    if(m_pConfig->m_bAutoUse)
    {
        switch (m_pConfig->m_iUseType)
        {
        case CLogicConfigDefine::EIUT_GiftBag:
        {
            std::vector<CPackGameItem> stItemVec;
            if(CLogicConfig::GetGiftBagConfig().OpenGift(m_pConfig->m_iItemID, stItemVec, m_iAmount))
            {
                CLogicCacheCommon::AddGameItem(m_pUser, stItemVec, GetCmd());
                return m_iAmount;
            }
            break;
        }
        case CLogicConfigDefine::EIUT_Buff:
        {
            if(CLogicBuff::AddBuff(m_pUser, m_pConfig->m_iUsePara1) == SERVER_ERRCODE::SUCCESS)
                return m_iAmount;

            break;
        }
        default:
            break;
        }
    }

	auto stIter = m_pUser->m_stItemMap.Find(m_pConfig->m_iItemID);
	if (stIter == m_pUser->m_stItemMap.End())
	{
		TUserItemTableValueType stItemValue;
		stItemValue.m_iGroupID = m_pUser->m_iGroupID;
		stItemValue.m_iUid = m_pUser->m_iUin;
        stItemValue.m_iItemID = m_pConfig->m_iItemID;
		stItemValue.m_iOpenedCount = 0;
        stItemValue.m_iAmount = 0;
        stItemValue.m_iAddUpAmount = 0;

        // 家园音乐，记录获得时间
        if(m_pConfig->m_iSubType == CLogicConfigDefine::EIST_Music)
            stItemValue.m_iNextOpenTime = CLogicCommon::GetSec();

		typename decltype(m_pUser->m_stItemMap)::table_value_type stTableValue(stItemValue.m_iItemID, stItemValue);
		auto stRet = m_pUser->m_stItemMap.Insert(stTableValue);
		if (!stRet.second) return (-1);

        stIter = stRet.first;
	}

    int32_t iOldNum = stIter->second->GetAmount();
    // 如果是限制获取数量的道具
    if(m_pConfig->m_iUniqueCount > 0)
    {
        int iMissNum = m_iAmount;
        if(stIter->second->GetAddUpAmount() < m_pConfig->m_iUniqueCount)
        {
            int iLastCount = m_pConfig->m_iUniqueCount - stIter->second->GetAddUpAmount();
            int iCanAddNum = std::min(m_iAmount, iLastCount);
            iMissNum = m_iAmount - iCanAddNum;

            stIter->second->AddAmount(iCanAddNum);
            stIter->second->AddAddUpAmount(iCanAddNum);
        }

        // 重复获得兑换道具
        if(m_pConfig->m_stRepeatExchangeItem.m_iItemType > 0 && iMissNum > 0)
        {
            auto stExchangeItem = m_pConfig->m_stRepeatExchangeItem;
            stExchangeItem.m_iNum *= iMissNum;
            CLogicCacheCommon::AddGameItem(m_pUser, stExchangeItem, GetCmd());
        }
    } else if (m_pConfig->m_iHasCountLimit > 0)
    {
        if(m_pConfig->m_iHasCountLimit > stIter->second->GetAmount())
        {
            int iCanAddNum = std::min(m_iAmount, m_pConfig->m_iHasCountLimit - stIter->second->GetAmount());
            stIter->second->AddAmount(iCanAddNum);
        }
    } else if (m_pConfig->m_iHomeBagCountLimit > 0) //家园和背包共持有有限制
    {
        int iMissNum = m_iAmount;
        int iTotalGoodCount = stIter->second->GetAmount();
        int iHomeGoodCount = 0;
        for(auto& iterGoods : m_pUser->m_stHomeGoodsMap)
        {
            for(auto iter = iterGoods.second.Begin(); iter != iterGoods.second.End(); ++iter)
            {
                if (iter->second->GetGoodsID() == m_pConfig->m_iItemID)
                {
                    iHomeGoodCount++;
                }
            }
        }
        iTotalGoodCount += iHomeGoodCount;
        if (iTotalGoodCount <= m_pConfig->m_iHomeBagCountLimit)
        {
            int iLastCount = m_pConfig->m_iHomeBagCountLimit - iTotalGoodCount;
            int iCanAddNum = std::min(m_iAmount, iLastCount);
            iMissNum = m_iAmount - iCanAddNum;

            stIter->second->AddAmount(iCanAddNum);
            stIter->second->AddAddUpAmount(iCanAddNum);
        }

        // 重复获得兑换道具
        if(m_pConfig->m_stRepeatExchangeItem.m_iItemType > 0 && iMissNum > 0)
        {
            auto stExchangeItem = m_pConfig->m_stRepeatExchangeItem;
            stExchangeItem.m_iNum *= iMissNum;
            CLogicCacheCommon::AddGameItem(m_pUser, stExchangeItem, GetCmd());
        }
    }
    else
    {
        auto iMaxValue = std::numeric_limits<int32_t>::max();
        if(iMaxValue - m_iAmount < stIter->second->GetAmount())
        {
            stIter->second->SetAmount(iMaxValue);
        }
        else
        {
            stIter->second->AddAmount(m_iAmount);
        }
    }

	CLogicFunctionalGameItemChannel::data_type stData;
	stData.m_iItemType = GetItemType();
	stData.m_iItemID = m_pConfig->m_iItemID;
	stData.m_iCurrentNum = stIter->second->GetAmount();
	stData.m_iDeltaNum = m_iAmount;
	stData.m_iCmd = GetCmd();
	stData.m_strInfo = GetInfo();
	LOGIC_PUBLISHER_INS->PublishFunctionalGameItemInfo(m_pUser, stData);

    iOldNum = stIter->second->GetAmount() - iOldNum;

    if(iOldNum > 0)
    {
        // 伙伴专属技能点道具在伙伴升级时通过通知的方式发放，因为某些结算界面不希望显示技能点获得
        if(m_pConfig->m_iSubType != CLogicConfigDefine::EIST_SkillPoint)
        {
            m_pUser->m_stCacheBonusData.m_stBonus.m_stItems.emplace_back(GetItemType(), m_pConfig->m_iItemID, iOldNum);
        }

        // 通用事件
        if(m_pConfig->m_iSubType == CLogicConfigDefine::EIST_RuneCard)
        {
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_RuneCardNum;
            stEventData.m_iTimes = iOldNum;
            stEventData.m_iCmd = GetCmd();
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventData);

            // 徽章
            CLogicBadgeManage::UpdateBadgeCond(m_pUser, CLogicConfigDefine::EBCT_CollectRuneCard, iOldNum, {});

            if(m_pConfig->m_iTypePara1)
            {
                CLogicBadgeManage::UpdateBadgeCond(m_pUser, CLogicConfigDefine::EBCT_CollectSpecRuneCard, iOldNum, {});
            }
        }

        // 通用事件
        if(GetCmd() != MSG_LOGIC_HOME_SAVE_HOME && m_pConfig->m_iSubType == CLogicConfigDefine::EIST_HomeOrnament)
        {
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_HomeOrnamentNum;
            stEventData.m_iTimes = iOldNum;
            stEventData.m_iCmd = GetCmd();
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventData);
        }
    }

	return (m_iAmount);
}

int CLogicItemGameItem::RealConsume()
{
	auto stIter = m_pUser->m_stItemMap.Find(m_pConfig->m_iItemID);
	if (stIter != m_pUser->m_stItemMap.End())
	{
        stIter->second->AddAmount(-m_iAmount);

        CLogicFunctionalGameItemChannel::data_type stData;
        stData.m_iItemType = GetItemType();
        stData.m_iItemID = m_pConfig->m_iItemID;
        stData.m_iCurrentNum = stIter->second->GetAmount();
        stData.m_iDeltaNum = (-m_iAmount);
        stData.m_iCmd = GetCmd();
        stData.m_strInfo = GetInfo();
        LOGIC_PUBLISHER_INS->PublishFunctionalGameItemInfo(m_pUser, stData);

        // 缓存
        m_pUser->m_stCacheBonusData.m_stConsume.m_stItems.emplace_back(GetItemType(), m_pConfig->m_iItemID, m_iAmount);

        return (m_iAmount);
	}

	using namespace SERVER_ERRCODE;
	CLogicError::SetErrCode(NOT_ENOUGH_ITEM, GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_ITEM));

	return (-1);
}

bool CLogicItemGameItem::CanConsume() const
{
	auto stIter = m_pUser->m_stItemMap.Find(m_pConfig->m_iItemID);
	if (stIter != m_pUser->m_stItemMap.End())
	{
		if (stIter->second->GetAmount() >= m_iAmount)
		{
			return (true);
		}
	}

	using namespace SERVER_ERRCODE;
	CLogicError::SetErrCode(NOT_ENOUGH_ITEM, GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_ITEM));

	return (false);
}

int CLogicItemGameItem::Sell()
{
    if(m_pConfig->m_iSellPrice <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(GAME_ITEM_CAN_NOT_SELL);
        return -1;
    }

    if(CanConsume() && RealConsume() > 0)
    {
        CLogicCacheCommon::AddGameItem(m_pUser, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GoldID, m_iAmount * m_pConfig->m_iSellPrice, GetCmd());

        return m_iAmount;
    }

    return -1;
}

CLogicHeroCardGameItem::CLogicHeroCardGameItem(int iAmount, const TLogicHeroCardConfigElem* pConfig, user_data_table_ptr_type pUser)
    : m_iAmount(iAmount), m_pConfig(pConfig), m_pUser(pUser) {}

int CLogicHeroCardGameItem::RealAdd()
{
    auto stIter = m_pUser->m_stHeroCardTableMap.Find(m_pConfig->m_iHeroCardID);
    if (stIter == m_pUser->m_stHeroCardTableMap.End())
    {
        THeroCardInfoTableValueType stHeroCardVal;
        stHeroCardVal.m_iGroupID = m_pUser->m_iGroupID;
        stHeroCardVal.m_iUid = m_pUser->m_iUin;
        stHeroCardVal.m_iCardID = m_pConfig->m_iHeroCardID;
        stHeroCardVal.m_cCardColor = m_pConfig->m_iInitColor;
        stHeroCardVal.m_cCardStar = m_pConfig->m_iInitStar;
        stHeroCardVal.m_iCardLevel = 1;
        stHeroCardVal.m_iCardExp = 0;
        stHeroCardVal.m_iGradeLevel = m_pConfig->m_iInitGrade;
        stHeroCardVal.m_iCardTime = CLogicCommon::GetSec();
        auto stRet = m_pUser->m_stHeroCardTableMap.Insert(std::make_pair(m_pConfig->m_iHeroCardID, stHeroCardVal));
        if (!stRet.second) return (-1);

        --m_iAmount;

        CLogicGameItemAddChannel::data_type stData;
        stData.m_iItemType = GetItemType();
        stData.m_iCardID = m_pConfig->m_iHeroCardID;
        stData.m_iNum = 1;
        LOGIC_PUBLISHER_INS->PublishGameItemAddInfo(m_pUser, stData);

		CLogicFunctionalGameItemChannel::data_type stFunData;
		stFunData.m_iItemType = GetItemType();
		stFunData.m_iItemID = m_pConfig->m_iHeroCardID;
		stFunData.m_iCurrentNum = 1;
		stFunData.m_iDeltaNum = 1;
		stFunData.m_iCmd = GetCmd();
		stFunData.m_strInfo = GetInfo();
		LOGIC_PUBLISHER_INS->PublishFunctionalGameItemInfo(m_pUser, stFunData);

		CLogicDailyTaskSystem::SetProgress(m_pUser, CLogicDailyTaskSystem::HERO_STAR, stHeroCardVal.m_iCardID, stHeroCardVal.m_cCardStar);

        // 更新伙伴传记
        CLogicGlobal::UpdateHeroHistory(m_pUser, m_pConfig->m_iHeroCardID);

        CLogicDailyTaskSystem::SetProgress(m_pUser, CLogicDailyTaskSystem::HERO_HEART_LEVEL, m_pConfig->m_iHeroCardID, stHeroCardVal.m_iHeartLevel);

        // 好感度基金
        CLogicActiveBase::Notify(m_pUser, CLogicConfigDefine::ACTIVITY_TYPE_HEART_INSURE, 1);

        CLogicGlobal::PublishRewardCardBroadcast(m_pUser->m_strNick, m_pConfig->m_iHeroCardID, GetCmd());

        // 缓存
        m_pUser->m_stCacheBonusData.m_stBonus.m_stItems.emplace_back(GetItemType(), m_pConfig->m_iHeroCardID, 1);

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_CardGradeNum;
        stEventData.m_iPara = m_pConfig->m_iInitGrade;
        stEventData.m_iCmd = GetCmd();
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventData);

        // 通用事件
        CLogicEventData stEventHeroData;
        stEventHeroData.m_eType = CLogicConfigDefine::ECET_GetTargetHeroCard;
        stEventHeroData.m_iCmd = GetCmd();
        stEventHeroData.m_iPara = m_pConfig->m_iHeroCardID;
        stEventHeroData.m_iCount = m_iAmount;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventHeroData);
    }

    // 转换碎片
    if (m_iAmount > 0)
    {
        int iSoulNum = CLogicConfig::GetUpgradeHeroCardStarConfig().GetHeroRepeatExchange(m_pConfig->m_iInitGrade) * m_iAmount;
        CLogicCacheCommon::AddGameItem(m_pUser, CLogicConfigDefine::EXCHANGE_ITEM, m_pConfig->m_iSoulID, iSoulNum, GetCmd());
    }
    
    return (1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CLogicAvatarGameItem::CLogicAvatarGameItem(int iAmount, const TLogicAvatarElem* pConfig, user_data_table_ptr_type pUser)
    : m_iAmount(iAmount), m_pConfig(pConfig), m_pUser(pUser) {}

int 
CLogicAvatarGameItem::RealAdd()
{
    auto stIter = m_pUser->m_stUserAvatarMap.Find(m_pConfig->m_iAvatarID);
    if(stIter == m_pUser->m_stUserAvatarMap.End())
    {
        TUserAvatarTableValueType stValue;
        stValue.m_iGroupID = m_pUser->m_iGroupID;
        stValue.m_iUid = m_pUser->m_iUin;
        stValue.m_iAvatarID = m_pConfig->m_iAvatarID;
        stValue.m_iAvatarType = m_pConfig->m_iAvatarType;
        stValue.m_iTime = 0;
        m_pUser->m_stUserAvatarMap.Insert(std::make_pair(stValue.m_iAvatarID, stValue));

        // 缓存
        m_pUser->m_stCacheBonusData.m_stBonus.m_stAvatarVec.emplace_back(CLogicCacheCommon::GetAvatarInfo(stValue));

        return (1);
    }
    
    return (0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CLogicEquipGameItem::CLogicEquipGameItem(int iAmount, const TLogicEquipElem* pConfig, user_data_table_ptr_type pUser)
    : m_iAmount(iAmount), m_pConfig(pConfig), m_pUser(pUser) {}

int CLogicEquipGameItem::RealAdd()
{
    for(int i = 0; i < m_iAmount; ++i)
    {
        TUserEquipTableValueType stValue;
        stValue.m_iUid = m_pUser->m_iUin;
        stValue.m_iGroupID = m_pUser->m_iGroupID;
        stValue.m_iUniqueID = CLogicCacheCommon::GetUserPrimaryKeyIndex(m_pUser);
        stValue.m_iGrade = m_pConfig->m_iGrade;
        stValue.m_iEquipmentTag = m_pConfig->m_iEquipType;

        auto pAttrRandBag = CLogicConfig::GetEquipAttrRandConfig().GetConfig(m_pConfig->m_strAttrBag);
        if(pAttrRandBag == nullptr)
        {
            LOGIC_LOG_TRACE_ERROR << "Find Equip RandBag Error|UIN:" << m_pUser->m_iUin << "|GROUP_ID:" << m_pUser->m_iGroupID
                                  << "|CMD:" << GetCmd() << "|ITEM:" << m_pConfig->m_iItemID << std::endl;
            continue;
        }

        stValue.m_iAttrIndex = CLogicCacheCommon::RandStlCont(pAttrRandBag->m_stIndexRandMap, pAttrRandBag->m_iTotalWeight);
        if(stValue.m_iAttrIndex < 0)
        {
            LOGIC_LOG_TRACE_ERROR << "Rand Equip Attr Error|UIN:" << m_pUser->m_iUin << "|GROUP_ID:" << m_pUser->m_iGroupID
                                  << "|CMD:" << GetCmd() << "|ITEM:" << m_pConfig->m_iItemID << std::endl;
            continue;
        }

        auto iterResult = m_pUser->m_stEquipMap.Insert(std::make_pair(stValue.m_iUniqueID, stValue));
        if(!iterResult.second)
        {
            LOGIC_LOG_TRACE_ERROR << "Insert Equip Error|UIN:" << m_pUser->m_iUin << "|GROUP_ID:" << m_pUser->m_iGroupID
                                  << "|CMD:" << GetCmd() << "|ITEM:" << m_pConfig->m_iItemID << "|INDEX:" << stValue.m_iAttrIndex << std::endl;
            continue;
        }

        auto& iterData = iterResult.first;

        // 缓存
        m_pUser->m_stCacheBonusData.m_stBonus.m_stEquipVec.push_back(CLogicCacheCommon::GetEquipInfo(iterData->second->m_stData));
        // bill 记录
        CLogicBill::SaveEquipBill(m_pUser->m_iUin, m_pUser->m_iGroupID, m_pUser->m_iChannelID, GetCmd(), 1, iterData->second->m_stData);

        if(m_pConfig->m_iGrade >= CLogicConfigDefine::ECGT_SSR)
        {
            // 通用事件
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_EquipGradeNum;
            stEventData.m_iPara = m_pConfig->m_iGrade;
            stEventData.m_iCmd = GetCmd();
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventData);
        }
    }

    return m_iAmount;
}

CLogicConstellationGameItem::CLogicConstellationGameItem(int iAmount, const TLogicConstelElem* pConfig, user_data_table_ptr_type pUser)
    : m_iAmount(iAmount), m_pConfig(pConfig), m_pUser(pUser) {}

int CLogicConstellationGameItem::RealAdd()
{
    bool bMaxGrade = false;
    for(int i = 0; i < m_iAmount; ++i)
    {
        int32_t iEquipValue = RandomConstel(m_pConfig->m_iEquipmentSelectType, m_pConfig->m_stEquipmentValue,
                CLogicConfigDefine::EHET_Invalid + 1, CLogicConfigDefine::EHET_Max);

        auto iterSuit = CLogicConfig::GetConstelAttrConfig().m_stSuitMap.begin();
        std::advance(iterSuit, CLogicCommon::GetRandNum() % CLogicConfig::GetConstelAttrConfig().m_stSuitMap.size());
        int32_t iSuitValue = RandomConstel(m_pConfig->m_iSuitSelectType, m_pConfig->m_stSuitValue, iterSuit->first, iterSuit->first);
        if(iEquipValue == 0 || iSuitValue == 0)
        {
            LOGIC_LOG_TRACE_ERROR << "Add Constel RandomConstel Error|UIN:" << m_pUser->m_iUin << "|GROUP_ID:" << m_pUser->m_iGroupID << "|CMD:" << GetCmd()
                                  << "|ITEM:" << m_pConfig->m_iItemID << "|EQUIP_VALUE:" << iEquipValue << "|SUIT_VALUE:" << iSuitValue << std::endl;
            continue;
        }

        if(false == CLogicCacheCommon::AddConstelItem(m_pUser, m_pConfig->m_iGrade, iEquipValue, iSuitValue))
        {
            LOGIC_LOG_TRACE_ERROR << "Add Constel RandomConstel Error|UIN:" << m_pUser->m_iUin << "|GROUP_ID:" << m_pUser->m_iGroupID << "|CMD:" << GetCmd()
                                  << "|ITEM:" << m_pConfig->m_iItemID << "|EQUIP_VALUE:" << iEquipValue << "|SUIT_VALUE:" << iSuitValue << std::endl;
            continue;
        }

        if(m_pConfig->m_iGrade >= CLogicConfigDefine::ECGT_SSR)
        {
            // 通用事件
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_ConstelGradeNum;
            stEventData.m_iPara = m_pConfig->m_iGrade;
            stEventData.m_iCmd = GetCmd();
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventData);
        }

        if(m_pConfig->m_iGrade == CLogicConfigDefine::ECGT_UR)
        {
            bMaxGrade = true;
        }
    }

    // 6星礼装套装事件
    if(bMaxGrade)
    {
        CLogicEventData stEventSuit;
        stEventSuit.m_eType = CLogicConfigDefine::ECET_ConstelURSuitNum;
        stEventSuit.m_iCmd = GetCmd();
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUser, stEventSuit);
    }

    return m_iAmount;
}

int32_t CLogicConstellationGameItem::RandomConstel(int32_t iSelectType, const std::vector<int32_t>& stValues, const int32_t iRandomMin, const int32_t iRandomMax)
{
    int32_t iValue = 0;

    switch (iSelectType)
    {
    case CLogicConfigDefine::ECESRT_Given:
        if(!stValues.empty())
            iValue = stValues[0];

        break;
    case CLogicConfigDefine::ECESRT_Random:
        if(stValues.empty())
            iValue = CLogicCommon::GetRandNum(iRandomMin, iRandomMax);
        else
            iValue = stValues[CLogicCommon::GetRandNum() % stValues.size()];

        break;
    default:
        break;
    }

    return iValue;
}
