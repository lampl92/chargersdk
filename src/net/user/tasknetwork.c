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
#include "errorcode.h"

#include "taskcreate.h"
#include "interface_network.h"
#include "interface_ftpclient.h"
#include "interface_ftpserver.h"
#include "net_device.h"
#include "cfg_parse.h"
#include "utils.h"
#include "yaffsfs.h"
#include "yaffs2misc.h"
#include "file_op.h"
#include "evse_globals.h"

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
        "NET_STATE_DISCONNECT",
        "NET_STATE_ERR"
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
    
    error = net_dev->init(net_dev);
    if (error == NO_ERROR)
    {
        ifconfig_update(net_dev);
        netChangeState(net_dev, NET_STATE_CONNECT);
    }
    else
    {
        netChangeState(net_dev, NET_STATE_ERR);
    }
}
static void netStateConnect(net_device_t *net_dev)
{
    error_t error; 

    if (pechProto->info.ftp.ucDownloadStart == 1)
    {
        netChangeState(net_dev, NET_STATE_FTP);
        return;
    }
    error = net_dev->connect(net_dev);
    if (error == NO_ERROR)
    {
        netChangeState(net_dev, NET_STATE_TCP_ON);
    }
    else
    {
        netChangeState(net_dev, NET_STATE_ERR);
//        vTaskDelay(10000);
    }
}
static void netStateDisconnect(net_device_t *net_dev)
{
    net_dev->disconnect(net_dev);
    netChangeState(net_dev, NET_STATE_CONNECT);
}
static void netStateTcpOn(net_device_t *net_dev)
{
    uint8_t buffer[5000]; 
    size_t length; 
    error_t error; 
    int i;
    
    EventBits_t uxBit;
    
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
    if (net_dev->interface->linkState)
    {
        
    }
    else
    {
        //Link is down
        netChangeState(net_dev, NET_STATE_DISCONNECT);
    }
}
static void netStateFTP(net_device_t *net_dev)
{
    uint32_t crc32_calc, crc32_orig;
    char ch_crc32[9] = { 0 };
    filelist_e filelist;
    ul2uc ul2ucCrc32;
    char filepath[64 + 1];
    char filepath_rename[64 + 1];
    char upflg;
    int i;
    flist_t flist;
    EchFtpCfg_t ftpcfg;
    ErrorCode_t errcode;
    int res, res_copy;
    
    res = ftp_download_file(&pechProto->info.ftp, net_dev);
 
    if (res == 1)
    {
        taskappSuspend();
#if EVSE_USING_GUI
        vTaskDelay(1500);
        LCD_Clear(BLUE);
#endif
        
        while (parse_flist(pathDownloadList, &ftpcfg, &flist) == ERR_NO)
        {
            res = ftp_download_file(&ftpcfg, net_dev);
            if (res != 1)
            {
                upflg = '3';
                set_tmp_file(pathUpgradeTmp, &upflg);
                break;
            }
            sprintf(filepath, "%s%s", pathDownloadDir, flist.strFilename);
            GetFileCrc32(filepath, &crc32_calc);
            if (crc32_calc == StrCrc32ToUint32(flist.strCrc32))
            {
                printf_safe("下载%s成功\n", filepath);
                sprintf(filepath_rename, "%s%s", flist.strLocalpath, flist.strFilename);
                res_copy = copy_in_a_file(filepath_rename, filepath);//拷贝下载文件到目的地址
                if (res_copy == 1)
                {
                    printf_safe("移动%s到%s.\n", filepath, filepath_rename);
                }
                else
                {
                    printf_safe("移动%s到%s 失败.\n", filepath, filepath_rename);
                    upflg = '3';
                    set_tmp_file(pathUpgradeTmp, &upflg);
                    yaffs_unlink(filepath); //删除下载文件
                    break;
                }
            }
            else
            {
                printf_safe("CRC32校验失败 %s\n", filepath);
                upflg = '3';
                set_tmp_file(pathUpgradeTmp, &upflg);
                yaffs_unlink(filepath); //删除下载文件
                break;
            }
            yaffs_unlink(filepath);//删除下载文件
        }
        yaffs_unlink(pathDownloadList);

        pechProto->info.ftp.ucDownloadStart = 0;
        cfg_set_uint8(pathFTPCfg, &pechProto->info.ftp.ucDownloadStart, "%s", jnFtpDownloadStart);
        NVIC_SystemReset();
    }
    else
    {
        pechProto->info.ftp.ucDownloadStart = 0;
        cfg_set_uint8(pathFTPCfg, &pechProto->info.ftp.ucDownloadStart, "%s", jnFtpDownloadStart);
        netChangeState(net_dev, NET_STATE_CONNECT);
    }
}
static void netStateErr(net_device_t *net_dev)
{
    net_dev->close_hard(net_dev);
    netChangeState(net_dev, NET_STATE_INIT);
}

void ifconfig_change_for_ftp(void)
{
    ifconfig.info.ucAdapterSel = 1;
    ifconfig.info.ucDHCPEnable = 1;
    sprintf(ifconfig.info.strHostName, "startup");
    sprintf(ifconfig.info.strMAC, "00-AB-CD-EF-04-29");
}

extern error_t smtpClientTest(void);
void vTaskTCPClient(void *pvParameters)
{
#if EVSE_USING_GUI
    char flg;

    flg = get_bmp_check_tmp();
    if (flg == 3)
    {
        ifconfig_change_for_ftp();
    }
#endif
    
    net_dev = net_device_create(ifconfig.info.ucAdapterSel);

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
        case NET_STATE_ERR:
            netStateErr(net_dev);
            break;
        default:
            netChangeState(net_dev, NET_STATE_IDLE);
            break;
        }

        vTaskDelay(100);
    }
}
