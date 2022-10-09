#include <sys/resource.h>
#include "common_flag.h"
#include "common_base.h"

#include "connector_define.h"
#include "connector_frame.h"
#include "connector_log.h"

int32_t initialize(int32_t mode = 1);

void sigusr1_handle( int32_t iSigVal );
void sigusr2_handle( int32_t iSigVal );

CConnFrame *g_pFrame;

#define SERVER_VERSION  "CB9"
#define CONNECT_PID_NAME "connectorsvrd.pid"

void sigusr1_handle( int32_t iSigVal )
{
	g_byRunFlag.set(run_flag_reload );
	signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle( int32_t iSigVal )
{
	g_byRunFlag.set(run_flag_exit);
	signal(SIGUSR2, sigusr2_handle);
}

int32_t load_config()
{
	CConnectConfig stConfigFile;

	if (0 != stConfigFile.LoadConfig(CONNECT_CONFIG))
	{
		fprintf(stdout, "open config file :%s failed\n", CONNECT_CONFIG);
		return fail;
	}

	memset(&g_stSvrConfig, 0, sizeof(g_stSvrConfig));

	g_stSvrConfig.m_iLogLevel = stConfigFile.m_iLogLevel;
	g_stSvrConfig.m_nMaxfd    = stConfigFile.m_nMaxfd;
	
	return success;
}

int32_t set_openfile_limit(int32_t filecount)
{
	struct rlimit rlim;

	rlim.rlim_cur = rlim.rlim_max = filecount;
	if (setrlimit(RLIMIT_NOFILE, &rlim) < 0)
	{
		TRACESVR(log_mask_system_error, "[%s] setrlimit failed!, error = %s\n",
			__FUNCTION__, strerror(errno));
		return fail;
	}

	rlim.rlim_cur = rlim.rlim_max = 0;
	if (getrlimit(RLIMIT_NOFILE, &rlim) < 0)
	{
		TRACESVR(log_mask_system_error, "[%s] getrlimit failed!, error = %s\n",
			__FUNCTION__, strerror(errno));
		return fail;
	}

	TRACESVR(log_mask_detail, "[%s] setlimit successfully! now max open file = %d\n",
		__FUNCTION__, rlim.rlim_cur);
        
    rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &rlim);

	return success;
}

int32_t initialize(int32_t mode)
{
	int32_t ret = success;

	//安装信号处理函数
	signal(SIGUSR1, sigusr1_handle);
	signal(SIGUSR2, sigusr2_handle);

	g_byRunFlag.initialize(0);

	//load config for key
	if (success != load_config())
	{
		//TRACESVR("[%s] load_config failed!\n", __FUNCTION__);
		fprintf(stdout, "[%s] load_config failed!\n", __FUNCTION__);
		return fail;
	}

	SETTRACELEVEL(g_stSvrConfig.m_iLogLevel);//设置日志等级/掩码

	if (success != set_openfile_limit((max_socket_count + 200)))
	{
		return fail;
	}

	g_pFrame = new CConnFrame;
	if(NULL == g_pFrame)
	{
		exit(-1);
	}

	return ret;
}

int32_t show_help()
{
	printf("Usage:gow_connsvrd [operator] [OPTION]\n");
	printf("-new\n");
	printf("\ttranslate config(.cfg) to xml\n");
	printf("\texample: ./gow_connsvrd -new\n");
	printf("-delete\n");
	printf("\tdelete server source from config(.xml)\n");
	printf("\texample: ./gow_connsvrd -delete 1\n");
	printf("-merge");
	printf("\tmerge server\n");
	printf("\texample: ./gow_connsvrd -merge 1 2\n");
	printf("\tmerge dst server(2) to src server(1).\n");
	printf("-add");
	printf("\tadd one server\n");
	printf("\texample: ./gow_connsvrd -add 1 192.168.1.1 20001 30001 10001]\n");
	printf("\tadd one server group id(1), logic server ip(192.168.1.1), logic server listen port(20001), logic server common port(30001), connect server common port(10001).\n");
	return success;
}

int main( int argc, char **argv )
{
	if (argc > 1 && 0 == strcasecmp(argv[1], "-v"))
	{
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_) || (__DEBUG__)
		fprintf(stdout, "connector server debug socket %u logic %u version %s %s %s\n", max_socket_count, max_router_server_count, SERVER_VERSION, __DATE__, __TIME__);
#else
		fprintf(stdout, "connector server release socket %u logic %u version %s %s %s\n", max_socket_count, max_router_server_count, SERVER_VERSION, __DATE__, __TIME__);
#endif
		return 0;
	}
	if (argc == 2 && 0 == strcasecmp(argv[1], "--help"))
	{
		show_help();
		return success;
	}
	if (argc == 3 && 0 == strcasecmp(argv[1], "-delete"))
	{
		CConnectConfig stConfig;
		int32_t iGroupID = atoi(argv[2]);
		stConfig.CloseServer(iGroupID);
		return success;
	}
	if (argc == 4 && 0 == strcasecmp(argv[1], "-merge"))
	{
		CConnectConfig stConfig;
		int32_t iSrcGroupID = atoi(argv[2]);
		int32_t iDstGroupID = atoi(argv[3]);
		stConfig.CloseServer(iDstGroupID);
		stConfig.MergeServer(iSrcGroupID, iDstGroupID);
		return success;
	}
	if (argc >= 4 && 0 == strcasecmp(argv[1], "-add"))
	{
		CConnectConfig stConfig;
		int32_t iGroupID = atoi(argv[2]);
		if (argc >= 4)
		{
			int32_t iListenPort = 0;
			int32_t iLogicPort  = 0;
			int32_t iLocalPort  = 0;
			if (argc < 7)
			{
				iListenPort = 20000 + iGroupID % 10000;
				iLogicPort  = 30000 + iGroupID % 10000;
				iLocalPort  = 10000 + iGroupID % 10000;
			}
			else
			{
				iListenPort = atoi(argv[4]);
				iLogicPort  = atoi(argv[5]);
				iLocalPort  = atoi(argv[6]);
			}

			stConfig.ConnectServer(iGroupID, argv[3], iListenPort, iLogicPort, iLocalPort);
		}
		return success;
	}

	bool is_daemon = false;

	if(1 < argc && !strcasecmp(argv[1], "-d" ) )
	{
		is_daemon = true;
	}
	else
	{
		is_daemon = false;
	}

	int lock_fd = open((const char *)argv[0], O_RDONLY);
	if(lock_fd < 0 )
	{
		exit(1);
	}

	if( flock(lock_fd, LOCK_EX | LOCK_NB) < 0 )
	{
		printf("Connector Server was lauched!\n");
		exit(1);
	}


	if (is_daemon)
	{
		InitDaemon(1,0);
	}

	int32_t ret = 0;
	int32_t mode = 0;

	ret = initialize(mode);
	if(success != ret)
	{
		printf("Connector Server fail to launch!\n");
		exit(0);
	}

	fprintf(stdout, "connectsvrd is going to launch ...\n");

    delete_pid_file(CONNECT_PID_NAME);
	ret = g_pFrame->initialize();
	if( success != ret )
	{
		fprintf(stdout, "connectsvrd launch failed!\n");
		exit(0);
	}

	fprintf(stdout, "connectsvrd is working ...\n");
	TRACESVR(log_mask_system_config, "connectsvrd is working ...\n");

    create_pid_file(CONNECT_PID_NAME);
	g_pFrame->working();

	TRACESVR(" connector server exit now ...\n");

	return 0;
}

