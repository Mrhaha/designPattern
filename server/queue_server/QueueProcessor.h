#pragma once

#include "thread"
#include "QueueAppCommon.h"
#include "common_codequeue.h"
#include "common_log_utility.h"
#include "common_memory.h"
#include "common_message.h"
#include "common_msg_head_queue.h"
#include "server_errcode.h"
#include "blocking_queue.hpp"
#include <random>

#define ProcessName "processor"

class CQueueProcessor
{
public:
    static const char* GetProcessName() { return ProcessName; };

public:
    CQueueProcessor();
    virtual ~CQueueProcessor();

    virtual bool Start();
    virtual bool Stop();
    virtual void Refresh();
    virtual bool Initialize(int32_t nLogMask, const char* pszLogFileName, int32_t nProcessID);

	virtual int32_t ProcessMessages(TQueueMsgHeadInfo& stHeadInfo, const char* pszMsgBody, const int32_t iBodyLength) = 0;

    void Reload(int32_t nLogMask);
    void Log(int32_t nMask,const char *sFormat, ...) __attribute__ ((format (printf, 3, 4)));
    void WriteBinLog(int32_t logmask, const char* pszBuf, int32_t nBufLen, const char* szTitle = nullptr);

public:
    int32_t GetProcessID() const;
	void AddMsgQueue(const TQueueMsgData msg);

protected:
	template <typename MSGPACK_T>
	bool DecodeMsgPack(MSGPACK_T& stParam, const char* pszMsgBody, int32_t iBodyLength);

    void Run();

private:
    int32_t m_nProcessID;
    CQueueAppLog m_Log;
    std::unique_ptr<std::thread> m_pThread;
    dobala::blocking_queue<TQueueMsgData> m_stMsgQueue;         // 消息队列
};

template <typename MSGPACK_T>
bool CQueueProcessor::DecodeMsgPack(MSGPACK_T& stParam, const char* pszMsgBody, int32_t iBodyLength) try
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


