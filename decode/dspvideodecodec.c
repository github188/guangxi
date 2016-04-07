#include <stdio.h>
#include <string.h>

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

#include "dspvideodecodec.h"
#include <stdlib.h>

/* Temporary compilation error fix for OMAP 3530. 
   Remove the definition when it uses CE 2.22 or later */
#ifdef Dmai_Device_omap3530
#define XDM_YUV_420SP 9 
#endif


/* Align buffers to this cache line size (in bytes)*/
#define BUFSIZEALIGN            128
#define VIDEOSTD                VideoStd_720P_50

#if DECODE_TEST
/* vbuf size that has been selected based on size/performance tradeoff */
#define VBUFSIZE                20480
FILE                   *outFile      = NULL;
static Char vbuffer[VBUFSIZE];
#endif

typedef struct _Decoder{
	char 			*engineName;
	char 			*codecName;
	Engine_Handle 	hEngine;
	Vdec2_Handle    hVd2;
	int 			numBufs;
	Buffer_Handle   hInBuf;
	int 			ret;
    Int             numFrame;
    VIDDEC2_Params          params;   
    VIDDEC2_DynamicParams   dynParams;
    BufTab_Handle          hBufTab;
}Decoder;

Decoder dspDecoder = {
	.engineName 	= 	"decode",
	.codecName		=	"h264dec",
	.hEngine 		=	NULL,
	.hVd2 			=	NULL,
	.numBufs 		=   3,
	.hInBuf   		= 	NULL,
	.ret 			=   Dmai_EOK,
    .numFrame       =   1,
    .hBufTab        =   NULL
};


/******************************************************************************
 * writeFrameUYVY
******************************************************************************/
static Int writeFrameUYVY(Buffer_Handle hBuf, FILE *outFile)
{
    Int8 *ptr;
    UInt32 offset;
    Int y;

    BufferGfx_Dimensions dim;
    BufferGfx_getDimensions(hBuf, &dim);

    offset = dim.y * dim.lineLength + dim.x * 2;
    ptr = Buffer_getUserPtr(hBuf) + offset;

    for (y = 0; y < dim.height; y++) {
        if (fwrite(ptr, dim.width * 2, 1, outFile) != 1) {
            printf("Failed to write data to disk\n");
            return -1;
        }

        ptr += dim.lineLength;
    }

    printf("Wrote UYVY frame size %d (%dx%d) to disk\n",
           (Int) (dim.width * 2 * dim.height),
           (Int) dim.width, (Int) dim.height);

    return 0;
}


static int videodec1_init()
{
    Int32 width, height;                      
    VIDDEC2_Params         *params;
    VIDDEC2_DynamicParams  *dynParams;

    
   
    if (VideoStd_getResolution(VIDEOSTD, &width, &height) < 0) {
        printf("Failed to calculate resolution of video standard\n");
        return -1;
    }

    

    params = &(dspDecoder.params);
    dynParams = &(dspDecoder.dynParams);

    params->maxWidth = width;//VideoStd_1080P_WIDTH;
    params->maxHeight = height;//VideoStd_1080P_HEIGHT + 8;
    params->forceChromaFormat = XDM_YUV_420SP;
    params->maxFrameRate = 60000;
    params->maxBitRate = 4000000;
    
    printf("params->maxWidth[%d],params->maxHeight[%d] .\n ",params->maxWidth ,params->maxHeight);
   
    /* Create the video encoder */
    dspDecoder.hVd2 = Vdec2_create(dspDecoder.hEngine, dspDecoder.codecName, params, dynParams);
    if (dspDecoder.hVd2 == NULL) {
        printf("Failed to create video decoder: %s\n", dspDecoder.codecName);
        return -1;
    }


    BufferGfx_Attrs gfxAttrs  = BufferGfx_Attrs_DEFAULT;
    Buffer_Attrs    bAttrs    = Buffer_Attrs_DEFAULT;
    int inBufSize, outBufSize;

    /* Ask the codec how much data it needs */
    inBufSize   = Vdec2_getInBufSize(dspDecoder.hVd2);
    outBufSize  = Vdec2_getOutBufSize(dspDecoder.hVd2);

    gfxAttrs.bAttrs.memParams.align = bAttrs.memParams.align = BUFSIZEALIGN; 
    gfxAttrs.dim.width      = params->maxWidth;
    gfxAttrs.dim.height     = params->maxHeight;
    gfxAttrs.colorSpace     = ColorSpace_YUV420P;
    gfxAttrs.dim.lineLength = BufferGfx_calcLineLength(gfxAttrs.dim.width,  gfxAttrs.colorSpace);

    /* Create a table of output buffers of the size requested by the codec */
    dspDecoder.hBufTab = BufTab_create(dspDecoder.numBufs, Dmai_roundUp(outBufSize, BUFSIZEALIGN),
            BufferGfx_getBufferAttrs(&gfxAttrs));
    
    if (dspDecoder.hBufTab == NULL) {
        printf("Failed to allocate contiguous buffers\n");
        return -1;
    }

    /* Set input buffer table */
    Vdec2_setBufTab(dspDecoder.hVd2, dspDecoder.hBufTab);

#if 0
    bAttrs.reference = TRUE;
    /* Create the input buffer for decode video data */
    dspDecoder.hInBuf = Buffer_create(Dmai_roundUp(inBufSize , BUFSIZEALIGN),&bAttrs);
    if (dspDecoder.hInBuf == NULL) {
        printf("Failed to create contiguous buffer\n");
        return -1;
    }
#else

    //bAttrs = Buffer_Attrs_DEFAULT;
    dspDecoder.hInBuf = Buffer_create(inBufSize, &bAttrs); 
    if (dspDecoder.hInBuf == NULL) {
        printf("Failed to create contiguous buffer\n");
        return -1;
    }

//    gfxAttrs.bAttrs.useMask = CODEC_FREE | DISPLAY_FREE;

#endif
    return 0;
}

int tidec_init(void)
{
	Cpu_Device             device;
    dspDecoder.params         =   Vdec2_Params_DEFAULT;
    dspDecoder.dynParams      =   Vdec2_DynamicParams_DEFAULT;

	printf("Initing tidsp decodec...\n");

	/* Initialize the codec engine run time */
    CERuntime_init();

    /* Initialize DMAI */
    Dmai_init();


    /* Determine which device the application is running on */
    if (Cpu_getDevice(NULL, &device) < 0) {
        printf("Failed to determine target board\n");
        goto cleanup;
    }

    /* Open the codec engine */
    Engine_Error errorcode;
    dspDecoder.hEngine = Engine_open(dspDecoder.engineName, NULL, &errorcode);

    if (dspDecoder.hEngine == NULL) {
        printf("Failed to open codec engine: %s, error=%d\n", dspDecoder.engineName, errorcode);
        goto cleanup;
    }

    if(videodec1_init() < 0){
        printf("video dec2 init failure\n");
        goto cleanup;
    }

#if DECODE_TEST
     /* Open output file */
    outFile = fopen("output.h264", "wb");

    if (outFile == NULL) {
        printf("Failed to create output file \n");
        goto cleanup;
    }

    /* Using a larger vbuf to enhance performance of file i/o */
    if (setvbuf(outFile, vbuffer, _IOFBF, sizeof(vbuffer)) != 0) {
        printf("Failed to setvbuf on file descriptor\n");
        goto cleanup;   
    }
#endif

    printf("Start dsp DEcode normal!\n");

    return 0;    
cleanup:
    destroy();

	return -1;
}

/******************************************************************************
 * resizeBufTab
******************************************************************************/
static Int resizeBufTab(Vdec2_Handle hVd2, Int displayBufs)
{
    BufTab_Handle hBufTab = Vdec2_getBufTab(dspDecoder.hVd2);
    Int numBufs, numCodecBuffers, numExpBufs;
    Buffer_Handle hBuf;
    Int32 frameSize;

    /* How many buffers can the codec keep at one time? */
    numCodecBuffers = Vdec2_getMinOutBufs(dspDecoder.hVd2);

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
    frameSize = Vdec2_getOutBufSize(dspDecoder.hVd2);

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
            BufTab_collapse(Vdec2_getBufTab(dspDecoder.hVd2));

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

static void free_unusedbuf()
{
    /* Get a buffer to free from the codec */
    Buffer_Handle  hFreeBuf = Vdec2_getFreeBuf(dspDecoder.hVd2);

    while (hFreeBuf) {
        /* The codec is no longer using the buffer */
        BufTab_freeBuf(hFreeBuf);
        hFreeBuf = Vdec2_getFreeBuf(dspDecoder.hVd2);
    }
}

int tidec_process(BufInfo *user_inbuf, BufInfo *user_outbuf, int *got_picture)
{
    Buffer_Handle hDstBuf   = NULL;
    Buffer_Handle hOutBuf   = NULL;

    hDstBuf = BufTab_getFreeBuf(dspDecoder.hBufTab);    
    if (hDstBuf == NULL) {
        printf("Failed to get a free contiguous buffer from BufTab\n");
        BufTab_print(dspDecoder.hBufTab);
        goto cleanup;
    }
    
#if 1        
    memcpy(Buffer_getUserPtr(dspDecoder.hInBuf), user_inbuf->ptr, user_inbuf->num_byteused);
#else
    if(dspDecoder.numFrame){
        if(Buffer_setUserPtr(dspDecoder.hInBuf, (Int8*)user_inbuf->ptr)<0){
            printf("Buffer_setUserPtr Error\n");
        }
            printf("Set inptr\n");
    }
#endif
    Buffer_setNumBytesUsed(dspDecoder.hInBuf, user_inbuf->num_byteused);

    /* Make sure the whole buffer is used for input */
    BufferGfx_resetDimensions(hDstBuf);
           
    /* Encode the video buffer */
    if (Vdec2_process(dspDecoder.hVd2, dspDecoder.hInBuf, hDstBuf) < 0) {
        printf("Failed to decode video buffer\n");
        goto cleanup;
    }

    user_outbuf->num_byteused=Buffer_getNumBytesUsed(dspDecoder.hInBuf);
    /* Get a buffer for display from the codec */
    hOutBuf = Vdec2_getDisplayBuf(dspDecoder.hVd2);
    while (hOutBuf) {
        *got_picture=1;
        if(!user_outbuf->ptr){
            printf("Set outptr\n");
            user_outbuf->ptr = Buffer_getUserPtr(hOutBuf);
        }
    #if DECODE_TEST
        writeFrameUYVY(hOutBuf, outFile);
    #endif
        /* Get another buffer for display from the codec */
        hOutBuf = Vdec2_getDisplayBuf(dspDecoder.hVd2);
    }

    free_unusedbuf();
    if (dspDecoder.numFrame == 1) {
        /* Resize the BufTab after the first frame has been processed */
        printf("resize the buftab...\n");
        dspDecoder.numBufs = resizeBufTab(dspDecoder.hVd2, dspDecoder.numBufs);
        if (dspDecoder.numBufs < 0) {
            goto cleanup;
        }
        dspDecoder.numFrame=0;
    } 

    return 0;

cleanup:
    destroy();
    return -1; 
}


void destroy()
{
    printf("destroying .....\n");

    free_unusedbuf();
    
    if (dspDecoder.hBufTab){
        BufTab_delete(dspDecoder.hBufTab);
    }

    if (dspDecoder.hVd2){
        Vdec2_delete(dspDecoder.hVd2);
    }

    if (dspDecoder.hInBuf){
        Buffer_delete(dspDecoder.hInBuf);
    }

    if (dspDecoder.hEngine) {
        Engine_close(dspDecoder.hEngine);
    }

    dspDecoder.numFrame = 1;
#if DECODE_TEST
    if(outFile)
        fclose(outFile);
#endif

}

unsigned char * dsp_memory_alloc(int size)
{
    Memory_AllocParams params = Memory_DEFAULTPARAMS;
    params.type = Memory_CONTIGHEAP;
    Ptr ptr = Memory_alloc(size, &params);
    if(ptr)
        printf("Alloc Success\n");
    return ptr;
} 

void dsp_memory_free(unsigned char *ptr, int size)
{
    Memory_AllocParams params = Memory_DEFAULTPARAMS;
    params.type = Memory_CONTIGHEAP;
    if(ptr){
        if(Memory_free(ptr, size, &params))
            printf("Free Success\n");
    }
}