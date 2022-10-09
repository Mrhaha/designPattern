#pragma once

#include <map>
#include "tc_singleton.h"

#include "i_logic_statistics.h"
#include "logic_service_tcpconnection.h"
#include "logic_service_mysql_handle.h"

class CLogicStatistics : public ILogicStatistics
{
private:
	struct TPlayerStatInfo
	{
		TPlayerStatInfo();
		void clear();
		void refresh();
		void clear_add();

		int32_t m_iChannelID;
		int32_t m_iCreateTime;
		std::string m_strNick;
		int32_t m_iLevel;
		int32_t m_iAddLoginCount;	//增量统计
		int32_t m_iLastLoginTime;
		int32_t m_iAddOnlineTime;	//增量统计
		int32_t m_iAddDailyPayCash;	//增量统计
		int32_t m_iPayCash;
		int32_t m_iYuanbao;
		int32_t m_iAddFairWinCount;
		std::string m_strFairTeamCard;
        int32_t m_iRoleCombat;
		int32_t m_iLastUpdateTime;
		int32_t m_iLastReportTime;
	};

	struct TEventStatInfo
	{
		TEventStatInfo();
		void clear();
		void refresh();
		void clear_add();

		int32_t m_iAddEventNum;
		int32_t m_iAddExtraNum;
		int32_t m_iLastUpdateTime;
		int32_t m_iLastReportTime;
	};

public:
    static int32_t GetNowSec();

    static std::string GetNowDateString();

public:
	CLogicStatistics();

    void AddCreateRoleCount(int32_t iChannelID) override;

    void UpdateLogin(int32_t iUin, int32_t iGroupID) override;

    void UpdatePlayerInfo(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCreateTime, const string &strNick
                          , int32_t iLevel, int32_t iYuanbao, int32_t iPayCash, int32_t iCombat) override;

    void AddPlayerOnlineTime(int32_t iUin, int32_t iGroupID, int32_t iAddOnlineTime) override;

    void AddPlayerDailyPayCash(int32_t iUin, int32_t iGroupID, int32_t iAddDailyPayCash, int32_t iTotalPayCash) override;

    void UpdatePlayerFairResult(int32_t iUin, int32_t iGroupID, int32_t iAddWinCount, const std::vector<int32_t>& iTeamVec) override;

    void AddEventStat(const std::string& strEvent, const std::string& strEventID, int32_t iAddEventNum, int32_t iAddExtraNum) override;

public:
	void Refresh();		//刷新上报时间

	void LastExec();	//停服前最后一次上报

private:
	void ExecStatOnline();

	void ExecStatPlayer();

	void ExecStatEvent();

private:
    void OnCrossOneDay();

	void RefreshStatPlayer();

	void RefreshStatEvent();
	
	TPlayerStatInfo& GetPlayerStatInfo(int32_t iUin, int32_t iGroupID);

	TEventStatInfo& GetEventStatInfo(const std::string& strEvent, const std::string& strEventID);

private:
	string m_strStatDate;

	std::map<int32_t, int32_t> m_iCreateRoleCountMap;

	map<std::pair<int32_t, int32_t>, TPlayerStatInfo> m_astPlayerStatInfo;
	map<std::string, map<std::string, TEventStatInfo>> m_astEventStatInfo;
};


#define SERVICE_STATISTICS taf::TC_Singleton<CLogicStatistics>::getInstance()

