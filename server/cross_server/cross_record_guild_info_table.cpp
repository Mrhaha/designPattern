#include "cross_record_guild_info_table.h"

#include <cassert>


const char* CRecordGuildInfoTable::s_strGuildName = "guild_name";
const char* CRecordGuildInfoTable::s_strGuildExp = "guild_exp";
const char* CRecordGuildInfoTable::s_strChiefUid = "chief_uid";
const char* CRecordGuildInfoTable::s_strChiefGroup = "chief_group";
const char* CRecordGuildInfoTable::s_strBulletin = "bulletin";
const char* CRecordGuildInfoTable::s_strGuildBossTotalHurt = "guild_boss_total_hurt";
const char* CRecordGuildInfoTable::s_strLastRefreshTime = "last_refresh_time";
const char* CRecordGuildInfoTable::s_strChangeNameCd = "change_name_cd";
const char* CRecordGuildInfoTable::s_strChangeNameFree = "change_name_free";
const char* CRecordGuildInfoTable::s_strGuildDonate = "guild_donate";
const char* CRecordGuildInfoTable::s_strGuildSignRewardLevel = "guild_sign_reward_level";
const char* CRecordGuildInfoTable::s_strGuildUnlockItem = "guild_unlock_item";
const char* CRecordGuildInfoTable::s_strGuildUnlockDecorate = "guild_unlock_decorate";
const char* CRecordGuildInfoTable::s_strGuildFootCloth = "guild_footCloth";
const char* CRecordGuildInfoTable::s_strGuildDecorate = "guild_decorate";
const char* CRecordGuildInfoTable::s_strGuildWarBossInfo = "guild_war_boss_info";
const char* CRecordGuildInfoTable::s_strGuildWarRecord = "guild_war_record";
const char* CRecordGuildInfoTable::s_strGuildWarScore = "guild_war_score";
const char* CRecordGuildInfoTable::s_strGuildWarEnd = "guild_war_end";

CRecordGuildInfoTable::~CRecordGuildInfoTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordGuildInfoTable::MakeInsertSQL()
{
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "guild_id" + std::string(",");
    strVal += Int2DBString(m_iGuildID) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO guild_info_t %s VALUES %s;" , strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordGuildInfoTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE guild_info_t SET %s WHERE guild_id=%d;", strSet.c_str(), m_iGuildID);
    return szBuffer;
}
std::string
CRecordGuildInfoTable::MakeDeleteSQL()
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM guild_info_t WHERE guild_id=%d;", m_iGuildID);
    return szBuffer;
}

int
CRecordGuildInfoTable::GetGuildID()
{
    return m_iGuildID;
}

std::vector<TGuildInfoTableValueType>
CRecordGuildInfoTable::SelectDataFromSQL(int iGuildID)
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM guild_info_t WHERE guild_id=%d;" , iGuildID);
    std::vector<TGuildInfoTableValueType> stRet;
    TGuildInfoTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iGuildID = ::atoi(stData[i]["guild_id"].c_str());
            stValue.m_strGuildName = stData[i]["guild_name"];
            stValue.m_iGuildExp = ::atoi(stData[i]["guild_exp"].c_str());
            stValue.m_iChiefUid = ::atoi(stData[i]["chief_uid"].c_str());
            stValue.m_iChiefGroup = ::atoi(stData[i]["chief_group"].c_str());
            stValue.m_strBulletin = stData[i]["bulletin"];
            stValue.m_ulGuildBossTotalHurt = (uint64_t)strtoull(stData[i]["guild_boss_total_hurt"].c_str(), nullptr, 10);
            stValue.m_iLastRefreshTime = ::atoi(stData[i]["last_refresh_time"].c_str());
            stValue.m_iChangeNameCd = ::atoi(stData[i]["change_name_cd"].c_str());
            stValue.m_cChangeNameFree = (char)::atoi(stData[i]["change_name_free"].c_str());
            stValue.m_iGuildDonate = ::atoi(stData[i]["guild_donate"].c_str());
            stValue.m_iGuildSignRewardLevel = ::atoi(stData[i]["guild_sign_reward_level"].c_str());
            FormatDBBin(stData[i]["guild_unlock_item"], stValue.m_stGuildUnlockItem);
            FormatDBBin(stData[i]["guild_unlock_decorate"], stValue.m_stGuildUnlockDecorate);
            FormatDBBin(stData[i]["guild_footCloth"], stValue.m_stGuildFootCloth);
            FormatDBBin(stData[i]["guild_decorate"], stValue.m_stGuildDecorate);
            FormatDBBin(stData[i]["guild_war_boss_info"], stValue.m_stGuildWarBossInfo);
            FormatDBBin(stData[i]["guild_war_record"], stValue.m_stGuildWarRecord);
            stValue.m_ulGuildWarScore = (uint64_t)strtoull(stData[i]["guild_war_score"].c_str(), nullptr, 10);
            stValue.m_iGuildWarEnd = ::atoi(stData[i]["guild_war_end"].c_str());
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
