#include "tc_md5.h"
#include "common_datetime.h"
#include "common_mysql.h"
#include "logic_service_statistics.h"
#include "logic_service_text_checker.h"
#include "logic_service_mysql_handle.h"


std::string escape_stat_sql_string(const std::string& sql)
{
    return SERVICE_MYSQL_HANDLE->GetGameQueryConn()->escapeString(sql);
}


CLogicStatistics::TPlayerStatInfo::TPlayerStatInfo()
{
	clear();
	m_iLastUpdateTime = GetNowSec();
	m_iLastReportTime = GetNowSec();

}

void CLogicStatistics::TPlayerStatInfo::clear()
{
    m_iChannelID = 0;
	m_iCreateTime = 0;
	m_strNick.clear();
	m_iLevel = 0;
	m_iAddLoginCount = 0;
	m_iLastLoginTime = 0;
	m_iAddOnlineTime = 0;
	m_iAddDailyPayCash = 0;
	m_iPayCash = 0;
	m_iYuanbao = 0;
	m_iAddFairWinCount = 0;
	m_strFairTeamCard.clear();
    m_iRoleCombat = 0;
}

void CLogicStatistics::TPlayerStatInfo::refresh()
{
	m_iAddLoginCount = 1;
	m_iLastLoginTime = GetNowSec();
	m_iAddOnlineTime = 0;
	m_iAddDailyPayCash = 0;
	m_iAddFairWinCount = 0;
	m_strFairTeamCard.clear();
    m_iLastUpdateTime = GetNowSec();
    m_iLastReportTime = 0;
}

void CLogicStatistics::TPlayerStatInfo::clear_add()
{
	m_iAddLoginCount = 0;
	m_iAddOnlineTime = 0;
	m_iAddDailyPayCash = 0;
	m_iAddFairWinCount = 0;
	m_iLastUpdateTime = GetNowSec();
	m_iLastReportTime = GetNowSec();
}

CLogicStatistics::TEventStatInfo::TEventStatInfo()
{
	clear();
	m_iLastUpdateTime = GetNowSec();
	m_iLastReportTime = GetNowSec();
}

void CLogicStatistics::TEventStatInfo::clear()
{
	m_iAddEventNum = 0;
	m_iAddExtraNum = 0;
}

void CLogicStatistics::TEventStatInfo::refresh()
{
	m_iAddEventNum = 0;
	m_iAddExtraNum = 0;
}

void CLogicStatistics::TEventStatInfo::clear_add()
{
	m_iAddEventNum = 0;
	m_iAddExtraNum = 0;
}

int32_t CLogicStatistics::GetNowSec()
{
#if LOGIC_BUILD_PLATFORM == BUILD_PLATFORM_LOCAL
    return now_sec();
#endif
	return SERVICE_API_INS->GetLogicSoGameApi()->GetNowSec();
}

std::string CLogicStatistics::GetNowDateString()
{
	return get_date_string(GetNowSec());
}

CLogicStatistics::CLogicStatistics() : m_strStatDate(GetNowDateString())
{
	m_astPlayerStatInfo.clear();
}

void CLogicStatistics::AddCreateRoleCount(int32_t iChannelID)
{
    ++m_iCreateRoleCountMap[iChannelID];
}

void CLogicStatistics::UpdateLogin(int32_t iUin, int32_t iGroupID)
{
    TPlayerStatInfo& rstInfo = GetPlayerStatInfo(iUin, iGroupID);
    ++rstInfo.m_iAddLoginCount;
    rstInfo.m_iLastLoginTime = GetNowSec();
    rstInfo.m_iLastUpdateTime = GetNowSec();
}

void CLogicStatistics::UpdatePlayerInfo(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCreateTime
                                        , const string &strNick, int32_t iLevel, int32_t iYuanbao, int32_t iPayCash, int32_t iCombat)
{
    TPlayerStatInfo& rstInfo = GetPlayerStatInfo(iUin, iGroupID);

    if (rstInfo.m_iCreateTime != iCreateTime
        || rstInfo.m_strNick != strNick
        || rstInfo.m_iLevel != iLevel
        || rstInfo.m_iYuanbao != iYuanbao
        || rstInfo.m_iPayCash != iPayCash
        || rstInfo.m_iRoleCombat != iCombat
        )
    {
        rstInfo.m_iChannelID = iChannelID > 0 ? iChannelID : rstInfo.m_iChannelID;
        rstInfo.m_iCreateTime = iCreateTime;
        rstInfo.m_strNick = strNick;
        rstInfo.m_iLevel = iLevel;
        rstInfo.m_iPayCash = iPayCash;
        rstInfo.m_iYuanbao = iYuanbao;
        rstInfo.m_iRoleCombat = iCombat;
        rstInfo.m_iLastUpdateTime = GetNowSec();
    }
}

void CLogicStatistics::AddPlayerOnlineTime(int32_t iUin, int32_t iGroupID, int32_t iAddOnlineTime)
{
    TPlayerStatInfo& rstInfo = GetPlayerStatInfo(iUin, iGroupID);
    rstInfo.m_iAddOnlineTime += iAddOnlineTime;
    rstInfo.m_iLastUpdateTime = GetNowSec();
}

void CLogicStatistics::AddPlayerDailyPayCash(int32_t iUin, int32_t iGroupID, int32_t iAddDailyPayCash, int32_t iTotalPayCash)
{
    TPlayerStatInfo& rstInfo = GetPlayerStatInfo(iUin, iGroupID);
    rstInfo.m_iAddDailyPayCash += iAddDailyPayCash;
    rstInfo.m_iPayCash = iTotalPayCash;
    rstInfo.m_iLastUpdateTime = GetNowSec();
}

void CLogicStatistics::UpdatePlayerFairResult(int32_t iUin, int32_t iGroupID, int32_t iAddWinCount, const std::vector<int32_t>& iTeamVec)
{
    TPlayerStatInfo& rstInfo = GetPlayerStatInfo(iUin, iGroupID);
    if (iAddWinCount > 0)
    {
        rstInfo.m_iAddFairWinCount += iAddWinCount;
        rstInfo.m_iLastUpdateTime = GetNowSec();
    }

    if (rstInfo.m_strFairTeamCard.empty())
    {
        rstInfo.m_strFairTeamCard = vector_join(iTeamVec, ", ");
        rstInfo.m_iLastUpdateTime = GetNowSec();
    }
}

void CLogicStatistics::AddEventStat(const std::string& strEvent, const std::string& strEventID, int32_t iAddEventNum, int32_t iAddExtraNum)
{
    auto& rstInfo = GetEventStatInfo(strEvent, strEventID);
    rstInfo.m_iAddEventNum += iAddEventNum;
    rstInfo.m_iAddExtraNum += iAddExtraNum;
    rstInfo.m_iLastUpdateTime = GetNowSec();
}

void CLogicStatistics::Refresh()
{
	int32_t iNow = GetNowSec();

	static int32_t s_iLastCheckTodayTime = iNow;	//每日0时定时器
	static int32_t s_iLastCheckReportOnline = iNow; //整点5分上报定时器

	if (!is_same_day(iNow, s_iLastCheckTodayTime))
	{
		OnCrossOneDay();
		s_iLastCheckTodayTime = iNow;
	}

	if ((s_iLastCheckReportOnline / 300) != (iNow / 300))
	{
        ExecStatPlayer();

        ExecStatOnline();

        ExecStatEvent();

		s_iLastCheckReportOnline = iNow;
	}
}

void CLogicStatistics::LastExec()
{
    ExecStatPlayer();
    ExecStatEvent();
}

CLogicStatistics::TPlayerStatInfo& CLogicStatistics::GetPlayerStatInfo(int32_t iUin, int32_t iGroupID)
{
	return m_astPlayerStatInfo[std::make_pair(iUin, iGroupID)];
}

CLogicStatistics::TEventStatInfo& CLogicStatistics::GetEventStatInfo(const std::string& strEvent, const std::string& strEventID)
{
	return m_astEventStatInfo[strEvent][strEventID];
}

void CLogicStatistics::ExecStatOnline()
{
	const auto& system_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameServerConfig();

	std::map<int32_t, int32_t> OnlineRoleCountMap = SERVICE_API_INS->GetLogicSoGameApi()->GetOnlineChannelCount();
	for (const auto& create : m_iCreateRoleCountMap)
	{
        OnlineRoleCountMap.insert(std::make_pair(create.first, 0));
	}

	int32_t iTotalOnline = 0;
    for (const auto& online : OnlineRoleCountMap)
	{
        iTotalOnline += online.second;
        auto sql = build_insert_escape_sql("stat_online", {
                { "stat_time",  get_datetime_string(GetNowSec())},
                { "platform",   system_config.m_strPlatform},
                { "server_id",  system_config.m_iServerID},
                { "channel_id", online.first},
                { "online_num", online.second},
                { "create_num", m_iCreateRoleCountMap[online.first]}
	    }, escape_stat_sql_string);
        SERVICE_MYSQL_HANDLE->ExecStatSQL(0, 0, "EXEC_STAT_EVENT", "EXEC_STAT_EVENT", sql );
	}

    m_iCreateRoleCountMap.clear();

    if (iTotalOnline > 10 && SERVICE_API_INS->GetLogicSoGameApi()->GetGameServerConfig().m_iLoginPushover)
    {
        SERVICE_API_INS->RequestServiceAlert("online_too_more", {
                { "online", std::to_string(iTotalOnline) }
        });
    }
}

void CLogicStatistics::ExecStatPlayer()
{
	const auto& system_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameServerConfig();
	for (auto & player : m_astPlayerStatInfo)
	{
	    if (player.second.m_iLastUpdateTime > player.second.m_iLastReportTime)
        {
            TPlayerStatInfo& rstInfo = player.second;
            auto sql = build_insert_escape_sql("stat_player", {
                    { "stat_date",    m_strStatDate},
                    { "uin",          player.first.first},
                    { "platform",     system_config.m_strPlatform},
                    { "channel_id",   rstInfo.m_iChannelID},
                    { "group_id",     player.first.second},
                    { "server_id",    system_config.m_iServerID},
                    { "create_time",  rstInfo.m_iCreateTime},
                    { "nick",         rstInfo.m_strNick},
                    { "level",        rstInfo.m_iLevel},
                    { "login_count",  rstInfo.m_iAddLoginCount},
                    { "login_time",   rstInfo.m_iLastLoginTime},
                    { "online_time",  rstInfo.m_iAddOnlineTime},
                    { "daily_pay_cash", rstInfo.m_iAddDailyPayCash},
                    { "pay_cash",     rstInfo.m_iPayCash},
                    { "yuanbao",      rstInfo.m_iYuanbao},
                    { "fair_win_count", rstInfo.m_iAddFairWinCount},
                    { "fair_team_card", rstInfo.m_strFairTeamCard},
                    { "role_combat",    rstInfo.m_iRoleCombat},
            }, escape_stat_sql_string);

            sql.pop_back();
            sql += " on duplicate key update `platform`=values(`platform`), `channel_id`=values(`channel_id`)"
                   ", `group_id`=values(`group_id`), `server_id`=values(`server_id`)"
                   ", `create_time`=values(`create_time`), `nick`=values(`nick`), `level`=values(`level`)"
                   ", `login_count`=login_count+values(`login_count`), `login_time`=values(`login_time`)"
                   ", `online_time`=online_time+values(`online_time`), `daily_pay_cash`=`daily_pay_cash`+values(`daily_pay_cash`)"
                   ", `pay_cash`=values(`pay_cash`), `yuanbao`=values(`yuanbao`), `role_combat`=values(`role_combat`)"
                   ", `fair_win_count`=`fair_win_count`+values(`fair_win_count`), `fair_team_card`=values(`fair_team_card`);";

            SERVICE_MYSQL_HANDLE->ExecStatSQL(player.first.second, player.first.first, "EXEC_STAT_PLAYER", "EXEC_STAT_PLAYER",  sql );

            rstInfo.clear_add();
        }
	}
}

void CLogicStatistics::ExecStatEvent()
{
	const auto& system_config = SERVICE_API_INS->GetLogicSoGameApi()->GetGameServerConfig();
	for (auto& info : m_astEventStatInfo)
	{
		for (auto& id : info.second)
		{
			if (id.second.m_iLastUpdateTime >= id.second.m_iLastReportTime)
			{
				auto sql = build_insert_escape_sql("stat_event", {
                        { "stat_date", get_date_string(id.second.m_iLastUpdateTime) },
                        { "platform",  system_config.m_strPlatform },
                        { "event",     info.first},
                        { "event_id",  id.first},
                        { "event_num", id.second.m_iAddEventNum},
                        { "extra_num", id.second.m_iAddExtraNum},
				}, escape_stat_sql_string);
                sql.pop_back();
				sql += " on duplicate key update `event_num`=`event_num`+values(`event_num`), `extra_num`=`extra_num`+values(`extra_num`)";
                SERVICE_MYSQL_HANDLE->ExecStatSQL(0, 0, "REPORT_STAT_EVENT", "REPORT_STAT_EVENT",  sql );

				id.second.clear_add();
				id.second.m_iLastReportTime = GetNowSec();
			}
		}
	}
}

void CLogicStatistics::OnCrossOneDay()
{
    RefreshStatPlayer();

    RefreshStatEvent();

    m_strStatDate = GetNowDateString();
}


void CLogicStatistics::RefreshStatPlayer()
{
    ExecStatPlayer();

	auto iter = m_astPlayerStatInfo.begin();
	while (iter != m_astPlayerStatInfo.end())
	{
	    const auto info = SERVICE_API_INS->GetLogicSoGameApi()->GetClientInfo(iter->first.first);
		if (info.m_iGroupID != iter->first.second)
		{
			m_astPlayerStatInfo.erase(iter++);
		}
		else
		{
			iter->second.refresh();
			++iter;
		}
	}
}

void CLogicStatistics::RefreshStatEvent()
{
    ExecStatEvent();
}


