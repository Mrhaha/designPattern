#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserConstellationTable;
struct TUserConstellationTableValueType;

class CRecordUserConstellationTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserConstellationTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserConstellationTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetUniqueID();

    static std::vector<TUserConstellationTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strGrade;
    static const char* s_strEquipmentTag;
    static const char* s_strSuit;
    static const char* s_strLevel;
    static const char* s_strExp;
    static const char* s_strTotalExp;
    static const char* s_strEquipCardID;
    static const char* s_strIsLocked;
    static const char* s_strIsDiscard;
    static const char* s_strMainAttrType;
    static const char* s_strExAttr;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strUniqueID;

    TUserConstellationTable* m_pCache;

    void SetGrade(int iGrade)
    {
        m_stIntData[s_strGrade] += iGrade;
    }
    void SetEquipmentTag(int iEquipmentTag)
    {
        m_stIntData[s_strEquipmentTag] += iEquipmentTag;
    }
    void SetSuit(int iSuit)
    {
        m_stIntData[s_strSuit] += iSuit;
    }
    void SetLevel(int iLevel)
    {
        m_stIntData[s_strLevel] += iLevel;
    }
    void SetExp(int iExp)
    {
        m_stIntData[s_strExp] += iExp;
    }
    void SetTotalExp(int iTotalExp)
    {
        m_stIntData[s_strTotalExp] += iTotalExp;
    }
    void SetEquipCardID(int iEquipCardID)
    {
        m_stIntData[s_strEquipCardID] += iEquipCardID;
    }
    void SetIsLocked(char cIsLocked)
    {
        m_stCharData[s_strIsLocked] += cIsLocked;
    }
    void SetIsDiscard(char cIsDiscard)
    {
        m_stCharData[s_strIsDiscard] += cIsDiscard;
    }
    void SetMainAttrType(int iMainAttrType)
    {
        m_stIntData[s_strMainAttrType] += iMainAttrType;
    }
    void SetExAttr(const std::map<int, std::pair<int, int>>& stExAttr)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stExAttr);
        m_stVarData[s_strExAttr] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iUniqueID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iUniqueID = iUniqueID;
    }
    void SetCachePtr(TUserConstellationTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iUniqueID;
};

struct TUserConstellationTableValueType
{
    TUserConstellationTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iUniqueID(0)
    , m_iGrade(0)
    , m_iEquipmentTag(0)
    , m_iSuit(0)
    , m_iLevel(0)
    , m_iExp(0)
    , m_iTotalExp(0)
    , m_iEquipCardID(0)
    , m_cIsLocked(0)
    , m_cIsDiscard(0)
    , m_iMainAttrType(0)
    , m_stExAttr()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iUniqueID;
    int                     m_iGrade;
    int                     m_iEquipmentTag;
    int                     m_iSuit;
    int                     m_iLevel;
    int                     m_iExp;
    int                     m_iTotalExp;
    int                     m_iEquipCardID;
    char                    m_cIsLocked;
    char                    m_cIsDiscard;
    int                     m_iMainAttrType;
    std::map<int, std::pair<int, int>>    m_stExAttr;
};

struct TUserConstellationTable
{
    TUserConstellationTable() : m_pRecord(nullptr) { }
    ~TUserConstellationTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserConstellationTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserConstellationTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iUniqueID);
        m_pRecord->SetGrade(stData.m_iGrade);
        m_pRecord->SetEquipmentTag(stData.m_iEquipmentTag);
        m_pRecord->SetSuit(stData.m_iSuit);
        m_pRecord->SetLevel(stData.m_iLevel);
        m_pRecord->SetExp(stData.m_iExp);
        m_pRecord->SetTotalExp(stData.m_iTotalExp);
        m_pRecord->SetEquipCardID(stData.m_iEquipCardID);
        m_pRecord->SetIsLocked(stData.m_cIsLocked);
        m_pRecord->SetIsDiscard(stData.m_cIsDiscard);
        m_pRecord->SetMainAttrType(stData.m_iMainAttrType);
        m_pRecord->SetExAttr(stData.m_stExAttr);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetGrade() const
    {
        return m_stData.m_iGrade;
    }
    void AddGrade(int iGrade)
    {
        if (0 == iGrade)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGrade(iGrade);
        m_stData.m_iGrade += iGrade;
    }
    void SetGrade(int iGrade)
    {
        if (m_stData.m_iGrade == iGrade)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGrade - m_stData.m_iGrade;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGrade(RealAdd);
        m_stData.m_iGrade = iGrade;
    }
    const int& GetEquipmentTag() const
    {
        return m_stData.m_iEquipmentTag;
    }
    void AddEquipmentTag(int iEquipmentTag)
    {
        if (0 == iEquipmentTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipmentTag(iEquipmentTag);
        m_stData.m_iEquipmentTag += iEquipmentTag;
    }
    void SetEquipmentTag(int iEquipmentTag)
    {
        if (m_stData.m_iEquipmentTag == iEquipmentTag)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquipmentTag - m_stData.m_iEquipmentTag;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipmentTag(RealAdd);
        m_stData.m_iEquipmentTag = iEquipmentTag;
    }
    const int& GetSuit() const
    {
        return m_stData.m_iSuit;
    }
    void AddSuit(int iSuit)
    {
        if (0 == iSuit)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSuit(iSuit);
        m_stData.m_iSuit += iSuit;
    }
    void SetSuit(int iSuit)
    {
        if (m_stData.m_iSuit == iSuit)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSuit - m_stData.m_iSuit;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSuit(RealAdd);
        m_stData.m_iSuit = iSuit;
    }
    const int& GetLevel() const
    {
        return m_stData.m_iLevel;
    }
    void AddLevel(int iLevel)
    {
        if (0 == iLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLevel(iLevel);
        m_stData.m_iLevel += iLevel;
    }
    void SetLevel(int iLevel)
    {
        if (m_stData.m_iLevel == iLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLevel - m_stData.m_iLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLevel(RealAdd);
        m_stData.m_iLevel = iLevel;
    }
    const int& GetExp() const
    {
        return m_stData.m_iExp;
    }
    void AddExp(int iExp)
    {
        if (0 == iExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExp(iExp);
        m_stData.m_iExp += iExp;
    }
    void SetExp(int iExp)
    {
        if (m_stData.m_iExp == iExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iExp - m_stData.m_iExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExp(RealAdd);
        m_stData.m_iExp = iExp;
    }
    const int& GetTotalExp() const
    {
        return m_stData.m_iTotalExp;
    }
    void AddTotalExp(int iTotalExp)
    {
        if (0 == iTotalExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalExp(iTotalExp);
        m_stData.m_iTotalExp += iTotalExp;
    }
    void SetTotalExp(int iTotalExp)
    {
        if (m_stData.m_iTotalExp == iTotalExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iTotalExp - m_stData.m_iTotalExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTotalExp(RealAdd);
        m_stData.m_iTotalExp = iTotalExp;
    }
    const int& GetEquipCardID() const
    {
        return m_stData.m_iEquipCardID;
    }
    void AddEquipCardID(int iEquipCardID)
    {
        if (0 == iEquipCardID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipCardID(iEquipCardID);
        m_stData.m_iEquipCardID += iEquipCardID;
    }
    void SetEquipCardID(int iEquipCardID)
    {
        if (m_stData.m_iEquipCardID == iEquipCardID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquipCardID - m_stData.m_iEquipCardID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipCardID(RealAdd);
        m_stData.m_iEquipCardID = iEquipCardID;
    }
    const char& GetIsLocked() const
    {
        return m_stData.m_cIsLocked;
    }
    void AddIsLocked(char cIsLocked)
    {
        if (0 == cIsLocked)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsLocked(cIsLocked);
        m_stData.m_cIsLocked += cIsLocked;
    }
    void SetIsLocked(char cIsLocked)
    {
        if (m_stData.m_cIsLocked == cIsLocked)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cIsLocked - m_stData.m_cIsLocked;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsLocked(RealAdd);
        m_stData.m_cIsLocked = cIsLocked;
    }
    const char& GetIsDiscard() const
    {
        return m_stData.m_cIsDiscard;
    }
    void AddIsDiscard(char cIsDiscard)
    {
        if (0 == cIsDiscard)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsDiscard(cIsDiscard);
        m_stData.m_cIsDiscard += cIsDiscard;
    }
    void SetIsDiscard(char cIsDiscard)
    {
        if (m_stData.m_cIsDiscard == cIsDiscard)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cIsDiscard - m_stData.m_cIsDiscard;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsDiscard(RealAdd);
        m_stData.m_cIsDiscard = cIsDiscard;
    }
    const int& GetMainAttrType() const
    {
        return m_stData.m_iMainAttrType;
    }
    void AddMainAttrType(int iMainAttrType)
    {
        if (0 == iMainAttrType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMainAttrType(iMainAttrType);
        m_stData.m_iMainAttrType += iMainAttrType;
    }
    void SetMainAttrType(int iMainAttrType)
    {
        if (m_stData.m_iMainAttrType == iMainAttrType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iMainAttrType - m_stData.m_iMainAttrType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMainAttrType(RealAdd);
        m_stData.m_iMainAttrType = iMainAttrType;
    }
    const std::map<int, std::pair<int, int>>& GetExAttr() const
    {
        return m_stData.m_stExAttr;
    }
    void SetExAttr(const std::map<int, std::pair<int, int>>& stExAttr)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExAttr(stExAttr);
        m_stData.m_stExAttr = stExAttr;
    }

    CRecordUserConstellationTable* m_pRecord;
    TUserConstellationTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserConstellationTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserConstellationTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iUniqueID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserConstellationTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
