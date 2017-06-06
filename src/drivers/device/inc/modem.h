#ifndef _MODEM_H_
#define _MODEM_H_

#define MAX_COMMAND_LEN                  512  /* 最大命令长度 */

/* error code */
typedef enum
{
    DR_MODEM_ERROR,
    DR_MODEM_OK,
    DR_MODEM_TIMEOUT,
    DR_MODEM_CPIN_READY,
    DR_MODEM_CPIN_OTHER,
    DR_MODEM_REG_LOCAl,
    DR_MODEM_REG_ROAMING

} DR_MODEM_e;




#define MODEM_GET_UART_STA_ERR           1<<0
#define MODEM_GET_SIM_STA_ERR            1<<1
#define MODEM_GET_SIGNAL_STRENGTH_ERR    1<<2
#define MODEM_GET_SOFTWARE_VER_ERR       1<<3
#define MODEM_GET_GPRS_STA_ERR           1<<6


typedef struct
{
    int   uart_sta;                                     /* 串口状态 */
    int   err_code;                                     /* 错误码 */
} ModemInfo_t;

typedef struct
{
    uint8_t ucSimStat;        //CPIN   见errcode
    uint8_t ucNetReg;            //CREG 网络注册信息    1 || 5
    uint8_t ucGprsReg;       //CGREG GPRS网络注册信息  1 || 5
    uint8_t ucSignalQuality;   //CSQ 信号强度  rssi:0-99  越小越好
} ModemStatus_t;

typedef enum
{
    DS_MODEM_OFF,
    DS_MODEM_ON,
    DS_MODEM_ERR,
    DS_MODEM_GET_STATUS,
    DS_MODEM_TCP
}ModemState_e;

typedef struct _dev_modem
{
    ModemInfo_t info;
    ModemStatus_t status;
    ModemState_e state;
} DevModem_t;

extern DevModem_t *pModem;

DevModem_t *DevModemCreate(void);
DR_MODEM_e modem_init(void);
void modem_get_info(DevModem_t *pModem);

#endif/*_MODEM_H_*/
