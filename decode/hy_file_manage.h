/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        : send picture or record to server
* Change log		 :  Jimmy2015-7-15
********************* (C) COPYRIGHT 2015 HY *******************/
#ifndef _HY_FILE_MANAGE_H
#define _HY_FILE_MANAGE_H
#include "hy_type.h"
#include <time.h>
#include <sys/time.h>

/*
该结构体用于传送图片到A9
*/
typedef struct _hy_alarm_file_info
{
	unsigned int 		flag;			//0x646101BB
	unsigned int		ipc;			//IPC从1开始编号
	unsigned int 		size;		 	//数据大小
	time_t alarm_time;
//	int year;
//	int month;
//	int day;
//	int hour;
//	int min;
//	int sec;
} hy_alarm_file_info_t;

#ifdef __cplusplus
extren "C"
{
#endif

void *hy_file_manage_thread_cb(void *s);
void *hy_start_file_manage_thread();
void *hy_stop_file_manage_thread();

void *hy_data_handle_pic_record_task(void *param);
void *hy_data_handle_video_record_task(void *param);

#ifdef __cplusplus
}
#endif

#endif
