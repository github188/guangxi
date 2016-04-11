#include <xdc/std.h>

#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/CERuntime.h>

#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/Time.h>
#include <ti/sdo/dmai/Cpu.h>
#include <ti/sdo/dmai/BufTab.h>
#include <ti/sdo/dmai/Loader.h>
#include <ti/sdo/dmai/Display.h>
#include <ti/sdo/dmai/ColorSpace.h>
#include <ti/sdo/dmai/VideoStd.h>
#include <ti/sdo/dmai/ce/Vdec2.h>
#include <ti/sdo/dmai/BufferGfx.h>

//Jimmy
#include <ti/sdo/ce/utils/trace/TraceUtil.h>

//jinming
#include <ti/sdo/ce/osal/Memory.h>
#include <ti/sdo/dmai/ce/Venc.h>
#include <ti/sdo/ce/video/videnc.h>

#include "global_config.h"
#include "hy_base_config.h"
#include "hy_debug.h"
#include "hy_time.h"

#include "hy_thread_utils.h"
#include "hy_stream_ctrl.h"
#include "hy_deque.h"
#include "hy_protocl.h"
#include "decode.h"
#include "hy_detect_flow_ctrl.h"
#include "hy_zlog.h"

#include "hy_picture_convert.h"

//static Args glb_args={
/*ToDo:*/
//    10000,
//    1,
//    FALSE,
//    TRUE,
//    0,
//    "h264dec",
//    "2.h264",
//    "2.yuv",
//    "decode"
//};

//#define VENC_RECORD_YUV
//#define ALARM_TEST
#define CONVERT
#define OFFSETINFO 1280*600
/* Align buffers to this cache line size (in bytes)*/
#define BUFSIZEALIGN            128
/* vbuf size that has been selected based on size/performance tradeoff */
#define VBUFSIZE                20480
#define MIN_FRAME_SIZE          0
static Char vbuffer[VBUFSIZE];
static Bool cache = TRUE;

static hy_thread_handler_t *hy_decode_thread;
static String engine_name = "decode";
static String codec_name = "h264dec";
static String encoder_Name = "videnc_copy";

/******************************************************************************
 * resizeBufTab
******************************************************************************/
static Int resizeBufTab(Vdec2_Handle hVd2, Int displayBufs)
{
    BufTab_Handle hBufTab = Vdec2_getBufTab(hVd2);
    Int numBufs, numCodecBuffers, numExpBufs;
    Buffer_Handle hBuf;
    Int32 frameSize;

    /* How many buffers can the codec keep at one time? */
    numCodecBuffers = Vdec2_getMinOutBufs(hVd2);
    if (numCodecBuffers < 0) {
        printf("Failed to get buffer requirements\n");
        return -1;
    }

    /*
     * Total number of frames needed are the number of buffers the codec
     * can keep at any time, plus the number of frames in the display pipe.
     */
    numBufs = numCodecBuffers + displayBufs;

    /* Get the size of output buffers needed from codec */
    frameSize = Vdec2_getOutBufSize(hVd2);

    /*
     * Get the first buffer of the BufTab to determine buffer characteristics.
     * All buffers in a BufTab share the same characteristics.
     */
    hBuf = BufTab_getBuf(hBufTab, 0);

    /* Do we need to resize the BufTab? */
    if (numBufs > BufTab_getNumBufs(hBufTab) ||
        frameSize < Buffer_getSize(hBuf)) {

        /* Should we break the current buffers in to many smaller buffers? */
        if (frameSize < Buffer_getSize(hBuf)) {

            /* First undo any previous chunking done */
            BufTab_collapse(Vdec2_getBufTab(hVd2));

            /*
             * Chunk the larger buffers of the BufTab in to smaller buffers
             * to accomodate the codec requirements.
             */
            numExpBufs = BufTab_chunk(hBufTab, numBufs, frameSize);

            if (numExpBufs < 0) {
                printf("Failed to chunk %d bufs size %d to %d bufsize %d\n",
                    BufTab_getNumBufs(hBufTab), (Int)Buffer_getSize(hBuf),
                    numBufs, (Int)frameSize);
                return -1;
            }

            /*
             * Did the current BufTab fit the chunked buffers,
             * or do we need to expand the BufTab (numExpBufs > 0)?
             */
            if (BufTab_expand(hBufTab, numExpBufs) < 0) {
                printf("Failed to expand BufTab with %d buffers\n",
                       numExpBufs);
                return -1;
            }
        }
        else {
            /* Just expand the BufTab with more buffers */
            if (BufTab_expand(hBufTab, numBufs - BufTab_getNumBufs(hBufTab)) < 0) {
                printf("Failed to expand BufTab with %d buffers\n",
                       numCodecBuffers);
                return -1;
            }
        }
    }

    return numBufs;
}


void *hy_decode_thread_cb(void *s)
{
    hy_zlog_info("hy_decode_thread_cb.\n");
    hy_thread_handler_t *thread_handler = (hy_thread_handler_t *)s;
    thread_handler->b_thread_running = HY_TRUE;

    //stream
    while (1)
    {
        if (hy_simple_stream_init() == -1)
        {
            hy_sleep(10);
            continue;
        }
        else
        {
            break;
        }
    }

    //算法会用到的各个变量
    VIDDEC2_Params          params       = Vdec2_Params_DEFAULT;
    VIDDEC2_DynamicParams   dynParams    = Vdec2_DynamicParams_DEFAULT;
    Loader_Attrs            lAttrs       = Loader_Attrs_DEFAULT;
    BufferGfx_Attrs         gfxAttrs     = BufferGfx_Attrs_DEFAULT;
    Time_Attrs              tAttrs       = Time_Attrs_DEFAULT;
    Vdec2_Handle            hVd2         = NULL;
	//jinming
	Venc_Handle				HVidenc = NULL;
	VIDENC_Params           Vidparams       = Venc_Params_DEFAULT;
    VIDENC_DynamicParams    ViddynParams    = Venc_DynamicParams_DEFAULT;
	Int32 FrmNum=0;

	VIDENC_InArgs               encInArgs;
    VIDENC_OutArgs              encOutArgs;
    VIDENC_DynamicParams        encDynParams;
    VIDENC_Status               encStatus;
	Int32                       status;

	XDM_BufDesc                 inBufDesc;
    XDM_BufDesc                 encodedBufDesc;

	Memory_AllocParams allocParams;

	XDAS_Int8 *outdBuf,*indBuf;

    Buffer_Attrs            bAttrs       = Buffer_Attrs_DEFAULT;
    Engine_Handle           hEngine      = NULL;
    BufTab_Handle           hBufTab      = NULL;
    Buffer_Handle           hDstBuf      = NULL;
    Bool                    flushed      = FALSE;
    Int                     numFrame     = 1;
    Int                     numFrameDisp = 1;
    Buffer_Handle           hInBuf = NULL;
    IVIDENC_DynamicParams   VIDdynParams;
    IVIDENC_Status          IVDstatus;
	Buffer_Handle           hDstBuf123     = NULL;
    Buffer_Handle           hFreeBuf ,hOutBuf = NULL;

    /*CPU and ColorSpace*/
    ColorSpace_Type         colorSpace;
    Cpu_Device              device;
    Int                     numBufs;
    UInt32                  time;
    BufferGfx_Dimensions    dim;
    Bool                    vc1Offset   = FALSE;

    //操作相关的逻辑控制
    Bool                    IFrame = FALSE;
    Bool                    resize_flag = TRUE;
    hy_u32                  FrameNum = 0;
    hy_u32                  AlarmIntervalNum = 100;
//    Int                     cur_channel = 0;
//    Int                     change_to_channel = 0;
    Int                     frame_before_decode = 0;


    /*用于算法的计时*/
    long codec_start_time;
    long codec_end_time;
    /*用于记录流数据*/
    hy_s8 *frame_data = NULL;
    hy_u32 frame_size = 0;

    /* Initialize the codec engine run time */
    CERuntime_init();
    /* Initialize DMAI */
    Dmai_init();

    /* Determine which device the application is running on */
    if (Cpu_getDevice(NULL, &device) < 0)
    {
        hy_zlog_info("Failed to determine target board.\n");
        goto DECODE_CLEANUP;
    }

    /* Initialize platform-specific settings */
    if ((device == Cpu_Device_DM6467) ||
        (device == Cpu_Device_DM365))
    {
        colorSpace = ColorSpace_YUV420PSEMI;
        numBufs    = 5;
    }
    else
    {
        colorSpace = ColorSpace_UYVY;
        numBufs    = 2;
    }

    /* Set up codec parameters depending on device */
    if ((device == Cpu_Device_DM6467) ||
        (device == Cpu_Device_DM365))
    {
        params.forceChromaFormat    = XDM_YUV_420SP;
        params.maxWidth             = 1280;//VideoStd_1080I_WIDTH;
        params.maxHeight            = 720;//VideoStd_1080I_HEIGHT + 8;
    }
    else
    {
        params.forceChromaFormat    = XDM_YUV_422ILE;
        params.maxWidth             = VideoStd_D1_WIDTH;
        params.maxHeight            = VideoStd_D1_PAL_HEIGHT;
    }
    if(device == Cpu_Device_DM6467)
    {
        if (!strcmp(codec_name, "mpeg4dec"))
        {
            params.maxWidth  = VideoStd_576P_WIDTH;
            params.maxHeight = VideoStd_576P_HEIGHT;
        }
    }
    if(device == Cpu_Device_DM365) {
        if (!strcmp(codec_name, "vc1dec"))
        {
            vc1Offset = TRUE;
        }
    }

    //Jimmy:trace
    TraceUtil_start(engine_name);
    /* Open the codec engine */
    hEngine = Engine_open(engine_name, NULL, NULL);
    if (hEngine == NULL)
    {
        hy_zlog_info("Failed to open codec engine %s\n", engine_name);
        goto DECODE_CLEANUP;
    }
    /* Create the XDM 1.2 based video decoder */
    hVd2 = Vdec2_create(hEngine, codec_name, &params, &dynParams);
    if (hVd2 == NULL)
    {
        hy_zlog_info("Failed to create video decoder: %s\n", codec_name);
        goto DECODE_CLEANUP;
    }

	/*Create encoder*/
	Vidparams.size =sizeof(VIDENC_Params);
	ViddynParams.size=sizeof(VIDENC_DynamicParams);
	HVidenc = Venc_create(hEngine, encoder_Name, &Vidparams, &ViddynParams);
    if (HVidenc == NULL)
    {
        hy_zlog_info("Failed to create videnc_copy: %s\n", encoder_Name);
        goto DECODE_CLEANUP;
    }



    /*配置buffer属性*/
    /* Align buffers to cache line boundary */
    gfxAttrs.bAttrs.memParams.align = lAttrs.mParams.align = BUFSIZEALIGN;
    /* Use cached buffers if requested */
    if (cache)
    {
        gfxAttrs.bAttrs.memParams.flags = lAttrs.mParams.flags = Memory_CACHED;
    }
    gfxAttrs.colorSpace     = colorSpace;
    gfxAttrs.dim.width      = params.maxWidth;
    gfxAttrs.dim.height     = params.maxHeight;
    gfxAttrs.dim.lineLength = BufferGfx_calcLineLength(params.maxWidth,
                                                       colorSpace);

    /* Create a table of output buffers of the size requested by the decode */
    hBufTab = BufTab_create(numBufs, Dmai_roundUp(Vdec2_getOutBufSize(hVd2), BUFSIZEALIGN),
        BufferGfx_getBufferAttrs(&gfxAttrs));
    if (hBufTab == NULL)
    {
        hy_zlog_info("Failed to allocate contiguous buffers.\n");
        goto DECODE_CLEANUP;
    }
    /* Set output buffer table */
    Vdec2_setBufTab(hVd2, hBufTab);


    /* Ask the codec how much input data it needs */
    lAttrs.readSize = Vdec2_getInBufSize(hVd2);
    /* Make the total ring buffer larger */
    lAttrs.readBufSize = Dmai_roundUp(lAttrs.readSize * 2, BUFSIZEALIGN);
    /* Increase the stdio buffer size for loader for better RTDX performance */
    lAttrs.vBufSize = VBUFSIZE;

    bAttrs.memParams.align = BUFSIZEALIGN;
    hInBuf = Buffer_create(Dmai_roundUp(lAttrs.readSize , BUFSIZEALIGN), &bAttrs);
    if (hInBuf == NULL)
    {
        hy_zlog_info("Failed to create hInBuf buffer。\n");
        goto DECODE_CLEANUP;
    }

    /*hDstBuf123 used for encode*/
	lAttrs.readSize = Dmai_roundUp(1280*720*4, BUFSIZEALIGN);
    hDstBuf123= Buffer_create(Dmai_roundUp(lAttrs.readSize , BUFSIZEALIGN), &bAttrs);
    if (hDstBuf123 == NULL)
    {
        hy_zlog_info("Failed to create hDstBuf123 buffer\n");
        goto DECODE_CLEANUP;
    }

    /*  Jimmy on 2015-09-25在检测之前,需要初始化的东西*/
    hy_sig_pre_detect();
    hy_sleep(2);
    while(thread_handler->b_thread_running)
    {
//#ifdef ALARM_TEST
//        while (1)
//        {
//            hy_sleep(30);
//            hy_simple_debug();
//            hy_mesg_t *mesg = (hy_mesg_t *)hy_malloc(sizeof(hy_mesg_t));
//            ALARM_DATA *p_alarm_data = (ALARM_DATA *)hy_malloc(sizeof(ALARM_DATA));
//            p_alarm_data->ipc = g_hy_detect_info_t.cur_channel;
//            hy_zlog_info("find an alarm cur_channel %d %d\n", g_hy_detect_info_t.cur_channel, p_alarm_data->ipc);
//            p_alarm_data->stream = 1;
//            struct tm * p_tm = hy_get_cur_tm();
//            p_alarm_data->year = p_tm->tm_year + 1900;
//            p_alarm_data->month = p_tm->tm_mon + 1;
//            p_alarm_data->day = p_tm->tm_mday;
//            p_alarm_data->hour = p_tm->tm_hour;
//            p_alarm_data->minute = p_tm->tm_min;
//            p_alarm_data->second = p_tm->tm_sec;
//            p_alarm_data->type = 12;
//            p_alarm_data->level = 2;

//            //hy_free(p_tm);//这边不需要free,free会出错
//            hy_init_mesg(mesg, HY_ALARM_MESG, p_alarm_data);
//            if (hy_put_mesg(data_mesg_deque, mesg))
//            {
//                hy_zlog_info("put mesg error.\n");
//            }
//        }
//#endif
        /*判断是否要开启检测*/
        if (!g_hy_detect_info_t.starting)
        {
            hy_zlog_info("mv detect is not open.\n");
            FrameNum = 0;
            IFrame = FALSE;
            hy_sleep(1);
            continue;
        }

        //change_to_channel may be modify by another thread
        if (g_hy_detect_info_t.cur_channel != g_hy_detect_info_t.change_to_channel)
        {
            g_hy_detect_info_t.cur_channel = g_hy_detect_info_t.change_to_channel;
            FrameNum = 0;
            IFrame = FALSE;
            hy_zlog_info("change to channel %d \n", g_hy_detect_info_t.change_to_channel);
        }


        if (frame_data != NULL)
        {
            hy_free(frame_data);
            frame_data = NULL;
            frame_size = 0;
        }

        int get_frame = hy_simple_stream_get_original_frame(g_hy_detect_info_t.cur_channel, &frame_data, &frame_size);
        if (get_frame < 0)
        {
            hy_zlog_info("get frame error");
            continue;
        }
        if (frame_size == 0)
        {
            hy_zlog_info("frame size is %d.", frame_size);
            continue;
        }

        FrameNum += 1;
        //hy_zlog_info("Frame Num = %d \n", FrameNum);

        /*切换通道后要过一段时间才检验*/
        if (FrameNum < 4)
        {
            hy_zlog_info("interval not alowed.\n");
            continue;
        }

        /*after find i frame then can continue next step*/
        if ((frame_data[4] & 0x0f) == 7)
        {
            IFrame = TRUE;
            hy_zlog_info("find I frame.\n");
        }
        if (!IFrame)
        {
            continue;
        }


        /* Get a free buffer from the BufTab */
        hDstBuf = BufTab_getFreeBuf(hBufTab);
        if (hDstBuf == NULL)
        {
            hy_zlog_info("Failed to get a free contiguous buffer from BufTab\n");
            BufTab_print(hBufTab);
            goto DECODE_CLEANUP;
        }

        /*先暂时让他一直缓冲*/
        if (cache)
        {
            /*
              *  To meet xDAIS DMA Rule 7, when input buffers are cached, we
              *  must writeback the cache into physical memory.  Also, per DMA
              *  Rule 7, we must invalidate the output buffer from
              *  cache before providing it to any xDAIS algorithm.
              */
             Memory_cacheWbInv(Buffer_getUserPtr(hInBuf), Buffer_getSize(hInBuf));
             /* Per DMA Rule 7, our output buffer cache lines must be cleaned */
             Memory_cacheInv(Buffer_getUserPtr(hDstBuf), Buffer_getSize(hDstBuf));
        }

        /* Make sure the whole buffer is used for output */
        BufferGfx_resetDimensions(hDstBuf);
        memcpy(Buffer_getUserPtr(hInBuf), (Int8*)frame_data, frame_size);
        Buffer_setNumBytesUsed(hInBuf, frame_size);

        codec_start_time = hy_get_cur_time_ms();
        //hy_zlog_info("cur_channel %d start decoding...\n", g_hy_detect_info_t.cur_channel);
        /* Decode the video buffer */
        int ret = Vdec2_process(hVd2, hInBuf, hDstBuf);
        if (ret < 0)
        {
            hy_zlog_info("Failed to decode video buffer\n");
            goto DECODE_CLEANUP;
        }

#ifdef CONVERT
        int bpp = ColorSpace_getBpp(BufferGfx_getColorSpace(hDstBuf));
        BufferGfx_Dimensions    dim;
        BufferGfx_getDimensions(hDstBuf, &dim);
        int offset = (dim.y * dim.lineLength) + (dim.x * (bpp >> 3));
        Int *yuv_data = Buffer_getUserPtr(hDstBuf) + offset;
#endif

        //TODO:set alarm rect

        Int8 *yPtr = Buffer_getUserPtr(hDstBuf123);
        Int32 *yyPtr =(Int32 *)(yPtr+OFFSETINFO);
        memset(yyPtr, 0, 1280);//初始化区域参数
    	Int32 *yyPtr1 =(Int32 *)(yPtr+OFFSETINFO+1280);

    	if(g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1].rectcount)
    	{
    	    /*为了测试，先这样做，换成自己的平台的时候要去掉这一部分*/
            yyPtr1[0]= 5;
    		yyPtr1[1]= 1000;
    		yyPtr1[2]= 1000;

    		int i;
    		yyPtr[1]= 1;//Jimmy:标志有没有设置，区域
    	    yyPtr[2]= g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1].rectcount;
            for(i=0; i<yyPtr[2]; i++)
    		{
//    			printf("rect%d left=%d ,right=%d ,top=%d ,bottom=%d \n", i+1,\
//    			g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1].rect[i].left,
//    			g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1].rect[i].right,
//    			g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1].rect[i].top,
//    			g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1].rect[i].bottom);
    	    	yyPtr[4+i*4] = g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1].rect[i].left + 24;
    			yyPtr[4+i*4+1] = g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1].rect[i].right+ 24;
    			yyPtr[4+i*4+2] = g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1].rect[i].top;
    			yyPtr[4+i*4+3] = g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1].rect[i].bottom;
    		}
    	}

        //hy_zlog_info("cur_channel %d start encoding...\n", g_hy_detect_info_t.cur_channel);
        status = Venc_process(HVidenc, hDstBuf, hDstBuf123);
   		if (status < 0)
   		{
            hy_zlog_info("************************Failed to video anlytics************************************* \n");
            goto DECODE_CLEANUP;
        }

        codec_end_time = hy_get_cur_time_ms();
        //hy_zlog_info("process codec cast %d ms.\n", codec_end_time - codec_start_time);

        //TODO:check alarm or not
        if (AlarmIntervalNum != 0)
        {
            AlarmIntervalNum--;
        }
        if (yyPtr[0])   //有告警
        {
            hy_zlog_info("find an alarm, but interval is not allowed.\n");
            if (AlarmIntervalNum == 0)
            {
                //alarm
                AlarmIntervalNum = 100;

                //hy_simple_debug();
                hy_mesg_t *mesg = (hy_mesg_t *)hy_malloc(sizeof(hy_mesg_t));
                ALARM_DATA *p_alarm_data = (ALARM_DATA *)hy_malloc(sizeof(ALARM_DATA));
                p_alarm_data->ipc = g_hy_detect_info_t.cur_channel;
                //hy_zlog_info("find an alarm cur_channel %d %d\n", g_hy_detect_info_t.cur_channel, p_alarm_data->ipc);
                p_alarm_data->stream = 1;
                struct tm * p_tm = hy_get_cur_tm();
                p_alarm_data->year = p_tm->tm_year + 1900;
                p_alarm_data->month = p_tm->tm_mon + 1;
                p_alarm_data->day = p_tm->tm_mday;
                p_alarm_data->hour = p_tm->tm_hour;
                p_alarm_data->minute = p_tm->tm_min;
                p_alarm_data->second = p_tm->tm_sec;
                p_alarm_data->type = 12;
                p_alarm_data->level = 2;

                hy_s8 file_name[128];//在这里用原始的YUV压缩成jpg
                sprintf(file_name, "/root/pic_record/%d-%d-%d%02d%02d-%02d%02d%02d.jpg", g_hy_nvr_info.hy_nvr_general_info.device_id,g_hy_detect_info_t.cur_channel,
                        p_alarm_data->year, p_alarm_data->month, p_alarm_data->day, p_alarm_data->hour, p_alarm_data->minute, p_alarm_data->second);
                unsigned char rgb24_data[1280*720*3] = {0};
                yuv420P2rgb24(yuv_data, rgb24_data, 1280, 720);

                int i = 0;
                for (i=0; i<yyPtr[2]; i++)
                {
                    hy_alarm_rect_t alarm_rect;
                    alarm_rect.left = yyPtr[3 + i*4];
                    alarm_rect.top = yyPtr[3 + i*4 + 1];
                    alarm_rect.right = yyPtr[3 + i*4 + 2];
                    alarm_rect.bottom = yyPtr[3 + i*4 + 3];
                    hy_zlog_info("alarm left %d top %d right %d bottom %d\n", alarm_rect.left, alarm_rect.top, alarm_rect.right, alarm_rect.bottom);
                    print_one_rect_on_rgb(rgb24_data, 1280, 720, 5, &alarm_rect);
                }

                save_rgb24_to_jpg(rgb24_data, file_name, 1280, 720);
                //hy_free(p_tm);//这边不需要free,free会出错
                hy_init_mesg(mesg, HY_ALARM_MESG, p_alarm_data);
                if (hy_put_mesg(data_mesg_deque, mesg))
                {
                    hy_zlog_info("put mesg error.\n");
                }
            }
        }

        //used for test
#ifdef ALARM_TEST
        if (FrameNum % 500 == 0)
        {

#ifdef VENC_RECORD_YUV
    //Jimmy
    char yuv_file_name[64];
    sprintf(yuv_file_name, "/root/pre%d.yuv", FrameNum);
    FILE * pre_yuv_file = fopen(yuv_file_name,"wb");
    if ( pre_yuv_file == NULL ) {
        printf("\n ************************input open file err ************************************.\n");
    }

    //Jimmy:自己给hDstBuf赋值
    int bpp = ColorSpace_getBpp(BufferGfx_getColorSpace(hDstBuf));

    BufferGfx_Dimensions    dim;
    BufferGfx_getDimensions(hDstBuf, &dim);
    int offset = (dim.y * dim.lineLength) + (dim.x * (bpp >> 3));

    Int *outPtr = Buffer_getUserPtr(hDstBuf) + offset;
    fwrite(outPtr, 1328*720*3/2, 1, pre_yuv_file);
#endif
            //hy_simple_debug();
            hy_mesg_t *mesg = (hy_mesg_t *)hy_malloc(sizeof(hy_mesg_t));
            ALARM_DATA *p_alarm_data = (ALARM_DATA *)hy_malloc(sizeof(ALARM_DATA));
            p_alarm_data->ipc = g_hy_detect_info_t.cur_channel;
            hy_zlog_info("find an alarm cur_channel %d %d\n", g_hy_detect_info_t.cur_channel, p_alarm_data->ipc);
            p_alarm_data->stream = 1;
            struct tm * p_tm = hy_get_cur_tm();
            p_alarm_data->year = p_tm->tm_year + 1900;
            p_alarm_data->month = p_tm->tm_mon + 1;
            p_alarm_data->day = p_tm->tm_mday;
            p_alarm_data->hour = p_tm->tm_hour;
            p_alarm_data->minute = p_tm->tm_min;
            p_alarm_data->second = p_tm->tm_sec;
            p_alarm_data->type = 12;
            p_alarm_data->level = 2;

            hy_s8 file_name[128];//在这里用原始的YUV压缩成jpg
            sprintf(file_name, "/root/pic_record/%d-%d-%d%02d%02d-%02d%02d%02d.jpg", g_hy_nvr_info.hy_nvr_general_info.device_id,g_hy_detect_info_t.cur_channel,
                     p_alarm_data->year, p_alarm_data->month, p_alarm_data->day, p_alarm_data->hour, p_alarm_data->minute, p_alarm_data->second);
            unsigned char rgb24_data[1280*720*3] = {0};
            yuv420P2rgb24(yuv_data, rgb24_data, 1280, 720);
            hy_alarm_rect_t alarm_rect = {100,100,200,200};
            print_one_rect_on_rgb(rgb24_data, 1280, 720, 5, &alarm_rect);
            save_rgb24_to_jpg(rgb24_data, file_name, 1280, 720);
            //hy_free(p_tm);//这边不需要free,free会出错
            hy_init_mesg(mesg, HY_ALARM_MESG, p_alarm_data);
            if (hy_put_mesg(data_mesg_deque, mesg))
            {
                hy_zlog_info("put mesg error.\n");
            }
        }
#endif


        //必须要进行这一步的操作
        if (resize_flag)
        {
            /* Resize the BufTab after the first frame has been processed */
            numBufs = resizeBufTab(hVd2, 0);
            if (numBufs < 0)
            {
                goto DECODE_CLEANUP;
            }

            resize_flag = FALSE;
        }

        /* Query the codec for display buffers and write them to a file */
        hOutBuf = Vdec2_getDisplayBuf(hVd2);
        while (hOutBuf)
        {
            if (cache)
            {
                /* Writeback the outBuf. */
                Memory_cacheWb(Buffer_getUserPtr(hOutBuf),
                Buffer_getSize(hOutBuf));
            }
            /* Get another buffer for display from the codec */
            hOutBuf = Vdec2_getDisplayBuf(hVd2);
        }

        /* Get a buffer to free from the codec */
        hFreeBuf = Vdec2_getFreeBuf(hVd2);
        while (hFreeBuf)
        {
        /* The codec is no longer using the buffer */
            Buffer_freeUseMask(hFreeBuf, 0x1);
            hFreeBuf = Vdec2_getFreeBuf(hVd2);
        }

        //Buffer_setUseMask(hBufTab, 0);

    }

DECODE_CLEANUP:
    /* Clean up the application */
    hy_zlog_info("decode cleanup.\n");
    if (hInBuf)
    {
        Buffer_delete(hInBuf);
    }

    if (hDstBuf123)
    {
        Buffer_delete(hDstBuf123);
    }

    if (hBufTab)
    {
        BufTab_delete(hBufTab);
    }

    if (hEngine)
    {
        Engine_close(hEngine);
    }

    if (hVd2)
    {
        Vdec2_delete(hVd2);
    }

    if (HVidenc)
    {
        Vdec2_delete(HVidenc);
    }

    TraceUtil_stop();

    pthread_exit("thread normal exit");
}

void *hy_start_decode_thread()
{
    hy_decode_thread = (hy_thread_handler_t *)hy_malloc(sizeof(hy_thread_handler_t));
    hy_init_thread(hy_decode_thread, hy_decode_thread_cb, NULL);
    hy_start_thread(hy_decode_thread);

    return NULL;
}

void *hy_stop_decode_thread()
{
    hy_stop_thread(hy_decode_thread);
    return NULL;
}

