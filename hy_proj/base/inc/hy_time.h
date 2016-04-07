#ifndef _HY_TIME_H
#define _HY_TIME_H
#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>





/*1970年以后的秒数*/
time_t hy_get_cur_time_s();

/*1970年以后的毫秒数*/
long hy_get_cur_time_ms();

/*使用完了之后，应该free p_tm*/
struct tm *hy_get_cur_tm();

#endif
