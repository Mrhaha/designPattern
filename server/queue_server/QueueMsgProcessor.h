#pragma once

#include "QueueAppCommon.h"
#include "QueueProcessor.h"
#include <algorithm>
#include "common_msg_head_client.h"

struct TQueueUserInfo
{
    uint32_t m_uIndex;              // 自己的队列序号
    int32_t m_iJoinTime;            // 入队时间
    int32_t m_iLastAliveTime;       // 上次活跃时间
};

class CQueueServerQueManage
{
public:
    CQueueServerQueManage() : m_uMinIndex(1), m_uMaxIndex(1), m_stUserQueue(), m_stOnlineUser(), m_stUserProtectTime() {}

    void Refresh();

    void UserLogin(int32_t iUin);
    void UserLogout(int32_t iUin, bool bProtect);
    bool CheckCanLogin(int32_t iUin);

    // 获取前面排队人数以及更新活跃时间
    int32_t GetBeforeNumAndUpdateAlive(int32_t iUin);
    int32_t AddQueue(int32_t iUin);

    int32_t GetCurQueueNum() const { return m_stUserQueue.size(); }
    int32_t GetCurOnlineNum() const { return m_stOnlineUser.size(); }
    int32_t GetCurProtectNum() const { return m_stUserProtectTime.size(); }

private:
    uint32_t m_uMinIndex;      // 当前队列序号最小值
    uint32_t m_uMaxIndex;      // 当前队列序号下一个可用序号
    std::unordered_map<int32_t, TQueueUserInfo> m_stUserQueue;  // 当前排队中的玩家
    std::unordered_set<int32_t> m_stOnlineUser;                 // 当前在线玩家
    std::unordered_map<int32_t, int32_t> m_stUserProtectTime;   // 处于登陆保护期、下线保护期的玩家
};

class CQueueMsgProcessor : public CQueueProcessor
{
public:
    CQueueMsgProcessor(void);

public:
    void Refresh() override;
    bool Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID) override;
    int32_t ProcessMessages(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength) override;

protected:
    int32_t ProcessUserLoginNotify(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessUserListOnlineNotify(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessUserQueueHeartbeat(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);
    int32_t ProcessUserReconnectCheck(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength);

private:
    template <class MSGPACK_T>
    int32_t SendMsgToClient(const TQueueMsgHeadInfo& stHeadInfo, CMsgHeadClient& rstReqHead, MSGPACK_T& rstRspBody);

    template <class MSGPACK_T>
    int32_t SendMsgToLogicSvr(const TQueueMsgHeadInfo& stHeadInfo, CMsgHeadQueue& rstQueueHead, MSGPACK_T& rstRspBody);

private:
    time_t m_tmLastRefreshTime;
    std::unordered_map<int32_t, CQueueServerQueManage> m_stQueueManager;
};
