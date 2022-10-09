#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_user_protocol.h"
#include "logic_link_error.h"
#include "logic_link_common.h"
#include "logic_game_active_info.h"
#include "logic_config.h"
#include "logic_game_multi_step_task.h"
#include "logic_game_processor_base.h"
#include "logic_game_cache_common.h"
#include "logic_game_day_refresh_system.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_monitor.h"
#include "logic_game_global.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_version_data_update.h"

static const int64_t SLOW_LOG_TIME_LIMIT = 20000;

CLogicProcessorBase::CLogicProcessorBase(uint16_t usCmd, const std::string& strCmdName)
	: m_usCmd(usCmd), m_strCmdName(strCmdName), m_iUin(0), m_iGroupID(0), m_stUserConnectorData(), m_pUserInfo(nullptr)
{
    CLogicBill::RefreshBillRation(m_usCmd, strCmdName);
};

CLogicProcessorBase::~CLogicProcessorBase() { };

void CLogicProcessorBase::SetClientUser(int32_t iUin, int32_t iGroupID, int32_t iChannelID)
{
	m_iUin = iUin;
	m_iGroupID = iGroupID;
	m_iChannelID = iChannelID;
}

ILogicConnectorSender* CLogicProcessorBase::GetConnectorSender()
{
	return LOGIC_SERVICE_API_INS->GetConnectorSender();
}

ILogicTCPConnection* CLogicProcessorBase::GetCrossConnection()
{
	return LOGIC_SERVICE_API_INS->GetCrossConnection();
}

void CLogicProcessorBase::SendToClient(int iErrorCode)
{
    if (iErrorCode != SERVER_ERRCODE::SUCCESS)
    {
        msgpack::sbuffer stResultData;
        GetConnectorSender()->NotifyClientBuffer(m_iGroupID, m_iUin, m_usCmd, m_stUserConnectorData.m_usConnectionIndex, iErrorCode, stResultData);
    }
    else
    {
        CResponseBody stResultData;
        SendToClient(stResultData, iErrorCode);
    }
}

void CLogicProcessorBase::SendSuccessCacheItemToClient()
{
    if(m_pUserInfo)
    {
        CResponseCacheItemBody stResult;
        stResult.m_stCacheItem = m_pUserInfo->m_stCacheBonusData;
        SendToClient(stResult, SERVER_ERRCODE::SUCCESS);
    }
}

bool CLogicProcessorBase::DoUserProcess(const char *pBuffer, unsigned int uiLength)
{
    CLogicMultiStepTaskBase::GetChangedMutiStepTask().clear();

    static int64_t lStartMicroSecond = 0;
    static int64_t lEndMicroSecond = 0;
    static int64_t lCostMicroSecond = 0;

    lStartMicroSecond = taf::TC_Common::now2us();

	if (!CLogicConfig::GetSystemConfig().isSelfGroup(m_iGroupID))
	{
		PROCESSOR_LOG_ERROR << "USER UIN:" << m_iUin << " GROUP_ID:" << m_iGroupID << " NOT IN THIS SERVER!!!" << std::endl;
		return (false);
	}

    CLogicUser::const_iterator it = CLogicUser::GetInstance().GetUserInfo(m_iUin);
    if(it == CLogicUser::GetInstance().End())
    {
        PROCESSOR_LOG_ERROR << "CAN NOT FIND USER IN CONNECTOR" << std::endl;
        return (false);
    }

    m_stUserConnectorData = it->second;

    if(GetConnectorSender() == nullptr)
    {
        PROCESSOR_LOG_ERROR << "CONNECTOR SENDER IS INVALID|CAN NOT CONNECTOR TO CONNECTOR" << std::endl;
        return (false);
    }

    msgpack::unpacked stResult;
    if(uiLength > 0)
    {
        try
        {
            msgpack::unpack(&stResult, pBuffer, uiLength);
        }
        catch(const std::runtime_error& stEx)
        {
            PROCESSOR_LOG_ERROR << "USER DATA IS NOT MSGPACK FORMAT|MSG:" << stEx.what() << std::endl;

            GetConnectorSender()->NotifyCloseClient(m_iUin,
                                         m_stUserConnectorData.m_usConnectionIndex, 
                                         m_stUserConnectorData.m_iClientIP, 
                                         m_stUserConnectorData.m_usClientPort);

            return (false);
        }
    }

    if (CLogicConfig::GetSystemConfig().IsPrintPackInfo())
    {
        PrintRecvMsg(stResult);
    }

    // 协议开关判断
    if(CLogicMonitor::ProtoSwitch(m_usCmd))
    {
        PROCESSOR_LOG_DEBUG << "PROTOCOL SWITCH LIMIT | RETURN" << std::endl;
        using namespace SERVER_ERRCODE;
        SendToClient(PROTO_SWITCH_LIMIT);

        return (false);
    }

    if ((int)m_usCmd != MSG_LOGIC_CREATE_ROLE &&
        (int)m_usCmd != MSG_LOGIC_CHECK_ROLE_EXIST &&
        (int)m_usCmd != MSG_LOGIC_CHECK_NICK_CORRECT)
    {
        m_pUserInfo = CLogicCacheCommon::LoadUserData(m_iGroupID, m_iUin);
        if (!m_pUserInfo)
        {
            if ((int)m_usCmd == MSG_LOGIC_RECONNECT)
            {
                CResponseReconnect stProto;
                stProto.m_iSvrTimestamp = CLogicCommon::GetSec();
                SendToClient(stProto, SERVER_ERRCODE::SUCCESS);
                return (true);
            }
            
            using namespace SERVER_ERRCODE;
            int iErrcode = (int)USER_ROLE_NOT_EXIST;
            PROCESSOR_LOG_ERROR << "PROCESS ERROR|ERRCODE:" << iErrcode 
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                                << std::endl;

            SendToClient(iErrcode);
            return (false);
        }

        LOGIC_STATISTICS_INS->UpdatePlayerInfo(m_iUin, m_iGroupID, m_iChannelID, m_pUserInfo->m_stUserInfoTable.GetCreateTime()
                                                , m_pUserInfo->m_strNick, m_pUserInfo->m_stUserInfoTable.GetLevel()
                                                , m_pUserInfo->m_stUserInfoTable.GetRmb(), m_pUserInfo->m_stUserInfoTable.GetAddUpRmb()
                                                , m_pUserInfo->m_stUserInfoTable.GetRoleCombatMax());
    }

    if (m_pUserInfo)
    {
        // 游客模式防沉迷
        if(m_usCmd != MSG_LOGIC_GET_USER_INFO && m_usCmd != MSG_LOGIC_UPDATE_USER_AGE)
        {
            if(m_pUserInfo->m_stUserInfoTable.GetAge() == 0 && CLogicGlobal::AntiAddHandleVisitor(m_iUin, m_iGroupID))
            {
                // 请求更新一次年龄
                CLogicGlobal::AntiAddNotifyUpdateUserAge(m_iUin, m_iGroupID);

                PROCESSOR_LOG_DEBUG << "ANTI_ADDICTION:AntiAddHandleVisitor return true Prevent Login"
                                    << "|MSG:User Age:" << m_pUserInfo->m_stUserInfoTable.GetAge() << std::endl;
                return false;
            }
        }

        if (CLogicCacheCommon::IsBanAccount(m_pUserInfo))
        {
            CResponseUserBanError stRsp;

            stRsp.m_iBanBeginTime = m_pUserInfo->m_stUserInfoTable.GetBanBeginTime();
            stRsp.m_iBanEndTime = m_pUserInfo->m_stUserInfoTable.GetBanEndTime();
    
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "PROCESS ERROR|ERRCODE:" << USER_IS_IN_BAN_TIME 
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_IS_IN_BAN_TIME)
                                << "|BEGIN_TIME:" << m_pUserInfo->m_stUserInfoTable.GetBanBeginTime()
                                << "|END_TIME:" << m_pUserInfo->m_stUserInfoTable.GetBanEndTime()
                                << std::endl;

            SendToClient(stRsp, USER_IS_IN_BAN_TIME);

			GetConnectorSender()->NotifyCloseClient(m_iUin,
				m_stUserConnectorData.m_usConnectionIndex,
				m_stUserConnectorData.m_iClientIP,
				m_stUserConnectorData.m_usClientPort);

            return (false);
        }

		if (m_usCmd != MSG_LOGIC_RECONNECT && m_usCmd != MSG_LOGIC_GET_USER_INFO
			&& CLogicConfig::GetSystemConfig().isServerMaintain() && !CLogicConfig::GetZooLogicPlatformConfig().isWhiteAccount(m_pUserInfo->m_strPid))
		{
			SendToClient(SERVER_ERRCODE::SERVER_MAINTAIN);
			return (false);
		}

        m_pUserInfo->m_iCommand = m_usCmd;
        m_pUserInfo->m_iChannelID = m_iChannelID;

        CLogicCacheCommon::AutoAddPhyPower(m_pUserInfo);

        // 放在自动恢复体力之后，以防自动恢复的体力加到奖励列表里
		m_pUserInfo->m_stCacheBonusData = {};

		CLogicDayRefreshSystem::CheckUserDataRefresh(m_pUserInfo);

        // 版本更新，部分老数据需要转换成新数据
        CLogicVersionDataUpdateSystem::CheckVersionDataUpdate(m_pUserInfo);
    }

	if (m_stUserConnectorData.m_iResVersion > 0 && m_stUserConnectorData.m_iResVersion < CLogicConfig::GetZooLogicGameConfig().m_iMinResVersion)
	{
		PROCESSOR_LOG_ERROR << "CLIENT RES VERSION:" << m_stUserConnectorData.m_iResVersion << " TOO LOW, MUST MORE THAN " << CLogicConfig::GetZooLogicGameConfig().m_iMinResVersion << std::endl;
		SendToClient(SERVER_ERRCODE::RES_VER_TOO_LOW);
		return (false);
	}

    PROCESSOR_LOG_DEBUG << "PROCESS START" << std::endl;

    bool bRet = DoUserRun(stResult);

    ///////////////// Task Delegate
    if(m_pUserInfo && !m_pUserInfo->m_stOnlyInCacheData.m_stCacheQuestIDSet.empty())
    {
        CLogicUserNotifySystem::NotifyQuestData(m_pUserInfo, m_pUserInfo->m_stOnlyInCacheData.m_stCacheQuestIDSet);
        m_pUserInfo->m_stOnlyInCacheData.m_stCacheQuestIDSet.clear();
    }

    if(m_pUserInfo && !m_pUserInfo->m_stOnlyInCacheData.m_stCacheBadgeIDSet.empty())
    {
        CLogicUserNotifySystem::NotifyBadgeData(m_pUserInfo, m_pUserInfo->m_stOnlyInCacheData.m_stCacheBadgeIDSet);
        m_pUserInfo->m_stOnlyInCacheData.m_stCacheBadgeIDSet.clear();
    }

    /////////////////////////////////////////////////////////////////////
    ///////////////// Activity Delegate
    LOGIC_ACTIVITY_CHANGE_LIST_INS->NotifyClientUser(m_iGroupID, m_iUin);

    /////////////////////////////////////////////////////////////////////
    ///////////////// Broadcast Delegate
	/*RK_LOGIC_BROADCAST_SYSTEM_INS->RefreshSystemBulletins();
	RK_LOGIC_BROADCAST_SYSTEM_INS->NotifyClientUser();*/

    /////////////////////////////////////////////////////////////////////
    // 协议监控
    if(bRet && m_pUserInfo) CLogicMonitor::ProtoMonitor(m_pUserInfo, m_usCmd, m_strCmdName);

    ///////////////// Mysql Delegate
    LOGIC_RECORD_CACHE_INS->ExecSQL(m_iGroupID, m_iUin, m_strCmdName);

    lEndMicroSecond = taf::TC_Common::now2us();
    lCostMicroSecond = lEndMicroSecond - lStartMicroSecond;

    /////////////////////////////////////////////////////////////////////
    ///////////////// Slow Log
    if (m_strCmdName != "MSG_LOGIC_CREATE_ROLE" && lCostMicroSecond > SLOW_LOG_TIME_LIMIT)
    {
        LOGIC_LOG_SLOW_INFO << "GROUP_ID:" << m_iGroupID
                      << "|UIN:" << m_iUin << "|CMD:" << m_usCmd
                      << "|CMD_NAME:" << m_strCmdName << "|CONSUME_MICRO_TIME:" 
                      << lCostMicroSecond << std::endl;
    }

    PROCESSOR_LOG_DEBUG << "REQUEST_CONSUME_MICRO_TIME:" << lCostMicroSecond << std::endl;

#if LOGIC_BUILD_PLATFORM == BUILD_PLATFORM_LOCAL
    PROCESSOR_LOG_DEBUG << "OFFSET TIME:" << CLogicCommon::GetSec() << "|DATETIME:" << CLogicCommon::GetDateTimeString() << std::endl;
#endif

    return (bRet);
}

bool CLogicProcessorBase::DoServerProcess(const CMsgHeadCross &stMsgHead, const char *pBuffer, unsigned int uiLength)
{
	static int64_t lStartMicroSecond = 0;
	static int64_t lEndMicroSecond = 0;
	static int64_t lCostMicroSecond = 0;

	lStartMicroSecond = taf::TC_Common::now2us();

	m_iUin = stMsgHead.m_iUin;
	m_iGroupID = stMsgHead.m_nGroupID;

	const int32_t iGroupID = stMsgHead.m_nGroupID;
	const int32_t iUin = stMsgHead.m_iUin;
	
	CLogicUser::const_iterator it = CLogicUser::GetInstance().GetUserInfo(m_iUin);
	if (it != CLogicUser::GetInstance().End())
	{
		m_stUserConnectorData = it->second;
	}

	if (stMsgHead.m_nSvrRsp != SERVER_ERRCODE::SUCCESS)
	{
		SendToClient(stMsgHead.m_nSvrRsp);
		return true;
	}

	if (m_usCmd != MSG_CROSS_ATTACK_DEFENSE_AWARD
		&& m_usCmd != MSG_CROSS_ATTACK_DEFENSE_START
		&& m_usCmd != MSG_CROSS_GUILD_WAR_REPORT_NOTIFY
		&& m_usCmd != MSG_CROSS_NOTIFY_CROSS_BROADCAST
        && m_usCmd != MSG_CROSS_ATTACK_DEFENSE_PREPARE
        && m_usCmd != MSG_CROSS_ADMIN_MODIFY_GUILD
        && m_usCmd != MSG_CROSS_ADMIN_DISMISS_GUILD
        && m_usCmd != MSG_CROSS_ADMIN_SET_GUILD_CHIEF
        && m_usCmd != MSG_CROSS_ADMIN_CHANGE_GUILD_NAME
        && m_usCmd != MSG_CROSS_GUILD_BOSS_REWARD_NOTIFY
        && m_usCmd != MSG_CROSS_GUILD_CHANGE_NOTIFY
        && m_usCmd != MSG_CROSS_USER_TALK_NOTIFY)
	{
		if (!CLogicConfig::GetSystemConfig().isSelfGroup(m_iGroupID))
		{
			PROCESSOR_LOG_ERROR << "USER UIN:" << iUin << " GROUP_ID:" << iGroupID << " NOT IN THIS SERVER!!!" << std::endl;

			return (false);
		}

		m_pUserInfo = CLogicCacheCommon::LoadUserData(m_iGroupID, m_iUin);
		if (!m_pUserInfo)
		{
			PROCESSOR_LOG_ERROR << "PROCESS ERROR|ERRCODE:" << SERVER_ERRCODE::USER_ROLE_NOT_EXIST
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
				<< "USER UIN:" << iUin << " GROUP_ID:" << iGroupID 
				<< "|CMD:" << stMsgHead.m_usCmd  << "|CMD_NAME:"<< m_strCmdName
				<< std::endl;

			return (false);
		}

		m_pUserInfo->m_stCacheBonusData = {};
		m_pUserInfo->m_iCommand = m_usCmd;
	}

	msgpack::unpacked stResult;
	if (uiLength > 0)
	{
		try
		{
			msgpack::unpack(&stResult, pBuffer, uiLength);
		}
		catch (const std::runtime_error& stEx)
		{
			PROCESSOR_LOG_ERROR << "USER DATA IS NOT MSGPACK FORMAT|MSG:" << stEx.what() << std::endl;

			return (false);
		}
	}

	PROCESSOR_LOG_DEBUG << "PROCESS START" << std::endl;

	const bool bRet = DoServerRun(stMsgHead, stResult);

	/////////////////////////////////////////////////////////////////////
	///////////////// Mysql Delegate
	LOGIC_RECORD_CACHE_INS->ExecSQL(iGroupID, iUin, m_strCmdName);

	lEndMicroSecond = taf::TC_Common::now2us();
	lCostMicroSecond = lEndMicroSecond - lStartMicroSecond;

	PROCESSOR_LOG_DEBUG << "REQUEST_CONSUME_MICRO_TIME:" << lCostMicroSecond << std::endl;
	return (bRet);
}

bool CLogicProcessorBase::DoServerProcess(const CMsgHeadQueue &stMsgHead, const char *pBuffer, unsigned int uiLength)
{
    static int64_t lStartMicroSecond = 0;
    static int64_t lEndMicroSecond = 0;
    static int64_t lCostMicroSecond = 0;

    lStartMicroSecond = taf::TC_Common::now2us();

    msgpack::unpacked stResult;
    if (uiLength > 0)
    {
        try
        {
            msgpack::unpack(&stResult, pBuffer, uiLength);
        }
        catch (const std::runtime_error& stEx)
        {
            PROCESSOR_LOG_ERROR << "QUEUE USER DATA IS NOT MSGPACK FORMAT|MSG:" << stEx.what() << std::endl;

            return (false);
        }
    }

    PROCESSOR_LOG_DEBUG << "QUEUE PROCESS START" << std::endl;

    const bool bRet = DoServerRun(stMsgHead, stResult);

    /////////////////////////////////////////////////////////////////////
    ///////////////// Mysql Delegate
    LOGIC_RECORD_CACHE_INS->ExecSQL(0, 0, m_strCmdName);

    lEndMicroSecond = taf::TC_Common::now2us();
    lCostMicroSecond = lEndMicroSecond - lStartMicroSecond;

    PROCESSOR_LOG_DEBUG << "QUEUE REQUEST_CONSUME_MICRO_TIME:" << lCostMicroSecond << std::endl;
    return (bRet);
}

void
CLogicProcessorBase::PrintSendMsg(const msgpack::unpacked& stResult)
{
    PROCESSOR_LOG_DEBUG << "SEND MSG|" << stResult.get() << std::endl;
}

void
CLogicProcessorBase::PrintRecvMsg(const msgpack::unpacked& stResult)
{
    PROCESSOR_LOG_DEBUG << "RECV MSG|" << stResult.get() << std::endl;
}
