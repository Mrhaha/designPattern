#pragma once

#include "frame_common.h"

typedef struct _shm
{
    void*  data;
    size_t size;
    size_t used;
} shm_t;

shm_t* frame_shm_create(key_t key, size_t size);
void*  frame_shm_alloc(shm_t* shm, size_t size);
int32_t frame_shm_destroy(key_t key);
