//
// Created by hzl on 2019/4/24.
//

#include <random>
#include "MatchPlayer.h"

const CMatchPlayer::CMatchPlayerList& CMatchPlayer::GetMatchPlayerList() const
{
    return m_stPlayerList;
}

void CMatchPlayer::AddPlayer(int32_t uin, int16_t group_id, int32_t fight_power)
{
    if (auto RoleIter = m_stRoleMap.find(uin); RoleIter == m_stRoleMap.end())
    {
        CPackMatchPlayer player = { uin, group_id, fight_power };
        m_stPlayerList.push_front(player);
        m_stRoleMap.emplace(uin, m_stPlayerList.begin());
        m_stFightPowerMap.emplace(fight_power, m_stPlayerList.begin());
    }

    RandomReducePlayer();
}

void CMatchPlayer::RemovePlayerByRole(int32_t uin)
{
    if (auto RoleIter = m_stRoleMap.find(uin); RoleIter != m_stRoleMap.end())
    {
        auto FightPowerRange = m_stFightPowerMap.equal_range(RoleIter->second->m_iFightPower);
        for (auto FightPowerIter = FightPowerRange.first; FightPowerIter != FightPowerRange.second; ++FightPowerIter)
        {
            if (FightPowerIter->second == RoleIter->second)
            {
                m_stFightPowerMap.erase(FightPowerIter);
                break;
            }
        }
        m_stPlayerList.erase(RoleIter->second);
        m_stRoleMap.erase(RoleIter);
    }
}

const CPackMatchPlayer* CMatchPlayer::GetPlayerByFightPower(int32_t near_power)
{
    const auto FightPowerIter = m_stFightPowerMap.lower_bound(near_power);
    if (FightPowerIter != m_stFightPowerMap.end() && FightPowerIter->first > 0)
    {
        return &(*FightPowerIter->second);
    }
    return nullptr;
}

void CMatchPlayer::RandomReducePlayer()
{
    if (m_stPlayerList.size() > 20000)
    {
        const auto min_power = m_stFightPowerMap.begin()->first;
        const auto max_power = m_stFightPowerMap.rbegin()->first;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> power_dis(min_power, max_power);

        for (auto count = 0 ; count < 500; ++count)
        {
            const int32_t power = power_dis(gen);
            const auto iterPower = m_stFightPowerMap.lower_bound(power);
            if (iterPower != m_stFightPowerMap.end())
            {
                RemovePlayerByRole(iterPower->second->m_iUin);
            }
        }
    }
}
