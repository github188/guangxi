/******************** (C) COPYRIGHT 2014 HY ********************
* File Name          : hy_define.h
* Author             : 
* Revision           : 
* Date              : 
* Description        : base define header file 
* Change log		: 1.Initial version :2014-04-12 by zcx
*                   ++++++++++++++++++
********************************************************************************
* THE PRESENT SOFTWARE IS DISTRIBUTED IN THE HOPE THAT IT WILL PROVIDE CUSTOMERS 
* WITH CODING INFORMATION TO SPEED UP THEIR PRODUCT DEVELOPMENT, BUT WITHOUT
* ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS
* FOR A PARTICULAR PURPOSE. AS A RESULT, TECHWELL SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef _HY_DEFINE_H_
#define _HY_DEFINE_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    HY_ENC_MODULE_TYPE              = 0,   //����ģ��
    HY_MV_MODULE_TYPE                  ,   //MVģ��
    HY_CAPTURE_MODULE_TYPE             ,  //Capture ģ��
    HY_ALARM_MODULE_TYPE               ,   //ҵ�񱨾�ģ��
    HY_FILE_STORE_MODULE_TYPE          ,   //�ļ��洢ģ��
    HY_FILE_TRANSFER_MODULE_TYPE       ,   //�ļ��ϴ�ģ��
    HY_STREAM_CTL_MODULE_TYPE          ,   //stream ctl module
//    HY_RS422_MODULE_TYPE               ,   //�����ϱ�ģ��
    HY_SYS_MONITOR_MODULE_TYPE         ,   //ϵͳ���ģ��                                       //
    HY_NET_CTL_MODULE_TYPE             ,   //�������ģ��
    HY_NET_STREAM_MODULE_TYPE          ,   //����������ģ��
   
    
    /*����Ϊ���������ģ�飬����ζ��һЩ��Դ�����ᱻ����������Ϣ���е�*/
    HY_USER_MGT_MODULE_TYPE            ,   //�û�����ģ��
    HY_SYS_CFG_MODULE_TYPE             ,   //ϵͳ����
    HY_DB_MODULE                       ,   //���ݿ�ģ�飬������ݿⲻ�ܱ���ȡ��д��
    HY_LOG_MODULE_TYPE                 ,   //ϵͳ��־ģ�飬�û���ѯ������

    HY_MODULE_BUTTON                   ,   //ģ���������Ҫ����ѭ���������ж�


}HY_MODULE_DEF_E;






#ifdef __cplusplus
}
#endif //__cplusplus


#endif  //_HY_DEFINE_H_
