#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include <stdint.h>

#ifdef __GNUC__ //gnu gcc/g++
#include <stdint.h>
#include <inttypes.h>
#endif // __GNUC__
#define MAX_PROCESS_COUNT   128
#define MAX_SQL_WORKER_COUNT 32
#define MAX_ROOT_PATH_LEN   256

#define RECORD_LOG           0  //记录日志
#define RECORD_BILL          1  //记录账单

#define IS_OK      1
#define IS_NOT_OK  0

#define success 0
#define fail    1
#define set_void(n) ((void*)n)
#if 64 == __WORDSIZE
#define UINT64_FMT "lu"
#define INT64_FMT "ld"
#else
#define UINT64_FMT "llu"
#define INT64_FMT "lld"
#endif

#ifdef _WIN32
int32_t snprintf(char* str, size_t size, const char* format, ...);
int strncasecmp(const char *s1, const char *s2, size_t n);
typedef uint32_t pid_t;
typedef uint32_t key_t;
typedef union epoll_data
{
    void *ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

struct epoll_event
{
    uint32_t events;      /* Epoll events */
    epoll_data_t data;      /* User data variable */
};
#endif

//配置文件模块
typedef struct _cfg_t
{
    void* data;     //配置文件内容
    int32_t (*init_cfg)(void* data);
    int32_t (*parse_file)(const char* file, void* data);//解析配置文件
    int32_t (*dump_cfg)(void* data, void* log);//打印信息
    void (*destroy)(void* data);    //数据销毁
} config_t;

//每个处理实际协议的模块
typedef struct _module_t
{
    int32_t (*init_module)(void* data);    //初始化模块信息
    int32_t (*handler)(void* mod, char* src, int32_t len); //处理本次请求函数
    int32_t (*decode)(void* body, char* src, int32_t len); //decode协议
    int32_t (*encode)(void* body, char* out, int32_t* len);  //encode协议
    int32_t db_idx; //需要用哪个数据库链接处理
} module_t;

//集中管理每个协议模块
typedef struct module_col
{
    int32_t (*init_modules)(void* data);
    module_t* (*get_module)(int32_t id);
} module_col_t;

int32_t frame_common_file_exists(const char* file);
int32_t frame_common_create_pid_file(const char* file);
int32_t frame_common_delete_pid_file(const char* file);
int32_t frame_common_decode_int8(char** in, uint8_t* val);
int32_t frame_common_encode_int8(char** out, uint8_t val);
int32_t frame_common_decode_int16(char** in, uint16_t* val);
int32_t frame_common_encode_int16(char** out, uint16_t val);
int32_t frame_common_decode_int32(char** in, uint32_t* val);
int32_t frame_common_encode_int32(char** out, uint32_t val);
int32_t frame_common_decode_string(char** in, char* val, int16_t max);
int32_t frame_common_encode_string(char** out, char* src, int16_t max);
int32_t frame_common_decode_memory(char** in, char* val, int32_t size);
int32_t frame_common_encode_memory(char** out, char* val, int32_t size);
int32_t frame_common_root_path(char* src, int32_t len);
int32_t frame_common_flock(char* str);
