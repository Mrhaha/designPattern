#include "frame_codequeue.h"
static int32_t frame_codequeue_init(code_queue_t* queue, char* buf, int32_t size);
static int32_t frame_codequeue_append(void* data, const char* src, int32_t len);
static int32_t frame_codequeue_pop(void* data, char* out, int32_t* len);
static int32_t frame_codequeue_head_tail_get(code_queue_t* queue, int32_t* head, int32_t* tail);
static int32_t frame_codequeue_head_tail_set(code_queue_t* queue, int32_t head, int32_t tail);
static int32_t frame_codequeue_free_size(code_queue_t* queue);

static int32_t frame_codequeue_append(void* data, const char* src, int32_t len)
{
    if (NULL == data || NULL == src || 0 >= len)
    {
        return -1;
    }
    code_queue_t* queue = (code_queue_t*) data;
    char* buf_start = &queue->buf[0];
    char* len_str = NULL;
    int32_t i;
    int32_t head = 0;
    int32_t tail = 0;
    int32_t free_size = frame_codequeue_free_size(queue);
    //队列满了
    if (0 >= free_size)
    {
        return -2;
    }
    frame_codequeue_head_tail_get(queue, &head, &tail);
    //队列内部出错了
    if (head < 0 || head >= queue->size
        || tail < 0 || tail >= queue->size)
    {
        head = 0;
        tail = 0;
        frame_codequeue_head_tail_set(queue, head, tail);
        return -3;
    }
    //判断剩余空间和即将放进去的数据大小
    if (free_size < (int32_t)sizeof(int32_t) + len)
    {
        return -4;
    }

    //将长度压入队尾
    len_str = (char*)&len;
    for (i=0; i < (int32_t)sizeof(int32_t); ++i)
    {
        buf_start[tail] = len_str[i];
        tail = (tail+1) % queue->size;
    }
    if (head > tail)
    {
        memcpy(&buf_start[tail], (const void*) src, (size_t)len);
    }
    else
    {
        //需要分段
        if (len > (queue->size - tail))
        {
            memcpy(&buf_start[tail], (const void*)src, (size_t)(queue->size - tail));
            memcpy(&buf_start[0], (const void*)(src + (queue->size - tail)),
                (size_t)(len - (queue->size - tail)));
        }
        else
        {
            memcpy(&buf_start[tail], (const void*) src, (size_t)len);
        }
    }
    tail = (tail + len) % queue->size;

    frame_codequeue_head_tail_set(queue, -1, tail);
    return success;
}
static int32_t frame_codequeue_pop(void* data, char* out, int32_t* len)
{
    if (NULL == data || NULL == out || 0 >= *len)
    {
        return -1;
    }
    code_queue_t* queue = (code_queue_t*)data;
    int32_t head = 0;
    int32_t tail = 0;
    int32_t code_size = (queue->size - frame_codequeue_free_size(queue));
    int32_t ori = *len;
    int32_t pop_len = 0;
    int32_t i;
    char* src = NULL;
    char* buf_start = &queue->buf[0];

    frame_codequeue_head_tail_get(queue, &head, &tail);
    //空队列
    if (head == tail)
    {
        return -2;
    }
    //剩余的大小比int32_t长度更小，则队列出现损坏
    if (code_size < (int32_t)sizeof(int32_t))
    {
        head = 0;
        tail = 0;
        *len = 0;
        frame_codequeue_head_tail_set(queue, head, tail);
        return -3;
    }

    //取当前buf的长度
    src = (char*)&pop_len;
    for (i=0; i < (int32_t)sizeof(int32_t); ++i)
    {
        src[i] = buf_start[head];
        head = (head + 1) % queue->size;
    }
    //长度不符合要求
    if (pop_len <= 0
        || pop_len > (code_size - (int32_t)sizeof(int32_t)))
    {
        *len = 0;
        head = 0;
        tail = 0;
        frame_codequeue_head_tail_set(queue, head, tail);
        return -4;
    }
    //如果输出长度不够
    if (ori < pop_len)
    {
        //跳过此段长度
        head = (head + pop_len) % queue->size;
        frame_codequeue_head_tail_set(queue, head, -1);
        return -5;
    }
    //到此处，所有的验证都完成了
    *len = pop_len;
    src = out;
    if (tail > head)
    {
        memcpy(src, (const void*)&buf_start[head], pop_len);
    }
    else
    {
        if (pop_len > (queue->size - head))
        {
            memcpy(src, (const void*)&buf_start[head], (queue->size - head));
            memcpy((void*)&src[queue->size-head], (const void*)&buf_start[0],
                (pop_len - (queue->size - head)));
        }
        else
        {
            memcpy(src, (const void*)&buf_start[head], pop_len);
        }
    }
    head = (head + pop_len) % queue->size;
    frame_codequeue_head_tail_set(queue, head, -1);

    return success;
}
static int32_t frame_codequeue_head_tail_get(code_queue_t* queue, int32_t* head, int32_t* tail)
{
    *head = queue->head;
    *tail = queue->tail;
    return success;
}
static int32_t frame_codequeue_head_tail_set(code_queue_t* queue, int32_t head, int32_t tail)
{
    if (head >= 0 && head < queue->size)
    {
        queue->head = head;
    }
    if (tail >= 0 && tail < queue->size)
    {
        queue->tail = tail;
    }
    return success;
}
static int32_t frame_codequeue_free_size(code_queue_t* queue)
{
    int32_t head = 0;
    int32_t tail = 0;
    int32_t free_size = 0;
    frame_codequeue_head_tail_get(queue, &head, &tail);

    if (head == tail)
    {
        free_size = queue->size;
    }
    else if (head > tail)
    {
        free_size = head - tail;
    }
    else
    {
        free_size = queue->size - (tail - head);
    }

    return free_size;
}
static int32_t frame_codequeue_init(code_queue_t* queue, char* buf, int32_t size)
{
    queue->buf = buf;
    queue->size = size;
    queue->head = queue->tail = queue->offset = 0;
    queue->append = frame_codequeue_append;
    queue->pop = frame_codequeue_pop;
    return success;
}
//static function end
//////////////////////////////////////////////////////////////////////////

code_queue_t* frame_codequeue_create( int32_t size )
{
    code_queue_t* queue = NULL;
    char* buf = NULL;
    if (0 >= size)
    {
        size = DEFAULT_CODE_QUEUE_SIZE;
    }
    if (MAX_CODE_QUEUE_SIZE < size)
    {
        size = MAX_CODE_QUEUE_SIZE;
    }
    queue = (code_queue_t*) malloc(sizeof(*queue));
    if (NULL == queue)
    {
        return NULL;
    }
    buf = (char*) malloc(size);
    if (NULL == buf)
    {
        free(queue);
        return NULL;
    }
    frame_codequeue_init(queue, buf, size);
    return queue;
}

int32_t frame_codequeue_destroy( code_queue_t* queue )
{
    if (NULL == queue)
    {
        return fail;
    }
    if (NULL != queue->buf)
    {
        free(queue->buf);
        queue->buf = NULL;
    }
    if (NULL != queue)
    {
        free(queue);
    }
    return success;
}

