//
// Created by hzl on 2019/4/25.
//

#include "AttackDefenseChallenge.h"

bool CAttackDefenseChallenge::IsPlayerChallenging(int32_t uin) const
{
    return m_stPlayerMap.find(uin) != m_stPlayerMap.end();
}

bool CAttackDefenseChallenge::IsPlayerBeChallenged(int32_t uin, int32_t difficult) const
{
    const auto range = m_stDstMap.equal_range(uin);
    for (auto iter = range.first; iter != range.second; ++iter)
    {
        if (iter->second->m_iDstDifficult == difficult)
        {
            return true;
        }
    }
    return false;
}

void CAttackDefenseChallenge::AddPlayerChallenge(const value_type& value)
{
    if (!IsPlayerChallenging(value.m_iUin) && !IsPlayerBeChallenged(value.m_iDstUin, value.m_iDstDifficult))
    {
        m_stChallengeList.push_front(value);

        m_stPlayerMap.emplace(value.m_iUin, m_stChallengeList.begin());

        m_stDstMap.emplace(value.m_iDstUin, m_stChallengeList.begin());

        m_stTimeMap.emplace(value.m_iChallengeTime, m_stChallengeList.begin());
    }
}

void CAttackDefenseChallenge::RemoveChallengeByPlayer(int32_t uin)
{
    if (auto iter = m_stPlayerMap.find(uin); iter != m_stPlayerMap.end())
    {
        auto iterList = iter->second;

        m_stPlayerMap.erase(iter);

        const auto range_dst = m_stDstMap.equal_range(iterList->m_iDstUin);
        for (auto iterDst = range_dst.first; iterDst != range_dst.second; ++iterDst)
        {
            if (iterList == iterDst->second)
            {
                m_stDstMap.erase(iterDst);
                break;
            }
        }

        const auto range_time = m_stTimeMap.equal_range(iterList->m_iChallengeTime);
        for (auto iterTime = range_time.first; iterTime != range_time.second; ++iterTime)
        {
            if (iterList == iterTime->second)
            {
                m_stTimeMap.erase(iterTime);
                break;
            }
        }

        m_stChallengeList.erase(iterList);
    }
}

void CAttackDefenseChallenge::ClearChallengeInfo()
{
    m_stChallengeList.clear();
    m_stPlayerMap.clear();
    m_stDstMap.clear();
    m_stTimeMap.clear();
}

const CAttackDefenseChallenge::value_type* CAttackDefenseChallenge::GetChallengeByPlayer(int32_t uin) const
{
    const auto iter = m_stPlayerMap.find(uin);
    return iter != m_stPlayerMap.end() ? &(*iter->second) : nullptr;
}

const CAttackDefenseChallenge::value_type* CAttackDefenseChallenge::GetChallengeByEarliest() const
{
    return m_stTimeMap.empty() ? nullptr : &(*(m_stTimeMap.begin()->second));
}


