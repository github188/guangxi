/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :
* Change log		 :  Jimmy2015-7-10
********************* (C) COPYRIGHT 2015 HY *******************/

#ifndef _HY_STREAM_CTRL_H
#define _HY_STREAM_CTRL_H
#include "hy_type.h"

#ifdef __cplusplus
extren "C"
{
#endif

/**********************************
*Name:hy_stream_init
*Desc:
*Param:
*Return:
*Author:Jimmy
*Create Time:2015-7-10
*ChangeLog:
**********************************/
int hy_simple_stream_init();
void hy_simple_stream_release();

/**********************************
*Name:hy_simple_get_original_frame
*Desc:
*Param: channel 要采集数据的通道号
        p_data  指向采集到的数据的指针的地址 采集失败时使指针指向NULL
        data_size 一帧数据的大小
*Return:
*Author:Jimmy
*Create Time:2015-7-10
*ChangeLog:
**********************************/
int hy_simple_stream_get_original_frame(hy_u8 channel, hy_s8 **p_data, hy_u32 *data_size);

int hy_simple_stream_record_video(hy_u8 channel, const hy_s8 *file_name, hy_u8 seconds);

int hy_simple_stream_caputre_picture(hy_u8 channel, const hy_s8 *file_name);

int hy_simple_stream_start_realplay(hy_char *p_ip, unsigned int dvr, unsigned int ipc, unsigned int stream);
//int hy_simple_stream_start_realplay(const hy_char *source_rtsp_addr, const hy_char *publish_addr);

int hy_simple_stream_stop_realplay();

int hy_init_rtsp_addrs();

#ifdef __cplusplus
}
#endif
#endif
