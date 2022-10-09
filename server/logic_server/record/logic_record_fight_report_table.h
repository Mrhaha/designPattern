#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TFightReportTable;
struct TFightReportTableValueType;

class CRecordFightReportTable : public CLogicSQLTableRecordBase
{
public:
    CRecordFightReportTable() : m_pCache(nullptr) { }

    virtual ~CRecordFightReportTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetReportType();
    virtual int GetReportID();

    static std::vector<TFightReportTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strReportContent;
    static const char* s_strFightResult;
    static const char* s_strParams;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strReportType;
    static const char* s_strReportID;

    TFightReportTable* m_pCache;

    void SetReportContent(const std::string& strReportContent)
    {
        m_stStringData[s_strReportContent] = strReportContent;
    }
    void SetFightResult(char cFightResult)
    {
        m_stCharData[s_strFightResult] += cFightResult;
    }
    void SetParams(const std::vector<int32_t>& stParams)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stParams);
        m_stVarData[s_strParams] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iReportType, int iReportID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iReportType = iReportType;
        m_iReportID = iReportID;
    }
    void SetCachePtr(TFightReportTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iReportType;
    int m_iReportID;
};

struct TFightReportTableValueType
{
    TFightReportTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iReportType(0)
    , m_iReportID(0)
    , m_strReportContent("")
    , m_cFightResult(0)
    , m_stParams()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iReportType;
    int                     m_iReportID;
    std::string             m_strReportContent;
    char                    m_cFightResult;
    std::vector<int32_t>    m_stParams;
};

struct TFightReportTable
{
    TFightReportTable() : m_pRecord(nullptr) { }
    ~TFightReportTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TFightReportTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TFightReportTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iReportType, stData.m_iReportID);
        m_pRecord->SetReportContent(stData.m_strReportContent);
        m_pRecord->SetFightResult(stData.m_cFightResult);
        m_pRecord->SetParams(stData.m_stParams);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const std::string& GetReportContent() const
    {
        return m_stData.m_strReportContent;
    }
    void SetReportContent(const std::string& strReportContent)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetReportContent(strReportContent);
        m_stData.m_strReportContent = strReportContent;
    }
    const char& GetFightResult() const
    {
        return m_stData.m_cFightResult;
    }
    void AddFightResult(char cFightResult)
    {
        if (0 == cFightResult)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightResult(cFightResult);
        m_stData.m_cFightResult += cFightResult;
    }
    void SetFightResult(char cFightResult)
    {
        if (m_stData.m_cFightResult == cFightResult)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cFightResult - m_stData.m_cFightResult;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetFightResult(RealAdd);
        m_stData.m_cFightResult = cFightResult;
    }
    const std::vector<int32_t>& GetParams() const
    {
        return m_stData.m_stParams;
    }
    void SetParams(const std::vector<int32_t>& stParams)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetParams(stParams);
        m_stData.m_stParams = stParams;
    }

    CRecordFightReportTable* m_pRecord;
    TFightReportTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordFightReportTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordFightReportTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iReportType, m_stData.m_iReportID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordFightReportTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
