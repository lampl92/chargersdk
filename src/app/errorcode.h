/**
* @file errorcode.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#ifndef  __ERRORCODE_H
#define  __ERRORCODE_H

#define THROW_ERROR(errcode,errlevel)   if(errcode != ERR_NO) {ThrowErrorCode(errcode,errlevel);}

typedef enum _ErrorCode
{
    ERR_NO,                 //No Error
    ERR_SCRAM,              //��ͣ��������
    ERR_OTHER,              //��������
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
}ErrorPackage_t;

extern const uint8_t *strErrorCode[];

void ThrowErrorCode(ErrorCode_t errcode, ErrorLevel_t errlevel);

#endif
