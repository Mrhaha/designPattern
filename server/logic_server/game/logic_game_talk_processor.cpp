#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_tactics_protocol.h"
#include "server_cross_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_game_talk_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_global_frame.h"
#include "logic_game_cache_common.h"
#include "logic_game_talk_common.h"
#include "logic_game_daily_task_system.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_USER_TALK, CLogicTalkProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_HIDE_USER_TALK_REQ, CLogicTalkProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_JUBAO_USER_REQ, CLogicTalkProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_USER_TALK_NOTIFY, CLogicTalkProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_READ_PRIVATE_TALK_REQ, CLogicTalkProcessor)

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_AVATAR_INFO, CLogicAvatarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SET_AVATAR_ICON, CLogicAvatarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SET_AVATAR_BORDER, CLogicAvatarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ADD_BGM, CLogicAvatarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_DEL_BGM, CLogicAvatarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SET_BGM_LIST, CLogicAvatarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SET_THEME_SKIN, CLogicAvatarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_BADGE_LIST, CLogicAvatarProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SET_BADGE_LIST, CLogicAvatarProcessor)





CLogicTalkProcessor::CLogicTalkProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicTalkProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	switch ((int)m_usCmd)
	{
    case MSG_LOGIC_USER_TALK:
    {
        CRequestTalk stParam;
        if (!DecodeMsgPack(stFormatData, stParam)) return (false);

        auto pChannelStrategy = CLogicTalk::CreateTalkChannelStrategy(stParam.m_iTalkChannel);
        if(!pChannelStrategy)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        if(!pChannelStrategy->CheckTalk(m_pUserInfo, stParam, m_stUserConnectorData.m_strClientIP))
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        CLogicBill::SaveChatBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID,
                                 m_pUserInfo->m_stUserInfoTable.GetLevel(), m_pUserInfo->m_stUserInfoTable.GetVipLevel(),
                                 m_pUserInfo->m_strNick, stParam.m_iTalkChannel, stParam.m_strTalkData, stParam.m_iDstUin);

        pChannelStrategy->SendToCross(m_pUserInfo, stParam);

        SendToClient(SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|NICK:" << m_pUserInfo->m_strNick << "|TALK CHANNEL ID:" << stParam.m_iTalkChannel
                            << "|TALK CONTENT:" << stParam.m_strTalkData << "|DST_UIN:" << stParam.m_iDstUin
                            << "|DST_GROUP:" << stParam.m_iDstGroupID << std::endl;

        break;
    }
    case MSG_LOGIC_READ_PRIVATE_TALK_REQ:
    {
        CRequestOperateUser stParam;
        if (!DecodeMsgPack(stFormatData, stParam)) return (false);

        //发送者私聊信息修改
        auto iter = m_pUserInfo->m_stOnlyInCacheData.m_stPrivateTalkCache.find(stParam.m_iUid);
        if(iter != m_pUserInfo->m_stOnlyInCacheData.m_stPrivateTalkCache.end())
        {
            iter->second.m_bHasRead = true;
        }

        SendToClient(SERVER_ERRCODE::SUCCESS);
        break;
    }
	case MSG_LOGIC_HIDE_USER_TALK_REQ:
    {
        CRequestOperateUser stParam;
        if(!DecodeMsgPack(stFormatData, stParam)) return (false);

        if(stParam.m_iUid <= 0 || stParam.m_iGroupID <= 0 || stParam.m_iUid == m_iUin)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        m_pUserInfo->m_stOnlyInCacheData.m_setPingbiUser.insert(stParam.m_iUid);

        SendToClient(SERVER_ERRCODE::SUCCESS);

        break;
    }
	case MSG_LOGIC_JUBAO_USER_REQ:
    {
        //1.解析协议
        CRequestJubaoUser stParam;

        if (!DecodeMsgPack(stFormatData, stParam)) return (false);

        if (m_pUserInfo->m_stUserDailyTable.GetJubaoCount() >= 10)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::JUBAO_COUNT_MAX);
        }

        //找到目标玩家，如果找到，则修改自身私聊信息
        auto pDstInfo = CLogicCacheCommon::LoadUserData(stParam.m_iDesGroupID, stParam.m_iDesUin);
        if (nullptr == pDstInfo)
        {
            SET_ERR_INFO(SERVER_ERRCODE::USER_ROLE_NOT_EXIST);
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        m_pUserInfo->m_stUserDailyTable.AddJubaoCount(1);
        m_pUserInfo->m_stUserInfoTable.AddJubaoCount(1);
        pDstInfo->m_stUserInfoTable.AddJubaoedCount(1);

        CLogicBill::SaveJubaoBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_pUserInfo->m_strNick, pDstInfo->m_iUin
                , pDstInfo->m_iGroupID, pDstInfo->m_strNick, pDstInfo->m_stUserInfoTable.GetLevel(), pDstInfo->m_stUserInfoTable.GetVipLevel()
                , pDstInfo->m_stUserInfoTable.GetRoleCombat(), stParam.m_iType, stParam.m_strContent, stParam.m_strFromUI, 0, "");

        SendToClient(SERVER_ERRCODE::SUCCESS);

        break;
    }
	}
    
    return (true);
}

bool CLogicTalkProcessor::DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    bool bRet = false;
    switch (stMsgHead.m_usCmd)
    {
    case MSG_CROSS_USER_TALK_NOTIFY:
        bRet = DoServerRunUserTalkNotify(stMsgHead, stFormatData);
        break;
    default:
        break;
    }

    return bRet;
}

bool CLogicTalkProcessor::DoServerRunUserTalkNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CSNotifyUserTalk stNotify;
    if (!DecodeMsgPack(stFormatData, stNotify)) return (false);

    if (!CLogicConfig::GetSystemConfig().isSelfGroup(stNotify.m_iDstGroupID))
    {
        PROCESSOR_LOG_ERROR << "TALK NOTIFY RECV ERROR GROUP!|DST_GROUP_ID:" << stNotify.m_iDstGroupID
                            << "|TALK_CHANNEL:" << stNotify.m_stTalkInfo.m_iTalkChannel
                            << "|SRC_UIN:" << stNotify.m_stTalkInfo.m_stUserBrief.m_iUid
                            << "|SRC_GROUP:" << stNotify.m_stTalkInfo.m_stUserBrief.m_iGroupID << std::endl;
        return (false);
    }

    auto pChannelStrategy = CLogicTalk::CreateTalkChannelStrategy(stNotify.m_stTalkInfo.m_iTalkChannel);
    if(pChannelStrategy)
    {
        pChannelStrategy->HandleTalk(stNotify.m_stTalkInfo, stNotify.m_iDstUin, stNotify.m_iDstGroupID, stNotify.m_stUinVec);
    }

    return true;
}

CLogicAvatarProcessor::CLogicAvatarProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicAvatarProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	if ((int)m_usCmd == MSG_LOGIC_GET_AVATAR_INFO)
	{
		CResponseAvatarInfo stProto;
		for (auto stIter = m_pUserInfo->m_stUserAvatarMap.Begin(); stIter != m_pUserInfo->m_stUserAvatarMap.End(); ++stIter)
		{
			switch (stIter->second->m_stData.m_iAvatarType)
			{
				case (int)CLogicConfigDefine::AVATAR_DEFINE::ICON_TYPE:
				{
					stProto.m_stAvatarIconIDList.push_back(stIter->first);
					break;
				}
				case (int)CLogicConfigDefine::AVATAR_DEFINE::BORDER_TYPE:
				{
					stProto.m_stAvatarBorderIDList.push_back(stIter->first);
					break;
				}
			}
		}

		for (auto stIter = CLogicConfig::GetAvatarConfig().m_stAvatarList.begin();
             stIter != CLogicConfig::GetAvatarConfig().m_stAvatarList.end(); ++stIter)
		{
			if(stIter->second.m_bIsFree)
			{
				switch (stIter->second.m_iAvatarType)
				{
					case (int)CLogicConfigDefine::AVATAR_DEFINE::ICON_TYPE:
					{
						if (stIter->second.m_iUnlockValue)
						{
							auto stCardIT = m_pUserInfo->m_stHeroCardTableMap.Find(stIter->second.m_iUnlockValue);
							if (stCardIT != m_pUserInfo->m_stHeroCardTableMap.End())
							{
								stProto.m_stAvatarIconIDList.push_back(stIter->first);
							}
						}
						else
						{
							stProto.m_stAvatarIconIDList.push_back(stIter->first);
						}

						break;
					}
					case (int)CLogicConfigDefine::AVATAR_DEFINE::BORDER_TYPE:
					{
						if (m_pUserInfo->m_stUserInfoTable.GetVipLevel() >= stIter->second.m_iUnlockValue)
						{
							stProto.m_stAvatarBorderIDList.push_back(stIter->first);
						}

						break;
					}
				}
			}
		}

		SendToClient(stProto, SERVER_ERRCODE::SUCCESS);

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;
	}
	else if ((int)m_usCmd == MSG_LOGIC_SET_AVATAR_ICON)
	{
		CRequestSetAvatar stParam;
		if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

		int iAvatarID = stParam.m_iAvatarID;
		auto stConfigIter = CLogicConfig::GetAvatarConfig().m_stAvatarList.find(iAvatarID);
		if (stConfigIter != CLogicConfig::GetAvatarConfig().m_stAvatarList.end())
		{
			if (stConfigIter->second.m_iAvatarType == (int)CLogicConfigDefine::AVATAR_DEFINE::ICON_TYPE)
			{
				if (stConfigIter->second.m_bIsFree)
				{
					if (stConfigIter->second.m_iUnlockValue != 0)
					{
						auto stCardIT = m_pUserInfo->m_stHeroCardTableMap.Find(stConfigIter->second.m_iUnlockValue);
						if (stCardIT == m_pUserInfo->m_stHeroCardTableMap.End())
						{
							using namespace SERVER_ERRCODE;
							PROCESSOR_LOG_ERROR << "ERRCODE:" << AVATAR_LOCKED << "|MSG:" << GET_ERRMSG_BY_ERRCODE(AVATAR_LOCKED) << std::endl;
							SEND_ERRCODE_AND_RET(AVATAR_LOCKED)
						}
					}
				}
				else
				{
					auto stAvatarIter = m_pUserInfo->m_stUserAvatarMap.Find(iAvatarID);
					if (stAvatarIter == m_pUserInfo->m_stUserAvatarMap.End())
					{
						using namespace SERVER_ERRCODE;
						PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_OWN_AVATAR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_OWN_AVATAR) << std::endl;
						SEND_ERRCODE_AND_RET(NOT_OWN_AVATAR)
					}
				}

				CResponseSetAvatar stRsp;
				stRsp.m_iAvatarID = iAvatarID;
				m_pUserInfo->m_stUserInfoTable.SetAvatarID(iAvatarID);
				SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

				PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|AVATAR ID:" << iAvatarID << std::endl;
				return (true);
			}
		}

		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << AVATAR_NOT_CONFIGED << "|MSG:" << GET_ERRMSG_BY_ERRCODE(AVATAR_NOT_CONFIGED) << std::endl;
		SEND_ERRCODE_AND_RET(AVATAR_NOT_CONFIGED)
	}
	else if ((int)m_usCmd == MSG_LOGIC_SET_AVATAR_BORDER)
	{
		CRequestSetAvatar stParam;
		if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

		int iAvatarID = stParam.m_iAvatarID;
		auto stConfigIter = CLogicConfig::GetAvatarConfig().m_stAvatarList.find(iAvatarID);
		if (stConfigIter != CLogicConfig::GetAvatarConfig().m_stAvatarList.end())
		{
			if (stConfigIter->second.m_iAvatarType == (int)CLogicConfigDefine::AVATAR_DEFINE::BORDER_TYPE)
			{
				if (stConfigIter->second.m_bIsFree)
				{
					if (m_pUserInfo->m_stUserInfoTable.GetVipLevel() < stConfigIter->second.m_iUnlockValue)
					{
						using namespace SERVER_ERRCODE;
						PROCESSOR_LOG_ERROR << "ERRCODE:" << AVATAR_LOCKED << "|MSG:" << GET_ERRMSG_BY_ERRCODE(AVATAR_LOCKED) << std::endl;
						SEND_ERRCODE_AND_RET(AVATAR_LOCKED)
					}
				}
				else
				{
					auto stAvatarIter = m_pUserInfo->m_stUserAvatarMap.Find(iAvatarID);
					if (stAvatarIter == m_pUserInfo->m_stUserAvatarMap.End())
					{
						using namespace SERVER_ERRCODE;
						PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_OWN_AVATAR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_OWN_AVATAR) << std::endl;
						SEND_ERRCODE_AND_RET(NOT_OWN_AVATAR)
					}
				}

				CResponseSetAvatar stRsp;
				stRsp.m_iAvatarID = iAvatarID;
				m_pUserInfo->m_stUserInfoTable.SetAvatarBorder(iAvatarID);
				SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

				PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|AVATAR ID:" << iAvatarID << std::endl;

				return (true);
			}
		}

		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << AVATAR_NOT_CONFIGED << "|MSG:" << GET_ERRMSG_BY_ERRCODE(AVATAR_NOT_CONFIGED) << std::endl;
		SEND_ERRCODE_AND_RET(AVATAR_NOT_CONFIGED)
	}
    else if(m_usCmd == MSG_LOGIC_ADD_BGM)
    {
        CRequestAddBGM stReq;
        CResponseAddBGM stRsp;
        if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

        if(stReq.m_stBgmIDs.empty())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        if(stReq.m_stBgmIDs.size() > CLogicConfigDefine::ESD_MaxBGMNum)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MUSIC_LIST_TOO_LENGTH)
        }

        for(auto bgm_id : stReq.m_stBgmIDs)
        {
            auto pConfig = CLogicConfig::GetItemConfig().GetConfig(bgm_id);
            if(nullptr == pConfig || pConfig->m_iSubType != CLogicConfigDefine::EIST_Music)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GAME_ITEM_NOT_EXIST)
            }

            if(CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, bgm_id) <= 0)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ITEM_NOT_OWNED)
            }
        }

        switch (stReq.m_iPos)
        {
        case CRequestAddBGM::pos_main:
        {
            auto stData = m_pUserInfo->m_stIBusyData.GetMainBgm();
            if(stReq.m_stBgmIDs.size() + stData.size() > CLogicConfigDefine::ESD_MaxBGMNum)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MUSIC_LIST_TOO_LENGTH)
            }

            stData.insert(stData.end(), stReq.m_stBgmIDs.begin(), stReq.m_stBgmIDs.end());
            m_pUserInfo->m_stIBusyData.SetMainBgm(stData);

            break;
        }
        case CRequestAddBGM::pos_home:
        {
            auto stData = m_pUserInfo->m_stIBusyData.GetHomeBgm();
            if(stReq.m_stBgmIDs.size() + stData.size() > CLogicConfigDefine::ESD_MaxBGMNum)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MUSIC_LIST_TOO_LENGTH)
            }

            stData.insert(stData.end(), stReq.m_stBgmIDs.begin(), stReq.m_stBgmIDs.end());
            m_pUserInfo->m_stIBusyData.SetHomeBgm(stData);

            break;
        }
        default:
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }
        }

        stRsp.m_stBgmIDs = stReq.m_stBgmIDs;
        stRsp.m_iPos = stReq.m_iPos;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return true;
    }
    else if(m_usCmd == MSG_LOGIC_DEL_BGM)
    {
        CRequestAddBGM stReq;
        CResponseAddBGM stRsp;
        if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

        if(stReq.m_stBgmIDs.size() > CLogicConfigDefine::ESD_MaxBGMNum)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MUSIC_LIST_TOO_LENGTH)
        }

        switch (stReq.m_iPos)
        {
        case CRequestAddBGM::pos_main:
        {
            auto stData = m_pUserInfo->m_stIBusyData.GetMainBgm();

            for(auto bgm_id : stReq.m_stBgmIDs)
            {
                stData.remove(bgm_id);
            }

            if(stReq.m_stBgmIDs.empty())
                stData.clear();

            m_pUserInfo->m_stIBusyData.SetMainBgm(stData);

            break;
        }
        case CRequestAddBGM::pos_home:
        {
            auto stData = m_pUserInfo->m_stIBusyData.GetHomeBgm();
            for(auto bgm_id : stReq.m_stBgmIDs)
            {
                stData.remove(bgm_id);
            }

            if(stReq.m_stBgmIDs.empty())
                stData.clear();

            m_pUserInfo->m_stIBusyData.SetHomeBgm(stData);

            break;
        }
        default:
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }
        }

        stRsp.m_stBgmIDs = stReq.m_stBgmIDs;
        stRsp.m_iPos = stReq.m_iPos;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return true;
    }
    else if(m_usCmd == MSG_LOGIC_SET_BGM_LIST)
    {
        CRequestAddBGM stReq;
        CResponseAddBGM stRsp;
        if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

        if(stReq.m_stBgmIDs.size() > CLogicConfigDefine::ESD_MaxBGMNum)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MUSIC_LIST_TOO_LENGTH)
        }

        for(auto bgm_id : stReq.m_stBgmIDs)
        {
            auto pConfig = CLogicConfig::GetItemConfig().GetConfig(bgm_id);
            if(nullptr == pConfig || pConfig->m_iSubType != CLogicConfigDefine::EIST_Music)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GAME_ITEM_NOT_EXIST)
            }

            if(CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, bgm_id) <= 0)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ITEM_NOT_OWNED)
            }
        }

        switch (stReq.m_iPos)
        {
        case CRequestAddBGM::pos_main:
        {
            m_pUserInfo->m_stIBusyData.SetMainBgm(stReq.m_stBgmIDs);
            break;
        }
        case CRequestAddBGM::pos_home:
        {
            m_pUserInfo->m_stIBusyData.SetHomeBgm(stReq.m_stBgmIDs);
            break;
        }
        default:
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }
        }

        stRsp.m_stBgmIDs = stReq.m_stBgmIDs;
        stRsp.m_iPos = stReq.m_iPos;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return true;
    }
    else if(m_usCmd == MSG_LOGIC_SET_THEME_SKIN)
    {
        CRequestSetThemeSkin stReq;
        CResponseSetThemeSkin stRsp;
        if (!DecodeMsgPack(stFormatData, stReq)) return (false);

        stRsp.m_iSkinID = stReq.m_iSkinID;
        if(m_pUserInfo->m_stUserInfoTable.GetThemeSkin() == stReq.m_iSkinID)
        {
            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
            return true;
        }

        if (!stReq.isDefault)
        {
            if(CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, stReq.m_iSkinID) <= 0)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ITEM_NOT_OWNED)
            }

            auto pConfig = CLogicConfig::GetItemConfig().GetConfig(stReq.m_iSkinID);
            if(nullptr == pConfig || pConfig->m_iSubType != CLogicConfigDefine::EIST_ThemeSkin)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GAME_ITEM_NOT_EXIST)
            }
        }

        m_pUserInfo->m_stUserInfoTable.SetThemeSkin(stReq.m_iSkinID);

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return true;
    }
    else if(m_usCmd == MSG_LOGIC_GET_BADGE_LIST)
    {
        CResponseGetBadgeList stRsp;
        stRsp.m_stList = m_pUserInfo->m_stIBusyData.GetBadgeList();
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return true;
    }
    else if(m_usCmd == MSG_LOGIC_SET_BADGE_LIST)
    {
        CRequestSetBadgeList stReq;
        if(DecodeMsgPack(stFormatData, stReq) == false) return (false);

        if(stReq.m_stList.size() > 30)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        m_pUserInfo->m_stIBusyData.SetBadgeList(stReq.m_stList);

        SendToClient(SERVER_ERRCODE::SUCCESS);
        return true;
    }

	return (true);
}
