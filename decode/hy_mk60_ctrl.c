/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :与MK60的交互 主要是喇叭开关电  摄像机开关电
* Change log		 :  Jimmy2015-12-22
********************* (C) COPYRIGHT 2015 HY *******************/
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "hy_zlog.h"

#include "hy_mk60_ctrl.h"
#include "check.h"

#define MK60_PORT 6666
#define MK60_IP "192.168.8.186"

static char hy_mk60_ip[24];
static int hy_mk60_port;
static int socketfd = -1;
static struct sockaddr_in addr;


/**********************************
*Name:send_cmd_package_to_mk60
*Desc:
*Param:pack_type  包类型
       data       命令 0x01 打开电源 0x02 关闭电源
*Return: -1
*Author:Jimmy
*Create Time:2015-12-23
*ChangeLog:
**********************************/
int send_cmd_package_to_mk60(u_int8_t pack_type, u_int8_t data)
{
    if (socketfd < 0)
    {
        return -1;
    }

    dsp_mk60_frame_pack_t frame_data;
    frame_data.flag = 0x5AA5;
    frame_data.id = 1;
    frame_data.frame_type = FRAM_TYPE_DEVICE_TO_SRV;
    frame_data.pack_type = pack_type;
    frame_data.data = data;
    frame_data.data_len = 1;

    unsigned char buf[32];
	memmove(buf, (unsigned char *)&frame_data, sizeof(frame_data));
	buf[sizeof(frame_data)] =  (unsigned char )((CRC16(buf, sizeof(frame_data))) & 0x00ff);
	buf[sizeof(frame_data) + 1] =  (unsigned char )(((CRC16(buf, sizeof(frame_data))) >> 8) & 0x00ff);

    int send_len = sendto(socketfd, buf, sizeof(frame_data)+2, 0, &addr, sizeof(addr));
    if (send_len < 0)
    {
        hy_zlog_debug("send data to mk60 failed.");
    }
    else
    {
        int i;
        for (i=0; i<sizeof(frame_data)+2; i++)
        {
            hy_zlog_debug("%x ", buf[i]);
        }
    }

    return send_len;
}
int send_open_camera_package_to_mk60()
{
    return send_cmd_package_to_mk60(MT_PACK_TYPE_VISABLE_CAMER_POWER, 0x01);
}

void send_close_camera_package_to_mk60(void *parm)
{
    send_cmd_package_to_mk60(MT_PACK_TYPE_VISABLE_CAMER_POWER, 0x02);
}

int send_open_alarm_package_to_mk60()
{
    return send_cmd_package_to_mk60(MT_PACK_TYPE_HORN_POWER, 0x01);
}

void send_close_alarm_package_to_mk60(void *parm)
{
    send_cmd_package_to_mk60(MT_PACK_TYPE_HORN_POWER, 0x02);
}

static int get_mk60_config()
{
    FILE *fp;

    /* 打开配置文件 */
    fp = fopen("./mk60.ini","r");
    if(fp == NULL)
    {
        hy_zlog_info("can't open a9.ini\n");
        return -1;
    }

    /* 从文件读取所有数据 */
    char buf[512 + 1];
    int ret = fread(buf, 512, 1, fp);
    if(ret < 0)
    {
        hy_zlog_info("fread a9.ini config file fail\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);

    /* 读出所有的参数 */
    /* 1.a9 ip */
    char *p ;
    p = strstr(buf, "hy_mk60_ip");
    if(p == NULL)
    {
        hy_zlog_info("hy_mk60_ip item is not in a9 ini\n");
        return -1;
    }
    sscanf(p,"%*[^=]=%s", hy_mk60_ip);
    hy_zlog_info("hy_mk60_ip is %s\n", hy_mk60_ip);

    /* 2.a9 port */
    p = strstr(buf, "hy_mk60_port");
    if(p == NULL)
    {
        hy_zlog_info("hy_mk60_port item is not in a9 ini\n");
        return -1;
    }
    sscanf(p,"%*[^=]=%d", &hy_mk60_port);

    /* 检查得到的数据是否正常 */
    hy_zlog_info("hy_mk60_port is %d\n", hy_mk60_port);

    return 0;
}

void *recv_order_from_mk60(void *s)
{
    int ret = get_mk60_config();
    if (ret < 0)
    {
        strcpy(hy_mk60_ip, MK60_IP);
        hy_mk60_port = MK60_PORT;
    }
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0)   //建立一个socket
    {
        hy_zlog_info("mk60 create fd filed.");
        return -1;
    }

    int addr_len = sizeof(addr);
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;              //添加端口与地址信息
    addr.sin_port = htons(hy_mk60_port);
    addr.sin_addr.s_addr = inet_addr(hy_mk60_ip);

    send_close_camera_package_to_mk60(NULL);

    while(1)
	{
	    unsigned char package_buf[1024];
		int recv_len = recvfrom(socketfd, package_buf, 1024, 0, (struct sockaddr *)&addr, &addr_len);
		if(recv_len < 0)
		{
			hy_zlog_info("recv mk60 data failed");
			continue;
		}
		hy_zlog_info("recv mk60 data sucess");
	    int i = 0;
	    for (i; i<sizeof(dsp_mk60_frame_pack_t); i++)
	    {

            printf("%x ", package_buf[i]);
	    }
	    printf("\n");
	}
}

int start_recv_order_from_mk60_thread()
{
     int ret;
     pthread_attr_t attr;
     pthread_t pid;

     pthread_attr_init(&attr);
	 pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

     ret = pthread_create(&pid, &attr , recv_order_from_mk60, NULL);
     if ( ret < 0 )
     {
        hy_zlog_info("creat recv_order_from_mk60 thread failed.");
        return -1;
     }

     pthread_attr_destroy(&attr);

     return 0;
}