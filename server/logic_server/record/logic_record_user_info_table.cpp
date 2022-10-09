#include "logic_record_user_info_table.h"

#include <cassert>

const int CRecordUserInfoTable::TABLE_DIV = 32;

const char* CRecordUserInfoTable::s_strAge = "age";
const char* CRecordUserInfoTable::s_strVipLevel = "vip_level";
const char* CRecordUserInfoTable::s_strGuideMajorStep = "guide_major_step";
const char* CRecordUserInfoTable::s_strGuideMinorStep = "guide_minor_step";
const char* CRecordUserInfoTable::s_strGuideSpecialStep = "guide_special_step";
const char* CRecordUserInfoTable::s_strPrimaryKeyIndex = "primary_key_index";
const char* CRecordUserInfoTable::s_strNick = "nick";
const char* CRecordUserInfoTable::s_strBanType = "ban_type";
const char* CRecordUserInfoTable::s_strBanBeginTime = "ban_begin_time";
const char* CRecordUserInfoTable::s_strBanEndTime = "ban_end_time";
const char* CRecordUserInfoTable::s_strBanComment = "ban_comment";
const char* CRecordUserInfoTable::s_strJubaoCount = "jubao_count";
const char* CRecordUserInfoTable::s_strJubaoedCount = "jubaoed_count";
const char* CRecordUserInfoTable::s_strGuildID = "guild_id";
const char* CRecordUserInfoTable::s_strGuildName = "guild_name";
const char* CRecordUserInfoTable::s_strChangeNickCount = "change_nick_count";
const char* CRecordUserInfoTable::s_strAvatarID = "avatar_id";
const char* CRecordUserInfoTable::s_strAvatarBorder = "avatar_border";
const char* CRecordUserInfoTable::s_strLevel = "level";
const char* CRecordUserInfoTable::s_strExp = "exp";
const char* CRecordUserInfoTable::s_strVipExp = "vip_exp";
const char* CRecordUserInfoTable::s_strAddUpRmb = "add_up_rmb";
const char* CRecordUserInfoTable::s_strMonthAddUpRmb = "month_add_up_rmb";
const char* CRecordUserInfoTable::s_strFirstDepositBonus = "first_deposit_bonus";
const char* CRecordUserInfoTable::s_strLastLogin = "last_login";
const char* CRecordUserInfoTable::s_strLastLogout = "last_logout";
const char* CRecordUserInfoTable::s_strAddUpLoginDay = "add_up_login_day";
const char* CRecordUserInfoTable::s_strCreateTime = "create_time";
const char* CRecordUserInfoTable::s_strFightLevelID = "fight_level_id";
const char* CRecordUserInfoTable::s_strGlobalMailTimePos = "global_mail_time_pos";
const char* CRecordUserInfoTable::s_strNpcMailDay = "npc_mail_day";
const char* CRecordUserInfoTable::s_strNpcMailServer = "npc_mail_server";
const char* CRecordUserInfoTable::s_strNpcMailDatetime = "npc_mail_datetime";
const char* CRecordUserInfoTable::s_strNpcMailLevel = "npc_mail_level";
const char* CRecordUserInfoTable::s_strNpcMailCardtime = "npc_mail_cardtime";
const char* CRecordUserInfoTable::s_strThemeSkin = "theme_skin";
const char* CRecordUserInfoTable::s_strRuneCard = "rune_card";
const char* CRecordUserInfoTable::s_strLastSignTime = "last_sign_time";
const char* CRecordUserInfoTable::s_strTotalSign = "total_sign";
const char* CRecordUserInfoTable::s_strActiveDoubleDepositCount = "active_double_deposit_count";
const char* CRecordUserInfoTable::s_strRefreshDoubleDepositTime = "refresh_double_deposit_time";
const char* CRecordUserInfoTable::s_strRmb = "rmb";
const char* CRecordUserInfoTable::s_strPayRmb = "pay_rmb";
const char* CRecordUserInfoTable::s_strGold = "gold";
const char* CRecordUserInfoTable::s_strEvilBrand = "evil_brand";
const char* CRecordUserInfoTable::s_strCharm = "charm";
const char* CRecordUserInfoTable::s_strPhyPower = "phy_power";
const char* CRecordUserInfoTable::s_strPhyStore = "phy_store";
const char* CRecordUserInfoTable::s_strLastAutoAddTime = "last_auto_add_time";
const char* CRecordUserInfoTable::s_strVitality = "vitality";
const char* CRecordUserInfoTable::s_strAddVitalityTime = "add_vitality_time";
const char* CRecordUserInfoTable::s_strLastMergeRefreshTime = "last_merge_refresh_time";
const char* CRecordUserInfoTable::s_strRoleCombat = "role_combat";
const char* CRecordUserInfoTable::s_strRoleCombatMax = "role_combat_max";
const char* CRecordUserInfoTable::s_strDailyDeposit = "daily_deposit";
const char* CRecordUserInfoTable::s_strDailyDepositBonus = "daily_deposit_bonus";
const char* CRecordUserInfoTable::s_strLastDepositTime = "last_deposit_time";
const char* CRecordUserInfoTable::s_strScoreRankBonusTime = "score_rank_bonus_time";
const char* CRecordUserInfoTable::s_strCombatRankBonusTime = "combat_rank_bonus_time";
const char* CRecordUserInfoTable::s_strDailyTaskChangeTime = "daily_task_change_time";
const char* CRecordUserInfoTable::s_strFightPowerHighestRank = "fight_power_highest_rank";
const char* CRecordUserInfoTable::s_strBuyGoldTimes = "buy_gold_times";
const char* CRecordUserInfoTable::s_strBuyGoldTotalTimes = "buy_gold_total_times";
const char* CRecordUserInfoTable::s_strBuyEnergyTimes = "buy_energy_times";
const char* CRecordUserInfoTable::s_strLastBuyEnergyTime = "last_buy_energy_time";
const char* CRecordUserInfoTable::s_strBuyVitalityTimes = "buy_vitality_times";
const char* CRecordUserInfoTable::s_strLastBuyVitalityTime = "last_buy_vitality_time";
const char* CRecordUserInfoTable::s_strTotalOnlineTime = "total_online_time";
const char* CRecordUserInfoTable::s_strRefreshPvpTimes = "refresh_pvp_times";
const char* CRecordUserInfoTable::s_strLastRefreshPvpTime = "last_refresh_pvp_time";
const char* CRecordUserInfoTable::s_strCrossFightResetTime = "cross_fight_reset_time";
const char* CRecordUserInfoTable::s_strCrossFightResetTimes = "cross_fight_reset_times";
const char* CRecordUserInfoTable::s_strCrossFightRefreshResetTimesTimes = "cross_fight_refresh_reset_times_times";
const char* CRecordUserInfoTable::s_strCrossFightLastRefreshResetTimesTime = "cross_fight_last_refresh_reset_times_time";
const char* CRecordUserInfoTable::s_strPvpHighestRank = "pvp_highest_rank";
const char* CRecordUserInfoTable::s_strPvpFightTimes = "pvp_fight_times";
const char* CRecordUserInfoTable::s_strPvpVictoryTimes = "pvp_victory_times";
const char* CRecordUserInfoTable::s_strMatchScore = "match_score";
const char* CRecordUserInfoTable::s_strMatchMaxScore = "match_max_score";
const char* CRecordUserInfoTable::s_strMatchWinCount = "match_win_count";
const char* CRecordUserInfoTable::s_strMatchLoseCount = "match_lose_count";
const char* CRecordUserInfoTable::s_strMatchMaxLevelAwardBitmap = "match_max_level_award_bitmap";
const char* CRecordUserInfoTable::s_strMatchDailyCount = "match_daily_count";
const char* CRecordUserInfoTable::s_strMatchStoreBuyCount = "match_store_buy_count";
const char* CRecordUserInfoTable::s_strFairPvpWinCount = "fair_pvp_win_count";
const char* CRecordUserInfoTable::s_strFairPvpLoseCount = "fair_pvp_lose_count";
const char* CRecordUserInfoTable::s_strFairPvpStoreCard = "fair_pvp_store_card";
const char* CRecordUserInfoTable::s_strFairPvpTeamCard = "fair_pvp_team_card";
const char* CRecordUserInfoTable::s_strNormalChapterBonus1 = "normal_chapter_bonus1";
const char* CRecordUserInfoTable::s_strNormalChapterBonus2 = "normal_chapter_bonus2";
const char* CRecordUserInfoTable::s_strNormalChapterBonus3 = "normal_chapter_bonus3";
const char* CRecordUserInfoTable::s_strEliteChapterBonus1 = "elite_chapter_bonus1";
const char* CRecordUserInfoTable::s_strEliteChapterBonus2 = "elite_chapter_bonus2";
const char* CRecordUserInfoTable::s_strEliteChapterBonus3 = "elite_chapter_bonus3";
const char* CRecordUserInfoTable::s_strVipLevelBuyBonus = "vip_level_buy_bonus";
const char* CRecordUserInfoTable::s_strRecommendedLineupBitmap = "recommended_lineup_bitmap";
const char* CRecordUserInfoTable::s_strFightPowerRankActive = "fight_power_rank_active";
const char* CRecordUserInfoTable::s_strClimbTowerCurrentOrder = "climb_tower_current_order";
const char* CRecordUserInfoTable::s_strClimbTowerMaxOrder = "climb_tower_max_order";
const char* CRecordUserInfoTable::s_strClimbTowerResetCount = "climb_tower_reset_count";
const char* CRecordUserInfoTable::s_strLastClimbTowerResetTime = "last_climb_tower_reset_time";
const char* CRecordUserInfoTable::s_strClimbTowerSweepEndTime = "climb_tower_sweep_end_time";
const char* CRecordUserInfoTable::s_strClimbTowerHighestRank = "climb_tower_highest_rank";
const char* CRecordUserInfoTable::s_strSerialPayAwarded = "serial_pay_awarded";
const char* CRecordUserInfoTable::s_strCornucopiaProfitYuanbao = "cornucopia_profit_yuanbao";
const char* CRecordUserInfoTable::s_strGuildCoin = "guild_coin";
const char* CRecordUserInfoTable::s_strLastLeaveGuildTime = "last_leave_guild_time";
const char* CRecordUserInfoTable::s_strJubaoAwardCount = "jubao_award_count";
const char* CRecordUserInfoTable::s_strTodayJubaoAwardCount = "today_jubao_award_count";
const char* CRecordUserInfoTable::s_strLastJubaoAwardTime = "last_jubao_award_time";
const char* CRecordUserInfoTable::s_strWeiBuyNum = "wei_buy_num";
const char* CRecordUserInfoTable::s_strWeiAvgPrice = "wei_avg_price";
const char* CRecordUserInfoTable::s_strShuBuyNum = "shu_buy_num";
const char* CRecordUserInfoTable::s_strShuAvgPrice = "shu_avg_price";
const char* CRecordUserInfoTable::s_strWuBuyNum = "wu_buy_num";
const char* CRecordUserInfoTable::s_strWuAvgPrice = "wu_avg_price";
const char* CRecordUserInfoTable::s_strEntireWarContribution = "entire_war_contribution";
const char* CRecordUserInfoTable::s_strEntireWarMilitarkLevel = "entire_war_militark_level";
const char* CRecordUserInfoTable::s_strEntireWarWorshippedCount = "entire_war_worshipped_count";
const char* CRecordUserInfoTable::s_strLastEntireWarTime = "last_entire_war_time";
const char* CRecordUserInfoTable::s_strWeekWarContribution = "week_war_contribution";
const char* CRecordUserInfoTable::s_strWeekLimitBossScore = "week_limit_boss_score";
const char* CRecordUserInfoTable::s_strActionCoin = "action_coin";
const char* CRecordUserInfoTable::s_strLastAddActionTime = "last_add_action_time";
const char* CRecordUserInfoTable::s_strEntireWarAwardBitmap = "entire_war_award_bitmap";
const char* CRecordUserInfoTable::s_strEntireWarNodeAwardStat = "entire_war_node_award_stat";
const char* CRecordUserInfoTable::s_strEntireWarExtraStatus = "entire_war_extra_status";
const char* CRecordUserInfoTable::s_strEntireWarNodePara = "entire_war_node_para";
const char* CRecordUserInfoTable::s_strDayQiyuID = "day_qiyu_id";
const char* CRecordUserInfoTable::s_strCangbaotuQiyuID = "cangbaotu_qiyu_id";
const char* CRecordUserInfoTable::s_strQiyuEndtime = "qiyu_endtime";
const char* CRecordUserInfoTable::s_strCangbaotuEndtime = "cangbaotu_endtime";
const char* CRecordUserInfoTable::s_strQiyuBuff = "qiyu_buff";
const char* CRecordUserInfoTable::s_strCangbaotuBuff = "cangbaotu_buff";
const char* CRecordUserInfoTable::s_strCangbaotuSuipian = "cangbaotu_suipian";
const char* CRecordUserInfoTable::s_strBanquetAllTeamID = "banquet_all_team_id";
const char* CRecordUserInfoTable::s_strDataUpdateVersion = "data_update_version";
const char* CRecordUserInfoTable::s_strNewPassTotalExp = "new_pass_total_exp";
const char* CRecordUserInfoTable::s_strNewPassNormalLevel = "new_pass_normal_level";
const char* CRecordUserInfoTable::s_strNewPassAdvancedLevel = "new_pass_advanced_level";
const char* CRecordUserInfoTable::s_strLimitBossTotalPoint = "limit_boss_total_point";
const char* CRecordUserInfoTable::s_strFindLimitBoss = "find_limit_boss";
const char* CRecordUserInfoTable::s_strPvpScore = "pvp_score";
const char* CRecordUserInfoTable::s_strPvpHighestScore = "pvp_highest_score";
const char* CRecordUserInfoTable::s_strPvpRefreshCount = "pvp_refresh_count";
const char* CRecordUserInfoTable::s_strPvpWeeklyTask = "pvp_weekly_task";
const char* CRecordUserInfoTable::s_strHomeExp = "home_exp";
const char* CRecordUserInfoTable::s_strHomeExpReward = "home_exp_reward";
const char* CRecordUserInfoTable::s_strMonthAgeLimit = "month_age_limit";
const char* CRecordUserInfoTable::s_strLastMonthAgeChange = "last_month_age_change";
const char* CRecordUserInfoTable::s_strLastLimitCommodityID = "last_limit_commodity_id";
const char* CRecordUserInfoTable::s_strHotSpringRewardEmail = "hot_spring_reward_email";
const char* CRecordUserInfoTable::s_strHotSpringRewardOther = "hot_spring_reward_other";
const char* CRecordUserInfoTable::s_strHotSpringReward = "hot_spring_reward";
const char* CRecordUserInfoTable::s_strHotSpringInvite = "hot_spring_invite";
const char* CRecordUserInfoTable::s_strHotSpringJoin = "hot_spring_join";
const char* CRecordUserInfoTable::s_strIsBindEmail = "is_bind_email";
const char* CRecordUserInfoTable::s_strDailyGuildDonate = "daily_guild_donate";
const char* CRecordUserInfoTable::s_strGuildPersonDonate = "guild_person_donate";
const char* CRecordUserInfoTable::s_strGuildExtraLevel = "guild_extra_level";
const char* CRecordUserInfoTable::s_strDailySignFlag = "daily_sign_flag";
const char* CRecordUserInfoTable::s_strDailySignReward = "daily_sign_reward";
const char* CRecordUserInfoTable::s_strCompleteSignReward = "complete_sign_reward";
const char* CRecordUserInfoTable::s_strZhuhaiScore = "zhuhai_score";

CRecordUserInfoTable::~CRecordUserInfoTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordUserInfoTable::MakeInsertSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO user_info_t_%d %s VALUES %s;" , iTableMod, strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordUserInfoTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE user_info_t_%d SET %s WHERE uid=%d AND group_id=%d;", iTableMod, strSet.c_str(), m_iUid, m_iGroupID);
    return szBuffer;
}
std::string
CRecordUserInfoTable::MakeDeleteSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM user_info_t_%d WHERE uid=%d AND group_id=%d;", iTableMod, m_iUid, m_iGroupID);
    return szBuffer;
}

int
CRecordUserInfoTable::GetUid()
{
    return m_iUid;
}

int
CRecordUserInfoTable::GetGroupID()
{
    return m_iGroupID;
}

std::vector<TUserInfoTableValueType>
CRecordUserInfoTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    int iTableMod = iUin % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM user_info_t_%d WHERE uid=%d AND group_id=%d;" , iTableMod, iUin, iGroupID);
    std::vector<TUserInfoTableValueType> stRet;
    TUserInfoTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iAge = ::atoi(stData[i]["age"].c_str());
            stValue.m_cVipLevel = (char)::atoi(stData[i]["vip_level"].c_str());
            stValue.m_cGuideMajorStep = (char)::atoi(stData[i]["guide_major_step"].c_str());
            stValue.m_cGuideMinorStep = (char)::atoi(stData[i]["guide_minor_step"].c_str());
            stValue.m_ulGuideSpecialStep = (uint64_t)strtoull(stData[i]["guide_special_step"].c_str(), nullptr, 10);
            stValue.m_iPrimaryKeyIndex = ::atoi(stData[i]["primary_key_index"].c_str());
            stValue.m_strNick = stData[i]["nick"];
            stValue.m_iBanType = ::atoi(stData[i]["ban_type"].c_str());
            stValue.m_iBanBeginTime = ::atoi(stData[i]["ban_begin_time"].c_str());
            stValue.m_iBanEndTime = ::atoi(stData[i]["ban_end_time"].c_str());
            stValue.m_strBanComment = stData[i]["ban_comment"];
            stValue.m_iJubaoCount = ::atoi(stData[i]["jubao_count"].c_str());
            stValue.m_iJubaoedCount = ::atoi(stData[i]["jubaoed_count"].c_str());
            stValue.m_iGuildID = ::atoi(stData[i]["guild_id"].c_str());
            stValue.m_strGuildName = stData[i]["guild_name"];
            stValue.m_iChangeNickCount = ::atoi(stData[i]["change_nick_count"].c_str());
            stValue.m_iAvatarID = ::atoi(stData[i]["avatar_id"].c_str());
            stValue.m_iAvatarBorder = ::atoi(stData[i]["avatar_border"].c_str());
            stValue.m_iLevel = ::atoi(stData[i]["level"].c_str());
            stValue.m_iExp = ::atoi(stData[i]["exp"].c_str());
            stValue.m_iVipExp = ::atoi(stData[i]["vip_exp"].c_str());
            stValue.m_iAddUpRmb = ::atoi(stData[i]["add_up_rmb"].c_str());
            stValue.m_iMonthAddUpRmb = ::atoi(stData[i]["month_add_up_rmb"].c_str());
            stValue.m_iFirstDepositBonus = ::atoi(stData[i]["first_deposit_bonus"].c_str());
            stValue.m_iLastLogin = ::atoi(stData[i]["last_login"].c_str());
            stValue.m_iLastLogout = ::atoi(stData[i]["last_logout"].c_str());
            stValue.m_iAddUpLoginDay = ::atoi(stData[i]["add_up_login_day"].c_str());
            stValue.m_iCreateTime = ::atoi(stData[i]["create_time"].c_str());
            stValue.m_iFightLevelID = ::atoi(stData[i]["fight_level_id"].c_str());
            stValue.m_iGlobalMailTimePos = ::atoi(stData[i]["global_mail_time_pos"].c_str());
            stValue.m_iNpcMailDay = ::atoi(stData[i]["npc_mail_day"].c_str());
            stValue.m_iNpcMailServer = ::atoi(stData[i]["npc_mail_server"].c_str());
            stValue.m_iNpcMailDatetime = ::atoi(stData[i]["npc_mail_datetime"].c_str());
            stValue.m_iNpcMailLevel = ::atoi(stData[i]["npc_mail_level"].c_str());
            stValue.m_iNpcMailCardtime = ::atoi(stData[i]["npc_mail_cardtime"].c_str());
            stValue.m_iThemeSkin = ::atoi(stData[i]["theme_skin"].c_str());
            stValue.m_iRuneCard = ::atoi(stData[i]["rune_card"].c_str());
            stValue.m_iLastSignTime = ::atoi(stData[i]["last_sign_time"].c_str());
            stValue.m_iTotalSign = ::atoi(stData[i]["total_sign"].c_str());
            stValue.m_iActiveDoubleDepositCount = ::atoi(stData[i]["active_double_deposit_count"].c_str());
            stValue.m_iRefreshDoubleDepositTime = ::atoi(stData[i]["refresh_double_deposit_time"].c_str());
            stValue.m_iRmb = ::atoi(stData[i]["rmb"].c_str());
            stValue.m_iPayRmb = ::atoi(stData[i]["pay_rmb"].c_str());
            stValue.m_iGold = ::atoi(stData[i]["gold"].c_str());
            stValue.m_iEvilBrand = ::atoi(stData[i]["evil_brand"].c_str());
            stValue.m_iCharm = ::atoi(stData[i]["charm"].c_str());
            stValue.m_iPhyPower = ::atoi(stData[i]["phy_power"].c_str());
            stValue.m_iPhyStore = ::atoi(stData[i]["phy_store"].c_str());
            stValue.m_iLastAutoAddTime = ::atoi(stData[i]["last_auto_add_time"].c_str());
            stValue.m_iVitality = ::atoi(stData[i]["vitality"].c_str());
            stValue.m_iAddVitalityTime = ::atoi(stData[i]["add_vitality_time"].c_str());
            stValue.m_iLastMergeRefreshTime = ::atoi(stData[i]["last_merge_refresh_time"].c_str());
            stValue.m_iRoleCombat = ::atoi(stData[i]["role_combat"].c_str());
            stValue.m_iRoleCombatMax = ::atoi(stData[i]["role_combat_max"].c_str());
            stValue.m_iDailyDeposit = ::atoi(stData[i]["daily_deposit"].c_str());
            stValue.m_iDailyDepositBonus = ::atoi(stData[i]["daily_deposit_bonus"].c_str());
            stValue.m_iLastDepositTime = ::atoi(stData[i]["last_deposit_time"].c_str());
            stValue.m_iScoreRankBonusTime = ::atoi(stData[i]["score_rank_bonus_time"].c_str());
            stValue.m_iCombatRankBonusTime = ::atoi(stData[i]["combat_rank_bonus_time"].c_str());
            stValue.m_iDailyTaskChangeTime = ::atoi(stData[i]["daily_task_change_time"].c_str());
            stValue.m_iFightPowerHighestRank = ::atoi(stData[i]["fight_power_highest_rank"].c_str());
            stValue.m_iBuyGoldTimes = ::atoi(stData[i]["buy_gold_times"].c_str());
            stValue.m_iBuyGoldTotalTimes = ::atoi(stData[i]["buy_gold_total_times"].c_str());
            stValue.m_iBuyEnergyTimes = ::atoi(stData[i]["buy_energy_times"].c_str());
            stValue.m_iLastBuyEnergyTime = ::atoi(stData[i]["last_buy_energy_time"].c_str());
            stValue.m_iBuyVitalityTimes = ::atoi(stData[i]["buy_vitality_times"].c_str());
            stValue.m_iLastBuyVitalityTime = ::atoi(stData[i]["last_buy_vitality_time"].c_str());
            stValue.m_iTotalOnlineTime = ::atoi(stData[i]["total_online_time"].c_str());
            stValue.m_iRefreshPvpTimes = ::atoi(stData[i]["refresh_pvp_times"].c_str());
            stValue.m_iLastRefreshPvpTime = ::atoi(stData[i]["last_refresh_pvp_time"].c_str());
            stValue.m_iCrossFightResetTime = ::atoi(stData[i]["cross_fight_reset_time"].c_str());
            stValue.m_iCrossFightResetTimes = ::atoi(stData[i]["cross_fight_reset_times"].c_str());
            stValue.m_iCrossFightRefreshResetTimesTimes = ::atoi(stData[i]["cross_fight_refresh_reset_times_times"].c_str());
            stValue.m_iCrossFightLastRefreshResetTimesTime = ::atoi(stData[i]["cross_fight_last_refresh_reset_times_time"].c_str());
            stValue.m_iPvpHighestRank = ::atoi(stData[i]["pvp_highest_rank"].c_str());
            stValue.m_iPvpFightTimes = ::atoi(stData[i]["pvp_fight_times"].c_str());
            stValue.m_iPvpVictoryTimes = ::atoi(stData[i]["pvp_victory_times"].c_str());
            stValue.m_iMatchScore = ::atoi(stData[i]["match_score"].c_str());
            stValue.m_iMatchMaxScore = ::atoi(stData[i]["match_max_score"].c_str());
            stValue.m_iMatchWinCount = ::atoi(stData[i]["match_win_count"].c_str());
            stValue.m_iMatchLoseCount = ::atoi(stData[i]["match_lose_count"].c_str());
            stValue.m_strMatchMaxLevelAwardBitmap = stData[i]["match_max_level_award_bitmap"];
            stValue.m_iMatchDailyCount = ::atoi(stData[i]["match_daily_count"].c_str());
            FormatDBBin(stData[i]["match_store_buy_count"], stValue.m_stMatchStoreBuyCount);
            stValue.m_iFairPvpWinCount = ::atoi(stData[i]["fair_pvp_win_count"].c_str());
            stValue.m_iFairPvpLoseCount = ::atoi(stData[i]["fair_pvp_lose_count"].c_str());
            FormatDBBin(stData[i]["fair_pvp_store_card"], stValue.m_stFairPvpStoreCard);
            FormatDBBin(stData[i]["fair_pvp_team_card"], stValue.m_stFairPvpTeamCard);
            stValue.m_ulNormalChapterBonus1 = (uint64_t)strtoull(stData[i]["normal_chapter_bonus1"].c_str(), nullptr, 10);
            stValue.m_ulNormalChapterBonus2 = (uint64_t)strtoull(stData[i]["normal_chapter_bonus2"].c_str(), nullptr, 10);
            stValue.m_ulNormalChapterBonus3 = (uint64_t)strtoull(stData[i]["normal_chapter_bonus3"].c_str(), nullptr, 10);
            stValue.m_ulEliteChapterBonus1 = (uint64_t)strtoull(stData[i]["elite_chapter_bonus1"].c_str(), nullptr, 10);
            stValue.m_ulEliteChapterBonus2 = (uint64_t)strtoull(stData[i]["elite_chapter_bonus2"].c_str(), nullptr, 10);
            stValue.m_ulEliteChapterBonus3 = (uint64_t)strtoull(stData[i]["elite_chapter_bonus3"].c_str(), nullptr, 10);
            stValue.m_ulVipLevelBuyBonus = (uint64_t)strtoull(stData[i]["vip_level_buy_bonus"].c_str(), nullptr, 10);
            stValue.m_ulRecommendedLineupBitmap = (uint64_t)strtoull(stData[i]["recommended_lineup_bitmap"].c_str(), nullptr, 10);
            stValue.m_ulFightPowerRankActive = (uint64_t)strtoull(stData[i]["fight_power_rank_active"].c_str(), nullptr, 10);
            stValue.m_iClimbTowerCurrentOrder = ::atoi(stData[i]["climb_tower_current_order"].c_str());
            stValue.m_iClimbTowerMaxOrder = ::atoi(stData[i]["climb_tower_max_order"].c_str());
            stValue.m_iClimbTowerResetCount = ::atoi(stData[i]["climb_tower_reset_count"].c_str());
            stValue.m_iLastClimbTowerResetTime = ::atoi(stData[i]["last_climb_tower_reset_time"].c_str());
            stValue.m_iClimbTowerSweepEndTime = ::atoi(stData[i]["climb_tower_sweep_end_time"].c_str());
            stValue.m_iClimbTowerHighestRank = ::atoi(stData[i]["climb_tower_highest_rank"].c_str());
            stValue.m_iSerialPayAwarded = ::atoi(stData[i]["serial_pay_awarded"].c_str());
            stValue.m_iCornucopiaProfitYuanbao = ::atoi(stData[i]["cornucopia_profit_yuanbao"].c_str());
            stValue.m_iGuildCoin = ::atoi(stData[i]["guild_coin"].c_str());
            stValue.m_iLastLeaveGuildTime = ::atoi(stData[i]["last_leave_guild_time"].c_str());
            stValue.m_iJubaoAwardCount = ::atoi(stData[i]["jubao_award_count"].c_str());
            stValue.m_iTodayJubaoAwardCount = ::atoi(stData[i]["today_jubao_award_count"].c_str());
            stValue.m_iLastJubaoAwardTime = ::atoi(stData[i]["last_jubao_award_time"].c_str());
            stValue.m_iWeiBuyNum = ::atoi(stData[i]["wei_buy_num"].c_str());
            stValue.m_iWeiAvgPrice = ::atoi(stData[i]["wei_avg_price"].c_str());
            stValue.m_iShuBuyNum = ::atoi(stData[i]["shu_buy_num"].c_str());
            stValue.m_iShuAvgPrice = ::atoi(stData[i]["shu_avg_price"].c_str());
            stValue.m_iWuBuyNum = ::atoi(stData[i]["wu_buy_num"].c_str());
            stValue.m_iWuAvgPrice = ::atoi(stData[i]["wu_avg_price"].c_str());
            stValue.m_iEntireWarContribution = ::atoi(stData[i]["entire_war_contribution"].c_str());
            stValue.m_iEntireWarMilitarkLevel = ::atoi(stData[i]["entire_war_militark_level"].c_str());
            stValue.m_iEntireWarWorshippedCount = ::atoi(stData[i]["entire_war_worshipped_count"].c_str());
            stValue.m_iLastEntireWarTime = ::atoi(stData[i]["last_entire_war_time"].c_str());
            stValue.m_iWeekWarContribution = ::atoi(stData[i]["week_war_contribution"].c_str());
            stValue.m_iWeekLimitBossScore = ::atoi(stData[i]["week_limit_boss_score"].c_str());
            stValue.m_iActionCoin = ::atoi(stData[i]["action_coin"].c_str());
            stValue.m_iLastAddActionTime = ::atoi(stData[i]["last_add_action_time"].c_str());
            stValue.m_ulEntireWarAwardBitmap = (uint64_t)strtoull(stData[i]["entire_war_award_bitmap"].c_str(), nullptr, 10);
            FormatDBBin(stData[i]["entire_war_node_award_stat"], stValue.m_stEntireWarNodeAwardStat);
            FormatDBBin(stData[i]["entire_war_extra_status"], stValue.m_stEntireWarExtraStatus);
            FormatDBBin(stData[i]["entire_war_node_para"], stValue.m_stEntireWarNodePara);
            stValue.m_iDayQiyuID = ::atoi(stData[i]["day_qiyu_id"].c_str());
            stValue.m_iCangbaotuQiyuID = ::atoi(stData[i]["cangbaotu_qiyu_id"].c_str());
            stValue.m_iQiyuEndtime = ::atoi(stData[i]["qiyu_endtime"].c_str());
            stValue.m_iCangbaotuEndtime = ::atoi(stData[i]["cangbaotu_endtime"].c_str());
            FormatDBBin(stData[i]["qiyu_buff"], stValue.m_stQiyuBuff);
            FormatDBBin(stData[i]["cangbaotu_buff"], stValue.m_stCangbaotuBuff);
            FormatDBBin(stData[i]["cangbaotu_suipian"], stValue.m_stCangbaotuSuipian);
            FormatDBBin(stData[i]["banquet_all_team_id"], stValue.m_stBanquetAllTeamID);
            stValue.m_iDataUpdateVersion = ::atoi(stData[i]["data_update_version"].c_str());
            stValue.m_iNewPassTotalExp = ::atoi(stData[i]["new_pass_total_exp"].c_str());
            FormatDBBin(stData[i]["new_pass_normal_level"], stValue.m_stNewPassNormalLevel);
            FormatDBBin(stData[i]["new_pass_advanced_level"], stValue.m_stNewPassAdvancedLevel);
            stValue.m_iLimitBossTotalPoint = ::atoi(stData[i]["limit_boss_total_point"].c_str());
            FormatDBBin(stData[i]["find_limit_boss"], stValue.m_stFindLimitBoss);
            stValue.m_iPvpScore = ::atoi(stData[i]["pvp_score"].c_str());
            stValue.m_iPvpHighestScore = ::atoi(stData[i]["pvp_highest_score"].c_str());
            stValue.m_iPvpRefreshCount = ::atoi(stData[i]["pvp_refresh_count"].c_str());
            FormatDBBin(stData[i]["pvp_weekly_task"], stValue.m_stPvpWeeklyTask);
            stValue.m_iHomeExp = ::atoi(stData[i]["home_exp"].c_str());
            FormatDBBin(stData[i]["home_exp_reward"], stValue.m_stHomeExpReward);
            stValue.m_cMonthAgeLimit = (char)::atoi(stData[i]["month_age_limit"].c_str());
            stValue.m_iLastMonthAgeChange = ::atoi(stData[i]["last_month_age_change"].c_str());
            stValue.m_iLastLimitCommodityID = ::atoi(stData[i]["last_limit_commodity_id"].c_str());
            stValue.m_cHotSpringRewardEmail = (char)::atoi(stData[i]["hot_spring_reward_email"].c_str());
            stValue.m_cHotSpringRewardOther = (char)::atoi(stData[i]["hot_spring_reward_other"].c_str());
            stValue.m_cHotSpringReward = (char)::atoi(stData[i]["hot_spring_reward"].c_str());
            stValue.m_cHotSpringInvite = (char)::atoi(stData[i]["hot_spring_invite"].c_str());
            stValue.m_cHotSpringJoin = (char)::atoi(stData[i]["hot_spring_join"].c_str());
            stValue.m_cIsBindEmail = (char)::atoi(stData[i]["is_bind_email"].c_str());
            stValue.m_cDailyGuildDonate = (char)::atoi(stData[i]["daily_guild_donate"].c_str());
            stValue.m_iGuildPersonDonate = ::atoi(stData[i]["guild_person_donate"].c_str());
            stValue.m_iGuildExtraLevel = ::atoi(stData[i]["guild_extra_level"].c_str());
            stValue.m_cDailySignFlag = (char)::atoi(stData[i]["daily_sign_flag"].c_str());
            FormatDBBin(stData[i]["daily_sign_reward"], stValue.m_stDailySignReward);
            FormatDBBin(stData[i]["complete_sign_reward"], stValue.m_stCompleteSignReward);
            stValue.m_iZhuhaiScore = ::atoi(stData[i]["zhuhai_score"].c_str());
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
