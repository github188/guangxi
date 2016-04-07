#include <signal.h>
#include <sys/time.h>

#include "hy_base_config.h"
#include "hy_type.h"
#include "hy_debug.h"

#include "hy_thread_utils.h"
#include "hy_timer.h"

#define TIMER_EXEC_INTERVAL 1

static hy_timer_mgt_t timer_mgt;
static hy_thread_handler_t *hy_thread_handler;

//init mgt
hy_s32 hy_init_timer_mgt()
{
    //init condition
    if (pthread_cond_init(&timer_mgt.timer_mgt_condition, NULL))
    {
        hy_print("init condition failed.\n");
        return -1;
    }
    //init mutex
    if (pthread_mutex_init(&timer_mgt.timer_mgt_condition_mutex, NULL))
    {
        hy_print("init mutex failed.\n");
        return -1;
    }
    //init mutex
    if (pthread_mutex_init(&timer_mgt.timer_mutex, NULL))
    {
        hy_print("init mutex failed.\n");
        return -1;
    }

    //init head
    INIT_LIST_HEAD(&timer_mgt.timer_head);
    timer_mgt.ontime = HY_FALSE;
    timer_mgt.is_inited = HY_TRUE;

    return 0;
}

//void hy_show_timer(FILE * stream, hy_char *tile);
void hy_release_timer_mgt(void);
//malloc hy_timer_t and init it.if malloc falied return NULL
hy_timer_t *hy_init_timer(hy_u32 timer_type, void (*fun) (void *), void *parm)
{
    hy_timer_t * p = (hy_timer_t *)hy_malloc(sizeof(hy_timer_t));
    if (p == NULL)
    {
        return NULL;
    }

    p->timer_type = timer_type;
    p->function = fun;
    p->parm = parm;
    p->save_expires_value = 0;
    p->expires = 0;

    p->is_inited = HY_TRUE;

    return p;
}

//set relative time to hy__timer_t -> expires and add it to node list
hy_s32 hy_start_timer(hy_timer_t *tp, hy_u32 time)
{
    if (tp == NULL || !tp->is_inited || time <= 0 || !timer_mgt.is_inited)
    {
        return -1;
    }

    tp->save_expires_value = time;
    tp->expires = time;
    struct list_head *new_node = (struct list_head *)tp;
    //add
    pthread_mutex_lock(&timer_mgt.timer_mutex);
    list_add_tail(new_node, &timer_mgt.timer_head);
    pthread_mutex_unlock(&timer_mgt.timer_mutex);

    return 0;
}
//stop timer: del timer from node list and free it(contain parm)
hy_s32 hy_stop_timer(hy_timer_t *tp)
{
    if (tp == NULL)
    {
        return -1;
    }

    //pthread_mutex_lock(&timer_mgt.timer_mutex);
    list_del(&tp->node);
    //pthread_mutex_unlock(&timer_mgt.timer_mutex);
    if (tp->parm)
        hy_free(tp->parm);
    hy_free(tp);

    return 0;
}

//used in other mutex so it do not using mutex
static hy_s32 hy_reset_timer(hy_timer_t *tp)
{
    tp->expires = tp->save_expires_value;
    return 0;
}


static void signal_handler_cb(int signal_num)
{
    hy_print("alarm on time.\n");
    timer_mgt.ontime = HY_TRUE;
    pthread_mutex_lock(&timer_mgt.timer_mgt_condition_mutex);
    pthread_cond_signal(&timer_mgt.timer_mgt_condition);
    pthread_mutex_unlock(&timer_mgt.timer_mgt_condition_mutex);
}

//timer thread manupuation
static void *hy_timer_thread_cb(void *s)
{
    hy_print("create hy timer thread success.\n");
    hy_thread_handler_t *thread_handler = (hy_thread_handler_t *)s;
    thread_handler->b_thread_running = HY_TRUE;

    /*set real timer*/
//    signal(SIGALRM, signal_handler_cb);
//    struct itimerval val;
//    val.it_value.tv_sec = TIMER_EXEC_INTERVAL;
//    val.it_value.tv_usec = 0;
//    val.it_interval.tv_sec = TIMER_EXEC_INTERVAL;
//    val.it_interval.tv_usec = 0;
//    setitimer(ITIMER_REAL, &val, NULL);

    while(thread_handler->b_thread_running)
    {
//        if (timer_mgt.ontime == HY_FALSE)
//        {
//            pthread_cond_wait(&timer_mgt.timer_mgt_condition, &timer_mgt.timer_mgt_condition_mutex);
//        }
//        timer_mgt.ontime = HY_FALSE;
        hy_msleep(1000 * TIMER_EXEC_INTERVAL);

        pthread_mutex_lock(&timer_mgt.timer_mutex);
        hy_timer_t *pos;
        //hy_simple_debug();
        for (pos = list_entry((&timer_mgt.timer_head)->next, typeof(*pos), node);        \
             prefetch(pos->node.next), &pos->node != (&timer_mgt.timer_head);)
        {
            pos->expires = pos->expires - TIMER_EXEC_INTERVAL;
            //hy_simple_debug();
            //hy_print("pos->expires = %d .\n", pos->expires);
            if (pos->expires <= 0)
            {
                pos->function(pos->parm);
                if (pos->timer_type == HY_SINGLE_TIRGGER)
                {
                    hy_timer_t *tmp_pos = pos;//used to del
                    pos = list_entry(pos->node.next, typeof(*pos), node);
                    hy_stop_timer(tmp_pos);
                    continue;
                }
                else
                {
                    hy_reset_timer(pos);
                }
            }

            //for condition
            pos = list_entry(pos->node.next, typeof(*pos), node);
        }
//        list_for_each_entry(pos, &timer_mgt.timer_head, node)
//        {
//            pos->expires = pos->expires - TIMER_EXEC_INTERVAL;
//            hy_simple_debug();
//            //hy_print("pos->expires = %d .\n", pos->expires);
//            if (pos->expires <= 0)
//            {
//                pos->function(pos->parm);
//                if (pos->timer_type == HY_SINGLE_TIRGGER)
//                {
//                    hy_stop_timer(pos);
//                }
//                else
//                {
//                    hy_reset_timer(pos);
//                }
//            }
//        }
        pthread_mutex_unlock(&timer_mgt.timer_mutex);

    }

    pthread_exit("hy timer thread normal exit");
}

void *hy_start_timer_thread()
{
    hy_thread_handler = (hy_thread_handler_t *)hy_malloc(sizeof(hy_thread_handler_t));
    hy_init_thread(hy_thread_handler, hy_timer_thread_cb, NULL);
    hy_start_thread(hy_thread_handler);
    return NULL;
}
void *hy_stop_timer_thread()
{
    hy_stop_thread(hy_thread_handler);
    return NULL;
}
