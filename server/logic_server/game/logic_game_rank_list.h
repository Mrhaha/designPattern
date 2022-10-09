#pragma once

#include "server_recovery_protocol.h"
#include "logic_game_cache_common.h"

class CLogicRankList
{
public:
	template<class RankType>
	static vector<CPackUserRank> GetUserRank(RankType& RankTypeIns);

	static void RemoveUserRank(int32_t iUin, int32_t iGroupID);

	static void RestoreUserRank(user_data_table_ptr_type pUserData);
};

template<class RankType>
vector<CPackUserRank> CLogicRankList::GetUserRank(RankType& RankTypeIns)
{
	vector<CPackUserRank> stRankVec;
	int32_t iRank = 1;
	for (auto iter = RankTypeIns.begin(); iter != RankTypeIns.end() && iRank <= 30; ++iter, ++iRank)
	{
		const user_info_key_t stKey(iter->first);
		const auto pUserInfo = CLogicCacheCommon::GetInCacheUserData(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
		if (nullptr != pUserInfo)
		{
			stRankVec.push_back(CLogicCacheCommon::GetUserRankInfo(pUserInfo, iRank, iter->second));
		}
		else
		{
			CPackUserRank stPack;
			stPack.m_iUid = stKey.m_stRoleIDDetail.m_iUin;
			stPack.m_iGroupID = stKey.m_stRoleIDDetail.m_iGroupID;
			stPack.m_iRank = iRank;
			stPack.m_iRankScore = iter->second;
			stRankVec.push_back(stPack);
		}
	}
	return stRankVec;
}




