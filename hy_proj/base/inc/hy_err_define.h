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
 * 新舟错误类型定义
 * 
 * @author zcx 
 * 
 * 
 *  
 ************************************************************/

typedef enum{

    HY_OPEN_IO_FAILED     = -10,               //打开IO口失败
    HY_NO_MEM             = -11,               //没有内存

    HY_ENC_ERR_BASE       =  SET_ERR_MASK(HY_ENC_MODULE_TYPE) -100,   //编码错误开始,为了不与HY_FAILURE冲突，留空到-100
    HY_ENC_START_ERROR = HY_ENC_ERR_BASE -1,  // 编码模块启动错误
    HY_ENC_STOP_ERROR = HY_ENC_START_ERROR -1			, // 编码模块结束错误
    HY_ENC_EXIT_ERROR = HY_ENC_STOP_ERROR -1			,  // 编码模块退出错误
    HY_ENC_SET_BOLT_VENC_PARAM_ERROR  = HY_ENC_EXIT_ERROR -1		, // 枪机编码参数错误
    HY_ENC_GET_BOLT_VENC_PARAM_ERROR = HY_ENC_SET_BOLT_VENC_PARAM_ERROR -1		, // 获取枪机编码参数出错
    HY_ENC_SET_BALL_VENC_PARAM_ERROR = HY_ENC_GET_BOLT_VENC_PARAM_ERROR -1		, // 球机编码参数错误 
    HY_ENC_GET_BALL_VENC_PARAM_ERROR	 = HY_ENC_SET_BALL_VENC_PARAM_ERROR -1	, // 获取球机编码参数出错
    HY_ENC_SET_BOLT_VENC_CONFIG_ERROR = HY_ENC_GET_BALL_VENC_PARAM_ERROR -1	, // 枪机编码参数正确，配置HDVICP时出错
    HY_ENC_SET_BALL_VENC_CONFIG_ERROR = HY_ENC_SET_BOLT_VENC_CONFIG_ERROR -1	,// 球机编码参数正确，配置HDVICP时出错
    HY_ENC_SET_MDTRACK_ALG_PARAM_ERROR = HY_ENC_SET_BALL_VENC_CONFIG_ERROR -1	, // 算法运行参数错误
    HY_ENC_GET_MDTRACK_ALG_PARAM_ERROR = HY_ENC_SET_MDTRACK_ALG_PARAM_ERROR -1	, // 获取算法运行参数出错
    HY_ENC_SET_MDTRACK_ROI_PARAM_ERROR = HY_ENC_GET_MDTRACK_ALG_PARAM_ERROR -1	, // ROI
    HY_ENC_GET_MDTRACK_ROI_PARAM_ERROR = HY_ENC_SET_MDTRACK_ROI_PARAM_ERROR -1	,
    HY_ENC_SET_MDTRACK_SEMIAUTO_PARAM_ERROR = HY_ENC_GET_MDTRACK_ROI_PARAM_ERROR -1	,
    HY_ENC_GET_MDTRACK_SEMIAUTO_PARAM_ERROR = HY_ENC_SET_MDTRACK_SEMIAUTO_PARAM_ERROR -1	,
    HY_ENC_SET_MDTRACK_TRIPWIRE_PARAM_ERROR = HY_ENC_GET_MDTRACK_SEMIAUTO_PARAM_ERROR -1	,
    HY_ENC_GET_MDTRACK_TRIPWIRE_PARAM_ERROR = HY_ENC_SET_MDTRACK_TRIPWIRE_PARAM_ERROR -1	,
    HY_ENC_CREATE_THREAD_ERR = HY_ENC_GET_MDTRACK_TRIPWIRE_PARAM_ERROR -1        ,  // 创建模块线程出错
    HY_ENC_BUF_RELEASE_ERR  = HY_ENC_CREATE_THREAD_ERR -1,   // 申请的空间释放出错
    
    HY_ENC_ERR_END        =  HY_ENC_BUF_RELEASE_ERR -1        ,

    
    HY_ALARM_ERR_BASE        =  SET_ERR_MASK(HY_ALARM_MODULE_TYPE), //业务报警错误开始


   
    HY_NET_CTL_ERR_BASE   =  SET_ERR_MASK(HY_NET_CTL_MODULE_TYPE), //网络控制消息开始


    HY_NET_STREAM_ERR_BASE = SET_ERR_MASK(HY_NET_STREAM_MODULE_TYPE), //网络流传输消息开始


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
 * 根据错误号获取错误字符串
 *  
 ************************************************************/
const hy_char * hy_get_str_by_err_code(HY_ERR_CODE_TYPE_E err_code);


#ifdef __cplusplus
}
#endif //__cplusplus


#endif  //_HY_ERR_DEFINE_H_
