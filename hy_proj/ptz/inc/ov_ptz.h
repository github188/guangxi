/**************************************
 *onvif ptz������������
 *date:     2014.6.23
 *changelog:
 *Author:   Kevin_zhang
 *************************************/

#ifndef __OV_PTZ_H__
#define __OV_PTZ_H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <uuid/uuid.h>
#include "soapH.h"

#include "ov_types.h"

ov_tour_t *ov_get_tour_by_name(ov_dev_t *dev, char *tour_name);

/******************************************************************************
* Name:
* Desc:  �ƶ������������ƣ����ƣ����ƣ����ƣ���������Զ��ֹͣ
* Param:
    struct soap *soap
    ov_dev_t *dev                       �豸�ṹ�壬���а����豸������Ҫ�漰���ĸ��ֵ�ַ����ǩ
    x       ˮƽ�����ƶ�����
    y       ��ֱ�����ƶ�����
    z       �佹����
* Return: int,  error number
 ******************************************************************************/

int ov_ptz_relative_move(struct soap *soap, ov_dev_t *dev, float x, float y, float z);
int ov_ptz_continuous_move(struct soap *soap, ov_dev_t *dev, float x, float y, float z);
int ov_ptz_stop_move(struct soap *soap, ov_dev_t *dev);


/******************************************************************************
* Name:  ov_ptz_set_preset
*
* Desc:  ���õ�ǰλ��Ϊһ��Ԥ�õ�
* Param:
    struct soap *soap
    ov_dev_t *dev                 �豸�ṹ�壬���а����豸������Ҫ�漰���ĸ��ֵ�ַ����ǩ
    const char *preset_token      ���tokenΪNULL�Ļ����𴴽��µ�Ԥ�õ㣻�����ΪNULL���޸Ķ�Ӧ
                                  preset_token������
    const char *presetname        Ԥ�õ������
* Return: int,  error number

 ******************************************************************************/

int ov_ptz_return_home_position(struct soap *soap, ov_dev_t *dev);
int ov_ptz_set_home_position(struct soap *soap, ov_dev_t *dev);

int ov_ptz_get_presets(struct soap *soap, ov_dev_t *dev);
int ov_ptz_set_preset(struct soap *soap, ov_dev_t *dev, int id);
int ov_ptz_goto_preset(struct soap *soap, ov_dev_t *dev, int id);
int ov_ptz_remove_preset(struct soap *soap, ov_dev_t *dev, int id);

//int ov_ptz_get_configs(struct soap *soap, ov_dev_t *dev, struct _tptz__GetConfigurationsResponse *tptz__GetConfigurationsResponse);
//int ov_ptz_add_config(struct soap *soap, ov_dev_t *dev, char *ptz_config_token);
//int ov_ptz_remove_config(struct soap *soap, ov_dev_t *dev);

/*Ԥ��·����ȡ*/
int ov_ptz_get_tours(struct soap *soap, ov_dev_t *dev);
/*Ԥ��·������*/
int ov_ptz_add_tour(struct soap *soap, ov_dev_t *dev, int id, ov_preset_t *presets, int presets_num);
/*Ԥ��·��ɾ��*/
int ov_ptz_del_tour(struct soap *soap, ov_dev_t *dev, int id);
/*����tour����·�������ļ�*/
int ov_ptz_create_tours_ini(ov_dev_t *dev);
void write_tours_number(FILE *fp, ov_dev_t *dev);
/*��ӡ����Ԥ��·��*/
void write_single_tour_message(FILE *fp, ov_tour_t *tour);
/*��ӡ����·�������е�Ԥ�Ƶ�*/
void write_single_tour_presets_message(FILE *fp, ov_tour_t *tour);

int ov_ptz_create_tour(struct soap *soap, ov_dev_t *dev, ov_preset_t *presets, int presets_num);
int ov_ptz_modify_tour(struct soap *soap, ov_dev_t *dev, char *tour_token, ov_preset_t *presets, int presets_num);
int ov_ptz_remove_tour(struct soap *soap, ov_dev_t *dev, char *tour_token);
int ov_ptz_start_one_tour(struct soap *soap, ov_dev_t *dev, int id);
int ov_ptz_stop_one_tour(struct soap *soap, ov_dev_t *dev, int id);
int ov_ptz_operate_tour(struct soap *soap, ov_dev_t *dev, char *tour_token, enum tt__PTZPresetTourOperation op);

#endif

