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
//    modbus_set_debug(meter->mb, 1);
    modbus_set_error_recovery(meter->mb, MODBUS_ERROR_RECOVERY_PROTOCOL);
    res = modbus_connect(meter->mb);
    if (res < 0)
    {
        free(meter->mb);
        free(meter);
        return NULL;
    }
    modbus_set_response_timeout(meter->mb, 0, config->timeout_us);
    
    memcpy(&meter->config, config, sizeof(meter_config_s));
    
    return meter;
}

#include "meter_acrel.h"

meter_s *meter_init(int phase)
{
    meter_s *meter;
    
    meter = meter_acrel_init();

    return meter;
}