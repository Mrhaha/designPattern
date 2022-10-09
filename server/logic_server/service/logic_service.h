/* 
 * File:   rk_logic_service.h
 * Author: plantang
 *
 * Created on 2014年10月22日, 上午10:09
 */

#pragma once

#include <string>
#include "tc_server_base.h"

#include "logic_service_admin_container.h"
#include "logic_service_zookeeper.h"
#include "logic_service_connector_receiver.h"
#include "logic_service_tcpconnection.h"
#include "logic_service_connector_sender.h"

class CLogicService : public taf::CTCServerBase
{
public:
	CLogicService();

    bool    LoadConfig(const CLogicZookeeper& stZookeeper);

	void	RealRun(int iArgc, char** pArgv) override;

private:
	bool BeforeRun(int iArgc, char** pArgv);

	void AfterRun(int iArgc, char** pArgv);

	void TickRunning(int iArgc, char** pArgv);

	void EpollRunning(int iArgc, char** pArgv);

private:
	bool	IsConnector(const std::string& strIP, unsigned short usPort);

	bool	RefreshCrossConn(int32_t iNowSec);

    bool	RefreshQueueConn(int32_t iNowSec);

	/////////////////////////////////////////////////////
    bool    InitZookeeper();
    bool    InitLogicConn();
    bool    InitAdminConn();
	bool	InitCrossConn();
    bool	InitQueueConn();
    bool    InitMysqlThread();
    bool    InitConnectorSend();
    bool    InitOtherSystem();

private:
    static bool                 s_bLoadConfigSuccess;

    int                         m_iLogicAcceptFd;
    int                         m_iAdminAcceptFd;

	CLogicConnectorSender     m_stConnectorSender;

	CLogicConnectorReceiver	m_stConnectorReceiver;

    CLogicAdminContainer      m_stAdminContainer;

    CLogicZookeeper     m_stZookeeper;

	CLogicCrossConnection		m_stCrossConnection;

    CLogicQueueConnection       m_stQueueConnection;
};

