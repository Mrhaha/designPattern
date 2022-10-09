#pragma once

#include <signal.h>

namespace taf
{

#define QUIT_FLAG       0x01
#define RELOAD_FLAG     0x02

class CTCServerBase
{
public:
    virtual ~CTCServerBase() {  };

    void                RunEmbed(int iArgc, char** pArgv);

    void                RunAlone(int iArgc, char** pArgv);

    static void         sigusr1_handle(int signo);

protected:
    virtual void        RealRun(int iArgc, char** pArgv) = 0;

    static void         sigusr2_handle(int signo);
    
    static bool         IsQuit();    

    static bool         IsReload();

private:
    static void         SetSystemLimit();

    static void         Daemon();
        
protected:
    static volatile unsigned char   m_uiFlag;
};

}
