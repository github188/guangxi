/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :
* Change log		 :  Jimmy2015-7-21
********************* (C) COPYRIGHT 2015 HY *******************/
#include <pthread.h>

#include "hy_type.h"
#include "hy_debug.h"
#include "hy_list.h"

typedef enum _HY_MESG_TYPE_E
{
    HY_ALARM_MESG = 0,
} HY_MESG_TYPE_E;

typedef struct _hy_mesg_t
{
    struct list_head node;

    HY_MESG_TYPE_E mesg_type;
    HY_BOOL is_inited;
    void * mesg_buf;
    void ( * hy_release_mesg)(struct _hy_mesg_t *mesg);
} hy_mesg_t;

typedef struct _hy_mesg_deque_t
{
    HY_BOOL is_inited;
    hy_u32 mesg_nums;
    pthread_mutex_t deque_mutex;
    pthread_mutex_t deque_condition_mutex;//is there have mesg to get?
    pthread_cond_t deque_condition;

    struct list_head *head;//do not use head to contain mesg.

    void * (* hy_get_mesg)(struct _hy_mesg_deque_t *mesg_deque);
    void (* hy_put_mesg)(struct _hy_mesg_deque_t *mesg_deque, hy_mesg_t *mesg);
    void (* hy_release_mesg_deque)(struct _hy_mesg_deque_t *mesg_deque);

} hy_mesg_deque_t;

extern hy_mesg_deque_t *data_mesg_deque;

hy_s32 hy_init_mesg_deque_t(hy_mesg_deque_t *mesg_deque);
void hy_release_mesg_deque(struct _hy_mesg_deque_t *mesg_deque);


hy_s32 hy_init_mesg(hy_mesg_t *mesg, HY_MESG_TYPE_E mesg_type, void *mesg_context);
void hy_release_mesg(struct _hy_mesg_t *mesg);//after get mesg and use it  youo should release it
hy_s32 hy_put_mesg(struct _hy_mesg_deque_t *mesg_deque, hy_mesg_t *mesg);
hy_mesg_t* hy_get_mesg(struct _hy_mesg_deque_t *mesg_deque);
