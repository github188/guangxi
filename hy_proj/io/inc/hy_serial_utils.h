

/******************** (C) COPYRIGHT 2014 HY ********************
* File Name          : hy_stream_ctl.h
* Author             :
* Revision           :
* Date              :
* Description        : stream_ctl header file
* Change log		: 1.Initial version :2014-04-12 by zcx
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

#ifndef _HY_SERIAL_H_
#define _HY_SERIAL_H_

#include "hy_type.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define RS422_BAURRATE  (38400)
//#define RS422_DATABIT   (8)
//#define RS422_STOPBIT   (1)

//#define RS422_DEV "/dev/ttyS0"

hy_s32 set_tty_param(hy_s32 fd, hy_s32 speed, hy_s32 databits, hy_s32 stopbits, hy_s32 parity);
hy_s32 tty_open(hy_char *dev);
hy_s32 tty_recv(hy_s32 fd, hy_s8 *rcv_buf, hy_s32 data_len);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif  //_HY_RS422_H_

