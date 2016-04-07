/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          : hy_422_protocol.cpp
* Author             :
* Revision           :
* Date              :
* Description        : hy_422_protocol implement
* Change log		: 1.Initial version :2015-04-15 by JimmyZhang
*                   ++++++++++++++++++
********************************************************************************
* THE PRESENT SOFTWARE IS DISTRIBUTED IN THE HOPE THAT IT WILL PROVIDE CUSTOMERS
* WITH CODING INFORMATION TO SPEED UP THEIR PRODUCT DEVELOPMENT, BUT WITHOUT
* ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS
* FOR A PARTICULAR PURPOSE. AS A RESULT, TECHWELL SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>

#include "hy_serial_ctrl_protocol.h"
#include "hy_debug.h"
#include "hy_io_utils.h"
#include "hy_zlog.h"

#define NEW_BORAD

#ifndef HY_DEBUG
#define HY_DEBUG
#endif

#ifdef NEW_BORAD
#define RS422_BAURRATE  (115200)
#else
#define RS422_BAURRATE  (38400)
#endif

#define RS422_DATABIT   (8)
#define RS422_STOPBIT   (1)
#define RS422_DEV "/dev/ttyS0"

static const int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300};
static const int name_arr[] =  {115200, 38400, 19200, 9600, 4800, 2400, 1200, 300};

/*
*���ô������Ժ���
*/
static hy_s32 set_tty_param(hy_s32 fd, hy_s32 speed, hy_s32 databits, hy_s32 stopbits, hy_s32 parity)
{
    hy_s32   i;
    hy_s32   status;
    struct termios   Opt;

    tcgetattr(fd, &Opt);
    /* set data bits */
    Opt.c_cflag &= ~CSIZE;
    switch (databits) {
    case 7:
        Opt.c_cflag |= CS7;
        break;
    case 8:
        Opt.c_cflag |= CS8;
        break;
    default:
        hy_zlog_info("Unsupported data size\n");
        return HY_FAILTURE;
    }
    /* set stop bits */
    switch (stopbits) {
    case 1:
        Opt.c_cflag &= ~CSTOPB;
        break;
    case 2:
        Opt.c_cflag |= CSTOPB;
        break;
    default:
        hy_zlog_info("Unsupported stop bits\n");
        return  HY_FAILTURE;
    }
    /* set parity */
    switch (parity) {
    case 'n':
    case 'N':
        Opt.c_cflag &= ~PARENB;   /* Clear parity enable */
        Opt.c_iflag &= ~INPCK;     /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        Opt.c_cflag |= (PARODD | PARENB); /* ����Ϊ��Ч��*/
        Opt.c_iflag |= INPCK;             /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        Opt.c_cflag |= PARENB;     /* Enable parity */
        Opt.c_cflag &= ~PARODD;   /* ת��ΪżЧ��*/
        Opt.c_iflag |= INPCK;       /* Disnable parity checking */
        break;
    case 'S':
    case 's':  /*as no parity*/
        Opt.c_cflag &= ~PARENB;
        Opt.c_cflag &= ~CSTOPB;
        break;
    default:
        hy_zlog_info("Unsupported parity\n");
        return HY_FAILTURE;
    }
    /* set as raw work mode */
    Opt.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    Opt.c_oflag  &= ~OPOST;   /*Output*/
    Opt.c_iflag   &=   ~ICRNL;
    Opt.c_iflag   &=   ~IXON;
    Opt.c_iflag   &=   ~IXOFF;
    /* set baud rate */
    for (i= 0;  i < sizeof(speed_arr) / sizeof(hy_s32);  i++) {
        if (speed == name_arr[i]) {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if (status != 0) {
                perror("tcsetattr fd");
                return HY_FAILTURE;
            } else
                hy_zlog_info("the speed is %d.\n", name_arr[i]);
            tcflush(fd, TCIOFLUSH);
        }
    }
    return HY_SUCCESS;
}

/*
*   �򿪴��ں�����
*   ��Ϊ��������
*/
hy_s32 tty_cmd_open(void)
{
    hy_s32 fd_rs422;
    /*��������*/
    fd_rs422 = open(RS422_DEV, O_RDWR|O_NOCTTY|O_NDELAY);
    if (fd_rs422 < 0) {
        hy_zlog_info("open device %s failed.\n", RS422_DEV);
        return HY_FAILTURE;
    } else
    {
        hy_zlog_info("open device %s success.\n", RS422_DEV);
    }

    //Jimmy:�ָ�����Ϊ����״̬
    if(fcntl(fd_rs422, F_SETFL, 0) < 0)
    {
         hy_zlog_info("fcntl device %s failed!\n", RS422_DEV);
          return HY_FAILTURE;
    }

    if (HY_FAILTURE == set_tty_param(fd_rs422, RS422_BAURRATE, RS422_DATABIT, RS422_STOPBIT, 's')) {

        hy_zlog_info("Set %s Error\n", RS422_DEV);
        hy_close_fd(fd_rs422);
        return HY_FAILTURE;

    } else{
        hy_zlog_info("set device %s success.\n", RS422_DEV);
    }
    return fd_rs422;
}

/*******************************************************************
* ���ƣ�                  cmd_recv
* ���ܣ�                ���մ�������
* ��ڲ�����        fd                  :�ļ�������
*                   rcv_buf             :���մ��������ݴ���rcv_buf��������
*                   data_len            :һ֡���ݵĳ���
* ���ڲ�����        ��ȷ����Ϊ���յĳ��ȣ�ʧ�ܷ�����
*******************************************************************/
hy_s32 tty_cmd_recv(hy_s32 fd, hy_s8 *rcv_buf, hy_s32 data_len)
{
    hy_s32 len,fs_sel;
    fd_set fs_read;

    struct timeval time;

    FD_ZERO(&fs_read);
    FD_SET(fd,&fs_read);

    time.tv_sec = 2;
    time.tv_usec = 0;

    //ʹ��selectʵ�ִ��ڵĶ�·ͨ��
    fs_sel = select(fd+1, &fs_read, NULL, NULL, &time);
    if(fs_sel == -1)
    {
        hy_zlog_info("recv cmd form rs422 error.\n");
        return 0;
    }
    else if (fs_sel == 0)
    {
        hy_zlog_info("recv cmd form rs422 time out.\n");
        return 0;
    }
    else
    {
        len = hy_read(fd, rcv_buf, data_len);
        if (len == data_len)
            hy_zlog_info("recv cmd form rs422 success.\n");
        return len;
    }
}



//���������ۼӣ�������λ��ֻ��������ֽڣ������ֽ�ȡ��
static bool get_checksum(hy_u8 *buf, hy_u32 buf_size, hy_u8 *p_checksum)
{
	if (buf != 0)
	{
		hy_u8 *p = buf;
		hy_u32 i;
		hy_u8 checksum = 0x00;
		for (i=0; i<buf_size; i++)
		{
//		    printf("%x \n", *p);
			checksum += (*p);
			//printf("%d \n", *p);
			p++;
		}

//        printf("%x \n", checksum);
		checksum = ~checksum;
		*p_checksum = checksum;
		return true;
	}
	else
	{
		return false;
	}
}


//����ָ���������buf,Ŀǰ�汾�ĺ���ֻ֧��ÿ��������ָ������֧�ֶ���ָ��Ļ�����һ�����������ݲ���
//���õ�ʱ��buf_sizeָ����buf�Ŀ��ô�С
//�Żص�ʱ��buf_size�ᱻ��Ϊ���ݱ��Ĵ�С
void fill_in_rs422_date_buf(hy_u8 *buf, hy_u32 *buf_size, \
                        hy_u8 datagram_type, hy_u8 data_type, hy_u8 data_context)
{
	hy_u8 datagram_start_index_hight = 0x57;
	hy_u8 datagram_start_index_low = 0x50;
	hy_u8 datagram_stop_index = 0x16;

	hy_u16 datagram_context_lenght = 1 + 1 + 1;
	hy_u16 datagram_lenght = datagram_context_lenght + 7;

	hy_u8 *p = buf;//�����ƶ�
	*p = datagram_start_index_hight;
	p++;
	*p = datagram_start_index_low;
	p++;

	hy_u8 *p_context = p;//�������ݵĵ�ַ
	*p = datagram_context_lenght;//��1byte
	p++;
	*p = (datagram_context_lenght/256);//��1byte
	p++;
	*p = datagram_type;//��������
	p++;

	*p = data_type;//С��������
	p++;
	*p = 0x01;//С���ݵĳ���
	p++;
	*p = data_context;//����
	p++;

	//У��
	hy_u8 checksum;
	get_checksum(p_context, datagram_context_lenght + 3, &checksum);
	*p = checksum;
	p++;

	//������
	*p = datagram_stop_index;

	*buf_size = datagram_lenght;

#ifdef HY_DEBUG
	hy_u32 i;
	hy_u8 *p_buf = buf;
	for (i=0; i<*buf_size; i++)
	{
        printf("%x  ", *p_buf);
		p_buf++;
	}
	printf("\n");
#endif

}


//���У���,У��ɹ�����true
bool check_checksum(hy_u8 *buf, hy_u32 buf_size, hy_u8 checksum)
{
	hy_u8 local_checksum;
	if (get_checksum(buf, buf_size, &local_checksum))
	{
		if (local_checksum == checksum)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void restart_horn_power_by_uart(hy_u8 *buf, hy_u32 *buf_size)
{
	hy_u8 dadagram_type = HY_RS422_POWER_MANAGEMENT;
	hy_u8 data_type = HY_RS422_HORN_POWER;
	hy_u8 data_context = POWER_MANAGEMENT_RESTART;

	fill_in_rs422_date_buf(buf, buf_size, dadagram_type, data_type, data_context);
}

void open_horn_power_by_uart(hy_u8 *buf, hy_u32 *buf_size)
{
	hy_u8 dadagram_type = HY_RS422_POWER_MANAGEMENT;
	hy_u8 data_type = HY_RS422_HORN_POWER;
	hy_u8 data_context = POWER_MANAGEMENT_OPEN;

	fill_in_rs422_date_buf(buf, buf_size, dadagram_type, data_type, data_context);
}

void close_horn_power_by_uart(hy_u8 *buf, hy_u32 *buf_size)
{
	hy_u8 dadagram_type = HY_RS422_POWER_MANAGEMENT;
	hy_u8 data_type = HY_RS422_HORN_POWER;
	hy_u8 data_context = POWER_MANAGEMENT_CLOSE;

	fill_in_rs422_date_buf(buf, buf_size, dadagram_type, data_type, data_context);
}

void open_a9_power_by_uart(hy_u8 *buf, hy_u32 *buf_size)
{
	hy_u8 dadagram_type = HY_RS422_POWER_MANAGEMENT;
	hy_u8 data_type = HY_RS422_A9_POWER;
	hy_u8 data_context = POWER_MANAGEMENT_OPEN;

	fill_in_rs422_date_buf(buf, buf_size, dadagram_type, data_type, data_context);
}

void close_a9_power_by_uart(hy_u8 *buf, hy_u32 *buf_size)
{
	hy_u8 dadagram_type = HY_RS422_POWER_MANAGEMENT;
	hy_u8 data_type = HY_RS422_A9_POWER;
	hy_u8 data_context = POWER_MANAGEMENT_CLOSE;

	fill_in_rs422_date_buf(buf, buf_size, dadagram_type, data_type, data_context);
}

void open_ball_camera_power_by_uart(hy_u8 *buf, hy_u32 *buf_size)
{
    hy_u8 dadagram_type = HY_RS422_POWER_MANAGEMENT;
	hy_u8 data_type = HY_RS422_CAMERAL1_POWER;
	hy_u8 data_context = POWER_MANAGEMENT_OPEN;

	fill_in_rs422_date_buf(buf, buf_size, dadagram_type, data_type, data_context);
}

void open_horn_power(hy_s32 fd)
{
    hy_char buf[128] = {0};
    hy_u32 buf_size = 128;
    open_horn_power_by_uart(buf, &buf_size);
    hy_write(fd, buf, buf_size);
}

void close_horn_power(void * p_fd)
{
    hy_s32 fd = *((hy_s32 *)p_fd);
    hy_char buf[128] = {0};
    hy_u32 buf_size = 128;
    close_horn_power_by_uart(buf, &buf_size);
    hy_write(fd, buf, buf_size);
}

void open_a9_power(hy_s32 fd)
{
    hy_char buf[128] = {0};
    hy_u32 buf_size = 128;
    open_a9_power_by_uart(buf, &buf_size);
    hy_write(fd, buf, buf_size);
}

void close_a9_power(hy_s32 fd)
{
    hy_char buf[128] = {0};
    hy_u32 buf_size = 128;
    close_a9_power_by_uart(buf, &buf_size);
    hy_write(fd, buf, buf_size);
}

void open_ball_camera_power(hy_s32 fd)
{
    hy_char buf[128] = {0};
    hy_u32 buf_size = 128;
    open_ball_camera_power_by_uart(buf, &buf_size);
    hy_write(fd, buf, buf_size);
}

