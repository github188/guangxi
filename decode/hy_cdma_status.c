/*************************************
 *3G模块工作状态上传
 *date:     2015.1.5
 *changelog:
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
#include <termios.h>

#include "hy_protocl.h"
#include "hy_debug.h"
#include "hy_base_config.h"
#include "hy_cdma_status.h"
//#include "hy_serial_utils.h"
#include "global_config.h"
#include "hy_zlog.h"

static int 	cdma_srv_status;
static int	cdma_sys_mode;
static int 	cdma_sim_state;
static int	cdma_1x_rssi;
static int 	cdma_1x_fer;
static int 	cdma_hdr_rssi;

int cdma_state_monitor_thread_start()
{
    int ret;
    pthread_t tid;

    ret = pthread_create(&tid, NULL, cdma_state_monitor_thread, NULL);
    if (ret != 0)
    {
        hy_zlog_info("create cdma_state_monitor_thread fail\n");
        perror("create cdma_state_monitor_thread");
        return -1;
    }

    return 0;
}

void *cdma_state_monitor_thread(void *s)
{
    hy_zlog_info("cdma_state_monitor_thread start...\n");

    /* 打开4G模块串口ttyUSB2 */
    serial_info_t serial = {"/dev/ttyUSB2", 115200, 8, 1, 'N'};
    int fd;
    while (1)
    {
        fd = init_serial(&serial);
        if(fd == -1)
        {
            hy_zlog_info("fd open %s fail.\n", "/dev/ttyUSB2");
            hy_sleep(30);
            continue;
        }
        break;
    }

    int ret, server_sockfd;
    struct sockaddr_in server_addr;

    CDMA_MODEL_STATUS cdma;
    DATA_STRUCT data_info;
    data_info.flag = DEFAULT_FLAG;
    data_info.type = DTOA_3G_MODULE_STATUS;
    data_info.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    int count = 0;
    char temp[128] = {0};

    while(1)
    {
        sleep(10);
        count ++;
        /* 取消回显 */
        cdma_state_send_at_cmd(fd,"ATE0");
        /*AT^CPIN? */
//        cdma_state_send_at_cmd(fd,"ATI");
//        cdma_state_read_uart(fd, temp);

        /* 获取系统状态 */
        cdma_state_send_at_cmd(fd,"AT^SYSINFO");
        usleep(300*1000);
        cdma_state_get_sysinfo(fd);
        hy_zlog_info("srv_state:%d,system_mode:%d,sim_state:%d\n",cdma_srv_status,cdma_sys_mode,cdma_sim_state);


        /* 获取1x信号强度 */
        cdma_state_send_at_cmd(fd,"AT+CSQ");
        usleep(300*1000);
        cdma_state_get_cdma1x_csq(fd);
        hy_zlog_info("+CSQ:%d, fer:%d\n",cdma_1x_rssi, cdma_1x_fer);


        /* 上报1 */

        /* 上报2 */
        if(count % 5 == 0)
        {
            server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
            if(server_sockfd < 0)
            {
                perror("server_sockfd socket");
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
                continue;
            }

            cdma.srv_status = cdma_srv_status;
            cdma.sys_mode = cdma_sys_mode;
            cdma.sim_state = cdma_sim_state;
            cdma.rssi = cdma_1x_rssi;
            cdma.fer = cdma_1x_fer;
            cdma.hdr_rssi = cdma_hdr_rssi;


            memmove(&(data_info.cdma), &cdma, sizeof(CDMA_MODEL_STATUS));
            write(server_sockfd, &data_info, sizeof(DATA_STRUCT));
            sleep(1);
            close(server_sockfd);
        }

    }
}

/**
 *根据通信口参数配置串口参数
 */
int set_serial_para(const char *device, int baud, char parity, int data_bit, int stop_bit)
{
    struct termios tios;
    speed_t speed;

    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL);
    if (fd == -1)
    {
        fprintf(stderr, "ERROR Can't open the device %s \n", device);
        return -1;
    }

    memset(&tios, 0, sizeof(struct termios));

    /* C_ISPEED     Input baud (new interface)
       C_OSPEED     Output baud (new interface)
    */
    switch (baud)
    {
    case 110:
        speed = B110;
        break;
    case 300:
        speed = B300;
        break;
    case 600:
        speed = B600;
        break;
    case 1200:
        speed = B1200;
        break;
    case 2400:
        speed = B2400;
        break;
    case 4800:
        speed = B4800;
        break;
    case 9600:
        speed = B9600;
        break;
    case 19200:
        speed = B19200;
        break;
    case 38400:
        speed = B38400;
        break;
    case 57600:
        speed = B57600;
        break;
    case 115200:
        speed = B115200;
        break;
    default:
        speed = B9600;
    }

    /* Set the baud rate */
    if ((cfsetispeed(&tios, speed) < 0) || (cfsetospeed(&tios, speed) < 0))
    {
        close(fd);
        fd = -1;
        return -1;
    }

    /* C_CFLAG      Control options
     CLOCAL       Local line - do not change "owner" of port
     CREAD        Enable receiver
     */
    tios.c_cflag |= (CREAD | CLOCAL);
    /* CSIZE, HUPCL, CRTSCTS (hardware flow control) */

    /* Set data bits (5, 6, 7, 8 bits)
       CSIZE        Bit mask for data bits
    */
    tios.c_cflag &= ~CSIZE;
    switch (data_bit)
    {
    case 5:
        tios.c_cflag |= CS5;
        break;
    case 6:
        tios.c_cflag |= CS6;
        break;
    case 7:
        tios.c_cflag |= CS7;
        break;
    case 8:
    default:
        tios.c_cflag |= CS8;
        break;
    }

    /* Stop bit (1 or 2) */
    if (stop_bit == 1)
        tios.c_cflag &= ~CSTOPB;
    else
        /* 2 */
        tios.c_cflag |= CSTOPB;

    /* PARENB       Enable parity bit
     PARODD       Use odd parity instead of even */
    if (parity == 'N')
    {
        /* None */
        tios.c_cflag &= ~PARENB;
    }
    else if (parity == 'E')
    {
        /* Even */
        tios.c_cflag |= PARENB;
        tios.c_cflag &= ~PARODD;
    }
    else
    {
        /* Odd */
        tios.c_cflag |= PARENB;
        tios.c_cflag |= PARODD;
    }

    /* Read the man page of termios if you need more information. */
    /* Raw input */
    tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* C_IFLAG      Input options*/
    if (parity == 'N')
    {
        /* None */
        tios.c_iflag &= ~INPCK;
    }
    else
    {
        tios.c_iflag |= INPCK;
    }

    /* Software flow control is disabled */
    tios.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* Raw ouput */
    tios.c_oflag &= ~OPOST;

    /* C_CC         Control characters
     VMIN         Minimum number of characters to read
     VTIME        Time to wait for data (tenths of seconds)
     Unused because we use open with the NDELAY option */
    tios.c_cc[VMIN] = 0;
    tios.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tios) < 0)
    {
        close(fd);
        fd = -1;
        return -1;
    }
    return fd;
}

/**
 * 初始化串口
 * 返回设置完成后的串口句柄
 */
int init_serial(serial_info_t *serial)
{
    int fd;

    fd = set_serial_para(serial->identifier, serial->baud, serial->parity, serial->data_bits, serial->stop_bits);
    if (fd == -1)
    {
        printf("%s serial open or init fail\n",serial->identifier);
        return -1;
    }

    return fd;
}


/* at指令发送 */
int cdma_state_send_at_cmd(int serial_fd, char *cmd)
{
    int nwrite;
    char buf[128];

    memset(buf, 0, sizeof(buf));
    strcpy(buf,cmd);
    strcat(buf,"\r");//以回车结尾

    tcflush(serial_fd, TCIOFLUSH);
    nwrite = write(serial_fd,buf,strlen(buf));
    hy_zlog_info("nwrite=%d,%s\n",nwrite,buf);

    return nwrite;
}

/*读3G模块回码*/
int cdma_state_read_uart(int serial_fd,char reply[])
{
    int nread;
    char buf[128];

    memset(buf, 0, sizeof(buf));
    nread = read(serial_fd,buf,sizeof(buf));
    hy_zlog_info("nread=%d, %s\n",nread, buf);
    strcpy(reply,buf);

    return nread;
}

/* AT^SYSINFO回码解析 */
int cdma_state_get_sysinfo(int serial_fd)
{
    int nread,  temp;
    char *p ;
    char reply[128];

    nread = cdma_state_read_uart(serial_fd,reply);
    if(nread > 0)
    {
        p = strstr(reply, "SYSINFO");
        if(p == NULL)
        {
            hy_zlog_info("don't get cdma sysinfo\n");
            return -1;
        }
        sscanf(p,"%*[^:]:%d,%d,%d,%d,%d", &cdma_srv_status, &temp, & temp, \
               &cdma_sys_mode, &cdma_sim_state);
    }
    return 0;
}

/* AT+CSQ回码解析 */
int cdma_state_get_cdma1x_csq(int serial_fd)
{
    int nread;
    char *p ;
    char reply[128];

    nread = cdma_state_read_uart(serial_fd,reply);
    if(nread > 0)
    {
        p = strstr(reply, "CSQ");
        if(p == NULL)
        {
            hy_zlog_info("don't get cdma1x csq\n");
            return -1;
        }
        sscanf(p,"%*[^:]:%d,%d", &cdma_1x_rssi, &cdma_1x_fer);
    }
    return 0;
}

/* AT^HDRCSQ回码解析 */
int cdma_state_get_cdma3g_csq(int serial_fd)
{
    int nread;
    char *p ;
    char reply[128];

    nread = cdma_state_read_uart(serial_fd,reply);
    if(nread > 0)
    {
        p = strstr(reply, "HDRCSQ");
        if(p == NULL)
        {
            hy_zlog_info("don't get cdma 3g csq\n");
            return -1;
        }
        sscanf(p,"%*[^:]:%d", &cdma_hdr_rssi);
    }
    return 0;
}

/*AT+CSQ回码解析*/
int cdma_state_get_cdma4g_csq(int serial_fd)
{
    int nread, temp;
    char *p ;
    char reply[128];

    nread = cdma_state_read_uart(serial_fd,reply);
    if(nread > 0)
    {
        p = strstr(reply, "CSQ");
        if(p == NULL)
        {
            hy_zlog_info("don't get cdma 3g csq\n");
            return -1;
        }
        sscanf(p,"%*[^:]:%d,%d", &cdma_hdr_rssi, &temp);
    }
    return 0;
}
