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
//    hy_char ip[24];
//    hy_u16 port;
//    HY_BOOL b_thread_running;
//    HY_BOOL b_is_connected;
//    hy_s32 fd;
//    pthread_t pid;


/*thread handler*/
typedef void *(*thread_func_t)(void *);
typedef struct _hy_thread_handler
{
    pthread_t pid;
    HY_BOOL b_thread_running;
    thread_func_t thread_func;
    void * data;//self-data
} hy_thread_handler_t;

//disciption:thread_handler data  malloc
void hy_init_thread(hy_thread_handler_t *thread_handler, thread_func_t func, void * data);
hy_s32 hy_start_thread(hy_thread_handler_t *thread_handler);
//free thread_handler data  malloc
void hy_stop_thread(hy_thread_handler_t *thread_handler);

#ifdef __cplusplus
}
#endif

#endif
