//
// Created by luoxiaohu on 2020/10/10.
//

#include "logic_game_version_data_update.h"
#include "logic_game_cache_common.h"
#include "logic_config.h"
#include "logic_link_define.h"

void CLogicVersionDataUpdateSystem::CheckVersionDataUpdate(user_data_table_ptr_type pUserInfo)
{
    if(pUserInfo->m_stUserInfoTable.GetDataUpdateVersion() == EV_CurVersion) return;

    // 摆件馆修改
    if(pUserInfo->m_stUserInfoTable.GetDataUpdateVersion() < EV_CardGradeMod)
        UpdateCardGradeData(pUserInfo);

    // 老玩家添加CD音乐
    if(pUserInfo->m_stUserInfoTable.GetDataUpdateVersion() < EV_AddNewMusic)
    {
        auto createInitConfig = CLogicConfig::GetAwardConfig();
        CLogicCacheCommon::AddGameItem(pUserInfo,createInitConfig.m_stAddReward,pUserInfo->m_iCommand);
    }

    // 记录最新版本
    pUserInfo->m_stUserInfoTable.SetDataUpdateVersion(EV_CurVersion);
}

void CLogicVersionDataUpdateSystem::UpdateCardGradeData(user_data_table_ptr_type pUserInfo)
{
    for(auto iter = pUserInfo->m_stHeroCardTableMap.Begin(); iter != pUserInfo->m_stHeroCardTableMap.End(); ++iter)
    {
        auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iter->first);
        if(pConfig)
        {
            iter->second->SetGradeLevel(pConfig->m_iInitGrade);
        }
    }
}
