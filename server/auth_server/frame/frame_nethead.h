#pragma once

#include "frame_common.h"
/**
*整个协议头(nethead)布局：
*pkg_len(int32_t) + uin(int32_t) + sock_fd(int32_t)
*+sequence(int32_t) + msg_id(int16_t) 
*/
//将数据发送给哪种进程
#define SEND_TO_MASTER  0
#define SEND_TO_SIBLING 1
#define TRANS_TO_CLIENT   0  //发送给客户端
#define TRANS_TO_WORKER   1  //发送给工作进程
#define TRANS_TO_SQL      2  //发送给sql执行进程
typedef struct _net_head
{
    int32_t pkg_len;
    int32_t uin;
    int32_t sock_fd;
    int32_t sequence;
    int16_t msg_id;
    int16_t trans_type;     //主进程转发子进程消息类别
} net_head_t;

#define MIN_PKG_LEN   sizeof(int32_t)

#define POSI_UIN      sizeof(int32_t)
#define POSI_SOCK_FD  (POSI_UIN + sizeof(int32_t))
#define POSI_SEQUENCE (POSI_SOCK_FD + sizeof(int32_t))
#define POSI_MSG_ID   (POSI_SEQUENCE + sizeof(int32_t))
#define POSI_TRANS_TYPE (POSI_MSG_ID + sizeof(int16_t))
#define POSI_MSG_BODY (POSI_TRANS_TYPE + sizeof(int16_t))
#define NET_HEAD_LEN  POSI_MSG_BODY

#define get_pkg_len(src) ((int32_t)ntohl(*((uint32_t*)(src))))
#define get_uin(src) ((int32_t)ntohl(*((uint32_t*)(src + POSI_UIN))))
#define get_sock_fd(src) ((int32_t)ntohl(*((uint32_t*)(src + POSI_SOCK_FD))))
#define get_sequence(src) ((int32_t)ntohl(*((uint32_t*)(src + POSI_SEQUENCE))))
#define get_msg_id(src) ((int16_t)ntohs(*((uint16_t*)(src + POSI_MSG_ID))))
#define get_trans_type(src) ((int16_t)ntohs(*((uint16_t*)(src + POSI_TRANS_TYPE))))

int32_t frame_nethead_encode(char* src, const  net_head_t* head);
int32_t frame_nethead_decode(const char* src, net_head_t* head);
