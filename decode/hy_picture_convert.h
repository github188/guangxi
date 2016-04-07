/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :????????
* Change log		 :  Jimmy2015-12-18
********************* (C) COPYRIGHT 2015 HY *******************/
#ifndef HY_PIC_CONVERT_H
#define HY_PIC_CONVERT_H

typedef struct _hy_alarm_rect_t
{
	int left;
	int top;
	int right;
	int bottom;
} hy_alarm_rect_t;

typedef struct _rgb_meta_data_t
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} rgb_meta_data_t;

int yuv420P2rgb24(unsigned char *yuv_data, unsigned char *rgb24_data, int width, int hight);
void print_one_rect_on_rgb(unsigned char *rgb_data, int width, int height, int line_width, const hy_alarm_rect_t *p_rect);
int save_rgb24_to_jpg(unsigned char *pdata, char *jpg_file, int width, int height);

#endif