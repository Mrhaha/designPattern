#include "authdb_auth.h"
#include "frame_worker.h"
const char authdb_auth_crypt_key[MAX_ENCRYPT_KEY_LEN] = {'L', 'a', 'u', 'r', 'e', 'l', '@', '1', '2', '3', 'u', '.', 'c', 'o', 'm', '$'};
static int32_t authdb_auth_insert(record_t* log, record_t* bill, cached_t* obj, mysql_t* link, role_info_t* role);

static int32_t authdb_auth_insert(record_t* log, record_t* bill, cached_t* obj, mysql_t* link, role_info_t* role)
{
    if (NULL == obj || NULL == link || NULL == log)
    {
        return fail;
    }
    int32_t err = 0;
    role->last_login_time = time(NULL);
    //拉取原始uin
    if (fail == authdb_auth_get_uin(&role->uin, link))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]account:%s get uin fail\n", __FILE__, __FUNCTION__,
            obj->account);
        return fail;
    }
    link->is_query = IS_NOT_OK;
    link->sql_len = snprintf(link->sql, sizeof(link->sql), "insert into %s.%s_%d"
        " (uin,account,platform,sdk,login_time,channel_id,group_id,create_time) values (%d,'%s','%s','%s',%d,%d,%d,%d)",
        link->cfg->db_name, MMO_AUTH_NAME, get_table_id(obj->hash_md5), role->uin,
        obj->account, role->platform, role->sdk, role->last_login_time, role->channel_id, role->group_id,
        role->last_login_time);
    if (fail == frame_worker_send_sql(log, role->uin, link, BILL_NORMAL))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]uin(%d) sql query(%s) fail, err(%d)\n", __FILE__, __FUNCTION__,
            role->uin, link->sql, err);
        return fail;
    }

    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]sql query(%s) success\n", __FILE__, __FUNCTION__, link->sql);

	if (NULL != bill)
	{
		bill->write_file(log_mask_error, (void*)bill, "%d,%d,%d,%d,%d,%s,%s,%s\n", err, role->uin, role->channel_id, role->group_id, role->last_login_time, role->account, role->platform, role->sdk);
	}

    return success;
}
//static function end
//////////////////////////////////////////////////////////////////////////
int32_t authdb_auth_signature_encode(auth_signature_t* sig, char* out, int32_t* len)
{
    if (NULL == sig || NULL == out || NULL == len)
    {
        return fail;
    }
    char* ptmp = out;
    int32_t encode_len = 0;
    int32_t out_len = 0;

    //key
    encode_len = frame_common_encode_memory(&ptmp, sig->ses_key, sizeof(sig->ses_key));
    out_len += encode_len;
    //uin
    encode_len = frame_common_encode_int32(&ptmp, sig->uin);
    out_len += encode_len;
	//channel id
	encode_len = frame_common_encode_int32(&ptmp, sig->channel_id);
	out_len += encode_len;
	//group id
	encode_len = frame_common_encode_int32(&ptmp, sig->group_id);
	out_len += encode_len;
	//platform
	encode_len = frame_common_encode_string(&ptmp, sig->platform, sizeof(sig->platform));
	out_len += encode_len;
    //account
    encode_len = frame_common_encode_string(&ptmp, sig->account, sizeof(sig->account));
    out_len += encode_len;
	//sdk
	encode_len = frame_common_encode_string(&ptmp, sig->sdk, sizeof(sig->sdk));
	out_len += encode_len;
    //service bit
    encode_len = frame_common_encode_memory(&ptmp, sig->service_bit, sizeof(sig->service_bit));
    out_len += encode_len;
    //timestamp
    encode_len = frame_common_encode_int32(&ptmp, sig->timestamp);
    out_len += encode_len;

    *len = out_len;

    return success;
}

int32_t authdb_auth_signature_decode(auth_signature_t* sig, char* in, int32_t len)
{
    if (NULL == sig || NULL == in || 0 >= len)
    {
        return fail;
    }
    char* ptmp = in;
    int16_t remain = len;
    int32_t decode_len = 0;
    
    decode_len = frame_common_decode_memory(&ptmp, sig->ses_key, sizeof(sig->ses_key));
    remain -= decode_len;

    decode_len = frame_common_decode_int32(&ptmp, (uint32_t*)&sig->uin);
    remain -= decode_len;

	decode_len = frame_common_decode_int32(&ptmp, (uint32_t*)&sig->channel_id);
	remain -= decode_len;

	decode_len = frame_common_decode_int32(&ptmp, (uint32_t*)&sig->group_id);
	remain -= decode_len;

	decode_len = frame_common_decode_string(&ptmp, sig->platform, sizeof(sig->platform));
	remain -= decode_len;

    decode_len = frame_common_decode_string(&ptmp, sig->account, sizeof(sig->account));
    remain -= decode_len;

	decode_len = frame_common_decode_string(&ptmp, sig->sdk, sizeof(sig->sdk));
	remain -= decode_len;

    decode_len = frame_common_decode_memory(&ptmp, sig->service_bit, sizeof(sig->service_bit));
    remain -= decode_len;

    decode_len = frame_common_decode_int32(&ptmp, (uint32_t*)&sig->timestamp);
    remain -= decode_len;
    if (0 > remain)
    {
        return fail;
    }

    return success;
}

int32_t authdb_auth_req_decode( void* body, char* in, int32_t len )
{
    if (NULL == body || NULL == in || 0 >= len)
    {
        return fail;
    }
    char* ptmp = in;
    int16_t remain = len;
    int32_t decode_len = 0;
    req_auth_t* info = (req_auth_t*) body;

	decode_len = frame_common_decode_int32(&ptmp, (uint32_t*)&info->channel_id);
	remain -= decode_len;
    decode_len = frame_common_decode_int32(&ptmp, (uint32_t*)&info->group_id);
    remain -= decode_len;
	decode_len = frame_common_decode_string(&ptmp, info->platform, sizeof(info->platform));
	remain -= decode_len;
    decode_len = frame_common_decode_string(&ptmp, info->account, sizeof(info->account));
    remain -= decode_len;
	decode_len = frame_common_decode_string(&ptmp, info->sdk, sizeof(info->sdk));
	remain -= decode_len;

    decode_len = frame_common_decode_int16(&ptmp, (uint16_t*)&info->opt_len);
    remain -= decode_len;
    if (MAX_AUTH_OPTION_LEN < info->opt_len)
    {
        return fail;
    }
    if (0 < info->opt_len)
    {
        decode_len = frame_common_decode_memory(&ptmp, info->opt, info->opt_len);
        remain -= decode_len;
    }
    if (0 > remain)
    {
        return fail;
    }
    return success;
}

int32_t authdb_auth_res_encode(void* body, char* out, int32_t* len)
{
    if (NULL == body || NULL == out || NULL == len)
    {
        return fail;
    }
    char* ptmp = out;
    int32_t encode_len = 0;
    int32_t out_len = 0;
    res_auth_t* info = (res_auth_t*)body;

    encode_len = frame_common_encode_int16(&ptmp, info->result_id);
    out_len += encode_len;

    encode_len = frame_common_encode_int32(&ptmp, info->uin);
    out_len += encode_len;

	encode_len = frame_common_encode_int32(&ptmp, info->channel_id);
	out_len += encode_len;

    encode_len = frame_common_encode_int32(&ptmp, info->group_id);
    out_len += encode_len;

	encode_len = frame_common_encode_int32(&ptmp, info->is_new);
	out_len += encode_len;

	encode_len = frame_common_encode_string(&ptmp, info->platform, sizeof(info->platform));
	out_len += encode_len;

    encode_len = frame_common_encode_string(&ptmp, info->account, sizeof(info->account));
    out_len += encode_len;

	encode_len = frame_common_encode_string(&ptmp, info->sdk, sizeof(info->sdk));
	out_len += encode_len;

    encode_len = frame_common_encode_memory(&ptmp, info->session_key, sizeof(info->session_key));
    out_len += encode_len;

    if (MAX_SIGNATURE_LEN < info->sign_len)
    {
        info->sign_len = MAX_SIGNATURE_LEN;
    }
    encode_len = frame_common_encode_int16(&ptmp, info->sign_len);
    out_len += encode_len;
    if (0 < info->sign_len)
    {
        encode_len = frame_common_encode_memory(&ptmp, info->sign_buf, info->sign_len);
        out_len += encode_len;
    }
    *len = out_len;

    return success;
}

int32_t authdb_auth_build_res( res_auth_t* res )
{
    if (NULL == res)
    {
        return -1;
    }
    auth_signature_t signature;
    memset(&signature, 0, sizeof(signature));
    char tmp[8] = {0};
    char str_md5[MD5_MAX_LEN] = {0};
    char sign_buf[MAX_AUTH_OPTION_LEN] = {0};
    int32_t len = 0;
    int32_t encypt_len = 0;
    
    *((uint32_t*)&tmp[0]) = (uint32_t)rand();
    *((uint32_t*)&tmp[4]) = (uint32_t)rand();
    md5_string((const unsigned char*)&tmp[0], &str_md5[0], sizeof(tmp));
    str_md5[MD5_MAX_LEN-1] = '\0';
	
    md5_convert(str_md5, res->session_key);
	
    memcpy(signature.ses_key, res->session_key, sizeof(signature.ses_key));
    signature.uin = res->uin;
	signature.channel_id = res->channel_id;
	signature.group_id = res->group_id;
	snprintf(signature.platform, sizeof(signature.platform), "%s", res->platform);
    snprintf(signature.account, sizeof(signature.account), "%s", res->account);
	snprintf(signature.sdk, sizeof(signature.sdk), "%s", res->sdk);
    signature.timestamp = time(NULL);
	
    authdb_auth_signature_encode(&signature, &sign_buf[0], &len);
    encypt_len = frame_xtea_encrypt(sign_buf, (size_t)len, res->sign_buf,
        sizeof(res->sign_buf), authdb_auth_crypt_key);
    if (0 >= encypt_len)
    {
        return -2;
    }
    res->sign_len = (int16_t) encypt_len;

    return success;
}

int32_t authdb_auth_load( record_t* log, record_t* bill, cached_t* obj, mysql_t* link, role_info_t* role, int32_t is_updata )
{
    if (NULL == obj || NULL == link || NULL == log)
    {
        return fail;
    }
    int32_t err = -1;
    link->is_query = IS_OK;
    link->sql_len = snprintf(link->sql, sizeof(link->sql), "select uin,account,"
        "login_time from %s_%d where account='%s' and platform='%s' and sdk='%s' and group_id=%d",
        MMO_AUTH_NAME, get_table_id(obj->hash_md5), obj->account, role->platform, role->sdk, role->group_id);
    if (fail == link->sql_query((void*)link, &err))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]sql query(%s) return fail, errno(%d)\n", __FILE__,
            __FUNCTION__, link->sql, err);
        return fail;
    }
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]sql query(%s) success,affect rows num(%d)\n", __FILE__,
        __FUNCTION__, link->sql, link->rows_num);
    //如果没有搜索到记录，则表示需要新建一条记录
    if (0 >= link->rows_num)
    {
        if (fail == authdb_auth_insert(log, bill, obj, link, role))
        {
            log->write_file(log_mask_error, (void*)log,
                "[%s::%s]authdb insert new record fail\n", __FILE__, __FUNCTION__);
            return fail;
        }
		role->is_new = IS_OK;
    }
    else //有记录，则只需要load下数据就行
    {
        link->fetch_row((void*)link);
        if (link->cur_row[0])
        {
            role->uin = (int32_t)atoi(link->cur_row[0]);
        }
        if (link->cur_row[2])
        {
            role->last_login_time = (int32_t) atoi(link->cur_row[2]);
        }
        if (IS_OK == is_updata)
        {
            role->last_login_time = time(NULL);
            authdb_auth_update_time(log, bill, obj, link, role);
        }
    }
    role->is_load = IS_OK;
    return success;
}

int32_t authdb_auth_get_uin(int32_t* uin, mysql_t* link)
{
    link->is_query = IS_NOT_OK;
    //开始一个事务
    link->sql_len = snprintf(link->sql, sizeof(link->sql), "update %s.uin set "
        "uin=LAST_INSERT_ID(uin+1) where id=1", link->cfg->db_name);
    link->sql_query((void*)link, NULL);
    *uin = frame_mysql_last_id(link);
    if (0 >= *uin)
    {
        return fail;
    }
    //结束整个事务

    return success;
}
int32_t authdb_auth_update_time( record_t* log, record_t* bill, cached_t* obj, mysql_t* link, role_info_t* role )
{
    int32_t err = 0;

    //更新最后登录时间
    link->is_query = IS_NOT_OK;
    link->sql_len = snprintf(link->sql, sizeof(link->sql), "update %s.%s_%d set"
        " login_time=%d where uin=%d", link->cfg->db_name, MMO_AUTH_NAME,
        get_table_id(obj->hash_md5), role->last_login_time, role->uin);
    if (fail == frame_worker_send_sql(log, role->uin, link, BILL_NORMAL))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]uin(%d) sql query(%s) fail, err(%d)\n", __FILE__, __FUNCTION__,
            role->uin, link->sql, err);
        return fail;
    }

    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]sql query(%s) success\n", __FILE__, __FUNCTION__, link->sql);
    
	if (NULL != bill)
    {
        bill->write_file(log_mask_error, (void*)bill, "%d,%d,%d,%d,%d,%s,%s,%s\n", err, role->uin, role->channel_id, role->group_id, role->last_login_time, role->account, role->platform, role->sdk);
    }

    return success;
}

role_info_t* authdb_auth_get_role( cached_t* obj, const char* platform, const char* sdk, int32_t group_id, int16_t is_create )
{
    if (NULL == obj || NULL == platform || NULL == sdk || 0 >= group_id)
    {
        return NULL;
    }
    int16_t i;
    role_info_t* find = NULL;
    for (i=0; i < obj->role_count; ++i)
    {
        find = &obj->roles[i];
        if (group_id == find->group_id && 0 == strcmp(find->platform, platform) && 0 == strcmp(find->sdk, sdk))
        {
            return find;
        }
    }
    if (NULL == find && IS_OK == is_create)
    {
        if (MAX_ROLE_COUNT <= obj->role_count)
        {
            //找到时间最小的那个
            find = &obj->roles[0];
            for (i = 1; i < obj->role_count; ++i)
            {
                role_info_t* tmp = &obj->roles[i];
                if (tmp->last_login_time < find->last_login_time)
                {
                    find = tmp;
                }
            }
            goto ret_step;
        }
        find = &obj->roles[obj->role_count];
        ++obj->role_count;
    }
ret_step:
    if (NULL != find)
    {
		snprintf(find->platform, sizeof(find->platform), "%s", platform);
		snprintf(find->sdk, sizeof(find->sdk), "%s", sdk);
        find->group_id = group_id;
        find->uin = 0;
		find->channel_id = 0;
        find->is_load = IS_NOT_OK;
        //find->last_login_time = time(NULL);
    }
    return find;
}
