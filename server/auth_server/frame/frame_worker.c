#include "frame_worker.h"
#include "frame_statis.h"
#include "frame_mysql.h"
#include "frame_data.h"
//int32_t worker_recv_channel;
//int32_t worker_send_channel;

worker_recv_t* frame_worker_recv = NULL;
worker_recv_t* frame_worker_send = NULL;
record_t* frame_worker_sql = NULL;

static record_t* frame_worker_log = NULL;
static module_col_t* frame_worker_module = NULL;

static int32_t frame_worker_netbuf_init(record_t* log, process_t* proc);
static worker_recv_t* frame_worker_creat_buf(int32_t fd, int32_t buf_size);
static void frame_worker_destroy();
static int32_t frame_worker_process_message(record_t* log, worker_recv_t* info);
static int32_t frame_worker_sql_message(record_t* log, worker_recv_t* info);
static int32_t frame_worker_dispath_message(record_t* log, char* src, int32_t len);
static int32_t frame_worker_sql_dispath(record_t* log, char* src, int32_t len);

static int32_t frame_worker_netbuf_init(record_t* log, process_t* proc)
{
    if (NULL == log)
    {
        log = frame_worker_log;
    }
    //开始申请recv
    frame_worker_recv = frame_worker_creat_buf(proc->worker_channel[0],
        MAX_WORKER_NET_LEN);
    //frame_worker_recv = frame_worker_creat_buf(worker_recv_channel);
    if (NULL == frame_worker_recv)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]create recv buf struct fail, so return\n", __FILE__,
            __FUNCTION__);
        return fail;
    }
    else
    {
        log->write_file(log_mask_detail, (void*)log,
            "[%s::%s]create recv buf struct(fd:%d,size:%d) success\n", __FILE__,
            __FUNCTION__, frame_worker_recv->sock_fd,
            frame_worker_recv->buf_size);
    }
    //开始申请send
    frame_worker_send = frame_worker_creat_buf(proc->worker_channel[0], 0);
    //frame_worker_send = frame_worker_creat_buf(worker_send_channel);
    if (NULL == frame_worker_send)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]create send buf struct fail, so return\n", __FILE__,
            __FUNCTION__);
        return fail;
    }
    else
    {
        log->write_file(log_mask_detail, (void*)log,
            "[%s::%s]create send buf struct(fd:%d,size:%d) success\n", __FILE__,
            __FUNCTION__, frame_worker_send->sock_fd,
            frame_worker_send->buf_size);
    }
    return success;
}
static worker_recv_t* frame_worker_creat_buf(int32_t fd, int32_t buf_size)
{
    size_t size = sizeof(worker_recv_t);
    //开始申请
    worker_recv_t* rv = (worker_recv_t*) malloc(size);
    memset(rv, 0, size);
    if (NULL == rv)
    {
        frame_worker_log->write_file(log_mask_error, (void*)frame_worker_log,
            "[%s::%s]malloc worker struct(size=%d) fail\n", __FILE__,
            __FUNCTION__, size);
        return NULL;
    }
    rv->buf = NULL;
    rv->buf_size = buf_size;
    rv->recv_bytes = 0;
    rv->sock_fd = fd;
    if (0 < buf_size)
    {
        rv->buf = (char*) malloc(rv->buf_size);
        if (NULL == rv->buf)
        {
            frame_worker_log->write_file(log_mask_error, (void*)frame_worker_log,
                "[%s::%s]malloc worker struct buf(size=%d) fail\n",
                __FILE__, __FUNCTION__, buf_size);
            free(rv);
            return NULL;
        }
    }
    return rv;
}
static void frame_worker_destroy()
{
    if (NULL != frame_worker_recv)
    {
        if (NULL != frame_worker_recv->buf)
        {
            free(frame_worker_recv->buf);
            frame_worker_recv->buf = NULL;
        }
        free(frame_worker_recv);
    }
    if (NULL != frame_worker_send)
    {
        if (NULL != frame_worker_send->buf)
        {
            free(frame_worker_send->buf);
            frame_worker_send->buf = NULL;
        }
        free(frame_worker_send);
    }
}
static int32_t frame_worker_process_message(record_t* log, worker_recv_t* info)
{
    if (NULL == info)
    {
        return fail;
    }
    if (NULL == log)
    {
        log = frame_worker_log;
    }
    int32_t remained = info->recv_bytes;
    int32_t pkg_size = 0;
    char* buf_start = info->buf;
    char* tmp = NULL;

    if (remained <= (int32_t)MIN_PKG_LEN)
    {
        return success;
    }
    while (1)
    {
        if ((int32_t)MIN_PKG_LEN >= remained)
        {
            break;
        }
        tmp = buf_start;
        pkg_size = get_pkg_len(tmp);
        //后面的数据已经出现错误，丢弃掉
        if ((int32_t)(MAX_CLIENT_PKG_LEN+NET_HEAD_LEN) <= pkg_size 
            || (int32_t)MIN_PKG_LEN >= pkg_size)
        {
            log->write_file(log_mask_error, (void*)log,
                "[%s::%s]package size(%d) is invalid,min pkg size(%d),"
                "max pkg size(%d),so return\n", __FILE__, __FUNCTION__,
                pkg_size, MIN_PKG_LEN, MAX_CLIENT_PKG_LEN);
            return fail;
        }
        //如果是不完整的包
        if (pkg_size > remained)
        {
            break;
        }
        int32_t ret = frame_worker_dispath_message(log, tmp, pkg_size);
        if (success == ret)
        {
            buf_start += pkg_size;
            remained -= pkg_size;
        }
        else
        {
            log->write_file(log_mask_error, (void*)log,
                "[%s::%s]recv master message dispath fail, return value(%d)\n",
                __FILE__, __FUNCTION__, ret);
            return fail;
        }
    }
    info->recv_bytes = remained;

    //将未处理的数据移动到首部
    if (buf_start != info->buf && info->recv_bytes > 0)
    {
        memmove(info->buf, buf_start, remained);
    }
    return success;
}
static int32_t frame_worker_sql_message(record_t* log, worker_recv_t* info)
{
    if (NULL == info)
    {
        return fail;
    }
    if (NULL == log)
    {
        log = frame_worker_log;
    }
    int32_t remained = info->recv_bytes;
    int32_t pkg_size = 0;
    char* buf_start = info->buf;
    char* tmp = NULL;

    if (remained <= (int32_t)MIN_PKG_LEN)
    {
        return success;
    }
    while (1)
    {
        if ((int32_t)MIN_PKG_LEN >= remained)
        {
            break;
        }
        tmp = buf_start;
        pkg_size = get_pkg_len(tmp);
        //后面的数据已经出现错误，丢弃掉
        if ((int32_t)(MAX_SQL_LEN + NET_HEAD_LEN) <= pkg_size 
            || (int32_t)MIN_PKG_LEN >= pkg_size)
        {
            log->write_file(log_mask_error, (void*)log,
                "[%s::%s]package size(%d) is invalid,min pkg size(%d),"
                "max pkg size(%d),so return\n", __FILE__, __FUNCTION__,
                pkg_size, MIN_PKG_LEN, MAX_SQL_LEN);
            return fail;
        }
        //如果是不完整的包
        if (pkg_size > remained)
        {
            break;
        }
        //移除掉nethead的长度
        int32_t ret = frame_worker_sql_dispath(log, &tmp[NET_HEAD_LEN],
            (pkg_size-NET_HEAD_LEN));
        if (success == ret)
        {
            buf_start += pkg_size;
            remained -= pkg_size;
        }
        else
        {
            log->write_file(log_mask_error, (void*)log,
                "[%s::%s]recv master message dispath fail, return value(%d)\n",
                __FILE__, __FUNCTION__, ret);
            return fail;
        }
    }
    info->recv_bytes = remained;

    //将未处理的数据移动到首部
    if (buf_start != info->buf && info->recv_bytes > 0)
    {
        memmove(info->buf, buf_start, remained);
    }
    return success;
}

static int32_t frame_worker_dispath_message(record_t* log, char* src, int32_t len)
{
    if (NULL == src || 0 >= len || NULL == frame_worker_module)
    {
        return -1;
    }
    int32_t id = get_msg_id(src);
    int32_t uin = get_uin(src);

    frame_statis_dump(log);
    frame_statis_begin();
    module_t* mod = frame_worker_module->get_module(id);
    if (NULL != mod && NULL != mod->handler)
    {
        log->write_file(log_mask_detail, (void*)log, 
            "[%s::%s]dispath one message,dumpbin:\n", __FILE__, __FUNCTION__);
        log->dump_bin(log_mask_detail, (void*)log, src, len);
        if (success == mod->handler((void*)mod, src, len))
        {
            frame_statis_end();
            log->write_file(log_mask_detail, (void*)log,
                "[%s::%s]dispath pkg count(%d)\n", __FILE__, __FUNCTION__,
                ++frame_process_transport_pkg_count);
        }
        else
        {
            log->write_file(log_mask_warning, (void*)log,
                "[%s::%s]process uin(%d) msgid(0X%X) fail\n", __FILE__,
                __FUNCTION__, uin, id);
        }
    }
    else
    {
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]uin(%d) can not id(%d) get process module\n", __FILE__,
            __FUNCTION__, get_uin(src), id);
    }
    return success;
}
static int32_t frame_worker_sql_dispath(record_t* log, char* src, int32_t len)
{
    if (NULL == src || 0 >= len)
    {
        return -1;
    }
    mysql_body_t body;
    int32_t ret;
    int32_t err = -1;
    mysql_t* link = NULL;
    memset(&body, 0, sizeof(body));

    frame_statis_dump(log);
    frame_statis_begin();
    
    log->write_file(log_mask_detail, (void*)log, 
        "[%s::%s]dispath one message,dumpbin:\n", __FILE__, __FUNCTION__);
    log->dump_bin(log_mask_detail, (void*)log, src, len);

    ret = frame_mysql_decode(&body, src, len);
    if (success == ret)
    {
        link = frame_mysql_get(body.db_idx);
        if (NULL != link)
        {
            link->is_query = IS_NOT_OK;
            link->sql_len = snprintf(link->sql, sizeof(link->sql), "%s",
                body.sql);
            //只记录账单
            if (BILL_JUST == body.just_bill)
            {
                frame_statis_end();
                log->write_file(log_mask_detail, (void*)log,
                    "[%s::%s]query sql(%s) success, dispath pkg count(%d)\n", 
                    __FILE__, __FUNCTION__, body.sql, 
                    ++frame_process_transport_pkg_count);
                err = 0;
                goto just_bill;
            }
            if (fail == link->sql_query((void*)link, &err))
            {
                log->write_file(log_mask_error, (void*)log,
                    "[%s::%s]query sql(%s) fail,len=%d err(%d)\n", __FILE__,
                    __FUNCTION__, body.sql, len,err);
                goto just_bill;
            }
            else
            {
                frame_statis_end();
                log->write_file(log_mask_detail, (void*)log,
                    "[%s::%s]query sql(%s) success, dispath pkg count(%d)\n", 
                    __FILE__, __FUNCTION__, body.sql, 
                    ++frame_process_transport_pkg_count);
            }
        }
        else
        {
            log->write_file(log_mask_warning, (void*)log,
                "[%s::%s]get mysql link(idx:%d) return NULL\n", __FILE__, 
                __FUNCTION__, body.db_idx);
            goto just_bill;
        }
    }
    else
    {
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]decode mysql string fail\n", __FILE__, __FUNCTION__);
    }
just_bill:
    frame_worker_sql->write_file(log_mask_error, (void*)frame_worker_sql,
        "%4d,%s\n", err, body.sql);
    return success;
}
//static function end
//////////////////////////////////////////////////////////////////////////
void frame_worker_set_module_col(module_col_t* mod, record_t* log)
{
    if (NULL != mod)
    {
        frame_worker_module = mod;
        frame_worker_module->init_modules((void*)log);
    }
}
void frame_worker_exit(record_t* log)
{
	if (NULL == log)
	{
		//do nothing
	}

    frame_worker_destroy();
    frame_mysql_module_destroy();
    frame_data_destroy();
    exit(0);
}
void frame_worker_init(record_t* log)
{
    if (NULL == log)
    {
        return ;
    }
    uint16_t i;
    frame_worker_log = log;
    process_t* m = &frame_process_col[frame_process_slot];
    for (i=0; i < frame_process_count; ++i)
    {
        process_t* proc = &frame_process_col[i];
        //只处理不是本进程的数据
        if (0 > proc->pid || i == frame_process_slot)
        {
            continue;
        }
        // add by liuqy 2011-12-22
        if (PROCESS_MYSQL == proc->proc_type)
        {
            log->write_file(log_mask_detail, set_void(log),
                "[%s::%s][%d::%d]close sqlexe no used channel(0:%d,1:%d)\n",
                __FILE__, __FUNCTION__, i, frame_process_count,
                proc->worker_channel[0], proc->worker_channel[1]);
        }
        else
        {
            log->write_file(log_mask_detail, set_void(log),
                "[%s::%s][%d::%d]close worker no used channel(0:%d,1:%d)\n",
                __FILE__, __FUNCTION__, i, frame_process_count,
                proc->worker_channel[0], proc->worker_channel[1]);
        }
        //其它兄弟进程将管道0关闭
        frame_socket_close_channel(log, proc->worker_channel);
        proc->worker_channel[0] = proc->worker_channel[1] = -1;
    }
    //初始化收发数据buffer区，非socket系统缓冲区
    if (fail == frame_worker_netbuf_init(log, m))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]worker process init fail,so exit\n", __FILE__,
            __FUNCTION__);
        exit(0);
    }
    //关闭掉本进程的1管道
    log->write_file(log_mask_detail, set_void(log),
        "[%s::%s]close worker process no used channel(1:%d)\n", __FILE__,
        __FUNCTION__, m->worker_channel[1]);
    if (0 == close(m->worker_channel[1]))
    {
        m->worker_channel[1] = -1;
    }
    else
    {
        log->write_file(log_mask_warning, set_void(log),
            "[%s::%s]close worker process no used channel(1) fail,errno(%d),"
            "msg(%s)\n", __FILE__, __FUNCTION__, errno, strerror(errno));
    }
}
void frame_sql_init(record_t* log)
{
    if (NULL == log)
    {
        return ;
    }
    uint16_t i;
    frame_worker_log = log;
    process_t* m = &frame_process_col[frame_process_slot];
    for (i=0; i < frame_process_count; ++i)
    {
        process_t* proc = &frame_process_col[i];
        //只处理不是本进程的数据
        if (0 > proc->pid || i == frame_process_slot)
        {
            continue;
        }
        // add by liuqy 2011-12-22
        if (PROCESS_MYSQL == frame_process_type      //如果本身是sql执行进程
            && PROCESS_MYSQL == proc->proc_type)    //循环的也是sql执行进程
        {
            log->write_file(log_mask_detail, set_void(log),
                "[%s::%s][%d::%d]close sqlexe no used channel(0:%d,1:%d)\n",
                __FILE__, __FUNCTION__, i, frame_process_count,
                proc->worker_channel[0], proc->worker_channel[1]);
            frame_socket_close_channel(log, proc->worker_channel);
            proc->worker_channel[0] = proc->worker_channel[1] = -1;
        }
    }
    if (fail == frame_worker_netbuf_init(log, m))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]worker process init fail,so exit\n", __FILE__,
            __FUNCTION__);
        exit(0);
    }
    //关闭掉本进程的1管道
    log->write_file(log_mask_detail, set_void(log),
        "[%s::%s]close worker process no used channel(1:%d)\n", __FILE__,
        __FUNCTION__, m->worker_channel[1]);
    if (0 == close(m->worker_channel[1]))
    {
        m->worker_channel[1] = -1;
    }
    else
    {
        log->write_file(log_mask_warning, set_void(log),
            "[%s::%s]close worker process no used channel(1) fail,errno(%d),"
            "msg(%s)\n", __FILE__, __FUNCTION__, errno, strerror(errno));
    }
}
int32_t frame_worker_recv_message( record_t* log, int16_t type )
{
    int32_t ready = 0;
    fd_set reads;
    struct timeval timeout;
    int32_t fd = frame_worker_recv->sock_fd;

    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    FD_ZERO(&reads);
    FD_SET(fd, &reads);

    ready = select(fd+1, &reads, NULL, NULL, &timeout);
    //select 出错或者没有信息
    if (0 > ready)
    {
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]select error,no(%d),msg(%s)\n", __FILE__,
            __FUNCTION__, errno, strerror(errno));
        return fail;
    }
    else if (0 == ready) //没有收到信息
    {
        return success;
    }

    if (FD_ISSET(fd, &reads))
    {
        //开始接收数据
        int32_t offset = frame_worker_recv->recv_bytes;
        char* buf_start = frame_worker_recv->buf + offset;
        int32_t buf_size = frame_worker_recv->buf_size - offset;
        int32_t recved = frame_socket_recv(fd, buf_start, buf_size);

        if (0 >= recved)
        {
            //管道出错了，只能停掉子进程了
            log->write_file(log_mask_error, (void*)log,
                "[%s::%s]recv master sock(%d) message fail, return value(%d)"
                ",so process exit\n", __FILE__, __FUNCTION__, fd, recved);
            exit(EXIT_FAILURE);
        }
        frame_worker_recv->recv_bytes += recved;

        //开始处理收到的包
        if (PROCESS_WORKER == type)
        {
            frame_worker_process_message(log, frame_worker_recv);
        }
        else if (PROCESS_MYSQL == type)
        {
            //TODO
            frame_worker_sql_message(log, frame_worker_recv);
        }
    }

    return success;
}

int32_t frame_worker_send_message( record_t* log, char* src, int32_t len )
{
    if (NULL == log || NULL == src || 0 >= len)
    {
        return fail;
    }
    //测试一下，看看速度瓶颈
    int32_t ret = frame_socket_send(frame_worker_send->sock_fd, src, len, log);
    //int32_t ret = 0;
    if (0 > ret)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]send data(len=%d) to master fail ret(%d)\n",
            __FILE__, __FUNCTION__, len, ret);
        return fail;
    }
    else
    {
        log->write_file(log_mask_detail, (void*)log,
            "[%s::%s]process one pkg(len=%d) to master,dumpbin:\n", __FILE__, 
            __FUNCTION__, len);
        log->dump_bin(log_mask_detail, (void*)log, src, len);
    }

    return success;
}

int32_t frame_worker_send_sql( record_t* log, int32_t uin, mysql_t* link, int16_t bill_type )
{
    if (NULL == log || NULL == link)
    {
        return fail;
    }
    int32_t ret;
    int32_t len;
    mysql_body_t body;
    net_head_t nethead; //新增nethead用于转包
    char src[MAX_SQL_LEN*2] = {0};
    memset(&body, 0, sizeof(body));
    memset(&nethead, 0, sizeof(nethead));
    nethead.uin = uin;
    nethead.trans_type = TRANS_TO_SQL;
    nethead.pkg_len = NET_HEAD_LEN;

    body.db_idx = link->cfg->idx;
    ret = snprintf(body.sql, sizeof(body.sql), "%s", link->sql);
    body.pkg_len = frame_mysql_body_len(ret+1);
    body.just_bill = bill_type;
    frame_mysql_encode(&body, &src[NET_HEAD_LEN], &len);
    nethead.pkg_len += len;
    frame_nethead_encode(&src[0], &nethead);

    ret = frame_socket_send(frame_worker_send->sock_fd, src,
        nethead.pkg_len, log);
    if (0 > ret)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]send data(len=%d,uin=%d) to master process fail ret(%d)\n",
            __FILE__, __FUNCTION__, len, uin, ret);
        return fail;
    }
    else
    {
        log->write_file(log_mask_detail, (void*)log,
            "[%s::%s]process one pkg(uin=%d,len=%d) to master process,"
            "sock(fd=%d),dumpbin:\n", __FILE__, __FUNCTION__, uin, len,
            frame_worker_send->sock_fd);
        log->dump_bin(log_mask_detail, set_void(log), src, len);
    }

    return success;
}
