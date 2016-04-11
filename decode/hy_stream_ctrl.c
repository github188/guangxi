/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :
* Change log		 :  Jimmy2015-7-10
********************* (C) COPYRIGHT 2015 HY *******************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "ov_rtsp.h"
#include "hy_stream_ctrl.h"
#include "global_config.h"
#include "hy_debug.h"
#include "hy_base_config.h"
#include "hy_zlog.h"

#define SEND_TO_A9
#ifdef SEND_TO_A9
#define PICTURE_SIZE "640x480"//用于发送给A9
#else
#define PICTURE_SIZE "1280x720"//用于发送给汇源后台
#endif

static int last_played_channel = -1;
static HY_BOOL stream_is_opened = HY_FALSE;
static char current_play_cmd[256];


//数据传递给算法
static hy_s8 *rtsp_addrs[MAX_CHANLE_NUM] =
{
    NULL,
    NULL,
//    "rtsp://admin:huiyuan123@172.16.8.245:554/Streaming/Channels/1?transportmode=unicast&profile=Profile_1",
//    "rtsp://admin:huiyuan123@172.16.8.245:554/Streaming/Channels/1?transportmode=unicast&profile=Profile_1",
    NULL,
    NULL,
};

//录像、截图
static hy_s8 *record_rtsp_addrs[MAX_CHANLE_NUM] =
{
    NULL,
    NULL,
//    "rtsp://admin:huiyuan123@172.16.8.245:554/Streaming/Channels/1?transportmode=unicast&profile=Profile_1",
//    "rtsp://admin:huiyuan123@172.16.8.245:554/Streaming/Channels/1?transportmode=unicast&profile=Profile_1",
    NULL,
    NULL,
};

static hy_char *real_play_rtsp_addrs[MAX_CHANLE_NUM] =
{
    NULL,
    NULL,
//    "rtsp://admin:huiyuan123@172.16.8.245:554/Streaming/Channels/2?transportmode=unicast&profile=Profile_2",
//    "rtsp://admin:huiyuan123@172.16.8.245:554/Streaming/Channels/3?transportmode=unicast&profile=Profile_3",
    NULL,
    NULL,
};

static AVFormatContext	*rtsp_fmt_ctx[MAX_CHANLE_NUM] =
{
    NULL,
    NULL,
    NULL,
    NULL,
};

static AVPacket *rtsp_packets[MAX_CHANLE_NUM] =
{
    NULL,
    NULL,
    NULL,
    NULL
};


int hy_init_rtsp_addrs()
{
    int i;
    for (i=0; i<g_hy_nvr_info.hy_nvr_general_info.max_chan; i++)
    {
       rtsp_addrs[i] = (hy_s8 *)hy_malloc(256);
       record_rtsp_addrs[i] = rtsp_addrs[i];
       sprintf(rtsp_addrs[i], "rtsp://%s:%s@%s:554/Streaming/Channels/1?transportmode=unicast&profile=Profile_1", \
              g_hy_nvr_info.hy_nvr_channel_info[i].login_name,  g_hy_nvr_info.hy_nvr_channel_info[i].login_passwd, \
              g_hy_nvr_info.hy_nvr_channel_info[i].ip);
       hy_zlog_info("record_rtsp_addrs: %s \n", record_rtsp_addrs[i]);

       real_play_rtsp_addrs[i] = (hy_s8 *)hy_malloc(256);
       if (g_hy_nvr_info.hy_nvr_channel_info[i].type == 1)
       {
           sprintf(real_play_rtsp_addrs[i], "rtsp://%s:%s@%s:554/Streaming/Channels/2?transportmode=unicast&profile=Profile_2", \
                  g_hy_nvr_info.hy_nvr_channel_info[i].login_name,  g_hy_nvr_info.hy_nvr_channel_info[i].login_passwd, \
                  g_hy_nvr_info.hy_nvr_channel_info[i].ip);
       }
       else
       {
           sprintf(real_play_rtsp_addrs[i], "rtsp://%s:%s@%s:554/Streaming/Channels/3?transportmode=unicast&profile=Profile_3", \
                  g_hy_nvr_info.hy_nvr_channel_info[i].login_name,  g_hy_nvr_info.hy_nvr_channel_info[i].login_passwd, \
                  g_hy_nvr_info.hy_nvr_channel_info[i].ip);
       }
       hy_zlog_info("real_play_rtsp_addrs: %s \n", real_play_rtsp_addrs[i]);

    }
}

int hy_simple_stream_init()
{
    /*注册所有组件*/
    hy_zlog_info("register all\n");
    av_register_all();
    /*网络支持*/
    hy_zlog_info("support network stream\n");
    avformat_network_init();

    /*连接所有的流到IPC*/
    hy_u8 i;
    /*ToDo:这个地方暂时设置为2，以后使用全局的变量，从配置文件读取*/
    //for(i=0; i<g_hy_nvr_info.hy_nvr_general_info.max_chan; i++)
    for(i=0; i<1; i++)
    {
    	rtsp_fmt_ctx[i] = avformat_alloc_context();//初始化AVFormatContext
        if(rtsp_fmt_ctx[i] == NULL)
        {
            hy_zlog_info("alloc avformat fail\n");
            return -1;
        }

    	/*打开文件*/
    	/*Jimmy 20160401*/
    	AVDictionary* options = NULL;
        //av_dict_set(&options, "rtsp_transport", "tcp", 0);
    	if(avformat_open_input(&rtsp_fmt_ctx[i], rtsp_addrs[i], NULL, &options) != 0)//得到AVStream信息
    	{
    	    hy_zlog_info("Couldn't open input stream.\n");
    	    return -1;
    	}
    	/*得到流信息*/
    	if(avformat_find_stream_info(rtsp_fmt_ctx[i], NULL) < 0)//得到更多AVStream信息
    	{
    	    hy_zlog_info("Couldn't find stream information.\n");
    	    return -1;
    	}
        /*打印流信息*/
        av_dump_format(rtsp_fmt_ctx[i], 0, rtsp_addrs[i], 0);

        rtsp_packets[i] = (AVPacket *)av_malloc(sizeof(AVPacket));
        av_init_packet(rtsp_packets[i]);
    }

    return 0;
}


void hy_simple_stream_release()
{
    hy_u8 i;
    for (i=0; i<g_hy_nvr_info.hy_nvr_general_info.max_chan; i++)
    {
        av_free_packet(rtsp_packets[i]);
        avformat_close_input(&rtsp_fmt_ctx[i]);//	已包含avformat_free_context(pFormatCtx)操作;
    }
}


//用于获取数据
int hy_simple_stream_get_original_frame(hy_u8 channel, hy_s8 **p_data, hy_u32 *data_size)
{
   channel = channel - 1;
   int ret;

   avformat_set_packet_miss_flag();
   avcodec_set_decode_error_flag();
   avcodec_set_next_picture_error_flag();
   avcodec_set_av_caval_error_flag();
   avcodec_set_h264_error_flag();

   ret = av_read_frame(rtsp_fmt_ctx[channel], rtsp_packets[channel]);
   if( ret == 0 )
   {
        if (avformat_get_packet_miss_flag() == -1 || avcodec_get_next_picture_error_flag() == -1 || avcodec_get_decode_error_flag() == -1
            || avcodec_get_av_caval_error_flag() == -1 || avcodec_get_h264_error_flag() == -1)
        {
            //hy_zlog_info("miss package");
            ret = -2;
        }
        else
        {
            //hy_zlog_info("rtsp_data_size is %d stream_id is %d\n",rtsp_packets[channel]->size, rtsp_packets[channel]->stream_index);
            *p_data = rtsp_packets[channel]->data;
            *data_size = rtsp_packets[channel]->size;
        }
   }
   else
   {
        hy_zlog_info("av_read_frame error with %d\n", ret);

        hy_zlog_info("avformat_close_input\n");
        av_free_packet(rtsp_packets[channel]);
        *p_data = NULL;
        avformat_close_input(&rtsp_fmt_ctx[channel]);

    	AVDictionary* options = NULL;
        av_dict_set(&options, "rtsp_transport", "tcp", 0);
        hy_zlog_info("reopen stream\n");
    	if(avformat_open_input(&rtsp_fmt_ctx[channel], rtsp_addrs[channel], NULL, &options) != 0)//得到AVStream信息
    	{
    	    hy_zlog_info("Couldn't open input stream.\n");
    	}
    	/*得到流信息*/
    	if(avformat_find_stream_info(rtsp_fmt_ctx[channel], NULL) < 0)//得到更多AVStream信息
    	{
    	    hy_zlog_info("Couldn't find stream information.\n");
    	}
        /*打印流信息*/
        av_dump_format(rtsp_fmt_ctx[channel], 0, rtsp_addrs[channel], 0);
        rtsp_packets[channel] = (AVPacket *)av_malloc(sizeof(AVPacket));
        av_init_packet(rtsp_packets[channel]);
   }

   return ret;
}

int hy_simple_stream_record_video(hy_u8 channel, const hy_s8 *file_name, hy_u8 seconds)
{
    hy_zlog_info("stream record channel %d.\n", channel);
    if (channel < 1 || channel >g_hy_nvr_info.hy_nvr_general_info.max_chan)
    {
        return -1;
    }
	 /*得到录像指令*/
	 //ffmpeg -i "rtsp://admin:12345@172.16.8.196:554/Streaming/Channels/2?transportmode=unicast&profile=Profile_2" -c copy -f flv test.flv
     //sprintf(command, "ffmpeg -rtsp_transport tcp -i \"%s\" -vcodec copy -an -movflags rtphint -t %d %s",rtsp_addr, second, filename);
	 char command[256];
	 channel = channel - 1;// 1-------------->0
	 sprintf(command, "/root/ffmpeg -rtsp_transport tcp -i \"%s\" -vcodec copy -an -movflags rtphint -t %d %s", record_rtsp_addrs[channel], seconds, file_name);
	 hy_zlog_info("starting record. record video command:\n");
	 hy_zlog_info("%s\n", command);
	 system(command);
	 hy_zlog_info("cpature record done.\n");
	 return 0;
}

int hy_simple_stream_caputre_picture(hy_u8 channel, const hy_s8 *file_name)
{
    hy_zlog_info("stream picture caputure channel %d.\n", channel);
    if (channel < 1 || channel >g_hy_nvr_info.hy_nvr_general_info.max_chan)
    {
        return -1;
    }
	 /*得到截图指令*/
	 //ffmpeg -i "rtsp://admin:12345@172.16.8.245:554/Streaming/Channels/2?transportmode=unicast&profile=Profile_2" -t 0.001 test.jpg
	 char command[256];
	 hy_zlog_info("channel %d.\n", channel);
	 channel = channel - 1;// 1-------------->0
	 //-s 640x480 图片大小
	 sprintf(command, "/root/ffmpeg -i \"%s\" -t 0.001 -s %s  %s",record_rtsp_addrs[channel], PICTURE_SIZE, file_name);
	 hy_zlog_info("starting capture. capture picture command:\n");
	 hy_zlog_info("%s\n",command);
	 system(command);
     hy_zlog_info("cpature pic done.\n");
	 return 0;
}

int hy_simple_stream_start_realplay(hy_char *p_ip, unsigned int dvr, unsigned int ipc, unsigned int stream)
{
    if(ipc > g_hy_nvr_info.hy_nvr_general_info.max_chan || ipc < 1)
    {
        return 0;
    }

    hy_char *source_rtsp_addr = real_play_rtsp_addrs[ipc - 1];
    hy_char publish_addr[128] = {0};
    sprintf(publish_addr, "rtsp://%s/%05d-%02d-%02d.sdp", p_ip, dvr, ipc, stream);

    /*get cmd*/
    //TBJ:sprintf(ffmpeg_cmd, "ffmpeg -rtsp_transport tcp -i \"%s\" -c copy -an -f rtsp -rtsp_transport tcp rtsp://%s/%05d-01-02.sdp", rtsp,stream_ip,dvr_id);
    if (stream_is_opened)
    {
        hy_simple_stream_stop_realplay();
    }

    //ffmpeg -i "rtsp:/admin:huiyuan123@172.16.8.245:554/Streaming/Channels/3?transportmode=unicast&profile=Profile_3"  -c copy -an -f rtsp rtsp://172.16.8.114/10000-01-02.sdp
    memset(current_play_cmd, 0, 256);
    sprintf(current_play_cmd, "/root/ffmpeg -rtsp_transport tcp -i \"%s\" -c copy -an -f rtsp -rtsp_transport tcp %s &", source_rtsp_addr, publish_addr);
    hy_zlog_info("start realplay command:\n %s\n", current_play_cmd);
    pid_t status = system(current_play_cmd);//system function is blocking

    //暂时不在切换通道的时候改变算法通道
    //g_hy_detect_info_t.change_to_channel = ipc;
    stream_is_opened = HY_TRUE;

    return 0;
}

//int hy_simple_stream_start_realplay(const hy_char *source_rtsp_addr, const hy_char *publish_addr)
//{
//    /*get cmd*/
//    //ffmpeg -rtsp_transport tcp -i "rtsp://admin:12345@172.16.8.245:554/Streaming/Channels/1?transportmode=unicast&profile=Profile_1" -f rtsp rtsp://172.16.8.114/test.sdp
//    //TBJ:sprintf(ffmpeg_cmd, "ffmpeg -rtsp_transport tcp -i \"%s\" -c copy -an -f rtsp -rtsp_transport tcp rtsp://%s/%05d-01-02.sdp", rtsp,stream_ip,dvr_id);
//    if (stream_is_opened)
//    {
//        hy_simple_stream_stop_realplay();
//    }

//    //ffmpeg -i "rtsp:/admin:huiyuan123@172.16.8.245:554/Streaming/Channels/3?transportmode=unicast&profile=Profile_3"  -c copy -an -f rtsp rtsp://172.16.8.114/10000-01-02.sdp
//    memset(current_play_cmd, 0, 256);
//    sprintf(current_play_cmd, "ffmpeg -rtsp_transport tcp -i \"%s\" -c copy -an -f rtsp -rtsp_transport tcp %s &", source_rtsp_addr, publish_addr);
//    hy_zlog_info("start realplay command:\n");
//    hy_zlog_info("%s\n", current_play_cmd);
//    pid_t status = system(current_play_cmd);//system function is blocking

//    stream_is_opened = HY_TRUE;
//    //if (WIFEXITED(status))
//    //{
//    //    hy_zlog_info("exec %s failed\n.", cmd);
//    //    return -1;
//    //}

//    return 0;
//}

int hy_simple_stream_stop_realplay()
{
    if (stream_is_opened)
    {
//        hy_u32 i = 0;
//        current_play_cmd[strlen(current_play_cmd) -1 ] = 0;
//        for (i=0; i<strlen(current_play_cmd); i++)
//        {
//            if (current_play_cmd[i] == '"')
//            {
//                memmove(&current_play_cmd[i], &current_play_cmd[i+1], strlen(current_play_cmd)-i+1);
//            }
//        }

        hy_char cmd[256];
        sprintf(cmd, "kill -9 `pidof %s`", "ffmpeg");

        hy_zlog_info("cmd is %s\n", cmd);
        system(cmd);
        usleep(1000);//1 ms
        stream_is_opened = HY_FALSE;
    }
    return 0;
}
