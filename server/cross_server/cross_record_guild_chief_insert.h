#pragma once

#include "cross_record_guild_member_table.h"

struct TGuildMemberTable;
struct TGuildMemberTableValueType;

class CRecordGuildChiefInsert : public CRecordGuildMemberTable
{
public:
    CRecordGuildChiefInsert() { }

    virtual std::string MakeInsertSQL();

    static std::vector<TGuildMemberTableValueType> SelectDataFromSQL(int iUin, int iGroupID) = delete;
    static std::vector<TGuildMemberTableValueType> SelectDataFromSQLWithGuildID(int iGuildID) = delete;
};
