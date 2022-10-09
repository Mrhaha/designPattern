#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserQuestTable;
struct TUserQuestTableValueType;

class CRecordUserQuestTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserQuestTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserQuestTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetQuestID();

    static std::vector<TUserQuestTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strParaIndex;
    static const char* s_strProgress;
    static const char* s_strState;
    static const char* s_strProgressSet;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strQuestID;

    TUserQuestTable* m_pCache;

    void SetParaIndex(int iParaIndex)
    {
        m_stIntData[s_strParaIndex] += iParaIndex;
    }
    void SetProgress(int iProgress)
    {
        m_stIntData[s_strProgress] += iProgress;
    }
    void SetState(char cState)
    {
        m_stCharData[s_strState] += cState;
    }
    void SetProgressSet(const std::set<int32_t>& stProgressSet)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stProgressSet);
        m_stVarData[s_strProgressSet] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iQuestID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iQuestID = iQuestID;
    }
    void SetCachePtr(TUserQuestTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iQuestID;
};

struct TUserQuestTableValueType
{
    TUserQuestTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iQuestID(0)
    , m_iParaIndex(0)
    , m_iProgress(0)
    , m_cState(0)
    , m_stProgressSet()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iQuestID;
    int                     m_iParaIndex;
    int                     m_iProgress;
    char                    m_cState;
    std::set<int32_t>       m_stProgressSet;
};

struct TUserQuestTable
{
    TUserQuestTable() : m_pRecord(nullptr) { }
    ~TUserQuestTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserQuestTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserQuestTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iQuestID);
        m_pRecord->SetParaIndex(stData.m_iParaIndex);
        m_pRecord->SetProgress(stData.m_iProgress);
        m_pRecord->SetState(stData.m_cState);
        m_pRecord->SetProgressSet(stData.m_stProgressSet);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetParaIndex() const
    {
        return m_stData.m_iParaIndex;
    }
    void AddParaIndex(int iParaIndex)
    {
        if (0 == iParaIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetParaIndex(iParaIndex);
        m_stData.m_iParaIndex += iParaIndex;
    }
    void SetParaIndex(int iParaIndex)
    {
        if (m_stData.m_iParaIndex == iParaIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iParaIndex - m_stData.m_iParaIndex;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetParaIndex(RealAdd);
        m_stData.m_iParaIndex = iParaIndex;
    }
    const int& GetProgress() const
    {
        return m_stData.m_iProgress;
    }
    void AddProgress(int iProgress)
    {
        if (0 == iProgress)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetProgress(iProgress);
        m_stData.m_iProgress += iProgress;
    }
    void SetProgress(int iProgress)
    {
        if (m_stData.m_iProgress == iProgress)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iProgress - m_stData.m_iProgress;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetProgress(RealAdd);
        m_stData.m_iProgress = iProgress;
    }
    const char& GetState() const
    {
        return m_stData.m_cState;
    }
    void AddState(char cState)
    {
        if (0 == cState)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetState(cState);
        m_stData.m_cState += cState;
    }
    void SetState(char cState)
    {
        if (m_stData.m_cState == cState)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cState - m_stData.m_cState;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetState(RealAdd);
        m_stData.m_cState = cState;
    }
    const std::set<int32_t>& GetProgressSet() const
    {
        return m_stData.m_stProgressSet;
    }
    void SetProgressSet(const std::set<int32_t>& stProgressSet)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetProgressSet(stProgressSet);
        m_stData.m_stProgressSet = stProgressSet;
    }

    CRecordUserQuestTable* m_pRecord;
    TUserQuestTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserQuestTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserQuestTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iQuestID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserQuestTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
