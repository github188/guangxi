#include <pthread.h>
#include "hy_thread_utils.h"



void *hy_thread_cb(void *s)
{
    hy_thread_handler_t *thread_handler = (hy_thread_handler_t *)s;
    thread_handler->b_thread_running = HY_TRUE;

    while(thread_handler->b_thread_running)
    {

    }

    pthread_exit("thread normal exit");
}


void hy_init_thread(hy_thread_handler_t *thread_handler, thread_func_t func,  void * data)
{
    thread_handler->b_thread_running = HY_FALSE;
    thread_handler->pid = 0;
    thread_handler->data = data;
    thread_handler->thread_func = func;
}

hy_s32 hy_start_thread(hy_thread_handler_t *thread_handler)
{
     hy_s32 ret;
     pthread_attr_t attr;
     pthread_t pid;

     pthread_attr_init(&attr);
	 pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

     ret = pthread_create(&thread_handler->pid, &attr , thread_handler->thread_func , thread_handler);
     if ( ret < 0 )
     {
         return HY_FAILTURE;
     }

     pthread_attr_destroy(&attr);

     return HY_SUCCESS;

}

void hy_stop_thread(hy_thread_handler_t *thread_handler)
{

    if (thread_handler->b_thread_running)
    {
        thread_handler->b_thread_running = 0;
        pthread_join(thread_handler->pid, 0);
    }

    if (NULL != thread_handler->data)
        free(thread_handler->data);
    free(thread_handler);
}
