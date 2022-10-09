#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "server_user_protocol.h"
#include "server_cross_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_config.h"
#include "logic_game_reconnect_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_global.h"
#include "logic_game_cache_common.h"
#include "server_guild_protocol.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_RECONNECT, CLogicReconnectProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SYNC_SERVER_TIMESTAMP, CLogicReconnectProcessor)
REGISTER_CMD_FACTORY(MSG_QUEUE_USER_RECONNECT_CHECK, CLogicReconnectProcessor)

CLogicReconnectProcessor::CLogicReconnectProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicReconnectProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if((int)m_usCmd == MSG_LOGIC_RECONNECT)
    {
        // 如果下线时间过长，不允许重连，且不加入下线保护队列
        if(CLogicCommon::GetSec() - m_pUserInfo->m_stUserInfoTable.GetLastLogout() > 3600)
        {
            CLogicUser::GetInstance().AddQueueTag(m_iUin);

            CResponseNotifyClientLogout proto;
            proto.m_iLogoutType = CResponseNotifyClientLogout::ENCLT_JustLogout;
            GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, MSG_LOGIC_NOTIFY_CLIENT_LOGOUT, proto);
            return (false);
        }

        // 如果重连的时候正好跨天刷新，不允许重连
        if(m_pUserInfo->m_stUserDailyTable.GetLastRefreshTime() == CLogicCommon::GetSec())
        {
            if(CLogicCommon::GetSec() - m_pUserInfo->m_stUserInfoTable.GetLastLogout() > CLogicConfig::GetZooQueueConfig().m_iLogoutProtectTime)
            {
                CLogicUser::GetInstance().AddQueueTag(m_iUin);
            }

            CResponseNotifyClientLogout proto;
            proto.m_iLogoutType = CResponseNotifyClientLogout::ENCLT_JustLogout;
            GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, MSG_LOGIC_NOTIFY_CLIENT_LOGOUT, proto);
            return (false);
        }

        CPackUserStatInfo stParam;
        const msgpack::object& stMsgObj = stFormatData.get();
        try
        {
            stMsgObj.convert(&stParam);

            m_stUserConnectorData.m_stStatInfo.m_strAccount = stParam.m_strAccount;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceCarrier = stParam.m_strDeviceCarrier;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceID = stParam.m_strDeviceID;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceModel = stParam.m_strDeviceModel;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceNet = stParam.m_strDeviceNet;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceOS = stParam.m_strDeviceOS;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceRatio = stParam.m_strDeviceRatio;
            m_stUserConnectorData.m_stStatInfo.m_strPlatformID = stParam.m_strPlatformID;
            m_stUserConnectorData.m_stStatInfo.m_strPlatformSign = stParam.m_strPlatformSign;
            m_stUserConnectorData.m_iResVersion = stParam.m_iResVer;
            m_stUserConnectorData.m_iChannelID = m_iChannelID;

            m_pUserInfo->m_strPid = stParam.m_strAccount;
            m_pUserInfo->m_strDeviceID = stParam.m_strDeviceID;
            CLogicUser::GetInstance().SetUserInfo(m_iUin, m_stUserConnectorData);
        }
        catch(const std::bad_cast& stEx)
        {
        }

        LOGIC_LOG_TRACE_DEBUG << "GROUP_ID:" << m_iGroupID << "|UIN:" << m_iUin
                              << "|REMOTE_HOST:" << m_stUserConnectorData.m_strClientIP << ":" << m_stUserConnectorData.m_usClientPort
                              << "|ACCOUNT:" << m_stUserConnectorData.m_stStatInfo.m_strAccount << "|DEVICE_ID:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceID
                              << "|DEVICE_MODEL:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceModel << "|DEVICE_OS:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceOS
                              << "|DEVICE_RATIO:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceRatio << "|DEVICE_NET:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceNet
                              << "|DEVICE_CARRIER:" << m_stUserConnectorData.m_stStatInfo.m_strDeviceCarrier << "|PLATFORM_ID:" << m_stUserConnectorData.m_stStatInfo.m_strPlatformID
                              << "|ROLE_LEVEL:" << m_pUserInfo->m_stUserInfoTable.GetLevel() << "|NICK:" << m_pUserInfo->m_strNick << "|LOGIN SUCCESS" << std::endl;

        // 判断排队开关以及下线保护时间
        if(CLogicConfig::GetZooQueueConfig().m_bSwitch && LOGIC_SERVICE_API_INS->GetQueueConnection()->IsConnected())
        {
            // 如果下线保护时间超时，需要重新排队
            if(CLogicCommon::GetSec() - m_pUserInfo->m_stUserInfoTable.GetLastLogout() > CLogicConfig::GetZooQueueConfig().m_iLogoutProtectTime)
            {
                CLogicGlobal::NotifyQueueSvrReconnectCheck(m_iUin);
                PROCESSOR_LOG_DEBUG << "RECONNECT SEND TO QUEUE CHECK" << std::endl;

                return true;
            }
        }

        // 处理重连
        HandleReconnect();
    }
    else if((int)m_usCmd == MSG_LOGIC_SYNC_SERVER_TIMESTAMP)
    {
        CResponseSyncTimestamp stProto;
        stProto.m_iSvrTimestamp = CLogicCommon::GetSec();
        SendToClient(stProto, SERVER_ERRCODE::SUCCESS);
    
        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|SERVER TIMESTAMP:" << stProto.m_iSvrTimestamp << std::endl;
    }
    
    return (true);
}

bool CLogicReconnectProcessor::DoServerRun(const CMsgHeadQueue &stMsgHead, const msgpack::unpacked &stFormatData)
{
    bool bRet = false;

    switch (m_usCmd)
    {
    case MSG_QUEUE_USER_RECONNECT_CHECK:
        bRet = DoServerRunUserReconnectCheck(stMsgHead, stFormatData);
        break;

    default:
        break;
    }

    return bRet;
}

bool CLogicReconnectProcessor::DoServerRunUserReconnectCheck(const CMsgHeadQueue &stMsgHead, const msgpack::unpacked &stFormatData)
{
    CQueueResponseUserReconnectCheck stRspBody;
    if (!DecodeMsgPack(stFormatData, stRspBody))
    {
        return false;
    }

    CLogicUser::const_iterator it = CLogicUser::GetInstance().GetUserInfo(stRspBody.m_iUin);
    if(it == CLogicUser::GetInstance().End())
    {
        PROCESSOR_LOG_ERROR << "CAN NOT FIND USER IN CONNECTOR" << std::endl;
        return (false);
    }

    m_stUserConnectorData = it->second;
    m_iGroupID = it->second.m_iGroupID;
    m_iUin = it->first;

    m_pUserInfo = CLogicCacheCommon::LoadUserData(m_iGroupID, m_iUin);
    if (!m_pUserInfo)
    {
        using namespace SERVER_ERRCODE;
        PROCESSOR_LOG_ERROR << "QUEUE PROCESS ERROR|ERRCODE:" << USER_ROLE_NOT_EXIST
                            << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NOT_EXIST)
                            << "USER UIN:" << m_iUin << " GROUP_ID:" << m_iGroupID
                            << "|CMD:" << stMsgHead.m_usCmd  << "|CMD_NAME:"<< m_strCmdName
                            << std::endl;

        return (false);
    }

    m_pUserInfo->m_iCommand = m_usCmd;

    // 可登录
    if(stRspBody.m_bCanLogin)
    {
        HandleReconnect();
    }
    else
    {
        // 不可登陆，需要排队
        // 添加到排队标记里，表示下线不能加入保护队列
        CLogicUser::GetInstance().AddQueueTag(m_iUin);

        CResponseNotifyClientLogout proto;
        proto.m_iLogoutType = CResponseNotifyClientLogout::ENCLT_JustLogout;
        GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, MSG_LOGIC_NOTIFY_CLIENT_LOGOUT, proto);
    }

    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|CAN_LOGIN:" << stRspBody.m_bCanLogin << std::endl;

    return true;
}

void CLogicReconnectProcessor::HandleReconnect()
{
    CResponseReconnect stProto;
    stProto.m_iSvrTimestamp = CLogicCommon::GetSec();
    GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, MSG_LOGIC_RECONNECT, stProto);

    CLogicUserNotifySystem::NotifyUserDataToClient(m_pUserInfo);

    ////拉取跨服队伍ID
    std::string strEmpty;
    SendToCrossServer(enmCmdType_request, MSG_CROSS_BANQUET_UPDATE_LOGIN, strEmpty);

    ////更新限时boss状态
    std::string strEmptyBoss;
    SendToCrossServer(enmCmdType_request, MSG_CROSS_NEW_LIMIT_BOSS_GET_INFO, strEmptyBoss);

    ////获取最近一次公会战排名
    CRequestGuildWarSelfRankInfo stReq;
    stReq.selfRankInfo = m_pUserInfo->m_stIBusyData.GetGuildWarCalInfo();
    if (stReq.selfRankInfo.m_iRankID != 0)
    {
        SendToCrossServer(enmCmdType_request, MSG_CROSS_GUILD_WAR_GET_SELF_RANK, stReq);
    }
    PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|SERVER TIMESTAMP:" << stProto.m_iSvrTimestamp << std::endl;
}
