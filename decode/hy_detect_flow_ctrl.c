#include "hy_detect_flow_ctrl.h"
#include "global_config.h"
#include "hy_ptz_ctrl.h"
#include "hy_zlog.h"


//开启算法前要进行的PTZ操作
hy_s32 hy_sig_pre_detect()
{
    simple_ptz_goto_preset(g_hy_detect_info_t.cur_channel, 1);
}
hy_s32 hy_sig_handle_ptz_manu_req()
{
    g_hy_detect_info_t.starting = HY_FALSE;

    return 0;
}