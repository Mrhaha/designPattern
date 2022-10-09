#pragma once

#include "common_base.h"

/************************************************************************/
/*					    各服务器之间通用的协议框架                      */
/************************************************************************/

class CMsgHeadCross
{
public:
	static constexpr auto SIZE = (sizeof(int32_t) + sizeof(uint32_t) + 2 * sizeof(int16_t) + sizeof(uint16_t) +  sizeof(int8_t));

	CMsgHeadCross();
	~CMsgHeadCross();

	int32_t encode(char* pszOut, int32_t& iOutLength);
	int32_t decode(const char* pszIn, int32_t iInLength);

	uint32_t m_iPackageLen;
	int8_t m_byCmdType;
	uint16_t m_usCmd;
	int32_t m_iUin;
	int16_t m_nGroupID;
	int16_t m_nSvrRsp;
};
