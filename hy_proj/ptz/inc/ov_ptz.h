/**************************************
 *onvif ptz操作函数声明
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
* Desc:  移动操作、如左移，右移，上移，下移，拉近，拉远，停止
* Param:
    struct soap *soap
    ov_dev_t *dev                       设备结构体，其中包含设备操作需要涉及到的各种地址，标签
    x       水平方向移动幅度
    y       竖直方向移动幅度
    z       变焦幅度
* Return: int,  error number
 ******************************************************************************/

int ov_ptz_relative_move(struct soap *soap, ov_dev_t *dev, float x, float y, float z);
int ov_ptz_continuous_move(struct soap *soap, ov_dev_t *dev, float x, float y, float z);
int ov_ptz_stop_move(struct soap *soap, ov_dev_t *dev);


/******************************************************************************
* Name:  ov_ptz_set_preset
*
* Desc:  设置当前位置为一个预置点
* Param:
    struct soap *soap
    ov_dev_t *dev                 设备结构体，其中包含设备操作需要涉及到的各种地址，标签
    const char *preset_token      如果token为NULL的话，责创建新的预置点；如果不为NULL责修改对应
                                  preset_token的配置
    const char *presetname        预置点的名字
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

/*预置路径获取*/
int ov_ptz_get_tours(struct soap *soap, ov_dev_t *dev);
/*预置路径创建*/
int ov_ptz_add_tour(struct soap *soap, ov_dev_t *dev, int id, ov_preset_t *presets, int presets_num);
/*预置路径删除*/
int ov_ptz_del_tour(struct soap *soap, ov_dev_t *dev, int id);
/*根据tour创建路径配置文件*/
int ov_ptz_create_tours_ini(ov_dev_t *dev);
void write_tours_number(FILE *fp, ov_dev_t *dev);
/*打印单个预制路径*/
void write_single_tour_message(FILE *fp, ov_tour_t *tour);
/*打印单个路径中所有的预制点*/
void write_single_tour_presets_message(FILE *fp, ov_tour_t *tour);

int ov_ptz_create_tour(struct soap *soap, ov_dev_t *dev, ov_preset_t *presets, int presets_num);
int ov_ptz_modify_tour(struct soap *soap, ov_dev_t *dev, char *tour_token, ov_preset_t *presets, int presets_num);
int ov_ptz_remove_tour(struct soap *soap, ov_dev_t *dev, char *tour_token);
int ov_ptz_start_one_tour(struct soap *soap, ov_dev_t *dev, int id);
int ov_ptz_stop_one_tour(struct soap *soap, ov_dev_t *dev, int id);
int ov_ptz_operate_tour(struct soap *soap, ov_dev_t *dev, char *tour_token, enum tt__PTZPresetTourOperation op);

#endif

