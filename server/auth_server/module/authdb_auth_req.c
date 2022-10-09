// 本协议为请求认证协议
#include "authdb_auth.h"
#include "authdb_config.h"
#include "authdb_worker.h"
#include "frame_nethead.h"
#include "frame_md5.h"
#include "frame_data.h"
#include "frame_worker.h"

static record_t* auth_log = NULL;
static record_t auth_sql_bill;

static int32_t authdb_auth_req_init(void* data);
static int32_t authdb_auth_req_handler(void* mod, char* src, int32_t len);
static int32_t authdb_auth_req_process(req_auth_t* req, res_auth_t* res, mysql_t* link);

module_t authdb_auth_req = {
    authdb_auth_req_init,
    authdb_auth_req_handler,
    authdb_auth_req_decode,
    authdb_auth_res_encode,
    0
};

static int32_t authdb_auth_req_init(void* data)
{
    if (NULL == data)
    {
        return fail;
    }
    char prefix[MAX_PREFIX_LEN] = {0};
    auth_log = (record_t*) data;
    snprintf(prefix, sizeof(prefix), "auth-%d", authdb_worker_process_id);
    auth_sql_bill.init_record = frame_log_bill_init;
    auth_sql_bill.init_record(RECORD_BILL, (void*)&auth_sql_bill, prefix);
    srand(time(NULL));

    return success;
}

static int32_t authdb_auth_req_handler(void* mod, char* src, int32_t len)
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
    mysql_t* link = NULL;
    char res_buf[MAX_AUTH_OPTION_LEN] = {0};
    int32_t res_len = 0;

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
		"=%d,trans_type=%d,len=%d,net_len=%d) body(account:%s,opt_len:%d,opt:%s)\n"
		, __FILE__, __FUNCTION__,
        nethead.msg_id, nethead.sequence, nethead.uin, nethead.sock_fd,
        nethead.pkg_len, nethead.trans_type,
		len,NET_HEAD_LEN,req.account, req.opt_len, req.opt);

    link = frame_mysql_get(cmd->db_idx);
    if (NULL == link)
    {
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]get mysql link(idx:%d) return NULL\n", __FILE__,
            __FUNCTION__, cmd->db_idx);
        return fail;
    }
    if (fail == authdb_auth_req_process(&req, &res, link))
    {
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]process auth req info return fail\n", __FILE__, __FUNCTION__);
        //goto send_msg;
    }
    auth_log->write_file(log_mask_detail, (void*)auth_log,
        "[%s::%s]res info:result_id:%d,uin:%d,account:%s,sign_len:%d\n", __FILE__,
        __FUNCTION__, res.result_id, res.uin, res.account, res.sign_len);

//send_msg:
    //编码回复信息
    cmd->encode((void*)&res, &res_buf[NET_HEAD_LEN], &res_len);
    //编码nethead信息
    nethead.pkg_len = NET_HEAD_LEN + res_len;
	nethead.trans_type = TRANS_TO_CLIENT;
    frame_nethead_encode(&res_buf[0], &nethead);
    //发送给主线程
    frame_worker_send_message(auth_log, res_buf, res_len + NET_HEAD_LEN);

    return success;
}

static int32_t authdb_auth_req_process(req_auth_t* req, res_auth_t* res, mysql_t* link)
{
    //just for test
    //return success;

    uint64_t hash_md5;
    cached_t* obj = NULL;
    role_info_t* role = NULL;
    int32_t ret;
    authdb_cfg_t* cfg = (authdb_cfg_t*) authdb_config_info.data;
    char str_md5[MD5_MAX_LEN] = {0};
    char post_str[1024] = {0};
    res->result_id = result_id_success;
    if (0 >= req->group_id || 0 >= req->channel_id)
    {
        res->result_id = result_id_invalid_group_id;
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]account(%s) send invalid group id(%d) or channel id(%d)\n", __FILE__,
            __FUNCTION__, req->account, req->group_id, req->channel_id);
        return fail;
    }
	snprintf(res->platform, sizeof(res->platform), "%s", req->platform);
    snprintf(res->account, sizeof(res->account), "%s", req->account);
	snprintf(res->sdk, sizeof(res->sdk), "%s", req->sdk);

    int32_t len = snprintf(post_str, sizeof(post_str), "%s%s", req->account,
        cfg->secret_key);

    md5_string((const unsigned char*)post_str, &str_md5[0], len);
    str_md5[MD5_MAX_LEN-1] = '\0';
    if (0 != strncasecmp(str_md5, req->opt, strlen(str_md5)))
    {
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]account:%s session key(req:%s,computer:%s) not match\n",
            __FILE__, __FUNCTION__, req->account, req->opt, str_md5);
        res->result_id = result_id_invalid_ses_key;
        return fail;
    }

	uint64_t tmp_md5 = 0;
    md5_int64(&tmp_md5, &hash_md5, req->account, strlen(req->account));
    //查找用户
    if (NULL == (obj = (cached_t*)frame_data_get_by(hash_md5, IS_OK)))
    {
        auth_log->write_file(log_mask_error, (void*)auth_log,
            "[%s::%s]account:%s,key:%"UINT64_FMT" IS_OK find obj return NULL\n", 
            __FILE__, __FUNCTION__, req->account, hash_md5);
        res->result_id = result_id_not_obj;
        return fail;
    }
    if (IS_NOT_OK == obj->is_load_normal)
    {
        obj->hash_md5 = hash_md5;
        snprintf(obj->account, sizeof(obj->account), "%s", req->account);
        obj->is_load_normal = IS_OK;
        obj->role_count = 0;
    }
    role = authdb_auth_get_role(obj, req->platform, req->sdk, req->group_id, IS_OK);
	role->is_new = IS_NOT_OK;
	role->channel_id = req->channel_id;
	snprintf(role->account, sizeof(role->account), "%s", req->account);

    //新创建的对象，需要从数据库中load数据
    if (IS_NOT_OK == role->is_load)
    {
        //if (fail == authdb_auth_load(auth_log, &auth_sql_bill, obj, link, IS_OK))
        if (fail == authdb_auth_load(auth_log, &auth_sql_bill, obj, link, role, IS_OK))
        {
            res->result_id = result_id_system;
            auth_log->write_file(log_mask_error, (void*)auth_log,
                "[%s::%s]load data from database error, result id(%d)\n",
                __FILE__, __FUNCTION__, res->result_id);
            return fail;
        }
    }
    else
    {
        role->last_login_time = time(NULL);
        authdb_auth_update_time(auth_log, &auth_sql_bill, obj, link, role);
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
