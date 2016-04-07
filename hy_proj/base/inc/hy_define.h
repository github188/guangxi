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
    HY_ENC_MODULE_TYPE              = 0,   //编码模块
    HY_MV_MODULE_TYPE                  ,   //MV模块
    HY_CAPTURE_MODULE_TYPE             ,  //Capture 模块
    HY_ALARM_MODULE_TYPE               ,   //业务报警模块
    HY_FILE_STORE_MODULE_TYPE          ,   //文件存储模块
    HY_FILE_TRANSFER_MODULE_TYPE       ,   //文件上传模块
    HY_STREAM_CTL_MODULE_TYPE          ,   //stream ctl module
//    HY_RS422_MODULE_TYPE               ,   //串口上报模块
    HY_SYS_MONITOR_MODULE_TYPE         ,   //系统监控模块                                       //
    HY_NET_CTL_MODULE_TYPE             ,   //网络控制模块
    HY_NET_STREAM_MODULE_TYPE          ,   //网络流处理模块
   
    
    /*以下为无生命体的模块，这意味着一些资源将不会被创建，如消息队列等*/
    HY_USER_MGT_MODULE_TYPE            ,   //用户管理模块
    HY_SYS_CFG_MODULE_TYPE             ,   //系统参数
    HY_DB_MODULE                       ,   //数据库模块，如果数据库不能被读取或写入
    HY_LOG_MODULE_TYPE                 ,   //系统日志模块，用户查询或设置

    HY_MODULE_BUTTON                   ,   //模块结束，主要用于循环操作及判断


}HY_MODULE_DEF_E;






#ifdef __cplusplus
}
#endif //__cplusplus


#endif  //_HY_DEFINE_H_
