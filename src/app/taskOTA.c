/**
* @file taskOTA.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "taskcreate.h"
#include "taskOTA.h"
#include "evse_define.h"
#include "evse_config.h"
#include "interface_ftpserver.h"
#include "bsp.h"

#include "lcddrv.h"
#include "ifconfig.h"
#include "file_op.h"
#include "stringName.h"

void ota_gui(void)
{
    char disp_str[200] = { 0 };

    LCD_ShowString(100, 100, 200, 20, 16, ifconfig.status.strIP);
    LCD_ShowString(100, 120, 200, 20, 16, ifconfig.status.strGate);
    LCD_ShowString(100, 140, 200, 20, 16, ifconfig.status.strMask);
    LCD_ShowString(100, 160, 200, 20, 16, ifconfig.status.strDNS1);
    LCD_ShowString(100, 180, 200, 20, 16, ifconfig.status.strDNS2);

    sprintf(disp_str, "ftp://%s", ifconfig.status.strIP);
    LCD_ShowString(100, 220, 300, 20, 16, disp_str);
}

void vTaskOTA(void *pvParameters)
{
#if EVSE_USING_GUI
    EventBits_t bits;
    char disp_str[200] = { 0 };
    int flg;

    flg = get_bmp_check_tmp();
#endif

    int gps_handle;
    char ch[10] = { 0 };
    uint8_t i = 0;
    gps_handle = uart_open("USART6", 115200, 8, 'N', 1);
    
    while (1)
    {
#if EVSE_USING_GUI
        if (flg == 3)//有文件并且设置过3
        {
            bits = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_REQ_StartFTP, pdTRUE, pdTRUE, 0);
            if ((bits & defEventBitHMI_REQ_StartFTP) == defEventBitHMI_REQ_StartFTP)
            {
                LCD_Clear(BLUE);
                LCD_ShowString(100, 20, 340, 20, 16, "Please MUST plug the PHY moudle And cable!");
                LCD_ShowString(100, 60, 300, 20, 16, "network starting...");
                while (net_dev->state == NET_STATE_INIT || net_dev->state == NET_STATE_ERR)
                {
                    vTaskDelay(100);
                }
                LCD_ShowString(100, 80, 300, 20, 16, "ftp start...");
                ota_gui();
            }
            else
            {
                LCD_ShowString(100, 40, 300, 20, 16, "Checking files, please wait...");
                vTaskDelay(500);
            }
        }
#endif

        sprintf(ch, "GPS,%d\r\n", i++);
        uart_write_fast(gps_handle, ch, strlen(ch));
#if DEBUG_TASK
        printf_safe("%s\n", TASKNAME_OTA);
#endif
        vTaskDelay(1000);
    }
}
