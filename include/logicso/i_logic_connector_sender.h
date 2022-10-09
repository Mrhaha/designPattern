#pragma once
#include <vector>

#include "msgpack.hpp"
#include "logicso_define.h"

class ILogicConnectorSender
{
public:
	virtual ~ILogicConnectorSender() = default;

	virtual int Send(const char* pBuffer, size_t uiLength) = 0;

	virtual ssize_t SendMultiBuffer(const std::vector<struct iovec> &stBufferList) = 0;

	virtual void NotifyCloseClient(int iUin, unsigned short usConnectionFd, int iSourceIP, unsigned short usPort) = 0;

	virtual bool NotifyClientBuffer(int iGroupID, int iUin, int iCmd, int iIndex, int iErrcode, msgpack::sbuffer& stBuffer) = 0;

	virtual stLogicSoSenderUser GetClientInfo(int iGroupID, int iUin) = 0;

public:
	template<typename T>
	bool NotifyClientUser(int iGroupID, int iUin, int iCmd, T& stRsp, int iErrcode = 0);
};

template <typename T>
bool ILogicConnectorSender::NotifyClientUser(int iGroupID, int iUin, int iCmd, T& stRsp, int iErrcode)
{
	const auto stClientInfo = GetClientInfo(iGroupID, iUin);
	if (stClientInfo.m_iConnectionIndex < 0)
	{
		return false;
	}
	
	stRsp.m_stSyncData.m_iSyncGold = stClientInfo.m_iUserGold;
	stRsp.m_stSyncData.m_iSyncYuanBao = stClientInfo.m_iUserRmb;
	stRsp.m_stSyncData.m_iSyncPhyPower = stClientInfo.m_iUserPhyPower;
	stRsp.m_stSyncData.m_iSyncLastAutoAddTime = stClientInfo.m_iUserLastAutoAddTime;

	msgpack::sbuffer stBuffer;
	msgpack::pack(stBuffer, stRsp);

	NotifyClientBuffer(iGroupID, iUin, iCmd, stClientInfo.m_iConnectionIndex, iErrcode, stBuffer);

	return false;
}
