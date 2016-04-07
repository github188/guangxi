/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          : hy_422_protocol.h
* Author             :
* Revision           :
* Date              :
* Description        : hy_422_protocol header file
* Change log		: 1.Initial version :2015-04-15 by JimmyZhang
*                   ++++++++++++++++++
********************************************************************************
* THE PRESENT SOFTWARE IS DISTRIBUTED IN THE HOPE THAT IT WILL PROVIDE CUSTOMERS
* WITH CODING INFORMATION TO SPEED UP THEIR PRODUCT DEVELOPMENT, BUT WITHOUT
* ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS
* FOR A PARTICULAR PURPOSE. AS A RESULT, TECHWELL SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef HY_SERIAL_CTRL_PROTOCOL_H
#define HY_SERIAL_CTRL_PROTOCOL_H
#include <stdbool.h>

#include "hy_type.h"

#ifdef __cplusplus
extern "C" {
#endif


//define the type of datagram transformed by rs422
static const hy_u8 HY_RS422_ALARM_STATUS = 0X01;
static const hy_u8 HY_RS422_POWER_MANAGEMENT = 0X02;
static const hy_u8 HY_RS422_DSP_STATUS = 0X03;


//define the type of date transformed by rs422
static const hy_u8 HY_RS422_CAR_INVADE_ALARM = 01;//工程车告警
static const hy_u8 HY_RS422_FLOAT_OBJECT_ALARM = 02;

static const hy_u8 HY_RS422_CAMERAL1_POWER = 20;
static const hy_u8 HY_RS422_CAMERAL2_POWER = 21;
static const hy_u8 HY_RS422_CAMERAL3_POWER = 22;
static const hy_u8 HY_RS422_HORN_POWER = 23;//喇叭电源
static const hy_u8 HY_RS422_3G_POWER = 24;
static const hy_u8 HY_RS422_A9_POWER = 25;
static const hy_u8 HY_ball_camera_POWER = 26;

//告警级别内容
typedef enum
{
    CLOSE_ALARM = 0x00,
    ALARM_LEVEL_1 = 0X01,
    ALARM_LEVEL_2 = 0X02,
    ALARM_LEVEL_3 = 0X03,
} ALARM_LEVER_E_T;
//电源管理内容
typedef enum
{
    POWER_MANAGEMENT_CLOSE = 0X00,
     POWER_MANAGEMENT_OPEN = 0x01,
    POWER_MANAGEMENT_RESTART = 0X02,
} POWER_MANAGEMENT_E_T;

//根据指令类型填充buf
void fill_in_rs422_date_buf(hy_u8 *buf, hy_u32 *buf_size, \
                        hy_u8 datagram_type, hy_u8 data_type, hy_u8 data_context);
//检查校验和,校验成功返回true
bool check_checksum(hy_u8 *buf, hy_u32 buf_size, hy_u8 checksum);

//喇叭电源管理
void restart_horn_power_by_uart(hy_u8 *buf, hy_u32 *buf_size);
void close_horn_power_by_uart(hy_u8 *buf, hy_u32 *buf_size);
void open_horn_power_by_uart(hy_u8 *buf, hy_u32 *buf_size);
void open_ball_camera_power_by_uart(hy_u8 *buf, hy_u32 *buf_size);
//串口相关函数
hy_s32 tty_cmd_open(void);//返回告警联动相关的串口
void open_horn_power(hy_s32 fd);
void close_horn_power(void *p_fd);//用于定时器回调
void open_ball_camera_power(hy_s32 fd);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif