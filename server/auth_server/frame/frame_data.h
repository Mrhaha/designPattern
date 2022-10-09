#pragma once

#include "frame_list.h"
#include "frame_hash.h"
#include "frame_shm.h"
#include "frame_log.h"
#if defined(_DEBUG) || defined(DEBUG)
#define RECYCLE_ONCE 4      //一次性回收个数
#else
#define RECYCLE_ONCE 100
#endif
typedef struct _data
{
    list_t* recycled;   //待回收的
    list_t* unused;     //空闲的
    dict_t* hash_table; //查找数据
    shm_t*  shm;        //申请的共享内存
    void*   start;      //实际数据存储地址
    key_t   key;        //共享内存key
    int32_t data_size;  //数据对象大小
    int32_t data_count; //数据个数
} frame_data_t;

#define get_data(s, i, c) ((void*)(s + i * c))

int32_t frame_data_create(record_t* log, key_t key, int32_t size, int32_t count);
int32_t frame_data_destroy();
int32_t frame_data_recycle(uint64_t key);

//void* frame_data_get_by(int32_t key, int32_t create);
void* frame_data_get_by(uint64_t key, int32_t create);

extern frame_data_t frame_data;
