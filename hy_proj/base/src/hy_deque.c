#include "hy_debug.h"
#include "hy_base_config.h"
#include "hy_deque.h"
hy_mesg_deque_t *data_mesg_deque = NULL;

void hy_release_mesg(struct _hy_mesg_t *mesg)
{
    if (NULL == mesg || !mesg->is_inited)
    {
        return;
    }

    if (mesg->mesg_buf)
        hy_free(mesg->mesg_buf);
    mesg->is_inited = HY_FALSE;
    hy_free(mesg);
    return;
}

hy_s32 hy_init_mesg(hy_mesg_t *mesg, HY_MESG_TYPE_E mesg_type, void *mesg_context)
{
    mesg->mesg_type = mesg_type;
    mesg->mesg_buf = mesg_context;
    mesg->hy_release_mesg = hy_release_mesg;
    mesg->is_inited = HY_TRUE;

    return 0;
}

//mesg may be constructed by user, and trans to mesg caremeter
hy_s32 hy_put_mesg(struct _hy_mesg_deque_t *mesg_deque, hy_mesg_t *mesg)
{
    if (mesg_deque == NULL || !mesg_deque->is_inited || mesg == NULL)
    {
        return -1;
    }

    struct list_head *new_node = (struct list_head *)mesg;
    //pthread_cond_wait(mesg_deque->deque_condition, mesg_deque->deque_condition_mutex);
    //add
    pthread_mutex_lock(&mesg_deque->deque_mutex);
    list_add_tail(new_node, mesg_deque->head);
    mesg_deque->mesg_nums += 1;
    pthread_mutex_unlock(&mesg_deque->deque_mutex);

    //
    hy_print("have %d mesgs in mesg_qeue.\n", mesg_deque->mesg_nums);
    if (mesg_deque->mesg_nums == 1)
    {
        pthread_mutex_lock(&mesg_deque->deque_condition_mutex);
        pthread_cond_signal(&mesg_deque->deque_condition);
        pthread_mutex_unlock(&mesg_deque->deque_condition_mutex);
    }

    return 0;
}

hy_mesg_t* hy_get_mesg(struct _hy_mesg_deque_t *mesg_deque)
{
    if (mesg_deque == NULL || !mesg_deque->is_inited)
    {
        return NULL;
    }

    if (mesg_deque->mesg_nums == 0)
    {
        pthread_cond_wait(&mesg_deque->deque_condition, &mesg_deque->deque_condition_mutex);
    }

    struct list_head *got_node = mesg_deque->head->next;
    pthread_mutex_lock(&mesg_deque->deque_mutex);
    list_del(mesg_deque->head->next);
    mesg_deque->mesg_nums = mesg_deque->mesg_nums - 1;
    pthread_mutex_unlock(&mesg_deque->deque_mutex);

    return got_node;
}

void hy_release_mesg_deque(struct _hy_mesg_deque_t *mesg_deque)
{
    //prevent using  get put
    mesg_deque->is_inited = HY_FALSE;
    //get each mesg and free it
    pthread_mutex_lock(&mesg_deque->deque_mutex);
    hy_mesg_t *pos;
    list_for_each_entry(pos, mesg_deque->head, node)
    {
        pos->hy_release_mesg(pos);
    }
    pthread_mutex_unlock(&mesg_deque->deque_mutex);

    //free condition and mutex
    pthread_cond_destroy(&mesg_deque->deque_condition);
    pthread_mutex_destroy(&mesg_deque->deque_condition_mutex);
    pthread_mutex_destroy(&mesg_deque->deque_mutex);
    //hy_free head
    hy_free(mesg_deque->head);
    //hy_free self
    hy_free(mesg_deque);
}

hy_s32 hy_init_mesg_deque_t(hy_mesg_deque_t *mesg_deque)
{
    //init condition
    if (pthread_cond_init(&mesg_deque->deque_condition, NULL))
    {
        hy_print("init condition failed.\n");
        return -1;
    }
    //init mutex
    if (pthread_mutex_init(&mesg_deque->deque_condition_mutex, NULL))
    {
        hy_print("init mutex failed.\n");
        return -1;
    }
    //init mutex
    if (pthread_mutex_init(&mesg_deque->deque_mutex, NULL))
    {
        hy_print("init mutex failed.\n");
        return -1;
    }
    //init head
    mesg_deque->head = (struct list_head*)hy_malloc(sizeof(struct list_head));
    if (NULL == mesg_deque->head)
    {
        hy_print("malloc head faile.\n");
        return -1;
    }

    mesg_deque->is_inited = HY_TRUE;
    INIT_LIST_HEAD(mesg_deque->head);
    mesg_deque->mesg_nums = 0;

    //init function
    mesg_deque->hy_get_mesg = hy_get_mesg;
    mesg_deque->hy_put_mesg = hy_put_mesg;
    mesg_deque->hy_release_mesg_deque = hy_release_mesg_deque;
    return 0;
}
