// 本协议为生成signature
#include "authdb_auth.h"
#include "authdb_config.h"
#include "authdb_worker.h"
#include "frame_nethead.h"
#include "frame_md5.h"
#include "frame_data.h"
#include "frame_worker.h"

static record_t* s_log = NULL;
static int32_t authdb_make_sign_init(void* data);
static int32_t authdb_make_sign_handler(void* mod, char* src, int32_t len);

module_t authdb_make_sign = {
    authdb_make_sign_init,
    authdb_make_sign_handler,
    authdb_auth_req_decode,
    authdb_auth_res_encode,
    0
};
int32_t authdb_make_sign_init(void* data)
{
    s_log = NULL != data ? data : NULL;
    return success;
}
static int32_t authdb_make_sign_process(req_auth_t* req, res_auth_t* res)
{
    //uint64_t hash_md5;
    int32_t ret;
    authdb_cfg_t* cfg = (authdb_cfg_t*) authdb_config_info.data;
    char str_md5[MD5_MAX_LEN] = {0};
    char post_str[1024] = {0};
    res->result_id = result_id_success;
    if (0 >= req->group_id)
    {
        res->result_id = result_id_invalid_group_id;
        s_log->write_file(log_mask_error, (void*)s_log,
            "[%s::%s]account(%s) send invalid group id(%d)\n", __FILE__,
            __FUNCTION__, req->account, req->group_id);
        return fail;
    }
	snprintf(res->platform, sizeof(res->platform), "%s", req->platform);
    snprintf(res->account, sizeof(res->account), "%s", req->account);
	snprintf(res->sdk, sizeof(res->sdk), "%s", req->sdk);
    res->uin = atoi(req->account);

    int32_t len = snprintf(post_str, sizeof(post_str), "%s%s", req->account,
        cfg->secret_key);

    md5_string((const unsigned char*)post_str, &str_md5[0], len);
    str_md5[MD5_MAX_LEN-1] = '\0';
    if (0 != strncasecmp(str_md5, req->opt, strlen(str_md5)))
    {
        s_log->write_file(log_mask_error, (void*)s_log,
            "[%s::%s]account:%s session key(req:%s,computer:%s) not match\n",
            __FILE__, __FUNCTION__, req->account, req->opt, str_md5);
        res->result_id = result_id_invalid_ses_key;
        return fail;
    }
    res->group_id = req->group_id;
    if (0 > (ret = authdb_auth_build_res(res)))
    {
        res->result_id = result_id_system;
        s_log->write_file(log_mask_error, (void*)s_log,
            "[%s::%s]build response info fail, return val(%d)\n", __FILE__, 
            __FUNCTION__, ret);
        return fail;
    }
    s_log->write_file(log_mask_detail, (void*)s_log, 
        "[%s::%s]dump session key:\n", __FILE__, __FUNCTION__);
    s_log->dump_bin(log_mask_detail, (void*)s_log, res->session_key,
        sizeof(res->session_key));

    return success;
}
int32_t authdb_make_sign_handler(void* mod, char* src, int32_t len)
{
    if (NULL == mod || NULL == src || 0 >= len)
    {
        return fail;
    }
    req_auth_t req;
    memset(&req, 0, sizeof(req));
    res_auth_t res;
    memset(&res, 0, sizeof(res));
    net_head_t nethead;
    module_t* cmd = (module_t*) mod;
    //mysql_t* link = NULL;
    char res_buf[MAX_AUTH_OPTION_LEN] = {0};
    int32_t res_len = 0;

    frame_nethead_decode(src, &nethead);
    if (fail == cmd->decode((void*)&req, (src + POSI_MSG_BODY),
        (len - NET_HEAD_LEN)))
    {
        s_log->write_file(log_mask_error, (void*)s_log,
            "[%s::%s]decode message body fail,msgid=0x%04x,seq=%d,uin=%d\n",
            __FILE__, __FUNCTION__, nethead.msg_id, nethead.sequence,
            nethead.uin);
        return fail;
    }
    s_log->write_file(log_mask_detail, (void*)s_log, "[%s::%s]req msg "
        "info: nethead(msgid:0x%04x,seq:%d,uin:%d,fd:%d,pkg_len:%d,"
        "trans_type=%d,len=%d,net_len=%d) body(account:%s,opt_len:%d,opt:%s)\n"
        , __FILE__, __FUNCTION__, nethead.msg_id, nethead.sequence, nethead.uin,
        nethead.sock_fd, nethead.pkg_len, nethead.trans_type, len,
        NET_HEAD_LEN,req.account, req.opt_len, req.opt);

    if (fail == authdb_make_sign_process(&req, &res))
    {
        s_log->write_file(log_mask_error, (void*)s_log, "[%s::%s]process "
            "auth req info return fail\n", __FILE__, __FUNCTION__);
    }
    s_log->write_file(log_mask_detail, (void*)s_log, "[%s::%s]res info:"
        "result_id:%d,uin:%d,account:%s,sign_len:%d\n", __FILE__, __FUNCTION__,
        res.result_id, res.uin, res.account, res.sign_len);

    //编码回复信息
    cmd->encode((void*)&res, &res_buf[NET_HEAD_LEN], &res_len);
    //编码nethead信息
    nethead.pkg_len = NET_HEAD_LEN + res_len;
    nethead.trans_type = TRANS_TO_CLIENT;
    frame_nethead_encode(&res_buf[0], &nethead);
    //发送给主线程
    frame_worker_send_message(s_log, res_buf, res_len + NET_HEAD_LEN);

    return success;

}
