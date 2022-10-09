#include <cerrno>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <memory>

#include "common_msg_head_client.h"
#include "common_msg_head_connector.h"
#include "logic_service_poller.h"
#include "logic_service_logger.h"
#include "logic_service_connector_sender.h"
#include "logic_service_api.h"

CLogicConnectorSender::CLogicConnectorSender() : m_strIP(), m_usPort(0), m_usSendPort(0), m_iSenderFd(-1)
{
}

CLogicConnectorSender::~CLogicConnectorSender()
{
	if(m_iSenderFd >= 0)
	{
		::close(m_iSenderFd);
	}
}

int CLogicConnectorSender::Connect(const std::string& strConnectorIP, unsigned short usPort, unsigned short usSendPort)
{
	int iFd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(iFd < 0)
	{
        SERVICE_LOG_TRACE_ERROR << "CREATE SOCKET ERROR|MSG:" << ::strerror(errno) << std::endl;
		return (-1);
	}

	int iFlag = 1;
    ::setsockopt(iFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&iFlag, sizeof(iFlag));

	struct sockaddr_in stLocalAddr;
	stLocalAddr.sin_family = AF_INET;
	stLocalAddr.sin_port = ::htons(usSendPort);
	stLocalAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	if(::bind(iFd, (struct sockaddr*)&stLocalAddr, sizeof(stLocalAddr)) != 0)
   	{
        SERVICE_LOG_TRACE_ERROR << "BIND PORT ERROR:" << ::strerror(errno) << "|BIND_PORT:"
                        << usSendPort << std::endl;
   		::close(iFd);

		iFd = -1;

   		return (-1);
   	}

	struct sockaddr_in stConnectorAddr;
	stConnectorAddr.sin_family = AF_INET;
	stConnectorAddr.sin_port = ::htons(usPort);
	stConnectorAddr.sin_addr.s_addr = ::inet_addr(strConnectorIP.c_str());
	if(::connect(iFd, (struct sockaddr*)&stConnectorAddr, sizeof(stConnectorAddr)) != 0)
	{
        SERVICE_LOG_TRACE_ERROR << "CONNECT TO CONNECTOR ERROR|" << strConnectorIP.c_str()
                        << ":" << usPort << "|BIND PORT:" << usSendPort << "|MSG:" 
                        << ::strerror(errno) << std::endl;

		::close(iFd);

		iFd = -1;

		return (-1);
	}

	SERVICE_POLLER_INS->add(iFd, (long long)iFd, EPOLLIN);

	return (iFd);
}

void CLogicConnectorSender::PrintPackInfo(int iGroupID, int iUin, int iCmd, const msgpack::sbuffer & stBuffer, bool bIn)
{
	msgpack::unpacked stResult;
    std::string strIn = (bIn ? "RECV MSG" : "SEND MSG");
    try
    {
        msgpack::unpack(&stResult, stBuffer.data(), stBuffer.size());
    }
    catch (const std::runtime_error& stEx)
    {
        SERVICE_LOG_TRACE_DEBUG << strIn << "|GROUP_ID:" << iGroupID << "|UIN:" << iUin << "|CMD:" << iCmd << "|MSGPACK FORMAT ERROR|MSG:" << stEx.what() << std::endl;

        return ;
    }

    SERVICE_LOG_TRACE_DEBUG << strIn << "|GROUP_ID:" << iGroupID << "|UIN:" << iUin << "|CMD:" << iCmd << "|" << stResult.get() << std::endl;
}

bool CLogicConnectorSender::Initialize(const std::string& strConnectorIP, unsigned short usPort, unsigned short usSendPort)
{
	m_strIP = strConnectorIP;
	m_usPort = usPort;
	m_usSendPort = usSendPort;

	return (true);
}

int CLogicConnectorSender::Read()
{
	char pBuffer[256];

	if (m_iSenderFd < 0)
	{
		return (0);
	}

	int iRet = ::read(m_iSenderFd, pBuffer, sizeof(pBuffer));
	if (0 == iRet)
	{
		SERVICE_LOG_TRACE_ERROR << "CONNECTOR SENDER FAULT|CONNECTOR CLOSE THE SENDER CONNECTION" << std::endl;
		Close();
	}
	else
	{
		SERVICE_LOG_ROLL_DEBUG << "CONNECTOR SENDER READ HELLO FROM CONNECTOR!" << std::endl;
	}

	return (0);
}

int CLogicConnectorSender::GetFd() const
{
	return (m_iSenderFd);
}

void CLogicConnectorSender::Close()
{
	::close(m_iSenderFd);
	m_iSenderFd = -1;
}

bool CLogicConnectorSender::Check(int iNow)
{
	static int iTryTime = 0;
	if(m_iSenderFd < 0)
	{
		if(abs(iNow - iTryTime) > 10)
		{
			if(0 == iTryTime)
			{
                SERVICE_LOG_TRACE_DEBUG << "CONNECT TO CONNECTOR|" << m_strIP << ":" << m_usPort << std::endl;
			}
			else
			{
                SERVICE_LOG_TRACE_DEBUG << "MSG_LOGIC_RECONNECT TO CONNECTOR|" << m_strIP << ":" << m_usPort << std::endl;
			}

			iTryTime = iNow;

			if((m_iSenderFd = Connect(m_strIP, m_usPort, m_usSendPort)) < 0)
			{
                SERVICE_LOG_TRACE_ERROR << "MSG_LOGIC_RECONNECT TO CONNECTOR FAILED|" << m_strIP << ":" << m_usPort << std::endl;
				return (false);
			}
		}
	}

	return (true);
}

int CLogicConnectorSender::Send(const char* pBuffer, size_t uiLength)
{
	if(m_iSenderFd < 0)
	{
        SERVICE_LOG_TRACE_DEBUG << "MSG_LOGIC_RECONNECT TO CONNECTOR|" << m_strIP << ":" << m_usPort << std::endl;
		if((m_iSenderFd = Connect(m_strIP, m_usPort, m_usSendPort)) < 0)
		{
            SERVICE_LOG_TRACE_ERROR << "MSG_LOGIC_RECONNECT TO CONNECTOR FAILED|" << m_strIP << ":" << m_usPort << std::endl;
			return (-1);
		}
	}
    
    if(nullptr == pBuffer)
    {
        return (0);
    }
	
	size_t uiLeft = uiLength;
	while(uiLeft > 0)
	{
		 ssize_t iRet = ::write(m_iSenderFd, pBuffer + uiLength - uiLeft, uiLength);
		 if(iRet < 0)
		 {
            SERVICE_LOG_TRACE_ERROR << "WRITE TO CONNECTOR ERROR|" << m_strIP << ":" << m_usPort
                            << "|ALREADY SEND SIZE:" << uiLength - uiLeft << "|MSG:"
                            << ::strerror(errno) << std::endl; 

		 	return (int(uiLength - uiLeft));
		 }
		 else
		 {
		 	uiLeft -= (size_t)iRet;
		 }
	}

	return ((int)uiLength);
}

ssize_t CLogicConnectorSender::SendMultiBuffer(const std::vector<struct iovec> &stBufferList)
{
    if(m_iSenderFd < 0)
	{
        SERVICE_LOG_TRACE_DEBUG << "MSG_LOGIC_RECONNECT TO CONNECTOR|" << m_strIP << ":" << m_usPort << std::endl;
		if((m_iSenderFd = Connect(m_strIP, m_usPort, m_usSendPort)) < 0)
		{
            SERVICE_LOG_TRACE_ERROR << "MSG_LOGIC_RECONNECT TO CONNECTOR FAILED|" << m_strIP << ":" << m_usPort << std::endl;
			return 0;
		}
	}
    
    if(stBufferList.empty())
    {
        return 0;
    }
    
    struct iovec* pBufferList = new struct iovec[stBufferList.size()];
    for(size_t i = 0; i < stBufferList.size(); i ++)
    {
        pBufferList[i] = stBufferList[i];
    }
    
    ssize_t count = writev(m_iSenderFd, pBufferList, (int)stBufferList.size());
    
    delete[] pBufferList;

    return count;
}

bool CLogicConnectorSender::NotifyClientBuffer(int iGroupID, int iUin, int iCmd, int iIndex, int iErrcode, msgpack::sbuffer& stBuffer)
{
	int iTotal = 0;
	char pszBuffer[512];

	////////////////////////////////////////////////////////////////////////
	//////////////// Connector Header
	CMsgHeadConnector stHead;
	stHead.m_byControlType = head_type_transform_request;
	stHead.m_usConnectionIndex = (uint16_t)iIndex;
	stHead.m_stControlInfo.m_iTime = 0; // CLogicCommon::GetSec();
	iTotal += stHead.Size();

	////////////////////////////////////////////////////////////////////////
	//////////////// Client Header
	CMsgHeadClient stClientHead;
	stClientHead.m_stStaticData.m_iChannelID = 0;
	stClientHead.m_stStaticData.m_iGroupID = iGroupID;
	stClientHead.m_stStaticData.m_iUin = iUin;
	stClientHead.m_stStaticData.m_ucEncrpytFlag = 0;
	stClientHead.m_stStaticData.m_uiSequence = 0;
	stClientHead.m_stStaticData.m_uiVersion = 0;
	stClientHead.m_stStaticData.m_usCmd = (unsigned short)iCmd;
	stClientHead.m_stStaticData.m_iSvrRsp = static_cast<short>(iErrcode);
	stClientHead.m_stStaticData.m_ucSessionLen = 0;
	iTotal += sizeof(stClientHead.m_stStaticData);

	iTotal += stBuffer.size();

	////////////////////////////////////////////////////////////////////////
	//////////////// 
	if (SERVICE_API_INS->GetLogicSoGameApi()->NeedPrintMessageInfo())
	{
		PrintPackInfo(iGroupID, iUin, iCmd, stBuffer, false);
	}

	////////////////////////////////////////////////////////////////////////
	////////////////  Header Length
	stHead.m_iPackageSize = iTotal;
	stClientHead.m_stStaticData.m_uiPacketLen = (uint32_t)(iTotal - stHead.Size());
	int iOffset = stHead.Encode(pszBuffer, (int)sizeof(pszBuffer));
	iOffset += stClientHead.Encode(pszBuffer + iOffset, (int)sizeof(pszBuffer) - iOffset);

	////////////////////////////////////////////////////////////////////////
	////////////////  Multi Send 
	std::vector<struct iovec> stBufferList;
	struct iovec stTmpBuffer;
	stTmpBuffer.iov_base = pszBuffer;
	stTmpBuffer.iov_len = iOffset;

	stBufferList.push_back(stTmpBuffer);

	stTmpBuffer.iov_len = stBuffer.size();
	stTmpBuffer.iov_base = stBuffer.data();

	stBufferList.push_back(stTmpBuffer);

	auto iSendCount = SendMultiBuffer(stBufferList);

    SERVICE_LOG_TRACE_DEBUG << "SEND USER BUFFER|UIN:" << iUin << "|GROUP_ID:" << iGroupID << "|CMD:" << iCmd
                    << "|ERROR:" << iErrcode << "|TOTAL:" << iTotal << "|BODY:" << stBuffer.size()
                    << "|SENDER:" << iSendCount<< std::endl;

    if (iTotal != iSendCount)
    {
        SERVICE_LOG_TRACE_ERROR << "SEND USER BUFFER ERROR|UIN:" << iUin << "|GROUP_ID:" << iGroupID << "|CMD:" << iCmd
                        << "|ERROR:" << iErrcode << "|TOTAL:" << iTotal << "|BODY:" << stBuffer.size()
                        << "|SENDER:" << iSendCount<< std::endl;
    }

	return true;
}

void CLogicConnectorSender::NotifyCloseClient(int iUin, unsigned short usConnectionFd, int iSourceIP, unsigned short usPort)
{
	char szBuffer[256];
	CMsgHeadConnector stHead;
	stHead.m_byControlType = head_type_force_close_connection;
	stHead.m_usConnectionIndex = usConnectionFd;
	stHead.m_stControlInfo.m_stDisconnectInfo.m_byDisconnectType = enmForceCloseType_Now;
	stHead.m_stControlInfo.m_stDisconnectInfo.m_iSourceIP = iSourceIP;
	stHead.m_stControlInfo.m_stDisconnectInfo.m_usSourcePort = usPort;
	stHead.m_iPackageSize = stHead.Size();

	struct in_addr stAddr;
	stAddr.s_addr = (unsigned long)iSourceIP;
	int iLength = stHead.Encode(szBuffer, sizeof(szBuffer));
	if(iLength <= 0)
	{
        SERVICE_LOG_TRACE_DEBUG << "ENCODE HEAD ERROR|UIN:" << iUin << "|FD:" << usConnectionFd
                        << "|" << ::inet_ntoa(stAddr) << ":" << usPort << std::endl;
		return;
	}
	
	int iRet = Send(szBuffer, iLength);
	if(iRet != iLength)
	{
        SERVICE_LOG_TRACE_DEBUG << "SEND NOTIFY BUFFER ERROR|TOTAL:" << iLength << "|ALREADY SEND:"
                        << iRet << "|UIN:" << iUin << "|FD:" << usConnectionFd 
                        << ::inet_ntoa(stAddr) << ":" << usPort << std::endl;
	}

	SERVICE_API_INS->GetLogicSoGameApi()->DelClientUser(iUin);
    
    SERVICE_LOG_TRACE_DEBUG << "NOTIFY CLIENT CLOSE SUCCESS|UIN:" << iUin << "|FD:" << usConnectionFd 
                    << "|REMOTE:" << ::inet_ntoa(stAddr) << ":" << usPort << std::endl;
}

stLogicSoSenderUser CLogicConnectorSender::GetClientInfo(int iGroupID, int iUin)
{
	return SERVICE_API_INS->GetLogicSoGameApi()->GetClientUser(iGroupID, iUin);
}

