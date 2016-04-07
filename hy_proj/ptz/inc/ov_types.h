/************************************
 *用户自定义类型
 *date:     2014.6.23
 *changelog:
 *
 ************************************/

#ifndef __OV_TYPES_H__
#define __OV_TYPES_H__

#include "ov_list.h"

#define OV_DEV_MAX_SIZE 10 //最大支持IPC数
#define OV_PRESETS_NUM_EACH_DEV 32 //每个IPC最多支持的预置点数量
#define OV_TOURS_NUM_EACH_DEV 8     //每个IPC最多支持的预置路径数量

/*预置点描述*/
typedef struct
{
	int id;               //预置点编号
	char token[4];		  //预置点表征
	long long  duration;  //预置点停留时间
	float speed;		  //移动至该预置点速度

	struct list_head list;
}ov_preset_t;

/*预置路径描述*/
typedef struct
{
	int id;
	char token[4];

	struct list_head list;

	struct list_head presets_list;
	int presets_count;
}ov_tour_t;

/*设备profile描述*/
typedef struct
{
	char name[16];//mainStream,subStream,thirdStream
	char token[16];
	char rtsp_addr[196];//rtsp_addr
}ov_profile_t;


typedef struct
{
	char usr[16];
	char pwd[16];
}ov_authentication_t;


/*onvif设备描述*/
typedef struct
{
	char *serial_number;
	ov_authentication_t *login;

	char *dev_srv_addr;//http://172.16.8.196/onvif/device_service
	char *med_srv_addr;//http://172.16.8.196/onvif/Media
	char *ptz_srv_addr;//http://172.16.8.196/onvif/ptz

	ov_profile_t *profiles;//ptz控制默认使用profile[0]
	int profiles_size;

	struct list_head presets_list;
	int presets_count;
	struct list_head tours_list;
	ov_tour_t tours[OV_TOURS_NUM_EACH_DEV];
	int tours_count;
}ov_dev_t;

typedef struct
{
	ov_dev_t dev[OV_DEV_MAX_SIZE];
	int num;
}ov_devices_list;

#endif
