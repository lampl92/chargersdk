#include "interface_network.h"
#include "evse_globals.h"

#include "drivers/mac/stm32f4x9_eth_driver.h"
#include "drivers/phy/lan8720_driver.h"
#include "dhcp/dhcp_client.h"
#include "debug.h"

DhcpClientContext dhcpClientContext;
Socket *socket_eth; //Socket 套接字 

error_t net_eth_connect(void *pvnet_dev)
{
    error_t error;
    
    socket_eth = netif_connect_server_via_tcp(net_dev, pechProto->info.strServerIP, pechProto->info.usServerPort, &error);
    
    return error;
}

error_t net_eth_reconnect(void *pvnet_dev)
{
    error_t error;
    
    socketClose(socket_eth);
    error = net_eth_connect(pvnet_dev);
    
    return error;
}
error_t net_eth_disconnect(void *pvnet_dev)
{
    socketClose(socket_eth);
    socket_eth = NULL;
    return NO_ERROR;
}
error_t net_eth_close_hard(void *pvnet_dev)
{
    PAout(8) = 0;
    bsp_DelayMS(500);
    PAout(8) = 1;
    bsp_DelayMS(500);
    
    return NO_ERROR;
}
/**
 * @fn  error_t net_eth_init(net_device_t *net_dev, uint32_t n)
 *
 * @brief   Net eth initialize
 *
 * @param [out]  net_dev If non-null, the net device.
 * @param           n       the netcard number.
 *
 * @return  An error_t.
 */

error_t net_eth_init(void *pvnet_dev)
{
    net_device_t *net_dev;
    DhcpClientSettings dhcpClientSettings;

    DhcpState dhcpstate;
    error_t error;
    NetInterface *interface;
    MacAddr macAddr;
    Ipv4Addr ipv4Addr;
    
    net_dev = (net_device_t*)pvnet_dev;
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
        time_t dhcp_start = time(NULL);
        uint32_t dhcp_waittime = 0;
        while ((dhcpstate = dhcpClientGetState(&dhcpClientContext)) != DHCP_STATE_BOUND)
        {
            dhcp_waittime = time(NULL) - dhcp_start;

            if (interface->linkState)
            {
                if (dhcp_waittime > 60)
                {
                    TRACE_ERROR("Link up & DHCP客户端超时!\n");
                    dhcp_waittime = 0;
                    return ERROR_FAILURE;
                }
            }
            else//LINK DOWN 
            {            
                if (dhcp_waittime > 15)
                {
                    TRACE_ERROR("Link down & DHCP客户端超时!\n");
                    dhcp_waittime = 0;
                    return ERROR_FAILURE;
                }
            }
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
    
    return error;
}

net_device_t *net_eth_create(void)
{
    net_device_t *net;
    net = (net_device_t*)malloc(sizeof(net_device_t));
    memset(net, 0, sizeof(net_device_t));
    sprintf(net->name, "eth%d", 0);
    
    net->init = net_eth_init;
    net->connect = net_eth_connect;
    net->disconnect = net_eth_disconnect;
    net->close_hard = net_eth_close_hard;
    
    return net;
}
