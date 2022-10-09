#include "logic_record_user_ibusy_market_table.h"

#include <cassert>

const int CRecordUserIBusyMarketTable::TABLE_DIV = 32;

const char* CRecordUserIBusyMarketTable::s_strLastBuyTime = "last_buy_time";
const char* CRecordUserIBusyMarketTable::s_strCommodityItemType = "commodity_item_type";
const char* CRecordUserIBusyMarketTable::s_strCommodityItemID = "commodity_item_id";
const char* CRecordUserIBusyMarketTable::s_strCommodityItemNum = "commodity_item_num";
const char* CRecordUserIBusyMarketTable::s_strConsumeItemType = "consume_item_type";
const char* CRecordUserIBusyMarketTable::s_strConsumeItemID = "consume_item_id";
const char* CRecordUserIBusyMarketTable::s_strConsumeItemNum = "consume_item_num";
const char* CRecordUserIBusyMarketTable::s_strDiscount = "discount";

CRecordUserIBusyMarketTable::~CRecordUserIBusyMarketTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordUserIBusyMarketTable::MakeInsertSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "uid" + std::string(",");
    strVal += Int2DBString(m_iUid) + ",";
    strCol += "group_id" + std::string(",");
    strVal += Int2DBString(m_iGroupID) + ",";
    strCol += "commodity_id" + std::string(",");
    strVal += Int2DBString(m_iCommodityID) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO ibusy_market_t_%d %s VALUES %s;" , iTableMod, strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordUserIBusyMarketTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE ibusy_market_t_%d SET %s WHERE uid=%d AND group_id=%d AND commodity_id=%d;", iTableMod, strSet.c_str(), m_iUid, m_iGroupID, m_iCommodityID);
    return szBuffer;
}
std::string
CRecordUserIBusyMarketTable::MakeDeleteSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM ibusy_market_t_%d WHERE uid=%d AND group_id=%d AND commodity_id=%d;", iTableMod, m_iUid, m_iGroupID, m_iCommodityID);
    return szBuffer;
}

int
CRecordUserIBusyMarketTable::GetUid()
{
    return m_iUid;
}

int
CRecordUserIBusyMarketTable::GetGroupID()
{
    return m_iGroupID;
}

int
CRecordUserIBusyMarketTable::GetCommodityID()
{
    return m_iCommodityID;
}

std::vector<TUserIbusyMarketTableValueType>
CRecordUserIBusyMarketTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    int iTableMod = iUin % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM ibusy_market_t_%d WHERE uid=%d AND group_id=%d;" , iTableMod, iUin, iGroupID);
    std::vector<TUserIbusyMarketTableValueType> stRet;
    TUserIbusyMarketTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iCommodityID = ::atoi(stData[i]["commodity_id"].c_str());
            stValue.m_iLastBuyTime = ::atoi(stData[i]["last_buy_time"].c_str());
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
