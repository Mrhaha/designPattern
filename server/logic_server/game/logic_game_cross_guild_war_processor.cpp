#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_errcode.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_cross_guild_war_processor.h"
#include "logic_config.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_global_frame.h"
#include "logic_game_publisher.h"
#include "logic_game_talk_common.h"
#include "logic_game_active.h"
#include "logic_game_cache_common.h"
#include "logic_game_global.h"

REGISTER_CMD_FACTORY(MSG_CROSS_GET_ATTACK_DEFENSE_INFO, CLogicCrossGuildWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_FIGHT_ATTACK_DEFENSE, CLogicCrossGuildWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_FIGHT_ATTACK_DEFENSE_RESULT, CLogicCrossGuildWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_REPAIR_ATTACK_DEFENSE, CLogicCrossGuildWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_REMOVE_FIGHT_FAIL_CD, CLogicCrossGuildWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_ATTACK_DEFENSE_AWARD, CLogicCrossGuildWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_ATTACK_DEFENSE_START, CLogicCrossGuildWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WAR_REPORT_NOTIFY, CLogicCrossGuildWarProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_ATTACK_DEFENSE_PREPARE, CLogicCrossGuildWarProcessor)

CLogicCrossGuildWarProcessor::CLogicCrossGuildWarProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicCrossGuildWarProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	bool bRet = false;

	if (!GetCrossConnection()->IsConnected())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MATCH_SERVER_NOT_CONN);
	}
/*
	switch (m_usCmd)
	{
	case MSG_CROSS_GET_ATTACK_DEFENSE_INFO:
		bRet = DoUserRunGetAttackDefenseInfo(stFormatData);
		break;

	case MSG_CROSS_FIGHT_ATTACK_DEFENSE:
		bRet = DoUserRunFightAttackDefense(stFormatData);
		break;

	case MSG_CROSS_FIGHT_ATTACK_DEFENSE_RESULT:
		bRet = DoUserRunFightAttackDefenseResult(stFormatData);
		break;

	case MSG_CROSS_REPAIR_ATTACK_DEFENSE:
		bRet = DoUserRunRepairAttackDefense(stFormatData);
		break;

	case MSG_CROSS_REMOVE_FIGHT_FAIL_CD:
		bRet = DoUserRunRemoveFightFailCD(stFormatData);
		break;

	default:
		break;
	}
*/
	return bRet;
}
/*
bool CLogicCrossGuildWarProcessor::DoUserRunGetAttackDefenseInfo(const msgpack::unpacked& stFormatData)
{
	CResponseGetAttackDefenseInfo stRspBody;
	if (!CLogicConfig::GetGuildWarConfig().IsAttackDefenseFighting(CLogicCommon::GetSec()))
	{
		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
		return true;
	}

	const auto* pstGuild = CLogicGuild::GetGuild(m_pUserInfo->m_stGuildCacheData.m_iGuildID);
	if (!pstGuild)
	{
		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
		return true;
	}

	CSRequestGetAttackDefenseInfo stReqBody;
	stReqBody.m_iGuildID = pstGuild->GetGuildID();
	SendToCrossServer(enmCmdType_request, m_usCmd, stReqBody);

	return true;
}

bool CLogicCrossGuildWarProcessor::DoUserRunFightAttackDefense(const msgpack::unpacked& stFormatData)
{
	CRequestFightAttackDefense stReqBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	if (!CheckCanAttackDefenseAction(m_pUserInfo->m_stGuildCacheData.m_iGuildID, m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, CLogicConfig::GetGuildWarConfig().m_stAtkDefInfo.m_iFightCostArmyPoint))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	CSRequestFightAttackDefense stInfoBody;
	stInfoBody.m_strNick = m_pUserInfo->m_strNick;
	stInfoBody.m_iGuildID = m_pUserInfo->m_stGuildCacheData.m_iGuildID;
	stInfoBody.m_iDstUid = stReqBody.m_iDstUid;
	stInfoBody.m_iDstGroupID = stReqBody.m_iDstGroupID;
	stInfoBody.m_iDstDifficult = stReqBody.m_iDstDifficult;

	SendToCrossServer(enmCmdType_request, m_usCmd, stInfoBody);

	return true;
}

bool CLogicCrossGuildWarProcessor::DoUserRunFightAttackDefenseResult(const msgpack::unpacked & stFormatData)
{
	CRequestFightAttackDefenseResult stReqBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	SendToCrossServer(enmCmdType_request, m_usCmd, stReqBody);

	return true;
}

bool CLogicCrossGuildWarProcessor::DoUserRunRepairAttackDefense(const msgpack::unpacked & stFormatData)
{
	CRequestRepairAttackDefense stReqBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	if (!CheckCanAttackDefenseAction(m_pUserInfo->m_stGuildCacheData.m_iGuildID, m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, CLogicConfig::GetGuildWarConfig().m_stAtkDefInfo.m_iRepairCostArmyPoint))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	CSRequestRepairAttackDefense stInfoBody;
	stInfoBody.m_iGuildID = m_pUserInfo->m_stGuildCacheData.m_iGuildID;
	stInfoBody.m_iDstUid = stReqBody.m_iDstUid;
	stInfoBody.m_iDstGroupID = stReqBody.m_iDstGroupID;
	stInfoBody.m_strNick = m_pUserInfo->m_strNick;
	SendToCrossServer(enmCmdType_request, m_usCmd, stInfoBody);

	return true;
}

bool CLogicCrossGuildWarProcessor::DoUserRunRemoveFightFailCD(const msgpack::unpacked & stFormatData)
{
	CResponseRemoveFightFailCd stRspBody;

	if (!CLogicConfig::GetGuildWarConfig().IsAttackDefenseFighting(CLogicCommon::GetSec()))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
	}

	auto* pstGuild = CLogicGuild::GetGuild(m_pUserInfo->m_stGuildCacheData.m_iGuildID);
	if (!pstGuild)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_NOT_FOUND);
	}

	const auto* pstMemberValue = pstGuild->GetMemberValue(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
	if (nullptr == pstMemberValue)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND);
	}

	if (!pstMemberValue->m_cCanGuildWar)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PLAYER_CANT_GUILD_WAR);
	}

	if (CLogicCommon::GetSec() >= pstMemberValue->m_iCanAtkDefTime)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SUCCESS);
	}

	stRspBody.m_stCostItem = {
	        CLogicConfigDefine::CURRENCY,
	        CLogicConfigDefine::ECIT_YuanBaoID,
	        CLogicConfig::GetGuildWarConfig().m_stAtkDefInfo.m_iRemoveCDCost
	};
	if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRspBody.m_stCostItem, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

    pstGuild->RemoveAttackDefenseMemberFightCD(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}
*/
bool CLogicCrossGuildWarProcessor::DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
	bool bRet = false;
/*
	switch (m_usCmd)
	{
	case MSG_CROSS_GET_ATTACK_DEFENSE_INFO:
		bRet = DoServerRunGetAttackDefenseInfo(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_FIGHT_ATTACK_DEFENSE:
		bRet = DoServerRunFightAttackDefense(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_FIGHT_ATTACK_DEFENSE_RESULT:
		bRet = DoServerRunFightAttackDefenseResult(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_REPAIR_ATTACK_DEFENSE:
		bRet = DoServerRunRepairAttackDefense(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_ATTACK_DEFENSE_AWARD:
		bRet = DoServerRunCrossAttackDefenseAward(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_ATTACK_DEFENSE_START:
		bRet = DoServerRunCrossAttackDefenseStart(stMsgHead, stFormatData);
		break;

	case MSG_CROSS_GUILD_WAR_REPORT_NOTIFY:
		bRet = DoServerRunCrossGuildWarReportNotify(stMsgHead, stFormatData);
		break;

    case MSG_CROSS_ATTACK_DEFENSE_PREPARE:
        bRet = DoServerRunCrossAttackDefensePrepare(stMsgHead, stFormatData);
        break;

	default:
		break;
	}
*/
	return bRet;
}
/*
bool CLogicCrossGuildWarProcessor::DoServerRunGetAttackDefenseInfo(const CMsgHeadCross &stMsgHead,
                                                                     const msgpack::unpacked &stFormatData)
{
	CResponseGetAttackDefenseInfo stRspBody;
	if (!DecodeMsgPack(stFormatData, stRspBody))
	{
		return false;
	}

	auto* pstGuild = CLogicGuild::GetGuild(m_pUserInfo);
	if (!pstGuild)
	{
		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
		return true;
	}

	const auto* pstMemberValue = pstGuild->GetMemberValue(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
	if (!pstMemberValue)
	{
		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
		return true;
	}

	stRspBody.m_iArmyPoint = pstMemberValue->m_iArmyPoint;
	stRspBody.m_iLastAddTime = pstMemberValue->m_iLastArmyPointTime;
	stRspBody.m_iCanFightTime = pstMemberValue->m_iCanAtkDefTime;

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossGuildWarProcessor::DoServerRunFightAttackDefense(const CMsgHeadCross &stMsgHead,
                                                                   const msgpack::unpacked &stFormatData)
{
	CResponseFightAttackDefense stRspBody;
	if (!DecodeMsgPack(stFormatData, stRspBody))
	{
		return false;
	}

	if (!CheckCanAttackDefenseAction(m_pUserInfo->m_stGuildCacheData.m_iGuildID, m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, CLogicConfig::GetGuildWarConfig().m_stAtkDefInfo.m_iFightCostArmyPoint))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	const auto* pstFightConfig = CLogicConfig::GetGuildWarConfig().GetAttackDefenseFightConfig(stRspBody.m_iDstAreaType, stRspBody.m_iDstDifficult);
	if (nullptr == pstFightConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ATK_DEF_AREA_NOT_FOUND);
	}

	stRspBody.m_stChallengerPlayer.m_dAtk = pstFightConfig->m_dAtk;
	stRspBody.m_stChallengerPlayer.m_dHp = pstFightConfig->m_dHp;
	stRspBody.m_stChallengerPlayer.m_dDef = pstFightConfig->m_dDef;
	stRspBody.m_stChallengerPlayer.m_dCri = pstFightConfig->m_dCri;

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossGuildWarProcessor::DoServerRunFightAttackDefenseResult(const CMsgHeadCross &stMsgHead,
                                                                         const msgpack::unpacked &stFormatData)
{
	CSResponseFightAttackDefenseResult stCrossBody;
	CResponseFightAttackDefenseResult stRspBody;
	if (!DecodeMsgPack(stFormatData, stCrossBody))
	{
		return false;
	}

	const auto pstGuild = CLogicGuild::GetGuild(m_pUserInfo);
	if (!pstGuild)
	{
		return false;
	}

	const auto* pstMemberValue = pstGuild->GetMemberValue(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
	if (!pstMemberValue)
	{
		return false;		
	}

	if (!pstMemberValue->m_cCanGuildWar)
	{
		return false;
	}

	stRspBody.m_bResult = stCrossBody.m_bResult;
    pstGuild->FightAttackDefenseMemberResult(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, stCrossBody.m_bResult);

	if (stCrossBody.m_bResult)
	{
		const auto* pstFightConfig = CLogicConfig::GetGuildWarConfig().GetAttackDefenseFightConfig(stCrossBody.m_iDstAreaType, stCrossBody.m_iDstDifficult);
		if (nullptr != pstFightConfig)
		{
            const auto& strFightAward = CLogicConfig::GetAwardConfig().IsInActivityTime(CEnumActivityTimeType::ATKDEF_FIGHT, CLogicCommon::GetSec()) ?
                                        pstFightConfig->m_strActivityFightAwardBag : pstFightConfig->m_strFightAwardBag;
            CLogicConfig::GetCardbagConfig().GetRandBonusByName(strFightAward, stRspBody.m_stBonusVec);
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusVec, m_usCmd);
		}
	}

	if (!stCrossBody.m_bTimeOut)
	{
		stRspBody.m_iArmyPoint = pstMemberValue->m_iArmyPoint;
		stRspBody.m_iLastAddTime = pstMemberValue->m_iLastArmyPointTime;
		stRspBody.m_iCanFightTime = pstMemberValue->m_iCanAtkDefTime;
		stRspBody.m_stChallengerPlayer = stCrossBody.m_stChallengerPlayer;
		stRspBody.m_strFightNameVec = stCrossBody.m_strFightNameVec;
		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	}

	return true;
}

bool CLogicCrossGuildWarProcessor::DoServerRunRepairAttackDefense(const CMsgHeadCross &stMsgHead,
                                                                    const msgpack::unpacked &stFormatData)
{
	CResponseRepairAttackDefense stRspBody;
	if (!DecodeMsgPack(stFormatData, stRspBody))
	{
		return false;
	}

	//回调结果不再判断军令数，避免军令在其他地方消耗了而导致不足。因为异步扣除军令，可能会导致军令会变成负数
	if (!CheckCanAttackDefenseAction(m_pUserInfo->m_stGuildCacheData.m_iGuildID, m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, 0))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	auto* pstGuild = CLogicGuild::GetGuild(m_pUserInfo);
	if (!pstGuild)
	{
		SET_ERR_INFO(SERVER_ERRCODE::GUILD_NOT_FOUND);
		return false;
	}

	const auto* pstMemberValue = pstGuild->GetMemberValue(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
	if (!pstMemberValue)
	{
		SET_ERR_INFO(SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND);
		return false;		
	}

	const auto* pstFightConfig = CLogicConfig::GetGuildWarConfig().GetAttackDefenseFightConfig(stRspBody.m_iRepairAreaType, stRspBody.m_iRepairDifficult);
	if (nullptr == pstFightConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ATK_DEF_AREA_NOT_FOUND);
	}

	const auto& strRepairAward = CLogicConfig::GetAwardConfig().IsInActivityTime(CEnumActivityTimeType::ATKDEF_FIGHT, CLogicCommon::GetSec()) ?
		pstFightConfig->m_strActivityRepairAwardBag : pstFightConfig->m_strRepairAwardBag;
	CLogicConfig::GetCardbagConfig().GetRandBonusByName(strRepairAward, stRspBody.m_stBonusVec);
	CLogicCacheCommon::AddGameItem(m_pUserInfo, stRspBody.m_stBonusVec, m_usCmd);

    pstGuild->RepairAttackDefenseAreaSuccess(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);

	stRspBody.m_iArmyPoint = pstMemberValue->m_iArmyPoint;
	stRspBody.m_iLastAddTime = pstMemberValue->m_iLastArmyPointTime;

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicCrossGuildWarProcessor::DoServerRunCrossAttackDefenseAward(const CMsgHeadCross &stMsgHead,
                                                                        const msgpack::unpacked &stFormatData)
{
	CSNotifyAttackDefenseAward stNotify;
	if (!DecodeMsgPack(stFormatData, stNotify))
	{
		return false;
	}

	auto* pstGuild = CLogicGuild::GetGuild(stNotify.m_iGuildID);
	if (pstGuild)
    {
        pstGuild->AttackDefenseEndAward(stNotify.m_iStar, stNotify.m_iDstGuildID, stNotify.m_iDstStar, stNotify.m_strDstGuildName);

        pstGuild->ClearGuildFightReport(CLogicConfigDefine::FIGHT_REPORT_GUILD_ATKDEF);
    }

	return true;
}

bool CLogicCrossGuildWarProcessor::DoServerRunCrossAttackDefenseStart(const CMsgHeadCross &stMsgHead,
                                                                        const msgpack::unpacked &stFormatData)
{
	CSNotifyAttackDefenseStart stNotify;
	if (!DecodeMsgPack(stFormatData, stNotify))
	{
		return false;
	}

	for (const auto& match : stNotify.m_iGuildMatched)
	{
	    auto* pstGuild = CLogicGuild::GetGuild(match.first);
	    if (pstGuild)
        {
	        pstGuild->AddAttackDefenseMatchHistory(match.second);
	        pstGuild->SendGuildEventMail("guild_attack_defense_start", { });

            auto pChiefInfo = pstGuild->LoadGuildChief();
            if (pChiefInfo)
            {
                CLogicTalk::SendWordBroadcastTalk(pChiefInfo, CLogicConfigDefine::talk_channel_guild, "attack_defense_start_talk", std::map<std::string, std::string>{});
            }
        }
	}

	return true;
}

bool CLogicCrossGuildWarProcessor::DoServerRunCrossGuildWarReportNotify(const CMsgHeadCross &stMsgHead,
                                                                          const msgpack::unpacked &stFormatData)
{
	CSNotifyGuildWarReport stNotify;
	if (!DecodeMsgPack(stFormatData, stNotify))
	{
		return false;
	}

	AddGuildWarFightReport(stNotify.m_iGuildID, stNotify.m_iReportType, stNotify.m_iReportValue, stNotify.m_stParamMap, stNotify.m_stParamVec);
	return true;
}

bool CLogicCrossGuildWarProcessor::DoServerRunCrossAttackDefensePrepare(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    //还未同步数据但活动已经开始准备了，则同步
    if (LOGIC_SERVICE_API_INS->GetCrossConnection()->IsConnected())
    {
        CLogicGuild::AttackDefenseDataSync();
    }

    return true;
}

bool CLogicCrossGuildWarProcessor::CheckCanAttackDefenseAction(int32_t iGuildID, int32_t iUid, int32_t iGroupID, int32_t iNeedArmyPoint)
{
	if (!CLogicConfig::GetGuildWarConfig().IsAttackDefenseFighting(CLogicCommon::GetSec()))
	{
		SET_ERR_INFO(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
		return false;
	}

	auto pstGuild = CLogicGuild::GetGuild(iGuildID);
	if (!pstGuild)
	{
		SET_ERR_INFO(SERVER_ERRCODE::GUILD_NOT_FOUND);
		return false;
	}

	const auto* pstMemberValue = pstGuild->GetMemberValue(iUid, iGroupID);
	if (!pstMemberValue)
	{
		SET_ERR_INFO(SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND);
		return false;
	}

	if (!pstMemberValue->m_cCanGuildWar)
	{
		SET_ERR_INFO(SERVER_ERRCODE::PLAYER_CANT_GUILD_WAR);
		return false;
	}

	if (pstMemberValue->m_iArmyPoint < iNeedArmyPoint)
	{
		SET_ERR_INFO(SERVER_ERRCODE::ARMY_POINT_NOT_ENOUGH);
		return false;
	}

	if (CLogicCommon::GetSec() < pstMemberValue->m_iCanAtkDefTime)
	{
		SET_ERR_INFO(SERVER_ERRCODE::ATK_DEF_IN_FAIL_CD);
		return false;
	}

	return true;
}

void CLogicCrossGuildWarProcessor::AddGuildWarFightReport(int32_t iGuildID, int32_t iReportType, int32_t iReportValue, const std::map<std::string, std::string>& stParamMap, const std::vector<int32_t>& stParamVec)
{
	static const std::map<int32_t, std::string> report_map = {
		{ CLogicConfigDefine::FIGHT_REPORT_GUILD_ATKDEF , "guild_attack_defense"}
	};

	auto* pstGuild = CLogicGuild::GetGuild(iGuildID);
	if (!pstGuild)
    {
	    return ;
    }

	const auto iterReport = report_map.find(iReportType);
	if (iterReport == report_map.end())
	{
		return;
	}

	for (auto iter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound(iterReport->second); iter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound(iterReport->second); ++iter)
	{
		if (iter->second.m_iTriggerValue == iReportValue)
		{
			CPackFightReportItem report;
			report.m_iReportType = iReportType;
			report.m_iReportID = CLogicCommon::GetSec();
			report.m_strReportContent = iter->second.m_stContentObj.Replace(stParamMap);
			report.m_cFightResult = iReportValue;
			report.m_stParams = stParamVec;

			pstGuild->AddGuildFightReport(report);
			break;
		}
	}
}

*/
