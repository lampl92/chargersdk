#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define MAX_QUEUES              10  // 消息邮箱的数量
#define MAX_QUEUE_ENTRIES       20  // 消息邮箱的大小


/* # 基础架构# */

#define NO_SYS 0                //使用操作系统

/* ## Core locking and MPU*/
#define SYS_LIGHTWEIGHT_PROT 1  //为1时使用实时操作系统的轻量级保护,保护关键代码不被中断打断

/* ## Heap and memory pools*/
#define MEM_LIBC_MALLOC     1
#define MEMP_MEM_MALLOC     1   //使用malloc代替mem_pool
#define MEM_ALIGNMENT       4   //使用4字节对齐模式
#define MEM_SIZE            16000 //内存堆heap大小
#define MEMP_OVERFLOW_CHECK 2   // when >= 2, checks each element in every pool every time memp_malloc() or memp_free() is called (useful but slow!)
#define MEMP_SANITY_CHECK   1   //run a sanity check after each memp_free() to make sure that there are no cycles in the linked lists.

/* ## Internal memory pools*/
#define MEMP_NUM_PBUF           16 //MEMP_NUM_PBUF:memp结构的pbuf数量,如果应用从ROM或者静态存储区发送大量数据时,这个值应该设置大一点
#define MEMP_NUM_UDP_PCB        4 //MEMP_NUM_UDP_PCB:UDP协议控制块(PCB)数量.每个活动的UDP"连接"需要一个PCB.
#define MEMP_NUM_TCP_PCB        5 //MEMP_NUM_TCP_PCB:同时建立激活的TCP数量
#define MEMP_NUM_TCP_PCB_LISTEN 8 //TCP监听数量
#define MEMP_NUM_TCP_SEG        16 //MEMP_NUM_TCP_SEG:最多同时在队列中的TCP段数量
#define MEMP_NUM_SYS_TIMEOUT    (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + (PPP_SUPPORT*6*MEMP_NUM_PPP_PCB) + (LWIP_IPV6 ? (1 + LWIP_IPV6_REASS + LWIP_IPV6_MLD) : 0)) //能够同时激活的timeout个数
#define PBUF_POOL_SIZE          16 //the number of buffers in the pbuf pool.

/* ## Threading*/
#define TCPIP_THREAD_NAME           "tcpip_thread"
#define TCPIP_THREAD_STACKSIZE      1024
#define TCPIP_THREAD_PRIO           11
#define TCPIP_MBOX_SIZE             MAX_QUEUE_ENTRIES
/** @todo (rgw#1#): Define LWIP_TCPIP_THREAD_ALIVE to something that triggers a watchdog.  */
#define LWIP_TCPIP_THREAD_ALIVE ()  //This is called from tcpip_thread after processing a message.

#define SLIPIF_THREAD_NAME          "slipif_loop"
#define SLIPIF_THREAD_STACKSIZE     1024
#define SLIPIF_THREAD_PRIO          1
#define DEFAULT_THREAD_NAME         "lwIP"
#define DEFAULT_THREAD_STACKSIZE    1024
#define DEFAULT_THREAD_PRIO         1

#define DEFAULT_TCP_RECVMBOX_SIZE       MAX_QUEUE_ENTRIES
#define DEFAULT_UDP_RECVMBOX_SIZE       2000//The mailbox size for the incoming packets on a NETCONN_UDP.
#define DEFAULT_ACCEPTMBOX_SIZE         MAX_QUEUE_ENTRIES

/* ## Checksum*/
#define     LWIP_CHECKSUM_CTRL_PER_NETIF 0
#define     CHECKSUM_GEN_IP              1 //软件生成与检查校验和
#define     CHECKSUM_GEN_UDP             1
#define     CHECKSUM_GEN_TCP             1
#define     CHECKSUM_CHECK_IP            CHECKSUM_GEN_IP
#define     CHECKSUM_CHECK_UDP           CHECKSUM_GEN_UDP
#define     CHECKSUM_CHECK_TCP           CHECKSUM_GEN_TCP

/* ## Hooks*/

/* # Callback-style APIs*/
/* ## TCP*/
#define     LWIP_TCP         1  //使用TCP
#define     TCP_TTL          (IP_DEFAULT_TTL)
#define TCP_WND              (2*TCP_MSS) //TCP发送窗口
#define TCP_QUEUE_OOSEQ      1  //当TCP的数据段超出队列时的控制位,当设备的内存过小的时候此项应为0
#define     TCP_MSS          536//最大TCP分段,TCP_MSS  = (MTU - IP报头大小 - TCP报头大小
#define     TCP_SND_BUF      (4 * TCP_MSS) //TCP发送缓冲区大小(bytes).越大性能越好,至少(2 * TCP_MSS)
#define     TCP_SND_QUEUELEN ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS)) //TCP发送缓冲区大小(pbuf).
#define     LWIP_EVENT_API   0
/*LWIP_EVENT_API and LWIP_CALLBACK_API:
Only one of these should be set to 1.
LWIP_EVENT_API                                        == 1: The user defines lwip_tcp_event() to receive all events (accept, sent, etc)
that happen in the system. LWIP_CALLBACK_API          == 1: The PCB callback function is called directly for the event.
This is the default.*/

/* ## UDP*/
#define LWIP_UDP   1
#define UDP_TTL   (IP_DEFAULT_TTL)

/* ## DNS*/
//Default

/* ## RAW*/
//Default

/* # Thread-safe APIs*/
/* ## Netconn*/
#define LWIP_NETCONN                    1   //LWIP_NETCONN==1:使能NETCON函数(要求使用api_lib.c)
#define LWIP_TCPIP_TIMEOUT   0//Enable tcpip_timeout/tcpip_untimeout to create timers running in tcpip_thread from another thread.

/* ## Sockets*/
#define LWIP_SOCKET                     1   //LWIP_SOCKET==1:使能Sicket API(要求使用sockets.c)
#define LWIP_SO_RCVTIMEO                1   //通过定义LWIP_SO_RCVTIMEO使能netconn结构体中recv_timeout,使用recv_timeout可以避免阻塞线程
#define LWIP_SO_SNDTIMEO                1

#define LWIP_TCP_KEEPALIVE   1 //↓
/*Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT options processing.
Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set in seconds. */


/* #IPv4*/
/* ## ICMP*/
#define 	LWIP_ICMP                    1
/* ## DHCP*/
#define LWIP_DHCP                        1
/* ## PBUF*/
#define 	PBUF_LINK_HLEN               (14 + ETH_PAD_SIZE)
#define 	PBUF_LINK_ENCAPSULATION_HLEN 0u
#define 	PBUF_POOL_BUFSIZE            LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_ENCAPSULATION_HLEN+PBUF_LINK_HLEN)
/* ## NETIF*/

/* # Debugging*/
/* ## Statistics*/
#define LWIP_STATS 0            // Enable statistics collection in lwip_stats.

/* #opt.h*/

//LWIP调试选项
#define LWIP_DEBUG                       0   //关闭DEBUG选项
#define ICMP_DEBUG                      LWIP_DBG_OFF //开启/关闭ICMPdebug

#endif /* __LWIPOPTS_H__ */

