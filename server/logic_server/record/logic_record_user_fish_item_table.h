#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserFishItemTable;
struct TUserFishItemTableValueType;

class CRecordUserFishItemTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserFishItemTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserFishItemTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetFishIndex();

    static std::vector<TUserFishItemTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strFishID;
    static const char* s_strValue;
    static const char* s_strGrade;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strFishIndex;

    TUserFishItemTable* m_pCache;

    void SetFishID(int iFishID)
    {
        m_stIntData[s_strFishID] += iFishID;
    }
    void SetValue(int iValue)
    {
        m_stIntData[s_strValue] += iValue;
    }
    void SetGrade(char cGrade)
    {
        m_stCharData[s_strGrade] += cGrade;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iFishIndex)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iFishIndex = iFishIndex;
    }
    void SetCachePtr(TUserFishItemTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iFishIndex;
};

struct TUserFishItemTableValueType
{
    TUserFishItemTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iFishIndex(0)
    , m_iFishID(0)
    , m_iValue(0)
    , m_cGrade(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iFishIndex;
    int                     m_iFishID;
    int                     m_iValue;
    char                    m_cGrade;
};

struct TUserFishItemTable
{
    TUserFishItemTable() : m_pRecord(nullptr) { }
    ~TUserFishItemTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserFishItemTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserFishItemTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iFishIndex);
        m_pRecord->SetFishID(stData.m_iFishID);
        m_pRecord->SetValue(stData.m_iValue);
        m_pRecord->SetGrade(stData.m_cGrade);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetFishID() const
    {
        return m_stData.m_iFishID;
    }
    void AddFishID(int iFishID)
    {
        if (0 == iFishID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFishID(iFishID);
        m_stData.m_iFishID += iFishID;
    }
    void SetFishID(int iFishID)
    {
        if (m_stData.m_iFishID == iFishID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFishID - m_stData.m_iFishID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFishID(RealAdd);
        m_stData.m_iFishID = iFishID;
    }
    const int& GetValue() const
    {
        return m_stData.m_iValue;
    }
    void AddValue(int iValue)
    {
        if (0 == iValue)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetValue(iValue);
        m_stData.m_iValue += iValue;
    }
    void SetValue(int iValue)
    {
        if (m_stData.m_iValue == iValue)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iValue - m_stData.m_iValue;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetValue(RealAdd);
        m_stData.m_iValue = iValue;
    }
    const char& GetGrade() const
    {
        return m_stData.m_cGrade;
    }
    void AddGrade(char cGrade)
    {
        if (0 == cGrade)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGrade(cGrade);
        m_stData.m_cGrade += cGrade;
    }
    void SetGrade(char cGrade)
    {
        if (m_stData.m_cGrade == cGrade)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cGrade - m_stData.m_cGrade;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGrade(RealAdd);
        m_stData.m_cGrade = cGrade;
    }

    CRecordUserFishItemTable* m_pRecord;
    TUserFishItemTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserFishItemTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserFishItemTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iFishIndex);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserFishItemTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
