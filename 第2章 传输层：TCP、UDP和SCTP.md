# 第2章 传输层：TCP、UDP 和 SCTP

## 2.1 概述

* TCP/IP 协议栈：TCP/IP 协议栈，除了 TCP 和 IP 这两个主要协议外，还有许多其它的成员，例如 ARP、RARP、ICMP、IGMP、UDP等。
* UDP：用户数据包协议，在 RFC 768 中有详细说明，UDP 协议提供的是无连接的，不可靠的服务。
* TCP：传输控制协议，TCP 是面向连接的协议，首先 TCP 客户端与 TCP 服务器之间会建立一个连接，然后再通过这个连接传输数据。其次，TCP 还提供可靠性。
* SCTP：流控制传输协议，SCTP 在客户和服务器之间提供关联，并像 TCP 那样给应用提供可靠性、排序、流量控制以及全双工的数据传送。关联与连接之间的区别是：一个连接只涉及两个 IP 地址之间的通信。一个关联指代两个系统之间的一次通信，它可能因为 SCTP 支持多宿而涉及不止两个地址。

## 2.4 传输控制协议 TCP

### 三路握手

1. 服务器准备好接受外来的连接。通常通过调用 socket、bind 和 listen 这3个函数来完成，称为被动打开。
2. 客户端通过调用 connect 发起主动打开。这导致客户 TCP 发送一个 SYN 分节，告诉服务器客户将在（待建立的）连接中发送的数据的初始序列号。通常 SYN 分节不携带数据，其所在 IP 数据报只含有一个 IP 首部、一个 TCP 首部及可能有的 TCP 选项。
3. 服务器必须确认（ACK）客户的 SYN，同时自己也得发送一个 SYN 分节，它含有服务器将在同一连接中发送的数据的初始序列号。服务器在单个分节中发送 SYN 和对客户 SYN 的 ACK（确认）。
4. 客户必须确认服务器的 SYN。

TCP 的三次握手流程图：

![image-20220728162248287](https://gitee.com/linnanc/image/raw/main/img/image-20220728162248287.png)

### TCP 选项

* MSS 选项：发送 SYN 的 TCP 一端使用本选项通告对端它的最大分节大小即 MSS。
* 窗口规模选项：TCP 连接任何一端能够通告对端的最大窗口大小是65535，因为在 TCP 首部中相应的字段占16位。
* 时间戳选项：这个选项对于高速网络连接是必要的，它可以防止由失而复现的分组可能造车的数据损坏。

### TCP 连接终止

TCP 建立一个连接需要3个分节，终止一个连接则需要4个分节。

1. 某个应用进程首先调用 close，称之为主动关闭。该端的 TCP 于是发送一个 FIN 分节，表示数据发送完毕。
2. 接收到这个 FIN 的对端执行被动关闭。这个 FIN 由 TCP 确认。它的接收也作为一个文件结束符传递给接收端应用进程，因为 FIN 的接收意味着接收端应用进程在相应连接上再无额外数据可接收。
3. 一段时间后，接收到这个文件结束符的应用进程将调用 close 关闭它的套接字。这导致 TCP 也发送一个 FIN。
4. 接收这个最终 FIN 的原发送端 TCP （即执行主动关闭的那一端）确认这个 FIN。

TCP 连接关闭时的分组交换

![image-20220728172314329](https://gitee.com/linnanc/image/raw/main/img/image-20220728172314329.png)

在步骤2和步骤3之间，从执行被动关闭一端到主动关闭一端流动数据是可能的。这称为半关闭。

### TCP 的状态转移图

![image-20220807113447550](https://gitee.com/linnanc/image/raw/main/img/image-20220807113447550.png)

### TCP 连接的分组交换

 ![image-20220808212131506](https://gitee.com/linnanc/image/raw/main/img/image-20220808212131506.png)

### TIME_WAIT 状态

TCP 客户端发起主动关闭后会进入一个 TIME_WAIT 状态，这个状态的持续时间是2MSL（2倍最长分节生命期）。

问：TIME_WAIT 的状态持续时间为什么是2MSL？

客户端收到 FIN N 后，要发送 ACK N+1，此时客户端想要知道服务器是否收到了这个 ACK N+1，ACK N+1 传给服务器的最长时间是 MSL，如果服务器没有收到 ACK N+1，那么它会重新发送 FIN N，而这个 FIN N 传给客户端的最长时间也是 MSL，所以加起来就是2MSL。如果 TIME_WAIT 的时间内没有收到 FIN N了，就说明服务器以及收到 ACK N+1，则可以关闭了。

TIME_WAIT 状态有两个存在的理由：

1. 可靠地实现 TCP 全双工连接的终止。
2. 允许老的重复分节在网络中消逝。

## 2.5 SCTP 协议

先略过 SCTP 协议。后续使用到再回头看。

## 2.9 端口号

端口号是16位的整数，用于区分不同的进程。端口号的范围是0~65535.

* 0~1023，这些端口由 IANA 分配和控制。
* 1024~49151，这些端口已经登记，不受 IANA 控制。
* 49152~65535，这些端口是动态的或者私用的。

## 2.11 缓冲区大小及限制

* IPv4 数据报的最大大小是65535字节，包括 IPv4 首部。
* IPv6 数据报的最大大小是65575字节，包括40字节的 IPv6 首部。IPv6 的净荷字段不包括 IPv6 的首部，IPv4 的总长度是包括 IPv4 首部的。IPv6 还有一个特大净荷选项，它把净荷长度字段扩展到32位，这个选项需要 MTU 超过65535的数据链路提供支持。
* 许多网络有一个可由硬件规定的 MTU。例如，以太网的 MTU 是1500字节，另有一些链路（如 PPP 协议的点对点链路）其 MTU 可以人为配置。IPv4 要求的最小链路 MTU 是68字节。
* 当一个 IP 数据报将从某个接口送出时，如果它的大小超过相应链路的 MTU，IPv4 和 IPv6 都将执行分片。

## 2.14 小结

* UDP 是一个简单，不可靠，无连接的协议，而 TCP 是一个复杂，可靠，面向连接的协议。
* TCP 使用三次握手建立连接，使用四次挥手终止连接。
* TCP 的 TIME_WAIT 状态是为了实现 TCP 的全双工连接终止，并允许老的重复分节在网络中消逝。

## 习题

2.1 我们已经提到IPv4（IP版本4）和IPv6（版本6）。IP版本5情况如何，IP版本0、1、2和3又是什么？

> http://www.iana.org/assignments/version-numbers/version-numbers.xhtml
>
> https://blog.alertlogic.com/blog/where-is-ipv1,-2,-3,and-5/
>
> https://forum.huawei.com/enterprise/en/where-are-ipv1-2-3-and-5/thread/521375-887

2.2　你从哪里可以找到有关IP版本5的信息？

> https://baike.baidu.com/item/IPv5

2.3 在讲解图2-15时我们说过，如果没收到来自对端的MSS选项，本端TCP就采用536这个MSS值。为什么使用这个值？

> TCP 的最小重组缓冲区字节数是576，减去 IPv4 首部20字节和 TCP 首部20字节后，MSS （最大分节大小）就为536字节了。

2.4 给在第1章中讲解的时间获取客户/服务器应用画出类似于图2-5的分组交换过程，假设服务器在单个TCP分节中返回26个字节的完整数据。![image-20220621152646715.png](https://gitee.com/linnanc/image/raw/main/img/image-20220621152646715.png)

> 使用 wireshark 抓到的数据包查看 MSS 是65495。
>
> ![image-20220813211212542](https://gitee.com/linnanc/image/raw/main/img/image-20220813211212542.png)
>
> 此外，主动关闭的是服务端，而不是客户端
>
> ![image-20220814084303898](https://gitee.com/linnanc/image/raw/main/img/image-20220814084303898.png)

2.5 在一个以太网上的主机和一个令牌环网上的主机之间建立一个连接，其中以太网上主机的TCP通告的MSS为1460，令牌环网上主机的TCP通告的MSS为4096。两个主机都没有实现路径MTU发现功能。观察分组，我们在两个相反方向上都找不到大于1460字节的数据，为什么？

> 令牌环网上发送到以太网的数据大小不大于1460字节的原因是因为以太网上主要通告的MSS值就为1460个字节，所以令牌环网上发送出去的数据的长度不能够大于 MSS 值；令牌环网上主机通告的 MSS 值为4096，也即是说以太网能够发送到令牌环网上的TCP净荷值为4096，但是以太网的 MTU 值又是由硬件所决定的，最大只支持1500（包括IP头至少20B和TCP头至少20B），为避免分片，因此以太网发送到令牌环网的数据的净荷也为1500-20-20=1460B，所以两个方向的净数据长度不会大于1460字节。

2.6 在讲解图2-19时我们说过OSPF直接使用IP。承载OSPF数据报的IPv4首部（见图A-1）的协议字段是什么值？

> OSPF 的协议字段为89。

2.7 在讨论SCTP输出时我们说过，SCTP发送端必须等待累积确认点超过已发送的数据，才可以从套接字缓冲区中释放该数据。假设某个选择性确认（SACK）表明累积确认点之后的数据也得到了确认，这样的数据为什么却不能被释放呢？

> 略