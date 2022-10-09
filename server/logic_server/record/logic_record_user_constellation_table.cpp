#include "logic_record_user_constellation_table.h"

#include <cassert>

const int CRecordUserConstellationTable::TABLE_DIV = 32;

const char* CRecordUserConstellationTable::s_strGrade = "grade";
const char* CRecordUserConstellationTable::s_strEquipmentTag = "equipment_tag";
const char* CRecordUserConstellationTable::s_strSuit = "suit";
const char* CRecordUserConstellationTable::s_strLevel = "level";
const char* CRecordUserConstellationTable::s_strExp = "exp";
const char* CRecordUserConstellationTable::s_strTotalExp = "total_exp";
const char* CRecordUserConstellationTable::s_strEquipCardID = "equip_card_id";
const char* CRecordUserConstellationTable::s_strIsLocked = "is_locked";
const char* CRecordUserConstellationTable::s_strIsDiscard = "is_discard";
const char* CRecordUserConstellationTable::s_strMainAttrType = "main_attr_type";
const char* CRecordUserConstellationTable::s_strExAttr = "ex_attr";

CRecordUserConstellationTable::~CRecordUserConstellationTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordUserConstellationTable::MakeInsertSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "uid" + std::string(",");
    strVal += Int2DBString(m_iUid) + ",";
    strCol += "group_id" + std::string(",");
    strVal += Int2DBString(m_iGroupID) + ",";
    strCol += "unique_id" + std::string(",");
    strVal += Int2DBString(m_iUniqueID) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO user_constellation_t_%d %s VALUES %s;" , iTableMod, strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordUserConstellationTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE user_constellation_t_%d SET %s WHERE uid=%d AND group_id=%d AND unique_id=%d;", iTableMod, strSet.c_str(), m_iUid, m_iGroupID, m_iUniqueID);
    return szBuffer;
}
std::string
CRecordUserConstellationTable::MakeDeleteSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM user_constellation_t_%d WHERE uid=%d AND group_id=%d AND unique_id=%d;", iTableMod, m_iUid, m_iGroupID, m_iUniqueID);
    return szBuffer;
}

int
CRecordUserConstellationTable::GetUid()
{
    return m_iUid;
}

int
CRecordUserConstellationTable::GetGroupID()
{
    return m_iGroupID;
}

int
CRecordUserConstellationTable::GetUniqueID()
{
    return m_iUniqueID;
}

std::vector<TUserConstellationTableValueType>
CRecordUserConstellationTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    int iTableMod = iUin % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM user_constellation_t_%d WHERE uid=%d AND group_id=%d;" , iTableMod, iUin, iGroupID);
    std::vector<TUserConstellationTableValueType> stRet;
    TUserConstellationTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iUniqueID = ::atoi(stData[i]["unique_id"].c_str());
            stValue.m_iGrade = ::atoi(stData[i]["grade"].c_str());
            stValue.m_iEquipmentTag = ::atoi(stData[i]["equipment_tag"].c_str());
            stValue.m_iSuit = ::atoi(stData[i]["suit"].c_str());
            stValue.m_iLevel = ::atoi(stData[i]["level"].c_str());
            stValue.m_iExp = ::atoi(stData[i]["exp"].c_str());
            stValue.m_iTotalExp = ::atoi(stData[i]["total_exp"].c_str());
            stValue.m_iEquipCardID = ::atoi(stData[i]["equip_card_id"].c_str());
            stValue.m_cIsLocked = (char)::atoi(stData[i]["is_locked"].c_str());
            stValue.m_cIsDiscard = (char)::atoi(stData[i]["is_discard"].c_str());
            stValue.m_iMainAttrType = ::atoi(stData[i]["main_attr_type"].c_str());
            FormatDBBin(stData[i]["ex_attr"], stValue.m_stExAttr);
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
