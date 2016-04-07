/*s网络编程相关头文件*/
#include <sys/time.h>  //time
#include <sys/types.h>
#include <sys/socket.h>//connect
#include <unistd.h>    //selcet read write
#include <arpa/inet.h> //htons inet_addr, inet_aton
#include <netdb.h>     //gethostbyname

/*打印调试相关头文件*/
#include <stdio.h>
#include <error.h>         //perror
#include <errno.h>         //errno

#include <string.h>        // memset

/*自定义头文件*/
#include "hy_io_utils.h"
#include "hy_debug.h"

hy_s32 hy_tcp_connect(hy_char *ip, hy_u16 port)
{
    hy_s32 fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("create fd faild.");
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = PF_INET;
    inet_pton(PF_INET, ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    hy_s32 ret = connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1)
    {
        perror("connect faild.");
        close(fd);
        return -1;
    }

    return fd;
}

hy_s32 hy_write(hy_s32 fd, void * buf, size_t size)
{
    hy_s32 have_send = 0;
    hy_s32 ret = 0;
    while (have_send != size)
    {
        ret = write(fd, buf, size-have_send);
        if (ret == -1)
        {
            perror("write to server failed.");
            return -1;
        }
        have_send += ret;
    }

    return have_send;
}

hy_s32 hy_read(hy_s32 fd, void * buf, size_t size)
{
    return read(fd, buf, size);
}

void hy_close_fd(hy_s32 fd)
{
    close(fd);
}
