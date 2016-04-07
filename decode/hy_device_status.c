#include "hy_thread_utils.h"
#include "hy_device_status.h"
#include "hy_debug.h"
#include "hy_type.h"
#include "hy_base_config.h"
#include "hy_protocl.h"
#include "global_config.h"
#include "hy_zlog.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

static hy_thread_handler_t *hy_device_status_thread;



/* 网络设备连通性检查 */
int device_net_status_judge(char *ip)
{
	char cmd[128] = {0};
	sprintf(cmd, "ping -c 1 %s", ip);

	FILE *pingfp =  popen(cmd, "r");
    if(pingfp == NULL)
    {
        printf("ping error\n");
        return -1;
    }

    char ping_resp[256];
    read(fileno(pingfp), ping_resp, sizeof(ping_resp));

    pclose(pingfp);

    if(strstr(ping_resp, "ttl") == NULL)
    {
        return -1;
    }

    return 0;
}

static void *hy_device_status_thread_cb(void *s)
{
    hy_thread_handler_t *thread_handler = (hy_thread_handler_t *)s;
    thread_handler->b_thread_running = HY_TRUE;

    while(thread_handler->b_thread_running)
    {
    //TODO:get status
        int channel1_on = -1;
        int channel2_on = -1;
        channel1_on = device_net_status_judge(g_hy_nvr_info.hy_nvr_channel_info[0].ip);
        if (g_hy_nvr_info.hy_nvr_general_info.max_chan > 1)
        {
            channel2_on = device_net_status_judge(g_hy_nvr_info.hy_nvr_channel_info[1].ip);
        }

        DATA_STRUCT data;
        data.flag = DEFAULT_FLAG;
        data.type = DTOA_DEVICE_STATUS_INFO;
        data.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
        data.device_status.ir_camera_status = channel1_on;//0 ok -1 dis
        data.device_status.visu_camera_status = channel2_on;
    //TODO:connect
    	hy_s32 fd = hy_tcp_connect(g_hy_nvr_info.hy_nvr_server_info.server_ip, g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    	if(fd == -1)
    	{
    		hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n", g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                    g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    		hy_sleep(10);
    		continue;
    	}
    	else
    	{
            hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                    g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    	}

    //TODO:send
        hy_write(fd, &data, sizeof(data));
        hy_close_fd(fd);
        hy_sleep(10);
    }

    pthread_exit("thread normal exit");
}

void *hy_start_device_status_thread()
{
    hy_device_status_thread = (hy_thread_handler_t *)hy_malloc(sizeof(hy_thread_handler_t));
    hy_init_thread(hy_device_status_thread, hy_device_status_thread_cb, NULL);
    hy_start_thread(hy_device_status_thread);

    return NULL;
}

void *hy_stop_device_status_thread()
{
    hy_stop_thread(hy_device_status_thread);
    return NULL;
}
