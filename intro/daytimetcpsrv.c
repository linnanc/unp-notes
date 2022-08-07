/**
 * @file daytimetcpsrv.c
 * @author your name (you@domain.com)
 * @brief 图1-9 TCP 时间获取服务端程序
 * @version 0.1
 * @date 2022-07-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "unp.h"
#include <time.h>

int main(int argc, char **argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    time_t ticks;
    // 创建一个 socket
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(/* 13 */9999);

    // 绑定地址和端口
    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    // 监听
    Listen(listenfd, LISTENQ);

    for ( ; ; ) {
        // 服务器进程调用 accept 后进入睡眠，等待某个客户的连接到达并被内核接受
        connfd = Accept(listenfd, (SA *)NULL, NULL);
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        int total = strlen(buff);
        int n = 0;
        while (n < total) {
            Write(connfd, buff + n, 1);
            n++;
        }
        // Write(connfd, buff, strlen(buff));
        Close(connfd);
    }

}