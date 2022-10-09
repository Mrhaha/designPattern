#include "logic_record_global_store_reward_table.h"

#include <cassert>


const char* CRecordGlobalStoreRewardTable::s_strEndTime = "end_time";

CRecordGlobalStoreRewardTable::~CRecordGlobalStoreRewardTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordGlobalStoreRewardTable::MakeInsertSQL()
{
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "server_id" + std::string(",");
    strVal += Int2DBString(m_iServerID) + ",";
    strCol += "uid" + std::string(",");
    strVal += Int2DBString(m_iUid) + ",";
    strCol += "group_id" + std::string(",");
    strVal += Int2DBString(m_iGroupID) + ",";
    strCol += "deposit_id" + std::string(",");
    strVal += Int2DBString(m_iDepositID) + ",";
    strCol += "increase_index" + std::string(",");
    strVal += Int2DBString(m_iIncreaseIndex) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO global_store_daily_reward_t %s VALUES %s;" , strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordGlobalStoreRewardTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE global_store_daily_reward_t SET %s WHERE server_id=%d AND uid=%d AND group_id=%d AND deposit_id=%d AND increase_index=%d;", strSet.c_str(), m_iServerID, m_iUid, m_iGroupID, m_iDepositID, m_iIncreaseIndex);
    return szBuffer;
}
std::string
CRecordGlobalStoreRewardTable::MakeDeleteSQL()
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM global_store_daily_reward_t WHERE server_id=%d AND uid=%d AND group_id=%d AND deposit_id=%d AND increase_index=%d;", m_iServerID, m_iUid, m_iGroupID, m_iDepositID, m_iIncreaseIndex);
    return szBuffer;
}

int
CRecordGlobalStoreRewardTable::GetServerID()
{
    return m_iServerID;
}

int
CRecordGlobalStoreRewardTable::GetUid()
{
    return m_iUid;
}

int
CRecordGlobalStoreRewardTable::GetGroupID()
{
    return m_iGroupID;
}

int
CRecordGlobalStoreRewardTable::GetDepositID()
{
    return m_iDepositID;
}

int
CRecordGlobalStoreRewardTable::GetIncreaseIndex()
{
    return m_iIncreaseIndex;
}

std::vector<TGlobalStoreRewardTableValueType>
CRecordGlobalStoreRewardTable::SelectDataFromSQL(int iServerID)
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM global_store_daily_reward_t WHERE server_id=%d;" , iServerID);
    std::vector<TGlobalStoreRewardTableValueType> stRet;
    TGlobalStoreRewardTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iServerID = ::atoi(stData[i]["server_id"].c_str());
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iDepositID = ::atoi(stData[i]["deposit_id"].c_str());
            stValue.m_iIncreaseIndex = ::atoi(stData[i]["increase_index"].c_str());
            stValue.m_iEndTime = ::atoi(stData[i]["end_time"].c_str());
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
