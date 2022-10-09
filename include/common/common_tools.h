#pragma once

#include "common_base.h"
#include <string>
#include <sstream>

void create_pid_file(const char* pszName);
void delete_pid_file(const char* pszName);

bool create_directory(const char* pszPath);

bool copy_file(const char* source, const char* destination);

/*
* nochdir 非0，则daemon进程将改变当前工作目录到"/"
* noclodes 非0， 则daemon进程将重定向标准输入输出以及错误到"/dev/null"
*/
void InitDaemon(int8_t nochdir, int8_t noclose);

std::string CoreBacktrace(const std::string &version, int32_t iSigno);
template<typename T>
static std::string int2bin(T iNumber)
{
    if (iNumber == 0) return std::string("0");

    int iPosition = 0;
    const size_t uiSize = sizeof(T) * 8;
    std::string strRet(uiSize, '0');
    for (size_t i = 0; i < uiSize; ++i)
    {
        if (iNumber & ((T) 0x1 << i))
        {
            iPosition = uiSize - 1 - i;
            strRet[iPosition] = '1';
        }
    }
    return strRet.substr(iPosition);
}

template<typename T>
static int32_t bin1num(T iNumber)
{
	int32_t iCount = 0;
	while (iNumber)
	{
		iNumber = iNumber & (iNumber - 1);
		++iCount;
	}
	return iCount;
}

std::vector<std::string> string_split(const std::string& s, const std::string& delim);

template<class S, class T>
std::string vector_join(const std::vector<T>& elems, const S& delim) 
{
	std::stringstream ss;
	if (!elems.empty())
	{
		auto e = elems.begin();
		ss << *e++;
		for (; e != elems.end(); ++e) 
		{
			ss << delim << *e;
		}
	}
	return ss.str();
}

std::string url_encode(const char* pszInput, size_t nInputLength);

bool is_leader_card(int32_t card_id);

bool is_same_card(int32_t card_id1, int32_t card_id2);

#define STRNCPY(dst, src, length)					      \
	do								      \
	{								      \
		strncpy(dst, src, (length-1));			      \
		dst[(length-1)] = '\0';					      \
	}								      \
	while(0)

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete

#define GET_MACRO_STRING(MACRO) #MACRO
