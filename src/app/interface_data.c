/**
* @file interface_data.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#include "includes.h"
#include "interface.h"
#include "cJSON.h"
#include "stringName.h"
#include "utils.h"

#include "dbparser.h"
#include "debug.h"

#define BYTES_LEN 1024

ErrorCode_t OrderDBCreate(void)
{
    char memseg[BYTES_LEN];
    db_query_mm_t mm;

    if(db_fileexists("OrderDB") == 1)
    {
        return ERR_NO;
    }
    else
    {
        init_query_mm(&mm, memseg, BYTES_LEN);
        DB_PRINTF_DEBUG("CREATE TABLE!!\n");
        parse("CREATE TABLE OrderDB (OrderSN       STRING(32), \
                                      CardID        STRING(32), \
                                      Balance       INT, \
                                      CONID         INT, \
                                      StartTime     STRING(32), \
                                      StartType     INT, \
                                      LimitFee      INT, \
                                      StartPower    INT, \
                                      ServType      INT, \
                                      TotalPower    INT, \
                                      TotalPowerFee INT, \
                                      TotalServFee  INT, \
                                      TotalFee      INT, \
                                      PowerFee_sharp        INT, \
                                      ServiceFee_sharp      INT, \
                                      TotalPower_sharp      INT, \
                                      TotalPowerFee_sharp   INT, \
                                      TotalServFee_sharp    INT, \
                                      TotalTime_sharp       INT, \
                                      PowerFee_peak        INT, \
                                      ServiceFee_peak      INT, \
                                      TotalPower_peak      INT, \
                                      TotalPowerFee_peak   INT, \
                                      TotalServFee_peak    INT, \
                                      TotalTime_peak       INT, \
                                      PowerFee_shoulder        INT, \
                                      ServiceFee_shoulder      INT, \
                                      TotalPower_shoulder      INT, \
                                      TotalPowerFee_shoulder   INT, \
                                      TotalServFee_shoulder    INT, \
                                      TotalTime_shoulder       INT, \
                                      PowerFee_off_peak        INT, \
                                      ServiceFee_off_peak      INT, \
                                      TotalPower_off_peak      INT, \
                                      TotalPowerFee_off_peak   INT, \
                                      TotalServFee_off_peak    INT, \
                                      TotalTime_off_peak       INT, \
                                      PayType       INT, \
                                      StopType      INT, \
                                      StopTime      STRING(32));", &mm);
        return ERR_NO;
    }

}

ErrorCode_t OrderDBInsertItem(OrderData_t *pOrder)
{
    char memseg[BYTES_LEN];
    char cmd[BYTES_LEN] = {0};
    char strCardID[64];
    db_query_mm_t mm;
    int i;

    struct tm *ts;
    char tbuf_start [80] = {0};
    char tbuf_stop [80] = {0};

    ts = localtime (& pOrder->tStartTime);
    strftime (tbuf_start, sizeof (tbuf_start), "%Y-%m-%d %H:%M:%S", ts);
    ts = localtime (& pOrder->tStopTime);
    strftime (tbuf_stop, sizeof (tbuf_stop), "%Y-%m-%d %H:%M:%S", ts);

    HexToStr(pOrder->ucCardID, strCardID, 8);
    memset(cmd, '\0', sizeof(cmd));
    sprintf(cmd, "INSERT INTO OrderDB VALUES ('%s', '%s', %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s');\0",
                                                             pOrder->strOrderSN,
                                                             strCardID,
                                                             (int)(pOrder->dBalance * 100),
                                                             pOrder->ucCONID,
                                                             tbuf_start,
                                                             pOrder->ucStartType,
                                                             (int)(pOrder->dLimitFee * 100),
                                                             (int)(pOrder->dStartPower * 100),
                                                             (int)(pOrder->dTotalPower * 100),
                                                             (int)(pOrder->dTotalPowerFee * 100),
                                                             (int)(pOrder->dTotalServFee * 100),
                                                             (int)(pOrder->dTotalFee * 100),

                                                             (int)(pechProto->info.dPowerFee_sharp * 10000),
                                                             (int)(pechProto->info.dServiceFee_sharp * 10000),
                                                             (int)(pOrder->dTotalPower_sharp * 100),
                                                             (int)(pOrder->dTotalPowerFee_sharp * 100),
                                                             (int)(pOrder->dTotalServFee_sharp * 100),
                                                             (int)(pOrder->ulTotalTime_sharp),

                                                             (int)(pechProto->info.dPowerFee_peak * 10000),
                                                             (int)(pechProto->info.dServiceFee_peak * 10000),
                                                             (int)(pOrder->dTotalPower_peak * 100),
                                                             (int)(pOrder->dTotalPowerFee_peak * 100),
                                                             (int)(pOrder->dTotalServFee_peak * 100),
                                                             (int)(pOrder->ulTotalTime_peak),

                                                             (int)(pechProto->info.dPowerFee_shoulder * 10000),
                                                             (int)(pechProto->info.dServiceFee_shoulder * 10000),
                                                             (int)(pOrder->dTotalPower_shoulder * 100),
                                                             (int)(pOrder->dTotalPowerFee_shoulder * 100),
                                                             (int)(pOrder->dTotalServFee_shoulder * 100),
                                                             (int)(pOrder->ulTotalTime_shoulder),

                                                             (int)(pechProto->info.dPowerFee_off_peak * 10000),
                                                             (int)(pechProto->info.dServiceFee_off_peak * 10000),
                                                             (int)(pOrder->dTotalPower_off_peak * 100),
                                                             (int)(pOrder->dTotalPowerFee_off_peak * 100),
                                                             (int)(pOrder->dTotalServFee_off_peak * 100),
                                                             (int)(pOrder->ulTotalTime_off_peak),

                                                             pOrder->ucPayType,
                                                             pOrder->ucStopType,
                                                             tbuf_stop);
    init_query_mm(&mm, memseg, BYTES_LEN);
    parse(cmd, &mm);
}
#if 0
ErrorCode_t CreateOrderFile(void)
{
    FRESULT res;
    ErrorCode_t errcode;
    FIL fil;
    uint8_t *p;
    UINT br, bw;
    cJSON *order_root, *order_item;

    res = f_open(&fil, pathOrder,  FA_CREATE_NEW);
    switch(res)
    {
    case FR_OK:
        f_close(&fil);
        order_root = cJSON_CreateObject();
        order_item = cJSON_CreateArray();
        cJSON_AddItemToObject(order_root, jnMaxIndex, cJSON_CreateNumber(0));
        cJSON_AddItemToObject(order_root, jnOrder, order_item);
        p = cJSON_Print(order_root);
#ifdef DEBUG_INTERFACE_DATA
        printf_safe("order.txt �����ɹ�\n");
        printf_safe("%s\n", p);
#endif
        f_open(&fil, pathOrder, FA_WRITE);
        f_write(&fil, p, strlen(p), &bw);
        f_close(&fil);
        free(p);
        cJSON_Delete(order_root);
        errcode = ERR_NO;
        break;
    case FR_EXIST:
        f_close(&fil);
        f_open(&fil, pathOrder, FA_READ);
        p = (uint8_t *)malloc(f_size(&fil) * sizeof(uint8_t));

        f_read(&fil, p, f_size(&fil), &br);
        if(br == f_size(&fil))
        {
            order_root = cJSON_Parse(p);
        }
        else
        {
            errcode = ERR_FILE_RW;
            return errcode ;
        }
        order_item = cJSON_GetObjectItem(order_root, jnMaxIndex);
#ifdef DEBUG_INTERFACE_DATA
        printf_safe("%s\n", p);
        printf_safe("MaxIndex = %d\n", order_item->valueint);
#endif
        free(p);
        f_close(&fil);
        cJSON_Delete(order_root);
        errcode = ERR_NO;
        break;
    default:
        errcode = ERR_FILE_RW;
        break;
    }
    return errcode;
}

/** @brief
 *
 * @param pulIndex uint32_t*   ���ؽڵ�λ��
 * @param order mxml_node_t*   Ҫ��ӵĽڵ�
 * @return ErrorCode_t
 *
 */
ErrorCode_t DataAddOrderToFile(uint32_t *pulIndex, cJSON *OrderObjToAdd)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

//1. ��ȡ���ڵ�
//2. ��ȡ���ڵ�Index
//3. ��order��Index����Ϊ+1
//4. ��ӵ��ڵ����

    /*********************/

    *pulIndex = 1;

    return errcode;
}

/** @brief
 *
 * @param ulIndex uint32_t  ����
 * @param strNode uint8_t*  �ֶ����֣�����"StartDate"
 * @param pvGetVal void*    �����ֶ�����
 * @return ErrorCode_t
 *
 */
ErrorCode_t DataGetOrderFromFile( uint32_t ulIndex, uint8_t *strNode, void *pvGetVal)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */


    /*********************/

    return errcode;
}
cJSON *DataOrderObjCreate(CON_t *pCON, EVSE_t *pEVSE, uint32_t ulIndex)
{
    cJSON *pOrderObj;
    uint8_t strCardID[defCardIDLength * 2 + 1];
    uint8_t strOrderSN[7 * 2 + 1];
    int i;

    pOrderObj = cJSON_CreateObject();
    HexToStr(pCON->order.ucCardID, strCardID, defCardIDLength);


    cJSON_AddNumberToObject(pOrderObj, jnIndex, ulIndex);
    cJSON_AddStringToObject(pOrderObj, jnCardID, strCardID);
//    cJSON_AddStringToObject




//    pCON->order.OrderState = 5;         //1:�����У�2������У�3��ֹͣ�У�4���ѽ�����5��δ֪
//    pCON->order.ucAccountStatus = 0;    //�ʻ�״̬ 1��ע�Ῠ 0��δע�Ῠ
//    pCON->order.dBalance =0;            //���
//
//    pCON->order.dTotalPower =0;                  //�ܵ���
//    pCON->order.ucPaymentType =0;                //֧����ʽ 0������֧�� 1��������֧�� 2������δ֧��
//    pCON->order.ucStopType =0;                   //ֹͣ����
//    pCON->order.ucReserved =0;                   //����
//    memset(pCON->order.ucCardID, 0, defCardIDLength);//����//��taskrfid�и�ֵ
//    pCON->order.dTotalFee =0;                 //�ܷ���
//    memset(pCON->order.strOrderSN, 0, 7);       //������ˮ��
//    pCON->order.ucServiceFeeType =0;          //���������
//    pCON->order.dServiceFee =0;                //�����
//    pCON->order.ucTotalSegment =0;             //�����ϸ����
//    pCON->order.dDefSegPower =0;               //Ĭ�϶ε���
//    pCON->order.dDefSegFee =0;                //Ĭ�϶ε��
//    pCON->order.pChargeSegment = UserListCreate();


    return pOrderObj;
}
ErrorCode_t DataDelOrder(uint32_t ulIndex)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;
    return errcode;
}
#endif
