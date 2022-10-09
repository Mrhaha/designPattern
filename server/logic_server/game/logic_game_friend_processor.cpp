#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_friend_protocol.h"
#include "logic_game_friend_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_item.h"
#include "logic_config.h"
#include "logic_game_rank_list.h"
#include "logic_game_publisher.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FRIEND_LIST, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FRIEND_APPLY_LIST, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_DEAL_FRIEND_APPLY, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ADD_FRIEND, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_DELETE_FRIEND, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SEND_FRIEND_ENERGY, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_FASET_SEND_FRIEND_ENERGY, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FRIEND_RECOMMEND, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SEARCH_FRIEND, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_NOTIFY_HAVE_FRIEND_APPLY, CLogicFriendProcessor)

REGISTER_CMD_FACTORY(MSG_CROSS_FRIEND_DEAL_APPLY_CROSS_CHECK, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_FRIEND_CROSS_DIRECT_ADD, CLogicFriendProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_FRIEND_CROSS_DIRECT_DEL, CLogicFriendProcessor)

CLogicFriendProcessor::CLogicFriendProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicFriendProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if (!GetCrossConnection()->IsConnected())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MATCH_SERVER_NOT_CONN);
    }

	bool bRet = false;
	switch (m_usCmd)
	{
	case MSG_LOGIC_GET_FRIEND_LIST:
		bRet = DoUserRunGetFriendList(stFormatData);
		break;
	case MSG_LOGIC_GET_FRIEND_APPLY_LIST:
		bRet = DoUserRunGetFriendApplyList(stFormatData);
		break;
	case MSG_LOGIC_DEAL_FRIEND_APPLY:
		bRet = DoUserRunDealFriendApply(stFormatData);
		break;
    case MSG_LOGIC_ADD_FRIEND:
        bRet = DoUserRunAddFriend(stFormatData);
        break;
    case MSG_LOGIC_DELETE_FRIEND:
        bRet = DoUserRunDeleteFriend(stFormatData);
        break;
    case MSG_LOGIC_SEND_FRIEND_ENERGY:
        bRet = DoUserRunSendFriendEnergy(stFormatData);
        break;
    case MSG_LOGIC_FASET_SEND_FRIEND_ENERGY:
        bRet = DoUserRunFastSendFriendEnergy(stFormatData);
        break;
    case MSG_LOGIC_GET_FRIEND_RECOMMEND:
        bRet = DoUserRunGetFriendRecommend(stFormatData);
        break;
	case MSG_LOGIC_SEARCH_FRIEND:
		bRet = DoUserRunSearchFriend(stFormatData);
		break;
	default:
		PROCESSOR_LOG_ERROR << "NOT FOUND PROCESSOR WITH CMD:" << m_usCmd << std::endl;
		break;
	}

	return bRet;
}

bool CLogicFriendProcessor::DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    bool bRet = false;

    switch (stMsgHead.m_usCmd)
    {
    case MSG_LOGIC_GET_FRIEND_LIST:
        bRet = DoServerRunGetFriendList(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GET_FRIEND_APPLY_LIST:
        bRet = DoServerRunGetFriendApplyList(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_NOTIFY_HAVE_FRIEND_APPLY:
        bRet = DoServerRunNotifyFriendApply(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_SEARCH_FRIEND:
        bRet = DoServerRunSearchFriend(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_DEAL_FRIEND_APPLY:
        bRet = DoServerRunDealFriendApply(stMsgHead, stFormatData);
        break;

    case MSG_CROSS_FRIEND_DEAL_APPLY_CROSS_CHECK:
        bRet = DoServerRunFriendDealApplyCrossCheck(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_FRIEND_CROSS_DIRECT_ADD:
        bRet = DoServerRunFriendCrossDirectAdd(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_FRIEND_CROSS_DIRECT_DEL:
        bRet = DoServerRunFriendCrossDirectDel(stMsgHead, stFormatData);
        break;
    default:
        break;
    }
    return bRet;
}

bool CLogicFriendProcessor::DoUserRunGetFriendList(const msgpack::unpacked& stFormatData)
{
    CRequestGetFriendList stReq;
    for(auto iter = m_pUserInfo->m_stFriendMap.Begin(); iter != m_pUserInfo->m_stFriendMap.End(); ++iter)
    {
        CRequestGetFriendList::user_key stKey;
        stKey.m_iUin = iter->first;
        stKey.m_iGroupID = iter->second->m_stData.m_iFriendGroupID;
        stReq.m_stUserVec.push_back(stKey);
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);
	return true;
}

bool CLogicFriendProcessor::DoUserRunGetFriendApplyList(const msgpack::unpacked& stFormatData)
{
    CRequestGetFriendApplyList stReq;
    for(auto iter = m_pUserInfo->m_stFriendApplyMap.Begin(); iter != m_pUserInfo->m_stFriendApplyMap.End(); ++iter)
    {
        CRequestGetFriendApplyList::user_key stKey;
        stKey.m_iUin = iter->first;
        stKey.m_iGroupID = iter->second->m_stData.m_iFriendGroupID;
        stKey.m_iApplyTime = iter->second->GetApplyTime();
        stReq.m_stUserVec.push_back(stKey);
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);
    return true;
}

bool CLogicFriendProcessor::DoUserRunDealFriendApply(const msgpack::unpacked& stFormatData)
{
    CRequestDealFriendApply stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return (false);

    auto iterApply = m_pUserInfo->m_stFriendApplyMap.Find(stReq.m_iDstUid);
    if(iterApply == m_pUserInfo->m_stFriendApplyMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FRIEND_APPLY_EXPIRED)
    }

    if(iterApply->second->m_stData.m_iFriendGroupID != stReq.m_iDstGroupID)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(!stReq.m_bIsAgree)
    {
        // 拒绝
        m_pUserInfo->m_stFriendApplyMap.Erase(iterApply);

        SendToClient(SERVER_ERRCODE::SUCCESS);
        return true;
    }
    else
    {
        // 判断好友上限
        if(int(m_pUserInfo->m_stFriendMap.Size()) >= CLogicConfig::GetFriendConfig().m_iMaxFriendCount)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FRIEND_IS_FULL)
        }

        // 本服玩家
        if(CLogicConfig::GetSystemConfig().isSelfGroup(stReq.m_iDstGroupID))
        {
            auto pDstUser = CLogicCacheCommon::LoadUserData(stReq.m_iDstGroupID, stReq.m_iDstUid);
            if(nullptr == pDstUser)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::USER_LEVEL_FORMAT_ERROR)
            }

            if(!AddFriend(pDstUser, m_iUin, m_iGroupID))
            {
                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }

            AddFriend(m_pUserInfo, stReq.m_iDstUid, stReq.m_iDstGroupID);

            SendToClient(SERVER_ERRCODE::SUCCESS);
            return true;
        }

        // 跨服
        CCrossFriendDealApplyCheck stCross;
        stCross.m_iDstUid = m_iUin;
        stCross.m_iDstGroupID = m_iGroupID;
        SendToCrossServer(enmCmdType_cross, MSG_CROSS_FRIEND_DEAL_APPLY_CROSS_CHECK, stCross, stReq.m_iDstUid, stReq.m_iDstGroupID);
    }

    return true;
}

bool CLogicFriendProcessor::DoUserRunAddFriend(const msgpack::unpacked& stFormatData)
{
	CRequestAddFriend stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iDstUin == m_iUin)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::CANT_ADD_SELF_FRIEND)
    }

    if(stParam.m_iDstUin <= 0 || stParam.m_iDstGroupID <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    // 判断好友上限
    if (int(m_pUserInfo->m_stFriendMap.Size()) >= CLogicConfig::GetFriendConfig().m_iMaxFriendCount)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FRIEND_IS_FULL)
    }

    if(m_pUserInfo->m_stFriendMap.Find(stParam.m_iDstUin) != m_pUserInfo->m_stFriendMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ALREADY_IS_FRIEND)
    }

    int iDstUin = stParam.m_iDstUin;
    int iDstGroupID = stParam.m_iDstGroupID;
    stParam.m_iDstUin = m_iUin;
    stParam.m_iDstGroupID = m_iGroupID;
    SendToCrossServer(enmCmdType_cross, MSG_LOGIC_NOTIFY_HAVE_FRIEND_APPLY, stParam, iDstUin, iDstGroupID);

    SendToClient(SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicFriendProcessor::DoUserRunDeleteFriend(const msgpack::unpacked& stFormatData)
{
    CRequestAddFriend stParam;
	if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    auto iter = m_pUserInfo->m_stFriendMap.Find(stParam.m_iDstUin);
    if(iter != m_pUserInfo->m_stFriendMap.End())
    {
        if(iter->second->m_stData.m_iFriendGroupID != stParam.m_iDstGroupID)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        m_pUserInfo->m_stFriendMap.Erase(iter);

        CRequestAddFriend stCross;
        stCross.m_iDstUin = m_iUin;
        stCross.m_iDstGroupID = m_iGroupID;
        SendToCrossServer(enmCmdType_cross, MSG_CROSS_FRIEND_CROSS_DIRECT_DEL, stCross, stParam.m_iDstUin, stParam.m_iDstGroupID);
    }

    SendToClient(SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicFriendProcessor::DoUserRunSendFriendEnergy(const msgpack::unpacked& stFormatData)
{
    /*
	CRequestSendFriendEnergy stParam;
	CResponseSendFriendEnergy stRsp;

	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	user_info_key_t stFriendKey;
	stFriendKey.m_stRoleIDDetail.m_iUin = stParam.m_iDstUid;
	stFriendKey.m_stRoleIDDetail.m_iGroupID = stParam.m_iDstGroupID;
	auto iter = m_pUserInfo->m_stFriendMap.Find(stFriendKey.m_ulRoleID);
	if (iter == m_pUserInfo->m_stFriendMap.End())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << FRIEND_NOT_EXSIT
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(FRIEND_NOT_EXSIT) << std::endl;

		SEND_ERRCODE_AND_RET(FRIEND_NOT_EXSIT);
	}

	if (m_pUserInfo->m_stUserInfoTable.GetAddFriendEnergyCount() >= CLogicConfig::GetFriendConfig().m_iDailyPraiseCount)
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << COUNT_NOT_ENOUGH
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(COUNT_NOT_ENOUGH) << std::endl;

		SEND_ERRCODE_AND_RET(COUNT_NOT_ENOUGH);
	}

	CLogicFriend::CheckAndRefreshFriend(*iter->second);

	if (iter->second->GetIsSendEnergy())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << ALREADY_SEND_FRIEND_ENERGY
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(ALREADY_SEND_FRIEND_ENERGY) << std::endl;

		SEND_ERRCODE_AND_RET(ALREADY_SEND_FRIEND_ENERGY);
	}

	const int32_t iIntimateLevel = CLogicConfig::GetFriendConfig().GetIntimateLevel(iter->second->GetIntimacy());
	const auto iterIntimacyConfig = CLogicConfig::GetFriendConfig().m_stIntimateLevelMap.find(iIntimateLevel);
	if (iterIntimacyConfig == CLogicConfig::GetFriendConfig().m_stIntimateLevelMap.end())
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << PARAMETER_ERROR
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR) << std::endl;

		SEND_ERRCODE_AND_RET(PARAMETER_ERROR);
	}

	iter->second->SetIsSendEnergy(true);
	m_pUserInfo->m_stUserInfoTable.AddAddFriendEnergyCount(1);

	CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_EnergyID, iterIntimacyConfig->second.m_iPraiseEnergy, m_usCmd);
	CLogicCacheCommon::AddFriendIntimacy(m_pUserInfo, stParam.m_iDstUid, stParam.m_iDstGroupID, iterIntimacyConfig->second.m_iPraiseIntimacy);

	logic_game_user_action_channel_data_t stData;
	stData.m_iTimes = 1;
	stData.m_iAction = CLogicGameChannelEnum::ACTION_SEND_ENERGY;
	LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

	PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;
	stRsp.m_iAddFriendEnergyCount = m_pUserInfo->m_stUserInfoTable.GetAddFriendEnergyCount();
	stRsp.m_stFriend = CLogicFriend::GetFriendInfoPack(m_pUserInfo, stParam.m_iDstUid, stParam.m_iDstGroupID);
	stRsp.m_iAddEnergyValue = iterIntimacyConfig->second.m_iPraiseEnergy;
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
*/
	return true;
}

bool cmp_by_value(const std::pair<uint64_t, int32_t>& lhs, const std::pair<uint64_t, int32_t>& rhs)
{
	return lhs.second > rhs.second;
}

bool CLogicFriendProcessor::DoUserRunFastSendFriendEnergy(const msgpack::unpacked& stFormatData)
{
    /*
	CResponseFastSendFriendEnergy stRsp;
	auto iAddCount = 0;
	std::vector<std::pair<uint64_t, int32_t>> vecFriend;
	for (auto iter = m_pUserInfo->m_stFriendMap.Begin(); iter != m_pUserInfo->m_stFriendMap.End(); ++iter)
	{
		CLogicFriend::CheckAndRefreshFriend(*iter->second);
		if (!iter->second->GetIsSendEnergy())
		{
			vecFriend.emplace_back(iter->first, iter->second->GetIntimacy());
		}
		
	}
	stRsp.m_iAddEnergyValue = 0;
	std::sort(vecFriend.begin(), vecFriend.end(), cmp_by_value);
	for (auto iter = vecFriend.begin(); iter != vecFriend.end(); ++iter)
	{
		auto iterFriend = m_pUserInfo->m_stFriendMap.Find(iter->first);
		const int32_t iIntimateLevel = CLogicConfig::GetFriendConfig().GetIntimateLevel(iterFriend->second->GetIntimacy());
		const auto iterIntimacyConfig = CLogicConfig::GetFriendConfig().m_stIntimateLevelMap.find(iIntimateLevel);
		if (iterIntimacyConfig == CLogicConfig::GetFriendConfig().m_stIntimateLevelMap.end())
		{
			continue;
		}

		if (m_pUserInfo->m_stUserInfoTable.GetAddFriendEnergyCount() >= CLogicConfig::GetFriendConfig().m_iDailyPraiseCount)
		{
			break;
		}

		iterFriend->second->SetIsSendEnergy(true);
		m_pUserInfo->m_stUserInfoTable.AddAddFriendEnergyCount(1);
		++iAddCount;

		stRsp.m_iAddEnergyValue += iterIntimacyConfig->second.m_iPraiseEnergy;
		CLogicCacheCommon::AddFriendIntimacy(m_pUserInfo, iterFriend->second->m_stData.m_iFriendUid, iterFriend->second->m_stData.m_iFriendGroupID, iterIntimacyConfig->second.m_iPraiseIntimacy);

		stRsp.m_vtDstUser.push_back(CLogicFriend::GetFriendInfoPack(m_pUserInfo, iterFriend->second->m_stData.m_iFriendUid, iterFriend->second->m_stData.m_iFriendGroupID));
	}

	CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_EnergyID, stRsp.m_iAddEnergyValue, m_usCmd);

	logic_game_user_action_channel_data_t stData;
	stData.m_iTimes = iAddCount;
	stData.m_iAction = CLogicGameChannelEnum::ACTION_SEND_ENERGY;
	LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

	PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;
	
	stRsp.m_iAddFriendEnergyCount = m_pUserInfo->m_stUserInfoTable.GetAddFriendEnergyCount();
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
*/
	return true;
}

bool CLogicFriendProcessor::DoUserRunGetFriendRecommend(const msgpack::unpacked& stFormatData)
{
    CResponseGetFriendRecommend stRsp;

	int32_t iTotalSize = CLogicUser::GetInstance().GetUserNum();
	if (iTotalSize > 0)
	{
		for (auto iter = CLogicUser::GetInstance().Begin(); iter != CLogicUser::GetInstance().End(); ++iter)
		{
            if (stRsp.m_stRecommendVec.size() >= 100)
                break;

            if(iter->first == m_pUserInfo->m_iUin)
                continue;

			const auto pUser = CLogicCacheCommon::GetInCacheUserData(iter->second.m_iGroupID, iter->first);
			if (pUser && pUser->m_stUserInfoTable.GetLevel() > 10)
			{
				if(m_pUserInfo->m_stFriendMap.Find(iter->first) == m_pUserInfo->m_stFriendMap.End())
				{
					stRsp.m_stRecommendVec.push_back(CLogicCacheCommon::GetUserBriefInfo(pUser));
				}
			}
		}
	}

	std::random_shuffle(stRsp.m_stRecommendVec.begin(), stRsp.m_stRecommendVec.end());

	if (stRsp.m_stRecommendVec.size() > 15)
	{
		stRsp.m_stRecommendVec.resize(15);
	}

	PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

	return true;
}

bool CLogicFriendProcessor::DoUserRunSearchFriend(const msgpack::unpacked& stFormatData)
{
	CRequestSearchFriend stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    bool is_search_uid = std::find_if_not(stParam.m_strSearch.begin(), stParam.m_strSearch.end(),
                                          [](decltype(stParam.m_strSearch.begin())::value_type arg)
                                          { return arg >= '0' && arg <= '9';}) == stParam.m_strSearch.end();

	char szBuffer[1024];
    std::string strEscapeSearch = CLogicSQLTableRecordBase::GetMysqlConn()->escapeString(stParam.m_strSearch);
    if (is_search_uid)
        snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM server_nick_t WHERE uid=%s;", strEscapeSearch.c_str());
    else
        snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM server_nick_t WHERE nick=\"%s\";", strEscapeSearch.c_str());

    CRequestAddFriend stCross;
    taf::TC_Mysql::MysqlData stData;
    try {
        stData = CLogicSQLTableRecordBase::GetMysqlConn()->queryRecord(szBuffer);
        if(stData.size() > 0)
        {
            stCross.m_iDstUin = ::atoi(stData[0]["uid"].c_str());
            stCross.m_iDstGroupID = ::atoi(stData[0]["group_id"].c_str());
        }
    }
    catch (const taf::TC_Mysql_Exception& stEx) {}

    if(stCross.m_iDstUin <= 0 || stCross.m_iDstGroupID <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::USER_ROLE_NOT_EXIST)
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stCross);
	return true;
}

//////////////////////////////////////////// Server Run ///////////////////////////////////////////////////
bool CLogicFriendProcessor::DoServerRunGetFriendList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGetFriendList stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicFriendProcessor::DoServerRunGetFriendApplyList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGetFriendApplyList stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicFriendProcessor::DoServerRunNotifyFriendApply(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CRequestAddFriend stReq;
    if(!DecodeMsgPack(stFormatData, stReq))
    {
        return false;
    }

    AddFriendApply(stReq.m_iDstUin, stReq.m_iDstGroupID);

    return true;
}

bool CLogicFriendProcessor::DoServerRunSearchFriend(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseSearchFriend stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicFriendProcessor::DoServerRunDealFriendApply(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossFriendDealApplyCheck stCross;
    if(!DecodeMsgPack(stFormatData, stCross)) return false;

    if(stCross.m_iRetCode != SERVER_ERRCODE::SUCCESS)
    {
        SEND_ERRCODE_AND_RET(stCross.m_iRetCode)
    }

    AddFriend(m_pUserInfo, stCross.m_iDstUid, stCross.m_iDstGroupID);

    SendToClient(SERVER_ERRCODE::SUCCESS);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLogicFriendProcessor::DoServerRunFriendDealApplyCrossCheck(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossFriendDealApplyCheck stCross;
    if(!DecodeMsgPack(stFormatData, stCross)) return false;

    if(!AddFriend(m_pUserInfo, stCross.m_iDstUid, stCross.m_iDstGroupID))
    {
        stCross.m_iRetCode = CLogicError::GetErrCode();
    }

    int iDstUin = stCross.m_iDstUid;
    int iDstGroupID = stCross.m_iDstGroupID;
    stCross.m_iDstUid = m_iUin;
    stCross.m_iDstGroupID = m_iGroupID;
    SendToCrossServer(enmCmdType_cross, MSG_LOGIC_DEAL_FRIEND_APPLY, stCross, iDstUin, iDstGroupID);
    return true;
}

bool CLogicFriendProcessor::DoServerRunFriendCrossDirectAdd(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossFriendDealApplyCheck stCross;
    if(!DecodeMsgPack(stFormatData, stCross)) return false;

    AddFriend(m_pUserInfo, stCross.m_iDstUid, stCross.m_iDstGroupID);

    return true;
}

bool CLogicFriendProcessor::DoServerRunFriendCrossDirectDel(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CRequestAddFriend stCross;
    if(!DecodeMsgPack(stFormatData, stCross)) return false;

    m_pUserInfo->m_stFriendMap.Erase(stCross.m_iDstUin);

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CLogicFriendProcessor::AddFriendApply(int iFriendUin, int iFriendGroupID)
{
    if(iFriendUin == m_iUin)
    {
        SET_ERR_INFO(SERVER_ERRCODE::CANT_ADD_SELF_FRIEND);
        return false;
    }

    if(m_pUserInfo->m_stFriendMap.Find(iFriendUin) != m_pUserInfo->m_stFriendMap.End())
    {
        // 发送申请通过通知
        CCrossFriendDealApplyCheck stCross;
        stCross.m_iDstUid = m_iUin;
        stCross.m_iDstGroupID = m_iGroupID;
        SendToCrossServer(enmCmdType_cross, MSG_CROSS_FRIEND_CROSS_DIRECT_ADD, stCross, iFriendUin, iFriendGroupID);

        return true;
    }

    auto iterApply = m_pUserInfo->m_stFriendApplyMap.Find(iFriendUin);
    if(iterApply != m_pUserInfo->m_stFriendApplyMap.End())
    {
        iterApply->second->SetApplyTime(CLogicCommon::GetSec());
        return true;
    }

    // 申请列表是否已满
    if(m_pUserInfo->m_stFriendApplyMap.Size() >= CLogicConfigFriendParser::E_MaxFriendApplyCount)
    {
        std::vector<std::pair<int32_t, int32_t>> stApply;
        for (auto iter = m_pUserInfo->m_stFriendApplyMap.Begin(); iter != m_pUserInfo->m_stFriendApplyMap.End(); ++iter)
        {
            stApply.emplace_back(iter->first, iter->second->GetApplyTime());
        }

        std::sort(stApply.begin(), stApply.end(),
                  [](const decltype(stApply.begin())::value_type& a, const decltype(stApply.begin())::value_type& b) {return a.second > b.second;});

        while (stApply.size() >= CLogicConfigFriendParser::E_MaxFriendApplyCount)
        {
            auto apply = stApply.back();
            stApply.pop_back();
            m_pUserInfo->m_stFriendApplyMap.Erase(apply.first);
        }
    }

    TUserFriendApplyTableValueType stValue;
    stValue.m_iUid = m_iUin;
    stValue.m_iGroupID = m_iGroupID;
    stValue.m_iFriendUid = iFriendUin;
    stValue.m_iFriendGroupID = iFriendGroupID;
    stValue.m_iApplyTime = CLogicCommon::GetSec();
    m_pUserInfo->m_stFriendApplyMap.Insert(std::make_pair(iFriendUin, stValue));

    CResponseBody notify;
    GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, MSG_LOGIC_NOTIFY_HAVE_FRIEND_APPLY, notify);
    return true;
}

bool CLogicFriendProcessor::AddFriend(user_data_table_ptr_type pUser, int iFriendUin, int iFriendGroupID)
{
    if(pUser->m_iUin == iFriendUin)
    {
        SET_ERR_INFO(SERVER_ERRCODE::CANT_ADD_SELF_FRIEND);
        return false;
    }

    auto iterDst = pUser->m_stFriendMap.Find(iFriendUin);
    if (iterDst != pUser->m_stFriendMap.End())
    {
        return true;
    }

    if (pUser->m_stFriendMap.Size() >= uint32_t(CLogicConfig::GetFriendConfig().m_iMaxFriendCount))
    {
        SET_ERR_INFO(SERVER_ERRCODE::DST_FRIEND_IS_FULL);
        return false;
    }

    TUserFriendTableValueType stValue;
    stValue.m_iUid = pUser->m_iUin;
    stValue.m_iGroupID = pUser->m_iGroupID;
    stValue.m_iFriendUid = iFriendUin;
    stValue.m_iFriendGroupID = iFriendGroupID;
    auto stRet = pUser->m_stFriendMap.Insert(std::make_pair(iFriendUin, stValue));
    if(!stRet.second)
    {
        SET_ERR_INFO(SERVER_ERRCODE::INTERN_ERROR);
        return false;
    }

    // 删除申请
    pUser->m_stFriendApplyMap.Erase(iFriendUin);

    return true;
}
