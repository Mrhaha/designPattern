#include "logic_record_user_daily_table.h"

#include <cassert>

const int CRecordUserDailyTable::TABLE_DIV = 32;

const char* CRecordUserDailyTable::s_strLastRefreshTime = "last_refresh_time";
const char* CRecordUserDailyTable::s_strFreeTurnTableCount = "free_turn_table_count";
const char* CRecordUserDailyTable::s_strFreeCountdownHeroTableCount = "free_countdown_hero_table_count";
const char* CRecordUserDailyTable::s_strCostEnergy = "cost_energy";
const char* CRecordUserDailyTable::s_strLeftTurnCardCount = "left_turn_card_count";
const char* CRecordUserDailyTable::s_strTurnCardBitmap = "turn_card_bitmap";
const char* CRecordUserDailyTable::s_strTurnCardAwardBitmap = "turn_card_award_bitmap";
const char* CRecordUserDailyTable::s_strVipDailyAwardBitmap = "vip_daily_award_bitmap";
const char* CRecordUserDailyTable::s_strWorshipWarGodCount = "worship_war_god_count";
const char* CRecordUserDailyTable::s_strEntireWarWorshippedCount = "entire_war_worshipped_count";
const char* CRecordUserDailyTable::s_strEntireWarDailyGameTimes = "entire_war_daily_game_times";
const char* CRecordUserDailyTable::s_strBuyActionCoinCount = "buy_action_coin_count";
const char* CRecordUserDailyTable::s_strJubaoCount = "jubao_count";
const char* CRecordUserDailyTable::s_strMergeLevelCount = "merge_level_count";
const char* CRecordUserDailyTable::s_strLuckyHammerLogCount = "lucky_hammer_log_count";
const char* CRecordUserDailyTable::s_strQuarterVipAwardLevel = "quarter_vip_award_level";
const char* CRecordUserDailyTable::s_strDailyAwardState = "daily_award_state";
const char* CRecordUserDailyTable::s_strHeartDialogCard = "heart_dialog_card";
const char* CRecordUserDailyTable::s_strDatingPlaceTimes = "dating_place_times";
const char* CRecordUserDailyTable::s_strSimpleGameTimes = "simple_game_times";
const char* CRecordUserDailyTable::s_strFlyArrayReward = "fly_array_reward";
const char* CRecordUserDailyTable::s_strFlyArrayRewardState = "fly_array_reward_state";
const char* CRecordUserDailyTable::s_strRedBagEarnings = "red_bag_earnings";
const char* CRecordUserDailyTable::s_strLimitBossDailyNum = "limit_boss_daily_num";
const char* CRecordUserDailyTable::s_strLimitBoxDailyNum = "limit_box_daily_num";
const char* CRecordUserDailyTable::s_strLimitBossAction = "limit_boss_action";
const char* CRecordUserDailyTable::s_strFishingTimes = "fishing_times";
const char* CRecordUserDailyTable::s_strWishSwapTimes = "wish_swap_times";

CRecordUserDailyTable::~CRecordUserDailyTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordUserDailyTable::MakeInsertSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO user_daily_t_%d %s VALUES %s;" , iTableMod, strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordUserDailyTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE user_daily_t_%d SET %s WHERE uid=%d AND group_id=%d;", iTableMod, strSet.c_str(), m_iUid, m_iGroupID);
    return szBuffer;
}
std::string
CRecordUserDailyTable::MakeDeleteSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM user_daily_t_%d WHERE uid=%d AND group_id=%d;", iTableMod, m_iUid, m_iGroupID);
    return szBuffer;
}

int
CRecordUserDailyTable::GetUid()
{
    return m_iUid;
}

int
CRecordUserDailyTable::GetGroupID()
{
    return m_iGroupID;
}

std::vector<TUserDailyTableValueType>
CRecordUserDailyTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    int iTableMod = iUin % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM user_daily_t_%d WHERE uid=%d AND group_id=%d;" , iTableMod, iUin, iGroupID);
    std::vector<TUserDailyTableValueType> stRet;
    TUserDailyTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iLastRefreshTime = ::atoi(stData[i]["last_refresh_time"].c_str());
            stValue.m_iFreeTurnTableCount = ::atoi(stData[i]["free_turn_table_count"].c_str());
            stValue.m_iFreeCountdownHeroTableCount = ::atoi(stData[i]["free_countdown_hero_table_count"].c_str());
            stValue.m_iCostEnergy = ::atoi(stData[i]["cost_energy"].c_str());
            stValue.m_iLeftTurnCardCount = ::atoi(stData[i]["left_turn_card_count"].c_str());
            stValue.m_ulTurnCardBitmap = (uint64_t)strtoull(stData[i]["turn_card_bitmap"].c_str(), nullptr, 10);
            stValue.m_ulTurnCardAwardBitmap = (uint64_t)strtoull(stData[i]["turn_card_award_bitmap"].c_str(), nullptr, 10);
            stValue.m_ulVipDailyAwardBitmap = (uint64_t)strtoull(stData[i]["vip_daily_award_bitmap"].c_str(), nullptr, 10);
            stValue.m_iWorshipWarGodCount = ::atoi(stData[i]["worship_war_god_count"].c_str());
            stValue.m_iEntireWarWorshippedCount = ::atoi(stData[i]["entire_war_worshipped_count"].c_str());
            stValue.m_iEntireWarDailyGameTimes = ::atoi(stData[i]["entire_war_daily_game_times"].c_str());
            stValue.m_iBuyActionCoinCount = ::atoi(stData[i]["buy_action_coin_count"].c_str());
            stValue.m_iJubaoCount = ::atoi(stData[i]["jubao_count"].c_str());
            stValue.m_iMergeLevelCount = ::atoi(stData[i]["merge_level_count"].c_str());
            stValue.m_iLuckyHammerLogCount = ::atoi(stData[i]["lucky_hammer_log_count"].c_str());
            stValue.m_iQuarterVipAwardLevel = ::atoi(stData[i]["quarter_vip_award_level"].c_str());
            stValue.m_ulDailyAwardState = (uint64_t)strtoull(stData[i]["daily_award_state"].c_str(), nullptr, 10);
            stValue.m_iHeartDialogCard = ::atoi(stData[i]["heart_dialog_card"].c_str());
            FormatDBBin(stData[i]["dating_place_times"], stValue.m_stDatingPlaceTimes);
            FormatDBBin(stData[i]["simple_game_times"], stValue.m_stSimpleGameTimes);
            FormatDBBin(stData[i]["fly_array_reward"], stValue.m_stFlyArrayReward);
            stValue.m_ulFlyArrayRewardState = (uint64_t)strtoull(stData[i]["fly_array_reward_state"].c_str(), nullptr, 10);
            FormatDBBin(stData[i]["red_bag_earnings"], stValue.m_stRedBagEarnings);
            stValue.m_cLimitBossDailyNum = (char)::atoi(stData[i]["limit_boss_daily_num"].c_str());
            stValue.m_cLimitBoxDailyNum = (char)::atoi(stData[i]["limit_box_daily_num"].c_str());
            stValue.m_iLimitBossAction = ::atoi(stData[i]["limit_boss_action"].c_str());
            stValue.m_iFishingTimes = ::atoi(stData[i]["fishing_times"].c_str());
            stValue.m_iWishSwapTimes = ::atoi(stData[i]["wish_swap_times"].c_str());
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
