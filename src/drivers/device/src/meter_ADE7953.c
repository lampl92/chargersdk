#include "meter.h"
#include "sysinit.h"
#include "cfg_parse.h"
#include "stringName.h"
#include "evse_globals.h"
#include "Metering.h"

static int meter_module_get_all(void *pvmeter, int dev_addr)
{
    meter_s *meter;
    
    meter = (meter_s *)pvmeter;
    ReadAde7953DataRegister();
    meter->status.volt[0] = TEST_YUYE_VA;
    vTaskDelay(1000);
    
    return 0; //内部模块在meterinit中初始化
}

int meter_ADE7953_init(meter_s *meter)
{
    //ADE7953 初始化代码
    get_ade7953();
    //代码end
    meter->get_all = meter_module_get_all;

    return 0;
}