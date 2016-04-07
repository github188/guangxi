/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :hy_session_handler
* Author             :
* Revision           :
* Description        :建立同服务器的连接、登陆、收发数据、处理请求。
* Change log		 :  Jimmy2015-7-13
********************* (C) COPYRIGHT 2015 HY *******************/
#ifndef _HY_SESSION_H
#define _HY_SESSION_H
#include "hy_type.h"
#include "hy_protocl.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _hy_session_handler
{
    hy_char ip[24];
    hy_u16 port;
    HY_BOOL b_thread_running;
    HY_BOOL b_is_connected;
    hy_s32 fd;
    pthread_t pid;
} hy_session_t;
extern hy_session_t hy_sig_session;


typedef struct _hy_mesg_buf_t
{
    hy_u32 current_head_num;//already readed num
    hy_u32 current_contex_num;
    hy_u32 need_head_num;//head neeed
    hy_u32 need_context_num;//conntext need
    M_HY_HEADER head;
    hy_char mesg_context[1024];
} hy_mesg_buf_t;

hy_s32 hy_sig_session_handle_thread_start();
void hy_stop_sig_session( void );
void *real_time_stream_request_thread(void *s);

void hy_sig_init_mesg_buf(hy_mesg_buf_t *mesg_buf);
//return -2 not finish
//return -1 read error
//return 0  finish
hy_s32 hy_sig_read_mesg_buf(hy_s32 fd, hy_mesg_buf_t *mesg_buf);

#ifdef __cplusplus
}
#endif

#endif

