#pragma once

#include "frame_common.h"
#include "authdb_define.h"
#define MAX_AUTH_PLATFORM_LEN 65
#define MAX_AUTH_ACCOUNT_LEN 65
#define MAX_AUTH_SDK_LEN 65
#define MAX_ROLE_COUNT       32
//[2012-4-7 add by liuqy]
typedef struct _role
{
    int32_t uin;
	int32_t channel_id;
    int32_t group_id;
    int32_t last_login_time;
    int32_t is_load:1;
	int32_t is_new : 1;
	char    platform[MAX_AUTH_PLATFORM_LEN];
	char    account[MAX_AUTH_ACCOUNT_LEN];
	char	sdk[MAX_AUTH_SDK_LEN];
} role_info_t;
typedef struct _cached
{
    uint64_t hash_md5;
    char     account[MAX_AUTH_ACCOUNT_LEN];
    int16_t  role_count;
    role_info_t roles[MAX_ROLE_COUNT];
    int32_t  is_load_normal:1;
} cached_t;
