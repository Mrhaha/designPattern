#pragma once

class CLogicConnectorReceiver
{
public:
	enum COMMON
	{
		MAX_CONNECTOR_PACKET = 4096
	};

	CLogicConnectorReceiver();

	~CLogicConnectorReceiver();

	void	SetFd(int iFd, const std::string& strConnectorIP, unsigned short usPort);

	int 	GetFd() const;

	int 	Read();

	int 	HeartBeat(int iNow);

private:
	int                             m_iFd;

	std::string                     m_strPeerIP;

	unsigned short                  m_usPeerPort;

	int                             m_iHeartBeatTime;

	int                             m_iLength;

	char                            m_pBuffer[MAX_CONNECTOR_PACKET];
};


