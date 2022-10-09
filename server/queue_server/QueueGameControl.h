#pragma once

#include "QueueAlertProcessor.h"
#include "QueueAppCommon.h"
#include "QueueAppConfig.h"
#include "QueueAppConfigZookeeper.h"
#include "QueueMsgProcessor.h"
#include "blocking_queue.hpp"
#include "common_flag.h"
#include <sys/epoll.h>

class CQueueGameControl
{
public:
    CQueueGameControl(void);
public:
	~CQueueGameControl(void);
public:
	static CQueueGameControl* GetInstance();

	bool Initialize();//初始化...
	bool Run();//运行....
	void After_Run();

	static const char* GetVerInfo();
	static CQueueAppConfig m_appCfg;//系统配置档
	static CQueueAppConfigZookeeper m_stZookeeper;
	static bool s_bLoadZooConfgSuccess;

	//通过GroupID 获取 Server ID
	int32_t GetServerIDByGroupID(int32_t iGroupID);

public:
	int32_t ActiveAllProcessor();
    void AddMsgQueue(const TQueueMsgData msg)
    {
        m_stMsgQueue.emplace_back(msg);
    }

private:
    // epoll
    int32_t epoll_init();
    bool epoll_new_socket(int fd);
    bool epoll_op_socket(int fd, int op, __uint32_t events);
    void epoll_destroy();

    bool CheckPath();
	bool DispatchMessage();

	void CheckTimers();
    void SendClientMsg();           // 发送客户端 Response
	bool CheckServerContext();
	bool InitZookeeper();
	bool LoadQueueConfig(const CQueueAppConfigZookeeper& stZookeeper);

    //检查连接是否存活
    void CheckConnectionAlive();
    void CheckClientAlive();

    //receive client message
    int32_t recv_client_messages(TSocketInfo& stSocket);
    void transfer_code(TSocketInfo& stSocket);
    int32_t transfer_one_code(TSocketInfo& stSocket, const char* pszCode, int32_t iCodeSize);
    int32_t socket_recv(int fd, char* data, int32_t size);
    int32_t socket_send(int fd, const char* data, int32_t size);
    bool AddMessageQueueProcessor(const TQueueMsgData& stMsgData);

private:
    dobala::blocking_queue<TQueueMsgData> m_stMsgQueue;         // 待发送的消息队列

    CQueueAlertProcessor m_stAlertProcessor;				    // 告警线程ID为 5
    CQueueMsgProcessor m_stMsgProcessor[MSG_PROCESSOR_NUM];     // 消息处理线程 ID从 10 开始

	time_t m_tmLastCheckRouterHeartBeat;	//上次检查路由心跳的时间
    time_t m_tmLastCheckClientSocketTime;	//上次检查客户端心跳的时间
	time_t m_tmLastCheckConfigTime;			//上次检查配置的时间
	time_t m_tmLastActiveProcessTime;       //上次激活线程的时间
	
	char abyCodeBuf[max_package_size];

    static CQueueRouterManager m_stRouterManager;   // TCP连接的管理

    // variable of epoll
    struct epoll_event* m_pevents;
    int	m_epoll_fd;
    int m_epoll_max_events;
    struct epoll_event m_epoll_event;
};

extern CFlag g_byRunFlag;
extern CQueueGameControl* g_pQueueGameCtrl;
