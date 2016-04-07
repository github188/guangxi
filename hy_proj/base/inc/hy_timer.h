/******************** (C) COPYRIGHT 2014 HY ********************
* File Name          : hy_timer.h
* Author             :
* Revision           :
* Date               :
* Description        : timer relealize source file
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

#ifndef _HY_TIMER_H_
#define _HY_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "pthread.h"

#include "hy_list.h"
#include "hy_type.h"

typedef enum
{
    HY_SINGLE_TIRGGER,
    HY_CIRCLE_TIRGGER,
} hy_timer_t_type_e;

/*  timer structure */
typedef struct _hy_timer_t
{
    struct list_head node;
    hy_u32 timer_type;           //循环触发,还是单触发
    hy_u32 expires;
    hy_u32 save_expires_value;   //循环触发时保存的时间值
    HY_BOOL is_inited;
    void (*function) (void *);
    void *parm;
} hy_timer_t;

/* timer management structure*/
typedef struct _hy_timer_mgt_t
{
    struct list_head timer_head;
    pthread_mutex_t timer_mutex;
    HY_BOOL ontime;
    HY_BOOL is_inited;
    pthread_mutex_t timer_mgt_condition_mutex;//is
    pthread_cond_t timer_mgt_condition;
} hy_timer_mgt_t;

//init mgt
hy_s32 hy_init_timer_mgt();
//void hy_show_timer(FILE * stream, hy_char *tile);
void hy_release_timer_mgt(void);
//malloc hy_timer_t and init it.if malloc falied return NULL
hy_timer_t *hy_init_timer(hy_u32 timer_type, void (*fun) (void *), void *parm);
//set relative time to hy__timer_t -> expires and add it to node list
hy_s32 hy_start_timer(hy_timer_t *tp, hy_u32 time);
//stop timer: del timer from node list and free it
hy_s32 hy_stop_timer(hy_timer_t *tp);

//timer thread manupuation
void *hy_start_timer_thread();
void *hy_stop_timer_thread();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
