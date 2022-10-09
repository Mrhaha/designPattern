#pragma once
#include <string>

#ifndef QUEUE_BUILD_PLATFORM
#define QUEUE_BUILD_PLATFORM BUILD_PLATFORM_TCGAME
#endif

extern const char g_GIT_SHA1[];
extern const char g_BUILD_DATE[];
extern const char g_BUILD_TIME[];

std::string get_build_info();
