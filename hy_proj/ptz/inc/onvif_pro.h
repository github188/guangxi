#ifndef __ONVIF_PRO_H__
#define __ONVIF_PRO_H__

/*PTZ操作*/
int onvif_ptz_move_up(struct soap * soap, ov_dev_t *dev, float step);									 //上
int onvif_ptz_move_down(struct soap * soap, ov_dev_t *dev, float step);								 	 //下
int onvif_ptz_move_left(struct soap * soap, ov_dev_t *dev, float step);								 	 //左
int onvif_ptz_move_right(struct soap * soap, ov_dev_t *dev, float step);								 //右
int onvif_ptz_move_left_up(struct soap * soap, ov_dev_t *dev, float step);								 //左上
int onvif_ptz_move_right_up(struct soap * soap, ov_dev_t *dev, float step);							 	 //右上
int onvif_ptz_move_left_down(struct soap * soap, ov_dev_t *dev, float step);							 //左下
int onvif_ptz_move_right_down(struct soap * soap, ov_dev_t *dev, float step);							 //右下
int onvif_ptz_zoom_in(struct soap * soap, ov_dev_t *dev, float step);									 //拉近
int onvif_ptz_zoom_out(struct soap * soap, ov_dev_t *dev, float step);									 //拉远
int onvif_ptz_move_stop(struct soap * soap, ov_dev_t *dev);											 	 //停止

/*预置点操作*/
int onvif_preset_set(struct soap *soap, ov_dev_t *dev, int id);    										 //设置预置点
int onvif_preset_clear(struct soap *soap, ov_dev_t *dev, int id);										 //清除预置点
int onvif_preset_goto(struct soap *soap, ov_dev_t *dev, int id);										 //转至预置点

/*预置路径操作*/
int onvif_tour_init(struct soap *soap ,ov_dev_t *dev);												     //路径初始化，返回0才能执行创建路径操作
int onvif_tour_create(struct soap *soap, ov_dev_t *dev, int id, ov_preset_t *presets, int presets_num);  //创建路径
int onvif_tour_del(struct soap *soap, ov_dev_t *dev, int id);											 //删除路径
int onvif_tour_start(struct soap *soap, ov_dev_t *dev, int id);											 //执行路径
int onvif_tour_stop(struct soap *soap, ov_dev_t *dev, int id);											 //停止路径

#endif
