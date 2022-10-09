#include "frame_log.h"
static int32_t frame_log_check_time(record_t* log);
static int32_t frame_log_create_file(record_t* log);
static char* frame_log_get_log_title(int32_t level);
static int32_t frame_log_rename_file(record_t* log);

static log_title g_title[] = 
{
    {log_mask_none, ""},
    {log_mask_error, "error"},
    {log_mask_warning, "warning"},
    {log_mask_config, "config"},
    {log_mask_performance, "performance"},
    {log_mask_detail, "detail"},
};
static char* frame_log_get_log_title(int32_t level)
{
    if (log_mask_none >= level)
    {
        return NULL;
    }
    uint16_t i;
    for (i = 0; i < (sizeof(g_title)/sizeof(g_title[0])); ++i)
    {
        if (level == g_title[i].level)
        {
            return g_title[i].title;
        }
    }
    return NULL;
}

static int32_t frame_log_check_time( record_t* log )
{
    if (NULL == log)
    {
        return fail;
    }
    time_t now = time(NULL);
    //当前时间大于保存的时间，则需要重新创建新文件
    if (now > log->end_time)
    {
        struct tm now_tm;
        if (NULL != localtime_r(&now, &now_tm))
        {
            uint16_t hour = 0;
            if (RECORD_LOG == log->flag) //日志是记录每天的
            {
                //将时间改成当日23:59:59
                hour = 23;
            }
            else if (RECORD_BILL == log->flag)
            {
                hour = now_tm.tm_hour; //账单是记录每小时的
            }
            now_tm.tm_min  = 59;
            now_tm.tm_sec  = 59;
            log->end_time = mktime(&now_tm);
            //顺便更改下文件路径
            char date_str[MAX_PATH_LEN] = {0};
            frame_time_get_date_str(log->end_time, date_str, sizeof(date_str),
                log->flag);
            snprintf(log->file_name, MAX_FILE_NAME_LEN,
                "%s/%s_%s.%s", log->path, log->prefix, date_str, log->postfix);
        }
        return fail;
    }

    return success;
}

static int32_t frame_log_create_file( record_t* log )
{
    if (NULL == log)
    {
        return fail;
    }
    if (NULL != log->fp)
    {
        fclose(log->fp);
        log->fp = NULL;
    }
    log->fp = fopen(log->file_name, "a+");
    if (NULL == log->fp)
    {
        //只能丢失掉日志
        log->fp = NULL;
        return fail;
    }
    return success;
}

static int32_t frame_log_rename_file(record_t* log)
{
    if (NULL == log)
    {
        return -1;
    }
    struct stat st;
    if (0 > stat(log->file_name, &st))
    {
        return -2;
    }
    //这个说明没有必要处理
    if (MAX_LOG_FILE_SIZE > st.st_size)
    {
        return 1;
    }
    char file_name[MAX_FILE_NAME_LEN] = {0};
    char rename_file_name[MAX_FILE_NAME_LEN] = {0};
    snprintf(file_name, sizeof(file_name), "%s", log->file_name);
    snprintf(rename_file_name, sizeof(rename_file_name), "%s",
        log->file_name);
    int32_t offset = strlen(log->file_name)-(strlen(log->postfix)+1);
    get_log_name_ex(&file_name[offset], sizeof(file_name)-offset,
        MAX_LOG_FILE_COUNT-1, log->postfix);
    //删除掉序列最大的那个文件
    if (0 == access(file_name, F_OK))
    {
        if (0 > remove(file_name))
        {
            return -3;
        }
    }
    //开始循环重命名文件
    int16_t i;
    for (i=(MAX_LOG_FILE_COUNT-2); i >= 0; --i)
    {
        if (0 != i)
        {
            get_log_name_ex(&file_name[offset],
                sizeof(file_name)-offset, i, log->postfix);
        }
        else
        {
            snprintf(file_name, sizeof(file_name), "%s",
                log->file_name);
        }
        if (0 == access(file_name, F_OK))
        {
            get_log_name_ex(&rename_file_name[offset],
                sizeof(rename_file_name)-offset, i+1, log->postfix);
            if (0 > rename(file_name, rename_file_name))
            {
                return -4;
            }
        }
    }

    return success;
}
//static分隔
//////////////////////////////////////////////////////////////////////

int32_t frame_log_error( int32_t level, void* data, const char* fmt, ... )
{
    if (NULL == data)
    {
        return -1;
    }
    record_t* log = (record_t*) data;
    //不需要记录该账单
    if (0 == (log->mask & level))
    {
        return 0;
    }
    log->check_file(data);
    if (NULL == log->fp)
    {
        return -2;
    }
    char date_time[MAX_FILE_NAME_LEN] = {0};
    frame_time_get_datetime(0, date_time, sizeof(date_time));

    int32_t bytes = 0;
    if (RECORD_LOG == log->flag)
    {
        char* title = frame_log_get_log_title(level);
        bytes = fprintf(log->fp, "[[%s]][%s]", date_time, title);
    }
    else if (RECORD_BILL == log->flag)
    {
        bytes = fprintf(log->fp, "%s,", date_time);
    }

    va_list ap;
    va_start(ap, fmt);
    bytes += vfprintf(log->fp, fmt, ap);
    va_end(ap);
    fflush(log->fp);

    if (RECORD_LOG == log->flag)
    {
        //还需检查日志大小是否已经超过最大值
        frame_log_rename_file(log);
    }
    return bytes;
}

int32_t frame_log_check_file( void* data )
{
    if (NULL == data)
    {
        return fail;
    }
    record_t* log = (record_t*) data;
    //检查两项
    //1.该文件是否已经存在
    //2.时间是否对头
    int32_t fret = frame_common_file_exists(log->file_name);
    int32_t tret = frame_log_check_time(log);
    //需要重新创建文件了
    if (fail == fret || fail == tret)
    {
        frame_log_create_file(log);
    }
    return success;
}

int32_t frame_log_dump_bin( int32_t level, void* data, const char* src, int32_t size )
{
    if (NULL == data || NULL == src || 0 >= size)
    {
        return -1;
    }
    record_t* log = (record_t*) data;
    int32_t bytes = 0;
    if (0 == (level & log->mask))
    {
        return 0;
    }
    if (size > MAX_DUMP_INFO_SIZE)
    {
        size = MAX_DUMP_INFO_SIZE;
    }
    char datetime[MAX_FILE_NAME_LEN] = {0};
    frame_time_get_datetime(0, datetime, sizeof(datetime));
    bytes += fprintf(log->fp, "[[%s]][buffer size:%4d]", datetime,
        size);

    int16_t i;
    for (i=0; i < size; ++i)
    {
        if (0 == (i % MAX_DUMP_LINE_SIZE))
        {
            bytes += fprintf(log->fp, "\n[%s] %04d>\t", datetime,
                (i / MAX_DUMP_LINE_SIZE + 1));
        }
        bytes += fprintf(log->fp, "%02X ", (unsigned char)src[i]);
    }
    bytes += fprintf(log->fp, "\n");
    fflush(log->fp);

    return bytes;
}

int32_t frame_log_bill_init( int32_t flag, void* data, char* prefix )
{
    if (NULL == data || NULL == prefix)
    {
        return fail;
    }
    struct tm now_tm;
    time_t now_time = time(NULL);
    record_t* bill = (record_t*) data;
    bill->check_file = frame_log_check_file;
    bill->write_file = frame_log_error;
    bill->dump_bin = NULL;
    bill->mask = -1;
    bill->flag = flag;

    snprintf(bill->prefix, sizeof(bill->prefix), "%s", prefix);
    snprintf(bill->path, sizeof(bill->path), BILL_DEFAULT_PATH);
    snprintf(bill->postfix, sizeof(bill->postfix), BILL_DEFAULT_FILE_POSTFIX);
    if (NULL != localtime_r(&now_time, &now_tm))
    {
        //将时间改成当日xx:59:59
        now_tm.tm_min  = 59;
        now_tm.tm_sec  = 59;
        bill->end_time = mktime(&now_tm);
        //顺便更改下文件路径
        snprintf(bill->file_name, MAX_FILE_NAME_LEN,
            "%s/%s_%04d-%02d-%02d-%02d.%s", bill->path, bill->prefix,
            now_tm.tm_year+1900, now_tm.tm_mon+1,
            now_tm.tm_mday, now_tm.tm_hour, bill->postfix);
    }
    if (NULL != bill->fp)
    {
        fclose(bill->fp);
        bill->fp = NULL;
    }
    bill->fp = fopen(bill->file_name, "a+");

    return success;
}
