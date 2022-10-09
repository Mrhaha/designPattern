#include "gy_session_creator.h"

#include <arpa/inet.h>

#include <stdlib.h>
#include <string.h>

int		
CGYSessionCreator::Encrypt(const char szKey[16], const gy_session_t& stSession, std::string& strSession)
{
	gy_session_t stTemp;
	stTemp.m_iGroupID = (int)htonl((unsigned int)stSession.m_iGroupID);
	stTemp.m_iUin = (int)htonl((unsigned int)stSession.m_iUin);
	stTemp.m_iTimestamp = (int)htonl((unsigned int)time(NULL));

	::srand(stTemp.m_iTimestamp);
	stTemp.m_iRandom = (int)htonl((unsigned int)rand());

	char pInBuffer[128], pOutBuffer[256];
	size_t uiOffset = 0;
	::memcpy(pInBuffer + uiOffset, &stTemp.m_iGroupID, sizeof(stTemp.m_iGroupID));
	uiOffset += sizeof(stTemp.m_iGroupID);
	::memcpy(pInBuffer + uiOffset, &stTemp.m_iUin, sizeof(stTemp.m_iUin));
	uiOffset += sizeof(stTemp.m_iUin);
	::memcpy(pInBuffer + uiOffset, &stTemp.m_iTimestamp, sizeof(stTemp.m_iTimestamp));
	uiOffset += sizeof(stTemp.m_iTimestamp);
	::memcpy(pInBuffer + uiOffset, &stTemp.m_iRandom, sizeof(stTemp.m_iRandom));
	uiOffset += sizeof(stTemp.m_iRandom);

	size_t uiLength = ::strlen(stSession.m_szExternAccount);
	*(pInBuffer + uiOffset) = (char)uiLength;
	uiOffset += sizeof(char);
	if(uiLength > 0)
	{
		::strcpy(pInBuffer + uiOffset, stSession.m_szExternAccount);
		uiOffset += uiLength;
	}

	uiLength = ::strlen(stSession.m_szInternAccount);
	*(pInBuffer + uiOffset) = (char)uiLength;
	uiOffset += sizeof(char);
	if(uiLength > 0)
	{
		::strcpy(pInBuffer + uiOffset, stSession.m_szInternAccount);
		uiOffset += uiLength;
	}

	size_t uiRet = CXTEA::Encrypt(pInBuffer, uiOffset, pOutBuffer, sizeof(pOutBuffer), szKey);
	if(0 == uiRet)
	{
		return (-1);
	}
	else
	{
		strSession = std::string(pOutBuffer, uiRet);
	}

	return (0);
}

int 			
CGYSessionCreator::Decrypt(const char szKey[16], char* pInBuffer, size_t uiInBufferLength, gy_session_t& stSession)
{
	char pOutBuffer[256];
	size_t uiRet = CXTEA::Decrypt(pInBuffer, uiInBufferLength, pOutBuffer, sizeof(pOutBuffer), szKey);
	if(0 == uiRet)
	{
		return (-1);
	}
	else
	{
		if(uiRet < (4 * sizeof(int)) + 2)
		{
			return (-2);
		}

		size_t uiOffset = 0;
		unsigned int uiGroupID = *((unsigned int*)(pOutBuffer + uiOffset));
		uiOffset += sizeof(unsigned int);
		unsigned int uiUin = *((unsigned int*)(pOutBuffer + uiOffset));
		uiOffset += sizeof(unsigned int);
		unsigned int uiTimestamp = *((unsigned int*)(pOutBuffer + uiOffset));
		uiOffset += sizeof(unsigned int);
		unsigned int uiRandom = *((unsigned int*)(pOutBuffer + uiOffset));
		uiOffset += sizeof(unsigned int);

		stSession.m_iGroupID = (int)ntohl(uiGroupID);
		stSession.m_iUin = (int)ntohl(uiUin);
		stSession.m_iTimestamp = (int)ntohl(uiTimestamp);
		stSession.m_iRandom = (int)ntohl(uiRandom);

		char ucLength = *(pOutBuffer + uiOffset);
		uiOffset += sizeof(char);
		if((uiOffset + (size_t)ucLength >= uiRet) || ((size_t)ucLength > ACCOUNT_LENGTH))
		{
			return (-3);
		}
		
		if(ucLength > 0)
		{
			::memcpy(stSession.m_szExternAccount, pOutBuffer + uiOffset, (size_t)ucLength);
		}
		stSession.m_szExternAccount[(size_t)ucLength] = '\0';
		uiOffset += (size_t)ucLength;

		ucLength = *(pOutBuffer + uiOffset);
		uiOffset += sizeof(char);
		if((uiOffset + (size_t)ucLength > uiRet) || ((size_t)ucLength > ACCOUNT_LENGTH))
		{
			return (-4);
		}

		if(ucLength > 0)
		{
			::memcpy(stSession.m_szInternAccount, pOutBuffer + uiOffset, (size_t)ucLength);
		}
		stSession.m_szInternAccount[(size_t)ucLength] = '\0';
	}

	return (0);
}
