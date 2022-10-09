#include "logic_record_global_entire_extra_table.h"

#include <cassert>


const char* CRecordGlobalEntireExtraTable::s_strCommodityItemType = "commodity_item_type";
const char* CRecordGlobalEntireExtraTable::s_strCommodityItemID = "commodity_item_id";
const char* CRecordGlobalEntireExtraTable::s_strCommodityItemNum = "commodity_item_num";
const char* CRecordGlobalEntireExtraTable::s_strConsumeItemType = "consume_item_type";
const char* CRecordGlobalEntireExtraTable::s_strConsumeItemID = "consume_item_id";
const char* CRecordGlobalEntireExtraTable::s_strConsumeItemNum = "consume_item_num";
const char* CRecordGlobalEntireExtraTable::s_strDiscount = "discount";

CRecordGlobalEntireExtraTable::~CRecordGlobalEntireExtraTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordGlobalEntireExtraTable::MakeInsertSQL()
{
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "server_id" + std::string(",");
    strVal += Int2DBString(m_iServerID) + ",";
    strCol += "war_id" + std::string(",");
    strVal += Int2DBString(m_iWarID) + ",";
    strCol += "extra_id" + std::string(",");
    strVal += Int2DBString(m_iExtraID) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO global_entire_extra_t %s VALUES %s;" , strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordGlobalEntireExtraTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE global_entire_extra_t SET %s WHERE server_id=%d AND war_id=%d AND extra_id=%d;", strSet.c_str(), m_iServerID, m_iWarID, m_iExtraID);
    return szBuffer;
}
std::string
CRecordGlobalEntireExtraTable::MakeDeleteSQL()
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM global_entire_extra_t WHERE server_id=%d AND war_id=%d AND extra_id=%d;", m_iServerID, m_iWarID, m_iExtraID);
    return szBuffer;
}

int
CRecordGlobalEntireExtraTable::GetServerID()
{
    return m_iServerID;
}

int
CRecordGlobalEntireExtraTable::GetWarID()
{
    return m_iWarID;
}

int
CRecordGlobalEntireExtraTable::GetExtraID()
{
    return m_iExtraID;
}

std::vector<TGlobalEntireExtraTableValueType>
CRecordGlobalEntireExtraTable::SelectDataFromSQL(int iServerID)
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM global_entire_extra_t WHERE server_id=%d;" , iServerID);
    std::vector<TGlobalEntireExtraTableValueType> stRet;
    TGlobalEntireExtraTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iServerID = ::atoi(stData[i]["server_id"].c_str());
            stValue.m_iWarID = ::atoi(stData[i]["war_id"].c_str());
            stValue.m_iExtraID = ::atoi(stData[i]["extra_id"].c_str());
            stValue.m_iCommodityItemType = ::atoi(stData[i]["commodity_item_type"].c_str());
            stValue.m_iCommodityItemID = ::atoi(stData[i]["commodity_item_id"].c_str());
            stValue.m_iCommodityItemNum = ::atoi(stData[i]["commodity_item_num"].c_str());
            stValue.m_iConsumeItemType = ::atoi(stData[i]["consume_item_type"].c_str());
            stValue.m_iConsumeItemID = ::atoi(stData[i]["consume_item_id"].c_str());
            stValue.m_iConsumeItemNum = ::atoi(stData[i]["consume_item_num"].c_str());
            stValue.m_iDiscount = ::atoi(stData[i]["discount"].c_str());
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
