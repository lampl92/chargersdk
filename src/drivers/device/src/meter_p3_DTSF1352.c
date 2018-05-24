#include "meter.h"
#include <stdlib.h>


static int meter_get_all(void *pvmeter, int addr)
{
    meter_s *meter;
    uint32_t u32tab_reg[1] = { 0 };
    uint16_t u16tab_reg[6] = { 0 };
    int regs;
    
    meter = (meter_s *)pvmeter;
    //设置modbus从机地址
    modbus_set_slave(meter->mb, addr);
    
    //读取电能
    regs = modbus_read_registers(meter->mb, meter->config.energy_addr, 2, (uint16_t *)u32tab_reg);
    if (regs != 2)
        return -1;
    meter->status.energy = u32tab_reg[0] * 0.01;//0.01kWh
    //同时读取了电压ABC->电流ABC
    regs = modbus_read_registers(meter->mb, meter->config.volt_addr, 6, u16tab_reg);
    if (regs != 6)
        return -1;
    meter->status.volt[0] = u16tab_reg[0] * 0.1;//0.1V
    meter->status.volt[1] = u16tab_reg[1] * 0.1;//0.1V
    meter->status.volt[2] = u16tab_reg[2] * 0.1;//0.1V
    meter->status.curr[0] = u16tab_reg[3] * 0.01;//0.01A
    meter->status.curr[1] = u16tab_reg[4] * 0.01;//0.01A
    meter->status.curr[2] = u16tab_reg[5] * 0.01;//0.01A
    //计算功率
    meter->status.pwr[0] = meter->status.volt[0] * meter->status.curr[0];
    meter->status.pwr[1] = meter->status.volt[1] * meter->status.curr[1];
    meter->status.pwr[2] = meter->status.volt[2] * meter->status.curr[2];
    //没有频率直接赋值
    meter->status.freq = 50;
    
    return 0;
}

meter_s *meter_DTSF1352_init(void)
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
    config.volt_addr   = 0x0042;
    config.curr_addr   = 0x0045;
    config.pwr_addr    = 0xffff;//表示没有
    config.freq_addr   = 0xffff;
    
    meter = meter_create(&config);
    if (meter == NULL)
        return NULL;
    meter->get_all = meter_get_all;
    
    return meter;
}
