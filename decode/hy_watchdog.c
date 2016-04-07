#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "hy_debug.h"
#include "hy_type.h"
#include "hy_base_config.h"
//#include "hy_protocl.h"
#include "hy_zlog.h"

#include "hy_thread_utils.h"
#include "hy_watchdog.h"

#define WATCHDOG_DRIVER_NAME  "/dev/watchdog"
#define WATCHDOG_WRITE_TIMEVAL (5)

static hy_thread_handler_t *thread_handler;

static void hy_reboot_sys( void )
{
    hy_s32 ret,value;
    char buff[32];

    memset(buff,0x00,sizeof(buff));
    sprintf(buff,"reboot");
    ret = system(buff);
    value = WEXITSTATUS(ret);
    if( value ) {
        fprintf(stderr,"Can't reboot sys .\n");
        _exit(-1);
    }
}

static void *hy_thread_cb(void *s)
{
    hy_thread_handler_t *thread_handler = (hy_thread_handler_t *)s;
    thread_handler->b_thread_running = HY_TRUE;

	int wdg_fd = open(WATCHDOG_DRIVER_NAME, O_RDWR);
	if(wdg_fd < 0)
	{
		int err = wdg_fd;
		hy_zlog_info("\n!!! FAILED to open /dev/watchdog, errno: %d, %s\n", err, strerror(err));
        hy_reboot_sys();
    }
    else
    {
        hy_zlog_info("watch dog set set success.\n");
    }

    hy_s32 apple = 0;
    while(thread_handler->b_thread_running)
    {
        struct timeval tv;
        tv.tv_sec = WATCHDOG_WRITE_TIMEVAL;
        tv.tv_usec = 0;

        int ret = select(0, NULL, NULL, NULL, &tv);
        if (ret == 0)
        {
            hy_write(wdg_fd, &apple, sizeof(hy_s32));
            hy_zlog_info("feed dog.\n");
        }
        else if (ret == -1)
        {
            hy_zlog_info("select error.\n");
        }
    }

    pthread_exit("file manage thread normal exit");
}
void *hy_start_watchdog_thread()
{
    thread_handler = (hy_thread_handler_t *)hy_malloc(sizeof(hy_thread_handler_t));
    hy_init_thread(thread_handler, hy_thread_cb, NULL);
    hy_start_thread(thread_handler);
}
void *hy_stop_watchdog_thread()
{
    hy_stop_thread(thread_handler);
}
