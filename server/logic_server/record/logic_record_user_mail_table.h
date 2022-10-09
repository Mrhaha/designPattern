#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserMailTable;
struct TUserMailTableValueType;

class CRecordUserMailTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserMailTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserMailTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();
    virtual int GetMailID();

    static std::vector<TUserMailTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strIsNew;
    static const char* s_strIsReceived;
    static const char* s_strCreateTime;
    static const char* s_strReadTime;
    static const char* s_strTitle;
    static const char* s_strSender;
    static const char* s_strContent;
    static const char* s_strAttachment;
    static const char* s_strAwardMultiple;
    static const char* s_strExpiredTime;
    static const char* s_strUid;
    static const char* s_strGroupID;
    static const char* s_strMailID;

    TUserMailTable* m_pCache;

    void SetIsNew(int iIsNew)
    {
        m_stIntData[s_strIsNew] += iIsNew;
    }
    void SetIsReceived(int iIsReceived)
    {
        m_stIntData[s_strIsReceived] += iIsReceived;
    }
    void SetCreateTime(int iCreateTime)
    {
        m_stIntData[s_strCreateTime] += iCreateTime;
    }
    void SetReadTime(int iReadTime)
    {
        m_stIntData[s_strReadTime] += iReadTime;
    }
    void SetTitle(const std::string& strTitle)
    {
        m_stStringData[s_strTitle] = strTitle;
    }
    void SetSender(const std::string& strSender)
    {
        m_stStringData[s_strSender] = strSender;
    }
    void SetContent(const std::string& strContent)
    {
        m_stStringData[s_strContent] = strContent;
    }
    void SetAttachment(const std::vector<CPackGameItem>& stAttachment)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stAttachment);
        m_stVarData[s_strAttachment] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetAwardMultiple(int iAwardMultiple)
    {
        m_stIntData[s_strAwardMultiple] += iAwardMultiple;
    }
    void SetExpiredTime(int iExpiredTime)
    {
        m_stIntData[s_strExpiredTime] += iExpiredTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID, int iMailID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
        m_iMailID = iMailID;
    }
    void SetCachePtr(TUserMailTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
    int m_iMailID;
};

struct TUserMailTableValueType
{
    TUserMailTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iMailID(0)
    , m_iIsNew(0)
    , m_iIsReceived(0)
    , m_iCreateTime(0)
    , m_iReadTime(0)
    , m_strTitle("")
    , m_strSender("")
    , m_strContent("")
    , m_stAttachment()
    , m_iAwardMultiple(1)
    , m_iExpiredTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iMailID;
    int                     m_iIsNew;
    int                     m_iIsReceived;
    int                     m_iCreateTime;
    int                     m_iReadTime;
    std::string             m_strTitle;
    std::string             m_strSender;
    std::string             m_strContent;
    std::vector<CPackGameItem>    m_stAttachment;
    int                     m_iAwardMultiple;
    int                     m_iExpiredTime;
};

struct TUserMailTable
{
    TUserMailTable() : m_pRecord(nullptr) { }
    ~TUserMailTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserMailTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserMailTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID, stData.m_iMailID);
        m_pRecord->SetIsNew(stData.m_iIsNew);
        m_pRecord->SetIsReceived(stData.m_iIsReceived);
        m_pRecord->SetCreateTime(stData.m_iCreateTime);
        m_pRecord->SetReadTime(stData.m_iReadTime);
        m_pRecord->SetTitle(stData.m_strTitle);
        m_pRecord->SetSender(stData.m_strSender);
        m_pRecord->SetContent(stData.m_strContent);
        m_pRecord->SetAttachment(stData.m_stAttachment);
        m_pRecord->SetAwardMultiple(stData.m_iAwardMultiple);
        m_pRecord->SetExpiredTime(stData.m_iExpiredTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetIsNew() const
    {
        return m_stData.m_iIsNew;
    }
    void AddIsNew(int iIsNew)
    {
        if (0 == iIsNew)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsNew(iIsNew);
        m_stData.m_iIsNew += iIsNew;
    }
    void SetIsNew(int iIsNew)
    {
        if (m_stData.m_iIsNew == iIsNew)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iIsNew - m_stData.m_iIsNew;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsNew(RealAdd);
        m_stData.m_iIsNew = iIsNew;
    }
    const int& GetIsReceived() const
    {
        return m_stData.m_iIsReceived;
    }
    void AddIsReceived(int iIsReceived)
    {
        if (0 == iIsReceived)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsReceived(iIsReceived);
        m_stData.m_iIsReceived += iIsReceived;
    }
    void SetIsReceived(int iIsReceived)
    {
        if (m_stData.m_iIsReceived == iIsReceived)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iIsReceived - m_stData.m_iIsReceived;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsReceived(RealAdd);
        m_stData.m_iIsReceived = iIsReceived;
    }
    const int& GetCreateTime() const
    {
        return m_stData.m_iCreateTime;
    }
    void AddCreateTime(int iCreateTime)
    {
        if (0 == iCreateTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCreateTime(iCreateTime);
        m_stData.m_iCreateTime += iCreateTime;
    }
    void SetCreateTime(int iCreateTime)
    {
        if (m_stData.m_iCreateTime == iCreateTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCreateTime - m_stData.m_iCreateTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCreateTime(RealAdd);
        m_stData.m_iCreateTime = iCreateTime;
    }
    const int& GetReadTime() const
    {
        return m_stData.m_iReadTime;
    }
    void AddReadTime(int iReadTime)
    {
        if (0 == iReadTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetReadTime(iReadTime);
        m_stData.m_iReadTime += iReadTime;
    }
    void SetReadTime(int iReadTime)
    {
        if (m_stData.m_iReadTime == iReadTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iReadTime - m_stData.m_iReadTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetReadTime(RealAdd);
        m_stData.m_iReadTime = iReadTime;
    }
    const std::string& GetTitle() const
    {
        return m_stData.m_strTitle;
    }
    void SetTitle(const std::string& strTitle)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTitle(strTitle);
        m_stData.m_strTitle = strTitle;
    }
    const std::string& GetSender() const
    {
        return m_stData.m_strSender;
    }
    void SetSender(const std::string& strSender)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetSender(strSender);
        m_stData.m_strSender = strSender;
    }
    const std::string& GetContent() const
    {
        return m_stData.m_strContent;
    }
    void SetContent(const std::string& strContent)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetContent(strContent);
        m_stData.m_strContent = strContent;
    }
    const std::vector<CPackGameItem>& GetAttachment() const
    {
        return m_stData.m_stAttachment;
    }
    void SetAttachment(const std::vector<CPackGameItem>& stAttachment)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAttachment(stAttachment);
        m_stData.m_stAttachment = stAttachment;
    }
    const int& GetAwardMultiple() const
    {
        return m_stData.m_iAwardMultiple;
    }
    void AddAwardMultiple(int iAwardMultiple)
    {
        if (0 == iAwardMultiple)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAwardMultiple(iAwardMultiple);
        m_stData.m_iAwardMultiple += iAwardMultiple;
    }
    void SetAwardMultiple(int iAwardMultiple)
    {
        if (m_stData.m_iAwardMultiple == iAwardMultiple)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iAwardMultiple - m_stData.m_iAwardMultiple;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAwardMultiple(RealAdd);
        m_stData.m_iAwardMultiple = iAwardMultiple;
    }
    const int& GetExpiredTime() const
    {
        return m_stData.m_iExpiredTime;
    }
    void AddExpiredTime(int iExpiredTime)
    {
        if (0 == iExpiredTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExpiredTime(iExpiredTime);
        m_stData.m_iExpiredTime += iExpiredTime;
    }
    void SetExpiredTime(int iExpiredTime)
    {
        if (m_stData.m_iExpiredTime == iExpiredTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iExpiredTime - m_stData.m_iExpiredTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetExpiredTime(RealAdd);
        m_stData.m_iExpiredTime = iExpiredTime;
    }

    CRecordUserMailTable* m_pRecord;
    TUserMailTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserMailTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserMailTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID, m_stData.m_iMailID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserMailTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
