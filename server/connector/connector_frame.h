#pragma once

#include "connector_define.h"
#include <sys/epoll.h> //by aprilliu
//add by liuqy 2011-03-15
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "common_tcp_connection.h"
#include "common_msg_head_connector.h"
#include "connect_config.h"

using TcpConnect = CTCPConnectionHandle<CSocketEmptyPolicy>;

struct stSocketStatistics
{
	uint32_t m_unSocketCount;
	uint32_t m_unSocketAdd;
};

enum
{
	hearbeat_interval       = 30,						//心跳包间隔
	max_unactive_interval   = 6*hearbeat_interval,		//大于此时间间隔将认为是对端
	check_interval          = max_unactive_interval,	//检查间隔
	//max_group_id            = 128,
};

struct stServer
{
	char   	m_szIP[16];
	int32_t m_iIP;
	int16_t m_nPort;
	int32_t m_iGroupID;
	bool    m_bHaveHello;
	time_t  m_tmLastActiveTime;
	int16_t m_nLocalPort;
};

struct stClient
{
	char   		m_szIP[16];
	int32_t    	m_iIP;
	int16_t    	m_nPort;
	bool       	m_bHaveHello;
	time_t     	m_tmLastActiveTime;
};

extern CFlag g_byRunFlag;

extern stConnectorConfig g_stSvrConfig;

//singleton framework
class CConnFrame
{
public:
	CConnFrame();                                                    
	~CConnFrame();

	int32_t  initialize();                                             
	int32_t  working();		                                            

	void* operator new( size_t size)throw();                             
	void  operator delete(void* ptr);   

	static CConnFrame* spawn();                             
	static int32_t size();		

private:
	CConnFrame(const CConnFrame& original);
	CConnFrame& operator = (const CConnFrame& original);

private:		
	int32_t  load_config();
	int32_t  reload_config();  

	//do some check 
	int32_t check_self(); 

	//reset current socket
	void clear_current_socket(EnumClientCloseReason reason);
	
	//receive message
	int32_t  recv_messages();                                        

	//receive client message
	int32_t  recv_client_messages(int32_t fd);

	int  check_and_send_data();                                    
		
	int32_t  socket_send(int fd, char* data, int32_t size);
	int32_t  socket_recv(int fd, char* data, int32_t size);

	int32_t getUniqueID();
	
protected:
	void check_flag();


	int32_t process_code_from_service(const char* pszInCode, const int32_t iInCodeSize);

	void transfer_code(int32_t iIndex);

	int32_t transfer_one_code(int32_t iIndex, const char* pszCode, int32_t iCodeSize);

	int32_t open_epoll_socket(uint16_t port);
    //add by liuqy 2011-03-15
    int32_t open_epoll_socket(uint16_t port, char* local_ip);

	//initialize epoll
	int32_t  epoll_init(void);

	//create one epoll socket
	int  epoll_socket(int domain, int type, int protocol);	

	//add one socket
	int  epoll_new_socket(int fd);

	//close socket socket
	void  epoll_close(int fd);

	//destroy epoll
	void  epoll_destroy();	

	//
	void log_statistics();
		
private:
	CConnectConfig	m_stConfig;
	int	m_iSocket;	
        
	stSocketInfo m_stSocketInfo[max_socket_count];   /*socket结构数组，用于记录客户端的信息*/		
 	stSocketInfo* m_pstCurrentSocket;                   /*当前的socket结构指针*/ 
	
	char m_szMsgBuf[max_package_size]; //
	stSocketStatistics	m_stSocketSatistics;

	//variable of epoll
	struct epoll_event* m_pevents;
	int	m_epoll_fd;
	int m_maxfd;				
	struct epoll_event  m_epoll_event;

	time_t	m_tmLastCheckTime;		//上次检查连接的时间
	time_t  m_tmLastStatisticTime;	//上次进行统计的时间

public:
	int16_t     m_nRouterCount;			// connsvrd 连接的server个数
	stServer    m_astServer[max_router_server_count];		// connsvrd 可连接的server 信息
	TcpConnect  m_astTCPConnection[max_router_server_count];		// connsvrd 与 server 的TCP连接
	
	int32_t check_server_to(int32_t iGroupID = 1);
	int32_t refresh();
	int32_t on_hello( int32_t iSrcIP, int16_t nSrcPort);
	TcpConnect* get_connection_to(int32_t iGroupID = 1);

	int16_t     m_nClientCount;
	stClient    m_astClient[max_router_server_count];
	TcpConnect  m_astClientConnection[max_router_server_count];
	TcpConnect* get_client_connection(uint32_t uIP, uint16_t nPort);
	time_t      m_tmLastCheckConnection;

private:
	int32_t	check_config();
private:
	time_t	m_tmConfigLastModifyTime;//配置文件最后一次修改的时间
	time_t	m_tmLastCheckConfigTime; //最后一次检查配置文件的时间
};
