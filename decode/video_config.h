/*************************************
 *视频参数设置
 *date:     2015.02.09
 *changelog:
 *
 *************************************/

#ifndef __VEDIO_CONFIG_H__
#define __VEDIO_CONFIG_H__

void video_config_set_encoder(char *ip, char *cmd, int len);
void video_osd_clear(void);
void video_osd_show(char *name, int num);
void video_set_camera_time(void);
void video_set_main_stream_params(void);

#endif

