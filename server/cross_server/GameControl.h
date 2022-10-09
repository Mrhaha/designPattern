#pragma once

#include "AppCommon.h"
#include "AppConfig.h"
#include "common_flag.h"
#include "AppConfigZookeeper.h"
#include "blocking_queue.hpp"
#include "HttpProcessor.h"
#include "CentreDataProcessor.h"
#include "MatchProcessor.h"
#include "TeamProcessor.h"

class CGameControl
{
public:
	CGameControl(void);
public:
	~CGameControl(void);
public:
	static CGameControl* GetInstance();

	bool Initialize();//初始化...
	bool Run();//运行....
	void After_Run();

	static const char* GetVerInfo();
	static CAppConfig m_appCfg;//系统配置档
	static CAppConfigZookeeper m_stZookeeper;
	static bool s_bLoadZooConfgSuccess;
	static CRouterManager m_stRouterManager;//TCP连接的管理

    void AddMsgQueue(const TCrossMsgData msg)
    {
        m_stMsgQueue.emplace_back(msg);
    }

public:
    //通过GroupID 获取 Server ID
    int32_t GetServerIDByGroupID(int32_t iGroupID);
    //通过Group ID判断与该服的连接是否建立
    bool IsGroupConnecting(int32_t iGroupID);

private:
    //通过Server ID得到对应的连接
    ConnectionType* GetServerConnection(int32_t iServerID);
    //通过Group ID得到对应的连接
    ConnectionType* GetGroupConnection(int32_t iGroupID);

    bool DispatchOneMessage(const TCrossMsgData& stMsgData);
    int32_t ActiveAllProcessor();
	bool CheckPath();
	int32_t DispatchMessage();
	void CheckTimers();
    void SendClientMsg();           // 发送 Response
	bool CheckServerContext();
	bool InitZookeeper();
	bool LoadCrossConfig(const CAppConfigZookeeper& stZookeeper);

	//设置和处理长连接
	void SetServerFDSet(fd_set& fds_read, fd_set& fds_write);
	void ManServerFDSet(const fd_set& fds_read, const fd_set& fds_write, int32_t& iOpenFDNum);
private:
    dobala::blocking_queue<TCrossMsgData> m_stMsgQueue;         // 待发送的消息队列

    CHttpProcessor          m_stHttpProcessor;          // 告警线程ID为 5
    CCentreDataProcessor    m_stCentreDataProcessor;    // 中心数据线程 ID为 10
    CMatchProcessor         m_stMatchProcessor;         // 匹配线程 ID为 20
    CTeamProcessor          m_stTeamProcessor;          // 组队线程 ID为 30

	time_t m_tmLastCheckRouterHeartBeat;	//上次检查路由心跳的时间
	time_t m_tmLastCheckConfigTime;			//上次检查配置的时间
	time_t m_tmLastActiveProcessTime;       //上次激活线程的时间
	
	char abyCodeBuf[max_package_size];
};

extern CFlag g_byRunFlag;
extern CGameControl* g_pGameCtrl;
