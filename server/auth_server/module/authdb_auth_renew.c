//请求更换signature
#include "authdb_auth.h"
#include "authdb_worker.h"
#include "frame_nethead.h"
#include "frame_md5.h"
#include "frame_data.h"
#include "frame_worker.h"

static record_t* auth_log = NULL;

static int32_t authdb_auth_renew_init(void* data);
static int32_t authdb_auth_renew_handler(void* mod, char* src, int32_t len);
static int32_t authdb_auth_renew_process(req_auth_t* req, res_auth_t* res, mysql_t* link);
static int32_t authdb_auth_renew_check(req_auth_t* req);

module_t authdb_auth_renew = {
    authdb_auth_renew_init,
    authdb_auth_renew_handler,
    authdb_auth_req_decode,
    authdb_auth_res_encode,
    0
};

static int32_t authdb_auth_renew_init(void* data)
{
    if (NULL == data)
    {
        return fail;
    }
    auth_log = (record_t*) data;
    return success;
}

static int32_t authdb_auth_renew_handler(void* mod, char* src, int32_t len)
{
    if (NULL == mod || NULL == src || 0 >= len)
    {
        return fail;
    }
    req_auth_t req;
    res_auth_t res;
    net_head_t nethead;
    module_t* cmd = (module_t*) mod;
    char res_buf[MAX_AUTH_OPTION_LEN] = {0};
    int32_t res_len = 0;
    mysql_t* link = frame_mysql_get(cmd->db_idx);

    frame_nethead_decode(src, &nethead);
    if (fail == cmd->decode((void*)&req, (src + POSI_MSG_BODY),
        (len - NET_HEAD_LEN)))
    {
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]decode message body fail,msgid=0x%04x,seq=%d,uin=%d\n",
            __FILE__, __FUNCTION__, nethead.msg_id, nethead.sequence,
            nethead.uin);
        return fail;
    }
    auth_log->write_file(log_mask_detail, (void*)auth_log,
        "[%s::%s]req msg info: nethead(msgid:0x%04x,seq:%d,uin:%d,fd:%d,pkg_len"
        "=%d),body(account:%s,opt_len:%d),dump opt:\n", __FILE__, __FUNCTION__,
        nethead.msg_id, nethead.sequence, nethead.uin, nethead.sock_fd,
        nethead.pkg_len, req.account, req.opt_len);
    auth_log->dump_bin(log_mask_detail, (void*)auth_log, req.opt, req.opt_len-1);

    if (fail == authdb_auth_renew_process(&req, &res, link))
    {
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]process auth(uin:%d,account:%s) renew fail\n", __FILE__, 
            __FUNCTION__, nethead.uin, req.account);
        //return fail;
    }
    auth_log->write_file(log_mask_detail, (void*)auth_log,
        "[%s::%s]res info:result_id:%d,uin:%d,account:%s,sign_len:%d\n", __FILE__,
        __FUNCTION__, res.result_id, res.uin, res.account, res.sign_len);
    //编码回复信息
    cmd->encode((void*)&res, &res_buf[NET_HEAD_LEN], &res_len);
    //编码nethead信息
    nethead.pkg_len = NET_HEAD_LEN + res_len;
    frame_nethead_encode(&res_buf[0], &nethead);
    //发送给主线程
    frame_worker_send_message(auth_log, res_buf, res_len + NET_HEAD_LEN);

    return success;
}

static int32_t authdb_auth_renew_process(req_auth_t* req, res_auth_t* res, mysql_t* link)
{
    uint64_t hash_md5;
    cached_t* obj = NULL;
    role_info_t* role = NULL;
    int32_t ret;

    res->result_id = result_id_success;
	snprintf(res->platform, sizeof(res->platform), "%s", req->platform);
    snprintf(res->account, sizeof(res->account), "%s", req->account);
	snprintf(res->sdk, sizeof(res->sdk), "%s", req->sdk);
    //检查用户group_id
    if (0 >= req->group_id || 0 >= req->channel_id)
    {
        res->result_id = result_id_invalid_group_id;
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]account(%s) send invalid group id(%d) or channel id(%d)\n", __FILE__,
            __FUNCTION__, req->account, req->group_id, req->channel_id);
        return fail;
    }
    //检查原始signature的合法性
    ret = authdb_auth_renew_check(req);
    if (0 > ret)
    {
        res->result_id = result_id_system;
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]account(%s) check renew signatrue return val(%d)\n",
            __FILE__, __FUNCTION__, req->account, ret);
        return fail;
    }

	uint64_t tmp_md5 = 0;
    md5_int64(&tmp_md5, &hash_md5, req->account, strlen(req->account));
    if (NULL == (obj = (cached_t*)frame_data_get_by(hash_md5, IS_OK)))
    {
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]account:%s,key:%"UINT64_FMT" IS_OK find obj return NULL\n", 
            __FILE__, __FUNCTION__, req->account, hash_md5);
        res->result_id = result_id_not_obj;
        return fail;
    }
    role = authdb_auth_get_role(obj, req->platform, req->sdk, req->group_id, IS_OK);
    if (IS_NOT_OK == obj->is_load_normal)
    {
        obj->hash_md5 = hash_md5;
        snprintf(obj->account, sizeof(obj->account), "%s", req->account);
        obj->role_count = 0;
        obj->is_load_normal = IS_OK;
    }
	role->is_new = IS_NOT_OK;
    if (IS_NOT_OK == role->is_load)
    {
        if (fail == authdb_auth_load(auth_log, NULL, obj, link, role, IS_NOT_OK))
        {
            res->result_id = result_id_system;
            auth_log->write_file(log_mask_error, (void*)auth_log,
                "[%s::%s]load data from database error, result id(%d)\n",
                __FILE__, __FUNCTION__, res->result_id);
            return fail;
        }
    }
    res->uin = role->uin;
	res->channel_id = role->channel_id;
    res->group_id = role->group_id;
	res->is_new = role->is_new;

    if (0 > (ret = authdb_auth_build_res(res)))
    {
        res->result_id = result_id_system;
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]build response info fail, return val(%d)\n", __FILE__, 
            __FUNCTION__, ret);
        return fail;
    }
    auth_log->write_file(log_mask_detail, (void*)auth_log, 
        "[%s::%s]dump session key:\n", __FILE__, __FUNCTION__);
    auth_log->dump_bin(log_mask_detail, (void*)auth_log, res->session_key,
        sizeof(res->session_key));

    return success;
}

static int32_t authdb_auth_renew_check( req_auth_t* req )
{
    if (NULL == req)
    {
        return -1;
    }
    auth_signature_t sig;
    char sign_buf[MAX_AUTH_OPTION_LEN] = {0};
    int32_t len;
    int32_t now_time;
    memset(&sig, 0, sizeof(sig));

    len = frame_xtea_decrypt(req->opt, req->opt_len, sign_buf, sizeof(sign_buf),
        authdb_auth_crypt_key);
    if (0 > len)
    {
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]account:%s decrypt fail,return val(%d)\n", __FILE__,
            __FUNCTION__, req->account, len);
        return -2;
    }
    authdb_auth_signature_decode(&sig, sign_buf, len);
    auth_log->write_file(log_mask_detail, (void*)auth_log,
        "[%s::%s]decode signature info:uin:%d,account:%s,timestamp:%d,dump "
        "ses_key:\n", __FILE__, __FUNCTION__, sig.uin, sig.account, sig.timestamp);
    auth_log->dump_bin(log_mask_detail, (void*)auth_log, sig.ses_key,
        sizeof(sig.ses_key));
    if (0 != strncasecmp(req->account, sig.account, strlen(req->account)))
    {
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]req account(%s) not equal sign account(%s)\n", __FILE__,
            __FUNCTION__, req->account, sig.account);
        return -3;
    }
#if defined(_DEBUG) || defined(DEBUG)
    //do nothing
    now_time = 0;
#else
    now_time = time(NULL);
    if (HALF_HOUR_SECS < (now_time - sig.timestamp))
    {
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]now_time(%d) - sign time(%d) more than times(%d)\n",
            __FILE__, __FUNCTION__, now_time, sig.timestamp, HALF_HOUR_SECS);
        return -4;
    }
#endif
    return 0;
}
