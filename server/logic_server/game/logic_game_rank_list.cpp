#include "logic_link_common.h"
#include "logic_game_rank_list.h"
#include "logic_game_global.h"
#include "logic_game_cache_common.h"
#include "logic_config.h"
#include "logic_game_cache_table.h"

void CLogicRankList::RemoveUserRank(int32_t iUin, int32_t iGroupID)
{
	user_info_key_t stKey;
	stKey.m_stRoleIDDetail.m_iUin = iUin;
	stKey.m_stRoleIDDetail.m_iGroupID = iGroupID;

    LOGIC_FIGHT_POWER_RANK_INS.erase(stKey.m_ulRoleID);
	LOGIC_CLIMB_TOWER_RANK_INS.erase(stKey.m_ulRoleID);
	LOGIC_COUNTDOWN_HERO_RANK_INS.erase(stKey.m_ulRoleID);
	LOGIC_MILITARY_LEVEL_RANK_INS.erase(stKey.m_ulRoleID);
	LOGIC_HOME_RANK_INS.erase(stKey.m_ulRoleID);
	LOGIC_ENTIRE_WAR_RANK_INS.erase(stKey.m_ulRoleID);
	LOGIC_CHARM_RANK_INS.erase(stKey.m_ulRoleID);
	LOGIC_IBUSY_SCORE_RANK_INS.erase(stKey.m_ulRoleID);

	//竞技场移除排行榜时需要找个机器人替补
	for (const auto& stVal : CLogicConfig::GetPVPRobotConfig().m_stRobotList)
	{
		if (LOGIC_PVP_RANK_INS.find(stVal.first) == LOGIC_PVP_RANK_INS.end())
		{
			auto uiRank = LOGIC_PVP_RANK_INS.GetRank(stKey.m_ulRoleID);
			if (uiRank > 0)
			{
				LOGIC_PVP_RANK_INS.erase(stKey.m_ulRoleID);
                user_info_key_t key(stVal.first);
				CLogicGlobal::UpdateOrInsertPvpRank(key.m_stRoleIDDetail.m_iUin, 0, uiRank);
			}
			break;
		}
	}

    // 通用排行榜
    for(auto& iterRank : CLogicGlobal::GetGlobalCache()->m_stCommonRankMap)
    {
        iterRank.second.erase(stKey.m_ulRoleID);
    }
}

void CLogicRankList::RestoreUserRank(user_data_table_ptr_type pUserData)
{
    CLogicGlobal::UpdateOrInsertCharmRank(pUserData->m_iUin, pUserData->m_iGroupID, pUserData->m_stUserInfoTable.GetCharm());

    if (!LOGIC_CHECK_WEEK_REFRESH(pUserData->m_stUserInfoTable.GetLastEntireWarTime()))
    {//本周进行了全民战役才进入周排行榜
        CLogicGlobal::UpdateOrInsertEntireWarRank(pUserData->m_iUin, pUserData->m_iGroupID, pUserData->m_stUserInfoTable.GetWeekWarContribution());
    }

    CLogicGlobal::UpdateOrInsertMilitaryRank(pUserData->m_iUin, pUserData->m_iGroupID, pUserData->m_stUserInfoTable.GetEntireWarMilitarkLevel());

    CLogicGlobal::UpdateOrInsertPowerRank(pUserData->m_iUin, pUserData->m_iGroupID, pUserData->m_stUserInfoTable.GetRoleCombat());

    CLogicGlobal::UpdateOrInsertTowerRank(pUserData->m_iUin, pUserData->m_iGroupID, pUserData->m_stUserInfoTable.GetClimbTowerMaxOrder());
}
