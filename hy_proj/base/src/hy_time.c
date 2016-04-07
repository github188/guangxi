#include "hy_time.h"

time_t hy_get_cur_time_s()
{
    time_t t;
    t = time(NULL);
    return t;
}

struct tm *hy_get_cur_tm()
{
    time_t timer;
    timer = time(NULL);

    return localtime(&timer);
}

long hy_get_cur_time_ms()
{
    struct  timeval    tv;
    struct  timezone   tz;
    gettimeofday(&tv,&tz);

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
