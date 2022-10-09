#include "GameControl.h"
#include "common_tools.h"
#include "AppPlatform.h"
#include "ServerAlert.h"
#include "ServerConfig.h"

void sigusr1_handle( int iSigVal )
{
	g_byRunFlag.set(run_flag_reload);
	signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle( int iSigVal )
{
	g_byRunFlag.set(run_flag_exit);
	signal(SIGUSR2, sigusr2_handle);
}

void signal_crash_handler(int signo)
{
    ::signal(signo, SIG_DFL);

    std::shared_ptr<CCrossServerConfig> cross_config;
    CCrossServerConfig::GetConfig(cross_config);

    CServerAlert::SendAlertNotice("cross_crash", {
            { "signo", std::to_string(signo) },
            { "hostname", cross_config->GetZooCrossConfig().m_stCrossConfig.m_strHostname },
            { "server_ip", cross_config->GetZooCrossConfig().m_stCrossConfig.m_strHost },
            { "platform", cross_config->GetZooCrossConfig().m_stCrossConfig.m_strPlatform },
    });

    TRACESVR(log_mask_system_error, "[%s]cross server core dump: %s!\n", __FUNCTION__, CoreBacktrace(get_build_info(), signo).c_str());

    ::raise(signo);
}

int main(int argc, char **argv)
{
	bool is_daemon = false;

	for (int x=1; x < argc; ++x)
	{
		//支持版本信息查询
		if (0 == strcasecmp(argv[x], "-v"))
		{
			printf("%s", CGameControl::GetVerInfo());
			exit(0);
		}

		if (0 == strcasecmp(argv[x], "-d"))
		{
			is_daemon = true;
		}
	}

	int lock_fd = open((const char *)argv[0], O_RDONLY);
	if (lock_fd < 0)
	{
		exit(1);
	}

	if (flock(lock_fd, LOCK_EX | LOCK_NB) < 0)
	{
		printf("Game Server was lauched!\n");
		exit(1);
	}

	constexpr char VERSION_FILE[] = "cross.ver";
	delete_compile_file(VERSION_FILE);
	create_compile_file(VERSION_FILE, g_GIT_SHA1, g_BUILD_DATE, g_BUILD_TIME);

	INITLOG(APPLOG_GLOBAL_PREFIX_FILENAME);

    ::signal(SIGBUS, signal_crash_handler);     // 总线错误
    ::signal(SIGSEGV, signal_crash_handler);    // SIGSEGV，非法内存访问
    ::signal(SIGFPE, signal_crash_handler);     // SIGFPE，数学相关的异常，如被0除，浮点溢出，等等
    ::signal(SIGABRT, signal_crash_handler);    // SIGABRT，由调用abort函数产生，进程非正常退出

	g_pGameCtrl = CGameControl::GetInstance();
	if (nullptr == g_pGameCtrl)
	{
		printf("CGameControl::GetInstance return nullptr,then exit ...\n");
		exit(0);
	}

	if (!g_pGameCtrl->Initialize())
	{
		printf("gamectrl.Initialize() return false,then exit ...\n");
		exit(0);
	}

	//初始化守护进程
	if (is_daemon)
	{
		InitDaemon(1,0);
	}
	else
	{
		signal(SIGPIPE, SIG_IGN);
	}

	//安装信号处理函数
	signal(SIGUSR1, sigusr1_handle);
	signal(SIGUSR2, sigusr2_handle);

	g_byRunFlag.initialize((uint32_t) 0);

	//启动GameCtrl服务.
    if(!g_pGameCtrl->Run())
    {
        printf("gamectrl.Run() return false, then exit ...\n");
    }

	//结束运行
	g_pGameCtrl->After_Run();

	return 0;
}


