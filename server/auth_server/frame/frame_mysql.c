#include "frame_mysql.h"
static mysql_t* frame_mysql_col = NULL;
static int32_t frame_mysql_count = 0;
static record_t* frame_mysql_log = NULL;

static int32_t frame_mysql_init(void* data, mysql_cfg_t* cfg);
static int32_t frame_mysql_connect(mysql_t* p);
static int32_t frame_mysql_query(void* data, int32_t* err);
static int32_t frame_mysql_reset(mysql_t* p);
static int32_t frame_mysql_fetch_row(void* data);

static int32_t frame_mysql_init(void* data, mysql_cfg_t* cfg)
{
    if (NULL == data || NULL == cfg)
    {
        return fail;
    }
    mysql_t* p = (mysql_t*) data;
    p->cfg = cfg;
    int32_t ret;
    if (0 > (ret = frame_mysql_connect(p)))
    {
        frame_mysql_log->write_file(log_mask_error, (void*)frame_mysql_log,
            "[%s::%s]connect mysql fail,return val(%d)\n", __FILE__,
            __FUNCTION__, ret);
        return fail;
    }
    return success;
}
static int32_t frame_mysql_connect(mysql_t* p)
{
    if (NULL == p)
    {
        return -1;
    }
    frame_mysql_reset(p);

    p->link = mysql_init(NULL);
    if (NULL == p->link)
    {
        return -2;
    }

    mysql_options(p->link, MYSQL_SET_CHARSET_NAME, DEFAULT_DB_CHARSET);
    
	if (NULL == mysql_real_connect(p->link, p->cfg->db_ip, p->cfg->usr_name,
        p->cfg->usr_pwd, p->cfg->db_name, p->cfg->port, NULL, 0))
    {
        return -3;
    }
    p->is_connected = IS_OK;

	my_bool reconnect_flag = 1;
	mysql_options(p->link, MYSQL_OPT_RECONNECT, &reconnect_flag);

    p->last_active_time = (uint32_t) time(NULL);
    return success;
}
static int32_t frame_mysql_query(void* data, int32_t* err)
{
    if (NULL == data)
    {
        return fail;
    }

    if (NULL != err)
    {
        *err = 0;
    }

    mysql_t* p = (mysql_t*) data;
    char s = p->sql[0];

	//先释放之前的数据
	frame_mysql_free_result(p);

    if (IS_OK == p->is_query
        && (s != 's' && s != 'S'))
    {
        frame_mysql_log->write_file(log_mask_warning, (void*)frame_mysql_log,
            "[%s::%s]is_query IS_OK, but sql(%s) is not query\n", __FILE__,
            __FUNCTION__, p->sql);
        p->is_query = IS_NOT_OK;
    }

    if (IS_OK != p->is_connected)
    {
        frame_mysql_log->write_file(log_mask_error, (void*)frame_mysql_log,
            "[%s::%s]the connected is IS_NOT_OK\n", __FILE__, __FUNCTION__);
        frame_mysql_reset(p);

        frame_mysql_connect(p);
		if (IS_OK != p->is_connected)
		{
			return fail;
		}
    }
	else
	{
		//检查数据库是否还连接上
		if (0 != mysql_ping(p->link))
		{
			return fail;
		}
	}

    if (0 != mysql_real_query(p->link, p->sql, p->sql_len))
    {
        int32_t no;
        no = mysql_errno(p->link);
        if (NULL != err)
        {
            *err = no;
        }

        frame_mysql_log->write_file(log_mask_error, (void*)frame_mysql_log,
            "[%s::%s]mysql real query[%s] fail,errno(%d),msg(%s)\n", __FILE__,
            __FUNCTION__, p->sql, no, mysql_error(p->link));
        return fail;
    }
	else
	{
		p->last_active_time = (uint32_t) time(NULL);
	}

    if (IS_OK == p->is_query)
    {
        p->records = mysql_store_result(p->link);
        p->rows_num = mysql_num_rows(p->records);
        p->is_record = IS_OK;
    }
    return success;
}
static int32_t frame_mysql_reset(mysql_t* p)
{
    if (NULL == p)
    {
        return fail;
    }

	frame_mysql_free_result(p);

    if (IS_OK == p->is_connected)
    {
		mysql_close(p->link);
        p->is_connected = IS_NOT_OK;
        p->link = NULL;
    }

    return success;
}
static int32_t frame_mysql_fetch_row(void* data)
{
    if (NULL == data)
    {
        return fail;
    }

    mysql_t* p = (mysql_t*) data;
    if (IS_NOT_OK == p->is_record)
    {
        frame_mysql_log->write_file(log_mask_error, (void*)frame_mysql_log,
            "[%s::%s]is_record=IS_NOT_OK,so return\n", __FILE__, __FUNCTION__);
        return fail;
    }

    if (0 >= p->rows_num)
    {
        frame_mysql_log->write_file(log_mask_error, (void*)frame_mysql_log,
            "[%s::%s]affect row num(%d) invalid\n", __FILE__, __FUNCTION__,
            p->rows_num);
        return fail;
    }

    p->cur_row = mysql_fetch_row(p->records);
    return success;
}
//static function end
//////////////////////////////////////////////////////////////////////////
int32_t frame_mysql_module_init( record_t* log, mysql_cfg_t* cfg, int32_t count )
{
    if (NULL == cfg || 0 >= count)
    {
        return fail;
    }
    if (count > MAX_DB_COUNT)
    {
        count = MAX_DB_COUNT;
    }
    int32_t idx;
    frame_mysql_count = count;
    frame_mysql_log = log;
    int32_t size = count * sizeof(mysql_t);
    frame_mysql_col = (mysql_t*) malloc(size);
    if (NULL == frame_mysql_col)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]malloc size(%d) fail\n", __FILE__, __FUNCTION__, size);
        return fail;
    }
    memset(frame_mysql_col, 0, size);
    mysql_t* tmp = frame_mysql_col;
    mysql_cfg_t* ctmp = cfg;
    for (idx=0; idx < count; ++idx,++tmp,++ctmp)
    {
        tmp->init_mysql = frame_mysql_init;
        tmp->sql_query = frame_mysql_query;
        tmp->fetch_row = frame_mysql_fetch_row;
        if (fail == tmp->init_mysql((void*)tmp, ctmp))
        {
            log->write_file(log_mask_error, (void*)log,
                "[%s::%s][%d:%d]init mysql(ip:%s,port:%d,user:%s,pwd:%s,db:%s)"
                " link fail\n", __FILE__, __FUNCTION__, idx, count,
                ctmp->db_ip, ctmp->port, ctmp->usr_name, ctmp->usr_pwd,
                ctmp->db_name);
            return fail;
        }
    }
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]init mysql count(%d) success\n", __FILE__, __FUNCTION__,
        count);
    return success;
}

int32_t frame_mysql_module_destroy()
{
    if (NULL != frame_mysql_col && 0 < frame_mysql_count)
    {
        int32_t i;
        for (i=0; i < frame_mysql_count; ++i)
        {
            mysql_t* p = frame_mysql_col + i;
            if (IS_OK == p->is_record)
            {
                mysql_free_result(p->records);
            }
            if (IS_OK == p->is_connected)
            {
                mysql_close(p->link);
            }
            p->link = NULL;
        }
        free(frame_mysql_col);
        frame_mysql_col = NULL;
    }
    return success;
}

mysql_t* frame_mysql_get( int32_t idx )
{
    if (0 > idx)
    {
        return NULL;
    }

    mysql_t* p = NULL;
    int16_t i;

    if (idx < frame_mysql_count)
    {
        p = frame_mysql_col + idx;
        if (p->cfg->idx != idx)
        {
            p = NULL;
        }
    }

    if (NULL == p)
    {
        for (i=0; i < frame_mysql_count; ++i)
        {
            p = frame_mysql_col + i;
            if (p->cfg->idx == idx)
            {
                break;
            }
        }
    }

    return p;
}

int32_t frame_mysql_last_id( mysql_t* link )
{
    if (NULL == link)
    {
        return -1;
    }
    return (int32_t)mysql_insert_id(link->link);
}

int32_t frame_mysql_encode( mysql_body_t* info, char* out, int32_t* len )
{
    if (NULL == info || NULL == out || NULL == len)
    {
        return fail;
    }
    char* ptmp = out;
    int32_t encode_len = 0;
    int32_t out_len = 0;

    encode_len = frame_common_encode_int32(&ptmp, info->pkg_len);
    out_len += encode_len;

    encode_len = frame_common_encode_int16(&ptmp, info->db_idx);
    out_len += encode_len;

	encode_len = frame_common_encode_int8(&ptmp, info->just_bill);
	out_len += encode_len;

    encode_len = frame_common_encode_string(&ptmp, info->sql, sizeof(info->sql));
    out_len += encode_len;

    *len = out_len;

    return success;
}

int32_t frame_mysql_decode( mysql_body_t* info, char* in, int32_t len )
{
    if (NULL == info || NULL == in || 0 >= len)
    {
        return fail;
    }
    char* ptmp = in;
    int16_t remain = len;
    int32_t decode_len = 0;

    decode_len = frame_common_decode_int32(&ptmp, (uint32_t*)&info->pkg_len);
    remain -= decode_len;

    decode_len = frame_common_decode_int16(&ptmp, (uint16_t*)&info->db_idx);
    remain -= decode_len;

	decode_len = frame_common_decode_int8(&ptmp, (uint8_t*)&info->just_bill);
	remain -= decode_len;

    decode_len = frame_common_decode_string(&ptmp, info->sql, sizeof(info->sql));
    remain -= decode_len;

    if (0 > remain)
    {
        return fail;
    }
    return success;
}
int32_t frame_mysql_free_result( mysql_t* link )
{
	if (NULL == link)
	{
		return fail;
	}

	if (IS_OK == link->is_record)
	{
		mysql_free_result(link->records);
		link->is_record = IS_NOT_OK;
		link->records = NULL;
	}

	link->rows_num = 0;
	return success;
}
int32_t frame_mysql_string_escape( char* src, char* dst, mysql_t* link )
{
	if (NULL == src || NULL == dst || NULL == link)
	{
		return fail;
	}
	int32_t ret = mysql_real_escape_string(link->link, dst, src, strlen(src));

	return ret > 0 ? success : fail;
}

int32_t frame_mysql_check_alive( int32_t gap )
{
	uint32_t now_time = (uint32_t)time(NULL);
	int16_t i;
	for (i = 0; i < frame_mysql_count; ++i)
	{
		mysql_t* p = (frame_mysql_col+i);
		//需要重新激活下该链接
		if ((int32_t)(now_time - p->last_active_time) >= gap)
		{
			frame_mysql_log->write_file(log_mask_detail, set_void(frame_mysql_log),
				"[%s::%s][%d::%d]check mysql alive(now:%d,last_alive:%d,gap:%d)\n",
				__FILE__, __FUNCTION__, i+1, frame_mysql_count, now_time,
				p->last_active_time, gap);
			p->is_query = IS_OK;
			//随便搜索下，避免和server断开连接
			p->sql_len = snprintf(p->sql, sizeof(p->sql), "select version()");
			if (fail == p->sql_query(set_void(p), NULL))
			{
				p->last_active_time = now_time;
			}
			else
			{
				frame_mysql_free_result(p);
			}
		}
	}
	return success;
}

