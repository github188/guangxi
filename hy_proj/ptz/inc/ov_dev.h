/**************************************
 *onvif协议获取设备信息操作函数声明
 *date:     2014.6.23
 *changelog:
 *
 *************************************/

#ifndef __OV_DEV_H__
#define __OV_DEV_H__

#include "ov_types.h"

/*设备发现，获取设备入口地址*/
int ov_discovery_device(char *path, ov_devices_list *dev_list);
/*显示设备列表*/
void ov_show_device_list(ov_devices_list *dev_list);

#endif

