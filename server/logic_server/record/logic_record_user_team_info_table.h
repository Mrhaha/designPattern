#pragma once

#include "server_common_protocol.h"
#include "logic_record_sql_table.h"
#include "logic_record_sql_table_cache.h"

struct TUserTeamInfoTable;
struct TUserTeamInfoTableValueType;

class CRecordUserTeamInfoTable : public CLogicSQLTableRecordBase
{
public:
    CRecordUserTeamInfoTable() : m_pCache(nullptr) { }

    virtual ~CRecordUserTeamInfoTable();

    virtual std::string MakeInsertSQL();
    virtual std::string MakeUpdateSQL();
    virtual std::string MakeDeleteSQL();

    virtual int GetUid();
    virtual int GetGroupID();

    static std::vector<TUserTeamInfoTableValueType> SelectDataFromSQL(int iUin, int iGroupID);

    static const int TABLE_DIV;
    static const char* s_strTableName;
    static const char* s_strTeam1;
    static const char* s_strTeam2;
    static const char* s_strTeam3;
    static const char* s_strTeam4;
    static const char* s_strTeam5;
    static const char* s_strTeam6;
    static const char* s_strTeam7;
    static const char* s_strTeam8;
    static const char* s_strTeam9;
    static const char* s_strTeam10;
    static const char* s_strTeam11;
    static const char* s_strTeam12;
    static const char* s_strTeam13;
    static const char* s_strTeam14;
    static const char* s_strTeam15;
    static const char* s_strTeam16;
    static const char* s_strTeam17;
    static const char* s_strTeam18;
    static const char* s_strTeam19;
    static const char* s_strTeam20;
    static const char* s_strTeam21;
    static const char* s_strTeam22;
    static const char* s_strTeam23;
    static const char* s_strTeam24;
    static const char* s_strTeam25;
    static const char* s_strTeam26;
    static const char* s_strTeam27;
    static const char* s_strTeam28;
    static const char* s_strTeam29;
    static const char* s_strTeam30;
    static const char* s_strUid;
    static const char* s_strGroupID;

    TUserTeamInfoTable* m_pCache;

    void SetTeam1(const CPackCardTeam& stTeam1)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam1);
        m_stVarData[s_strTeam1] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam2(const CPackCardTeam& stTeam2)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam2);
        m_stVarData[s_strTeam2] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam3(const CPackCardTeam& stTeam3)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam3);
        m_stVarData[s_strTeam3] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam4(const CPackCardTeam& stTeam4)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam4);
        m_stVarData[s_strTeam4] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam5(const CPackCardTeam& stTeam5)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam5);
        m_stVarData[s_strTeam5] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam6(const CPackCardTeam& stTeam6)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam6);
        m_stVarData[s_strTeam6] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam7(const CPackCardTeam& stTeam7)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam7);
        m_stVarData[s_strTeam7] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam8(const CPackCardTeam& stTeam8)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam8);
        m_stVarData[s_strTeam8] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam9(const CPackCardTeam& stTeam9)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam9);
        m_stVarData[s_strTeam9] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam10(const CPackCardTeam& stTeam10)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam10);
        m_stVarData[s_strTeam10] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam11(const CPackCardTeam& stTeam11)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam11);
        m_stVarData[s_strTeam11] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam12(const CPackCardTeam& stTeam12)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam12);
        m_stVarData[s_strTeam12] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam13(const CPackCardTeam& stTeam13)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam13);
        m_stVarData[s_strTeam13] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam14(const CPackCardTeam& stTeam14)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam14);
        m_stVarData[s_strTeam14] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam15(const CPackCardTeam& stTeam15)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam15);
        m_stVarData[s_strTeam15] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam16(const CPackCardTeam& stTeam16)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam16);
        m_stVarData[s_strTeam16] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam17(const CPackCardTeam& stTeam17)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam17);
        m_stVarData[s_strTeam17] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam18(const CPackCardTeam& stTeam18)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam18);
        m_stVarData[s_strTeam18] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam19(const CPackCardTeam& stTeam19)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam19);
        m_stVarData[s_strTeam19] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam20(const CPackCardTeam& stTeam20)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam20);
        m_stVarData[s_strTeam20] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam21(const CPackCardTeam& stTeam21)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam21);
        m_stVarData[s_strTeam21] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam22(const CPackCardTeam& stTeam22)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam22);
        m_stVarData[s_strTeam22] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam23(const CPackCardTeam& stTeam23)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam23);
        m_stVarData[s_strTeam23] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam24(const CPackCardTeam& stTeam24)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam24);
        m_stVarData[s_strTeam24] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam25(const CPackCardTeam& stTeam25)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam25);
        m_stVarData[s_strTeam25] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam26(const CPackCardTeam& stTeam26)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam26);
        m_stVarData[s_strTeam26] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam27(const CPackCardTeam& stTeam27)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam27);
        m_stVarData[s_strTeam27] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam28(const CPackCardTeam& stTeam28)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam28);
        m_stVarData[s_strTeam28] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam29(const CPackCardTeam& stTeam29)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam29);
        m_stVarData[s_strTeam29] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetTeam30(const CPackCardTeam& stTeam30)
    {
        msgpack::sbuffer stBuffer;
        msgpack::pack(stBuffer, stTeam30);
        m_stVarData[s_strTeam30] = std::string(stBuffer.data(), stBuffer.size());
    }
    void SetPrimaryKey(int iUid, int iGroupID)
    {
        m_iUid = iUid;
        m_iGroupID = iGroupID;
    }
    void SetCachePtr(TUserTeamInfoTable* pCache)
    {
        m_pCache = pCache;
    }
private:
    int m_iUid;
    int m_iGroupID;
};

struct TUserTeamInfoTableValueType
{
    TUserTeamInfoTableValueType()
    : m_iUid(0)
    , m_iGroupID(0)
    , m_stTeam1()
    , m_stTeam2()
    , m_stTeam3()
    , m_stTeam4()
    , m_stTeam5()
    , m_stTeam6()
    , m_stTeam7()
    , m_stTeam8()
    , m_stTeam9()
    , m_stTeam10()
    , m_stTeam11()
    , m_stTeam12()
    , m_stTeam13()
    , m_stTeam14()
    , m_stTeam15()
    , m_stTeam16()
    , m_stTeam17()
    , m_stTeam18()
    , m_stTeam19()
    , m_stTeam20()
    , m_stTeam21()
    , m_stTeam22()
    , m_stTeam23()
    , m_stTeam24()
    , m_stTeam25()
    , m_stTeam26()
    , m_stTeam27()
    , m_stTeam28()
    , m_stTeam29()
    , m_stTeam30()
    {}

    int                     m_iUid;
    int                     m_iGroupID;
    CPackCardTeam           m_stTeam1;
    CPackCardTeam           m_stTeam2;
    CPackCardTeam           m_stTeam3;
    CPackCardTeam           m_stTeam4;
    CPackCardTeam           m_stTeam5;
    CPackCardTeam           m_stTeam6;
    CPackCardTeam           m_stTeam7;
    CPackCardTeam           m_stTeam8;
    CPackCardTeam           m_stTeam9;
    CPackCardTeam           m_stTeam10;
    CPackCardTeam           m_stTeam11;
    CPackCardTeam           m_stTeam12;
    CPackCardTeam           m_stTeam13;
    CPackCardTeam           m_stTeam14;
    CPackCardTeam           m_stTeam15;
    CPackCardTeam           m_stTeam16;
    CPackCardTeam           m_stTeam17;
    CPackCardTeam           m_stTeam18;
    CPackCardTeam           m_stTeam19;
    CPackCardTeam           m_stTeam20;
    CPackCardTeam           m_stTeam21;
    CPackCardTeam           m_stTeam22;
    CPackCardTeam           m_stTeam23;
    CPackCardTeam           m_stTeam24;
    CPackCardTeam           m_stTeam25;
    CPackCardTeam           m_stTeam26;
    CPackCardTeam           m_stTeam27;
    CPackCardTeam           m_stTeam28;
    CPackCardTeam           m_stTeam29;
    CPackCardTeam           m_stTeam30;
};

struct TUserTeamInfoTable
{
    TUserTeamInfoTable() : m_pRecord(nullptr) { }
    ~TUserTeamInfoTable()
    {
        if(nullptr != m_pRecord) m_pRecord->SetCachePtr(nullptr);
    }

    void InitializeWithoutSQL(const TUserTeamInfoTableValueType& stData)
    {
        m_stData = stData;
    }
    void Initialize(const TUserTeamInfoTableValueType& stData)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::INSERT);
        m_pRecord->SetPrimaryKey(stData.m_iUid, stData.m_iGroupID);
        m_pRecord->SetTeam1(stData.m_stTeam1);
        m_pRecord->SetTeam2(stData.m_stTeam2);
        m_pRecord->SetTeam3(stData.m_stTeam3);
        m_pRecord->SetTeam4(stData.m_stTeam4);
        m_pRecord->SetTeam5(stData.m_stTeam5);
        m_pRecord->SetTeam6(stData.m_stTeam6);
        m_pRecord->SetTeam7(stData.m_stTeam7);
        m_pRecord->SetTeam8(stData.m_stTeam8);
        m_pRecord->SetTeam9(stData.m_stTeam9);
        m_pRecord->SetTeam10(stData.m_stTeam10);
        m_pRecord->SetTeam11(stData.m_stTeam11);
        m_pRecord->SetTeam12(stData.m_stTeam12);
        m_pRecord->SetTeam13(stData.m_stTeam13);
        m_pRecord->SetTeam14(stData.m_stTeam14);
        m_pRecord->SetTeam15(stData.m_stTeam15);
        m_pRecord->SetTeam16(stData.m_stTeam16);
        m_pRecord->SetTeam17(stData.m_stTeam17);
        m_pRecord->SetTeam18(stData.m_stTeam18);
        m_pRecord->SetTeam19(stData.m_stTeam19);
        m_pRecord->SetTeam20(stData.m_stTeam20);
        m_pRecord->SetTeam21(stData.m_stTeam21);
        m_pRecord->SetTeam22(stData.m_stTeam22);
        m_pRecord->SetTeam23(stData.m_stTeam23);
        m_pRecord->SetTeam24(stData.m_stTeam24);
        m_pRecord->SetTeam25(stData.m_stTeam25);
        m_pRecord->SetTeam26(stData.m_stTeam26);
        m_pRecord->SetTeam27(stData.m_stTeam27);
        m_pRecord->SetTeam28(stData.m_stTeam28);
        m_pRecord->SetTeam29(stData.m_stTeam29);
        m_pRecord->SetTeam30(stData.m_stTeam30);
        m_stData = stData;
    }
    void ClearFromSQL()
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::DELETE);
    }
    const CPackCardTeam& GetTeam1() const
    {
        return m_stData.m_stTeam1;
    }
    void SetTeam1(const CPackCardTeam& stTeam1)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam1(stTeam1);
        m_stData.m_stTeam1 = stTeam1;
    }
    const CPackCardTeam& GetTeam2() const
    {
        return m_stData.m_stTeam2;
    }
    void SetTeam2(const CPackCardTeam& stTeam2)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam2(stTeam2);
        m_stData.m_stTeam2 = stTeam2;
    }
    const CPackCardTeam& GetTeam3() const
    {
        return m_stData.m_stTeam3;
    }
    void SetTeam3(const CPackCardTeam& stTeam3)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam3(stTeam3);
        m_stData.m_stTeam3 = stTeam3;
    }
    const CPackCardTeam& GetTeam4() const
    {
        return m_stData.m_stTeam4;
    }
    void SetTeam4(const CPackCardTeam& stTeam4)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam4(stTeam4);
        m_stData.m_stTeam4 = stTeam4;
    }
    const CPackCardTeam& GetTeam5() const
    {
        return m_stData.m_stTeam5;
    }
    void SetTeam5(const CPackCardTeam& stTeam5)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam5(stTeam5);
        m_stData.m_stTeam5 = stTeam5;
    }
    const CPackCardTeam& GetTeam6() const
    {
        return m_stData.m_stTeam6;
    }
    void SetTeam6(const CPackCardTeam& stTeam6)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam6(stTeam6);
        m_stData.m_stTeam6 = stTeam6;
    }
    const CPackCardTeam& GetTeam7() const
    {
        return m_stData.m_stTeam7;
    }
    void SetTeam7(const CPackCardTeam& stTeam7)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam7(stTeam7);
        m_stData.m_stTeam7 = stTeam7;
    }
    const CPackCardTeam& GetTeam8() const
    {
        return m_stData.m_stTeam8;
    }
    void SetTeam8(const CPackCardTeam& stTeam8)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam8(stTeam8);
        m_stData.m_stTeam8 = stTeam8;
    }
    const CPackCardTeam& GetTeam9() const
    {
        return m_stData.m_stTeam9;
    }
    void SetTeam9(const CPackCardTeam& stTeam9)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam9(stTeam9);
        m_stData.m_stTeam9 = stTeam9;
    }
    const CPackCardTeam& GetTeam10() const
    {
        return m_stData.m_stTeam10;
    }
    void SetTeam10(const CPackCardTeam& stTeam10)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam10(stTeam10);
        m_stData.m_stTeam10 = stTeam10;
    }
    const CPackCardTeam& GetTeam11() const
    {
        return m_stData.m_stTeam11;
    }
    void SetTeam11(const CPackCardTeam& stTeam11)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam11(stTeam11);
        m_stData.m_stTeam11 = stTeam11;
    }
    const CPackCardTeam& GetTeam12() const
    {
        return m_stData.m_stTeam12;
    }
    void SetTeam12(const CPackCardTeam& stTeam12)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam12(stTeam12);
        m_stData.m_stTeam12 = stTeam12;
    }
    const CPackCardTeam& GetTeam13() const
    {
        return m_stData.m_stTeam13;
    }
    void SetTeam13(const CPackCardTeam& stTeam13)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam13(stTeam13);
        m_stData.m_stTeam13 = stTeam13;
    }
    const CPackCardTeam& GetTeam14() const
    {
        return m_stData.m_stTeam14;
    }
    void SetTeam14(const CPackCardTeam& stTeam14)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam14(stTeam14);
        m_stData.m_stTeam14 = stTeam14;
    }
    const CPackCardTeam& GetTeam15() const
    {
        return m_stData.m_stTeam15;
    }
    void SetTeam15(const CPackCardTeam& stTeam15)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam15(stTeam15);
        m_stData.m_stTeam15 = stTeam15;
    }
    const CPackCardTeam& GetTeam16() const
    {
        return m_stData.m_stTeam16;
    }
    void SetTeam16(const CPackCardTeam& stTeam16)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam16(stTeam16);
        m_stData.m_stTeam16 = stTeam16;
    }
    const CPackCardTeam& GetTeam17() const
    {
        return m_stData.m_stTeam17;
    }
    void SetTeam17(const CPackCardTeam& stTeam17)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam17(stTeam17);
        m_stData.m_stTeam17 = stTeam17;
    }
    const CPackCardTeam& GetTeam18() const
    {
        return m_stData.m_stTeam18;
    }
    void SetTeam18(const CPackCardTeam& stTeam18)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam18(stTeam18);
        m_stData.m_stTeam18 = stTeam18;
    }
    const CPackCardTeam& GetTeam19() const
    {
        return m_stData.m_stTeam19;
    }
    void SetTeam19(const CPackCardTeam& stTeam19)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam19(stTeam19);
        m_stData.m_stTeam19 = stTeam19;
    }
    const CPackCardTeam& GetTeam20() const
    {
        return m_stData.m_stTeam20;
    }
    void SetTeam20(const CPackCardTeam& stTeam20)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam20(stTeam20);
        m_stData.m_stTeam20 = stTeam20;
    }
    const CPackCardTeam& GetTeam21() const
    {
        return m_stData.m_stTeam21;
    }
    void SetTeam21(const CPackCardTeam& stTeam21)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam21(stTeam21);
        m_stData.m_stTeam21 = stTeam21;
    }
    const CPackCardTeam& GetTeam22() const
    {
        return m_stData.m_stTeam22;
    }
    void SetTeam22(const CPackCardTeam& stTeam22)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam22(stTeam22);
        m_stData.m_stTeam22 = stTeam22;
    }
    const CPackCardTeam& GetTeam23() const
    {
        return m_stData.m_stTeam23;
    }
    void SetTeam23(const CPackCardTeam& stTeam23)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam23(stTeam23);
        m_stData.m_stTeam23 = stTeam23;
    }
    const CPackCardTeam& GetTeam24() const
    {
        return m_stData.m_stTeam24;
    }
    void SetTeam24(const CPackCardTeam& stTeam24)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam24(stTeam24);
        m_stData.m_stTeam24 = stTeam24;
    }
    const CPackCardTeam& GetTeam25() const
    {
        return m_stData.m_stTeam25;
    }
    void SetTeam25(const CPackCardTeam& stTeam25)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam25(stTeam25);
        m_stData.m_stTeam25 = stTeam25;
    }
    const CPackCardTeam& GetTeam26() const
    {
        return m_stData.m_stTeam26;
    }
    void SetTeam26(const CPackCardTeam& stTeam26)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam26(stTeam26);
        m_stData.m_stTeam26 = stTeam26;
    }
    const CPackCardTeam& GetTeam27() const
    {
        return m_stData.m_stTeam27;
    }
    void SetTeam27(const CPackCardTeam& stTeam27)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam27(stTeam27);
        m_stData.m_stTeam27 = stTeam27;
    }
    const CPackCardTeam& GetTeam28() const
    {
        return m_stData.m_stTeam28;
    }
    void SetTeam28(const CPackCardTeam& stTeam28)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam28(stTeam28);
        m_stData.m_stTeam28 = stTeam28;
    }
    const CPackCardTeam& GetTeam29() const
    {
        return m_stData.m_stTeam29;
    }
    void SetTeam29(const CPackCardTeam& stTeam29)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam29(stTeam29);
        m_stData.m_stTeam29 = stTeam29;
    }
    const CPackCardTeam& GetTeam30() const
    {
        return m_stData.m_stTeam30;
    }
    void SetTeam30(const CPackCardTeam& stTeam30)
    {
        if (m_pRecord == nullptr)
            m_pRecord = GetRecord();
        m_pRecord->SetOperation(CLogicSQLTableRecordBase::UPDATE);
        m_pRecord->SetTeam30(stTeam30);
        m_stData.m_stTeam30 = stTeam30;
    }

    CRecordUserTeamInfoTable* m_pRecord;
    TUserTeamInfoTableValueType m_stData;

private:
    bool HasRecord()
    {
        return m_pRecord != nullptr;
    }
    CRecordUserTeamInfoTable* GetRecord()
    {
        if(!m_pRecord)
        {
            m_pRecord = new CRecordUserTeamInfoTable;
            m_pRecord->SetPrimaryKey(m_stData.m_iUid, m_stData.m_iGroupID);
            m_pRecord->SetCachePtr(this);
            LOGIC_RECORD_CACHE_INS->m_stRecordVec.push_back(std::shared_ptr<CRecordUserTeamInfoTable>(m_pRecord));
        }
        return m_pRecord;
    }
};
