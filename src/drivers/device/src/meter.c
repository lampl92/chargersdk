#include "meter.h"
#include "modbus.h"
#include "sysinit.h"
#include <stdlib.h>
#include <string.h>

#include "meter_acrel.h"
#include "meter_ADE7953.h"

int meter_dummy_get_all(void *pvmeter, int dev_addr)
{
    //不支持的电表
    return 0;
}

meter_s *meter_init(int phase)
{
    meter_s *meter;
    int res;

    meter = (meter_s *)malloc(sizeof(meter_s));
    if (meter == NULL)
        return NULL;
    memset(meter, 0, sizeof(meter_s));

    if (xSysconf.xModule.use_meter == 2 ||     //2:DDSD1352-C(安科瑞单相)
        xSysconf.xModule.use_meter == 3 ||     //三相
        xSysconf.xModule.use_meter == 4)  //老电表
        {
            res = meter_acrel_init(meter);
        }
    else if (xSysconf.xModule.use_meter == 1)// 内部模块
    {
        res = meter_ADE7953_init(meter);
    }
    else
    {
        meter->get_all = meter_dummy_get_all;
    }
    
    if (res < 0)
    {
        free(meter);
        meter = NULL;
    }

    return meter;
}