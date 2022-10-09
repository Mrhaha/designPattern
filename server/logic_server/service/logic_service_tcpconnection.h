#pragma once

#include "common_msg_head_cross.h"
#include "common_tcp_connection.h"
#include "i_logic_tcpconnection.h"
#include "logic_service_logger.h"
#include "logic_service_api.h"

class CSocketEpollPolicy
{
public:
	static void NotifyConnecting(int32_t iFd);

	static void NotifyClosed(int32_t iFd);
};

template<typename MessageHead>
class CLogicTCPConnection : public CTCPConnectionHandle<CSocketEpollPolicy>, public ILogicTCPConnection
{
public:
	using MessageHeadType = MessageHead;

	void HeartBeat() override;

    void CmdHeartBeat(uint16_t usCmd) override;

	void RecvData() override;

	int32_t SendData(const char* data, size_t size) override;

	bool IsConnected() override;
};

using CLogicCrossConnection = CLogicTCPConnection<CMsgHeadCross>;

template <typename MessageHead>
void CLogicTCPConnection<MessageHead>::HeartBeat()
{
	MessageHeadType stMsgHead;
	stMsgHead.m_byCmdType = enmCmdType_heart;
	static char abyCodeBuf[MessageHeadType::SIZE];
	int32_t nTmpCodeLength = MessageHeadType::SIZE;
	if (success != encode_message_to_buffer(abyCodeBuf, nTmpCodeLength, stMsgHead))
	{
		SERVICE_LOG_TRACE_ERROR << "ENCODE CROSS HEART BEAT FAIL" << std::endl;
		return;
	}

	SendData(abyCodeBuf, static_cast<size_t>(nTmpCodeLength));
}

template <typename MessageHead>
void CLogicTCPConnection<MessageHead>::CmdHeartBeat(uint16_t usCmd)
{
    MessageHeadType stMsgHead;
    stMsgHead.m_usCmd = usCmd;
    static char abyCodeBuf[MessageHeadType::SIZE];
    int32_t nTmpCodeLength = MessageHeadType::SIZE;
    if (success != encode_message_to_buffer(abyCodeBuf, nTmpCodeLength, stMsgHead))
    {
        SERVICE_LOG_TRACE_ERROR << "ENCODE CROSS HEART BEAT FAIL" << std::endl;
        return;
    }

    SendData(abyCodeBuf, static_cast<size_t>(nTmpCodeLength));
}

template <typename MessageHead>
void CLogicTCPConnection<MessageHead>::RecvData()
{
	const auto ret = CTCPConnectionHandle<CSocketEpollPolicy>::RecvData();
	if (enmConnErrorCode_peer_closed == ret)
	{
		SERVICE_LOG_TRACE_ERROR << "peer(" << GetEntityType() << "," << GetEntityId()
			<< ") ip(" << GetStrIp() << ":" << GetPort() << "close link" << std::endl;
	}

	static char abyCodeBuf[max_package_size];
	size_t nTmpCodeLength = 0;
	while (GetOneCode(abyCodeBuf, nTmpCodeLength) > 0)
	{
		SERVICE_LOG_TRACE_DEBUG << "get one code from peer(" << GetEntityType() << "," << GetEntityId()
			<< ") ip(" << GetStrIp() << ":" << GetPort() << std::endl;

		MessageHeadType stMsgHead;
		char* pszBody = nullptr;
		int32_t iBodyLength = 0;

		if (success !=
                decode_buffer_to_message(static_cast<const char *>(abyCodeBuf), static_cast<int32_t>(nTmpCodeLength),
                                         stMsgHead, &pszBody, iBodyLength))
		{
			SERVICE_LOG_TRACE_ERROR << "decode server message failed!, code size = " << nTmpCodeLength << std::endl;
			continue;
		}

		SERVICE_API_INS->GetLogicSoGameApi()->ProcessServerMessage(stMsgHead, pszBody, iBodyLength);
	}
}

template <typename MessageHead>
int32_t CLogicTCPConnection<MessageHead>::SendData(const char* data, size_t size)
{
	return CTCPConnectionHandle<CSocketEpollPolicy>::SendData(data, size);
}

template<typename MessageHead>
bool CLogicTCPConnection<MessageHead>::IsConnected()
{
	return BeConnected();
}

template<typename MessageHead>
class CLogicQueueTCPConnection : public CTCPConnectionHandle<CSocketEpollPolicy>, public ILogicTCPConnection
{
  public:
    using MessageHeadType = MessageHead;

    void HeartBeat() override;

    void CmdHeartBeat(uint16_t usCmd) override;

    void RecvData() override;

    int32_t SendData(const char* data, size_t size) override;

    bool IsConnected() override;
};

using CLogicQueueConnection = CLogicQueueTCPConnection<CMsgHeadQueue>;

template <typename MessageHead>
void CLogicQueueTCPConnection<MessageHead>::HeartBeat()
{
    //
}

template <typename MessageHead>
void CLogicQueueTCPConnection<MessageHead>::CmdHeartBeat(uint16_t usCmd)
{
    MessageHeadType stMsgHead;
    stMsgHead.m_usCmd = usCmd;
    static char abyCodeBuf[MessageHeadType::SIZE];
    int32_t nTmpCodeLength = MessageHeadType::SIZE;
    if (success != encode_message_to_buffer(abyCodeBuf, nTmpCodeLength, stMsgHead))
    {
        SERVICE_LOG_TRACE_ERROR << "ENCODE CROSS HEART BEAT FAIL" << std::endl;
        return;
    }

    SendData(abyCodeBuf, static_cast<size_t>(nTmpCodeLength));
}

template <typename MessageHead>
void CLogicQueueTCPConnection<MessageHead>::RecvData()
{
    const auto ret = CTCPConnectionHandle<CSocketEpollPolicy>::RecvData();
    if (enmConnErrorCode_peer_closed == ret)
    {
        SERVICE_LOG_TRACE_ERROR << "peer(" << GetEntityType() << "," << GetEntityId()
                                << ") ip(" << GetStrIp() << ":" << GetPort() << "close link" << std::endl;
    }

    static char abyCodeBuf[max_package_size];
    size_t nTmpCodeLength = 0;
    while (GetOneCode(abyCodeBuf, nTmpCodeLength) > 0)
    {
        SERVICE_LOG_TRACE_DEBUG << "get one code from peer(" << GetEntityType() << "," << GetEntityId()
                                << ") ip(" << GetStrIp() << ":" << GetPort() << std::endl;

        MessageHeadType stMsgHead;
        char* pszBody = nullptr;
        int32_t iBodyLength = 0;

        if (success !=
            decode_buffer_to_message(static_cast<const char *>(abyCodeBuf), static_cast<int32_t>(nTmpCodeLength),
                                     stMsgHead, &pszBody, iBodyLength))
        {
            SERVICE_LOG_TRACE_ERROR << "decode server message failed!, code size = " << nTmpCodeLength << std::endl;
            continue;
        }

        SERVICE_API_INS->GetLogicSoGameApi()->ProcessServerMessage(stMsgHead, pszBody, iBodyLength);
    }
}

template <typename MessageHead>
int32_t CLogicQueueTCPConnection<MessageHead>::SendData(const char* data, size_t size)
{
    return CTCPConnectionHandle<CSocketEpollPolicy>::SendData(data, size);
}

template<typename MessageHead>
bool CLogicQueueTCPConnection<MessageHead>::IsConnected()
{
    return BeConnected();
}
