#include "logic_record_level_info_table.h"

#include <cassert>

const int CRecordLevelInfoTable::TABLE_DIV = 32;

const char* CRecordLevelInfoTable::s_strLevelStar = "level_star";
const char* CRecordLevelInfoTable::s_strSharedTimes = "shared_times";
const char* CRecordLevelInfoTable::s_strPlayTimes = "play_times";
const char* CRecordLevelInfoTable::s_strLastPlayTime = "last_play_time";
const char* CRecordLevelInfoTable::s_strResetTimes = "reset_times";
const char* CRecordLevelInfoTable::s_strLastResetTime = "last_reset_time";
const char* CRecordLevelInfoTable::s_strTotalVictoryTimes = "total_victory_times";
const char* CRecordLevelInfoTable::s_strCostTime = "cost_time";
const char* CRecordLevelInfoTable::s_strAchieveTag = "achieve_tag";
const char* CRecordLevelInfoTable::s_strBonusTag = "bonus_tag";
const char* CRecordLevelInfoTable::s_strAdvRewardTimes = "adv_reward_times";

CRecordLevelInfoTable::~CRecordLevelInfoTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordLevelInfoTable::MakeInsertSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "uid" + std::string(",");
    strVal += Int2DBString(m_iUid) + ",";
    strCol += "group_id" + std::string(",");
    strVal += Int2DBString(m_iGroupID) + ",";
    strCol += "level_id" + std::string(",");
    strVal += Int2DBString(m_iLevelID) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO level_info_t_%d %s VALUES %s;" , iTableMod, strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordLevelInfoTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE level_info_t_%d SET %s WHERE uid=%d AND group_id=%d AND level_id=%d;", iTableMod, strSet.c_str(), m_iUid, m_iGroupID, m_iLevelID);
    return szBuffer;
}
std::string
CRecordLevelInfoTable::MakeDeleteSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM level_info_t_%d WHERE uid=%d AND group_id=%d AND level_id=%d;", iTableMod, m_iUid, m_iGroupID, m_iLevelID);
    return szBuffer;
}

int
CRecordLevelInfoTable::GetUid()
{
    return m_iUid;
}

int
CRecordLevelInfoTable::GetGroupID()
{
    return m_iGroupID;
}

int
CRecordLevelInfoTable::GetLevelID()
{
    return m_iLevelID;
}

std::vector<TLevelInfoTableValueType>
CRecordLevelInfoTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    int iTableMod = iUin % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM level_info_t_%d WHERE uid=%d AND group_id=%d;" , iTableMod, iUin, iGroupID);
    std::vector<TLevelInfoTableValueType> stRet;
    TLevelInfoTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iLevelID = ::atoi(stData[i]["level_id"].c_str());
            stValue.m_cLevelStar = (char)::atoi(stData[i]["level_star"].c_str());
            stValue.m_iSharedTimes = ::atoi(stData[i]["shared_times"].c_str());
            stValue.m_iPlayTimes = ::atoi(stData[i]["play_times"].c_str());
            stValue.m_iLastPlayTime = ::atoi(stData[i]["last_play_time"].c_str());
            stValue.m_iResetTimes = ::atoi(stData[i]["reset_times"].c_str());
            stValue.m_iLastResetTime = ::atoi(stData[i]["last_reset_time"].c_str());
            stValue.m_iTotalVictoryTimes = ::atoi(stData[i]["total_victory_times"].c_str());
            stValue.m_iCostTime = ::atoi(stData[i]["cost_time"].c_str());
            stValue.m_cAchieveTag = (char)::atoi(stData[i]["achieve_tag"].c_str());
            stValue.m_cBonusTag = (char)::atoi(stData[i]["bonus_tag"].c_str());
            stValue.m_iAdvRewardTimes = ::atoi(stData[i]["adv_reward_times"].c_str());
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
