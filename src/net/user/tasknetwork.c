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
    DhcpState dhcpstate;
    error_t error; 
    char_t buffer[5000]; 
    size_t length; 

    error = net_eth_init(&eth_dev, 0);
    ifconfig_update(&eth_dev);
    error = net_eth_connect();
    pEVSE->status.ulSignalState |= defSignalEVSE_State_Network_Online;
    xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectFail); //rgw OK
    xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectOK); //rgw OK
    if (pechProto->info.ftp.ucDownloadStart == 1)
    {
        ftp_download_file(&pechProto->info.ftp, &eth_dev);
    }

    while (1)
    {
        length = netRecv(buffer, sizeof(buffer));
        if (length > 0)
        {
            pechProto->recvResponse(pechProto, pEVSE, buffer, length, 3);
        }
        vTaskDelay(100);
    }
}
