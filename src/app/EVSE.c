/**
* @file EVSE.c
* @brief EVSE��ʼ��
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "includes.h"
#include "globals.h"
#include "userlib_list.h"
#include "interface.h"

/** @brief ��ü�ͣ״̬
 *          0 �޼�ͣ
 *          1 �м�ͣ
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetScramState(EVSE_t *pEVSE)
{
    uint32_t tmpScramState;
    ErrorCode_t errcode;

    //errcode = ERR_SCRAM;
    errcode = ERR_NO;
    tmpScramState = 0;

    /** @todo (rgw#1#): ʵ�ִ��� */

    pEVSE->state.ulScramState = tmpScramState;

    return errcode;
}

/** @brief �Ӵ�������ȡײ��״̬
 *          0 ��ײ��
 *          1 ��ײ��
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetKnockState(EVSE_t *pEVSE)
{
    uint32_t tmpKnockState;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    tmpKnockState = 0;

    /** @todo (rgw#1#): ʵ�ִ��� */

    pEVSE->state.ulKnockState = tmpKnockState;

    return  errcode;
}

/** @brief ��ȡ�����ӵ�������״̬
 *          0 ����
 *          1 �쳣
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetPEState(EVSE_t *pEVSE)
{
    uint32_t tmpPEState;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    tmpPEState = 0;

    /** @todo (rgw#1#): ʵ�ִ��� */

    pEVSE->state.ulPEState = tmpPEState;

    return errcode;
}

/** @brief ��ȡ����״̬
 *          0 ����
 *          1 ����
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetPowerOffState(EVSE_t *pEVSE)
{
    uint32_t tmpOffState;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    tmpOffState = 0;

    /** @todo (rgw#1#): ʵ�ִ��� */

    pEVSE->state.ulPowerOffState = tmpOffState;

    return errcode;
}

/** @brief ��ȡ������״̬
 *          0 ����
 *          1 �쳣
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetArresterState(EVSE_t *pEVSE)
{
    uint32_t tmpArresterState;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    tmpArresterState = 0;
    pEVSE->state.ulArresterState = tmpArresterState;
    return errcode;
}

EVSE_t *EVSECreate(void)
{
    EVSE_t *pEVSE;
    pEVSE = (EVSE_t *)malloc(sizeof(EVSE_t));

    return pEVSE;
}

void EVSEinit(void)
{
    static ChargePoint_t *pchargepoint[2];  //�ڶ��ж���
    uint8_t ucTotal;

    THROW_ERROR(GetTotalChargePoint(&ucTotal), ERR_LEVEL_WARNING);
    pEVSE = EVSECreate();
    pListChargePoint = UserListCreate();
    int i;
    for(i = 0; i < ucTotal; i++)
    {
        pchargepoint[i] = ChargePointCreate(i);
        pListChargePoint->Add(pListChargePoint, pchargepoint[i]);
    }
}
