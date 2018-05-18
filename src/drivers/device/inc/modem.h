#ifndef _MODEM_H_
#define _MODEM_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "userlib_queue.h"
#include "retarget.h"


#define MODEM_UARTx         "UART5"
#define MODEM_UART_BPS      115200

extern int modemlog;
#define printf_modem(...) do{if(modemlog > 0)printf_safe(__VA_ARGS__);}while(0);

#define MAX_COMMAND_LEN              128  /* 最大命令长度 */
#define TCP_CLIENT_BUFSIZE           4096
#define QUE_BUFSIZE                  4096

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
} ModemParam_e;

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
} ModemConStat_e;

typedef struct
{
    char strAPN[16+1];
    char strATD[32 + 1];
    uint8_t ucContext;
    uint8_t ucTPMode;
} ModemInfo_t;

typedef struct
{
    ModemParam_e eSimStat;      //CPIN   CPIN_OTHER || CPIN_READY
    ModemParam_e eNetReg;       //CREG 网络注册信息    REG_LOCAl || REG_ROAMING
    ModemParam_e eGprsReg;      //CGREG GPRS网络注册信息 REG_LOCAl || REG_ROAMING
    uint8_t ucSignalQuality;    //CSQ 信号强度  rssi:0-31，越大越好,  99 信号异常
    char strLocIP[15+1];     //本地IP
    ModemParam_e eConnect;
    ModemConStat_e statConStat;
} ModemStatus_t;

typedef enum
{
    DS_MODEM_OFF,
    DS_MODEM_ON,
    DS_MODEM_ERR,
    DS_MODEM_PPP_Diag,
    DS_MODEM_PPP_On,
    DS_MODEM_ACT_PDP,
    DS_MODEM_DEACT_PDP,
//    DS_MODEM_TRANSPARENT,
    DS_MODEM_TCP_OPEN,
    DS_MODEM_TCP_KEEP,
    DS_MODEM_TCP_CLOSE,
    DS_MODEM_FTP_OPEN,
    DS_MODEM_FTP_CLOSE,
    DS_MODEM_FTP_GET,
    DS_MODEM_FTP_REGET,
    DS_MODEM_FTP_CHECK,
    DS_MODEM_FTP_ERR
} ModemState_e;


typedef struct
{
    uint32_t sent;
    uint32_t acked;
    uint32_t nAcked;
    uint32_t readable;
} ModemFlag_t;

typedef DR_MODEM_e(*modem_ft)(void *pModem);
    
typedef struct _dev_modem
{
    ModemInfo_t info;
    ModemStatus_t status;
    volatile ModemState_e state;
    ModemFlag_t flag;
    int uart_handle;
    
    modem_ft open;
    modem_ft init;
    modem_ft keyon;
    modem_ft keyoff;
    modem_ft diag_PPP;
    modem_ft act_PDP;
    modem_ft deact_PDP;
    DR_MODEM_e(*open_TCP)(void *pModem, char *server_ip, uint16_t port);
    modem_ft close_TCP;
    DR_MODEM_e(*open_FTP)(void *pModem, char *server_ip, uint16_t port, char *user, char *pass);
    DR_MODEM_e(*set_ftp_path)(void *pModem, char *path);
    DR_MODEM_e(*ftp_get)(void *pModem, char *fname);
    modem_ft close_FTP;
    modem_ft soft_reset;

} DevModem_t;

extern DevModem_t *pModem;

void modem_delayms(int ms);
DevModem_t *DevModemCreate(void);
void Modem_Poll(DevModem_t *pModem);
uint32_t modem_send_at(char *format, ...);
DR_MODEM_e modem_get_at_reply(char *reply, uint32_t len, const char *key, uint32_t second);

#endif/*_MODEM_H_*/
