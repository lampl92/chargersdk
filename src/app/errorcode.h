/**
* @file errorcode.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#ifndef  __ERRORCODE_H
#define  __ERRORCODE_H

#define THROW_ERROR(_errcode,_errlevel)   {                                           \
                                            ErrorCode_t _macro_errcode = _errcode;         \
                                            if(_macro_errcode != ERR_NO)                  \
                                            {                                       \
                                                ThrowErrorCode(_macro_errcode,_errlevel);  \
                                            }                                       \
                                        }

typedef enum _ErrorCode
{
    ERR_NO,                 //No Error
    ERR_SCRAM_FAULT,        //��ͣ��������
    ERR_GSENSOR_FAULT,      //�������ٶȴ�������������
    ERR_PE_FAULT,           //�����ӵ������Դ�������������
    ERR_POWEROFF_DECT_FAULT,//����״̬�������������
    ERR_ARRESTER_FAULT,     //��������������

    ERR_POINT1_METER_FAULT,        //���ͨ�Ź���
    ERR_POINT1_CP_FAULT,           //���CP����
    ERR_POINT1_CC_FAULT,           //���CC����
    ERR_POINT1_PLUG_FAULT,         //��ǹ״̬�������������
    ERR_POINT1_B_LOCK_FAULT,       //B������ǹ��״̬�������������
    ERR_POINT1_CANT_LOCK,          //B������ǹ���޷���ֹ
    ERR_POINT1_ACLTEMP_DECT_FAULT, //L�����¶ȼ�ⷢ������
    ERR_POINT1_ACNTEMP_DECT_FAULT, //N�����¶ȼ�ⷢ������
    ERR_POINT1_BTEMP1_DECT_FAULT,  //B�������¶ȼ���1��������
    ERR_POINT1_BTEMP2_DECT_FAULT,  //B�������¶ȼ���2��������
    ERR_POINT1_STARTCHARGE,        //��ʼ������
    ERR_POINT1_STOPCHARGE,         //����������

    ERR_POINT2_METER_FAULT,        //���ͨ�Ź���
    ERR_POINT2_CP_FAULT,           //���CP����
    ERR_POINT2_CC_FAULT,           //���CC����
    ERR_POINT2_PLUG_FAULT,         //��ǹ״̬�������������
    ERR_POINT2_B_LOCK_FAULT,       //B������ǹ��״̬�������������
    ERR_POINT2_CANT_LOCK,          //B������ǹ���޷���ֹ
    ERR_POINT2_ACLTEMP_DECT_FAULT, //L�����¶ȼ�ⷢ������
    ERR_POINT2_ACNTEMP_DECT_FAULT, //N�����¶ȼ�ⷢ������
    ERR_POINT2_BTEMP1_DECT_FAULT,  //B�������¶ȼ���1��������
    ERR_POINT2_BTEMP2_DECT_FAULT,  //B�������¶ȼ���2��������
    ERR_POINT2_STARTCHARGE,        //��ʼ������
    ERR_POINT2_STOPCHARGE,         //����������

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
} ErrorCode_t;

typedef enum _ErrorLevel
{
    ERR_LEVEL_TIPS,
    ERR_LEVEL_WARNING,
    ERR_LEVEL_CRITICAL,
    ERR_LEVEL_CLEAR
} ErrorLevel_t;

typedef struct _ErrorPackage
{
    ErrorCode_t code;
    ErrorLevel_t level;
    uint8_t *str;
}ErrorPackage_t;

extern const uint8_t *strErrorCode[];

void ThrowErrorCode(ErrorCode_t errcode, ErrorLevel_t errlevel);

#endif
