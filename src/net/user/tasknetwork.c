/**
* @file tasknetwork.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-27
*/
#include "FreeRTOS.h"
#include "task.h"

#include "modem.h"
#include "cfg_parse.h"
#include "stringName.h"

#include "taskcreate.h"
#include "interface_network.h"
#include "interface_ftpclient.h"
#include "interface_ftpserver.h"
#include "net_eth.h"

void vTaskTCPClient(void *pvParameters)
{
    net_device_t *net_dev;
    DhcpState dhcpstate;
    error_t error; 
    char_t buffer[5000]; 
    size_t length; 

    net_dev = &eth_dev;
    error = net_eth_init(net_dev, 0);
    
    pEVSE->status.ulSignalState |= defSignalEVSE_State_Network_Online;
    xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectFail); //rgw OK
    xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectOK); //rgw OK
    if (pechProto->info.ftp.ucDownloadStart == 1)
    {
        ftp_download_file(&pechProto->info.ftp, net_dev);
    }

    while (1)
    {
        switch (net_dev->state)
        {
        case NET_STATE_INIT:
            ifconfig_update(net_dev);
            error = net_eth_connect();
            if (error == NO_ERROR)
            {
                net_dev->state = NET_STATE_CONNECT;
            }
            break;
        case NET_STATE_CONNECT:
            break;
        case NET_STATE_WORKING:
            break;
        default:
            break;
        }
        length = netRecv(buffer, sizeof(buffer));
        if (length > 0)
        {
            pechProto->recvResponse(pechProto, pEVSE, buffer, length, 3);
        }
        vTaskDelay(100);
    }
}
