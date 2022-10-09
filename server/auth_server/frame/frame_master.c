#include "frame_master.h"
#include "frame_statis.h"
//int32_t worker_recv_channel;
//int32_t worker_send_channel;
code_queue_t* frame_master_client_queue = NULL; //处理用户包
code_queue_t* frame_master_worker_queue = NULL; //处理worker进程包

static epoll_event_t* frame_master_events = NULL;
static int32_t frame_master_recv(record_t* log, client_info_t* info);
static int32_t frame_master_process_pkg(record_t* log, client_info_t* info);
static int32_t frame_master_process_total_pkg(record_t* log, char* buf, int32_t len);
static int32_t frame_master_recv_worker_message(record_t* log, client_info_t* info);
static int32_t frame_master_dispath_client(record_t* log, client_info_t* info);
static int32_t frame_master_client_response(record_t* log, char* buf, int32_t len);
//static int32_t frame_master_queue_push(record_t* log, code_queue_t* queue, char* buf, int32_t len);

static int32_t frame_master_recv(record_t* log, client_info_t* info)
{
    if (NULL == log || NULL == info)
    {
        return fail;
    }
    //int32_t sock_fd = info->sock_fd;
    int32_t offset = info->recv_bytes;
    char* buf_start = info->buf + offset;
    int32_t buf_size = info->buf_size - offset;
    int32_t recved = frame_socket_recv(info->sock_fd, buf_start, buf_size);

    if (0 >= recved)
    {
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]client(%s:%d) sock id(%d) close connection, return"
            " value(%d)\n", __FILE__, __FUNCTION__,
            get_addr(info->peer_addr), info->peer_port, info->sock_fd,
            recved);
        frame_socket_clear(info);
        return fail;
    }
    info->recv_bytes += recved;
    info->last_recv_time = time(NULL);

    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]dump recv client message len(%d):\n", __FILE__,
        __FUNCTION__, info->recv_bytes);
    log->dump_bin(log_mask_detail, (void*)log, info->buf,
        info->recv_bytes);

    //开始处理收到的包
    frame_master_process_pkg(log, info);

    return success;
}
static int32_t frame_master_process_pkg(record_t* log, client_info_t* info)
{
	if (NULL == info)
	{
		return fail;
	}
	int32_t remained = info->recv_bytes;
	int32_t pkg_size = 0;
	char* buf_start = &info->buf[0];
	char* tmp = NULL;

	//如果剩余的长度小于最小的头长度，则不需要处理
	if (remained <= (int32_t)MIN_PKG_LEN)
	{
		return success;
	}
	while (1)
	{
		if (remained <= (int32_t)MIN_PKG_LEN)
		{
			break;
		}

		tmp = buf_start;
		pkg_size = get_pkg_len(tmp);

		if (pkg_size >= MAX_CLIENT_PKG_LEN 
			|| pkg_size <= (int32_t)MIN_PKG_LEN)
		{
			log->write_file(log_mask_error, (void*)log,
				"[%s::%s]package size(%d) is invalid, max pkg size(%d),"
				"so close the client(%s:%d)\n", __FILE__, __FUNCTION__,
				pkg_size, MAX_CLIENT_PKG_LEN, get_addr(info->peer_addr),
				info->peer_port);

			frame_socket_clear(info);

			return fail;
		}
		//如果接收到的数据不足一个完整包
		if (remained < pkg_size)
		{
			break;
		}

		//将该socket_fd加入到nethead中
		*(uint32_t*)(tmp + POSI_SOCK_FD) = htonl((uint32_t)info->sock_fd);
		//将sequence加入到nethead中，供主进程收到子进程信息时校验
		*(uint32_t*)(tmp + POSI_SEQUENCE) = htonl((uint32_t)info->sequence);
		int32_t ret = frame_master_process_total_pkg(log, tmp, pkg_size);
		//int32_t ret = frame_master_queue_push(log, frame_master_client_queue,
		//    tmp, pkg_size);
		if (success == ret)
		{
			buf_start += pkg_size;
			remained  -= pkg_size;
		}
		else
		{
			if (0 < ret) //接受队列出了问题，所以要跳出循环
			{
				log->write_file(log_mask_warning, (void*)log,
					"[%s::%s]appand code to queue fail,so break\n", __FILE__,
					__FUNCTION__);
				break;
			}
			else
			{
				log->write_file(log_mask_error, (void*)log,
					"[%s::%s]client(%s:%d) sock(%d) process one total pkg fail\n",
					__FILE__, __FUNCTION__, get_addr(info->peer_addr),
					info->peer_port, info->sock_fd);
				return fail;
			}
		}
	}
	info->recv_bytes = remained;
	if (1 != info->recved)
	{
		info->recved = 1;
	}

	//将未处理的数据移动到首部
	if (buf_start != info->buf && info->recv_bytes > 0)
	{
		memmove(info->buf, buf_start, remained);
	}
	return success;
}
static int32_t frame_master_process_total_pkg(record_t* log, char* buf, int32_t len)
{
    if (NULL == log || NULL == buf || 0 >= len)
    {
        return fail;
    }
    int32_t uin = get_uin(buf);
    int32_t ret;
    process_t* proc = frame_process_get(uin, PROCESS_WORKER);
    if (NULL == proc)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]get worker process return NULL\n", __FILE__,
            __FUNCTION__);
        return fail;
    }
    //打印统计信息
    frame_statis_dump(log);
    frame_statis_begin();

    ret = frame_socket_send(proc->worker_channel[1], buf, len, log);
    if (0 > ret)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]send data(len=%d,uin=%d) to process(pid=%d) fail ret(%d)\n",
            __FILE__, __FUNCTION__, len, uin, proc->pid, ret);
        return fail;
    }
    else
    {
        frame_statis_end();
        log->write_file(log_mask_detail, (void*)log,
            "[%s::%s]dispath pkg count(%d)\n", __FILE__, __FUNCTION__,
            ++frame_process_transport_pkg_count);
        log->write_file(log_mask_detail, (void*)log,
            "[%s::%s]process one pkg(uin=%d,len=%d) to process(pid=%d),"
            "sock(fd=%d),dumpbin:\n", __FILE__, __FUNCTION__, uin, len,
            proc->pid, proc->worker_channel[1]);
        log->dump_bin(log_mask_detail, (void*)log, buf, len);
    }

    return success;
}
static int32_t frame_master_process_sql_pkg(record_t* log, char* buf, int32_t len)
{
    if (NULL == log || NULL == buf || 0 >= len)
    {
        return fail;
    }
    int32_t uin = get_uin(buf);
    int32_t ret;
    process_t* proc = frame_process_get(uin, PROCESS_MYSQL);
    if (NULL == proc)
    {
        log->write_file(log_mask_error, set_void(log),
            "[%s::%s]get sqlexe process return NULL\n", __FILE__,
            __FUNCTION__);
        return fail;
    }

    ret = frame_socket_send(proc->worker_channel[1], buf, len, log);
    if (0 > ret)
    {
        log->write_file(log_mask_error, set_void(log),
            "[%s::%s] uin = %d fd=%d send data(len=%d,uin=%d) to "
			" process(pid=%d) fail ret(%d)\n",
            __FILE__, __FUNCTION__,uin,proc->worker_channel[1],len, 
			uin, proc->pid, ret);
        return fail;
    }
    else
    {
        log->write_file(log_mask_detail, set_void(log),
            "[%s::%s]process one pkg(uin=%d,len=%d) to process(pid=%d),"
            "sock(fd=%d),dumpbin:\n", __FILE__, __FUNCTION__, uin, len,
            proc->pid, proc->worker_channel[1]);
        log->dump_bin(log_mask_detail, set_void(log), buf, len);
    }

    return success;
}
static int32_t frame_master_recv_worker_message(record_t* log, client_info_t* info)
{
    if (NULL == log || NULL == info)
    {
        return fail;
    }
    int32_t offset = info->recv_bytes;
    char* buf_start = info->buf + offset;
    int32_t buf_size = info->buf_size - offset;
    int32_t recved = frame_socket_recv(info->sock_fd, buf_start, buf_size);

    if (0 >= recved)
    {
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]client(%s:%d) sock id(%d) close connection, return"
            " value(%d)\n", __FILE__, __FUNCTION__,
            get_addr(info->peer_addr), info->peer_port, info->sock_fd,
            recved);
        frame_socket_clear(info);
        return fail;
    }
    info->recv_bytes += recved;
    info->last_recv_time = time(NULL);

    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]dump recv client message len(%d):\n", __FILE__,
        __FUNCTION__, info->recv_bytes);
    log->dump_bin(log_mask_detail, (void*)log, info->buf,
        info->recv_bytes);

    frame_master_dispath_client(log, info);

    return success;
}
static int32_t frame_master_dispath_client(record_t* log, client_info_t* info)
{
    if (NULL == log || NULL == info)
    {
        return fail;
    }
    int32_t remained = info->recv_bytes;
    int32_t pkg_size = 0;
    int16_t trans_type = TRANS_TO_CLIENT;
    char* buf_start = &info->buf[0];
    char* tmp = NULL;

    //如果剩余的长度小于最小的头长度，则不需要处理
    if (remained <= (int32_t)MIN_PKG_LEN)
    {
        return success;
    }
    while (1)
    {
        if (remained <= (int32_t)MIN_PKG_LEN)
        {
            break;
        }

        tmp = buf_start;
        pkg_size = get_pkg_len(tmp);
        trans_type = get_trans_type(tmp);
        //收到的包出现了错乱，则只能清空掉整个buf了
        if (pkg_size > remained || pkg_size <= (int32_t)MIN_PKG_LEN)
        {
            log->write_file(log_mask_error, (void*)log,
                "[%s::%s]package size(%d) is invalid, min pkg size(%d),"
                "remained size(%d)\n", __FILE__, __FUNCTION__,
                pkg_size, MIN_PKG_LEN, remained);
            //remained = 0;
            break;
        }
        //将数据发回给客户端
        if (TRANS_TO_CLIENT == trans_type)
        {
            frame_master_client_response(log, tmp, pkg_size);
        }
        else if (TRANS_TO_WORKER == trans_type) //转发给工作进程
        {
            frame_master_process_total_pkg(log, tmp, pkg_size);
        }
        else if (TRANS_TO_SQL == trans_type)
        {
            //TODO
            frame_master_process_sql_pkg(log, tmp, pkg_size);
        }
        else
        {
            log->write_file(log_mask_warning, set_void(log),
                "[%s::%s]unkown trans type(%d),so drop this message(len:%d)\n",
                __FILE__, __FUNCTION__, trans_type, pkg_size);
        }
        //移除处理过的包
        buf_start += pkg_size;
        remained  -= pkg_size;
    }
    info->recv_bytes = remained;
    if (1 != info->recved)
    {
        info->recved = 1;
    }

    //将未处理的数据移动到首部
    if (buf_start != info->buf && info->recv_bytes > 0)
    {
        memmove(info->buf, buf_start, remained);
    }
    return success;
}
static int32_t frame_master_client_response(record_t* log, char* src, int32_t len)
{
    if (NULL == log || NULL == src || 0 >= len)
    {
        return fail;
    }
    //将该socket_fd从nethead中取出
    int32_t sock_fd = get_sock_fd(src);
    //将sequence从nethead中取出
    int32_t seq = get_sequence(src);
    int32_t uin = get_uin(src);
    //找到客户端的信息
    client_info_t* p = frame_socket_get_client(sock_fd, 0);
    if (NULL == p)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]get client fd(%d) return fail\n", __FILE__, __FUNCTION__);
        return fail;
    }
    //校验是否是相同的seq，有可能该fd是重新accept的
    if (p->sequence != seq)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]client seq(%d) is not equal msg seq(%d)\n", __FILE__,
            __FUNCTION__, p->sequence, seq);
        return fail;
    }
    //所有校验完成之后，就开始发送消息
    //测试一下，所以暂时屏蔽
    int32_t ret = frame_socket_send(sock_fd, src, len, log);
    //int32_t ret = 0;
    if (0 > ret)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]send data(uin=%d,len=%d) to client(%s:%d) fail ret(%d)\n",
            __FILE__, __FUNCTION__, uin, len, get_addr(p->peer_addr),
            p->peer_port, ret);
        return fail;
    }
    else
    {
        log->write_file(log_mask_detail, (void*)log,
            "[%s::%s]process one pkg(uin=%d,len=%d) to client(%s:%d),"
            "sock(fd=%d),dumpbin:\n", __FILE__, __FUNCTION__, uin, len,
            get_addr(p->peer_addr), p->peer_port, sock_fd);
        log->dump_bin(log_mask_detail, (void*)log, src, len);
    }
    return success;
}
//static int32_t frame_master_queue_push(record_t* log, code_queue_t* queue, char* buf, int32_t len)
//{
//    if (NULL == log || NULL == queue || NULL == buf || 0 >= len)
//    {
//        return -1;
//    }
//    int32_t ret = queue->append((void*)queue, buf, len);
//    if (0 > ret)
//    {
//        log->write_file(log_mask_error, (void*)log,
//            "[%s::%s]append to code queue fail, val(%d)\n", __FILE__,
//            __FUNCTION__, ret);
//        return 1;
//    }
//    return success;
//}
//static function end
//////////////////////////////////////////////////////////////////////////
int32_t frame_master_spawn_process(record_t* log, frame_process_spawn_proc proc, uint16_t id, void* data, void* record, int16_t proc_type)
{
    int32_t pid;
    uint16_t s;
    int32_t ret;
    process_t* process = &frame_process_col[0];
    //找到第一个可用的位置
    for (s = 0; s < frame_process_count; ++s, ++process)
    {
        if (0 >= process->pid)
        {
            break;
        }
    }
    if (s >= MAX_PROCESS_COUNT)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]no more than %d process can be spawn\n", __FILE__,
            __FUNCTION__, MAX_PROCESS_COUNT);
        return -1;
    }
    frame_process_slot = s;
    process = &frame_process_col[s];
    process->proc_type = proc_type;
    ret = frame_socket_create_socketpair(log, process->worker_channel);
    if (success != ret)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]create worker recv channel socketpair return(%d)\n",
            __FILE__, __FUNCTION__, ret);
        return -2;
    }
    /*
    ret = frame_socket_create_socketpair(log, process->master_channel);
    if (success != ret)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]create master recv channel socketpair return(%d)\n",
            __FILE__, __FUNCTION__, ret);
        return -3;
    }*/
    //开始创建子进程
    pid = fork();
    switch (pid)
    {
    case -1:
        log->write_file(log_mask_error, (void*)log, 
            "[%s::%s]fork() failed\n", __FILE__, __FUNCTION__);
        return -4;
        break;
    case 0: //子进程运行
        frame_process_id = getpid();
        proc(id, data, record);
        break;
    default:
        log->write_file(log_mask_detail, (void*)log, 
            "[%s::%s]master process continue\n", __FILE__, __FUNCTION__);
        break;
    }
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]start worker(%d) pid(%u)\n", __FILE__, __FUNCTION__,
        id, pid);
    process->pid = pid;
    process->exited = 0;

    if (s == frame_process_count)
    {
        ++frame_process_count;
    }
    return pid;
}
void frame_master_exit(record_t* log)
{
    //关掉所有的socket监听
    frame_socket_exit();
    //关闭掉管道
    //frame_codequeue_destroy(frame_master_client_queue);
    //frame_codequeue_destroy(frame_master_worker_queue);

    frame_process_signal_worker(log, SIGUSR2);
}

int32_t frame_master_init( record_t* log )
{
    uint16_t i;
    client_info_t* ci = NULL;
    process_t* p = &frame_process_col[0];
    frame_master_events = frame_socket_get_event();
    //创建数据收发管道
    /*
    frame_master_client_queue = frame_codequeue_create(MAX_CODE_QUEUE_SIZE);
    frame_master_worker_queue = frame_codequeue_create(MAX_CODE_QUEUE_SIZE);
    if (NULL == frame_master_client_queue
        || NULL == frame_master_worker_queue)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]create client queue(%p) or worker_queue(%p) fail\n",
            __FILE__, __FUNCTION__, frame_master_client_queue,
            frame_master_worker_queue);
        exit(EXIT_FAILURE);
    }
    */
    for (i=0; i < frame_process_count; ++i, ++p)
    {
        if (0 > p->pid)
        {
            continue;
        }
        //监听从子进程发送过来的信息
        ci = frame_socket_get_client(p->worker_channel[1], 
            MAX_WORKER_NET_LEN);
        if (NULL != ci)
        {
            ci->sock_type = SOCK_TYPE_RECV;
            if (fail == frame_socket_epoll_ctl(ci->sock_fd, EPOLL_ADD))
            {
                log->write_file(log_mask_error, (void*)log,
                    "[%s::%s]add sock(%d) to epoll event fail\n",
                    __FILE__, __FUNCTION__, ci->sock_fd);
                return fail;
            }
            log->write_file(log_mask_detail, (void*)log,
                "[%s::%s]add sock(%d),type(SOCK_TYPE_RECV) to epoll event"
                " success\n", __FILE__, __FUNCTION__, ci->sock_fd);
        }
        else
        {
            log->write_file(log_mask_error, (void*)log,
                "[%s::%s]get client(%d: %d) return NULL\n", __FILE__,
                __FUNCTION__, p->worker_channel[1], ci->sock_fd);
            return fail;
        }
        //关闭掉不用的管道
        log->write_file(log_mask_detail, set_void(log),
            "[%s::%s]close %s process no used channel(0:%d)\n", __FILE__,
            __FUNCTION__, (PROCESS_MYSQL==p->proc_type ? "sqlexe":"worker"),
            p->worker_channel[0]);
        if (0 == close(p->worker_channel[0]))
        {
            p->worker_channel[0] = -1;
        }
        else
        {
            log->write_file(log_mask_warning, set_void(log),
                "[%s::%s]pid(%d) close %s process channel(1) fail,"
                "errno(%d),msg(%s)\n", __FILE__, __FUNCTION__, p->pid,
                (PROCESS_MYSQL==p->proc_type?"sqlexe":"worker"), errno,
                strerror(errno));
        }
    }
    return success;
}

int32_t frame_master_process( record_t* log )
{
    int32_t timeout = 1;
    int32_t count = 0;
    int32_t sock_fd = -1;
    struct epoll_event* pev = NULL;
    client_info_t* cur = NULL;
    int32_t i;

    count = epoll_wait(frame_master_events->epoll_fd,
        frame_master_events->events, frame_master_events->max_fd, timeout);
    if (0 > count)
    {
        return fail;
    }
    pev = frame_master_events->events;
    for (i = 0; i < count; ++i,++pev)
    {
        sock_fd = pev->data.fd;
        if (sock_fd < 0
            || sock_fd >= frame_master_events->max_fd)
        {
            log->write_file(log_mask_warning, (void*)log,
                "[%s::%s]recv fd(%d) invalid,max fd count(%d)\n",
                __FILE__, __FUNCTION__, sock_fd, 
                frame_master_events->max_fd);
            continue;
        }
        cur = frame_socket_get_client(sock_fd, 0);

        if (0 != (EPOLLERR & pev->events))
        {
            int err = 0;
            socklen_t len = (socklen_t)sizeof(err);
            getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &err, &len);
            log->write_file(log_mask_warning, (void*)log,
                "[%s::%s]client[ip:%s,port:%d] socket(%d) errno(%d) "
                "recv epoll error, so closed and clear the socket info\n",
                __FILE__, __FUNCTION__, get_addr(cur->peer_addr),
                cur->peer_port, sock_fd, err);

            frame_socket_clear(cur);
            continue;
        }
        if (0 == (EPOLLIN & pev->events))
        {
            continue;
        }
        //如果是监听端口
        if (SOCK_TYPE_LISTEN == cur->sock_type)
        {
            frame_socket_accept(log, sock_fd);
        }
        //主进程收到子进程处理完的数据
        else if (SOCK_TYPE_RECV == cur->sock_type)
        {
            //do something send
            log->write_file(log_mask_detail, (void*)log,
                "[%s::%s]recv the worker process(fd=%d) message\n",
                __FILE__, __FUNCTION__, cur->sock_fd);
            frame_master_recv_worker_message(log, cur);
        }
        else if (SOCK_TYPE_COMMON == cur->sock_type)
        {
            //开始接收数据
            frame_master_recv(log, cur);
        }
    }

    return success;
}

int32_t frame_master_dispath_worker( record_t* log )
{
    if (NULL == log)
    {
        return fail;
    }

    return success;
}

int32_t frame_master_check_exit()
{
    int16_t sql_cnt = 0;
    int16_t worker_cnt = 0;
    uint16_t i;
    for (i=0; i < frame_process_count; ++i)
    {
        process_t* pro = &frame_process_col[i];
        //如果还有子进程没有退出，则主进程不能退出
        if (0 < pro->pid)
        {
            if (PROCESS_MYSQL == pro->proc_type)
            {
                ++sql_cnt;
            }
            else if (PROCESS_WORKER == pro->proc_type)
            {
                ++worker_cnt;
            }
        }
    }

    //只有全部都停止了，主进程才停止
    if (0 == sql_cnt && 0 == worker_cnt)
    {
        frame_common_delete_pid_file("");
        exit(0);
    }
    //有一种类型的进程已经全部挂掉了，则需要通知其它的子进程要停止了
    else if ((0 == sql_cnt || 0 == worker_cnt) 
        && SIGUSR2 != frame_process_last_sig)
    {
        frame_process_signal_worker(NULL, SIGUSR2);
    }
    //没有其它的else了
    return success;
}

