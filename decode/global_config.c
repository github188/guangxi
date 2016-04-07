/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :
* Change log		 :  Jimmy2015-7-10
********************* (C) COPYRIGHT 2015 HY *******************/
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "global_config.h"
#include "iniparser.h"
#include "hy_debug.h"
#include "hy_zlog.h"

//全局配置
hy_nvr_info_t g_hy_nvr_info;
hy_detect_info_t g_hy_detect_info_t;
hy_f32 version = 0.1;
int a9_update_flag = 0;

/**********************************
*Name:parse_nvr_ini_file
*Desc:
*Param:
*Return:-1 error
        0  ok
*Author:Jimmy
*Create Time:2015-7-10
*ChangeLog:
**********************************/
int parse_nvr_ini_file(hy_nvr_info_t *p_hy_nvr_info)
{
	dictionary	*	ini ;

	ini = iniparser_load("/root/NVR.ini");
	if (ini == NULL) {
		hy_zlog_info("cannot parse file: %s\n", "/root/NVR.ini");
		return -1 ;
	}

    //parse general
    p_hy_nvr_info->hy_nvr_general_info.max_chan = iniparser_getint(ini, "General:max_chan", -1);
    p_hy_nvr_info->hy_nvr_general_info.device_id= iniparser_getint(ini, "General:device_id", -1);
    p_hy_nvr_info->hy_nvr_general_info.width= iniparser_getint(ini, "General:width", -1);
    p_hy_nvr_info->hy_nvr_general_info.height= iniparser_getint(ini, "General:height", -1);
    sprintf(p_hy_nvr_info->hy_nvr_general_info.recv_path, "%s", iniparser_getstring(ini, "General:rec_path", NULL));

    //parse server
    p_hy_nvr_info->hy_nvr_server_info.server_ctl_port = iniparser_getint(ini, "Server:server_ctl_port", -1);
    p_hy_nvr_info->hy_nvr_server_info.server_data_port= iniparser_getint(ini, "Server:server_data_port", -1);
    p_hy_nvr_info->hy_nvr_server_info.upgrade_ctl_port= iniparser_getint(ini, "Server:upgrade_ctl_port", -1);
    p_hy_nvr_info->hy_nvr_server_info.server_ctl_port = iniparser_getint(ini, "Server:server_ctl_port", -1);
    sprintf(p_hy_nvr_info->hy_nvr_server_info.server_ip, "%s", iniparser_getstring(ini, "Server:server_ip", NULL));

    //parse channel
    hy_u8 i = 0;
    hy_s8 chan_type[24] = "Chan1:type";
    hy_s8 chan_ip[24] = "Chan1:ip";
    hy_s8 chan_login_name[24] = "Chan1:login_name";
    hy_s8 chan_login_passwd[24] = "Chan1:login_passwd";
    for (i; i<p_hy_nvr_info->hy_nvr_general_info.max_chan; i++)
    {
        chan_type[4] = i+48 +1;
        chan_ip[4] = i+48 + 1;
        chan_login_name[4] = i+48 + 1;
        chan_login_passwd[4] = i+48 + 1;
        p_hy_nvr_info->hy_nvr_channel_info[i].type = iniparser_getint(ini, chan_type, -1);
        sprintf(p_hy_nvr_info->hy_nvr_channel_info[i].ip, "%s", iniparser_getstring(ini, chan_ip, NULL));
        sprintf(p_hy_nvr_info->hy_nvr_channel_info[i].login_name, "%s", iniparser_getstring(ini, chan_login_name, NULL));
        sprintf(p_hy_nvr_info->hy_nvr_channel_info[i].login_passwd, "%s", iniparser_getstring(ini, chan_login_passwd, NULL));
    }



	iniparser_freedict(ini);

	return 0 ;
}

int parse_detect_info_file(hy_detect_info_t *p_hy_detect_info_t)
{
    struct stat temp;
    hy_u32 file_len;
    FILE *fp;
    int stat_ret = stat("/root/mv.config", &temp);
    if (stat_ret != -1)
    {
        file_len =  temp.st_size;
    	fp = fopen("/root/mv.config", "rb+");
    	if (!fp)
    	{
    		hy_zlog_info("open mv detect config failed.\n");
    		return -1;
    	}
    }
    else
    {
        file_len = 0;
    	fp = fopen("/root/mv.config", "wb+");
    	if (!fp)
    	{
    		hy_zlog_info("open mv detect config failed.\n");
    		return -1;
    	}
    }

    if (file_len < sizeof(hy_detect_info_t))//重新初始化
    {
        hy_zlog_info("file_len %d < %d re init hy_detect_info_t\n", file_len, sizeof(hy_detect_info_t));
        memset(p_hy_detect_info_t->channel_detect_area, 0, sizeof(hy_channel_detect_area_t)*MAX_CHANLE_NUM);
        fwrite(p_hy_detect_info_t, sizeof(hy_detect_info_t), 1, fp);
    }
    else
    {
        fread(p_hy_detect_info_t, sizeof(hy_detect_info_t), 1, fp);
        hy_zlog_info("parsing detect area\n");
        int i;
        for (i=0; i<g_hy_nvr_info.hy_nvr_general_info.max_chan; i++)
        {
            hy_zlog_info("channel %d have %d rect.\n", i+1, p_hy_detect_info_t->channel_detect_area[i].rectcount);
            int j;
            for (j=0; j<p_hy_detect_info_t->channel_detect_area[i].rectcount; j++)
            {
                hy_zlog_info("chanel %d rect %d  left %d top %d right %d bottom %d\n", i+1, j+1, p_hy_detect_info_t->channel_detect_area[i].rect[j].left, \
                p_hy_detect_info_t->channel_detect_area[i].rect[j].top,\
                p_hy_detect_info_t->channel_detect_area[i].rect[j].right,\
                p_hy_detect_info_t->channel_detect_area[i].rect[j].bottom);
            }
        }
    }

    //初始化算法要使用的通道
    //p_hy_detect_info_t->cur_channel = g_hy_nvr_info.hy_nvr_general_info.max_chan;
    //p_hy_detect_info_t->change_to_channel = g_hy_nvr_info.hy_nvr_general_info.max_chan;
    p_hy_detect_info_t->cur_channel = 1;
    p_hy_detect_info_t->change_to_channel = 1;
    p_hy_detect_info_t->starting = HY_TRUE;
    p_hy_detect_info_t->scheduler_starting = HY_TRUE;
    p_hy_detect_info_t->update_config = HY_TRUE;

    hy_zlog_info("parse detect area done.\n");
    fclose(fp);
    return 0;
}

int update_detect_info_file(hy_detect_info_t *p_hy_detect_info_t)
{
	FILE *fp;
	fp = fopen("/root/mv.config", "rb+");
	if (!fp)
	{
		hy_zlog_info("open mv detect config failed.\n");
		return -1;
	}

    int i;
    for (i=0; i<g_hy_nvr_info.hy_nvr_general_info.max_chan; i++)
    {
        hy_zlog_info("channel %d have %d rect.\n", i+1, p_hy_detect_info_t->channel_detect_area[i].rectcount);
        int j;
        for (j=0; j<p_hy_detect_info_t->channel_detect_area[i].rectcount; j++)
        {
            hy_zlog_info("chanel %d rect %d  left %d top %d right %d bottom %d\n", i+1, j+1, p_hy_detect_info_t->channel_detect_area[i].rect[j].left, \
            p_hy_detect_info_t->channel_detect_area[i].rect[j].top,\
            p_hy_detect_info_t->channel_detect_area[i].rect[j].right,\
            p_hy_detect_info_t->channel_detect_area[i].rect[j].bottom);
        }
    }
    fwrite(p_hy_detect_info_t, sizeof(hy_detect_info_t), 1, fp);

    fclose(fp);
    return 0;
}