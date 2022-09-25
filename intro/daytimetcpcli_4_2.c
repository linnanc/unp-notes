/**
 * @file daytimetcpcli_4_2.c
 * @author your name (you@domain.com)
 * @brief 习题4.2
 * @version 0.1
 * @date 2022-09-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "unp.h"

int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    if (argc != 2) {
        err_quit("usage: a.out <IPaddress>");
    }

    // 创建一个 socket
    if ((sockfd = socket(AF_INET/* 9999 */, SOCK_STREAM, 0)) < 0) {
        printf("errno is %d, %s\n", errno, strerror(errno));
        err_sys("socket error");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // 时间获取服务器的端口号是 13，htons 是主机字节序到网络字节序的转换
    servaddr.sin_port = htons(/* 13 */9999);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        err_quit("inet_pton error for %s", argv[1]);
    }
    // 连接
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
        err_sys("connect error");
    }
    socklen_t len = 0;
    len = sizeof(cliaddr);
    Getsockname(sockfd, (SA *) &cliaddr, &len);
    printf("local addr: %s\n", Sock_ntop((SA *) &cliaddr, len));

    int count = 0;
    while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF) {
            err_sys("fputs error");
        }
        count++;
    }

    printf("count is %d\n", count);
    
    if (n < 0) {
        err_sys("read error");
    }

    exit(0);
}