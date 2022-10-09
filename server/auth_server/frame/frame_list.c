#include "frame_list.h"
static int32_t frame_list_push(void* data, node_t* node, int32_t type);
static node_t* frame_list_pop(void* data, int32_t type);
static int32_t frame_list_del(void* data, node_t* node);

static int32_t frame_list_push(void* data, node_t* node, int32_t type)
{
    if (NULL == data || NULL == node)
    {
        return fail;
    }
    list_t* l = (list_t*)data;
    //如果是空链表
    if (0 >= l->size)
    {
        node->links.next = node->links.prev = INVALID_INDEX;
        l->head = l->tail = node;
    }
    else
    {
        node_t* p = NULL;
        //如果是添加在头部
        if (LIST_FROM_HEAD == type)
        {
            node->links.prev = INVALID_INDEX;
            //保存原始头部
            p = l->head;
            if (p == node)
            {
                return success;
            }
            p->links.prev = node->id;
            node->links.next = p->id;
            //修改新头部
            l->head = node;
        }
        else if (LIST_FROM_TAIL == type) //加在尾部
        {
            node->links.next = INVALID_INDEX;
            p = l->tail;
            if (p == node)
            {
                return success;
            }
            p->links.next = node->id;
            node->links.prev = p->id;
            l->tail = node;
        }
    }
    ++l->size;

    return success;
}
static node_t* frame_list_pop(void* data, int32_t type)
{
    if (NULL == data)
    {
        return NULL;
    }
    list_t* l = (list_t*) data;
    node_t* p = NULL;
    //没有空余的数据了
    if (0 >= l->size)
    {
        return p;
    }
    if (LIST_FROM_HEAD == type)
    {
        //取队列的第一个元素
        p = l->head;
        //判断下一个是否存在
        if (p->links.next != INVALID_INDEX)
        {
            node_t* n = get_list_node(l, p->links.next);
            n->links.prev = INVALID_INDEX;
            l->head = n;
        }
        else //已经到达队尾
        {
            l->head = l->tail = NULL;
        }
    }
    else if (LIST_FROM_TAIL == type)
    {
        p = l->tail;
        //判断上一个是否存在
        if (p->links.prev != INVALID_INDEX)
        {
            node_t* last = get_list_node(l, p->links.prev);
            last->links.next = INVALID_INDEX;
            l->tail = last;
        }
        else
        {
            l->head = l->tail = NULL;
        }
    }
    --l->size;
    return p;
}
static int32_t frame_list_del(void* data, node_t* node)
{
    if (NULL == data || NULL == node)
    {
        return fail;
    }
    list_t* l = (list_t*) data;
    node_t* pre = NULL;
    node_t* nex = NULL;
    if (INVALID_INDEX != node->links.prev)
    {
        pre = get_list_node(l, node->links.prev);
        pre->links.next = node->links.next;
    }
    else
    {
        l->head = (INVALID_INDEX == node->links.next) ? NULL : 
            get_list_node(l, node->links.next);
    }
    if (INVALID_INDEX != node->links.next)
    {
        nex = get_list_node(l, node->links.next);
        nex->links.prev = node->links.prev;
    }
    else
    {
        l->tail = (INVALID_INDEX == node->links.prev) ? NULL :
            get_list_node(l, node->links.prev);
    }
    --l->size;

    return success;
}
//static function end
//////////////////////////////////////////////////////////////////////////
int32_t frame_list_init(void* data, node_t* start, int32_t size)
{
    if (NULL == data || NULL == start)
    {
        return fail;
    }
    int32_t i;
    list_t* l = (list_t*)data;
    l->start = start;
    l->list_del = frame_list_del;
    l->list_pop = frame_list_pop;
    l->list_push = frame_list_push;
    if (0 >= size)
    {
        l->head = NULL;
        l->tail = NULL;
        l->size = 0;
        return success;
    }
    l->head = start;
    l->size = size;
    for (i = 0; i < size; ++i,++start)
    {
        frame_list_hash_used_reset(&start->hash_use);
        start->id = i;
        if (0 == i)
        {
            start->links.prev = INVALID_INDEX;
            start->links.next = i + 1;
        }
        else
        {
            start->links.prev = i - 1;
            start->links.next = i + 1;
        }
    }
    l->tail = --start;
    l->tail->links.next = INVALID_INDEX;

    return success;
}

int32_t frame_list_hash_used_reset( hash_used_t* hut )
{
    if (NULL == hut)
    {
        return fail;
    }
    hut->links.next = hut->links.prev = INVALID_INDEX;
    hut->val = INVALID_INDEX;
    return success;
}

size_t frame_list_size( int32_t count )
{
    return (size_t)(count * sizeof(node_t));
}

int32_t frame_list_exchange( list_t* list, node_t* node, int32_t type )
{
    if (NULL == list || NULL == node)
    {
        return fail;
    }
    //先删除掉该node
    list->list_del((void*)list, node);
    //然后将该node加入到队列
    list->list_push((void*)list, node, type);

    return success;
}

int32_t frame_list_dump( record_t* log, list_t* list )
{
    if (NULL == log || NULL == list)
    {
        return fail;
    }
    int32_t i=0;
    node_t* start = list->head;
    while (start)
    {
        log->write_file(log_mask_detail, (void*)log,
            "[%s::%s][%d:%d]obj info:id:%d,prev:%d,next:%d\n", __FILE__,
            __FUNCTION__, i, list->size, start->id, start->links.prev,
            start->links.next);
        ++i;
        start = (INVALID_INDEX == start->links.next) ? NULL:
            (get_list_node(list, start->links.next));
    }
    return success;
}
