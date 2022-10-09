#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct THeroCardInfoTable;
struct THeroCardInfoTableValueType;

class CRecordHeroCardInfoTable : public CLogicSQLTableRecordBase
{
public:
    CRecordHeroCardInfoTable() : m_pCache(nullptr) { }

    virtual ~CRecordHeroCardInfoTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetCardID();

    static std::vector<THeroCardInfoTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strCardColor;
    static const char* s_strCardStar;
    static const char* s_strFashionID;
    static const char* s_strCardTime;
    static const char* s_strCardLevel;
    static const char* s_strCardExp;
    static const char* s_strCrossFightCostHp;
    static const char* s_strQiyuFightCostHp;
    static const char* s_strCangbaotuFightCostHp;
    static const char* s_strUpgradeColor1;
    static const char* s_strUpgradeColor2;
    static const char* s_strUpgradeColor3;
    static const char* s_strUpgradeColor4;
    static const char* s_strFightPower;
    static const char* s_strFightHp;
    static const char* s_strHeartLevel;
    static const char* s_strHeartExp;
    static const char* s_strDailyGiftNum;
    static const char* s_strLastGiftTime;
    static const char* s_strHeartGiftLevel;
    static const char* s_strGradeLevel;
    static const char* s_strDatingTimes;
    static const char* s_strImpression;
    static const char* s_strEquipConstellation1;
    static const char* s_strEquipConstellation2;
    static const char* s_strEquipConstellation3;
    static const char* s_strEquipConstellation4;
    static const char* s_strEquip1;
    static const char* s_strEquip2;
    static const char* s_strEquip3;
    static const char* s_strEquip4;
    static const char* s_strTalentPoint;
    static const char* s_strTalentPoint1;
    static const char* s_strTalentTotalConsume;
    static const char* s_strTalentMap;
    static const char* s_strPassLevel;
    static const char* s_strUseSkillCount;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strCardID;

    THeroCardInfoTable* m_pCache;

    void SetCardColor(char cCardColor)
    {
        m_stCharData[s_strCardColor] += cCardColor;
    }
    void SetCardStar(char cCardStar)
    {
        m_stCharData[s_strCardStar] += cCardStar;
    }
    void SetFashionID(int iFashionID)
    {
        m_stIntData[s_strFashionID] += iFashionID;
    }
    void SetCardTime(int iCardTime)
    {
        m_stIntData[s_strCardTime] += iCardTime;
    }
    void SetCardLevel(int iCardLevel)
    {
        m_stIntData[s_strCardLevel] += iCardLevel;
    }
    void SetCardExp(int iCardExp)
    {
        m_stIntData[s_strCardExp] += iCardExp;
    }
    void SetCrossFightCostHp(int iCrossFightCostHp)
    {
        m_stIntData[s_strCrossFightCostHp] += iCrossFightCostHp;
    }
    void SetQiyuFightCostHp(int iQiyuFightCostHp)
    {
        m_stIntData[s_strQiyuFightCostHp] += iQiyuFightCostHp;
    }
    void SetCangbaotuFightCostHp(int iCangbaotuFightCostHp)
    {
        m_stIntData[s_strCangbaotuFightCostHp] += iCangbaotuFightCostHp;
    }
    void SetUpgradeColor1(int iUpgradeColor1)
    {
        m_stIntData[s_strUpgradeColor1] += iUpgradeColor1;
    }
    void SetUpgradeColor2(int iUpgradeColor2)
    {
        m_stIntData[s_strUpgradeColor2] += iUpgradeColor2;
    }
    void SetUpgradeColor3(int iUpgradeColor3)
    {
        m_stIntData[s_strUpgradeColor3] += iUpgradeColor3;
    }
    void SetUpgradeColor4(int iUpgradeColor4)
    {
        m_stIntData[s_strUpgradeColor4] += iUpgradeColor4;
    }
    void SetFightPower(int iFightPower)
    {
        m_stIntData[s_strFightPower] += iFightPower;
    }
    void SetFightHp(int iFightHp)
    {
        m_stIntData[s_strFightHp] += iFightHp;
    }
    void SetHeartLevel(int iHeartLevel)
    {
        m_stIntData[s_strHeartLevel] += iHeartLevel;
    }
    void SetHeartExp(int iHeartExp)
    {
        m_stIntData[s_strHeartExp] += iHeartExp;
    }
    void SetDailyGiftNum(int iDailyGiftNum)
    {
        m_stIntData[s_strDailyGiftNum] += iDailyGiftNum;
    }
    void SetLastGiftTime(int iLastGiftTime)
    {
        m_stIntData[s_strLastGiftTime] += iLastGiftTime;
    }
    void SetHeartGiftLevel(int iHeartGiftLevel)
    {
        m_stIntData[s_strHeartGiftLevel] += iHeartGiftLevel;
    }
    void SetGradeLevel(int iGradeLevel)
    {
        m_stIntData[s_strGradeLevel] += iGradeLevel;
    }
    void SetDatingTimes(int iDatingTimes)
    {
        m_stIntData[s_strDatingTimes] += iDatingTimes;
    }
    void SetImpression(const std::vector<int>& stImpression)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stImpression);
        m_stVarData[s_strImpression] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetEquipConstellation1(int iEquipConstellation1)
    {
        m_stIntData[s_strEquipConstellation1] += iEquipConstellation1;
    }
    void SetEquipConstellation2(int iEquipConstellation2)
    {
        m_stIntData[s_strEquipConstellation2] += iEquipConstellation2;
    }
    void SetEquipConstellation3(int iEquipConstellation3)
    {
        m_stIntData[s_strEquipConstellation3] += iEquipConstellation3;
    }
    void SetEquipConstellation4(int iEquipConstellation4)
    {
        m_stIntData[s_strEquipConstellation4] += iEquipConstellation4;
    }
    void SetEquip1(int iEquip1)
    {
        m_stIntData[s_strEquip1] += iEquip1;
    }
    void SetEquip2(int iEquip2)
    {
        m_stIntData[s_strEquip2] += iEquip2;
    }
    void SetEquip3(int iEquip3)
    {
        m_stIntData[s_strEquip3] += iEquip3;
    }
    void SetEquip4(int iEquip4)
    {
        m_stIntData[s_strEquip4] += iEquip4;
    }
    void SetTalentPoint(int iTalentPoint)
    {
        m_stIntData[s_strTalentPoint] += iTalentPoint;
    }
    void SetTalentPoint1(int iTalentPoint1)
    {
        m_stIntData[s_strTalentPoint1] += iTalentPoint1;
    }
    void SetTalentTotalConsume(int iTalentTotalConsume)
    {
        m_stIntData[s_strTalentTotalConsume] += iTalentTotalConsume;
    }
    void SetTalentMap(const std::map<int, int>& stTalentMap)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTalentMap);
        m_stVarData[s_strTalentMap] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPassLevel(const std::map<int, bool>& stPassLevel)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stPassLevel);
        m_stVarData[s_strPassLevel] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetUseSkillCount(int iUseSkillCount)
    {
        m_stIntData[s_strUseSkillCount] += iUseSkillCount;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iCardID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iCardID = iCardID;
    }
    void SetCachePtr(THeroCardInfoTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iCardID;
};

struct THeroCardInfoTableValueType
{
    THeroCardInfoTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iCardID(0)
    , m_cCardColor(1)
    , m_cCardStar(0)
    , m_iFashionID(0)
    , m_iCardTime(0)
    , m_iCardLevel(0)
    , m_iCardExp(0)
    , m_iCrossFightCostHp(0)
    , m_iQiyuFightCostHp(0)
    , m_iCangbaotuFightCostHp(0)
    , m_iUpgradeColor1(0)
    , m_iUpgradeColor2(0)
    , m_iUpgradeColor3(0)
    , m_iUpgradeColor4(0)
    , m_iFightPower(0)
    , m_iFightHp(0)
    , m_iHeartLevel(1)
    , m_iHeartExp(0)
    , m_iDailyGiftNum(0)
    , m_iLastGiftTime(0)
    , m_iHeartGiftLevel(0)
    , m_iGradeLevel(0)
    , m_iDatingTimes(0)
    , m_stImpression()
    , m_iEquipConstellation1(0)
    , m_iEquipConstellation2(0)
    , m_iEquipConstellation3(0)
    , m_iEquipConstellation4(0)
    , m_iEquip1(0)
    , m_iEquip2(0)
    , m_iEquip3(0)
    , m_iEquip4(0)
    , m_iTalentPoint(0)
    , m_iTalentPoint1(0)
    , m_iTalentTotalConsume(0)
    , m_stTalentMap()
    , m_stPassLevel()
    , m_iUseSkillCount(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iCardID;
    char                    m_cCardColor;
    char                    m_cCardStar;
    int                     m_iFashionID;
    int                     m_iCardTime;
    int                     m_iCardLevel;
    int                     m_iCardExp;
    int                     m_iCrossFightCostHp;
    int                     m_iQiyuFightCostHp;
    int                     m_iCangbaotuFightCostHp;
    int                     m_iUpgradeColor1;
    int                     m_iUpgradeColor2;
    int                     m_iUpgradeColor3;
    int                     m_iUpgradeColor4;
    int                     m_iFightPower;
    int                     m_iFightHp;
    int                     m_iHeartLevel;
    int                     m_iHeartExp;
    int                     m_iDailyGiftNum;
    int                     m_iLastGiftTime;
    int                     m_iHeartGiftLevel;
    int                     m_iGradeLevel;
    int                     m_iDatingTimes;
    std::vector<int>        m_stImpression;
    int                     m_iEquipConstellation1;
    int                     m_iEquipConstellation2;
    int                     m_iEquipConstellation3;
    int                     m_iEquipConstellation4;
    int                     m_iEquip1;
    int                     m_iEquip2;
    int                     m_iEquip3;
    int                     m_iEquip4;
    int                     m_iTalentPoint;
    int                     m_iTalentPoint1;
    int                     m_iTalentTotalConsume;
    std::map<int, int>      m_stTalentMap;
    std::map<int, bool>     m_stPassLevel;
    int                     m_iUseSkillCount;
};

struct THeroCardInfoTable
{
    THeroCardInfoTable() : m_pRecord(nullptr) { }
    ~THeroCardInfoTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const THeroCardInfoTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const THeroCardInfoTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iCardID);
        m_pRecord->SetCardColor(stData.m_cCardColor);
        m_pRecord->SetCardStar(stData.m_cCardStar);
        m_pRecord->SetFashionID(stData.m_iFashionID);
        m_pRecord->SetCardTime(stData.m_iCardTime);
        m_pRecord->SetCardLevel(stData.m_iCardLevel);
        m_pRecord->SetCardExp(stData.m_iCardExp);
        m_pRecord->SetCrossFightCostHp(stData.m_iCrossFightCostHp);
        m_pRecord->SetQiyuFightCostHp(stData.m_iQiyuFightCostHp);
        m_pRecord->SetCangbaotuFightCostHp(stData.m_iCangbaotuFightCostHp);
        m_pRecord->SetUpgradeColor1(stData.m_iUpgradeColor1);
        m_pRecord->SetUpgradeColor2(stData.m_iUpgradeColor2);
        m_pRecord->SetUpgradeColor3(stData.m_iUpgradeColor3);
        m_pRecord->SetUpgradeColor4(stData.m_iUpgradeColor4);
        m_pRecord->SetFightPower(stData.m_iFightPower);
        m_pRecord->SetFightHp(stData.m_iFightHp);
        m_pRecord->SetHeartLevel(stData.m_iHeartLevel);
        m_pRecord->SetHeartExp(stData.m_iHeartExp);
        m_pRecord->SetDailyGiftNum(stData.m_iDailyGiftNum);
        m_pRecord->SetLastGiftTime(stData.m_iLastGiftTime);
        m_pRecord->SetHeartGiftLevel(stData.m_iHeartGiftLevel);
        m_pRecord->SetGradeLevel(stData.m_iGradeLevel);
        m_pRecord->SetDatingTimes(stData.m_iDatingTimes);
        m_pRecord->SetImpression(stData.m_stImpression);
        m_pRecord->SetEquipConstellation1(stData.m_iEquipConstellation1);
        m_pRecord->SetEquipConstellation2(stData.m_iEquipConstellation2);
        m_pRecord->SetEquipConstellation3(stData.m_iEquipConstellation3);
        m_pRecord->SetEquipConstellation4(stData.m_iEquipConstellation4);
        m_pRecord->SetEquip1(stData.m_iEquip1);
        m_pRecord->SetEquip2(stData.m_iEquip2);
        m_pRecord->SetEquip3(stData.m_iEquip3);
        m_pRecord->SetEquip4(stData.m_iEquip4);
        m_pRecord->SetTalentPoint(stData.m_iTalentPoint);
        m_pRecord->SetTalentPoint1(stData.m_iTalentPoint1);
        m_pRecord->SetTalentTotalConsume(stData.m_iTalentTotalConsume);
        m_pRecord->SetTalentMap(stData.m_stTalentMap);
        m_pRecord->SetPassLevel(stData.m_stPassLevel);
        m_pRecord->SetUseSkillCount(stData.m_iUseSkillCount);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const char& GetCardColor() const
    {
        return m_stData.m_cCardColor;
    }
    void AddCardColor(char cCardColor)
    {
        if (0 == cCardColor)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardColor(cCardColor);
        m_stData.m_cCardColor += cCardColor;
    }
    void SetCardColor(char cCardColor)
    {
        if (m_stData.m_cCardColor == cCardColor)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cCardColor - m_stData.m_cCardColor;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardColor(RealAdd);
        m_stData.m_cCardColor = cCardColor;
    }
    const char& GetCardStar() const
    {
        return m_stData.m_cCardStar;
    }
    void AddCardStar(char cCardStar)
    {
        if (0 == cCardStar)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardStar(cCardStar);
        m_stData.m_cCardStar += cCardStar;
    }
    void SetCardStar(char cCardStar)
    {
        if (m_stData.m_cCardStar == cCardStar)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cCardStar - m_stData.m_cCardStar;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardStar(RealAdd);
        m_stData.m_cCardStar = cCardStar;
    }
    const int& GetFashionID() const
    {
        return m_stData.m_iFashionID;
    }
    void AddFashionID(int iFashionID)
    {
        if (0 == iFashionID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFashionID(iFashionID);
        m_stData.m_iFashionID += iFashionID;
    }
    void SetFashionID(int iFashionID)
    {
        if (m_stData.m_iFashionID == iFashionID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFashionID - m_stData.m_iFashionID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFashionID(RealAdd);
        m_stData.m_iFashionID = iFashionID;
    }
    const int& GetCardTime() const
    {
        return m_stData.m_iCardTime;
    }
    void AddCardTime(int iCardTime)
    {
        if (0 == iCardTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardTime(iCardTime);
        m_stData.m_iCardTime += iCardTime;
    }
    void SetCardTime(int iCardTime)
    {
        if (m_stData.m_iCardTime == iCardTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCardTime - m_stData.m_iCardTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardTime(RealAdd);
        m_stData.m_iCardTime = iCardTime;
    }
    const int& GetCardLevel() const
    {
        return m_stData.m_iCardLevel;
    }
    void AddCardLevel(int iCardLevel)
    {
        if (0 == iCardLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardLevel(iCardLevel);
        m_stData.m_iCardLevel += iCardLevel;
    }
    void SetCardLevel(int iCardLevel)
    {
        if (m_stData.m_iCardLevel == iCardLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCardLevel - m_stData.m_iCardLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardLevel(RealAdd);
        m_stData.m_iCardLevel = iCardLevel;
    }
    const int& GetCardExp() const
    {
        return m_stData.m_iCardExp;
    }
    void AddCardExp(int iCardExp)
    {
        if (0 == iCardExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardExp(iCardExp);
        m_stData.m_iCardExp += iCardExp;
    }
    void SetCardExp(int iCardExp)
    {
        if (m_stData.m_iCardExp == iCardExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCardExp - m_stData.m_iCardExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCardExp(RealAdd);
        m_stData.m_iCardExp = iCardExp;
    }
    const int& GetCrossFightCostHp() const
    {
        return m_stData.m_iCrossFightCostHp;
    }
    void AddCrossFightCostHp(int iCrossFightCostHp)
    {
        if (0 == iCrossFightCostHp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrossFightCostHp(iCrossFightCostHp);
        m_stData.m_iCrossFightCostHp += iCrossFightCostHp;
    }
    void SetCrossFightCostHp(int iCrossFightCostHp)
    {
        if (m_stData.m_iCrossFightCostHp == iCrossFightCostHp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCrossFightCostHp - m_stData.m_iCrossFightCostHp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCrossFightCostHp(RealAdd);
        m_stData.m_iCrossFightCostHp = iCrossFightCostHp;
    }
    const int& GetQiyuFightCostHp() const
    {
        return m_stData.m_iQiyuFightCostHp;
    }
    void AddQiyuFightCostHp(int iQiyuFightCostHp)
    {
        if (0 == iQiyuFightCostHp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQiyuFightCostHp(iQiyuFightCostHp);
        m_stData.m_iQiyuFightCostHp += iQiyuFightCostHp;
    }
    void SetQiyuFightCostHp(int iQiyuFightCostHp)
    {
        if (m_stData.m_iQiyuFightCostHp == iQiyuFightCostHp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iQiyuFightCostHp - m_stData.m_iQiyuFightCostHp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQiyuFightCostHp(RealAdd);
        m_stData.m_iQiyuFightCostHp = iQiyuFightCostHp;
    }
    const int& GetCangbaotuFightCostHp() const
    {
        return m_stData.m_iCangbaotuFightCostHp;
    }
    void AddCangbaotuFightCostHp(int iCangbaotuFightCostHp)
    {
        if (0 == iCangbaotuFightCostHp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCangbaotuFightCostHp(iCangbaotuFightCostHp);
        m_stData.m_iCangbaotuFightCostHp += iCangbaotuFightCostHp;
    }
    void SetCangbaotuFightCostHp(int iCangbaotuFightCostHp)
    {
        if (m_stData.m_iCangbaotuFightCostHp == iCangbaotuFightCostHp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCangbaotuFightCostHp - m_stData.m_iCangbaotuFightCostHp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCangbaotuFightCostHp(RealAdd);
        m_stData.m_iCangbaotuFightCostHp = iCangbaotuFightCostHp;
    }
    const int& GetUpgradeColor1() const
    {
        return m_stData.m_iUpgradeColor1;
    }
    void AddUpgradeColor1(int iUpgradeColor1)
    {
        if (0 == iUpgradeColor1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUpgradeColor1(iUpgradeColor1);
        m_stData.m_iUpgradeColor1 += iUpgradeColor1;
    }
    void SetUpgradeColor1(int iUpgradeColor1)
    {
        if (m_stData.m_iUpgradeColor1 == iUpgradeColor1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iUpgradeColor1 - m_stData.m_iUpgradeColor1;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUpgradeColor1(RealAdd);
        m_stData.m_iUpgradeColor1 = iUpgradeColor1;
    }
    const int& GetUpgradeColor2() const
    {
        return m_stData.m_iUpgradeColor2;
    }
    void AddUpgradeColor2(int iUpgradeColor2)
    {
        if (0 == iUpgradeColor2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUpgradeColor2(iUpgradeColor2);
        m_stData.m_iUpgradeColor2 += iUpgradeColor2;
    }
    void SetUpgradeColor2(int iUpgradeColor2)
    {
        if (m_stData.m_iUpgradeColor2 == iUpgradeColor2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iUpgradeColor2 - m_stData.m_iUpgradeColor2;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUpgradeColor2(RealAdd);
        m_stData.m_iUpgradeColor2 = iUpgradeColor2;
    }
    const int& GetUpgradeColor3() const
    {
        return m_stData.m_iUpgradeColor3;
    }
    void AddUpgradeColor3(int iUpgradeColor3)
    {
        if (0 == iUpgradeColor3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUpgradeColor3(iUpgradeColor3);
        m_stData.m_iUpgradeColor3 += iUpgradeColor3;
    }
    void SetUpgradeColor3(int iUpgradeColor3)
    {
        if (m_stData.m_iUpgradeColor3 == iUpgradeColor3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iUpgradeColor3 - m_stData.m_iUpgradeColor3;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUpgradeColor3(RealAdd);
        m_stData.m_iUpgradeColor3 = iUpgradeColor3;
    }
    const int& GetUpgradeColor4() const
    {
        return m_stData.m_iUpgradeColor4;
    }
    void AddUpgradeColor4(int iUpgradeColor4)
    {
        if (0 == iUpgradeColor4)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUpgradeColor4(iUpgradeColor4);
        m_stData.m_iUpgradeColor4 += iUpgradeColor4;
    }
    void SetUpgradeColor4(int iUpgradeColor4)
    {
        if (m_stData.m_iUpgradeColor4 == iUpgradeColor4)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iUpgradeColor4 - m_stData.m_iUpgradeColor4;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUpgradeColor4(RealAdd);
        m_stData.m_iUpgradeColor4 = iUpgradeColor4;
    }
    const int& GetFightPower() const
    {
        return m_stData.m_iFightPower;
    }
    void AddFightPower(int iFightPower)
    {
        if (0 == iFightPower)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightPower(iFightPower);
        m_stData.m_iFightPower += iFightPower;
    }
    void SetFightPower(int iFightPower)
    {
        if (m_stData.m_iFightPower == iFightPower)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFightPower - m_stData.m_iFightPower;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightPower(RealAdd);
        m_stData.m_iFightPower = iFightPower;
    }
    const int& GetFightHp() const
    {
        return m_stData.m_iFightHp;
    }
    void AddFightHp(int iFightHp)
    {
        if (0 == iFightHp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightHp(iFightHp);
        m_stData.m_iFightHp += iFightHp;
    }
    void SetFightHp(int iFightHp)
    {
        if (m_stData.m_iFightHp == iFightHp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFightHp - m_stData.m_iFightHp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightHp(RealAdd);
        m_stData.m_iFightHp = iFightHp;
    }
    const int& GetHeartLevel() const
    {
        return m_stData.m_iHeartLevel;
    }
    void AddHeartLevel(int iHeartLevel)
    {
        if (0 == iHeartLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeartLevel(iHeartLevel);
        m_stData.m_iHeartLevel += iHeartLevel;
    }
    void SetHeartLevel(int iHeartLevel)
    {
        if (m_stData.m_iHeartLevel == iHeartLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iHeartLevel - m_stData.m_iHeartLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeartLevel(RealAdd);
        m_stData.m_iHeartLevel = iHeartLevel;
    }
    const int& GetHeartExp() const
    {
        return m_stData.m_iHeartExp;
    }
    void AddHeartExp(int iHeartExp)
    {
        if (0 == iHeartExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeartExp(iHeartExp);
        m_stData.m_iHeartExp += iHeartExp;
    }
    void SetHeartExp(int iHeartExp)
    {
        if (m_stData.m_iHeartExp == iHeartExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iHeartExp - m_stData.m_iHeartExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeartExp(RealAdd);
        m_stData.m_iHeartExp = iHeartExp;
    }
    const int& GetDailyGiftNum() const
    {
        return m_stData.m_iDailyGiftNum;
    }
    void AddDailyGiftNum(int iDailyGiftNum)
    {
        if (0 == iDailyGiftNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGiftNum(iDailyGiftNum);
        m_stData.m_iDailyGiftNum += iDailyGiftNum;
    }
    void SetDailyGiftNum(int iDailyGiftNum)
    {
        if (m_stData.m_iDailyGiftNum == iDailyGiftNum)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDailyGiftNum - m_stData.m_iDailyGiftNum;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDailyGiftNum(RealAdd);
        m_stData.m_iDailyGiftNum = iDailyGiftNum;
    }
    const int& GetLastGiftTime() const
    {
        return m_stData.m_iLastGiftTime;
    }
    void AddLastGiftTime(int iLastGiftTime)
    {
        if (0 == iLastGiftTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastGiftTime(iLastGiftTime);
        m_stData.m_iLastGiftTime += iLastGiftTime;
    }
    void SetLastGiftTime(int iLastGiftTime)
    {
        if (m_stData.m_iLastGiftTime == iLastGiftTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastGiftTime - m_stData.m_iLastGiftTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastGiftTime(RealAdd);
        m_stData.m_iLastGiftTime = iLastGiftTime;
    }
    const int& GetHeartGiftLevel() const
    {
        return m_stData.m_iHeartGiftLevel;
    }
    void AddHeartGiftLevel(int iHeartGiftLevel)
    {
        if (0 == iHeartGiftLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeartGiftLevel(iHeartGiftLevel);
        m_stData.m_iHeartGiftLevel += iHeartGiftLevel;
    }
    void SetHeartGiftLevel(int iHeartGiftLevel)
    {
        if (m_stData.m_iHeartGiftLevel == iHeartGiftLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iHeartGiftLevel - m_stData.m_iHeartGiftLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeartGiftLevel(RealAdd);
        m_stData.m_iHeartGiftLevel = iHeartGiftLevel;
    }
    const int& GetGradeLevel() const
    {
        return m_stData.m_iGradeLevel;
    }
    void AddGradeLevel(int iGradeLevel)
    {
        if (0 == iGradeLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGradeLevel(iGradeLevel);
        m_stData.m_iGradeLevel += iGradeLevel;
    }
    void SetGradeLevel(int iGradeLevel)
    {
        if (m_stData.m_iGradeLevel == iGradeLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGradeLevel - m_stData.m_iGradeLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGradeLevel(RealAdd);
        m_stData.m_iGradeLevel = iGradeLevel;
    }
    const int& GetDatingTimes() const
    {
        return m_stData.m_iDatingTimes;
    }
    void AddDatingTimes(int iDatingTimes)
    {
        if (0 == iDatingTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDatingTimes(iDatingTimes);
        m_stData.m_iDatingTimes += iDatingTimes;
    }
    void SetDatingTimes(int iDatingTimes)
    {
        if (m_stData.m_iDatingTimes == iDatingTimes)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDatingTimes - m_stData.m_iDatingTimes;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDatingTimes(RealAdd);
        m_stData.m_iDatingTimes = iDatingTimes;
    }
    const std::vector<int>& GetImpression() const
    {
        return m_stData.m_stImpression;
    }
    void SetImpression(const std::vector<int>& stImpression)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetImpression(stImpression);
        m_stData.m_stImpression = stImpression;
    }
    const int& GetEquipConstellation1() const
    {
        return m_stData.m_iEquipConstellation1;
    }
    void AddEquipConstellation1(int iEquipConstellation1)
    {
        if (0 == iEquipConstellation1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipConstellation1(iEquipConstellation1);
        m_stData.m_iEquipConstellation1 += iEquipConstellation1;
    }
    void SetEquipConstellation1(int iEquipConstellation1)
    {
        if (m_stData.m_iEquipConstellation1 == iEquipConstellation1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquipConstellation1 - m_stData.m_iEquipConstellation1;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipConstellation1(RealAdd);
        m_stData.m_iEquipConstellation1 = iEquipConstellation1;
    }
    const int& GetEquipConstellation2() const
    {
        return m_stData.m_iEquipConstellation2;
    }
    void AddEquipConstellation2(int iEquipConstellation2)
    {
        if (0 == iEquipConstellation2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipConstellation2(iEquipConstellation2);
        m_stData.m_iEquipConstellation2 += iEquipConstellation2;
    }
    void SetEquipConstellation2(int iEquipConstellation2)
    {
        if (m_stData.m_iEquipConstellation2 == iEquipConstellation2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquipConstellation2 - m_stData.m_iEquipConstellation2;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipConstellation2(RealAdd);
        m_stData.m_iEquipConstellation2 = iEquipConstellation2;
    }
    const int& GetEquipConstellation3() const
    {
        return m_stData.m_iEquipConstellation3;
    }
    void AddEquipConstellation3(int iEquipConstellation3)
    {
        if (0 == iEquipConstellation3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipConstellation3(iEquipConstellation3);
        m_stData.m_iEquipConstellation3 += iEquipConstellation3;
    }
    void SetEquipConstellation3(int iEquipConstellation3)
    {
        if (m_stData.m_iEquipConstellation3 == iEquipConstellation3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquipConstellation3 - m_stData.m_iEquipConstellation3;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipConstellation3(RealAdd);
        m_stData.m_iEquipConstellation3 = iEquipConstellation3;
    }
    const int& GetEquipConstellation4() const
    {
        return m_stData.m_iEquipConstellation4;
    }
    void AddEquipConstellation4(int iEquipConstellation4)
    {
        if (0 == iEquipConstellation4)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipConstellation4(iEquipConstellation4);
        m_stData.m_iEquipConstellation4 += iEquipConstellation4;
    }
    void SetEquipConstellation4(int iEquipConstellation4)
    {
        if (m_stData.m_iEquipConstellation4 == iEquipConstellation4)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquipConstellation4 - m_stData.m_iEquipConstellation4;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipConstellation4(RealAdd);
        m_stData.m_iEquipConstellation4 = iEquipConstellation4;
    }
    const int& GetEquip1() const
    {
        return m_stData.m_iEquip1;
    }
    void AddEquip1(int iEquip1)
    {
        if (0 == iEquip1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquip1(iEquip1);
        m_stData.m_iEquip1 += iEquip1;
    }
    void SetEquip1(int iEquip1)
    {
        if (m_stData.m_iEquip1 == iEquip1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquip1 - m_stData.m_iEquip1;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquip1(RealAdd);
        m_stData.m_iEquip1 = iEquip1;
    }
    const int& GetEquip2() const
    {
        return m_stData.m_iEquip2;
    }
    void AddEquip2(int iEquip2)
    {
        if (0 == iEquip2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquip2(iEquip2);
        m_stData.m_iEquip2 += iEquip2;
    }
    void SetEquip2(int iEquip2)
    {
        if (m_stData.m_iEquip2 == iEquip2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquip2 - m_stData.m_iEquip2;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquip2(RealAdd);
        m_stData.m_iEquip2 = iEquip2;
    }
    const int& GetEquip3() const
    {
        return m_stData.m_iEquip3;
    }
    void AddEquip3(int iEquip3)
    {
        if (0 == iEquip3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquip3(iEquip3);
        m_stData.m_iEquip3 += iEquip3;
    }
    void SetEquip3(int iEquip3)
    {
        if (m_stData.m_iEquip3 == iEquip3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquip3 - m_stData.m_iEquip3;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquip3(RealAdd);
        m_stData.m_iEquip3 = iEquip3;
    }
    const int& GetEquip4() const
    {
        return m_stData.m_iEquip4;
    }
    void AddEquip4(int iEquip4)
    {
        if (0 == iEquip4)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquip4(iEquip4);
        m_stData.m_iEquip4 += iEquip4;
    }
    void SetEquip4(int iEquip4)
    {
        if (m_stData.m_iEquip4 == iEquip4)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquip4 - m_stData.m_iEquip4;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquip4(RealAdd);
        m_stData.m_iEquip4 = iEquip4;
    }
    const int& GetTalentPoint() const
    {
        return m_stData.m_iTalentPoint;
    }
    void AddTalentPoint(int iTalentPoint)
    {
        if (0 == iTalentPoint)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTalentPoint(iTalentPoint);
        m_stData.m_iTalentPoint += iTalentPoint;
    }
    void SetTalentPoint(int iTalentPoint)
    {
        if (m_stData.m_iTalentPoint == iTalentPoint)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTalentPoint - m_stData.m_iTalentPoint;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTalentPoint(RealAdd);
        m_stData.m_iTalentPoint = iTalentPoint;
    }
    const int& GetTalentPoint1() const
    {
        return m_stData.m_iTalentPoint1;
    }
    void AddTalentPoint1(int iTalentPoint1)
    {
        if (0 == iTalentPoint1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTalentPoint1(iTalentPoint1);
        m_stData.m_iTalentPoint1 += iTalentPoint1;
    }
    void SetTalentPoint1(int iTalentPoint1)
    {
        if (m_stData.m_iTalentPoint1 == iTalentPoint1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTalentPoint1 - m_stData.m_iTalentPoint1;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTalentPoint1(RealAdd);
        m_stData.m_iTalentPoint1 = iTalentPoint1;
    }
    const int& GetTalentTotalConsume() const
    {
        return m_stData.m_iTalentTotalConsume;
    }
    void AddTalentTotalConsume(int iTalentTotalConsume)
    {
        if (0 == iTalentTotalConsume)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTalentTotalConsume(iTalentTotalConsume);
        m_stData.m_iTalentTotalConsume += iTalentTotalConsume;
    }
    void SetTalentTotalConsume(int iTalentTotalConsume)
    {
        if (m_stData.m_iTalentTotalConsume == iTalentTotalConsume)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTalentTotalConsume - m_stData.m_iTalentTotalConsume;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTalentTotalConsume(RealAdd);
        m_stData.m_iTalentTotalConsume = iTalentTotalConsume;
    }
    const std::map<int, int>& GetTalentMap() const
    {
        return m_stData.m_stTalentMap;
    }
    void SetTalentMap(const std::map<int, int>& stTalentMap)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTalentMap(stTalentMap);
        m_stData.m_stTalentMap = stTalentMap;
    }
    const std::map<int, bool>& GetPassLevel() const
    {
        return m_stData.m_stPassLevel;
    }
    void SetPassLevel(const std::map<int, bool>& stPassLevel)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPassLevel(stPassLevel);
        m_stData.m_stPassLevel = stPassLevel;
    }
    const int& GetUseSkillCount() const
    {
        return m_stData.m_iUseSkillCount;
    }
    void AddUseSkillCount(int iUseSkillCount)
    {
        if (0 == iUseSkillCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUseSkillCount(iUseSkillCount);
        m_stData.m_iUseSkillCount += iUseSkillCount;
    }
    void SetUseSkillCount(int iUseSkillCount)
    {
        if (m_stData.m_iUseSkillCount == iUseSkillCount)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iUseSkillCount - m_stData.m_iUseSkillCount;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetUseSkillCount(RealAdd);
        m_stData.m_iUseSkillCount = iUseSkillCount;
    }

    CRecordHeroCardInfoTable* m_pRecord;
    THeroCardInfoTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordHeroCardInfoTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordHeroCardInfoTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iCardID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordHeroCardInfoTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
