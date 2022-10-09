#include "frame_socket.h"
#include "frame_process.h"
static epoll_event_t frame_socket_event;
static client_info_t *frame_socket_client_info = NULL;
static time_t frame_socket_last_check = 0;
static int32_t frame_socket_seq = 0;

static int32_t frame_socket_epoll_init(uint16_t max);
static int32_t frame_socket_create();
static int32_t frame_socket_client_info_init(uint16_t max);
static void frame_socket_event_close();
static void frame_socket_setopt(int32_t fd);

static int32_t frame_socket_epoll_init(uint16_t max)
{
    if (NULL != frame_socket_event.events)
    {
        return success;
    }
    frame_socket_event.max_fd = max;
    memset(&frame_socket_event.ev, 0, sizeof(frame_socket_event.ev));
    frame_socket_event.ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    frame_socket_event.ev.data.ptr = NULL;
    frame_socket_event.ev.data.fd = -1;

    frame_socket_event.events = (struct epoll_event*) malloc(max * sizeof(struct epoll_event));
    if (NULL == frame_socket_event.events)
    {
        return -1;
    }
    frame_socket_event.epoll_fd = epoll_create(max);
    if (0 > frame_socket_event.epoll_fd)
    {
        return -2;
    }
    return success;
}
static void frame_socket_event_close()
{
    if (0 < frame_socket_event.sock_fd)
    {
        close(frame_socket_event.sock_fd);
        frame_socket_event.sock_fd = -1;
    }
    if (NULL != frame_socket_event.events)
    {
        free(frame_socket_event.events);
        frame_socket_event.events = NULL;
    }
    if (0 < frame_socket_event.epoll_fd)
    {
        close(frame_socket_event.epoll_fd);
        frame_socket_event.epoll_fd = -1;
    }
    return;
}
static int32_t frame_socket_create()
{
    int32_t fd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > fd)
    {
        return -1;
    }
    if (0 > set_nonblocking(fd))
    {
        close(fd);
        return -2;
    }
    return fd;
}
static void frame_socket_setopt(int32_t fd)
{
    if (0 > fd)
    {
        return;
    }
    int flag = 1;
    struct linger ling = {0, 0};
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
        &flag, sizeof(flag));
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
        &flag, sizeof(flag));
    setsockopt(fd, SOL_SOCKET, SO_LINGER,
        &ling, sizeof(ling));
    frame_socket_set_buffer_size(fd, MAX_NET_BUF_LEN);
    return ;
}
static int32_t frame_socket_client_info_init(uint16_t max)
{
    if (0 >= max)
    {
        return fail;
    }
    if (NULL != frame_socket_client_info)
    {
        return success;
    }
    size_t size = max * sizeof(client_info_t);
    frame_socket_client_info = (client_info_t *) malloc(size);
    if (NULL == frame_socket_client_info)
    {
        return fail;
    }
    memset(frame_socket_client_info, 0, size);
    return success;
}
//static function end
//////////////////////////////////////////////////////////////////////////
void frame_socket_close_channel(record_t* log, int32_t* fd)
{
    if (NULL == log || NULL == fd)
    {
        return;
    }
    if (-1 == close(fd[0]))
    {
        log->write_file(log_mask_warning, (void*)log, 
            "[%s::%s]close(%d) channal 0 failed\n", __FILE__,
            __FUNCTION__, fd[0]);
    }
    if (-1 == close(fd[1]))
    {
        log->write_file(log_mask_warning, (void*)log, 
            "[%s::%s]close(%d) channal 1 failed\n", __FILE__,
            __FUNCTION__, fd[1]);
    }
}
int32_t frame_socket_set_buffer_size( int32_t fd, size_t size )
{
    if (0 > fd)
    {
        return fail;
    }
    size_t recv_size = size;
    size_t send_size = size;
    socklen_t len = (socklen_t) sizeof(recv_size);
    if (0 != setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &recv_size, len))
    {
        fprintf(stderr, "[%s::%s]setsockopt SO_RCVBUF size(%ld) fail,"
            "errno(%d),msg(%s)\n", __FILE__, __FUNCTION__, recv_size,
            errno, strerror(errno));
        return fail;
    }

    len = (socklen_t) sizeof(send_size);
    if (0 != setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &send_size, len))
    {
        fprintf(stderr, "[%s::%s]setsockopt SO_SNDBUF size(%ld) fail,"
            "errno(%d),msg(%s)\n", __FILE__, __FUNCTION__, send_size,
            errno, strerror(errno));
        return fail;
    }

    return success;
}

int32_t frame_socket_create_socketpair(record_t* log, int32_t* pairs)
{
    if (NULL == pairs || NULL == log)
    {
        return -5;
    }
    u_long on;

    //开始创建本地socket
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pairs) == -1)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]socketpair failed while spawning errno(%d),msg(%s)\n",
            __FILE__, __FUNCTION__, errno, strerror(errno));
        return -1;
    }

    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]channel 0:%d, 1:%d\n", __FILE__, __FUNCTION__,
        pairs[0], pairs[1]);
    //将socket设置为非阻塞
    if (-1 == set_nonblocking(pairs[0]))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]failed while set socket(%d) nonblocking\n",
            __FILE__, __FUNCTION__, pairs[0]);
        frame_socket_close_channel(log, pairs);
        return -2;
    }

    if (-1 == set_nonblocking(pairs[1]))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]failed while set socket(%d) nonblocking\n",
            __FILE__, __FUNCTION__, pairs[1]);
        frame_socket_close_channel(log, pairs);
        return -3;
    }
    //设置buf长度
    frame_socket_set_buffer_size(pairs[0], MAX_NET_BUF_LEN);
    frame_socket_set_buffer_size(pairs[1], MAX_NET_BUF_LEN);

    on = 1;
    //使用异步I/O
    if (-1 == ioctl(pairs[0], FIOASYNC, &on))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]ioctl(%d,FIOASYNC) failed while spawning\n",
            __FILE__, __FUNCTION__, pairs[0]);
        frame_socket_close_channel(log, pairs);
        return -4;
    }
    if (-1 == ioctl(pairs[1], FIOASYNC, &on))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]ioctl(%d,FIOASYNC) failed while spawning\n",
            __FILE__, __FUNCTION__, pairs[1]);
        frame_socket_close_channel(log, pairs);
        return -5;
    }
    return success;
}

int32_t frame_socket_svrd_init( record_t* log, char* ip, uint16_t port, uint16_t max_fd )
{
    if (NULL == log || 0 >= max_fd)
    {
        return fail;
    }
    struct sockaddr_in addr;
    //初始化客户信息
    if (fail == frame_socket_client_info_init(max_fd))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]client info init count(%d),size(%ld) fail\n",
            __FILE__, __FUNCTION__, max_fd, 
            (max_fd*sizeof(client_info_t)));
        goto error_step;
    }

    int32_t ret = frame_socket_epoll_init(max_fd);
    if (0 > ret)
    {
        log->write_file(log_mask_error, (void*)log, 
            "[%s::%s]epoll init fail,return value(%d)\n", __FILE__,
            __FUNCTION__, ret);
        goto error_step;
    }
    //创建
    ret = frame_socket_create();
    if (0 > ret)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]socket create fail,return value(%d)\n", __FILE__,
            __FUNCTION__, ret);
        goto error_step;
    }
    frame_socket_event.sock_fd = ret;
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]create epoll fd(%d),socket fd(%d)\n", __FILE__,
        __FUNCTION__, frame_socket_event.epoll_fd, ret);

    frame_socket_setopt(frame_socket_event.sock_fd);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (0 >= (int32_t)strlen(ip))
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        inet_pton(AF_INET, ip, &addr.sin_addr);
    }
    //bind
    ret = bind(frame_socket_event.sock_fd, (struct sockaddr*) &addr,
        sizeof(addr));
    if (0 > ret)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]bind svrd(ip:%s,port:%d) fail,errno(%d),msg(%s)\n",
            __FILE__, __FUNCTION__, ip, port, errno, strerror(errno));
        goto error_step;
    }
    //listen
    ret = listen(frame_socket_event.sock_fd, LISTEN_COUNT);
    if (0 > ret)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]listen(%d) fail\n", __FILE__, __FUNCTION__,
            LISTEN_COUNT);
        goto error_step;
    }
    //将数据加入到epoll队列
    if (fail == frame_socket_epoll_ctl(frame_socket_event.sock_fd,
        EPOLL_ADD))
    {
        log->write_file(log_mask_error, (void*)log, 
            "[%s::%s]add epoll event fail\n", __FILE__, __FUNCTION__);
        goto error_step;
    }
    //将监听fd加入到数组中
    client_info_t* p = frame_socket_get_client(frame_socket_event.sock_fd, 0);
    if (NULL == p)
    {
        log->write_file(log_mask_error, (void*)log, 
            "[%s::%s]add client array fail\n", __FILE__, __FUNCTION__);
        goto error_step;
    }
    p->create_time = time(NULL);
    p->sock_type = SOCK_TYPE_LISTEN;

    return success;

error_step:
    frame_socket_event_close();
    frame_process_signal_worker(log, SIGUSR2);
    return fail;
}

int32_t frame_socket_epoll_ctl(int32_t fd, int32_t ctl)
{
    if (0 > fd)
    {
        return fail;
    }
    frame_socket_event.ev.data.fd = fd;
    if (0 > epoll_ctl(frame_socket_event.epoll_fd, ctl, fd,
        &frame_socket_event.ev))
    {
        return fail;
    }
    return success;
}
void frame_socket_client_destroy()
{
    if (NULL != frame_socket_client_info)
    {
        uint16_t i;
        client_info_t* p = frame_socket_client_info;
        for (i=0; i < frame_socket_event.max_fd; ++i,++p)
        {
            if (NULL !=p->buf)
            {
                free(p->buf);
                p->buf = NULL;
            }
        }
        free(frame_socket_client_info);
        frame_socket_client_info = NULL;
    }
}

client_info_t* frame_socket_get_client( int32_t fd, int32_t size )
{
    if (0 > fd || fd >= frame_socket_event.max_fd)
    {
        return NULL;
    }
    client_info_t* p = NULL;
    p = frame_socket_client_info + fd;
    //如果本来就存在
    if (1 == p->used)
    {
        return p;
    }
    //只有当buf为NULL，并且size不为0时，才能申请内存
    if (NULL == p->buf && 0 != size)
    {
        if (0 > size)
        {
            p->buf_size = MAX_CLIENT_PKG_LEN;
        }
        else
        {
            p->buf_size = size;
        }
        p->buf = (char*) malloc(p->buf_size);
        if (NULL == p->buf)
        {
            return NULL;
        }
    }
    p->sock_fd = fd;
    p->used = 1;
    p->sequence = ++frame_socket_seq;
    if (0 > frame_socket_seq)
    {
        frame_socket_seq = 0;
    }
    return p;
}

void frame_socket_exit()
{
    frame_socket_client_destroy();
    frame_socket_event_close();
}

epoll_event_t* frame_socket_get_event()
{
    return &frame_socket_event;
}

int32_t frame_socket_clear( client_info_t* info )
{
    if (NULL == info)
    {
        return fail;
    }
    if (0 < info->sock_fd)
    {
        frame_socket_epoll_ctl(info->sock_fd, EPOLL_DEL);
        close(info->sock_fd);
    }
    frame_socket_client_reset(info);
    return success;
}
int32_t frame_socket_client_reset(client_info_t* info)
{
    if (NULL == info)
    {
        return fail;
    }
    info->peer_addr = 0;
    info->peer_port = 0;
    info->recv_bytes = 0;
    info->sock_fd = SOCK_FD_INVALID;
    info->create_time = 0;
    info->last_recv_time = 0;
    info->sock_type = SOCK_TYPE_INIT;
    info->used = 0;
    info->sended = 0;
    info->recved = 0;
    info->sequence = -1;

    return success;
}

int32_t frame_socket_accept( record_t* log, int32_t fd )
{
    if (NULL == log || 0 > fd)
    {
        return fail;
    }
    int32_t accp_fd = -1;
    client_info_t* ci = NULL;
    struct sockaddr_in addr;
    socklen_t len = (socklen_t)sizeof(addr);

    accp_fd = accept(fd, (struct sockaddr*) &addr, &len);
    if (0 >= accp_fd)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]accept client(%s:%d) error,errno(%d),msg(%s)\n",
            __FILE__, __FUNCTION__, inet_ntoa(addr.sin_addr),
            ntohs(addr.sin_port), errno, strerror(errno));
        return fail;
    }
    if (accp_fd >= frame_socket_event.max_fd)
    {
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]connection max(%d), so refused\n", __FILE__,
            __FUNCTION__, frame_socket_event.max_fd);
        close(accp_fd);
        return fail;
    }
    if (0 > set_nonblocking(accp_fd))
    {
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]set socket(%d) nonblocking fail\n", __FILE__,
            __FUNCTION__, accp_fd);
        return fail;
    }
    if (fail == frame_socket_epoll_ctl(accp_fd, EPOLL_ADD))
    {
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]socket(%d) add epoll event fail\n", __FILE__,
            __FUNCTION__, accp_fd);
        return fail;
    }
    ci = frame_socket_get_client(accp_fd, -1);
    if (NULL != ci)
    {
        ci->sock_type = SOCK_TYPE_COMMON;
        ci->used = 1;
        ci->sended = 0;
        ci->recved = 0;
        ci->peer_addr = addr.sin_addr.s_addr;
        ci->peer_port = ntohs(addr.sin_port);
        ci->create_time = time(NULL);
        ci->last_recv_time = ci->create_time;
    }
    else
    {
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]socket(%d) get client info return NULL\n",
            __FILE__, __FUNCTION__, accp_fd);
        return fail;
    }
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]established new connection client(%s:%d),socket id(%d)\n",
        __FILE__, __FUNCTION__, inet_ntoa(addr.sin_addr), ci->peer_port,
        accp_fd);

    return success;
}

int32_t frame_socket_recv( int32_t fd, char* buf, int32_t size )
{
    if ( NULL == buf || size <= 0)
    {
        return -1;
    }

    ssize_t recved = 0;
    while (1)
    {
        recved = recv(fd, buf, size, 0);
        if (0 < recved)
        {
            return recved;
        }
        else
        {
            if (0 > recved && 
                //当errno为EINTR，或者为EAGAIN时，要继续操作
                (errno == EINTR || errno == EAGAIN))
            {
                continue;
            }
            //当返回值为0时，表示对端已经关闭了链接
            return recved;
        }
    }
    return -2;
}

int32_t frame_socket_send( int32_t fd, char* buf, int32_t size, record_t* log )
{
    if (NULL == buf || 0 >= fd || 0 >= size)
    {
        return -1;
    }
    int32_t remained = size;
    ssize_t sended = 0;
    int32_t s_count = 0;
    char* tmp = buf;

    while (remained > 0)
    {
        sended = send(fd, tmp, (size_t)remained, 0);
        if (sended > 0)
        {
            tmp += sended;
            remained -= sended;
        }
        else
        {
            //如果errno是EINTR，或者是EAGAIN，则可以继续
            if (0 > sended 
                && (errno == EINTR || errno == EAGAIN))
            {
                continue;
            }
            else
            {
                if (NULL != log)
                {
                    log->write_file(log_mask_warning, (void*)log,
                        "[%s::%s]send data(size:%d) error(no:%d), msg(%s)\n",
                        __FILE__, __FUNCTION__, remained, errno,
                        strerror(errno));
                }
                //break;
            }
            return sended;
        }
        s_count++;
    }
    if (s_count > 1 && remained == 0 && NULL != log)
    {
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]fd(%d) send data bytes(%d), need %d times\n",
            __FILE__, __FUNCTION__, fd, size, s_count);
    }

    return (size - remained);
}

int32_t frame_socket_check_timeout( record_t*log, int32_t interval, int32_t unalive, int32_t first_pkg )
{
    if (NULL == log || NULL == frame_socket_client_info)
    {
        return fail;
    }
    if (0 >= interval)
    {
        interval = SOCK_CHECK_INTERVAL;
    }
    if (0 >= unalive)
    {
        unalive = SOCK_UNALIVE_TIME;
    }
    if (0 >= first_pkg)
    {
        first_pkg = WAIT_FIRST_PKG;
    }
    char create_date[MAX_PATH_LEN] = {0};
    char last_date[MAX_PATH_LEN] = {0};
    time_t now = time(NULL);
    client_info_t* p = frame_socket_client_info;
    uint16_t i;
    if (frame_socket_last_check <= 0)
    {
        frame_socket_last_check = now;
    }
    if (interval < (now - frame_socket_last_check))
    {
        return success;
    }

    for (i = 0; i < frame_socket_event.max_fd; ++i, ++p)
    {
        if (SOCK_TYPE_LISTEN == p->sock_type
            || SOCK_TYPE_RECV == p->sock_type
            || SOCK_TYPE_INIT == p->sock_type
            || 0 >= p->sock_fd)
        {
            continue;
        }
        frame_time_get_datetime(p->create_time, create_date,
            sizeof(create_date));
        frame_time_get_datetime(p->last_recv_time, last_date,
            sizeof(last_date));
        if (1 == p->recved) //如果有收到client数据包
        {
            if (unalive <= (now - p->last_recv_time) )
            {
                log->write_file(log_mask_warning, (void*)log,
                    "[%s::%s]client(%s:%d) sock fd(%d) on idx(%d) create "
                    "time(%s),last recv time(%s) was  timeout, so clear it\n",
                    __FILE__, __FUNCTION__, get_addr(p->peer_addr),
                    p->peer_port, p->sock_fd, i, create_date, last_date);
                frame_socket_clear(p);
            }
        }
        else  //一直没有收到client数据包
        {
            if (first_pkg <= (now - p->create_time))
            {
                log->write_file(log_mask_warning, (void*)log,
                    "[%s::%s]client(%s:%d) sock fd(%d) on idx(%d) create "
                    "time(%s),last recv time(%s) wait first pkg timeout, "
                    "so clear it\n", __FILE__, __FUNCTION__,
                    get_addr(p->peer_addr), p->peer_port, p->sock_fd, i,
                    create_date, last_date);
                frame_socket_clear(p);
            }
        }
    }
    frame_socket_last_check = now;
    return success;
}
