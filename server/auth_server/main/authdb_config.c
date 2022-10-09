#include "authdb_config.h"
#include "frame_iniparser.h"
#include "frame_log.h"
static authdb_cfg_t authdb_config_cfg;
config_t authdb_config_info = {
    (void*)&authdb_config_cfg,
    authdb_config_init,
    authdb_config_parse,
    authdb_config_dump,
    authdb_config_destroy
};

static int32_t authdb_config_load(const char* file, authdb_cfg_t* cfg);
static int32_t authdb_config_load_common(authdb_cfg_t* cfg, dictionary* ini);
static int32_t authdb_config_load_mysql(authdb_cfg_t* cfg, dictionary* ini);

static int32_t authdb_config_load(const char* file, authdb_cfg_t* cfg)
{
    if (NULL == file || NULL == cfg)
    {
        return fail;
    }
    dictionary* ini = NULL;
    if (NULL == (ini = iniparser_load((char*)file)))
    {
        fprintf(stderr, "[%s::%s]%s load fail\n", __FILE__, __FUNCTION__,
            file);
        return fail;
    }
    if (fail == authdb_config_load_common(cfg, ini))
    {
        fprintf(stderr, "[%s::%s]%s load common fail\n", __FILE__,
            __FUNCTION__, file);
        return fail;
    }

    if (fail == authdb_config_load_mysql(cfg, ini))
    {
        fprintf(stderr, "[%s::%s]%s load mysql fail\n", __FILE__, 
            __FUNCTION__, file);
        return fail;
    }

    return success;
}
static int32_t authdb_config_load_common(authdb_cfg_t* cfg, dictionary* ini)
{
    char* p = NULL;
    cfg->worker_count = (int16_t) iniparser_getint(ini, 
        "common:worker_count", DEFAULT_PROCESS_COUNT);
    if (MAX_PROCESS_COUNT < cfg->worker_count)
    {
        cfg->worker_count = MAX_PROCESS_COUNT;
    }
    cfg->sql_worker_count = (int16_t) iniparser_getint(ini,
        "common:sql_worker_count", DEFAULT_SQL_PROC_COUNT);
    if (MAX_SQL_WORKER_COUNT < cfg->sql_worker_count)
    {
        cfg->sql_worker_count = MAX_SQL_WORKER_COUNT;
    }
    cfg->log_mask = (int32_t) iniparser_getint(ini,
        "common:log_mask", DEFAULT_LOG_MASK);
    cfg->port = (uint16_t) iniparser_getint(ini,
        "common:local_port", DEFAULT_LISTEN_PORT);
    cfg->max_fd = (int16_t) iniparser_getint(ini,
        "common:max_fd", DEFAULT_MAX_FD);
    cfg->check_interval = (int16_t) iniparser_getint(ini,
        "common:check_socket_interval", DEFAULT_SOCK_INTERVAL);
	cfg->db_timeout = (uint32_t)iniparser_getint(ini, 
		"common:check_db_timeout", DEFAULT_DB_TIMEOUT);
    cfg->socket_alive = (int16_t) iniparser_getint(ini,
        "common:socket_unalive_time", DEFAULT_SOCK_UNALIVE);
    cfg->wait_first_pkg = (int16_t) iniparser_getint(ini,
        "common:wait_first_package", DEFAULT_WAIT_FIRST_PKG);
    cfg->cached_count = (int32_t) iniparser_getint(ini,
        "common:cached_count", DEFAULT_CACHED_COUNT);
    cfg->shm_key = (key_t) iniparser_getint(ini,
        "common:shm_key", DEFAULT_SHM_KEY);

    p = iniparser_getstring(ini, "common:local_ip", "");
    if (NULL != p)
    {
        snprintf(cfg->local_ip, sizeof(cfg->local_ip), "%s", p);
    }

    p = iniparser_getstring(ini, "auth:secret_key", "");
    if (NULL != p)
    {
        snprintf(cfg->secret_key, sizeof(cfg->secret_key), "%s", p);
    }
    return success;
}
static int32_t authdb_config_load_mysql(authdb_cfg_t* cfg, dictionary* ini)
{
    if (NULL == cfg || NULL == ini)
    {
        return fail;
    }
    char* p = NULL;
    int16_t i;
    char item[MAX_ACCOUNT_LEN] = {0};

    cfg->db_count = iniparser_getint(ini,
        "mysql:db_count", 0);
    if (MAX_DB_COUNT < cfg->db_count)
    {
        cfg->db_count = MAX_DB_COUNT;
    }
    size_t size = cfg->db_count * sizeof(mysql_cfg_t);
    if (NULL == (cfg->db_cfg = (mysql_cfg_t*)malloc(size)))
    {
        fprintf(stderr, "[%s::%s]malloc mysql_cfg_t size(%ld) fail\n",
            __FILE__, __FUNCTION__, size);
        return fail;
    }
    mysql_cfg_t* db = cfg->db_cfg;
    for (i=0; i < cfg->db_count; ++i, ++db)
    {
        snprintf(item, sizeof(item), "mysql:db.%d.ip", i);
        p = iniparser_getstring(ini, item, NULL);
        if (NULL != p)
        {
            snprintf(db->db_ip, sizeof(db->db_ip), "%s", p);
        }
        snprintf(item, sizeof(item), "mysql:db.%d.port", i);
        db->port = (uint16_t)iniparser_getint(ini, 
            item, DEFAULT_MYSQL_PORT);

        snprintf(item, sizeof(item), "mysql:db.%d.user", i);
        p = iniparser_getstring(ini, item, NULL);
        if (NULL != p)
        {
            snprintf(db->usr_name, sizeof(db->usr_name), "%s", p);
        }
        snprintf(item, sizeof(item), "mysql:db.%d.passwd", i);
        p = iniparser_getstring(ini, item, NULL);
        if (NULL != p)
        {
            snprintf(db->usr_pwd, sizeof(db->usr_pwd), "%s", p);
        }
        snprintf(item, sizeof(item), "mysql:db.%d.database", i);
        p = iniparser_getstring(ini, item, NULL);
        if (NULL != p)
        {
            snprintf(db->db_name, sizeof(db->db_name), "%s", p);
        }
        db->idx = i;    
    }

    return success;
}
//static function end
//////////////////////////////////////////////////////////////////////////
int32_t authdb_config_init(void* data)
{
    if (NULL == data)
    {
        return fail;
    }
    config_t* cfg = (config_t*) data;
    cfg->data = (void*) &authdb_config_cfg;
    cfg->parse_file = authdb_config_parse;
    cfg->dump_cfg = authdb_config_dump;
    cfg->destroy = authdb_config_destroy;

    return success;
}
int32_t authdb_config_parse( const char* file, void* data )
{
    if (NULL == file || NULL == data)
    {
        return fail;
    }
    if (fail == frame_common_file_exists(file))
    {
        fprintf(stderr, "[%s::%s]%s is not exists\n", __FILE__, 
            __FUNCTION__, file);
        return fail;
    }
    authdb_cfg_t* cfg = (authdb_cfg_t*) data;
    if (fail == authdb_config_load(file, cfg))
    {
        fprintf(stderr, "[%s::%s]load file %s return fail\n", __FILE__,
            __FUNCTION__, file);
        return fail;
    }
    return success;
}
int32_t authdb_config_dump(void* data, void* slog)
{
    if (NULL == data || NULL == slog)
    {
        return fail;
    }
    authdb_cfg_t* cfg = (authdb_cfg_t*) data;
    record_t* log = (record_t*) slog;
    int16_t i;
    log->write_file(log_mask_config, slog,
        "[%s::%s]worker_count=%d,sql_worker_count=%d,max_fd=%d,log_mask=%d\n",
        __FILE__, __FUNCTION__, cfg->worker_count, cfg->sql_worker_count,
        cfg->max_fd, cfg->log_mask);
    log->write_file(log_mask_config, slog,
        "[%s::%s]local_ip=%s,secret_key=%s,local_port=%d,db_count=%d\n",
        __FILE__, __FUNCTION__, cfg->local_ip, cfg->secret_key, cfg->port,
        cfg->db_count);
    for (i = 0; i < cfg->db_count; ++i)
    {
        mysql_cfg_t* p = cfg->db_cfg + i;
        log->write_file(log_mask_config, slog, 
            "[%s::%s][%d:%d]ip=%s,port=%d,user=%s,pwd=%s,database=%s\n",
            __FILE__, __FUNCTION__, i+1, cfg->db_count, p->db_ip,
            p->port, p->usr_name, p->usr_pwd, p->db_name);
    }

    return success;
}
void authdb_config_destroy(void* data)
{
    if (NULL == data)
    {
        return;
    }
    authdb_cfg_t* cfg = (authdb_cfg_t*) data;
    if (NULL != cfg->db_cfg)
    {
        free(cfg->db_cfg);
        cfg->db_cfg = NULL;
    }
}
