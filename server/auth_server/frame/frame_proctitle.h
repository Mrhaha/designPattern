#pragma once

#include "frame_common.h"
#include "frame_log.h"
int32_t frame_proctitle_init(record_t* log, char** argv);
int32_t frame_proctitle_set(char* prefix, char* title);
