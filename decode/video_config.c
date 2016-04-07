/*************************************
 *视频编码器参数设置
 *date:     2015.02.09
 *changelog:
 *
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
#include "global_config.h"

void video_config_set_encoder(char *ip, char *cmd, int len)
{
	int ret, server_sockfd;
    struct sockaddr_in server_addr;

	server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(server_sockfd < 0)
    {
        perror("server_sockfd socket");
		return;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    inet_pton(PF_INET, ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(80);
    ret = connect(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret == -1)
    {
         perror("connect");
         close(server_sockfd);
         return;
    }

    write(server_sockfd, cmd, len);
    //sleep(2);
    close(server_sockfd);
}

/* 取消osd显示 */
void video_osd_clear()
{
	char osd_clear[] = "PUT /ISAPI/System/Video/inputs/channels/1/overlays HTTP/1.1\r\n\
Accept: */*\r\n\
Cookie: updateTips=false; tabSystem_curTab=0; tabAudioAndVideo_curTab=0; tabPtz_curTab=0; curImageTypeIndex=0; language=zh; menu_onemenu=101; menu_twomenu=2_4; timelockset=0; tabVideoSettings_curTab=1;\r\n\
Accept-Language: zh-cn\r\n\
Referer: http://172.16.8.145/doc/page/paramconfig.asp?version=3.1.3.140723\r\n\
x-requested-with: XMLHttpRequest\r\n\
If-Modified-Since: 0\r\n\
Content-Type: text/xml; charset=UTF-8\r\n\
Accept-Encoding: gzip, deflate\r\n\
User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET4.0C; .NET4.0E; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; InfoPath.3; msn OptimizedIE8;ZHCN; UBrowser/5.0.595.32)\r\n\
Host: 172.16.8.145\r\n\
Content-Length: 920\r\n\
Connection: Keep-Alive\r\n\
Cache-Control: no-cache\r\n\
Authorization: Basic YWRtaW46MTIzNDU=\r\n\
\r\n\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n\
<VideoOverlay version=\"2.0\" xmlns=\"http://www.hikvision.com/ver20/XMLSchema\">\r\n\
\t<normalizedScreenSize>\r\n\
\t\t<normalizedScreenWidth>704</normalizedScreenWidth>\r\n\
\t\t<normalizedScreenHeight>576</normalizedScreenHeight>\r\n\
\t</normalizedScreenSize>\r\n\
\t<attribute>\r\n\
\t\t<transparent>false</transparent>\r\n\
\t\t<flashing>false</flashing>\r\n\
\t</attribute>\r\n\
\t<fontSize>0</fontSize>\r\n\
\t<DateTimeOverlay>\r\n\
\t\t<enabled>false</enabled>\r\n\
\t\t<positionX>0</positionX>\r\n\
\t\t<positionY>560</positionY>\r\n\
\t\t<dateStyle>MM-DD-YYYY</dateStyle>\r\n\
\t\t<timeStyle>24hour</timeStyle>\r\n\
\t\t<displayWeek>true</displayWeek>\r\n\
\t</DateTimeOverlay>\r\n\
\t<channelNameOverlay version=\"2.0\" xmlns=\"http://www.hikvision.com/ver20/XMLSchema\">\r\n\
\t\t<enabled>false</enabled>\r\n\
\t\t<positionX>512</positionX>\r\n\
\t\t<positionY>96</positionY>\r\n\
\t</channelNameOverlay>\r\n\
\t<frontColorMode>auto</frontColorMode>\r\n\
\t<frontColor>000000</frontColor>\r\n\
</VideoOverlay>";

	video_config_set_encoder(g_hy_nvr_info.hy_nvr_channel_info[g_hy_detect_info_t.cur_channel].ip,  osd_clear, sizeof(osd_clear));
}

/* 开启osd显示 */
void video_osd_show(char *name, int num)
{
	char osd_name[2048];
	sprintf(osd_name, "PUT /ISAPI/System/Video/inputs/channels/1 HTTP/1.1\r\n\
Accept: */*\r\n\
Cookie: updateTips=false; tabSystem_curTab=0; tabAudioAndVideo_curTab=0; tabPtz_curTab=0; curImageTypeIndex=0; language=zh; menu_onemenu=101; menu_twomenu=2_4; timelockset=0; tabVideoSettings_curTab=1;\r\n\
Accept-Language: zh-cn\r\n\
Referer: http://172.16.8.145/doc/page/paramconfig.asp?version=3.1.3.140723\r\n\
x-requested-with: XMLHttpRequest\r\n\
If-Modified-Since: 0\r\n\
Content-Type: text/xml; charset=utf-8\r\n\
Accept-Encoding: gzip, deflate\r\n\
User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET4.0C; .NET4.0E; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; InfoPath.3; msn OptimizedIE8;ZHCN; UBrowser/5.0.595.32)\r\n\
Host: 172.16.8.145\r\n\
Content-Length: %d\r\n\
Connection: Keep-Alive\r\n\
Cache-Control: no-cache\r\n\
Authorization: Basic YWRtaW46MTIzNDU=\r\n\
\r\n\
<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n\
<VideoInputChannel><id>1</id><inputPort>1</inputPort><name>%s</name></VideoInputChannel>", 134-6+num, name);

	char osd_show[] = "PUT /ISAPI/System/Video/inputs/channels/1/overlays HTTP/1.1\r\n\
Accept: */*\r\n\
Cookie: updateTips=false; tabSystem_curTab=0; tabAudioAndVideo_curTab=0; tabPtz_curTab=0; curImageTypeIndex=0; language=zh; menu_onemenu=101; menu_twomenu=2_4; timelockset=0; tabVideoSettings_curTab=1;\r\n\
Accept-Language: zh-cn\r\n\
Referer: http://172.16.8.145/doc/page/paramconfig.asp?version=3.1.3.140723\r\n\
x-requested-with: XMLHttpRequest\r\n\
If-Modified-Since: 0\r\n\
Content-Type: text/xml; charset=UTF-8\r\n\
Accept-Encoding: gzip, deflate\r\n\
User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET4.0C; .NET4.0E; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; InfoPath.3; msn OptimizedIE8;ZHCN; UBrowser/5.0.595.32)\r\n\
Host: 172.16.8.145\r\n\
Content-Length: 918\r\n\
Connection: Keep-Alive\r\n\
Cache-Control: no-cache\r\n\
Authorization: Basic YWRtaW46MTIzNDU=\r\n\
\r\n\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n\
<VideoOverlay version=\"2.0\" xmlns=\"http://www.hikvision.com/ver20/XMLSchema\">\r\n\
\t<normalizedScreenSize>\r\n\
\t\t<normalizedScreenWidth>704</normalizedScreenWidth>\r\n\
\t\t<normalizedScreenHeight>576</normalizedScreenHeight>\r\n\
\t</normalizedScreenSize>\r\n\
\t<attribute>\r\n\
\t\t<transparent>false</transparent>\r\n\
\t\t<flashing>false</flashing>\r\n\
\t</attribute>\r\n\
\t<fontSize>0</fontSize>\r\n\
\t<DateTimeOverlay>\r\n\
\t\t<enabled>true</enabled>\r\n\
\t\t<positionX>0</positionX>\r\n\
\t\t<positionY>560</positionY>\r\n\
\t\t<dateStyle>MM-DD-YYYY</dateStyle>\r\n\
\t\t<timeStyle>24hour</timeStyle>\r\n\
\t\t<displayWeek>true</displayWeek>\r\n\
\t</DateTimeOverlay>\r\n\
\t<channelNameOverlay version=\"2.0\" xmlns=\"http://www.hikvision.com/ver20/XMLSchema\">\r\n\
\t\t<enabled>true</enabled>\r\n\
\t\t<positionX>512</positionX>\r\n\
\t\t<positionY>96</positionY>\r\n\
\t</channelNameOverlay>\r\n\
\t<frontColorMode>auto</frontColorMode>\r\n\
\t<frontColor>000000</frontColor>\r\n\
</VideoOverlay>";

	video_config_set_encoder(g_hy_nvr_info.hy_nvr_channel_info[g_hy_detect_info_t.cur_channel].ip,  osd_name, sizeof(osd_name));
    video_config_set_encoder(g_hy_nvr_info.hy_nvr_channel_info[g_hy_detect_info_t.cur_channel].ip,  osd_show, sizeof(osd_show));
}

/* 设置摄像机时间 */
void video_set_camera_time()
{
	time_t t = time(NULL);
    struct tm *ltm = localtime(&t);

    int year = 1900 + ltm->tm_year;
    int month = 1 + ltm->tm_mon;
    int day= ltm->tm_mday;
    int hour = ltm->tm_hour;
    int minute = ltm->tm_min;
    int second = ltm->tm_sec;

	char set_time[1024];

	sprintf(set_time, "PUT /ISAPI/System/time HTTP/1.1\r\n\
Host: 172.16.8.245\r\n\
Connection: keep-alive\r\n\
Content-Length: 152\r\n\
Cache-Control: max-age=0\r\n\
Authorization: Basic YWRtaW46MTIzNDU=\r\n\
Accept: */*\r\n\
Origin: http://172.16.8.245\r\n\
X-Requested-With: XMLHttpRequest\r\n\
If-Modified-Since: 0\r\n\
User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 UBrowser/5.1.1591.57 Safari/537.36\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
Referer: http://172.16.8.245/doc/page/paramconfig.asp?version=3.1.3.140723\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept-Language: zh-CN,zh;q=0.8\r\n\
Cookie: updateTips=true; language=zh; _curTab=0; tabNetwork_curTab=0; tabAudioAndVideo_curTab=0; tabPtz_curTab=0; curImageTypeIndex=0; tabVideoSettings_curTab=1; menu_onemenu=110; menu_twomenu=1_0; timelockset=0; tabSystem_curTab=1\r\n\
\r\n\
<?xml version=\"1.0\" encoding=\"utf-8\"?><Time><timeMode>manual</timeMode><localTime>%4d-%2d-%2dT%2d:%2d:%2d</localTime><timeZone>CST-8:00:00</timeZone></Time>",\
																				year, month, day, hour, minute, second);
	video_config_set_encoder(g_hy_nvr_info.hy_nvr_channel_info[g_hy_detect_info_t.cur_channel].ip, set_time, sizeof(set_time));
}

/* 修改主码流参数
 * 帧率:8帧
 * I帧间隔:2
 * 码率:4096 */
void video_set_main_stream_params()
{
	char stream_params[] = "POST /onvif/Media HTTP/1.1\r\n\
Content-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration\"\r\n\
Host: 192.168.8.246\r\n\
Content-Length: 1977\r\n\
Accept-Encoding: gzip, deflate\r\n\
Connection: Close\r\n\
\r\n\
<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\"><s:Header><Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"><UsernameToken><Username>admin</Username><Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">3mGSmhjWHcbECAOSYvk4UjfiWf8=</Password><Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">Y1/6ZKV2UUqTyM1cJ3lTWSwAAAAAAA==</Nonce><Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">2011-08-08T09:16:20.432Z</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SetVideoEncoderConfiguration xmlns=\"http://www.onvif.org/ver10/media/wsdl\"><Configuration token=\"VideoEncoderToken_1\"><Name xmlns=\"http://www.onvif.org/ver10/schema\">VideoEncoder_1</Name><UseCount xmlns=\"http://www.onvif.org/ver10/schema\">1</UseCount><Encoding xmlns=\"http://www.onvif.org/ver10/schema\">H264</Encoding><Resolution xmlns=\"http://www.onvif.org/ver10/schema\"><Width>1920</Width><Height>1080</Height></Resolution><Quality xmlns=\"http://www.onvif.org/ver10/schema\">3</Quality><RateControl xmlns=\"http://www.onvif.org/ver10/schema\"><FrameRateLimit>8</FrameRateLimit><EncodingInterval>1</EncodingInterval><BitrateLimit>4096</BitrateLimit></RateControl><H264 xmlns=\"http://www.onvif.org/ver10/schema\"><GovLength>2</GovLength><H264Profile>Main</H264Profile></H264><Multicast xmlns=\"http://www.onvif.org/ver10/schema\"><Address><Type>IPv4</Type><IPv4Address>0.0.0.0</IPv4Address></Address><Port>8600</Port><TTL>1</TTL><AutoStart>false</AutoStart></Multicast><SessionTimeout xmlns=\"http://www.onvif.org/ver10/schema\">PT5S</SessionTimeout></Configuration><ForcePersistence>true</ForcePersistence></SetVideoEncoderConfiguration></s:Body></s:Envelope>";
	video_config_set_encoder(g_hy_nvr_info.hy_nvr_channel_info[g_hy_detect_info_t.cur_channel].ip, stream_params, sizeof(stream_params));
}



