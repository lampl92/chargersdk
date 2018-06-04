#include "meter.h"
#include <stdlib.h>


static int meter_get_all(void *pvmeter, int addr)
{
    meter_s *meter;
    uint32_t u32energy = 0;
    uint16_t u16tab_reg[3] = { 0 };
    int regs;
    
    meter = (meter_s *)pvmeter;
    
    modbus_set_slave(meter->mb, addr);
    //读取电能
    regs = modbus_read_registers(meter->mb, meter->config.energy_addr, 2, u16tab_reg);
    if (regs != 2)
        return -1;
    u32energy = (u16tab_reg[0] << 16) + u16tab_reg[1];
    meter->status.energy = u32energy * 0.01;//0.01kWh
    //同时读取了电压->电流->有功功率
    regs = modbus_read_registers(meter->mb, meter->config.volt_addr, 3, u16tab_reg);
    if (regs != 3)
        return -1;
    meter->status.volt[0] = u16tab_reg[0] * 0.1;//0.1V
    meter->status.curr[0] = u16tab_reg[1] * 0.01;//0.01A
    meter->status.pwr[0] = u16tab_reg[2] * 0.001;//0.001kW
    //读取频率
    regs = modbus_read_registers(meter->mb, meter->config.freq_addr, 1, u16tab_reg);
    if (regs != 1)
        return -1;
    meter->status.freq = u16tab_reg[0] * 0.01;//0.01Hz 
    
    return 0;
}

meter_s *meter_DDSD1352_init(void)
{
    meter_s *meter;
    meter_config_s config;
    
    config.strUART = "UART7";
    config.band = 9600;
    config.data_bit = 8;
    config.parity = 'N';
    config.stop_bit = 1;
    config.mode = MODBUS_RTU_RS485;
    config.timeout_us = 100000;
    
    config.energy_addr = 0x0000;
    config.volt_addr   = 0x000B;
    config.curr_addr   = 0x000C;
    config.pwr_addr    = 0x000D;
    config.freq_addr   = 0x0011;
    
    meter = meter_create(&config);
    if (meter == NULL)
        return NULL;
    meter->get_all = meter_get_all;
    
    return meter;
}
