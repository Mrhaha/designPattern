#pragma once

#include "frame_common.h"

int32_t authdb_modules_init(void* data);
module_t* authdb_modules_get(int32_t idx);

extern module_col_t authdb_module_col;
