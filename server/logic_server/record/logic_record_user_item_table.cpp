#include "logic_record_user_item_table.h"

#include <cassert>

const int CRecordUserItemTable::TABLE_DIV = 32;

const char* CRecordUserItemTable::s_strAmount = "amount";
const char* CRecordUserItemTable::s_strAddUpAmount = "add_up_amount";
const char* CRecordUserItemTable::s_strOpenedCount = "opened_count";
const char* CRecordUserItemTable::s_strNextOpenTime = "next_open_time";

CRecordUserItemTable::~CRecordUserItemTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordUserItemTable::MakeInsertSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "uid" + std::string(",");
    strVal += Int2DBString(m_iUid) + ",";
    strCol += "group_id" + std::string(",");
    strVal += Int2DBString(m_iGroupID) + ",";
    strCol += "item_id" + std::string(",");
    strVal += Int2DBString(m_iItemID) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO user_item_t_%d %s VALUES %s;" , iTableMod, strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordUserItemTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE user_item_t_%d SET %s WHERE uid=%d AND group_id=%d AND item_id=%d;", iTableMod, strSet.c_str(), m_iUid, m_iGroupID, m_iItemID);
    return szBuffer;
}
std::string
CRecordUserItemTable::MakeDeleteSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM user_item_t_%d WHERE uid=%d AND group_id=%d AND item_id=%d;", iTableMod, m_iUid, m_iGroupID, m_iItemID);
    return szBuffer;
}

int
CRecordUserItemTable::GetUid()
{
    return m_iUid;
}

int
CRecordUserItemTable::GetGroupID()
{
    return m_iGroupID;
}

int
CRecordUserItemTable::GetItemID()
{
    return m_iItemID;
}

std::vector<TUserItemTableValueType>
CRecordUserItemTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    int iTableMod = iUin % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM user_item_t_%d WHERE uid=%d AND group_id=%d;" , iTableMod, iUin, iGroupID);
    std::vector<TUserItemTableValueType> stRet;
    TUserItemTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iItemID = ::atoi(stData[i]["item_id"].c_str());
            stValue.m_iAmount = ::atoi(stData[i]["amount"].c_str());
            stValue.m_iAddUpAmount = ::atoi(stData[i]["add_up_amount"].c_str());
            stValue.m_iOpenedCount = ::atoi(stData[i]["opened_count"].c_str());
            stValue.m_iNextOpenTime = ::atoi(stData[i]["next_open_time"].c_str());
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
