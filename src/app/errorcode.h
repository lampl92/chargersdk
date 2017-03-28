/**
* @file errorcode.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#ifndef  __ERRORCODE_H
#define  __ERRORCODE_H

#define THROW_ERROR(_dev, _errcode,_errlevel)   do{                                           \
                                                    ErrorCode_t _macro_errcode = _errcode;         \
                                                    if(_macro_errcode != ERR_NO)                  \
                                                    {                                       \
                                                        ThrowErrorCode(_dev, _macro_errcode,_errlevel);  \
                                                    }                                       \
                                                }while(0);
typedef enum _ErrorCode
{
    ERR_NO,                 //No Error
    ERR_SCRAM_FAULT,        //��ͣ��������
    ERR_GSENSOR_FAULT,      //�������ٶȴ�������������
    ERR_PE_FAULT,           //�����ӵ������Դ�������������
    ERR_POWEROFF_DECT_FAULT,//����״̬�������������
    ERR_ARRESTER_FAULT,     //��������������

    ERR_CON_METER_FAULT,        //���ͨ�Ź���
    ERR_CON_CP_FAULT,           //���CP����
    ERR_CON_CC_FAULT,           //���CC����
    ERR_CON_PLUG_FAULT,         //��ǹ״̬�������������
    ERR_CON_B_LOCK_FAULT,       //B������ǹ��״̬�������������
    ERR_CON_CANT_LOCK,          //B������ǹ���޷���ֹ
    ERR_CON_ACLTEMP_DECT_FAULT, //L�����¶ȼ�ⷢ������
    ERR_CON_ACNTEMP_DECT_FAULT, //N�����¶ȼ�ⷢ������
    ERR_CON_BTEMP1_DECT_FAULT,  //B�������¶ȼ���1��������
    ERR_CON_BTEMP2_DECT_FAULT,  //B�������¶ȼ���2��������
    ERR_CON_STARTCHARGE,        //��ʼ������
    ERR_CON_STOPCHARGE,         //����������

    ERR_FILE_RW,            //�ļ���д����
    ERR_FILE_NO,            //��Ŀ���ļ�
    ERR_OTHER,              //��������
    ERR_RFID_FAULT,         //��������ȡUID����
    ERR_RFID_PE,            //������������żУ�����
    ERR_RFID_NE,            //������������������
    ERR_RFID_FE,            //����������֡����
    ERR_RFID_ORE,           //���������ڳ���
    ERR_RFID_DMA,           //����������DMA�������
    ERR_NETMODULE_PE,       //����ģ�鴮����żУ�����
    ERR_NETMODULE_NE,       //����ģ�鴮����������
    ERR_NETMODULE_FE,       //����ģ�鴮��֡����
    ERR_NETMODULE_ORE,      //����ģ�鴮�ڳ���
    ERR_NETMODULE_DMA,      //����ģ�鴮��DMA�������

    ERR_NONET,               //����ͨ�Ź���
    ERR_NET_TIMEOUT,        //����ͨ�ų�ʱ
    ERR_MEMORY                //Malloc����

} ErrorCode_t;

typedef enum _ErrorLevel
{
    ERR_LEVEL_OK,
    ERR_LEVEL_TIPS,
    ERR_LEVEL_WARNING,
    ERR_LEVEL_CRITICAL,
    ERR_LEVEL_CLEAR
} ErrorLevel_t;

typedef struct _ErrorPackage
{
    uint32_t ulDevID;
    ErrorCode_t code;
    ErrorLevel_t level;
} ErrorPackage_t;

//���ǹID��С�����壬�����豸�Ӵ�С����
//DevID 0~? ���ǹID
#define defDevID_System     250
#define defDevID_Cloud      251
#define defDevID_File       252
#define defDevID_RFID       253
#define defDevID_NetMoudle  254
#define defDevID_EVSE       255 //0xff


extern const uint8_t *strErrorCode[];

void ThrowErrorCode(uint32_t ulDevID, ErrorCode_t errcode, ErrorLevel_t errlevel);

#endif
