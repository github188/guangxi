/******************** (C) COPYRIGHT 2014 HY ********************
* File Name          : hy_debug.h
* Author             :
* Revision           :
* Date               :
* Description        : Debug header file
* Change log		 : 1.Initial version :2014-03-27 by zcx
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


#ifndef  _HY_DEBUG_H
#define  _HY_DEBUG_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


#include "hy_type.h"
#include "hy_time.h"


#define BUG_NORMAL 0
#define BUG_WARNING 1
#define BUG_ERROR 	2

#define DEBUG_BUFFER_SIZE (2048)

typedef enum{
    HY_DBG_I    =  0x01,
    HY_DBG_W    =  0x02,
    HY_DBG_D    =  0x03,
    HY_DBG_E    =  0x04

}hy_dbg_level_e;

typedef enum{

    HY_DBG_PRINT = 0x01,
    HY_DBG_FILE

}hy_dbg_mode_e;

#define HY_DEBUG

#ifdef HY_DEBUG

  #define hy_assert(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            printf("\nassert failed at:\n"\
                   "  >File name: %s\n"   \
                   "  >Function : %s\n"   \
                   "  >Line No. : %d\n"   \
                   "  >Condition: %s\n",  \
                    __FILE__,__FUNCTION__, __LINE__, #expr);\
            _exit(-1);\
        } \
    }while(0)

#else

    #define hy_assert(expr)

#endif

#ifdef HY_DEBUG
#define hy_print(fmt...)   \
    do {\
        struct tm * p_tm = hy_get_cur_tm();\
        printf("[%d-%02d-%02d %02d:%02d:%02d] ",p_tm->tm_year+1900, p_tm->tm_mon + 1, p_tm->tm_mday, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);\
        printf("[%s:%s]-%d: ",__FILE__,__FUNCTION__, __LINE__);\
        printf(fmt);\
    }while(0)
#else
#define  hy_print(fmt...)
#endif

#ifdef HY_DEBUG
#define hy_simple_debug() \
do{\
   fprintf(stderr,"file:{%s},func:[%s],line:-%d.\n",__FILE__,__FUNCTION__,__LINE__); \
}while(0)
#else
#define hy_simple_debug()
#endif


#ifdef __cplusplus
}
#endif

#endif
