/******************** (C) COPYRIGHT 2014 HY ********************
* File Name          : hy_err_define.h
* Author             : 
* Revision           : 
* Date              : 
* Description        : ERR define header file 
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

#ifndef _HY_ERR_DEFINE_H_
#define _HY_ERR_DEFINE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"
#include "hy_define.h"

#define ERR_STR_BUFF_SIZE (64)

#define SET_ERR_MASK(X)  (-(X<<10))
#define GET_ERR_MASK(X)  ((-X)>>10)

#define SET_ERR_TYPE(MODULE,ERR_CODE) (SET_ERR_MASK(MODULE)-(ERR_CODE))
#define GET_MODULE_N_BY_ERR_CODE(ERR_CODE)  ((-(ERR_CODE))>>10)

#define GET_MODULE_CHILD_ERR_CODE(ERR_CODE) ((-(ERR_CODE))&0x3ff)


/************************************************************
 * ���۴������Ͷ���
 * 
 * @author zcx 
 * 
 * 
 *  
 ************************************************************/

typedef enum{

    HY_OPEN_IO_FAILED     = -10,               //��IO��ʧ��
    HY_NO_MEM             = -11,               //û���ڴ�

    HY_ENC_ERR_BASE       =  SET_ERR_MASK(HY_ENC_MODULE_TYPE) -100,   //�������ʼ,Ϊ�˲���HY_FAILURE��ͻ�����յ�-100
    HY_ENC_START_ERROR = HY_ENC_ERR_BASE -1,  // ����ģ����������
    HY_ENC_STOP_ERROR = HY_ENC_START_ERROR -1			, // ����ģ���������
    HY_ENC_EXIT_ERROR = HY_ENC_STOP_ERROR -1			,  // ����ģ���˳�����
    HY_ENC_SET_BOLT_VENC_PARAM_ERROR  = HY_ENC_EXIT_ERROR -1		, // ǹ�������������
    HY_ENC_GET_BOLT_VENC_PARAM_ERROR = HY_ENC_SET_BOLT_VENC_PARAM_ERROR -1		, // ��ȡǹ�������������
    HY_ENC_SET_BALL_VENC_PARAM_ERROR = HY_ENC_GET_BOLT_VENC_PARAM_ERROR -1		, // �������������� 
    HY_ENC_GET_BALL_VENC_PARAM_ERROR	 = HY_ENC_SET_BALL_VENC_PARAM_ERROR -1	, // ��ȡ��������������
    HY_ENC_SET_BOLT_VENC_CONFIG_ERROR = HY_ENC_GET_BALL_VENC_PARAM_ERROR -1	, // ǹ�����������ȷ������HDVICPʱ����
    HY_ENC_SET_BALL_VENC_CONFIG_ERROR = HY_ENC_SET_BOLT_VENC_CONFIG_ERROR -1	,// ������������ȷ������HDVICPʱ����
    HY_ENC_SET_MDTRACK_ALG_PARAM_ERROR = HY_ENC_SET_BALL_VENC_CONFIG_ERROR -1	, // �㷨���в�������
    HY_ENC_GET_MDTRACK_ALG_PARAM_ERROR = HY_ENC_SET_MDTRACK_ALG_PARAM_ERROR -1	, // ��ȡ�㷨���в�������
    HY_ENC_SET_MDTRACK_ROI_PARAM_ERROR = HY_ENC_GET_MDTRACK_ALG_PARAM_ERROR -1	, // ROI
    HY_ENC_GET_MDTRACK_ROI_PARAM_ERROR = HY_ENC_SET_MDTRACK_ROI_PARAM_ERROR -1	,
    HY_ENC_SET_MDTRACK_SEMIAUTO_PARAM_ERROR = HY_ENC_GET_MDTRACK_ROI_PARAM_ERROR -1	,
    HY_ENC_GET_MDTRACK_SEMIAUTO_PARAM_ERROR = HY_ENC_SET_MDTRACK_SEMIAUTO_PARAM_ERROR -1	,
    HY_ENC_SET_MDTRACK_TRIPWIRE_PARAM_ERROR = HY_ENC_GET_MDTRACK_SEMIAUTO_PARAM_ERROR -1	,
    HY_ENC_GET_MDTRACK_TRIPWIRE_PARAM_ERROR = HY_ENC_SET_MDTRACK_TRIPWIRE_PARAM_ERROR -1	,
    HY_ENC_CREATE_THREAD_ERR = HY_ENC_GET_MDTRACK_TRIPWIRE_PARAM_ERROR -1        ,  // ����ģ���̳߳���
    HY_ENC_BUF_RELEASE_ERR  = HY_ENC_CREATE_THREAD_ERR -1,   // ����Ŀռ��ͷų���
    
    HY_ENC_ERR_END        =  HY_ENC_BUF_RELEASE_ERR -1        ,

    
    HY_ALARM_ERR_BASE        =  SET_ERR_MASK(HY_ALARM_MODULE_TYPE), //ҵ�񱨾�����ʼ


   
    HY_NET_CTL_ERR_BASE   =  SET_ERR_MASK(HY_NET_CTL_MODULE_TYPE), //���������Ϣ��ʼ


    HY_NET_STREAM_ERR_BASE = SET_ERR_MASK(HY_NET_STREAM_MODULE_TYPE), //������������Ϣ��ʼ


    HY_USER_MGT_ERR_BASE   = SET_ERR_MASK(HY_USER_MGT_MODULE_TYPE),

    HY_SYS_CFG_ERR_BASE    = SET_ERR_MASK(HY_SYS_CFG_MODULE_TYPE),

    HY_DB_ERR_BASE         = SET_ERR_MASK(HY_DB_MODULE),

    HY_LOG_ERR_BASE        = SET_ERR_MASK(HY_LOG_MODULE_TYPE),


}HY_ERR_CODE_TYPE_E;


typedef struct _hy_err_map_t{
    HY_ERR_CODE_TYPE_E  err_code;
    const hy_char *           err_str;
}hy_err_map_t;


    

/************************************************************
 *  
 * ���ݴ���Ż�ȡ�����ַ���
 *  
 ************************************************************/
const hy_char * hy_get_str_by_err_code(HY_ERR_CODE_TYPE_E err_code);


#ifdef __cplusplus
}
#endif //__cplusplus


#endif  //_HY_ERR_DEFINE_H_
