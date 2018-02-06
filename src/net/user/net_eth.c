#include "interface_network.h"
#include "evse_globals.h"

#include "drivers/mac/stm32f4x9_eth_driver.h"
#include "drivers/phy/lan8720_driver.h"
#include "dhcp/dhcp_client.h"
#include "debug.h"

net_device_t eth_dev;
DhcpClientContext dhcpClientContext;
Socket *socket_eth; //Socket 套接字 

error_t net_eth_connect(void)
{
    error_t error;
    
    socket_eth = net_connect_server_via_tcp(&eth_dev, pechProto->info.strServerIP, pechProto->info.usServerPort, &error);
    
    return error;
}

error_t net_eth_reconnect(void)
{
    error_t error;
    
    socketClose(socket_eth);
    
    error = net_eth_connect();
    
    return error;
}
void net_eth_disconnect(void)
{
    socketClose(socket_eth);
    socket_eth = NULL;
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

error_t net_eth_init(net_device_t *net_dev)
{
    DhcpClientSettings dhcpClientSettings;

    DhcpState dhcpstate;
    error_t error;
    NetInterface *interface;
    OsTask *task;
    MacAddr macAddr;
    Ipv4Addr ipv4Addr;
  
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
