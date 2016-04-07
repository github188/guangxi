/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        : SOCKET IO 的基础操作
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
*Return: -1 连接失败
         >0 返回建立的fd
*Author:Jimmy
*Create Time:2015-7-14
*ChangeLog:
**********************************/
hy_s32 hy_tcp_connect(hy_char *ip, hy_u16 port);


/**********************************
*Name:hy_write
*Desc: 该函数不是线程安全的
*Param:
*Return:
*Author:Jimmy
*Create Time:2015-7-14
*ChangeLog:
**********************************/
hy_s32 hy_write(hy_s32 fd, void *buf, size_t size);

/**********************************
*Name:hy_read
*Desc:循环读取固定大小,处于select和包大小的考虑，认为每次读的时候，能够读取到
      需要的大小。如果不是需要的大小，则认为有问题
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
