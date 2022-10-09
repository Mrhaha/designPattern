#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGlobalMailTable;
struct TGlobalMailTableValueType;

class CRecordGlobalMailTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGlobalMailTable() : m_pCache(nullptr) { }

    virtual ~CRecordGlobalMailTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetServerID();
    virtual int GetMailID();
    virtual int GetUid() { return 0; }
    virtual int GetGroupID() { return 0; }

    static std::vector<TGlobalMailTableValueType> SelectDataFromSQL(int iServerID);

    static const char* s_strTableName;
    static const char* s_strBeginTime;
    static const char* s_strEndTime;
    static const char* s_strVipLevel;
    static const char* s_strTitle;
    static const char* s_strSender;
    static const char* s_strContent;
    static const char* s_strAttachment;
    static const char* s_strChannelIDVec;
    static const char* s_strServerID;
    static const char* s_strMailID;

    TGlobalMailTable* m_pCache;

    void SetBeginTime(int iBeginTime)
    {
        m_stIntData[s_strBeginTime] += iBeginTime;
    }
    void SetEndTime(int iEndTime)
    {
        m_stIntData[s_strEndTime] += iEndTime;
    }
    void SetVipLevel(int iVipLevel)
    {
        m_stIntData[s_strVipLevel] += iVipLevel;
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
    void SetChannelIDVec(const std::set<int>& stChannelIDVec)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stChannelIDVec);
        m_stVarData[s_strChannelIDVec] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iServerID, int iMailID)
    {
        m_iServerID = iServerID;
        m_iMailID = iMailID;
    }
    void SetCachePtr(TGlobalMailTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iServerID;
    int m_iMailID;
};

struct TGlobalMailTableValueType
{
    TGlobalMailTableValueType()
    : m_iServerID(0)
    , m_iMailID(0)
    , m_iBeginTime(0)
    , m_iEndTime(0)
    , m_iVipLevel(0)
    , m_strTitle("")
    , m_strSender("")
    , m_strContent("")
    , m_stAttachment()
    , m_stChannelIDVec()
    {}

    int                     m_iServerID;
    int                     m_iMailID;
    int                     m_iBeginTime;
    int                     m_iEndTime;
    int                     m_iVipLevel;
    std::string             m_strTitle;
    std::string             m_strSender;
    std::string             m_strContent;
    std::vector<CPackGameItem>    m_stAttachment;
    std::set<int>           m_stChannelIDVec;
};

struct TGlobalMailTable
{
    TGlobalMailTable() : m_pRecord(nullptr) { }
    ~TGlobalMailTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGlobalMailTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGlobalMailTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iServerID, stData.m_iMailID);
        m_pRecord->SetBeginTime(stData.m_iBeginTime);
        m_pRecord->SetEndTime(stData.m_iEndTime);
        m_pRecord->SetVipLevel(stData.m_iVipLevel);
        m_pRecord->SetTitle(stData.m_strTitle);
        m_pRecord->SetSender(stData.m_strSender);
        m_pRecord->SetContent(stData.m_strContent);
        m_pRecord->SetAttachment(stData.m_stAttachment);
        m_pRecord->SetChannelIDVec(stData.m_stChannelIDVec);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetBeginTime() const
    {
        return m_stData.m_iBeginTime;
    }
    void AddBeginTime(int iBeginTime)
    {
        if (0 == iBeginTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBeginTime(iBeginTime);
        m_stData.m_iBeginTime += iBeginTime;
    }
    void SetBeginTime(int iBeginTime)
    {
        if (m_stData.m_iBeginTime == iBeginTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iBeginTime - m_stData.m_iBeginTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetBeginTime(RealAdd);
        m_stData.m_iBeginTime = iBeginTime;
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
    const int& GetVipLevel() const
    {
        return m_stData.m_iVipLevel;
    }
    void AddVipLevel(int iVipLevel)
    {
        if (0 == iVipLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetVipLevel(iVipLevel);
        m_stData.m_iVipLevel += iVipLevel;
    }
    void SetVipLevel(int iVipLevel)
    {
        if (m_stData.m_iVipLevel == iVipLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iVipLevel - m_stData.m_iVipLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetVipLevel(RealAdd);
        m_stData.m_iVipLevel = iVipLevel;
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
    const std::set<int>& GetChannelIDVec() const
    {
        return m_stData.m_stChannelIDVec;
    }
    void SetChannelIDVec(const std::set<int>& stChannelIDVec)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetChannelIDVec(stChannelIDVec);
        m_stData.m_stChannelIDVec = stChannelIDVec;
    }

    CRecordGlobalMailTable* m_pRecord;
    TGlobalMailTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGlobalMailTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGlobalMailTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iServerID, m_stData.m_iMailID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGlobalMailTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
