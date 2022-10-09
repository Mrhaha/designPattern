#include "logic_record_global_limit_boss_table.h"

#include <cassert>


const char* CRecordGlobalLimitBossTable::s_strOwnerGroup = "owner_group";
const char* CRecordGlobalLimitBossTable::s_strNick = "nick";
const char* CRecordGlobalLimitBossTable::s_strHard = "hard";
const char* CRecordGlobalLimitBossTable::s_strIsOpen = "is_open";
const char* CRecordGlobalLimitBossTable::s_strEndTime = "end_time";
const char* CRecordGlobalLimitBossTable::s_strOpenTime = "open_time";
const char* CRecordGlobalLimitBossTable::s_strDeathTime = "death_time";
const char* CRecordGlobalLimitBossTable::s_strKillerUin = "killer_uin";
const char* CRecordGlobalLimitBossTable::s_strKillerGroup = "killer_group";
const char* CRecordGlobalLimitBossTable::s_strMaxHp = "max_hp";
const char* CRecordGlobalLimitBossTable::s_strCurHurt = "cur_hurt";
const char* CRecordGlobalLimitBossTable::s_strHurtLog = "hurt_log";
const char* CRecordGlobalLimitBossTable::s_strAttackerList = "attacker_list";

CRecordGlobalLimitBossTable::~CRecordGlobalLimitBossTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordGlobalLimitBossTable::MakeInsertSQL()
{
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "server_id" + std::string(",");
    strVal += Int2DBString(m_iServerID) + ",";
    strCol += "owner_uin" + std::string(",");
    strVal += Int2DBString(m_iOwnerUin) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO global_limit_boss_t %s VALUES %s;" , strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordGlobalLimitBossTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE global_limit_boss_t SET %s WHERE server_id=%d AND owner_uin=%d;", strSet.c_str(), m_iServerID, m_iOwnerUin);
    return szBuffer;
}
std::string
CRecordGlobalLimitBossTable::MakeDeleteSQL()
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM global_limit_boss_t WHERE server_id=%d AND owner_uin=%d;", m_iServerID, m_iOwnerUin);
    return szBuffer;
}

int
CRecordGlobalLimitBossTable::GetServerID()
{
    return m_iServerID;
}

int
CRecordGlobalLimitBossTable::GetOwnerUin()
{
    return m_iOwnerUin;
}

std::vector<TGlobalLimitBossTableValueType>
CRecordGlobalLimitBossTable::SelectDataFromSQL(int iServerID)
{
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM global_limit_boss_t WHERE server_id=%d;" , iServerID);
    std::vector<TGlobalLimitBossTableValueType> stRet;
    TGlobalLimitBossTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iServerID = ::atoi(stData[i]["server_id"].c_str());
            stValue.m_iOwnerUin = ::atoi(stData[i]["owner_uin"].c_str());
            stValue.m_iOwnerGroup = ::atoi(stData[i]["owner_group"].c_str());
            stValue.m_strNick = stData[i]["nick"];
            stValue.m_cHard = (char)::atoi(stData[i]["hard"].c_str());
            stValue.m_cIsOpen = (char)::atoi(stData[i]["is_open"].c_str());
            stValue.m_iEndTime = ::atoi(stData[i]["end_time"].c_str());
            stValue.m_iOpenTime = ::atoi(stData[i]["open_time"].c_str());
            stValue.m_iDeathTime = ::atoi(stData[i]["death_time"].c_str());
            stValue.m_iKillerUin = ::atoi(stData[i]["killer_uin"].c_str());
            stValue.m_iKillerGroup = ::atoi(stData[i]["killer_group"].c_str());
            stValue.m_ulMaxHp = (uint64_t)strtoull(stData[i]["max_hp"].c_str(), nullptr, 10);
            stValue.m_ulCurHurt = (uint64_t)strtoull(stData[i]["cur_hurt"].c_str(), nullptr, 10);
            FormatDBBin(stData[i]["hurt_log"], stValue.m_stHurtLog);
            FormatDBBin(stData[i]["attacker_list"], stValue.m_stAttackerList);
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
