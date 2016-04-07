#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "hy_debug.h"
#include "hy_type.h"
#include "hy_base_config.h"
#include "hy_protocl.h"
#include "hy_global.h"

#include "hy_thread_utils.h"
#include "hy_channel_scheduler.h"
#include "hy_zlog.h"


static hy_thread_handler_t *thread_handler;


static void *hy_thread_cb(void *s)
{
	/*
    hy_thread_handler_t *thread_handler = (hy_thread_handler_t *)s;
    thread_handler->b_thread_running = HY_TRUE;

    while(thread_handler->b_thread_running)
    {
        if (!g_hy_detect_info_t->starting)//算法开启的情况下
        {
            hy_sleep(5);
        }
        for ()//通道
        {
            //跟换通道
            g_hy_detect_info_t->change_to_channel =;
            for ()//场景
            {
                //为考虑场景重构的问题，每个场景都应该有一个重构标记位，在设置参数的时候，应该置标记位
                //调用场景，转到预置点

                hy_sleep(3);//延时，等待转到预置点
                g_hy_detect_info_t->scheduler_starting;

            }

        }

        //每个场景30秒
        hy_sleep(30);
        //关闭侦测
        g_hy_detect_info_t->scheduler_starting = HY_FALSE;

    }
	*/
    pthread_exit("file manage thread normal exit");
}

void *hy_start_channel_scheduler_thread()
{
    thread_handler = (hy_thread_handler_t *)hy_malloc(sizeof(hy_thread_handler_t));
    hy_init_thread(thread_handler, hy_thread_cb, NULL);
    hy_start_thread(thread_handler);
}
void *hy_stop_channel_scheduler_thread()
{
    hy_stop_thread(thread_handler);
}
