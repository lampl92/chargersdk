#ifndef METER_H
#define METER_H

#include "modbus.h"

typedef struct _meter_reg
{ 
    uint16_t energy_addr;
    uint16_t volt_addr;
    uint16_t curr_addr;
    uint16_t freq_addr;
    uint16_t pwr_addr;
}meter_reg_s;

typedef struct _meter_config
{
    char *strUART;
    int band;
    int data_bit;
    char parity;
    int stop_bit;
    int mode;
    uint32_t timeout_us;
}meter_config_s;

typedef struct _meter_status
{
    double energy;
    double volt[3];
    double curr[3];
    double pwr[3];
    double freq;
    
}meter_status_s;

typedef struct _meter
{
    modbus_t *mb;
    meter_reg_s regs;
    meter_config_s config;
    meter_status_s status;
    int (*get_all)(void *meter, int dev_addr);
    
}meter_s;

meter_s *meter_create(meter_config_s *config);
meter_s *meter_init(int phase);

#endif