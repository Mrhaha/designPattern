#pragma once

#include <sstream>
#include "common_tools.h"

#define BUILD_PLATFORM_LOCAL 0
#define BUILD_PLATFORM_THHANT 1
#define BUILD_PLATFORM_TOUHOU 2
// #define BUILD_PLATFORM_HYKOKR 3
// #define BUILD_PLATFORM_ENGLISH 4

template<int platform>
std::string get_platform_info();

template<>
inline std::string get_platform_info<BUILD_PLATFORM_LOCAL>()
{
	return GET_MACRO_STRING(BUILD_PLATFORM_LOCAL);
}

template<>
inline std::string get_platform_info<BUILD_PLATFORM_THHANT>()
{
	return GET_MACRO_STRING(BUILD_PLATFORM_THHANT);
}

template<>
inline std::string get_platform_info<BUILD_PLATFORM_TOUHOU>()
{
	return GET_MACRO_STRING(BUILD_PLATFORM_TOUHOU);
}

/*
template<>
inline std::string get_platform_info<BUILD_PLATFORM_HYKOKR>()
{
    return GET_MACRO_STRING(BUILD_PLATFORM_HYKOKR);
}
*/

/*
template<>
inline std::string get_platform_info<BUILD_PLATFORM_ENGLISH>()
{
    return GET_MACRO_STRING(BUILD_PLATFORM_ENGLISH);
}
*/

void create_compile_file(const char* pszName, const char* git_sha1, const char* build_date, const char* build_time);
void delete_compile_file(const char* pszName);

#ifdef _MSC_VER // for MSVC
	#define __FORCE_INLINE__ __forceinline
	char *strptime(const char * __restrict, const char * __restrict, struct tm * __restrict);
#elif defined __GNUC__ // for gcc on Linux/Apple OS X
	#define __FORCE_INLINE__ __inline__ __attribute__((always_inline))
#else
	#define __FORCE_INLINE__ inline
#endif

#define __FILE_BASE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
