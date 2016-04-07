/*********************************************
 *onvif协议访问摄像机
 *date:     2014.6.23
 *changelog:
 *2015.03.23 : 修改增加预置点函数-不能加入同名预置点
 *2015.05.06 : 修改变焦函数为步进变焦，通过延时发送停止命令实现
 *Discription：
 	1，实现控制流的访问
 	2，实现媒体流的访问
 	3，能够实现登陆、录像、抓图、预览、预置位（设置、调用、删除）、路径（添加、开始、结束、删除）
 *********************************************/
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

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"

#include "uuid/uuid.h"
#include "soapH.h"
#include "wsseapi.h"

#include "ov_types.h"
#include "ov_dev.h"
#include "ov_ptz.h"
#include "ov_list_op.h"
#include "onvif_pro.h"

/*镜头上移*/
int onvif_ptz_move_up(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n", step);
    //ov_ptz_relative_move(soap,dev,0,step,0);
    return ov_ptz_continuous_move(soap, dev, 0, step, 0);
}
/*镜头下移*/
int onvif_ptz_move_down(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n", step);
    step = 0 - step;
    //ov_ptz_relative_move(soap,dev,0,step,0);
    return ov_ptz_continuous_move(soap, dev, 0, step, 0);
}
/*镜头左移*/
int onvif_ptz_move_left(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n", step);
	step = 0 - step;
    //ov_ptz_relative_move(soap,dev,step,0,0);
    return ov_ptz_continuous_move(soap, dev, step, 0, 0);
}
/*镜头右移*/
int onvif_ptz_move_right(struct soap * soap, ov_dev_t *dev, float step)
{
    //ov_ptz_relative_move(soap,dev,step,0,0);
    printf("float is %f\n",step);
    return ov_ptz_continuous_move(soap, dev, step, 0, 0);
}

/*镜头左上*/
int onvif_ptz_move_left_up(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
	return ov_ptz_continuous_move(soap, dev, 0-step, step, 0);
}

/*镜头右上*/
int onvif_ptz_move_right_up(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
	return ov_ptz_continuous_move(soap, dev, step, step, 0);
}

/*镜头左下*/
int onvif_ptz_move_left_down(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
	return ov_ptz_continuous_move(soap, dev, 0-step, 0-step, 0);
}

/*镜头右下*/
int onvif_ptz_move_right_down(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
	return ov_ptz_continuous_move(soap, dev, step, 0-step, 0);
}

/*镜头拉近*/
int onvif_ptz_zoom_in(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
    //ov_ptz_relative_move(soap,dev,0,0,step);
    return ov_ptz_continuous_move(soap, dev, 0, 0, step);
}
/*镜头拉远*/
int onvif_ptz_zoom_out(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
    step = 0 - step;
    //ov_ptz_relative_move(soap,dev,0,0,step);
    return ov_ptz_continuous_move(soap, dev, 0, 0, step);
}

/*镜头连续移动需输入指令停止*/
/*镜头连续左移*/
int onvif_ptz_move_left_continue(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("input 9 stop continue move\n");
    return ov_ptz_continuous_move(soap, dev, step, 0, 0);
}
/*镜头连续右移*/
int onvif_ptz_move_right_continue(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("input 9 stop continue move\n");
    step = 0 - step;
    return ov_ptz_continuous_move(soap, dev, step, 0, 0);
}
/*镜头停止移动*/
int onvif_ptz_move_stop(struct soap * soap, ov_dev_t *dev)
{
    return ov_ptz_stop_move(soap, dev);
}

/*镜头移至home position*/
int onvif_ptz_return_home(struct soap * soap, ov_dev_t *dev)
{
    return ov_ptz_return_home_position(soap, dev);
}
/*设置当前为home position*/
int onvif_ptz_set_home(struct soap * soap, ov_dev_t *dev)
{
    return ov_ptz_set_home_position(soap, dev);
}
