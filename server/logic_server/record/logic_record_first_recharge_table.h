#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TFirstRechargeTable;
struct TFirstRechargeTableValueType;

class CRecordFirstRechargeTable : public CLogicSQLTableRecordBase
{
public:
    CRecordFirstRechargeTable() : m_pCache(nullptr) { }

    virtual ~CRecordFirstRechargeTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();

    static std::vector<TFirstRechargeTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const char* s_strTableName;
    static const char* s_strLevel;
    static const char* s_strPayCash;
    static const char* s_strPayType;
    static const char* s_strPayGamecoin;
    static const char* s_strPayItem;
    static const char* s_strStatDate;
    static const char* s_strCreateRoleTime;
    static const char* s_strLastLoginTime;
    static const char* s_strUid;
    static const char* s_strGroupID;

    TFirstRechargeTable* m_pCache;

    void SetLevel(int iLevel)
    {
        m_stIntData[s_strLevel] += iLevel;
    }
    void SetPayCash(const std::string& strPayCash)
    {
        m_stStringData[s_strPayCash] = strPayCash;
    }
    void SetPayType(int iPayType)
    {
        m_stIntData[s_strPayType] += iPayType;
    }
    void SetPayGamecoin(int iPayGamecoin)
    {
        m_stIntData[s_strPayGamecoin] += iPayGamecoin;
    }
    void SetPayItem(int iPayItem)
    {
        m_stIntData[s_strPayItem] += iPayItem;
    }
    void SetStatDate(const std::string& strStatDate)
    {
        m_stStringData[s_strStatDate] = strStatDate;
    }
    void SetCreateRoleTime(int iCreateRoleTime)
    {
        m_stIntData[s_strCreateRoleTime] += iCreateRoleTime;
    }
    void SetLastLoginTime(int iLastLoginTime)
    {
        m_stIntData[s_strLastLoginTime] += iLastLoginTime;
    }
    void SetPrimaryKey(int iUid, int iGroupID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TFirstRechargeTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
};

struct TFirstRechargeTableValueType
{
    TFirstRechargeTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_iLevel(0)
    , m_strPayCash("")
    , m_iPayType(0)
    , m_iPayGamecoin(0)
    , m_iPayItem(0)
    , m_strStatDate("")
    , m_iCreateRoleTime(0)
    , m_iLastLoginTime(0)
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    int                     m_iLevel;
    std::string             m_strPayCash;
    int                     m_iPayType;
    int                     m_iPayGamecoin;
    int                     m_iPayItem;
    std::string             m_strStatDate;
    int                     m_iCreateRoleTime;
    int                     m_iLastLoginTime;
};

struct TFirstRechargeTable
{
    TFirstRechargeTable() : m_pRecord(nullptr) { }
    ~TFirstRechargeTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TFirstRechargeTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TFirstRechargeTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID);
        m_pRecord->SetLevel(stData.m_iLevel);
        m_pRecord->SetPayCash(stData.m_strPayCash);
        m_pRecord->SetPayType(stData.m_iPayType);
        m_pRecord->SetPayGamecoin(stData.m_iPayGamecoin);
        m_pRecord->SetPayItem(stData.m_iPayItem);
        m_pRecord->SetStatDate(stData.m_strStatDate);
        m_pRecord->SetCreateRoleTime(stData.m_iCreateRoleTime);
        m_pRecord->SetLastLoginTime(stData.m_iLastLoginTime);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const int& GetLevel() const
    {
        return m_stData.m_iLevel;
    }
    void AddLevel(int iLevel)
    {
        if (0 == iLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLevel(iLevel);
        m_stData.m_iLevel += iLevel;
    }
    void SetLevel(int iLevel)
    {
        if (m_stData.m_iLevel == iLevel)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLevel - m_stData.m_iLevel;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLevel(RealAdd);
        m_stData.m_iLevel = iLevel;
    }
    const std::string& GetPayCash() const
    {
        return m_stData.m_strPayCash;
    }
    void SetPayCash(const std::string& strPayCash)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayCash(strPayCash);
        m_stData.m_strPayCash = strPayCash;
    }
    const int& GetPayType() const
    {
        return m_stData.m_iPayType;
    }
    void AddPayType(int iPayType)
    {
        if (0 == iPayType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayType(iPayType);
        m_stData.m_iPayType += iPayType;
    }
    void SetPayType(int iPayType)
    {
        if (m_stData.m_iPayType == iPayType)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPayType - m_stData.m_iPayType;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayType(RealAdd);
        m_stData.m_iPayType = iPayType;
    }
    const int& GetPayGamecoin() const
    {
        return m_stData.m_iPayGamecoin;
    }
    void AddPayGamecoin(int iPayGamecoin)
    {
        if (0 == iPayGamecoin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayGamecoin(iPayGamecoin);
        m_stData.m_iPayGamecoin += iPayGamecoin;
    }
    void SetPayGamecoin(int iPayGamecoin)
    {
        if (m_stData.m_iPayGamecoin == iPayGamecoin)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPayGamecoin - m_stData.m_iPayGamecoin;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayGamecoin(RealAdd);
        m_stData.m_iPayGamecoin = iPayGamecoin;
    }
    const int& GetPayItem() const
    {
        return m_stData.m_iPayItem;
    }
    void AddPayItem(int iPayItem)
    {
        if (0 == iPayItem)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayItem(iPayItem);
        m_stData.m_iPayItem += iPayItem;
    }
    void SetPayItem(int iPayItem)
    {
        if (m_stData.m_iPayItem == iPayItem)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iPayItem - m_stData.m_iPayItem;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetPayItem(RealAdd);
        m_stData.m_iPayItem = iPayItem;
    }
    const std::string& GetStatDate() const
    {
        return m_stData.m_strStatDate;
    }
    void SetStatDate(const std::string& strStatDate)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetStatDate(strStatDate);
        m_stData.m_strStatDate = strStatDate;
    }
    const int& GetCreateRoleTime() const
    {
        return m_stData.m_iCreateRoleTime;
    }
    void AddCreateRoleTime(int iCreateRoleTime)
    {
        if (0 == iCreateRoleTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCreateRoleTime(iCreateRoleTime);
        m_stData.m_iCreateRoleTime += iCreateRoleTime;
    }
    void SetCreateRoleTime(int iCreateRoleTime)
    {
        if (m_stData.m_iCreateRoleTime == iCreateRoleTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iCreateRoleTime - m_stData.m_iCreateRoleTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetCreateRoleTime(RealAdd);
        m_stData.m_iCreateRoleTime = iCreateRoleTime;
    }
    const int& GetLastLoginTime() const
    {
        return m_stData.m_iLastLoginTime;
    }
    void AddLastLoginTime(int iLastLoginTime)
    {
        if (0 == iLastLoginTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastLoginTime(iLastLoginTime);
        m_stData.m_iLastLoginTime += iLastLoginTime;
    }
    void SetLastLoginTime(int iLastLoginTime)
    {
        if (m_stData.m_iLastLoginTime == iLastLoginTime)
            return;
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        int RealAdd = iLastLoginTime - m_stData.m_iLastLoginTime;
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetLastLoginTime(RealAdd);
        m_stData.m_iLastLoginTime = iLastLoginTime;
    }

    CRecordFirstRechargeTable* m_pRecord;
    TFirstRechargeTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordFirstRechargeTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordFirstRechargeTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordFirstRechargeTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
