#pragma once
#include <vector>

#include "msgpack.hpp"
#include "common_message.h"
#include "common_msg_head_cross.h"
#include "common_msg_head_queue.h"

class ILogicTCPConnection
{
public:
	virtual ~ILogicTCPConnection() = default;

	virtual void HeartBeat() = 0;

    virtual void CmdHeartBeat(uint16_t usCmd) = 0;

	virtual void RecvData() = 0;

	virtual int32_t SendData(const char* data, size_t size) = 0;

	virtual bool IsConnected() = 0;

	template <class MessageHead, class MSGPACK_T>
	bool SendToServer(MessageHead& stMsgHead, MSGPACK_T& stParam);

	template <class MessageHead>
	bool SendToServer(MessageHead& stMsgHead);
};

template <class MessageHead, class MSGPACK_T>
bool ILogicTCPConnection::SendToServer(MessageHead& stMsgHead, MSGPACK_T& stParam)
{
	static char szPackage[max_package_size] = { 0 };
	int32_t iPackageSize = sizeof(szPackage);
	if (success != encode_message_to_buffer(szPackage, iPackageSize, stMsgHead, stParam))
	{
		return false;
	}

	SendData(szPackage, static_cast<size_t>(iPackageSize));

	return true;
}

template<class MessageHead>
bool ILogicTCPConnection::SendToServer(MessageHead &stMsgHead)
{
	static char szPackage[max_package_size] = { 0 };
	int32_t iPackageSize = sizeof(szPackage);
	if (success != encode_message_to_buffer(szPackage, iPackageSize, stMsgHead))
	{
		return false;
	}

	SendData(szPackage, static_cast<size_t>(iPackageSize));

	return true;
}

