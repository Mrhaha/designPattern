#pragma once

#include "frame_common.h"
#include "frame_log.h"
#include "frame_time.h"
#if defined(_DEBUG) || defined(DEBUG)
#define LISTEN_COUNT   8
#define MAX_SOCK_COUNT 0x00f0
#define MAX_NET_BUF_LEN 0x8000
#define MAX_WORKER_NET_LEN 0x4000
#else
#define LISTEN_COUNT   256
#define MAX_SOCK_COUNT 0xfff0
#define MAX_NET_BUF_LEN 0x2000000   //32M
#define MAX_WORKER_NET_LEN 0x400000
#endif

#define MAX_CLIENT_PKG_LEN 5120     //客户端一次性发送包最大长度

#define SOCK_FD_INVALID -1
#define SOCK_TYPE_INIT  -1
#define SOCK_TYPE_LISTEN 0  //监听
#define SOCK_TYPE_COMMON 1  //普通客户端
#define SOCK_TYPE_RECV   2  //主进程接收子进程信息
#define SOCK_TYPE_SEND   3  //主进程发送信息给子进程

#define EPOLL_ADD     EPOLL_CTL_ADD
#define EPOLL_MOD     EPOLL_CTL_MOD
#define EPOLL_DEL     EPOLL_CTL_DEL

#define SOCK_CHECK_INTERVAL   10
#define SOCK_UNALIVE_TIME     90
#define WAIT_FIRST_PKG        30

typedef struct _event
{
    int32_t max_fd;
    int32_t sock_fd;
    int32_t epoll_fd;
    struct epoll_event* events;
    struct epoll_event ev;
}epoll_event_t;
//主进程保存客户端信息
typedef struct _sock_info
{
    char*    buf;
    uint32_t buf_size;
    int32_t  sock_fd;
    uint32_t peer_addr;
    uint32_t recv_bytes;
    uint32_t create_time;
    uint32_t last_recv_time;
    int32_t  sequence;
    uint16_t peer_port;
    int16_t  sock_type;
    int8_t   sended;
    int8_t   recved;
    int8_t   used;
}client_info_t;
//工作进程接收主进程发送的信息
typedef struct _worker_info
{
    char*    buf;
    uint32_t buf_size;
    int32_t  sock_fd;
    uint32_t recv_bytes;
} worker_recv_t;

#define set_nonblocking(s) fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK)
#define get_addr(s) (inet_ntoa(*(struct in_addr*)&s))

void frame_socket_close_channel(record_t* log, int32_t* fd);
void frame_socket_client_destroy();
void frame_socket_exit();

int32_t frame_socket_set_buffer_size(int32_t fd, size_t size);
int32_t frame_socket_create_socketpair(record_t* log, int32_t* pairs);
int32_t frame_socket_svrd_init(record_t* log, char* ip, uint16_t port, uint16_t max_fd);
int32_t frame_socket_epoll_ctl(int32_t fd, int32_t ctl);
client_info_t* frame_socket_get_client(int32_t fd, int32_t size);
epoll_event_t* frame_socket_get_event();
int32_t frame_socket_clear(client_info_t* info);
int32_t frame_socket_client_reset(client_info_t* info);
int32_t frame_socket_accept(record_t* log, int32_t fd);
int32_t frame_socket_recv(int32_t fd, char* buf, int32_t size);
int32_t frame_socket_send(int32_t fd, char* buf, int32_t size, record_t* log);
int32_t frame_socket_check_timeout( record_t* log, int32_t interval, int32_t unalive, int32_t first_pkg);
