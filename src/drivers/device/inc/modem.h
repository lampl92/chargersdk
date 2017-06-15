#ifndef _MODEM_H_
#define _MODEM_H_

#include "FreeRTOS.h"
#include "semphr.h"

#define MAX_COMMAND_LEN                  1024  /* 最大命令长度 */
typedef enum
{
    CPIN_OTHER,
    CPIN_READY,
    REG_LOCAl,
    REG_ROAMING,
    REG_SEARCH,
    REG_UNKNOWN,
    CONNECT_OK,
    CONNECT_FAIL
}ModemParam_e;

/* error code */
typedef enum
{
    DR_MODEM_ERROR,
    DR_MODEM_OK,
    DR_MODEM_TIMEOUT,
    DR_MODEM_READ,
    DR_MODEM_CLOSED
} DR_MODEM_e;


typedef enum
{
    IP_INITIAL,                      //初始化
    IP_START,                        //启动任务
    IP_CONFIG,                       //配置场景
    IP_IND,                          //激活 GPRS/CSD 场景中
    IP_GPRSACT,                      //接收场景配置
    IP_STATUS,                       //获得本地 IP 地址（参考 AT+QILOCIP 命令）
    TCP_CONNECTING,                  //TCP 连接中
    //UDP_CONNECTING,                  //UDP 连接中 (暂时判断不出来这个状态，因为判断时把UDP或TCP的头去掉了，只留下Connect)
    IP_CLOSE,                        //TCP/UDP 连接关闭
    TCP_CONNECT_OK,                      //TCP/UDP 连接成功
    PDP_DEACT                        //GPRS/CSD  场景异常关闭
}ModemConStat_e;

typedef struct
{
    uint8_t strAPN[16+1];
    uint8_t ucContext;
} ModemInfo_t;

typedef struct
{
    ModemParam_e eSimStat;      //CPIN   CPIN_OTHER || CPIN_READY
    ModemParam_e eNetReg;       //CREG 网络注册信息    REG_LOCAl || REG_ROAMING
    ModemParam_e eGprsReg;      //CGREG GPRS网络注册信息 REG_LOCAl || REG_ROAMING
    uint8_t ucSignalQuality;    //CSQ 信号强度  rssi:0-31，越大越好,  99 信号异常
    uint8_t strLocIP[15+1];     //本地IP
    ModemParam_e eConnect;
    ModemConStat_e statConStat;
} ModemStatus_t;

typedef enum
{
    DS_MODEM_OFF,
    DS_MODEM_ON,
    DS_MODEM_ERR,
    DS_MODEM_TCP_ACT_PDP,
    DS_MODEM_TCP_DEACT_PDP,
    DS_MODEM_TCP_OPEN,
    DS_MODEM_TCP_KEEP,
    DS_MODEM_TCP_CLOSE
}ModemState_e;


typedef struct
{
    uint32_t sent;
    uint32_t acked;
    uint32_t nAcked;
    uint32_t readable;
}ModemFlag_t;

typedef struct _dev_modem
{
    ModemInfo_t info;
    ModemStatus_t status;
    volatile ModemState_e state;
    ModemFlag_t flag;
    SemaphoreHandle_t xMutex;
} DevModem_t;

extern DevModem_t *pModem;

DevModem_t *DevModemCreate(void);
DR_MODEM_e modem_open(DevModem_t *pModem);
DR_MODEM_e modem_init(DevModem_t *pModem);
void Modem_Poll(DevModem_t *pModem);

#endif/*_MODEM_H_*/
