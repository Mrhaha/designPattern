#include <fcntl.h>
#include <sys/ioctl.h>

#include <sys/param.h>
#include <sys/cdefs.h>
#include <poll.h>

#include "common_ipv4.h"


int32_t ip_string_to_addr(const char* string_ip, uint32_t& ip)
{
	struct in_addr stAddr;
	if(!string_ip || inet_pton(AF_INET, string_ip, &stAddr) <=0)
	{
		return -1;
	}

	ip = stAddr.s_addr;
	return 0;
}

int32_t ip_addr_to_string(uint32_t ip, char* string_ip, size_t buflen)
{
	if (NULL == string_ip || buflen < INET_ADDRSTRLEN)
	{
		return -1;
	}

	if (NULL == inet_ntop(AF_INET, &ip, string_ip, buflen))
	{
		return -1;
	}

	return 0;
}

int32_t socket_address_to_string(struct sockaddr_in& stAddress, char* pszIPPort, size_t size)
{
	if (NULL == pszIPPort || size < 22)
	{
		return -1;
	}

	char* pszTmpIP = NULL;
	pszTmpIP = inet_ntoa(stAddress.sin_addr);
	if( NULL == pszTmpIP)
	{
		return -1;
	}

	uint16_t port = ntohs(stAddress.sin_port);
	snprintf(pszIPPort, size-1, "%s:%d", pszTmpIP, port);
	pszTmpIP[size-1] = '\0';

	return 0;
}

int32_t set_non_block(int32_t fd)
{
	int flag = 0;
	flag = fcntl(fd, F_GETFL, 0);
	if (flag < 0)
	{
		return -1;
	}

	flag |= O_NONBLOCK;
	flag |= O_NDELAY;
	
	if (fcntl(fd, F_SETFL, flag) < 0)
	{
		return -1;
	}

	return 0;
}

int32_t set_socket_buffer_size(int32_t fd, int32_t socket_buffer_size)
{	
	if (fd < 0 )
	{
		return -1;
	}

	int result = 0;
	socklen_t opt_length = (socklen_t)sizeof(socket_buffer_size);

	result = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &socket_buffer_size, opt_length);
	if(0 != result)
	{
		return -1;
	}

	result = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &socket_buffer_size, opt_length);
	if (0 != result)
	{
		return -1;
	}

	return 0;
}

int32_t get_nic_addrs( struct sockaddr_in* pstNicAddrs, int32_t& count )
{
	if (NULL == pstNicAddrs || 0 >= count)
	{
		return -1;
	}

	int32_t fd = -1;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		return -1;
	}

	struct ifconf nicConfig;
	struct ifreq astNics[1024];

	nicConfig.ifc_buf = (char*)&astNics[0];
	nicConfig.ifc_len = sizeof(astNics);

	int32_t ret = -1;
	ret = ioctl(fd, SIOCGIFCONF, &nicConfig);
	if (-1 == ret)
	{
		close(fd);
		return -1;
	}

	int32_t tmpCount = int32_t(nicConfig.ifc_len/sizeof(ifreq));
	if(tmpCount < count)
	{
		count = tmpCount;
	}

	for(int32_t i = 0; i < count; ++i)
	{
		memcpy(&pstNicAddrs[i], &(astNics[i].ifr_addr), sizeof(struct sockaddr_in));
	}

	close(fd);
	
	return 0;
}

int32_t connect_nonblock(int fd, struct sockaddr_in* serv_addr, socklen_t addrlen, int32_t msecond)
{
	if (msecond < 0)
	{
		return -1;
	}

	int flag = 0;
	flag = fcntl(fd, F_GETFL, 0);
	if (flag < 0)
	{
		return -1;
	}

	if (fcntl(fd, F_SETFL, (flag|O_NONBLOCK)) < 0)
	{
		return -1;
	}

	int error_code = 0;
	int ret = 0;

	ret = connect(fd, (const sockaddr*)serv_addr, addrlen);
	if (ret < 0)
	{
		if (errno != EINPROGRESS)
		{
			return -1;
		}

		//on connecting 
		struct pollfd stPollFD;
		memset(&stPollFD, 0, sizeof(stPollFD));
		stPollFD.fd = fd;
		stPollFD.events = POLLIN | POLLOUT;

		ret = poll(&stPollFD, 1, msecond);
		if (0 >= ret )
		{
			close(fd);
			return -1;
		}

		//when some socket fd was read(ret > 0)
		if (stPollFD.revents & POLLIN || stPollFD.revents & POLLOUT)
		{			
			socklen_t len = (socklen_t)sizeof(error_code);
			if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error_code, &len) < 0)
			{
				close(fd);
				return -1;
			}
		}
		else //impossible
		{
			close(fd);
			return -1;
		}

	}//if (ret < 0)

	fcntl(fd, F_SETFL, flag);
	if (error_code)
	{
		close(fd);
		return -1;
	}

	return 0;
}

//AÀà£º10.0.0.0 ~ 10.255.255.255
//BÀà£º172.16.0.0 ~ 172.31.255.255
//CÀà£º192.168.0.0 ~ 192.168.255.255
bool intranet_ip(uint32_t ip)
{
	ip = ntohl(ip);

	if ((ip >= 0x0A000000 && ip <= 0x0AFFFFFF) ||
		(ip >= 0xAC100000 && ip <= 0xAC1FFFFF) ||
		(ip >= 0xC0A80000 && ip <= 0xC0A8FFFF) ||
		(ip == 0x7F000001))
	{
		return true;
	}

	return false;
}








