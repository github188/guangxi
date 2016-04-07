/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :
* Change log		 :  Jimmy2015-12-22
********************* (C) COPYRIGHT 2015 HY *******************/

#ifndef HY_MK60_CTRL_H
#define HY_MK60_CTRL_H
//#include "common_types.h"

#define FRAM_TYPE_DEVICE_TO_SRV 0X20   //AR-MK60
#define FRAM_TYPE_SRV_TO_DEVICE 0X21   //MK60-AR
#define MT_PACK_TYPE_VISABLE_CAMER_POWER  0x40//�򿪿ɼ������
#define MT_PACK_TYPE_HORN_POWER  0x41//�����ȵ�Դ

/*DSPͬMK60������֡�ṹ*/
typedef struct _dsp_mk60_frame_pack_t
{
    u_int16_t flag;                       // 2byte ֡��ʼ��־������Ϊ5AA5
    u_int16_t data_len;                   // 2byte �������ݳ���
    u_int8_t id;                          //1byte�豸ID
    u_int8_t frame_type;                  //1byte֡����
    u_int8_t pack_type;                   //1byte��������
    u_int8_t data;                     //���ƵĽ���ֻ��1���ֽڵ�����
} dsp_mk60_frame_pack_t;

typedef struct _power_satua_t
{
     u_int8_t status;//0x01 �򿪵�Դ 0x02 �رյ�Դ
} power_status_t;

typedef struct _recv_status_t
{
     u_int8_t  status;//0xff�ɹ� 0x00 ʧ��
} recv_status_t;

int send_open_camera_package_to_mk60();
void send_close_camera_package_to_mk60(void *parm);
int send_open_alarm_package_to_mk60();
void send_close_alarm_package_to_mk60(void *parm);

int send_cmd_package_to_mk60(u_int8_t pack_type, u_int8_t data);
int start_recv_order_from_mk60_thread();

#endif