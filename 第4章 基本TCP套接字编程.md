# 第4章 基本TCP套接字编程

## 4.2 socket 函数

```C
#include <sys/socket.h>
/**
 * @brief 创建一个 socket
 * 
 * @param family 协议族
 * @param type 套接字类型
 * @param protocol 协议
 * @return int 成功返回非负描述符，失败返回-1
 */
int socket(int family, int type, int protocol);
```

**图4-1 基本TCP客户/服务器程序的套接字函数**



| family   | 说明       |
| -------- | ---------- |
| AF_INET  | IPv4协议   |
| AF_INET6 | IPv6协议   |
| AF_LOCAL | Unix域协议 |
| AF_ROUTE | 路由套接字 |
| AF_KEY   | 秘钥套接字 |

**图4-2 socket函数的family常值**



| type           | 说明           |
| -------------- | -------------- |
| SOCK_STREAM    | 字节流套接字   |
| SOCK_DGRAM     | 数据报套接字   |
| SOCK_SEQPACKET | 有序分组套接字 |
| SOCK_RAW       | 原始套接字     |

**图4-3 socket函数的type常值**



| protocol     | 说明         |
| ------------ | ------------ |
| IPPROTO_CP   | TCP传输协议  |
| IPPROTO_UDP  | UDP传输协议  |
| IPPROTO_SCTP | SCTP传输协议 |

**图4-4 socket函数AF_INET或AF_INET6的protocol常值**



|                | AF_INET  | AF_INET6  | AF_LOCAL | AF_ROUTE | AF_KEY |
| -------------- | -------- | --------- | -------- | -------- | ------ |
| SOCK_STREAM    | TCP/SCTP | TCP\|SCTP | 是       |          |        |
| SOCK_DGRAM     | UDP      | UDP       | 是       |          |        |
| SOCK_SEQPACKET | SCTP     | SCTP      | 是       |          |        |
| SOCK_RAW       | IPv4     | IPv6      |          | 是       | 是     |

**图4-5 socket函数中family和type参数的组合**

## 4.3 connect函数

TCP客户使用connect函数来建立与TCP服务器的连接。

```C
#include <sys/socket.h>
/**
 * @brief 
 * 
 * @param sockfd 套接字描述符
 * @param servaddr 指向套接字的结构体指针
 * @param addrlen 指向套接字的结构体的大小
 * @return int 成功返回0，出错返回-1
 */
int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
```

客户端在调用connect前不是必须要先调用bind函数。如果是TCP套接字，调用connect函数将激发TCP的三路握手。TCP连接建立成功或者出错时才返回，出错返回有下面几种可能的情况：

1. 如果TCP客户没有收到SYN分节的响应，则返回ETIMEOUT错误。
2. 如果TCP收到的响应是RST（表示复位），则表明该服务器主机在我们指定的端口上没有进程在等待与之连接。这是一种`硬错误`，客户一接收到RST就马上返回ECONNREFUSED错误。
3. 如果客户发出的SYN在中间的某个路由器上引发了一个“destination unreachable ”，则认为是一种软错误。

## 4.4 bind函数

bind函数把一个本地协议地址赋予一个套接字。

```C
#include <sys/socket.h>

/**
 * @brief 
 * 
 * @param sockfd 套接字描述符
 * @param myaddr 指向特定于协议的地址结构的指针
 * @param addrlen myaddr所代表的结构的大小
 * @return int 成功返回0，出错返回-1
 */
int bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);
```

服务器在启动时会绑定它们众所周知的端口，而客户端不用。

## 4.5 listen函数

listen函数只在TCP服务器端调用。

```C
#include <sys/socket.h>
/**
 * @brief 
 * 
 * @param sockfd 套接字描述符
 * @param backlog 未连接队列和已连接队列之和的最大值
 * @return int 
 */
int listen(int sockfd, int backlog);
```

* 未完成连接队列：服务器收到客户端发出的SYN分节，这些套接字处于SYN_RCVD状态。
* 已完成连接队列：每个已完成TCP三路握手过程的客户端对应

## 4.6 accept函数

accept函数由TCP服务器调用，用于从已完成连接队列队头返回下一个已完成连接。如果已完成队列为空，那么进程被投入睡眠。

```c
#include <sys/socket.h>
/**
 * @brief 
 * 
 * @param sockfd 
 * @param cliaddr 
 * @param addrlen 
 * @return int 
 */
int accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
```

## 4.7 fork和exec函数

```C
#include <unistd.h>
/**
 * @brief 
 * 
 * @return pid_t 返回：在子进程中为0，在父进程中为子进程ID，若出错返回-1
 */
pid_t fork(void);
```

fork函数调用一次，会返回两次。

fork的两个典型用法：

1. 一个进程创建一个自身的副本，这样每个副本都可以在另一个副本执行其它任务的同时处理各自的某个操作。这是网络服务器的典型用法。
2. 一个进程想要执行另一个程序。既然创建新进程的唯一方法是调用fork，该进程于是首先调用fork创建一个自身的副本，然后其中的一个副本（通常为子进程）调用exec把自身替换成新的程序，这是诸如shell之类的典型用法。

### 扩展：为什么禁止使用system

system的源码如下：

```C
/**
 * @brief 
 * 
 * @param cmdstring 
 * @return int 
 * 如果fork失败，system函数返回-1；
 * 如果exec执行成功，则返回command通过exit或return返回的值，失败时返回127；
 * 如果command为NULL，则system函数返回非0值，一般为1
 */
int system(const char *cmdstring)
{
    pid_t pid;
    int status;

    if (cmdstring == NULL) {
        return 1;
    }

    if((pid = fork()) < 0) {
        status = -1;
    } else if (pid == 0) {
        execl("/bin/sh", "sh", "-c", cmdstring , NULL);
        _exit(127); /* exec执行失败返回127，注意exec只在执行失败才返回到调用者，成功时会将控制权传递给新程序 */
    } else {
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }
    }
    return(status);
}
```

可以看到system的实现中，调用了fork创建子进程，然后在子进程中调用exec函数执行command，在父进程中调用waitpid等待子进程结束。

**system()函数调用/bin/sh来执行参数指定的命令**，/bin/sh 一般是一个软连接，指向某个具体的shell，比如bash，-c选项是告诉shell从字符串command中读取命令；而在执行command期间，SIGCHLD是被阻塞的，SIGINT和SIGQUIT是被忽略的，进程如果此时收到这两个信号不会有任何动作。

system()函数使用起来容易出错，它的返回值太多，而且返回值很容易跟command的返回值混淆。可以使用popen()函数替代。

## 4.8 并发服务器

```C
int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	Signal(SIGCHLD, sig_chld);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				err_sys("accept error");
		}

		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */
			str_echo(connfd);	/* process the request */
            close(connfd); /* done with this client */
			exit(0);
		}
        /* 为什么父进程调用close没有终止它与客户端的连接？ */
        /* accept返回后与connfd关联的文件表项的引用计数值也为1。然而fork返回后，这两个描述符就在父进程与子进程间共享（也就是被复制），因此与这两个套接字相关联的文件表项各自的访问计数值均为2。*/
		Close(connfd);			/* parent closes connected socket */
	}
}
```

上面的代码是一个典型的并发服务器。当一个连接建立时，accept返回，服务器接着调用fork，然后由子进程服务客户（通过已连接套接字connfd），父进程则等待另一个连接。

## 4.9 close函数

通常的Unix close函数也用来关闭套接字，并终止TCP连接。

```C
#include <unistd.h>

int close(int sockfd);
```

close一个TCP套接字的默认行为是把该套接字标记成已关闭，然后立即返回到调用进程。该套接字描述符不能再由调用进程使用，也就是说它不能再作为read或write的第一个参数。然而TCP将尝试发送已排队等待发送到对端的任何数据，发送完毕后发送的是正常的TCP连接终止序列。

close函数只是把描述符的引用计数值减去1，如果这个值在减1之后仍然大于0，这个close调用并不引发TCP的四次挥手。如果确实想要引发四次挥手，可以调用shutdown。

## 4.10 getsockname和getpeername函数

这两个函数或者返回与某个套接字关联的本地协议地址（`getsockname`），或者返回与某个套接字关联的外地协议地址（`getpeername`）。

## 习题

4.1 在4.4节中，我们说头文件`<netinet/in.h>`中定义的`INADDR_`常值是主机字节序的。我们应该如何辨别？

> ```c
> /* Defines for Multicast INADDR.  */
> #define INADDR_UNSPEC_GROUP	((in_addr_t) 0xe0000000) /* 224.0.0.0 */
> #define INADDR_ALLHOSTS_GROUP	((in_addr_t) 0xe0000001) /* 224.0.0.1 */
> #define INADDR_ALLRTRS_GROUP    ((in_addr_t) 0xe0000002) /* 224.0.0.2 */
> #define INADDR_MAX_LOCAL_GROUP  ((in_addr_t) 0xe00000ff) /* 224.0.0.255 */
> ```
>
> 从注释就可以看出来，使用的是主机字节序。

4.2 把图1-5改为在`connect`成功返回后调用`getsockname`。使用`sock_ntop`显示赋予TCP套接字的本地IP地址和本地端口号。你的系统的临时端口在什么范围内。

>代码参考：daytimetcpcli_4_2.c
>
>运行截图：
>
>![image-20220925172436734](https://gitee.com/linnanc/image/raw/main/img/image-20220925172436734.png)

4.3 在一个并发服务器中，假设`fork`调用返回后子进程先运行，而且子进程随后在`fork`调用返回父进程之前就完成对客户的服务。图4-13中的两个`close`调用将会发生什么？

> 子进程调用close会使引用计数由2减为1，因此不会发送FIN，随后父进程调用close，引用计数由1减为0，发送FIN。

4.4 在图4-11中，先把服务器的端口号从13改为9999（这样不需要超级用户特权就能启动程序），再删掉`listen`调用，将会发生什么？

> accept会返回EINVAL，因为listenfd没有被监听。

4.5 继续上一题。删掉`bind`调用，但是保留`listen`调用，又将发生什么？

> listen调用会赋予套接字一个临时端口。

