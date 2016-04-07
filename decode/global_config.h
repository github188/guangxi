/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :global_config.h
* Author             :Jimmy
* Revision           :
* Description        :1、定义全局变量
                      2、NVR.ini DetectRoutePlan.ini RecordPlan.ini 等文件的解析
* Change log		 :  Jimmy2015-7-9
********************* (C) COPYRIGHT 2015 HY *******************/

#ifndef _GLOBAL_CONFIG_H
#define _GLOBAL_CONFIG_H
#include "hy_type.h"
#include "hy_protocl.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
    MAX_CHANLE_NUM = 4
} HY_CHANLE_NUM_E;
typedef enum
{
    MAX_DETECT_RECT = 4
} HY_DETECT_RECT_NUM_E;

struct _hy_nvr_general_info_t
{
    hy_u8 max_chan;
    hy_u32 device_id;
    hy_s8 recv_path[24];
    hy_u32 width;
    hy_u32 height;
};

struct _hy_nvr_server_info_t
{
    hy_s8 server_ip[24];
    hy_u16 server_ctl_port;
    hy_u16 server_data_port;
    hy_u16 audio_data_port;
    hy_u16 upgrade_ctl_port;
};

struct _hy_nvr_channel_info_t
{
    hy_s8 type;
    hy_s8 ip[24];
    hy_s8 login_name[24];
    hy_s8 login_passwd[24];
};


typedef struct _hy_nvr_info_t
{
    struct _hy_nvr_general_info_t hy_nvr_general_info;
    struct _hy_nvr_server_info_t hy_nvr_server_info;
    struct _hy_nvr_channel_info_t hy_nvr_channel_info[4];
} hy_nvr_info_t;



//typedef struct _hy_mv_rect_t
//{
//	int left;
//	int top;
//	int right;
//	int bottom;
//} hy_mv_rect_t;

//typedef struct _hy_channel_detect_area_t
//{
//	unsigned char	channel;	// 通道号
//	unsigned char   delay;		// 0 关闭检测 ， 大于0 标志打开检测及报警延迟时间,单位：秒
//	unsigned char	grade ;		// 运动检测的灵敏度,设定得值范围3-8--->input
//	unsigned char	rectcount;	// 当前路视频实际的运动检测区域数量--->input
//	float           w_h_scale;  // 宽高比
//	float           Arescale;   // 宽高比
//	hy_mv_rect_t		rect[MAX_DETECT_RECT];//运动检测区域坐标--->input
//} hy_channel_detect_area_t;

typedef struct _hy_detect_info_t
{
    HY_BOOL starting;//检测算法是否开启.
    HY_BOOL scheduler_starting;//通道调度是否开启，即自动切换的算法是否开启,decode线程的算法是否开启取决的顺序是starting, scheduler_starting
    hy_u8 cur_channel;//当前检测的通道
    hy_u8 change_to_channel;//将要切换到的通道

    HY_BOOL update_config;//在启动的时候，在WEB端更改配置的时候会配置为updating
    hy_channel_detect_area_t channel_detect_area[MAX_CHANLE_NUM];
} hy_detect_info_t;

typedef struct _hy_scene_parameters_t
{
    HY_BOOL starting;//检测算法是否开启.
    HY_BOOL scheduler_starting;//通道调度是否开启，即自动切换的算法是否开启,decode线程的算法是否开启取决的顺序是starting, scheduler_starting
    hy_u8 cur_channel;//当前检测的通道
    hy_u8 change_to_channel;//将要切换到的通道

    HY_BOOL update_config;//在启动的时候，在WEB端更改配置的时候会配置为updating
    int num;//场景的个数
    hy_scene_parameter_t scene_param[12];
} _hy_scene_parameters;

extern hy_detect_info_t g_hy_detect_info_t;
extern hy_nvr_info_t g_hy_nvr_info;
extern hy_f32 version;
extern int a9_update_flag;

int parse_nvr_ini_file(hy_nvr_info_t *p_hy_nvr_info);
int parse_detect_info_file(hy_detect_info_t *p_hy_detect_info_t);
int update_detect_info_file(hy_detect_info_t *p_hy_detect_info_t);

#ifdef __cplusplus
}
#endif

#endif

