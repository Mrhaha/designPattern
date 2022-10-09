//
// Created by hzl on 2019/4/25.
//

#pragma once


#include <cstdint>
#include <list>
#include <map>
#include <unordered_map>
#include <string>

class CAttackDefenseChallenge
{
private:
    struct TAttackDefenseChallenge
    {
        int32_t m_iGuildID;
        uint32_t m_iUin;
        int16_t m_nGroupID;
        std::string m_strNick;
        int32_t m_iDstGuildID;
        int32_t m_iDstUin;
        int16_t m_nDstGroupID;
        std::string m_strDstNick;
        int32_t m_iDstAreaType;
        int32_t m_iDstDifficult;
        int32_t m_iChallengeTime;
    };

public:
    using value_type = TAttackDefenseChallenge;
    using CChallengeList = std::list<value_type>;
    using CChallengeIter = CChallengeList::iterator;

public:
    bool IsPlayerChallenging(int32_t uin) const;

    bool IsPlayerBeChallenged(int32_t uin, int32_t difficult) const;

    void AddPlayerChallenge(const value_type& value);

    void RemoveChallengeByPlayer(int32_t uin);

    void ClearChallengeInfo();

    const value_type* GetChallengeByPlayer(int32_t uin) const;

    const value_type* GetChallengeByEarliest() const;

private:
    CChallengeList m_stChallengeList;
    std::unordered_map<int32_t, CChallengeIter> m_stPlayerMap;
    std::unordered_multimap<int32_t, CChallengeIter> m_stDstMap;
    std::multimap<int32_t, CChallengeIter> m_stTimeMap;
};



