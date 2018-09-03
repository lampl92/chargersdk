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

static uint32_t delay_ms = 1000; // 重连时间, 如果重连不成功, 时间加1000ms, 最大600000(10分钟), 重连成功后变为1000
static uint32_t start_delay_ms;
void flush_reconnect_time(void)
{
    delay_ms = 1000;
    start_delay_ms = 0;
}
uint32_t get_reconnect_remain_time(void)
{
    if (start_delay_ms != 0)
    {
        if((int)(delay_ms - (clock() - start_delay_ms)) < 0)
            return 0;
        return delay_ms - (clock() - start_delay_ms); 
    }
    else
    {
        return 0;
    }
}
static void inc_reconnect_time(void)
{
    start_delay_ms = clock();
    if(delay_ms == 600000)
        return;
    delay_ms += 1000;
}

void netChangeState(net_device_t *net_dev, net_state new_state)
{
    static const char_t *stateLabel[] =
    {
        "NET_STATE_IDLE",
        "NET_STATE_INIT",
        "NET_STATE_CONNECT",
        "NET_STATE_FTP",
        "NET_STATE_FTPServer",
        "NET_STATE_TCP_ON",
        "NET_STATE_DISCONNECT",
        "NET_STATE_ERR"
    };

    if ((int)(net_dev->state) < arraysize(stateLabel) && (int)new_state < arraysize(stateLabel))
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
        break;
    case NET_STATE_DISCONNECT:
        pEVSE->status.ulSignalState &= ~defSignalEVSE_State_Network_Online;
        break;
    default:
        break;
    }
    
    net_dev->state = new_state;
}
static void netStateInit(net_device_t *net_dev)
{
    error_t error; 
    uint32_t n = 0;//接口号
    char flg;
    
    //初始化网络设备
    net_dev->interface = &netInterface[n];
    
    error = net_dev->init(net_dev);
    if (error == NO_ERROR)
    {
        ifconfig_update(net_dev);

        flg = get_bmp_check_tmp();
        if (flg == 3)
        {
            netChangeState(net_dev, NET_STATE_FTPServer);
        }
        else
        {
            netChangeState(net_dev, NET_STATE_CONNECT);
        }
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
        flush_reconnect_time();
        netChangeState(net_dev, NET_STATE_TCP_ON);
    }
    else
    {
        netChangeState(net_dev, NET_STATE_ERR);
    }
}
static void netStateDisconnect(net_device_t *net_dev)
{
    net_dev->disconnect(net_dev);
    netChangeState(net_dev, NET_STATE_CONNECT);
}
static void netStateTcpOn(net_device_t *net_dev)
{
    uint8_t buffer[1500]; 
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
EchFtpCfg_t ftpcfg;
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
    ErrorCode_t errcode;
    int res, res_copy;
    
    pechProto->info.ftp.ucDownloadStatus = 1;
    cfg_set_uint8(pathFTPCfg, &pechProto->info.ftp.ucDownloadStatus, "%s", jnFtpDownloadStatus);
    res = ftp_download_file(&pechProto->info.ftp, net_dev);
 
    if (res == 1)
    {
        taskappSuspend();
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
                    pechProto->info.ftp.ucDownloadStatus = 2;
                    cfg_set_uint8(pathFTPCfg, &pechProto->info.ftp.ucDownloadStatus, "%s", jnFtpDownloadStatus);
                    printf_safe("移动%s到%s.\n", filepath, filepath_rename);
                }
                else
                {
                    pechProto->info.ftp.ucDownloadStatus = 3;
                    cfg_set_uint8(pathFTPCfg, &pechProto->info.ftp.ucDownloadStatus, "%s", jnFtpDownloadStatus);
                    printf_safe("移动%s到%s 失败.\n", filepath, filepath_rename);
                    upflg = '3';
                    set_tmp_file(pathUpgradeTmp, &upflg);
                    yaffs_unlink(filepath); //删除下载文件
                    break;
                }
            }
            else
            {
                pechProto->info.ftp.ucDownloadStatus = 3;
                cfg_set_uint8(pathFTPCfg, &pechProto->info.ftp.ucDownloadStatus, "%s", jnFtpDownloadStatus);
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
        pechProto->info.ftp.ucDownloadStatus = 3;
        cfg_set_uint8(pathFTPCfg, &pechProto->info.ftp.ucDownloadStatus, "%s", jnFtpDownloadStatus);
        pechProto->info.ftp.ucDownloadStart = 0;
        cfg_set_uint8(pathFTPCfg, &pechProto->info.ftp.ucDownloadStart, "%s", jnFtpDownloadStart);
        netChangeState(net_dev, NET_STATE_CONNECT);
    }
}
static void netStateFTPServer(net_device_t *net_dev)
{
    #include "stm32f4xx_hal_adc.h"
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_Stop_DMA(&hadc1);
    net_ftp_server_start(net_dev);
    xEventGroupSetBits(xHandleEventHMI, defEventBitHMI_REQ_StartFTP);
    netChangeState(net_dev, NET_STATE_IDLE);
}
static void netStateErr(net_device_t *net_dev)
{
    inc_reconnect_time();
    vTaskDelay(delay_ms);
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
        case NET_STATE_FTPServer:
            netStateFTPServer(net_dev);
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
