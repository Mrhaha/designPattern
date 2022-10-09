#pragma once

#include "authdb_config.h"
#include "authdb_cached.h"
#include "frame_md5.h"
#include "frame_xtea.h"
#include "frame_mysql.h"
#include "frame_log.h"

#define HALF_HOUR_SECS         1800
#define MAX_AUTH_OPTION_LEN    4096
#define XTEA_KEY_LEN      16
#define MAX_SIGNATURE_LEN 256
#if defined(_DEBUG) || defined(DEBUG)
#define MAX_TABLE_NUM     1    //表个数
#else
#define MAX_TABLE_NUM     32    //表个数
#endif
#define MMO_AUTH_NAME     "auth_info"
#define get_table_id(i) ((int32_t)(i % MAX_TABLE_NUM))

//认证请求
typedef struct _auth
{
	// 修改渠道ID [2018-01-11 modify by acshiryu]
	int32_t channel_id;
    // 新增服ID [2012-4-7 add by liuqy]
    int32_t group_id;
	// 新增平台 [2018-12-6 add by acshiryu]
	char    platform[MAX_AUTH_PLATFORM_LEN];
    char    account[MAX_AUTH_ACCOUNT_LEN];
	char	sdk[MAX_AUTH_SDK_LEN];
    int16_t opt_len;
    char    opt[MAX_AUTH_OPTION_LEN];
} req_auth_t;
//认证回复
typedef struct _auth_res
{
    int16_t result_id;
    int32_t uin;
	int32_t channel_id;
    int32_t group_id;
	int32_t is_new;
	char    platform[MAX_AUTH_PLATFORM_LEN];
    char    account[MAX_AUTH_ACCOUNT_LEN];
	char	sdk[MAX_AUTH_SDK_LEN];
    char    session_key[XTEA_KEY_LEN];
    int16_t sign_len;
    char    sign_buf[MAX_SIGNATURE_LEN];
} res_auth_t;
//signature信息
typedef struct _signature_t
{
    char ses_key[XTEA_KEY_LEN]; //session_key
    int32_t uin;
	int32_t channel_id;
	int32_t group_id;
	char platform[MAX_AUTH_PLATFORM_LEN];
    char account[MAX_AUTH_ACCOUNT_LEN];
	char sdk[MAX_AUTH_SDK_LEN];
    char service_bit[XTEA_KEY_LEN]; //没用到
    int32_t timestamp;
} auth_signature_t;

int32_t authdb_auth_req_decode(void* body, char* in, int32_t len);
int32_t authdb_auth_res_encode(void* body, char* out, int32_t* len);
int32_t authdb_auth_signature_encode(auth_signature_t* sig, char* out, int32_t* len);
int32_t authdb_auth_signature_decode(auth_signature_t* sig, char* in, int32_t len);
int32_t authdb_auth_build_res(res_auth_t* res);
int32_t authdb_auth_load(record_t* log, record_t* bill, cached_t* obj, mysql_t* link, role_info_t* role, int32_t is_updata);
int32_t authdb_auth_get_uin(int32_t* uin, mysql_t* link);
int32_t authdb_auth_update_time(record_t* log, record_t* bill, cached_t* obj, mysql_t* link, role_info_t* role);
role_info_t* authdb_auth_get_role(cached_t* obj, const char* platform, const char* sdk, int32_t group_id, int16_t is_create);

extern const char authdb_auth_crypt_key[];
