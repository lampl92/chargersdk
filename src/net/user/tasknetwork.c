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
#include "net_device.h"
#include "cfg_parse.h"
#include "utils.h"
#include "yaffsfs.h"

#include "debug.h"
#include "evse_debug.h"

void netChangeState(net_device_t *net_dev, net_state new_state)
{
    static const char_t *stateLabel[] =
    {
        "NET_STATE_IDLE",
        "NET_STATE_INIT",
        "NET_STATE_CONNECT",
        "NET_STATE_FTP",
        "NET_STATE_TCP_ON",
        "NET_STATE_DISCONNECT"
    };

    if (net_dev->state < arraysize(stateLabel) && new_state < arraysize(stateLabel))
    {
        TRACE_INFO("TASK NET FSM: %s (%u) -> %s (%u)\r\n",
            stateLabel[net_dev->state],
            net_dev->state,
            stateLabel[new_state],
            new_state);
    }
    
    switch (new_state)
    {
    case NET_STATE_TCP_ON:
        pEVSE->status.ulSignalState |= defSignalEVSE_State_Network_Online;
        xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectOK); //rgw OK
        break;
    case NET_STATE_DISCONNECT:
        pEVSE->status.ulSignalState &= ~defSignalEVSE_State_Network_Online;
        xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectOK); //rgw OK
        break;
    }
    
    net_dev->state = new_state;
}
static void netStateInit(net_device_t *net_dev)
{
    error_t error; 
    uint32_t n = 0;//接口号
    
    //初始化网络设备
    net_dev->interface = &netInterface[n];
    sprintf(net_dev->name, "eth%d", n);
    
    error = net_dev_init(net_dev);
    if (error == NO_ERROR)
    {
        netChangeState(net_dev, NET_STATE_CONNECT);
    }
}
static void netStateFTP(net_device_t *net_dev)
{
    uint32_t crc32_calc, crc32_orig;
    double dcrc32_calc;
    char ch_crc32[9] = { 0 };
    ul2uc ul2ucCrc32;
    char filepath[64 + 1];
    int i;
    
    int res;
    
    res = ftp_download_file(&pechProto->info.ftp, net_dev);
    if (res == 1)
    {
        for (i = 0; i < 8; i++)
        {
            ch_crc32[i] = pechProto->info.ftp.strNewFileName[i + 2];
        }
        sprintf(filepath, "%s%s", pathSystemDir, pechProto->info.ftp.strNewFileName);
        StrToHex(ch_crc32, ul2ucCrc32.ucVal, strlen(ch_crc32));
        crc32_orig = ntohl(ul2ucCrc32.ulVal);
        GetFileCrc32(filepath, &crc32_calc);
        printf_safe("crc32_calc = %x\n", crc32_calc);
        printf_safe("crc32_orgi = %x\n", crc32_orig);
        if (crc32_calc == crc32_orig)
        {
            taskENTER_CRITICAL();
            dcrc32_calc = (uint32_t)crc32_calc;
            cfg_set_uint32(pathSysCfg, &crc32_calc, "%s", jnSysChargersdk_bin_crc32);
//            xSysconf.SetSysCfg(jnSysChargersdk_bin_crc32, (void*)&dcrc32_calc, ParamTypeDouble);
            yaffs_unlink(pathBin);
            yaffs_rename(filepath, pathBin);
            taskEXIT_CRITICAL();
            xSysconf.xUpFlag.chargesdk_bin = 1;
            pechProto->info.ftp.ucDownloadStart = 0;
            cfg_set_uint8(pathSysCfg, &xSysconf.xUpFlag.chargesdk_bin, "%s", jnSysChargersdk_bin);
            cfg_set_uint8(pathFTPCfg, &pechProto->info.ftp.ucDownloadStart, "%s", jnFtpDownloadStart);
            NVIC_SystemReset();
        }
    }
        
    if (res != 0)
    {
        netChangeState(net_dev, NET_STATE_TCP_ON);
    }
}
static void netStateDisconnect(net_device_t *net_dev)
{
    net_dev_disconnect();
    netChangeState(net_dev, NET_STATE_CONNECT);
}
static void netStateTcpOn(net_device_t *net_dev)
{
    char_t buffer[5000]; 
    size_t length; 
    error_t error; 
    int i;
    
    EventBits_t uxBit;
    
    if (pechProto->info.ftp.ucDownloadStart == 1)
    {
        netChangeState(net_dev, NET_STATE_FTP);
        return;
    }
    
    length = netRecv(buffer, sizeof(buffer));
    if (length > 0)
    {
        pechProto->recvResponse(pechProto, pEVSE, buffer, length, 3);
        {
            printf_protodetail("\nTCP Recv: ");
            for (i = 0; i < length; i++)
            {
                printf_protodetail("%02X ", buffer[i]);
            }
            printf_protodetail("\n"); 
        }
        
    }
    
    uxBit = xEventGroupWaitBits(xHandleEventRemote, defEventBitRemoteError, pdTRUE, pdTRUE, 0);
    if ((uxBit & defEventBitRemoteError) == defEventBitRemoteError)
    {
        netChangeState(net_dev, NET_STATE_DISCONNECT);
    }
}
static void netStateConnect(net_device_t *net_dev)
{
    error_t error; 
    ifconfig_update(net_dev);
    error = net_dev_connect();
    if (error == NO_ERROR)
    {
        netChangeState(net_dev, NET_STATE_TCP_ON);
    }
    else
    {
        vTaskDelay(10000);
    }
}
extern error_t smtpClientTest(void);
void vTaskTCPClient(void *pvParameters)
{
    net_device_t *net_dev;

    net_dev = get_net_device_handler(ifconfig.info.ucAdapterSel);

    netChangeState(net_dev, NET_STATE_INIT);

    while (1)
    {
        switch (net_dev->state)
        {
        case NET_STATE_IDLE:
            break;
        case NET_STATE_INIT:
            netStateInit(net_dev);
            break;
        case NET_STATE_CONNECT:
            netStateConnect(net_dev);
            break;
        case NET_STATE_TCP_ON:
            netStateTcpOn(net_dev);
            break;
        case NET_STATE_DISCONNECT:
            netStateDisconnect(net_dev);
            break;
        case NET_STATE_FTP:
            netStateFTP(net_dev);
            break;
        default:
            netChangeState(net_dev, NET_STATE_IDLE);
            break;
        }

        vTaskDelay(100);
    }
}
