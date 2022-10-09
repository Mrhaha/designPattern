#pragma once

#include "frame_list.h"
#include "frame_log.h"

#define DICT_INIT_SIZE 4
#define INT_MAX_SIZE 0x7fffffff
typedef struct _dict
{
    int32_t (*hash_init)(void* data, node_t* start, int32_t size);
    uint32_t (*hash_func)(uint64_t key);
    int32_t (*hash_add)(void* data, node_t* node, uint64_t key);
    node_t* (*hash_find)(void* data, uint64_t key);
    int32_t (*hash_del_by_key)(void* data, uint64_t key);
    int32_t (*hash_del)(void* data, node_t* node);
    int32_t (*hash_destroy)(void* data);
    node_t** buckets;
    node_t* start;  //索引区开始位置
    int32_t size;   //整个hash桶大小
    int32_t sizemask;//hash值掩码
    int32_t used;   //已经用掉了多少个位置
    int32_t eles;   //实际插入元素个数
} dict_t;

int32_t frame_hash_init(void* data, node_t* start, int32_t size);

int32_t frame_hash_dump(record_t* log, dict_t* hash);
