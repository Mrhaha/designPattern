#include "frame_proctitle.h"
static char* frame_proctitle_last = NULL;
static char** frame_proctitle_argv;
extern char** environ;
/*****
 *Linux中argv和environ是连续的一段内存，而用ps命令显示进程名是argv控制的
 *这里的解决方式是将environ移到别的地方，将argv[0]占用整个argv和environ空间
 *这样，就可以显示更多的内容，需要注意的是，argv[0]的长度也是有限，不能太过离谱
 ****/
int32_t frame_proctitle_init(record_t* log, char** argv)
{
    char* p;
    size_t size = 0;
    int32_t i;
    frame_proctitle_argv = argv;

    //先求出environ的总长度
    for (i=0; environ[i]; ++i)
    {
        size += strlen(environ[i]) + 1;
    }
    p = (char*) malloc(size);
    if (NULL == p)
    {
        log->write_file(log_mask_error, (void*)log,
            "[%s::%s]malloc size(%d) return NULL\n", __FILE__, __FUNCTION__,
            size);
        return fail;
    }
    //遍历参数数组
    frame_proctitle_last = argv[0];
    for (i=0; argv[i]; ++i)
    {
        if (frame_proctitle_last == argv[i])
        {
            frame_proctitle_last = argv[i] + strlen(argv[i]) + 1;
        }
    }
    //将environ表中的数据copy到新申请的连续内存中
    for (i=0; environ[i]; ++i)
    {
        if (frame_proctitle_last == environ[i])
        {
            size = strlen(environ[i]) + 1;
            frame_proctitle_last = environ[i] + size;

            snprintf(p, size, "%s", environ[i]);
            environ[i] = (char*) p;
            p += size;
        }
    }
    --frame_proctitle_last;
    return success;
}

int32_t frame_proctitle_set( char* prefix, char* title )
{
    char* p = frame_proctitle_argv[0];
    frame_proctitle_argv[1] = NULL;

    int32_t ret = snprintf((char*)p, frame_proctitle_last - p, "%s", prefix);
    p += ret;

    ret = snprintf(p, frame_proctitle_last - p, "%s", title);
    p += ret;
    if (0 < (frame_proctitle_last - p))
    {
        memset(p, '\0', (frame_proctitle_last - p));
    }
    return success;
}

