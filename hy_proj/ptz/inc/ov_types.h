/************************************
 *�û��Զ�������
 *date:     2014.6.23
 *changelog:
 *
 ************************************/

#ifndef __OV_TYPES_H__
#define __OV_TYPES_H__

#include "ov_list.h"

#define OV_DEV_MAX_SIZE 10 //���֧��IPC��
#define OV_PRESETS_NUM_EACH_DEV 32 //ÿ��IPC���֧�ֵ�Ԥ�õ�����
#define OV_TOURS_NUM_EACH_DEV 8     //ÿ��IPC���֧�ֵ�Ԥ��·������

/*Ԥ�õ�����*/
typedef struct
{
	int id;               //Ԥ�õ���
	char token[4];		  //Ԥ�õ����
	long long  duration;  //Ԥ�õ�ͣ��ʱ��
	float speed;		  //�ƶ�����Ԥ�õ��ٶ�

	struct list_head list;
}ov_preset_t;

/*Ԥ��·������*/
typedef struct
{
	int id;
	char token[4];

	struct list_head list;

	struct list_head presets_list;
	int presets_count;
}ov_tour_t;

/*�豸profile����*/
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


/*onvif�豸����*/
typedef struct
{
	char *serial_number;
	ov_authentication_t *login;

	char *dev_srv_addr;//http://172.16.8.196/onvif/device_service
	char *med_srv_addr;//http://172.16.8.196/onvif/Media
	char *ptz_srv_addr;//http://172.16.8.196/onvif/ptz

	ov_profile_t *profiles;//ptz����Ĭ��ʹ��profile[0]
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
