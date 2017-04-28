/**
* @file lwip_init.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-27
*/
#include "ip.h"
#include "init.h"
#include "netif/ppp/ppp.h"
#include "netif/ppp/pppapi.h"
#include "netif/ppp/pppos.h"
#include "lwip/dns.h"
#include "bsp.h"

typedef void (*ctx_cb_fn)(uint8_t *msg);


void tcpip_init_done(void *arg)
{
    LWIP_UNUSED_ARG(arg);
    xEventGroupSetBits(xHandleEventlwIP, defEventBitTCPIPinit);
}

/** @brief PPPoS 串口输出回调函数
 *
 * @param pcb ppp_pcb*  PPP控制块
 * @param data u8_t*    写到串口的buffer
 * @param len u32_t     buffer length
 * @param ctx void*     用户回调函数指针
 * @return u32_t        成功写入长度
 *
 */
static u32_t output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
    (*(ctx_cb_fn)ctx)("output_cb called\n");
    return uart_write(UART_PORT_GPRS, data, len);
}

/** @brief PPP status 回调函数，lwIP内核线程会在PPP状态改变（up，down，...）时调用
 *
 * @param pcb ppp_pcb*
 * @param err_code int
 * @param ctx void*
 * @return void
 *
 */
static void status_cb(ppp_pcb *pcb, int err_code, void *ctx)
{
    struct netif *pppif = ppp_netif(pcb);
    //LWIP_UNUSED_ARG(ctx);
    (*(ctx_cb_fn)ctx)("status_cb called\n");
    switch(err_code)
    {
    case PPPERR_NONE:
    {
#if LWIP_DNS
        const ip_addr_t *ns;
#endif /* LWIP_DNS */
        printf_safe("status_cb: Connected\n");
#if PPP_IPV4_SUPPORT
        printf_safe("   our_ipaddr  = %s\n", ipaddr_ntoa(&pppif->ip_addr));
        printf_safe("   his_ipaddr  = %s\n", ipaddr_ntoa(&pppif->gw));
        printf_safe("   netmask     = %s\n", ipaddr_ntoa(&pppif->netmask));
#if LWIP_DNS
        ns = dns_getserver(0);
        printf_safe("   dns1        = %s\n", ipaddr_ntoa(ns));
//        ns = dns_getserver(1);
        printf_safe("   dns2        = %s\n", ipaddr_ntoa(ns));
#endif /* LWIP_DNS */
#endif /* PPP_IPV4_SUPPORT */
#if PPP_IPV6_SUPPORT
        printf_safe("   our6_ipaddr = %s\n", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */
        break;
    }
    case PPPERR_PARAM:
    {
        printf_safe("status_cb: Invalid parameter\n");
        break;
    }
    case PPPERR_OPEN:
    {
        printf_safe("status_cb: Unable to open PPP session\n");
        break;
    }
    case PPPERR_DEVICE:
    {
        printf_safe("status_cb: Invalid I/O device for PPP\n");
        break;
    }
    case PPPERR_ALLOC:
    {
        printf_safe("status_cb: Unable to allocate resources\n");
        break;
    }
    case PPPERR_USER:
    {
        printf_safe("status_cb: User interrupt\n");
        break;
    }
    case PPPERR_CONNECT:
    {
        printf_safe("status_cb: Connection lost\n");
        break;
    }
    case PPPERR_AUTHFAIL:
    {
        printf_safe("status_cb: Failed authentication challenge\n");
        break;
    }
    case PPPERR_PROTOCOL:
    {
        printf_safe("status_cb: Failed to meet protocol\n");
        break;
    }
    case PPPERR_PEERDEAD:
    {
        printf_safe("status_cb: Connection timeout\n");
        break;
    }
    case PPPERR_IDLETIMEOUT:
    {
        printf_safe("status_cb: Idle Timeout\n");
        break;
    }
    case PPPERR_CONNECTTIME:
    {
        printf_safe("status_cb: Max connect time reached\n");
        break;
    }
    case PPPERR_LOOPBACK:
    {
        printf_safe("status_cb: Loopback detected\n");
        break;
    }
    default:
    {
        printf_safe("status_cb: Unknown error code %d\n", err_code);
        break;
    }
    }

    if (err_code == PPPERR_NONE)
    {
        return;
    }

    /* ppp_close() 被用户调用，不要重新连接 */
    if (err_code == PPPERR_USER)
    {
        /* ppp_free(); -- 可以在这里调用 */
        return;
    }

    /** @todo (zshare#1#): 在这里对modem进行重连，连接好后通过信号量或者时间通知该函数，然后对ppp进行重连 */

    ppp_connect(pcb, 30);//30s
}

void ctx_cb(uint8_t *msg)
{
    printf_safe("%s", msg);
}

void lwip_init_task()
{
    ppp_pcb *ppp;           /* PPP control block */
    struct netif ppp_netif; /* PPP IP interface */
    EventBits_t uxBitTCP;

    /*创建 PPPoS 控制块*/
    ppp = pppapi_pppos_create(&ppp_netif, output_cb, status_cb, ctx_cb);

    /*创建 PPP 连接*/

    /*
     * 初始化 PPP 客户端连接
     * ==============================
     */
    pppapi_set_default(ppp);    //设置ppp为默认线路（default route）

    tcpip_init(tcpip_init_done, NULL);
    uxBitTCP = xEventGroupWaitBits(xHandleEventlwIP, defEventBitTCPIPinit,
                                   pdTRUE, pdTRUE, portMAX_DELAY);
    if((uxBitTCP & defEventBitTCPIPinit) != defEventBitTCPIPinit)
    {
        //当portMAX_DELAY修改为超时时间后，当超时后仍未初始化完成则进入超时处理。

    }

    /**/
    pppapi_connect(ppp, 0);     //初始化PPP协商，等待时间为0（holdoff = 0;）。只在PPP对话挂掉状态时进行调用



//    ip_addr_t ip_addr, netmask, gw;
//
//    tcpip_init(tcpip_init_done, NULL);
//    IP4_ADDR(&gw, 192, 168, 1, 1);
//    IP4_ADDR(&ip_addr, 192, 168, 1, 100);
//    IP4_ADDR(&netmask, 255, 255, 255, 0);
//    netif_add();
//    netif_set_default()
//    netif_set_up();
}
