#include "logic_record_global_basic_table.h"

#include <cassert>


const char* CRecordGlobalBasicTable::s_strPrimaryKeyIndex = "primary_key_index";
const char* CRecordGlobalBasicTable::s_strLastAwardRankTime = "last_award_rank_time";
const char* CRecordGlobalBasicTable::s_strFightPowerAwardRankTime = "fight_power_award_rank_time";
const char* CRecordGlobalBasicTable::s_strLastMergeServerTime = "last_merge_server_time";
const char* CRecordGlobalBasicTable::s_strCountdownHeroActivityID = "countdown_hero_activity_id";
const char* CRecordGlobalBasicTable::s_strCheckUpdateTime = "check_update_time";
const char* CRecordGlobalBasicTable::s_strEntireLastUpdateTime = "entire_last_update_time";
const char* CRecordGlobalBasicTable::s_strEntireWarLevel = "entire_war_level";
const char* CRecordGlobalBasicTable::s_strEntireWarThemeID = "entire_war_theme_id";
const char* CRecordGlobalBasicTable::s_strEntireWarGodUid = "entire_war_god_uid";
const char* CRecordGlobalBasicTable::s_strEntireWarGodGroupID = "entire_war_god_group_id";
const char* CRecordGlobalBasicTable::s_strEntireWarBuff = "entire_war_buff";
const char* CRecordGlobalBasicTable::s_strNewsData = "news_data";
const char* CRecordGlobalBasicTable::s_strAffixVec = "affix_vec";
const char* CRecordGlobalBasicTable::s_strGuildWarCurBoss = "guild_war_cur_boss";

CRecordGlobalBasicTable::~CRecordGlobalBasicTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordGlobalBasicTable::MakeInsertSQL()
{
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "server_id" + std::string(",");
    strVal += Int2DBString(m_iServerID) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO global_basic_t %s VALUES %s;" , strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordGlobalBasicTable::MakeUpdateSQL()
{
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE global_basic_t SET %s WHERE server_id=%d;", strSet.c_str(), m_iServerID);
    return szBuffer;
}
std::string
CRecordGlobalBasicTable::MakeDeleteSQL()
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM global_basic_t WHERE server_id=%d;", m_iServerID);
    return szBuffer;
}

int
CRecordGlobalBasicTable::GetServerID()
{
    return m_iServerID;
}

std::vector<TGlobalBasicTableValueType>
CRecordGlobalBasicTable::SelectDataFromSQL(int iServerID)
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM global_basic_t WHERE server_id=%d;" , iServerID);
    std::vector<TGlobalBasicTableValueType> stRet;
    TGlobalBasicTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iServerID = ::atoi(stData[i]["server_id"].c_str());
            stValue.m_iPrimaryKeyIndex = ::atoi(stData[i]["primary_key_index"].c_str());
            stValue.m_iLastAwardRankTime = ::atoi(stData[i]["last_award_rank_time"].c_str());
            stValue.m_iFightPowerAwardRankTime = ::atoi(stData[i]["fight_power_award_rank_time"].c_str());
            stValue.m_iLastMergeServerTime = ::atoi(stData[i]["last_merge_server_time"].c_str());
            stValue.m_iCountdownHeroActivityID = ::atoi(stData[i]["countdown_hero_activity_id"].c_str());
            stValue.m_iCheckUpdateTime = ::atoi(stData[i]["check_update_time"].c_str());
            stValue.m_iEntireLastUpdateTime = ::atoi(stData[i]["entire_last_update_time"].c_str());
            stValue.m_iEntireWarLevel = ::atoi(stData[i]["entire_war_level"].c_str());
            stValue.m_iEntireWarThemeID = ::atoi(stData[i]["entire_war_theme_id"].c_str());
            stValue.m_iEntireWarGodUid = ::atoi(stData[i]["entire_war_god_uid"].c_str());
            stValue.m_iEntireWarGodGroupID = ::atoi(stData[i]["entire_war_god_group_id"].c_str());
            FormatDBBin(stData[i]["entire_war_buff"], stValue.m_stEntireWarBuff);
            FormatDBBin(stData[i]["news_data"], stValue.m_stNewsData);
            FormatDBBin(stData[i]["affix_vec"], stValue.m_stAffixVec);
            FormatDBBin(stData[i]["guild_war_cur_boss"], stValue.m_stGuildWarCurBoss);
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
