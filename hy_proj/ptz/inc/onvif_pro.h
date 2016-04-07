#ifndef __ONVIF_PRO_H__
#define __ONVIF_PRO_H__

/*PTZ����*/
int onvif_ptz_move_up(struct soap * soap, ov_dev_t *dev, float step);									 //��
int onvif_ptz_move_down(struct soap * soap, ov_dev_t *dev, float step);								 	 //��
int onvif_ptz_move_left(struct soap * soap, ov_dev_t *dev, float step);								 	 //��
int onvif_ptz_move_right(struct soap * soap, ov_dev_t *dev, float step);								 //��
int onvif_ptz_move_left_up(struct soap * soap, ov_dev_t *dev, float step);								 //����
int onvif_ptz_move_right_up(struct soap * soap, ov_dev_t *dev, float step);							 	 //����
int onvif_ptz_move_left_down(struct soap * soap, ov_dev_t *dev, float step);							 //����
int onvif_ptz_move_right_down(struct soap * soap, ov_dev_t *dev, float step);							 //����
int onvif_ptz_zoom_in(struct soap * soap, ov_dev_t *dev, float step);									 //����
int onvif_ptz_zoom_out(struct soap * soap, ov_dev_t *dev, float step);									 //��Զ
int onvif_ptz_move_stop(struct soap * soap, ov_dev_t *dev);											 	 //ֹͣ

/*Ԥ�õ����*/
int onvif_preset_set(struct soap *soap, ov_dev_t *dev, int id);    										 //����Ԥ�õ�
int onvif_preset_clear(struct soap *soap, ov_dev_t *dev, int id);										 //���Ԥ�õ�
int onvif_preset_goto(struct soap *soap, ov_dev_t *dev, int id);										 //ת��Ԥ�õ�

/*Ԥ��·������*/
int onvif_tour_init(struct soap *soap ,ov_dev_t *dev);												     //·����ʼ��������0����ִ�д���·������
int onvif_tour_create(struct soap *soap, ov_dev_t *dev, int id, ov_preset_t *presets, int presets_num);  //����·��
int onvif_tour_del(struct soap *soap, ov_dev_t *dev, int id);											 //ɾ��·��
int onvif_tour_start(struct soap *soap, ov_dev_t *dev, int id);											 //ִ��·��
int onvif_tour_stop(struct soap *soap, ov_dev_t *dev, int id);											 //ֹͣ·��

#endif
