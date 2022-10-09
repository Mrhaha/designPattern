#pragma once

#include "common_ipv4.h"
#include "common_base.h"
#include "poll.h"
#include "netdb.h"

#define smtp_socket_buffer_size 2048        // socket buffer长度
#define smtp_socket_connect_timeout 2000    // socket连接超时时间
#define smtp_socket_timeout 1000            // socket等待超时时间 毫秒

std::string Base64Encode(const std::string& src)
{
    int i, j, srcLen = src.length();
    std::string dst(srcLen / 3 * 4 + 4, 0);
    for(i = 0, j= 0; i <=srcLen - 3; i+=3, j+=4) {
        dst[j] = (src[i] & 0xFC) >> 2;
        dst[j+1] = ((src[i] & 0x03) << 4) + ((src[i+1] & 0xF0) >> 4);
        dst[j+2] = ((src[i+1] & 0x0F) << 2) + ((src[i+2] & 0xC0) >> 6);
        dst[j+3] = src[i+2] & 0x3F;
    }
    if( srcLen % 3 == 1 ) {
        dst[j] = (src[i] & 0xFC) >> 2;
        dst[j+1] = ((src[i] & 0x03) << 4);
        dst[j+2] = 64;
        dst[j+3] = 64;
        j += 4;
    }
    else if( srcLen % 3 == 2 ) {
        dst[j] = (src[i] & 0xFC) >> 2;
        dst[j+1] = ((src[i] & 0x03) << 4) + ((src[i+1] & 0xF0) >> 4);
        dst[j+2] = ((src[i+1] & 0x0F) << 2);
        dst[j+3] = 64;
        j+=4;
    }

    static unsigned char *base64 = (unsigned char*)("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=");
    for(i = 0; i < j; ++i) {    //map 6 bit value to base64 ASCII character
        dst[i] = base64[(int)dst[i]];
    }

    return dst;
}

class CSMTPSocket
{
public:
    CSMTPSocket();
	~CSMTPSocket();

    CSMTPSocket(const CSMTPSocket&) = delete;
    void operator=(const CSMTPSocket&) = delete;

	void close_socket();

	int32_t get_fd() const;
	enmTCPConnectionStatus get_status() const;

	//open socket as a client, use bind
	int32_t open_as_client(const char* localIP = nullptr, uint16_t localPort = 0);

	//
	int32_t connect_to(const char* ip, uint16_t port, bool nonblock = true, int32_t msecond = 100);

	//receive data from socket stream, and store recv buffer, wait for processing
	int32_t recv_data(void);

	//send data to remote peer
	int32_t send_data(const char* data, size_t size);

	//get one completed code from receive buffer
	//return value: >0 -- success, <= 0  -- some error
	//size: 应该传入data的大小,传出得到的实际数据大小（目前size没有传入data的大小，仅传出数据的实际大小）
    EnumMsgHeadPackERROR get_one_code(char* data, size_t& size, int32_t max_code_size);

    std::string ConnectEmailServer(const std::string& smtpServer, const std::string& username, const std::string& pw);
    std::string SendOneEmail(const std::string& username, const std::vector<std::string>& to, const std::string& subject, const std::string& data);
    bool GetOneCode(char* data, const std::string& strCode);
    bool CloseEmailConnect();

protected:
	int32_t m_socket_fd;

	enmTCPConnectionStatus m_socket_status;

	//receiving buffer
	int32_t m_recv_head;
	int32_t m_recv_tail;
	char m_recv_buffer[smtp_socket_buffer_size + 1]; //特别注意：不是循环缓冲区

												//sending buffer
	int32_t m_send_head;
	int32_t m_send_tail;
	char m_send_buffer[smtp_socket_buffer_size + 1];//特别注意：不是循环缓冲区

#ifdef __MULTI_THREAD__
	pthread_mutex_t m_mutex;			//the mutual exclusive variable
#endif
};

CSMTPSocket::CSMTPSocket()
{
	m_socket_fd = -1;
	m_socket_status = enmTCPConnectionStatus_closed;

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

#ifdef __MULTI_THREAD__
	pthread_mutex_init(&m_mutex, NULL);
#endif
}

CSMTPSocket::~CSMTPSocket()
{
	if (0 < m_socket_fd || enmTCPConnectionStatus_closed != m_socket_status)
	{
		close_socket();
	}
}

void CSMTPSocket::close_socket()
{
	if (m_socket_fd > 0)
	{
		close(m_socket_fd);
	}

	m_socket_fd = -1;
	m_socket_status = enmTCPConnectionStatus_closed;

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;
}

int32_t CSMTPSocket::get_fd() const
{
	return m_socket_fd;
}

enmTCPConnectionStatus CSMTPSocket::get_status() const
{
	return m_socket_status;
}

int32_t CSMTPSocket::open_as_client(const char* localIP /*= nullptr*/, uint16_t localPort/*=0*/)
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

		const socklen_t addressSize = static_cast<socklen_t>(sizeof(stLocalAddress));
		if (0 != bind(m_socket_fd, reinterpret_cast<const struct sockaddr*>(&stLocalAddress), addressSize))
		{
			close_socket();
			return -1;
		}
	}

	set_socket_buffer_size(m_socket_fd, smtp_socket_buffer_size);

	m_socket_status = enmTCPConnectionStatus_opened;

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	return 0;
}

int32_t CSMTPSocket::connect_to(const char* ip, uint16_t port, bool nonblock /*= true*/, int32_t msecond /* = 100*/)
{
    if (nullptr == ip)
    {
        return -1;
    }

	if (enmTCPConnectionStatus_opened != m_socket_status || 0 > m_socket_fd)
	{
		return -3;
	}

	struct sockaddr_in stDstAddress;
	memset(&stDstAddress, 0x0, sizeof(stDstAddress));
	stDstAddress.sin_family = AF_INET;
	stDstAddress.sin_addr.s_addr = inet_addr(ip);
	stDstAddress.sin_port = htons(port);
	const socklen_t sockSize = static_cast<socklen_t>(sizeof(stDstAddress));

	if (0 != connect_nonblock(m_socket_fd, &stDstAddress, sockSize, msecond))
	{
		close_socket();
		return -2;
	}

	if (nonblock)
	{
		set_non_block(m_socket_fd);
	}

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	//set socket status to connected !!
	m_socket_status = enmTCPConnectionStatus_connected;
	return 0;
}

int32_t CSMTPSocket::recv_data(void)
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

int32_t CSMTPSocket::send_data(const char* data, size_t size)
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
			if (smtp_socket_buffer_size - m_send_tail >= (int32_t)size)
			{
				memcpy(&m_send_buffer[m_send_tail], data, (size_t)size);
				m_send_tail += size;
				ret = 0;  //暂且认为已经发生成功
			}
			else if (smtp_socket_buffer_size - m_send_tail + m_send_head >= (int32_t)size)
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
	}

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
	}

#ifdef __MULTI_THREAD__
	pthread_mutex_unlock(&m_mutex);
#endif

	return ret;
}

EnumMsgHeadPackERROR CSMTPSocket::get_one_code(char* data, size_t& size, int32_t max_code_size)
{
	if (nullptr == data)
	{
		return Enum_MHPE_GetOneCode_Invalid;
	}

	int32_t buffer_data_size = m_recv_tail - m_recv_head;

	//判断接收缓冲区内的数据大小
	if (buffer_data_size < 3)
	{
		return Enum_MHPE_GetOneCode_NoData;
	}

	//
	//最好判断一下传入参数:接收buffer的大小,目前的实现是没有判断，这就要求传入的buffer长度是足够的
	if(buffer_data_size > max_code_size)
        return Enum_MHPE_GetOneCode_CodeSizeNotEnough;

	size = (size_t)buffer_data_size;
	memcpy(data, &m_recv_buffer[m_recv_head], size);

	//
	m_recv_head += buffer_data_size;
	if (m_recv_tail == m_recv_head)
	{
		m_recv_head = 0;
		m_recv_tail = 0;
	}

	return Enum_MHPE_Success;
}

bool CSMTPSocket::GetOneCode(char* data, const std::string& strCode)
{
    //on connecting
    struct pollfd stPollFD;
    memset(&stPollFD, 0, sizeof(stPollFD));
    stPollFD.fd = m_socket_fd;
    stPollFD.events = POLLIN | POLLPRI;

    int ret = poll(&stPollFD, 1, smtp_socket_timeout);
    if (0 >= ret)
    {
        sprintf(data, "poll timeout");
        return false;
    }

    //when some socket fd was read(ret > 0)
    if (stPollFD.revents & POLLIN || stPollFD.revents & POLLPRI)
    {
        int iRecvCode = recv_data();
        if(iRecvCode != 0)
        {
            sprintf(data, "recv_data() error:%d", iRecvCode);
            return false;
        }

        size_t size;
        int code  = get_one_code(data, size, smtp_socket_buffer_size);
        if(code <= 0)
        {
            sprintf(data, "get_one_code() error:%d", code);
            return false;
        }

        if (std::string(data).substr(0, 3) != strCode)
        {
            return false;
        }

        return true;
    }
    else
    {
        sprintf(data, "poll revents error");
        return false;
    }
}

std::string CSMTPSocket::ConnectEmailServer(const std::string& smtpServer, const std::string& username, const std::string& pw)
{
    hostent *ph = gethostbyname(smtpServer.c_str());
    if(ph == NULL)
    {
        return "gethostbyname() error";
    }

    if (open_as_client() != 0)
    {
        return "open_as_client() error";
    }

    char str[32];
    int32_t iConRet = connect_to(inet_ntop(ph->h_addrtype, ph->h_addr_list[0], str, sizeof(str)), 25, true, smtp_socket_connect_timeout);
    if(iConRet != 0)
    {
        return "connect_to() error|" + std::to_string(iConRet);
    }

    //
    char recvBuffer[smtp_socket_buffer_size];
    if(false == GetOneCode(recvBuffer, "220"))
    {
        return "before Helo retCode error|" + std::string(recvBuffer);
    }

    std::string msg = std::string("HELO ") + smtpServer + "\r\n";
    send_data(msg.c_str(), msg.length());
    if(false == GetOneCode(recvBuffer, "250"))
    {
        return "Helo retCode error|" + std::string(recvBuffer);
    }

    //start to log in
    msg = "auth login\r\n";
    send_data(msg.c_str(), msg.length());
    if(false == GetOneCode(recvBuffer, "334"))
    {
        return "login retCode error|" + std::string(recvBuffer);
    }

    msg = Base64Encode(username) + "\r\n";
    send_data(msg.c_str(), msg.length());
    if(false == GetOneCode(recvBuffer, "334"))
    {
        return "username retCode error|" + std::string(recvBuffer);
    }

    msg = Base64Encode(pw) + "\r\n";
    send_data(msg.c_str(), msg.length());
    if(false == GetOneCode(recvBuffer, "235"))
    {
        return "pw retCode error|" + std::string(recvBuffer);
    }

    return "";
}

std::string CSMTPSocket::SendOneEmail(const std::string& username, const std::vector<std::string>& to, const std::string& subject, const std::string& data)
{
    //Set sender
    char recvBuffer[smtp_socket_buffer_size];
    std::string msg = "mail from:<" + username + ">\r\n";
    send_data(msg.c_str(), msg.length());
    if(false == GetOneCode(recvBuffer, "250"))
    {
        return "mail from retCode error|" + std::string(recvBuffer);
    }

    //set receiver
    for(const auto& addr : to)
    {
        msg = "rcpt to:<" + addr + ">\r\n";
        send_data(msg.c_str(), msg.length());
        if(false == GetOneCode(recvBuffer, "250"))
        {
            return "rcpt to retCode error|" + std::string(recvBuffer);
        }
    }

    //send data
    msg = "data\r\n";
    send_data(msg.c_str(), msg.length());
    if(false == GetOneCode(recvBuffer, "354"))
    {
        return "data retCode error|" + std::string(recvBuffer);
    }

    msg = "from:" + username + "\n" + "subject:" + subject + "\r\n\r\n" + data + "\r\n.\r\n";
    send_data(msg.c_str(), msg.length());
    if(false == GetOneCode(recvBuffer, "250"))
    {
        return "content retCode error|" + std::string(recvBuffer);
    }

    return "";
}

bool CSMTPSocket::CloseEmailConnect()
{
    if(m_socket_status == enmTCPConnectionStatus_connected)
    {
        std::string msg = "quit\r\n";
        send_data(msg.c_str(), msg.length());

        /*
        char recvBuffer[smtp_socket_buffer_size];
        std::size_t size;
        if(GetOneCode(recvBuffer, size) == false)
        {
            return false;
        }
        */
    }

    close_socket();

    return true;
}
