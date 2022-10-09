#pragma once

#include "logic_game_processor_base.h"

class CLogicRankProcessor : public CLogicProcessorBase
{
public:
	CLogicRankProcessor(uint16_t usCmd, const std::string& strCmdName);

protected:
    bool DoUserRun(const msgpack::unpacked& stFormatData) override;

private:
    CPackGetPvpRankListData
    GetPVPPlayerInfo(int iGroupID, int iUin);

    CPackFightPowerRankListData
    GetFightPowerPlayerInfo(int iGroupID, int iUin);

	CPackClimbTowerRankListData
	GetClimbTowerPlayerInfo(int iGroupID, int iUin, int iScore);

    std::vector<CPackRankBaseCardInfo> GetTeamCardInfo(user_data_table_ptr_type pUserInfo, int32_t iType);

    CPackRankBaseCardInfo GetCardInfo(user_data_table_ptr_type pUserInfo, int iCardID);

};
