/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        : send device status to server
* Change log		 :  Jimmy2015-7-15
********************* (C) COPYRIGHT 2015 HY *******************/
#ifndef _HY_DEVICE_STATUS_H
#define _HY_DEVICE_STATUS_H
#include "hy_type.h"

#ifdef __cplusplus
extren "C"
{
#endif
typedef struct _device_statu_thread_data_t
{
    void *p;
} device_statu_thread_data_t;

/*ÍøÂç×´Ì¬¼ì²é*/
int device_net_status_judge(char *ip);

void *hy_device_status_thread_cb(void *s);
void *hy_start_device_status_thread();
void *hy_stop_device_status_thread();

#ifdef __cplusplus
}
#endif

#endif
