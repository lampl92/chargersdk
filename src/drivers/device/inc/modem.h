#ifndef _MODEM_H_
#define _MODEM_H_

#include "FreeRTOS.h"
#include "semphr.h"

#define MAX_COMMAND_LEN                  1024  /* �������� */
typedef enum
{
    CPIN_OTHER,
    CPIN_READY,
    REG_LOCAl,
    REG_ROAMING,
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
    IP_INITIAL,                      //��ʼ��
    IP_START,                        //��������
    IP_CONFIG,                       //���ó���
    IP_IND,                          //���� GPRS/CSD ������
    IP_GPRSACT,                      //���ճ�������
    IP_STATUS,                       //��ñ��� IP ��ַ���ο� AT+QILOCIP ���
    TCP_CONNECTING,                  //TCP ������
    //UDP_CONNECTING,                  //UDP ������ (��ʱ�жϲ��������״̬����Ϊ�ж�ʱ��UDP��TCP��ͷȥ���ˣ�ֻ����Connect)
    IP_CLOSE,                        //TCP/UDP ���ӹر�
    TCP_CONNECT_OK,                      //TCP/UDP ���ӳɹ�
    PDP_DEACT                        //GPRS/CSD  �����쳣�ر�
}ModemConStat_e;

typedef struct
{
    uint8_t strAPN[16+1];
    uint8_t ucContext;
} ModemInfo_t;

typedef struct
{
    ModemParam_e eSimStat;      //CPIN   CPIN_OTHER || CPIN_READY
    ModemParam_e eNetReg;       //CREG ����ע����Ϣ    REG_LOCAl || REG_ROAMING
    ModemParam_e eGprsReg;      //CGREG GPRS����ע����Ϣ REG_LOCAl || REG_ROAMING
    uint8_t ucSignalQuality;    //CSQ �ź�ǿ��  rssi:0-31��Խ��Խ��,  99 �ź��쳣
    uint8_t strLocIP[15+1];     //����IP
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
void modem_get_info(DevModem_t *pModem);
void Modem_Poll(DevModem_t *pModem);

#endif/*_MODEM_H_*/
