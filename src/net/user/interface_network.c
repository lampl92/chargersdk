#include "evse_globals.h"
#include "ifconfig.h"
#include "net_device.h"
#include "net_eth.h"

#include "core/net.h"
#include "debug.h"

int netSend(uint8_t *pbuff, uint32_t len)
{
    error_t error; 
    error = socketSend(socket_eth, pbuff, len, NULL, 0); 
    if (error) return 0; 
    return 1;
}

int netRecv(uint8_t *pbuff, uint32_t buff_len)
{
    size_t recv_len;
    error_t error; 
    error = socketReceive(socket_eth, pbuff, buff_len, &recv_len, 0); 
    if (error) return 0; 
    return recv_len;
}

Socket *net_connect_server_via_tcp(net_device_t *net_dev, const char *server_ip, uint16_t port, error_t *perror)
{
    error_t error; 
    IpAddr ipAddr; //DNS 解析得到的地址 
    Socket *socketfd;
 
    TRACE_INFO("\r\n\r\n解析域名...\r\n"); 
 
    //解析地址 
    error = getHostByName(net_dev->interface, server_ip, &ipAddr, 0); 
    if (error) 
    { 
        TRACE_INFO("解析域名失败!\r\n"); 
        *perror = error;
        return NULL; 
    } 
    
    socketfd = socketOpen(SOCKET_TYPE_STREAM, SOCKET_IP_PROTO_TCP); 
    if (!socketfd) 
    { 
        TRACE_INFO("Socket 打开失败!\r\n"); 
        *perror = ERROR_OPEN_FAILED;
        return NULL; 
    } 
    do 
    { 
        TRACE_INFO("连接到服务器 %s:%d\r\n", ipAddrToString(&ipAddr, NULL), port); 
        error = socketConnect(socketfd, &ipAddr, port); 
        if (error) 
            break; 
        TRACE_INFO("连接成功\r\n"); 
    } while (0);
    *perror = error;
    return socketfd;
}

void ifconfig_update(net_device_t *net_dev)
{
    NetInterface *interface = net_dev->interface;
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

/**
 * @fn  error_t net_stack_init(void)
 *
 * @brief   Net stack initialize
 *
 * @return  An error_t.
 */

error_t net_stack_init(void)
{
    error_t error;
    
    error = netInit();
    if (error)
    {
        TRACE_ERROR("初始化 TCP/IP 协议栈失败!\r\n");
    }
    return error;
}
