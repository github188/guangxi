/**************************************
 *onvifЭ���ȡ�豸��Ϣ������������
 *date:     2014.6.23
 *changelog:
 *
 *************************************/

#ifndef __OV_DEV_H__
#define __OV_DEV_H__

#include "ov_types.h"

/*�豸���֣���ȡ�豸��ڵ�ַ*/
int ov_discovery_device(char *path, ov_devices_list *dev_list);
/*��ʾ�豸�б�*/
void ov_show_device_list(ov_devices_list *dev_list);

#endif

