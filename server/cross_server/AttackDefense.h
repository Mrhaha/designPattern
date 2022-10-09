//
// Created by hzl on 2019/4/25.
//

#pragma once

#include "server_cross_protocol.h"
#include "Processor.h"

class CAttackDefense
{
public:
    using value_type = CPackAttackDefense;
    using value_list = std::list<value_type>;
    using iterator = value_list ::iterator;

public:
    CAttackDefense(CProcessor* processor);

    void ToBegin();

    bool IsFighting() const;

    void DumpToGuildWar(CPackGuildWar& data) const;

    void RecoveryFromGuildWar(const CPackGuildWar& data);

    const value_list & GetAttackDefenseList() const;

    const value_type * GetAttackDefenseByGuild(int32_t guild_id) const;

    void AddAttackDefenseGuild(const value_type& value, bool tibu);

    void RemoveAttackDefenseGuild(int32_t guild_id);

    void AttackDefenseAreaRepaired(int32_t guild_id, int32_t uin, int32_t index);

    void AttackDefenseAreaOccupied(int32_t guild_id, int32_t uin, int32_t index, const std::string& name);


private:
    CProcessor* pstProcessor;
    bool m_bFighting;               //公会攻防是否正在战斗中
    value_list m_stAttackDefenseList;
    value_type m_stAttackDefenseTiBu;   //攻防替补，当匹配中的队伍是奇数时，使用替补上阵

    std::unordered_map<int32_t, iterator > m_stGuildMap;
    std::multimap<int32_t, iterator > m_stOpenDayMap;
    std::multimap<int32_t, iterator > m_stGuildExpMap;
};



