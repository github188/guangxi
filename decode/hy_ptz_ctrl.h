/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :封装PTZ控制指令，使用户能够更加方便的使用
* Change log		 :  Jimmy2015-7-10
********************* (C) COPYRIGHT 2015 HY *******************/

#ifndef _HY_PTZ_CTRL_H
#define _HY_PTZ_CTRL_H
#include "hy_type.h"

#ifdef __cplusplus
extren "C"
{
#endif

/**********************************
*Name:ptz_ctrl_init
*Desc:
*Param:
*Return: -1 初始化失败，程序不应该继续往下进行
         0  初始化成功，之后可以进行PTZ操作
*Author:Jimmy
*Create Time:2015-7-10
*ChangeLog:
**********************************/
int ptz_ctrl_init();

/**********************************
*Name:ptz_ctrl_release
*Desc:
*Param:释放ONVIF资源
*Return:
*Author:Jimmy
*Create Time:2015-7-10
*ChangeLog:
**********************************/
void ptz_ctrl_release();

/**********************************
*Name:simple_ptz_move_up
*Desc:
*Param:channel 要操作的通道号  取值小于4
       step    操作执行过程中的速度 取值为0-1之间
*Return:
*Author:Jimmy
*Create Time:2015-7-10
*ChangeLog:
**********************************/
void simple_ptz_move_up(hy_u8 channel, hy_f32 step);
void simple_ptz_move_down(hy_u8 channel, hy_f32 step);
void simple_ptz_move_left(hy_u8 channel, hy_f32 step);
void simple_ptz_move_right(hy_u8 channel, hy_f32 step);
void simple_ptz_move_left_up(hy_u8 channel, hy_f32 step);
void simple_ptz_move_left_down(hy_u8 channel, hy_f32 step);
void simple_ptz_move_right_up(hy_u8 channel, hy_f32 step);
void simple_ptz_move_right_down(hy_u8 channel, hy_f32 step);
void simple_ptz_zoom_in(hy_u8 channel, hy_f32 step);
void simple_ptz_zoom_out(hy_u8 channel, hy_f32 step);
void simple_ptz_move_stop(hy_u8 channel);

void simple_ptz_set_preset(hy_u8 channel, hy_u32 preset_id);
void simple_ptz_clear_preset(hy_u8 channel, hy_u32 preset_id);
void simple_ptz_goto_preset(hy_u8 channel, hy_u32 preset_id);

#ifdef _cplusplus
}
#endif


#endif

