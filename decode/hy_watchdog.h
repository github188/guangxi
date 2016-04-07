/******************** (C) COPYRIGHT 2014 HY ********************
* File Name          : hy_watchdog.h
* Author             :
* Revision           :
* Date              :
* Description        : watchdog file
* Change log		: 1.Initial version :2014-03-27 by zcx
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

#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include "hy_type.h"

#ifdef __cplusplus
extern "C" {
#endif

void *hy_start_watchdog_thread();
void *hy_stop_watchdog_thread();

#ifdef __cplusplus
}
#endif //__cplusplus


#endif  //_VIDEO_H_
