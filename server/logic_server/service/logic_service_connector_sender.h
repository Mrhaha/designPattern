#pragma once

#include <sys/uio.h>

#include <string>

#include "i_logic_connector_sender.h"

class CLogicConnectorSender : public ILogicConnectorSender
{
public:
	CLogicConnectorSender();

	~CLogicConnectorSender() override;

	bool		Initialize(const std::string& strConnectorIP, unsigned short usPort, unsigned short usSendPort);

	int 		Read();

	int 		GetFd() const;

	void		Close();

	bool 		Check(int iNow);

public:
	int 		Send(const char* pBuffer, size_t uiLength) override;
    
    ssize_t     SendMultiBuffer(const std::vector<struct iovec> &stBufferList) override;

	bool        NotifyClientBuffer(int iGroupID, int iUin, int iCmd, int iIndex, int iErrcode, msgpack::sbuffer& stBuffer) override;

	void		NotifyCloseClient(int iUin, unsigned short usConnectionFd, int iSourceIP, unsigned short usPort) override;

	stLogicSoSenderUser GetClientInfo(int iGroupID, int iUin) override;

private:
	int 		Connect(const std::string& strConnectorIP, unsigned short usPort, unsigned short usSendPort);

	void        PrintPackInfo(int iGroupID, int iUin, int iCmd, const msgpack::sbuffer& stBuffer, bool bIn);

private:
	std::string			m_strIP;

	unsigned short		m_usPort;

	unsigned short 		m_usSendPort;

	int 				m_iSenderFd;
};


