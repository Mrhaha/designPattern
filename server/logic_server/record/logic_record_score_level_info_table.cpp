#include "logic_record_score_level_info_table.h"

#include <cassert>

const int CRecordScoreLevelInfoTable::TABLE_DIV = 32;

const char* CRecordScoreLevelInfoTable::s_strXingdongli = "xingdongli";
const char* CRecordScoreLevelInfoTable::s_strLastXingdongliTime = "last_xingdongli_time";
const char* CRecordScoreLevelInfoTable::s_strBuyXingdongliCount = "buy_xingdongli_count";
const char* CRecordScoreLevelInfoTable::s_strScore = "score";
const char* CRecordScoreLevelInfoTable::s_strScoreAwardBitmap = "score_award_bitmap";
const char* CRecordScoreLevelInfoTable::s_strDepositAwardBitmap = "deposit_award_bitmap";
const char* CRecordScoreLevelInfoTable::s_strScoreLevelBitmap = "score_level_bitmap";
const char* CRecordScoreLevelInfoTable::s_strChallengeLevelBitmap = "challenge_level_bitmap";

CRecordScoreLevelInfoTable::~CRecordScoreLevelInfoTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordScoreLevelInfoTable::MakeInsertSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "uid" + std::string(",");
    strVal += Int2DBString(m_iUid) + ",";
    strCol += "group_id" + std::string(",");
    strVal += Int2DBString(m_iGroupID) + ",";
    strCol += "activity_id" + std::string(",");
    strVal += Int2DBString(m_iActivityID) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO score_level_info_t_%d %s VALUES %s;" , iTableMod, strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordScoreLevelInfoTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE score_level_info_t_%d SET %s WHERE uid=%d AND group_id=%d AND activity_id=%d;", iTableMod, strSet.c_str(), m_iUid, m_iGroupID, m_iActivityID);
    return szBuffer;
}
std::string
CRecordScoreLevelInfoTable::MakeDeleteSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM score_level_info_t_%d WHERE uid=%d AND group_id=%d AND activity_id=%d;", iTableMod, m_iUid, m_iGroupID, m_iActivityID);
    return szBuffer;
}

int
CRecordScoreLevelInfoTable::GetUid()
{
    return m_iUid;
}

int
CRecordScoreLevelInfoTable::GetGroupID()
{
    return m_iGroupID;
}

int
CRecordScoreLevelInfoTable::GetActivityID()
{
    return m_iActivityID;
}

std::vector<TScoreLevelInfoTableValueType>
CRecordScoreLevelInfoTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    int iTableMod = iUin % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM score_level_info_t_%d WHERE uid=%d AND group_id=%d;" , iTableMod, iUin, iGroupID);
    std::vector<TScoreLevelInfoTableValueType> stRet;
    TScoreLevelInfoTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iActivityID = ::atoi(stData[i]["activity_id"].c_str());
            stValue.m_iXingdongli = ::atoi(stData[i]["xingdongli"].c_str());
            stValue.m_iLastXingdongliTime = ::atoi(stData[i]["last_xingdongli_time"].c_str());
            stValue.m_iBuyXingdongliCount = ::atoi(stData[i]["buy_xingdongli_count"].c_str());
            stValue.m_iScore = ::atoi(stData[i]["score"].c_str());
            stValue.m_ulScoreAwardBitmap = (uint64_t)strtoull(stData[i]["score_award_bitmap"].c_str(), nullptr, 10);
            stValue.m_ulDepositAwardBitmap = (uint64_t)strtoull(stData[i]["deposit_award_bitmap"].c_str(), nullptr, 10);
            stValue.m_ulScoreLevelBitmap = (uint64_t)strtoull(stData[i]["score_level_bitmap"].c_str(), nullptr, 10);
            stValue.m_ulChallengeLevelBitmap = (uint64_t)strtoull(stData[i]["challenge_level_bitmap"].c_str(), nullptr, 10);
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
