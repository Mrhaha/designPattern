#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserHomeTable;
struct TUserHomeTableValueType;

class CRecordUserHomeTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserHomeTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserHomeTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetHomeID();

    static std::vector<TUserHomeTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strHomeType;
    static const char* s_strPos;
    static const char* s_strPageID;
    static const char* s_strFloorPageID;
    static const char* s_strHeroVec;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strHomeID;

    TUserHomeTable* m_pCache;

    void SetHomeType(int iHomeType)
    {
        m_stIntData[s_strHomeType] += iHomeType;
    }
    void SetPos(int iPos)
    {
        m_stIntData[s_strPos] += iPos;
    }
    void SetPageID(int iPageID)
    {
        m_stIntData[s_strPageID] += iPageID;
    }
    void SetFloorPageID(int iFloorPageID)
    {
        m_stIntData[s_strFloorPageID] += iFloorPageID;
    }
    void SetHeroVec(const std::map<int32_t, CPackHomeHeroInfo>& stHeroVec)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stHeroVec);
        m_stVarData[s_strHeroVec] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iHomeID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iHomeID = iHomeID;
    }
    void SetCachePtr(TUserHomeTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iHomeID;
};

struct TUserHomeTableValueType
{
    TUserHomeTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iHomeID(0)
    , m_iHomeType(0)
    , m_iPos(0)
    , m_iPageID(0)
    , m_iFloorPageID(0)
    , m_stHeroVec()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iHomeID;
    int                     m_iHomeType;
    int                     m_iPos;
    int                     m_iPageID;
    int                     m_iFloorPageID;
    std::map<int32_t, CPackHomeHeroInfo>    m_stHeroVec;
};

struct TUserHomeTable
{
    TUserHomeTable() : m_pRecord(nullptr) { }
    ~TUserHomeTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserHomeTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserHomeTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iHomeID);
        m_pRecord->SetHomeType(stData.m_iHomeType);
        m_pRecord->SetPos(stData.m_iPos);
        m_pRecord->SetPageID(stData.m_iPageID);
        m_pRecord->SetFloorPageID(stData.m_iFloorPageID);
        m_pRecord->SetHeroVec(stData.m_stHeroVec);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetHomeType() const
    {
        return m_stData.m_iHomeType;
    }
    void AddHomeType(int iHomeType)
    {
        if (0 == iHomeType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHomeType(iHomeType);
        m_stData.m_iHomeType += iHomeType;
    }
    void SetHomeType(int iHomeType)
    {
        if (m_stData.m_iHomeType == iHomeType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iHomeType - m_stData.m_iHomeType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHomeType(RealAdd);
        m_stData.m_iHomeType = iHomeType;
    }
    const int& GetPos() const
    {
        return m_stData.m_iPos;
    }
    void AddPos(int iPos)
    {
        if (0 == iPos)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPos(iPos);
        m_stData.m_iPos += iPos;
    }
    void SetPos(int iPos)
    {
        if (m_stData.m_iPos == iPos)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPos - m_stData.m_iPos;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPos(RealAdd);
        m_stData.m_iPos = iPos;
    }
    const int& GetPageID() const
    {
        return m_stData.m_iPageID;
    }
    void AddPageID(int iPageID)
    {
        if (0 == iPageID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPageID(iPageID);
        m_stData.m_iPageID += iPageID;
    }
    void SetPageID(int iPageID)
    {
        if (m_stData.m_iPageID == iPageID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPageID - m_stData.m_iPageID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPageID(RealAdd);
        m_stData.m_iPageID = iPageID;
    }
    const int& GetFloorPageID() const
    {
        return m_stData.m_iFloorPageID;
    }
    void AddFloorPageID(int iFloorPageID)
    {
        if (0 == iFloorPageID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFloorPageID(iFloorPageID);
        m_stData.m_iFloorPageID += iFloorPageID;
    }
    void SetFloorPageID(int iFloorPageID)
    {
        if (m_stData.m_iFloorPageID == iFloorPageID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iFloorPageID - m_stData.m_iFloorPageID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFloorPageID(RealAdd);
        m_stData.m_iFloorPageID = iFloorPageID;
    }
    const std::map<int32_t, CPackHomeHeroInfo>& GetHeroVec() const
    {
        return m_stData.m_stHeroVec;
    }
    void SetHeroVec(const std::map<int32_t, CPackHomeHeroInfo>& stHeroVec)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHeroVec(stHeroVec);
        m_stData.m_stHeroVec = stHeroVec;
    }

    CRecordUserHomeTable* m_pRecord;
    TUserHomeTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserHomeTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserHomeTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iHomeID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserHomeTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
