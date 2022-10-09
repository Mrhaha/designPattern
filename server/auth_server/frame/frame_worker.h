#pragma once

#include "frame_common.h"
#include "frame_log.h"
#include "frame_process.h"
#include "frame_nethead.h"
#include "frame_socket.h"
#include "frame_mysql.h"

void frame_worker_set_module_col(module_col_t* mod, record_t* log);
void frame_worker_init(record_t* log);
void frame_worker_exit(record_t* log);
int32_t frame_worker_recv_message(record_t* log, int16_t type);
int32_t frame_worker_send_message(record_t* log, char* src, int32_t len);
int32_t frame_worker_send_sql(record_t* log, int32_t uin, mysql_t* link, int16_t bill_type);
extern record_t* frame_worker_sql;
