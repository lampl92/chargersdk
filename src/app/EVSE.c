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


    /*---------------------------------------------------------------------------/
    /                               ���ļ���ȡ���׮��Ϣ
    /---------------------------------------------------------------------------*/


/** @brief �豸Ψһ���к�,�ͳ���
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetSN(void *pvEVSE)
{
    uint8_t tmpSN[24];
    uint8_t tmpLength;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    memset(tmpSN, 0, 24);
    tmpLength = 0;

/** @todo (rgw#1#): ���ļ���ȡSN ����ȡSN����*/
    pEVSE->info.ucSNLength = tmpLength;
    memmove(pEVSE->info.ucSN, tmpSN, 24);

    return errcode;
}

/** @brief ƽ̨����ID
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetID(void *pvEVSE)
{
    uint8_t tmpID[24];
    uint8_t tmpLength;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    memset(tmpID, 0, 24);
    tmpLength = 0;

/** @todo (rgw#1#): ���ļ���ȡID ����ȡID����*/
    pEVSE->info.ucIDLenght = tmpLength;
    memmove(pEVSE->info.ucID, tmpID, 24);

    return errcode;
}

/** @brief ����豸����
 *         1��ֱ���豸
 *         2�������豸
 *         3����ֱ��һ���豸
 *         4�������豸
 *         5������
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetType(void *pvEVSE)
{
    uint8_t tmpType;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    tmpType = 0;
    errcode = ERR_NO;

/** @todo (rgw#1#): ���ļ���ȡ */

    pEVSE->info.ucType = tmpType;

    return errcode;
}

/** @brief ��γ�ȣ���������λ
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetLngLat(void *pvEVSE)
{
    double tmpLng,tmpLat;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    tmpLng = 0;
    tmpLat = 0;
    errcode = ERR_NO;

/** @todo (rgw#1#): ���ļ���ȡ */

    pEVSE->info.dLng = tmpLng;
    pEVSE->info.dLat = tmpLng;

    return errcode;
}


    /*---------------------------------------------------------------------------/
    /                               ��������ȡ���׮״̬
    /---------------------------------------------------------------------------*/


/** @brief ��ü�ͣ״̬
 *          0 �޼�ͣ
 *          1 �м�ͣ
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetScramState(void *pvEVSE)
{
    uint32_t tmpScramState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
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
static ErrorCode_t GetKnockState(void *pvEVSE)
{
    uint32_t tmpKnockState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
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
static ErrorCode_t GetPEState(void *pvEVSE)
{
    uint32_t tmpPEState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
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
static ErrorCode_t GetPowerOffState(void *pvEVSE)
{
    uint32_t tmpOffState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
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
static ErrorCode_t GetArresterState(void *pvEVSE)
{
    uint32_t tmpArresterState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    tmpArresterState = 0;
    pEVSE->state.ulArresterState = tmpArresterState;
    return errcode;
}

EVSE_t *EVSECreate(void)
{
    EVSE_t *pEVSE;
    pEVSE = (EVSE_t *)malloc(sizeof(EVSE_t));

    memset(pEVSE->info.ucSN, 0, 24);
    memset(pEVSE->info.ucID, 0, 24);
    pEVSE->info.ucType = 2;
    pEVSE->info.dLng = 116.275833;
    pEVSE->info.dLat = 39.831944;

    pEVSE->info.GetSN = GetSN;
    pEVSE->info.GetID = GetID;
    pEVSE->info.GetType = GetType;
    pEVSE->info.GetLngLat = GetLngLat;

    pEVSE->state.ulArresterState = 0;
    pEVSE->state.ulKnockState = 0;
    pEVSE->state.ulPEState = 0;
    pEVSE->state.ulPowerOffState = 0;
    pEVSE->state.ulScramState = 0;

    pEVSE->state.GetArresterState = GetArresterState;
    pEVSE->state.GetKnockState = GetKnockState;
    pEVSE->state.GetPEState = GetPEState;
    pEVSE->state.GetPowerOffState = GetPowerOffState;
    pEVSE->state.GetScramState = GetScramState;

    return pEVSE;
}

static void ChargePointInit(void)
{
    static ChargePoint_t *pchargepoint[2];  //�ڶ��ж���
    uint8_t ucTotal;

    THROW_ERROR(GetTotalChargePoint(&ucTotal), ERR_LEVEL_WARNING);

    pListChargePoint = UserListCreate();
    int i;
    for(i = 0; i < ucTotal; i++)
    {
        pchargepoint[i] = ChargePointCreate(i);
        THROW_ERROR(pchargepoint[i]->info.GetConnectorType(pchargepoint[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(pchargepoint[i]->info.GetVolatageUpperLimits(pchargepoint[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(pchargepoint[i]->info.GetVolatageLowerLimits(pchargepoint[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(pchargepoint[i]->info.GetRatedCurrent(pchargepoint[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(pchargepoint[i]->info.GetRatedPower(pchargepoint[i]), ERR_LEVEL_WARNING);
        pListChargePoint->Add(pListChargePoint, pchargepoint[i]);
    }
}
void EVSEinit(void)
{
    pEVSE = EVSECreate();

    THROW_ERROR(pEVSE->info.GetSN(pEVSE), ERR_LEVEL_WARNING);
    THROW_ERROR(pEVSE->info.GetID(pEVSE), ERR_LEVEL_WARNING);
    THROW_ERROR(pEVSE->info.GetType(pEVSE), ERR_LEVEL_WARNING);
    THROW_ERROR(pEVSE->info.GetLngLat(pEVSE), ERR_LEVEL_WARNING);

    ChargePointInit();
}
