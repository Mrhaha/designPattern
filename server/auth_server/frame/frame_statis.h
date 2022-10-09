#pragma once

#include "frame_common.h"
#include "frame_log.h"
typedef struct _statis
{
    time_t  max_pps_time;
    time_t  cur_pps_time;
    int32_t max_pps;
    int32_t cur_pps;
    int32_t send_ok;
    int32_t send_err;
    int32_t recv_count;
} statis_t;
statis_t* frame_statis_get();
void frame_statis_dump(record_t* log);
void frame_statis_begin();
void frame_statis_end();
