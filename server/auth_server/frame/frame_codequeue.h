#pragma once

#include "frame_common.h"
#define DEFAULT_CODE_QUEUE_SIZE 0x20000
#if defined(_DEBUG) || defined(DEBUG)
#define MAX_CODE_QUEUE_SIZE  0x40000
#else
#define MAX_CODE_QUEUE_SIZE  0x4000000
#endif
typedef struct _code_queue
{
    int32_t (*append)(void* data, const char* src, int32_t len);
    int32_t (*pop)(void* data, char* out, int32_t* len);
    char*   buf;
    int32_t size;
    int32_t head;
    int32_t tail;
    int32_t offset;
} code_queue_t;

code_queue_t* frame_codequeue_create(int32_t size);
int32_t frame_codequeue_destroy(code_queue_t* queue);
