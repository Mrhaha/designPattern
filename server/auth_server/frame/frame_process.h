#pragma once

#include "frame_common.h"
#include "frame_log.h"
#include "frame_socket.h"
#define PROCESS_MASTER 0
#define PROCESS_WORKER 1
#define PROCESS_MYSQL  2    //专门处理mysql语句进程

typedef struct _signal
{
    char* name;
    char* signame;
    void (*handler)(int32_t signo);
    int32_t signo;
} signal_t;
typedef struct _process
{
    int32_t pid;
    int32_t status;
    int32_t worker_channel[2]; //0:子进程，1:主进程
    uint16_t exited;
    int16_t proc_type;  //PROCESS_XXX宏
} process_t;

typedef void (*frame_process_spawn_proc)(uint16_t id, void* data, void* record);
int32_t frame_process_signal_init(record_t* log);
void frame_process_signal_worker(record_t* log, int32_t signo);
process_t* frame_process_get(int32_t uin, int16_t type);
int32_t frame_process_daemon();
void frame_process_check_flag(record_t* log);

extern uint16_t frame_process_type;
extern process_t frame_process_col[MAX_PROCESS_COUNT];
extern uint16_t frame_process_count;
extern uint16_t frame_process_slot;
extern record_t* frame_process_record;
extern int32_t frame_process_id;
extern int32_t frame_process_transport_pkg_count;
extern int32_t frame_process_last_sig;

//extern int32_t worker_recv_channel;
//extern int32_t worker_send_channel;
