#pragma once
#include <string>

#ifndef CROSS_BUILD_PLATFORM
#define CROSS_BUILD_PLATFORM BUILD_PLATFORM_TCGAME
#endif

extern const char g_GIT_SHA1[];
extern const char g_BUILD_DATE[];
extern const char g_BUILD_TIME[];

std::string get_build_info();
