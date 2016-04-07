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

/*通过名字找到tour*/
static ov_tour_t *ov_get_tour_by_id(ov_dev_t *dev, int id)
{
	int i, flag = 0;
	ov_tour_t *tour;
	for(i = 0; i < dev->tours_count; i++)
	{
		if(id == dev->tours[i].id)
		{
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		return NULL;
	}

    tour = &(dev->tours[i]);

	return tour;
}

/*从预置路径中删除预置点*/
static void ov_ptz_del_preset_from_tour(ov_tour_t *tour)
{
	struct list_head *pos, *tmp_pos;
    ov_preset_t *tmp_node;

	list_for_each_safe(pos,tmp_pos,&(tour->presets_list))
	{
		tmp_node = list_entry(pos, ov_preset_t, list);
	    ov_remove_preset_from_list(tmp_node);
	}
	INIT_LIST_HEAD(&(tour->presets_list));
}

/*预置路径获取*/
int onvif_tour_init(struct soap *soap ,ov_dev_t *dev)
{
	/*入参判断，tours_count>0则代表本地与设备已经同步*/
	if(dev->tours_count > 0)
	{
		return 0;
	}

	int i,j, ret;
	FILE *fp = NULL;
	ov_preset_t *preset;
	char buf[32], token[32];

	/*判断配置文件是否存在*/
	ret = access("tours.ini", F_OK);
	if(ret == -1)
	{
		/*未存在配置文件，则认为IPC第一次启动，删除机内所有路径，共8条*/
       for(i = 1; i < OV_TOURS_NUM_EACH_DEV ; i ++)
       {
        	sprintf(token, "%d", i);
			ov_ptz_remove_tour(soap, dev, token);
       }
       printf("tour count is %d\n", dev->tours_count);
		return 0;
	}

	/*配置文件存在则读取配置文件*/
	fp = fopen("tours.ini", "r");
	if(fp == NULL)
	{
		printf("fopen tours.ini fail\n");
		return -1;
	}

	if(fgets(buf,sizeof(buf),fp) == NULL)
	{
		printf("tours config have no data\n");
		return -1;
	}

    sscanf(buf,"%d",&(dev->tours_count));
    printf("tour count is %d\n", dev->tours_count);
	for(i = 0; i < dev->tours_count; i++)
	{
		fgets(buf,sizeof(buf),fp);
		sscanf(buf,"%d:%[^:]:%d",&(dev->tours[i].id), dev->tours[i].token, &(dev->tours[i].presets_count));
		for(j = 0; j < dev->tours[i].presets_count; j++)
		{
			preset = (ov_preset_t *)calloc(1,sizeof(ov_preset_t));
			fgets(buf, sizeof(buf), fp);
			sscanf(buf,"(%d,%[^,],%lld)",&(preset->id), preset->token, &(preset->duration));
			list_add_tail(&(preset->list),&(dev->tours[i].presets_list));
		}
	}

	fclose(fp);
	return 0;
}

int onvif_tour_create(struct soap *soap, ov_dev_t *dev, int id, ov_preset_t *presets, int presets_num)
{
    /*已存在该编号的路径*/
	if(ov_get_tour_by_id(dev, id) != NULL)
	{
		printf("already exist this tour\n");
		return -1;
	}

	//新路径来袭
	/*入参判断，tour数量最多OV_TOURS_NUM_EACH_DEV条*/
	if(dev->tours_count >= OV_TOURS_NUM_EACH_DEV)
	{
		printf("already max tours count\n");
		return -1;
	}

    /*写设备*/
	if(ov_ptz_create_tour(soap, dev, presets,presets_num) != 0)
	{
		return -1;
	}

	/*写本地*/
	dev->tours[dev->tours_count].id = id;
	dev->tours[dev->tours_count].presets_count = presets_num;

	int i;
	ov_preset_t *preset;
	for(i = 0; i < presets_num; i++)
	{
		preset = (ov_preset_t *)calloc(1, sizeof(ov_preset_t));
		preset->id = presets[i].id;
		strcpy(preset->token, presets[i].token);
		preset->duration = presets[i].duration;
		preset->speed = presets[i].speed;
		list_add_tail(&(preset->list),&(dev->tours[dev->tours_count].presets_list));
	}

	dev->tours_count++;

	/*写文件操作*/
	ov_ptz_create_tours_ini(dev);

	return 0;
}

int onvif_tour_del(struct soap *soap, ov_dev_t *dev, int id)
{
  /*不存在该编号的路径*/
	int i, flag = 0;
	for(i = 0; i < dev->tours_count; i++)
	{
		if(id == dev->tours[i].id)
		{
			flag = 1;
			break;
		}
	}
	if(flag == 0)
	{
		printf("not exist this tour\n");
		return -1;
	}

	/*从设备*/
	if(ov_ptz_remove_tour(soap, dev, dev->tours[i].token) != 0)
	{
		return -1;
	}

	/*从本地*/
	ov_ptz_del_preset_from_tour(&(dev->tours[i]));

    int j;
    for (j = i; j < dev->tours_count - 1; j++)
    {
        dev->tours[j].id = dev->tours[j+1].id;
		strcpy(dev->tours[j].token, dev->tours[j+1].token);
		dev->tours[j].presets_count = dev->tours[j+1].presets_count;
		list_splice_init(&(dev->tours[j+1].presets_list),&(dev->tours[j].presets_list));
    }

    dev->tours_count--;

	/*写文件操作*/
    ov_ptz_create_tours_ini(dev);

    return 0;
}

int onvif_tour_start(struct soap *soap, ov_dev_t *dev, int id)
{
 	/*不存在该编号的路径*/
	ov_tour_t *tour = ov_get_tour_by_id(dev, id);
	if(tour == NULL)
	{
		printf("not exist this tour\n");
		return -1;
	}
    printf("tour token is %s\n",  tour->token);
	return ov_ptz_operate_tour(soap, dev, tour->token, tt__PTZPresetTourOperation__Start);
}

int onvif_tour_stop(struct soap *soap, ov_dev_t *dev, int id)
{
 	/*不存在该编号的路径*/
	ov_tour_t *tour = ov_get_tour_by_id(dev, id);
	if(tour == NULL)
	{
		printf("not exist this tour\n");
		return -1;
	}
	printf("tour token is %s\n",  tour->token);
	return ov_ptz_operate_tour(soap, dev, tour->token, tt__PTZPresetTourOperation__Stop);
}

