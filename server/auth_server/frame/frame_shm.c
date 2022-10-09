#include "frame_shm.h"

shm_t* frame_shm_create( key_t key, size_t size )
{
    int shm_id;
    shm_t* shm = NULL;
    shm = (shm_t*) malloc(sizeof(shm_t));
    if (NULL == shm)
    {
        return NULL;
    }
    shm_id = shmget(key, size, IPC_CREAT|IPC_EXCL|0666);
    //创建出错了
    if (0 > shm_id)
    {
        if (EEXIST != errno)
        {
            goto err_step;
        }
        //再创建一次
        shm_id = shmget(key, size, 0666);
        //还是失败
        if (0 > shm_id)
        {
            //再创建，不指定大小
            shm_id = shmget(key, 0, 0666);
            if (0 > shm_id)
            {
                //只能失败了
                goto err_step;
            }
            else
            {
                //删除掉，因为大小不符合
                if (0 > shmctl(shm_id, IPC_RMID, NULL))
                {
                    goto err_step;
                }
                //再重新创建一次
                shm_id = shmget(key, size, IPC_CREAT|IPC_EXCL|0666);
                if (0 > shm_id)
                {
                    goto err_step;
                }
            }
        }
    }
    shm->data = shmat(shm_id, NULL, 0);
    shm->size = size;
    shm->used = 0;
    return shm;

err_step:
    if (NULL != shm)
    {
        free(shm);
        shm = NULL;
    }
    return NULL;
}

void* frame_shm_alloc( shm_t* shm, size_t size )
{
    if ( NULL == shm || 0 >= size)
    {
        return NULL;
    }
    void* ptr = NULL;
    if (NULL == shm->data)
    {
        return NULL;
    }
    if (size > (shm->size - shm->used))
    {
        return NULL;
    }
    ptr = (void*) (shm->data + shm->used);
    shm->used += size;
    return ptr;
}

int32_t frame_shm_destroy( key_t key )
{
    int shm_id = 0;
    shm_id = shmget(key, 0, 0666);
    if (0 > shm_id)
    {
        return fail;
    }
    if (0 != shmctl(shm_id, IPC_RMID, NULL))
    {
        return fail;
    }
    return success;
}
