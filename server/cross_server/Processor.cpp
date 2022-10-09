#include <vector>
#include <algorithm>
#include <cstdlib>
#include "AppPlatform.h"
#include "Processor.h"
#include "ServerConfig.h"
#include "GameControl.h"

CProcessor::CProcessor() : m_stRandDevice(), m_stMt19937(m_stRandDevice())
{
    m_nProcessID = 0;
}

CProcessor::~CProcessor()
{
    m_stMsgQueue.stop();

    if (m_pThread)
    {
        m_pThread->join();
    }
}

bool CProcessor::Start()
{
    m_pThread = std::make_unique<std::thread>([this] { Run(); });
    if(nullptr == m_pThread)
    {
        return false;
    }

    return true;
}

bool CProcessor::Stop()
{
    m_stMsgQueue.stop();

    if (m_pThread)
    {
        m_pThread->join();
    }

    Log(log_mask_system_warning, "[CProcessor::%s] stop...\n", __FUNCTION__);

    return true;
}

void CProcessor::Refresh()
{
    Log(log_mask_detail, "[CProcessor::%s] refresh...\n", __FUNCTION__);
}

bool CProcessor::Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID)
{
    m_nProcessID = nProcessID;
    //初始化本地的.....
    char szLogFileName[MAX_FILEPATHLEN] = {0};
    sprintf(szLogFileName,"%s-%d", pszLogFileName, nProcessID);
    m_Log.InitLog(nLogMask, (char*)szLogFileName);

    Log(log_mask_initialize,"[CProcessor::%s] init success!\n", __FUNCTION__);

    return true;
}

void CProcessor::Run()
{
    Log(log_mask_initialize,"[CProcessor::%s]start.....\n",__FUNCTION__);

    static int64_t lStartMicroSecond = 0;
    static int64_t lEndMicroSecond = 0;
    static int64_t lCostMicroSecond = 0;

    std::optional<TCrossMsgData> opt;
    while ((opt = m_stMsgQueue.pop_front()).has_value())
    {
        Refresh();

        auto& msg = opt.value();

        // 心跳消息
        if(0 == msg.m_stHeadInfo.m_usCmd) continue;

        lStartMicroSecond = now_msec();

        const auto ret = ProcessMessages(msg.m_stHeadInfo, msg.m_strBody.c_str(), msg.m_strBody.length());
        if (success != ret)
        {
            Log(log_mask_system_error, "[CProcessor::%s] process message failed. cmd:%d, from server(%d) or player(%d,%d)\n",
                __FUNCTION__, msg.m_stHeadInfo.m_usCmd, msg.m_stHeadInfo.m_iServerID, msg.m_stHeadInfo.m_iUin, msg.m_stHeadInfo.m_iGroupID);
        }

        lEndMicroSecond = now_msec();
        lCostMicroSecond = lEndMicroSecond - lStartMicroSecond;
        if (lCostMicroSecond > 10)
        {
            TRACESVR(log_mask_system_error, "[CProcessor:%s] Run process message too slow! COST %ld ms! cmd:%d, from server(%d) or player(%d,%d)\n",
                     __FUNCTION__, lCostMicroSecond, msg.m_stHeadInfo.m_usCmd, msg.m_stHeadInfo.m_iServerID, msg.m_stHeadInfo.m_iUin, msg.m_stHeadInfo.m_iGroupID);
        }
    }
}

int32_t CProcessor::GetProcessID() const
{
    return m_nProcessID;
}

int32_t CProcessor::GetRandNum()
{
    return m_stMt19937() & 0x7fffffff;
}

int32_t CProcessor::GetRandNum(int32_t iMin, int32_t iMax)
{
    if(iMin == iMax) return (iMin);

    if(iMin > iMax)
    {
        std::swap(iMin, iMax);
    }

    return (iMin + (GetRandNum() % (iMax - iMin + 1)));
}

int32_t CProcessor::GetNowSec()
{
    int32_t iNowSec = now_sec();
#if CROSS_BUILD_PLATFORM == BUILD_PLATFORM_LOCAL
    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);
    iNowSec += cross_config->GetZooCrossConfig().m_stCrossConfig.m_iDebugTimeOffset;
#endif
    return iNowSec;
}

void CProcessor::AddMsgQueue(const TCrossMsgData msg)
{
    m_stMsgQueue.emplace_back(msg);
}

int32_t CProcessor::GetServerIDByGroupID(int32_t iGroupID)
{
    return g_pGameCtrl->GetServerIDByGroupID(iGroupID);
}

bool CProcessor::IsGroupConnecting(int32_t iGroupID)
{
    return g_pGameCtrl->IsGroupConnecting(iGroupID);
}

void CProcessor::AddGameCtrlMsgQueue(const TCrossMsgData& stData)
{
    g_pGameCtrl->AddMsgQueue(stData);
}

void CProcessor::Reload(int32_t nLogMask)
{
    this->m_Log.InitLog(nLogMask,nullptr);
}

void CProcessor::Log(int32_t nMask,const char *sFormat, ...)
{
    va_list vaList;
    va_start(vaList, sFormat);
    m_Log.WriteLog(nMask, sFormat, vaList);
    va_end(vaList);
}

void CProcessor::WriteBinLog(int32_t logmask, const char *pszBuf, int32_t nBufLen, const char *szTitle)
{
    m_Log.WriteLogBin(logmask, pszBuf, nBufLen, szTitle);
}

int32_t CProcessor::SendErrCode(const TCrossMsgHeadInfo& stHeadInfo, int16_t nRspCode)
{
    std::string stEmptyBody;
    return SendResponse(stHeadInfo, stEmptyBody, nRspCode);
}
