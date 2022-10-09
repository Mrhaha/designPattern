#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGlobalWorldQuestTable;
struct TGlobalWorldQuestTableValueType;

class CRecordGlobalWorldQuestTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGlobalWorldQuestTable() : m_pCache(nullptr) { }

    virtual ~CRecordGlobalWorldQuestTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetServerID();
    virtual int GetQuestID();
    virtual int GetUid() { return 0; }
    virtual int GetGroupID() { return 0; }

    static std::vector<TGlobalWorldQuestTableValueType> SelectDataFromSQL(int iServerID);

    static const char* s_strTableName;
    static const char* s_strProgress;
    static const char* s_strState;
    static const char* s_strOpenTime;
    static const char* s_strEndTime;
    static const char* s_strAwardInfo;
    static const char* s_strServerID;
    static const char* s_strQuestID;

    TGlobalWorldQuestTable* m_pCache;

    void SetProgress(int iProgress)
    {
        m_stIntData[s_strProgress] += iProgress;
    }
    void SetState(char cState)
    {
        m_stCharData[s_strState] += cState;
    }
    void SetOpenTime(int iOpenTime)
    {
        m_stIntData[s_strOpenTime] += iOpenTime;
    }
    void SetEndTime(int iEndTime)
    {
        m_stIntData[s_strEndTime] += iEndTime;
    }
    void SetAwardInfo(const std::vector<CPackWorldQuestAwardInfo>& stAwardInfo)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stAwardInfo);
        m_stVarData[s_strAwardInfo] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iServerID, int iQuestID)
    {
        m_iServerID = iServerID;
        m_iQuestID = iQuestID;
    }
    void SetCachePtr(TGlobalWorldQuestTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iServerID;
    int m_iQuestID;
};

struct TGlobalWorldQuestTableValueType
{
    TGlobalWorldQuestTableValueType()
    : m_iServerID(0)
    , m_iQuestID(0)
    , m_iProgress(0)
    , m_cState(0)
    , m_iOpenTime(0)
    , m_iEndTime(0)
    , m_stAwardInfo()
    {}

    int                     m_iServerID;
    int                     m_iQuestID;
    int                     m_iProgress;
    char                    m_cState;
    int                     m_iOpenTime;
    int                     m_iEndTime;
    std::vector<CPackWorldQuestAwardInfo>    m_stAwardInfo;
};

struct TGlobalWorldQuestTable
{
    TGlobalWorldQuestTable() : m_pRecord(nullptr) { }
    ~TGlobalWorldQuestTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGlobalWorldQuestTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGlobalWorldQuestTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iServerID, stData.m_iQuestID);
        m_pRecord->SetProgress(stData.m_iProgress);
        m_pRecord->SetState(stData.m_cState);
        m_pRecord->SetOpenTime(stData.m_iOpenTime);
        m_pRecord->SetEndTime(stData.m_iEndTime);
        m_pRecord->SetAwardInfo(stData.m_stAwardInfo);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
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
    const int& GetOpenTime() const
    {
        return m_stData.m_iOpenTime;
    }
    void AddOpenTime(int iOpenTime)
    {
        if (0 == iOpenTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetOpenTime(iOpenTime);
        m_stData.m_iOpenTime += iOpenTime;
    }
    void SetOpenTime(int iOpenTime)
    {
        if (m_stData.m_iOpenTime == iOpenTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iOpenTime - m_stData.m_iOpenTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetOpenTime(RealAdd);
        m_stData.m_iOpenTime = iOpenTime;
    }
    const int& GetEndTime() const
    {
        return m_stData.m_iEndTime;
    }
    void AddEndTime(int iEndTime)
    {
        if (0 == iEndTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEndTime(iEndTime);
        m_stData.m_iEndTime += iEndTime;
    }
    void SetEndTime(int iEndTime)
    {
        if (m_stData.m_iEndTime == iEndTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iEndTime - m_stData.m_iEndTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetEndTime(RealAdd);
        m_stData.m_iEndTime = iEndTime;
    }
    const std::vector<CPackWorldQuestAwardInfo>& GetAwardInfo() const
    {
        return m_stData.m_stAwardInfo;
    }
    void SetAwardInfo(const std::vector<CPackWorldQuestAwardInfo>& stAwardInfo)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAwardInfo(stAwardInfo);
        m_stData.m_stAwardInfo = stAwardInfo;
    }

    CRecordGlobalWorldQuestTable* m_pRecord;
    TGlobalWorldQuestTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGlobalWorldQuestTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGlobalWorldQuestTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iServerID, m_stData.m_iQuestID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGlobalWorldQuestTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
