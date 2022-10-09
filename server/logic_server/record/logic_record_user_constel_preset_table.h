#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserConstelPresetTable;
struct TUserConstelPresetTableValueType;

class CRecordUserConstelPresetTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserConstelPresetTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserConstelPresetTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetUniqueID();

    static std::vector<TUserConstelPresetTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strName;
    static const char* s_strConstelId1;
    static const char* s_strConstelId2;
    static const char* s_strConstelId3;
    static const char* s_strConstelId4;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strUniqueID;

    TUserConstelPresetTable* m_pCache;

    void SetName(const std::string& strName)
    {
        m_stStringData[s_strName] = strName;
    }
    void SetConstelId1(int iConstelId1)
    {
        m_stIntData[s_strConstelId1] += iConstelId1;
    }
    void SetConstelId2(int iConstelId2)
    {
        m_stIntData[s_strConstelId2] += iConstelId2;
    }
    void SetConstelId3(int iConstelId3)
    {
        m_stIntData[s_strConstelId3] += iConstelId3;
    }
    void SetConstelId4(int iConstelId4)
    {
        m_stIntData[s_strConstelId4] += iConstelId4;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iUniqueID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iUniqueID = iUniqueID;
    }
    void SetCachePtr(TUserConstelPresetTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iUniqueID;
};

struct TUserConstelPresetTableValueType
{
    TUserConstelPresetTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iUniqueID(0)
    , m_strName("")
    , m_iConstelId1(0)
    , m_iConstelId2(0)
    , m_iConstelId3(0)
    , m_iConstelId4(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iUniqueID;
    std::string             m_strName;
    int                     m_iConstelId1;
    int                     m_iConstelId2;
    int                     m_iConstelId3;
    int                     m_iConstelId4;
};

struct TUserConstelPresetTable
{
    TUserConstelPresetTable() : m_pRecord(nullptr) { }
    ~TUserConstelPresetTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserConstelPresetTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserConstelPresetTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iUniqueID);
        m_pRecord->SetName(stData.m_strName);
        m_pRecord->SetConstelId1(stData.m_iConstelId1);
        m_pRecord->SetConstelId2(stData.m_iConstelId2);
        m_pRecord->SetConstelId3(stData.m_iConstelId3);
        m_pRecord->SetConstelId4(stData.m_iConstelId4);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const std::string& GetName() const
    {
        return m_stData.m_strName;
    }
    void SetName(const std::string& strName)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetName(strName);
        m_stData.m_strName = strName;
    }
    const int& GetConstelId1() const
    {
        return m_stData.m_iConstelId1;
    }
    void AddConstelId1(int iConstelId1)
    {
        if (0 == iConstelId1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelId1(iConstelId1);
        m_stData.m_iConstelId1 += iConstelId1;
    }
    void SetConstelId1(int iConstelId1)
    {
        if (m_stData.m_iConstelId1 == iConstelId1)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConstelId1 - m_stData.m_iConstelId1;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelId1(RealAdd);
        m_stData.m_iConstelId1 = iConstelId1;
    }
    const int& GetConstelId2() const
    {
        return m_stData.m_iConstelId2;
    }
    void AddConstelId2(int iConstelId2)
    {
        if (0 == iConstelId2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelId2(iConstelId2);
        m_stData.m_iConstelId2 += iConstelId2;
    }
    void SetConstelId2(int iConstelId2)
    {
        if (m_stData.m_iConstelId2 == iConstelId2)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConstelId2 - m_stData.m_iConstelId2;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelId2(RealAdd);
        m_stData.m_iConstelId2 = iConstelId2;
    }
    const int& GetConstelId3() const
    {
        return m_stData.m_iConstelId3;
    }
    void AddConstelId3(int iConstelId3)
    {
        if (0 == iConstelId3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelId3(iConstelId3);
        m_stData.m_iConstelId3 += iConstelId3;
    }
    void SetConstelId3(int iConstelId3)
    {
        if (m_stData.m_iConstelId3 == iConstelId3)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConstelId3 - m_stData.m_iConstelId3;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelId3(RealAdd);
        m_stData.m_iConstelId3 = iConstelId3;
    }
    const int& GetConstelId4() const
    {
        return m_stData.m_iConstelId4;
    }
    void AddConstelId4(int iConstelId4)
    {
        if (0 == iConstelId4)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelId4(iConstelId4);
        m_stData.m_iConstelId4 += iConstelId4;
    }
    void SetConstelId4(int iConstelId4)
    {
        if (m_stData.m_iConstelId4 == iConstelId4)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iConstelId4 - m_stData.m_iConstelId4;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetConstelId4(RealAdd);
        m_stData.m_iConstelId4 = iConstelId4;
    }

    CRecordUserConstelPresetTable* m_pRecord;
    TUserConstelPresetTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserConstelPresetTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserConstelPresetTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iUniqueID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserConstelPresetTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
