#include "meter.h"
#include "modbus.h"
#include <stdlib.h>
#include <string.h>

meter_s *meter_create(meter_config_s *config)
{
    meter_s *meter;
    int res;
    
    meter = (meter_s *)malloc(sizeof(meter_s));
    if (meter == NULL)
        return NULL;
    memset(meter, 0, sizeof(meter_s));
    
    meter->mb = modbus_new_rtu(config->strUART, config->band, config->parity, config->data_bit, config->stop_bit);
    if (meter->mb == NULL)
    {
        free(meter);
        return NULL;
    }
    modbus_rtu_set_serial_mode(meter->mb, config->mode);
    res = modbus_connect(meter->mb);
    if (res < 0)
    {
        free(meter->mb);
        free(meter);
        return NULL;
    }
    modbus_set_response_timeout(meter->mb, 0, config->timeout_us);
    
    return meter;
}

#include "meter_p1_DDSD1352.h"
#include "meter_p3_DTSF1352.h"

meter_s *meter_init(int phase)
{
    meter_s *meter;
    
    if (phase == 1)
        meter = meter_DDSD1352_init();
    else
        meter = meter_DTSF1352_init();

    return meter;
}