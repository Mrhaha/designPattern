#pragma once

#include "frame_common.h"
#include "frame_master.h"
#include "frame_process.h"
#include "authdb_config.h"

void authdb_master_process_cycle(record_t* log, authdb_cfg_t* cfg);

extern char** authdb_master_argv;
