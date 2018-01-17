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
#include "cfg_parse.h"
#include "stringName.h"
#include "evse_globals.h"
#include "taskcreate.h"

#include "os_port.h"
#include "core/net.h"
#include "drivers/mac/stm32f4x9_eth_driver.h"
#include "drivers/phy/lan8720_driver.h"
#include "dhcp/dhcp_client.h"
#include "debug.h"

#include "ftp/ftp_server.h"
#include "path.h"

#include "ftp/ftp_client.h"

//Forward declaration of functions
uint_t ftpServerCheckUserCallback(FtpClientConnection *connection,
    const char_t *user);

uint_t ftpServerCheckPasswordCallback(FtpClientConnection *connection,
    const char_t *user,
    const char_t *password);

uint_t ftpServerGetFilePermCallback(FtpClientConnection *connection,
    const char_t *user,
    const char_t *path);

error_t ftpClientTest(void);

int_t eth_init(void);
void ifconfig_update(void);
int_t eth_connect(void);
DhcpClientContext dhcpClientContext;
Socket *socketfd; //Socket 套接字 
size_t length; 

int netSend(uint8_t *pbuff, uint32_t len)
{
    error_t error; 
    error = socketSend(socketfd, pbuff, len, NULL, 0); 
    if (error) return 0; 
    return 1;
}

void vTaskTCPClient(void *pvParameters)
{
    DhcpState dhcpstate;
    error_t error; 
    char_t buffer[5000]; 
    NetInterface *interface = &netInterface[0];
//    pModem = DevModemCreate();
//    modem_open(pModem);
//    modem_init(pModem);
//    Modem_Poll(pModem);//这是任务
    eth_init();
    ifconfig_update();
    eth_connect();
    pEVSE->status.ulSignalState |= defSignalEVSE_State_Network_Online;
    xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectFail); //rgw OK
    xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectOK); //rgw OK
    ftpClientTest();
    while (1)
    {
        error = socketReceive(socketfd, buffer, sizeof(buffer) - 1, &length, 0); 
        if (error == NO_ERROR && length > 0)
        {
            pechProto->recvResponse(pechProto, pEVSE, buffer, length, 3);
        }
        vTaskDelay(100);
    }
}

int_t eth_connect(void)
{
    error_t error; 
    IpAddr ipAddr; //DNS 解析得到的地址 

    //Debug message 
    TRACE_INFO("\r\n\r\nResolving server name...\r\n"); 
 
    //Resolve HTTP server name 解析地址 
    error = getHostByName(NULL, pechProto->info.strServerIP, &ipAddr, 0); 
    //Any error to report? 
    if (error) 
    { 
        //Debug message 
        TRACE_INFO("Failed to resolve server name!\r\n"); 
        //Exit immediately 
        return error; 
    } 
    
    socketfd = socketOpen(SOCKET_TYPE_STREAM, SOCKET_IP_PROTO_TCP); 
    if (!socketfd) 
    { 
        //Debug message 
        TRACE_INFO("Failed to open socket!\r\n"); 
        //Exit immediately 
        return ERROR_OPEN_FAILED; 
    } 
    do 
    { 
        //Debug message
        TRACE_INFO("Connecting to server %s\r\n", ipAddrToString(&ipAddr, NULL)); 
 
       //Connect to the HTTP server 连接到 HTTP 服务器 
        error = socketConnect(socketfd, &ipAddr, pechProto->info.usServerPort); 
        //Any error to report? 
        if (error) break; 
 
        //Debug message 
        TRACE_INFO("Successful connection\r\n"); 
    } while (0);
}
void ifconfig_update(void)
{
    NetInterface *interface = &netInterface[0];
    Ipv4Addr ipv4Addr;
    
    ipv4GetHostAddr(interface, &ipv4Addr);
    ipv4AddrToString(ipv4Addr, ifconfig.status.strIP);
    
    ipv4GetDefaultGateway(interface, &ipv4Addr);
    ipv4AddrToString(ipv4Addr, ifconfig.status.strGate);
    
    ipv4GetSubnetMask(interface, &ipv4Addr);
    ipv4AddrToString(ipv4Addr, ifconfig.status.strMask);
    
    ipv4GetDnsServer(interface, 0, &ipv4Addr);
    ipv4AddrToString(ipv4Addr, ifconfig.status.strDNS1);
    
    ipv4GetDnsServer(interface, 1, &ipv4Addr);
    ipv4AddrToString(ipv4Addr, ifconfig.status.strDNS2);
}

FtpServerSettings ftpServerSettings;
FtpServerContext ftpServerContext;

int_t eth_init(void)
{
    DhcpClientSettings dhcpClientSettings;

    DhcpState dhcpstate;
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
        while ((dhcpstate = dhcpClientGetState(&dhcpClientContext)) != DHCP_STATE_BOUND)
        {
            printf_safe("dhcpstate = %d\n", dhcpstate);
            vTaskDelay(1000);
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
    
    
    
    ///ftp
    fsInit();
    //Get default settings
    ftpServerGetDefaultSettings(&ftpServerSettings);
    //Bind FTP server to the desired interface
    ftpServerSettings.interface = &netInterface[0];
    //Listen to port 21
    ftpServerSettings.port = FTP_PORT;
    //Root directory
    strcpy(ftpServerSettings.rootDir, YAFFS_MOUNT_POINT);
    //User verification callback function
    ftpServerSettings.checkUserCallback = ftpServerCheckUserCallback;
    //Password verification callback function
    ftpServerSettings.checkPasswordCallback = ftpServerCheckPasswordCallback;
    //Callback used to retrieve file permissions
    ftpServerSettings.getFilePermCallback = ftpServerGetFilePermCallback;

       //FTP server initialization
    error = ftpServerInit(&ftpServerContext, &ftpServerSettings);
    //Failed to initialize FTP server?
    if (error)
    {
       //Debug message
        TRACE_ERROR("Failed to initialize FTP server!\r\n");
    }

       //Start FTP server
    error = ftpServerStart(&ftpServerContext);
    //Failed to start FTP server?
    if (error)
    {
       //Debug message
        TRACE_ERROR("Failed to start FTP server!\r\n");
    }

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}

/**
 * @brief User verification callback function
 * @param[in] connection Handle referencing a client connection
 * @param[in] user NULL-terminated string that contains the user name
 * @return Access status (FTP_ACCESS_ALLOWED, FTP_ACCESS_DENIED or FTP_PASSWORD_REQUIRED)
 **/

uint_t ftpServerCheckUserCallback(FtpClientConnection *connection,
    const char_t *user)
{
   //Debug message
    TRACE_INFO("FTP server: User verification\r\n");

       //Manage authentication policy
    if (!strcmp(user, "anonymous"))
    {
        return FTP_ACCESS_ALLOWED;
    }
    else if (!strcmp(user, "admin"))
    {
        return FTP_PASSWORD_REQUIRED;
    }
    else
    {
        return FTP_ACCESS_DENIED;
    }
}


/**
 * @brief Password verification callback function
 * @param[in] connection Handle referencing a client connection
 * @param[in] user NULL-terminated string that contains the user name
 * @param[in] password NULL-terminated string that contains the corresponding password
 * @return Access status (FTP_ACCESS_ALLOWED or FTP_ACCESS_DENIED)
 **/

uint_t ftpServerCheckPasswordCallback(FtpClientConnection *connection,
    const char_t *user,
    const char_t *password)
{
   //Debug message
    TRACE_INFO("FTP server: Password verification\r\n");

       //Verify password
    if (!strcmp(user, "admin") && !strcmp(password, "admin"))
    {
        return FTP_ACCESS_ALLOWED;
    }
    else
    {
        return FTP_ACCESS_DENIED;
    }
}


/**
 * @brief Callback used to retrieve file permissions
 * @param[in] connection Handle referencing a client connection
 * @param[in] user NULL-terminated string that contains the user name
 * @param[in] path Canonical path of the file
 * @return Permissions for the specified file
 **/

uint_t ftpServerGetFilePermCallback(FtpClientConnection *connection,
    const char_t *user,
    const char_t *path)
{
    uint_t perm;

       //Debug message
    TRACE_INFO("FTP server: Checking access rights for %s\r\n", path);

       //Manage access rights
    if (!strcmp(user, "anonymous"))
    {
       //Allow read/write access to temp directory
        if (pathMatch(path, "/temp/*"))
            perm = FTP_FILE_PERM_LIST | FTP_FILE_PERM_READ | FTP_FILE_PERM_WRITE;
      //Allow read access only to other directories
        else
            perm = FTP_FILE_PERM_LIST | FTP_FILE_PERM_READ;
    }
    else if (!strcmp(user, "admin"))
    {
       //Allow read/write access
        perm = FTP_FILE_PERM_LIST | FTP_FILE_PERM_READ | FTP_FILE_PERM_WRITE;
    }
    else
    {
       //Deny access
        perm = 0;
    }

       //Return the relevant permissions
    return perm;
}


/**
 * @brief FTP client test routine
 * @return Error code
 **/

error_t ftpClientTest(void)
{
    error_t error;
    size_t length;
    IpAddr ipAddr;
    FtpClientContext ftpContext;
    static char_t buffer[256];

       //Debug message
    TRACE_INFO("\r\n\r\nResolving server name...\r\n");
    //Resolve FTP server name
    error = getHostByName(NULL, "ftp.gnu.org", &ipAddr, 0);

       //Any error to report?
    if (error)
    {
       //Debug message
        TRACE_INFO("Failed to resolve server name!\r\n");
        //Exit immediately
        return error;
    }

       //Debug message
    TRACE_INFO("Connecting to FTP server %s\r\n", ipAddrToString(&ipAddr, NULL));
    //Connect to the FTP server
    error = ftpConnect(&ftpContext, NULL, &ipAddr, 21, FTP_NO_SECURITY | FTP_PASSIVE_MODE);

       //Any error to report?
    if (error)
    {
       //Debug message
        TRACE_INFO("Failed to connect to FTP server!\r\n");
        //Exit immediately
        return error;
    }

       //Debug message
    TRACE_INFO("Successful connection\r\n");

       //Start of exception handling block
    do
    {
       //Login to the FTP server using the provided username and password
        error = ftpLogin(&ftpContext, "anonymous", "password", "");
        //Any error to report?
        if (error)
            break;

                  //Open the specified file for reading
        error = ftpOpenFile(&ftpContext, "welcome.msg", FTP_FOR_READING | FTP_BINARY_TYPE);
        //Any error to report?
        if (error)
            break;

                  //Dump the contents of the file
        while (1)
        {
           //Read data
            error = ftpReadFile(&ftpContext, buffer, sizeof(buffer) - 1, &length, 0);
            //End of file?
            if (error)
                break;

                         //Properly terminate the string with a NULL character
            buffer[length] = '\0';
            //Dump current data
            TRACE_INFO("%s", buffer);
        }

              //End the string with a line feed
        TRACE_INFO("\r\n");
        //Close the file
        error = ftpCloseFile(&ftpContext);

              //End of exception handling block
    } while (0);

       //Close the connection
    ftpClose(&ftpContext);
    //Debug message
    TRACE_INFO("Connection closed...\r\n");

       //Return status code
    return error;
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
