#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <error.h>

#include "hy_session_handler.h"
#include "global_config.h"
#include "hy_io_utils.h"
#include "hy_protocl.h"
#include "hy_ptz_ctrl.h"
#include "hy_stream_ctrl.h"
#include "hy_file_manage.h"
#include "hy_flow_moniter.h"

#include "hy_debug.h"
#include "hy_base_config.h"
#include "hy_update.h"
#include "hy_timer.h"
#include "hy_detect_flow_ctrl.h"
#include "hy_zlog.h"

#include "hy_serial_ctrl_protocol.h"
#include "hy_device_status.h"
#include "hy_deque.h"
#include "hy_mk60_ctrl.h"

//#define OLD_IPC 1 //IPC admin:12345
#define NEW_IPC 1 //IPC admin:huiyuan123

//应该弄一个全局的，连接相关的对象(我提供函数就可以了，我不需要，把内容放出来)
hy_session_t hy_sig_session;

//read 和 write都可以用一个简单的函数，可以提供给其他的线程调用，而且应该是线程安全的
hy_s32 real_time_fd;//这个就是你问那个参数 设置的是全局变量
static hy_s32 hy_sig_handle_ptz_auto_req();
static void hy_sig_seesion_init()
{
    strcpy(hy_sig_session.ip, g_hy_nvr_info.hy_nvr_server_info.server_ip);
    hy_sig_session.port = g_hy_nvr_info.hy_nvr_server_info.server_ctl_port;
    hy_sig_session.fd = 0;
    hy_sig_session.b_thread_running = HY_FALSE;
    hy_sig_session.b_is_connected = HY_FALSE;
    hy_sig_session.pid = 0;
}

static hy_s32 hy_sig_send_login_meg(hy_s32 fd)
{
    /*发送登录信息及dvr基本信息*/
    HY_LOGIN login = {"hy", "Mv"};
    Dvr_INFO basic_info;
    basic_info.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    basic_info.ipc_num = g_hy_nvr_info.hy_nvr_general_info.max_chan;
    basic_info.soft_version = version;/*ToDo:现在先写死，以后，每一个版本提供一个全局变量*/

    hy_s32 ret = hy_write(fd, &login, sizeof(HY_LOGIN));
    if (ret == -1)
    {
        hy_zlog_info("write to server failed.\n");
        return -1;
    }
    ret = hy_write(fd, &basic_info, sizeof(Dvr_INFO));
    if (ret == -1)
    {
        hy_zlog_info("write to server failed.\n");
        return -1;
    }

    return 0;
}

static hy_s32 hy_sig_handle_set_realtime(hy_s32 fd, REAL_TIME *hy_time)
{
    /* 更新系统时钟 */
    char cmd[128] = {0};
    sprintf(cmd, "date %02d%02d%02d%02d%d.%02d",hy_time->month,hy_time->day,\
            hy_time->hour, hy_time->min, hy_time->year, hy_time->sec);
    system(cmd);
    /* 更新rtc时钟 */
    system("hwclock -w");

    time_t now_time;
    time(&now_time);
    hy_zlog_info("set time to %s",ctime(&now_time));

    M_HY_HEADER resp;
    resp.flag = DEFAULT_FLAG;
    resp.command = DTOA_SET_ARM_REAL_TIME;
    resp.len = 0;
    hy_s32 ret;
    ret = hy_write(fd, &resp, sizeof(resp));
    if (ret == -1)
    {
        return -1;
    }

    return 0;
}

hy_s32 hy_sig_set_video_config(hy_s32 fd, hy_s32 config)
{
    printf("set video config req comming\n");
    printf("%d\n", config);
    hy_char *p_ip = g_hy_nvr_info.hy_nvr_channel_info[g_hy_detect_info_t.cur_channel].ip;

	switch(config)
    {
    case SET_352_288_6:
    {
        char set_352_288_6[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8;\r\naction=http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\r\nHost: 172.16.8.245\r\nContent-Length: 1976\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">P/fFFIp2VjfaZ6ZnzuAaURUFlH4=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">TFRVLLCdY0mtWzj9b/9/eEgAAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-02-06T03:50:44.436Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>352</Width><Height>288</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">3</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>6</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>1024</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>12</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
        video_config_set_encoder(p_ip, set_352_288_6, sizeof(set_352_288_6));
    }
    break;

    case SET_704_576_6:
    {
        char set_704_576_6[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8;\r\naction=http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\r\nHost: 172.16.8.245\r\nContent-Length: 1975\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">P/fFFIp2VjfaZ6ZnzuAaURUFlH4=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">TFRVLLCdY0mtWzj9b/9/eEgAAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-02-06T03:50:44.436Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>704</Width><Height>576</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">3</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>6</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>512</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>18</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
        video_config_set_encoder(p_ip, set_704_576_6, sizeof(set_704_576_6));
    }
    break;

    case SET_704_576_10:
    {
        char set_704_576_10[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8;\r\naction=http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\r\nHost: 172.16.8.245\r\nContent-Length: 1977\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">P/fFFIp2VjfaZ6ZnzuAaURUFlH4=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">TFRVLLCdY0mtWzj9b/9/eEgAAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-02-06T03:50:44.436Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>704</Width><Height>576</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">3</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>10</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>1024</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>50</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
        video_config_set_encoder(p_ip, set_704_576_10, sizeof(set_704_576_10));
    }
    break;

    case SET_704_576_15:
    {
        char set_704_576_15[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8;\r\naction=http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\r\nHost: 172.16.8.245\r\nContent-Length: 1977\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">P/fFFIp2VjfaZ6ZnzuAaURUFlH4=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">TFRVLLCdY0mtWzj9b/9/eEgAAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-02-06T03:50:44.436Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>704</Width><Height>576</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">3</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>15</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>1024</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>50</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
        video_config_set_encoder(p_ip, set_704_576_15, sizeof(set_704_576_15));
    }
    break;

    case SET_704_576_20:
    {
        char set_704_576_20[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8;\r\naction=http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\r\nHost: 172.16.8.245\r\nContent-Length: 1977\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">P/fFFIp2VjfaZ6ZnzuAaURUFlH4=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">TFRVLLCdY0mtWzj9b/9/eEgAAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-02-06T03:50:44.436Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>704</Width><Height>576</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">3</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>20</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>1024</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>50</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
        video_config_set_encoder(p_ip, set_704_576_20, sizeof(set_704_576_20));
    }
    break;

    #ifdef NEW_IPC
	case SET_704_576_6_12_M_1024:
    {
		char set_704_576_6_12_m_1024[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\"\r\nHost: 172.16.8.245\r\nContent-Length: 1976\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">jpQOaTYifffW2QBREkhWX8O8hf8=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">q1LylQ+7vESuS6BHTEYHsM4AAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-08-05T06:27:21.660Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>704</Width><Height>576</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">3</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>6</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>1024</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>12</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
        video_config_set_encoder(p_ip, set_704_576_6_12_m_1024, sizeof(set_704_576_6_12_m_1024));
    }
    break;

	case SET_704_576_6_12_3L_512:
    {
		char set_704_576_6_12_3l_512[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\"\r\nHost: 172.16.8.245\r\nContent-Length: 1975\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">l5ETitsNDwL8AN24qp9ZBexMnW0=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">q1LylQ+7vESuS6BHTEYHsOcAAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-08-05T06:35:29.019Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>704</Width><Height>576</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">0</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>6</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>512</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>12</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
        video_config_set_encoder(p_ip, set_704_576_6_12_3l_512, sizeof(set_704_576_6_12_3l_512));
    }
    break;

	case SET_352_288_6_12_M_256:
    {
		char set_352_288_6_12_m_256[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\"\r\nHost: 172.16.8.245\r\nContent-Length: 1975\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">jpQOaTYifffW2QBREkhWX8O8hf8=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">q1LylQ+7vESuS6BHTEYHsM4AAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-08-05T06:27:21.660Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>352</Width><Height>288</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">3</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>6</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>256</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>12</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
        video_config_set_encoder(p_ip, set_352_288_6_12_m_256, sizeof(set_352_288_6_12_m_256));
    }
    break;

	case SET_704_576_6_12_2H_1024:
    {
		char set_704_576_6_12_2h_1024[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\"\r\nHost: 172.16.8.245\r\nContent-Length: 1976\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">jpQOaTYifffW2QBREkhWX8O8hf8=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">q1LylQ+7vESuS6BHTEYHsM4AAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-08-05T06:27:21.660Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>704</Width><Height>576</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">5</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>6</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>1024</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>12</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
        video_config_set_encoder(p_ip, set_704_576_6_12_2h_1024, sizeof(set_704_576_6_12_2h_1024));
    }
    break;
    #endif

    #ifdef OLD_IPC
		case SET_704_576_6_12_M_1024:
		{
			char set_704_576_6_12_m_1024[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\"\r\nHost: 172.16.8.245\r\nContent-Length: 1976\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">VGOo5H3xSHCYFCzSQZCm05ZiKsk=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">q1LylQ+7vESuS6BHTEYHsJQBAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-08-05T07:35:20.724Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>704</Width><Height>576</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">3</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>6</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>1024</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>12</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
			video_config_set_encoder(p_ip, set_704_576_6_12_m_1024, sizeof(set_704_576_6_12_m_1024));
		}
		break;

		case SET_704_576_6_12_3L_512:
		{
			char set_704_576_6_12_3l_512[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\"\r\nHost: 172.16.8.245\r\nContent-Length: 1975\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">VGOo5H3xSHCYFCzSQZCm05ZiKsk=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">q1LylQ+7vESuS6BHTEYHsJQBAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-08-05T07:35:20.724Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>704</Width><Height>576</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">0</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>6</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>512</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>12</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
			video_config_set_encoder(p_ip, set_704_576_6_12_3l_512, sizeof(set_704_576_6_12_3l_512));
		}
		break;

		case SET_352_288_6_12_M_256:
		{
			char set_352_288_6_12_m_256[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\"\r\nHost: 172.16.8.245\r\nContent-Length: 1975\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">VGOo5H3xSHCYFCzSQZCm05ZiKsk=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">q1LylQ+7vESuS6BHTEYHsJQBAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-08-05T07:35:20.724Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>352</Width><Height>288</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">3</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>6</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>256</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>12</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
			video_config_set_encoder(p_ip, set_352_288_6_12_m_256, sizeof(set_352_288_6_12_m_256));
		}
		break;

		case SET_704_576_6_12_2H_1024:
		{
			char set_704_576_6_12_2h_1024[] = "POST /onvif/Media HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\"\r\nHost: 172.16.8.245\r\nContent-Length: 1976\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">VGOo5H3xSHCYFCzSQZCm05ZiKsk=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">q1LylQ+7vESuS6BHTEYHsJQBAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2015-08-05T07:35:20.724Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_2\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_2</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>704</Width><Height>576</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">5</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>6</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>1024</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>12</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
			video_config_set_encoder(p_ip, set_704_576_6_12_2h_1024, sizeof(set_704_576_6_12_2h_1024));
		}
		break;
	#endif

    default:
        hy_zlog_info("undefined video config level.");
        break;
    }

    M_HY_HEADER head;
    head.flag = DEFAULT_FLAG;
    head.command = DTOA_SET_VIDEO_CONFIG;
    head.len = 0;
    hy_write(fd, &head, sizeof(M_HY_HEADER));

    return 0;
}

static hy_s32 hy_sig_handle_heart_beat(hy_s32 fd)
{
    M_HY_HEADER head;
    head.flag = DEFAULT_FLAG;
    head.command = DTOA_KEEP_HEART_BEAT;
    head.len = 0;
    hy_write(fd, &head, sizeof(M_HY_HEADER));
    return 0;
}

static hy_s32 hy_sig_handle_send_real_stream_rsp(REAL_TIME_REQUEST *ipc, hy_u8 stream)//default success
{
    DATA_STRUCT data;
    data.flag = DEFAULT_FLAG;
    data.type = DTOA_REAL_TIME_STREAM_INFO;
    data.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    data.stream.ipc = ipc->ipc;
    data.stream.stream = stream;
    data.stream.error = 0;//错误码，0无错误，1 IPC不存在，2 IPC ping不通， 3 正在播放， 4 推流中断 ，5待定
    //TODO:connect
    hy_s32 fd = hy_tcp_connect(g_hy_nvr_info.hy_nvr_server_info.server_ip, g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    if(fd == -1)
    {
    	hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n", g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    	return -1;
    }
    else
    {
           hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    }

    //TODO:send
    hy_write(fd, &data, sizeof(data));
    hy_close_fd(fd);
    return 0;
}


/*
注意提供rtsp源地址和发布地址
*/
static hy_s32 hy_sig_handle_realtime_req_without_power(hy_s32 fd, REAL_TIME_REQUEST *ipc)
{
    //send rsp
    M_HY_HEADER head;
    head.flag = DEFAULT_FLAG;
    head.command = DTOA_REAL_TIME_RSP;
    head.len = 0;
    hy_write(fd, &head, sizeof(M_HY_HEADER));


    DATA_STRUCT data;
    data.flag = DEFAULT_FLAG;
    data.type = DTOA_REAL_TIME_STREAM_INFO;
    data.dvr = g_hy_nvr_info.hy_nvr_general_info.device_id;
    data.stream.ipc = ipc->ipc;
    data.stream.stream = 2;
    data.stream.error = 2;//错误码，0无错误，1 IPC不存在，2 IPC ping不通， 3 正在播放， 4 推流中断 ，5待定
    //TODO:connect
    hy_s32 fd1 = hy_tcp_connect(g_hy_nvr_info.hy_nvr_server_info.server_ip, g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    if(fd1 == -1)
    {
    	hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n", g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    	return -1;
    }
    else
    {
           hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",g_hy_nvr_info.hy_nvr_server_info.server_ip, \
                   g_hy_nvr_info.hy_nvr_server_info.server_data_port);
    }

    //TODO:send
    hy_write(fd1, &data, sizeof(data));
    hy_close_fd(fd1);

    return 0;
}

/*
注意提供rtsp源地址和发布地址
*/
static hy_s32 hy_sig_handle_realtime_req(hy_s32 fd, REAL_TIME_REQUEST *ipc)
{
    //TODO:get source_addr by channel

    //TODO:get publis addr
//    char source_addr[128] = "rtsp:/admin:huiyuan123@172.16.8.245:554/Streaming/Channels/3?transportmode=unicast&profile=Profile_3";
//    char publish_addr[64];
//    sprintf(publish_addr, "rtsp://%s/%05d-%02d-%02d.sdp", g_hy_nvr_info.hy_nvr_server_info.server_ip, ipc->dvr, ipc->ipc, 2);
    hy_simple_stream_start_realplay(g_hy_nvr_info.hy_nvr_server_info.server_ip, ipc->dvr, ipc->ipc, 2);
    //hy_simple_stream_start_realplay(source_addr, publish_addr);

    //send rsp
    M_HY_HEADER head;
    head.flag = DEFAULT_FLAG;
    head.command = DTOA_REAL_TIME_RSP;
    head.len = 0;
    hy_write(fd, &head, sizeof(M_HY_HEADER));

    //send rsp
    hy_sig_handle_send_real_stream_rsp(ipc, 2);
    hy_simple_debug();
    return 0;
}

static hy_sig_handle_realtime_stop_req(hy_s32 fd, REAL_TIME_REQUEST *ipc)
{
    //close
    hy_simple_debug();
    hy_simple_stream_stop_realplay();
    if (ipc->ipc != 1)
    {
        send_close_camera_package_to_mk60(NULL);
        hy_zlog_info("send_close_camera_package_to_mk60");
    }

    hy_sig_handle_ptz_auto_req();//停止播放后自动开始算法

    hy_msleep(10);
    //send message
    //send rsp
    M_HY_HEADER head;
    head.flag = DEFAULT_FLAG;
    head.command = DTOA_REAL_STREAM_STOP;
    head.len = 0;
    hy_write(fd, &head, sizeof(M_HY_HEADER));
    return 0;
}

static hy_s32 hy_sig_handle_ptz(hy_s32 fd, SYS_PTZ *ptz_cmd)
{
    hy_u8 channel = ptz_cmd->ipc;
    hy_u8 zoom = ptz_cmd->zoom;
    hy_f32 step = ptz_cmd->spead / 256.0;
    hy_u32 cmd = ptz_cmd->cmd;
    switch (cmd)
    {
    case MOVE_UP:
        hy_zlog_info("ptz MOVE_UP, step %f channel %d.\n", step, channel);
        simple_ptz_move_up(channel, step);
        break;
    case MOVE_L_UP:
        hy_zlog_info("ptz MOVE_L_UP, step %f channel %d.\n", step, channel);
        simple_ptz_move_left_up(channel, step);
        break;
    case MOVE_R_UP:
        hy_zlog_info("ptz MOVE_R_UP, step %f channel %d.\n", step, channel);
        simple_ptz_move_right_up(channel, step);
        break;
    case MOVE_DOWN:
        hy_zlog_info("ptz MOVE_DOWN, step %f channel %d.\n", step, channel);
        simple_ptz_move_down(channel, step);
        break;
    case MOVE_L_DOWN:
        hy_zlog_info("ptz MOVE_L_DOWN, step %f channel %d.\n", step, channel);
        simple_ptz_move_left_down(channel, step);
        break;
    case MOVE_R_DOWN:
        hy_zlog_info("ptz MOVE_R_DOWN, step %f channel %d.\n", step, channel);
        simple_ptz_move_right_down(channel, step);
        break;
    case MOVE_L:
        hy_zlog_info("ptz MOVE_L, step %f channel %d.\n", step, channel);
        simple_ptz_move_left(channel, step);
        break;
    case MOVE_R:
        hy_zlog_info("ptz MOVE_R, step %f channel %d.\n", step, channel);
        simple_ptz_move_right(channel, step);
        break;
    case VISU_CAM_ZOOM:
        hy_zlog_info("ptz VISU_CAM_ZOOM, in or out %d channel %d.\n", zoom, channel);
        if (zoom == 0x01)
        {
            simple_ptz_zoom_in(channel, 0.3);
        }
        else
        {
            simple_ptz_zoom_out(channel, 0.3);
        }
        break;
    case VISU_CAM_ZOOM_STOP:
        hy_zlog_info("ptz VISU_CAM_ZOOM_STOP\n");
        simple_ptz_move_stop(channel);
        break;
    case MOVE_STOP:
        hy_zlog_info("ptz MOVE_STOP.\n");
        simple_ptz_move_stop(channel);
        break;
    case VISU_CAM_SWITH:
        hy_zlog_info("ptz VISU_CAM_SWITH.\n");
        break;
    default:
        hy_zlog_info("unknown ptz cmd is 0X%04x.\n", cmd);
        break;

    }
    return 0;
}

static hy_s32 hy_sig_handle_update(hy_s32 fd)
{
	int ret;
	pthread_t tid;
    M_HY_HEADER resp;


    if (a9_update_flag == 0)
    {
        ret = pthread_create(&tid, NULL, a9_handle_update_thread, NULL);
        if (ret != 0)
        {
            hy_zlog_info("create a9_handle_update thread failed.\n");
    		a9_update_flag = 0;
        }
    }

	resp.flag = DEFAULT_FLAG;
    resp.command = DTOA_SYS_UPDATE;
    resp.len = ret;

    hy_write(fd, &resp, sizeof(M_HY_HEADER));

    return 0;
}

static hy_s32 hy_sig_handle_reboot(hy_s32 fd)
{
    system("reboot");
    return 0;
}
static hy_s32 hy_sig_handle_preset(hy_s32 fd, PRESET_MANAGEMENT *preset_cmd)
{
    hy_u32 channel = preset_cmd->ipc;
    hy_u32 cmd = preset_cmd->cmd;
    hy_u32 preset_id = preset_cmd->preset.preset_id;

    if (cmd == SET_PRESET)
    {
        simple_ptz_set_preset(channel, preset_id);
    }
    else if(cmd == CLEAR_PRESET)
    {
        //simple_ptz_clear_preset(channel, preset_id);
    }
    else if (cmd == GOTO_PRESET)
    {
        simple_ptz_goto_preset(channel, preset_id);
    }
    return 0;
}

 static hy_s32 hy_sig_handle_ping_camera(hy_s32 fd)
{
    /*ping curent playing channel*/
    M_HY_HEADER head;
    head.flag = DEFAULT_FLAG;
    head.command = DTOA_PING_CAMERA;
    head.len = 1;//0disable 1able
    hy_write(fd, &head, sizeof(M_HY_HEADER));
    return 0;
}

static hy_s32 hy_sig_start_detect()
{
    g_hy_detect_info_t.starting = HY_TRUE;
}

static hy_s32 hy_sig_handle_ptz_auto_req()
{
    hy_sig_pre_detect();
    hy_timer_t * p_open_detect = hy_init_timer(HY_SINGLE_TIRGGER, hy_sig_start_detect, NULL);
    hy_start_timer(p_open_detect, 2);//两秒后开启算法
    return 0;
}

void hy_sig_handle_set_detect_area(hy_s32 fd, hy_channel_detect_area_t *p_area)
{
    M_HY_HEADER head;
    head.flag = DEFAULT_FLAG;
    head.command = DTOA_SET_DETECT_AREA;
    head.len = 0;
    hy_write(fd, &head, sizeof(M_HY_HEADER));

    hy_zlog_info("channel:%d\t rect_count:%d\n", p_area->channel, p_area->rectcount);
    if (p_area->channel > 1 || p_area->rectcount > 16)//只有枪机作为检测
    {
        hy_zlog_info("set detect broken because of harge channel or rect_count\n");
        return ;
    }
    memcpy(&(g_hy_detect_info_t.channel_detect_area[p_area->channel - 1]), p_area, sizeof(hy_channel_detect_area_t));
    //写入到配置文件
    update_detect_info_file(&g_hy_detect_info_t);
}

static hy_s32 hy_sig_handle_get_detect_area(hy_s32 fd)
{
    M_HY_HEADER head;
    head.flag = DEFAULT_FLAG;
    head.command = DTOA_GET_DETECT_AREA;
    head.len = sizeof(hy_channel_detect_area_t);
    hy_write(fd, &head, sizeof(M_HY_HEADER));

    hy_channel_detect_area_t area;
    memcpy(&area, &(g_hy_detect_info_t.channel_detect_area[g_hy_detect_info_t.cur_channel - 1]), sizeof(hy_channel_detect_area_t));
    int i;
    for (i=0; i<area.rectcount; i++)
    {
        hy_zlog_info("detect area  left%d top%d right%d bottom%d\n", area.rect[i].left, area.rect[i].top, area.rect[i].right, area.rect[i].bottom);
    }
    hy_write(fd, &area, sizeof(area));

    return 0;
}

static hy_s32 hy_sig_handle_picture_record_task(hy_s32 fd, picture_record_t *task)
{
    M_HY_HEADER head;
    head.flag = DEFAULT_FLAG;
    head.command = DTOA_PICTURE_RECORD_TASK;
    head.len = 0;
    hy_write(fd, &head, sizeof(M_HY_HEADER));

    pthread_t id;
    picture_record_t *p_task = (picture_record_t *)hy_malloc(sizeof(picture_record_t));
    memcpy(p_task, task, sizeof(picture_record_t));
    hy_s32 ret = pthread_create(&id, NULL, hy_data_handle_pic_record_task, p_task);
    if ( ret < 0 )
    {
        hy_zlog_info("create pic record task thread failed.\n");
        return -1;
    }

    return 0;
}

static hy_s32 hy_sig_handle_video_record_task(hy_s32 fd, video_record_t *task)
{
    M_HY_HEADER head;
    head.flag = DEFAULT_FLAG;
    head.command = DTOA_VIDEO_RECORD_TASK;
    head.len = 0;
    hy_write(fd, &head, sizeof(M_HY_HEADER));

    pthread_t id;
    video_record_t *p_task = (video_record_t *)hy_malloc(sizeof(video_record_t));
    memcpy(p_task, task, sizeof(video_record_t));
    hy_s32 ret = pthread_create(&id, NULL, hy_data_handle_video_record_task, p_task);
    if ( ret < 0 )
    {
        hy_zlog_info("create pic record task thread failed.\n");
        return -1;
    }

    return 0;
}

static int hy_sig_handle_get_flow_value_req(hy_s32 fd, REAL_TIME *time)
{
    int ret;
    pthread_t tid;

    REAL_TIME *newtime = (REAL_TIME *)malloc(sizeof(REAL_TIME));
    memmove(newtime, time, sizeof(REAL_TIME));

    ret = pthread_create(&tid, NULL, order_handle_get_flow_value_req_thread, newtime);
    if (ret != 0)
    {
        hy_zlog_info("pthread_create mk60_handle_update thread fail\n");
        return -1;
    }

    M_HY_HEADER resp;
    resp.flag = DEFAULT_FLAG;
    resp.command= DTOA_MOBILE_FLOW_VALUE;
    resp.len = 0;

    hy_write(fd, &resp, sizeof(resp));
    return 0;
}

/**********************************
*Name:hy_sig_cmd_handler
*Desc:
*Param:
*Return:-1 网络断开
*Author:Jimmy
*Create Time:2015-7-14
*ChangeLog:
**********************************/
static hy_s32 hy_sig_cmd_handler(hy_s32 fd, hy_mesg_buf_t *mesg_buf)
{
    M_HY_HEADER *cmd_head = &mesg_buf->head;
    REAL_TIME_REQUEST *req;
    SYS_PTZ *ptz;
    REAL_TIME *hy_time;
    PRESET_MANAGEMENT *preset_cmd;
    hy_channel_detect_area_t *p_area;
    picture_record_t *p_picture_record;
    video_record_t *p_video_record;
    hy_s32 *p_video_config;

    if (cmd_head->flag != DEFAULT_FLAG)
    {
        hy_zlog_info("cmd_head's flag is not corect.\n");
        hy_sig_session.b_is_connected = HY_FALSE;
        return -1;
    }

    switch(cmd_head->command)
    {
    case ATOD_SET_ARM_REAL_TIME://服务器时间
         hy_zlog_info("ATOD_SET_ARM_REAL_TIME.\n");
         hy_time = (REAL_TIME *)mesg_buf->mesg_context;
         hy_sig_handle_set_realtime(fd, hy_time);
         break;

    case ATOD_KEEP_HEART_BEAT://心跳包
         hy_zlog_info("ATOD_KEEP_HEART_BEAT.\n");
         hy_sig_handle_heart_beat(fd);
         break;

    case ATOD_REAL_TIME_REQUEST://实时流//我们是在这里嵌入的那个线程  因为我怕在PING的时候REQ变化在这里设置了个REQ1传入了线程
         hy_zlog_info("ATOD_REAL_TIME_REQUEST.\n");
         //Jimmy modify on 20151125
         REAL_TIME_REQUEST *req1 = (REAL_TIME_REQUEST *)hy_malloc(sizeof(REAL_TIME_REQUEST));
         memcpy(req1, mesg_buf->mesg_context, sizeof(REAL_TIME_REQUEST));
         pthread_attr_t patr;
         pthread_t tid1;
         pthread_attr_init(&patr);
         pthread_attr_setdetachstate(&patr, PTHREAD_CREATE_DETACHED);
         int ret;
         real_time_fd=fd;
         ret = pthread_create(&tid1, &patr, real_time_stream_request_thread, req1);
         if (ret != 0)
         {
           hy_zlog_info("can't creat stream pthread.\n");
         }
         break;

    case ATOD_REAL_STREAM_STOP://实时流
         hy_zlog_info("ATOD_REAL_STREAM_STOP.\n");
         req = (REAL_TIME_REQUEST *)mesg_buf->mesg_context;
         hy_sig_handle_realtime_stop_req(fd, req);
         break;

    case ATOD_SYS_PTZ://PTZ
         //hy_zlog_info("ATOD_SYS_PTZ.\n");
         ptz = (SYS_PTZ *)mesg_buf->mesg_context;
         hy_sig_handle_ptz(fd, ptz);
         break;

    case ATOD_SYS_UPDATE://程序升级
         hy_zlog_info("ATOD_SYS_UPDATE.\n");
         hy_sig_handle_update(fd);
         break;

    case ATOD_PTZ_MANU://ptz手动控制,关闭算法
         hy_zlog_info("ATOD_PTZ_MANU.\n");
         hy_sig_handle_ptz_manu_req();
         break;

    case ATOD_PTZ_AUTO://ptz自动控制，打开算法
         hy_zlog_info("ATOD_PTZ_AUTO.\n");
         hy_sig_handle_ptz_auto_req();
         break;

    case ATOD_A9_RESTART://A9手动重启
         hy_zlog_info("ATOD_A9_RESTART.\n");
         hy_sig_handle_reboot(fd);
         break;

    case ATOD_MANAGE_PRESET://预置点操作
         hy_zlog_info("ATOD_MANAGE_PRESET.\n");
         preset_cmd = (PRESET_MANAGEMENT *)mesg_buf->mesg_context;
         hy_sig_handle_preset(fd, preset_cmd);
         break;

    case ATOD_MOBILE_FLOW_VALUE: //3G流量
         hy_zlog_info("ATOD_GET_FLOW_VALUE.\n");
         hy_time = (REAL_TIME *)mesg_buf->mesg_context;
         hy_sig_handle_get_flow_value_req(fd, hy_time);
         break;
    case ATOD_PING_CAMERA:
         hy_zlog_info("ATOD_PING_CAMERA.\n");
         hy_sig_handle_ping_camera(fd);
         break;
    case ATOD_SET_DETECT_AREA:
         hy_zlog_info("ATOD_SET_DETECT_AREA.\n");
         p_area = (hy_channel_detect_area_t *)mesg_buf->mesg_context;
         hy_sig_handle_set_detect_area(fd, p_area);
         break;
    case ATOD_GET_DETECT_AREA:
         hy_zlog_info("ATOD_GET_DETECT_AREA.\n");
         hy_sig_handle_get_detect_area(fd);
         break;
	case ATOD_SET_VIDEO_CONFIG://设置视频清晰度
	     hy_zlog_info("ATOD_SET_VIDEO_CONFIG.\n");
	     p_video_config = (hy_s32 *)mesg_buf->mesg_context;
         //hy_sig_set_video_config(fd, *p_video_config);
         break;
	case ATOD_PICTURE_RECORD_TASK://计划录像任务
	     hy_zlog_info("ATOD_PICTURE_RECORD_TASK.\n");
	     p_picture_record = (picture_record_t *)mesg_buf->mesg_context;
         hy_sig_handle_picture_record_task(fd, p_picture_record);
		 break;
	case ATOD_VIDEO_RECORD_TASK://计划截图任务
	     hy_zlog_info("ATOD_VIDEO_RECORD_TASK.\n");
	     p_video_record = (video_record_t *)mesg_buf->mesg_context;
	     hy_sig_handle_video_record_task(fd, p_video_record);
	     break;
    default:
        hy_zlog_info("unsporting request, cmd is 0X%08x.\n", cmd_head->command);
        break;
    }

}
void *real_time_stream_request_thread(void *s)
{

    REAL_TIME_REQUEST *req1 = (REAL_TIME_REQUEST *)s;

    hy_zlog_info("open channel %d ", req1->ipc);
    if (req1->ipc != 1)
    {
        //开球机电源
        send_open_camera_package_to_mk60();
        hy_zlog_info("send_open_camera_package_to_mk60");

        if (device_net_status_judge(g_hy_nvr_info.hy_nvr_channel_info[req1->ipc - 1].ip) != 0)//不通的情况
        {
            hy_sig_handle_realtime_req_without_power(real_time_fd, s);
        }
        else//通的情况
        {
            hy_sig_handle_realtime_req(real_time_fd, s);
        }
    }
    else if (req1->ipc == 1)
    {
        hy_sig_handle_realtime_req(real_time_fd, s);
    }

    hy_free(s);
}

void *hy_sig_session_handle_thread(void *s)
{
    hy_sig_session.b_thread_running = HY_TRUE;
/*ToDo:连接到服务器，并发出登陆请求*/
    while(hy_sig_session.b_thread_running)
    {
        /*一直连接，直到连接成功*/
        while(hy_sig_session.b_thread_running && hy_sig_session.b_is_connected==HY_FALSE)
        {
    		hy_sig_session.fd = hy_tcp_connect( hy_sig_session.ip, hy_sig_session.port);
    		if( hy_sig_session.fd == -1)
    		{
    			hy_zlog_info( "dvr Client connect to serv[%s] port[%d] failed .\n",hy_sig_session.ip ,hy_sig_session.port);
    			hy_sleep(10);
    			continue;
    		}
    		else
    		{
                hy_zlog_info( "dvr Client connect to serv[%s] port[%d] success .\n",hy_sig_session.ip ,hy_sig_session.port);
    		}

    		break;
        }

        if (-1 == hy_sig_send_login_meg(hy_sig_session.fd))
        {
            hy_zlog_info("send login meg to server failed.\n");
            goto DISCONNECTED;
        }
        else
        {
            hy_zlog_info("send login meg to server success.\n");
            hy_sig_session.b_is_connected = HY_TRUE;
        }

        struct timeval tv;
        fd_set readset;
        hy_mesg_buf_t mesg_buf;//ready buf
        hy_sig_init_mesg_buf(&mesg_buf);
        while (hy_sig_session.b_is_connected == HY_TRUE && hy_sig_session.b_thread_running)
        {
            tv.tv_sec = 120;
            tv.tv_usec = 0;
            FD_ZERO(&readset);
        	FD_SET(hy_sig_session.fd, &readset);
            hy_s32 ret = select(hy_sig_session.fd+1 , &readset, NULL, NULL, &tv);
            if (ret == 0)/*ToDo:超时处理*/
            {
                hy_zlog_info("session time out.\n");
                goto DISCONNECTED;
            }
            else if (ret == -1)/*ToDo:出错处理*/
            {
                hy_zlog_info("session error.\n");
                goto DISCONNECTED;
            }
            else if (ret > 0)/*ToDo:正常的命令处理*/
            {
                if (FD_ISSET(hy_sig_session.fd, &readset ))
                {
                    //TODO:get enought buf
                    hy_s32 ret = hy_sig_read_mesg_buf(hy_sig_session.fd, &mesg_buf);
                    if (0 == ret)
                    {
                        //hy_zlog_info("read mesg buf finish.\n");
                        hy_sig_cmd_handler(hy_sig_session.fd, &mesg_buf);
                        hy_sig_init_mesg_buf(&mesg_buf);
                    }
                    else if (-2 == ret)
                    {
                        hy_zlog_info("read mesg buf not finish.\n");
                        hy_sleep(2);
                    }
                    else if(-1 == ret)
                    {
                        hy_zlog_info("reset by peer.\n");
                        goto DISCONNECTED;
                    }
                }

            }

        }

DISCONNECTED:
        hy_zlog_info("hy_sig_session disconnected.\n");
        hy_close_fd(hy_sig_session.fd);
        hy_sig_session.b_is_connected = HY_FALSE;
        hy_simple_stream_stop_realplay();
        hy_sleep(10);
    }


    pthread_exit("dvr client thread normal exit");
}

hy_s32 hy_sig_session_handle_thread_start()
{
     hy_s32 ret;
     pthread_attr_t attr;
     pthread_t pid;

     pthread_attr_init(&attr);
	 pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

     hy_sig_seesion_init();
     ret = pthread_create(&hy_sig_session.pid, &attr , hy_sig_session_handle_thread , NULL);
     if ( ret < 0 )
     {
         return HY_FAILTURE;
     }

     pthread_attr_destroy(&attr);

     return HY_SUCCESS;

}

void hy_stop_sig_session( void )
{
    if (hy_sig_session.b_thread_running)
    {
        hy_sig_session.b_thread_running = 0;
        pthread_join(hy_sig_session.pid, 0);
    }
}

void hy_sig_init_mesg_buf(hy_mesg_buf_t *mesg_buf)
{
    mesg_buf->current_contex_num = 0;
    mesg_buf->current_head_num = 0;
    mesg_buf->need_context_num = 0;
    mesg_buf->need_head_num = sizeof(M_HY_HEADER);
    memset(&mesg_buf->head, 0, sizeof(M_HY_HEADER));
    memset(mesg_buf->mesg_context, 0, 1024);
}

hy_s32 hy_sig_read_mesg_buf(hy_s32 fd, hy_mesg_buf_t *mesg_buf)
{
    hy_s32 ret;
    hy_char *p_head = (hy_char *)(&mesg_buf->head);
    hy_char *p_context = mesg_buf->mesg_context;
    if (mesg_buf->current_head_num < mesg_buf->need_head_num)
    {
        ret = hy_read(fd, p_head + mesg_buf->current_head_num, mesg_buf->need_head_num - mesg_buf->current_head_num);
        if (ret == -1)
        {
            perror("read mesg.");
            return -1;
        }
        else if(ret == 0)
        {
            return -1;
        }
        else
        {
            mesg_buf->current_head_num += ret;
        }
    }


    if (mesg_buf->current_head_num == mesg_buf->need_head_num)
    {
        mesg_buf->need_context_num = mesg_buf->head.len;
        ret = hy_read(fd, p_context + mesg_buf->current_contex_num, mesg_buf->need_context_num - mesg_buf->current_contex_num);
        if (ret == -1)
        {
            perror("read mesg.");
            return -1;
        }
        else
        {
            mesg_buf->current_contex_num += ret;
        }
    }

    if (mesg_buf->current_contex_num == mesg_buf->need_context_num && mesg_buf->current_head_num == mesg_buf->need_head_num)
    {
        return 0;
    }
    else
    {
        hy_zlog_info("current_head_num%d need_head_num%d\n", mesg_buf->current_head_num, mesg_buf->need_head_num);
        hy_zlog_info("current_contex_num%d need_context_num%d\n", mesg_buf->current_contex_num, mesg_buf->need_context_num);
        return -2;
    }
}
