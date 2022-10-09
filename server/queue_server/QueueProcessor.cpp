#include "QueueAppPlatform.h"
#include "QueueGameControl.h"
#include "QueueProcessor.h"
#include "QueueServerConfig.h"
#include "server_cmd_cross_define.h"
#include <algorithm>
#include <cstdlib>
#include <vector>

CQueueProcessor::CQueueProcessor()
{
	m_nProcessID = 0;
}

CQueueProcessor::~CQueueProcessor()
{
    m_stMsgQueue.stop();

    if (m_pThread)
    {
        m_pThread->join();
    }
}

void CQueueProcessor::Run()
{
    Log(log_mask_detail,"[CQueueProcessor::%s] start.....\n",__FUNCTION__);

    static int64_t lStartMicroSecond = 0;
    static int64_t lEndMicroSecond = 0;
    static int64_t lCostMicroSecond = 0;

    std::optional<TQueueMsgData> opt;
    while ((opt = m_stMsgQueue.pop_front()).has_value())
    {
        // 刷新
        Refresh();

        auto& msg = opt.value();

        // 心跳消息
        if(0 == msg.m_stHeadInfo.m_usCmd) continue;

        lStartMicroSecond = now_msec();
        const auto ret = ProcessMessages(msg.m_stHeadInfo, msg.m_strBody.c_str(), msg.m_strBody.length());
        if (success != ret)
        {
            Log(log_mask_system_error, "[CQueueProcessor::%s] process message failed. cmd:%d, from server(%d) or player(%d,%d)\n",
                __FUNCTION__, msg.m_stHeadInfo.m_usCmd, msg.m_stHeadInfo.m_iServerID, msg.m_stHeadInfo.m_iUin, msg.m_stHeadInfo.m_iGroupID);
        }

        lEndMicroSecond = now_msec();
        lCostMicroSecond = lEndMicroSecond - lStartMicroSecond;
        if (lCostMicroSecond > 10)
        {
            TRACESVR(log_mask_system_error, "[CQueueProcessor:%s] process message too slow! COST %ld ms! cmd:%d, from server(%d) or player(%d,%d)\n",
                     __FUNCTION__, lCostMicroSecond, msg.m_stHeadInfo.m_usCmd, msg.m_stHeadInfo.m_iServerID, msg.m_stHeadInfo.m_iUin, msg.m_stHeadInfo.m_iGroupID);
        }
    }
}

bool CQueueProcessor::Start()
{
    m_pThread = std::make_unique<std::thread>([this] { Run(); });
    if(nullptr == m_pThread)
    {
        return false;
    }

    return true;
}

bool CQueueProcessor::Stop()
{
    m_stMsgQueue.stop();

    if (m_pThread)
    {
        m_pThread->join();
    }

    Log(log_mask_system_warning, "[CQueueProcessor::%s] stop...\n", __FUNCTION__);

    return true;
}

void CQueueProcessor::Refresh()
{
    Log(log_mask_detail, "[CQueueProcessor::%s] refresh...\n", __FUNCTION__);
}

bool CQueueProcessor::Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID)
{
    m_nProcessID = nProcessID;
    //初始化本地的.....
    char szLogFileName[MAX_FILEPATHLEN] = {0};
    sprintf(szLogFileName,"%s-%d", pszLogFileName, nProcessID);
    m_Log.InitLog(nLogMask, (char*)szLogFileName);

    Log(log_mask_initialize,"[CQueueProcessor::%s] init success!\n", __FUNCTION__);

    return true;
}

int32_t CQueueProcessor::GetProcessID() const
{
    return m_nProcessID;
}

void CQueueProcessor::AddMsgQueue(const TQueueMsgData msg)
{
    m_stMsgQueue.emplace_back(msg);
}

void CQueueProcessor::Reload(int32_t nLogMask)
{
    this->m_Log.InitLog(nLogMask,nullptr);
}

void CQueueProcessor::Log(int32_t nMask,const char *sFormat, ...)
{
    va_list vaList;
    va_start(vaList, sFormat);
    m_Log.WriteLog(nMask, sFormat, vaList);
    va_end(vaList);
}

void CQueueProcessor::WriteBinLog(int32_t logmask, const char *pszBuf, int32_t nBufLen, const char *szTitle)
{
    m_Log.WriteLogBin(logmask, pszBuf, nBufLen, szTitle);
}
