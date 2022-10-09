#pragma once

#include "frame_common.h"
#include "frame_time.h"
#define LOG_DEFAULT_PATH            "./log"
#define LOG_DEFAULT_FILE_POSTFIX    "log"
#define BILL_DEFAULT_PATH           "./bill"
#define BILL_DEFAULT_FILE_POSTFIX   "txt"

//1G
#define MAX_LOG_FILE_SIZE           0x40000000
//#define MAX_LOG_FILE_SIZE           0x400
#define MAX_LOG_FILE_COUNT          40

#define MAX_PATH_LEN                48     //路径最大值
#define MAX_PREFIX_LEN              32     //前缀最大值
#define MAX_POSTFIX_LEN             16     //后缀最大值
#define MAX_FILE_NAME_LEN           128    //文件名最大值
#define MAX_DUMP_INFO_SIZE          0x400
#define MAX_DUMP_LINE_SIZE          0x10

#define log_mask_none        0x0000
#define log_mask_error       0x0001
#define log_mask_warning     0x0002
#define log_mask_config      0x0004
#define log_mask_performance 0x0008     //系统性能
#define log_mask_detail      0x0010
#define get_log_name_ex(src, max, no, postfix)\
    snprintf(src, max, "%d.%s", no, postfix)

typedef struct _title
{
    int32_t level;
    char*   title;
} log_title;
//日志和账单处理
typedef struct _record_s
{
    int32_t (*init_record)( int32_t flag, void* data, char* prefix);   //初始化函数
    int32_t (*write_file)(int32_t level, void* data, const char* fmt, ...); //记录日志函数
    int32_t (*dump_bin)(int32_t level, void* data, const char* src, int32_t size); //记录网络包
    int32_t (*check_file)(void* data);  //判断是否需要新建文件
    FILE*   fp;
    time_t  end_time;   //当前时间小于该值，则不需要重新打开新文件
    int32_t mask;   //记录等级
    int32_t flag;   //记录类别
    char    prefix[MAX_PREFIX_LEN]; //前缀
    char    postfix[MAX_POSTFIX_LEN];//后缀
    char    path[MAX_PATH_LEN];//文件目录
    char    file_name[MAX_FILE_NAME_LEN];//文件名称
}record_t;

int32_t frame_log_error(int32_t level, void* data, const char* fmt, ...);
int32_t frame_log_check_file(void* data);
int32_t frame_log_dump_bin(int32_t level, void* data, const char* src, int32_t size);
int32_t frame_log_bill_init(int32_t flag, void* data, char* prefix);
