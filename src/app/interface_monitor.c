/**
* @file interface_monitor.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#include "includes.h"
#include "interface.h"


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
