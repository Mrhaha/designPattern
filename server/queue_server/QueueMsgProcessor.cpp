#include "QueueMsgProcessor.h"
#include "common_datetime.h"
#include "tc_common.h"
#include "tc_md5.h"
#include <fstream>
#include <random>
#include "server_errcode.h"
#include "server_cmd_cross_define.h"
#include "server_cross_protocol.h"
#include "QueueGameControl.h"
#include "QueueServerConfig.h"

void CQueueServerQueManage::Refresh()
{
    const auto tCurTime = time(nullptr);
    // 刷新保护期队列
    for(auto iter = m_stUserProtectTime.begin(); iter != m_stUserProtectTime.end(); )
    {
        if(iter->second < tCurTime)
        {
            m_stUserProtectTime.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }

    int32_t iMaxOnlineNum = 0;
    int32_t iLoginProtectTime = 0;
    {
        //queue_config
        std::shared_ptr<CQueueServerConfig> queue_config;
        CQueueServerConfig::GetConfig(queue_config);
        iMaxOnlineNum = queue_config->GetZooQueueConfig().m_stQueueConfig.m_iMaxOnlineNum;
        iLoginProtectTime = queue_config->GetZooQueueConfig().m_stQueueConfig.m_iLoginProtectTime;
    }

    // 刷新排队队列
    int32_t iAllowNum = iMaxOnlineNum - GetCurOnlineNum() - GetCurProtectNum();
    if(iAllowNum < 0) iAllowNum = 0;
    uint32_t uCurIndex = m_uMinIndex + iAllowNum;

    for(auto iter = m_stUserQueue.begin(); iter != m_stUserQueue.end(); )
    {
        // 如果活跃时间已超时
        if(iter->second.m_iLastAliveTime < tCurTime)
        {
            m_stUserQueue.erase(iter++);
            continue;
        }

        // 如果排队成功
        if(iter->second.m_uIndex < uCurIndex)
        {
            m_stUserProtectTime[iter->first] = tCurTime + iLoginProtectTime;
            m_stUserQueue.erase(iter++);
            continue;
        }

        ++iter;
    }

    m_uMinIndex = std::min(uCurIndex, m_uMaxIndex);

    // 排队队列为空的话重置
    if(m_stUserQueue.empty()) m_uMinIndex = m_uMaxIndex = 1;
}

void CQueueServerQueManage::UserLogin(int32_t iUin)
{
    m_stUserQueue.erase(iUin);
    m_stUserProtectTime.erase(iUin);

    m_stOnlineUser.insert(iUin);
}

void CQueueServerQueManage::UserLogout(int32_t iUin, bool bProtect)
{
    m_stUserQueue.erase(iUin);
    m_stOnlineUser.erase(iUin);

    if(bProtect)
    {
        //queue_config
        std::shared_ptr<CQueueServerConfig> queue_config;
        CQueueServerConfig::GetConfig(queue_config);
        m_stUserProtectTime[iUin] = time(nullptr) + queue_config->GetZooQueueConfig().m_stQueueConfig.m_iLogoutProtectTime;
    }
}

bool CQueueServerQueManage::CheckCanLogin(int32_t iUin)
{
    if(m_stUserProtectTime.find(iUin) != m_stUserProtectTime.end())
        return true;

    if(m_stOnlineUser.find(iUin) != m_stOnlineUser.end())
        return true;

    if(m_stUserQueue.empty())
    {
        //queue_config
        std::shared_ptr<CQueueServerConfig> queue_config;
        CQueueServerConfig::GetConfig(queue_config);

        // 检查当前在线数量，判断需不需要排队
        if(GetCurOnlineNum() + GetCurProtectNum() < queue_config->GetZooQueueConfig().m_stQueueConfig.m_iMaxOnlineNum)
        {
            // 添加登陆保护期
            m_stUserProtectTime[iUin] = time(nullptr) + queue_config->GetZooQueueConfig().m_stQueueConfig.m_iLoginProtectTime;
            return true;
        }
    }

    return false;
}

int32_t CQueueServerQueManage::GetBeforeNumAndUpdateAlive(int32_t iUin)
{
    auto iter = m_stUserQueue.find(iUin);
    if(iter != m_stUserQueue.end())
    {
        //queue_config
        std::shared_ptr<CQueueServerConfig> queue_config;
        CQueueServerConfig::GetConfig(queue_config);

        iter->second.m_iLastAliveTime = time(nullptr) + queue_config->GetZooQueueConfig().m_stQueueConfig.m_iQueueAliveInterval;
        return iter->second.m_uIndex - m_uMinIndex + 1;
    }

    return (-1);
}

int32_t CQueueServerQueManage::AddQueue(int32_t iUin)
{
    //queue_config
    std::shared_ptr<CQueueServerConfig> queue_config;
    CQueueServerConfig::GetConfig(queue_config);

    TQueueUserInfo stInfo;
    stInfo.m_uIndex = m_uMaxIndex++;
    stInfo.m_iJoinTime = time(nullptr);
    stInfo.m_iLastAliveTime = stInfo.m_iJoinTime + queue_config->GetZooQueueConfig().m_stQueueConfig.m_iQueueAliveInterval;

    m_stUserQueue.emplace(iUin, stInfo);

    return m_uMaxIndex - m_uMinIndex;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CQueueMsgProcessor::CQueueMsgProcessor(void)
{
    m_tmLastRefreshTime = 0;
    m_stQueueManager.clear();
}

void CQueueMsgProcessor::Refresh()
{
    // 一秒刷新一次
    time_t tNow = time(nullptr);
    if (tNow == m_tmLastRefreshTime)
    {
        return;
    }

    m_tmLastRefreshTime = tNow;

    // 刷新
    for(auto& manager : m_stQueueManager)
    {
        Log(log_mask_detail, "[CQueueMsgProcessor:%s] server(%d) queue_num(%d) online_num(%d) protect_num(%d).\n",
            __FUNCTION__, manager.first, manager.second.GetCurQueueNum(), manager.second.GetCurOnlineNum(), manager.second.GetCurProtectNum());

        manager.second.Refresh();
    }
}

bool CQueueMsgProcessor::Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID)
{
    if(false == CQueueProcessor::Initialize(nLogMask, pszLogFileName, nProcessID))
    {
        return false;
    }

    return true;
}

int32_t CQueueMsgProcessor::ProcessMessages(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    int32_t iRet = fail;
    switch(stHeadInfo.m_usCmd)
    {
    case MSG_QUEUE_USER_LOGIN_NOTIFY:
        iRet = ProcessUserLoginNotify(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_QUEUE_USER_LIST_ONLINE_NOTIFY:
        iRet = ProcessUserListOnlineNotify(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_QUEUE_USER_QUEUE_HEARTBEAT:
        iRet = ProcessUserQueueHeartbeat(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    case MSG_QUEUE_USER_RECONNECT_CHECK:
        iRet = ProcessUserReconnectCheck(stHeadInfo, pszMsgBody, iBodyLength);
        break;

    default:
        Log(log_mask_system_error, "[CQueueMsgProcessor:%s] recv cmd %u no process\n", __FUNCTION__, stHeadInfo.m_usCmd);
        break;
    }

    return iRet;
}

int32_t CQueueMsgProcessor::ProcessUserLoginNotify(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CQueueUserLoginNotify stNotify;
    if (!DecodeMsgPack(stNotify, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    if(stNotify.m_bLogin)
    {
        m_stQueueManager[stHeadInfo.m_iServerID].UserLogin(stNotify.m_iUin);
    }
    else
    {
        m_stQueueManager[stHeadInfo.m_iServerID].UserLogout(stNotify.m_iUin, stNotify.m_bProtect);
    }

    Log(log_mask_detail, "[CQueueMsgProcessor:%s] server(%d) login:%d uin:%d protect:%d.\n",
        __FUNCTION__, stHeadInfo.m_iServerID, stNotify.m_bLogin, stNotify.m_iUin, stNotify.m_bProtect);

    return success;
}

int32_t CQueueMsgProcessor::ProcessUserListOnlineNotify(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CQueueUserListOnlineNotify stNotify;
    if (!DecodeMsgPack(stNotify, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    auto& manager = m_stQueueManager[stHeadInfo.m_iServerID];
    for(auto& uin : stNotify.m_stUinVec)
    {
        manager.UserLogin(uin);
    }

    Log(log_mask_detail, "[CQueueMsgProcessor:%s] server(%d) online_num:%d.\n",
        __FUNCTION__, stHeadInfo.m_iServerID, (int32_t)stNotify.m_stUinVec.size());

    return success;
}

int32_t CQueueMsgProcessor::ProcessUserQueueHeartbeat(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CQueueUserQueueHeartbeat stRsp;
    int16_t sRet = SERVER_ERRCODE::SUCCESS;

    auto& manager = m_stQueueManager[stHeadInfo.m_iServerID];
    if(manager.CheckCanLogin(stHeadInfo.m_iUin))
    {
        // 可登陆
        stRsp.m_bCanLogin = true;

        CGYSessionCreator::gy_session_t mySession;
        mySession.m_iUin = stHeadInfo.m_iUin;
        mySession.m_iGroupID = stHeadInfo.m_iGroupID;
        if(0 != CGYSessionCreator::Encrypt(CQueueGameControl::m_appCfg.m_szConnSvrSessionKey, mySession, stRsp.m_strSession))
        {
            sRet = SERVER_ERRCODE::INTERN_ERROR;

            Log(log_mask_system_error, "[CQueueMsgProcessor:%s] server(%d) session encrypt error! uin(%d) group(%d).\n",
                __FUNCTION__, stHeadInfo.m_iServerID, stHeadInfo.m_iUin, stHeadInfo.m_iGroupID);
        }
    }
    else
    {
        // 不可登陆
        stRsp.m_bCanLogin = false;

        stRsp.m_iBeforeNum = manager.GetBeforeNumAndUpdateAlive(stHeadInfo.m_iUin);

        // 需要排队
        if(stRsp.m_iBeforeNum < 0)
        {
            stRsp.m_iBeforeNum = manager.AddQueue(stHeadInfo.m_iUin);
        }

        stRsp.m_iRemainWaitTime = stRsp.m_iBeforeNum * 30;
    }

    // 客户端消息头
    CMsgHeadClient stClientHead;
    stClientHead.m_stStaticData.m_iGroupID = stHeadInfo.m_iGroupID;
    stClientHead.m_stStaticData.m_iUin = stHeadInfo.m_iUin;
    stClientHead.m_stStaticData.m_usCmd = stHeadInfo.m_usCmd;
    stClientHead.m_stStaticData.m_iSvrRsp = sRet;

    SendMsgToClient(stHeadInfo, stClientHead, stRsp);

    Log(log_mask_detail, "[CQueueMsgProcessor:%s] server(%d) uin(%d) can_login(%d).\n",
        __FUNCTION__, stHeadInfo.m_iServerID, stHeadInfo.m_iUin, stRsp.m_bCanLogin);

    return success;
}

int32_t CQueueMsgProcessor::ProcessUserReconnectCheck(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength)
{
    CQueueRequestUserReconnectCheck stNotify;
    CQueueResponseUserReconnectCheck stRsp;
    if (!DecodeMsgPack(stNotify, pszMsgBody, iBodyLength))
    {
        return fail;
    }

    stRsp.m_iUin = stNotify.m_iUin;
    stRsp.m_bCanLogin = true;

    auto& manager = m_stQueueManager[stHeadInfo.m_iServerID];
    if(manager.GetCurQueueNum() > 0)
    {
        stRsp.m_bCanLogin = false;
    }

    // queue_svr消息头
    CMsgHeadQueue stQueueHead;
    stQueueHead.m_usCmd = stHeadInfo.m_usCmd;

    SendMsgToLogicSvr(stHeadInfo, stQueueHead, stRsp);

    Log(log_mask_detail, "[CQueueMsgProcessor:%s] server(%d) reconnect uin:%d can_login:%d.\n",
        __FUNCTION__, stHeadInfo.m_iServerID, stRsp.m_iUin, stRsp.m_bCanLogin);

    return success;
}

template <class MSGPACK_T>
int32_t CQueueMsgProcessor::SendMsgToClient(const TQueueMsgHeadInfo& stHeadInfo, CMsgHeadClient& rstReqHead, MSGPACK_T& rstRspBody)
{
    static char pszBuffer[512];

    msgpack::sbuffer stBuffer;
    msgpack::pack(stBuffer, rstRspBody);

    rstReqHead.m_stStaticData.m_uiPacketLen = sizeof(rstReqHead.m_stStaticData) + rstReqHead.m_stStaticData.m_ucSessionLen + stBuffer.size();
    if(rstReqHead.m_stStaticData.m_uiPacketLen >= sizeof(pszBuffer))
    {
        Log(log_mask_system_error, "[CQueueMsgProcessor::%s] cmd=%d buffer too small.\n", __FUNCTION__, rstReqHead.m_stStaticData.m_usCmd);
        return -1;
    }

    int32_t iOffset = rstReqHead.Encode(pszBuffer, sizeof(pszBuffer));
    if(iOffset < 0)
    {
        Log(log_mask_system_error, "[CQueueMsgProcessor::%s] cmd=%d convert msg to code failed.\n", __FUNCTION__, rstReqHead.m_stStaticData.m_usCmd);
        return -1;
    }

    memcpy(pszBuffer + iOffset, stBuffer.data(), stBuffer.size());

    TQueueMsgData stData;
    stData.m_stHeadInfo = stHeadInfo;
    stData.m_strBody = std::string(pszBuffer, rstReqHead.m_stStaticData.m_uiPacketLen);

    // 加入到主线程消息队列
    if(nullptr != g_pQueueGameCtrl)
    {
        g_pQueueGameCtrl->AddMsgQueue(stData);
    }

    return success;
}

template <class MSGPACK_T>
int32_t CQueueMsgProcessor::SendMsgToLogicSvr(const TQueueMsgHeadInfo& stHeadInfo, CMsgHeadQueue& rstQueueHead, MSGPACK_T& rstRspBody)
{
    static char szPackage[512] = { 0 };
    int32_t iPackageSize = sizeof(szPackage);
    if (success != encode_message_to_buffer(szPackage, iPackageSize, rstQueueHead, rstRspBody))
    {
        Log(log_mask_system_error, "[CQueueMsgProcessor::%s] cmd=%d convert msg to code failed.\n", __FUNCTION__, rstQueueHead.m_usCmd);
        return -1;
    }

    TQueueMsgData stData;
    stData.m_stHeadInfo = stHeadInfo;
    stData.m_strBody = std::string(szPackage, iPackageSize);

    // 加入到主线程消息队列
    if(nullptr != g_pQueueGameCtrl)
    {
        g_pQueueGameCtrl->AddMsgQueue(stData);
    }

    return success;
}
