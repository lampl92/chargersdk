/**
* @file interface_charge.c
* @brief ���������Ҫ�Ľӿں���
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#include "includes.h"

/** @brief ��ȡ����ѹ����⾫�� +/-0.1V
 *
 * @param void
 * @return double �з����ͣ�����ʵ�ʵ�ѹ
 *
 */
double GetChargingVoltage(void)
{
    /** @todo (rgw#1#): Ŀǰ�ǻ�ȡ���ܱ��ѹ */
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
double GetChargingCurrent(void)
{
    /** @todo (rgw#1#): Ŀǰ�ǻ�ȡ���ܱ���� */
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
double GetChargingFrequence(void)
{
/** @todo (rgw#1#): �ӵ���ȡ */
    double tmpFreq;
    tmpFreq = 0;
    return tmpFreq;
}
