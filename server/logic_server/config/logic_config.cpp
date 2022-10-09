#include "logic_config.h"
#include "common_utf8handle.h"
#include "server_errcode.h"
#include "logic_link_define.h"
#include "logic_link_common.h"

///////////////////////////////////////////////////////////////////////////////////////////////
void CLogicConfig::ParseWeekDay(const std::string& strWeekDay, char& refWeekDayArray)
{
    refWeekDayArray = 0;

    bool bIsSet = false;
    if(strWeekDay.size() >= 3)
    {            
        std::string strTmp = strWeekDay.substr(1, strWeekDay.size() - 2);
        std::vector<unsigned int> stData = taf::TC_Common::sepstr<unsigned int>(strTmp, ",");

        for(std::vector<unsigned int>::const_iterator stIT = stData.begin(); 
                stIT != stData.end(); stIT ++)
        {
            if((*stIT >= 1) &&(*stIT <= 7))
            {
                if(*stIT == 7)
                {
                    refWeekDayArray |= 1;
                }
                else
                {
                    refWeekDayArray |= (1 << (*stIT));
                }

                bIsSet = true;
            }
        }
    }

    if(bIsSet == false)
    {
        refWeekDayArray = 0xff;
    }
}

void CLogicConfig::ParseDay(const std::string& strOpenDay, int& iStartDayTimestamp, int& iEndDayTimestamp)
{
    iStartDayTimestamp = 0;
    iEndDayTimestamp = 0x7FFFFFFF;

    if(strOpenDay.size() >= 3)
    {
        std::string strTmp = strOpenDay.substr(1, strOpenDay.size() - 2);
        std::vector<std::string> stData = taf::TC_Common::sepstr<std::string>(strTmp, ",");
        if(stData.size() == 2)
        {
            std::string strStartDay = stData[0];
            std::string strEndDay = stData[1];

            if(strStartDay.empty() == false)
            {
                struct tm stTM;
                memset(&stTM, 0, sizeof(stTM));
                if(strptime(strStartDay.c_str(), "%F", &stTM) != nullptr)
                {
                    iStartDayTimestamp = (int)mktime(&stTM);
                }
            }

            if(strEndDay.empty() == false)
            {
                struct tm stTM;
                memset(&stTM, 0, sizeof(stTM));
                if(strptime(strEndDay.c_str(), "%F", &stTM) != nullptr)
                {
                    stTM.tm_hour = 23;
                    stTM.tm_min = 59;
                    stTM.tm_sec = 59;
                    iEndDayTimestamp = (int)mktime(&stTM);
                }
            }
        }
    }
}

void CLogicConfig::ParseHour(const std::string& strHour, int& iStartHour, int& iStartMinute,
                               int& iStartSecond, int& iEndHour, int& iEndMinute, int& iEndSecond)
{
    iStartHour = 0;iStartMinute = 0;iStartSecond = 0;
    iEndHour = 23;iEndMinute = 59;iEndSecond = 59;

    if(strHour.size() >= 3)
    {
        std::string strTmp = strHour.substr(1, strHour.size() - 2);
        std::vector<std::string> stData = taf::TC_Common::sepstr<std::string>(strTmp, ",");
        if(stData.size() == 2)
        {
            std::string strStartTime = stData[0];
            std::string strEndTime = stData[1];

            if(strStartTime.empty() == false)
            {
                struct tm stTM;
                if(strptime(strStartTime.c_str(), "%T", &stTM) != nullptr)
                {
                    iStartHour = stTM.tm_hour;
                    iStartMinute = stTM.tm_min;
                    iStartSecond = stTM.tm_sec;
                }
            }

            if(strEndTime.empty() == false)
            {
                struct tm stTM;
                if(strptime(strEndTime.c_str(), "%T", &stTM) != nullptr)
                {
                    iEndHour = stTM.tm_hour;
                    iEndMinute = stTM.tm_min;
                    iEndSecond = stTM.tm_sec;
                }
            }
        }
    }
}

int CLogicConfig::GetChapterTypeByLevelID(int iLevelID)
{
    if ( iLevelID < CLogicConfigDefine::NORMAL_CHAPTER_LIMIT )
        return CLogicConfigDefine::NORMAL_CHAPTER;
    else if ( iLevelID < CLogicConfigDefine::ELITE_CHAPTER_LIMIT )
        return CLogicConfigDefine::ELITE_CHAPTER;
    else if ( iLevelID < CLogicConfigDefine::DEVIL_CHAPTER_LIMIT )
        return CLogicConfigDefine::DEVIL_CHAPTER;
	else if (iLevelID < CLogicConfigDefine::EVENT_CHAPTER_LIMIT)
		return CLogicConfigDefine::EVENT_CHAPTER;
	else if (iLevelID < CLogicConfigDefine::TOWER_CHAPTER_LIMIT)
		return CLogicConfigDefine::TOWER_CHAPTER;	
	else if (iLevelID < CLogicConfigDefine::TERRITORY_CHAPTER_LIMIT)
		return CLogicConfigDefine::TERRITORY_CHAPTER;
	else if (iLevelID < CLogicConfigDefine::GUILD_CHAPTER_LIMIT)
		return CLogicConfigDefine::GUILD_CHAPTER;
    else
        return CLogicConfigDefine::UNLIMITED_CHAPTER;
}

int CLogicConfig::GetChapterIDByLevelID(int iLevelID)
{
    int iBase = 0;
    if ( iLevelID < CLogicConfigDefine::NORMAL_CHAPTER_LIMIT )
        iBase = CLogicConfigDefine::NORMAL_CHAPTER_BASE;
    else if ( iLevelID < CLogicConfigDefine::ELITE_CHAPTER_LIMIT )
        iBase = CLogicConfigDefine::ELITE_CHAPTER_BASE;
    else if ( iLevelID < CLogicConfigDefine::DEVIL_CHAPTER_LIMIT )
        iBase = CLogicConfigDefine::DEVIL_CHAPTER_BASE;
    else if ( iLevelID < CLogicConfigDefine::EVENT_CHAPTER_LIMIT )
        iBase = CLogicConfigDefine::EVENT_CHAPTER_BASE;
    return ((iLevelID % 10000) / 20 + 1 + iBase);
}

int CLogicConfig::GetChapterTypeByChapterID(int iChapterID)
{
   return (iChapterID / 1000 + 1);
}

std::string CLogicConfig::GetItemName(int32_t iItemType, int32_t iItemID)
{
	std::string strItemName("");

    switch (iItemType)
    {
    case CLogicConfigDefine::HERO_CARD:
    {
        auto pConfigItem = CLogicConfig::GetHeroCardConfig().GetConfig(iItemID);
        if (pConfigItem) return pConfigItem->m_strHeroCardName;
        break;
    }
    case CLogicConfigDefine::CURRENCY:
    {
        auto pConfig = CLogicConfig::GetCurrencyConfig().GetConfig(iItemID);
        if(pConfig) return pConfig->m_strName;
        break;
    }
    case CLogicConfigDefine::FASHION:
    {
        auto pConfig = CLogicConfig::GetFashionConfig().GetFashionConfig(iItemID);
        if(pConfig) return pConfig->m_strName;
        break;
    }
    case CLogicConfigDefine::AVATAR:
    {
        break;
    }
    case CLogicConfigDefine::EXCHANGE_ITEM:
    {
        auto pConfig = CLogicConfig::GetItemConfig().GetConfig(iItemID);
        if(pConfig) return pConfig->m_strName;
        break;
    }
    case CLogicConfigDefine::EQUIPMENT:
    {
        break;
    }
    case CLogicConfigDefine::CONSTELLATION:
    {
        break;
    }
    default:
        break;
    }

	return strItemName;
}

std::string CLogicConfig::GenRewardContent(const vector<CPackGameItem> &items)
{
    string content;

    map<tuple<int, int>, int> sameItems;
    for (auto &iter : items)
    {
        sameItems[make_tuple(iter.m_iItemType, iter.m_iCardID)] += iter.m_iNum;
    }

    for (auto &iter : sameItems)
    {
        if (!content.empty())
        {
            content += (",");
        }

        content += "[" + CLogicConfig::GetItemName(get<0>(iter.first), get<1>(iter.first)) + "]";
        content += " * " + to_string(iter.second);
    }

    return content;
}

bool CLogicConfig::IsValidGameItem(int32_t iType, int32_t iItemID, int32_t iNum)
{
    if(iNum <= 0) return false;

    switch (iType)
    {
    case CLogicConfigDefine::HERO_CARD:
        return CLogicConfig::GetHeroCardConfig().GetConfig(iItemID) != nullptr;
    case CLogicConfigDefine::CURRENCY:
        return CLogicConfig::GetCurrencyConfig().GetConfig(iItemID) != nullptr;
    case CLogicConfigDefine::FASHION:
        return CLogicConfig::GetFashionConfig().GetFashionConfig(iItemID) != nullptr;
    case CLogicConfigDefine::AVATAR:
        return CLogicConfig::GetAvatarConfig().GetConfig(iItemID) != nullptr;
    case CLogicConfigDefine::EXCHANGE_ITEM:
        return CLogicConfig::GetItemConfig().GetConfig(iItemID) != nullptr;
    case CLogicConfigDefine::EQUIPMENT:
        return CLogicConfig::GetEquipItemConfig().GetConfig(iItemID) != nullptr;
    case CLogicConfigDefine::CONSTELLATION:
        return CLogicConfig::GetConstelConfig().GetConfig(iItemID) != nullptr;
    default:
        return false;
    }
}

bool CLogicConfig::IsValidGameItem(int32_t iType, int32_t iItemID)
{
    return IsValidGameItem(iType, iItemID, 1);
}

bool CLogicConfig::IsValidGameItem(const CPackGameItem& stItem)
{
    return IsValidGameItem(stItem.m_iItemType, stItem.m_iCardID, stItem.m_iNum);
}

CLogicConfig* CLogicConfig::m_pConfig = nullptr;

CLogicConfig::config_loader_t CLogicConfig::CONFIG_LOADER_LIST[] =
{
    { "logic_config",                   CLogicConfig::GetSystemConfigParser                 },
	{ "zoo_logic_platform",             CLogicConfig::GetZooLogicPlatformConfigParser       },
	{ "zoo_logic_game",                 CLogicConfig::GetZooLogicGameConfigParser           },
	{ "zoo_logic_switch",				CLogicConfig::GetProtoSwitchConfigParser			},
    { "zoo_logic_anti_addiction",				CLogicConfig::GetAntiAddictionConfigParser			},
    { "zoo_logic_holiday_calendar",				CLogicConfig::GetHolidayCalendarConfigParser},
    { "defaultconfig",                  CLogicConfig::GetGlobalConfigParser                 },
    { "no_monster",                     CLogicConfig::GetNoCoinMonsterConfigParser          },
    { "boss",                           CLogicConfig::GetBossConfigParser                   },
    { "buff",                           CLogicConfig::GetBuffConfigParser},
    { "avy_time_config",                CLogicConfig::GetAvyTimeConfigParser},

    { "currency_item",                  CLogicConfig::GetCurrencyConfigParser},
    { "exchange_item",                  CLogicConfig::GetItemConfigParser},
    { "exchange_item_3",                CLogicConfig::GetItemConfigParser},
    { "card",                           CLogicConfig::GetHeroCardConfigParser},
    { "fashion",						 CLogicConfig::GetFashionConfigParser},

    { "equipment_attr",                 CLogicConfig::GetEquipAttrConfigParser},
    { "equipment_attr_rand",            CLogicConfig::GetEquipAttrRandConfigParser},
    { "equipment_item",                 CLogicConfig::GetEquipItemConfigParser},

    { "equipment_constellation_attr",   CLogicConfig::GetConstelAttrConfigParser},
    { "equipment_constellation",        CLogicConfig::GetConstelConfigParser},

    { "cardbag",                        CLogicConfig::GetCardbagConfigParser                },
    { "cardbag_1",                      CLogicConfig::GetCardbagConfigParser                },
    { "level_drop",                     CLogicConfig::GetLevelDropConfigParser              },
    { "level_drop_bag",                 CLogicConfig::GetLevelDropBagConfigParser           },
    { "chapter",                        CLogicConfig::GetChapterConfigParser                },
    { "level",                          CLogicConfig::GetLevelConfigParser                  },
    { "level_1",                        CLogicConfig::GetLevelConfigParser                  },
    { "user_exp",                       CLogicConfig::GetUserExpConfigParser                },
    { "calculator",                     CLogicConfig::GetCalcConfigParser                   },
    { "card_soul",                      CLogicConfig::GetUpgradeHeroCardStarConfigParser    },
    { "card_exp",                       CLogicConfig::GetUpgradeHeroCardLevelConfigParser   },
    { "hero_upgrade_color",             CLogicConfig::GetUpgradeHeroCardColorConfigParser   },
    { "main_line_quest",                CLogicConfig::GetMainTaskConfigParser               },
    { "mall",                           CLogicConfig::GetMallConfigParser                   },
    { "ranklist",                       CLogicConfig::GetRankListBonusConfigParser          },
    { "dirty_words_name_config",        CLogicConfig::GetDirtyWordConfigParser              },
        { "dirty_words_chat_config",    CLogicConfig::GetDirtyWordChatConfigParser          },
    { "pvp_rank_bots",                  CLogicConfig::GetPVPRobotConfigParser               },
    { "pvp_config",                     CLogicConfig::GetPVPConfigParser                    },
    { "pvp_match_rules",                CLogicConfig::GetPVPMatchRuleConfigParser           },
	{ "match_pvp",						CLogicConfig::GetMatchPvpConfigParser				  },
	{ "fair_pvp",						CLogicConfig::GetFairPvpConfigParser				  },
    { "activity_config",                CLogicConfig::GetActivityConfigParser               },
	{ "activity_normal_config",         CLogicConfig::GetActivityConfigParser               },
	{ "activity_serial_pay",            CLogicConfig::GetActivitySerialPayConfigParser	  },
    { "vip_config",                     CLogicConfig::GetVipConfigParser                    },
    { "buy_resource",                   CLogicConfig::GetBuyResourceConfigParser            },
    { "avatar",                         CLogicConfig::GetAvatarConfigParser                 },
    { "vip_unlock",                     CLogicConfig::GetVipUnlockConfigParser              },
    { "mallbag",                        CLogicConfig::GetMallBagConfigParser                },
    { "broadcast",                      CLogicConfig::GetBroadcastConfigParser              },
	{ "activity_broadcast",				CLogicConfig::GetActivityBroadcastConfigParser	  },
	{ "fight_report",					CLogicConfig::GetFightReportConfigParser			  },
    { "first_deposit",                  CLogicConfig::GetFirstDepositConfigParser           },
    { "rankrecord",                     CLogicConfig::GetHighestRecordConfigParser          },
    { "vip_daily_award",                CLogicConfig::GetVipDailyAwardConfigParser          },
    { "vip_bag",                        CLogicConfig::GetVipBagConfigParser                 },
    { "open_server_activity",           CLogicConfig::GetActivityConfigParser               },
    { "fragment_combine",               CLogicConfig::GetFragmentCombineConfigParser        },
    { "cross_fight",                    CLogicConfig::GetCrossFightConfigParser             },
    { "daily_deposit",                  CLogicConfig::GetDailyDepositConfigParser           },
    { "fight_power_rank_active",        CLogicConfig::GetFightPowerRankActiveConfigParser   },
    { "theme_activity_task",            CLogicConfig::GetActivityConfigParser               },
    { "theme_activity_exchange",        CLogicConfig::GetActivityConfigParser               },
	{ "climb_tower",					CLogicConfig::GetClimbTowerConfigParser			  },
	{ "guild",							CLogicConfig::GetGuildConfigParser				  },
	{ "guild_war",						CLogicConfig::GetGuildWarConfigParser			      },
	{ "deposit",					    CLogicConfig::GetDepositConfigParser                },
	{ "mail",							CLogicConfig::GetMailConfigParser                   },
	{ "shop",							CLogicConfig::GetShopConfigParser                   },
	{ "bank_price",						CLogicConfig::GetBankPriceConfigParser              },
	{ "award",							CLogicConfig::GetAwardConfigParser                  },
	{ "activity_multi_task",			CLogicConfig::GetActivityMultiTaskConfigParser      },
	{ "honor",					    	CLogicConfig::GetHonorConfigParser				  },
	{ "activity_messy_task",			CLogicConfig::GetActivityTaskConfigParser           },
	{ "merge_activity",					CLogicConfig::GetMergeActivityConfigParser },
	{ "entire_war",						CLogicConfig::GetEntireWarConfigParser },
	{ "question",						CLogicConfig::GetQuestionConfigParser },
	{ "friend",							CLogicConfig::GetFriendConfigParser },
	{ "jubao",							CLogicConfig::GetJubaoConfigParser },
	{ "qiyu",							CLogicConfig::GetQiyuConfigParser },
	{ "qiyu_bots",						CLogicConfig::GetQiyuBotConfigParser },
	{ "theme_activity_charge",			CLogicConfig::GetThemeActivityChargeConfigParser },
	{ "ibusy",							CLogicConfig::GetIBusyConfigParser },
	{ "scorelevel_activity",			CLogicConfig::GetScoreLevelConfigParser },
	{ "day_single_recharge_activity",	CLogicConfig::GetDaySingleRechargeConfigParser 		},
	{ "protocol_monitor",				CLogicConfig::GetProtocolMonitorConfigParser 		},
	{ "item_monitor",					CLogicConfig::GetItemMonitorConfigParser			},
    { "activity_special_gift",			CLogicConfig::GetSpecialGiftConfigParser			},
    { "month_pass",                     CLogicConfig::GetMonthPassConfigParser			    },
    { "hero_heart",                     CLogicConfig::GetHeroHeartConfigParser			    },
    { "card_dialog",                     CLogicConfig::GetCardDialogConfigParser			    },
	{ "wenwen_news",                     CLogicConfig::GetWenWenNewsConfigParser			    },
    { "money_tree_exp",                  CLogicConfig::GetMoneyTreeExpConfigParser			    },
    { "random_store",                    CLogicConfig::GetRandomStoreConfigParser},
    { "heart_store_dialog_discount",     CLogicConfig::GetHeartStoreDiscountConfigParser},
    { "hero_dating",                    CLogicConfig::GetHeroDatingConfigParser},
    { "hero_date_question",             CLogicConfig::GetHeroDatingDialogConfigParser},
    { "simple_game",                    CLogicConfig::GetSimpleGameConfigParser},
    { "red_bag",                        CLogicConfig::GetRedBagConfigParser},
    { "world_quest",                    CLogicConfig::GetWorldQuestConfigParser},
        { "rune",                    CLogicConfig::GetRuneConfigParser},
    { "questionnaire",                  CLogicConfig::GetQuestionnaireConfigParser},
    { "rank_config",                    CLogicConfig::GetRankConfigParser},
    { "entire_war_map",                 CLogicConfig::GetEntireWarMapConfigParser},
    { "reward_bag",                     CLogicConfig::GetRewardBagConfigParser},
    { "entire_war_limit_boss",          CLogicConfig::GetEntireWarLimitBossConfigParser},
    { "zoo_queue_config",               CLogicConfig::GetZooQueueConfigParser},
    { "constellation_level_drop",       CLogicConfig::GetConstelLevelDropConfigParser},
    { "home_goods",                     CLogicConfig::GetHomeGoodsConfigParser},
    { "home_func",                      CLogicConfig::GetHomeFuncConfigParser},
    { "home_garden_explore",            CLogicConfig::GetHomeGardenExploreConfigParser},
        { "home_level",                 CLogicConfig::GetHomeLevelExpConfigParser},
    { "quest_list",                     CLogicConfig::GetQuestConfigParser},
    { "quest_list_mult",                CLogicConfig::GetQuestConfigParser},
    { "gift_bag",                       CLogicConfig::GetGiftBagConfigParser},
    { "avy_score_award_config",         CLogicConfig::GetAvyScoreAwardConfigParser},
    { "zhuhai_level",                   CLogicConfig::GetZhuHaiConfigParser},
    { "sign",                           CLogicConfig::GetSignConfigParser},
    { "badge",                          CLogicConfig::GetBadgeConfigParser},
        { "new_pass",                   CLogicConfig::GetNewPassConfigParser},
        { "limit_boss",                 CLogicConfig::GetLimitBossConfigParser},
        { "double_reward",              CLogicConfig::GetDoubleRewardConfigParser},
        { "register_reward",            CLogicConfig::GetRegisterRewardConfigParser},
        { "gashapon_activity",          CLogicConfig::GetGashaponConfigParser},
        { "complete_atlas",             CLogicConfig::GetCompleteAtlasConfigParser},

};

size_t CLogicConfig::s_iConfigSize = sizeof(CLogicConfig::CONFIG_LOADER_LIST) / sizeof(CLogicConfig::config_loader_t);

