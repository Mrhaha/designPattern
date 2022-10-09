#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_cross_protocol.h"
#include "logic_link_common.h"
#include "logic_game_user.h"
#include "logic_game_cache_common.h"
#include "logic_game_bill.h"
#include "logic_game_global.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"

CLogicUser* CLogicUser::m_pLogicUser = nullptr;

CLogicUser& CLogicUser::GetInstance()
{
	if(nullptr == m_pLogicUser)
	{
		m_pLogicUser = new CLogicUser();
	}

	return (*m_pLogicUser);
}

CLogicUser::const_iterator CLogicUser::Begin() const
{
	return (m_stUserContainer.begin());
}

CLogicUser::const_iterator CLogicUser::End() const
{
	return (m_stUserContainer.end());
}

CLogicUser::size_type CLogicUser::GetUserNum() const
{
	return (m_stUserContainer.size());
}

CLogicUser::const_iterator CLogicUser::GetUserInfo(int iUin) const
{
	return (m_stUserContainer.find(iUin));
}

stLogicSoClientInfo CLogicUser::GetClientInfo(int32_t iUin) const
{
	const auto iter = GetUserInfo(iUin);
	return iter == End() ? stLogicSoClientInfo() : iter->second;
}

bool CLogicUser::SetUserInfo(int iUin, const stLogicSoClientInfo& stUserInfo)
{
	auto stIter = m_stUserContainer.find(iUin);
    if (stIter != m_stUserContainer.end())
    {
        stIter->second = stUserInfo;
        return true;
    }
    return false;
}

std::pair<CLogicUser::iterator, bool> CLogicUser::AddUser(int iGroupID, int iUin, stLogicSoClientInfo& stUserInfo)
{
    stUserInfo.m_iGroupID = iGroupID;
    stUserInfo.m_iLoginTime = CLogicCommon::GetSec();
	stUserInfo.m_iLastOnlineTime = CLogicCommon::GetSec();

	OnUserLogin(iUin, iGroupID, stUserInfo.m_iChannelID);

    return (m_stUserContainer.insert(std::pair<int, stLogicSoClientInfo>(iUin, stUserInfo)));
}

std::pair<CLogicUser::iterator, bool> CLogicUser::GetUser(int iUin)
{
    auto stIter = m_stUserContainer.find(iUin);
    return (std::make_pair(stIter, stIter != m_stUserContainer.end()));
}

bool CLogicUser::DelUser(int iUin)
{
    auto stIter = m_stUserContainer.find(iUin);
    if (stIter != m_stUserContainer.end())
    {
        LOGIC_LOG_TRACE_DEBUG << "GROUP_ID:" << stIter->second.m_iGroupID << "|UIN:" << iUin
                        << "|REMOTE_HOST:" << stIter->second.m_strClientIP << ":" 
                        << stIter->second.m_usClientPort << "|ACCOUNT:" << stIter->second.m_stStatInfo.m_strAccount 
                        << "|DEVICE_ID:" << stIter->second.m_stStatInfo.m_strDeviceID 
                        << "|DEVICE_MODEL:" << stIter->second.m_stStatInfo.m_strDeviceModel 
                        << "|DEVICE_OS:" << stIter->second.m_stStatInfo.m_strDeviceOS 
                        << "|DEVICE_RATIO:" << stIter->second.m_stStatInfo.m_strDeviceRatio 
                        << "|DEVICE_NET:" << stIter->second.m_stStatInfo.m_strDeviceNet 
                        << "|DEVICE_CARRIER:" << stIter->second.m_stStatInfo.m_strDeviceCarrier 
                        << "|PLATFORM_ID:" << stIter->second.m_stStatInfo.m_strPlatformID 
                        << "|ONLINE TIME:" << CLogicCommon::GetSec() - stIter->second.m_iLoginTime 
                        << "|LOGOUT SUCCESS" << std::endl;

		OnUserLogout(iUin, stIter->second.m_iGroupID, stIter->second.m_iChannelID, CLogicCommon::GetSec() - stIter->second.m_iLoginTime);

        m_stUserContainer.erase(iUin);
    }
    
	return (true);
}

void CLogicUser::OnUserLogin(int32_t iUin, int32_t iGroupID, int32_t iChannelID)
{
    auto pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
    if (pUserInfo)
    {
        // 通知跨服玩家信息
        CLogicGlobal::NotifyCrossUserBriefUpdate(pUserInfo, true);
    }

    // 通知排队服务器
    CLogicGlobal::NotifyQueueSvrUserLogin(iUin, true);

	CLogicBill::SaveUserLoginBill(iUin, iGroupID, iChannelID, 0, "UserLogin Bill");
}

void CLogicUser::OnUserLogout(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iOnlineTime)
{
	auto pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
	if (pUserInfo)
	{
		//用户登出，屏蔽列表清空
		pUserInfo->m_stOnlyInCacheData.m_setPingbiUser.clear();
        pUserInfo->m_stOnlyInCacheData.m_stLimitBossInvite.clear();

        //用户登出，通知被拜访清除拜访记录
        auto& stVisitedInfo = pUserInfo->m_stOnlyInCacheData.m_iVisitedInfo;
        if (0 != stVisitedInfo.m_iUid)
        {
            CLogicGlobal::NotifyCrossUserVisitorLogout(pUserInfo,stVisitedInfo);
            stVisitedInfo.m_iUid = 0;
        }

		//如果未被封号，更新在线时间
		if (!CLogicCacheCommon::IsBanAccount(pUserInfo))
		{
			pUserInfo->m_stUserInfoTable.AddTotalOnlineTime(iOnlineTime);

			pUserInfo->m_stUserInfoTable.SetLastLogout(CLogicCommon::GetSec());

			if (!pUserInfo->m_stFirstRechargeMap.Empty())
			{
				pUserInfo->m_stFirstRechargeMap.Begin()->second->SetLastLoginTime(CLogicCommon::GetSec());
			}
		}

        // 通知跨服玩家信息
        CLogicGlobal::NotifyCrossUserBriefUpdate(pUserInfo, false);

        //数据打点上报
        CDataReportOffline params;
        params.m_iRegisterDate = pUserInfo->m_stUserInfoTable.GetCreateTime();
        params.m_iOnlineTime = iOnlineTime;

        params.m_stCommon.m_strEven = "tga_offline";
        params.m_stCommon.m_strAccount = pUserInfo->m_strPid;
        params.m_stCommon.m_strUdid = pUserInfo->m_strDeviceID;
        params.m_stCommon.m_strNickName = pUserInfo->m_strNick;
        CMsgHeadCross stMsgHead;
        stMsgHead.m_byCmdType = enmCmdType_request;
        stMsgHead.m_usCmd = SERVER_CMD_DATA_REPORT_DEFINE::MSG_REPORT_OFFLINE;
        stMsgHead.m_iUin = pUserInfo->m_iUin;
        stMsgHead.m_nGroupID = pUserInfo->m_iGroupID;
        LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stMsgHead, params);
	}

    // 通知排队服务器
    CLogicGlobal::NotifyQueueSvrUserLogin(iUin, false, !CheckQueueTag(iUin));

	CLogicBill::SaveUserLoginBill(iUin, iGroupID, iChannelID, 1, "UserLogin Bill");
}

bool CLogicUser::IsUserOnline(int32_t iUin, int32_t iGroupID)
{
	auto stIter = m_stUserContainer.find(iUin);
	return (stIter != m_stUserContainer.end() && stIter->second.m_iGroupID == iGroupID);
}

void CLogicUser::UpdateUserOnlineTime()
{
	int32_t iNow = CLogicCommon::GetSec();
	for (auto &iter : m_stUserContainer)
	{
		LOGIC_STATISTICS_INS->AddPlayerOnlineTime(iter.first, iter.second.m_iGroupID, iNow - iter.second.m_iLastOnlineTime);
        iter.second.m_iLastOnlineTime = iNow;
	}
}

void CLogicUser::AddQueueTag(int32_t iUin)
{
    m_stUserQueueTag.insert(iUin);
}

bool CLogicUser::CheckQueueTag(int32_t iUin)
{
    return m_stUserQueueTag.erase(iUin) > 0;
}
