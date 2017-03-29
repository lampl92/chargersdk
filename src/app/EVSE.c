/**
* @file EVSE.c
* @brief EVSE��ʼ��
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "includes.h"
#include "evse_globals.h"
#include "userlib_list.h"
#include "interface.h"
#include "stringName.h"
#include "cJSON.h"
#include "utils.h"
/*---------------------------------------------------------------------------/
/                               ���ļ���ȡ���׮��Ϣ
/---------------------------------------------------------------------------*/

static ErrorCode_t GetEVSECfg(void *pvEVSE)
{
    FIL f;
    FRESULT res;
    uint8_t *rbuff;
    FSIZE_t fsize;
    UINT  br;   //byte read

    cJSON *jsEVSEObj, *jsItem;
    char *str;
    uint32_t ulTotalSegs;
    static TemplSeg_t *pTemplSeg;
    struct tm *ts;

    EVSEInfo_t *pInfo;
    ErrorCode_t errcode;

    pInfo = &(((EVSE_t *)pvEVSE)->info);

    /*��ȡ�ļ�*/
    ThrowFSCode(res = f_open(&f, pathEVSECfg, FA_READ));
    if(res != FR_OK)
    {
        errcode = ERR_FILE_RW;
        return errcode;
    }
    fsize = f_size(&f);
    rbuff = (uint8_t *)malloc(fsize * sizeof(uint8_t));
    ThrowFSCode(res = f_read(&f, rbuff, fsize, &br));
    if(fsize != br)
    {
        errcode = ERR_FILE_RW;
        return errcode;
    }

    /*json����*/
    jsEVSEObj = cJSON_Parse(rbuff);
    jsItem = cJSON_GetObjectItem(jsEVSEObj, jnEVSESN);
//    pInfo->ucSN
    str = jsItem->valuestring;





    jsItem = cJSON_GetObjectItem(jsEVSEObj, jnTemplSegArray);
    ulTotalSegs = cJSON_GetArraySize(jsItem);
    cJSON_Delete(jsItem);
    printf_safe("ulTotalSegs = %d\n", ulTotalSegs);

    ulTotalSegs = 4;/** @fixme (rgw#1#): dummy,Ӧ�ô��ļ���ȡ */
    pTemplSeg = (TemplSeg_t *)malloc(sizeof(TemplSeg_t));
    //pTemplSeg->tStartTime =

    free(rbuff);
}

/** @brief �豸Ψһ���к�,�ͳ���
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *                     ERR_NO
 *                     ERR_FILE_RW
 *                     ERR_FILE_NO
 *
 */
static ErrorCode_t GetSN(void *pvEVSE)
{
    uint8_t tmpSN[defEVSESNLength];
    uint8_t tmpLength;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    memset(tmpSN, 0, defEVSESNLength);
    tmpLength = 0;

    /** @todo (rgw#1#): ���ļ���ȡSN ����ȡSN����*/

    //...

    /*********************/
    pEVSE->info.ucSNLength = tmpLength;
    memmove(pEVSE->info.ucSN, tmpSN, defEVSESNLength);

    return errcode;
}

/** @brief ƽ̨����ID
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *                     ERR_NO
 *                     ERR_FILE_RW
 *                     ERR_FILE_NO
 *
 */
static ErrorCode_t GetID(void *pvEVSE)
{
    uint8_t tmpID[defEVSEIDLength];
    uint8_t tmpLength;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    memset(tmpID, 0, defEVSEIDLength);
    tmpLength = 0;

    /** @todo (rgw#1#): ���ļ���ȡID ����ȡID����*/

    //...

    /*********************/

    pEVSE->info.ucIDLenght = tmpLength;
    memmove(pEVSE->info.ucID, tmpID, defEVSEIDLength);

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
 *                     ERR_NO
 *                     ERR_FILE_RW
 *                     ERR_FILE_NO
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

    //...

    /*********************/

    pEVSE->info.ucType = tmpType;

    return errcode;
}

/** @brief ǹ������
 *
 * @param pvEVSE void*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetTotalCON(void *pvEVSE)
{
    uint8_t tmpTotal;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    tmpTotal = 2;//ǹ��������2��������ǹ
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pEVSE->info.ucTotalCON = tmpTotal;

    return errcode;
}

/** @brief ��γ�ȣ���������λ
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *                     ERR_NO
 *                     ERR_FILE_RW
 *                     ERR_FILE_NO
 *
 */
static ErrorCode_t GetLngLat(void *pvEVSE)
{
    double tmpLng, tmpLat;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    tmpLng = 0;
    tmpLat = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pEVSE->info.dLng = tmpLng;
    pEVSE->info.dLat = tmpLng;

    return errcode;
}
/** @todo (rgw#1#): ����ļ�����
 */
//    "ServiceFeeType": 1,
//    "ServiceFee": 1,
//    "DefSegFee": 1.2,

/*---------------------------------------------------------------------------/
/                               ��������ȡ���׮״̬
/---------------------------------------------------------------------------*/


/** @brief ��ü�ͣ״̬
 *          0 �޼�ͣ
 *          1 �м�ͣ
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *             ERR_NO
 *             ERR_SCRAM_FAULT
 *
 */
static ErrorCode_t GetScramState(void *pvEVSE)
{
    uint32_t tmpScramState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    //errcode = ERR_SCRAM_FAULT;
    errcode = ERR_NO;
    tmpScramState = 0;

    /** @todo (rgw#1#): ʵ�ִ��� */

    //...

    /*********************/

    pEVSE->status.ulScramState = tmpScramState;

    return errcode;
}

/** @brief �Ӵ�������ȡײ��״̬
 *          0 ��ײ��
 *          1 ��ײ��
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *             ERR_NO
 *             ERR_GSENSOR_FAULT
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

    /** @todo (rgw#1#): ��ȡ�������ٶȴ����� */

    //...

    /*********************/

    pEVSE->status.ulKnockState = tmpKnockState;

    return  errcode;
}

/** @brief ��ȡ�����ӵ�������״̬
 *          0 ����
 *          1 �쳣
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *             ERR_NO
 *             ERR_PE_FAULT
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

    //...

    /*********************/

    pEVSE->status.ulPEState = tmpPEState;

    return errcode;
}

/** @brief ��ȡ����״̬
 *          0 ����
 *          1 ����
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *             ERR_NO
 *             ERR_POWEROFF_DECT_FAULT
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

    //...

    /*********************/

    pEVSE->status.ulPowerOffState = tmpOffState;

    return errcode;
}

/** @brief ��ȡ������״̬
 *          0 ����
 *          1 �쳣
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *             ERR_NO
 *             ERR_ARRESTER_FAULT
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

    /** @todo (rgw#1#): ʵ�ִ��� */

    //...

    /*********************/

    pEVSE->status.ulArresterState = tmpArresterState;
    return errcode;
}

EVSE_t *EVSECreate(void)
{
    EVSE_t *pEVSE;
    pEVSE = (EVSE_t *)malloc(sizeof(EVSE_t));

    memset(pEVSE->info.ucSN, 0, defEVSESNLength);
    memset(pEVSE->info.ucID, 0, defEVSEIDLength);
    pEVSE->info.ucType = 2;
    pEVSE->info.ucTotalCON = 2;
    pEVSE->info.dLng = 116.275833;
    pEVSE->info.dLat = 39.831944;
    pEVSE->info.ucServiceFeeType = 0;
    pEVSE->info.dServiceFee = 0;
    pEVSE->info.dDefSegFee = 0;

    pEVSE->info.GetEVSECfg = GetEVSECfg;
    pEVSE->info.GetSN = GetSN;
    pEVSE->info.GetID = GetID;
    pEVSE->info.GetType = GetType;
    pEVSE->info.GetTotalCON = GetTotalCON;
    pEVSE->info.GetLngLat = GetLngLat;

    pEVSE->info.pTemplSeg = NULL;
    pEVSE->info.pTemplSeg = UserListCreate();


    pEVSE->status.ulArresterState = 0;
    pEVSE->status.ulKnockState = 0;
    pEVSE->status.ulPEState = 0;
    pEVSE->status.ulPowerOffState = 0;
    pEVSE->status.ulScramState = 0;

    pEVSE->status.GetArresterState = GetArresterState;
    pEVSE->status.GetKnockState = GetKnockState;
    pEVSE->status.GetPEState = GetPEState;
    pEVSE->status.GetPowerOffState = GetPowerOffState;
    pEVSE->status.GetScramState = GetScramState;

    return pEVSE;
}

static void CONInit(void)
{
    static CON_t *pCON[2];  //�ڶ��ж���

    pListCON = UserListCreate();
    int i;
    for(i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON[i] = CONCreate(i);

        THROW_ERROR(i, pCON[i]->info.GetSocketType(pCON[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(i, pCON[i]->info.GetVolatageUpperLimits(pCON[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(i, pCON[i]->info.GetVolatageLowerLimits(pCON[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(i, pCON[i]->info.GetRatedCurrent(pCON[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(i, pCON[i]->info.GetRatedPower(pCON[i]), ERR_LEVEL_WARNING);

        pListCON->Add(pListCON, pCON[i]);
    }
}
void EVSEinit(void)
{
    pEVSE = EVSECreate();

    THROW_ERROR(defDevID_File, pEVSE->info.GetSN(pEVSE), ERR_LEVEL_WARNING);
    THROW_ERROR(defDevID_File, pEVSE->info.GetID(pEVSE), ERR_LEVEL_WARNING);
    THROW_ERROR(defDevID_File, pEVSE->info.GetType(pEVSE), ERR_LEVEL_WARNING);
    THROW_ERROR(defDevID_File, pEVSE->info.GetTotalCON(pEVSE), ERR_LEVEL_WARNING);
    THROW_ERROR(defDevID_File, pEVSE->info.GetLngLat(pEVSE), ERR_LEVEL_WARNING);

    CONInit();

    pRFIDDev = RFIDDevCreate();
}
