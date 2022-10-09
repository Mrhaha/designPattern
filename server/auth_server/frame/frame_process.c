#include "frame_process.h"
#include "frame_worker.h"
#include "frame_master.h"
uint16_t frame_process_type;
process_t frame_process_col[MAX_PROCESS_COUNT];
uint16_t frame_process_count;   //子进程个数
uint16_t frame_process_slot;    //子进程所处位置
int32_t  frame_process_id;      //进程ID
record_t* frame_process_record = NULL;
int32_t frame_process_transport_pkg_count = 0;
int32_t frame_process_last_sig;

static sig_atomic_t frame_process_quit;
static sig_atomic_t frame_process_reopen;

static int32_t frame_process_seq;

static void frame_process_signal_handler(int32_t signo);
static void frame_process_get_status(record_t* log);

//只捕获三种信号
signal_t g_signals[] = {
    {
        "reopen",
        "SIGUSR1",
        frame_process_signal_handler,
        SIGUSR1
    },
    {
        "quit",
        "SIGUSR2",
        frame_process_signal_handler,
        SIGUSR2
    },
    {
        "",
        "SIGCHLD",
        frame_process_signal_handler,
        SIGCHLD
    },
    {NULL, NULL, NULL, 0}
};

static void frame_process_signal_handler(int32_t signo)
{
    char* action;
    signal_t* sig;

    for (sig = g_signals; NULL != sig->signame; ++sig)
    {
        if (sig->signo == signo)
        {
            break;
        }
    }
    action = "";
    switch (signo)
    {
    case SIGUSR2:
        frame_process_quit = 1;
        frame_process_last_sig = signo;
        action = ", exiting";
        break;
    case SIGUSR1:
        frame_process_reopen = 1;
        action = ", restart server";
        break;
    }
    frame_process_record->write_file(log_mask_warning,
        (void*)frame_process_record, "[%s::%s]receive signal %d(%s)%s\n",
        __FILE__, __FUNCTION__, signo, sig->signame, action);

    if (SIGCHLD == signo)
    {
        frame_process_record->write_file(log_mask_warning,
            (void*)frame_process_record,
            "[%s::%s]receive signal SIGCHLD\n", __FILE__, __FUNCTION__);
        frame_process_get_status(frame_process_record);
        frame_master_check_exit();
    }
}
static void frame_process_get_status(record_t* log)
{
    int32_t status;
    int32_t pid;
    uint16_t i;
    uint16_t one;

    for (; ;)
    {
        pid = (int32_t)waitpid(-1, &status, WNOHANG);
        if (0 == pid)
        {
            return;
        }
        if (-1 == pid)
        {
            if (EINTR == errno)
            {
                continue;
            }
            if (ECHILD == errno || 1 == one)
            {
                return;
            }
            log->write_file(log_mask_error, (void*)log,
                "[%s::%s]waitpid failed errno(%d), msg(%s)\n",
                __FILE__, __FUNCTION__, errno, strerror(errno));
            return;
        }
        one = 1;

        for (i = 0; i < frame_process_count; i++)
        {
            if (pid == frame_process_col[i].pid ) 
            {
                frame_process_col[i].status = status;
                frame_process_col[i].exited = 1;
                frame_process_col[i].pid = -1;
                break;
            }
        }

        if (WTERMSIG(status))
        {
#ifdef WCOREDUMP
            log->write_file(log_mask_warning, (void*)log,
                "[%s::%s]pid(%u) exited on signal %d%s\n",
                __FILE__, __FUNCTION__, pid, WTERMSIG(status),
                WCOREDUMP(status) ? " (core dumped)" : "");
#else
            log->write_file(log_mask_warning, (void*)log,
                "[%s::%s]pid(%u) exited on signal %d\n",
                __FILE__, __FUNCTION__, pid, WTERMSIG(status));
#endif
        }
        else 
        {
            log->write_file(log_mask_warning, (void*)log,
                "[%s::%s]pid(%u) exited with code %d\n",
                __FILE__, __FUNCTION__, pid, WEXITSTATUS(status));
        }
    }
}

//static function end
//////////////////////////////////////////////////////////////////////////
int32_t frame_process_signal_init(record_t* log)
{
    if (NULL == log)
    {
        return fail;
    }
    frame_process_record = log;
    signal_t* sig;
    struct sigaction sa;

    for (sig = g_signals; NULL != sig->signame; ++sig)
    {
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = sig->handler;
        sigemptyset(&sa.sa_mask);
        if (-1 == sigaction(sig->signo, &sa, NULL))
        {
            log->write_file(log_mask_error, (void*)log, 
                "[%s::%s]sigaction(%s) failed\n", __FILE__, __FUNCTION__,
                sig->signame);
            return fail;
        }
    }
    return success;
}

process_t* frame_process_get( int32_t uin, int16_t type )
{
    int32_t count = 0;
    int32_t i;
    int32_t idx;
    process_t* pro = NULL;
    //计算出没有退出的进程数
    for (i=0; i < frame_process_count; ++i)
    {
        pro = &frame_process_col[i];
        if (pro->pid > 0 && pro->exited != 1
            && type == pro->proc_type)
        {
            count++;
        }
    }
    //hash到某个未退出的进程上
    idx = (uin == 0) ? (frame_process_seq++) % count : uin % count;
    count = 0;
    for (i=0; i < frame_process_count; ++i)
    {
        pro = &frame_process_col[i];
        if (pro->pid > 0 && pro->exited != 1
            && type == pro->proc_type)
        {
            if (count == idx)
            {
                break;
            }
            count++;
        }
    }
    if (frame_process_seq < 0)
    {
        frame_process_seq = 0;
    }
    return pro;
}

int32_t frame_process_daemon()
{
    int  fd;
    record_t* log = frame_process_record;

    switch (fork()) 
    {
    case -1:
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]fork() failed\n", __FILE__, __FUNCTION__);
        return fail;
    case 0:
        log->write_file(log_mask_detail, (void*)log,
            "[%s::%s]master process continue\n", __FILE__, __FUNCTION__);
        break;
    default:
        exit(0);
    }

    frame_process_id = getpid();

    if (setsid() == -1)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s] setsid() failed\n", __FILE__, __FUNCTION__);
        return fail;
    }

    umask(0);

    fd = open("/dev/null", O_RDWR);
    if (fd == -1)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]open(\"/dev/null\") failed\n", __FILE__,
            __FUNCTION__);
        return fail;
    }

    if (dup2(fd, STDIN_FILENO) == -1)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]dup2(STDIN) failed\n", __FILE__, __FUNCTION__);
        return fail;
    }

    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]dup2(STDOUT) failed\n", __FILE__, __FUNCTION__);
        return fail;    
    }

    if (fd > STDERR_FILENO) 
    {
        if (close(fd) == -1) 
        {
            log->write_file(log_mask_error, (void*)log, 
                "[%s::%s]close(%u) failed\n", __FILE__, __FUNCTION__,
                fd);
            return fail;
        }
    }
    return success;
}
void frame_process_check_flag(record_t* log)
{
    //收到退出信号
    if (1 == frame_process_quit)
    {
//         log->write_file(log_mask_warning, (void*)log,
//             "[%s::%s]receive quit signal,so eixt\n", __FILE__,
//             __FUNCTION__);
    }
    if (1 == frame_process_reopen)
    {
		frame_process_reopen = 0;
        log->write_file(log_mask_warning, (void*)log,
            "[%s::%s]receive reopen signal,so reopen\n", __FILE__,
            __FUNCTION__);
    }
    if (1 == frame_process_quit)
    {
        if (PROCESS_MASTER == frame_process_type)
        {
            frame_master_exit(log);
        }
        else if (PROCESS_WORKER == frame_process_type 
            || PROCESS_MYSQL == frame_process_type)
        {
            frame_worker_exit(log);
        }
        frame_process_quit = 0;
    }
}

void frame_process_signal_worker( record_t* log, int32_t signo )
{
    if (NULL == log || 0 > signo)
    {
        return;
    }
    uint16_t i;
    process_t* pro = &frame_process_col[0];

    for (i=0; i < frame_process_count; ++i, ++pro)
    {
        if (0 >= pro->pid
            || 1 == pro->exited)
        {
            continue;
        }
        if (-1 == kill(pro->pid, signo))
        {
            if (NULL != log)
            {
                log->write_file(log_mask_error, (void*)log, 
                    "[%s::%s] kill(%d %d) failed\n", __FILE__, __FUNCTION__,
                    pro->pid, signo);
            }
            if (ESRCH == errno)
            {
                pro->exited = 1;
                pro->pid = -1;
            }

            continue;
        }
    }
    return ;
}
