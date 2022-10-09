#include "tc_server_base.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <iostream>

namespace taf
{

volatile unsigned char CTCServerBase::m_uiFlag = 0;

void                
CTCServerBase::SetSystemLimit()
{
    struct rlimit rlim;
	/* raise open files */
	rlim.rlim_cur = 100000;
	rlim.rlim_max = 100000;
	::setrlimit(RLIMIT_NOFILE, &rlim);

	/* allow core dump */
	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;
	::setrlimit(RLIMIT_CORE, &rlim);
}

void                
CTCServerBase::Daemon()
{
    ::signal(SIGHUP, SIG_IGN);
    ::signal(SIGPIPE, SIG_IGN);
    ::signal(SIGTTOU, SIG_IGN);
    ::signal(SIGTTIN, SIG_IGN);
    ::signal(SIGCHLD, SIG_IGN);

    ::signal(SIGINT,  SIG_IGN);
    ::signal(SIGTERM, SIG_IGN);
    ::daemon(1, 0);

    CTCServerBase::m_uiFlag = 0;
    
    ::signal(SIGUSR1, CTCServerBase::sigusr1_handle);
    ::signal(SIGUSR2, CTCServerBase::sigusr2_handle);
}

void         
CTCServerBase::sigusr1_handle(int signo)
{
    m_uiFlag |= RELOAD_FLAG;

    if (signo == SIGUSR1)
        ::signal(SIGUSR1, CTCServerBase::sigusr1_handle);
}

void 
CTCServerBase::sigusr2_handle(int signo)
{
    m_uiFlag |= QUIT_FLAG;

    if (signo == SIGUSR2)
        ::signal(SIGUSR2, CTCServerBase::sigusr2_handle);
}

bool    
CTCServerBase::IsQuit()
{
    return ((m_uiFlag & QUIT_FLAG) == QUIT_FLAG);
}

bool    
CTCServerBase::IsReload()
{
    bool bReload = ((m_uiFlag & RELOAD_FLAG) == RELOAD_FLAG);
    if(true == bReload)
    {
        m_uiFlag &= ~RELOAD_FLAG;

        return (true);
    }

    return (false);
}

void                
CTCServerBase::RunEmbed(int iArgc, char** pArgv)
{
    SetSystemLimit();
    RealRun(iArgc, pArgv);
}

void                
CTCServerBase::RunAlone(int iArgc, char** pArgv)
{
    SetSystemLimit();
    Daemon();
    RealRun(iArgc, pArgv);
}

}
