#include "frame_statis.h"
static statis_t frame_statis_info;

statis_t* frame_statis_get()
{
    return &frame_statis_info;
}

void frame_statis_dump( record_t* log )
{
    if (NULL == log)
    {
        return;
    }
    time_t now = time(NULL);
    //还是当前时间
    if (now != frame_statis_info.cur_pps_time)
    {
        char max_pps_date[MAX_PATH_LEN] = {0};
        char cur_pps_date[MAX_PATH_LEN] = {0};
        if (frame_statis_info.cur_pps > frame_statis_info.max_pps)
        {
            frame_statis_info.max_pps = frame_statis_info.cur_pps;
            frame_statis_info.max_pps_time = frame_statis_info.cur_pps_time;
        }
        frame_time_get_datetime(frame_statis_info.max_pps_time, max_pps_date,
            sizeof(max_pps_date));
        frame_time_get_datetime(frame_statis_info.cur_pps_time, cur_pps_date,
            sizeof(cur_pps_date));
        log->write_file(log_mask_performance, (void*)log,
            "[%s::%s][cur_time=%s,cur_pps=%d],[max_pps_time=%s,max_pps=%d],"
            "recv_count=%d,send_ok=%d,send_err=%d\n", __FILE__, __FUNCTION__,
            cur_pps_date, frame_statis_info.cur_pps, max_pps_date,
            frame_statis_info.max_pps, frame_statis_info.recv_count,
            frame_statis_info.send_ok, frame_statis_info.send_err);

        frame_statis_info.cur_pps = 0;
        frame_statis_info.cur_pps_time = now;
    }
    return;
}

void frame_statis_begin()
{
    ++frame_statis_info.cur_pps;
    ++frame_statis_info.recv_count;
    ++frame_statis_info.send_err;
}
void frame_statis_end()
{
    ++frame_statis_info.send_ok;
    --frame_statis_info.send_err;
}
