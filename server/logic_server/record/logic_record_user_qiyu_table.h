#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserQiyuTable;
struct TUserQiyuTableValueType;

class CRecordUserQiyuTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserQiyuTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserQiyuTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetMapID();
    virtual int GetNodeID();

    static std::vector<TUserQiyuTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strNodeState;
    static const char* s_strNodeEventType;
    static const char* s_strNodeEventID;
    static const char* s_strParamsInt;
    static const char* s_strParamsStr;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strMapID;
    static const char* s_strNodeID;

    TUserQiyuTable* m_pCache;

    void SetNodeState(int iNodeState)
    {
        m_stIntData[s_strNodeState] += iNodeState;
    }
    void SetNodeEventType(int iNodeEventType)
    {
        m_stIntData[s_strNodeEventType] += iNodeEventType;
    }
    void SetNodeEventID(int iNodeEventID)
    {
        m_stIntData[s_strNodeEventID] += iNodeEventID;
    }
    void SetParamsInt(const std::vector<int32_t>& stParamsInt)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stParamsInt);
        m_stVarData[s_strParamsInt] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetParamsStr(const std::vector<std::string>& stParamsStr)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stParamsStr);
        m_stVarData[s_strParamsStr] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iMapID, int iNodeID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iMapID = iMapID;
        m_iNodeID = iNodeID;
    }
    void SetCachePtr(TUserQiyuTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iMapID;
    int m_iNodeID;
};

struct TUserQiyuTableValueType
{
    TUserQiyuTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iMapID(0)
    , m_iNodeID(0)
    , m_iNodeState(0)
    , m_iNodeEventType(0)
    , m_iNodeEventID(0)
    , m_stParamsInt()
    , m_stParamsStr()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iMapID;
    int                     m_iNodeID;
    int                     m_iNodeState;
    int                     m_iNodeEventType;
    int                     m_iNodeEventID;
    std::vector<int32_t>    m_stParamsInt;
    std::vector<std::string>    m_stParamsStr;
};

struct TUserQiyuTable
{
    TUserQiyuTable() : m_pRecord(nullptr) { }
    ~TUserQiyuTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserQiyuTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserQiyuTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iMapID, stData.m_iNodeID);
        m_pRecord->SetNodeState(stData.m_iNodeState);
        m_pRecord->SetNodeEventType(stData.m_iNodeEventType);
        m_pRecord->SetNodeEventID(stData.m_iNodeEventID);
        m_pRecord->SetParamsInt(stData.m_stParamsInt);
        m_pRecord->SetParamsStr(stData.m_stParamsStr);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetNodeState() const
    {
        return m_stData.m_iNodeState;
    }
    void AddNodeState(int iNodeState)
    {
        if (0 == iNodeState)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNodeState(iNodeState);
        m_stData.m_iNodeState += iNodeState;
    }
    void SetNodeState(int iNodeState)
    {
        if (m_stData.m_iNodeState == iNodeState)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNodeState - m_stData.m_iNodeState;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNodeState(RealAdd);
        m_stData.m_iNodeState = iNodeState;
    }
    const int& GetNodeEventType() const
    {
        return m_stData.m_iNodeEventType;
    }
    void AddNodeEventType(int iNodeEventType)
    {
        if (0 == iNodeEventType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNodeEventType(iNodeEventType);
        m_stData.m_iNodeEventType += iNodeEventType;
    }
    void SetNodeEventType(int iNodeEventType)
    {
        if (m_stData.m_iNodeEventType == iNodeEventType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNodeEventType - m_stData.m_iNodeEventType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNodeEventType(RealAdd);
        m_stData.m_iNodeEventType = iNodeEventType;
    }
    const int& GetNodeEventID() const
    {
        return m_stData.m_iNodeEventID;
    }
    void AddNodeEventID(int iNodeEventID)
    {
        if (0 == iNodeEventID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNodeEventID(iNodeEventID);
        m_stData.m_iNodeEventID += iNodeEventID;
    }
    void SetNodeEventID(int iNodeEventID)
    {
        if (m_stData.m_iNodeEventID == iNodeEventID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iNodeEventID - m_stData.m_iNodeEventID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNodeEventID(RealAdd);
        m_stData.m_iNodeEventID = iNodeEventID;
    }
    const std::vector<int32_t>& GetParamsInt() const
    {
        return m_stData.m_stParamsInt;
    }
    void SetParamsInt(const std::vector<int32_t>& stParamsInt)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetParamsInt(stParamsInt);
        m_stData.m_stParamsInt = stParamsInt;
    }
    const std::vector<std::string>& GetParamsStr() const
    {
        return m_stData.m_stParamsStr;
    }
    void SetParamsStr(const std::vector<std::string>& stParamsStr)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetParamsStr(stParamsStr);
        m_stData.m_stParamsStr = stParamsStr;
    }

    CRecordUserQiyuTable* m_pRecord;
    TUserQiyuTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserQiyuTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserQiyuTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iMapID, m_stData.m_iNodeID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserQiyuTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
