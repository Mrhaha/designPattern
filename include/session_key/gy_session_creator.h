#pragma once

#include <string>

#include "gy_xtea.h"

class CGYSessionCreator
{
public:
	enum ACCOUNT
	{
		ACCOUNT_LENGTH = 48
	};

	typedef struct gy_session_s
	{
		int					m_iGroupID;
		int					m_iUin;
		int					m_iTimestamp;
		int 				m_iRandom;

		char				m_szExternAccount[ACCOUNT_LENGTH + 1];
		char				m_szInternAccount[ACCOUNT_LENGTH + 1];
	}gy_session_t;

public:
	static int		Encrypt(const char szKey[16], const gy_session_t& stSession, std::string& strSession);

	static int 		Decrypt(const char szKey[16], char* pInBuffer, size_t uiInBufferLength, gy_session_t& stSession);
};
