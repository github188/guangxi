/******************** (C) COPYRIGHT 2014 HY ********************
* File Name          : hy_base_config.h
* Author             :
* Revision           :
* Date              :
* Description        : base configure define header file
* Change log		: 1.Initial version :2014-03-27 by zcx
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

#ifndef _HY_BASE_CONFIG_H_
#define _HY_BASE_CONFIG_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/***************************************
 * 分配释放函数重定义
 **************************************/
//#define HY_MALLOC_ALIGN

#define ALIGN_SIZE (32)

#ifndef hy_malloc
    #define  hy_malloc(a) malloc(a)
#endif
//;printf("------\n")
#ifndef hy_free
    #define  hy_free free
#endif

#ifndef hy_calloc
    #define  hy_calloc calloc
#endif

#ifndef hy_sleep
    #define hy_sleep(x)  sleep((x))
#endif

#ifndef hy_msleep
    #define hy_msleep(x)  usleep((x)*1000)
#endif

#define DEFAULT_VIDEO_WIDTH  (1280)
#define DEFAULT_VIDEO_HEIGHT (720)

#define MAX_RTSP_URL_LEN (256)



/***************************************
 * 最大线程数
 **************************************/

#ifndef HY_MAX_IPV4_LEN
    #define HY_MAX_IPV4_LEN (32)
#endif

#ifndef HY_MAX_IPV6_LEN
    #define HY_MAX_IPV6_LEN (128)
#endif


#ifndef HY_MAX_THREADS_NUMBER
    #define HY_MAX_THREADS_NUMBER (6)
#endif

#ifndef DEFAULT_FRAMES_NUMBER
    #define DEFAULT_FRAMES_NUMBER  (120)
#endif

#ifndef DEFAULT_MAX_ENC_CHN_NUM
    #define DEFAULT_MAX_ENC_CHN_NUM  (4)
#endif

#define HY_BUFF_16_NUMBER  (32)

#define HY_BUFF_32_NUMBER  (32)

#define HY_BUFF_64_NUMBER  (32)

#define HY_BUFF_128_NUMBER  (16)

#define HY_BUFF_256_NUMBER  (16)

#define HY_BUFF_512_NUMBER  (8)

#define HY_SIZE_16  (16)

#define HY_SIZE_32  (32)

#define HY_SIZE_64  (64)

#define HY_SIZE_128  (128)

#define HY_SIZE_256  (256)

#define HY_SIZE_512  (1024)

#define CHECK_BIT_VALUE(n,x)  (((n>>(x))&0x01) ? (1) :(0))

#define CLEAR_BIT_VALUE(n,x) ((n)&(~(1L <<(x))))

#define SET_BIT_VALUE(n,x) ((n) | (1L << (x)))

#define hy_min_rule(x,y) ({ \
    typeof(x) _x = (x); \
    typeof(y) _y = (y); \
    (void) (&_x == &_y);   \
    _x < _y ? _x : _y; })

#define hy_max_rule(x,y) ({ \
    typeof(x) _x = (x); \
    typeof(y) _y = (y); \
    (void) (&_x == &_y);   \
    _x > _y ? _x : _y; })

#define hy_min(a,b) ( ((a)<(b)) ? (a) : (b) )

#define hy_max(a,b) ( ((a)>(b)) ? (a) : (b) )

#define hy_timeradd(tvp, hy_sec,hy_usec, vvp)					\
	do { \
        (vvp)->tv_sec = (tvp)->tv_sec +hy_sec;		\
        (vvp)->tv_usec = (tvp)->tv_usec + hy_usec;       \
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)

#define hy_timeraddn(tvp, hy_sec,hy_nsec, vvp)					\
	do { \
        (vvp)->tv_sec = (tvp)->tv_sec +hy_sec;		\
        (vvp)->tv_usec = (tvp)->tv_usec + hy_usec;       \
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)


typedef enum{

    HY_STATIC_ALLOC_TYPE  = 0,
    HY_DYNAMIC_ALLOC_TYPE = 1,
}HY_ALLOC_TYPE_E;

typedef enum{
    HY_NOTIFY_ONCE = 0,
    HY_NOTIFY_FOREVER
}HY_NOTIFY_TYPE_E;

typedef enum{
    HY_EV_BLOCK = 0 ,
    HY_EV_NONEBLOCK
}HY_BLOCK_TYPE_E;


typedef enum {
    H264_NALU_PSLICE  = 1,
    H264_NALU_ISLICE  = 5,
    H264_NALU_IDRSLICE = 6,
    H264_NALU_SPS        = 7,
    H264_NALU_PPS        = 8,
    H264_NALU_BUTT
}HY_H264_NALU_TYPE_E;

typedef enum {
    H264_ENC_CHN_1_MAIN_TYPE = 0,
    H264_ENC_CHN_2_MAIN_TYPE ,
    H264_ENC_CHN_3_MAIN_TYPE ,
    H264_ENC_CHN_4_MAIN_TYPE ,
    H264_IPC_ENC_TYPE        ,
    AUDIO_MIC_CHN_TYPE       ,
    AUDIO_SPEAK_CHN_TYPE     ,
    CHN_ENC_TYPE_BUTTON
}HY_CHN_ENC_CHAN_TYPE_E;


typedef enum{
    ENC_DATA_TYPE_IS_H264    ,
    ENC_DATA_TYPE_IS_MJPEG   ,
    ENC_DATA_TYPE_IS_AUDIO   ,

    ENC_DATA_TYPE_BUTTON
}HY_ENC_DATA_TYPE_E;


typedef enum{
    NET_TCP_TYPE = 0,
    NET_UDP_TYPE,
    NET_MULTICAST_TYPE,
    NET_NETLINK_TYPE,
    NET_BROADCAST_TYPE,
    FILE_STORAGE_TYPE ,
    NET_TRANS_TYPE_BUTTON
}HY_NET_TRANS_TYPE_E;


typedef enum{
    HY_STREAM_IS_VIDEO = 1,
    HY_STREAM_IS_AUDIO = 2,
    HY_STREAM_BOTH_AV  = 3,

    HY_STREAM_IS_BUTTON

}HY_STREAM_TYPE_E;


typedef enum{
    NET_SIP_PRTC_TYPE = 0,
    NET_RTP_PRTC_TYPE,

    NET_PRTC_TYPE_BUTTON
}HY_NET_PROTOCOL_TYPE_E;

typedef enum{
    NODE_ONLY_DEL_METHOD,       //only node from queue
    NODE_DEL_AND_RSL_METHOD    //del and release node resource from queue
}HY_NODE_IN_QUE_DEL_E;

typedef enum{
    THREAD_STATUS_STOP,
    THREAD_STATUS_PAUSE,
    THREAD_STATUS_RUN,
    THREAD_STATUS_EXIT,
}HY_THREAD_STATUS_E;

typedef enum{
    IPC_IS_BOLT = 1,
    IPC_IS_BALL = 2,
}HY_IPC_TYPE_E;



#ifdef __cplusplus
}
#endif //__cplusplus


#endif  //_HY_BASE_CONFIG_H_
