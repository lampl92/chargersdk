/**
* @file taskOTA.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "taskcreate.h"
#include "taskOTA.h"
#include "modbus.h"
void vTaskOTA(void *pvParameters)
{
    modbus_t *mb;
    uint16_t tab_reg[32] = { 0 }; 
    int i = 0;
    int regs;
    int clock1, clock2;
    mb = modbus_new_rtu("UART7", 9600, 'N', 8, 1);
    modbus_rtu_set_serial_mode(mb, MODBUS_RTU_RS485);
    modbus_connect(mb);   
    modbus_set_response_timeout(mb, 0, 500000);  
  
    while(1)
    {
        printf("%d\n", clock1=clock());
    modbus_set_slave(mb, 1);  //设置modbus从机地址  
    regs = modbus_read_registers(mb, 0x40000, 10, tab_reg);  
        i++;
        printf("%d\n", clock2 = clock()-clock1);
        
#if DEBUG_TASK
        printf_safe("%s\n", TASKNAME_OTA);
#endif
        vTaskDelay(1000);
    }
}
