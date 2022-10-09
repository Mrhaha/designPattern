#include "authdb_master.h"
#include "authdb_worker.h"
#include "authdb_config.h"
#include "authdb_modules.h"
#include "authdb_cached.h"
#include "frame_worker.h"
#include "frame_proctitle.h"
#include "frame_mysql.h"
#include "frame_data.h"
int16_t authdb_worker_process_id;
record_t authdb_worker_sql;
void authdb_worker_process_cycle(uint16_t id, void* data, void* record)
{
    if (NULL == data || NULL == record)
    {
        return;
    }
    char prefix[MAX_PREFIX_LEN] = {0};
    frame_process_type = PROCESS_WORKER;
    authdb_worker_process_id = id;
    record_t* log = (record_t*) record;
    authdb_cfg_t* cfg = (authdb_cfg_t*) data;

    snprintf(prefix, sizeof(prefix), "worker-%d", id);
    //初始化日志
    log->init_record(RECORD_LOG, record, prefix);

    log->write_file(log_mask_detail, record,
        "[%s::%s]worker process id(%d),pid(%d) start working\n", __FILE__,
        __FUNCTION__, id, frame_process_id);

    frame_proctitle_set(PS_PREFIX_NAME, "worker process");

    //初始化一下子进程
    frame_worker_init(log);
    //初始化mysql链接
    if (fail == frame_mysql_module_init(log, cfg->db_cfg, cfg->db_count))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]mysql module init fail\n", __FILE__, __FUNCTION__);
        exit(EXIT_FAILURE);
    }
    //初始化共享内存
    if (fail == frame_data_create(log, cfg->shm_key+id, sizeof(cached_t), cfg->cached_count))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]frame data create fail, so exit\n", __FILE__, __FUNCTION__);
        exit(EXIT_FAILURE);
    }
    //初始化各模块
    frame_worker_set_module_col(&authdb_module_col, log);

    for ( ; ; )
    {
        frame_process_check_flag(log);
        frame_worker_recv_message(log, PROCESS_WORKER);
		frame_mysql_check_alive(cfg->db_timeout);
    }
}

void authdb_worker_sql_cycle(uint16_t id, void* data, void* record)
{
    if (NULL == data || NULL == record)
    {
        return;
    }
    char prefix[MAX_PREFIX_LEN] = {0};
    frame_process_type = PROCESS_MYSQL;
    authdb_worker_process_id = id;
    record_t* log = (record_t*) record;
    authdb_cfg_t* cfg = (authdb_cfg_t*) data;

    snprintf(prefix, sizeof(prefix), "sql-%d", id);
    //初始化日志
    log->init_record(RECORD_LOG, record, prefix);

    log->write_file(log_mask_detail, record,
        "[%s::%s]sql worker process id(%d),pid(%d) start working\n", __FILE__,
        __FUNCTION__, id, frame_process_id);

    snprintf(prefix, sizeof(prefix), "sql-%d", id);
    authdb_worker_sql.init_record = frame_log_bill_init;
    authdb_worker_sql.init_record(RECORD_BILL, (void*)&authdb_worker_sql, prefix);
    frame_worker_sql = &authdb_worker_sql;

    frame_proctitle_set(PS_PREFIX_NAME, "sqlexe process");

    //初始化一下子进程
    frame_worker_init(log);
    //初始化mysql链接
    if (fail == frame_mysql_module_init(log, cfg->db_cfg, cfg->db_count))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]mysql module init fail\n", __FILE__, __FUNCTION__);
        exit(EXIT_FAILURE);
    }

    for ( ; ; )
    {
        frame_process_check_flag(log);
        frame_worker_recv_message(log, PROCESS_MYSQL);
		frame_mysql_check_alive(cfg->db_timeout);
    }
}
