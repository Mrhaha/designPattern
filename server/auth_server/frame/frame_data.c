#include "frame_data.h"
#include "frame_log.h"
frame_data_t frame_data;
static record_t* frame_data_log = NULL;

static int32_t frame_data_init(record_t* log, frame_data_t* data);
static int32_t frame_data_swap_recycle(int32_t count);

static int32_t frame_data_init(record_t* log, frame_data_t* data)
{
    if (NULL == log || NULL == data)
    {
        return fail;
    }
    size_t size;
    data->recycled->list_init = frame_list_init;
    data->unused->list_init = frame_list_init;
    data->hash_table->hash_init = frame_hash_init;
    //从共享内存中申请索引区数据
    size = frame_list_size(data->data_count);
    node_t* idx_start = (node_t*) frame_shm_alloc(data->shm, size);
    if (NULL == idx_start)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]create index memory size(%ld) fail\n", __FILE__, 
            __FUNCTION__, size);
        return fail;
    }
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]create index memory size(%ld) success\n", __FILE__,
        __FUNCTION__, size);
    //初始化索引区
    if (fail ==data->recycled->list_init((void*)data->recycled, idx_start, 0)
        ||fail == data->unused->list_init((void*)data->unused, idx_start, data->data_count)
        || fail == data->hash_table->hash_init((void*)data->hash_table, idx_start, data->data_count))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]frame data init fail\n", __FILE__, __FUNCTION__);
        return fail;
    }
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]frame data init success\n", __FILE__, __FUNCTION__);

    //申请数据
    size = data->data_count * data->data_size;
    data->start = frame_shm_alloc(data->shm, size);
    if (NULL == data->start)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]create data memory size(%ld) fail\n", __FILE__,
            __FUNCTION__, size);
        return fail;
    }
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]create data memory size(%ld) success\n", __FILE__, __FUNCTION__,
        size);

    return success;
}
static int32_t frame_data_swap_recycle(int32_t count)
{
    int32_t swap_count = 0;
    int32_t i;
    uint64_t val;
    node_t* node = NULL;
    for (i=0; i < count; ++i)
    {
        node = frame_data.recycled->list_pop((void*)frame_data.recycled,
            LIST_FROM_HEAD);
        if (NULL == node)
        {
            frame_data_log->write_file(log_mask_detail, (void*)frame_data_log,
                "[%s::%s]all the recycle list of cache are cleaned up\n",
                __FILE__, __FUNCTION__);
            break;
        }
        val = node->hash_use.val;
        //删除掉在hashtable里的数据
        //frame_data.hash_table->hash_del_by_key((void*)frame_data.hash_table, val);
        int32_t idx = frame_data.hash_table->hash_func(val)
             & frame_data.hash_table->sizemask;
        frame_data_log->write_file(log_mask_detail, set_void(frame_data_log),
            "[%s::%s]dump del node info(id:%d,index[prev:%d,"
            "next:%d],hash used(val:%"PRIu64",index[prev:%d,next:%d],hash "
            "index:%d\n", __FILE__, __FUNCTION__, node->id, node->links.prev,
            node->links.next, node->hash_use.val, node->hash_use.links.prev,
            node->hash_use.links.next, idx);
        frame_data.hash_table->hash_del(set_void(frame_data.hash_table), node);
        //将数据加入到空闲队列
        //frame_list_hash_used_reset(&node->hash_use);
        frame_data.unused->list_push(set_void(frame_data.unused), node,
            LIST_FROM_HEAD);
        frame_data_log->write_file(log_mask_detail, set_void(frame_data_log),
            "[%s::%s]the cached(val:%"UINT64_FMT",obj_id:%d,index[prev:%d,"
            "next:%d],hash used(val:%"PRIu64",index[prev:%d,next:%d]) is "
            "deleted\n", __FILE__, __FUNCTION__, val, node->id, node->links.prev,
            node->links.next, node->hash_use.val, node->hash_use.links.prev,
            node->hash_use.links.next);
        ++swap_count;
    }
#if defined(_DEBUG) || defined(DEBUG)
    //just for test
    frame_data_log->write_file(log_mask_detail, set_void(frame_data_log),
        "[%s::%s]start dump hash table\n", __FILE__, __FUNCTION__);
    frame_hash_dump(frame_data_log, frame_data.hash_table);
#endif
    return swap_count;
}
//static function end
//////////////////////////////////////////////////////////////////////////
int32_t frame_data_create(record_t* log, key_t key, int32_t size, int32_t count)
{
    if (0 >= size || 0 >= count || NULL == log)
    {
        return fail;
    }
    size_t real_size;
    frame_data.data_size = size;
    frame_data.data_count = count;
    frame_data.key = key;
    frame_data_log = log;
    
    //先计算下索引区要占用的内存
    real_size = frame_list_size(count);
    //再计算实际数据区要占用的空间
    real_size += count * size;
    //申请共享内存
    frame_data.shm = frame_shm_create(key, real_size);
    if (NULL == frame_data.shm)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]create share memory(size:%ld) fail\n", __FILE__,
            __FUNCTION__, real_size);
        return fail;
    }
    log->write_file(log_mask_detail, (void*)log,
        "[%s::%s]create share memory(size:%ld) success\n", __FILE__,
        __FUNCTION__, real_size);

    frame_data.recycled = (list_t*) malloc(sizeof(list_t));
    if (NULL == frame_data.recycled)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]malloc recycled list(size:%d) fail\n", __FILE__,
            __FUNCTION__, sizeof(list_t));
        return fail;
    }
    memset(frame_data.recycled, 0, sizeof(list_t));
    frame_data.unused = (list_t*) malloc(sizeof(list_t));
    if (NULL == frame_data.unused)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]malloc unused list(size:%d) fail\n", __FILE__,
            __FUNCTION__, sizeof(list_t));
        return fail;
    }
    memset(frame_data.unused, 0, sizeof(list_t));
    frame_data.hash_table = (dict_t*) malloc(sizeof(dict_t));
    if (NULL == frame_data.hash_table)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]malloc used dict(size:%d) fail\n", __FILE__,
            __FUNCTION__, sizeof(dict_t));
        return fail;
    }
    memset(frame_data.hash_table, 0, sizeof(dict_t));

    if (fail == frame_data_init(log, &frame_data))
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]frame data init fail\n", __FILE__, __FUNCTION__);
        return fail;
    }

    return success;
}
int32_t frame_data_destroy()
{
    if (NULL != frame_data.shm)
    {
        frame_shm_destroy(frame_data.key);
        free(frame_data.shm);
        frame_data.shm = NULL;
    }
    if (NULL != frame_data.recycled)
    {
        free(frame_data.recycled);
        frame_data.recycled = NULL;
    }
    if (NULL != frame_data.unused)
    {
        free(frame_data.unused);
        frame_data.unused = NULL;
    }
    if (NULL != frame_data.hash_table)
    {
        frame_data.hash_table->hash_destroy((void*)frame_data.hash_table);
        free(frame_data.hash_table);
        frame_data.hash_table = NULL;
    }
    if (NULL != frame_data.start)
    {
        frame_data.start = NULL;
    }
    return success;
}

//void* frame_data_get_by( int32_t key, int32_t create )
void* frame_data_get_by( uint64_t key, int32_t create )
{
    void* find = NULL;
    //先在hashtable中找，看是否在表中
    node_t* node = frame_data.hash_table->hash_find((void*)frame_data.hash_table, key);
    if (NULL != node)
    {
        frame_data_log->write_file(log_mask_detail, (void*)frame_data_log,
            "[%s::%s]find key(=%"UINT64_FMT") in hash table,so return\n", __FILE__,
             __FUNCTION__, key);
        //将这个用户放到待回收队列队尾
        frame_list_exchange(frame_data.recycled, node, LIST_FROM_TAIL);
        find = get_data(frame_data.start, node->id, frame_data.data_size);
    }
    else if (NULL == node && IS_OK == create)   //没找到，需要重新申请
    {
        //如果已经没有剩余的空间了，需要回收一部分
        if (0 >= frame_data.unused->size)
        {
            frame_data_log->write_file(log_mask_warning, (void*)frame_data_log,
                "[%s::%s]there is no more nodes to use,so it's time to swap"
                " some unactive nodes out,used(%d),free(%d)\n", __FILE__,
                __FUNCTION__, frame_data.recycled->size, frame_data.unused->size);
            int32_t swap_count = frame_data_swap_recycle(RECYCLE_ONCE);
            frame_data_log->write_file(log_mask_detail, (void*)frame_data_log,
                "[%s::%s]swap recycle cache count(%d)\n", __FILE__, __FUNCTION__,
                swap_count);
        }
        node = frame_data.unused->list_pop((void*)frame_data.unused,
            LIST_FROM_HEAD);
        frame_data_log->write_file(log_mask_detail, set_void(frame_data_log),
            "[%s::%s]dump pop node info(id:%d,index[prev:%d,next:%d],hash "
            "used[val:%"PRIu64",index[prev:%d,next:%d]])\n", __FILE__,
            __FUNCTION__, node->id, node->links.prev, node->links.next,
            node->hash_use.val, node->hash_use.links.prev,
            node->hash_use.links.next);
        //将数据加入到hashtable中
        frame_data.hash_table->hash_add((void*)frame_data.hash_table, node, key);
        //将数据加入到待回收队列
        frame_data.recycled->list_push((void*)frame_data.recycled, node,
            LIST_FROM_TAIL);
        find = get_data(frame_data.start, node->id, frame_data.data_size);
        //初始化下数据
        memset(find, 0, frame_data.data_size);
    }
#if defined(_DEBUG) || defined(DEBUG)
    //just for test
    frame_data_log->write_file(log_mask_detail, set_void(frame_data_log),
        "[%s::%s]start dump hash table info:\n", __FILE__, __FUNCTION__);
    frame_hash_dump(frame_data_log, frame_data.hash_table);
    frame_data_log->write_file(log_mask_detail, (void*)frame_data_log,
        "[%s::%s]start dump unused list info:\n", __FILE__, __FUNCTION__);
    frame_list_dump(frame_data_log, frame_data.unused);
    frame_data_log->write_file(log_mask_detail, (void*)frame_data_log,
        "[%s::%s]start dump recycled list info:\n", __FILE__, __FUNCTION__);
    frame_list_dump(frame_data_log, frame_data.recycled);
#endif
    return find;
}

int32_t frame_data_recycle( uint64_t key )
{
    //先找出来相应的node
    node_t* node = frame_data.hash_table->hash_find(set_void(frame_data.hash_table), key);
    if (NULL == node)
    {
        return fail;
    }
    //在hashtable中删除掉该节点
    frame_data.hash_table->hash_del(set_void(frame_data.hash_table), node);
    //在已用的list中删除掉该信息
    frame_data.recycled->list_del(set_void(frame_data.recycled), node);
    //将数据加入到未使用队列
    frame_data.unused->list_push(set_void(frame_data.unused), node,
        LIST_FROM_HEAD);
    return success;
}
