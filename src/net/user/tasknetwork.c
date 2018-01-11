/**
* @file lwip_init.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-27
*/
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ifconfig.h"
#include "modem.h"

#include "os_port.h"
#include "core/net.h"
#include "drivers/mac/stm32f4x9_eth_driver.h"
#include "drivers/phy/lan8720_driver.h"
#include "dhcp/dhcp_client.h"
#include "debug.h"

int_t eth_init(void);
void vTaskTCPClient(void *pvParameters)
{
    
//    pModem = DevModemCreate();
//    modem_open(pModem);
//    modem_init(pModem);
//    Modem_Poll(pModem);//这是任务
    eth_init();
    while (1)
    {
        vTaskDelay(1000);
    }
}
int_t eth_init(void)
{
    DhcpClientSettings dhcpClientSettings;
    DhcpClientContext dhcpClientContext;
    error_t error;
    NetInterface *interface;
    OsTask *task;
    MacAddr macAddr;
    Ipv4Addr ipv4Addr;

    //初始化TCP/IP协议栈
    error = netInit();
    if (error)
    {
        TRACE_ERROR("Failed to initialize TCP/IP stack!\r\n");
    }

    //设置第一个以太网接口
    interface = &netInterface[0];

    //设置接口名称
    //netSetInterfaceName(interface, "eth0"); 已经设置
    //设置主机名
    netSetHostname(interface, ifconfig.info.strHostName);
    //选择网卡
    netSetDriver(interface, &stm32f4x9EthDriver);
    netSetPhyDriver(interface, &lan8720PhyDriver);
    //设置MAC地址
    macStringToAddr(ifconfig.info.strMAC, &macAddr);
    netSetMacAddr(interface, &macAddr);

    //初始化网路接口
    error = netConfigInterface(interface);
    if (error)
    {
        TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
    }

    if (ifconfig.info.ucDHCPEnable == 1)
    {
       //获取默认设置
        dhcpClientGetDefaultSettings(&dhcpClientSettings);
        //设置让DHCP去配置该接口
        dhcpClientSettings.interface = interface;
        //Disable rapid commit option
        dhcpClientSettings.rapidCommit = FALSE;

        //DHCP客户端初始化
        error = dhcpClientInit(&dhcpClientContext, &dhcpClientSettings);
        if (error)
        {
            TRACE_ERROR("Failed to initialize DHCP client!\r\n");
        }

        //启动DHCP客户端
        error = dhcpClientStart(&dhcpClientContext);
        if (error)
        {
            TRACE_ERROR("Failed to start DHCP client!\r\n");
        }
    }
    else
    {
        //设置主机 IPv4 地址
        ipv4StringToAddr(ifconfig.info.strIP, &ipv4Addr);
        ipv4SetHostAddr(interface, ipv4Addr);

        //设置子网掩码
        ipv4StringToAddr(ifconfig.info.strMask, &ipv4Addr);
        ipv4SetSubnetMask(interface, ipv4Addr);

        //设置默认网关
        ipv4StringToAddr(ifconfig.info.strGate, &ipv4Addr);
        ipv4SetDefaultGateway(interface, ipv4Addr);

        //设置DNS服务器
        ipv4StringToAddr(ifconfig.info.strDNS1, &ipv4Addr);
        ipv4SetDnsServer(interface, 0, ipv4Addr);
        ipv4StringToAddr(ifconfig.info.strDNS2, &ipv4Addr);
        ipv4SetDnsServer(interface, 1, ipv4Addr);
    }
    
    return 0;
}
#if 0

#define lwip1_4_1

int ppp;

typedef void (*ctx_cb_fn)(uint8_t *msg);

void tcpip_init_done(void *arg)
{
    LWIP_UNUSED_ARG(arg);
    xEventGroupSetBits(xHandleEventTCP, defEventBitTCPIPinit);
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
        xEventGroupSetBits(xHandleEventTCP, defEventBitConnectOK);
        return;
    }

    xEventGroupClearBits(xHandleEventTCP, defEventBitConnectOK);

    /* ppp_close() 被用户调用，不要重新连接 */
    if (err_code == PPPERR_USER)
    {
        /* ppp_free(); -- 可以在这里调用 */
        return;
    }

    /** @todo (zshare#1#): 在这里对modem进行重连，连接好后通过信号量或者时间通知该函数，然后对ppp进行重连 */
    uxBitLwip = xEventGroupSync(xHandleEventTCP,
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
        xEventGroupSetBits(xHandleEventTCP, defEventBitConnectOK);
        return;
    }

    xEventGroupClearBits(xHandleEventTCP, defEventBitConnectOK);

    /* ppp_close() 被用户调用时返回的代码，说明不要自动重新连接，可以进行释放 */
    if (errCode == PPPERR_USER)
    {
        /* ppp_free(); -- 可以在这里调用 */
        return;
    }

    /** @todo (zshare#1#): 在这里对modem进行重连，连接好后通过信号量或者时间通知该函数，然后对ppp进行重连 */
    xEventGroupSetBits(xHandleEventTCP, defEventBitReDail);
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
    uxBitLwIP = xEventGroupWaitBits(xHandleEventTCP, defEventBitTCPIPinit,
                                    pdTRUE, pdTRUE, portMAX_DELAY);
    if((uxBitLwIP & defEventBitTCPIPinit) != defEventBitTCPIPinit)
    {
        //当portMAX_DELAY修改为超时时间后，当超时后仍未初始化完成则进入超时处理。

    }

    uxBitLwIP = xEventGroupWaitBits(xHandleEventTCP, defEventBitDailCONNECT,
                                    pdTRUE, pdTRUE, portMAX_DELAY);
    if((uxBitLwIP & defEventBitDailCONNECT) == defEventBitDailCONNECT)
    {
        pppInit();
        /*创建 PPPoS 控制块*/
        pppSetAuth(PPPAUTHTYPE_PAP, "", "");
        ppp = pppOverSerialOpen(0, ppp_on_status, &ppp);
    }
    return ppp;
}

#endif