#pragma once

#include "thread"
#include <random>
#include "AppCommon.h"
#include "common_message.h"
#include "common_msg_head_cross.h"
#include "common_log_utility.h"
#include "server_errcode.h"
#include "blocking_queue.hpp"

#define ProcessName "processor"

class CProcessor
{
public:
    static int32_t GetNowSec();
    static const char* GetProcessName() { return ProcessName; };

public:
	CProcessor();
    virtual ~CProcessor();

    bool Start();
    virtual bool Stop();
    virtual void Refresh();
    virtual bool Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID);

    virtual int32_t ProcessMessages(TCrossMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength) = 0;

    void Reload(int32_t nLogMask);
    void Log(int32_t nMask,const char *sFormat, ...) __attribute__ ((format (printf, 3, 4)));
    void WriteBinLog(int32_t logmask, const char* pszBuf, int32_t nBufLen, const char* szTitle = nullptr);

public:
    int32_t GetProcessID() const;
    int32_t GetRandNum();
    int32_t GetRandNum(int32_t iMin, int32_t iMax);  //[iMin, iMax]
    void AddMsgQueue(const TCrossMsgData msg);
    int32_t GetServerIDByGroupID(int32_t iGroupID);
    bool IsGroupConnecting(int32_t iGroupID);

public:
    template <typename MSGPACK_T>
    int32_t SendResponse(const TCrossMsgHeadInfo& stHeadInfo, MSGPACK_T& rstRspBody, int16_t nRspCode = SERVER_ERRCODE::SUCCESS);

    int32_t SendErrCode(const TCrossMsgHeadInfo& stHeadInfo, int16_t nRspCode);

    template <class MSGPACK_T>
    int32_t NotifyMessage(int32_t iGroupID, int32_t iUin, uint16_t usCmd, MSGPACK_T& rstMsgBody);

protected:
	template <typename MSGPACK_T>
	bool DecodeMsgPack(MSGPACK_T& stParam, const char* pszMsgBody, int32_t iBodyLength);

private:
    void Run();
    void AddGameCtrlMsgQueue(const TCrossMsgData& stData);

    template <class MSGPACK_T>
    int32_t SendMsgToLogicSvr(const TCrossMsgHeadInfo& stHeadInfo, CMsgHeadCross& rstCrossHead, MSGPACK_T& rstRspBody);

private:
	int32_t m_nProcessID;
    CAppLog m_Log;
    std::unique_ptr<std::thread> m_pThread;
    dobala::blocking_queue<TCrossMsgData> m_stMsgQueue;         // 消息队列

    char m_abyCodeBuf[max_package_size];
    std::random_device   m_stRandDevice;
    std::mt19937         m_stMt19937;
};

template <class MSGPACK_T>
int32_t CProcessor::SendResponse(const TCrossMsgHeadInfo& stHeadInfo, MSGPACK_T& rstRspBody, int16_t nRspCode/* = SERVER_ERRCODE::SUCCESS*/)
{
    CMsgHeadCross rstReqHead;
    rstReqHead.m_byCmdType = enmCmdType_response;
    rstReqHead.m_usCmd = stHeadInfo.m_usCmd;
    rstReqHead.m_iUin = stHeadInfo.m_iUin;
    rstReqHead.m_nGroupID = stHeadInfo.m_iGroupID;
    rstReqHead.m_nSvrRsp = nRspCode;
    return SendMsgToLogicSvr(stHeadInfo, rstReqHead, rstRspBody);
}

template <class MSGPACK_T>
int32_t CProcessor::NotifyMessage(int32_t iGroupID, int32_t iUin, uint16_t usCmd, MSGPACK_T& rstMsgBody)
{
    TCrossMsgHeadInfo stMsgHead;
    stMsgHead.m_iServerID = GetServerIDByGroupID(iGroupID);
    if(stMsgHead.m_iServerID <= 0)
    {
        Log(log_mask_system_error, "[CProcessor::%s] cmd=%d groupid=%d uin=%d GetServerIDByGroupID error.\n", __FUNCTION__, usCmd, iGroupID, iUin);
        return fail;
    }

    stMsgHead.m_usCmd = usCmd;
    stMsgHead.m_iUin = iUin;
    stMsgHead.m_iGroupID = iGroupID;
    return SendResponse(stMsgHead, rstMsgBody);
}

template <class MSGPACK_T>
int32_t CProcessor::SendMsgToLogicSvr(const TCrossMsgHeadInfo& stHeadInfo, CMsgHeadCross& rstCrossHead, MSGPACK_T& rstRspBody)
{
    int32_t iPackageSize = sizeof(m_abyCodeBuf);
    if (success != encode_message_to_buffer(m_abyCodeBuf, iPackageSize, rstCrossHead, rstRspBody))
    {
        Log(log_mask_system_error, "[CProcessor::%s] cmd=%d convert msg to code failed.\n", __FUNCTION__, rstCrossHead.m_usCmd);
        return -1;
    }

    TCrossMsgData stData;
    stData.m_stHeadInfo = stHeadInfo;
    stData.m_strBody = std::string(m_abyCodeBuf, iPackageSize);

    // 加入到主线程消息队列
    AddGameCtrlMsgQueue(stData);

    return success;
}

template <typename MSGPACK_T>
bool CProcessor::DecodeMsgPack(MSGPACK_T& stParam, const char* pszMsgBody, int32_t iBodyLength) try
{
	msgpack::unpacked stResult;
	msgpack::unpack(&stResult, pszMsgBody, iBodyLength);
	const msgpack::object& stMsgObj = stResult.get();
	stMsgObj.convert(&stParam);
	return true;
}
catch (...)
{
	return false;
}
