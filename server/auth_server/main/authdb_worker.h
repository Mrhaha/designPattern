#pragma once

#include "frame_common.h"
#include "frame_process.h"

void authdb_worker_process_cycle(uint16_t id, void* data, void* record);
void authdb_worker_sql_cycle(uint16_t id, void* data, void* record);
extern int16_t authdb_worker_process_id;
