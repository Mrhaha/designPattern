#pragma once

#include "frame_common.h"
#include "frame_process.h"
#include "frame_socket.h"
#include "frame_nethead.h"
#include "frame_codequeue.h"

int32_t frame_master_spawn_process(record_t* log, frame_process_spawn_proc proc, uint16_t id, void* data, void* record, int16_t proc_type);
int32_t frame_master_init(record_t* log);
void frame_master_exit();
int32_t frame_master_process(record_t* log);
int32_t frame_master_dispath_worker(record_t* log);
int32_t frame_master_check_exit();
extern code_queue_t* frame_master_client_queue;
extern code_queue_t* frame_master_worker_queue;
