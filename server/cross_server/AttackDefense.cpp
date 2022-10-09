//
// Created by hzl on 2019/4/25.
//

#include <random>
#include <server_cmd_cross_define.h>
#include "AttackDefense.h"
#include "ServerConfig.h"

CAttackDefense::CAttackDefense(CProcessor* processor) : pstProcessor(processor), m_bFighting(false)
{

}

bool CAttackDefense::IsFighting() const
{
    return m_bFighting;
}

void CAttackDefense::ToBegin()
{
    if (!m_bFighting)
    {
        if (m_stAttackDefenseList.size() % 2 && m_stAttackDefenseTiBu.m_iGuildID > 0)
        {
            AddAttackDefenseGuild(m_stAttackDefenseTiBu, false);
            m_stAttackDefenseTiBu = {};
        }

        if (m_stAttackDefenseList.size() % 2)
        {
            RemoveAttackDefenseGuild(m_stGuildExpMap.begin()->second->m_iGuildID);
        }

        std::random_device rd;
        std::mt19937 gen(rd());


        for (auto self = m_stOpenDayMap.begin(); self != m_stOpenDayMap.end(); ++self)
        {
            pstProcessor->Log(log_mask_system_error, "++++[CAttackDefense:%s]server(%d) guild id %d exp :%d  score:%d start to match ++++++++++++++++++++++++++++++\n"
                    , __FUNCTION__, self->second->m_iServerID, self->second->m_iGuildID, self->second->m_iGuildExp, self->second->m_iAtkDefScore);


            if (self->second->m_iDstGuildID > 0)
            {
                pstProcessor->Log(log_mask_system_error, "====[CMatchProcessor:%s]server(%d) guild id %d exp :%d  score:%d had match to %d, end match =================================\n"
                        , __FUNCTION__, self->second->m_iServerID, self->second->m_iGuildID, self->second->m_iGuildExp, self->second->m_iAtkDefScore, self->second->m_iDstGuildID);

                continue;
            }

            const int32_t iDistanceServer = 3 + (self->second->m_iOpenDay / 40) * 2;
            std::multimap<int32_t, decltype(self)> match;
            for (auto dst = std::next(self); dst != m_stOpenDayMap.end(); ++dst)
            {
                if (dst->second->m_iDstGuildID > 0)
                {
                    pstProcessor->Log(log_mask_system_error, "[CMatchProcessor:%s]server(%d) guild id %d found match to %d, but it had matched, pass \n"
                            , __FUNCTION__, self->second->m_iServerID, self->second->m_iGuildID, dst->second->m_iGuildID);
                    continue;
                }

                const int32_t iDayDiff = abs(self->second->m_iServerID - dst->second->m_iServerID);
                if (iDayDiff > iDistanceServer && match.size() > 5)
                {
                    break;
                }

                if (self->second->m_stAtkDefMatchHistory.find(dst->second->m_iGuildID) != self->second->m_stAtkDefMatchHistory.end())
                {
                    pstProcessor->Log(log_mask_system_error, "[CMatchProcessor:%s]server(%d) guild id %d found server-match %d-%d, score:%d, but had matched, add to match list \n"
                            , __FUNCTION__, self->second->m_iServerID, self->second->m_iGuildID, dst->second->m_iServerID, dst->second->m_iGuildID, dst->second->m_iAtkDefScore);
                    match.emplace(1, dst);
                }
                else
                {
                    pstProcessor->Log(log_mask_system_error, "[CMatchProcessor:%s]server(%d) guild id %d found server-match %d-%d, score:%d, add to match list \n"
                            , __FUNCTION__, self->second->m_iServerID, self->second->m_iGuildID, dst->second->m_iServerID, dst->second->m_iGuildID, dst->second->m_iAtkDefScore);

                    match.emplace(dst->second->m_iAtkDefScore, dst);
                }
            }

            bool bMatch = false;
            for (double dMultiple = 0.05; !bMatch; dMultiple += 0.05)
            {
                const auto lower = self->second->m_iAtkDefScore * (1.0 - dMultiple);
                const auto upper = self->second->m_iAtkDefScore * (1.0 + dMultiple);
                if(lower > upper)
                {
                    pstProcessor->Log(log_mask_system_error, "====[CMatchProcessor:%s]server(%d) guild id %d exp :%d score:%d need multiple %lf, lower > upper, end match =================================\n"
                            , __FUNCTION__, self->second->m_iServerID, self->second->m_iGuildID, self->second->m_iGuildExp, self->second->m_iAtkDefScore, dMultiple);
                    break;
                }

                const auto iterBegin = match.lower_bound(lower);
                const auto iterEnd = match.upper_bound(upper);
                if (iterBegin != iterEnd)
                {
                    const int32_t iCount = std::distance(iterBegin, iterEnd);

                    auto dst = std::next(iterBegin, gen() % iCount)->second;

                    self->second->m_iDstGuildID = dst->second->m_iGuildID;
                    dst->second->m_iDstGuildID = self->second->m_iGuildID;

                    pstProcessor->Log(log_mask_system_error, "====[CMatchProcessor:%s]server(%d) guild id %d exp :%d  score:%d  need multiple %lf match to %d, score:%d, end match =================================\n"
                            , __FUNCTION__, self->second->m_iServerID, self->second->m_iGuildID, self->second->m_iGuildExp, self->second->m_iAtkDefScore, dMultiple, self->second->m_iDstGuildID, dst->second->m_iAtkDefScore);

                    bMatch = true;
                }
            }

        }

        std::map<int32_t, std::map<int32_t, int32_t>> stServerGuildMap;
        for (const auto& info : m_stAttackDefenseList)
        {
            pstProcessor->Log(log_mask_system_error, "[CMatchProcessor:%s]server %d guild(%d) exp(%d) match guild(%d) success\n"
                    , __FUNCTION__, info.m_iServerID, info.m_iGuildID, info.m_iGuildExp, info.m_iDstGuildID);

            stServerGuildMap[info.m_iServerID].emplace(info.m_iGuildID, info.m_iDstGuildID);
        }

        for (const auto& guild : stServerGuildMap)
        {
            CSNotifyAttackDefenseStart notify;
            notify.m_iGuildMatched = guild.second;
            pstProcessor->NotifyMessage(guild.first, 0, MSG_CROSS_ATTACK_DEFENSE_START, notify);
        }

        m_bFighting = true;
    }
}

void CAttackDefense::DumpToGuildWar(CPackGuildWar& data) const
{
    data.m_bAttackDefenseFighting = m_bFighting;
    data.m_stAttackDefenseList = m_stAttackDefenseList;
    data.m_stAttackDefenseTiBu = m_stAttackDefenseTiBu;
}

void CAttackDefense::RecoveryFromGuildWar(const CPackGuildWar& data)
{
    m_bFighting = data.m_bAttackDefenseFighting;

    for (const auto& info : data.m_stAttackDefenseList)
    {
        AddAttackDefenseGuild(info, false);
    }
    AddAttackDefenseGuild(data.m_stAttackDefenseTiBu, true);
}

const CAttackDefense::value_list & CAttackDefense::GetAttackDefenseList() const
{
    return m_stAttackDefenseList;
}

const CAttackDefense::value_type * CAttackDefense::GetAttackDefenseByGuild(int32_t guild_id) const
{
    const auto iter = m_stGuildMap.find(guild_id);
    return iter != m_stGuildMap.end() ? &(*iter->second) : nullptr;
}

void CAttackDefense::AddAttackDefenseGuild(const value_type& value, bool tibu)
{
    RemoveAttackDefenseGuild(value.m_iGuildID);

    if (!tibu)
    {
        m_stAttackDefenseList.push_front(value);
        m_stGuildMap.emplace(m_stAttackDefenseList.begin()->m_iGuildID, m_stAttackDefenseList.begin());
        m_stOpenDayMap.emplace(m_stAttackDefenseList.begin()->m_iOpenDay, m_stAttackDefenseList.begin());
        m_stGuildExpMap.emplace(m_stAttackDefenseList.begin()->m_iGuildExp, m_stAttackDefenseList.begin());
    }
    else if (value.m_iGuildExp >= m_stAttackDefenseTiBu.m_iGuildExp)
    {
        m_stAttackDefenseTiBu = value;
    }
}

void CAttackDefense::RemoveAttackDefenseGuild(int32_t guild_id)
{
    if (auto iterGuild = m_stGuildMap.find(guild_id); iterGuild != m_stGuildMap.end())
    {
        auto iter = iterGuild->second;

        m_stGuildMap.erase(iterGuild);

        const auto range_open = m_stOpenDayMap.equal_range(iter->m_iOpenDay);
        for (auto iterOpen = range_open.first; iterOpen != range_open.second; ++iterOpen)
        {
            if (iterOpen->second == iter)
            {
                m_stOpenDayMap.erase(iterOpen);
                break;
            }
        }

        const auto range_exp = m_stGuildExpMap.equal_range(iter->m_iGuildExp);
        for (auto iterExp = range_exp.first; iterExp != range_exp.second; ++iterExp)
        {
            if (iterExp->second == iter)
            {
                m_stGuildExpMap.erase(iterExp);
                break;
            }
        }

        m_stAttackDefenseList.erase(iter);
    }
}

void CAttackDefense::AttackDefenseAreaRepaired(int32_t guild_id, int32_t uin, int32_t index)
{
    if (auto iter = m_stGuildMap.find(guild_id); iter != m_stGuildMap.end())
    {
        if (auto iterArea = iter->second->m_stAreaMap.find(uin); iterArea != iter->second->m_stAreaMap.end())
        {
            if (index >= 1 && index <= 30 && index <= int32_t(iterArea->second.m_strFightNameVec.size()))
            {
                const uint32_t flag = 1 << index;
                if ((flag & iterArea->second.m_uiFightBitmap))
                {
                    iterArea->second.m_strFightNameVec[index - 1] = "";
                    iterArea->second.m_uiFightBitmap -= flag;
                    -- iter->second->m_iLoseAreaNum;
                }
            }
        }
    }
}

void CAttackDefense::AttackDefenseAreaOccupied(int32_t guild_id, int32_t uin, int32_t index, const std::string& name)
{
    if (auto iter = m_stGuildMap.find(guild_id); iter != m_stGuildMap.end())
    {
        if (auto iterArea = iter->second->m_stAreaMap.find(uin); iterArea != iter->second->m_stAreaMap.end())
        {
            if (index >= 1 && index <= 30 && index <= int32_t(iterArea->second.m_strFightNameVec.size()))
            {
                const uint32_t flag = 1 << index;
                if ((flag & iterArea->second.m_uiFightBitmap) == 0)
                {
                    iterArea->second.m_strFightNameVec[index - 1] = name;
                    iterArea->second.m_uiFightBitmap += flag;
                    ++ iter->second->m_iLoseAreaNum;
                }
            }
        }
    }
}




