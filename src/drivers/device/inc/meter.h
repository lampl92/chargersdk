#ifndef METER_H
#define METER_H

#include "modbus.h"

typedef struct _meter_config
{
    char *strUART;
    int band;
    int data_bit;
    char parity;
    int stop_bit;
    int mode;
    uint32_t timeout_us;
    
    uint16_t volt_addr;
    uint16_t curr_addr;
    uint16_t freq_addr;
    uint16_t pwr_addr;
    uint16_t energy_addr;
    
}meter_config_s;

typedef struct _meter_status
{
    double volt[3];
    double curr[3];
    double pwr[3];
    double freq;
    double energy;
    
}meter_status_s;

typedef struct _meter
{
    modbus_t *mb;
    meter_config_s config;
    meter_status_s status;
    int (*get_all)(void *meter, int addr);
    
}meter_s;

meter_s *meter_create(meter_config_s *config);
meter_s *meter_init(int phase);

#endif