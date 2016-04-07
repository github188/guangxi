/******************** (C) COPYRIGHT 2015 HY ********************
* File Name          :
* Author             :
* Revision           :
* Description        :���̣߳���ʼ�������������������߳�
* Change log		 :  Jimmy2015-7-13
********************* (C) COPYRIGHT 2015 HY *******************/
#include "global_config.h"
#include "hy_base_config.h"
#include "hy_debug.h"
#include "hy_stream_ctrl.h"

#include "hy_session_handler.h"
#include "hy_ptz_ctrl.h"
#include "hy_file_manage.h"

#include "hy_device_status.h"
#include "decode.h"
#include "hy_signal_handler.h"
#include "hy_cdma_status.h"
#include "hy_flow_moniter.h"
#include "hy_watchdog.h"
#include "hy_timer.h"
#include "hy_deque.h"
#include "hy_zlog.h"
#include "hy_mk60_ctrl.h"

int main()
{

    /*��ʼ��zlog��־��*/
    if(init_zlog())
    {
        return -1;
    }

    hy_zlog_info("version %f\n", version);
    /*����NVR.INI�����ļ�*/
    parse_nvr_ini_file(&g_hy_nvr_info);
    /*�����澯����*/
    parse_detect_info_file(&g_hy_detect_info_t);
    /*��������ַ*/
    hy_init_rtsp_addrs();

    hy_signal_handler_init();

    /*��ʼ��������Ϣ����*/
    data_mesg_deque = (hy_mesg_deque_t *)hy_malloc(sizeof(hy_mesg_deque_t));
    if (!data_mesg_deque)
    {
        return -1;
    }
    if (hy_init_mesg_deque_t(data_mesg_deque))
    {
        return -1;
    }

    /*���ݶ��д����߳�*/
    hy_start_file_manage_thread();

    /*��ʼ��Timer*/
    hy_init_timer_mgt();
    hy_start_timer_thread();

    hy_sig_session_handle_thread_start();

    hy_start_device_status_thread();

    cdma_state_monitor_thread_start();

    flow_monitor_handle_thread_start();
    start_recv_order_from_mk60_thread();
    hy_start_watchdog_thread();

    //hy_sleep(30);
    ptz_ctrl_init();
    hy_start_decode_thread();
    while(1)
    {
        hy_sleep(10);
    }
}
