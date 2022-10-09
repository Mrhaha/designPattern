#include "frame_common.h"
#include "frame_log.h"
#include "frame_socket.h"
#include "frame_process.h"
#include "frame_proctitle.h"
#include "frame_master.h"
#include "authdb_config.h"
#include "authdb_master.h"

#define AUTHDB_VERSION "AuthDB Version 2.0"
static int32_t main_log_init(int32_t flag, void* data, char* prefix);
static int32_t main_check_dir();
static void main_init(char** argv);

record_t main_log_info = {
    main_log_init,
    frame_log_error,
    frame_log_dump_bin,
    frame_log_check_file,
    NULL,-1,0,-1,"","","",""
};

static int32_t main_log_init(int32_t flag, void* data, char* prefix)
{
    if (NULL == data || NULL == prefix)
    {
        return fail;
    }
    time_t now = time(NULL);
    record_t* log = (record_t*) data;
    //log->check_file = frame_log_check_file;
    //log->write_file = frame_log_error;
    //log->dump_bin = frame_log_dump_bin;
    log->flag = flag;
    snprintf(log->prefix, sizeof(log->prefix), "%s", prefix);
    snprintf(log->path, sizeof(log->path), LOG_DEFAULT_PATH);
    snprintf(log->postfix, sizeof(log->postfix), LOG_DEFAULT_FILE_POSTFIX);
    struct tm now_tm;
    if (NULL != localtime_r(&now, &now_tm))
    {
        //将时间改成当日23:59:59
        now_tm.tm_hour = 23;
        now_tm.tm_min  = 59;
        now_tm.tm_sec  = 59;
        log->end_time = mktime(&now_tm);
        //顺便更改下文件路径
        snprintf(log->file_name, MAX_FILE_NAME_LEN,
            "%s/%s_%04d-%02d-%02d.%s", log->path, log->prefix,
            now_tm.tm_year+1900, now_tm.tm_mon+1,
            now_tm.tm_mday, log->postfix);
    }

    if (NULL != log->fp)
    {
        fclose(log->fp);
        log->fp = NULL;
    }
    log->fp = fopen(log->file_name, "a+");

    return success;
}
static int32_t main_check_dir()
{
    int32_t bit = 0;
    if (fail == frame_common_file_exists("./bill"))
    {
        fprintf(stderr, "./bill is not exists\n");
        bit |= 0x1;
    }
    if (fail == frame_common_file_exists("./log"))
    {
        fprintf(stderr, "./log is not exists\n");
        bit |= 0x2;
    }
    if (fail == frame_common_file_exists("./config"))
    {
        fprintf(stderr, "./config is not exists\n");
        bit |= 0x4;
    }
    if (0 < bit)
    {
        return fail;
    }
    return success;
}
static void main_init(char** argv)
{
    //只能启动单一进程
    if (fail == frame_common_flock(argv[0]))
    {
        exit(1);
    }
    //初始化配置文件
    authdb_config_info.init_cfg((void*)&authdb_config_info);
    authdb_config_info.parse_file(CONFIG_FILE, authdb_config_info.data);

    //authdb_cfg_t* cfg = (authdb_cfg_t*) authdb_config_info.data;
    //初始化日志
    main_log_info.init_record(RECORD_LOG, (void*)&main_log_info, "master");

    authdb_config_info.dump_cfg(authdb_config_info.data,
        (void*)&main_log_info);

    if (fail == frame_process_signal_init(&main_log_info))
    {
        fprintf(stderr, "[%s::%s]init signal fail\n", __FILE__,
            __FUNCTION__);
        exit(0);
    }
    
    authdb_master_argv = argv;
    frame_proctitle_init(&main_log_info, argv);
}
//static function end
///////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    int16_t is_daemon = 0;
    int16_t i;
    for (i = 1; i < argc; ++i)
    {
        if (0 == strncasecmp(argv[i], "-v", sizeof("-v")))
        {
#if defined(_DEBUG) || defined(DEBUG)
            printf("%s Debug Build In %s %s\n", AUTHDB_VERSION, 
                __DATE__, __TIME__);
#else
            printf("%s Release Build In %s %s\n", AUTHDB_VERSION, 
                __DATE__, __TIME__);
#endif
            exit(0);
        }
        if (0 == strncasecmp(argv[i], "-d", sizeof("-d")))
        {
            is_daemon = 1;
        }
    }
    if (fail == main_check_dir())
    {
        exit(0);
    }
    main_init(argv);
    //开始转入到daemon进程
    if (0 != is_daemon)
    {
        if (fail == frame_process_daemon())
        {
            fprintf(stderr, "[%s::%s]process daemon fail\n", __FILE__,
                __FUNCTION__);
            exit(0);
        }
    }
    //开启server
    authdb_master_process_cycle(&main_log_info, 
        (authdb_cfg_t*)authdb_config_info.data);

    return 0;
}
