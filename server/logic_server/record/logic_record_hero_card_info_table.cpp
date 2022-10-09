#include "logic_record_hero_card_info_table.h"

#include <cassert>

const int CRecordHeroCardInfoTable::TABLE_DIV = 32;

const char* CRecordHeroCardInfoTable::s_strCardColor = "card_color";
const char* CRecordHeroCardInfoTable::s_strCardStar = "card_star";
const char* CRecordHeroCardInfoTable::s_strFashionID = "fashion_id";
const char* CRecordHeroCardInfoTable::s_strCardTime = "card_time";
const char* CRecordHeroCardInfoTable::s_strCardLevel = "card_level";
const char* CRecordHeroCardInfoTable::s_strCardExp = "card_exp";
const char* CRecordHeroCardInfoTable::s_strCrossFightCostHp = "cross_fight_cost_hp";
const char* CRecordHeroCardInfoTable::s_strQiyuFightCostHp = "qiyu_fight_cost_hp";
const char* CRecordHeroCardInfoTable::s_strCangbaotuFightCostHp = "cangbaotu_fight_cost_hp";
const char* CRecordHeroCardInfoTable::s_strUpgradeColor1 = "upgrade_color_1";
const char* CRecordHeroCardInfoTable::s_strUpgradeColor2 = "upgrade_color_2";
const char* CRecordHeroCardInfoTable::s_strUpgradeColor3 = "upgrade_color_3";
const char* CRecordHeroCardInfoTable::s_strUpgradeColor4 = "upgrade_color_4";
const char* CRecordHeroCardInfoTable::s_strFightPower = "fight_power";
const char* CRecordHeroCardInfoTable::s_strFightHp = "fight_hp";
const char* CRecordHeroCardInfoTable::s_strHeartLevel = "heart_level";
const char* CRecordHeroCardInfoTable::s_strHeartExp = "heart_exp";
const char* CRecordHeroCardInfoTable::s_strDailyGiftNum = "daily_gift_num";
const char* CRecordHeroCardInfoTable::s_strLastGiftTime = "last_gift_time";
const char* CRecordHeroCardInfoTable::s_strHeartGiftLevel = "heart_gift_level";
const char* CRecordHeroCardInfoTable::s_strGradeLevel = "grade_level";
const char* CRecordHeroCardInfoTable::s_strDatingTimes = "dating_times";
const char* CRecordHeroCardInfoTable::s_strImpression = "impression";
const char* CRecordHeroCardInfoTable::s_strEquipConstellation1 = "equip_constellation_1";
const char* CRecordHeroCardInfoTable::s_strEquipConstellation2 = "equip_constellation_2";
const char* CRecordHeroCardInfoTable::s_strEquipConstellation3 = "equip_constellation_3";
const char* CRecordHeroCardInfoTable::s_strEquipConstellation4 = "equip_constellation_4";
const char* CRecordHeroCardInfoTable::s_strEquip1 = "equip_1";
const char* CRecordHeroCardInfoTable::s_strEquip2 = "equip_2";
const char* CRecordHeroCardInfoTable::s_strEquip3 = "equip_3";
const char* CRecordHeroCardInfoTable::s_strEquip4 = "equip_4";
const char* CRecordHeroCardInfoTable::s_strTalentPoint = "talent_point";
const char* CRecordHeroCardInfoTable::s_strTalentPoint1 = "talent_point_1";
const char* CRecordHeroCardInfoTable::s_strTalentTotalConsume = "talent_total_consume";
const char* CRecordHeroCardInfoTable::s_strTalentMap = "talent_map";
const char* CRecordHeroCardInfoTable::s_strPassLevel = "pass_level";
const char* CRecordHeroCardInfoTable::s_strUseSkillCount = "use_skill_count";

CRecordHeroCardInfoTable::~CRecordHeroCardInfoTable()
{
    if (m_pCache != nullptr)
    {
        m_pCache->m_pRecord = nullptr;
    }
}

std::string
CRecordHeroCardInfoTable::MakeInsertSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    std::string strCol = "(";
    std::string strVal = "(";

    strCol += "uid" + std::string(",");
    strVal += Int2DBString(m_iUid) + ",";
    strCol += "group_id" + std::string(",");
    strVal += Int2DBString(m_iGroupID) + ",";
    strCol += "card_id" + std::string(",");
    strVal += Int2DBString(m_iCardID) + ",";

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
    snprintf(szBuffer, sizeof(szBuffer), "INSERT INTO hero_card_info_t_%d %s VALUES %s;" , iTableMod, strCol.c_str(), strVal.c_str());
    return szBuffer;
}
std::string
CRecordHeroCardInfoTable::MakeUpdateSQL()
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
    snprintf(szBuffer, sizeof(szBuffer), "UPDATE hero_card_info_t_%d SET %s WHERE uid=%d AND group_id=%d AND card_id=%d;", iTableMod, strSet.c_str(), m_iUid, m_iGroupID, m_iCardID);
    return szBuffer;
}
std::string
CRecordHeroCardInfoTable::MakeDeleteSQL()
{
    int iTableMod = m_iUid % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "DELETE FROM hero_card_info_t_%d WHERE uid=%d AND group_id=%d AND card_id=%d;", iTableMod, m_iUid, m_iGroupID, m_iCardID);
    return szBuffer;
}

int
CRecordHeroCardInfoTable::GetUid()
{
    return m_iUid;
}

int
CRecordHeroCardInfoTable::GetGroupID()
{
    return m_iGroupID;
}

int
CRecordHeroCardInfoTable::GetCardID()
{
    return m_iCardID;
}

std::vector<THeroCardInfoTableValueType>
CRecordHeroCardInfoTable::SelectDataFromSQL(int iUin, int iGroupID)
{
    int iTableMod = iUin % TABLE_DIV;
    char szBuffer[1024];
    snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM hero_card_info_t_%d WHERE uid=%d AND group_id=%d;" , iTableMod, iUin, iGroupID);
    std::vector<THeroCardInfoTableValueType> stRet;
    THeroCardInfoTableValueType stValue;

    try {
        taf::TC_Mysql::MysqlData stData = s_pMysqlHandle->queryRecord(szBuffer);
        for (size_t i = 0; i < stData.size(); ++i)
        {
            stValue.m_iUid = ::atoi(stData[i]["uid"].c_str());
            stValue.m_iGroupID = ::atoi(stData[i]["group_id"].c_str());
            stValue.m_iCardID = ::atoi(stData[i]["card_id"].c_str());
            stValue.m_cCardColor = (char)::atoi(stData[i]["card_color"].c_str());
            stValue.m_cCardStar = (char)::atoi(stData[i]["card_star"].c_str());
            stValue.m_iFashionID = ::atoi(stData[i]["fashion_id"].c_str());
            stValue.m_iCardTime = ::atoi(stData[i]["card_time"].c_str());
            stValue.m_iCardLevel = ::atoi(stData[i]["card_level"].c_str());
            stValue.m_iCardExp = ::atoi(stData[i]["card_exp"].c_str());
            stValue.m_iCrossFightCostHp = ::atoi(stData[i]["cross_fight_cost_hp"].c_str());
            stValue.m_iQiyuFightCostHp = ::atoi(stData[i]["qiyu_fight_cost_hp"].c_str());
            stValue.m_iCangbaotuFightCostHp = ::atoi(stData[i]["cangbaotu_fight_cost_hp"].c_str());
            stValue.m_iUpgradeColor1 = ::atoi(stData[i]["upgrade_color_1"].c_str());
            stValue.m_iUpgradeColor2 = ::atoi(stData[i]["upgrade_color_2"].c_str());
            stValue.m_iUpgradeColor3 = ::atoi(stData[i]["upgrade_color_3"].c_str());
            stValue.m_iUpgradeColor4 = ::atoi(stData[i]["upgrade_color_4"].c_str());
            stValue.m_iFightPower = ::atoi(stData[i]["fight_power"].c_str());
            stValue.m_iFightHp = ::atoi(stData[i]["fight_hp"].c_str());
            stValue.m_iHeartLevel = ::atoi(stData[i]["heart_level"].c_str());
            stValue.m_iHeartExp = ::atoi(stData[i]["heart_exp"].c_str());
            stValue.m_iDailyGiftNum = ::atoi(stData[i]["daily_gift_num"].c_str());
            stValue.m_iLastGiftTime = ::atoi(stData[i]["last_gift_time"].c_str());
            stValue.m_iHeartGiftLevel = ::atoi(stData[i]["heart_gift_level"].c_str());
            stValue.m_iGradeLevel = ::atoi(stData[i]["grade_level"].c_str());
            stValue.m_iDatingTimes = ::atoi(stData[i]["dating_times"].c_str());
            FormatDBBin(stData[i]["impression"], stValue.m_stImpression);
            stValue.m_iEquipConstellation1 = ::atoi(stData[i]["equip_constellation_1"].c_str());
            stValue.m_iEquipConstellation2 = ::atoi(stData[i]["equip_constellation_2"].c_str());
            stValue.m_iEquipConstellation3 = ::atoi(stData[i]["equip_constellation_3"].c_str());
            stValue.m_iEquipConstellation4 = ::atoi(stData[i]["equip_constellation_4"].c_str());
            stValue.m_iEquip1 = ::atoi(stData[i]["equip_1"].c_str());
            stValue.m_iEquip2 = ::atoi(stData[i]["equip_2"].c_str());
            stValue.m_iEquip3 = ::atoi(stData[i]["equip_3"].c_str());
            stValue.m_iEquip4 = ::atoi(stData[i]["equip_4"].c_str());
            stValue.m_iTalentPoint = ::atoi(stData[i]["talent_point"].c_str());
            stValue.m_iTalentPoint1 = ::atoi(stData[i]["talent_point_1"].c_str());
            stValue.m_iTalentTotalConsume = ::atoi(stData[i]["talent_total_consume"].c_str());
            FormatDBBin(stData[i]["talent_map"], stValue.m_stTalentMap);
            FormatDBBin(stData[i]["pass_level"], stValue.m_stPassLevel);
            stValue.m_iUseSkillCount = ::atoi(stData[i]["use_skill_count"].c_str());
            stRet.push_back(stValue);
        }
    } catch(const taf::TC_Mysql_Exception& stEx) {
    }
    return stRet;
}
