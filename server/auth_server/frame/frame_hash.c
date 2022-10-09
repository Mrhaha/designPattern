#include "frame_hash.h"
static uint32_t frame_hash_power(uint32_t size);
//static uint32_t frame_hash_int32_func(uint32_t key);
static uint32_t frame_hash_int64_func(uint64_t key);
static int32_t frame_hash_add(void* data, node_t* node, uint64_t key);
static int32_t frame_hash_index(dict_t* ht, uint64_t key);
static node_t* frame_hash_find(void* data, uint64_t key);
static int32_t frame_hash_del_by_key(void* data, uint64_t key);
static int32_t frame_hash_destroy(void* data);
static int32_t frame_hash_del(void* data, node_t* node);

static uint32_t frame_hash_power(uint32_t size)
{
    uint32_t i = DICT_INIT_SIZE;
    if (size >= INT_MAX_SIZE)
    {
        return INT_MAX_SIZE;
    }
    while (1)
    {
        if (i >= size)
        {
            return i;
        }
        i *= 2;
    }
}
//Thomas Wang's 32 bit Mix Function
//static uint32_t frame_hash_int32_func(uint32_t key)
//{
//    key += ~(key << 15);
//    key ^=  (key >> 10);
//    key +=  (key << 3);
//    key ^=  (key >> 6);
//    key += ~(key << 11);
//    key ^=  (key >> 16);
//    return key;
//}
static uint32_t frame_hash_int64_func(uint64_t key)
{
    key = (~key) + (key << 18); // key = (key << 18) - key - 1;
    key = key ^ (key >> 31);
    key = key * 21; // key = (key + (key << 2)) + (key << 4);
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);
    return (uint32_t) key;
}
static int32_t frame_hash_add(void* data, node_t* node, uint64_t key)
{
    if (NULL == data || NULL == node)
    {
        return fail;
    }
    int32_t index;
    dict_t* dt = (dict_t*)data;
    node_t* snode = NULL;
    //代表已经在该hash表中了
    if (-1 == (index = frame_hash_index(dt, key)))
    {
        return fail;
    }
    snode = dt->buckets[index];
    //如果该位置没有数据
    if (NULL == snode)
    {
        dt->buckets[index] = node;
        node->hash_use.links.next = node->hash_use.links.prev = INVALID_INDEX;
        ++dt->used;
    }
    else
    {
        node->hash_use.links.prev = INVALID_INDEX;
        node->hash_use.links.next = snode->id;
        snode->hash_use.links.prev = node->id;
        dt->buckets[index] = node;
    }
    node->hash_use.val = key;
    ++dt->eles;
    return success;
}
static int32_t frame_hash_index(dict_t* ht, uint64_t key)
{
    node_t* n = NULL;
    uint32_t index = ht->hash_func(key) & ht->sizemask;
    //搜索下该key是否存在
    n = ht->buckets[index];
    while (n)
    {
        if (n->hash_use.val == key)
        {
            return -1;
        }
        n = (INVALID_INDEX == n->hash_use.links.next) ? NULL :
            get_list_node(ht, n->hash_use.links.next);
    }
    return index;
}
static node_t* frame_hash_find(void* data, uint64_t key)
{
    if (NULL == data)
    {
        return NULL;
    }
    node_t* node = NULL;
    int32_t index;
    dict_t* ht = (dict_t*)data;
    if (ht->used == 0)
    {
        return NULL;
    }
    index = ht->hash_func(key) & ht->sizemask;
    node = ht->buckets[index];
    while (node)
    {
        if (node->hash_use.val == key)
        {
            return node;
        }
        node = (INVALID_INDEX == node->hash_use.links.next) ? NULL :
            get_list_node(ht, node->hash_use.links.next);
    }
    return NULL;
}
static int32_t frame_hash_del_by_key(void* data, uint64_t key)
{
    if (NULL == data)
    {
        return fail;
    }
    node_t* node = NULL;
    int32_t index;
    dict_t* dt = (dict_t*)data;
    if (dt->used == 0)
    {
        return fail;
    }
    index = dt->hash_func(key) & dt->sizemask;
    node = dt->buckets[index];
    while (node)
    {
        //找到了
        if (node->hash_use.val == key)
        {
            //如果只有一个元素
            if (node->hash_use.links.next == INVALID_INDEX
                && INVALID_INDEX == node->hash_use.links.prev)
            {
                dt->buckets[index] = NULL;
                --dt->used;
            }
            if (INVALID_INDEX != node->hash_use.links.next)
            {
                node_t* hn = get_list_node(dt, node->hash_use.links.next);
                hn->hash_use.links.prev = node->hash_use.links.prev;
            }
            if (INVALID_INDEX != node->hash_use.links.prev)
            {
                node_t* pre = get_list_node(dt, node->hash_use.links.prev);
                pre->hash_use.links.next = node->hash_use.links.next;
            }
            --dt->eles;
            frame_list_hash_used_reset(&node->hash_use);
            return success;
        }
        node = (INVALID_INDEX == node->hash_use.links.next) ? NULL :
            (get_list_node(dt, node->hash_use.links.next));
    }
    return fail;
}
static int32_t frame_hash_del(void* data, node_t* node)
{
    if (NULL == data)
    {
        return fail;
    }
    int32_t index;
    dict_t* dt = (dict_t*)data;
    if (dt->used == 0)
    {
        return fail;
    }
    index = dt->hash_func(node->hash_use.val) & dt->sizemask;
    //如果只有一个元素
    if (node->hash_use.links.next == INVALID_INDEX
        && INVALID_INDEX == node->hash_use.links.prev)
    {
        dt->buckets[index] = NULL;
        --dt->used;
    }
    if (INVALID_INDEX != node->hash_use.links.next)
    {
        node_t* hn = get_list_node(dt, node->hash_use.links.next);
        hn->hash_use.links.prev = node->hash_use.links.prev;
        //如果删除的节点是第一个，则需要把下一个节点提到第一个
        if (INVALID_INDEX == node->hash_use.links.prev)
        {
            dt->buckets[index] = hn;
        }
    }
    if (INVALID_INDEX != node->hash_use.links.prev)
    {
        node_t* pre = get_list_node(dt, node->hash_use.links.prev);
        pre->hash_use.links.next = node->hash_use.links.next;
    }
    --dt->eles;
    frame_list_hash_used_reset(&node->hash_use);

    return success;
}
static int32_t frame_hash_destroy(void* data)
{
    if (NULL == data)
    {
        return fail;
    }
    dict_t* ht = (dict_t*)data;
    if (NULL != ht->buckets)
    {
        free(ht->buckets);
        ht->buckets = NULL;
    }
    return success;
}
//static function end
//////////////////////////////////////////////////////////////////////////
int32_t frame_hash_init(void* data, node_t* start, int32_t size)
{
    if (NULL == data || 0 >= size || NULL == start)
    {
        return -1;
    }
    dict_t* dt = (dict_t*)data;
    //计算桶的大小
    uint32_t realsize = frame_hash_power((uint32_t)size);
    dt->size = realsize;
    dt->sizemask = realsize - 1;
    dt->used = 0;
    dt->eles = 0;
    //dt->hash_func = frame_hash_int32_func;
    dt->hash_func = frame_hash_int64_func;
    dt->hash_add = frame_hash_add;
    dt->hash_find = frame_hash_find;
    dt->hash_del_by_key = frame_hash_del_by_key;
    dt->hash_destroy = frame_hash_destroy;
    dt->hash_del = frame_hash_del;
    dt->start = start;
    //申请realsize桶大小
    dt->buckets = (node_t**) malloc(realsize * sizeof(node_t*));
    if (NULL == dt->buckets)
    {
        return -2;
    }
    memset(dt->buckets, 0, realsize * sizeof(node_t*));

    return success;
}
int32_t frame_hash_dump(record_t* log, dict_t* hash)
{
    if (NULL == log || NULL == hash)
    {
        return fail;
    }
    node_t* start = NULL;
    int32_t i;
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]hash table elements count(%d)\n", __FILE__, __FUNCTION__,
        hash->eles);
    for (i=0; i < hash->size; ++i)
    {
        start = hash->buckets[i];
        int32_t a = 0;
        while (start)
        {
            log->write_file(log_mask_detail, (void*)log,
                "[%s::%s][%d::%d][%d]id:%d,val:%"UINT64_FMT",prev:%d,next:%d\n", 
                __FILE__, __FUNCTION__, i, hash->size, a, start->id,
                start->hash_use.val, start->hash_use.links.prev,
                start->hash_use.links.next);
            ++a;
            start = (INVALID_INDEX == start->hash_use.links.next) ? NULL:
                get_list_node(hash, start->hash_use.links.next);
        }
    }
    return success;
}
