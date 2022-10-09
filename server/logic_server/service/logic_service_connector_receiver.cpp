#include <time.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include "server_base_protocol.h"
#include "server_cmd_logic_define.h"

#include "common_msg_head_connector.h"
#include "common_msg_head_client.h"
#include "logic_service_logger.h"
#include "logic_service_statistics.h"
#include "logic_service_api.h"
#include "logic_service_connector_receiver.h"

CLogicConnectorReceiver::CLogicConnectorReceiver() : m_strPeerIP(), m_pBuffer{}
{
	m_iFd = -1;
	m_usPeerPort = 0;
	m_iHeartBeatTime = ::time(nullptr);
	m_iLength = 0;
}

CLogicConnectorReceiver::~CLogicConnectorReceiver()
{
	if(m_iFd >= 0)
	{
		::close(m_iFd);
	}
}

void	
CLogicConnectorReceiver::SetFd(int iFd, const std::string& strConnectorIP, unsigned short usPort)
{
	if(m_iFd >= 0)
	{
		::close(m_iFd);
	}

	m_iFd = iFd;
	m_strPeerIP = strConnectorIP;
	m_usPeerPort = usPort;
	m_iLength = 0;
}

int 	
CLogicConnectorReceiver::GetFd() const
{
	return (m_iFd);
}

int 	
CLogicConnectorReceiver::Read()
{
	if(m_iFd < 0)
	{
        SERVICE_LOG_TRACE_ERROR << "CAN'T NOT READ FROM CONNECTOR|MSG:FD INVALID" << std::endl; 
		return (-1);
	}

	bool bIsClose = false;
	while(1)
	{
		ssize_t iRet = ::read(m_iFd, m_pBuffer + m_iLength, MAX_CONNECTOR_PACKET - m_iLength);
		if(iRet > 0)
		{
			m_iLength += (int)iRet;
		}
		else if(0 == iRet)
		{
            SERVICE_LOG_TRACE_ERROR << "READ FROM CONNECTOR SUCCESS|CONNECTOR CLOSE CONNECTION|"
                            << m_strPeerIP << ":" << m_usPeerPort << std::endl;
			
			bIsClose = true;
		}
		else
		{
			if(EINTR == errno)
			{
                SERVICE_LOG_TRACE_DEBUG << "READ FROM CONNECTOR INTERRUPT BY SIGNAL" << std::endl;
				continue;
			}

            SERVICE_LOG_TRACE_ERROR << "READ FROM CONNECTOR ERROR|MSG:" << ::strerror(errno) << std::endl;
			
			bIsClose = true;
		}

		break;
	}

	int iOffset = 0;
	while(1)
	{
		if(CMsgHeadConnector::MiniSize() < m_iLength)
		{
			CMsgHeadConnector stHead;
			int iRet = stHead.Decode(m_pBuffer + iOffset, m_iLength);
			if(Enum_MHPE_BUFFER_TOO_SMALL == iRet)
			{
				if((iOffset > 0) && (m_iLength > 0))
				{
					::memmove(m_pBuffer, m_pBuffer + iOffset, m_iLength);
				}

                SERVICE_LOG_TRACE_DEBUG << "RECEVICE DATA SMALL THAN CONNECTOR HEADER|WAIT FOR NEW DATA" << std::endl;
				break;
			}
			else if(Enum_MHPE_DATA_ERROR == iRet)
			{
                SERVICE_LOG_TRACE_ERROR << "CONNECTOR HEADER ERROR|CRITICAL ERROR|DROP ALL BUFFER DATA" << std::endl;
				m_iLength = 0;
				break;
			}
			else
			{
                if(stHead.m_iPackageSize > (uint32_t)m_iLength)
                {
                    if(stHead.m_iPackageSize > MAX_CONNECTOR_PACKET)
                    {
                        SERVICE_LOG_TRACE_ERROR << "RECEVICE DATA ERROR|PACKET IS LARGER THAN MAX_CONNECTOR_PACKET|MAX_CONNECTOR_PACKET:" 
                                        << MAX_CONNECTOR_PACKET << "|COMPLETE PACKET SIZE:" << stHead.m_iPackageSize 
                                        << std::endl;
                        
                        m_iLength = 0;
                    }
                    else
                    {
                        if((iOffset > 0) && (m_iLength > 0))
                        {
                            ::memmove(m_pBuffer, m_pBuffer + iOffset, m_iLength);
                        }

                        SERVICE_LOG_TRACE_DEBUG << "RECEVICE DATA SMALL THAN COMPLETE PACKET|WAIT FOR NEW DATA" << std::endl;
                    }
                    
                    break;
                }
                
				if(stHead.m_byControlType == head_type_transform_request)
				{
					int iTime = stHead.m_stControlInfo.m_iTime;

					CMsgHeadClient stProto;
					int iClientProtoLen = stProto.Decode(m_pBuffer + iOffset + iRet, m_iLength - iRet);
					if(iClientProtoLen < 0)
					{
                        SERVICE_LOG_TRACE_ERROR << "DECODE GATEWAY CLIENT HEADER ERROR|TRANSFORM REQUEST|SKIP TO NEXT PACKET|TIME:"
                                        << iTime << std::endl;
					}
					else
					{
                        if((uint32_t)stProto.m_stStaticData.m_uiPacketLen + iRet != stHead.m_iPackageSize)
                        {
                            SERVICE_LOG_TRACE_ERROR << "DECODE GATEWAY PACKET ERROR|TRANSFORM REQUEST|TOTAL PACKET SIZE IN CONNECTOR HEADER:"
                                            << stHead.m_iPackageSize << "|CONNECTOR HEADER SIZE:" << iRet 
                                            << "|TOTAL PACKET SIZE IN GATEWAY HEADER:" << stProto.m_stStaticData.m_uiPacketLen 
                                            << "|GATEWAY HEADER SIZE:" << iClientProtoLen << "|CLEAR BUFFER" << std::endl;
                            
                            m_iLength = 0;
                            break;
                        }
                        
						int iCmd = (int)stProto.m_stStaticData.m_usCmd;
                        SERVICE_LOG_TRACE_DEBUG << "DECODE GATEWAY CLIENT HEADER SUCCESS|TRANSFORM REQUEST|CMD:"
                                        << iCmd << "|TIME:" << iTime << std::endl;
                        
						//if((iCmd <= CRkServerCmd::LOGIC_CMD_BEGIN) || (iCmd >= CRkServerCmd::LOGIC_CMD_END))
						//{
                        //    SERVICE_LOG_TRACE_ERROR << "CHANNELID:" << stProto.m_stStaticData.m_iChannelID
                        //                    << "|GROUPID:" << stProto.m_stStaticData.m_iGroupID
                        //                    << "|UIN:" << stProto.m_stStaticData.m_iUin << "|SEQUENCE:"
                        //                    << stProto.m_stStaticData.m_uiSequence << "|CMD:"
                        //                    << stProto.m_stStaticData.m_usCmd
                        //                    << "|CMD NOT SUPPORT|CLOSE CLIENT CONNECTION" << std::endl;
						//}

                        SERVICE_API_INS->GetLogicSoGameApi()->ProcessUserMessage(stProto.m_stStaticData.m_iUin
                        		, stProto.m_stStaticData.m_iGroupID
                        		, stProto.m_stStaticData.m_iChannelID
                        		, iCmd
                        		, m_pBuffer + iOffset + iRet + iClientProtoLen
                        		, stProto.m_stStaticData.m_uiPacketLen - iClientProtoLen);
					}

					iOffset += stHead.m_iPackageSize;
					m_iLength -= stHead.m_iPackageSize;
				}
				else if(stHead.m_byControlType == head_type_start_request)
				{                    
					struct sockaddr_in stClientAddr;
					stClientAddr.sin_addr.s_addr = stHead.m_stControlInfo.m_stSourceIPInfo.m_iSourceIP;
					char* szClientIP = ::inet_ntoa(stClientAddr.sin_addr);
					unsigned short usClientPort = stHead.m_stControlInfo.m_stSourceIPInfo.m_usSourcePort;

					CMsgHeadClient stProto;
					int iClientProtoLen = stProto.Decode(m_pBuffer + iOffset + iRet, m_iLength - iRet);
					if(iClientProtoLen < 0)
					{
                        SERVICE_LOG_TRACE_ERROR << "DECODE GATEWAY CLIENT HEADER ERROR|START REQUEST|SKIP TO NEXT PACKET|"
                                        << szClientIP << ":" << usClientPort << std::endl;
					}
					else
					{
                        if((uint32_t)stProto.m_stStaticData.m_uiPacketLen + iRet != stHead.m_iPackageSize)
                        {
                            SERVICE_LOG_TRACE_ERROR << "DECODE GATEWAY PACKET ERROR|TRANSFORM REQUEST|TOTAL PACKET SIZE IN CONNECTOR HEADER:"
                                            << stHead.m_iPackageSize << "|CONNECTOR HEADER SIZE:" << iRet 
                                            << "|TOTAL PACKET SIZE IN GATEWAY HEADER:" << stProto.m_stStaticData.m_uiPacketLen 
                                            << "|GATEWAY HEADER SIZE:" << iClientProtoLen << "|CLEAR BUFFER" << std::endl;

                            m_iLength = 0;
                            break;
                        }
                        
                        int iCmd = (int)stProto.m_stStaticData.m_usCmd;
						stLogicSoClientInfo stUserInfo;
                        
						stUserInfo.m_iClientIP = stHead.m_stControlInfo.m_stSourceIPInfo.m_iSourceIP;
						stUserInfo.m_usClientPort = stHead.m_stControlInfo.m_stSourceIPInfo.m_usSourcePort;
						stUserInfo.m_usConnectionIndex = stHead.m_usConnectionIndex;
						stUserInfo.m_strClientIP = std::string(szClientIP);
						stUserInfo.m_iChannelID = stProto.m_stStaticData.m_iChannelID;

                        /* 第一个数据包并不一定是 MSG_LOGIC_GET_USER_INFO 或 MSG_LOGIC_RECONNECT
                        if((iCmd == CRkServerCmd::MSG_LOGIC_GET_USER_INFO) || (iCmd == CRkServerCmd::MSG_LOGIC_RECONNECT))
                        {
                            uint32_t uiLength = stProto.m_stStaticData.m_uiPacketLen - iClientProtoLen;
                            char* pBuffer = m_pBuffer + iOffset + iRet + iClientProtoLen;

                            msgpack::unpacked stResult;
                            if(uiLength > 0)
                            {
                                try
                                {
                                    CPackUserStatInfo stStatInfo;
                                    msgpack::unpack(&stResult, pBuffer, uiLength);
                                    stResult.get().convert(&stStatInfo);
                                    stUserInfo.m_stStatInfo.m_strAccount = stStatInfo.m_strAccount;
                                    stUserInfo.m_stStatInfo.m_strDeviceCarrier = stStatInfo.m_strDeviceCarrier;
                                    stUserInfo.m_stStatInfo.m_strDeviceID = stStatInfo.m_strDeviceID;
                                    stUserInfo.m_stStatInfo.m_strDeviceModel = stStatInfo.m_strDeviceModel;
                                    stUserInfo.m_stStatInfo.m_strDeviceNet = stStatInfo.m_strDeviceNet;
                                    stUserInfo.m_stStatInfo.m_strDeviceOS = stStatInfo.m_strDeviceOS;
                                    stUserInfo.m_stStatInfo.m_strDeviceRatio = stStatInfo.m_strDeviceRatio;
                                    stUserInfo.m_stStatInfo.m_strPlatformID = stStatInfo.m_strPlatformID;
                                    stUserInfo.m_stStatInfo.m_strPlatformSign = stStatInfo.m_strPlatformSign;
                                }
                                catch(const std::runtime_error& stEx)
                                {
                                    SERVICE_LOG_TRACE_ERROR << "GROUP ID:" << stProto.m_stStaticData.m_iGroupID << "|UIN:" 
                                                    << stProto.m_stStaticData.m_iUin << "|HAVE NO STAT DATA:" << std::endl;
                                }
                                catch(const std::bad_cast& stEx)
                                {
                                    SERVICE_LOG_TRACE_ERROR << "GROUP ID:" << stProto.m_stStaticData.m_iGroupID << "|UIN:" 
                                                    << stProto.m_stStaticData.m_iUin << "|PROTO IS NOT MATCH|MSG:" 
                                                    << stEx.what() << "|PROTO_DATA:" << stResult.get() << std::endl;
                                }
                            }
                        }
                        */
                        

						const bool bSuccess = SERVICE_API_INS->GetLogicSoGameApi()->AddClientUser(stProto.m_stStaticData.m_iGroupID, stProto.m_stStaticData.m_iUin, stUserInfo);
						if(!bSuccess)
						{
                            SERVICE_LOG_TRACE_DEBUG << "NEW USER KICK OLD|PLATFORMID:" << stProto.m_stStaticData.m_iChannelID
                                            << "|GROUPID:" << stProto.m_stStaticData.m_iGroupID << "|UIN:"
                                            << stProto.m_stStaticData.m_iUin << "|CMD:" 
                                            << stProto.m_stStaticData.m_usCmd << std::endl;

                            CResponseNotifyClientLogout empty;
                            empty.m_iLogoutType = CResponseNotifyClientLogout::ENCLT_Repeat_Login;
							SERVICE_API_INS->GetConnectorSender()->NotifyClientUser(stProto.m_stStaticData.m_iGroupID, stProto.m_stStaticData.m_iUin, MSG_LOGIC_NOTIFY_CLIENT_LOGOUT, empty);

                            SERVICE_API_INS->GetLogicSoGameApi()->ProcessUserMessage(stProto.m_stStaticData.m_iUin
                            		, stProto.m_stStaticData.m_iGroupID
                            		, stProto.m_stStaticData.m_iChannelID
                            		, MSG_LOGIC_NULL_CMD
                            		, nullptr
                            		, 0);

							SERVICE_API_INS->GetLogicSoGameApi()->DelClientUser(stProto.m_stStaticData.m_iUin);
							SERVICE_API_INS->GetLogicSoGameApi()->AddClientUser(stProto.m_stStaticData.m_iGroupID, stProto.m_stStaticData.m_iUin, stUserInfo);
						}

						
                        SERVICE_LOG_TRACE_DEBUG << "DECODE GATEWAY CLIENT HEADER SUCCESS|START REQUEST|CMD:" << iCmd
                                        << "|" << szClientIP << ":" << usClientPort << std::endl;

						SERVICE_STATISTICS->UpdateLogin(stProto.m_stStaticData.m_iUin, stProto.m_stStaticData.m_iGroupID);

						SERVICE_API_INS->GetLogicSoGameApi()->ProcessUserMessage(stProto.m_stStaticData.m_iUin
								, stProto.m_stStaticData.m_iGroupID
								, stProto.m_stStaticData.m_iChannelID
								, iCmd
								, m_pBuffer + iOffset + iRet + iClientProtoLen
								, stProto.m_stStaticData.m_uiPacketLen - iClientProtoLen);
					}

					iOffset += stHead.m_iPackageSize;
					m_iLength -= stHead.m_iPackageSize;
				}
				else if(stHead.m_byControlType == head_type_disconnect_notify)
				{
					static const char* pInvalidConnectionReason = "INVALID CONNECTION";
					static const char* pInvalidSignatureReason = "INVALID SIGNATURE";
					static const char* pClientLogout = "CLIENT LOGOUT";
					static const char* pLogicReject = "LOGIC SERVICE REJECT USER";
					static const char* pClientTimeout = "CLIENT TIMEOUT";

					const char* pReason = pInvalidConnectionReason;

					int iUin = stHead.m_stControlInfo.m_stClientDisconnect.m_iUin;
					int iReason = (int)stHead.m_stControlInfo.m_stClientDisconnect.m_byDisconnectReason;
					short iPlatformID = stHead.m_stControlInfo.m_stClientDisconnect.m_iPlatformID;
					short iGroupID = stHead.m_stControlInfo.m_stClientDisconnect.m_iGroupID;

					int iInvalidConnection = enmClosedReason_InvalidConnection;
					int iInvalidSignature = enmClosedReason_InvalidSignature;
					int iClientLogout = enmClosedReason_Client;
					int iLogicReject = enmClosedReason_Service;
					int iClientTimeout = enmClosedReason_ClientTimeOut;

					if(iInvalidSignature == iReason)
					{
						pReason = pInvalidSignatureReason;
					}
					else if(iClientLogout == iReason)
					{
						pReason = pClientLogout;
					}
					else if(iLogicReject == iReason)
					{
						pReason = pLogicReject;
					}
					else if(iClientTimeout == iReason)
					{
						pReason = pClientTimeout;
					}

					if((iInvalidConnection == iReason) || (iInvalidSignature == iReason))
					{
						//maybe attack
                        SERVICE_LOG_TRACE_WARN << "PLATFORMID:" << iPlatformID << "|GROUPID:" << iGroupID
                                       << "|UIN:" << iUin << "|MAYBE ATTACK|MSG:" << pReason 
                                       << std::endl;
					}
					else
					{
                        SERVICE_LOG_TRACE_DEBUG << "PLATFORMID:" << iPlatformID << "|GROUPID:" << iGroupID
                                        << "|UIN:" << iUin << "|MSG:" << pReason << std::endl;
					}

					SERVICE_API_INS->GetLogicSoGameApi()->DelClientUser(iUin);

					m_iLength -= iRet;
					iOffset += iRet;
				}
				else
				{
                    SERVICE_LOG_TRACE_ERROR << "CONNECTOR HEADER ERROR|CONTROL TYPE UNKNOWN|DROP ALL BUFFER DATA" << std::endl;
					m_iLength = 0;
					break;
				}
			}	
		}
		else
		{
			if((iOffset > 0) && (m_iLength > 0))
			{
				::memmove(m_pBuffer, m_pBuffer + iOffset, m_iLength);
			}

            if(m_iLength < 0)
            {
                m_iLength = 0;
            }
            
			break;
		}
	}

	if(true == bIsClose)
	{
        SERVICE_LOG_TRACE_WARN << "CONNECTOR CLOSE RECEIVER CONNECTION|" << m_strPeerIP
                       << ":" << m_usPeerPort << std::endl;

		::close(m_iFd);
		m_iFd = -1;
		m_usPeerPort = 0;
		m_strPeerIP = "";
	}

	return (m_iLength);
}

int 	
CLogicConnectorReceiver::HeartBeat(int iNow)
{
	static char c = '!';

	if(m_iFd < 0)
	{
		return (0);
	}

	if(abs(iNow - m_iHeartBeatTime) > 60)
	{
		::write(m_iFd, &c, sizeof(c));
		m_iHeartBeatTime = iNow;
        SERVICE_LOG_ROLL_DEBUG << "HEART TO CONNECTOR!!!" << std::endl;
	}

	return (0);
}


