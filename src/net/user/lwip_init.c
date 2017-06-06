/**
* @file lwip_init.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-27
*/
#include "lwip/ip.h"
#include "lwip/tcpip.h"
#include "netif/ppp/ppp.h"
#include "bsp.h"


#define lwip1_4_1

int ppp;

typedef void (*ctx_cb_fn)(uint8_t *msg);

void tcpip_init_done(void *arg)
{
    LWIP_UNUSED_ARG(arg);
    xEventGroupSetBits(xHandleEventLwIP, defEventBitTCPIPinit);
}

/** @brief PPPoS ��������ص�����
 *
 * @param pcb ppp_pcb*  PPP���ƿ�
 * @param data u8_t*    д�����ڵ�buffer
 * @param len u32_t     buffer length
 * @param ctx void*     �û��ص�����ָ��
 * @return u32_t        �ɹ�д�볤��
 *
 */
#ifdef lwip2_0
static u32_t output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
    (*(ctx_cb_fn)ctx)("output_cb called\n");/** @todo (rgw#1#): ע�͵�����޷���ͨ */
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

/** @brief PPP status �ص�������lwIP�ں��̻߳���PPP״̬�ı䣨up��down��...��ʱ����
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
        xEventGroupSetBits(xHandleEventLwIP, defEventBitPPPup);
        return;
    }

    xEventGroupClearBits(xHandleEventLwIP, defEventBitPPPup);

    /* ppp_close() ���û����ã���Ҫ�������� */
    if (err_code == PPPERR_USER)
    {
        /* ppp_free(); -- ������������� */
        return;
    }

    /** @todo (zshare#1#): �������modem�������������Ӻú�ͨ���ź�������ʱ��֪ͨ�ú�����Ȼ���ppp�������� */
    uxBitLwip = xEventGroupSync(xHandleEventLwIP,
                                defEventBitReDail,
                                defEventBitDailCONNECT,
                                portMAX_DELAY);
    if((uxBitLwip & defEventBitDailCONNECT) == defEventBitDailCONNECT)
    {
        ppp_connect(pcb, 1);    //30s
    }
}
#endif
#ifdef lwip1_4_1
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
        xEventGroupSetBits(xHandleEventLwIP, defEventBitPPPup);
        return;
    }

    xEventGroupClearBits(xHandleEventLwIP, defEventBitPPPup);

    /* ppp_close() ���û�����ʱ���صĴ��룬˵����Ҫ�Զ��������ӣ����Խ����ͷ� */
    if (errCode == PPPERR_USER)
    {
        /* ppp_free(); -- ������������� */
        return;
    }

    /** @todo (zshare#1#): �������modem�������������Ӻú�ͨ���ź�������ʱ��֪ͨ�ú�����Ȼ���ppp�������� */
    xEventGroupSetBits(xHandleEventLwIP, defEventBitReDail);
}
#endif

void ctx_cb(uint8_t *msg)
{
    printf_safe("%s", msg);
}

int lwip_init_task(void)
{
    int *fd = NULL;
    int *linkstateCx = NULL;

    EventBits_t uxBitLwIP;

    tcpip_init(tcpip_init_done, NULL);
    uxBitLwIP = xEventGroupWaitBits(xHandleEventLwIP, defEventBitTCPIPinit,
                                    pdTRUE, pdTRUE, portMAX_DELAY);
    if((uxBitLwIP & defEventBitTCPIPinit) != defEventBitTCPIPinit)
    {
        //��portMAX_DELAY�޸�Ϊ��ʱʱ��󣬵���ʱ����δ��ʼ���������볬ʱ������

    }

    uxBitLwIP = xEventGroupWaitBits(xHandleEventLwIP, defEventBitDailCONNECT,
                                    pdTRUE, pdTRUE, portMAX_DELAY);
    if((uxBitLwIP & defEventBitDailCONNECT) == defEventBitDailCONNECT)
    {
        pppInit();
        /*���� PPPoS ���ƿ�*/
        pppSetAuth(PPPAUTHTYPE_PAP, "", "");
        ppp = pppOverSerialOpen(0, ppp_on_status, &ppp);
    }
    return ppp;
}