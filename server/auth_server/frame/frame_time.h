#pragma once

#include "frame_common.h"
int32_t frame_time_get_datetime(time_t times, char* src, int32_t len);
int32_t frame_time_get_date_str(time_t times, char* src, int32_t len, int32_t type);
