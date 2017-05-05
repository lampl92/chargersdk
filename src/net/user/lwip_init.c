/**
* @file lwip_init.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-27
*/
#include "lwip/ip.h"
#include "lwip/dns.h"
#include "netif/ppp/pppapi.h"
#include "netif/ppp/pppos.h"
#include "bsp.h"
#include "ifconfig.h"


#define lwip2_0
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
#ifdef lwip2_0
static u32_t output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
    (*(ctx_cb_fn)ctx)("output_cb called\n");/** @todo (rgw#1#): 注释掉后会无法拨通 */
//    int i;
//    printf_safe("mcu output: ");
//    for(i = 0; i < len; i++)
//    {
//        printf_safe("%02X ", data[i]);
//    }
//    printf_safe("\n");
//    printf_safe("len = %d\n", len);
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
    EventBits_t uxBitLwip;
    struct netif *pppif = ppp_netif(pcb);
    //LWIP_UNUSED_ARG(ctx);
    (*(ctx_cb_fn)ctx)("status_cb called\n");
    switch(err_code)
    {
    case PPPERR_NONE:
    {
        ifconfig.local_ip.addr = pppif->ip_addr.addr;
        ifconfig.gateway.addr = pppif->gw.addr;
        ifconfig.netmask.addr = pppif->netmask.addr;
        ifconfig_set();
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
        xEventGroupSetBits(xHandleEventlwIP, defEventBitPPPup);
        return;
    }

    xEventGroupClearBits(xHandleEventlwIP, defEventBitPPPup);

    /* ppp_close() 被用户调用，不要重新连接 */
    if (err_code == PPPERR_USER)
    {
        /* ppp_free(); -- 可以在这里调用 */
        return;
    }

    /** @todo (zshare#1#): 在这里对modem进行重连，连接好后通过信号量或者时间通知该函数，然后对ppp进行重连 */
    uxBitLwip = xEventGroupSync(xHandleEventlwIP,
                                defEventBitReDail,
                                defEventBitDailCONNECT,
                                portMAX_DELAY);
    if((uxBitLwip & defEventBitDailCONNECT) == defEventBitDailCONNECT)
    {
        ppp_connect(pcb, 1);    //30s
    }
}
#endif
#if lwip1_4_1
void ppp_on_status(void *ctx, int errCode, void *arg)
{
    int pd;
    EventBits_t uxBitLwip;

    pd = *((int *)arg);
    switch (errCode)
    {
    case PPPERR_PARAM:              /* Invalid parameter. */
        printf_safe("status_cb: Invalid parameter\n");
        break;
    case PPPERR_OPEN:               /* Unable to open PPP session. */
        printf_safe("status_cb: Unable to open PPP session\n");
        break;
    case PPPERR_DEVICE:             /* Invalid I/O device for PPP. */
        printf_safe("status_cb: Invalid I/O device for PPP\n");
        break;
    case PPPERR_ALLOC:              /* Unable to allocate resources. */
        printf_safe("status_cb: Unable to allocate resources\n");
        break;
    case PPPERR_CONNECT:            /* Connection lost. */
        printf_safe("status_cb: Connection lost\n");
        break;
    case PPPERR_AUTHFAIL:           /* Failed authentication challenge. */
        printf_safe("status_cb: Failed authentication challenge\n");
        break;
    case PPPERR_PROTOCOL:           /* Failed to meet protocol. */
        printf_safe("status_cb: Failed to meet protocol\n");
        break;
    default:
        break;
    }

    if (errCode == PPPERR_NONE)
    {
        return;
    }

    /* ppp_close() 被用户调用时返回的代码，说明不要自动重新连接，可以进行释放 */
    if (errCode == PPPERR_USER)
    {
        /* ppp_free(); -- 可以在这里调用 */
        return;
    }

    /** @todo (zshare#1#): 在这里对modem进行重连，连接好后通过信号量或者时间通知该函数，然后对ppp进行重连 */
    uxBitLwip = xEventGroupSync(xHandleEventlwIP,
                                defEventBitReDail,
                                defEventBitDailCONNECT,
                                portMAX_DELAY);
    if((uxBitLwip & defEventBitDailCONNECT) == defEventBitDailCONNECT)
    {
    }
}
#endif

void ctx_cb(uint8_t *msg)
{
    printf_safe("%s", msg);
}

ppp_pcb *lwip_init_task(void)
{
    ppp_pcb *ppp;           /* PPP control block */
    struct netif ppp_netif; /* PPP IP interface */

    EventBits_t uxBitLwIP;

    tcpip_init(tcpip_init_done, NULL);
    uxBitLwIP = xEventGroupWaitBits(xHandleEventlwIP, defEventBitTCPIPinit,
                                    pdTRUE, pdTRUE, portMAX_DELAY);
    if((uxBitLwIP & defEventBitTCPIPinit) != defEventBitTCPIPinit)
    {
        //当portMAX_DELAY修改为超时时间后，当超时后仍未初始化完成则进入超时处理。

    }

    uxBitLwIP = xEventGroupWaitBits(xHandleEventlwIP, defEventBitDailCONNECT,
                                    pdTRUE, pdTRUE, portMAX_DELAY);
    if((uxBitLwIP & defEventBitDailCONNECT) == defEventBitDailCONNECT)
    {
//        pppInit();
//        /*创建 PPPoS 控制块*/
//        pppSetAuth(PPPAUTHTYPE_PAP, "", "");
//        pd = pppOverSerialOpen(0, ppp_on_status, &pd);

        /*创建 PPPoS 控制块*/
        ppp = pppapi_pppos_create(&ppp_netif, output_cb, status_cb, ctx_cb);

        /*创建 PPP 连接*/

        /*
         * 初始化 PPP 客户端连接
         * ==============================
         */
        ppp_set_auth(ppp, PPPAUTHTYPE_PAP, "", "");
        ppp_set_silent(ppp, 1);
        pppapi_set_default(ppp);    //设置ppp为默认线路（default route）
        pppapi_connect(ppp, 0);     //初始化PPP协商，等待时间为0（holdoff = 0;）。只在PPP对话挂掉状态时进行调用
//        pppapi_listen(ppp);
    }
    return ppp;
}
