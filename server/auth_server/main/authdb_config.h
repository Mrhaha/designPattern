#pragma once

#include "frame_common.h"
#include "frame_mysql.h"

#define CONFIG_FILE "./config/authdbsvrd.ini"
#define PS_PREFIX_NAME "authdb: "

#define DEFAULT_PROCESS_COUNT  2
#define DEFAULT_SQL_PROC_COUNT 1
#define DEFAULT_LISTEN_PORT    12532
#define DEFAULT_LOG_MASK       -1
#define DEFAULT_MAX_FD         128
#define DEFAULT_SOCK_INTERVAL  10    //10s
#define DEFAULT_SOCK_UNALIVE   90    //90s
#define DEFAULT_WAIT_FIRST_PKG 30    //30s
#define DEFAULT_CACHED_COUNT   100
#define DEFAULT_SHM_KEY        700000
#define DEFAULT_SECRET_KEY     33
#define DEFAULT_DB_TIMEOUT     5*60

typedef struct _db_cfg
{
    char local_ip[MAX_IP_LEN]; //本地ip
    char secret_key[DEFAULT_SECRET_KEY];
    mysql_cfg_t* db_cfg;    //mysql配置
    int32_t  log_mask;      //日志等级
    uint16_t port;          //监听端口
    int16_t  worker_count;  //工作进程数
    int16_t  sql_worker_count;//sql执行工作进程
    int16_t  db_count;      //连接mysql数量
    int16_t  max_fd;        //最大连接数
    int16_t  check_interval;//超时检查间隔
    int16_t  socket_alive;  //连接成活时间
    int16_t  wait_first_pkg;//等待第一个包时间
    int32_t  cached_count;  //缓存多少数据
    key_t    shm_key;       //共享内存key
	uint32_t db_timeout;    //检查db超时时间
} authdb_cfg_t;

int32_t authdb_config_init(void* data);
int32_t authdb_config_parse(const char* file, void* data);
int32_t authdb_config_dump(void* data, void* log);
void    authdb_config_destroy(void* data);

extern config_t authdb_config_info;
