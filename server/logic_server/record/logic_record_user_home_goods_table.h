#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserHomeGoodsTable;
struct TUserHomeGoodsTableValueType;

class CRecordUserHomeGoodsTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserHomeGoodsTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserHomeGoodsTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetHomeID();
    virtual int GetGoodsIndex();

    static std::vector<TUserHomeGoodsTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strGoodsID;
    static const char* s_strPosX;
    static const char* s_strPosY;
    static const char* s_strStat;
    static const char* s_strQuadrant;
    static const char* s_strSeatIndex;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strHomeID;
    static const char* s_strGoodsIndex;

    TUserHomeGoodsTable* m_pCache;

    void SetGoodsID(int iGoodsID)
    {
        m_stIntData[s_strGoodsID] += iGoodsID;
    }
    void SetPosX(int iPosX)
    {
        m_stIntData[s_strPosX] += iPosX;
    }
    void SetPosY(int iPosY)
    {
        m_stIntData[s_strPosY] += iPosY;
    }
    void SetStat(char cStat)
    {
        m_stCharData[s_strStat] += cStat;
    }
    void SetQuadrant(char cQuadrant)
    {
        m_stCharData[s_strQuadrant] += cQuadrant;
    }
    void SetSeatIndex(int iSeatIndex)
    {
        m_stIntData[s_strSeatIndex] += iSeatIndex;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iHomeID, int iGoodsIndex)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iHomeID = iHomeID;
        m_iGoodsIndex = iGoodsIndex;
    }
    void SetCachePtr(TUserHomeGoodsTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iHomeID;
    int m_iGoodsIndex;
};

struct TUserHomeGoodsTableValueType
{
    TUserHomeGoodsTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iHomeID(0)
    , m_iGoodsIndex(0)
    , m_iGoodsID(0)
    , m_iPosX(0)
    , m_iPosY(0)
    , m_cStat(0)
    , m_cQuadrant(0)
    , m_iSeatIndex(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iHomeID;
    int                     m_iGoodsIndex;
    int                     m_iGoodsID;
    int                     m_iPosX;
    int                     m_iPosY;
    char                    m_cStat;
    char                    m_cQuadrant;
    int                     m_iSeatIndex;
};

struct TUserHomeGoodsTable
{
    TUserHomeGoodsTable() : m_pRecord(nullptr) { }
    ~TUserHomeGoodsTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserHomeGoodsTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserHomeGoodsTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iHomeID, stData.m_iGoodsIndex);
        m_pRecord->SetGoodsID(stData.m_iGoodsID);
        m_pRecord->SetPosX(stData.m_iPosX);
        m_pRecord->SetPosY(stData.m_iPosY);
        m_pRecord->SetStat(stData.m_cStat);
        m_pRecord->SetQuadrant(stData.m_cQuadrant);
        m_pRecord->SetSeatIndex(stData.m_iSeatIndex);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetGoodsID() const
    {
        return m_stData.m_iGoodsID;
    }
    void AddGoodsID(int iGoodsID)
    {
        if (0 == iGoodsID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGoodsID(iGoodsID);
        m_stData.m_iGoodsID += iGoodsID;
    }
    void SetGoodsID(int iGoodsID)
    {
        if (m_stData.m_iGoodsID == iGoodsID)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iGoodsID - m_stData.m_iGoodsID;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetGoodsID(RealAdd);
        m_stData.m_iGoodsID = iGoodsID;
    }
    const int& GetPosX() const
    {
        return m_stData.m_iPosX;
    }
    void AddPosX(int iPosX)
    {
        if (0 == iPosX)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPosX(iPosX);
        m_stData.m_iPosX += iPosX;
    }
    void SetPosX(int iPosX)
    {
        if (m_stData.m_iPosX == iPosX)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPosX - m_stData.m_iPosX;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPosX(RealAdd);
        m_stData.m_iPosX = iPosX;
    }
    const int& GetPosY() const
    {
        return m_stData.m_iPosY;
    }
    void AddPosY(int iPosY)
    {
        if (0 == iPosY)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPosY(iPosY);
        m_stData.m_iPosY += iPosY;
    }
    void SetPosY(int iPosY)
    {
        if (m_stData.m_iPosY == iPosY)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPosY - m_stData.m_iPosY;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPosY(RealAdd);
        m_stData.m_iPosY = iPosY;
    }
    const char& GetStat() const
    {
        return m_stData.m_cStat;
    }
    void AddStat(char cStat)
    {
        if (0 == cStat)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetStat(cStat);
        m_stData.m_cStat += cStat;
    }
    void SetStat(char cStat)
    {
        if (m_stData.m_cStat == cStat)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cStat - m_stData.m_cStat;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetStat(RealAdd);
        m_stData.m_cStat = cStat;
    }
    const char& GetQuadrant() const
    {
        return m_stData.m_cQuadrant;
    }
    void AddQuadrant(char cQuadrant)
    {
        if (0 == cQuadrant)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQuadrant(cQuadrant);
        m_stData.m_cQuadrant += cQuadrant;
    }
    void SetQuadrant(char cQuadrant)
    {
        if (m_stData.m_cQuadrant == cQuadrant)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cQuadrant - m_stData.m_cQuadrant;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetQuadrant(RealAdd);
        m_stData.m_cQuadrant = cQuadrant;
    }
    const int& GetSeatIndex() const
    {
        return m_stData.m_iSeatIndex;
    }
    void AddSeatIndex(int iSeatIndex)
    {
        if (0 == iSeatIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSeatIndex(iSeatIndex);
        m_stData.m_iSeatIndex += iSeatIndex;
    }
    void SetSeatIndex(int iSeatIndex)
    {
        if (m_stData.m_iSeatIndex == iSeatIndex)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iSeatIndex - m_stData.m_iSeatIndex;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSeatIndex(RealAdd);
        m_stData.m_iSeatIndex = iSeatIndex;
    }

    CRecordUserHomeGoodsTable* m_pRecord;
    TUserHomeGoodsTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserHomeGoodsTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserHomeGoodsTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iHomeID, m_stData.m_iGoodsIndex);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserHomeGoodsTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
