#ifndef _MODEM_H_
#define _MODEM_H_

#define MAX_COMMAND_LEN                  512  /* �������� */

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

/* information length */
#define MODEM_MANUFACTURER_LEN           7
#define MODEM_MODEL_LEN                  4
#define MODEM_SERIAL_NUMBER_LEN          15
#define MODEM_IMEI_LEN                   15
#define MODEM_SIMCARD_ID_LEN             20


typedef struct
{
    int   uart_sta;                                     /* ����״̬ */
    int   err_code;                                     /* ������ */
} ModemInfo_t;

typedef struct
{
    uint8_t ucSimStat;        //CPIN   ��errcode
    uint8_t ucNetReg;            //CREG ����ע����Ϣ    1 || 5
    uint8_t ucGprsReg;       //CGREG GPRS����ע����Ϣ  1 || 5
    uint8_t ucSignalQuality;   //CSQ �ź�ǿ��  rssi:0-99  ԽСԽ��
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



int     modem_send_at(char *format, ...);                                         /* ����AT���� */
int     modem_get_at_reply(uint8_t *reply, uint32_t len, const uint8_t *key, uint32_t second);    /* ��ȡAT����� */

DR_MODEM_e modem_init(void);
void modem_get_info(DevModem_t *pModem);






#endif/*_MODEM_H_*/
