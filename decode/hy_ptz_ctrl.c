/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :
* Change log		 :  Jimmy2015-7-10
********************* (C) COPYRIGHT 2015 HY *******************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <termios.h>

#include "uuid/uuid.h"
#include "wsdd.h"
#include "soapH.h"
#include "wsseapi.h"
#include "ov_types.h"
#include "ov_dev.h"
#include "ov_ptz.h"
#include "ov_list_op.h"
#include "onvif_pro.h"
#include "hy_ptz_ctrl.h"

#include "hy_debug.h"
#include "hy_base_config.h"
#include "hy_zlog.h"

static struct soap *soap = NULL;
static ov_devices_list *dev_list = NULL;



int ptz_ctrl_init()
{
    int i,j;
    int dev_input, option_input;
    /*创建soap通讯实体*/
    soap = soap_new();
    if(soap == NULL)
    {
        printf("soap new error\n");
        return -1;
    }

    soap_set_namespaces(soap, namespaces);
    soap->recv_timeout = 5;

    /*设备列表初始化*/
    dev_list = (ov_devices_list *)calloc(1, sizeof(ov_devices_list));
    for(i = 0; i < OV_DEV_MAX_SIZE; i++)
    {
        INIT_LIST_HEAD(&(dev_list->dev[i].presets_list));
        INIT_LIST_HEAD(&(dev_list->dev[i].tours_list));
        for(j = 0; j < OV_TOURS_NUM_EACH_DEV; j++)
        {
            INIT_LIST_HEAD(&(dev_list->dev[i].tours[j].presets_list));
        }
    }
    /*寻找网络上的IPC,获取设备入口decive_addr*/
    ov_discovery_device("/root/devinfo.ini", dev_list);
    ov_show_device_list(dev_list);

}


void ptz_ctrl_release()
{
	/*资源释放*/
	soap_destroy(soap);
	soap_end(soap);
	soap_free(soap);
}

void simple_ptz_move_up(hy_u8 channel, hy_f32 step)
{
    channel = channel - 1;
    onvif_ptz_move_up(soap, &dev_list->dev[channel], step);
}

void simple_ptz_move_down(hy_u8 channel, hy_f32 step)
{
    channel = channel - 1;
    onvif_ptz_move_down(soap, &dev_list->dev[channel], step);
}

void simple_ptz_move_left(hy_u8 channel, hy_f32 step)
{
    channel = channel - 1;
    onvif_ptz_move_left(soap, &dev_list->dev[channel], step);
}

void simple_ptz_move_right(hy_u8 channel, hy_f32 step)
{
    channel = channel - 1;
    onvif_ptz_move_right(soap, &dev_list->dev[channel], step);
}

void simple_ptz_zoom_in(hy_u8 channel, hy_f32 step)
{
    channel = channel - 1;
    onvif_ptz_zoom_in(soap, &dev_list->dev[channel], step);
}

void simple_ptz_zoom_out(hy_u8 channel, hy_f32 step)
{
    channel = channel - 1;
    onvif_ptz_zoom_out(soap, &dev_list->dev[channel], step);
}

void simple_ptz_move_left_up(hy_u8 channel, hy_f32 step)
{
    channel = channel - 1;
    onvif_ptz_move_left_up(soap, &dev_list->dev[channel], step);
}

void simple_ptz_move_left_down(hy_u8 channel, hy_f32 step)
{
    channel = channel - 1;
    onvif_ptz_move_left_down(soap, &dev_list->dev[channel], step);
}

void simple_ptz_move_right_up(hy_u8 channel, hy_f32 step)
{
    channel = channel - 1;
    onvif_ptz_move_right_up(soap, &dev_list->dev[channel], step);
}

void simple_ptz_move_right_down(hy_u8 channel, hy_f32 step)
{
    channel = channel - 1;
    onvif_ptz_move_right_down(soap, &dev_list->dev[channel], step);
}


void simple_ptz_move_stop(hy_u8 channel)
{
    channel = channel - 1;
    onvif_ptz_move_stop(soap, &dev_list->dev[channel]);
}

void simple_ptz_set_preset(hy_u8 channel, hy_u32 preset_id)
{
    channel = channel - 1;
    onvif_preset_set(soap, &dev_list->dev[channel], preset_id);
}
void simple_ptz_clear_preset(hy_u8 channel, hy_u32 preset_id)
{
//    channel = channel - 1;
//    onvif_preset_clear(soap, &dev_list->dev[channel], preset_id);
}

void simple_ptz_goto_preset(hy_u8 channel, hy_u32 preset_id)
{
    channel = channel - 1;
    onvif_preset_goto(soap, &dev_list->dev[channel], preset_id);
}
