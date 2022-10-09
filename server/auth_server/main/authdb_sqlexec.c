#include "authdb_worker.h"
#include "frame_common.h"
#include "frame_log.h"
#include "frame_nethead.h"
#include "frame_mysql.h"
typedef struct _sql_body
{
    int32_t idx;
    char sql[MAX_CLIENT_PKG_LEN];
} sql_body_t;

static record_t* authdb_sqlexec_log = NULL;
static record_t authdb_sqlexec_bill;

static int32_t authdb_sqlexec_init(void* data);
static int32_t authdb_sqlexec_handler(void* mod, char* src, int32_t len);
static int32_t authdb_sqlexec_decode(void* body, char* src, int32_t len);

module_t authdb_sqlexec = {
    authdb_sqlexec_init,
    authdb_sqlexec_handler,
    authdb_sqlexec_decode,
    NULL,
    -1
};

static int32_t authdb_sqlexec_init(void* data)
{
    if (NULL == data)
    {
        return fail;
    }
    char prefix[MAX_PREFIX_LEN] = {0};
    //初始化日志
    authdb_sqlexec_log = (record_t*) data;
    //初始化账单
    snprintf(prefix, sizeof(prefix), "sqlexec-%d", authdb_worker_process_id);
    authdb_sqlexec_bill.init_record = frame_log_bill_init;
    authdb_sqlexec_bill.init_record(RECORD_BILL, (void*)&authdb_sqlexec_bill, prefix);

    return success;
}
static int32_t authdb_sqlexec_handler(void* mod, char* src, int32_t len)
{
    if (NULL == mod || NULL == src || 0 >= len)
    {
        return fail;
    }
    sql_body_t sbt;
    net_head_t nethead;
    module_t* st = (module_t*) mod;
    mysql_t* mysql = NULL;
    int32_t err = -1;
    frame_nethead_decode(src, &nethead);
    if (fail == st->decode((void*)&sbt, src, len))
    {
        authdb_sqlexec_log->write_file(log_mask_error,
            (void*)authdb_sqlexec_log, "[%s::%s]decode message body fail\n"
            "msgid=%d,seq=%d,uin=%d\n", __FILE__, __FUNCTION__,
            nethead.msg_id, nethead.sequence, nethead.uin);
        return fail;
    }
    authdb_sqlexec_log->write_file(log_mask_detail, (void*)authdb_sqlexec_log,
        "[%s::%s]req message info:nethead(id:%d,seq:%d,uin:%d,fd:%d,"
        "pkg_len:%d),body(idx:%d,sql:[%s])\n", __FILE__, __FUNCTION__,
        nethead.msg_id, nethead.sequence, nethead.uin, nethead.sock_fd,
        nethead.pkg_len, sbt.idx, sbt.sql);

    mysql = frame_mysql_get(sbt.idx);
    if (NULL == mysql)
    {
        authdb_sqlexec_log->write_file(log_mask_error,(void*)authdb_sqlexec_log,
            "[%s::%s]nethead(id:%d,seq:%d,uin:%d,fd:%d,pkg_len:%d),body(idx:"
            "%d,sql:[%s]),get mysql link fail\n", __FILE__, __FUNCTION__,
            nethead.msg_id, nethead.sequence, nethead.uin, nethead.sock_fd,
            nethead.pkg_len, sbt.idx, sbt.sql);
        goto err_step;
    }
    mysql->sql_len = snprintf(mysql->sql, sizeof(mysql->sql), "%s", sbt.sql);
    if (0 > mysql->sql_len)
    {
        authdb_sqlexec_log->write_file(log_mask_error, (void*)authdb_sqlexec_log,
            "[%s::%s]snprintf sql(%s) error,return val(%d)\n", __FILE__,
            __FUNCTION__, sbt.sql, mysql->sql_len);
        goto err_step;
    }
    mysql->is_query = IS_NOT_OK;
    if (fail == mysql->sql_query((void*)mysql, &err))
    {
        authdb_sqlexec_log->write_file(log_mask_error, (void*)authdb_sqlexec_log,
            "[%s::%s]exec sql[%s] return fail,errno(%d)\n", __FILE__,
            __FUNCTION__, mysql->sql, err);
        goto err_step;
    }
    //记录账单
    authdb_sqlexec_bill.write_file(log_mask_error, (void*)&authdb_sqlexec_bill,
        "%4d,%s\n", err, sbt.sql);

    return success;

err_step:
    //只记录账单
    authdb_sqlexec_bill.write_file(log_mask_error, (void*)&authdb_sqlexec_bill,
        "%4d,%s\n", err, sbt.sql);
    return fail;
}
static int32_t authdb_sqlexec_decode(void* body, char* src, int32_t len)
{
    if (NULL == body || NULL == src || 0 >= len)
    {
        return fail;
    }
    int32_t encode_len;
    int32_t remained;
    sql_body_t* sbt = (sql_body_t*) body;
    //取body位置
    char* tmp = src + POSI_MSG_BODY;
    remained = len - NET_HEAD_LEN;
    encode_len = frame_common_decode_int32(&tmp, (uint32_t*)&sbt->idx);
    remained -= encode_len;
    
    encode_len = frame_common_decode_string(&tmp, sbt->sql, sizeof(sbt->sql));
    remained -= encode_len;
    if (0 > remained)
    {
        return fail;
    }
    return success;
}

