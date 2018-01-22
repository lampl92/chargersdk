#include "evse_globals.h"
#include "ifconfig.h"
#include "net_device.h"

#include "core/net.h"
#include "drivers/mac/stm32f4x9_eth_driver.h"
#include "drivers/phy/lan8720_driver.h"
#include "dhcp/dhcp_client.h"
#include "debug.h"

DhcpClientContext dhcpClientContext;
Socket *socketfd; //Socket 套接字 

int netSend(uint8_t *pbuff, uint32_t len)
{
    error_t error; 
    error = socketSend(socketfd, pbuff, len, NULL, 0); 
    if (error) return 0; 
    return 1;
}

int netRecv(uint8_t *pbuff, uint32_t buff_len)
{
    size_t recv_len;
    error_t error; 
    error = socketReceive(socketfd, pbuff, buff_len, &recv_len, 0); 
    if (error) return 0; 
    return recv_len;
}

int_t eth_connect(net_device_t *net_dev)
{
    error_t error; 
    IpAddr ipAddr; //DNS 解析得到的地址 
 
    TRACE_INFO("\r\n\r\n解析域名...\r\n"); 
 
    //Resolve HTTP server name 解析地址 
    error = getHostByName(NULL, pechProto->info.strServerIP, &ipAddr, 0); 
    //Any error to report? 
    if (error) 
    { 
        TRACE_INFO("Failed to resolve server name!\r\n"); 
        return error; 
    } 
    
    socketfd = socketOpen(SOCKET_TYPE_STREAM, SOCKET_IP_PROTO_TCP); 
    if (!socketfd) 
    { 
        TRACE_INFO("Failed to open socket!\r\n"); 
        return ERROR_OPEN_FAILED; 
    } 
    do 
    { 
        TRACE_INFO("连接到服务器 %s\r\n", ipAddrToString(&ipAddr, NULL)); 
        error = socketConnect(socketfd, &ipAddr, pechProto->info.usServerPort); 
        if (error) 
            break; 
        TRACE_INFO("连接成功\r\n"); 
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

int_t eth_init(net_device_t *net_dev)
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
        return error;
    }

    //设置第一个以太网接口
    interface = net_dev->interface;

    //设置接口名称
    netSetInterfaceName(interface, net_dev->name);
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
        TRACE_ERROR("配置接口 %s 失败!code = %d\r\n", interface->name, error);
        return error;
    }

    if (ifconfig.info.ucDHCPEnable == 1)
    {
        dhcpClientGetDefaultSettings(&dhcpClientSettings);
        dhcpClientSettings.interface = interface;
        error = dhcpClientInit(&dhcpClientContext, &dhcpClientSettings);
        if (error)
        {
            TRACE_ERROR("DHCP客户端初始化失败!code = %d\r\n", error);
            return error;
        }

        //启动DHCP客户端
        error = dhcpClientStart(&dhcpClientContext);
        if (error)
        {
            TRACE_ERROR("启动DHCP客户端失败! code = %d\r\n", error);
            return error;
        }
        while ((dhcpstate = dhcpClientGetState(&dhcpClientContext)) != DHCP_STATE_BOUND)
        {
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
    
    return 0;
}
