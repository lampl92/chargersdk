/**
* @file interface_remote.c
* @brief ��ƽ̨�ӿ�
* @author rgw
* @version v1.0
* @date 2017-02-06
*/

#include <time.h>
#include "includes.h"
#include "bsp.h"
#include "mbedtls/base64.h"

ErrorCode_t RemoteInit()
{
    uint8_t  ucAESKey_BASE64[] = "oFqTg0a0VrjiVU76M1WRVw==";
    uint8_t *pucAESKey_BASE64;

    pucAESKey_BASE64 = ucAESKey_BASE64;

    uint8_t ucAESKey[64];
    uint8_t ucAESKeyLen;
    uint8_t olen;
    int i;
    pucAESKey_BASE64 = ucAESKey_BASE64;
    mbedtls_base64_decode(ucAESKey, sizeof(ucAESKey), &olen, pucAESKey_BASE64, strlen(pucAESKey_BASE64));
    printf_safe("AESKEYBASE64 : %s\n", pucAESKey_BASE64);
    printf_safe("AESKey �� ");
    for(i = 0; i<olen; i++)
    {
        printf_safe("%02x ",ucAESKey[i]);
    }
    printf_safe("\n");
}

ErrorCode_t RemoteGetTime(struct tm *pTimeBlock)
{
    struct tm tmpTimeBlock;
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /** @todo (rgw#1#): ��ȡ������ʱ�� */


    *pTimeBlock = tmpTimeBlock;

    return errcode;
}

/** @brief
 *
 * @param ucRetVal uint8_t*     0ע��ɹ�  1ע��ʧ��
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteRegist(EVSE_t *pEVSE, uint8_t *ucRetVal )
{
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /** @todo (rgw#1#): ����ƽ̨ע��ӿ� */

    *ucRetVal = 0;
    return errcode;
}

/** @brief
 *
 * @param pucID uint8_t*
 * @param ucIDLength uint8_t
 * @param pucAccountStatus uint8_t*     0 δע�� ��1 ע��
 * @param pdBalance double*             �ʻ����
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteGetBalance(uint8_t *pucID, uint8_t ucIDLength, uint8_t *pucAccountStatus, double *pdBalance)
{
    double tmpBalance;
    uint8_t tmpAccountStat;
    ErrorCode_t errcode;

    tmpAccountStat = 1;
    tmpBalance = 88888.88;
    errcode = ERR_NO;

    /** @todo (rgw#1#): ������ƽ̨�ӿ� */

    *pucAccountStatus = tmpAccountStat;
    *pdBalance = tmpBalance;

//    errcode = ERR_NET_TIMEOUT;
//    errcode = ERR_NONET;
    return errcode;
}
