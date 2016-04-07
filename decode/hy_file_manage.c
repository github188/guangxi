#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#include "hy_debug.h"
#include "hy_type.h"
#include "hy_base_config.h"
//#include "hy_protocl.h"
#include "global_config.h"

#include "hy_deque.h"
#include "hy_thread_utils.h"
#include "hy_file_manage.h"
#include "hy_stream_ctrl.h"
//#include "hy_serial_ctrl_protocol.h"
//#include "hy_picture_trans.h"
#include "hy_timer.h"
#include "hy_time.h"
#include "hy_mk60_ctrl.h"

#include "hy_io_utils.h"
#include "hy_zlog.h"

//用于连接A9，发送图片
//#define SEND_ALARM_PIC_TO_A9
#define A9_IP "192.16.8.111"
#define A9_PORT "1001"

static hy_thread_handler_t *hy_file_manage_thread;
static HY_BOOL hy_mv_record_flag = HY_FALSE;//当已经在录像的时候就不再录像
static hy_u32 hy_mv_record_sending_cunt = 0;
//每次上传的时候，引用计数加1，使用完减1.当sending_count为0的时候
//执行rm /pic_record/* 删除没有发送成功的录像，防止录像一直上传不成功造成硬件损伤
//当cunt计数大于10的时候重启系统


static hy_s32 hy_data_handle_alarm_mesg(ALARM_DATA *p_alarm)//default success
{
    DATA_STRUCT data;
    data.flag = DEFAULT_FLAG;
    data.type = DTOA_ALARM_INFO;
    data.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    memcpy(&data.alarm, p_alarm, sizeof(ALARM_DATA));
    //TODO:connect
    hy_s32 fd = hy_tcp_connect(g_hy_nvr_info.hy_nvr_server_info.server_ip, g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    if(fd == -1)
    {
    	hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n", g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    	return -1;
    }
    else
    {
        hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    }

    //TODO:send alarm mesg
    hy_write(fd, &data, sizeof(data));
    hy_close_fd(fd);

    return 0;
}

//#define DTOA_PICTURE_INFO				0x64616901
//2．	录像,山火&外破
//#define DTOA_RECORD_FILE_INFO			0x64616902
//typedef struct
//{
//	unsigned int		ipc;
//	unsigned int 		stream;      //在LOG的情况下，1为Alarm 0为Normal
//	unsigned int 		size;		 //数据大小
//	int year;
//	int month;
//	int day;
//	int hour;
//	int min;
//	int sec;
//} VEDIO_OR_PICTURE_DATA;//32，录像或图片

static hy_u32 hy_get_file_size(const hy_char *file_path)
{
    struct stat file_stat;
    if(0 == stat(file_path, &file_stat))
	    return file_stat.st_size;
    else
        return 0;
}

static hy_s32 hy_send_file(hy_s8 *file_path, hy_s32 fd)
{
	FILE *fp;
	fp = fopen(file_path, "rb");
	if( !fp )
	{
	    hy_zlog_info("open %s file failed.\n", file_path);
		return -1;
	}

    struct stat file_stat;
    stat(file_path, &file_stat);
	hy_u32 file_size = file_stat.st_size;;

    hy_zlog_info("begin to send file to AccessServer\n");
	//发送图片给流媒体服务器
	hy_u32 SentSize = 0;//已送的大小
	hy_u32 SendSize;//当前传送的大小
	hy_char pDataBuf[1024*8] = {0};
	while(SentSize != file_size)
	{
		//构造pDATAbuf;
		memset(pDataBuf, 0, 1024*8);
		SendSize = fread(pDataBuf, 1, 1024*8, fp);
		if (SendSize == 0)
		{
		    hy_zlog_info("read file failed.\n");
			fclose(fp);
			hy_zlog_info("send %s to server failed. %d size have send to server.\n", file_path, SentSize);
			return -1;//进行下次调用
		}


		int ret = send(fd, pDataBuf, SendSize, 0);
		if (SendSize == ret)
		{
		}
		else
		{
			fclose(fp);
			hy_zlog_info("send %s to server failed. %d size have send to server.\n", file_path, SentSize);
			return -1;//读取和发送的自己不相同发送失败
		}

		SentSize += SendSize;
	}

    hy_zlog_info("send %s to server success. %d size have send to server.\n", file_path, SentSize);

	//关闭文件
	fclose(fp);
	return 0;
}

/*
删除/root/pic_record目录下的文件
*/
static hy_u32 hy_delete_files_in_dir()
{
    system("rm /root/pic_record/*");
    return 0;
}


static void *hy_data_handle_alarm_picture_send_to_a9(void *param)
{
    hy_s8 *file_path = param;
    ALARM_DATA *p_data = (ALARM_DATA *)(file_path + 128);
    hy_u32 fd;
    char rm_buf[128];


    //hy_sleep(60);//等待A9启动
    int i;
    while(1)
    {
        i ++;
        fd = hy_tcp_connect(A9_IP, A9_PORT);
    	if( fd == -1)
    	{
    		hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n",A9_IP, A9_PORT);
    		hy_sleep(10);
    		if (i > 20)
    		{
                goto delete_file;
    		}
    		continue;
    	}
    	else
    	{
            hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",A9_IP, A9_PORT);
    	}

    	break;
    }

    //送头
    hy_alarm_file_info_t file_info;
    file_info.flag = 0x646101BB;
    file_info.ipc = p_data->ipc;

    struct tm alarm_tm;
    alarm_tm.tm_year = p_data->year - 1900;
    alarm_tm.tm_mon = p_data->month - 1;
    alarm_tm.tm_mday = p_data->day;
    alarm_tm.tm_hour = p_data->hour;
    alarm_tm.tm_min = p_data->minute;
    alarm_tm.tm_sec = p_data->second;

    file_info.alarm_time = mktime(&alarm_tm);


//    file_info.year = p_data->year;
//    file_info.month = p_data->month;
//    file_info.day = p_data->day;
//    file_info.hour = p_data->hour;
//    file_info.min = p_data->minute;
//    file_info.sec = p_data->second;
    file_info.size = hy_get_file_size(file_path);
    hy_zlog_info("file time_t %ld\n", file_info.alarm_time);
    hy_zlog_info("file_path %s file_size %d\n", file_path, file_info.size);
    if (file_info.size == 0)
        goto delete_file;
    hy_write(fd, &file_info, sizeof(file_info));
    //送内容
    hy_s32 ret = hy_send_file(file_path, fd);
    if (ret != 0)
    {
        hy_zlog_info("send alarm pic to a9 failed.\n");
    }
    else
    {
        hy_zlog_info("send alarm pic to a9 success.\n");
    }

delete_file:
    //删文件
    sprintf(rm_buf, "rm %s", file_path);
    hy_zlog_info("%s", rm_buf);
    system(rm_buf);

    //释放线程参数
    hy_free(param);
}
/**********************************
*Name:hy_send_alarm_pic_to_a9
*Desc:发送告警图片到A9，A9随后将图片发送给MK60
*Param:
*Return:
*Author:Jimmy
*Create Time:2015-9-14
*ChangeLog:
**********************************/
static hy_s32 hy_send_alarm_pic_to_a9(hy_s8 *file_path, ALARM_DATA *p_alarm_data)
{
    //启动线程，循环连接发送文件
    pthread_t id;
    hy_s8 *p_alram_param = (hy_s8 *)hy_malloc(128 + sizeof(ALARM_DATA));
    if (!p_alram_param)
    {
        hy_zlog_info("malloc p_alram_param failed.\n");
        return -1;
    }

    memcpy(p_alram_param, file_path, 128);
    hy_s8 *p_alram_param1 = p_alram_param + 128;
    memcpy(p_alram_param1, p_alarm_data, sizeof(ALARM_DATA));
    hy_s32 ret = pthread_create(&id, NULL, hy_data_handle_alarm_picture_send_to_a9, p_alram_param);
    if ( ret < 0 )
    {
        hy_zlog_info("create pic send to a9 thread failed.\n");
        return -1;
    }
    else
    {
        hy_zlog_info("create pic send to a9 thread success.\n");
    }

    return 0;
}

static void *hy_data_handle_alarm_picture_and_record(void *param)//default success
{
    ALARM_DATA  *p_alarm = (ALARM_DATA *)param;
    DATA_STRUCT data;

    /*handler alarm mesg*/
    //TODO:connect
    hy_s32 fd = hy_tcp_connect(g_hy_nvr_info.hy_nvr_server_info.server_ip, g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    if(fd == -1)
    {
    	hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n", g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    	goto release;
    }
    else
    {
        hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    }
    memset(&data, 0, sizeof(DATA_STRUCT));
    data.flag = DEFAULT_FLAG;
    data.type = DTOA_ALARM_INFO;
    data.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    data.alarm.ipc = g_hy_detect_info_t.cur_channel;
    data.alarm.stream = p_alarm->stream;
    data.alarm.year = p_alarm->year;
    data.alarm.month = p_alarm->month;
    data.alarm.day = p_alarm->day;
    data.alarm.hour = p_alarm->hour;
    data.alarm.minute = p_alarm->minute;
    data.alarm.second = p_alarm->second;
    data.alarm.type = 12;
    data.alarm.level = 2;
    //TODO:send alarm mesg
    if (-1 == hy_write(fd, &data, sizeof(data)))
    {
        hy_zlog_info("send alarm to server failed.\n");
    }

    hy_close_fd(fd);

    /*
    handle pic
    */
    //get pic
    hy_s8 file_name[128];
    sprintf(file_name, "/root/pic_record/%d-%d-%d%02d%02d-%02d%02d%02d.jpg", g_hy_nvr_info.hy_nvr_general_info.device_id,g_hy_detect_info_t.cur_channel,
            p_alarm->year, p_alarm->month, p_alarm->day, p_alarm->hour, p_alarm->minute, p_alarm->second);
    //hy_simple_stream_caputre_picture(g_hy_detect_info_t.cur_channel, file_name);//图片已经在decode程序处录好

    //TODO:connect
    fd = hy_tcp_connect(g_hy_nvr_info.hy_nvr_server_info.server_ip, g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    if(fd == -1)
    {
    	hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n", g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);

    	goto release;
    }
    else
    {
        hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    }
    //data 数据
    memset(&data, 0, sizeof(DATA_STRUCT));
    data.flag = DEFAULT_FLAG;
    data.type = DTOA_VISU_PICTURE_INFO;
    data.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    data.pic_data.ipc = g_hy_detect_info_t.cur_channel;
    data.pic_data.stream = p_alarm->stream;
    data.pic_data.size = hy_get_file_size(file_name);
    data.pic_data.year = p_alarm->year;
    data.pic_data.month = p_alarm->month;
    data.pic_data.day = p_alarm->day;
    data.pic_data.hour = p_alarm->hour;
    data.pic_data.min = p_alarm->minute;
    data.pic_data.sec = p_alarm->second;

#ifdef SEND_ALARM_PIC_TO_A9
    hy_send_alarm_pic_to_a9(file_name, p_alarm);
#endif

#ifndef SEND_ALARM_PIC_TO_A9
    //TODO:send pic mesg
    hy_write(fd, &data, sizeof(data));
    //send
    hy_send_file(file_name, fd);
    hy_close_fd(fd);
    //TODO:send file to a9 , and then a9 send the file to mk60

    char rm_buf[128];

    sprintf(rm_buf, "rm %s", file_name);
    hy_zlog_info("%s", rm_buf);
    system(rm_buf);
#endif


    /*
    handle record
    */
/*
    if (hy_mv_record_flag)
    {
        hy_zlog_info("already  recording.\n");
        goto release;
    }

    memset(file_name, 0, 128);
    sprintf(file_name, "/root/pic_record/%d-%d-%d%02d%02d-%02d%02d%02d.mp4", g_hy_nvr_info.hy_nvr_general_info.device_id, g_hy_detect_info_t.cur_channel,
            p_alarm->year, p_alarm->month, p_alarm->day, p_alarm->hour, p_alarm->minute, p_alarm->second);
    hy_mv_record_flag = HY_TRUE;
//    if (hy_mv_record_sending_cunt > 5)
//    {
//        system("rm /root/pic_record/*");
//        system("reboot");
//    }
//    if (0 == hy_mv_record_sending_cunt)
//    {
//        system("rm /root/pic_record/*.mp4");
//    }

    hy_mv_record_sending_cunt++;
    hy_simple_stream_record_video(g_hy_detect_info_t.cur_channel, file_name, 10);
    hy_mv_record_flag = HY_FALSE;
    hy_sleep(60);

#if 1
    //TODO:connect
    fd = hy_tcp_connect(g_hy_nvr_info.hy_nvr_server_info.server_ip, g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    if(fd == -1)
    {
    	hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n", g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
        hy_mv_record_sending_cunt--;
    	goto release;
    }
    else
    {
        hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    }
    //data 数据;
    memset(&data, 0, sizeof(DATA_STRUCT));
    data.flag = DEFAULT_FLAG;
    data.type = DTOA_RECORD_FILE_INFO;
    data.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    data.pic_data.ipc = g_hy_detect_info_t.cur_channel;
    data.pic_data.stream = p_alarm->stream;
    data.pic_data.size = hy_get_file_size(file_name);
    data.pic_data.year = p_alarm->year;
    data.pic_data.month = p_alarm->month;
    data.pic_data.day = p_alarm->day;
    data.pic_data.hour = p_alarm->hour;
    data.pic_data.min = p_alarm->minute;
    data.pic_data.sec = p_alarm->second;
    //TODO:send record mesg
    hy_write(fd, &data, sizeof(data));
    //send
    hy_send_file(file_name, fd);
    hy_close_fd(fd);
#endif

    memset(rm_buf, 0, 128);
    sprintf(rm_buf, "rm %s", file_name);
    hy_zlog_info("%s", rm_buf);
    system(rm_buf);
    hy_mv_record_sending_cunt--;
*/

release:
    hy_free(param);
    return 0;
}

void *hy_data_handle_pic_record_task(void *param)
{
    picture_record_t *p_task = (picture_record_t *)param;
    if (p_task->ipc > g_hy_nvr_info.hy_nvr_general_info.max_chan)
    {
        hy_zlog_info("task's channel is %d biger than max channel.\n", p_task->ipc);
        goto release;
    }

    /*
    handle pic
    */
    //get pic
    hy_s8 file_name[128];
    struct tm * p_tm = hy_get_cur_tm();
    sprintf(file_name, "/root/pic_record/%d-%d-%d%02d%02d-%02d%02d%02d.jpg", g_hy_nvr_info.hy_nvr_general_info.device_id, p_task->ipc,
            p_tm->tm_year+1900, p_tm->tm_mon + 1, p_tm->tm_mday, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
    hy_simple_stream_caputre_picture(p_task->ipc, file_name);

    //TODO:connect
    hy_s32 fd = hy_tcp_connect(g_hy_nvr_info.hy_nvr_server_info.server_ip, g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    if(fd == -1)
    {
    	hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n", g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    	goto release;
    }
    else
    {
        hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    }
    //data 数据
    DATA_STRUCT data;
    memset(&data, 0, sizeof(DATA_STRUCT));
    data.flag = DEFAULT_FLAG;
    data.type = DTOA_PICTURE_TASK_INFO;
    data.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    data.pic_data.ipc = p_task->ipc;
    data.pic_data.stream = 1;
    data.pic_data.size = hy_get_file_size(file_name);
    data.pic_data.year = p_tm->tm_year+1900;
    data.pic_data.month = p_tm->tm_mon + 1;
    data.pic_data.day = p_tm->tm_mday;
    data.pic_data.hour = p_tm->tm_hour;
    data.pic_data.min = p_tm->tm_min;
    data.pic_data.sec = p_tm->tm_sec;
    //TODO:send pic mesg
    hy_write(fd, &data, sizeof(data));
    //send
    hy_send_file(file_name, fd);
    hy_close_fd(fd);
    char rm_buf[128];
    sprintf(rm_buf, "rm %s", file_name);
    hy_zlog_info("%s", rm_buf);
    system(rm_buf);

release:
    hy_free(param);
    return 0;
}

void *hy_data_handle_video_record_task(void *param)
{
    video_record_t *p_task = (video_record_t *)param;
    if (p_task->ipc > g_hy_nvr_info.hy_nvr_general_info.max_chan)
    {
        hy_zlog_info("task's channel is %d biger than max channel.\n", p_task->ipc);
        goto release;
    }

    /*
    handle record
    */
    if (hy_mv_record_flag)
    {
        hy_zlog_info("already  recording.\n");
        goto release;
    }

    hy_s8 file_name[128];
    struct tm * p_tm = hy_get_cur_tm();
    sprintf(file_name, "/root/pic_record/%d-%d-%d%02d%02d-%02d%02d%02d.mp4", g_hy_nvr_info.hy_nvr_general_info.device_id, p_task->ipc,
            p_tm->tm_year+1900, p_tm->tm_mon + 1, p_tm->tm_mday, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);

    hy_mv_record_flag = HY_TRUE;
    if (hy_mv_record_sending_cunt > 5)
    {
        system("rm /root/pic_record/*");
        system("reboot");
    }
    if (0 == hy_mv_record_sending_cunt)
    {
        system("rm /root/pic_record/*.mp4");
    }

    hy_mv_record_sending_cunt++;
    hy_simple_stream_record_video(p_task->ipc, file_name, p_task->runTime);
    hy_mv_record_flag = HY_FALSE;


    //TODO:connect
    fd = hy_tcp_connect(g_hy_nvr_info.hy_nvr_server_info.server_ip, g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    if(fd == -1)
    {
    	hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n", g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
        hy_mv_record_sending_cunt--;
    	goto release;
    }
    else
    {
        hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    }
    //data 数据;
    DATA_STRUCT data;
    memset(&data, 0, sizeof(DATA_STRUCT));
    data.flag = DEFAULT_FLAG;
    data.type = DTOA_VEDIO_TASK_INFO;
    data.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    data.pic_data.ipc = p_task->ipc;
    data.pic_data.stream = 1;
    data.pic_data.size = hy_get_file_size(file_name);
    data.pic_data.year = p_tm->tm_year+1900;
    data.pic_data.month = p_tm->tm_mon + 1;
    data.pic_data.day = p_tm->tm_mday;
    data.pic_data.hour = p_tm->tm_hour;
    data.pic_data.min = p_tm->tm_min;
    data.pic_data.sec = p_tm->tm_sec;
    //TODO:send record mesg
    hy_write(fd, &data, sizeof(data));
    //send
    hy_send_file(file_name, fd);
    hy_close_fd(fd);
    hy_char rm_buf[128];
    sprintf(rm_buf, "rm %s", file_name);
    hy_zlog_info("%s", rm_buf);
    system(rm_buf);
    hy_mv_record_sending_cunt--;

release:
    hy_free(param);
    return 0;
}



void *hy_file_manage_thread_cb(void *s)
{
    hy_thread_handler_t *thread_handler = (hy_thread_handler_t *)s;
    thread_handler->b_thread_running = HY_TRUE;
    hy_mesg_t *mesg = NULL;
    //打开告警联动串口
    //hy_s32 serial_fd = tty_cmd_open();
    //初始化，还未发送完的图片，默认删掉
    hy_delete_files_in_dir();

    while(thread_handler->b_thread_running)
    {
        mesg = hy_get_mesg(data_mesg_deque);
        if (mesg)
        {
            switch (mesg->mesg_type)
            {
                case HY_ALARM_MESG:
                    hy_zlog_info("HY_ALARM_MESG.\n");
                    //告警器联动
                    send_open_alarm_package_to_mk60();
                    //open_horn_power(serial_fd);
//                    hy_s32 *p_serial_alarm_para = (hy_s32 *)hy_malloc(sizeof(hy_s32));//必须使用malloc 出来的参数，因为后期会统一free
//                    *p_serial_alarm_para = serial_fd;
                    hy_timer_t * p_mk60_alarm = hy_init_timer(HY_SINGLE_TIRGGER, send_close_alarm_package_to_mk60, NULL);
                    hy_start_timer(p_mk60_alarm, 30);
                    //图片、录像
                    pthread_t id;
                    ALARM_DATA *p_alram_param = (ALARM_DATA *)hy_malloc(sizeof(ALARM_DATA));
                    memcpy(p_alram_param, mesg->mesg_buf, sizeof(ALARM_DATA));
                    hy_s32 ret = pthread_create(&id, NULL, hy_data_handle_alarm_picture_and_record, p_alram_param);
                    if ( ret < 0 )
                    {
                        hy_zlog_info("create pic send thread failed.\n");
                    }
                    break;
                default:
                    break;

            }
            //handler mesg


            //free
            hy_release_mesg(mesg);
        }
        else
        {
            hy_zlog_info("get mesg error.\n");
        }
    }

    pthread_exit("file manage thread normal exit");
}

void *hy_start_file_manage_thread()
{
    hy_file_manage_thread = (hy_thread_handler_t *)hy_malloc(sizeof(hy_thread_handler_t));
    hy_init_thread(hy_file_manage_thread, hy_file_manage_thread_cb, NULL);
    hy_start_thread(hy_file_manage_thread);
    return NULL;
}
void *hy_stop_file_manage_thread()
{
    hy_stop_thread(hy_file_manage_thread);
    return NULL;
}
