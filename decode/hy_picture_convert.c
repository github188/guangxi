/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :图片压缩转换
* Change log		 :  Jimmy2015-12-18
********************* (C) COPYRIGHT 2015 HY *******************/
#include <stdio.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixfmt.h>
#include "jpeglib.h"
#include "jerror.h"
#include "hy_picture_convert.h"

#define __STDC_CONSTANT_MACROS
#define JPEG_QUALITY 50 //图片质量

int yuv420P2rgb24(unsigned char *yuv_data, unsigned char *rgb24_data, int width, int hight)
{
	//Parameters
//	FILE *src_file =fopen("pre.yuv", "rb");
//	if (src_file == NULL)
//	{
//        perror("open pre.yuv error\n");
//	}
	const int src_w=1328,src_h=hight;//1328*720
	enum AVPixelFormat src_pixfmt=AV_PIX_FMT_YUV420P;

	int src_bpp=av_get_bits_per_pixel(av_pix_fmt_desc_get(src_pixfmt));

//	FILE *dst_file = fopen("sintel_1280x720_rgb24.rgb", "wb");
//    if (dst_file == NULL)
//	{
//        perror("open sintel_1280x720_rgb24.rgb error\n");
//	}
	const int dst_w=width,dst_h=hight;
	enum AVPixelFormat dst_pixfmt=AV_PIX_FMT_RGB24;
	int dst_bpp=av_get_bits_per_pixel(av_pix_fmt_desc_get(dst_pixfmt));

	//Structures
	uint8_t *src_data[4];
	int src_linesize[4];

	uint8_t *dst_data[4];
	int dst_linesize[4];

	int rescale_method=SWS_BICUBIC;
	struct SwsContext *img_convert_ctx;
	uint8_t *temp_buffer=(uint8_t *)malloc(src_w*src_h*src_bpp/8);

	int frame_idx=0;
	int ret=0;
	ret= av_image_alloc(src_data, src_linesize,src_w, src_h, src_pixfmt, 1);
	if (ret< 0) {
		printf( "Could not allocate source image\n");
		return -1;
	}
	ret = av_image_alloc(dst_data, dst_linesize,dst_w, dst_h, dst_pixfmt, 1);
	if (ret< 0) {
		printf( "Could not allocate destination image\n");
		return -1;
	}
	//-----------------------------
	//Init Method 1
	img_convert_ctx =sws_alloc_context();
	//Show AVOption
	//av_opt_show2(img_convert_ctx,stdout,AV_OPT_FLAG_VIDEO_PARAM,0);
	//Set Value
	av_opt_set_int(img_convert_ctx,"sws_flags",SWS_BICUBIC|SWS_PRINT_INFO,0);
	av_opt_set_int(img_convert_ctx,"srcw",src_w,0);
	av_opt_set_int(img_convert_ctx,"srch",src_h,0);
	av_opt_set_int(img_convert_ctx,"src_format",src_pixfmt,0);
	//'0' for MPEG (Y:0-235);'1' for JPEG (Y:0-255)
	av_opt_set_int(img_convert_ctx,"src_range",1,0);
	av_opt_set_int(img_convert_ctx,"dstw",dst_w,0);
	av_opt_set_int(img_convert_ctx,"dsth",dst_h,0);
	av_opt_set_int(img_convert_ctx,"dst_format",dst_pixfmt,0);
	av_opt_set_int(img_convert_ctx,"dst_range",1,0);
	sws_init_context(img_convert_ctx,NULL,NULL);

	//Init Method 2
	//img_convert_ctx = sws_getContext(src_w, src_h,src_pixfmt, dst_w, dst_h, dst_pixfmt,
	//	rescale_method, NULL, NULL, NULL);
	//-----------------------------
	/*
	//Colorspace
	ret=sws_setColorspaceDetails(img_convert_ctx,sws_getCoefficients(SWS_CS_ITU601),0,
		sws_getCoefficients(SWS_CS_ITU709),0,
		 0, 1 << 16, 1 << 16);
	if (ret==-1) {
		printf( "Colorspace not support.\n");
		return -1;
	}
	*/
//	while(1)
//	{
//
        memcpy(temp_buffer, yuv_data, src_w*src_h*src_bpp/8);
//		if (fread(temp_buffer, 1, src_w*src_h*src_bpp/8, src_file) != src_w*src_h*src_bpp/8){
//			break;
//		}

		switch(src_pixfmt){
		case AV_PIX_FMT_GRAY8:{
			memcpy(src_data[0],temp_buffer,src_w*src_h);
			break;
							  }
		case AV_PIX_FMT_YUV420P:{
			memcpy(src_data[0],temp_buffer,src_w*src_h);                    //Y
			memcpy(src_data[1],temp_buffer+src_w*src_h,src_w*src_h/4);      //U
			memcpy(src_data[2],temp_buffer+src_w*src_h*5/4,src_w*src_h/4);  //V
			break;
								}
		case AV_PIX_FMT_YUV422P:{
			memcpy(src_data[0],temp_buffer,src_w*src_h);                    //Y
			memcpy(src_data[1],temp_buffer+src_w*src_h,src_w*src_h/2);      //U
			memcpy(src_data[2],temp_buffer+src_w*src_h*3/2,src_w*src_h/2);  //V
			break;
								}
		case AV_PIX_FMT_YUV444P:{
			memcpy(src_data[0],temp_buffer,src_w*src_h);                    //Y
			memcpy(src_data[1],temp_buffer+src_w*src_h,src_w*src_h);        //U
			memcpy(src_data[2],temp_buffer+src_w*src_h*2,src_w*src_h);      //V
			break;
								}
		case AV_PIX_FMT_YUYV422:{
			memcpy(src_data[0],temp_buffer,src_w*src_h*2);                  //Packed
			break;
								}
		case AV_PIX_FMT_RGB24:{
			memcpy(src_data[0],temp_buffer,src_w*src_h*3);                  //Packed
			break;
								}
		default:{
			printf("Not Support Input Pixel Format.\n");
			break;
							  }
		}

		sws_scale(img_convert_ctx, src_data, src_linesize, 0, src_h, dst_data, dst_linesize);
		frame_idx++;

		switch(dst_pixfmt){
		case AV_PIX_FMT_GRAY8:{
		    memcpy(rgb24_data, dst_data[0], dst_w*dst_h);
			break;
							  }
		case AV_PIX_FMT_YUV420P:{
		    memcpy(rgb24_data, dst_data[0], dst_w*dst_h);rgb24_data+=dst_w*dst_h;
		    memcpy(rgb24_data, dst_data[1], dst_w*dst_h/4);rgb24_data+=dst_w*dst_h/4;
		    memcpy(rgb24_data, dst_data[2], dst_w*dst_h/4);rgb24_data+=dst_w*dst_h/4;
			break;
								}
		case AV_PIX_FMT_YUV422P:{
//			fwrite(dst_data[0],1,dst_w*dst_h,dst_file);					//Y
//			fwrite(dst_data[1],1,dst_w*dst_h/2,dst_file);				//U
//			fwrite(dst_data[2],1,dst_w*dst_h/2,dst_file);				//V
			break;
								}
		case AV_PIX_FMT_YUV444P:{
//			fwrite(dst_data[0],1,dst_w*dst_h,dst_file);                 //Y
//			fwrite(dst_data[1],1,dst_w*dst_h,dst_file);                 //U
//			fwrite(dst_data[2],1,dst_w*dst_h,dst_file);                 //V
			break;
								}
		case AV_PIX_FMT_YUYV422:{
//			fwrite(dst_data[0],1,dst_w*dst_h*2,dst_file);               //Packed
			break;
								}
		case AV_PIX_FMT_RGB24:{
		    memcpy(rgb24_data, dst_data[0], dst_w*dst_h*3);
//			fwrite(dst_data[0],1,dst_w*dst_h*3,dst_file);               //Packed
			break;
							  }
		default:{
			printf("Not Support Output Pixel Format.\n");
			break;
							}
		}
//	}

	sws_freeContext(img_convert_ctx);

	free(temp_buffer);
//	fclose(dst_file);
	av_freep(&src_data[0]);
	av_freep(&dst_data[0]);

	return 0;

}


void print_one_rect_on_rgb(unsigned char *rgb_data, int width, int height, int line_width, const hy_alarm_rect_t *p_rect)
{
//    if ((p_rect->left + p_rect->top) >= (p_rect->right + p_rect->bottom))
//    {
//        return;
//    }
//    if (p_rect->right > width || p_rect->bottom > height)
//    {
//        return;
//    }
    rgb_meta_data_t *p_rgb_data = (rgb_meta_data_t *)rgb_data;

    //get line_rect
    hy_alarm_rect_t line_rect[4];
    line_rect[0].left =  p_rect->left;
    line_rect[0].right = p_rect->right;
    line_rect[0].top =  p_rect->top;
    line_rect[0].bottom = p_rect->top + line_width;

    line_rect[1].left =  p_rect->left;
    line_rect[1].right = p_rect->left + line_width;
    line_rect[1].top =  p_rect->top;
    line_rect[1].bottom = p_rect->bottom;

    line_rect[2].left =  p_rect->right - line_width;
    line_rect[2].right = p_rect->right;
    line_rect[2].top =  p_rect->top;
    line_rect[2].bottom = p_rect->bottom;

    line_rect[3].left =  p_rect->left;
    line_rect[3].right = p_rect->right;
    line_rect[3].top =  p_rect->bottom - line_width;
    line_rect[3].bottom = p_rect->bottom;
    //print line

    int m;
    for (m=0; m<4; m++)
    {
        int i, j;
        for (i=line_rect[m].left; i<line_rect[m].right; i++)
        {
            for(j=line_rect[m].top; j<line_rect[m].bottom; j++)
            {
//                printf("%d %d %d %d %d %d \n", i, j, line_rect[m].left, line_rect[m].right, line_rect[m].top, j<line_rect[m].bottom);
                p_rgb_data[i*width + j].r = 255;
                p_rgb_data[i*width + j].g = 0;
                p_rgb_data[i*width + j].b = 0;
            }
        }
    }
}

int save_rgb24_to_jpg(unsigned char *pdata, char *jpg_file, int width, int height)
{  //分别为RGB数据，要保存的jpg文件名，图片长宽
    int depth = 3;
    JSAMPROW row_pointer[1];//指向一行图像数据的指针
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile;

    cinfo.err = jpeg_std_error(&jerr);//要首先初始化错误信息
    //* Now we can initialize the JPEG compression object.
    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(jpg_file, "wb")) == NULL)
    {
        fprintf(stderr, "can't open %s\n", jpg_file);
        return -1;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;             //* image width and height, in pixels
    cinfo.image_height = height;
    cinfo.input_components = depth;    //* # of color components per pixel
    cinfo.in_color_space = JCS_RGB;     //* colorspace of input image
    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE ); //* limit to baseline-JPEG values
    jpeg_start_compress(&cinfo, TRUE);

    int row_stride = width * 3;
    while (cinfo.next_scanline < cinfo.image_height)
           {
            row_pointer[0] = (JSAMPROW)(pdata + cinfo.next_scanline * row_stride);//一行一行数据的传，jpeg为大端数据格式
            jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);//这几个函数都是固定流程
    fclose(outfile);

    return 0;
}