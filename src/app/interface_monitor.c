/**
* @file interface_monitor.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#include "includes.h"
#include "interface.h"


/** @brief ��ȡ����ѹ����⾫�� +/-0.1V
 *
 * @param void
 * @return double �з����ͣ�����ʵ�ʵ�ѹ
 *
 */
double GetChargingVoltage(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->ucChargePointID;
    /** @todo (rgw#1#): ��ȡ���ܱ��ѹ */
    double tmpVolt;
    tmpVolt = 0;
    return tmpVolt;
}

/** @brief ��ȡ����������⾫��+/-0.1A
 *
 * @param void
 * @return double �з����ͣ�����ʵ�ʵ���
 *
 */
double GetChargingCurrent(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->ucChargePointID;
    /** @todo (rgw#1#): ��ȡ���ܱ���� */
    double tmpCurr;
    tmpCurr = 0;
    return tmpCurr;
}

/** @brief ��ȡ��ԴƵ��
 *
 * @param void
 * @return double
 *
 */
double GetChargingFrequence(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->ucChargePointID;
/** @todo (rgw#1#): �ӵ���ȡ */
    double tmpFreq;
    tmpFreq = 0;
    return tmpFreq;
}

/** @brief ��ü�ͣ״̬
 *
 * @param void
 * @return uint32_t 0 �޼�ͣ
 *                  1 �м�ͣ
 *
 */
/** @note (rgw#1#): ��ʱ�Բ�ѯ��ʽ��ȡ��ͣ״̬ */
uint32_t GetScramState(void)
{
    uint32_t tmpScramState;
    tmpScramState = 0;

    /** @todo (rgw#1#): ʵ�ִ��� */

    return tmpScramState;
}

/** @brief �Ӵ�������ȡײ��״̬
 *
 * @param void
 * @return uint32_t 0 ��ײ��
 *                  1 ��ײ��
 *
 */
uint32_t GetKnockState(void)
{
    uint32_t tmpKnockState;
    tmpKnockState = 0;

    /** @todo (rgw#1#): ʵ�ִ��� */

    return  tmpKnockState;
}

/** @brief ��ȡ�����ӵ�������״̬
 *
 * @param void
 * @return uint32_t 0 ����
 *                  1 �쳣
 *
 */
uint32_t GetPEState(void)
{
    uint32_t tmpPEState;
    tmpPEState = 0;

    /** @todo (rgw#1#): ʵ�ִ��� */

    return tmpPEState;
}

/** @brief ��ȡ��ǹ״̬��Ӧͬʱ������1��CC���ͼ���4��CP��
 *
 * @param pPoint ChargePoint_t*
 * @return uint32_t 0 �޲�ǹ
 *                  1 �в�ǹ
 *
 */
uint32_t GetPlugState(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->ucChargePointID;
    uint32_t tmpPlugState;
    tmpPlugState = 0;

    return tmpPlugState;
}

/** @brief ��ȡ����״̬
 *
 * @param void
 * @return uint32_t 0 ����
 *                  1 ����
 *
 */
uint32_t GetPowerOffState(void)
{
    return 0;
}

/** @brief ��ȡ������״̬
 *
 * @param void
 * @return uint32_t 0 ����
 *                  1 �쳣
 *
 */
uint32_t GetArresterState(void)
{
    return 0;
}
