#include "logic_record_user_ibusy_info_table.h"

#include <cassert>

const int CRecordUserIBusyInfoTable::TABLE_DIV = 32;

const char* CRecordUserIBusyInfoTable::s_strActivityID = "activity_id";
const char* CRecordUserIBusyInfoTable::s_strLevel = "level";
const char* CRecordUserIBusyInfoTable::s_strExp = "exp";
const char* CRecordUserIBusyInfoTable::s_strDailyExpAward = "daily_exp_award";
const char* CRecordUserIBusyInfoTable::s_strScore = "score";
const char* CRecordUserIBusyInfoTable::s_strExploreExp = "explore_exp";
const char* CRecordUserIBusyInfoTable::s_strTodayHuoyue = "today_huoyue";
const char* CRecordUserIBusyInfoTable::s_strChangeTaskCount = "change_task_count";
const char* CRecordUserIBusyInfoTable::s_strFreeChangeTaskCount = "free_change_task_count";
const char* CRecordUserIBusyInfoTable::s_strMarketLastRefreshTime = "market_last_refresh_time";
const char* CRecordUserIBusyInfoTable::s_strMarketRefreshTimes = "market_refresh_times";
const char* CRecordUserIBusyInfoTable::s_strGettedLevelReward = "getted_level_reward";
const char* CRecordUserIBusyInfoTable::s_strGettedDepositReward = "getted_deposit_reward";
const char* CRecordUserIBusyInfoTable::s_strTaskInfo = "task_info";
const char* CRecordUserIBusyInfoTable::s_strSpecialGiftWeekID = "special_gift_week_id";
const char* CRecordUserIBusyInfoTable::s_strSpecialGiftWeekLevel = "special_gift_week_level";
const char* CRecordUserIBusyInfoTable::s_strSpecialGiftMonthID = "special_gift_month_id";
const char* CRecordUserIBusyInfoTable::s_strSpecialGiftMonthLevel = "special_gift_month_level";
const char* CRecordUserIBusyInfoTable::s_strLuckyHammerActivityID = "lucky_hammer_activity_id";
const char* CRecordUserIBusyInfoTable::s_strLuckyHammerLotteryCount = "lucky_hammer_lottery_count";
const char* CRecordUserIBusyInfoTable::s_strMonthPassExp = "month_pass_exp";
const char* CRecordUserIBusyInfoTable::s_strMonthPassDay = "month_pass_day";
const char* CRecordUserIBusyInfoTable::s_strMonthPassRecharge = "month_pass_recharge";
const char* CRecordUserIBusyInfoTable::s_strMonthPassAwardBitmap = "month_pass_award_bitmap";
const char* CRecordUserIBusyInfoTable::s_strMonthPassDepositAwardBitmap = "month_pass_deposit_award_bitmap";
const char* CRecordUserIBusyInfoTable::s_strMonthPassWishEventID = "month_pass_wish_event_id";
const char* CRecordUserIBusyInfoTable::s_strMonthPassWishItemIndex = "month_pass_wish_item_index";
const char* CRecordUserIBusyInfoTable::s_strCardDialogLimitAvyStat = "card_dialog_limit_avy_stat";
const char* CRecordUserIBusyInfoTable::s_strCardDialogNextTime = "card_dialog_next_time";
const char* CRecordUserIBusyInfoTable::s_strCardDialogTimes = "card_dialog_times";
const char* CRecordUserIBusyInfoTable::s_strNewsLastTime = "news_last_time";
const char* CRecordUserIBusyInfoTable::s_strNewsLastData = "news_last_data";
const char* CRecordUserIBusyInfoTable::s_strAddupDepositBonus = "addup_deposit_bonus";
const char* CRecordUserIBusyInfoTable::s_strFoundationOpenTime = "foundation_open_time";
const char* CRecordUserIBusyInfoTable::s_strOnceEventStat = "once_event_stat";
const char* CRecordUserIBusyInfoTable::s_strRandomStoreRefreshData = "random_store_refresh_data";
const char* CRecordUserIBusyInfoTable::s_strSimpleGamePoint = "simple_game_point";
const char* CRecordUserIBusyInfoTable::s_strHeartInsureOpenTime = "heart_insure_open_time";
const char* CRecordUserIBusyInfoTable::s_strMoonMapSkillLevel = "moon_map_skill_level";
const char* CRecordUserIBusyInfoTable::s_strRedBagUniqueIndex = "red_bag_unique_index";
const char* CRecordUserIBusyInfoTable::s_strWorldQuestRewardState = "world_quest_reward_state";
const char* CRecordUserIBusyInfoTable::s_strWorldQuestRewardTimes = "world_quest_reward_times";
const char* CRecordUserIBusyInfoTable::s_strQuestionnaireState = "questionnaire_state";
const char* CRecordUserIBusyInfoTable::s_strQuestionnaireDurationTime = "questionnaire_duration_time";
const char* CRecordUserIBusyInfoTable::s_strLimitBossAccepted = "limit_boss_accepted";
const char* CRecordUserIBusyInfoTable::s_strLimitBossRand = "limit_boss_rand";
const char* CRecordUserIBusyInfoTable::s_strLimitBoxTime = "limit_box_time";
const char* CRecordUserIBusyInfoTable::s_strTotalHeartLevel = "total_heart_level";
const char* CRecordUserIBusyInfoTable::s_strTotalHeartExp = "total_heart_exp";
const char* CRecordUserIBusyInfoTable::s_strConstelExpPool = "constel_exp_pool";
const char* CRecordUserIBusyInfoTable::s_strFishingRod = "fishing_rod";
const char* CRecordUserIBusyInfoTable::s_strMainBgm = "main_bgm";
const char* CRecordUserIBusyInfoTable::s_strHomeBgm = "home_bgm";
const char* CRecordUserIBusyInfoTable::s_strMallBaodiTimes = "mall_baodi_times";
const char* CRecordUserIBusyInfoTable::s_strZhuHaiLevel = "zhu_hai_level";
const char* CRecordUserIBusyInfoTable::s_strZhuHaiGress = "zhu_hai_gress";
const char* CRecordUserIBusyInfoTable::s_strZhuHaiBuff = "zhu_hai_buff";
const char* CRecordUserIBusyInfoTable::s_strZhuHaiCardHp = "zhu_hai_card_hp";
const char* CRecordUserIBusyInfoTable::s_strZhuHaiBlessGroup = "zhu_hai_bless_group";
const char* CRecordUserIBusyInfoTable::s_strZhuHaiRoleBuff = "zhu_hai_role_buff";
const char* CRecordUserIBusyInfoTable::s_strZhuHaiRefreshStore = "zhu_hai_refresh_store";
const char* CRecordUserIBusyInfoTable::s_strZhuHaiSaveTeamFlag = "zhu_hai_save_team_flag";
const char* CRecordUserIBusyInfoTable::s_strGuildBossHurt = "guild_boss_hurt";
const char* CRecordUserIBusyInfoTable::s_strWishLastTime = "wish_last_time";
const char* CRecordUserIBusyInfoTable::s_strWishCardID = "wish_card_id";
const char* CRecordUserIBusyInfoTable::s_strWishNum = "wish_num";
const char* CRecordUserIBusyInfoTable::s_strWishRewardVec = "wish_reward_vec";
const char* CRecordUserIBusyInfoTable::s_strWishSwapCard = "wish_swap_card";
const char* CRecordUserIBusyInfoTable::s_strBadgeList = "badge_list";
const char* CRecordUserIBusyInfoTable::s_strNewLimitBossReward = "new_limit_boss_reward";
const char* CRecordUserIBusyInfoTable::s_strNewLimitBossDailyCount = "new_limit_boss_daily_count";
const char* CRecordUserIBusyInfoTable::s_strDailyGuildDonate = "daily_guild_donate";
const char* CRecordUserIBusyInfoTable::s_strDailyGuildSign = "daily_guild_sign";
const char* CRecordUserIBusyInfoTable::s_strDailyGuildRandomCount = "daily_guild_random_count";
const char* CRecordUserIBusyInfoTable::s_strDailyGuildRandom = "daily_guild_random";
const char* CRecordUserIBusyInfoTable::s_strDailyGuildBar = "daily_guild_bar";
const char* CRecordUserIBusyInfoTable::s_strWenwenChoose = "wenwen_choose";
const char* CRecordUserIBusyInfoTable::s_strConstelAdvTimes = "constel_adv_times";
const char* CRecordUserIBusyInfoTable::s_strGuildFragmentHistory = "guild_fragment_history";
const char* CRecordUserIBusyInfoTable::s_strHomeExpTag = "home_exp_tag";
const char* CRecordUserIBusyInfoTable::s_strRuneInfo = "rune_info";
const char* CRecordUserIBusyInfoTable::s_strConstelPutInfo = "constel_put_info";
const char* CRecordUserIBusyInfoTable::s_strDataChangeTag = "data_change_tag";
const char* CRecordUserIBusyInfoTable::s_strCbtRewardTag = "cbt_reward_tag";
const char* CRecordUserIBusyInfoTable::s_strConstelShop = "constel_shop";
const char* CRecordUserIBusyInfoTable::s_strConstelShopRefreshCount = "constel_shop_refresh_count";
const char* CRecordUserIBusyInfoTable::s_strConstelShopLastTime = "constel_shop_last_time";
const char* CRecordUserIBusyInfoTable::s_strCheatingCount = "cheating_count";
const char* CRecordUserIBusyInfoTable::s_strGashaponInfo = "gashapon_info";
const char* CRecordUserIBusyInfoTable::s_strCrushBossLevel = "crush_boss_level";
const char* CRecordUserIBusyInfoTable::s_strCrushBossInfo = "crush_boss_info";
const char* CRecordUserIBusyInfoTable::s_strCrushBossCount = "crush_boss_count";
const char* CRecordUserIBusyInfoTable::s_strHomeBuffID = "home_buff_id";
const char* CRecordUserIBusyInfoTable::s_strStartLevelTime = "start_level_time";
const char* CRecordUserIBusyInfoTable::s_strAtlasBuffInfo = "atlas_buff_info";
const char* CRecordUserIBusyInfoTable::s_strDailyGuildWarTime = "daily_guild_war_time";
const char* CRecordUserIBusyInfoTable::s_strGuildWarUseCard = "guild_war_use_card";
const char* CRecordUserIBusyInfoTable::s_strGuildSelfInfo = "guild_self_info";
const char* CRecordUserIBusyInfoTable::s_strGuildWarCalInfo = "guild_war_cal_info";
const char* CRecordUserIBusyInfoTable::s_strLimitBossBaodiTime = "limit_boss_baodi_time";

CRecordUserIBusyInfoTable::~CRecordUserIBusyInfoTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordUserIBusyInfoTable::MakeInsertSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "uid" + std::string(",");
    strVal += Int2DBString(m_iUid) + ",";
    strCol += "group_id" + std::string(",");
    strVal += Int2DBString(m_iGroupID) + ",";

    auto stIntDataITBeg = m_stIntData.begin();
    auto stIntDataITEnd = m_stIntData.end();
    for(; stIntDataITBeg != stIntDataITEnd; ++stIntDataITBeg)
    {
        strCol += stIntDataITBeg->first + std::string(",");
        strVal += Int2DBString(stIntDataITBeg->second) + ",";
    }
    auto stUInt64DataITBeg = m_stUInt64Data.begin();
    auto stUInt64DataITEnd = m_stUInt64Data.end();
    for(; stUInt64DataITBeg != stUInt64DataITEnd; ++stUInt64DataITBeg)
    {
        strCol += stUInt64DataITBeg->first + std::string(",");
        strVal += Uint642DBString(stUInt64DataITBeg->second.m_ulVal) + ",";
    }
    auto stStringDataITBeg = m_stStringData.begin();
    auto stStringDataITEnd = m_stStringData.end();
    for(; stStringDataITBeg != stStringDataITEnd; ++stStringDataITBeg)
    {
        strCol += stStringDataITBeg->first + std::string(",");
        strVal += String2DBString(stStringDataITBeg->second) + ",";
    }
    auto stVarDataITBeg = m_stVarData.begin();
    auto stVarDataITEnd = m_stVarData.end();
    for(; stVarDataITBeg != stVarDataITEnd; ++stVarDataITBeg)
    {
        strCol += stVarDataITBeg->first + std::string(",");
        strVal += Var2DBString(stVarDataITBeg->second) + ",";
    }
    auto stCharDataITBeg = m_stCharData.begin();
    auto stCharDataITEnd = m_stCharData.end();
    for(; stCharDataITBeg != stCharDataITEnd; ++stCharDataITBeg)
    {
        strCol += stCharDataITBeg->first + std::string(",");
        strVal += Char2DBString(stCharDataITBeg->second) + ",";
    }
    assert(!strCol.empty() && !strVal.empty());
    strCol[strCol.size() - 1] = ')';
    strVal[strVal.size() - 1] = ')';
    char szBuffer[12480];
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO user_ibusy_info_t_%d %s VALUES %s;" , iTableMod, strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordUserIBusyInfoTable::MakeUpdateSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    std::string strSet;
    auto stIntDataITBeg = m_stIntData.begin();
    auto stIntDataITEnd = m_stIntData.end();
    for(; stIntDataITBeg != stIntDataITEnd; ++stIntDataITBeg)
    {
        strSet += std::string(stIntDataITBeg->first) + "=" + std::string(stIntDataITBeg->first) + "+" + Int2DBString(stIntDataITBeg->second);
        strSet.push_back(',');
    }
    auto stUInt64DataITBeg = m_stUInt64Data.begin();
    auto stUInt64DataITEnd = m_stUInt64Data.end();
    for(; stUInt64DataITBeg != stUInt64DataITEnd; ++stUInt64DataITBeg)
    {
        strSet += std::string(stUInt64DataITBeg->first) + "=" + std::string(stUInt64DataITBeg->first) + std::string(1, stUInt64DataITBeg->second.m_cOperator) + Uint642DBString(stUInt64DataITBeg->second.m_ulVal);
        strSet.push_back(',');
    }
    auto stStringDataITBeg = m_stStringData.begin();
    auto stStringDataITEnd = m_stStringData.end();
    for(; stStringDataITBeg != stStringDataITEnd; ++stStringDataITBeg)
    {
        strSet += std::string(stStringDataITBeg->first) + "=" + String2DBString(stStringDataITBeg->second);
        strSet.push_back(',');
    }
    auto stVarDataITBeg = m_stVarData.begin();
    auto stVarDataITEnd = m_stVarData.end();
    for(; stVarDataITBeg != stVarDataITEnd; ++stVarDataITBeg)
    {
        strSet += std::string(stVarDataITBeg->first) + "=" + Var2DBString(stVarDataITBeg->second);
        strSet.push_back(',');
    }
    auto stCharDataITBeg = m_stCharData.begin();
    auto stCharDataITEnd = m_stCharData.end();
    for(; stCharDataITBeg != stCharDataITEnd; ++stCharDataITBeg)
    {
        strSet += std::string(stCharDataITBeg->first) + "=" + std::string(stCharDataITBeg->first) + "+" + Char2DBString(stCharDataITBeg->second);
        strSet.push_back(',');
    }
    assert(!strSet.empty());
    strSet[strSet.size() - 1] = ' ';
    char szBuffer[12480];
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE user_ibusy_info_t_%d SET %s WHERE uid=%d AND group_id=%d;", iTableMod, strSet.c_str(), m_iUid, m_iGroupID);
    return szBuffer;
}
std::string
CRecordUserIBusyInfoTable::MakeDeleteSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM user_ibusy_info_t_%d WHERE uid=%d AND group_id=%d;", iTableMod, m_iUid, m_iGroupID);
    return szBuffer;
}

int
CRecordUserIBusyInfoTable::GetUid()
{
    return m_iUid;
}

int
CRecordUserIBusyInfoTable::GetGroupID()
{
    return m_iGroupID;
}

std::vector<TUserIbusyInfoTableValueType>
CRecordUserIBusyInfoTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    int iTableMod = iUin % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM user_ibusy_info_t_%d WHERE uid=%d AND group_id=%d;" , iTableMod, iUin, iGroupID);
    std::vector<TUserIbusyInfoTableValueType> stRet;
    TUserIbusyInfoTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iActivityID = ::atoi(stData[i]["activity_id"].c_str());
            stValue.m_iLevel = ::atoi(stData[i]["level"].c_str());
            stValue.m_iExp = ::atoi(stData[i]["exp"].c_str());
            stValue.m_iDailyExpAward = ::atoi(stData[i]["daily_exp_award"].c_str());
            stValue.m_iScore = ::atoi(stData[i]["score"].c_str());
            stValue.m_iExploreExp = ::atoi(stData[i]["explore_exp"].c_str());
            stValue.m_iTodayHuoyue = ::atoi(stData[i]["today_huoyue"].c_str());
            stValue.m_iChangeTaskCount = ::atoi(stData[i]["change_task_count"].c_str());
            stValue.m_iFreeChangeTaskCount = ::atoi(stData[i]["free_change_task_count"].c_str());
            stValue.m_iMarketLastRefreshTime = ::atoi(stData[i]["market_last_refresh_time"].c_str());
            stValue.m_iMarketRefreshTimes = ::atoi(stData[i]["market_refresh_times"].c_str());
            FormatDBBin(stData[i]["getted_level_reward"], stValue.m_stGettedLevelReward);
            FormatDBBin(stData[i]["getted_deposit_reward"], stValue.m_stGettedDepositReward);
            FormatDBBin(stData[i]["task_info"], stValue.m_stTaskInfo);
            stValue.m_iSpecialGiftWeekID = ::atoi(stData[i]["special_gift_week_id"].c_str());
            stValue.m_iSpecialGiftWeekLevel = ::atoi(stData[i]["special_gift_week_level"].c_str());
            stValue.m_iSpecialGiftMonthID = ::atoi(stData[i]["special_gift_month_id"].c_str());
            stValue.m_iSpecialGiftMonthLevel = ::atoi(stData[i]["special_gift_month_level"].c_str());
            stValue.m_iLuckyHammerActivityID = ::atoi(stData[i]["lucky_hammer_activity_id"].c_str());
            stValue.m_iLuckyHammerLotteryCount = ::atoi(stData[i]["lucky_hammer_lottery_count"].c_str());
            stValue.m_iMonthPassExp = ::atoi(stData[i]["month_pass_exp"].c_str());
            stValue.m_iMonthPassDay = ::atoi(stData[i]["month_pass_day"].c_str());
            stValue.m_iMonthPassRecharge = ::atoi(stData[i]["month_pass_recharge"].c_str());
            stValue.m_strMonthPassAwardBitmap = stData[i]["month_pass_award_bitmap"];
            stValue.m_strMonthPassDepositAwardBitmap = stData[i]["month_pass_deposit_award_bitmap"];
            stValue.m_iMonthPassWishEventID = ::atoi(stData[i]["month_pass_wish_event_id"].c_str());
            FormatDBBin(stData[i]["month_pass_wish_item_index"], stValue.m_stMonthPassWishItemIndex);
            FormatDBBin(stData[i]["card_dialog_limit_avy_stat"], stValue.m_stCardDialogLimitAvyStat);
            stValue.m_iCardDialogNextTime = ::atoi(stData[i]["card_dialog_next_time"].c_str());
            stValue.m_iCardDialogTimes = ::atoi(stData[i]["card_dialog_times"].c_str());
            stValue.m_iNewsLastTime = ::atoi(stData[i]["news_last_time"].c_str());
            stValue.m_iNewsLastData = ::atoi(stData[i]["news_last_data"].c_str());
            stValue.m_iAddupDepositBonus = ::atoi(stData[i]["addup_deposit_bonus"].c_str());
            stValue.m_iFoundationOpenTime = ::atoi(stData[i]["foundation_open_time"].c_str());
            stValue.m_ulOnceEventStat = (uint64_t)strtoull(stData[i]["once_event_stat"].c_str(), nullptr, 10);
            FormatDBBin(stData[i]["random_store_refresh_data"], stValue.m_stRandomStoreRefreshData);
            FormatDBBin(stData[i]["simple_game_point"], stValue.m_stSimpleGamePoint);
            stValue.m_iHeartInsureOpenTime = ::atoi(stData[i]["heart_insure_open_time"].c_str());
            FormatDBBin(stData[i]["moon_map_skill_level"], stValue.m_stMoonMapSkillLevel);
            FormatDBBin(stData[i]["red_bag_unique_index"], stValue.m_stRedBagUniqueIndex);
            FormatDBBin(stData[i]["world_quest_reward_state"], stValue.m_stWorldQuestRewardState);
            stValue.m_iWorldQuestRewardTimes = ::atoi(stData[i]["world_quest_reward_times"].c_str());
            FormatDBBin(stData[i]["questionnaire_state"], stValue.m_stQuestionnaireState);
            FormatDBBin(stData[i]["questionnaire_duration_time"], stValue.m_stQuestionnaireDurationTime);
            FormatDBBin(stData[i]["limit_boss_accepted"], stValue.m_stLimitBossAccepted);
            FormatDBBin(stData[i]["limit_boss_rand"], stValue.m_stLimitBossRand);
            stValue.m_iLimitBoxTime = ::atoi(stData[i]["limit_box_time"].c_str());
            stValue.m_iTotalHeartLevel = ::atoi(stData[i]["total_heart_level"].c_str());
            stValue.m_iTotalHeartExp = ::atoi(stData[i]["total_heart_exp"].c_str());
            stValue.m_iConstelExpPool = ::atoi(stData[i]["constel_exp_pool"].c_str());
            stValue.m_iFishingRod = ::atoi(stData[i]["fishing_rod"].c_str());
            FormatDBBin(stData[i]["main_bgm"], stValue.m_stMainBgm);
            FormatDBBin(stData[i]["home_bgm"], stValue.m_stHomeBgm);
            stValue.m_iMallBaodiTimes = ::atoi(stData[i]["mall_baodi_times"].c_str());
            stValue.m_iZhuHaiLevel = ::atoi(stData[i]["zhu_hai_level"].c_str());
            stValue.m_iZhuHaiGress = ::atoi(stData[i]["zhu_hai_gress"].c_str());
            FormatDBBin(stData[i]["zhu_hai_buff"], stValue.m_stZhuHaiBuff);
            FormatDBBin(stData[i]["zhu_hai_card_hp"], stValue.m_stZhuHaiCardHp);
            FormatDBBin(stData[i]["zhu_hai_bless_group"], stValue.m_stZhuHaiBlessGroup);
            FormatDBBin(stData[i]["zhu_hai_role_buff"], stValue.m_stZhuHaiRoleBuff);
            stValue.m_iZhuHaiRefreshStore = ::atoi(stData[i]["zhu_hai_refresh_store"].c_str());
            stValue.m_cZhuHaiSaveTeamFlag = (char)::atoi(stData[i]["zhu_hai_save_team_flag"].c_str());
            FormatDBBin(stData[i]["guild_boss_hurt"], stValue.m_stGuildBossHurt);
            stValue.m_iWishLastTime = ::atoi(stData[i]["wish_last_time"].c_str());
            stValue.m_iWishCardID = ::atoi(stData[i]["wish_card_id"].c_str());
            stValue.m_iWishNum = ::atoi(stData[i]["wish_num"].c_str());
            FormatDBBin(stData[i]["wish_reward_vec"], stValue.m_stWishRewardVec);
            FormatDBBin(stData[i]["wish_swap_card"], stValue.m_stWishSwapCard);
            FormatDBBin(stData[i]["badge_list"], stValue.m_stBadgeList);
            FormatDBBin(stData[i]["new_limit_boss_reward"], stValue.m_stNewLimitBossReward);
            stValue.m_iNewLimitBossDailyCount = ::atoi(stData[i]["new_limit_boss_daily_count"].c_str());
            stValue.m_cDailyGuildDonate = (char)::atoi(stData[i]["daily_guild_donate"].c_str());
            stValue.m_cDailyGuildSign = (char)::atoi(stData[i]["daily_guild_sign"].c_str());
            stValue.m_iDailyGuildRandomCount = ::atoi(stData[i]["daily_guild_random_count"].c_str());
            FormatDBBin(stData[i]["daily_guild_random"], stValue.m_stDailyGuildRandom);
            stValue.m_cDailyGuildBar = (char)::atoi(stData[i]["daily_guild_bar"].c_str());
            stValue.m_cWenwenChoose = (char)::atoi(stData[i]["wenwen_choose"].c_str());
            stValue.m_iConstelAdvTimes = ::atoi(stData[i]["constel_adv_times"].c_str());
            FormatDBBin(stData[i]["guild_fragment_history"], stValue.m_stGuildFragmentHistory);
            FormatDBBin(stData[i]["home_exp_tag"], stValue.m_stHomeExpTag);
            FormatDBBin(stData[i]["rune_info"], stValue.m_stRuneInfo);
            FormatDBBin(stData[i]["constel_put_info"], stValue.m_stConstelPutInfo);
            stValue.m_cDataChangeTag = (char)::atoi(stData[i]["data_change_tag"].c_str());
            stValue.m_cCbtRewardTag = (char)::atoi(stData[i]["cbt_reward_tag"].c_str());
            FormatDBBin(stData[i]["constel_shop"], stValue.m_stConstelShop);
            stValue.m_iConstelShopRefreshCount = ::atoi(stData[i]["constel_shop_refresh_count"].c_str());
            stValue.m_iConstelShopLastTime = ::atoi(stData[i]["constel_shop_last_time"].c_str());
            stValue.m_iCheatingCount = ::atoi(stData[i]["cheating_count"].c_str());
            FormatDBBin(stData[i]["gashapon_info"], stValue.m_stGashaponInfo);
            stValue.m_iCrushBossLevel = ::atoi(stData[i]["crush_boss_level"].c_str());
            FormatDBBin(stData[i]["crush_boss_info"], stValue.m_stCrushBossInfo);
            stValue.m_iCrushBossCount = ::atoi(stData[i]["crush_boss_count"].c_str());
            FormatDBBin(stData[i]["home_buff_id"], stValue.m_stHomeBuffID);
            stValue.m_iStartLevelTime = ::atoi(stData[i]["start_level_time"].c_str());
            FormatDBBin(stData[i]["atlas_buff_info"], stValue.m_stAtlasBuffInfo);
            stValue.m_cDailyGuildWarTime = (char)::atoi(stData[i]["daily_guild_war_time"].c_str());
            FormatDBBin(stData[i]["guild_war_use_card"], stValue.m_stGuildWarUseCard);
            FormatDBBin(stData[i]["guild_self_info"], stValue.m_stGuildSelfInfo);
            FormatDBBin(stData[i]["guild_war_cal_info"], stValue.m_stGuildWarCalInfo);
            stValue.m_iLimitBossBaodiTime = ::atoi(stData[i]["limit_boss_baodi_time"].c_str());
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
