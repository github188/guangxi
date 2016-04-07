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

/*�ѵ�ǰλ������Ԥ�õ�*/
int onvif_preset_set(struct soap *soap, ov_dev_t *dev, int id)
{
    printf("add preset id is %d\n", id);
    /*����ж�*/
	if(id < 0 || id > OV_PRESETS_NUM_EACH_DEV)//idΪ0ʱ��������Զ�����token
	{
		printf("preset id is error\n");
		return -1;
	}

    return ov_ptz_set_preset(soap,  dev,  id);
}

/*���Ԥ�õ�*/
int onvif_preset_clear(struct soap *soap, ov_dev_t *dev, int id)
{
    printf("clear preset id is %d\n", id);
    /*����ж�*/
	if(id < 1 || id > OV_PRESETS_NUM_EACH_DEV)
	{
		printf("preset id is error\n");
		return -1;
	}
    return ov_ptz_remove_preset(soap,  dev,  id);
}

/*����Ԥ�õ�*/
int onvif_preset_goto(struct soap *soap, ov_dev_t *dev, int id)
{
    /*����ж�*/
	if(id < 1 || id > OV_PRESETS_NUM_EACH_DEV)
	{
		printf("preset id is error\n");
		return -1;
	}

    return ov_ptz_goto_preset(soap, dev, id);
}

