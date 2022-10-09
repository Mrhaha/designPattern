#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserEquipTable;
struct TUserEquipTableValueType;

class CRecordUserEquipTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserEquipTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserEquipTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetUniqueID();

    static std::vector<TUserEquipTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strGrade;
    static const char* s_strEquipmentTag;
    static const char* s_strAttrIndex;
    static const char* s_strIsLocked;
    static const char* s_strEquipmentExp;
    static const char* s_strEquipmentRank;
    static const char* s_strEquipmentExIndex;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strUniqueID;

    TUserEquipTable* m_pCache;

    void SetGrade(int iGrade)
    {
        m_stIntData[s_strGrade] += iGrade;
    }
    void SetEquipmentTag(int iEquipmentTag)
    {
        m_stIntData[s_strEquipmentTag] += iEquipmentTag;
    }
    void SetAttrIndex(int iAttrIndex)
    {
        m_stIntData[s_strAttrIndex] += iAttrIndex;
    }
    void SetIsLocked(char cIsLocked)
    {
        m_stCharData[s_strIsLocked] += cIsLocked;
    }
    void SetEquipmentExp(int iEquipmentExp)
    {
        m_stIntData[s_strEquipmentExp] += iEquipmentExp;
    }
    void SetEquipmentRank(int iEquipmentRank)
    {
        m_stIntData[s_strEquipmentRank] += iEquipmentRank;
    }
    void SetEquipmentExIndex(int iEquipmentExIndex)
    {
        m_stIntData[s_strEquipmentExIndex] += iEquipmentExIndex;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iUniqueID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iUniqueID = iUniqueID;
    }
    void SetCachePtr(TUserEquipTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iUniqueID;
};

struct TUserEquipTableValueType
{
    TUserEquipTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iUniqueID(0)
    , m_iGrade(0)
    , m_iEquipmentTag(0)
    , m_iAttrIndex(0)
    , m_cIsLocked(0)
    , m_iEquipmentExp(0)
    , m_iEquipmentRank(0)
    , m_iEquipmentExIndex(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iUniqueID;
    int                     m_iGrade;
    int                     m_iEquipmentTag;
    int                     m_iAttrIndex;
    char                    m_cIsLocked;
    int                     m_iEquipmentExp;
    int                     m_iEquipmentRank;
    int                     m_iEquipmentExIndex;
};

struct TUserEquipTable
{
    TUserEquipTable() : m_pRecord(nullptr) { }
    ~TUserEquipTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserEquipTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserEquipTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iUniqueID);
        m_pRecord->SetGrade(stData.m_iGrade);
        m_pRecord->SetEquipmentTag(stData.m_iEquipmentTag);
        m_pRecord->SetAttrIndex(stData.m_iAttrIndex);
        m_pRecord->SetIsLocked(stData.m_cIsLocked);
        m_pRecord->SetEquipmentExp(stData.m_iEquipmentExp);
        m_pRecord->SetEquipmentRank(stData.m_iEquipmentRank);
        m_pRecord->SetEquipmentExIndex(stData.m_iEquipmentExIndex);
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
    const int& GetAttrIndex() const
    {
        return m_stData.m_iAttrIndex;
    }
    void AddAttrIndex(int iAttrIndex)
    {
        if (0 == iAttrIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAttrIndex(iAttrIndex);
        m_stData.m_iAttrIndex += iAttrIndex;
    }
    void SetAttrIndex(int iAttrIndex)
    {
        if (m_stData.m_iAttrIndex == iAttrIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAttrIndex - m_stData.m_iAttrIndex;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAttrIndex(RealAdd);
        m_stData.m_iAttrIndex = iAttrIndex;
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
    const int& GetEquipmentExp() const
    {
        return m_stData.m_iEquipmentExp;
    }
    void AddEquipmentExp(int iEquipmentExp)
    {
        if (0 == iEquipmentExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipmentExp(iEquipmentExp);
        m_stData.m_iEquipmentExp += iEquipmentExp;
    }
    void SetEquipmentExp(int iEquipmentExp)
    {
        if (m_stData.m_iEquipmentExp == iEquipmentExp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquipmentExp - m_stData.m_iEquipmentExp;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipmentExp(RealAdd);
        m_stData.m_iEquipmentExp = iEquipmentExp;
    }
    const int& GetEquipmentRank() const
    {
        return m_stData.m_iEquipmentRank;
    }
    void AddEquipmentRank(int iEquipmentRank)
    {
        if (0 == iEquipmentRank)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipmentRank(iEquipmentRank);
        m_stData.m_iEquipmentRank += iEquipmentRank;
    }
    void SetEquipmentRank(int iEquipmentRank)
    {
        if (m_stData.m_iEquipmentRank == iEquipmentRank)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquipmentRank - m_stData.m_iEquipmentRank;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipmentRank(RealAdd);
        m_stData.m_iEquipmentRank = iEquipmentRank;
    }
    const int& GetEquipmentExIndex() const
    {
        return m_stData.m_iEquipmentExIndex;
    }
    void AddEquipmentExIndex(int iEquipmentExIndex)
    {
        if (0 == iEquipmentExIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipmentExIndex(iEquipmentExIndex);
        m_stData.m_iEquipmentExIndex += iEquipmentExIndex;
    }
    void SetEquipmentExIndex(int iEquipmentExIndex)
    {
        if (m_stData.m_iEquipmentExIndex == iEquipmentExIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEquipmentExIndex - m_stData.m_iEquipmentExIndex;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEquipmentExIndex(RealAdd);
        m_stData.m_iEquipmentExIndex = iEquipmentExIndex;
    }

    CRecordUserEquipTable* m_pRecord;
    TUserEquipTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserEquipTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserEquipTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iUniqueID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserEquipTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
