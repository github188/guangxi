#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "hy_debug.h"
#include "hy_base_config.h"
#include "hy_io_utils.h"
#include "hy_session_handler.h"//for socket fd release
#include "hy_signal_handler.h"
#include "decode.h"
#include "hy_zlog.h"


static void signal_handle_sigquit()
{
    hy_zlog_info("handle sigquit.\n");
}

static void signal_handle_sigint()
{
    hy_zlog_info("handle sigint.\n");
    /*close sig socket*/
    if (hy_sig_session.b_is_connected)
    {
        hy_zlog_info("close sig fd.\n");
        hy_sig_session.b_is_connected = HY_FALSE;
        hy_close_fd(hy_sig_session.fd);
    }

    hy_stop_decode_thread();
    exit(-1);
}

static void signal_handler_cb(int signal_num)
{
    switch (signal_num)
    {
    case SIGQUIT:
        hy_zlog_info("captured SIGQUIT.\n");
        signal_handle_sigquit();
        break;
    case SIGINT:
        hy_zlog_info("captured SIGINT.\n");
        signal_handle_sigint();
        break;
    case SIGKILL://it can never be capured.
        hy_zlog_info("captured SIGKILL.\n");
        break;
    case SIGPIPE:
        hy_zlog_info("captured SIGPIPE. Sokcet may be disconnected.\n");
        break;
    case SIGABRT:
        hy_zlog_info("captured SIGABRT.\n");
        break;
    default:
        break;
    }
}

void hy_signal_handler_init()
{
    signal(SIGQUIT, signal_handler_cb);
    signal(SIGINT, signal_handler_cb);
    signal(SIGKILL, signal_handler_cb);
    signal(SIGPIPE, signal_handler_cb);
}
