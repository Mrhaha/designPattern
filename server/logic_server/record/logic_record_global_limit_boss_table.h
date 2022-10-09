#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TGlobalLimitBossTable;
struct TGlobalLimitBossTableValueType;

class CRecordGlobalLimitBossTable : public CLogicSQLTableRecordBase
{
public:
    CRecordGlobalLimitBossTable() : m_pCache(nullptr) { }

    virtual ~CRecordGlobalLimitBossTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetServerID();
    virtual int GetOwnerUin();
    virtual int GetUid() { return 0; }
    virtual int GetGroupID() { return 0; }

    static std::vector<TGlobalLimitBossTableValueType> SelectDataFromSQL(int iServerID);

    static const char* s_strTableName;
    static const char* s_strOwnerGroup;
    static const char* s_strNick;
    static const char* s_strHard;
    static const char* s_strIsOpen;
    static const char* s_strEndTime;
    static const char* s_strOpenTime;
    static const char* s_strDeathTime;
    static const char* s_strKillerUin;
    static const char* s_strKillerGroup;
    static const char* s_strMaxHp;
    static const char* s_strCurHurt;
    static const char* s_strHurtLog;
    static const char* s_strAttackerList;
    static const char* s_strServerID;
    static const char* s_strOwnerUin;

    TGlobalLimitBossTable* m_pCache;

    void SetOwnerGroup(int iOwnerGroup)
    {
        m_stIntData[s_strOwnerGroup] += iOwnerGroup;
    }
    void SetNick(const std::string& strNick)
    {
        m_stStringData[s_strNick] = strNick;
    }
    void SetHard(char cHard)
    {
        m_stCharData[s_strHard] += cHard;
    }
    void SetIsOpen(char cIsOpen)
    {
        m_stCharData[s_strIsOpen] += cIsOpen;
    }
    void SetEndTime(int iEndTime)
    {
        m_stIntData[s_strEndTime] += iEndTime;
    }
    void SetOpenTime(int iOpenTime)
    {
        m_stIntData[s_strOpenTime] += iOpenTime;
    }
    void SetDeathTime(int iDeathTime)
    {
        m_stIntData[s_strDeathTime] += iDeathTime;
    }
    void SetKillerUin(int iKillerUin)
    {
        m_stIntData[s_strKillerUin] += iKillerUin;
    }
    void SetKillerGroup(int iKillerGroup)
    {
        m_stIntData[s_strKillerGroup] += iKillerGroup;
    }
    void SetMaxHp(uint64_t ulMaxHp, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strMaxHp];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulMaxHp;
        }
        else
        {
            if (stDataRef.m_ulVal > ulMaxHp)
            {
                stDataRef.m_ulVal -= ulMaxHp;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulMaxHp - stDataRef.m_ulVal;
            }
        }
    }
    void SetCurHurt(uint64_t ulCurHurt, char cOperator = '+')
    {
        auto& stDataRef = m_stUInt64Data[s_strCurHurt];
        if (stDataRef.m_cOperator == cOperator)
        {
            stDataRef.m_ulVal += ulCurHurt;
        }
        else
        {
            if (stDataRef.m_ulVal > ulCurHurt)
            {
                stDataRef.m_ulVal -= ulCurHurt;
            }
            else
            {
                stDataRef.m_cOperator = cOperator;
                stDataRef.m_ulVal = ulCurHurt - stDataRef.m_ulVal;
            }
        }
    }
    void SetHurtLog(const std::map<int32_t, std::pair<uint32_t, uint64_t>>& stHurtLog)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stHurtLog);
        m_stVarData[s_strHurtLog] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetAttackerList(const std::map<int32_t, int32_t>& stAttackerList)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stAttackerList);
        m_stVarData[s_strAttackerList] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iServerID, int iOwnerUin)
    {
        m_iServerID = iServerID;
        m_iOwnerUin = iOwnerUin;
    }
    void SetCachePtr(TGlobalLimitBossTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iServerID;
    int m_iOwnerUin;
};

struct TGlobalLimitBossTableValueType
{
    TGlobalLimitBossTableValueType()
    : m_iServerID(0)
    , m_iOwnerUin(0)
    , m_iOwnerGroup(0)
    , m_strNick("")
    , m_cHard(0)
    , m_cIsOpen(0)
    , m_iEndTime(0)
    , m_iOpenTime(0)
    , m_iDeathTime(0)
    , m_iKillerUin(0)
    , m_iKillerGroup(0)
    , m_ulMaxHp(0)
    , m_ulCurHurt(0)
    , m_stHurtLog()
    , m_stAttackerList()
    {}

    int                     m_iServerID;
    int                     m_iOwnerUin;
    int                     m_iOwnerGroup;
    std::string             m_strNick;
    char                    m_cHard;
    char                    m_cIsOpen;
    int                     m_iEndTime;
    int                     m_iOpenTime;
    int                     m_iDeathTime;
    int                     m_iKillerUin;
    int                     m_iKillerGroup;
    uint64_t                m_ulMaxHp;
    uint64_t                m_ulCurHurt;
    std::map<int32_t, std::pair<uint32_t, uint64_t>>    m_stHurtLog;
    std::map<int32_t, int32_t>    m_stAttackerList;
};

struct TGlobalLimitBossTable
{
    TGlobalLimitBossTable() : m_pRecord(nullptr) { }
    ~TGlobalLimitBossTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TGlobalLimitBossTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TGlobalLimitBossTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iServerID, stData.m_iOwnerUin);
        m_pRecord->SetOwnerGroup(stData.m_iOwnerGroup);
        m_pRecord->SetNick(stData.m_strNick);
        m_pRecord->SetHard(stData.m_cHard);
        m_pRecord->SetIsOpen(stData.m_cIsOpen);
        m_pRecord->SetEndTime(stData.m_iEndTime);
        m_pRecord->SetOpenTime(stData.m_iOpenTime);
        m_pRecord->SetDeathTime(stData.m_iDeathTime);
        m_pRecord->SetKillerUin(stData.m_iKillerUin);
        m_pRecord->SetKillerGroup(stData.m_iKillerGroup);
        m_pRecord->SetMaxHp(stData.m_ulMaxHp);
        m_pRecord->SetCurHurt(stData.m_ulCurHurt);
        m_pRecord->SetHurtLog(stData.m_stHurtLog);
        m_pRecord->SetAttackerList(stData.m_stAttackerList);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetOwnerGroup() const
    {
        return m_stData.m_iOwnerGroup;
    }
    void AddOwnerGroup(int iOwnerGroup)
    {
        if (0 == iOwnerGroup)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetOwnerGroup(iOwnerGroup);
        m_stData.m_iOwnerGroup += iOwnerGroup;
    }
    void SetOwnerGroup(int iOwnerGroup)
    {
        if (m_stData.m_iOwnerGroup == iOwnerGroup)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iOwnerGroup - m_stData.m_iOwnerGroup;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetOwnerGroup(RealAdd);
        m_stData.m_iOwnerGroup = iOwnerGroup;
    }
    const std::string& GetNick() const
    {
        return m_stData.m_strNick;
    }
    void SetNick(const std::string& strNick)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetNick(strNick);
        m_stData.m_strNick = strNick;
    }
    const char& GetHard() const
    {
        return m_stData.m_cHard;
    }
    void AddHard(char cHard)
    {
        if (0 == cHard)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHard(cHard);
        m_stData.m_cHard += cHard;
    }
    void SetHard(char cHard)
    {
        if (m_stData.m_cHard == cHard)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cHard - m_stData.m_cHard;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHard(RealAdd);
        m_stData.m_cHard = cHard;
    }
    const char& GetIsOpen() const
    {
        return m_stData.m_cIsOpen;
    }
    void AddIsOpen(char cIsOpen)
    {
        if (0 == cIsOpen)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsOpen(cIsOpen);
        m_stData.m_cIsOpen += cIsOpen;
    }
    void SetIsOpen(char cIsOpen)
    {
        if (m_stData.m_cIsOpen == cIsOpen)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        char RealAdd = cIsOpen - m_stData.m_cIsOpen;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetIsOpen(RealAdd);
        m_stData.m_cIsOpen = cIsOpen;
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
    const int& GetDeathTime() const
    {
        return m_stData.m_iDeathTime;
    }
    void AddDeathTime(int iDeathTime)
    {
        if (0 == iDeathTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDeathTime(iDeathTime);
        m_stData.m_iDeathTime += iDeathTime;
    }
    void SetDeathTime(int iDeathTime)
    {
        if (m_stData.m_iDeathTime == iDeathTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iDeathTime - m_stData.m_iDeathTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetDeathTime(RealAdd);
        m_stData.m_iDeathTime = iDeathTime;
    }
    const int& GetKillerUin() const
    {
        return m_stData.m_iKillerUin;
    }
    void AddKillerUin(int iKillerUin)
    {
        if (0 == iKillerUin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetKillerUin(iKillerUin);
        m_stData.m_iKillerUin += iKillerUin;
    }
    void SetKillerUin(int iKillerUin)
    {
        if (m_stData.m_iKillerUin == iKillerUin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iKillerUin - m_stData.m_iKillerUin;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetKillerUin(RealAdd);
        m_stData.m_iKillerUin = iKillerUin;
    }
    const int& GetKillerGroup() const
    {
        return m_stData.m_iKillerGroup;
    }
    void AddKillerGroup(int iKillerGroup)
    {
        if (0 == iKillerGroup)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetKillerGroup(iKillerGroup);
        m_stData.m_iKillerGroup += iKillerGroup;
    }
    void SetKillerGroup(int iKillerGroup)
    {
        if (m_stData.m_iKillerGroup == iKillerGroup)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iKillerGroup - m_stData.m_iKillerGroup;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetKillerGroup(RealAdd);
        m_stData.m_iKillerGroup = iKillerGroup;
    }
    const uint64_t& GetMaxHp() const
    {
        return m_stData.m_ulMaxHp;
    }
    void AddMaxHp(uint64_t ulMaxHp)
    {
        if (0 == ulMaxHp)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetMaxHp(ulMaxHp);
        m_stData.m_ulMaxHp += ulMaxHp;
    }
    void SetMaxHp(uint64_t ulMaxHp)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulMaxHp > ulMaxHp)
        {
            m_pRecord->SetMaxHp(m_stData.m_ulMaxHp - ulMaxHp, '-');
        }
        else
        {
            m_pRecord->SetMaxHp(ulMaxHp - m_stData.m_ulMaxHp, '+');
        }
        m_stData.m_ulMaxHp = ulMaxHp;
    }
    const uint64_t& GetCurHurt() const
    {
        return m_stData.m_ulCurHurt;
    }
    void AddCurHurt(uint64_t ulCurHurt)
    {
        if (0 == ulCurHurt)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCurHurt(ulCurHurt);
        m_stData.m_ulCurHurt += ulCurHurt;
    }
    void SetCurHurt(uint64_t ulCurHurt)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        if (m_stData.m_ulCurHurt > ulCurHurt)
        {
            m_pRecord->SetCurHurt(m_stData.m_ulCurHurt - ulCurHurt, '-');
        }
        else
        {
            m_pRecord->SetCurHurt(ulCurHurt - m_stData.m_ulCurHurt, '+');
        }
        m_stData.m_ulCurHurt = ulCurHurt;
    }
    const std::map<int32_t, std::pair<uint32_t, uint64_t>>& GetHurtLog() const
    {
        return m_stData.m_stHurtLog;
    }
    void SetHurtLog(const std::map<int32_t, std::pair<uint32_t, uint64_t>>& stHurtLog)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetHurtLog(stHurtLog);
        m_stData.m_stHurtLog = stHurtLog;
    }
    const std::map<int32_t, int32_t>& GetAttackerList() const
    {
        return m_stData.m_stAttackerList;
    }
    void SetAttackerList(const std::map<int32_t, int32_t>& stAttackerList)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetAttackerList(stAttackerList);
        m_stData.m_stAttackerList = stAttackerList;
    }

    CRecordGlobalLimitBossTable* m_pRecord;
    TGlobalLimitBossTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordGlobalLimitBossTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordGlobalLimitBossTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iServerID, m_stData.m_iOwnerUin);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordGlobalLimitBossTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
