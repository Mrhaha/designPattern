#include "common_datetime.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_errcode.h"
#include "server_cross_protocol.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_cross_team_war_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_cache_card_operation.h"
#include "logic_config.h"
#include "logic_game_global.h"
#include "logic_game_cache_common.h"
#include "logic_game_talk_common.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_publisher.h"
#include "logic_game_admin_processor.h"
#include "logic_game_common_publisher.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_BANQUET_INVITE, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_CREATE_BANQUET_TEAM, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_JOIN_BANQUET_TEAM, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GET_BANQUET_INFO, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_BANQUET_FINISH_NOTIFY, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_BANQUET_INFO_NOTIFY, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_BANQUET_UPDATE_LOGIN, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_BANQUET_INVITE_LIST, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_BANQUET_GET_FRIEND_OPEN, CLogicCrossTeamWarProcessor)




REGISTER_CMD_FACTORY(MSG_CROSS_ADMIN_MODIFY_GUILD, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_ADMIN_DISMISS_GUILD, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_ADMIN_SET_GUILD_CHIEF, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_ADMIN_CHANGE_GUILD_NAME, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_BANQUET_INVITE_NOTIFY, CLogicCrossTeamWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_BANQUET_QUIT_TEAM, CLogicCrossTeamWarProcessor)


CLogicCrossTeamWarProcessor::CLogicCrossTeamWarProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicCrossTeamWarProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	bool bRet = false;

	if (!GetCrossConnection()->IsConnected())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MATCH_SERVER_NOT_CONN);
	}

	switch (m_usCmd)
	{
    case MSG_CROSS_CREATE_BANQUET_TEAM:
        bRet = DoUserRunCreateBanquetTeam(stFormatData);
        break;
    case MSG_CROSS_JOIN_BANQUET_TEAM:
        bRet = DoUserRunJoinBanquetTeam(stFormatData);
        break;
    case MSG_CROSS_GET_BANQUET_INFO:
        bRet = DoUserRunGetBanquetInfo(stFormatData);
        break;
    case MSG_LOGIC_BANQUET_INVITE:
        bRet = DoUserRunBanquetInvite(stFormatData);
        break;
    case MSG_LOGIC_GET_BANQUET_INVITE_LIST:
        bRet = DoUserRunGetBanquetInviteList(stFormatData);
        break;
    case MSG_CROSS_BANQUET_QUIT_TEAM:
        bRet = DoUserRunQuitBanquetTeam(stFormatData);
        break;
    case MSG_CROSS_BANQUET_GET_FRIEND_OPEN:
        bRet = DoUserRunGetBanquetOpenTeam(stFormatData);
        break;
	default:
		break;
	}

	return bRet;
}

bool CLogicCrossTeamWarProcessor::DoUserRunCreateBanquetTeam(const msgpack::unpacked& stFormatData)
{
    CRequestCreateBanquetTeam stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    if(m_pUserInfo->m_stUserInfoTable.GetBanquetAllTeamID().m_iCreateId > 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BANQUET_IN_PROGRESS)
    }

    if(stReq.m_iShowCardID > 0)
    {
        if(m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iShowCardID) == m_pUserInfo->m_stHeroCardTableMap.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED)
        }
    }

    auto pConfig = CLogicConfig::GetHomeFuncConfig().GetBanquetItemConfig(stReq.m_iItemID);
    if(!pConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, stReq.m_iItemID) < 1)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM)
    }

    auto iterHome = m_pUserInfo->m_stHomeMap.Find(stReq.m_iHomeID);
    if(iterHome == m_pUserInfo->m_stHomeMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_NONEXISTENT)
    }

    auto iterGoods = m_pUserInfo->m_stHomeGoodsMap[stReq.m_iHomeID].Find(stReq.m_iDiningTableIndex);
    if(iterGoods == m_pUserInfo->m_stHomeGoodsMap[stReq.m_iHomeID].End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    auto pGoodsConfig = CLogicConfig::GetHomeGoodsConfig().GetHomeGoodsConfig(iterGoods->second->GetGoodsID());
    if(!pGoodsConfig || pGoodsConfig->m_iFuncType != CLogicConfigDefine::EHFGT_DiningTable)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(stReq.m_cPos < 1 || stReq.m_cPos > pGoodsConfig->m_iFuncPara)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    CCrossRequestCreateBanquetTeam stCross;
    stCross.m_iHomeID = stReq.m_iHomeID;
    stCross.m_iItemID = stReq.m_iItemID;
    stCross.m_iDiningTableIndex = stReq.m_iDiningTableIndex;
    stCross.m_iGuestNum = pGoodsConfig->m_iFuncPara;
    stCross.m_iBanquetTime = pConfig->m_iBanquetTime;
    stCross.m_iRewardTime = pConfig->m_iRewardTime;
    stCross.m_iShowCardID = stReq.m_iShowCardID;
    stCross.m_stBrief = CLogicCacheCommon::GetUserBriefInfo(m_pUserInfo);
    stCross.m_bOpen = stReq.m_bOpen;
    stCross.m_cPos = stReq.m_cPos;
    stCross.m_iSkinID = stReq.m_iSkinID;
    SendToCrossServer(enmCmdType_request, m_usCmd, stCross);

    auto itemConfig = CLogicConfig::GetItemConfig().GetConfig(stReq.m_iItemID);
    if (itemConfig != nullptr)
    {
        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_CreateBanquetUseItem;
        stEventData.m_iPara = itemConfig->m_cGrade;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
    }

    return true;
}

bool CLogicCrossTeamWarProcessor::DoUserRunJoinBanquetTeam(const msgpack::unpacked& stFormatData)
{
    CRequestJoinBanquetTeam stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    if(m_pUserInfo->m_stUserInfoTable.GetBanquetAllTeamID().m_iJoinId > 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::BANQUET_IN_PROGRESS)
    }

    if(stReq.m_iShowCardID > 0)
    {
        if(m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iShowCardID) == m_pUserInfo->m_stHeroCardTableMap.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED)
        }
    }

    if(stReq.m_cPos < 1)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    //如果参加的是被邀请的宴会清除内存邀请信息
    for(auto& inviteInfo  : m_pUserInfo->m_stOnlyInCacheData.m_stBanquetInviteMap)
    {
        if (inviteInfo.first == stReq.m_iTeamID)
        {
            m_pUserInfo->m_stOnlyInCacheData.m_stBanquetInviteMap.erase(inviteInfo.first);
            break;
        }
    }

    stReq.m_stBrief = CLogicCacheCommon::GetUserBriefInfo(m_pUserInfo);
    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);
    return true;
}

bool CLogicCrossTeamWarProcessor::DoUserRunGetBanquetInfo(const msgpack::unpacked& stFormatData)
{
    CRequestGetBanquetInfo stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);
    return true;
}

bool CLogicCrossTeamWarProcessor::DoUserRunBanquetInvite(const msgpack::unpacked& stFormatData)
{
    CRequestBanquetInvite stParam;
    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_stUinMap.empty())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(stParam.m_stUinMap.size() > 50)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INVITE_TOO_MANY)
    }

    for(auto& iter : stParam.m_stUinMap)
    {
        if(m_pUserInfo->m_stFriendMap.Find(iter.first) == m_pUserInfo->m_stFriendMap.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }
    }

    auto iterHome = m_pUserInfo->m_stHomeMap.Find(stParam.m_iHomeID);
    if(iterHome == m_pUserInfo->m_stHomeMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HOME_NONEXISTENT)
    }

    CCrossBanquetInvite stCross;
    stCross.m_iHomeID = stParam.m_iHomeID;
    stCross.m_stUinMap = stParam.m_stUinMap;
    stCross.m_stBrief = CLogicCacheCommon::GetUserBriefInfo(m_pUserInfo);
    SendToCrossServer(enmCmdType_request, m_usCmd, stCross);

    return true;
}

bool CLogicCrossTeamWarProcessor::DoUserRunGetBanquetInviteList(const msgpack::unpacked& stFormatData)
{
    CResponseGetBanquetInviteList stRsp;

    auto& stMap = m_pUserInfo->m_stOnlyInCacheData.m_stBanquetInviteMap;
    for(auto iter = stMap.begin(); iter != stMap.end(); )
    {
        if(iter->second.m_iEndTime <= CLogicCommon::GetSec() || iter->first == m_pUserInfo->m_stUserInfoTable.GetBanquetAllTeamID().m_iJoinId)
            stMap.erase(iter++);
        else
            ++iter;
    }

    CResponseGetBanquetInviteList::invite_data stInfo;
    for(auto iter : stMap)
    {
        stInfo.m_iTeamID = iter.first;
        stInfo.m_stOwnerBrief = iter.second.m_stBrief;
        stInfo.m_iHomeID = iter.second.m_iHomeID;
        stInfo.m_iItemID = iter.second.m_iItemID;
        stInfo.m_iEndTime = iter.second.m_iEndTime;
        stRsp.m_stInviteVec.push_back(stInfo);
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicCrossTeamWarProcessor::DoUserRunQuitBanquetTeam(const msgpack::unpacked& stFormatData) {

    CRequestBanquetQuitTeam stParam;

    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iTeamId != m_pUserInfo->m_stUserInfoTable.GetBanquetAllTeamID().m_iJoinId)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_IN_TEAM)
    }

    CCrossRequestBanquetQuitTeam stCross;
    stCross.m_iTeamId = stParam.m_iTeamId;
    SendToCrossServer(enmCmdType_request, m_usCmd, stCross);

    return true;
}

bool CLogicCrossTeamWarProcessor::DoUserRunGetBanquetOpenTeam(const msgpack::unpacked& stFormatData) {

    CCrossRequestFriendBanquet stCross;
    for(auto iter = m_pUserInfo->m_stFriendMap.Begin(); iter != m_pUserInfo->m_stFriendMap.End(); ++iter)
    {
        CCrossRequestFriendBanquet::user_key stKey;
        stKey.m_iUin = iter->first;
        stKey.m_iGroupID = iter->second->m_stData.m_iFriendGroupID;
        stCross.m_stUserVec.push_back(stKey);
    }
    SendToCrossServer(enmCmdType_request, m_usCmd, stCross);
    return true;
}



bool CLogicCrossTeamWarProcessor::DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
	bool bRet = false;
	switch (stMsgHead.m_usCmd)
	{
    case MSG_CROSS_CREATE_BANQUET_TEAM:
        bRet = DoServerRunCreateBanquetTeam(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GET_BANQUET_INFO:
        bRet = DoServerRunGetBanquetInfo(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_BANQUET_FINISH_NOTIFY:
        bRet = DoServerRunBanquetFinishNotify(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_BANQUET_INFO_NOTIFY:
        bRet = DoServerRunBanquetInfoNotify(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_BANQUET_UPDATE_LOGIN:
        bRet = DoServerRunBanquetUpdateLogin(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_BANQUET_INVITE_NOTIFY:
        bRet = DoServerRunBanquetInviteNotify(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_BANQUET_INVITE:
        bRet = DoServerRunBanquetInvite(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_BANQUET_QUIT_TEAM:
        bRet = DoServerRunBanquetQuitTeam(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_JOIN_BANQUET_TEAM:
        bRet = DoServerRunJoinBanquetTeam(stMsgHead,stFormatData);
        break;
    case MSG_CROSS_BANQUET_GET_FRIEND_OPEN:
        bRet = DoServerRunGetBanquetOpenTeam(stMsgHead,stFormatData);
        break;
    case MSG_CROSS_ADMIN_MODIFY_GUILD:
        bRet = DoServerRunAdminModifyGuild(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_ADMIN_DISMISS_GUILD:
        bRet = DoServerRunAdminDismissGuild(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_ADMIN_SET_GUILD_CHIEF:
        bRet = DoServerRunAdminSetGuildChief(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_ADMIN_CHANGE_GUILD_NAME:
        bRet = DoServerRunAdminChangeGuildName(stMsgHead, stFormatData);
        break;

	default:
		break;
	}

	return bRet;
}

bool CLogicCrossTeamWarProcessor::DoServerRunCreateBanquetTeam(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossResponseCreateBanquetTeam stCrossRsp;
    if(!DecodeMsgPack(stFormatData, stCrossRsp)) return (false);

    // 特殊判断，如果消耗失败，只记录日志，不返回
    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, stCrossRsp.m_iItemID, 1, m_usCmd))
    {
        PROCESSOR_LOG_ERROR << "Consume error! ERRCODE:" << CLogicError::GetErrCode() << "|MSG:" << CLogicError::GetErrMsg() << std::endl;
    }

    auto stCurrTeamIDs = m_pUserInfo->m_stUserInfoTable.GetBanquetAllTeamID();
    stCurrTeamIDs.m_iCreateId = stCrossRsp.m_uiTeamID;
    m_pUserInfo->m_stUserInfoTable.SetBanquetAllTeamID(stCurrTeamIDs);

    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_CreateBanquet;
    stEventData.m_iCmd = m_usCmd;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
    SendSuccessCacheItemToClient();
    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunGetBanquetInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CNotifyBanquetInfo stRsp;
    if(!DecodeMsgPack(stFormatData, stRsp)) return (false);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunBanquetFinishNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossNotifyBanquetReward stNotify;
    if(!DecodeMsgPack(stFormatData, stNotify)) return (false);

    bool bOwner = (m_pUserInfo->m_stUserInfoTable.GetBanquetAllTeamID().m_iCreateId == stNotify.m_iEndTeamID);
    auto stTeamIDs = m_pUserInfo->m_stUserInfoTable.GetBanquetAllTeamID();
    stTeamIDs.m_iCreateId = stNotify.m_iCreateID;
    stTeamIDs.m_iJoinId = stNotify.m_iJoinID;
    m_pUserInfo->m_stUserInfoTable.SetBanquetAllTeamID(stTeamIDs);

    auto& stConfig = CLogicConfig::GetHomeFuncConfig();
    auto pConfig = stConfig.GetBanquetItemConfig(stNotify.m_iItemID);
    if(pConfig)
    {
        PROCESSOR_LOG_DEBUG << "Banquet Reward Notify|ITEM_ID:" << stNotify.m_iItemID << std::endl;

        const std::string& content = bOwner ? stConfig.m_strBanquetMailContent : stConfig.m_strBanquetMailOtherContent;

        CLogicGlobal::AddAsyncUserMail(m_iUin, m_iGroupID, stConfig.m_strBanquetMailSender,
                                       stConfig.m_strBanquetMailTitle, content,
                                       (bOwner ? pConfig->m_stOwnerRewards : pConfig->m_stRewards), m_strCmdName);
    }

    CNotifyBanquetUpdateLogin notify;
    notify.m_iJoinId = stNotify.m_iJoinID;
    notify.m_iCreateId = stNotify.m_iCreateID;
    SendToClient(notify, SERVER_ERRCODE::SUCCESS);

    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunBanquetInfoNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CNotifyBanquetInfo stNotify;
    if(!DecodeMsgPack(stFormatData, stNotify)) return (false);

    auto stTeamIDs = m_pUserInfo->m_stUserInfoTable.GetBanquetAllTeamID();
    if (stNotify.m_stBanquetInfo.m_iUin == m_iUin) {//主办方
        stTeamIDs.m_iCreateId = stNotify.m_stBanquetInfo.m_iTeamID;
    } else {
        stTeamIDs.m_iJoinId = stNotify.m_stBanquetInfo.m_iTeamID;
    }

    SendToClient(stNotify, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunBanquetUpdateLogin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CNotifyBanquetUpdateLogin stNotify;
    if(!DecodeMsgPack(stFormatData, stNotify)) return (false);

    CPackBanquetTeamIds stTeamIDs;
    stTeamIDs.m_iCreateId = stNotify.m_iCreateId;
    stTeamIDs.m_iJoinId = stNotify.m_iJoinId;
    m_pUserInfo->m_stUserInfoTable.SetBanquetAllTeamID(stTeamIDs);

    SendToClient(stNotify, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunBanquetInviteNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossNotifyBanquetInvite stNotify;
    if(!DecodeMsgPack(stFormatData, stNotify)) return (false);

    if(m_pUserInfo->m_stOnlyInCacheData.m_stBanquetInviteMap.size() >= 20)
    {
        m_pUserInfo->m_stOnlyInCacheData.m_stBanquetInviteMap.erase(m_pUserInfo->m_stOnlyInCacheData.m_stBanquetInviteMap.begin());
    }

    auto& stInvite = m_pUserInfo->m_stOnlyInCacheData.m_stBanquetInviteMap[stNotify.m_iTeamID];
    stInvite.m_stBrief = stNotify.m_stOwnerBrief;
    stInvite.m_iHomeID = stNotify.m_iHomeID;
    stInvite.m_iItemID = stNotify.m_iItemID;
    stInvite.m_iEndTime = stNotify.m_iEndTime;

    SendToClient(stNotify, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunBanquetInvite(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseBody stRsp;
    if(!DecodeMsgPack(stFormatData, stRsp)) return (false);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunBanquetQuitTeam(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossResponseQuitBanquetTeam stRsp;
    if(!DecodeMsgPack(stFormatData, stRsp)) return (false);

    auto teamID = m_pUserInfo->m_stUserInfoTable.GetBanquetAllTeamID();
    teamID.m_iJoinId = 0;
    m_pUserInfo->m_stUserInfoTable.SetBanquetAllTeamID(teamID);
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunJoinBanquetTeam(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseJoinBanquetTeam stCrossRsp;
    if(!DecodeMsgPack(stFormatData, stCrossRsp)) return (false);

    auto stCurrTeamIDs = m_pUserInfo->m_stUserInfoTable.GetBanquetAllTeamID();
    stCurrTeamIDs.m_iJoinId = stCrossRsp.m_iTeamID;
    m_pUserInfo->m_stUserInfoTable.SetBanquetAllTeamID(stCurrTeamIDs);

    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_JoinFriendBanquet;
    stEventData.m_iCmd = m_usCmd;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunGetBanquetOpenTeam(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossResponseOpenBanquetTeam stRsp;
    if(!DecodeMsgPack(stFormatData, stRsp)) return (false);
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}


bool CLogicCrossTeamWarProcessor::DoServerRunAdminModifyGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossResponseModifyGuild stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    CLogicAdminProcessor::ResponseMessage(stRsp.m_iFd, MSG_ADMIN_MODIFY_GUILD, stRsp);

    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunAdminDismissGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossResponseModifyGuild stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    CLogicAdminProcessor::ResponseMessage(stRsp.m_iFd, MSG_ADMIN_DISMISS_GUILD, stRsp);

    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunAdminSetGuildChief(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossResponseModifyGuild stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    CLogicAdminProcessor::ResponseMessage(stRsp.m_iFd, MSG_ADMIN_SET_GUILD_CHIEF, stRsp);

    return true;
}

bool CLogicCrossTeamWarProcessor::DoServerRunAdminChangeGuildName(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossResponseModifyGuild stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    CLogicAdminProcessor::ResponseMessage(stRsp.m_iFd, MSG_ADMIN_CHANGE_GUILD_NAME, stRsp);

    return true;
}
