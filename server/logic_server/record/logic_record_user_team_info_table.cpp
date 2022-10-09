#include "logic_record_user_team_info_table.h"

#include <cassert>

const int CRecordUserTeamInfoTable::TABLE_DIV = 32;

const char* CRecordUserTeamInfoTable::s_strTeam1 = "team_1";
const char* CRecordUserTeamInfoTable::s_strTeam2 = "team_2";
const char* CRecordUserTeamInfoTable::s_strTeam3 = "team_3";
const char* CRecordUserTeamInfoTable::s_strTeam4 = "team_4";
const char* CRecordUserTeamInfoTable::s_strTeam5 = "team_5";
const char* CRecordUserTeamInfoTable::s_strTeam6 = "team_6";
const char* CRecordUserTeamInfoTable::s_strTeam7 = "team_7";
const char* CRecordUserTeamInfoTable::s_strTeam8 = "team_8";
const char* CRecordUserTeamInfoTable::s_strTeam9 = "team_9";
const char* CRecordUserTeamInfoTable::s_strTeam10 = "team_10";
const char* CRecordUserTeamInfoTable::s_strTeam11 = "team_11";
const char* CRecordUserTeamInfoTable::s_strTeam12 = "team_12";
const char* CRecordUserTeamInfoTable::s_strTeam13 = "team_13";
const char* CRecordUserTeamInfoTable::s_strTeam14 = "team_14";
const char* CRecordUserTeamInfoTable::s_strTeam15 = "team_15";
const char* CRecordUserTeamInfoTable::s_strTeam16 = "team_16";
const char* CRecordUserTeamInfoTable::s_strTeam17 = "team_17";
const char* CRecordUserTeamInfoTable::s_strTeam18 = "team_18";
const char* CRecordUserTeamInfoTable::s_strTeam19 = "team_19";
const char* CRecordUserTeamInfoTable::s_strTeam20 = "team_20";
const char* CRecordUserTeamInfoTable::s_strTeam21 = "team_21";
const char* CRecordUserTeamInfoTable::s_strTeam22 = "team_22";
const char* CRecordUserTeamInfoTable::s_strTeam23 = "team_23";
const char* CRecordUserTeamInfoTable::s_strTeam24 = "team_24";
const char* CRecordUserTeamInfoTable::s_strTeam25 = "team_25";
const char* CRecordUserTeamInfoTable::s_strTeam26 = "team_26";
const char* CRecordUserTeamInfoTable::s_strTeam27 = "team_27";
const char* CRecordUserTeamInfoTable::s_strTeam28 = "team_28";
const char* CRecordUserTeamInfoTable::s_strTeam29 = "team_29";
const char* CRecordUserTeamInfoTable::s_strTeam30 = "team_30";

CRecordUserTeamInfoTable::~CRecordUserTeamInfoTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordUserTeamInfoTable::MakeInsertSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO user_team_info_t_%d %s VALUES %s;" , iTableMod, strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordUserTeamInfoTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE user_team_info_t_%d SET %s WHERE uid=%d AND group_id=%d;", iTableMod, strSet.c_str(), m_iUid, m_iGroupID);
    return szBuffer;
}
std::string
CRecordUserTeamInfoTable::MakeDeleteSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM user_team_info_t_%d WHERE uid=%d AND group_id=%d;", iTableMod, m_iUid, m_iGroupID);
    return szBuffer;
}

int
CRecordUserTeamInfoTable::GetUid()
{
    return m_iUid;
}

int
CRecordUserTeamInfoTable::GetGroupID()
{
    return m_iGroupID;
}

std::vector<TUserTeamInfoTableValueType>
CRecordUserTeamInfoTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    int iTableMod = iUin % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM user_team_info_t_%d WHERE uid=%d AND group_id=%d;" , iTableMod, iUin, iGroupID);
    std::vector<TUserTeamInfoTableValueType> stRet;
    TUserTeamInfoTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            FormatDBBin(stData[i]["team_1"], stValue.m_stTeam1);
            FormatDBBin(stData[i]["team_2"], stValue.m_stTeam2);
            FormatDBBin(stData[i]["team_3"], stValue.m_stTeam3);
            FormatDBBin(stData[i]["team_4"], stValue.m_stTeam4);
            FormatDBBin(stData[i]["team_5"], stValue.m_stTeam5);
            FormatDBBin(stData[i]["team_6"], stValue.m_stTeam6);
            FormatDBBin(stData[i]["team_7"], stValue.m_stTeam7);
            FormatDBBin(stData[i]["team_8"], stValue.m_stTeam8);
            FormatDBBin(stData[i]["team_9"], stValue.m_stTeam9);
            FormatDBBin(stData[i]["team_10"], stValue.m_stTeam10);
            FormatDBBin(stData[i]["team_11"], stValue.m_stTeam11);
            FormatDBBin(stData[i]["team_12"], stValue.m_stTeam12);
            FormatDBBin(stData[i]["team_13"], stValue.m_stTeam13);
            FormatDBBin(stData[i]["team_14"], stValue.m_stTeam14);
            FormatDBBin(stData[i]["team_15"], stValue.m_stTeam15);
            FormatDBBin(stData[i]["team_16"], stValue.m_stTeam16);
            FormatDBBin(stData[i]["team_17"], stValue.m_stTeam17);
            FormatDBBin(stData[i]["team_18"], stValue.m_stTeam18);
            FormatDBBin(stData[i]["team_19"], stValue.m_stTeam19);
            FormatDBBin(stData[i]["team_20"], stValue.m_stTeam20);
            FormatDBBin(stData[i]["team_21"], stValue.m_stTeam21);
            FormatDBBin(stData[i]["team_22"], stValue.m_stTeam22);
            FormatDBBin(stData[i]["team_23"], stValue.m_stTeam23);
            FormatDBBin(stData[i]["team_24"], stValue.m_stTeam24);
            FormatDBBin(stData[i]["team_25"], stValue.m_stTeam25);
            FormatDBBin(stData[i]["team_26"], stValue.m_stTeam26);
            FormatDBBin(stData[i]["team_27"], stValue.m_stTeam27);
            FormatDBBin(stData[i]["team_28"], stValue.m_stTeam28);
            FormatDBBin(stData[i]["team_29"], stValue.m_stTeam29);
            FormatDBBin(stData[i]["team_30"], stValue.m_stTeam30);
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
