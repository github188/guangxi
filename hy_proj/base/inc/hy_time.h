#ifndef _HY_TIME_H
#define _HY_TIME_H
#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>





/*1970���Ժ������*/
time_t hy_get_cur_time_s();

/*1970���Ժ�ĺ�����*/
long hy_get_cur_time_ms();

/*ʹ������֮��Ӧ��free p_tm*/
struct tm *hy_get_cur_tm();

#endif
