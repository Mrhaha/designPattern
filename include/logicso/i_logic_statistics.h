#pragma once
#include <string>
#include <vector>

class ILogicStatistics
{
public:
	virtual ~ILogicStatistics() = default;

	virtual void AddCreateRoleCount(int32_t iChannelID) = 0;

	virtual void UpdateLogin(int32_t iUin, int32_t iGroupID) = 0;

	virtual void UpdatePlayerInfo(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCreateTime, const string &strNick
                                  , int32_t iLevel, int32_t iYuanbao, int32_t iPayCash, int32_t iCombat) = 0;

	virtual void AddPlayerOnlineTime(int32_t iUin, int32_t iGroupID, int32_t iAddOnlineTime) = 0;

	virtual void AddPlayerDailyPayCash(int32_t iUin, int32_t iGroupID, int32_t iAddDailyPayCash, int32_t iTotalPayCash) = 0;

	virtual void UpdatePlayerFairResult(int32_t iUin, int32_t iGroupID, int32_t iAddWinCount, const std::vector<int32_t>& iTeamVec) = 0;

	virtual void AddEventStat(const std::string& strEvent, const std::string& strEventID, int32_t iAddEventNum, int32_t iAddExtraNum) = 0;
};

