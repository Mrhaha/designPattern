#pragma once
#include <string>

#ifndef LOGIC_BUILD_PLATFORM
#define LOGIC_BUILD_PLATFORM BUILD_PLATFORM_TCGAME
#endif

extern const char g_szServerGitSha1[];
extern const char g_szServerBuildDate[];
extern const char g_szServerBuildTime[];

std::string get_build_info();
