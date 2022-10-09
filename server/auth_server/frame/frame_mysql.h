#pragma once

#include <mysql.h>
#include "frame_common.h"
#include "frame_log.h"

#define DEFAULT_DB_CHARSET "utf8"
#define MAX_IP_LEN 64
#define MAX_ACCOUNT_LEN 32
#define MAX_DB_NAME 32
#define MAX_PWD_LEN 33
#define MAX_DB_COUNT 32
#define MAX_SQL_LEN  10240
#define DEFAULT_MYSQL_PORT     3306
#define BILL_NORMAL 0
#define BILL_JUST   1

typedef struct _mysql_cfg
{
    uint16_t port;
    uint16_t idx;
    char     db_ip[MAX_IP_LEN];
    char     usr_name[MAX_ACCOUNT_LEN];
    char     usr_pwd[MAX_PWD_LEN];
    char     db_name[MAX_DB_NAME];
} mysql_cfg_t;

typedef struct _mysql
{
    int32_t (*init_mysql)(void* data, mysql_cfg_t* cfg);
    int32_t (*sql_query)(void* data, int32_t* err);
    int32_t (*fetch_row)(void* data);
    MYSQL*      link;    //mysql链接
    MYSQL_RES*  records; //mysql结果集
    mysql_cfg_t* cfg;    //cfg文件
    MYSQL_ROW   cur_row; //正在处理的行
    uint32_t    last_active_time;   //上次活动时间
    int32_t     rows_num;//结果集行数
    int32_t     sql_len; //执行sql语句长度
    int16_t     is_query;//是否为select
    int16_t     is_record;//是否有记录
    int16_t     is_connected;
    char        sql[MAX_SQL_LEN];
} mysql_t;
typedef struct _buf_mysql
{
    int32_t pkg_len;
    int16_t db_idx;
    int8_t  just_bill;  //仅仅只是记录下账单
    char    sql[MAX_SQL_LEN];
} mysql_body_t;

#define frame_mysql_body_len(i) (sizeof(int32_t) + 2*sizeof(int16_t) + (i))

int32_t frame_mysql_module_init(record_t* log, mysql_cfg_t* cfg, int32_t count);
int32_t frame_mysql_module_destroy();
mysql_t* frame_mysql_get(int32_t idx);
int32_t frame_mysql_last_id(mysql_t* link);
int32_t frame_mysql_encode(mysql_body_t* info, char* out, int32_t* len);
int32_t frame_mysql_decode(mysql_body_t* info, char* in, int32_t len);
int32_t frame_mysql_free_result(mysql_t* link);
int32_t frame_mysql_string_escape(char* src, char* dst, mysql_t* link);
int32_t frame_mysql_check_alive(int32_t gap);
