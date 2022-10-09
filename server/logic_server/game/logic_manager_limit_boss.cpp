//
// Created by luoxiaohu on 2021/4/12.
//

#include "logic_config.h"
#include "logic_link_common.h"
#include "common_datetime.h"
#include "logic_manager_limit_boss.h"
#include "logic_game_cache_common.h"
#include "logic_game_global.h"

CLogicManagerLimitBoss::limit_boss_container CLogicManagerLimitBoss::m_stLimitBossMap;
std::set<int32_t> CLogicManagerLimitBoss::m_stPrivateBossList;
std::set<int32_t> CLogicManagerLimitBoss::m_stOpenBossList;
std::set<int32_t> CLogicManagerLimitBoss::m_stDeathJudgeBossList;

bool CLogicManagerLimitBoss::Initialize()
{
    const auto iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
    auto stData = CRecordGlobalLimitBossTable::SelectDataFromSQL(iServerID);
    for(const auto& data : stData)
    {
        m_stLimitBossMap.InsertWithoutSQL(std::make_pair(data.m_iOwnerUin, data));

        if(data.m_iDeathTime > 0)
            m_stDeathJudgeBossList.emplace(data.m_iOwnerUin);
        else if(data.m_cIsOpen)
            m_stOpenBossList.emplace(data.m_iOwnerUin);
        else
            m_stPrivateBossList.emplace(data.m_iOwnerUin);
    }

    return true;
}

void CLogicManagerLimitBoss::Refresh()
{
    const auto now_time = CLogicCommon::GetSec();

    // 刷新死亡判定队列
    static int32_t s_iLastCheckDeathTime = 0;
    if (now_time >= s_iLastCheckDeathTime)
    {
        s_iLastCheckDeathTime = now_time + 10_sec;

        RefreshDeathList();

        // 存数据库
        LOGIC_RECORD_CACHE_INS->ExecSQL(0, 0, "LOGIC_MANAGER_LIMIT_BOSS_DEATH");
    }

    // 刷新私有队列
    static int32_t s_iLastCheckPrivateTime = 0;
    if (now_time >= s_iLastCheckPrivateTime)
    {
        s_iLastCheckPrivateTime = now_time + 3_min;

        RefreshPrivateList();
    }

    // 刷新公开队列
    static int32_t s_iLastCheckOpenTime = 0;
    if (now_time >= s_iLastCheckOpenTime)
    {
        s_iLastCheckOpenTime = now_time + 10_min;

        RefreshOpenList();
    }

    // 存数据库
    LOGIC_RECORD_CACHE_INS->ExecSQL(0, 0, "LOGIC_MANAGER_LIMIT_BOSS_REFRESH");
}

void CLogicManagerLimitBoss::ClearBoss()
{
    // 先刷新死亡判定列表
    RefreshDeathList();

    // 清空限时BOSS信息
    while (!m_stLimitBossMap.Empty())
    {
        m_stLimitBossMap.Erase(m_stLimitBossMap.Begin());
    }

    // 存数据库
    LOGIC_RECORD_CACHE_INS->ExecSQL(0, 0, "LOGIC_MANAGER_LIMIT_BOSS_CLEAR");

    m_stDeathJudgeBossList.clear();
    m_stPrivateBossList.clear();
    m_stOpenBossList.clear();
}

bool CLogicManagerLimitBoss::AddBoss(const TGlobalLimitBossTableValueType& stBoss)
{
    auto iterResult = m_stLimitBossMap.Insert(std::make_pair(stBoss.m_iOwnerUin, stBoss));
    if(iterResult.second)
    {
        m_stPrivateBossList.emplace(stBoss.m_iOwnerUin);
    }

    return iterResult.second;
}

const TGlobalLimitBossTableValueType* CLogicManagerLimitBoss::GetAndRefreshBossData(int32_t iOwnerUin)
{
    auto iterBoss = m_stLimitBossMap.Find(iOwnerUin);
    if(iterBoss == m_stLimitBossMap.End())
        return nullptr;

    const auto& stData = iterBoss->second->m_stData;
    // 死亡判定中
    if(stData.m_iDeathTime)
        return &stData;

    auto iCurTime = CLogicCommon::GetSec();
    // 已消失
    if(stData.m_iEndTime <= iCurTime)
    {
        m_stLimitBossMap.Erase(iterBoss);
        m_stPrivateBossList.erase(iOwnerUin);
        m_stOpenBossList.erase(iOwnerUin);
        return nullptr;
    }

    // 自动公开
    if(!stData.m_cIsOpen && stData.m_iOpenTime <= iCurTime)
    {
        iterBoss->second->SetIsOpen(1);
        m_stPrivateBossList.erase(iOwnerUin);
        m_stOpenBossList.emplace(iOwnerUin);
    }

    return &stData;
}

void CLogicManagerLimitBoss::SetBossOpen(int32_t iOwnerUin)
{
    auto iterBoss = m_stLimitBossMap.Find(iOwnerUin);
    if(iterBoss == m_stLimitBossMap.End() || iterBoss->second->GetIsOpen() || iterBoss->second->GetDeathTime())
        return;

    iterBoss->second->SetIsOpen(1);
    m_stPrivateBossList.erase(iOwnerUin);
    m_stOpenBossList.emplace(iOwnerUin);
}

void CLogicManagerLimitBoss::SetBossAttackerList(int32_t iOwnerUin, std::map<int32_t, int32_t>& stAttackerList)
{
    auto iterBoss = m_stLimitBossMap.Find(iOwnerUin);
    if(iterBoss == m_stLimitBossMap.End())
        return;

    iterBoss->second->SetAttackerList(stAttackerList);
}

void CLogicManagerLimitBoss::UpdateOwnerName(int32_t iOwnerUin, const std::string& strName)
{
    auto iterBoss = m_stLimitBossMap.Find(iOwnerUin);
    if(iterBoss == m_stLimitBossMap.End())
        return;

    iterBoss->second->SetNick(strName);
}

bool CLogicManagerLimitBoss::SetBossHurt(int32_t iOwnerUin, uint32_t iHurt, int32_t iAttackUin, int32_t iAttackGroupID)
{
    auto iterBoss = m_stLimitBossMap.Find(iOwnerUin);
    if(iterBoss == m_stLimitBossMap.End() || iterBoss->second->GetDeathTime())
        return false;

    auto uLimitHurt = iterBoss->second->GetMaxHp() - iterBoss->second->GetCurHurt();
    if(uLimitHurt < iHurt)
        iHurt = uLimitHurt;

    iterBoss->second->AddCurHurt(iHurt);
    if(iterBoss->second->GetMaxHp() <= iterBoss->second->GetCurHurt())
    {
        iterBoss->second->SetKillerGroup(iAttackGroupID);
        iterBoss->second->SetKillerUin(iAttackUin);
        iterBoss->second->SetDeathTime(CLogicCommon::GetSec() + CLogicConfig::GetEntireWarLimitBossConfig().m_iBossDeathWaitTime);

        m_stPrivateBossList.erase(iOwnerUin);
        m_stOpenBossList.erase(iOwnerUin);
        m_stDeathJudgeBossList.emplace(iOwnerUin);
    }

    // 更新伤害
    auto stHurtLogData = iterBoss->second->GetHurtLog();
    if(stHurtLogData.find(iAttackUin) == stHurtLogData.end())
    {
        stHurtLogData[iAttackUin] = std::make_pair(0, 0);
    }

    if(stHurtLogData[iAttackUin].first < iHurt) stHurtLogData[iAttackUin].first = iHurt;
    stHurtLogData[iAttackUin].second += iHurt;

    if(stHurtLogData.size() >= CLogicConfigEntireWarLimitBossParser::E_MaxHurtLogNum)
        stHurtLogData.erase(stHurtLogData.begin());

    iterBoss->second->SetHurtLog(stHurtLogData);

    return true;
}

CPackLimitBossInfo CLogicManagerLimitBoss::GetPackBossInfo(const TGlobalLimitBossTableValueType& stBoss, int32_t iUin)
{
    CPackLimitBossInfo stPackInfo;
    stPackInfo.m_iOwnerUin = stBoss.m_iOwnerUin;
    stPackInfo.m_iOwnerGroupID = stBoss.m_iOwnerGroup;
    stPackInfo.m_strNick = stBoss.m_strNick;
    stPackInfo.m_cHard = stBoss.m_cHard;
    stPackInfo.m_cIsOpen = stBoss.m_cIsOpen;
    stPackInfo.m_iEndTime = stBoss.m_iEndTime;
    stPackInfo.m_iOpenTime = stBoss.m_iOpenTime;
    stPackInfo.m_iDeathTime = stBoss.m_iDeathTime;
    stPackInfo.m_ulMaxHp = stBoss.m_ulMaxHp;
    stPackInfo.m_ulCurHurt = stBoss.m_ulCurHurt;
    if(iUin > 0)
    {
        auto iter = stBoss.m_stHurtLog.find(iUin);
        if(iter != stBoss.m_stHurtLog.end())
        {
            stPackInfo.m_uMyMaxHurt = iter->second.first;
            stPackInfo.m_ulMyTotalHurt = iter->second.second;
        }
    }

    return stPackInfo;
}

void CLogicManagerLimitBoss::GetSomeRandBoss(std::vector<CPackLimitBossInfo>& stBossVec, std::set<int32_t>& stExclude, int32_t iNum, int32_t iUin)
{
    if(m_stOpenBossList.empty() || iNum <= 0) return;

    auto iter = m_stOpenBossList.begin();
    int32_t iMaxNum = m_stOpenBossList.size() - iNum + 1;

    if(iMaxNum > 1)
    {
        int32_t iRandIndex = CLogicCommon::GetRandNum() % iMaxNum + 1;
        while (iRandIndex > 1 && iter != m_stOpenBossList.end())
        {
            --iRandIndex;
            ++iter;
        }
    }

    for(; iter != m_stOpenBossList.end() && iNum > 0; ++iter, --iNum)
    {
        if(stExclude.find(*iter) != stExclude.end())
            continue;

        auto pBossData = GetAndRefreshBossData(*iter);
        if(pBossData == nullptr || pBossData->m_iDeathTime)
            continue;

        stBossVec.emplace_back(GetPackBossInfo(*pBossData, iUin));
    }
}

void CLogicManagerLimitBoss::RefreshDeathList()
{
    auto& stBossConfig = CLogicConfig::GetEntireWarLimitBossConfig();
    for(auto iter = m_stDeathJudgeBossList.begin(); iter != m_stDeathJudgeBossList.end(); )
    {
        auto iterBoss = m_stLimitBossMap.Find(*iter);
        if(iterBoss == m_stLimitBossMap.End())
        {
            iter = m_stDeathJudgeBossList.erase(iter);
            continue;
        }

        if(iterBoss->second->GetDeathTime() <= CLogicCommon::GetSec())
        {
            // 发送奖励邮件
            auto pHardConfig = stBossConfig.GetHardConfig(iterBoss->second->GetHard());
            auto pKillerUser = CLogicCacheCommon::LoadUserData(iterBoss->second->GetKillerGroup(), iterBoss->second->GetKillerUin());
            if (nullptr != pKillerUser)
            {
                pKillerUser->m_stUserDailyTable.AddLimitBossAction(-(pHardConfig->m_iKillRewardCoin));
                auto stReward = CLogicConfig::GetRewardBagConfig().GetReward(pHardConfig->m_strKillRewardBagID,
                                                                             pKillerUser->m_stUserInfoTable.GetLevel(), pHardConfig->m_iHard);
                if(!stReward.empty())
                {
                    CLogicGlobal::AddAsyncUserMail(iterBoss->second->GetKillerUin(), iterBoss->second->GetKillerGroup(),
                                                   stBossConfig.m_strMailSender, stBossConfig.m_strKillerMailTitle,
                                                   stBossConfig.m_strKillerMailContent, stReward, "SEND_LIMIT_BOSS_KILLER_AWARD");
                }
            }

            // 发送发现者奖励
            auto pFinderUser = CLogicCacheCommon::LoadUserData(iterBoss->second->GetOwnerGroup(), iterBoss->first);
            if(nullptr != pFinderUser)
            {
                auto stFindReward = CLogicConfig::GetRewardBagConfig().GetReward(pHardConfig->m_strFinderRewardBagID,
                                                                                 pFinderUser->m_stUserInfoTable.GetLevel(), pHardConfig->m_iHard);
                if(!stFindReward.empty())
                {
                    CLogicGlobal::AddAsyncUserMail(iterBoss->first, iterBoss->second->GetOwnerGroup(),
                                                   stBossConfig.m_strMailSender, stBossConfig.m_strOwnerMailTitle,
                                                   stBossConfig.m_strOwnerMailContent, stFindReward, "SEND_LIMIT_BOSS_FINDER_AWARD");
                }
            }

            m_stLimitBossMap.Erase(iterBoss);
            iter = m_stDeathJudgeBossList.erase(iter);
            continue;
        }

        ++iter;
    }
}

void CLogicManagerLimitBoss::RefreshPrivateList()
{
    for(auto iter = m_stPrivateBossList.begin(); iter != m_stPrivateBossList.end(); )
    {
        auto iterBoss = m_stLimitBossMap.Find(*iter);
        if(iterBoss == m_stLimitBossMap.End())
        {
            iter = m_stPrivateBossList.erase(iter);
            continue;
        }

        if(iterBoss->second->GetIsOpen() || iterBoss->second->GetOpenTime() <= CLogicCommon::GetSec())
        {
            iterBoss->second->SetIsOpen(1);
            m_stOpenBossList.emplace(*iter);
            iter = m_stPrivateBossList.erase(iter);
            continue;
        }

        ++iter;
    }
}

void CLogicManagerLimitBoss::RefreshOpenList()
{
    for(auto iter = m_stOpenBossList.begin(); iter != m_stOpenBossList.end(); )
    {
        auto iterBoss = m_stLimitBossMap.Find(*iter);
        if(iterBoss == m_stLimitBossMap.End())
        {
            iter = m_stOpenBossList.erase(iter);
            continue;
        }

        if(iterBoss->second->GetEndTime() <= CLogicCommon::GetSec())
        {
            m_stLimitBossMap.Erase(iterBoss);
            iter = m_stOpenBossList.erase(iter);
            continue;
        }

        ++iter;
    }
}
