#include "logic_service_tcpconnection.h"
#include "logic_service_poller.h"

void CSocketEpollPolicy::NotifyConnecting(int32_t iFd)
{
	SERVICE_LOG_TRACE_DEBUG << "CONNECT SERVER SUCESS|FD:" << iFd << std::endl;
	SERVICE_POLLER_INS->add(iFd, static_cast<long long>(iFd), EPOLLIN);
}

void CSocketEpollPolicy::NotifyClosed(int32_t iFd)
{
	SERVICE_LOG_TRACE_DEBUG << "SERVER CLOSE CONN|FD:" << iFd << std::endl;
	SERVICE_POLLER_INS->del(iFd, static_cast<long long>(iFd), EPOLLIN);
}

