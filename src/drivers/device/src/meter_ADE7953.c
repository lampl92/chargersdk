#include "meter.h"
#include "sysinit.h"
#include "cfg_parse.h"
#include "stringName.h"
#include "evse_globals.h"
#include <stdlib.h>

static int meter_module_get_all(void *pvmeter, int dev_addr)
{
    return 0; //内部模块在meterinit中初始化
}

int meter_ADE7953_init(meter_s *meter)
{
    //ADE7953 初始化代码
    
    //代码end
    meter->get_all = meter_module_get_all;

    return 0;
}