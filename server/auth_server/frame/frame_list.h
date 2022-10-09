#pragma once

#include "frame_common.h"
#include "frame_log.h"
#define INVALID_INDEX  (-1)
#define LIST_FROM_HEAD  0    //添加到队列头
#define LIST_FROM_TAIL  1    //添加到队列尾
//指向上一个和下一个索引
typedef struct _index
{
    int32_t prev;   //上一个
    int32_t next;   //下一个
} index_t;
typedef struct _hash_used
{
    uint64_t val;    //实际保存的应该是用户的uin
    index_t links;   //双向链表
} hash_used_t;
//链表的每个单元
typedef struct _node
{
    int32_t     id; //索引ID
    hash_used_t hash_use;   //hashtable使用拉链法解决冲突，这个专供hashtable使用，这部分是冗余数据
    index_t     links;     //用于回收区，未使用区
} node_t;
typedef struct _link
{
    int32_t (*list_init)(void* data, node_t* start, int32_t size);
    int32_t (*list_push)(void* data, node_t* node, int32_t type);
    node_t* (*list_pop)(void* data, int32_t type);
    int32_t (*list_del)(void* data, node_t* node);
    node_t* start;  //链表的起始位置
    node_t* head;   //链表的第一个元素
    node_t* tail;   //链表的最后一个元素
    int32_t size;   //链表大小
} list_t;

#define get_list_node(l, posi) ((l)->start + posi)

int32_t frame_list_init(void* data, node_t* start, int32_t size);
int32_t frame_list_hash_used_reset(hash_used_t* hut);
size_t  frame_list_size(int32_t count);
//将数据交换到队尾
int32_t frame_list_exchange(list_t* list, node_t* node, int32_t type);
int32_t frame_list_dump(record_t* log, list_t* list);
