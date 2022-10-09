#include "logic_service.h"
#include "logic_service_platform.h"
#include "logic_service_api.h"
#include "common_datetime.h"

void signal_crash_handler(int signo)
{
    ::signal(signo, SIG_DFL);

    SERVICE_API_INS->RequestServiceAlert("logic_crash", {
            { "signo", std::to_string(signo) }
    });

    SERVICE_LOG_TRACE_ERROR << "LOGIC SERVER CORE DUMP:" << CoreBacktrace(get_build_info(), signo) << std::endl;

    ::raise(signo);
}

int main(int argc, char** argv)
{
	if (SERVICE_API_INS == nullptr)
	{
        std::cout << "logic server api create fail" << std::endl;
		return 0;
	}

    std::string strSvrMode;
    if(argc >= 2)
    {
        strSvrMode = std::string(argv[1]);
    }

	if ("-v" == strSvrMode)
	{
	    std::cout << SERVICE_API_INS->GetServiceVersion() << std::endl;
		return 0;
	}

	if ("-p" == strSvrMode && argc >= 3)
	{
		const std::string filename(argv[2]);
		return SERVICE_API_INS->GetLogicSoGameApi()->ParseRecoveryDataFile(filename);
	}

	constexpr char VERSION_FILE[] = "logic.ver";
	delete_compile_file(VERSION_FILE);
	create_compile_file(VERSION_FILE, g_szServerGitSha1, g_szServerBuildDate, g_szServerBuildTime);

    ::signal(SIGBUS, signal_crash_handler);     // 总线错误
    ::signal(SIGSEGV, signal_crash_handler);    // SIGSEGV，非法内存访问
    ::signal(SIGFPE, signal_crash_handler);     // SIGFPE，数学相关的异常，如被0除，浮点溢出，等等
    ::signal(SIGABRT, signal_crash_handler);    // SIGABRT，由调用abort函数产生，进程非正常退出

	CLogicService service;
    if("debug" == strSvrMode)
    {
		service.RunEmbed(argc, argv);
    }
    else
    {
		service.RunAlone(argc, argv);
    }

	return (0);
}

