#include "authdb_master.h"
#include "authdb_worker.h"
#include "frame_proctitle.h"

char** authdb_master_argv;
static void authdb_master_start_worker(record_t* log, authdb_cfg_t* cfg);
static void authdb_master_init(record_t* log, authdb_cfg_t* cfg);

static void authdb_master_start_worker(record_t* log, authdb_cfg_t* cfg)
{
    if (NULL == log || NULL == cfg)
    {
        return ;
    }
    uint16_t i;
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]start worker process\n", __FILE__, __FUNCTION__);
    //开启sql执行进程
    for (i =0; i < cfg->sql_worker_count; ++i)
    {
        frame_master_spawn_process(log, authdb_worker_sql_cycle,
            i, (void*)cfg, (void*)log, PROCESS_MYSQL);
    }
    //开启所有工作进程
    for (i = 0; i < cfg->worker_count; ++i)
    {
        frame_master_spawn_process(log, authdb_worker_process_cycle,
            i, (void*)cfg, (void*)log, PROCESS_WORKER);
    }
}
static void authdb_master_init(record_t* log, authdb_cfg_t* cfg)
{
    if (NULL == log || NULL == cfg)
    {
        return ;
    }
    //初始化server部分
    if (fail == frame_socket_svrd_init(log, cfg->local_ip,
        cfg->port, cfg->max_fd))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]create svrd error,so exit\n", __FILE__,
            __FUNCTION__);
        exit(0);
    }
    if (fail == frame_master_init(log))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]master process init fail,so exit\n", __FILE__,
            __FUNCTION__);
        exit(0);
    }
    //设置主进程名称
    char title[MAX_FILE_NAME_LEN] = {0};
    int32_t offset = snprintf(title, sizeof(title), "master process ");
    int32_t i;
    for (i=0; authdb_master_argv[i]; ++i)
    {
        offset += snprintf(title+offset, sizeof(title)-offset, "%s ",
            authdb_master_argv[i]);
    }
    frame_proctitle_set(PS_PREFIX_NAME, title);
}
//static function end
//////////////////////////////////////////////////////////////////////////

void authdb_master_process_cycle(record_t* log, authdb_cfg_t* cfg)
{
    if (NULL == log || NULL == cfg)
    {
        return;
    }
    frame_process_type = PROCESS_MASTER;
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]start master cycle\n", __FILE__, __FUNCTION__);
    //开始产生工作进程
    authdb_master_start_worker(log, cfg);

    //初始化主进程
    authdb_master_init(log, cfg);
    frame_common_create_pid_file("");

    for ( ; ; )
    {
        //检查是否收到信号
        frame_process_check_flag(log);
        //处理信息
        frame_master_process(log);
        //检查超时
        frame_socket_check_timeout(log, cfg->check_interval,
            cfg->socket_alive, cfg->wait_first_pkg);
    }
}
