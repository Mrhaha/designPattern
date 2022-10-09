#include "authdb_worker.h"
#include "authdb_cached.h"
#include "frame_log.h"
#include "frame_nethead.h"
#include "frame_data.h"

typedef struct _db_test
{
    int32_t uin;
} authdb_test_t;

static record_t* authdb_test_log = NULL;

static int32_t authdb_test_init(void* data);
static int32_t authdb_test_decode(void* body, char* src, int32_t len);
static int32_t authdb_test_handler(void* mod, char* src, int32_t len);
module_t authdb_test = {
    authdb_test_init,
    authdb_test_handler,
    authdb_test_decode,
    NULL,
    -1
};

static int32_t authdb_test_init(void* data)
{
    if (NULL == data)
    {
        return fail;
    }
    authdb_test_log = (record_t*)data;
    return success;
}
static int32_t authdb_test_decode(void* body, char* src, int32_t len)
{
    if (NULL == body || NULL == src || 0 >= len)
    {
        return fail;
    }
    int32_t encode_len;
    int32_t remained;
    authdb_test_t* sbt = (authdb_test_t*) body;
    //È¡bodyÎ»ÖÃ
    char* tmp = src + POSI_MSG_BODY;
    remained = len - NET_HEAD_LEN;
    encode_len = frame_common_decode_int32(&tmp, (uint32_t*)&sbt->uin);
    remained -= encode_len;

    if (0 > remained)
    {
        return fail;
    }
    return success;
}
static int32_t authdb_test_handler(void* mod, char* src, int32_t len)
{
    if (NULL == mod || NULL == src || 0 >= len)
    {
        return fail;
    }
    cached_t* obj = NULL;
    authdb_test_t sbt;
    net_head_t nethead;
    module_t* st = (module_t*) mod;

    frame_nethead_decode(src, &nethead);
    if (fail == st->decode((void*)&sbt, src, len))
    {
        authdb_test_log->write_file(log_mask_error,
            (void*)authdb_test_log, "[%s::%s]decode message body fail\n"
            "msgid=0X%X,seq=%d,uin=%d\n", __FILE__, __FUNCTION__,
            nethead.msg_id, nethead.sequence, nethead.uin);
        return fail;
    }
    authdb_test_log->write_file(log_mask_detail, (void*)authdb_test_log,
        "[%s::%s]req message info:nethead(id:%d,seq:%d,uin:%d,fd:%d,"
        "pkg_len:%d),body(uin:%d)\n", __FILE__, __FUNCTION__,
        nethead.msg_id, nethead.sequence, nethead.uin, nethead.sock_fd,
        nethead.pkg_len, sbt.uin);
    
    obj = (cached_t*) frame_data_get_by(sbt.uin, IS_OK);
    if (NULL == obj)
    {
        authdb_test_log->write_file(log_mask_error, (void*)authdb_test_log,
            "[%s::%s]get data(uin:%d) fail\n", __FILE__, __FUNCTION__,
            sbt.uin);
        return fail;
    }
    authdb_test_log->write_file(log_mask_detail, (void*)authdb_test_log,
        "[%s::%s]get data(uin:%d) success\n", __FILE__, __FUNCTION__,
        sbt.uin);
    //obj->uin = sbt.uin;

    return success;
}
