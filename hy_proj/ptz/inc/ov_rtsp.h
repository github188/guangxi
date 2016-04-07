/**************************************
 *针对rtsp地址的相关操作函数声明
 *date:     2014.6.23
 *changelog:
 *
 *************************************/

#ifndef __OV_RTSP_H__
#define __OV_RTSP_H__

#include "libavformat/avformat.h"

int ov_rtsp_stream_connect(AVFormatContext **fmt_ctx, const char *rtsp_addr);
void ov_rtsp_stream_disconnect(AVFormatContext **fmt_ctx);
void ov_rtsp_record_video(const char *rtsp_addr, const char *filename, int second);
void ov_rtsp_capture_picture(const char *rtsp_addr, const char *filename);

#endif


