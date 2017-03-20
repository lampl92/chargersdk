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
ErrorCode_t CreateOrderFile(void)
{
    FRESULT res;
    ErrorCode_t errcode;
    FIL fil;
    uint8_t *p;
    UINT br,bw;
    cJSON *order_root, *order_item;

    res = f_open(&fil, "system/order.txt",  FA_CREATE_NEW);
    switch(res)
    {
        case FR_OK:
            f_close(&fil);
            order_root = cJSON_CreateObject();
            order_item = cJSON_CreateArray();
            cJSON_AddItemToObject(order_root, "MaxIndex", cJSON_CreateNumber(0));
            cJSON_AddItemToObject(order_root, "Order", order_item);
            p = cJSON_Print(order_root);
            #ifdef DEBUG_INTERFACE_DATA
            printf_safe("order.txt �����ɹ�\n");
            printf_safe("%s\n", p);
            #endif
            f_open(&fil, "system/order.txt", FA_WRITE);
            f_write(&fil, p, strlen(p), &bw);
            f_close(&fil);
            free(p);
            cJSON_Delete(order_root);
            errcode = ERR_NO;
            break;
        case FR_EXIST:
            f_close(&fil);
            f_open(&fil, "system/order.txt", FA_READ);
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
            order_item = cJSON_GetObjectItem(order_root, "MaxIndex");
            #ifdef DEBUG_INTERFACE_DATA
            printf_safe("%s\n",p);
            printf_safe("MaxIndex = %d\n", order_item->valueint);
            #endif
            free(p);
            f_close(&fil);
            cJSON_Delete(order_root);
            errcode = ERR_NO;
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
ErrorCode_t DataAddOrder(uint32_t *pulIndex, cJSON *OrderObjToAdd)
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
ErrorCode_t DataGetOrder( uint32_t ulIndex, uint8_t *strNode, void *pvGetVal)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

/** @todo (rgw#1#):  */


/*********************/

    return errcode;
}
cJSON *DataOrderCreate(CON_t *pCON, EVSE_t *pEVSE, uint32_t ulIndex)
{
    cJSON *pOrderObj;
    pOrderObj = cJSON_CreateObject();

    cJSON_AddItemToObject(pOrderObj, "Index", cJSON_CreateNumber(ulIndex));


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
ErrorCode_t makeOrder()
{

}
