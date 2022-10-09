#include "common_utf8handle.h"
#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_talk_common.h"
#include "logic_config.h"
#include "logic_game_user.h"
#include "logic_game_cache_common.h"
#include "logic_game_global_frame.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_bill.h"

bool CLogicTalkChannelStrategy::CheckTalk(user_data_table_ptr_type pUser, CRequestTalk& stPara, std::string& strClientIP)
{
    if(pUser->m_stUserInfoTable.GetLevel() < CLogicConfig::GetGlobalConfig().m_iTalkLevelLimit)
    {
        SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_TEAM_LEVEL);
        return false;
    }

    if(CLogicCacheCommon::IsBanTalk(pUser))
    {
        SET_ERR_INFO(SERVER_ERRCODE::USER_IS_BAN_TALK);
        return false;
    }

    if(!CLogicTalk::FilterTalkContent(pUser, stPara.m_strTalkData))
    {
        return false;
    }

    if(!LOGIC_SERVICE_API_INS->GetCrossConnection()->IsConnected())
    {
        SET_ERR_INFO(SERVER_ERRCODE::MATCH_SERVER_NOT_CONN);
        return false;
    }

    return CheckTalkByChannel(pUser, stPara, strClientIP);
}

bool CLogicTalkChannelStrategy::SendToCross(user_data_table_ptr_type pUser, const CRequestTalk& stPara)
{
    pUser->m_stOnlyInCacheData.m_iLastWorldTalkTime = CLogicCommon::GetSec();

    // 仅自己可见
    if(m_bSelfTalk)
    {
        CPackTalkInfo stTalkInfo;
        stTalkInfo.m_stUserBrief = CLogicCacheCommon::GetUserBriefInfo(pUser);
        stTalkInfo.m_strTalkData = stPara.m_strTalkData;
        stTalkInfo.m_iTalkChannel = stPara.m_iTalkChannel;
        stTalkInfo.m_iTalkTime = CLogicCommon::GetSec();
        std::vector<int> stVec;
        HandleTalk(stTalkInfo, 0, 0, stVec);

        return true;
    }

    // 发送跨服
    CMsgHeadCross stMsgHead;
    stMsgHead.m_byCmdType = enmCmdType_request;
    stMsgHead.m_usCmd = MSG_LOGIC_USER_TALK;
    stMsgHead.m_iUin = pUser->m_iUin;
    stMsgHead.m_nGroupID = pUser->m_iGroupID;
    LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stMsgHead, stPara);

    return true;
}

void CLogicTalkChannelStrategy::HandleTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID, std::vector<int>& stIDVec)
{
    // 仅自己可见
    if(m_bSelfTalk)
    {
        CNotifyPushTalk stPushProto;
        stPushProto.m_stTalkInfo = stTalkInfo;
        LOGIC_CONNECTOR_SENDER->NotifyClientUser(stTalkInfo.m_stUserBrief.m_iGroupID, stTalkInfo.m_stUserBrief.m_iUid, MSG_LOGIC_PUSH_TALK, stPushProto);

        return;
    }

    // 发送聊天
    SendTalk(stTalkInfo, iDstUin, iDstGroupID, stIDVec);

    // 缓存聊天
    CacheTalk(stTalkInfo, iDstUin, iDstGroupID);
}

// 世界频道 ////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLogicTalkChannelWorldStrategy::CheckTalkByChannel(user_data_table_ptr_type pUser, const CRequestTalk& stPara, std::string& strClientIP)
{
    const int32_t iCalcLevel = pUser->m_stUserInfoTable.GetVipLevel() * pUser->m_stUserInfoTable.GetVipLevel() + pUser->m_stUserInfoTable.GetLevel();
    if(pUser->m_stUserInfoTable.GetLevel() < CLogicConfig::GetGlobalConfig().m_iBanIPTalkLevelLimit
        && stPara.m_strTalkData.size() >= CLogicConfigDefine::ESD_TalkLengthThreshold
        && CLogicConfig::GetZooLogicPlatformConfig().isBannedIp(strClientIP))
    {//被禁言ip60级之前的长聊天消息仅自己可见
        m_bSelfTalk = true;
    }
    else if(iCalcLevel < CLogicConfigDefine::ESD_TalkUserLevelThreshold
             && LOGIC_GLOBAL_FRAME_INS->IsRepeatTalk(pUser->m_iUin, pUser->m_iGroupID, stPara.m_strTalkData))
    {//低等级的长聊天消息仅自己可见
        m_bSelfTalk = true;
    }

    return true;
}

void CLogicTalkChannelWorldStrategy::SendTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID, std::vector<int>& stIDVec)
{
    CNotifyPushTalk stPushProto;
    stPushProto.m_stTalkInfo = stTalkInfo;

    for(auto stIter = CLogicUser::GetInstance().Begin(); stIter != CLogicUser::GetInstance().End(); ++stIter)
    {
        auto pDstInfo = CLogicCacheCommon::LoadUserData(stIter->second.m_iGroupID, stIter->first);
        if(pDstInfo && (pDstInfo->m_stOnlyInCacheData.m_setPingbiUser.find(stTalkInfo.m_stUserBrief.m_iUid) == pDstInfo->m_stOnlyInCacheData.m_setPingbiUser.end()))
        {
            LOGIC_CONNECTOR_SENDER->NotifyClientUser(stIter->second.m_iGroupID, stIter->first, MSG_LOGIC_PUSH_TALK, stPushProto);
        }
    }
}

void CLogicTalkChannelWorldStrategy::CacheTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID)
{
    LOGIC_GLOBAL_FRAME_INS->AddWorldTalkCache(stTalkInfo);

    auto pUserInfo = CLogicCacheCommon::GetInCacheUserData(stTalkInfo.m_stUserBrief.m_iGroupID, stTalkInfo.m_stUserBrief.m_iUid);
    if(pUserInfo)
    {
        if(stTalkInfo.m_strTalkData.size() > CLogicConfigDefine::ESD_TalkLengthThreshold && pUserInfo->m_stUserInfoTable.GetLevel() < CLogicConfigDefine::ESD_TalkUserLevelThreshold)
        {
            ++pUserInfo->m_stOnlyInCacheData.m_iTodayLongTalkCount;
            if(pUserInfo->m_stOnlyInCacheData.m_iTodayLongTalkCount == 10)
            {//低等级玩家每日的第10次长世界聊天消息报警
                LOGIC_SERVICE_API_INS->RequestServiceAlert("too_more_long_talk",
                                                           {{ "uin", std::to_string(pUserInfo->m_iUin) },
                                                             { "group_id", std::to_string(pUserInfo->m_iGroupID) },
                                                             { "talk", stTalkInfo.m_strTalkData }});
            }
        }
    }
}

// 公会频道 ////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLogicTalkChannelGuildStrategy::CheckTalkByChannel(user_data_table_ptr_type pUser, const CRequestTalk& stPara, std::string& strClientIP)
{
    if(pUser->m_stUserInfoTable.GetGuildID() <= 0)
    {
        SET_ERR_INFO(SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND);
        return false;
    }

    if(pUser->m_stUserInfoTable.GetLevel() < CLogicConfig::GetGlobalConfig().m_iBanIPTalkLevelLimit
        && stPara.m_strTalkData.size() >= CLogicConfigDefine::ESD_TalkLengthThreshold
        && CLogicConfig::GetZooLogicPlatformConfig().isBannedIp(strClientIP))
    {//被禁言ip60级之前的长聊天消息仅自己可见
        m_bSelfTalk = true;
    }

    return true;
}

void CLogicTalkChannelGuildStrategy::SendTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID, std::vector<int>& stIDVec)
{
    CNotifyPushTalk stPushProto;
    stPushProto.m_stTalkInfo = stTalkInfo;

    for(auto uin : stIDVec)
    {
        LOGIC_CONNECTOR_SENDER->NotifyClientUser(iDstGroupID, uin, MSG_LOGIC_PUSH_TALK, stPushProto);
    }
}

void CLogicTalkChannelGuildStrategy::CacheTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID)
{
    LOGIC_GLOBAL_FRAME_INS->AddGuildTalkCache(stTalkInfo, iDstUin);
}

// 私聊频道 ////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLogicTalkChannelPrivateStrategy::SendToCross(user_data_table_ptr_type pUser, const CRequestTalk& stPara)
{
    // 同服玩家直接发送
    if(CLogicConfig::GetSystemConfig().isSelfGroup(stPara.m_iDstGroupID))
    {
        CPackTalkInfo stTalkInfo;
        stTalkInfo.m_stUserBrief = CLogicCacheCommon::GetUserBriefInfo(pUser);
        stTalkInfo.m_strTalkData = stPara.m_strTalkData;
        stTalkInfo.m_iTalkChannel = stPara.m_iTalkChannel;
        stTalkInfo.m_iTalkTime = CLogicCommon::GetSec();
        stTalkInfo.m_iDstUin = stPara.m_iDstUin;

        std::vector<int> stVec;
        // 发送给对方
        HandleTalk(stTalkInfo, stPara.m_iDstUin, stPara.m_iDstGroupID, stVec);

        // 发送给自己
        HandleTalk(stTalkInfo, pUser->m_iUin, pUser->m_iGroupID, stVec);

        return true;
    }

    CLogicTalkChannelStrategy::SendToCross(pUser, stPara);

    return true;
}

bool CLogicTalkChannelPrivateStrategy::CheckTalkByChannel(user_data_table_ptr_type pUser, const CRequestTalk& stPara, std::string& strClientIP)
{
    if(stPara.m_iDstUin <= 0 || stPara.m_iDstGroupID <= 0 || stPara.m_iDstUin == pUser->m_iUin)
    {
        SET_ERR_INFO(SERVER_ERRCODE::PARAMETER_ERROR);
        return false;
    }

    if(pUser->m_stUserInfoTable.GetLevel() < CLogicConfig::GetGlobalConfig().m_iBanIPTalkLevelLimit
        && CLogicConfig::GetZooLogicPlatformConfig().isBannedIp(strClientIP))
    {//被禁言ip60级之前的私聊消息对方收不到
        SET_ERR_INFO(SERVER_ERRCODE::USER_IS_BAN_IP);
        return false;
    }

    return true;
}

void CLogicTalkChannelPrivateStrategy::SendTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID, std::vector<int>& stIDVec)
{
    auto pDstUser = CLogicCacheCommon::LoadUserData(iDstGroupID, iDstUin);
    if(!pDstUser || pDstUser->m_stOnlyInCacheData.m_setPingbiUser.find(stTalkInfo.m_stUserBrief.m_iUid) != pDstUser->m_stOnlyInCacheData.m_setPingbiUser.end())
    {
        return;
    }

    CNotifyPushTalk stPushProto;
    stPushProto.m_stTalkInfo = stTalkInfo;
    LOGIC_CONNECTOR_SENDER->NotifyClientUser(iDstGroupID, iDstUin, MSG_LOGIC_PUSH_TALK, stPushProto);
}

void CLogicTalkChannelPrivateStrategy::CacheTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID)
{
    auto pDstUser = CLogicCacheCommon::GetInCacheUserData(iDstGroupID, iDstUin);
    if(!pDstUser || pDstUser->m_stOnlyInCacheData.m_setPingbiUser.find(stTalkInfo.m_stUserBrief.m_iUid) != pDstUser->m_stOnlyInCacheData.m_setPingbiUser.end())
    {
        return;
    }

    int iCacheUin = (iDstUin == stTalkInfo.m_iDstUin ? stTalkInfo.m_stUserBrief.m_iUid : stTalkInfo.m_iDstUin);
    auto& stCacheUser = pDstUser->m_stOnlyInCacheData.m_stPrivateTalkCache[iCacheUin];
    stCacheUser.m_bHasRead = (iDstUin != stTalkInfo.m_iDstUin);
    stCacheUser.m_stTalkCacheList.emplace_back(stTalkInfo);
    if(stCacheUser.m_stTalkCacheList.size() >= CLogicConfigDefine::ESD_WorldTalkCacheCount)
    {
        stCacheUser.m_stTalkCacheList.pop_front();
    }

    if(pDstUser->m_stOnlyInCacheData.m_stPrivateTalkCache.size() > 100)
    {
        pDstUser->m_stOnlyInCacheData.m_stPrivateTalkCache.erase(pDstUser->m_stOnlyInCacheData.m_stPrivateTalkCache.begin());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLogicTalk::FilterTalkContent(user_data_table_ptr_type pUser, std::string& strTalkContent)
{
	int iTalkWordLimit = CLogicConfig::GetGlobalConfig().m_iTalkWordLimit;
	std::vector<uint32_t> stTalkContent;
	CUTF8Handle::Decode(strTalkContent, stTalkContent);

	if (iTalkWordLimit < (int)stTalkContent.size())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(TALK_WORD_COUNT_REACH_LIMIT);
		return (false);
	}

	int iLastWorldTalkTime = pUser->m_stOnlyInCacheData.m_iLastWorldTalkTime;
	if(CLogicCommon::GetSec() - iLastWorldTalkTime <= CLogicConfig::GetGlobalConfig().m_iMaxTalkInterval)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(TALK_TOO_FAST);
		return (false);
	}

	LOGIC_SERVICE_API_INS->FilterTalkContent(strTalkContent);

	return (true);
}

bool CLogicTalk::CheckUTF8ContentLength(const std::string& strContent, const int32_t iLength)
{
    std::vector<uint32_t> stTalkContent;
    CUTF8Handle::Decode(strContent, stTalkContent);

    if (iLength < (int)stTalkContent.size())
    {
        return false;
    }

    return true;
}

int CLogicTalk::SendWordBroadcastTalk(user_data_table_ptr_type pUserInfo, int32_t iTalkChannel, const std::string& strContent, const std::vector<int>& exParam)
{
    //
	return 0;
}

talk_strategy_sp CLogicTalk::CreateTalkChannelStrategy(int32_t iChannelType)
{
    talk_strategy_sp pStrategy = nullptr;
    switch (iChannelType)
    {
    case CRequestTalk::ETCT_World:
        pStrategy = std::make_shared<CLogicTalkChannelWorldStrategy>();
        break;
    case CRequestTalk::ETCT_Guild:
        pStrategy = std::make_shared<CLogicTalkChannelGuildStrategy>();
        break;
    case CRequestTalk::ETCT_Private:
        pStrategy = std::make_shared<CLogicTalkChannelPrivateStrategy>();
        break;
    default:
        return nullptr;
    }

    return pStrategy;
}
