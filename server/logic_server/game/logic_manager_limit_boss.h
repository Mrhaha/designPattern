//
// Created by luoxiaohu on 2021/4/12.
//

#pragma once

#include "logic_record_sql_associate_container.hpp"
#include "logic_record_global_limit_boss_table.h"
#include "server_entire_war_protocol.h"

class CLogicManagerLimitBoss
{
private:
    using limit_boss_container = CLogicSQLRecordAssociateContainer<int32_t, TGlobalLimitBossTable*, TGlobalLimitBossTableValueType>;

public:
    static bool Initialize();
    static void Refresh();
    static void ClearBoss();
    static int32_t GetBossNum() { return m_stLimitBossMap.Size(); };
    static bool AddBoss(const TGlobalLimitBossTableValueType& stBoss);
    static const TGlobalLimitBossTableValueType* GetAndRefreshBossData(int32_t iOwnerUin);
    static void SetBossOpen(int32_t iOwnerUin);
    static void SetBossAttackerList(int32_t iOwnerUin, std::map<int32_t, int32_t>& stAttackerList);
    static void UpdateOwnerName(int32_t iOwnerUin, const std::string& strName);
    static bool SetBossHurt(int32_t iOwnerUin, uint32_t iHurt, int32_t iAttackUin, int32_t iAttackGroupID);

    static CPackLimitBossInfo GetPackBossInfo(const TGlobalLimitBossTableValueType& stBoss, int32_t iUin);
    static void GetSomeRandBoss(std::vector<CPackLimitBossInfo>& stBossVec, std::set<int32_t>& stExclude, int32_t iNum, int32_t iUin);

private:
    static void RefreshDeathList();
    static void RefreshPrivateList();
    static void RefreshOpenList();

private:
    static limit_boss_container m_stLimitBossMap;              // 限时BOSS数据集合
    static std::set<int32_t>   m_stPrivateBossList;            // 未公开的BOSS列表
    static std::set<int32_t>   m_stOpenBossList;               // 已公开的BOSS列表
    static std::set<int32_t>   m_stDeathJudgeBossList;         // 死亡判定中的BOSS列表
};
