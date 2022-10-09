#pragma once

#include "common_base.h"
#include "common_ipv4.h"
#include "common_tools.h"

class CSocketEmptyPolicy
{
public:
	static void NotifyConnecting(int32_t iFd) {};
	static void NotifyClosed(int32_t iFd) {};
};

//IPv4的一些基本封装
//对TCP协议的Socket的封装
//主要接口:
//1.接收数据. 提供了在应用层的接收缓存区
//2.发送数据. 提供了在应用层对一次发生未发生完的数据的缓存
//3.建立监听端口(开启服务器端监听)
//4.建立连接(连接到监听的服务器端)
template <typename SocketPolicy = CSocketEmptyPolicy>
class CTCPSocket
{
public:
	CTCPSocket();
	~CTCPSocket();

	DISALLOW_COPY_AND_ASSIGN(CTCPSocket);

	bool be_connected() const;
	bool be_listening() const;

	void close_socket();

	int32_t get_fd() const;
	enmTCPConnectionStatus get_status() const;

	//open socket as  a server
	int32_t open_as_server(uint16_t port, const char* ip = nullptr);
	//open socket as a client, use bind
	int32_t open_as_client(const char* localIP = nullptr, uint16_t localPort = 0);

	//
	int32_t connect_to(const char* ip, uint16_t port, bool nonblock = true, int32_t msecond = 100);

	//ip : 网络字节序的地址
	int32_t connect_to(uint32_t ip, uint16_t port, bool nonblock = true, int32_t msecond = 100);

	//
	int32_t accept_fd(int32_t fd);

	//
	void set_4_byte_size(bool flag);

	//
	bool is_4_byte_size(void) const;


	//receive data from socket stream, and store recv buffer, wait for processing
	int32_t recv_data(void);

	//send data to remote peer
	int32_t send_data(const char* data, size_t size);

	//get one completed code from receive buffer
	//return value: >0 -- success, <= 0  -- some error
	//size: 应该传入data的大小,传出得到的实际数据大小（目前size没有传入data的大小，仅传出数据的实际大小）
    EnumMsgHeadPackERROR get_one_code(char* data, size_t& size, int32_t max_code_size);

	/**
	@brief 接收到的数据包前6个字节表示包长度
	*/
	int32_t get_one_code_ex(char* data, size_t& size);

protected:
	enmSocketTypes m_socket_type;
	int32_t m_socket_fd;

	enmTCPConnectionStatus m_socket_status;

	//标识是否用4字节表示长度，true :yes；否则，no
	//即使用int32_t表示长度,实际长度不会超过int16_t的范围
	//具体是使用4个字节还是2个字节表示长度，是由应用协议决定的，这会影响
	//对包实际长度的识别
	bool m_4bytes;

	//receiving buffer
	int32_t m_recv_head;
	int32_t m_recv_tail;
	char m_recv_buffer[socket_buffer_size + 1]; //特别注意：不是循环缓冲区

												//sending buffer
	int32_t m_send_head;
	int32_t m_send_tail;
	char m_send_buffer[socket_buffer_size + 1];//特别注意：不是循环缓冲区

#ifdef __MULTI_THREAD__
	pthread_mutex_t m_mutex;			//the mutual exclusive variable
#endif
};

template <typename SocketPolicy>
CTCPSocket<SocketPolicy>::CTCPSocket()
{
	m_socket_fd = -1;
	m_socket_type = enmSocketTypes_comm;
	m_socket_status = enmTCPConnectionStatus_closed;

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	m_4bytes = false;

#ifdef __MULTI_THREAD__
	pthread_mutex_init(&m_mutex, NULL);
#endif
}

template <typename SocketPolicy>
CTCPSocket<SocketPolicy>::~CTCPSocket()
{
	if (0 < m_socket_fd || enmTCPConnectionStatus_closed != m_socket_status)
	{
		close_socket();
	}
}


template <typename SocketPolicy>
bool CTCPSocket<SocketPolicy>::be_connected() const
{
	return (m_socket_status == enmTCPConnectionStatus_connected && m_socket_fd > 0);
}

template <typename SocketPolicy>
bool CTCPSocket<SocketPolicy>::be_listening() const
{
	return (m_socket_status == enmTCPConnectionStatus_listening && m_socket_fd > 0);
}

template <typename SocketPolicy>
void CTCPSocket<SocketPolicy>::close_socket()
{
	if (m_socket_fd > 0)
	{
		SocketPolicy::NotifyClosed(m_socket_fd);
		close(m_socket_fd);
	}

	m_socket_fd = -1;
	m_socket_status = enmTCPConnectionStatus_closed;

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	return;
}

template <typename SocketPolicy>
int32_t CTCPSocket<SocketPolicy>::get_fd() const
{
	return m_socket_fd;
}

template <typename SocketPolicy>
enmTCPConnectionStatus CTCPSocket<SocketPolicy>::get_status() const
{
	return m_socket_status;
}

template <typename SocketPolicy>
int32_t CTCPSocket<SocketPolicy>::open_as_server(uint16_t port, const char* ip /*= nullptr*/)
{
	//firstly, close the socket
	if (enmTCPConnectionStatus_closed != m_socket_status || m_socket_fd > 0)
	{
		close_socket();
	}

	//open socket
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd < 0)
	{
		close_socket();
		return -1;
	}

	struct linger ling = { 0, 0 };
	setsockopt(m_socket_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));

	int flags = 1;
	setsockopt(m_socket_fd, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));

	if (0 != setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags)))
	{
		close_socket();
		return -1;
	}

	set_socket_buffer_size(m_socket_fd, socket_buffer_size);

	//
	struct sockaddr_in stSocketAddr;
	memset(&stSocketAddr, 0x0, sizeof(stSocketAddr));
	stSocketAddr.sin_family = AF_INET;
	if (nullptr != ip)
	{
		stSocketAddr.sin_addr.s_addr = inet_addr(ip);
	}
	else
	{
		stSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	stSocketAddr.sin_port = htons(port);
	const socklen_t addrSize = socklen_t(sizeof(stSocketAddr));
	if (0 != bind(m_socket_fd, reinterpret_cast<const sockaddr*>(&stSocketAddr), addrSize))
	{
		close_socket();
		return -1;
	}

	if (0 != listen(m_socket_fd, 1024))
	{
		close_socket();
		return -1;
	}

	//设置为非阻塞
	set_non_block(m_socket_fd);

	m_socket_type = enmSocketTypes_listen; //监听socket
	m_socket_status = enmTCPConnectionStatus_listening;

	return 0;
}

template <typename SocketPolicy>
int32_t CTCPSocket<SocketPolicy>::open_as_client(const char* localIP /*= nullptr*/, uint16_t localPort/*=0*/)
{
	//firstly, close the socket
	if (enmTCPConnectionStatus_closed != m_socket_status || m_socket_fd > 0)
	{
		close_socket();
	}

	//open socket
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd < 0)
	{
		close_socket();
		return -1;
	}

	if (nullptr != localIP)
	{
		struct sockaddr_in stLocalAddress;
		memset(&stLocalAddress, 0x0, sizeof(stLocalAddress));

		stLocalAddress.sin_family = AF_INET;
		stLocalAddress.sin_addr.s_addr = inet_addr(localIP);
		if (0 != localPort)
		{
			stLocalAddress.sin_port = htons(localPort);
		}
		// 2010-5-22 add by liuqy
		int32_t iOptValue = 1;
		if (0 != setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, &iOptValue, sizeof(iOptValue)))
		{
			close_socket();
			return -1;
		}
		//////////////////////////////////////////////////////////////////////////

		const socklen_t addressSize = static_cast<socklen_t>(sizeof(stLocalAddress));
		if (0 != bind(m_socket_fd, reinterpret_cast<const struct sockaddr*>(&stLocalAddress), addressSize))
		{
			close_socket();
			return -1;
		}

	}

	set_socket_buffer_size(m_socket_fd, socket_buffer_size);

	m_socket_type = enmSocketTypes_comm;
	m_socket_status = enmTCPConnectionStatus_opened;

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	return 0;
}

template <typename SocketPolicy>
int32_t CTCPSocket<SocketPolicy>::connect_to(const char* ip, uint16_t port, bool nonblock /*= true*/, int32_t msecond /* = 100*/)
{
	if (nullptr == ip)
	{
		return -1;
	}

	const uint32_t uip = inet_addr(ip);
	return connect_to(uip, port, nonblock, msecond);
}

template <typename SocketPolicy>
int32_t CTCPSocket<SocketPolicy>::connect_to(uint32_t ip, uint16_t port, bool nonblock /*= true*/, int32_t msecond /* = 100*/)
{
	//check socket type (common or listening)
	if (enmSocketTypes_comm != m_socket_type)
	{
		return -1;
	}

	if (enmTCPConnectionStatus_opened != m_socket_status || 0 > m_socket_fd)
	{
		return -1;
	}

	struct sockaddr_in stDstAddress;
	memset(&stDstAddress, 0x0, sizeof(stDstAddress));
	stDstAddress.sin_family = AF_INET;
	stDstAddress.sin_addr.s_addr = ip;
	stDstAddress.sin_port = htons(port);
	const socklen_t sockSize = static_cast<socklen_t>(sizeof(stDstAddress));

	if (0 != connect_nonblock(m_socket_fd, &stDstAddress, sockSize, msecond))
	{
		close_socket();
		return -1;
	}

	if (nonblock)
	{
		set_non_block(m_socket_fd);
	}

	SocketPolicy::NotifyConnecting(m_socket_fd);

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	//set socket status to connected !!
	m_socket_status = enmTCPConnectionStatus_connected;
	return 0;
}

template <typename SocketPolicy>
int32_t CTCPSocket<SocketPolicy>::accept_fd(int32_t fd)
{
	if (fd < 0)
	{
		return -1;
	}

	if (enmTCPConnectionStatus_connected == m_socket_status || m_socket_fd > 0)
	{
		close_socket();
	}

	m_socket_fd = fd;
	m_socket_status = enmTCPConnectionStatus_connected;
	m_socket_type = enmSocketTypes_comm;

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	//set non block
	set_non_block(m_socket_fd);

	return 0;
}

template <typename SocketPolicy>
void CTCPSocket<SocketPolicy>::set_4_byte_size(bool flag)
{
	m_4bytes = flag;
}

template <typename SocketPolicy>
bool CTCPSocket<SocketPolicy>::is_4_byte_size(void) const
{
	return m_4bytes;
}


template <typename SocketPolicy>
int32_t CTCPSocket<SocketPolicy>::recv_data(void)
{
	int32_t ret = 0;

	if (enmTCPConnectionStatus_connected != m_socket_status || 0 > m_socket_fd)
	{
		return enmConnErrorCode_invalid_socket;
	}

	//
	if (m_recv_tail == m_recv_head)
	{
		m_recv_head = 0;
		m_recv_tail = 0;
	}

	//
	int32_t received_byte = 0;

	do //从网络连接上接收数据
	{
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			//接收缓冲区的头指针不在
			if (m_recv_head > 0)
			{
				//注意: 要使用memmove而不是memcpy
				memmove(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(m_recv_tail - m_recv_head));
				m_recv_tail -= m_recv_head;
				m_recv_head = 0;
			}
			else
			{
				//recv buffer was full, just stop receiving data
				ret = enmConnErrorCode_recv_notenouth_buffer;
				break;
			}
		}//if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))

		 //
		received_byte = recv(m_socket_fd, &m_recv_buffer[m_recv_tail], size_t(sizeof(m_recv_buffer) - m_recv_tail), 0);
		if (received_byte > 0)
		{
			m_recv_tail += received_byte;
		}
		else if (0 == received_byte)
		{
			//peer was closed, so just break receiving
			close_socket();
			ret = enmConnErrorCode_peer_closed;
			break;
		}
		else if (EAGAIN != errno) //received_byte < 0 && EAGAIN != errno
		{
			//must be some error!
			close_socket();
			ret = enmConnErrorCode_unknow;
			break;
		}

	} while (false);	//暂时修改为只读一次，2014-11-28

	return ret;
}

template <typename SocketPolicy>
int32_t CTCPSocket<SocketPolicy>::send_data(const char* data, size_t size)
{
	int32_t ret = 0;
	if (nullptr == data || 0 == size)
	{
		return enmConnErrorCode_invalid_param;
	}

#ifdef __MULTI_THREAD__
	pthread_mutex_lock(&m_mutex);
#endif

	if (enmTCPConnectionStatus_connected != m_socket_status || m_socket_fd < 0)
	{
#ifdef __MULTI_THREAD__
		pthread_mutex_unlock(&m_mutex);
#endif
		return enmConnErrorCode_invalid_socket;
	}

	int32_t data_remaind_byte = 0;
	data_remaind_byte = m_send_tail - m_send_head;

	//上次发生的剩余数据
	const char* pszDataPtr = &m_send_buffer[m_send_head];

	int32_t sended_byte = 0;
	while (data_remaind_byte > 0)
	{
		sended_byte = send(m_socket_fd, pszDataPtr, (size_t)data_remaind_byte, 0);
		if (sended_byte > 0)
		{
			m_send_head += sended_byte;
			pszDataPtr += sended_byte;
			data_remaind_byte -= sended_byte;
		}
		if (sended_byte < 0 && EAGAIN != errno)
		{
			close_socket();
			ret = enmConnErrorCode_unknow;
			break;
		}
		if (sended_byte < 0)
		{
			if (socket_buffer_size - m_send_tail >= (int32_t)size)
			{
				memcpy(&m_send_buffer[m_send_tail], data, (size_t)size);
				m_send_tail += size;
				ret = 0;  //暂且认为已经发生成功
			}
			else if (socket_buffer_size - m_send_tail + m_send_head >= (int32_t)size)
			{
				memmove(&m_send_buffer[0], &m_send_buffer[m_send_head], (size_t)data_remaind_byte);
				m_send_head = 0;
				m_send_tail = data_remaind_byte;

				memcpy(&m_send_buffer[m_send_tail], data, (size_t)size);
				m_send_tail += size;
				ret = 0;  //暂且认为已经发生成功
			}
			else
			{
				ret = enmConnErrorCode_send_notenouth_buffer;
			}
			break;
		}


	}//while (data_remaind_byte > 0)

	if (0 == data_remaind_byte)
	{
		m_send_head = 0;
		m_send_tail = 0;
	}
	else
	{
#ifdef __MULTI_THREAD__
		pthread_mutex_unlock(&m_mutex);
#endif

		return ret;
	}

	//
	data_remaind_byte = (int32_t)size;
	pszDataPtr = data;

	while (data_remaind_byte > 0)
	{
		sended_byte = send(m_socket_fd, pszDataPtr, (size_t)data_remaind_byte, 0);
		if (sended_byte > 0)
		{
			pszDataPtr += sended_byte;
			data_remaind_byte -= sended_byte;
		}
		else if (sended_byte < 0 && EAGAIN != errno)
		{
			close_socket();
			ret = enmConnErrorCode_unknow;
			break;
		}
		else if (sended_byte < 0)
		{
			//cache the remained data
			memcpy(&m_send_buffer[m_send_tail], pszDataPtr, (size_t)data_remaind_byte);
			m_send_tail += data_remaind_byte;
			ret = 0;  //暂且认为已经发生成功
			break;
		}

	}//while (data_remaind_byte > 0)

#ifdef __MULTI_THREAD__
	pthread_mutex_unlock(&m_mutex);
#endif

	return ret;
}

template <typename SocketPolicy>
EnumMsgHeadPackERROR CTCPSocket<SocketPolicy>::get_one_code(char* data, size_t& size, int32_t max_code_size)
{
	if (nullptr == data)
	{
		return Enum_MHPE_GetOneCode_Invalid;
	}

	int32_t buffer_data_size = 0;
	buffer_data_size = m_recv_tail - m_recv_head;

	//判断接收缓冲区内的数据大小
	if (buffer_data_size <= 0)
	{
		return Enum_MHPE_GetOneCode_NoData;
	}

	//根据表示长度的字节数，检查数据的合法性
	if ((is_4_byte_size() && buffer_data_size < (int32_t)sizeof(int32_t))
		|| (!is_4_byte_size() && buffer_data_size < (int32_t)sizeof(int16_t)))
	{
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			memcpy(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
			m_recv_head = 0;
			m_recv_tail = buffer_data_size;

		}

		return Enum_MHPE_GetOneCode_NoData;
	}

	//长度字段占用4byte
	uint32_t code_size = 0;
	void *ptr = reinterpret_cast<void*>(&m_recv_buffer[m_recv_head]);
	if (is_4_byte_size())
	{
		code_size = ntohl(*reinterpret_cast<uint32_t*>(ptr));
	}
	else
	{
		code_size = ntohs(*reinterpret_cast<uint16_t*>(ptr));
	}

	if (code_size <= 0)
	{
		return Enum_MHPE_GetOneCode_Invalid;
	}

	//若接收缓冲区内的数据不足一个code
	if ((uint32_t)buffer_data_size < code_size)
	{
		//并且数据已经存放到缓冲区尾了, 则移动数据到接收缓冲区头部
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			memmove(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
			m_recv_head = 0;
			m_recv_tail = buffer_data_size;
		}

		return Enum_MHPE_GetOneCode_NoData;
	}

	//
	//最好判断一下传入参数:接收buffer的大小,目前的实现是没有判断，这就要求传入的buffer长度是足够的
	if(code_size > (uint32_t)max_code_size)
        return Enum_MHPE_GetOneCode_CodeSizeNotEnough;

	size = (size_t)code_size;
	memcpy(data, &m_recv_buffer[m_recv_head], size);

	//
	m_recv_head += code_size;
	if (m_recv_tail == m_recv_head)
	{
		m_recv_head = 0;
		m_recv_tail = 0;
	}

	return Enum_MHPE_Success;
}

template <typename SocketPolicy>
int32_t CTCPSocket<SocketPolicy>::get_one_code_ex(char* data, size_t& size)
{
	if (NULL == data)
	{
		return -1;
	}

	int32_t buffer_data_size = 0;
	buffer_data_size = m_recv_tail - m_recv_head;

	//判断接收缓冲区内的数据大小
	if (buffer_data_size <= 0)
	{
		return 0;
	}

	//根据表示长度的字节数，检查数据的合法性
	if (buffer_data_size < 6)
	{
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			memcpy(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
			m_recv_head = 0;
			m_recv_tail = buffer_data_size;

		}

		return 0;
	}

	//长度字段占用6byte,取得code的长度
	int32_t code_size = 0;
	char pkgLenBuf[6];
	memset(pkgLenBuf, 0, sizeof(pkgLenBuf));
	memcpy(pkgLenBuf, &m_recv_buffer[m_recv_head], sizeof(pkgLenBuf));
	sscanf(pkgLenBuf, "%6x", &code_size);

	//若接收缓冲区内的数据不足一个code
	if (buffer_data_size < code_size)
	{
		//移动数据到接收缓冲区头部
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			memmove(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
			m_recv_head = 0;
			m_recv_tail = buffer_data_size;
		}

		return 0;
	}

	//
	//最好判断一下传入参数:接收buffer的大小.
	size_t t = (size_t)(code_size + sizeof(pkgLenBuf));
	if (size < t)
	{
		return -2;
	}
	size = t;//这意味着前6Byte中保存的长度并不包括6Byte自身
	memcpy(data, &m_recv_buffer[m_recv_head], size);

	//
	m_recv_head += size;
	if (m_recv_tail == m_recv_head)
	{
		m_recv_head = 0;
		m_recv_tail = 0;
	}

	return 1;
}