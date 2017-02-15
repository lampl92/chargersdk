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
const uint8_t *strErrorCode[100]=
{
    "No Error\n",     //ERR_NO
    "��ͣ��������\n",   //ERR_SCRAM
    "��������\n"      //ERR_OTHER
};

void ThrowErrorCode(ErrorCode_t errcode)
{
    //printf_safe("%s",strErrorCode[errcode]);
    xQueueSend(xHandleQueueErrorCode, (void *)&errcode, 0);
}
