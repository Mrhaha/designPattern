#include "common_datetime.h"
#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "logic_link_define.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_game_active_info.h"
#include "logic_config.h"
#include "logic_game_active.h"
#include "logic_game_cache_common.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_global_frame.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_global.h"
#include "logic_game_common_publisher.h"

///////////////////////////////////////////////////////////////////////
///////////////// CRkLogicActiveBase
std::map<int, CLogicActiveBase::create_active_obj_func_type>
CLogicActiveBase::CREATE_ACTIVE_FUNC_MAP =
{
    { CLogicConfigDefine::ACTIVITY_TYPE_DINNER,                        CLogicActiveBase::CreateActiveFunc<CLogicGetEnergyActive>                  },
    { CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_LOGIN,                CLogicActiveBase::CreateActiveFunc<CLogicCumulateLoginActive>              },
    { CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_DEPOSIT,              CLogicActiveBase::CreateActiveFunc<CLogicCumulateDepositActive>            },
    { CLogicConfigDefine::ACTIVITY_TYPE_CORNUCOPIA,                    CLogicActiveBase::CreateActiveFunc<CLogicCornucopiaActive>                 },
    { CLogicConfigDefine::ACTIVITY_TYPE_FOUNDATION,                    CLogicActiveBase::CreateActiveFunc<CLogicFoundationActive>                 },
    { CLogicConfigDefine::ACTIVITY_TYPE_LEVEL_RACE,                    CLogicActiveBase::CreateActiveFunc<CLogicLevelRaceActive>                  },
    { CLogicConfigDefine::ACTIVITY_TYPE_HERO_FLASH_SALE,               CLogicActiveBase::CreateActiveFunc<CLogicHeroFlashSaleActive>              },
    { CLogicConfigDefine::ACTIVITY_TYPE_OPEN_SERVER_LOGIN,             CLogicActiveBase::CreateActiveFunc<CLogicOpenServerLoginActive>            },
    { CLogicConfigDefine::ACTIVITY_TYPE_OPEN_SERVER_BUY,               CLogicActiveBase::CreateActiveFunc<CLogicOpenServerBuyActive>              },
    { CLogicConfigDefine::ACTIVITY_TYPE_NORMAL_LEVEL,                  CLogicActiveBase::CreateActiveFunc<CLogicNormalLevelActive>                },
    { CLogicConfigDefine::ACTIVITY_TYPE_ELITE_LEVEL,                   CLogicActiveBase::CreateActiveFunc<CLogicEliteLevelActive>                 },
    { CLogicConfigDefine::ACTIVITY_TYPE_PVP_RANK,                      CLogicActiveBase::CreateActiveFunc<CLogicPVPRankActive>                    },
    { CLogicConfigDefine::ACTIVITY_TYPE_UNLIMIT_LEVEL_SCORE,           CLogicActiveBase::CreateActiveFunc<CLogicUnlimitLevelScoreActive>          },
    { CLogicConfigDefine::ACTIVITY_TYPE_PASS_LEVEL_STAR,               CLogicActiveBase::CreateActiveFunc<CLogicPassLevelStarActive>              },
    { CLogicConfigDefine::ACTIVITY_TYPE_PASS_LEVEL_TIMES,              CLogicActiveBase::CreateActiveFunc<CLogicPassLevelTimesActive>             },
    { CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_LEVEL,  CLogicActiveBase::CreateActiveFunc<CLogicHeroEquipmentUpgradeLevelActive>  },
    { CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_STAR,   CLogicActiveBase::CreateActiveFunc<CLogicHeroEquipmentUpgradeStarActive>   },
    { CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_COLOR,            CLogicActiveBase::CreateActiveFunc<CLogicHeroUpgradeColorActive>           },
    { CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_LEVEL,            CLogicActiveBase::CreateActiveFunc<CLogicHeroUpgradeLevelActive>           },
    { CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_STAR,             CLogicActiveBase::CreateActiveFunc<CLogicHeroUpgradeStarActive>            },
    { CLogicConfigDefine::ACTIVITY_TYPE_TEAM_LEVEL_UPGRADE,            CLogicActiveBase::CreateActiveFunc<CLogicTeamUpgradeLevelActive>           },
    { CLogicConfigDefine::ACTIVITY_TYPE_UPGRADE_SKILL,                 CLogicActiveBase::CreateActiveFunc<CLogicUpgradeSkillActive>               },
    { CLogicConfigDefine::ACTIVITY_TYPE_FIGHT_POWER,                   CLogicActiveBase::CreateActiveFunc<CLogicFightPowerActive>                 },
    { CLogicConfigDefine::ACTIVITY_TYPE_HERO_ACHIEVE,                  CLogicActiveBase::CreateActiveFunc<CLogicHeroAchieveActive>                },
    { CLogicConfigDefine::ACTIVITY_TYPE_BENEFIT_STORE,                 CLogicActiveBase::CreateActiveFunc<CLogicBenefitStoreActive>               },
    { CLogicConfigDefine::ACTIVITY_TYPE_EXCHANGE_STORE,                CLogicActiveBase::CreateActiveFunc<CLogicExchangeStoreActive>              },
    { CLogicConfigDefine::ACTIVITY_TYPE_SINGLE_DEPOSIT,                CLogicActiveBase::CreateActiveFunc<CLogicSingleDepositActive>              },
    { CLogicConfigDefine::ACTIVITY_TYPE_DEADLINE_LOGIN,                CLogicActiveBase::CreateActiveFunc<CLogicDeadlineLoginActive>              },
    { CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_JEWEL,  CLogicActiveBase::CreateActiveFunc<CLogicHeroEquipmentUpgradeJewelActive>  },
    { CLogicConfigDefine::ACTIVITY_TYPE_BUY_COMMODITY,                 CLogicActiveBase::CreateActiveFunc<CLogicBuyCommodityActive>               },
    { CLogicConfigDefine::ACTIVITY_TYPE_DAILY_CUMULATE_DEPOSIT,        CLogicActiveBase::CreateActiveFunc<CLogicDailyCumulateDepositActive>       },
    { CLogicConfigDefine::ACTIVITY_TYPE_FIGHT_THEME_ACTIVE_LEVEL,      CLogicActiveBase::CreateActiveFunc<CLogicFightThemeActiveLevelActive>      },
    { CLogicConfigDefine::ACTIVITY_TYPE_PVP_FIGHT_TIMES,               CLogicActiveBase::CreateActiveFunc<CLogicPVPFightTimesActive>              },
    { CLogicConfigDefine::ACTIVITY_TYPE_ELITE_LEVEL_FIGHT_TIMES,       CLogicActiveBase::CreateActiveFunc<CLogicEliteLevelFightTimesActive>       },
    { CLogicConfigDefine::ACTIVITY_TYPE_NORMAL_LEVEL_FIGHT_TIMES,      CLogicActiveBase::CreateActiveFunc<CLogicNormalLevelFightTimesActive>      },
    { CLogicConfigDefine::ACTIVITY_TYPE_BUY_ENERGY_TIMES,              CLogicActiveBase::CreateActiveFunc<CLogicBuyEnergyTimesActive>             },
    { CLogicConfigDefine::ACTIVITY_TYPE_BUY_GOLD_TIMES,                CLogicActiveBase::CreateActiveFunc<CLogicBuyGoldTimesActive>               },
    { CLogicConfigDefine::ACTIVITY_TYPE_BUY_BLACK_STORE_TIMES,         CLogicActiveBase::CreateActiveFunc<CLogicBuyBlackStoreTimesActive>         },
    { CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_CONSUME_YUANBAO,      CLogicActiveBase::CreateActiveFunc<CLogicCumulateConsumeYuanBaoActive>     },
	{ CLogicConfigDefine::ACTIVITY_TYPE_DAILY_CONSUME_YUANBAO,		  CLogicActiveBase::CreateActiveFunc<CLogicDailyConsumeYuanBaoActive>        },
    { CLogicConfigDefine::ACTIVITY_TYPE_CROSS_FIGHT_PASS_LEVEL_TIMES,  CLogicActiveBase::CreateActiveFunc<CLogicCrossFightPassLevelTimesActive>   },
    { CLogicConfigDefine::ACTIVITY_TYPE_UNLIMIT_LEVEL_FIGHT_TIMES,     CLogicActiveBase::CreateActiveFunc<CLogicUnlimitLevelFightTimesActive>     },
	{ CLogicConfigDefine::ACTIVITY_TYPE_SINGLE_ITEM_EXCHANGE,		  CLogicActiveBase::CreateActiveFunc<CLogicSingleItemExchangeActive>         },
    { CLogicConfigDefine::ACTIVITY_TYPE_UPGRADE_TACTICS,				  CLogicActiveBase::CreateActiveFunc<CLogicUpgradeTacticsActive>             },
	{ CLogicConfigDefine::ACTIVITY_TYPE_HERO_TRAIN,                    CLogicActiveBase::CreateActiveFunc<CLogicHeroTrainActive>                  },
	{ CLogicConfigDefine::ACTIVITY_TYPE_BREAK_EGG,                     CLogicActiveBase::CreateActiveFunc<CLogicBreakEggActive>                   },
	{ CLogicConfigDefine::ACTIVITY_TYPE_YUANBAO_RECRUIT,               CLogicActiveBase::CreateActiveFunc<CLogicYuanbaoRecruitActive>             },
	{ CLogicConfigDefine::ACTIVITY_TYPE_CONSUME_ENERGY,                CLogicActiveBase::CreateActiveFunc<CLogicConsumeEnergyActive>              },
	{ CLogicConfigDefine::ACTIVITY_TYPE_CLIMB_TOWER_ORDER,             CLogicActiveBase::CreateActiveFunc<CLogicClimbTowerActive>                 },
	{ CLogicConfigDefine::ACTIVITY_TYPE_PVP_TERRITORY_ORDER,           CLogicActiveBase::CreateActiveFunc<CLogicPvpTerritoryActive>               },
	{ CLogicConfigDefine::ACTIVITY_TYPE_TACTICS_COUNT,				  CLogicActiveBase::CreateActiveFunc<CLogicTacticsCountActive>               },
	{ CLogicConfigDefine::ACTIVITY_TYPE_GUILD_DONATE,                  CLogicActiveBase::CreateActiveFunc<CLogicGuildDonateActive>                },
	{ CLogicConfigDefine::ACTIVITY_TYPE_GUILD_TASK,                    CLogicActiveBase::CreateActiveFunc<CLogicGuildTaskActive>                  },
	{ CLogicConfigDefine::ACTIVITY_TYPE_LUCKY_BOX,                     CLogicActiveBase::CreateActiveFunc<CLogicLuckyBoxActive>                   },
	{ CLogicConfigDefine::ACTIVITY_TYPE_TALENT_SCORE,                  CLogicActiveBase::CreateActiveFunc<CLogicTalentScoreActive>                },
	{ CLogicConfigDefine::ACTIVITY_TYPE_TOTAL_HERO_TRAIN,              CLogicActiveBase::CreateActiveFunc<CLogicTotalHeroTrainActive>             },
	{ CLogicConfigDefine::ACTIVITY_TYPE_TURN_TABLE,                    CLogicActiveBase::CreateActiveFunc<CLogicTurnTableScoreActive>             },
	{ CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_INSURE,                  CLogicActiveBase::CreateActiveFunc<CLogicLoginInsureActive>                },
	{ CLogicConfigDefine::ACTIVITY_TYPE_HERO_INSURE,                   CLogicActiveBase::CreateActiveFunc<CLogicHeroInsureActive>                 },
	{ CLogicConfigDefine::ACTIVITY_TYPE_HOME_INSURE,                   CLogicActiveBase::CreateActiveFunc<CLogicHomeInsureActive>                 },
	{ CLogicConfigDefine::ACTIVITY_TYPE_WISH_CHARACTER,                CLogicActiveBase::CreateActiveFunc<CLogicWishCharacterActive>				 },
	{ CLogicConfigDefine::ACTIVITY_TYPE_COUNTDOWN_HERO,				  CLogicActiveBase::CreateActiveFunc<CLogicCountDownHeroActive> },
	{ CLogicConfigDefine::ACTIVITY_TYPE_FLOWER_LANTERN,				  CLogicActiveBase::CreateActiveFunc<CLogicFlowerLanternActive> },
	{ CLogicConfigDefine::ACTIVITY_TYPE_PET_INSURE,                    CLogicActiveBase::CreateActiveFunc<CLogicPetInsureActive> },
	{ CLogicConfigDefine::ACTIVITY_TYPE_SEND_FLOWER,                   CLogicActiveBase::CreateActiveFunc<CLogicSendFlowerActive> },
	{ CLogicConfigDefine::ACTIVITY_TYPE_LUCKY_HAMMER,                  CLogicActiveBase::CreateActiveFunc<CLogicLuckyHammerActive> },
    { CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_1,                  CLogicActiveBase::CreateActiveFunc<CLogicLoginFundActive> },
    { CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_2,                  CLogicActiveBase::CreateActiveFunc<CLogicLoginFundActive> },
    { CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_3,                  CLogicActiveBase::CreateActiveFunc<CLogicLoginFundActive> },
    { CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_4,                  CLogicActiveBase::CreateActiveFunc<CLogicLoginFundActive> },
    { CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND_5,                  CLogicActiveBase::CreateActiveFunc<CLogicLoginFundActive> },
    { CLogicConfigDefine::ACTIVITY_TYPE_HEART_INSURE,                  CLogicActiveBase::CreateActiveFunc<CLogicHeartInsureActive> },
};

///////////////////////////////////////////////////////////////////////
///////////////// Notify Register 
std::map<int, CLogicActiveBase::active_notify_func_type>
CLogicActiveBase::ACTIVE_NOTIFY_FUNC_MAP = {};

void
CLogicActiveBase::RegisterActiveNotify(int iType, active_notify_func_type pFunc)
{
    ACTIVE_NOTIFY_FUNC_MAP.insert(std::make_pair(iType, pFunc));
}



#define _REGISTER_ACTIVE_NOTIFY_WITH_LINE(ACTIVE_ID, ITEM_CLASS_NAME, LINE_NUM) \
struct _static_ACTIVE_##ITEM_CLASS_NAME##_TEMP_##LINE_NUM \
{\
    _static_ACTIVE_##ITEM_CLASS_NAME##_TEMP_##LINE_NUM() \
    {\
        CLogicActiveBase::RegisterActiveNotify(ACTIVE_ID, ITEM_CLASS_NAME::Notify); \
    }\
}; \
static _static_ACTIVE_##ITEM_CLASS_NAME##_TEMP_##LINE_NUM g_stInstance##ITEM_CLASS_NAME##_TEMP_##LINE_NUM;

#define REGISTER_ACTIVE_NOTIFY_WITH_LINE(ACTIVE_ID, ITEM_CLASS_NAME, LINE_NUM) _REGISTER_ACTIVE_NOTIFY_WITH_LINE(ACTIVE_ID, ITEM_CLASS_NAME, LINE_NUM)

#define REGISTER_ACTIVE_NOTIFY(ACTIVE_ID, ITEM_CLASS_NAME) REGISTER_ACTIVE_NOTIFY_WITH_LINE(ACTIVE_ID, ITEM_CLASS_NAME, __LINE__)

REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_DEPOSIT, CLogicCumulateDepositActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_FOUNDATION, CLogicFoundationActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_LEVEL_RACE, CLogicLevelRaceActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HERO_FLASH_SALE, CLogicHeroFlashSaleActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_NORMAL_LEVEL, CLogicNormalLevelActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_ELITE_LEVEL, CLogicEliteLevelActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_PVP_RANK, CLogicPVPRankActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_UNLIMIT_LEVEL_SCORE,CLogicUnlimitLevelScoreActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_PASS_LEVEL_STAR, CLogicPassLevelStarActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_PASS_LEVEL_TIMES, CLogicPassLevelTimesActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_COLOR, CLogicHeroUpgradeColorActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_LEVEL, CLogicHeroUpgradeLevelActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_STAR, CLogicHeroUpgradeStarActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_TEAM_LEVEL_UPGRADE, CLogicTeamUpgradeLevelActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_UPGRADE_SKILL, CLogicUpgradeSkillActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_FIGHT_POWER, CLogicFightPowerActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HERO_ACHIEVE, CLogicHeroAchieveActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_SINGLE_DEPOSIT, CLogicSingleDepositActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_DEADLINE_LOGIN, CLogicDeadlineLoginActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_BUY_COMMODITY, CLogicBuyCommodityActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_PVP_FIGHT_TIMES, CLogicPVPFightTimesActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_BUY_ENERGY_TIMES, CLogicBuyEnergyTimesActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_BUY_GOLD_TIMES, CLogicBuyGoldTimesActive)

REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_FIGHT_THEME_ACTIVE_LEVEL, CLogicFightThemeActiveLevelActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_ELITE_LEVEL_FIGHT_TIMES, CLogicEliteLevelFightTimesActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_NORMAL_LEVEL_FIGHT_TIMES, CLogicNormalLevelFightTimesActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_BUY_BLACK_STORE_TIMES, CLogicBuyBlackStoreTimesActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_CONSUME_YUANBAO, CLogicCumulateConsumeYuanBaoActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_DAILY_CONSUME_YUANBAO, CLogicDailyConsumeYuanBaoActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_CROSS_FIGHT_PASS_LEVEL_TIMES, CLogicCrossFightPassLevelTimesActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_UNLIMIT_LEVEL_FIGHT_TIMES, CLogicUnlimitLevelFightTimesActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_DAILY_CUMULATE_DEPOSIT, CLogicDailyCumulateDepositActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_LEVEL, CLogicHeroEquipmentUpgradeLevelActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_STAR, CLogicHeroEquipmentUpgradeStarActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_JEWEL, CLogicHeroEquipmentUpgradeJewelActive)

REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_UPGRADE_TACTICS, CLogicUpgradeTacticsActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HERO_TRAIN, CLogicHeroTrainActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_BREAK_EGG, CLogicBreakEggActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_YUANBAO_RECRUIT, CLogicYuanbaoRecruitActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_CONSUME_ENERGY, CLogicConsumeEnergyActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_CLIMB_TOWER_ORDER, CLogicClimbTowerActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_PVP_TERRITORY_ORDER, CLogicPvpTerritoryActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_TACTICS_COUNT, CLogicTacticsCountActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_GUILD_DONATE, CLogicGuildDonateActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_GUILD_TASK, CLogicGuildTaskActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_LUCKY_BOX, CLogicLuckyBoxActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_TALENT_SCORE, CLogicTalentScoreActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_TOTAL_HERO_TRAIN, CLogicTotalHeroTrainActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_TURN_TABLE, CLogicTurnTableScoreActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_INSURE, CLogicLoginInsureActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HERO_INSURE, CLogicHeroInsureActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HOME_INSURE, CLogicHomeInsureActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_WISH_CHARACTER, CLogicWishCharacterActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_COUNTDOWN_HERO, CLogicCountDownHeroActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_FLOWER_LANTERN, CLogicFlowerLanternActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_PET_INSURE, CLogicPetInsureActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_SEND_FLOWER, CLogicSendFlowerActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_LUCKY_HAMMER, CLogicLuckyHammerActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND, CLogicLoginFundActive)
REGISTER_ACTIVE_NOTIFY(CLogicConfigDefine::ACTIVITY_TYPE_HEART_INSURE, CLogicHeartInsureActive)

///////////////////////////////////////////////////////////////////////
///////////////// CLogicActivityChangeList
CLogicActivityChangeList*
CLogicActivityChangeList::s_pInstance = nullptr;

CLogicActivityChangeList*
CLogicActivityChangeList::GetInstance()
{
    if (s_pInstance == nullptr)
        s_pInstance = new CLogicActivityChangeList();

    return s_pInstance;
}

void
CLogicActivityChangeList::NotifyClientUser(int iGroupID, int iUin)
{
    if (!m_stActivityChangeMap.empty())
    {
        CPackActiveStatus stElem;
        CNotifyActiveStatus stRsp;

        for (const auto& stActive : m_stActivityChangeMap)
        {
            stElem.m_iActivityID = stActive.first; 
            stElem.m_iRedPoint = stActive.second; 

            stRsp.m_stActiveStatus.push_back(stElem);
        }

        LOGIC_SERVICE_API_INS->GetConnectorSender()->NotifyClientUser(iGroupID, iUin, MSG_LOGIC_NOTIFY_ACTIVITY_CHANGE, stRsp);

        m_stActivityChangeMap.clear();
    }
}

template<typename T> CLogicActiveBase::active_obj_ptr
CLogicActiveBase::CreateActiveFunc(user_data_table_ptr_type pUserData, int iActivityID)
{
    return (active_obj_ptr(new T(pUserData, iActivityID)));
}

void
CLogicActiveBase::Notify(user_data_table_ptr_type pUserData, int iType, int iNum)
{
    auto stIter = ACTIVE_NOTIFY_FUNC_MAP.find(iType);
    if (stIter != ACTIVE_NOTIFY_FUNC_MAP.end())
    {
        (stIter->second)(pUserData, iNum);
    }
}

void
CLogicActiveBase::SubscribeGameDepositInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_deposit_channel_data_t& stData)
{
    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_SINGLE_DEPOSIT, stData.m_iNum);

    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_DEPOSIT, stData.m_iNum);

    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_DAILY_CUMULATE_DEPOSIT, stData.m_iNum);
}

void
CLogicActiveBase::SubscribeGameLevelInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_level_channel_data_t& stData)
{
    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_FLASH_SALE, stData.m_iLevelID);

    int iChapterType = CLogicConfig::GetChapterTypeByLevelID(stData.m_iLevelID);
    if (iChapterType == CLogicConfigDefine::NORMAL_CHAPTER)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_NORMAL_LEVEL, stData.m_iLevelID);
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_PASS_LEVEL_TIMES, stData.m_iTimes);
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_NORMAL_LEVEL_FIGHT_TIMES, stData.m_iTimes);
    }
    else if (iChapterType == CLogicConfigDefine::ELITE_CHAPTER)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_ELITE_LEVEL, stData.m_iLevelID);

        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_PASS_LEVEL_TIMES, stData.m_iTimes);
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_ELITE_LEVEL_FIGHT_TIMES, stData.m_iTimes);
    }
}

void
CLogicActiveBase::SubscribeGamePVPRankCallBack(user_data_table_ptr_type pUserData, const logic_pvp_rank_channel_data_t& stData)
{
    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_PVP_RANK, stData.m_iCurrRank);
}

void
CLogicActiveBase::SubscribeGameUnlimitLevelScoreCallBack(user_data_table_ptr_type pUserData, const logic_game_unlimit_level_score_channel_data_t& stData)
{
    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_UNLIMIT_LEVEL_SCORE, stData.m_iAddScore);
}

void
CLogicActiveBase::SubscribeGameAddLevelStarCallBack(user_data_table_ptr_type pUserData, const logic_game_add_level_star_channel_data_t& stData)
{
    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_PASS_LEVEL_STAR, stData.m_iAddStar);
}

void
CLogicActiveBase::SubscribeGameHeroEquipmentCallBack(user_data_table_ptr_type pUserData, const logic_game_hero_equipment_channel_data_t& stData)
{
    if (CLogicGameChannelEnum::ACTION_UPGRADE_EQUIPMENT_LEVEL == stData.m_enmActionType)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_LEVEL,1);
    }
    else if (CLogicGameChannelEnum::ACTION_UPGRADE_EQUIPMENT_STAR == stData.m_enmActionType)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_STAR,1);
    }
    else if (CLogicGameChannelEnum::ACTION_UPGRADE_EQUIPMENT_JEWEL == stData.m_enmActionType)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_EQUIPMENT_UPGRADE_JEWEL,1);
    }
}

void
CLogicActiveBase::SubscribeGameCardInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_card_channel_data_t& stData)
{
    if (CLogicGameChannelEnum::UPGRADE_CARD_COLOR_OPTION == stData.m_iCardOption)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_COLOR, 0);

		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_TALENT_SCORE, 0);
    }
    else if (CLogicGameChannelEnum::UPGRADE_CARD_LEVEL_OPTION == stData.m_iCardOption)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_LEVEL, 0);
    }
    else if (CLogicGameChannelEnum::UPGRADE_CARD_STAR_OPTION == stData.m_iCardOption)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_STAR, 0);

		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_TALENT_SCORE, 0);
    }
    else if (CLogicGameChannelEnum::UPGRADE_CARD_SKILL_LEVEL_OPTION == stData.m_iCardOption)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_UPGRADE_SKILL, 1);
    }
}

void
CLogicActiveBase::SubscribeGameItemAddInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_item_add_channel_data_t& stData)
{
    if (stData.m_iItemType == CLogicConfigDefine::HERO_CARD)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_UPGRADE_STAR, 0);

        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_ACHIEVE, stData.m_iCardID);

		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_INSURE, 0);
    }
}

void
CLogicActiveBase::SubscribeGameUserActionInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_user_action_channel_data_t& stData)
{
    if (CLogicGameChannelEnum::ACTION_UPGRADE_WUSHEN_LEVEL == stData.m_iAction)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_WUSHEN_UPGRADE_LEVEL, 0);
    }
    else if (CLogicGameChannelEnum::ACTION_PLAY_PVP == stData.m_iAction)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_PVP_FIGHT_TIMES, 1);
    }
    else if (CLogicGameChannelEnum::ACTION_BUY_ENERGY == stData.m_iAction)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_BUY_ENERGY_TIMES, 1);
    }
	else if (CLogicGameChannelEnum::ACTION_BENEFIT_CARD_ACTIVE == stData.m_iAction)
	{
		if (CLogicConfigDefine::LOGIN_INSURE_CARD_ID == stData.m_iData)
		{
			CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_INSURE, 1);
		}
		else if (CLogicConfigDefine::HERO_INSURE_CARD_ID == stData.m_iData)
		{
			CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_INSURE, 1);
		}
		else if (CLogicConfigDefine::HOME_INSURE_CARD_ID == stData.m_iData)
		{
			CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HOME_INSURE, 1);
		}
		else if (CLogicConfigDefine::PET_INSURE_CARD_ID == stData.m_iData)
		{
			CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_PET_INSURE, 1);
		}
		else if(CLogicConfigDefine::GROW_FOUNDATION_CARD_ID == stData.m_iData)
        {
            CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_FOUNDATION, 1);
        }
		else if(CLogicConfigDefine::HEART_INSURE_CARD_ID == stData.m_iData)
        {
            CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HEART_INSURE, 1);
        }
        else if (CLogicConfigDefine::LOGIN_FUND_1_CARD_ID == stData.m_iData
            || CLogicConfigDefine::LOGIN_FUND_2_CARD_ID == stData.m_iData
            || CLogicConfigDefine::LOGIN_FUND_3_CARD_ID == stData.m_iData
            || CLogicConfigDefine::LOGIN_FUND_4_CARD_ID == stData.m_iData
            || CLogicConfigDefine::LOGIN_FUND_5_CARD_ID == stData.m_iData)
        {
            CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_LOGIN_FUND, 1);
        }
	}
    else if (CLogicGameChannelEnum::ACTION_BUY_BLACK_MARKET == stData.m_iAction)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_BUY_BLACK_STORE_TIMES, 1);
    }
    else if (CLogicGameChannelEnum::ACTION_USE_SHAKE_GOLD == stData.m_iAction)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_BUY_GOLD_TIMES,stData.m_iTimes);
    }
    else if (CLogicGameChannelEnum::ACTION_FIGHT_THEME_ACTIVE_LEVEL == stData.m_iAction)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_FIGHT_THEME_ACTIVE_LEVEL, stData.m_iTimes);

        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_THEME_ACTIVE_LEVEL_PROGRESS, stData.m_iData);
    }
    else if (CLogicGameChannelEnum::ACTION_CROSS_FIGHT == stData.m_iAction)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_CROSS_FIGHT_PASS_LEVEL_TIMES,stData.m_iTimes);
    }
    else if (CLogicGameChannelEnum::ACTION_PLAY_UNLIMIT_LEVEL == stData.m_iAction)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_UNLIMIT_LEVEL_FIGHT_TIMES,stData.m_iTimes);
    }
	else if (CLogicGameChannelEnum::ACTION_HERO_TRAIN == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_HERO_TRAIN, stData.m_iTimes);

		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_TOTAL_HERO_TRAIN, 0);
	}
	else if (CLogicGameChannelEnum::ACTION_UPGRADE_TACTICS == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_UPGRADE_TACTICS, stData.m_iTimes);

		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_TACTICS_COUNT, 0);
	}
	else if (CLogicGameChannelEnum::ACTION_UNLOCK_TACTICS == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_TACTICS_COUNT, 0);
	}
	else if (CLogicGameChannelEnum::ACTION_COMPLETE_GUILD_TASK == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_GUILD_TASK, stData.m_iTimes);
	}
	else if (CLogicGameChannelEnum::ACTION_GUILD_DONATE == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_GUILD_DONATE, stData.m_iTimes);
	}
	else if (CLogicGameChannelEnum::ACTION_LUCKY_BOX == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_LUCKY_BOX, stData.m_iTimes);
	}
	else if (CLogicGameChannelEnum::ACTION_TURN_TABLE == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_TURN_TABLE, stData.m_iTimes);
	}
	else if (CLogicGameChannelEnum::ACTION_WISH_CHARACTER == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_WISH_CHARACTER, stData.m_iTimes);
	}
	else if (CLogicGameChannelEnum::ACTION_BREAK_EGG == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_BREAK_EGG, stData.m_iTimes);
	}
	else if (CLogicGameChannelEnum::ACTION_COUNTDOWN_HERO == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_COUNTDOWN_HERO, stData.m_iTimes);
	}
	else if (CLogicGameChannelEnum::ACTION_SEND_FLOWER == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_SEND_FLOWER, stData.m_iTimes);
	}
	else if (CLogicGameChannelEnum::ACTION_LUCKY_HAMMER == stData.m_iAction)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_LUCKY_HAMMER, stData.m_iTimes);
	}
}

void
CLogicActiveBase::SubscribeGameMallChannelInfoCallBack(user_data_table_ptr_type pUserData, const logic_game_mall_channel_data_t& stData)
{
    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_BUY_COMMODITY, stData.m_iCommodityNum);
    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_YUANBAO_RECRUIT, stData.m_iCommodityNum);
}

void
CLogicActiveBase::SubscribeFunctionalGameItemInfoCallBack(user_data_table_ptr_type pUserData, const logic_functional_game_item_channel_data_t& stData)
{
    if (CLogicConfigDefine::CURRENCY == stData.m_iItemType && CLogicConfigDefine::ECIT_YuanBaoID == stData.m_iItemID && stData.m_iDeltaNum < 0)
    {
        CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_CUMULATE_CONSUME_YUANBAO, -stData.m_iDeltaNum);
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_DAILY_CONSUME_YUANBAO, -stData.m_iDeltaNum);
    }
}

void CLogicActiveBase::SubscribeGameEnergyCallBack(user_data_table_ptr_type pUserData, const CLogicGameEnergyChannel::data_type& stData)
{
	if (stData.m_iNum < 0)
	{
		CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_CONSUME_ENERGY, -stData.m_iNum);
	}
}

void CLogicActiveBase::SubscribeClimbTowerOrderCallBack(user_data_table_ptr_type pUserData, const CLogicClimbTowerOrderChannel::data_type& stData)
{
	CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_CLIMB_TOWER_ORDER, stData.m_iOrder);
}

void CLogicActiveBase::SubscribePvpTerritoryOrderCallBack(user_data_table_ptr_type pUserData, const CLogicPvpTerritoryOrderChannel::data_type& stData)
{
	CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_PVP_TERRITORY_ORDER, stData.m_iOrder);
}

void
CLogicActiveBase::SubscribeGameUserLevelUpInfoCallBack(user_data_table_ptr_type pUserData)
{
    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_FOUNDATION, 0);

    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_LEVEL_RACE, 0);

    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_TEAM_LEVEL_UPGRADE, 0);
}

void 
CLogicActiveBase::SubscribeGameCombatInfoCallBack(user_data_table_ptr_type pUserData)
{
    CLogicActiveBase::Notify(pUserData, CLogicConfigDefine::ACTIVITY_TYPE_FIGHT_POWER, pUserData->m_stUserInfoTable.GetRoleCombat());
}

bool CLogicActiveBase::IsActiveOpen(user_data_table_ptr_type pUserData, int iActivityID)
{
	auto stIter = CLogicConfig::GetActivityConfig().m_stActivityRef.find(iActivityID);
	if (stIter == CLogicConfig::GetActivityConfig().m_stActivityRef.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return false;
	}

	if (nullptr != pUserData && pUserData->m_stUserInfoTable.GetLevel() < stIter->second.m_iLevel)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(NOT_ENOUGH_ROLE_LEVEL);
		return false;
	}

	if (!CLogicActive::IsWithinActivityTime(pUserData, stIter->second.m_iType, CLogicCommon::GetSec()
		, stIter->second.m_iTimeType, stIter->second.m_iStartDay, stIter->second.m_iEndDay
		, stIter->second.m_iServerDay, stIter->second.m_iStartTime, stIter->second.m_iEndTime
		, stIter->second.m_iCustomCreateTime, stIter->second.m_iCreateDay))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_OPEN);
		return false;
	}

	using namespace SERVER_ERRCODE;
	return true;
}

CLogicActiveBase::active_obj_ptr
CLogicActiveBase::CreateActiveObj(user_data_table_ptr_type pUserData, int iActivityID)
{
    auto stIter = CLogicConfig::GetActivityConfig().m_stActivityRef.find(iActivityID);
    if(stIter == CLogicConfig::GetActivityConfig().m_stActivityRef.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (active_obj_ptr(nullptr));
    }

	if (!IsActiveOpen(pUserData, iActivityID))
	{
		using namespace SERVER_ERRCODE;
		return (active_obj_ptr(nullptr));
	}

    auto stFuncIter = CREATE_ACTIVE_FUNC_MAP.find(stIter->second.m_iType);
    if (stFuncIter == CREATE_ACTIVE_FUNC_MAP.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_TYPE_IS_NOT_IN_CONFIG);
        return (active_obj_ptr(nullptr));
    }

    pUserData->m_iCommand = pUserData->m_iCommand + iActivityID * 100000;
    
    return ((stFuncIter->second)(pUserData, iActivityID));
}

void
CLogicActiveBase::GetAllActiveInfo(user_data_table_ptr_type pUserData, std::vector<CPackActiveInfo>& stActiveInfo)
{
    int iActiveID = 0;
    CPackActiveInfo stActive;
    CLogicActiveBase::active_info_t stInfo;
    for (const auto& stActiveConf : CLogicConfig::GetActivityConfig().m_stActivityRef)
    {
        iActiveID = stActiveConf.first;
        if (!IsActiveOpen(pUserData, iActiveID))
        {
            continue;
        }

        auto stFuncIter = CREATE_ACTIVE_FUNC_MAP.find(stActiveConf.second.m_iType);
        if (stFuncIter != CREATE_ACTIVE_FUNC_MAP.end())
        {
            auto pActive = (stFuncIter->second)(pUserData, iActiveID);
            if (pActive.get() != nullptr)
            {
                stInfo = pActive->GetActiveInfo();

                stActive.m_iActiveID = iActiveID;
                stActive.m_iTargetNum = stInfo.m_iTargetNum;
                stActive.m_iLastReceiveAwardTime = stInfo.m_iLastReceiveAwardTime;
                stActive.m_strReceivedBitmap=int2bin<uint64_t>(stInfo.m_ulReceivedBitmap);
                stActive.m_strCompleteBitmap=int2bin<uint64_t>(stInfo.m_ulCompleteBitmap);
				stActive.m_stParamMap = pActive->GetParamMap();
				stActive.m_strParaBitmap = int2bin<uint64_t>(stInfo.m_ulParaEx);
                stActiveInfo.push_back(stActive);

                continue;
            }
        }
    }
}

CLogicActiveBase::active_info_t
CLogicActiveBase::GetActiveInfo()
{
    CLogicActiveBase::active_info_t stActiveInfo;

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter != m_pUserData->m_stUserActiveMap.End())
    {
        stActiveInfo.m_iTargetNum = stIter->second->GetActiveTargetNum(); 
        stActiveInfo.m_ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
        stActiveInfo.m_ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
        stActiveInfo.m_ulParaEx = stIter->second->GetParaEx();
        stActiveInfo.m_iLastReceiveAwardTime = stIter->second->GetActiveTime();
    }
    
    return (stActiveInfo);
}

///////////////////////////////////////////////////////////////////////////////////////////
CLogicGetEnergyActive::CLogicGetEnergyActive(user_data_table_ptr_type pUserData, int iActivityID)
: CLogicActiveBase(pUserData, iActivityID) {}

CLogicActiveBase::active_info_t
CLogicGetEnergyActive::GetActiveInfo()
{
    CLogicActiveBase::active_info_t stActiveInfo;

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter != m_pUserData->m_stUserActiveMap.End())
    {
        if(CLogicCommon::IsCurrentDay(stIter->second->GetActiveTime()) == false)
        {
            stIter->second->SetActiveReceivedBitmap(0);
        }

        stActiveInfo.m_iTargetNum = stIter->second->GetActiveTargetNum(); 
        stActiveInfo.m_ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
        stActiveInfo.m_ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
        stActiveInfo.m_iLastReceiveAwardTime = stIter->second->GetActiveTime();
    }
    
    return (stActiveInfo);
}

bool 
CLogicGetEnergyActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    (void)iEntityID;

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stDinnerConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stDinnerConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    if(CLogicCommon::IsCurrentDay(stIter->second->GetActiveTime()) == false)
    {
        stIter->second->SetActiveReceivedBitmap(0);
    }

    uint64_t ulBitmapMask = 0x1, ulActiveBitmap = stIter->second->GetActiveReceivedBitmap();
    int iHour = CLogicCommon::GetDate().tm_hour;

    for(auto& stVal : stConfigIter->second)
    {
        if((iHour >= stVal.m_iStartTime) && (iHour < stVal.m_iEndTime))
        {
            if((ulBitmapMask & ulActiveBitmap) == 0)
            {
              ulActiveBitmap = ulBitmapMask | ulActiveBitmap;
              stIter->second->SetActiveReceivedBitmap(ulActiveBitmap);
              stIter->second->SetActiveTime(CLogicCommon::GetSec());

              CPackGameItem stItem;
              stItem.m_iItemType = CLogicConfigDefine::CURRENCY;
              stItem.m_iCardID = CLogicConfigDefine::ECIT_EnergyID;
              stItem.m_iNum = stVal.m_iEnergyAmount;

              stBonusList.push_back(stItem);

              return (true);
            }
            else
            {
                using namespace SERVER_ERRCODE;
                SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
                return (false);
            }
        }

        ulBitmapMask = ulBitmapMask << 1;
    }

    using namespace SERVER_ERRCODE;
    SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
    return (false);
}



///////////////////////////////////////////////////////////////////////////////////////////
CLogicBenefitStoreActive::CLogicBenefitStoreActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

CLogicActiveBase::active_info_t
CLogicBenefitStoreActive::GetActiveInfo()
{
    CLogicActiveBase::active_info_t stActiveInfo;

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter != m_pUserData->m_stUserActiveMap.End())
    {
        if (CLogicCommon::IsCurrentDay(stIter->second->GetActiveTime()) == false)
        {
            stIter->second->SetActiveReceivedBitmap(0);
        }

        stActiveInfo.m_iTargetNum = stIter->second->GetActiveTargetNum(); 
        stActiveInfo.m_ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
        stActiveInfo.m_ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
        stActiveInfo.m_iLastReceiveAwardTime = stIter->second->GetActiveTime();
    }
    
    return (stActiveInfo);
}

bool 
CLogicBenefitStoreActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBenefitStoreConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stBenefitStoreConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    if (CLogicCommon::IsCurrentDay(stIter->second->GetActiveTime()) == false)
    {
        stIter->second->SetActiveReceivedBitmap(0);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if (!CLogicCacheCommon::ConsumeGameItem(m_pUserData, stConfig[iEntityID - 1].m_stConsumeItem, iCmd, {
                {"id", std::to_string(m_iActivityID)}, { "index", std::to_string(iEntityID) }
        }))
        {
            return false;
        }

        ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
        ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

        stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
        stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

        stIter->second->SetActiveTime(CLogicCommon::GetSec());

        stBonusList.push_back(stConfig[iEntityID - 1].m_stBonusItem);

        return (true);
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
CLogicCumulateLoginActive::CLogicCumulateLoginActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

bool 
CLogicCumulateLoginActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stNewServerConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stNewServerConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if (m_pUserData->m_stUserInfoTable.GetAddUpLoginDay() >= stConfig[iEntityID - 1].m_iSignDay)
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
}

////////////////////////////////////////////////////////////////////////////////
CLogicDeadlineLoginActive::CLogicDeadlineLoginActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void 
CLogicDeadlineLoginActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stDeadlineLoginConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stDeadlineLoginConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool 
CLogicDeadlineLoginActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stDeadlineLoginConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stDeadlineLoginConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
}

int
CLogicDeadlineLoginActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iSignDay = stIter->second->GetActiveTargetNum(); 

    bool iHasRedPoint = 0;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
    uint64_t ulActiveBitmap = stIter->second->GetActiveReceivedBitmap();
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stDeadlineLoginConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stDeadlineLoginConfig.end())
    {
        for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if ((ulBitmapMask & ulActiveBitmap) == 0 &&
                (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iSignDay <= iSignDay)
                {
                    iHasRedPoint = 1;

                    ulCompleteBitmap |= ulBitmapMask;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (iHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicBuyCommodityActive::CLogicBuyCommodityActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void 
CLogicBuyCommodityActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stBuyCommodityConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stBuyCommodityConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool 
CLogicBuyCommodityActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBuyCommodityConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stBuyCommodityConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
}

int
CLogicBuyCommodityActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTimes = stIter->second->GetActiveTargetNum(); 

    bool iHasRedPoint = 0;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
    uint64_t ulActiveBitmap = stIter->second->GetActiveReceivedBitmap();
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBuyCommodityConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stBuyCommodityConfig.end())
    {
        for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if ((ulBitmapMask & ulActiveBitmap) == 0 &&
                (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTimes)
                {
                    iHasRedPoint = 1;

                    ulCompleteBitmap |= ulBitmapMask;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (iHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicCumulateDepositActive::CLogicCumulateDepositActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void 
CLogicCumulateDepositActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stDepositConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stDepositConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool 
CLogicCumulateDepositActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stDepositConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stDepositConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
}

int
CLogicCumulateDepositActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotal = stIter->second->GetActiveTargetNum(); 

    bool iHasRedPoint = 0;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
    uint64_t ulActiveBitmap = stIter->second->GetActiveReceivedBitmap();
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stDepositConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stDepositConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulActiveBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iAddUpRmb <= iTotal)
                {
                    iHasRedPoint = 1;

                    ulCompleteBitmap |= ulBitmapMask;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (iHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicSingleDepositActive::CLogicSingleDepositActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

CLogicActiveBase::active_info_t
CLogicSingleDepositActive::GetActiveInfo()
{
    CLogicActiveBase::active_info_t stActiveInfo;

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter != m_pUserData->m_stUserActiveMap.End())
    {
        if (CLogicCommon::IsCurrentDay(stIter->second->GetActiveTime()) == false)
        {
            stIter->second->SetActiveCompleteBitmap(0);
            stIter->second->SetActiveReceivedBitmap(0);
            stIter->second->SetActiveTime(CLogicCommon::GetSec());
        }

        stActiveInfo.m_iTargetNum = stIter->second->GetActiveTargetNum(); 
        stActiveInfo.m_ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
        stActiveInfo.m_ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
        stActiveInfo.m_iLastReceiveAwardTime = stIter->second->GetActiveTime();
    }
    
    return (stActiveInfo);
}

void 
CLogicSingleDepositActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stSingleDepositConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stSingleDepositConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool 
CLogicSingleDepositActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stSingleDepositConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stSingleDepositConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
}

int
CLogicSingleDepositActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    if (CLogicCommon::IsCurrentDay(stIter->second->GetActiveTime()) == false)
    {
        stIter->second->SetActiveCompleteBitmap(0);
        stIter->second->SetActiveReceivedBitmap(0);
        stIter->second->SetActiveTime(CLogicCommon::GetSec());
    }

    bool iHasRedPoint = 0;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
    uint64_t ulActiveBitmap = stIter->second->GetActiveReceivedBitmap();
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stSingleDepositConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stSingleDepositConfig.end())
    {
        for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if ((ulBitmapMask & ulActiveBitmap) == 0 &&
                (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iRmb == iNum)
                {
                    iHasRedPoint = 1;

                    ulCompleteBitmap |= ulBitmapMask;

                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (iHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicDailyCumulateDepositActive::CLogicDailyCumulateDepositActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

CLogicActiveBase::active_info_t
CLogicDailyCumulateDepositActive::GetActiveInfo()
{
    CLogicActiveBase::active_info_t stActiveInfo;

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter != m_pUserData->m_stUserActiveMap.End())
    {
        if (LOGIC_CHECK_DAY_REFRESH(stIter->second->GetActiveTime()))
        {
            stIter->second->SetActiveTargetNum(0);
            stIter->second->SetActiveCompleteBitmap(0);
            stIter->second->SetActiveReceivedBitmap(0);
            stIter->second->SetActiveTime(CLogicCommon::GetSec());
        }

        stActiveInfo.m_iTargetNum = stIter->second->GetActiveTargetNum(); 
        stActiveInfo.m_ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
        stActiveInfo.m_ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
        stActiveInfo.m_iLastReceiveAwardTime = stIter->second->GetActiveTime();
    }
    
    return (stActiveInfo);
}

void 
CLogicDailyCumulateDepositActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stDailyCumulateDepositConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stDailyCumulateDepositConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool 
CLogicDailyCumulateDepositActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stDailyCumulateDepositConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stDailyCumulateDepositConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
}

int
CLogicDailyCumulateDepositActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    if (LOGIC_CHECK_DAY_REFRESH(stIter->second->GetActiveTime()))
    {
        stIter->second->SetActiveTargetNum(0);
        stIter->second->SetActiveCompleteBitmap(0);
        stIter->second->SetActiveReceivedBitmap(0);
        stIter->second->SetActiveTime(CLogicCommon::GetSec());
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTarget = stIter->second->GetActiveTargetNum();

    bool iHasRedPoint = 0;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
    uint64_t ulActiveBitmap = stIter->second->GetActiveReceivedBitmap();
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stDailyCumulateDepositConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stDailyCumulateDepositConfig.end())
    {
        for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if ((ulBitmapMask & ulActiveBitmap) == 0 &&
                (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iRmb <= iTarget)
                {
                    iHasRedPoint = 1;

                    ulCompleteBitmap |= ulBitmapMask;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (iHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicCornucopiaActive::CLogicCornucopiaActive(user_data_table_ptr_type pUserData, int iActivityID)
: CLogicActiveBase(pUserData, iActivityID) {}

bool 
CLogicCornucopiaActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    (void) iEntityID;

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stCornucopiaConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stCornucopiaConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    int iTimes = stIter->second->GetActiveTargetNum();
    if (iTimes >= (int)stConfig.size())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfigElem = stConfig[iTimes];

    if (m_pUserData->m_stUserInfoTable.GetVipLevel() < stConfigElem.m_iVipLevel)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(NOT_ENOUGH_VIP_LEVEL);
        return (false);
    }

    if (stConfigElem.m_iNeedYuanBao <= m_pUserData->m_stUserInfoTable.GetRmb())
    {
        int iAddYuanBao = (CLogicCommon::GetRandNum() % (stConfigElem.m_iRealRebateMax - stConfigElem.m_iRealRebateMin)) + stConfigElem.m_iRealRebateMin;

        CPackGameItem stItem;
        stItem.m_iItemType = CLogicConfigDefine::CURRENCY;
        stItem.m_iCardID = CLogicConfigDefine::ECIT_YuanBaoID;
        stItem.m_iNum = iAddYuanBao - stConfigElem.m_iNeedYuanBao;

		m_pUserData->m_stUserInfoTable.AddCornucopiaProfitYuanbao(stItem.m_iNum);

        stBonusList.push_back(stItem);

        stIter->second->AddActiveTargetNum(1);

        char szBuffer[128];
        int iTotalTimes = stConfig.size();
        snprintf(szBuffer, sizeof(szBuffer), "%d", iTotalTimes - iTimes - 1);
        m_stParamMap["left_times"] = szBuffer;
        snprintf(szBuffer, sizeof(szBuffer), "%d", iTimes + 1);
        m_stParamMap["used_times"] = szBuffer;
        snprintf(szBuffer, sizeof(szBuffer), "%d", iAddYuanBao);
        m_stParamMap["add_yuanbao"] = szBuffer;
        snprintf(szBuffer, sizeof(szBuffer), "%d", stConfigElem.m_iNeedYuanBao);
        m_stParamMap["consume_yuanbao"] = szBuffer;
		m_stParamMap["profit_yuanbao"] = std::to_string(m_pUserData->m_stUserInfoTable.GetCornucopiaProfitYuanbao());

		CPackLuckyCatHistory stHistory;
		stHistory.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
		stHistory.m_iUID = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;;
		stHistory.m_strName = m_pUserData->m_strNick;
		stHistory.m_iNum = iAddYuanBao;
		CLogicGlobal::AddLuckCatHistory(stHistory);

        CLogicGlobal::PublishBroadcast("lucky_cat", iTimes + 1, {
			{ "times", std::to_string(iTimes + 1) },
			{ "nick", m_pUserData->m_strNick },
			{ "item_name", CLogicConfig::GetItemName(stItem.m_iItemType, stItem.m_iCardID) },
			{ "item_count", std::to_string(iAddYuanBao) },
			});

        return (true);
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(NOT_ENOUGH_RMB);
        return (false);
    }
}

CLogicActiveBase::active_info_t
CLogicCornucopiaActive::GetActiveInfo()
{
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stCornucopiaConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stCornucopiaConfig.end())
    {
        const auto& stConfig = stConfigIter->second;

        int iTimes = 0;

        auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
        if (stIter != m_pUserData->m_stUserActiveMap.End())
        {
            iTimes = stIter->second->GetActiveTargetNum();
        }

        char szBuffer[128];
        int iTotalTimes = stConfig.size();
        snprintf(szBuffer, sizeof(szBuffer), "%d", iTotalTimes - iTimes);
        m_stParamMap["left_times"] = szBuffer;
		m_stParamMap["profit_yuanbao"] = std::to_string(m_pUserData->m_stUserInfoTable.GetCornucopiaProfitYuanbao());
    }

    return (CLogicActiveBase::GetActiveInfo());
}

////////////////////////////////////////////////////////////////////////////////
CLogicFoundationActive::CLogicFoundationActive(user_data_table_ptr_type pUserData, int iActivityID)
: CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicFoundationActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stFoundationConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stFoundationConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool 
CLogicFoundationActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stFoundationConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stFoundationConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.m_stRewards.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            CPackGameItem stItem;

            stItem.m_iItemType = stConfig.m_stRewards[iEntityID - 1].m_iItemType;
            stItem.m_iCardID = stConfig.m_stRewards[iEntityID - 1].m_iCardID;
            stItem.m_iNum = stConfig.m_stRewards[iEntityID - 1].m_iNum;

            stBonusList.push_back(stItem);
            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
}

int
CLogicFoundationActive::UpdateActiveInfo(int iNum)
{
    if (m_pUserData->m_stIBusyData.GetFoundationOpenTime() <= 0)
    {
        return 0;
    }

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    int iUserLevel = m_pUserData->m_stUserInfoTable.GetLevel();

    int iHasRedPoint = 0;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap = 0x0;
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
    uint64_t ulActiveBitmap = stIter->second->GetActiveReceivedBitmap();
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stFoundationConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stFoundationConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.m_stUserLevel.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulActiveBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second.m_stUserLevel[uiIndex] <= iUserLevel)
                {
                    iHasRedPoint = 1;

                    ulCompleteBitmap |= ulBitmapMask;
                }
            }
        }
    }
    
    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (iHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicLevelRaceActive::CLogicLevelRaceActive(user_data_table_ptr_type pUserData, int iActivityID)
: CLogicActiveBase(pUserData, iActivityID) {}

void CLogicLevelRaceActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stLevelRaceConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stLevelRaceConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool CLogicLevelRaceActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stLevelRaceConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stLevelRaceConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID == 0)
    {
        int iTargetNum = stIter->second->GetActiveTargetNum();
        if (iTargetNum == EXTRA_AWARD_TASK_COMPLETING)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
        else if (iTargetNum == EXTRA_AWARD_TASK_RECEIVED)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
            return (false);
        }
        else
        {
            stIter->second->SetActiveTargetNum(EXTRA_AWARD_TASK_RECEIVED);

            CPackGameItem stItem;

            stItem.m_iItemType = stConfig.m_stExtraAwardItem.m_iItemType;
            stItem.m_iCardID = stConfig.m_stExtraAwardItem.m_iCardID;
            stItem.m_iNum = stConfig.m_stExtraAwardItem.m_iNum;

            stBonusList.push_back(stItem);
            return (true);
        }
    }

    if (iEntityID > (int)stConfig.m_stAwardVec.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
    uint64_t ulParaEx = stIter->second->GetParaEx();

    if ((ulBitmapMask & ulReceivedBitmap) && (ulBitmapMask & ulParaEx) == 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }

    if ((ulBitmapMask & ulReceivedBitmap) == 0 || (ulBitmapMask & ulParaEx) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0)
            {
                ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
                stBonusList = stConfig.m_stAwardVec[iEntityID - 1].m_stBonusItemVec;
            }

            if((ulBitmapMask & ulParaEx) == 0)
            {
                ulParaEx = ulBitmapMask | ulParaEx;
                stBonusList.push_back(stConfig.m_stAwardVec[iEntityID - 1].m_stQuarterReward);
            }
            // ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            // stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);
            stIter->second->SetParaEx(ulParaEx);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }

    return (true);
}

int
CLogicLevelRaceActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    int iUserLevel = m_pUserData->m_stUserInfoTable.GetLevel();

    int iHasRedPoint = 0;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
    uint64_t ulActiveBitmap = stIter->second->GetActiveReceivedBitmap();
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stLevelRaceConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stLevelRaceConfig.end())
    {
        auto& rstLevelRaceActive = CLogicGlobal::GetGlobalCache()->m_stRecoveryData.m_stLevelRaceActive[m_iActivityID];
        int iTotalAchieve = rstLevelRaceActive.m_iTotalAchieve;
        int iMemberLimit = stConfigIter->second.m_iExtraAwardMemberLimit;
        int iExtraBonusLevel = stConfigIter->second.m_iExtraAwardLevel;

        if (iTotalAchieve < iMemberLimit &&
            iExtraBonusLevel <= iUserLevel &&
            stIter->second->GetActiveTargetNum() == EXTRA_AWARD_TASK_COMPLETING)
        {
            ++ rstLevelRaceActive.m_iTotalAchieve;

            /*
            CPackLevelRaceRank stElem;
            stElem.m_iTime = CLogicCommon::GetSec();
            stElem.m_strName = m_pUserData->m_strNick;

            rstLevelRaceActive.m_stLevelRaceRank.push_back(stElem);
            */

            stIter->second->SetActiveTargetNum(EXTRA_AWARD_TASK_COMPLETED);
        }

        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.m_stAwardVec.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulActiveBitmap) == 0)
            {
                if (stConfigIter->second.m_stAwardVec[uiIndex].m_iUserLevel <= iUserLevel)
                {
                    iHasRedPoint = 1;

                    ulCompleteBitmap |= ulBitmapMask;
                }
            }
        }
    }

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (iHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicHeroFlashSaleActive::CLogicHeroFlashSaleActive(user_data_table_ptr_type pUserData, int iActivityID)
: CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicHeroFlashSaleActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stHeroFlashSaleConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stHeroFlashSaleConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            pActiveObjPtr->UpdateActiveInfo(iNum);
        }
    }
}

CLogicActiveBase::active_info_t
CLogicHeroFlashSaleActive::GetActiveInfo()
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        stVal.m_iActiveTime = 0;
        stVal.m_iActiveTargetNum = PURCHASE_LIMIT_TASK_COMPLETING;
        stVal.m_ulActiveReceivedBitmap = 0;
        stVal.m_ulActiveCompleteBitmap = 0;
        
        auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroFlashSaleConfig.find(m_iActivityID);
        if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeroFlashSaleConfig.end())
        {
            auto stLevelIter = m_pUserData->m_stLevelInfoTableMap.Find(stConfigIter->second.m_iLimitLevelID);
            if (stLevelIter != m_pUserData->m_stLevelInfoTableMap.End())
            {
                if (stLevelIter->second->GetTotalVictoryTimes() > 0)
                {
                    stVal.m_iActiveTargetNum = PURCHASE_LIMIT_TASK_COMPLETED;
                }
            }
        }

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        if (stRet.second == true) stIter = stRet.first;
    }

    CLogicActiveBase::active_info_t stActiveInfo;

    if(stIter != m_pUserData->m_stUserActiveMap.End())
    {
        stActiveInfo.m_iTargetNum = stIter->second->GetActiveTargetNum(); 
        stActiveInfo.m_ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
        stActiveInfo.m_ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
        stActiveInfo.m_iLastReceiveAwardTime = stIter->second->GetActiveTime();
    }
    
    return (stActiveInfo);
}

bool
CLogicHeroFlashSaleActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroFlashSaleConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroFlashSaleConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID == 1)
    {
        if (stIter->second->GetActiveTargetNum() == PURCHASE_LIMIT_TASK_COMPLETING)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
        else if (stIter->second->GetActiveTargetNum() == PURCHASE_LIMIT_TASK_RECEIVED)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
            return (false);
        }
        else
        {
            if (false == CLogicCacheCommon::ConsumeGameItem(m_pUserData, stConfig.m_stLimitConsumeItem, iCmd, {
                    {"id", std::to_string(m_iActivityID)}, { "index", std::to_string(iEntityID) }
            }))
            {
                return (false);
            }
        }
    }
    else
    {
        if (stIter->second->GetActiveTargetNum() == PURCHASE_LIMIT_TASK_RECEIVED)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
            return (false);
        }

        if (false == CLogicCacheCommon::ConsumeGameItem(m_pUserData, stConfig.m_stConsumeItem, iCmd, {
                {"id", std::to_string(m_iActivityID)}, { "index", std::to_string(iEntityID) }
        }))
        {
            return (false);
        }
    }

    CPackGameItem stItem;

    stItem.m_iItemType = stConfig.m_stAwardItem.m_iItemType;
    stItem.m_iCardID = stConfig.m_stAwardItem.m_iCardID;
    stItem.m_iNum = stConfig.m_stAwardItem.m_iNum;

    stBonusList.push_back(stItem);

    stIter->second->SetActiveTargetNum(PURCHASE_LIMIT_TASK_RECEIVED);

    return (true);
}

int
CLogicHeroFlashSaleActive::UpdateActiveInfo(int iNum)
{
    int iLevelID = iNum;

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroFlashSaleConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroFlashSaleConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (stConfig.m_iLimitLevelID == iLevelID && stIter->second->GetActiveTargetNum() == PURCHASE_LIMIT_TASK_COMPLETING)
    {
        stIter->second->SetActiveTargetNum(PURCHASE_LIMIT_TASK_COMPLETED);
    }

    return (0);
}

////////////////////////////////////////////////////////////////////////////////
CLogicOpenServerLoginActive::CLogicOpenServerLoginActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

bool
CLogicOpenServerLoginActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stOpenServerLoginConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stOpenServerLoginConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
		int32_t iDiffDay = day_diff(m_pUserData->m_stUserInfoTable.GetCreateTime(), CLogicCommon::GetSec()) + 1;
        if (iDiffDay >= iEntityID)
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
CLogicOpenServerBuyActive::CLogicOpenServerBuyActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

bool
CLogicOpenServerBuyActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stOpenServerBuyConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stOpenServerBuyConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        const auto& stConsumeItemVec = stConfig[iEntityID - 1].m_stConsumeItemVec;
        if (!CLogicCacheCommon::ConsumeGameItem(m_pUserData, stConsumeItemVec, iCmd, {
                {"id", std::to_string(m_iActivityID)}, { "index", std::to_string(iEntityID) }
        }))
        {
            return false;
        }

        ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
        ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

        stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
        stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

        stIter->second->SetActiveTime(CLogicCommon::GetSec());

        stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

        return (true);
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
CLogicSingleItemExchangeActive::CLogicSingleItemExchangeActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

bool
CLogicSingleItemExchangeActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    if (iReceiveTimes < 1)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(PARAMETER_ERROR);
        return (false);
    }

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stSingleItemExchangeConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stSingleItemExchangeConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

	if (m_pUserData->m_stUserInfoTable.GetVipLevel() < stConfig[iEntityID - 1].m_iVipLevel)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(NOT_ENOUGH_VIP_LEVEL);
		return (false);
	}

    int iTargetNum = stIter->second->GetActiveTargetNum();
    if (iTargetNum + iReceiveTimes <= stConfig[iEntityID - 1].m_iTimes)
    {
        const auto& stConsumeItemVec = stConfig[iEntityID - 1].m_stConsumeItemVec;
        std::vector<CPackGameItem> stConsumes;
        for(const auto& item : stConsumeItemVec)
        {
            stConsumes.emplace_back(item.m_iItemType, item.m_iCardID, item.m_iNum * iReceiveTimes);
        }

        if (CLogicCacheCommon::ConsumeGameItem(m_pUserData, stConsumes, iCmd,
                                               {{"id", std::to_string(m_iActivityID)},
                                                 { "index", std::to_string(iEntityID) },
                                                 {"times", std::to_string(iReceiveTimes)}}))
        {
            stIter->second->SetActiveTargetNum(iTargetNum + iReceiveTimes);
            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            const auto& stBonus = stConfig[iEntityID - 1].m_stBonusItem;
            stBonusList.emplace_back(stBonus.m_iItemType, stBonus.m_iCardID, stBonus.m_iNum * iReceiveTimes);

            return (true);
        }
        return false;
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
CLogicExchangeStoreActive::CLogicExchangeStoreActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

bool
CLogicExchangeStoreActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stExchangeStoreConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stExchangeStoreConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        const auto& stConsumeItemVec = stConfig[iEntityID - 1].m_stConsumeItemVec;
        if (!CLogicCacheCommon::ConsumeGameItem(m_pUserData, stConsumeItemVec, iCmd, {
                {"id", std::to_string(m_iActivityID)}, { "index", std::to_string(iEntityID) }
        }))
        {
            return false;
        }

        ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
        ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

        stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
        stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

        stIter->second->SetActiveTime(CLogicCommon::GetSec());

        stBonusList.push_back(stConfig[iEntityID - 1].m_stBonusItem);

        return (true);
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
CLogicNormalLevelActive::CLogicNormalLevelActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void 
CLogicNormalLevelActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stNormalLevelConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stNormalLevelConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicNormalLevelActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stNormalLevelConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stNormalLevelConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicNormalLevelActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    int iLevelID = iNum;
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stNormalLevelConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stNormalLevelConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iLevelID <= iLevelID)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicEliteLevelActive::CLogicEliteLevelActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicEliteLevelActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stEliteLevelConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stEliteLevelConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicEliteLevelActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stEliteLevelConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stEliteLevelConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicEliteLevelActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    int iLevelID = iNum;
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stEliteLevelConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stEliteLevelConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iLevelID <= iLevelID)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicPVPRankActive::CLogicPVPRankActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicPVPRankActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stPVPRankConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stPVPRankConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicPVPRankActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPVPRankConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stPVPRankConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicPVPRankActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    int iRank = iNum;
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPVPRankConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stPVPRankConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iRank >= iRank)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveTargetNum(iRank);
    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicUnlimitLevelScoreActive::CLogicUnlimitLevelScoreActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicUnlimitLevelScoreActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stUnlimitLevelScoreConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stUnlimitLevelScoreConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicUnlimitLevelScoreActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stUnlimitLevelScoreConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stUnlimitLevelScoreConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicUnlimitLevelScoreActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    int iScore = 0;//m_pUserData->m_stUserInfoTable.GetUnlimitLevelMaxScore();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    stIter->second->SetActiveTargetNum(iScore);

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stUnlimitLevelScoreConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stUnlimitLevelScoreConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iScore <= iScore)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicPassLevelStarActive::CLogicPassLevelStarActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicPassLevelStarActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stPassLevelStarConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stPassLevelStarConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicPassLevelStarActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPassLevelStarConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stPassLevelStarConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicPassLevelStarActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalStar = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPassLevelStarConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stPassLevelStarConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iStar <= iTotalStar)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicPassLevelTimesActive::CLogicPassLevelTimesActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicPassLevelTimesActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stPassLevelTimesConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stPassLevelTimesConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicPassLevelTimesActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPassLevelTimesConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stPassLevelTimesConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicPassLevelTimesActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPassLevelTimesConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stPassLevelTimesConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicBuyEnergyTimesActive::CLogicBuyEnergyTimesActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicBuyEnergyTimesActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stBuyEnergyTimesConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stBuyEnergyTimesConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicBuyEnergyTimesActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBuyEnergyTimesConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stBuyEnergyTimesConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicBuyEnergyTimesActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBuyEnergyTimesConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stBuyEnergyTimesConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicBuyGoldTimesActive::CLogicBuyGoldTimesActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicBuyGoldTimesActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stBuyGoldTimesConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stBuyGoldTimesConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicBuyGoldTimesActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBuyGoldTimesConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stBuyGoldTimesConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicBuyGoldTimesActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBuyGoldTimesConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stBuyGoldTimesConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicCrossFightPassLevelTimesActive::CLogicCrossFightPassLevelTimesActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicCrossFightPassLevelTimesActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stCrossFightPassLevelTimesConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stCrossFightPassLevelTimesConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicCrossFightPassLevelTimesActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stCrossFightPassLevelTimesConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stCrossFightPassLevelTimesConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicCrossFightPassLevelTimesActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stCrossFightPassLevelTimesConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stCrossFightPassLevelTimesConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicUnlimitLevelFightTimesActive::CLogicUnlimitLevelFightTimesActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicUnlimitLevelFightTimesActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stUnlimitLevelFightTimesConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stUnlimitLevelFightTimesConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicUnlimitLevelFightTimesActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stUnlimitLevelFightTimesConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stUnlimitLevelFightTimesConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicUnlimitLevelFightTimesActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stUnlimitLevelFightTimesConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stUnlimitLevelFightTimesConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicBuyBlackStoreTimesActive::CLogicBuyBlackStoreTimesActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicBuyBlackStoreTimesActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stBuyBlackStoreTimesConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stBuyBlackStoreTimesConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicBuyBlackStoreTimesActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBuyBlackStoreTimesConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stBuyBlackStoreTimesConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicBuyBlackStoreTimesActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBuyBlackStoreTimesConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stBuyBlackStoreTimesConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicCumulateConsumeYuanBaoActive::CLogicCumulateConsumeYuanBaoActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicCumulateConsumeYuanBaoActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stCumulateConsumeYuanBaoConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stCumulateConsumeYuanBaoConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicCumulateConsumeYuanBaoActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stCumulateConsumeYuanBaoConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stCumulateConsumeYuanBaoConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicCumulateConsumeYuanBaoActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stCumulateConsumeYuanBaoConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stCumulateConsumeYuanBaoConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iCount <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicDailyConsumeYuanBaoActive::CLogicDailyConsumeYuanBaoActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

CLogicActiveBase::active_info_t CLogicDailyConsumeYuanBaoActive::GetActiveInfo()
{
	CLogicActiveBase::active_info_t stActiveInfo;

	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter != m_pUserData->m_stUserActiveMap.End())
	{
		if (LOGIC_CHECK_DAY_REFRESH(stIter->second->GetActiveTime()))
		{
			stIter->second->SetActiveTargetNum(0);
			stIter->second->SetActiveCompleteBitmap(0);
			stIter->second->SetActiveReceivedBitmap(0);
			stIter->second->SetActiveTime(CLogicCommon::GetSec());
		}

		stActiveInfo.m_iTargetNum = stIter->second->GetActiveTargetNum();
		stActiveInfo.m_ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
		stActiveInfo.m_ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
		stActiveInfo.m_iLastReceiveAwardTime = stIter->second->GetActiveTime();
	}

	return (stActiveInfo);
}

void
CLogicDailyConsumeYuanBaoActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stDailyConsumeYuanBaoConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicDailyConsumeYuanBaoActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stDailyConsumeYuanBaoConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stDailyConsumeYuanBaoConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicDailyConsumeYuanBaoActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	if (LOGIC_CHECK_DAY_REFRESH(stIter->second->GetActiveTime()))
	{
		stIter->second->SetActiveTargetNum(0);
		stIter->second->SetActiveCompleteBitmap(0);
		stIter->second->SetActiveReceivedBitmap(0);
		stIter->second->SetActiveTime(CLogicCommon::GetSec());
	}

	stIter->second->AddActiveTargetNum(iNum);

	int iTotalTimes = stIter->second->GetActiveTargetNum();
	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stDailyConsumeYuanBaoConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stDailyConsumeYuanBaoConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTotalTimes)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}


////////////////////////////////////////////////////////////////////////////////
CLogicEliteLevelFightTimesActive::CLogicEliteLevelFightTimesActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicEliteLevelFightTimesActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stEliteLevelFightTimesConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stEliteLevelFightTimesConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicEliteLevelFightTimesActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stEliteLevelFightTimesConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stEliteLevelFightTimesConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicEliteLevelFightTimesActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stEliteLevelFightTimesConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stEliteLevelFightTimesConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicNormalLevelFightTimesActive::CLogicNormalLevelFightTimesActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicNormalLevelFightTimesActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stNormalLevelFightTimesConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stNormalLevelFightTimesConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicNormalLevelFightTimesActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stNormalLevelFightTimesConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stNormalLevelFightTimesConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicNormalLevelFightTimesActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stNormalLevelFightTimesConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stNormalLevelFightTimesConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicFightThemeActiveLevelActive::CLogicFightThemeActiveLevelActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicFightThemeActiveLevelActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stFightThemeActiveLevelConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stFightThemeActiveLevelConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicFightThemeActiveLevelActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stFightThemeActiveLevelConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stFightThemeActiveLevelConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicFightThemeActiveLevelActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stFightThemeActiveLevelConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stFightThemeActiveLevelConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicPVPFightTimesActive::CLogicPVPFightTimesActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicPVPFightTimesActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stPVPFightTimesConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stPVPFightTimesConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicPVPFightTimesActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPVPFightTimesConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stPVPFightTimesConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicPVPFightTimesActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    int iTotalTimes = stIter->second->GetActiveTargetNum();
    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPVPFightTimesConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stPVPFightTimesConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTotalTimes)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicHeroEquipmentUpgradeLevelActive::CLogicHeroEquipmentUpgradeLevelActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicHeroEquipmentUpgradeLevelActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeLevelConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeLevelConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicHeroEquipmentUpgradeLevelActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeLevelConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeLevelConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicHeroEquipmentUpgradeLevelActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeLevelConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeLevelConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                int iHeroNum = 0;
                int iTargetHeroNum = stConfigIter->second[uiIndex].m_iHeroNum;
                /*
                int iTargetEquipmentLevel = stConfigIter->second[uiIndex].m_iEquipmentLevel;

                auto stCardIterBeg = m_pUserData->m_stHeroCardTableMap.Begin();
                auto stCardIterEnd = m_pUserData->m_stHeroCardTableMap.End();
                for ( ; stCardIterBeg != stCardIterEnd; stCardIterBeg++ )
                {
                    if (stCardIterBeg->second->GetEquipmentLevel1() >= iTargetEquipmentLevel &&
                        stCardIterBeg->second->GetEquipmentLevel2() >= iTargetEquipmentLevel &&
                        stCardIterBeg->second->GetEquipmentLevel3() >= iTargetEquipmentLevel &&
                        stCardIterBeg->second->GetEquipmentLevel4() >= iTargetEquipmentLevel &&
                        stCardIterBeg->second->GetEquipmentLevel5() >= iTargetEquipmentLevel &&
                        stCardIterBeg->second->GetEquipmentLevel6() >= iTargetEquipmentLevel)
                    {
                        ++iHeroNum;
                    }
                }
                */

                if (iTargetHeroNum <= iHeroNum)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicHeroEquipmentUpgradeJewelActive::CLogicHeroEquipmentUpgradeJewelActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicHeroEquipmentUpgradeJewelActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeJewelConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeJewelConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicHeroEquipmentUpgradeJewelActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeJewelConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeJewelConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicHeroEquipmentUpgradeJewelActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeJewelConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeJewelConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                int iJewelNum = 0;
                int iTargetJewelNum = stConfigIter->second[uiIndex].m_iJewelNum;

                if (iTargetJewelNum <= iJewelNum)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicHeroEquipmentUpgradeStarActive::CLogicHeroEquipmentUpgradeStarActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicHeroEquipmentUpgradeStarActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeStarConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeStarConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicHeroEquipmentUpgradeStarActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeStarConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeStarConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicHeroEquipmentUpgradeStarActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeStarConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeroEquipmentUpgradeStarConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                int iEquipmentNum = 0;
                int iTargetEquipmentNum = stConfigIter->second[uiIndex].m_iEquipmentNum;
                /*
                int iTargetEquipmentStar = stConfigIter->second[uiIndex].m_iEquipmentStar;

                auto stCardIterBeg = m_pUserData->m_stHeroCardTableMap.Begin();
                auto stCardIterEnd = m_pUserData->m_stHeroCardTableMap.End();
                for ( ; stCardIterBeg != stCardIterEnd; stCardIterBeg++ )
                {
                    if (stCardIterBeg->second->GetEquipmentStar1() >= iTargetEquipmentStar) ++iEquipmentNum;
                    if (stCardIterBeg->second->GetEquipmentStar2() >= iTargetEquipmentStar) ++iEquipmentNum;
                    if (stCardIterBeg->second->GetEquipmentStar3() >= iTargetEquipmentStar) ++iEquipmentNum;
                    if (stCardIterBeg->second->GetEquipmentStar4() >= iTargetEquipmentStar) ++iEquipmentNum;
                    if (stCardIterBeg->second->GetEquipmentStar5() >= iTargetEquipmentStar) ++iEquipmentNum;
                    if (stCardIterBeg->second->GetEquipmentStar6() >= iTargetEquipmentStar) ++iEquipmentNum;
                }
                */

                if (iTargetEquipmentNum <= iEquipmentNum)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicHeroUpgradeColorActive::CLogicHeroUpgradeColorActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicHeroUpgradeColorActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stHeroUpgradeColorConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stHeroUpgradeColorConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicHeroUpgradeColorActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroUpgradeColorConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroUpgradeColorConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicHeroUpgradeColorActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroUpgradeColorConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeroUpgradeColorConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                int iHeroNum = 0;
                int iTargetHeroNum = stConfigIter->second[uiIndex].m_iHeroNum;
                int iTargetColor = stConfigIter->second[uiIndex].m_iColor;

                auto stCardIterBeg = m_pUserData->m_stHeroCardTableMap.Begin();
                auto stCardIterEnd = m_pUserData->m_stHeroCardTableMap.End();
                for ( ; stCardIterBeg != stCardIterEnd; stCardIterBeg++ )
                {
                    if (stCardIterBeg->second->GetCardColor() >= iTargetColor) ++iHeroNum;
                }

                if (iTargetHeroNum <= iHeroNum)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicHeroUpgradeLevelActive::CLogicHeroUpgradeLevelActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicHeroUpgradeLevelActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stHeroUpgradeLevelConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stHeroUpgradeLevelConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicHeroUpgradeLevelActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroUpgradeLevelConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroUpgradeLevelConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicHeroUpgradeLevelActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroUpgradeLevelConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeroUpgradeLevelConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                int iHeroNum = 0;
                int iTargetHeroNum = stConfigIter->second[uiIndex].m_iHeroNum;
                int iTargetLevel = stConfigIter->second[uiIndex].m_iLevel;

                auto stCardIterBeg = m_pUserData->m_stHeroCardTableMap.Begin();
                auto stCardIterEnd = m_pUserData->m_stHeroCardTableMap.End();
                for ( ; stCardIterBeg != stCardIterEnd; stCardIterBeg++ )
                {
                    if (stCardIterBeg->second->GetCardLevel() >= iTargetLevel) ++iHeroNum;
                }

                if (iTargetHeroNum <= iHeroNum)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicHeroUpgradeStarActive::CLogicHeroUpgradeStarActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicHeroUpgradeStarActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stHeroUpgradeStarConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stHeroUpgradeStarConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicHeroUpgradeStarActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroUpgradeStarConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroUpgradeStarConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicHeroUpgradeStarActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroUpgradeStarConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeroUpgradeStarConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                int iHeroNum = 0;
                int iTargetHeroNum = stConfigIter->second[uiIndex].m_iHeroNum;
                int iTargetStar = stConfigIter->second[uiIndex].m_iStar;

                auto stCardIterBeg = m_pUserData->m_stHeroCardTableMap.Begin();
                auto stCardIterEnd = m_pUserData->m_stHeroCardTableMap.End();
                for ( ; stCardIterBeg != stCardIterEnd; stCardIterBeg++ )
                {
                    if (stCardIterBeg->second->GetCardLevel() > 0 && (int)stCardIterBeg->second->GetCardStar() >= iTargetStar) ++iHeroNum;
                }

                if (iTargetHeroNum <= iHeroNum)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicTeamUpgradeLevelActive::CLogicTeamUpgradeLevelActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicTeamUpgradeLevelActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stTeamLevelUpgradeConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stTeamLevelUpgradeConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicTeamUpgradeLevelActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stTeamLevelUpgradeConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stTeamLevelUpgradeConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicTeamUpgradeLevelActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
    int iTeamLevel = m_pUserData->m_stUserInfoTable.GetLevel();
    stIter->second->SetActiveTargetNum(iTeamLevel);

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stTeamLevelUpgradeConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stTeamLevelUpgradeConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iLevel <= iTeamLevel)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicUpgradeSkillActive::CLogicUpgradeSkillActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicUpgradeSkillActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stUpgradeSkillConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stUpgradeSkillConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicUpgradeSkillActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stUpgradeSkillConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stUpgradeSkillConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicUpgradeSkillActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->AddActiveTargetNum(iNum);

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
    int iTargetNum = stIter->second->GetActiveTargetNum();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stUpgradeSkillConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stUpgradeSkillConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iTimes <= iTargetNum)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicFightPowerActive::CLogicFightPowerActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicFightPowerActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stFightPowerConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stFightPowerConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicFightPowerActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stFightPowerConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stFightPowerConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicFightPowerActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if(stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    stIter->second->SetActiveTargetNum(iNum);

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
    int iTargetNum = stIter->second->GetActiveTargetNum();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stFightPowerConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stFightPowerConfig.end())
    {
        for(size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if((ulBitmapMask & ulReceivedBitmap) == 0 &&
               (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (stConfigIter->second[uiIndex].m_iFightPower <= iTargetNum)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicHeroAchieveActive::CLogicHeroAchieveActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicHeroAchieveActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    auto stIterBeg = CLogicConfig::GetActivityConfig().m_stHeroAchieveConfig.begin();
    auto stIterEnd = CLogicConfig::GetActivityConfig().m_stHeroAchieveConfig.end();
    for ( ; stIterBeg != stIterEnd; ++stIterBeg)
    {
        int iActivityID = stIterBeg->first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool
CLogicHeroAchieveActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }
    
    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroAchieveConfig.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroAchieveConfig.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t) 0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList.push_back(stConfig[iEntityID - 1].m_stBonusItem);

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int
CLogicHeroAchieveActive::UpdateActiveInfo(int iNum)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        
        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    int iCardID = iNum;

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap= 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroAchieveConfig.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeroAchieveConfig.end())
    {
        for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex ++, ulBitmapMask = ulBitmapMask << 1)
        {
            if (stConfigIter->second[uiIndex].m_iCardID == iCardID)
            {
                if((ulBitmapMask & ulReceivedBitmap) == 0 &&
                   (ulBitmapMask & ulCompletedBitmap) == 0)
                {
                    CLogicCacheCardOperation stOpt(m_pUserData);
                    if (stOpt.CheckHeroCardExist(iCardID))
                    {
                        bHasRedPoint = true;

                        ulCompleteBitmap |= ulBitmapMask;
                    }
                }

                break;
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicUpgradeTacticsActive::CLogicUpgradeTacticsActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicUpgradeTacticsActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stUpgradeTacticsConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicUpgradeTacticsActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stUpgradeTacticsConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stUpgradeTacticsConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicUpgradeTacticsActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stUpgradeTacticsConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stUpgradeTacticsConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iUpgradeTacticsCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicHeroTrainActive::CLogicHeroTrainActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicHeroTrainActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stHeroTrainConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicHeroTrainActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroTrainConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroTrainConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicHeroTrainActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroTrainConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeroTrainConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iHeroTrainCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicBreakEggActive::CLogicBreakEggActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicBreakEggActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stBreakEggConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicBreakEggActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBreakEggConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stBreakEggConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicBreakEggActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stBreakEggConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stBreakEggConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iBreakEggCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicYuanbaoRecruitActive::CLogicYuanbaoRecruitActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicYuanbaoRecruitActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stYuanbaoRecruitConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicYuanbaoRecruitActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stYuanbaoRecruitConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stYuanbaoRecruitConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicYuanbaoRecruitActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stYuanbaoRecruitConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stYuanbaoRecruitConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iYuanbaoRecruitCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicConsumeEnergyActive::CLogicConsumeEnergyActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicConsumeEnergyActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stConsumeEnergyConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicConsumeEnergyActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stConsumeEnergyConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stConsumeEnergyConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicConsumeEnergyActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stConsumeEnergyConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stConsumeEnergyConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iConsumeEnergyValue <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicClimbTowerActive::CLogicClimbTowerActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicClimbTowerActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stClimbTowerConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicClimbTowerActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stClimbTowerConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stClimbTowerConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicClimbTowerActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->SetActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stClimbTowerConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stClimbTowerConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iClimbTowerOrder <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicPvpTerritoryActive::CLogicPvpTerritoryActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicPvpTerritoryActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stPvpTerritoryConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicPvpTerritoryActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPvpTerritoryConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stPvpTerritoryConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicPvpTerritoryActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->SetActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPvpTerritoryConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stPvpTerritoryConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iPvpTerritoryOrder <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicTacticsCountActive::CLogicTacticsCountActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicTacticsCountActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stTacticsCountConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicTacticsCountActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stTacticsCountConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stTacticsCountConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicTacticsCountActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}


	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stTacticsCountConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stTacticsCountConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iTacticsCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicGuildDonateActive::CLogicGuildDonateActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicGuildDonateActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stGuildDonateConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicGuildDonateActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stGuildDonateConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stGuildDonateConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicGuildDonateActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stGuildDonateConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stGuildDonateConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iGuildDonateCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicGuildTaskActive::CLogicGuildTaskActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicGuildTaskActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stGuildTaskConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicGuildTaskActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stGuildTaskConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stGuildTaskConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicGuildTaskActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stGuildTaskConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stGuildTaskConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iGuildTaskCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicLuckyBoxActive::CLogicLuckyBoxActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicLuckyBoxActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stLuckyBoxConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool
CLogicLuckyBoxActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stLuckyBoxConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stLuckyBoxConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			for (size_t i = 0; i < stConfig[iEntityID - 1].m_stBonusItemVec.size(); ++i)
			{
				if ((i < stConfig[iEntityID - 1].m_stNotify.size()) && (stConfig[iEntityID - 1].m_stNotify[i] > 0))
				{
					CLogicGlobal::PublishBroadcast("lucky_box2", 0, stConfig[iEntityID - 1].m_stNotify[i], {
							{ "nick", m_pUserData->m_strNick },
							{ "item_name", CLogicConfig::GetItemName(stConfig[iEntityID - 1].m_stBonusItemVec[i].m_iItemType, stConfig[iEntityID - 1].m_stBonusItemVec[i].m_iCardID) },
							{ "item_count", std::to_string(stConfig[iEntityID - 1].m_stBonusItemVec[i].m_iNum) }
					});
				}
			}

            // 大奖
            if(stConfig[iEntityID - 1].m_bIsBigPrize)
            {
                // 通用事件
                CLogicEventData stEventData;
                stEventData.m_eType = CLogicConfigDefine::ECET_LuckyHammerFinalAward;
                stEventData.m_iCmd = iCmd;
                LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserData, stEventData);
            }

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicLuckyBoxActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stLuckyBoxConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stLuckyBoxConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////

CLogicCountDownHeroActive::CLogicCountDownHeroActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void CLogicCountDownHeroActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_mapCountDownHeroConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool CLogicCountDownHeroActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_mapCountDownHeroConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_mapCountDownHeroConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			for (size_t i = 0; i < stConfig[iEntityID - 1].m_stBonusItemVec.size(); ++i)
			{
				if ((i < stConfig[iEntityID - 1].m_stNotify.size()) && (stConfig[iEntityID - 1].m_stNotify[i] > 0))
				{
					CLogicGlobal::PublishBroadcast("countdownhero2", 0, stConfig[iEntityID - 1].m_stNotify[i], {
							{ "nick", m_pUserData->m_strNick },
							{ "item_name", CLogicConfig::GetItemName(stConfig[iEntityID - 1].m_stBonusItemVec[i].m_iItemType, stConfig[iEntityID - 1].m_stBonusItemVec[i].m_iCardID) },
							{ "item_count", std::to_string(stConfig[iEntityID - 1].m_stBonusItemVec[i].m_iNum) },
					});
				}
			}

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int CLogicCountDownHeroActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_mapCountDownHeroConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_mapCountDownHeroConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////

CLogicTalentScoreActive::CLogicTalentScoreActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicTalentScoreActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stTalentScoreConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

CLogicActiveBase::active_info_t CLogicTalentScoreActive::GetActiveInfo()
{
	this->UpdateActiveInfo(0);

	CLogicActiveBase::active_info_t stActiveInfo;

	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter != m_pUserData->m_stUserActiveMap.End())
	{
		stActiveInfo.m_iTargetNum = stIter->second->GetActiveTargetNum();
		stActiveInfo.m_ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
		stActiveInfo.m_ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
		stActiveInfo.m_iLastReceiveAwardTime = stIter->second->GetActiveTime();
	}

	return (stActiveInfo);
}

bool
CLogicTalentScoreActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stTalentScoreConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stTalentScoreConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicTalentScoreActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stTalentScoreConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stTalentScoreConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicTotalHeroTrainActive::CLogicTotalHeroTrainActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicTotalHeroTrainActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stTotalHeroTrainConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

CLogicActiveBase::active_info_t CLogicTotalHeroTrainActive::GetActiveInfo()
{
	this->UpdateActiveInfo(0);

	CLogicActiveBase::active_info_t stActiveInfo;

	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter != m_pUserData->m_stUserActiveMap.End())
	{
		stActiveInfo.m_iTargetNum = stIter->second->GetActiveTargetNum();
		stActiveInfo.m_ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
		stActiveInfo.m_ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();
		stActiveInfo.m_iLastReceiveAwardTime = stIter->second->GetActiveTime();
	}

	return (stActiveInfo);
}

bool
CLogicTotalHeroTrainActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stTotalHeroTrainConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stTotalHeroTrainConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicTotalHeroTrainActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	// stIter->second->SetActiveTargetNum(m_pUserData->m_stUserInfoTable.GetTotalTrainCount());

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stTotalHeroTrainConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stTotalHeroTrainConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicTurnTableScoreActive::CLogicTurnTableScoreActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicTurnTableScoreActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stTurnTableScoreConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool CLogicTurnTableScoreActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stTurnTableScoreConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stTurnTableScoreConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			for (size_t i = 0; i < stConfig[iEntityID - 1].m_stBonusItemVec.size(); ++i)
			{
				if ((i < stConfig[iEntityID - 1].m_stNotify.size()) && (stConfig[iEntityID - 1].m_stNotify[i] > 0))
				{
					CLogicGlobal::PublishBroadcast("turn_table2", 0, stConfig[iEntityID - 1].m_stNotify[i], {
							{ "nick", m_pUserData->m_strNick },
							{ "item_name", CLogicConfig::GetItemName(stConfig[iEntityID - 1].m_stBonusItemVec[i].m_iItemType, stConfig[iEntityID - 1].m_stBonusItemVec[i].m_iCardID) },
							{ "item_count", std::to_string(stConfig[iEntityID - 1].m_stBonusItemVec[i].m_iNum) },
					});
				}
			}

            // 大奖
            if(stConfig[iEntityID - 1].m_bIsBigPrize)
            {
                // 通用事件
                CLogicEventData stEventData;
                stEventData.m_eType = CLogicConfigDefine::ECET_LuckyTurnTableFinalAward;
                stEventData.m_iCmd = iCmd;
                LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserData, stEventData);
            }

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int
CLogicTurnTableScoreActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stTurnTableScoreConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stTurnTableScoreConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

CLogicLoginInsureActive::CLogicLoginInsureActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID)
{
}

void CLogicLoginInsureActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stLoginInsureMap)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool CLogicLoginInsureActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stLoginInsureMap.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stLoginInsureMap.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int CLogicLoginInsureActive::UpdateActiveInfo(int iNum)
{
	// if (m_pUserData->m_stUserInfoTable.GetLoginInsureOpenTime() <= 0)
	{
		return 0;
	}

	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stLoginInsureMap.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stLoginInsureMap.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

CLogicHeroInsureActive::CLogicHeroInsureActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID)
{
}

void CLogicHeroInsureActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stHeroInsureMap)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool CLogicHeroInsureActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroInsureMap.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeroInsureMap.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int CLogicHeroInsureActive::UpdateActiveInfo(int iNum)
{
	// if (m_pUserData->m_stUserInfoTable.GetHeroInsureOpenTime() <= 0)
	{
		return 0;
	}

	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->SetActiveTargetNum(0);
	CLogicCacheCardOperation stOperation(m_pUserData);
	for (auto iterHero = m_pUserData->m_stHeroCardTableMap.Begin(); iterHero != m_pUserData->m_stHeroCardTableMap.End(); ++iterHero)
	{
		if (stOperation.CheckHeroCardExist(iterHero->second))
		{
			stIter->second->AddActiveTargetNum(1);
		}
	}

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeroInsureMap.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeroInsureMap.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

CLogicHomeInsureActive::CLogicHomeInsureActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID)
{
}

void CLogicHomeInsureActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stHomeInsureMap)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool CLogicHomeInsureActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHomeInsureMap.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHomeInsureMap.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int CLogicHomeInsureActive::UpdateActiveInfo(int iNum)
{
	// if (m_pUserData->m_stUserInfoTable.GetHomeInsureOpenTime() <= 0)
	{
		return 0;
	}

	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	// CPackHomeInfo stHome = CLogicCacheCommon::GetHomeInfo(m_pUserData);
	// stIter->second->SetActiveTargetNum(stHome.m_iEnvExpTotal);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHomeInsureMap.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHomeInsureMap.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicWishCharacterActive::CLogicWishCharacterActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicWishCharacterActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stWishCharacterConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool CLogicWishCharacterActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stWishCharacterConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stWishCharacterConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int CLogicWishCharacterActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stWishCharacterConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stWishCharacterConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicFlowerLanternActive::CLogicFlowerLanternActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void CLogicFlowerLanternActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stFlowerLanterConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

void CLogicFlowerLanternActive::AddCandle(user_data_table_ptr_type pUserData, int32_t iEvent, int32_t iEventValue)
{
	for (const auto& info : CLogicConfig::GetActivityConfig().m_stFlowerLanterInfoConfig)
	{
		int iActivityID = info.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			const int32_t iAddCandle = AddCandleLog(pUserData, info.second, iActivityID, iEvent, iEventValue);
			const int32_t iRedPoint = pActiveObjPtr->UpdateActiveInfo(iAddCandle);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

int32_t CLogicFlowerLanternActive::AddCandleLog(user_data_table_ptr_type pUserData, const TLogicActiveFlowerLanternInfoElem& rstInfo, int32_t iActivityID, int32_t iEvent, int32_t iEventValue)
{
	int32_t iMaxValue = 0, iMaxCandle = 0, iAddCandle = 0, iUnitValue = 0, iUnitValueCandle = 0;
	for (auto iter = pUserData->m_stFlowerLanternMap.Begin(); iter != pUserData->m_stFlowerLanternMap.End(); ++iter)
	{
		if (iter->second->GetActivityID() == iActivityID && iter->second->GetEvent() == iEvent)
		{
			iMaxValue = std::max(iMaxValue, iter->second->GetTotalValue());
			iMaxCandle = std::max(iMaxCandle, iter->second->GetTotalCandle());
		}
	}

	if (iEvent == CLogicConfigDefine::FLOWER_LANTERN_EVENT_LOGIN)
	{
		iUnitValue = rstInfo.m_iDailyLoginCandle;
		iUnitValueCandle = 1;
	}
	else if (iEvent == CLogicConfigDefine::FLOWER_LANTERN_EVENT_PAY)
	{
		iUnitValue = rstInfo.m_iUnitRecharge;
		iUnitValueCandle = rstInfo.m_iUnitRechargeCandle;
	}

	if (iUnitValue > 0 && iUnitValueCandle > 0)
	{
		iAddCandle = (iMaxValue + iEventValue) / iUnitValue * iUnitValueCandle - iMaxCandle;
		TFlowerLanternTableValueType stVal;
		stVal.m_iUid = pUserData->m_iUin;
		stVal.m_iGroupID = pUserData->m_iGroupID;
		stVal.m_iLogIndex = pUserData->m_stUserInfoTable.GetPrimaryKeyIndex();
		stVal.m_iLogTime = CLogicCommon::GetSec();
		stVal.m_iActivityID = iActivityID;
		stVal.m_iEvent = iEvent;
		stVal.m_iEventValue = iEventValue;
		stVal.m_iCandle = iAddCandle;
		stVal.m_iTotalValue = iMaxValue + iEventValue;
		stVal.m_iTotalCandle = iMaxCandle + iAddCandle;
		pUserData->m_stFlowerLanternMap.Insert(std::make_pair(stVal.m_iLogIndex, stVal));

		pUserData->m_stUserInfoTable.AddPrimaryKeyIndex(1);

		if (iAddCandle > 0)
		{
			CLogicUserNotifySystem::NotifyCandleAdd(pUserData);
		}
	}
	return iAddCandle;
}


bool CLogicFlowerLanternActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stFlowerLanterConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stFlowerLanterConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			Notify(m_pUserData, -stConfig[iEntityID - 1].m_iCount);

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int CLogicFlowerLanternActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	static std::vector<int32_t> tower = {
		11, 11, 11, 11, 12, 12, 12, 13, 13, 14,
		21, 21, 21, 21, 22, 22, 22, 23, 23, 24,
		31, 31, 31, 31, 32, 32, 32, 33, 33, 34,
		41, 41, 41, 41, 42, 42, 42, 43, 43, 44,
		51, 51, 51, 51, 52, 52, 52, 53, 53, 54,
	};
	int32_t tower_index = -1;

	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = ulCompletedBitmap;
	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stFlowerLanterConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stFlowerLanterConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size() && uiIndex < tower.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			//该层还有蜡烛未领取
			if (tower_index > 0 && tower_index != tower[uiIndex])
			{
				break;
			}

			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				tower_index = tower[uiIndex];
				//未领取，蜡烛达到条件
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
			}
			else if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap))
			{
				tower_index = tower[uiIndex];
				//未领取,蜡烛不够了
				if (stConfigIter->second[uiIndex].m_iCount > iTargetNum)
				{
					ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);
				}
			}
		}
	}

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

//////////////////////////////////////////////////////////////////////////////////////////
CLogicPetInsureActive::CLogicPetInsureActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID)
{
}

void CLogicPetInsureActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stPetInsureMap)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool CLogicPetInsureActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPetInsureMap.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stPetInsureMap.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int CLogicPetInsureActive::UpdateActiveInfo(int iNum)
{
	// if (m_pUserData->m_stUserInfoTable.GetPetInsureOpenTime() <= 0)
	{
		return 0;
	}

	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stPetInsureMap.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stPetInsureMap.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicHeartInsureActive::CLogicHeartInsureActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID)
{
}

void CLogicHeartInsureActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    for (const auto& config : CLogicConfig::GetActivityConfig().m_stHeartInsureMap)
    {
        int iActivityID = config.first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool CLogicHeartInsureActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeartInsureMap.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stHeartInsureMap.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int CLogicHeartInsureActive::UpdateActiveInfo(int iNum)
{
    if (m_pUserData->m_stIBusyData.GetHeartInsureOpenTime() <= 0)
    {
        return 0;
    }

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    std::vector<int32_t> stHeartLevelNum;
    for(auto iter = m_pUserData->m_stHeroCardTableMap.Begin(); iter != m_pUserData->m_stHeroCardTableMap.End(); ++iter)
    {
        if(iter->second->GetCardLevel() > 0)
        {
            if(iter->second->GetHeartLevel() >= int(stHeartLevelNum.size()))
                stHeartLevelNum.resize(iter->second->GetHeartLevel() + 1);

            for(int i = 1; i <= iter->second->GetHeartLevel(); ++i)
                ++stHeartLevelNum[i];
        }
    }

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap = 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stHeartInsureMap.find(m_iActivityID);
    if (stConfigIter != CLogicConfig::GetActivityConfig().m_stHeartInsureMap.end())
    {
        for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
        {
            if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
                (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                auto& stConfig = stConfigIter->second[uiIndex];

                if(stConfig.m_iHeartLevel >= int(stHeartLevelNum.size()))
                    break;

                if (stConfig.m_iCount <= stHeartLevelNum[stConfig.m_iHeartLevel])
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
            }
        }
    }

    ulCompleteBitmap |= ulCompletedBitmap;

    stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

    return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicSendFlowerActive::CLogicSendFlowerActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicSendFlowerActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stSendFlowerConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool CLogicSendFlowerActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stSendFlowerConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stSendFlowerConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int CLogicSendFlowerActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stSendFlowerConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stSendFlowerConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}

////////////////////////////////////////////////////////////////////////////////
CLogicLuckyHammerActive::CLogicLuckyHammerActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID) {}

void
CLogicLuckyHammerActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
	for (const auto& config : CLogicConfig::GetActivityConfig().m_stLuckyHammerConfig)
	{
		int iActivityID = config.first;
		auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
		if (pActiveObjPtr.get())
		{
			int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
			if (iRedPoint > 0)
			{
				LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
			}
		}
	}
}

bool CLogicLuckyHammerActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stLuckyHammerConfig.find(m_iActivityID);
	if (stConfigIter == CLogicConfig::GetActivityConfig().m_stLuckyHammerConfig.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	const auto& stConfig = stConfigIter->second;

	if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
		return (false);
	}

	uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

	if ((ulBitmapMask & ulReceivedBitmap) == 0)
	{
		if ((ulBitmapMask & ulCompleteBitmap))
		{
			ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
			ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

			stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
			stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

			stIter->second->SetActiveTime(CLogicCommon::GetSec());

			stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

			return (true);
		}
		else
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
			return (false);
		}
	}
	else
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
		return (false);
	}
	return true;
}

int CLogicLuckyHammerActive::UpdateActiveInfo(int iNum)
{
	auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
	if (stIter == m_pUserData->m_stUserActiveMap.End())
	{
		TActiveTableValueType stVal;
		stVal.m_iActiveID = m_iActivityID;
		stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
		stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

		auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
		stIter = stRet.first;
	}

	stIter->second->AddActiveTargetNum(iNum);

	bool bHasRedPoint = false;
	uint64_t ulBitmapMask = 0x1;
	uint64_t ulCompleteBitmap = 0x0;
	uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
	uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
	int iTargetNum = stIter->second->GetActiveTargetNum();

	auto stConfigIter = CLogicConfig::GetActivityConfig().m_stLuckyHammerConfig.find(m_iActivityID);
	if (stConfigIter != CLogicConfig::GetActivityConfig().m_stLuckyHammerConfig.end())
	{
		for (size_t uiIndex = 0; uiIndex < stConfigIter->second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
		{
			if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
				(ulBitmapMask & ulCompletedBitmap) == 0)
			{
				if (stConfigIter->second[uiIndex].m_iCount <= iTargetNum)
				{
					bHasRedPoint = true;

					ulCompleteBitmap |= ulBitmapMask;
				}
				else
				{
					// 线性任务, 减少判断次数
					break;
				}
			}
		}
	}

	ulCompleteBitmap |= ulCompletedBitmap;

	stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

	return (bHasRedPoint);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
CLogicLoginFundActive::CLogicLoginFundActive(user_data_table_ptr_type pUserData, int iActivityID) : CLogicActiveBase(pUserData, iActivityID)
{
}

void CLogicLoginFundActive::Notify(user_data_table_ptr_type pUserData, int iNum)
{
    for (const auto& config : CLogicConfig::GetActivityConfig().m_stLoginFundMap)
    {
        int iActivityID = config.first;
        auto pActiveObjPtr = CLogicActiveBase::CreateActiveObj(pUserData, iActivityID);
        if (pActiveObjPtr.get())
        {
            int iRedPoint = pActiveObjPtr->UpdateActiveInfo(iNum);
            if (iRedPoint > 0)
            {
                LOGIC_ACTIVITY_CHANGE_LIST_INS->GetActivityChangeMap()[iActivityID] = iRedPoint;
            }
        }
    }
}

bool CLogicLoginFundActive::GetActiveGift(int iEntityID, std::vector<CPackGameItem>& stBonusList, int32_t iCmd, int32_t iReceiveTimes/* = 1*/)
{
    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    auto stConfigIter = CLogicConfig::GetActivityConfig().m_stLoginFundMap.find(m_iActivityID);
    if (stConfigIter == CLogicConfig::GetActivityConfig().m_stLoginFundMap.end())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    const auto& stConfig = stConfigIter->second.second;

    if (iEntityID > (int)stConfig.size() || iEntityID <= 0)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_IS_NOT_IN_CONFIG);
        return (false);
    }

    uint64_t ulBitmapMask = ((uint64_t)0x1 << (iEntityID - 1));
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompleteBitmap = stIter->second->GetActiveCompleteBitmap();

    if ((ulBitmapMask & ulReceivedBitmap) == 0)
    {
        if ((ulBitmapMask & ulCompleteBitmap))
        {
            ulReceivedBitmap = ulBitmapMask | ulReceivedBitmap;
            ulCompleteBitmap = ulCompleteBitmap & (~ulBitmapMask);

            stIter->second->SetActiveReceivedBitmap(ulReceivedBitmap);
            stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);

            stIter->second->SetActiveTime(CLogicCommon::GetSec());

            stBonusList = stConfig[iEntityID - 1].m_stBonusItemVec;

            return (true);
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(ACTIVE_CONDITION_IS_NOT_REACHED);
            return (false);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(ACTIVE_BONUS_IS_RECEIVED);
        return (false);
    }
    return true;
}

int CLogicLoginFundActive::UpdateActiveInfo(int iNum)
{
    const std::map<int32_t, int32_t> login_fund_open_map = {
            // {CLogicConfigDefine::LOGIN_FUND_1_CARD_ID, m_pUserData->m_stUserInfoTable.GetLoginFund1OpenTime()},
            // {CLogicConfigDefine::LOGIN_FUND_2_CARD_ID, m_pUserData->m_stUserInfoTable.GetLoginFund2OpenTime()},
            // {CLogicConfigDefine::LOGIN_FUND_3_CARD_ID, m_pUserData->m_stUserInfoTable.GetLoginFund3OpenTime()},
            // {CLogicConfigDefine::LOGIN_FUND_4_CARD_ID, m_pUserData->m_stUserInfoTable.GetLoginFund4OpenTime()},
            // {CLogicConfigDefine::LOGIN_FUND_5_CARD_ID, m_pUserData->m_stUserInfoTable.GetLoginFund5OpenTime()},
    };

    const auto iterActiveConfig = CLogicConfig::GetActivityConfig().m_stLoginFundMap.find(m_iActivityID);
    if (iterActiveConfig == CLogicConfig::GetActivityConfig().m_stLoginFundMap.end())
    {
        return 0;
    }

    const auto* pstDepositConfig = CLogicConfig::GetDepositConfig().GetDepositConfig(iterActiveConfig->second.first);
    if (pstDepositConfig == nullptr)
    {
        return 0;
    }

    const auto iterLoginFund = login_fund_open_map.find(pstDepositConfig->m_iBuyItemID);
    if (iterLoginFund == login_fund_open_map.end() || iterLoginFund->second <= 0)
    {
        return 0;
    }

    auto stIter = m_pUserData->m_stUserActiveMap.Find(m_iActivityID);
    if (stIter == m_pUserData->m_stUserActiveMap.End())
    {
        TActiveTableValueType stVal;
        stVal.m_iActiveID = m_iActivityID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;

        auto stRet = m_pUserData->m_stUserActiveMap.Insert(std::make_pair(stVal.m_iActiveID, stVal));
        stIter = stRet.first;
    }

    bool bHasRedPoint = false;
    uint64_t ulBitmapMask = 0x1;
    uint64_t ulCompleteBitmap = 0x0;
    uint64_t ulReceivedBitmap = stIter->second->GetActiveReceivedBitmap();
    uint64_t ulCompletedBitmap = stIter->second->GetActiveCompleteBitmap();
    if (ulCompletedBitmap == 0)
    {
        stIter->second->AddActiveTargetNum(1);

        int iTargetNum = stIter->second->GetActiveTargetNum();

        for (size_t uiIndex = 0; uiIndex < iterActiveConfig->second.second.size(); uiIndex++, ulBitmapMask = ulBitmapMask << 1)
        {
            if ((ulBitmapMask & ulReceivedBitmap) == 0 &&
                (ulBitmapMask & ulCompletedBitmap) == 0)
            {
                if (iterActiveConfig->second.second[uiIndex].m_iCount <= iTargetNum)
                {
                    bHasRedPoint = true;

                    ulCompleteBitmap |= ulBitmapMask;
                }
                else
                {
                    // 线性任务, 减少判断次数
                    break;
                }
            }
        }

        ulCompleteBitmap |= ulCompletedBitmap;

        stIter->second->SetActiveCompleteBitmap(ulCompleteBitmap);
    }

    return (bHasRedPoint);
}




