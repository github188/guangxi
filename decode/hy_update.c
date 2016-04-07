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
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>

#include "global_config.h"
#include "hy_update.h"
#include "hy_base_config.h"
#include "hy_device_status.h"

#include "hy_debug.h"
#include "hy_zlog.h"

/* ͨ��ftp�ӷ���������a9�����ļ� */
static void a9_download_update_file(char *ip, int port, char *usr, char *pass)
{
    //printf("wget ftp://admin:admin@171.221.207.58:2121/fire_a9.tar.bz2\n");
	//system("wget ftp://admin:admin@171.221.207.58:2121/fire_mk60.tar.bz2");
    //system("wget ftp://admin:admin@171.221.207.58:2121/fire_mk60.md5");

	char wget_mk60_req[128], wget_md5_req[128];

	sprintf(wget_mk60_req, "wget ftp://%s:%s@%s:%d/mv_detect.tar.bz2", usr, pass, ip, port);
	sprintf(wget_md5_req, "wget ftp://%s:%s@%s:%d/mv_detect.md5", usr, pass, ip, port);
    hy_zlog_info("%s.\n", wget_mk60_req);
    hy_zlog_info("%s.\n", wget_md5_req);

	system(wget_mk60_req);
	system(wget_md5_req);
}

/* �ж����ص��������Ƿ���� */
static int a9_update_file_exist_judge()
{
	int exist_flag = -1;

    if(access("./mv_detect.tar.bz2",F_OK) == 0 && \
            access("./mv_detect.md5",F_OK) == 0  )
    {
        exist_flag = 0;
    }

	return exist_flag;
}

/* a9�������������ж� */
static int a9_file_md5_calc()
{
    FILE *md5_calc = NULL;
    FILE *md5_orig = NULL;

	md5_calc =  popen("md5sum mv_detect.tar.bz2 | awk '{print $1}'", "r" );
	md5_orig =  popen("cat mv_detect.md5 | awk '{print $1}'", "r" );

    if(md5_calc == NULL)
    {
        printf("mv_detect md5_calc get error\n");
        return -1;
    }
    if(md5_orig == NULL)
    {
        printf("mv_detect md5_orig get error\n");
        return -1;
    }

    char calc[32], orig[32];
    read(fileno(md5_calc), calc, sizeof(calc));
    read(fileno(md5_orig), orig, sizeof(orig));

    pclose(md5_calc);
    pclose(md5_orig);

    if(strncmp(calc,orig,32) != 0)
    {
        printf("mv_detect md5 calc fail\n");
        return -1;
    }

    return 0;
}

void *a9_handle_update_thread(void *s)
{
	/* ��a9������־ */
	a9_update_flag = 1;

	/* �ж������������Ƿ����� */
	if(device_net_status_judge(g_hy_nvr_info.hy_nvr_server_info.server_ip) == -1)
	{
		printf("a9 ftp is offline\n");
		a9_update_flag = 0;
		return (void *)(-1);
	}

    /*Ŀǰ�ݰ����̶ֹ�����*/
    system("rm mv_detect.md5");
    system("rm mv_detect.tar.bz2");
	/* ��ftp����������a9�����ļ� */
	/*ע���޸�����*/
	a9_download_update_file(g_hy_nvr_info.hy_nvr_server_info.server_ip, 2121, "admin", "admin");

	/* �ж��ļ��Ƿ����سɹ� */
	if(a9_update_file_exist_judge() == -1)
    {
		printf("a9 file download fail\n");
		a9_update_flag = 0;
        return (void *)(-1);
    }

	/* �Ը����ļ�����md5У�飬��֤�ļ������� */
    if(a9_file_md5_calc() == -1)
    {
    	printf("video a9 md5 calc fail");
		a9_update_flag = 0;
        return (void *)(-1);
    }

	/* ϵͳ������������������ */
//    system("touch /home/fire_project/update_status/update");
//    system("kill `cat /var/run/ppp0.pid`");//�ر�3G
//    system("zte_4g_poweroff");//�ر�4G
//    system("rm /var/lock/*");//����usb����ռ��
//    sleep(3);
    system("tar jxvf mv_detect.tar.bz2");//��ѹ
    hy_sleep(3);
    system("reboot");//ϵͳ������������������

    a9_update_flag = 0;
    return (void *)(-1);
}

