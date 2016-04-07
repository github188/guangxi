/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :
* Change log		 :  Jimmy2015-7-10
********************* (C) COPYRIGHT 2015 HY *******************/
/*************************************
 *3G流量监控
 *date:     2014.12.09
 *changelog:
 *	2014.12.27 : 增加时钟判断，当没获取到服务器时钟时，不进行流量统计
 *
 *************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <sqlite3.h>

#include "hy_flow_moniter.h"
#include "global_config.h"
#include "hy_protocl.h"
#include "hy_debug.h"
#include "hy_zlog.h"

typedef struct
{
	int year;
	int month;
	int flow; //流量值
}mobile_flow_value_t;

typedef struct//系统时间
{
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
} hy_time_t;

static sqlite3 *flow_db = NULL;

/* line 100 修改需监控的网卡
 * 未安装数据库的设备如何操作? 现象:上报的3G流量等于(-3)
 */
int flow_monitor_handle_thread_start()
{
    int ret;
    pthread_t tid;
    ret = pthread_create(&tid, NULL, flow_monitor_handle_thread, NULL);
    if (ret != 0)
    {
        perror("create flow_monitor_handle_thread");
        return -1;
    }

    return 0;
}

void *flow_monitor_handle_thread(void *s)
{
    printf("flow_monitor_handle_thread start...\n");
    sleep(30);//监控线程延迟30S启动

    int ret = 0,i = 0 , total, tx_pre, rx_pre, tx_now, rx_now;
    time_t now_time;
    struct tm *time_ptr;
    int year, month;

	/* 打开存储3G流量的数据库 */
	flow_db = NULL;
    ret = sqlite3_open("flow.db",&flow_db);
    if(ret != SQLITE_OK)
    {
        printf("Cannot open flow db: %s\n",sqlite3_errmsg(flow_db));
        return 0;
    }

    int server_sockfd;
    struct sockaddr_in server_addr;
    DATA_STRUCT data_info;
    data_info.flag = DEFAULT_FLAG;
    data_info.type = DTOA_MOBILE_FLOW_INFO;
    data_info.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;

    mobile_flow_value_t	flow_now;

	//获取当前时间,没有RTC时钟，只有当获取到服务器时钟才能进行流量统计，防止时钟错乱
	while(1)
	{
    	time(&now_time);
    	time_ptr=localtime(&now_time);
   		year = time_ptr->tm_year+1900;
   		month = time_ptr->tm_mon+1;
        printf("year %d \t month %d\n", year, month);
		if(year >= 2014)//获取到了服务器时钟
		{
			break;
		}

		sleep(15);
	}

	/* 判断网络接口是ppp0还是eth1 */
	char interface[8];
	while(1)
	{
		if(flow_net_interface_judge("eth0") == 0)
		{
			sprintf(interface,"eth0");
			break;
		}
		if(flow_net_interface_judge("ppp0") == 0)
		{
			sprintf(interface,"ppp0");
			break;
		}

		sleep(5);
	}


	//初始化
	//补偿1K的流量统计
	total = flow_monitor_get_value_from_database(flow_db, "total" , year, month);
    total += 1;
	flow_monitor_update_value_in_database(flow_db, "total", year, month, total);
	//
	rx_pre = 0;
	tx_pre = 0;
	tx_now = 0;
    rx_now = 0;

    while(1)
    {
        if(i >= 1)
        {
            sleep(60);
        }
        //获取当前时间
        time(&now_time);
        time_ptr=localtime(&now_time);
        year = time_ptr->tm_year+1900;
        month = time_ptr->tm_mon+1;

        //获取流量值
        if(flow_monitor_get_current_value(interface, &rx_now, &tx_now) == -1)
        {
            printf("get current flow value by ifconfig failed.");
            tx_now = 0;
            rx_now = 0;
            continue;//获取失败继续获取
        }
		tx_now /= 1000;
        rx_now /= 1000; // B -> KB
        if (i == 0)//第一次的不添加
        {
            rx_pre = rx_now;
            tx_pre = tx_now;
        }

        printf("tx_now %d\trx_now %d\n", tx_now, rx_now);
        printf("tx_pre %d\trx_pre %d\n", tx_pre, rx_pre);

        total = flow_monitor_get_value_from_database(flow_db, "total" , year, month);
        if (total == 0)//do not discovery so it is a new month
        {
            flow_monitor_insert_new_record_to_database(flow_db, year, month, total);
        }

        if(rx_pre <= rx_now || tx_pre <= tx_now)//youxiao  genxin
        {
            //total = flow_monitor_get_value_from_database(flow_db, "total" , year, month);
            total += (rx_now - rx_pre + tx_now - tx_pre);
            flow_monitor_update_value_in_database(flow_db,"total", year, month, total);
        }

        if (rx_pre > rx_now || tx_pre > tx_now)
        {
            //total = flow_monitor_get_value_from_database(flow_db, "total" , year, month);
            total += rx_now + tx_now;
            flow_monitor_update_value_in_database(flow_db,"total", year, month, total);
        }
        //把now值写入pre值
        rx_pre = rx_now;
        tx_pre = tx_now;

        if(i % 2 == 0)
        {
            //上报状态信息
            /* 连接服务器 */
            server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
            if(server_sockfd < 0)
            {
                perror("server_sockfd socket");
				i++;
                continue;
            }
            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = PF_INET;
            inet_pton(PF_INET, g_hy_nvr_info.hy_nvr_server_info.server_ip, &server_addr.sin_addr);
            server_addr.sin_port = htons(g_hy_nvr_info.hy_nvr_server_info.server_data_port);
            ret = connect(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
            if(ret == -1)
            {
                perror("connect");
                close(server_sockfd);
				i++;
                continue;
            }

            flow_now.year = year;
            flow_now.month = month;
            flow_now.flow = flow_monitor_get_value_from_database(flow_db, "total" , year, month);
            printf("current flow value is %d\n", flow_now.flow);
            memmove(&(data_info.mobile_flow_value), &flow_now, sizeof(mobile_flow_value_t));
            if(write(server_sockfd, &data_info, sizeof(data_info)) == -1)
            {
                perror("write");
            }
            else
            {
                printf("send 3g flow to server succed.\n");
            }
            sleep(1);
            close(server_sockfd);
        }

		i++;
    }
}


//获取当前ppp0使用的流量值
int flow_monitor_get_current_value(char *net, int *rx, int *tx)
{
	printf("get %s net flow\n",net);
    char cmd[16];
    sprintf(cmd, "ifconfig %s", net);

    FILE *flow =  popen(cmd, "r");
    if(flow == NULL)
    {
        printf("ifconfig %s error\n", net);
        return -1;
    }

    char buf[512];
    read(fileno(flow), buf, sizeof(buf));

    pclose(flow);

    char *p;
    p = strstr(buf, "RX bytes");
    if(p == NULL)
    {
        return -1;
    }
    sscanf(p,"%*[^:]:%d", rx);

    p = strstr(buf, "TX bytes");
    if(p == NULL)
    {
        return -1;
    }
    sscanf(p,"%*[^:]:%d", tx);

    return 0;
}

//从数据库中获取值，获取总量，rx,tx
int flow_monitor_get_value_from_database(sqlite3 *db, char *type, int year, int month)
{
    /* 入参判断 */
    if(db == NULL || type == NULL || \
            year < 2014 || year > 2020 ||
            month < 1 || month > 12)
    {
        printf("error time\n");
        return -1;
    }

    char *errmsg = 0;
    int ret = 0;

    char sqlcmd[128];
    sprintf(sqlcmd, "select %s from mobile_flow where year=%d and month=%d",\
            type, year, month);
    printf("sqlcmd %s\n", sqlcmd);

    int info = 0;
    ret = sqlite3_exec(db,sqlcmd, get_flow_value_callback, &info, &errmsg);
    if(ret != SQLITE_OK)
    {
        printf("get mobile_flow error: %s\n",errmsg);
        sqlite3_free(errmsg);
        return -1;
    }

    return info;
}

//更新数据库的值,设置总量，rx, tx
int flow_monitor_update_value_in_database(sqlite3 *db, char *type, int year, int month, int value)
{
    /* 入参判断 */
    if(db == NULL || type == NULL || \
            year < 2014 || year > 2020 ||
            month < 1 || month > 12)
    {
        printf("error time\n");
        return -1;
    }

    char *errmsg = 0;
    int ret = 0;

    char sqlcmd[128];
    sprintf(sqlcmd, "update mobile_flow set %s=%d where year=%d and month=%d",\
            type, value, year, month);
    printf("sqlcmd %s\n", sqlcmd);

    ret = sqlite3_exec(db,sqlcmd,NULL,NULL,&errmsg);
    if(ret != SQLITE_OK)
    {
        printf("update mobile_flow error: %s\n",errmsg);
        sqlite3_free(errmsg);
        return -1;
    }

    return 0;
}

int flow_monitor_insert_new_record_to_database(sqlite3 *db, int year, int month, int total)
{
    /* 入参判断 */
    if(db == NULL || \
            year < 2014 || year > 2020 ||
            month < 1 || month > 12)
    {
        printf("error time\n");
        return -1;
    }

    char *errmsg = 0;
    int ret = 0;

    char sqlcmd[128];
    sprintf(sqlcmd, "insert into mobile_flow(year,month,total,tx,rx) values(%d,%d,%d,0,0)",\
            year, month, total);
    printf("sqlcmd %s\n", sqlcmd);

    ret = sqlite3_exec(db,sqlcmd,NULL,NULL,&errmsg);
    if(ret != SQLITE_OK)
    {
        printf("update mobile_flow error: %s\n",errmsg);
        sqlite3_free(errmsg);
        return -1;
    }

    return 0;
}

int	get_flow_value_callback(void *para, int column, char **columnValue, char **columnName)
{
//    int *temp = (int *)para;
//    printf("%s\n", columnValue[0]);
//    *temp = atoi(columnValue[0]);

    int i;
    for(i=0; i<column; i++)
    {
        printf("%s = %s\n", columnName[i], columnValue[i] ? columnValue[i] : "NULL");
        *(int *)para = atoi(columnValue[i]);
    }

    return 0;
}


void *order_handle_get_flow_value_req_thread(void *s)
{
	hy_time_t *newtime = (hy_time_t *)s;
	int year = newtime->year;
	int month = newtime->month;
	free(newtime);

	int ret, total = 0, rx = 0, tx = 0;
    int server_sockfd;
    struct sockaddr_in server_addr;
    DATA_STRUCT data_info;
    data_info.flag = DEFAULT_FLAG;
    data_info.type = DTOA_MOBILE_FLOW_VALUE;
    data_info.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    mobile_flow_value_t	flow_now;

	/* 连接服务器 */
    server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(server_sockfd < 0)
    {
        perror("server_sockfd socket");
        free(newtime);
		//atod_get_flow_value_flag = 0;
        return (void *)(-1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    inet_pton(PF_INET, g_hy_nvr_info.hy_nvr_server_info.server_ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    ret = connect(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret == -1)
    {
        perror("connect");
        close(server_sockfd);
		//atod_get_flow_value_flag = 0;
		free(newtime);
        return (void *)(-1);
    }

    flow_now.year = year;
    flow_now.month = month;
    total = flow_monitor_get_value_from_database(flow_db, "total" , year, month);
	flow_now.flow = total;

    memmove(&(data_info.mobile_flow_value), &flow_now, sizeof(mobile_flow_value_t));
    write(server_sockfd, &data_info, sizeof(data_info));
    sleep(1);
    close(server_sockfd);

	//atod_get_flow_value_flag = 0;
	free(newtime);
	return (void *)0;
}


/* 判断当前使用的网络接口是ppp0还是eth1 */
int flow_net_interface_judge(char *interface)
{
	if(interface == NULL)
	{
		printf("interface is NULL\n");
		return -1;
	}

	/* ifconfig eth1 or ifconfig ppp0*/
	char cmd[128];
	sprintf(cmd, "ifconfig %s", interface);

	FILE *face = popen(cmd, "r" );
	if(face == NULL)
	{
		printf("get interface %s error\n", interface);
		return -1;
	}

    char dst[8];
    read(fileno(face), dst, sizeof(dst));
    pclose(face);

	if(strstr(dst, interface) == NULL)
	{
		return -1;
	}

    return 0;
}

