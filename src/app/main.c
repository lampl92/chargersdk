/**
* @file main.c
* @brief 1. ��ʼ������.
*        2. ��������.
*           1\ vTaskFailureHandle  �������еĸ������ݽ��й��ϼ�⣬��������ʱ����
*           ��������
*           2\ vTaskCharger
*
* @author rgw
* @version v1.0
* @date 2016-10-26
*/

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
/* Private functions ---------------------------------------------------------*/
int main(void)
{
    /**
     * �ر�ȫ���ж�(����NMI��HardFault), ����ֲ�ļ�port.c�е�
     * ����prvStartFirstTask�л����¿���ȫ���жϡ�
     */
    DISABLE_INT();

    bsp_Init();
    sys_Init();

    SysTaskCreate();
    AppTaskCreate();

    vTaskStartScheduler();

    /*
      ���ϵͳ���������ǲ������е�����ģ����е����Ｋ�п��������ڶ�ʱ�������
      �߿�������� heap�ռ䲻����ɴ���ʧ�ܣ���Ҫ�Ӵ�FreeRTOSConfig.h�ļ��ж���
      ��heap��С��
      #define configTOTAL_HEAP_SIZE       ( ( size_t ) ( 30 * 1024 ) )
    */
    while(1);
}


