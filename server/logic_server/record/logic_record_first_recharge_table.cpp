#include "logic_record_first_recharge_table.h"

#include <cassert>


const char* CRecordFirstRechargeTable::s_strLevel = "level";
const char* CRecordFirstRechargeTable::s_strPayCash = "pay_cash";
const char* CRecordFirstRechargeTable::s_strPayType = "pay_type";
const char* CRecordFirstRechargeTable::s_strPayGamecoin = "pay_gamecoin";
const char* CRecordFirstRechargeTable::s_strPayItem = "pay_item";
const char* CRecordFirstRechargeTable::s_strStatDate = "stat_date";
const char* CRecordFirstRechargeTable::s_strCreateRoleTime = "create_role_time";
const char* CRecordFirstRechargeTable::s_strLastLoginTime = "last_login_time";

CRecordFirstRechargeTable::~CRecordFirstRechargeTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordFirstRechargeTable::MakeInsertSQL()
{
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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO first_recharge_t %s VALUES %s;" , strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordFirstRechargeTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE first_recharge_t SET %s WHERE uid=%d AND group_id=%d;", strSet.c_str(), m_iUid, m_iGroupID);
    return szBuffer;
}
std::string
CRecordFirstRechargeTable::MakeDeleteSQL()
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM first_recharge_t WHERE uid=%d AND group_id=%d;", m_iUid, m_iGroupID);
    return szBuffer;
}

int
CRecordFirstRechargeTable::GetUid()
{
    return m_iUid;
}

int
CRecordFirstRechargeTable::GetGroupID()
{
    return m_iGroupID;
}

std::vector<TFirstRechargeTableValueType>
CRecordFirstRechargeTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM first_recharge_t WHERE uid=%d AND group_id=%d;" , iUin, iGroupID);
    std::vector<TFirstRechargeTableValueType> stRet;
    TFirstRechargeTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iLevel = ::atoi(stData[i]["level"].c_str());
            stValue.m_strPayCash = stData[i]["pay_cash"];
            stValue.m_iPayType = ::atoi(stData[i]["pay_type"].c_str());
            stValue.m_iPayGamecoin = ::atoi(stData[i]["pay_gamecoin"].c_str());
            stValue.m_iPayItem = ::atoi(stData[i]["pay_item"].c_str());
            stValue.m_strStatDate = stData[i]["stat_date"];
            stValue.m_iCreateRoleTime = ::atoi(stData[i]["create_role_time"].c_str());
            stValue.m_iLastLoginTime = ::atoi(stData[i]["last_login_time"].c_str());
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
