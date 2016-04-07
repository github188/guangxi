/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        : SOCKET IO �Ļ�������
* Change log		 :  Jimmy2015-7-14
********************* (C) COPYRIGHT 2015 HY *******************/
#ifndef _HY_IO_UTILS_H
#define _HY_IO_UTILS_H
#include "hy_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**********************************
*Name:hy_tcp_connect
*Desc:
*Param:
*Return: -1 ����ʧ��
         >0 ���ؽ�����fd
*Author:Jimmy
*Create Time:2015-7-14
*ChangeLog:
**********************************/
hy_s32 hy_tcp_connect(hy_char *ip, hy_u16 port);


/**********************************
*Name:hy_write
*Desc: �ú��������̰߳�ȫ��
*Param:
*Return:
*Author:Jimmy
*Create Time:2015-7-14
*ChangeLog:
**********************************/
hy_s32 hy_write(hy_s32 fd, void *buf, size_t size);

/**********************************
*Name:hy_read
*Desc:ѭ����ȡ�̶���С,����select�Ͱ���С�Ŀ��ǣ���Ϊÿ�ζ���ʱ���ܹ���ȡ��
      ��Ҫ�Ĵ�С�����������Ҫ�Ĵ�С������Ϊ������
*Param:
*Return:
*Author:Jimmy
*Create Time:2015-7-14
*ChangeLog:
**********************************/
hy_s32 hy_read(hy_s32 fd, void *buf, size_t size);

void hy_close_fd(hy_s32 fd);

#ifdef __cplusplus
}
#endif

#endif
