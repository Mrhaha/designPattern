//
// Created by hzl on 2019/4/25.
//

#include "FairPlayer.h"

const CFairPlayer::CFairPlayerVector& CFairPlayer::GetFairPlayerVector() const
{
    return m_stPlayerVector;
}

void CFairPlayer::UpdateFairPlayer(int32_t uin, int16_t group_id, const std::vector<int32_t>& cards)
{
    const auto iter = m_stRoleMap.find(uin);
    if (iter == m_stRoleMap.end())
    {
        CPackFairPlayer player = { uin, group_id, cards};
        m_stRoleMap[uin] = m_stPlayerVector.size();
        m_stPlayerVector.push_back(std::move(player));
    }
    else
    {
        m_stPlayerVector[iter->second].m_iCardVec = cards;
    }

    ReduceFairPlayer();
}

void CFairPlayer::ReduceFairPlayer()
{
    if (m_stPlayerVector.size() >= 20000)
    {
        m_stPlayerVector.erase(m_stPlayerVector.begin(), m_stPlayerVector.begin() + 2000);

        m_stRoleMap.clear();

        for (size_t i = 0; i < m_stPlayerVector.size(); ++i)
        {
            m_stRoleMap[m_stPlayerVector[i].m_iUin] = i;
        }
    }
}


