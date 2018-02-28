#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "modem.h"
#include "bsp.h"
#include "bsp_define.h"

#include "taskcreate.h"
#include "evse_define.h"
#include "utils.h"
#include "stringName.h"
#include "cfg_parse.h"
#include "evse_debug.h"

#include "FreeRTOS.h"
#include "event_groups.h"

#include "yaffsfs.h"

#include "quectel_m26.h"
#include "quectel_uc15.h"

#include "ppp/ppp_hdlc.h"

#ifdef EVSE_DEBUG

#undef   GPRS_set
#undef   GPRS_reset
#define GPRS_set        PBout(3) = 1
#define GPRS_reset      PBout(3) = 0

#endif

int modemlog = 1;

#define TCP_CLIENT_BUFSIZE           MAX_COMMAND_LEN
#define QUE_BUFSIZE                  5000
DevModem_t *pModem;
uint32_t ulTaskDelay_ms = 100;

uint8_t  tcp_client_recvbuf[TCP_CLIENT_BUFSIZE]; //TCP客户端接收数据缓冲区

uint32_t recv_len = 0;

void modem_delayms(int ms)
{
    vTaskDelay(ms);
}

void modem_enQue(uint8_t *pbuff, uint32_t len)
{
    int i;
#if USE_FreeRTOS
    if (xSemaphoreTake(pModem->pSendQue->xHandleMutexQue, 300) == pdPASS)
    {
#endif
        for (i = 0; i < len; i++)
        {
            pModem->pSendQue->EnElem(pModem->pSendQue, pbuff[i]);
        }
#if USE_FreeRTOS
        xSemaphoreGive(pModem->pSendQue->xHandleMutexQue);            
    }
#endif
}

static void modem_UART_putQue(DevModem_t *pModem)
{
    uint8_t ch; //这里需要测试是单个字符发送还是用while全部发送后再give mutex
#if USE_FreeRTOS
    if (xSemaphoreTake(pModem->pSendQue->xHandleMutexQue, 300) == pdPASS)
    {
#endif
	    if (pModem->pSendQue->isEmpty(pModem->pSendQue) != QUE_TRUE)
	    {
			printf_protodetail("PPP Send: ");
	    }
        while (pModem->pSendQue->isEmpty(pModem->pSendQue) != QUE_TRUE)
        {
            pModem->pSendQue->DeElem(pModem->pSendQue, &ch);
            printf_protodetail("%02X ", ch);
            gprs_uart_putc(ch);
        }
        printf_protodetail("\n");
#if USE_FreeRTOS
        xSemaphoreGive(pModem->pSendQue->xHandleMutexQue);            
    }
#endif
}

static uint32_t modem_UART_puts(uint8_t *pbuff, uint32_t len)
{
    return uart_write(UART_PORT_GPRS, pbuff, len);
}

static uint32_t modem_UART_gets(DevModem_t *pModem, uint8_t *rbuff, uint32_t len)
{
    return uart_read(UART_PORT_GPRS, rbuff, len, 100);
}

uint32_t modem_send_at(uint8_t *format, ...)
{
    uint8_t     cmd[MAX_COMMAND_LEN + 1]  = {0};
    va_list  va;
    uint32_t      n;


    va_start(va, format);
    n  = vsnprintf(cmd, sizeof(cmd) - 1, format, va);
    va_end(va);

//    cmd[strlen(cmd)] = '\n';
    modem_UART_puts(cmd, strlen(cmd));

    cmd[strlen(cmd) - 1]  = '\0';
    printf_modem("%s", cmd);

    return n;
}


/**
 * modem_get_at_reply
 *
 * @param      reply   The reply
 * @param      len     The length
 * @param      key     关键词
 * @param      second  延时时间
 *
 * @return     { description_of_the_return_value }
 */
DR_MODEM_e modem_get_at_reply(uint8_t *reply, uint32_t len, const uint8_t *key, uint32_t second)
{
    uint8_t  *p;
    uint32_t  time;
    uint32_t n;
    DR_MODEM_e    ret;

    time  = 0;
    while (1)
    {
        n  = modem_UART_gets(pModem, reply, len);
        if ( n > 0 )
        {
            //优先判断这两个模块会主动发出的命令
            p  = strstr(reply, "CLOSED");
            if ( p )
            {
                pModem->state = DS_MODEM_TCP_CLOSE;
                ret = DR_MODEM_CLOSED;
                break;
            }
            p  = strstr(reply, "+QIRDI:");
            if ( p )
            {
                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientRecvValid);
                ret = DR_MODEM_READ;
                break;
            }
            if(key == NULL)
            {
                ret = DR_MODEM_OK;
                break;
            }
            p  = strstr(reply, key);
            if ( p )
            {
                ret  = DR_MODEM_OK;
                break;
            }
            p  = strstr(reply, "ERROR");
            if ( p )
            {
                ret  = DR_MODEM_ERROR;
                break;
            }
            memset(reply, '\0', n);
            n  = 0;
        }

        time += 100;
        if ( time > second * 1000 )
        {
            ret  = DR_MODEM_TIMEOUT;
            break;
        }

        vTaskDelay(100);
    }

    printf_modem("%s\r\n\r\n", reply);
    return ret;
}


/** @brief 发送数据，超时时间20s
 *
 * @param pModem DevModem_t*
 * @param pbuff uint8_t*
 * @param len uint32_t
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e modem_write(DevModem_t *pModem, uint8_t *pbuff, uint32_t len)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint32_t n;
    DR_MODEM_e ret;

    n = 0;
    ret = DR_MODEM_ERROR;

    n = modem_UART_puts(pbuff, len);
    if (n == len)
    {
        ret = DR_MODEM_OK;
    }
    else
    {
        ret = DR_MODEM_TIMEOUT;
    }

    return ret;
}
uint32_t modem_read(DevModem_t *pModem, uint8_t *pbuff, uint32_t len)
{
    return modem_UART_gets(pModem, pbuff, len);
}

DevModem_t *DevModemCreate(void)
{
    DevModem_t *pMod = NULL;
    
    if (xSysconf.xModule.use_gprs == 2)
    {
        pMod = M26Create();
    }
    else if (xSysconf.xModule.use_gprs == 3)
    {
        pMod = UC15Create();
    }
    
    return pMod;
}

void Modem_Poll(DevModem_t *pModem)
{
    EventBits_t uxBits;
    DR_MODEM_e ret;
    int resp;
    int i;
    
    int fd;
    uint32_t bw;
    int fres;
    char filepath[64 + 1];
    uint32_t crc32_calc, crc32_orig;
    double dcrc32_calc;
    char ch_crc32[9] = { 0 };
    ul2uc ul2ucCrc32;
    uint8_t *pucFileBuffer;
    uint8_t *pucQueBuffer;
    uint32_t ulPos = 0;
        
    while(1)
    {
        switch (pModem->state)
        {
        case DS_MODEM_OFF:
            ret = pModem->open(pModem);
            if (ret != DR_MODEM_OK)
            {
                pModem->reset(pModem); 
                printf_modem("modem Key reset!: \r\n");
                vTaskDelay(10000);
            }
            break;
        case DS_MODEM_ON:
            ret = pModem->init(pModem);
            if (ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_PPP_Diag;
            }
#if 0
            if (ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_ACT_PDP;
            }
            else
            {
                pModem->state = DS_MODEM_DEACT_PDP;
            }
#endif
            break;
        case DS_MODEM_PPP_Diag:
            ret = M26_diag_PPP(pModem);
            if (ret == DR_MODEM_OK)
            {
                xEventGroupSetBits(xHandleEventTCP, defEventBitPPPDiagOK);
                pModem->state = DS_MODEM_PPP_On;
            }
            break;
        case DS_MODEM_PPP_On:
            modem_UART_putQue(pModem);
            /*=== read处理 ===*/
            recv_len = modem_read(pModem, tcp_client_recvbuf, MAX_COMMAND_LEN);
            if (recv_len > 0)
            {
                for (i = 0; i < recv_len; i++)
                {
                    pppHdlcDriverWriteRxQueue(net_dev->interface, tcp_client_recvbuf[i]);
                }

                if (strstr(tcp_client_recvbuf, "CLOSED") != NULL)
                {
                    printf_safe("\e[31;47mServer CLOSED\n\e[0m");
                    pModem->state = DS_MODEM_ERR;
                }
                else
                {
                    //
                }
                memset(tcp_client_recvbuf, 0, TCP_CLIENT_BUFSIZE);
                recv_len = 0;
            }
            uxBits = xEventGroupWaitBits(xHandleEventTCP, defEventBitPPPClosed, pdTRUE, pdTRUE, 0);
            if ((uxBits & defEventBitPPPClosed) == defEventBitPPPClosed)
            {
                pModem->state = DS_MODEM_ERR;
            }
            break;
        case DS_MODEM_ACT_PDP:
            ret = pModem->act_PDP(pModem);
            if (ret == DR_MODEM_OK)
            {
                if (pechProto->info.ftp.ucDownloadStart == 1)
                {
                    pModem->state = DS_MODEM_FTP_OPEN;
                }
                else
                {
#if EVSE_USING_RFID
                    xTimerStart(xHandleTimerRFID, 100); 
#endif
                    pModem->state = DS_MODEM_TCP_OPEN;
                }
            }
            else
            {
                pModem->state = DS_MODEM_DEACT_PDP;
            }
            break;
        case DS_MODEM_DEACT_PDP:
            ret = pModem->deact_PDP(pModem);
            if (ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_ACT_PDP;
            }
            else
            {
                pModem->state = DS_MODEM_ERR;
            }
            break;
        case DS_MODEM_TCP_OPEN:
            ret = pModem->open_TCP(pModem, pechProto->info.strServerIP, pechProto->info.usServerPort);
            switch (pModem->status.eConnect)
            {
            case CONNECT_OK:
                xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectFail); //rgw OK
                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectOK); //rgw OK
                pModem->state = DS_MODEM_TCP_KEEP;
                break;
            case CONNECT_FAIL:
                printf_modem("IP Status Error，Please check IP address，Call DEACT PDP!!\n");
                pModem->state = DS_MODEM_DEACT_PDP;
                break;
            default:
                break;
            }
            break;
        case DS_MODEM_TCP_KEEP: //临时注释，不要删
            pEVSE->status.ulSignalState |= defSignalEVSE_State_Network_Online;
#if 0
//            modem_get_STATE(pModem);
//            if(pModem->state == PDP_DEACT)
//            {
//                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectFail);
//                pModem->state = DS_MODEM_TCP_ACT_PDP;
//            }
//            if(pModem->state == IP_CLOSE)
//            {
//                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectFail);
//                pModem->state = DS_MODEM_TCP_OPEN;
//            }
            //等待remote发送请求
//            uxBits = xEventGroupWaitBits(xHandleEventTCP,
//                                         defEventBitTCPClientSendReq,
//                                         pdTRUE, pdTRUE, 0);
//            if((uxBits & defEventBitTCPClientSendReq) == defEventBitTCPClientSendReq) //有数据要发送
//            {
//                ret = modem_write(pModem, tcp_client_sendbuf, send_len);
//                if(ret == DR_MODEM_OK)
//                {
//                    printf_safe("\nTCP Send: ");
//                    for(i = 0; i < send_len; i++)
//                    {
//                        printf_safe("%02X ", tcp_client_sendbuf[i]);
//                    }
//                    printf_safe("\n");
//                    xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientSendOK);
//                }
//                else if(ret == DR_MODEM_READ)
//                {
//                    xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientSendOK);
//// 别删             xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientRecvValid); //DR_MODEM_READ处已经发送读事件，此处作为提示用途。
//                }
//                else if(ret == DR_MODEM_TIMEOUT)
//                {
//                    vTaskDelay(500);
//                }
//                else
//                {
//                    pModem->state = DS_MODEM_TCP_CLOSE;
//                    printf_safe("发送失败\r\n");
//                }
//            }
#endif
            modem_UART_putQue(pModem);
            /*=== read处理 ===*/
            recv_len = modem_read(pModem, tcp_client_recvbuf, MAX_COMMAND_LEN);
            if (recv_len > 0)
            {
                    
                printf_protolog("\nTCP Recv: ");
                for (i = 0; i < recv_len; i++)
                {
                    pppHdlcDriverWriteRxQueue(net_dev->interface, tcp_client_recvbuf[i]);
                    printf_protolog("%02X ", tcp_client_recvbuf[i]);
                }
                printf_protolog("\n");

                if (strstr(tcp_client_recvbuf, "CLOSED") != NULL)
                {
                    printf_safe("\e[31;47mServer CLOSED\n\e[0m");
                    pModem->state = DS_MODEM_ERR;
                }
                else
                {  
                    
                }
                memset(tcp_client_recvbuf, 0, TCP_CLIENT_BUFSIZE);
                recv_len = 0;
            }
            break;
        case DS_MODEM_TCP_CLOSE:
            pEVSE->status.ulSignalState &= ~defSignalEVSE_State_Network_Online;
            xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectFail); //rgw OK
            xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectOK); //rgw OK
            pModem->close_TCP(pModem);
            if (ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_TCP_OPEN;
            }
            else
            {
                pModem->state = DS_MODEM_DEACT_PDP;
            }
            break;
        case DS_MODEM_FTP_OPEN:
            NVIC_SetPriority(GPRS_IRQn, 1);
            pechProto->info.ftp.ftp_proc.ulFTPReOpenCnt++;
            if (pechProto->info.ftp.ftp_proc.ulFTPReOpenCnt >= 5)
            {
                pechProto->info.ftp.ftp_proc.ulFTPReOpenCnt = 0;
                pModem->state = DS_MODEM_FTP_ERR;
                break;
            }
            ret = pModem->open_FTP(pModem, 
                pechProto->info.strServerIP,
                pechProto->info.usServerPort,
                pechProto->info.ftp.strUser,
                pechProto->info.ftp.strPassword);
            if (ret != DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_DEACT_PDP;
                break;
            }
            ret = pModem->set_ftp_path(pModem, pechProto->info.ftp.strNewVersion);
            ret = pModem->ftp_get(pModem, pechProto->info.ftp.strNewFileName);
            if (ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_FTP_GET;
            }
            else
            {
                pModem->state = DS_MODEM_FTP_CLOSE;
            }
            break;
        case DS_MODEM_FTP_GET:
            ulTaskDelay_ms = 100;
            pechProto->info.ftp.ftp_proc.ulFTPReGetCnt++;//FTPGet次数
            pucFileBuffer = (uint8_t *)malloc(1024);
            pucQueBuffer = (uint8_t *)malloc(QUE_BUFSIZE);
            sprintf(filepath, "%s%s", pathSystemDir, pechProto->info.ftp.strNewFileName);
            fd = yaffs_open(filepath, O_CREAT | O_TRUNC | O_WRONLY , S_IWRITE | S_IREAD);
            if (fd < 0)
            {
                ThrowFSCode(fres = yaffs_get_error(), filepath, "ModemFTP-open");
                free(pucFileBuffer);
                free(pucQueBuffer);
                pModem->state = DS_MODEM_FTP_GET;//有待商榷
                break;
            }
            while (1)
            {
                recv_len = modem_read(pModem, pucQueBuffer, QUE_BUFSIZE);
                pechProto->info.ftp.ftp_proc.ulRecvFileSize += recv_len;
                if (strstr(pucQueBuffer, "+QFTPGET:") != NULL)
                {
                    for (i = 0; i < recv_len; i++)
                    {
                        printf_safe("%c", pucQueBuffer[i]);
                    }
                    pechProto->info.ftp.ftp_proc.ulRecvFileSize -= recv_len;//减去最后模块返回命令的长度
                    taskENTER_CRITICAL();
                    bw = yaffs_write(fd, pucFileBuffer, pechProto->info.ftp.ftp_proc.ulRecvFileSize);
                    yaffs_close(fd);
                    taskEXIT_CRITICAL();
                    free(pucFileBuffer);
                    free(pucQueBuffer);
                    pechProto->info.ftp.ftp_proc.ulRecvFileSize = 0;
                    ulPos = 0;
                    pModem->state = DS_MODEM_FTP_CHECK;
                    ulTaskDelay_ms = 1000;
                    break;
                }
                else
                {
                    if (recv_len > 0)
                    {
                        printf_safe("recv = %d\n", recv_len);                
                        printf_safe("total= %d\n", pechProto->info.ftp.ftp_proc.ulRecvFileSize);
//                        for (i = 0; i < recv_len; i++)
//                        {
//                            printf_safe("%02X ", pucQueBuffer[i]);
//                        }
                        taskENTER_CRITICAL();
                        pucFileBuffer = realloc(pucFileBuffer, pechProto->info.ftp.ftp_proc.ulRecvFileSize);
                        memcpy(pucFileBuffer + ulPos, pucQueBuffer, recv_len);
                        taskEXIT_CRITICAL();
                        ulPos += recv_len;
                    }
                }
            }
            break;
        case DS_MODEM_FTP_CHECK:
            for (i = 0; i < 8; i++)
            {
                ch_crc32[i] = pechProto->info.ftp.strNewFileName[i + 2];
            }
            StrToHex(ch_crc32, ul2ucCrc32.ucVal, strlen(ch_crc32));
            crc32_orig = ntohl(ul2ucCrc32.ulVal);
            GetFileCrc32(filepath, &crc32_calc);
            printf_safe("crc32_calc = %x\n", crc32_calc);
            printf_safe("crc32_orgi = %x\n", crc32_orig);
            if (crc32_calc == crc32_orig)
            {
                taskENTER_CRITICAL();
                dcrc32_calc = (uint32_t)crc32_calc;
                xSysconf.SetSysCfg(jnSysChargersdk_bin_crc32, (void*)&dcrc32_calc, ParamTypeDouble);
                yaffs_unlink(pathBin);
                yaffs_rename(filepath, pathBin);
                taskEXIT_CRITICAL();
                xSysconf.xUpFlag.chargesdk_bin = 1;
                pechProto->info.ftp.ucDownloadStart = 0;
                xSysconf.SetSysCfg(
                    jnSysChargersdk_bin, 
                    (void *)&(xSysconf.xUpFlag.chargesdk_bin), 
                    ParamTypeU8);
                pechProto->info.ftp.SetFtpCfg(
                    jnFtpDownloadStart,       
                    (void *)&(pechProto->info.ftp.ucDownloadStart), 
                    ParamTypeU8);
                NVIC_SystemReset();
            }
            else
            {
                pModem->state = DS_MODEM_FTP_REGET;
                break;
            }
            break;
        case DS_MODEM_FTP_REGET:
            if (pechProto->info.ftp.ftp_proc.ulFTPReGetCnt >= 5)
            {
                pechProto->info.ftp.ftp_proc.ulFTPReGetCnt = 0;
                pModem->state = DS_MODEM_FTP_ERR;
                break;
            }
            ret = pModem->ftp_get(pModem, pechProto->info.ftp.strNewFileName);
            if (ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_FTP_GET;
            }
            break;
        case DS_MODEM_FTP_CLOSE:
            NVIC_SetPriority(GPRS_IRQn, GPRS_Priority);
            ret = pModem->close_FTP(pModem);
            if (ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_DEACT_PDP;
            }
            break;
        case DS_MODEM_FTP_ERR:
            xEventGroupSetBits(xHandleEventHMI, defEventBitHMI_UP_FAILD);
            xSysconf.xUpFlag.chargesdk_bin = 3;
            pechProto->info.ftp.ucDownloadStart = 0;
            xSysconf.SetSysCfg(
                jnSysChargersdk_bin, 
                (void *)&(xSysconf.xUpFlag.chargesdk_bin), 
                ParamTypeU8);
            pechProto->info.ftp.SetFtpCfg(
                jnFtpDownloadStart,       
                (void *)&(pechProto->info.ftp.ucDownloadStart), 
                ParamTypeU8);
            pModem->state = DS_MODEM_FTP_CLOSE;
            break;
        case DS_MODEM_ERR:
            pEVSE->status.ulSignalState &= ~defSignalEVSE_State_Network_Online;
            xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectFail); //rgw OK
            xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectOK); //rgw OK
            bsp_Uart_Init(UART_PORT_GPRS, 2);
            ret = pModem->soft_reset(pModem);
            if(ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_ON;
            }
            else
            {
                pModem->state = DS_MODEM_OFF;
            }
            break;
        default:
            break;
        }
#if 0
//        uxBits = xEventGroupWaitBits(xHandleEventTCP,
//                                     defEventBitTCPClientFlushBuff,
//                                     pdTRUE, pdTRUE, 0); //定时请一次缓存
//        if((uxBits & defEventBitTCPClientFlushBuff) == defEventBitTCPClientFlushBuff)
//        {
//            //printf_safe("看看是不是定时清缓存这里出问题了\n");
//            recv_len = modem_read(pModem, tcp_client_recvbuf, MAX_COMMAND_LEN);
//            if(recv_len > 0)
//            {
//                printf_safe("\nTCP Recv: ");
//                for(i = 0; i < recv_len; i++)
//                {
//                    printf_safe("%02X ", tcp_client_recvbuf[i]);
//                }
//                printf_safe("\n");
//
//                pechProto->recvResponse(pechProto, pEVSE, tcp_client_recvbuf, recv_len, 3);
//                memset(tcp_client_recvbuf, 0, recv_len);
//                recv_len = 0;
//            }
//        }

        //modem_get_info(pModem);
#endif
        vTaskDelay(ulTaskDelay_ms);
    }
}
