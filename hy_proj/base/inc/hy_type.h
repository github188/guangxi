/******************** (C) COPYRIGHT 2014 HY ********************
* File Name          : hy_type.h
* Author             : 
* Revision           : 
* Date              : 
* Description        : Data type define header file 
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

#ifndef _HY_TYPE_H_
#define _HY_TYPE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include<sys/types.h>

//数据类型s
typedef unsigned char   hy_u8; 

typedef char            hy_char;

typedef char            hy_s8;

typedef int16_t         hy_s16;

typedef u_int16_t       hy_u16;

typedef int32_t         hy_s32;

typedef u_int32_t       hy_u32;

typedef int64_t         hy_s64;

typedef u_int64_t       hy_u64;

typedef float           hy_f32;
typedef double  hy_d64;

#define hy_void         void

#define HY_FAILTURE     (-1)

#define HY_SUCCESS       (0)


typedef enum{

    HY_FALSE    = 0,
    HY_TRUE     = 1

}HY_BOOL;



#ifdef __cplusplus
}
#endif //__cplusplus


#endif  //_HY_DATA_TYPE_H_
