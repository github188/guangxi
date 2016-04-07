/*********************************************
 *onvifЭ����������
 *date:     2014.6.23
 *changelog:
 *2015.03.23 : �޸�����Ԥ�õ㺯��-���ܼ���ͬ��Ԥ�õ�
 *2015.05.06 : �޸ı佹����Ϊ�����佹��ͨ����ʱ����ֹͣ����ʵ��
 *Discription��
 	1��ʵ�ֿ������ķ���
 	2��ʵ��ý�����ķ���
 	3���ܹ�ʵ�ֵ�½��¼��ץͼ��Ԥ����Ԥ��λ�����á����á�ɾ������·������ӡ���ʼ��������ɾ����
 *********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <termios.h>

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"

#include "uuid/uuid.h"
#include "soapH.h"
#include "wsseapi.h"

#include "ov_types.h"
#include "ov_dev.h"
#include "ov_ptz.h"
#include "ov_list_op.h"
#include "onvif_pro.h"

/*��ͷ����*/
int onvif_ptz_move_up(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n", step);
    //ov_ptz_relative_move(soap,dev,0,step,0);
    return ov_ptz_continuous_move(soap, dev, 0, step, 0);
}
/*��ͷ����*/
int onvif_ptz_move_down(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n", step);
    step = 0 - step;
    //ov_ptz_relative_move(soap,dev,0,step,0);
    return ov_ptz_continuous_move(soap, dev, 0, step, 0);
}
/*��ͷ����*/
int onvif_ptz_move_left(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n", step);
	step = 0 - step;
    //ov_ptz_relative_move(soap,dev,step,0,0);
    return ov_ptz_continuous_move(soap, dev, step, 0, 0);
}
/*��ͷ����*/
int onvif_ptz_move_right(struct soap * soap, ov_dev_t *dev, float step)
{
    //ov_ptz_relative_move(soap,dev,step,0,0);
    printf("float is %f\n",step);
    return ov_ptz_continuous_move(soap, dev, step, 0, 0);
}

/*��ͷ����*/
int onvif_ptz_move_left_up(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
	return ov_ptz_continuous_move(soap, dev, 0-step, step, 0);
}

/*��ͷ����*/
int onvif_ptz_move_right_up(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
	return ov_ptz_continuous_move(soap, dev, step, step, 0);
}

/*��ͷ����*/
int onvif_ptz_move_left_down(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
	return ov_ptz_continuous_move(soap, dev, 0-step, 0-step, 0);
}

/*��ͷ����*/
int onvif_ptz_move_right_down(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
	return ov_ptz_continuous_move(soap, dev, step, 0-step, 0);
}

/*��ͷ����*/
int onvif_ptz_zoom_in(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
    //ov_ptz_relative_move(soap,dev,0,0,step);
    return ov_ptz_continuous_move(soap, dev, 0, 0, step);
}
/*��ͷ��Զ*/
int onvif_ptz_zoom_out(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("float is %f\n",step);
    step = 0 - step;
    //ov_ptz_relative_move(soap,dev,0,0,step);
    return ov_ptz_continuous_move(soap, dev, 0, 0, step);
}

/*��ͷ�����ƶ�������ָ��ֹͣ*/
/*��ͷ��������*/
int onvif_ptz_move_left_continue(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("input 9 stop continue move\n");
    return ov_ptz_continuous_move(soap, dev, step, 0, 0);
}
/*��ͷ��������*/
int onvif_ptz_move_right_continue(struct soap * soap, ov_dev_t *dev, float step)
{
    printf("input 9 stop continue move\n");
    step = 0 - step;
    return ov_ptz_continuous_move(soap, dev, step, 0, 0);
}
/*��ͷֹͣ�ƶ�*/
int onvif_ptz_move_stop(struct soap * soap, ov_dev_t *dev)
{
    return ov_ptz_stop_move(soap, dev);
}

/*��ͷ����home position*/
int onvif_ptz_return_home(struct soap * soap, ov_dev_t *dev)
{
    return ov_ptz_return_home_position(soap, dev);
}
/*���õ�ǰΪhome position*/
int onvif_ptz_set_home(struct soap * soap, ov_dev_t *dev)
{
    return ov_ptz_set_home_position(soap, dev);
}
