//
// Created by hzl on 2019/4/24.
//

#pragma once
#include <cstdint>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include "server_cross_protocol.h"

class CMatchPlayer
{
private:
    using CMatchPlayerList = std::list<CPackMatchPlayer>;

    using CMatchPlayerIter = CMatchPlayerList::iterator;

public:
    const CMatchPlayerList& GetMatchPlayerList() const;

    void AddPlayer(int32_t uin, int16_t group_id, int32_t fight_power);

    void RemovePlayerByRole(int32_t uin);

    const CPackMatchPlayer* GetPlayerByFightPower(int32_t near_power);

    void RandomReducePlayer();

private:
    CMatchPlayerList m_stPlayerList;

    std::unordered_map<int32_t, CMatchPlayerIter> m_stRoleMap;

    std::multimap<int32_t, CMatchPlayerIter> m_stFightPowerMap;
};


