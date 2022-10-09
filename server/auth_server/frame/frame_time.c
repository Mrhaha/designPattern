#include "frame_time.h"

int32_t frame_time_get_datetime( time_t times, char* src, int32_t len )
{
    if (NULL == src || 0 >= len)
    {
        return fail;
    }
    if (0 >= times)
    {
        times = time(NULL);
    }
    struct tm now_tm;
    int32_t ret = 0;
    if (NULL != localtime_r(&times, &now_tm))
    {
        ret = snprintf(src, len-1, "%04d-%02d-%02d %02d:%02d:%02d",
            now_tm.tm_year+1900, now_tm.tm_mon+1, now_tm.tm_mday,
            now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec);
    }
    src[ret] = '\0';

    return success;
}

int32_t frame_time_get_date_str( time_t times, char* src, int32_t len, int32_t type )
{
    if (NULL == src || 0 >= len)
    {
        return fail;
    }
    if (0 >= times)
    {
        times = time(NULL);
    }
    struct tm now_tm;
    int32_t ret = 0;
    if (NULL != localtime_r(&times, &now_tm))
    {
        if (RECORD_BILL == type)
        {
            ret = snprintf(src, len-1, "%04d-%02d-%02d-%02d",
                now_tm.tm_year+1900, now_tm.tm_mon+1, now_tm.tm_mday,
                now_tm.tm_hour);
        }
        else if (RECORD_LOG == type)
        {
            ret = snprintf(src, len-1, "%04d-%02d-%02d",
                now_tm.tm_year+1900, now_tm.tm_mon+1, now_tm.tm_mday);
        }
    }
    src[ret] = '\0';
    return success;
}
