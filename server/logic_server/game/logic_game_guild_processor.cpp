#include "common_datetime.h"
#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_cross_protocol.h"
#include "server_guild_protocol.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_guild_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_item_factory.h"
#include "logic_game_global.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_common_publisher.h"
#include "server_cmd_data_report_define.h"
#include "server_data_report_protocol.h"


REGISTER_CMD_FACTORY(MSG_LOGIC_GET_GUILD_LIST, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_SELF_GUILD_INFO, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_GUILD_MEMBERS, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CREATE_GUILD, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_LEAVE_GUILD, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_UPDATE_GUILD_BULLETIN, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_UPDATE_GUILD_POST, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TRANSFER_GUILD, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_EXPEL_GUILD, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_APPLY_JOIN_GUILD, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_DEAL_GUILD_APPLY, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_GUILD_APPLY_LIST, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_OTHER_PLAYER_DETAIL, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GET_USER_DETAIL, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SEARCH_GUILD, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_CHANGE_GUILD_NAME, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_BOSS_REWARD_NOTIFY, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_BOSS_GET_HURT, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_BOSS_HURT_LIST, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_CHANGE_NOTIFY, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_GET_LOG, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_WISH_SET_CARD, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_WISH_SET_SWAP_CARD, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WISH_GET_LIST, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_WISH_GET_OTHER_SWAP_INFO, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WISH_GET_SWAP_INFO, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_WISH_PRESENT, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_WISH_SWAP, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_WISH_REWARD_INFO, CLogicGuildProcessor)


//公会新加功能
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_DONATE, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_SIGN_IN, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_SIGN_UPGRADE, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_RANDOM_EXPERIENCE, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_GET_RANDOM_REWARD, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_SAVE_DECORATE, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_GET_BOSS_HURT_RANK, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_SIT_BAR, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_GET_BAR_REWARD, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_UNLOCK_COLLECTION, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GUILD_SET_DECORATE, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_GUILD_FRAGMENT_HISTORY, CLogicGuildProcessor)


//公会战信息
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WAR_GET_INFO, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WAR_BEGIN, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WAR_END, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WAR_RANK, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WAR_GET_SELF_RANK, CLogicGuildProcessor)




// 跨服转发消息
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WISH_CROSS_CHECK, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WISH_CROSS_CHECK_RESULT, CLogicGuildProcessor)
REGISTER_CMD_FACTORY(MSG_CROSS_GUILD_WISH_CROSS_DST_GET, CLogicGuildProcessor)

CLogicGuildProcessor::CLogicGuildProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicGuildProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    // 如果跨服未连接，直接返回
    if(!GetCrossConnection()->IsConnected())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::MATCH_SERVER_NOT_CONN);
    }

	bool bRet = false;
	switch (m_usCmd)
	{
    case MSG_LOGIC_GET_GUILD_LIST:
        bRet = DoUserRunGetGuildList(stFormatData);
        break;
    case MSG_LOGIC_GET_SELF_GUILD_INFO:
        bRet = DoUserRunGetSelfGuildInfo(stFormatData);
        break;
    case MSG_LOGIC_GET_GUILD_MEMBERS:
        bRet = DoUserRunGetGuildMembers(stFormatData);
        break;
    case MSG_LOGIC_CREATE_GUILD:
        bRet = DoUserRunCreateGuild(stFormatData);
        break;
    case MSG_LOGIC_LEAVE_GUILD:
        bRet = DoUserRunLeaveGuild(stFormatData);
        break;
    case MSG_LOGIC_UPDATE_GUILD_BULLETIN:
        bRet = DoUserRunUpdateGuildBulletin(stFormatData);
        break;
    case MSG_LOGIC_UPDATE_GUILD_POST:
        bRet = DoUserRunUpdateGuildPost(stFormatData);
        break;
    case MSG_LOGIC_TRANSFER_GUILD:
        bRet = DoUserRunTransferGuild(stFormatData);
        break;
    case MSG_LOGIC_EXPEL_GUILD:
        bRet = DoUserRunExpelGuild(stFormatData);
        break;
    case MSG_LOGIC_APPLY_JOIN_GUILD:
        bRet = DoUserRunApplyJoinGuild(stFormatData);
        break;
    case MSG_LOGIC_DEAL_GUILD_APPLY:
        bRet = DoUserRunDealGuildApply(stFormatData);
        break;
    case MSG_LOGIC_GET_GUILD_APPLY_LIST:
        bRet = DoUserRunGetGuildApplyList(stFormatData);
        break;
    case MSG_LOGIC_SEARCH_GUILD:
        bRet = DoUserRunSearchGuild(stFormatData);
        break;
    case MSG_LOGIC_GET_OTHER_PLAYER_DETAIL:
        bRet = DoUserRunGetOtherPlayerDetail(stFormatData);
        break;
    case MSG_LOGIC_TOUHOU_CHANGE_GUILD_NAME:
        bRet = DoUserRunChangeGuildName(stFormatData);
        break;
    case MSG_LOGIC_GUILD_BOSS_GET_HURT:
        bRet = DoUserRunGuildBossGetHurt(stFormatData);
        break;
    case MSG_LOGIC_GUILD_BOSS_HURT_LIST:
        bRet = DoUserRunGuildBossHurtList(stFormatData);
        break;
    case MSG_LOGIC_GUILD_GET_LOG:
        bRet = DoUserRunGuildGetLog(stFormatData);
        break;
    case MSG_LOGIC_GUILD_WISH_SET_CARD:
        bRet = DoUserRunGuildWishSetCard(stFormatData);
        break;
    case MSG_LOGIC_GUILD_WISH_SET_SWAP_CARD:
        bRet = DoUserRunGuildWishSetSwapCard(stFormatData);
        break;
    case MSG_CROSS_GUILD_WISH_GET_LIST:
        bRet = DoUserRunGuildWishGetList(stFormatData);
        break;
    case MSG_LOGIC_GUILD_WISH_GET_OTHER_SWAP_INFO:
        bRet = DoUserRunGuildWishGetOtherSwapInfo(stFormatData);
        break;
    case MSG_LOGIC_GUILD_WISH_PRESENT:
        bRet = DoUserRunGuildWishPresent(stFormatData);
        break;
    case MSG_LOGIC_GUILD_WISH_SWAP:
        bRet = DoUserRunGuildWishSwap(stFormatData);
        break;
    case MSG_LOGIC_GUILD_DONATE:
        bRet = DoUserRunGuildDonate(stFormatData);
        break;
    case MSG_LOGIC_GUILD_SIGN_IN:
        bRet = DoUserRunGuildSignIn(stFormatData);
        break;
    case MSG_LOGIC_GUILD_SIGN_UPGRADE:
        bRet = DoUserRunGuildSignUpGrade(stFormatData);
        break;
    case MSG_LOGIC_GUILD_RANDOM_EXPERIENCE:
        bRet = DoUserRunGuildRandom(stFormatData);
        break;
    case MSG_LOGIC_GUILD_GET_RANDOM_REWARD:
        bRet = DoUserRunGuildGetRandomReward(stFormatData);
        break;
    case MSG_LOGIC_GUILD_SAVE_DECORATE:
        bRet = DoUserRunGuildSaveDecorate(stFormatData);
        break;
    case MSG_LOGIC_GUILD_GET_BOSS_HURT_RANK:
        bRet = DoUserRunGuildGetBossHurtRank(stFormatData);
        break;
    case MSG_LOGIC_GUILD_SIT_BAR:
        bRet = DoUserRunGuildSitBar(stFormatData);
        break;
    case MSG_LOGIC_GUILD_GET_BAR_REWARD:
        bRet = DoUserRunGuildGetBarReward(stFormatData);
        break;
    case MSG_LOGIC_GUILD_UNLOCK_COLLECTION:
        bRet = DoUserRunGuildUnlockCollection(stFormatData);
        break;
    case MSG_LOGIC_GUILD_SET_DECORATE:
        bRet = DoUserRunGuildSetDecorate(stFormatData);
        break;
    case MSG_LOGIC_GET_GUILD_FRAGMENT_HISTORY:
        bRet = DoUserRunGetGuildFragmentHistory(stFormatData);
        break;
    case MSG_LOGIC_GUILD_WISH_REWARD_INFO:
        bRet = DoUserRunGetGuildRewardInfo(stFormatData);
        break;
    case MSG_CROSS_GUILD_WAR_GET_INFO:
        bRet = DoUserRunGetGuildWarInfo(stFormatData);
        break;
    case MSG_CROSS_GUILD_WAR_BEGIN:
        bRet = DoUserRunGuildWarBegin(stFormatData);
        break;
    case MSG_CROSS_GUILD_WAR_END:
        bRet = DoUserRunGuildWarEnd(stFormatData);
        break;
    case MSG_CROSS_GUILD_WAR_RANK:
        bRet = DoUserRunGuildWarGetRankInfo(stFormatData);
        break;
	default:
		PROCESSOR_LOG_ERROR << "NOT FOUND PROCESSOR WITH CMD:" << m_usCmd << std::endl;
		break;
	}
	
	return bRet;
}

bool CLogicGuildProcessor::DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    bool bRet = false;

    switch (stMsgHead.m_usCmd)
    {
    case MSG_LOGIC_GET_GUILD_LIST:
        bRet = DoServerRunGetGuildList(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GET_SELF_GUILD_INFO:
        bRet = DoServerRunGetSelfGuildInfo(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GET_GUILD_MEMBERS:
        bRet = DoServerRunGetGuildMembers(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_CREATE_GUILD:
        bRet = DoServerRunCreateGuild(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_LEAVE_GUILD:
        bRet = DoServerRunLeaveGuild(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_UPDATE_GUILD_BULLETIN:
        bRet = DoServerRunUpdateGuildBulletin(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_UPDATE_GUILD_POST:
        bRet = DoServerRunUpdateGUildPost(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_TRANSFER_GUILD:
        bRet = DoServerRunTransferGuild(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_EXPEL_GUILD:
        bRet = DoServerRunExpelGuild(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_APPLY_JOIN_GUILD:
        bRet = DoServerRunApplyJoinGuild(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_DEAL_GUILD_APPLY:
        bRet = DoServerRunDealGuildApply(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GET_GUILD_APPLY_LIST:
        bRet = DoServerRunGetGuildApplyList(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_SEARCH_GUILD:
        bRet = DoServerRunSearchGuild(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GET_USER_DETAIL:
        bRet = DoServerRunGetUserDetail(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GET_OTHER_PLAYER_DETAIL:
        bRet = DoServerRunGetOtherPlayerDetail(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_TOUHOU_CHANGE_GUILD_NAME:
        bRet = DoServerRunChangeGuildName(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GUILD_BOSS_REWARD_NOTIFY:
        bRet = DoServerRunGuildBossRewardNotify(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_BOSS_HURT_LIST:
        bRet = DoServerRunGuildBossHurtList(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GUILD_CHANGE_NOTIFY:
        bRet = DoServerRunGuildChangeNotify(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_GET_LOG:
        bRet = DoServerRunGuildGetLog(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GUILD_WISH_GET_LIST:
        bRet = DoServerRunGuildWishGetList(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GUILD_WISH_GET_SWAP_INFO:
        bRet = DoServerRunGuildWishGetSwapInfo(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_WISH_GET_OTHER_SWAP_INFO:
        bRet = DoServerRunGuildWishGetOtherSwapInfo(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_DONATE:
        bRet = DoServerRunGuildDonate(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_SIGN_IN:
        bRet = DoServerRunGuildSign(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_SIGN_UPGRADE:
        bRet = DoServerRunGuildSignUpgrade(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_GET_BOSS_HURT_RANK:
        bRet = DoServerRunGetBossHurtRank(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_SIT_BAR:
        bRet = DoServerRunBossSitBar(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_GET_BAR_REWARD:
        bRet = DoServerRunGetBossSitBarReward(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_UNLOCK_COLLECTION:
        bRet = DoServerRunUnlockCollection(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_SET_DECORATE:
        bRet = DoServerRunSetDecorate(stMsgHead, stFormatData);
        break;
    case MSG_LOGIC_GUILD_SAVE_DECORATE:
        bRet = DoServerRunSaveGuildDecorate(stMsgHead, stFormatData);
        break;

    // 跨服转发消息
    case MSG_CROSS_GUILD_WISH_CROSS_CHECK:
        bRet = DoServerRunGuildWishCrossCheck(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GUILD_WISH_CROSS_CHECK_RESULT:
        bRet = DoServerRunGuildWishCrossCheckResult(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GUILD_WISH_CROSS_DST_GET:
        bRet = DoServerRunGuildWishCrossDstGet(stMsgHead, stFormatData);
        break;

    //公会战
    case MSG_CROSS_GUILD_WAR_GET_INFO:
        bRet = DoServerRunGuildWarInfo(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GUILD_WAR_BEGIN:
        bRet = DoServerRunGuildBegin(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GUILD_WAR_END:
        bRet = DoServerRunGuildEnd(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GUILD_WAR_RANK:
        bRet = DoServerRunGuildGetRankInfo(stMsgHead, stFormatData);
        break;
    case MSG_CROSS_GUILD_WAR_GET_SELF_RANK:
        bRet = DoServerRunGuildUpdateGuildWar(stMsgHead, stFormatData);
        break;
    default:
        break;
    }

    return bRet;
}

bool CLogicGuildProcessor::DoUserRunGetGuildList(const msgpack::unpacked& stFormatData)
{
    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGetSelfGuildInfo(const msgpack::unpacked& stFormatData)
{
    PROCESSOR_LOG_DEBUG << "|GUILD_END_TIME: "
    << m_pUserInfo->m_stIBusyData.GetGuildWarCalInfo().m_iRank << "|"
    << m_pUserInfo->m_stIBusyData.GetGuildWarCalInfo().m_iRankID << "|"
    << m_pUserInfo->m_stIBusyData.GetGuildWarCalInfo().m_iGuildID << "|now: "
    << CLogicCommon::GetSec() << std::endl;

    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGetGuildMembers(const msgpack::unpacked& stFormatData)
{
    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunCreateGuild(const msgpack::unpacked& stFormatData)
{
	CRequestCreateGuild stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID) < CLogicConfig::GetGuildConfig().m_iCreateCost)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_RMB)
    }

	if (m_pUserInfo->m_stUserInfoTable.GetLevel() < CLogicConfig::GetGuildConfig().m_iCreateLevel)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL)
	}

	const auto iErrorCode = CLogicCacheCommon::CheckGuildName(stParam.m_strGuildName);
	if(iErrorCode != SERVER_ERRCODE::SUCCESS)
    {
        PROCESSOR_LOG_ERROR << "ERRCODE:" << iErrorCode
                            << "|GUILD_NAME:" << stParam.m_strGuildName
                            << std::endl;
        SEND_ERRCODE_AND_RET(iErrorCode)
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
	return true;
}

bool CLogicGuildProcessor::DoUserRunLeaveGuild(const msgpack::unpacked& stFormatData)
{
    if (CheckGuildWarOpen())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_IS_OPEN)
    }

    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunUpdateGuildBulletin(const msgpack::unpacked& stFormatData)
{
	CRequestUpdateGuildBulletin stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

	if (LOGIC_SERVICE_API_INS->CheckNickHasDirtyWord(stParam.m_strBulletin))
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << GUILD_BULLETIN_HAS_DIRTY_WORD
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(GUILD_BULLETIN_HAS_DIRTY_WORD)
			<< "|CMD:" << m_usCmd << std::endl;
		SEND_ERRCODE_AND_RET(GUILD_BULLETIN_HAS_DIRTY_WORD)
	}

	if (stParam.m_strBulletin.size() > CLogicConfigGuildParser::E_MaxBulletinLength)
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << GUILD_BULLETIN_TOO_LONG
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(GUILD_BULLETIN_TOO_LONG)
			<< "|CMD:" << m_usCmd << std::endl;
		SEND_ERRCODE_AND_RET(GUILD_BULLETIN_TOO_LONG)
	}

	if (m_pUserInfo->m_stUserInfoTable.GetLevel() < CLogicConfig::GetGuildConfig().m_iUpdateBulletinLevel)
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_TEAM_LEVEL
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_TEAM_LEVEL)
			<< "|CMD:" << m_usCmd << std::endl;
		SEND_ERRCODE_AND_RET(NOT_ENOUGH_TEAM_LEVEL)
	}

    SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
    return true;
}

bool CLogicGuildProcessor::DoUserRunUpdateGuildPost(const msgpack::unpacked& stFormatData)
{
	CRequestUpdateGuildPost stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iDstUid == m_pUserInfo->m_iUin)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
    return true;
}

bool CLogicGuildProcessor::DoUserRunTransferGuild(const msgpack::unpacked& stFormatData)
{
	CRequestTransferGuild stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iDstUid == m_pUserInfo->m_iUin)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
    return true;
}

bool CLogicGuildProcessor::DoUserRunExpelGuild(const msgpack::unpacked& stFormatData)
{
    if (CheckGuildWarOpen())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_IS_OPEN)
    }

	CRequestExpelGuild stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iDstUid == m_pUserInfo->m_iUin)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
    return true;
}

bool CLogicGuildProcessor::DoUserRunApplyJoinGuild(const msgpack::unpacked& stFormatData)
{
	CRequestApplyJoinGuild stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iGuildID <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

	if (CLogicCommon::GetSec() - m_pUserInfo->m_stUserInfoTable.GetLastLeaveGuildTime() < CLogicConfig::GetGuildConfig().m_iJoinGuildCD)
	{
		using namespace SERVER_ERRCODE;
		PROCESSOR_LOG_ERROR << "ERRCODE:" << APPLY_JOIN_GUILD_CD
			<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(APPLY_JOIN_GUILD_CD)
			<< "|CMD:" << m_usCmd << std::endl;
		SEND_ERRCODE_AND_RET(APPLY_JOIN_GUILD_CD)
	}

    SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
    return true;
}

bool CLogicGuildProcessor::DoUserRunDealGuildApply(const msgpack::unpacked& stFormatData)
{
	CRequestDealGuildApply stParam;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iDstUid <= 0 || stParam.m_iDstGroupID <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGetGuildApplyList(const msgpack::unpacked& stFormatData)
{
    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGetOtherPlayerDetail(const msgpack::unpacked& stFormatData)
{
	CResponseGetOtherPlayerDetail stRsp;
	CRequestGetOtherPlayerDetail stParam;

	if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

    // 本服玩家或机器人
    if(stParam.m_iGroupID <= 0 || CLogicConfig::GetSystemConfig().isSelfGroup(stParam.m_iGroupID))
    {
        stRsp.m_stInfo = CLogicCacheCommon::GetUserDetailInfo(stParam.m_iGroupID, stParam.m_iUid, stParam.m_iTeamType);

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return true;
    }

    // 其他服玩家
    CRequestGetOtherPlayerDetail stCross;
    stCross.m_iTeamType = stParam.m_iTeamType;
    stCross.m_iUid = m_iUin;
    stCross.m_iGroupID = m_iGroupID;
    SendToCrossServer(enmCmdType_cross, MSG_CROSS_GET_USER_DETAIL, stCross, stParam.m_iUid, stParam.m_iGroupID);

	return true;
}

bool CLogicGuildProcessor::DoUserRunSearchGuild(const msgpack::unpacked& stFormatData)
{
	CRequestSearchGuild stParam;
    CRequestApplyJoinGuild stCross;
	if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    if (stParam.m_strSearch.empty() && stParam.m_iGuildID <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_NOT_FOUND)
    }

    if (stParam.m_strSearch.empty() && stParam.m_iGuildID > 0)
    {
        stCross.m_iGuildID = stParam.m_iGuildID;
    } else
    {
        std::string strEscapeSearch = CLogicSQLTableRecordBase::GetMysqlConn()->escapeString(stParam.m_strSearch);

        char szBuffer[1024];
        snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM guild_info_t WHERE guild_name=\"%s\";", strEscapeSearch.c_str());

        try {
            auto stData = CLogicSQLTableRecordBase::GetMysqlConn()->queryRecord(szBuffer);
            if(stData.size() > 0)
            {
                stCross.m_iGuildID = ::atoi(stData[0]["guild_id"].c_str());
            }
        }
        catch (const taf::TC_Mysql_Exception& stEx) {}

        if(stCross.m_iGuildID <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_NOT_FOUND)
        }
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stCross);
    return true;
}

bool CLogicGuildProcessor::DoUserRunChangeGuildName(const msgpack::unpacked& stFormatData)
{
    CRequestChangeGuildName stParam;
    if (!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID) < CLogicConfig::GetGuildConfig().m_iChangeNameCost)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_RMB)
    }

    const auto iErrorCode = CLogicCacheCommon::CheckGuildName(stParam.m_strNewGuildName);
    if(iErrorCode != SERVER_ERRCODE::SUCCESS)
    {
        PROCESSOR_LOG_ERROR << "ERRCODE:" << iErrorCode
                            << "|GUILD_NAME:" << stParam.m_strNewGuildName
                            << std::endl;
        SEND_ERRCODE_AND_RET(iErrorCode)
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildBossGetHurt(const msgpack::unpacked& stFormatData)
{
    CResponseGuildBossGetHurt stRsp;
    stRsp.m_stHurtMap = m_pUserInfo->m_stIBusyData.GetGuildBossHurt();
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildBossHurtList(const msgpack::unpacked& stFormatData)
{
    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildGetLog(const msgpack::unpacked& stFormatData)
{
    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildWishSetCard(const msgpack::unpacked& stFormatData)
{
    CRequestGuildWishSetCard stReq;
    CResponseGuildWishSetCard stRsp;

    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    GuildWishCheckAcceptCard();

    if(!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stIBusyData.GetWishLastTime()))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::WISH_IS_DOING)
    }

    if(m_pUserInfo->m_stUserDailyTable.GetWishSwapTimes() >= CLogicConfig::GetGuildConfig().m_iWishSwapTimes)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
    }

    auto iter = m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iCardID);
    if(iter == m_pUserInfo->m_stHeroCardTableMap.End())
    {
        SEND_ERRCODE_AND_RET1(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED, stReq.m_iCardID)
    }

    auto pConfig = CLogicConfig::GetGuildConfig().GetWishConfig(iter->second->GetGradeLevel());
    if(nullptr == pConfig)
    {
        SEND_ERRCODE_AND_RET1(SERVER_ERRCODE::WISH_GRADE_CANNOT_WISH, stReq.m_iCardID)
    }

    // 刷新今日祈愿
    m_pUserInfo->m_stIBusyData.SetWishCardID(stReq.m_iCardID);
    m_pUserInfo->m_stIBusyData.SetWishLastTime(CLogicCommon::GetSec());
    m_pUserInfo->m_stIBusyData.SetWishNum(0);
    m_pUserInfo->m_stIBusyData.SetWishSwapCard({});

    stRsp.m_iCardID = stReq.m_iCardID;
    SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);

    // 同步跨服
    GuildWishNotifyCross(m_pUserInfo);

    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_GuildWish;
    stEventData.m_iCmd = m_usCmd;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildWishSetSwapCard(const msgpack::unpacked& stFormatData)
{
    CRequestGuildWishSetSwapCard stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    if(m_pUserInfo->m_stUserDailyTable.GetWishSwapTimes() >= CLogicConfig::GetGuildConfig().m_iWishSwapTimes)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
    }

    if(LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stIBusyData.GetWishLastTime()))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::WISH_NOT_DONE)
    }

    if(stReq.m_stSwapCardVec.size() > CLogicConfigGuildParser::E_MaxWishSwapCardNum)
    {
        SEND_ERRCODE_AND_RET1(SERVER_ERRCODE::PARAMETER_ERROR, stReq.m_stSwapCardVec.size())
    }

    int iCardID = m_pUserInfo->m_stIBusyData.GetWishCardID();
    for(auto card_id : stReq.m_stSwapCardVec)
    {
        if(card_id == iCardID)
        {
            SEND_ERRCODE_AND_RET1(SERVER_ERRCODE::PARAMETER_ERROR, card_id)
        }

        if(!CLogicConfig::GetHeroCardConfig().GetConfig(card_id))
        {
            SEND_ERRCODE_AND_RET1(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED, card_id)
        }
    }

    m_pUserInfo->m_stIBusyData.SetWishSwapCard(stReq.m_stSwapCardVec);

    SendToClient(SERVER_ERRCODE::SUCCESS);

    // 同步跨服
    GuildWishNotifyCross(m_pUserInfo);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildWishGetList(const msgpack::unpacked& stFormatData)
{
    GuildWishCheckAcceptCard();

    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildWishGetOtherSwapInfo(const msgpack::unpacked& stFormatData)
{
    CResponseGuildWishGetOtherSwapInfo stRsp;
    CRequestGuildWishGetSwapInfo stParam;

    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iDstUin <= 0 || stParam.m_iDstGroupID <= 0 || stParam.m_iDstUin == m_iUin)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    // 本服玩家
    if(CLogicConfig::GetSystemConfig().isSelfGroup(stParam.m_iDstGroupID))
    {
        if(!GuildWishGetSwapInfo(stParam.m_iDstUin, stParam.m_iDstGroupID, stRsp))
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return true;
    }

    // 其他服玩家
    CRequestGuildWishGetSwapInfo stCross;
    stCross.m_iDstUin = m_iUin;
    stCross.m_iDstGroupID = m_iGroupID;
    SendToCrossServer(enmCmdType_cross, MSG_CROSS_GUILD_WISH_GET_SWAP_INFO, stCross, stParam.m_iDstUin, stParam.m_iDstGroupID);

    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildWishPresent(const msgpack::unpacked& stFormatData)
{
    CRequestGuildWishSwap stParam;
    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iSwapCardID != 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(!GuildWishHandleCheck(stParam))
    {
        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
    }

    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildWishSwap(const msgpack::unpacked& stFormatData)
{
    CRequestGuildWishSwap stParam;
    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iSwapCardID <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    if(!GuildWishHandleCheck(stParam))
    {
        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
    }

    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildDonate(const msgpack::unpacked& stFormatData)
{
    CRequestGuildDonate stParam;
    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    if(stParam.m_iDonateType <= 0 && stParam.m_iDonateType >= (int32_t)EnumGuildDonateType::GuildDonateMax)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
    }

    auto pDonateConfig =  CLogicConfig::GetGuildConfig().GetDonateConfig(stParam.m_iDonateType);
    if (nullptr == pDonateConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }

    //检查消耗是否满足
    for (auto consumeItem : pDonateConfig->m_stConsumeItem)
    {
        if(!CLogicCacheCommon::HasEnoughItem(m_pUserInfo,consumeItem))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM)
        }
    }

    //当天是否捐献过了
    if(m_pUserInfo->m_stUserInfoTable.GetDailyGuildDonate() == 1)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_TODAY_HAS_DONATE)
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
    return true;
}


bool CLogicGuildProcessor::DoUserRunGuildSignIn(const msgpack::unpacked& stFormatData)
{
    if (m_pUserInfo->m_stIBusyData.GetDailyGuildSign() != 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_DAILY_HAS_SIGN)
    }

    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildSignUpGrade(const msgpack::unpacked& stFormatData)
{
    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildRandom(const msgpack::unpacked& stFormatData)
{
    CResponseGuildRandomInfo stRsp;
    auto guildConfig = CLogicConfig::GetGuildConfig();

    auto randomIndex = 0;
    auto randomType = 0;

    if (m_pUserInfo->m_stIBusyData.GetDailyGuildRandom().m_iRandomType == 0)
    {
        if (m_pUserInfo->m_stIBusyData.GetDailyGuildRandomCount() >= guildConfig.m_iDailyRandomCount)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_RANDOM_REWARD_COUNT)
        }

        randomType = CLogicCommon::GetRandNum(static_cast<int32_t>(EnumGuildRandomType::GuildRandomTypeChat),
                                                  static_cast<int32_t>(EnumGuildRandomType::GuildRandomTypePk));
        if(randomType == static_cast<int32_t>(EnumGuildRandomType::GuildRandomTypeChat))
        {
            randomIndex = CLogicCommon::GetRandNum(1,guildConfig.m_stChatReward.size());

        } else if (randomType == static_cast<int32_t>(EnumGuildRandomType::GuildRandomTypePk))
        {
            randomIndex = CLogicCommon::GetRandNum(1,guildConfig.m_stPkReward.size());

        } else if (randomType == static_cast<int32_t>(EnumGuildRandomType::GuildRandomTypeExchange))
        {
            randomIndex = CLogicCommon::GetRandNum(1,guildConfig.m_stExchangeReward.size());
        }
        CPackGuildRandomInfo info;
        info.m_iRandomType = randomType;
        info.m_iRandomIndex = randomIndex;

        m_pUserInfo->m_stIBusyData.SetDailyGuildRandom(info);
    }

    stRsp.m_iCurrType = m_pUserInfo->m_stIBusyData.GetDailyGuildRandom().m_iRandomType;
    stRsp.m_iIndex = m_pUserInfo->m_stIBusyData.GetDailyGuildRandom().m_iRandomIndex;
    stRsp.m_iTodayCount = m_pUserInfo->m_stIBusyData.GetDailyGuildRandomCount();

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}


bool CLogicGuildProcessor::DoUserRunGuildGetRandomReward(const msgpack::unpacked& stFormatData)
{
    CRequestGuildGetRandomReward stParam;
    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    int32_t iRandomType = m_pUserInfo->m_stIBusyData.GetDailyGuildRandom().m_iRandomType;
    int32_t iRandomIndex = m_pUserInfo->m_stIBusyData.GetDailyGuildRandom().m_iRandomIndex;

    auto guildConfig = CLogicConfig::GetGuildConfig();

    if (iRandomType == 0 || iRandomIndex == 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_RANDOM_GET_REWARD_ERROR)
    }

    if (m_pUserInfo->m_stIBusyData.GetDailyGuildRandomCount() == guildConfig.m_iDailyRandomCount)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_RANDOM_REWARD_COUNT)
    }

    if(iRandomType == static_cast<int32_t>(EnumGuildRandomType::GuildRandomTypeChat))
    {
        auto chatConfig = guildConfig.GetChatRewardConfig(iRandomIndex);
        if (chatConfig == nullptr)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }
        static std::string strBonus;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, chatConfig->m_stReward, strBonus, m_usCmd);

    } else if (iRandomType == static_cast<int32_t>(EnumGuildRandomType::GuildRandomTypePk))
    {
        auto pkConfig = guildConfig.GetPkConfig(iRandomIndex);
        if (pkConfig == nullptr)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }
        static std::string strBonus;
        if (stParam.m_bWin)
        {
            CLogicCacheCommon::AddGameItem(m_pUserInfo, pkConfig->m_stWinReward, strBonus, m_usCmd);
        } else
        {
            CLogicCacheCommon::AddGameItem(m_pUserInfo, pkConfig->m_stLoseReward, strBonus, m_usCmd);
        }

    } else if (iRandomType == static_cast<int32_t>(EnumGuildRandomType::GuildRandomTypeExchange))
    {
        auto exchangeConfig = guildConfig.GetExchangeRewardConfig(iRandomIndex);
        if (exchangeConfig == nullptr)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }
        if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, exchangeConfig->m_stConsume, m_usCmd))
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << std::endl;
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }
        static std::string strBonus;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, exchangeConfig->m_stReward, strBonus, m_usCmd);
    }

    //更新奖励记录
    m_pUserInfo->m_stIBusyData.SetDailyGuildRandom({});
    m_pUserInfo->m_stIBusyData.AddDailyGuildRandomCount(1);

    CResponseGuildGetRandomReward stRsp;
    stRsp.m_stReward = m_pUserInfo->m_stCacheBonusData;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildSaveDecorate(const msgpack::unpacked& stFormatData)
{
    CRequestGuildSave stParam;
    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    for (auto& unEquip : stParam.m_stUnEquipGoods)
    {
        auto pItemConfig = CLogicConfig::GetItemConfig().GetConfig(unEquip.m_iItemID);
        if(nullptr == pItemConfig)
        {
            return (SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
        }
        unEquip.m_iType = pItemConfig->m_iSubType;
    }

    for (auto& equip : stParam.m_stEquipGood)
    {
        auto pItemConfig = CLogicConfig::GetItemConfig().GetConfig(equip.m_stGoodsPara.m_iGoodsID);
        if(nullptr == pItemConfig)
        {
            return (SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
        }
        equip.m_iType = pItemConfig->m_iSubType;
        equip.m_stGoodsPara.m_iGoodsIndex = CLogicCacheCommon::GetUserPrimaryKeyIndex(m_pUserInfo);
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stParam);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildGetBossHurtRank(const msgpack::unpacked& stFormatData)
{
    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildSitBar(const msgpack::unpacked& stFormatData)
{
    CRequestGuildSitBar stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    if (m_pUserInfo->m_stIBusyData.GetDailyGuildBar() == 1)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_TODAY_HAS_SIT_BAR)
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildGetBarReward(const msgpack::unpacked& stFormatData)
{
    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildUnlockCollection(const msgpack::unpacked& stFormatData)
{
    CRequestUnlockCollection stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildSetDecorate(const msgpack::unpacked& stFormatData)
{
    CRequestSetDecorate stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGetGuildFragmentHistory(const msgpack::unpacked& stFormatData)
{
    CResponseGuildFragmentHistory stRsp;
    auto guildFragmentHistory = m_pUserInfo->m_stIBusyData.GetGuildFragmentHistory();
    while (guildFragmentHistory.size() > FRAGEMENT_HISTORY_SIZE)
    {
        guildFragmentHistory.erase(guildFragmentHistory.begin());
    }
    m_pUserInfo->m_stIBusyData.SetGuildFragmentHistory(guildFragmentHistory);
    stRsp.m_stFragHistory = guildFragmentHistory;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGetGuildRewardInfo(const msgpack::unpacked& stFormatData)
{
    CResponseGuildWishRewardInfo stRsp;
    stRsp.m_iWishNum = m_pUserInfo->m_stIBusyData.GetWishRewardVec().size();
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

//不做处理直接转发给cross
bool CLogicGuildProcessor::DoUserRunGetGuildWarInfo(const msgpack::unpacked& stFormatData)
{
    if (!CheckGuildWarOpen())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_NOT_OPEN)
    }

    std::string stEmpty;
    SendToCrossServer(enmCmdType_request, m_usCmd, stEmpty);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildWarBegin(const msgpack::unpacked& stFormatData)
{
    if (!CheckGuildWarOpen())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_NOT_OPEN)
    }

    CRequestGuildWarBegin stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    if (stReq.m_iLevelID <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FIGHT_LEVEL_MISMATCH)
    }
    auto guildConfig = CLogicConfig::GetGuildConfig();
    if (m_pUserInfo->m_stIBusyData.GetDailyGuildWarTime() >= guildConfig.m_iDailyGuildWarCount)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_TIME_NOT_ENOUGH)
    }

    //检查阵容是否存在已经使用过的card
    auto usedCard = m_pUserInfo->m_stIBusyData.GetGuildWarUseCard();
    for(auto& useCard : stReq.m_stUseCard)
    {
        if (useCard == 0)
        {
            continue;
        }
        auto iterCard = usedCard.find(useCard);
        if (iterCard != usedCard.end())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_USED_CARD)
        }
    }

    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);
    return true;
}

bool CLogicGuildProcessor::DoUserRunGuildWarEnd(const msgpack::unpacked& stFormatData)
{
    if (!CheckGuildWarOpen())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_NOT_OPEN)
    }

    CRequestGuildWarEnd stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    if (stReq.m_iLevelID <= 0 && m_pUserInfo->m_stUserInfoTable.GetFightLevelID() != stReq.m_iLevelID)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FIGHT_LEVEL_MISMATCH)
    }

    //检查阵容是否存在已经使用过的card
    auto usedCard = m_pUserInfo->m_stIBusyData.GetGuildWarUseCard();
    for(auto& useCard : stReq.m_stUseCard)
    {
        if (useCard == 0)
        {
            continue;
        }
        auto iterCard = usedCard.find(useCard);
        if (iterCard != usedCard.end())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_USED_CARD)
        }
    }

    int32_t iSecNow = CLogicCommon::GetSec();
    if (iSecNow - m_pUserInfo->m_stIBusyData.GetStartLevelTime() <= CAL_TIME_INTERVAL)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_CAL_TIME_TOO_SHORT)
    }

    if (stReq.m_iHurt >= std::numeric_limits<int32_t>::max())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_HURT_EXCEPTION)
    }


    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);
    return true;
}

//获取公会战排名
bool CLogicGuildProcessor::DoUserRunGuildWarGetRankInfo(const msgpack::unpacked& stFormatData)
{
    CRequestGetGuildWarRank stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);
    if (stReq.m_iIndex < 0 || stReq.m_iIndex >= 10)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GUILD_WAR_RANK_INDEX_ERROR)
    }
    SendToCrossServer(enmCmdType_request, m_usCmd, stReq);
    return true;
}

//////////////////////////////////////////// Server Run ///////////////////////////////////////////////////
bool CLogicGuildProcessor::DoServerRunGetGuildList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGetGuildList stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGetSelfGuildInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGetSelfGuildInfo stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    m_pUserInfo->m_stUserInfoTable.SetGuildID(stRsp.m_stGuildInfo.m_iGuildID);
    if(m_pUserInfo->m_stUserInfoTable.GetGuildName().compare(stRsp.m_stGuildInfo.m_strGuildName) != 0)
        m_pUserInfo->m_stUserInfoTable.SetGuildName(stRsp.m_stGuildInfo.m_strGuildName);

    stRsp.m_iWishCardID = m_pUserInfo->m_stIBusyData.GetWishCardID();
    stRsp.m_iWishLastTime = m_pUserInfo->m_stIBusyData.GetWishLastTime();
    stRsp.m_stWishSwapCard = m_pUserInfo->m_stIBusyData.GetWishSwapCard();
    stRsp.m_iWishSwapTimes = m_pUserInfo->m_stUserDailyTable.GetWishSwapTimes();

    stRsp.m_bSign = m_pUserInfo->m_stIBusyData.GetDailyGuildSign();
    stRsp.m_bDonate = m_pUserInfo->m_stIBusyData.GetDailyGuildDonate();
    stRsp.m_iRandomCount = m_pUserInfo->m_stIBusyData.GetDailyGuildRandomCount();
    stRsp.m_iGuildBar = m_pUserInfo->m_stIBusyData.GetDailyGuildBar();

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    //每次获取公会信息更新当前的解锁buffID
    auto mapGuildBuffConfig = CLogicConfig::GetGuildConfig().m_stGuildUnlockItemBuff;
    auto guildBuffConfig = CLogicConfig::GetBuffConfig().m_stGuildBuffConfig;
    for(auto unLockItemID : stRsp.m_iUnlockItem)
    {
        auto guildBuff = mapGuildBuffConfig[unLockItemID];
        m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff[guildBuff.m_iBuffID] = guildBuffConfig[guildBuff.m_iBuffID];
    }

    if(stRsp.m_stGuildInfo.m_iGuildID > 0)
    {
        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_JoinGuild;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
    }
    return true;
}

bool CLogicGuildProcessor::DoServerRunGetGuildMembers(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGetGuildMembers stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunCreateGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseCreateGuild stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    // 特殊判断，如果消耗失败，只记录日志，不返回
    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID,
                                            CLogicConfig::GetGuildConfig().m_iCreateCost, m_usCmd))
    {
        PROCESSOR_LOG_ERROR << "Consume error! ERRCODE:" << CLogicError::GetErrCode()
                            << "|MSG:" << CLogicError::GetErrMsg() << std::endl;
    }

    m_pUserInfo->m_stUserInfoTable.SetGuildID(stRsp.m_iGuildID);
    m_pUserInfo->m_stUserInfoTable.SetGuildName(stRsp.m_strGuildName);

    stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    // 通用事件
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_JoinGuild;
    stEventData.m_iCmd = m_usCmd;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
    return true;
}

bool CLogicGuildProcessor::DoServerRunLeaveGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseLeaveGuild stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    m_pUserInfo->m_stUserInfoTable.SetLastLeaveGuildTime(CLogicCommon::GetSec());
    m_pUserInfo->m_stUserInfoTable.SetGuildID(0);
    m_pUserInfo->m_stUserInfoTable.SetGuildName("");

    SendToClient(SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunUpdateGuildBulletin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseUpdateGuildBulletin stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunUpdateGUildPost(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseUpdateGuildPost stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunTransferGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseTransferGuild stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunExpelGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseExpelGuild stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunApplyJoinGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseApplyJoinGuild stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunDealGuildApply(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseDealGuildApply stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGetGuildApplyList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGetGuildApplyList stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunSearchGuild(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseSearchGuild stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp))
    {
        return false;
    }

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGetUserDetail(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CRequestGetOtherPlayerDetail stReqCross;
    if (!DecodeMsgPack(stFormatData, stReqCross))
    {
        return false;
    }

    CResponseGetOtherPlayerDetail stRsp;
    stRsp.m_stInfo = CLogicCacheCommon::GetUserDetailInfo(m_iGroupID, m_iUin, stReqCross.m_iTeamType);
    SendToCrossServer(enmCmdType_cross, MSG_LOGIC_GET_OTHER_PLAYER_DETAIL, stRsp, stReqCross.m_iUid, stReqCross.m_iGroupID);

    return true;
}

bool CLogicGuildProcessor::DoServerRunGetOtherPlayerDetail(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGetOtherPlayerDetail stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunChangeGuildName(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseChangeGuildName stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    if(stRsp.m_bConsume)
    {
        // 特殊判断，如果消耗失败，只记录日志，不返回
        if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID,
                                               CLogicConfig::GetGuildConfig().m_iChangeNameCost, m_usCmd))
        {
            PROCESSOR_LOG_ERROR << "Consume error! ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg() << std::endl;
        }
    }

    stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildBossRewardNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossGuildBossRewardNotify stNotify;
    if (!DecodeMsgPack(stFormatData, stNotify)) return (false);

    if (!CLogicConfig::GetSystemConfig().isSelfGroup(stNotify.m_iGroupID))
    {
        PROCESSOR_LOG_ERROR << "USER UIN:" << m_iUin << " GROUP_ID:" << m_iGroupID << " NOT IN THIS SERVER!!!" << std::endl;
        return (false);
    }

    auto& stConfig = CLogicConfig::GetGuildConfig();
    auto pRewardConfig = stConfig.GetBossAllReward(stNotify.m_iWeek, stNotify.m_ulTotalHurt);
    if(pRewardConfig && !pRewardConfig->m_stRewards.empty())
    {
        for(auto uin : stNotify.m_stUinVec)
        {
            CLogicGlobal::AddAsyncUserMail(uin, stNotify.m_iGroupID, stConfig.m_strBossMailSender, stConfig.m_strBossMailTitle,
                                           stConfig.m_strBossMailContent, pRewardConfig->m_stRewards, m_strCmdName);
        }
    }

    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildBossHurtList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildBossHurtList stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    // 排序
    const auto sort_func = [](const CPackGuildBossHurtRankInfo& a, const CPackGuildBossHurtRankInfo& b){
      return a.m_ulHurt > b.m_ulHurt;
    };
    std::sort(stRsp.m_stHurtRank.begin(), stRsp.m_stHurtRank.end(), sort_func);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildChangeNotify(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CNotifyGuildChange stNotify;
    if (!DecodeMsgPack(stFormatData, stNotify)) return (false);

    auto pUser = CLogicCacheCommon::GetInCacheUserData(m_iGroupID, m_iUin);
    if(pUser)
    {
        pUser->m_stUserInfoTable.SetGuildID(stNotify.m_iGuildID);
        if(pUser->m_stUserInfoTable.GetGuildName().compare(stNotify.m_strGuildName) != 0)
            pUser->m_stUserInfoTable.SetGuildName(stNotify.m_strGuildName);

        GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, m_usCmd, stNotify);
    }

    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildGetLog(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildGetLog stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildWishGetList(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildWishGetList stRsp;
    if(!DecodeMsgPack(stFormatData, stRsp)) return (false);

    CResponseGuildWishGetList stFilterRsp;
    for (const auto& wishItem : stRsp.m_stWishVec)
    {
        if (!wishItem.m_stWish.m_bIsFull)
        {
            stFilterRsp.m_stWishVec.emplace_back(wishItem);
        }
    }

    SendToClient(stFilterRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildWishGetSwapInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CRequestGuildWishGetSwapInfo stReq;
    if(!DecodeMsgPack(stFormatData, stReq)) return (false);

    CResponseGuildWishGetOtherSwapInfo stRsp;

    if(!GuildWishGetSwapInfo(m_iUin, m_iGroupID, stRsp))
    {
        stRsp.m_iRetCode = CLogicError::GetErrCode();
    }

    SendToCrossServer(enmCmdType_cross, MSG_LOGIC_GUILD_WISH_GET_OTHER_SWAP_INFO, stRsp, stReq.m_iDstUin, stReq.m_iDstGroupID);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildWishGetOtherSwapInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildWishGetOtherSwapInfo stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return (false);

    if(stRsp.m_iRetCode != SERVER_ERRCODE::SUCCESS)
        SEND_ERRCODE_AND_RET(stRsp.m_iRetCode)

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}


bool CLogicGuildProcessor::DoServerRunGuildDonate(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CRequestGuildDonate stParam;
    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    auto pDonateConfig =  CLogicConfig::GetGuildConfig().GetDonateConfig(stParam.m_iDonateType);
    if (nullptr == pDonateConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }

    //在记录玩家当天已经捐献过了
    m_pUserInfo->m_stIBusyData.SetDailyGuildDonate(1);

    //扣除消耗
    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,pDonateConfig->m_stConsumeItem,m_usCmd))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM)
    }

    auto donateReward = pDonateConfig->m_stRewardItem;

    //检查公会buff
    auto guildBuffIter = m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.find(EnumGuildBuffType::GuildBuffGuildDonate);
    if (guildBuffIter != m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.end())
    {
        PROCESSOR_LOG_DEBUG << "HAS GUILD_BUFF: " << guildBuffIter->second.m_iBuffID << std::endl;
        for(auto& rewardItem : donateReward)
        {
            rewardItem.m_iNum = (int32_t)((float)rewardItem.m_iNum * (1 + guildBuffIter->second.m_iBuffParam[0]/100));
        }
    }

    //添加捐献奖励
    static std::string strBonus;
    CLogicCacheCommon::AddGameItem(m_pUserInfo, donateReward, strBonus, m_usCmd);

    CResponseGuildDonate stRsp;

    stRsp.m_iDonateType = stParam.m_iDonateType;
    stRsp.m_iReward = m_pUserInfo->m_stCacheBonusData;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildSign(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossGuildSignInReward stParam;
    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    auto pSignRewardConfig =  CLogicConfig::GetGuildConfig().GetSignRewardConfig(stParam.m_iLevel);
    if (nullptr == pSignRewardConfig)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST)
    }

    //记录签到
    m_pUserInfo->m_stIBusyData.SetDailyGuildSign(1);

    static std::string strBonus;
    std::vector<CPackGameItem> stSignReward;
    for(auto& bag : pSignRewardConfig->m_stReward)
    {
        CLogicConfig::GetCardbagConfig().GetRandBonusByName(bag.m_strBagName, stSignReward,bag.m_iNum);
    }
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stSignReward, strBonus, m_usCmd);

    CResponseGuildSign stRsp;
    stRsp.m_stRewardItem = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildSignUpgrade(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossGuildUpGradeSign stParam;
    if(!DecodeMsgPack(stFormatData, stParam)) return (false);

    SendToClient(stParam, SERVER_ERRCODE::SUCCESS);
    return true;
}

/////////////////////////////////////////////////////跨服转发消息///////////////////////////////////////////////////////
bool CLogicGuildProcessor::DoServerRunGuildWishCrossCheck(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossGuildWishOp stCross;
    if(!DecodeMsgPack(stFormatData, stCross)) return (false);

    if(!GuildWishDstCheck(m_pUserInfo, stCross))
    {
        stCross.m_iRetCode = CLogicError::GetErrCode();
    }
    stCross.m_stUserInfo = CLogicCacheCommon::GetUserDetailInfo(m_iGroupID, m_iUin);
    SendToCrossServer(enmCmdType_cross, MSG_CROSS_GUILD_WISH_CROSS_CHECK_RESULT, stCross, stCross.m_iUin, stCross.m_iGroupID);

    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildWishCrossCheckResult(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossGuildWishOp stCross;
    if(!DecodeMsgPack(stFormatData, stCross)) return (false);

    if(stCross.m_iRetCode != SERVER_ERRCODE::SUCCESS)
    {
        msgpack::sbuffer stResultData;
        GetConnectorSender()->NotifyClientBuffer(m_iGroupID, m_iUin, stCross.m_usCmd, m_stUserConnectorData.m_usConnectionIndex, stCross.m_iRetCode, stResultData);
        return true;
    }

    if(!GuildWishSelfConsume(stCross))
    {
        msgpack::sbuffer stResultData;
        GetConnectorSender()->NotifyClientBuffer(m_iGroupID, m_iUin, stCross.m_usCmd, m_stUserConnectorData.m_usConnectionIndex, CLogicError::GetErrCode(), stResultData);
        return true;
    }
    // 记录历史
    CPackGuildFragmentsHistory packGuildHistory;
    packGuildHistory.m_stScrDetail =  CLogicCacheCommon::GetUserDetailInfo(m_iGroupID, m_iUin);
    packGuildHistory.m_stDestDetail = stCross.m_stUserInfo;
    packGuildHistory.m_iCardID = stCross.m_iCardID;
    packGuildHistory.m_iSwapCardID = stCross.m_iSwapCardID;

    auto srcGuildHistory = m_pUserInfo->m_stIBusyData.GetGuildFragmentHistory();
    while (srcGuildHistory.size() > FRAGEMENT_HISTORY_SIZE)
    {
        srcGuildHistory.erase(srcGuildHistory.begin());
    }
    srcGuildHistory.emplace_back(packGuildHistory);
    m_pUserInfo->m_stIBusyData.SetGuildFragmentHistory(srcGuildHistory);

    stCross.m_stUserInfo = packGuildHistory.m_stScrDetail;
    // 记录
    CLogicBill::SaveGuildWish(stCross.m_iStatTime, stCross.m_iUin, stCross.m_iGroupID, stCross.m_iDstUin,
                              stCross.m_iDstGroupID, stCross.m_iCardID, stCross.m_iSwapCardID, 1, 0);

    SendToCrossServer(enmCmdType_cross, MSG_CROSS_GUILD_WISH_CROSS_DST_GET, stCross, stCross.m_iDstUin, stCross.m_iDstGroupID);

    // 直接返回，不用等待对方是否接收成功
    CResponseBody stResultData;
    GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, stCross.m_usCmd, stResultData, SERVER_ERRCODE::SUCCESS);

    // 通用事件
    auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stCross.m_iCardID);
    if(nullptr == pConfig)
    {
        SET_ERR_INFO(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
        return false;
    }
    auto itemConfig = CLogicConfig::GetItemConfig().GetConfig(pConfig->m_iSoulID);
    if(nullptr == pConfig)
    {
        SET_ERR_INFO(SERVER_ERRCODE::UNKNOW_GAME_ITEM_NOT_IN_CONFIG);
        return false;
    }
    CLogicEventData stEventData;
    stEventData.m_eType = CLogicConfigDefine::ECET_PresentGuildWish;
    stEventData.m_iCmd = m_usCmd;
    stEventData.m_iPara = itemConfig ? itemConfig->m_cGrade : 0;
    LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildWishCrossDstGet(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossGuildWishOp stCross;
    if(!DecodeMsgPack(stFormatData, stCross)) return (false);

    // 再次检查
    if(GuildWishDstCheck(m_pUserInfo, stCross))
    {
        // 接收赠送或者交换
        if(GuildWishDstGetCard(m_pUserInfo, stCross))
        {
            // 记录历史
            CPackGuildFragmentsHistory packGuildHistory;
            packGuildHistory.m_stScrDetail = stCross.m_stUserInfo;
            packGuildHistory.m_stDestDetail = CLogicCacheCommon::GetUserDetailInfo(m_iGroupID, m_iUin);
            packGuildHistory.m_iCardID = stCross.m_iCardID;
            packGuildHistory.m_iSwapCardID = stCross.m_iSwapCardID;

            auto srcGuildHistory = m_pUserInfo->m_stIBusyData.GetGuildFragmentHistory();
            while (srcGuildHistory.size() > FRAGEMENT_HISTORY_SIZE)
            {
                srcGuildHistory.erase(srcGuildHistory.begin());
            }
            srcGuildHistory.emplace_back(packGuildHistory);
            m_pUserInfo->m_stIBusyData.SetGuildFragmentHistory(srcGuildHistory);

            // 记录
            CLogicBill::SaveGuildWish(stCross.m_iStatTime, stCross.m_iUin, stCross.m_iGroupID, stCross.m_iDstUin,
                                      stCross.m_iDstGroupID, stCross.m_iCardID, stCross.m_iSwapCardID, 0, 1);
        }
    }

    return true;
}

bool CLogicGuildProcessor::DoServerRunGetBossHurtRank(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGetGuildBossHurtRank stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return false;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunBossSitBar(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildSitBar stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return false;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGetBossSitBarReward(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CCrossGuildGetSitBarReward cross;
    if (!DecodeMsgPack(stFormatData, cross)) return false;

    CResponseGetGuildSitBarReward stRsp;
    auto guidBarConfig = CLogicConfig::GetGuildConfig().m_stGuildBarConfig;

    if (cross.m_iCardID != guidBarConfig.m_iSpecialCardID)
    {
        CLogicCacheCommon::AddGameItem(m_pUserInfo, guidBarConfig.m_stReward, m_usCmd);
    } else
    {
        CLogicCacheCommon::AddGameItem(m_pUserInfo, guidBarConfig.m_stSpecialReward, m_usCmd);
    }
    m_pUserInfo->m_stIBusyData.SetDailyGuildBar(1);
    stRsp.m_stReward = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunUnlockCollection(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseUnlockCollection stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return false;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunSetDecorate(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseSetDecorate stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return false;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunSaveGuildDecorate(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildSave stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return false;

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildWarInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildWarInfo stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return false;
    stRsp.m_stUsedCard = m_pUserInfo->m_stIBusyData.GetGuildWarUseCard();
    stRsp.m_iDailyCount = m_pUserInfo->m_stIBusyData.GetDailyGuildWarTime();
    stRsp.m_stSelfGuildWarInfo = m_pUserInfo->m_stIBusyData.GetGuildSelfInfo();
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildBegin(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildWarBegin stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return false;

    m_pUserInfo->m_stUserInfoTable.SetFightLevelID(stRsp.m_iLevelID);
    //记录最近一个关卡的时间戳
    m_pUserInfo->m_stIBusyData.SetStartLevelTime(CLogicCommon::GetSec());

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildEnd(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildWarEnd stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return false;
    auto userCard = m_pUserInfo->m_stIBusyData.GetGuildWarUseCard();
    for(auto usedCard: stRsp.m_stUseCard)
    {
        userCard.insert(std::make_pair(usedCard,1));
    }
    //记录已经使用的card
    m_pUserInfo->m_stIBusyData.SetGuildWarUseCard(userCard);

    //累计每日挑战次数
    m_pUserInfo->m_stIBusyData.AddDailyGuildWarTime(1);

    //累计有效伤害
    auto guildSelfInfo = m_pUserInfo->m_stIBusyData.GetGuildSelfInfo();
    guildSelfInfo[stRsp.m_iLevelID].m_iHurt += stRsp.m_iHurt;

    //挑战记录
    m_pUserInfo->m_stIBusyData.SetGuildSelfInfo(guildSelfInfo);

    //更新玩家本期参与公会战信息
    CPackGuildWarSelfRankInfo selfRankInfo;
    selfRankInfo.m_iRankID = CLogicConfig::GetGuildConfig().m_iRankID;
    selfRankInfo.m_iGuildID = m_pUserInfo->m_stUserInfoTable.GetGuildID();
    selfRankInfo.m_iRank = stRsp.m_iCurGuildRank;
    selfRankInfo.m_iGuildWarEndTime = CLogicConfig::GetGuildConfig().m_iGuildWarEndTime;

    //记录异常
    auto passLevelTime = CLogicCommon::GetSec() - m_pUserInfo->m_stIBusyData.GetStartLevelTime();
    CPackCardTeam usedCard;
    for(auto useCard : stRsp.m_stUseCard)
    {
        usedCard.push_back(useCard);
    }

    std::unordered_map<int32_t,int64_t> unorderedMapCardFight;
    for(const auto& card: usedCard)
    {
        auto cardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(card);
        auto cardInfo = m_pUserInfo->m_stHeroCardTableMap.Find(card);
        if (cardConfig != nullptr && cardInfo != m_pUserInfo->m_stHeroCardTableMap.End())
        {
            int64_t lCardFight = 0;
            for(const auto& attr : cardConfig->m_stStarUpAttr[cardInfo->second->GetCardStar()].m_stAttrVec)
            {
                if (attr.first == CLogicConfigDefine::ECAT_Attack)
                {
                    lCardFight += attr.second;
                }
            }
            PROCESSOR_LOG_DEBUG << "Card_ID: " << card
                                << "Card_fight: " << lCardFight
                                << std::endl;
            unorderedMapCardFight.insert(make_pair(card,lCardFight));
        }
    }

    int64_t lDps = stRsp.m_iHurt / passLevelTime;
    int64_t totalFight = 0;
    for(const auto& cardFight : unorderedMapCardFight)
    {
        totalFight += cardFight.second;
    }

    int64_t lMaxTotalFight = totalFight * CLogicConfig::GetAwardConfig().iAntiCheatingRatio;

    //通关时间异常或者伤害异常记录
    const int32_t iPassTime = 90;
    if (passLevelTime < iPassTime || lDps > lMaxTotalFight)
    {
        CLogicBill::SaveAntiCheatingBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, stRsp.m_iLevelID,lDps, lMaxTotalFight, usedCard, "",passLevelTime);
    }
    PROCESSOR_LOG_DEBUG << "GUILD_WAR_END: m_iGuildWarEndTime: " << selfRankInfo.m_iRankID
                        << "| m_iGuildID: " << selfRankInfo.m_iGuildID
                        << "| m_iRank: " << selfRankInfo.m_iRank << std::endl;
    m_pUserInfo->m_stIBusyData.SetGuildWarCalInfo(selfRankInfo);

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildGetRankInfo(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildWarGetRankInfo stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return false;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGuildProcessor::DoServerRunGuildUpdateGuildWar(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CResponseGuildWarUpdateRankInfo stRsp;
    if (!DecodeMsgPack(stFormatData, stRsp)) return false;

    //更新玩家最近一次的公会战参加结算情况
    auto stRankCalInfo = m_pUserInfo->m_stIBusyData.GetGuildWarCalInfo();
    stRankCalInfo.m_iRank = stRsp.m_stRankSelf.m_iRank;

    m_pUserInfo->m_stIBusyData.SetGuildWarCalInfo(stRankCalInfo);
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CLogicGuildProcessor::GuildWishNotifyCross(user_data_table_ptr_type pUser,bool isFull)
{
    // 如果当天还未发布祈愿，则不同步
    if(LOGIC_CHECK_DAY_REFRESH(pUser->m_stIBusyData.GetWishLastTime()))
        return;

    // 同步跨服
    CPackGuildWish stNotify;
    stNotify.m_iCardID = pUser->m_stIBusyData.GetWishCardID();
    stNotify.m_iCurNum = pUser->m_stIBusyData.GetWishNum();
    stNotify.m_bSwap = !pUser->m_stIBusyData.GetWishSwapCard().empty();
    stNotify.m_bIsFull = isFull;
    SendToCrossServer(enmCmdType_request, MSG_CROSS_GUILD_WISH_NOTIFY, stNotify, pUser->m_iUin, pUser->m_iGroupID);
}

bool CLogicGuildProcessor::GuildWishGetSwapInfo(int iUin, int iGroupID, CResponseGuildWishGetOtherSwapInfo& stRsp)
{
    auto pDstUser = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
    if(nullptr == pDstUser)
    {
        SET_ERR_INFO(SERVER_ERRCODE::USER_LEVEL_FORMAT_ERROR);
        return false;
    }

    if(LOGIC_CHECK_DAY_REFRESH(pDstUser->m_stIBusyData.GetWishLastTime()))
    {
        SET_ERR_INFO(SERVER_ERRCODE::WISH_NOT_DONE);
        return false;
    }

    stRsp.m_iCardID = pDstUser->m_stIBusyData.GetWishCardID();
    stRsp.m_iCurNum = pDstUser->m_stIBusyData.GetWishNum();
    for(auto card_id : pDstUser->m_stIBusyData.GetWishSwapCard())
    {
        auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(card_id);
        if(nullptr == pConfig) continue;

        stRsp.m_stSwapCardMap[card_id] = CLogicCacheCommon::GetItemNum(pDstUser, CLogicConfigDefine::EXCHANGE_ITEM, pConfig->m_iSoulID);
    }

    return true;
}

bool CLogicGuildProcessor::GuildWishHandleCheck(CRequestGuildWishSwap& stReq)
{
    if(stReq.m_iDstUin <= 0 || stReq.m_iDstGroupID <= 0 || stReq.m_iDstUin == m_iUin || stReq.m_iCardID <= 0)
    {
        SET_ERR_INFO(SERVER_ERRCODE::WISH_NOT_DONE);
        return false;
    }

    // 操作间隔
    if(CLogicCommon::GetSec() - m_pUserInfo->m_stOnlyInCacheData.m_iGuildWishLastTime <= 5)
    {
        SET_ERR_INFO(SERVER_ERRCODE::OP_TIMELIMIT_ERR);
        return false;
    }

    m_pUserInfo->m_stOnlyInCacheData.m_iGuildWishLastTime = CLogicCommon::GetSec();

    auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stReq.m_iCardID);
    if(nullptr == pCardConfig)
    {
        SET_ERR_INFO(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
        return false;
    }

    if(CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, pCardConfig->m_iSoulID) <= 0)
    {
        SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_HERO_CARD_SOUL);
        return false;
    }

    if(stReq.m_iSwapCardID > 0)
    {
        if(m_pUserInfo->m_stUserDailyTable.GetWishSwapTimes() >= CLogicConfig::GetGuildConfig().m_iWishSwapTimes)
        {
            SET_ERR_INFO(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
            return false;
        }

        if(!CLogicConfig::GetHeroCardConfig().GetConfig(stReq.m_iSwapCardID))
        {
            SET_ERR_INFO(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
            return false;
        }
    }

    CCrossGuildWishOp stCross;
    stCross.m_iUin = m_iUin;
    stCross.m_iGroupID = m_iGroupID;
    stCross.m_iDstUin = stReq.m_iDstUin;
    stCross.m_iDstGroupID = stReq.m_iDstGroupID;
    stCross.m_iCardID = stReq.m_iCardID;
    stCross.m_iSwapCardID = stReq.m_iSwapCardID;
    stCross.m_usCmd = m_usCmd;
    stCross.m_iStatTime = CLogicCommon::GetSec();
    stCross.m_strNick = m_pUserInfo->m_strNick;

    // 本服玩家
    if(CLogicConfig::GetSystemConfig().isSelfGroup(stReq.m_iDstGroupID))
    {
        auto pDstUser = CLogicCacheCommon::LoadUserData(stReq.m_iDstGroupID, stReq.m_iDstUin);
        if(nullptr == pDstUser)
        {
            SET_ERR_INFO(SERVER_ERRCODE::USER_LEVEL_FORMAT_ERROR);
            return false;
        }

        // 检查发布者数据
        if(!GuildWishDstCheck(pDstUser, stCross))
        {
            return false;
        }

        // 扣除自己碎片
        if(!GuildWishSelfConsume(stCross))
        {
            return false;
        }

        // 发布者获得碎片
        GuildWishDstGetCard(pDstUser, stCross);

        // 记录历史
        CPackGuildFragmentsHistory packGuildHistory;
        packGuildHistory.m_stScrDetail =  CLogicCacheCommon::GetUserDetailInfo(m_iGroupID, m_iUin);
        packGuildHistory.m_stDestDetail = CLogicCacheCommon::GetUserDetailInfo(stReq.m_iDstGroupID, stReq.m_iDstUin);
        packGuildHistory.m_iCardID = stReq.m_iCardID;
        packGuildHistory.m_iSwapCardID = stReq.m_iSwapCardID;

        auto guildHistory = pDstUser->m_stIBusyData.GetGuildFragmentHistory();
        while (guildHistory.size() > FRAGEMENT_HISTORY_SIZE)
        {
            guildHistory.erase(guildHistory.begin());
        }
        guildHistory.emplace_back(packGuildHistory);
        pDstUser->m_stIBusyData.SetGuildFragmentHistory(guildHistory);

        auto srcGuildHistory = m_pUserInfo->m_stIBusyData.GetGuildFragmentHistory();
        while (srcGuildHistory.size() > FRAGEMENT_HISTORY_SIZE)
        {
            srcGuildHistory.erase(srcGuildHistory.begin());
        }
        srcGuildHistory.emplace_back(packGuildHistory);
        m_pUserInfo->m_stIBusyData.SetGuildFragmentHistory(guildHistory);

        SendToClient(SERVER_ERRCODE::SUCCESS);
        // 记录
        CLogicBill::SaveGuildWish(stCross.m_iStatTime, stCross.m_iUin, stCross.m_iGroupID, stCross.m_iDstUin,
                                  stCross.m_iDstGroupID, stCross.m_iCardID, stCross.m_iSwapCardID, 1, 1);


        // 通用事件
        auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stCross.m_iCardID);
        if(nullptr == pConfig)
        {
            SET_ERR_INFO(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
            return false;
        }
        auto itemConfig = CLogicConfig::GetItemConfig().GetConfig(pConfig->m_iSoulID);
        if(nullptr == pConfig)
        {
            SET_ERR_INFO(SERVER_ERRCODE::UNKNOW_GAME_ITEM_NOT_IN_CONFIG);
            return false;
        }
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_PresentGuildWish;
        stEventData.m_iCmd = m_usCmd;
        stEventData.m_iPara = itemConfig ? itemConfig->m_cGrade : 0;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
        return true;
    }

    // 其他服玩家
    SendToCrossServer(enmCmdType_cross, MSG_CROSS_GUILD_WISH_CROSS_CHECK, stCross, stCross.m_iDstUin, stCross.m_iDstGroupID);

    return true;
}

bool CLogicGuildProcessor::GuildWishDstCheck(user_data_table_ptr_type pDstUser, const CCrossGuildWishOp& stPara)
{
    if(LOGIC_CHECK_DAY_REFRESH(pDstUser->m_stIBusyData.GetWishLastTime()))
    {
        SET_ERR_INFO(SERVER_ERRCODE::WISH_NOT_DONE);
        return false;
    }

    if(pDstUser->m_stIBusyData.GetWishCardID() != stPara.m_iCardID)
    {
        SET_ERR_INFO(SERVER_ERRCODE::WISH_NOT_MATCH);
        return false;
    }

    auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stPara.m_iCardID);
    if(nullptr == pCardConfig)
    {
        SET_ERR_INFO(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
        return false;
    }

    auto pWishConfig = CLogicConfig::GetGuildConfig().GetWishConfig(pCardConfig->m_iInitGrade);
    if(nullptr == pWishConfig)
    {
        SET_ERR_INFO(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
        return false;
    }

    if(pDstUser->m_stIBusyData.GetWishNum() >= pWishConfig->m_iWishCount)
    {
        SET_ERR_INFO(SERVER_ERRCODE::WISH_IS_FULL);
        return false;
    }

    // 交换
    if(stPara.m_iSwapCardID > 0)
    {
        if (pDstUser->m_stUserDailyTable.GetWishSwapTimes() >= CLogicConfig::GetGuildConfig().m_iWishSwapTimes)
        {
            SET_ERR_INFO(SERVER_ERRCODE::WISH_OTHER_IS_FULL);
            return false;
        }

        auto& stSwapVec = pDstUser->m_stIBusyData.GetWishSwapCard();
        if(std::find(stSwapVec.begin(), stSwapVec.end(), stPara.m_iSwapCardID) == stSwapVec.end())
        {
            SET_ERR_INFO(SERVER_ERRCODE::WISH_SWAP_CARD_NOT_MATCH);
            return false;
        }

        auto pSwapCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stPara.m_iSwapCardID);
        if(nullptr == pSwapCardConfig)
        {
            SET_ERR_INFO(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
            return false;
        }

        if(CLogicCacheCommon::GetItemNum(pDstUser, CLogicConfigDefine::EXCHANGE_ITEM, pSwapCardConfig->m_iSoulID) <= 0)
        {
            SET_ERR_INFO(SERVER_ERRCODE::WISH_DST_CARD_NOT_ENOUGH);
            return false;
        }
    }

    return true;
}

bool CLogicGuildProcessor::GuildWishDstGetCard(user_data_table_ptr_type pDstUser, const CCrossGuildWishOp& stPara)
{
    // 交换
    if(stPara.m_iSwapCardID > 0)
    {
        auto pSwapCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stPara.m_iSwapCardID);
        if(nullptr == pSwapCardConfig)
        {
            SET_ERR_INFO(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
            return false;
        }

        CPackGameItem stItem(CLogicConfigDefine::EXCHANGE_ITEM, pSwapCardConfig->m_iSoulID, 1);
        if(!CLogicCacheCommon::ConsumeGameItem(pDstUser, stItem, m_usCmd))
        {
            SET_ERR_INFO(SERVER_ERRCODE::WISH_DST_CARD_NOT_ENOUGH);
            return false;
        }

        // 道具消耗通知
        CLogicUserNotifySystem::NotifyItemChange(pDstUser, {}, {stItem});
        pDstUser->m_stUserDailyTable.AddWishSwapTimes(1);
    }

    auto stVec = pDstUser->m_stIBusyData.GetWishRewardVec();
    stVec.emplace_back(stPara.m_strNick, stPara.m_iSwapCardID);
    pDstUser->m_stIBusyData.SetWishRewardVec(stVec);
    pDstUser->m_stIBusyData.AddWishNum(1);
    //三星卡不管是被赠送还是被交换都加次数
    if (stPara.m_iSwapCardID <= 0)
    {
        auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stPara.m_iCardID);
        if(nullptr == pCardConfig)
        {
            SET_ERR_INFO(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
            return false;
        }
        if (pCardConfig->m_iInitGrade >= CLogicConfigDefine::ECGT_UR)
        {
            pDstUser->m_stUserDailyTable.AddWishSwapTimes(1);
        }
    }


    PROCESSOR_LOG_DEBUG << "ITEM_LOG ACCEPT|CARD_ID:" << stPara.m_iCardID << "|DST_UIN:" << stPara.m_iDstUin << "|DST_GROUP_ID:" << stPara.m_iDstGroupID << "|SWAP_CARD_ID:" << stPara.m_iSwapCardID << std::endl;

    GuildWishNotifyCross(pDstUser);

    return true;
}

bool CLogicGuildProcessor::GuildWishSelfConsume(const CCrossGuildWishOp& stPara)
{
    if(stPara.m_iSwapCardID > 0 && m_pUserInfo->m_stUserDailyTable.GetWishSwapTimes() >= CLogicConfig::GetGuildConfig().m_iWishSwapTimes)
    {
        SET_ERR_INFO(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
        return false;
    }

    // 扣除自己的伙伴碎片
    auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stPara.m_iCardID);
    if(nullptr == pConfig)
    {
        SET_ERR_INFO(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED);
        return false;
    }

    CPackGameItem stConsume(CLogicConfigDefine::EXCHANGE_ITEM, pConfig->m_iSoulID, 1);
    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stConsume, m_usCmd))
    {
        return false;
    }

    std::vector<CPackGameItem> stGets;
    if(stPara.m_iSwapCardID > 0)
    {
        auto pSwapConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stPara.m_iSwapCardID);
        if(pSwapConfig)
        {
            stGets.emplace_back(CLogicConfigDefine::EXCHANGE_ITEM, pSwapConfig->m_iSoulID, 2);
        }
        m_pUserInfo->m_stUserDailyTable.AddWishSwapTimes(1);
    }

    auto pWishConfig = CLogicConfig::GetGuildConfig().GetWishConfig(pConfig->m_iInitGrade);
    if(pWishConfig && pWishConfig->m_iRewardGuildCoin > 0)
    {
        stGets.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GuildCoinID, pWishConfig->m_iRewardGuildCoin);
    }

    CLogicCacheCommon::AddGameItem(m_pUserInfo, stGets, m_usCmd);

    PROCESSOR_LOG_DEBUG << "ITEM_LOG SWAP|CARD_ID:" << stPara.m_iCardID << "|DST_UIN:" << stPara.m_iDstUin << "|DST_GROUP_ID:" << stPara.m_iDstGroupID << "|SWAP_CARD_ID:" << stPara.m_iSwapCardID << std::endl;

    //今日设置过卡且本次是交换,就通知跨服
    if(!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stIBusyData.GetWishLastTime()) && stPara.m_iSwapCardID > 0)
    {
        GuildWishNotifyCross(m_pUserInfo, true);
    }

    // 道具通知
    CLogicUserNotifySystem::NotifyItemChange(m_pUserInfo, stGets, {stConsume}, !stGets.empty());

    return true;
}

void CLogicGuildProcessor::GuildWishCheckAcceptCard()
{
    if(!m_pUserInfo->m_stIBusyData.GetWishRewardVec().empty())
    {
        auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(m_pUserInfo->m_stIBusyData.GetWishCardID());
        if (pConfig)
        {
            CPackGameItem stItem(CLogicConfigDefine::EXCHANGE_ITEM, pConfig->m_iSoulID, (2 * m_pUserInfo->m_stIBusyData.GetWishRewardVec().size()));
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stItem, m_usCmd);

            // 道具通知
            CResponseGuildWishRewardNotify notify;
            notify.m_iCardID = m_pUserInfo->m_stIBusyData.GetWishCardID();
            notify.m_stReward = stItem;
            notify.m_stLog = m_pUserInfo->m_stIBusyData.GetWishRewardVec();
            GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, MSG_LOGIC_GUILD_WISH_REWARD_NOTIFY, notify);
        }

        m_pUserInfo->m_stIBusyData.SetWishRewardVec({});
    }
}


bool CLogicGuildProcessor::CheckGuildWarOpen()
{
    if(CLogicCommon::GetSec() >= CLogicConfig::GetGuildConfig().m_iGuildWarStartTime &&
        CLogicCommon::GetSec() <= CLogicConfig::GetGuildConfig().m_iGuildWarEndTime)
    {
        return true;
    }
    return false;
}