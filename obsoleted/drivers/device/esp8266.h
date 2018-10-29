#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "FreeRTOS.h"
#include "semphr.h"

#define WIFI_MAX_COMMAND_LEN                  1024  /* 最大命令长度 */

typedef enum
{
    WIFI_AP_CONNECT_OK,
    WIFI_AP_CONNECT_FAIL
}WifParam_e;

/* error code */
typedef enum
{
    DR_WIFI_ERROR,
    DR_WIFI_OK,
    DR_WIFI_TIMEOUT,
    DR_WIFI_READ,
    DR_WIFI_CLOSED
} DR_WIFI_e;


typedef enum
{
    WifiConStat
}WifiConStat_e;

typedef struct
{
    uint8_t strSSID[64+1];
    uint8_t strPWD[64+1];
} WifiInfo_t;

typedef struct
{
    uint8_t ucSignalQuality;    //CSQ 信号强度  rssi:0-31，越大越好,  99 信号异常
    uint8_t strLocIP[15+1];     //本地IP
    WifParam_e eConnect;
    WifiConStat_e statConStat;
} WifiStatus_t;

typedef enum
{
    DS_WIFI_OFF,
    DS_WIFI_ON,
    DS_WIFI_ERR,
    DS_WIFI_CONNECT_AP,
    DS_WIFI_DISCONNECT_AP,
    DS_WIFI_TCP_OPEN,
    DS_WIFI_TCP_KEEP,
    DS_WIFI_TCP_CLOSE
}WifiState_e;


typedef struct
{
    uint32_t sent;
    uint32_t acked;
    uint32_t nAcked;
    uint32_t readable;
}WifiFlag_t;

typedef struct _dev_wifi
{
    WifiInfo_t info;
    WifiStatus_t status;
    volatile WifiState_e state;
    WifiFlag_t flag;
    SemaphoreHandle_t xMutex;
} DevWIFI_t;

extern DevWIFI_t *pWIFI;

DevWIFI_t *DevWifiCreate(void);
DR_WIFI_e wifi_open(DevWIFI_t *pWIFI);
DR_WIFI_e wifi_init(DevWIFI_t *pWIFI);
void wifi_get_info(DevWIFI_t *pWIFI);
void Wifi_Poll(DevWIFI_t *pWIFI);

#endif/*_MODEM_H_*/
