/*************************************
 *3G模块工作状态上传
 *date:     2015.1.5
 *changelog:
 *
 *************************************/

#ifndef __HY_CDMA_H__
#define __HY_CDMA_H__
typedef struct
{
    char 	identifier[16];//ttyS0
    int 	baud;
    int 	data_bits;
    int 	stop_bits;
    char 	parity;
} serial_info_t;

int cdma_state_monitor_thread_start();
void *cdma_state_monitor_thread(void *s);

int set_serial_para(const char *device, int baud, char parity, int data_bit, int stop_bit);
int init_serial(serial_info_t *serial);
int cdma_state_send_at_cmd(int serial_fd, char *cmd);
int cdma_state_read_uart(int serial_fd,char reply[]);
int cdma_state_get_sysinfo(int serial_fd);
int cdma_state_get_cdma1x_csq(int serial_fd);
int cdma_state_get_cdma3g_csq(int serial_fd);
int cdma_state_get_cdma4g_csq(int serial_fd);
#endif

