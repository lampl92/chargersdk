/**
* @file errorcode.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#include "includes.h"
#include "errorcode.h"

//
//  һ��Ҫ��֤code�� ˳�� ��ͷ�ļ�����һ��
//
const uint8_t *strErrorCode[100] =
{
    "No Error",
    "��ͣ��������",
    "��������",
    "������������żУ�����",
    "������������������",
    "����������֡����",
    "���������ڳ���",
    "����������DMA�������"
    "����ģ�鴮����żУ�����",
    "����ģ�鴮����������",
    "����ģ�鴮��֡����",
    "����ģ�鴮�ڳ���",
    "����ģ�鴮��DMA�������"
};

void ThrowErrorCode(ErrorCode_t errcode, ErrorLevel_t errlevel)
{
    ErrorPackage_t package;

    package.code = errcode;
    package.level = errlevel;

    xQueueSend(xHandleQueueErrorCode, (void *)&package, 0);
}
