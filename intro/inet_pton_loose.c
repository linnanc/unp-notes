/**
 * @file inet_pton_loose.c
 * @author your name (you@domain.com)
 * @brief 习题3-3
 * @version 0.1
 * @date 2022-08-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int inet_pton_loose(int family, const char *strptr, void *addrptr)
{
    struct in_addr in_val;
    char buf[64] = {0};

    switch (family) {
    case AF_INET:
        /* inet_pton 成功返回1*/
        if (inet_pton(family, strptr, addrptr) == 0) {
            
            if (inet_aton(strptr, &in_val) == 1) {
                memcpy(addrptr, &in_val, sizeof(struct in_addr));
                return 1;
            } else {
                printf("error: inet_pton failed! %s is \n", strptr);
            }
        }
        break;
    case AF_INET6:
        if (inet_pton(family, strptr, addrptr) == 0) {
            struct in_addr in_val;
            if (inet_aton(strptr, &in_val) == 1) {
                memcpy(addrptr, &in_val, sizeof(struct in_addr));
                snprintf(buf, 64, "0:0:0:0:0:FFFF:%s", strptr);
                memcpy(strptr, buf, sizeof(buf));
                return 1;   
            } else {
                printf("error: inet_pton failed! %s is \n", strptr);
            }
        }
        break;
    default:
        printf("error: family is invalid!\n");
        break;
    }

    return 0;
}

int main(int argc, char **argv)
{
    char strptr[64] = {0};
    struct  sockaddr_in servaddr;
    struct sockaddr_in6 servaddr6;


    if (argc != 2) {
        printf("usage: ./a.out <IPaddress>\n");
        return 0;
    }

    memcpy(strptr, argv[1], strlen(argv[1]) + 1);

    if (inet_pton_loose(AF_INET6, strptr, &servaddr6.sin6_addr)) {
        printf("inet_pton_loose6 success!\n");
        printf("strptr = %s\n", strptr);
    } else {
        printf("inet_pton_loose6 error!\n");
    }

    memcpy(strptr, argv[1], strlen(argv[1]) + 1);
    if (inet_pton_loose(AF_INET, strptr, &servaddr.sin_addr)) {
        printf("inet_pton_loose4 success!\n");
    } else {
        printf("inet_pton_loose4 error!\n");
    }

    return 0;
}
