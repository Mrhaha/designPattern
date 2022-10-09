#pragma once

#include <stdio.h>
#include "common_base.h"


//IPv4的一些基本封装

#ifdef __cplusplus
extern "C"
{
#endif

int32_t socket_address_to_string(struct sockaddr_in& stAddress, char* pszIPPort, size_t size);

int32_t set_non_block(int32_t fd);

//设置socket的接收&发送缓冲区大小.特别注意的是设置socket缓冲区大小的时机
//对于client来说:必须在调用connect函数前进行对接收缓冲区的设置
//对于server来说:必须在调用listen函数前进行对接收缓冲区的设置
int32_t set_socket_buffer_size(int32_t fd, int32_t tcp_buffer_size);

//获取本地网卡上所有地址信息
int32_t get_nic_addrs(struct sockaddr_in* pstNicAddrs, int32_t& count);

//网络字节序的IP值和点十格式的IP字符串之间进行转换
int32_t ip_string_to_addr(const char* string_ip, uint32_t& ip);
int32_t ip_addr_to_string(uint32_t ip, char* string_ip, size_t buflen);

//msecond的单位: ms
int32_t connect_nonblock(int fd, struct sockaddr_in* serv_addr, socklen_t addrlen, int32_t msecond);

//ip 网络字节序
bool intranet_ip(uint32_t ip);

#ifdef __cplusplus
}
#endif






