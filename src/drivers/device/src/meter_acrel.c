#include "meter.h"
#include "sysinit.h"
#include "cfg_parse.h"
#include "stringName.h"
#include "evse_globals.h"
#include <stdlib.h>

static int meter_p1_get_all(void *pvmeter, int dev_addr)
{
    meter_s *meter;
    uint32_t u32energy = 0;
    uint16_t u16tab_reg[3] = { 0 };
    int regs;
    
    meter = (meter_s *)pvmeter;
    
    modbus_set_slave(meter->mb, dev_addr);
    //读取电能
    regs = modbus_read_registers(meter->mb, meter->regs.energy_addr, 2, u16tab_reg);
    if (regs != 2)
        return -1;
    u32energy = (u16tab_reg[0] << 16) + u16tab_reg[1];
    meter->status.energy = u32energy * 0.01;//0.01kWh
    //同时读取了电压->电流->有功功率
    regs = modbus_read_registers(meter->mb, meter->regs.volt_addr, 3, u16tab_reg);
    if (regs != 3)
        return -1;
    meter->status.volt[0] = u16tab_reg[0] * 0.1;//0.1V
    meter->status.curr[0] = u16tab_reg[1] * 0.01;//0.01A
    meter->status.pwr[0] = u16tab_reg[2] * 0.001;//0.001kW
    //读取频率
    regs = modbus_read_registers(meter->mb, meter->regs.freq_addr, 1, u16tab_reg);
    if (regs != 1)
        return -1;
    meter->status.freq = u16tab_reg[0] * 0.01;//0.01Hz 
    
    return 0;
}

static int meter_p3_get_all(void *pvmeter, int addr)
{
    meter_s *meter;
    uint32_t u32energy = 0;
    uint16_t u16tab_reg[6] = { 0 };
    int regs;
    
    meter = (meter_s *)pvmeter;
    //设置modbus从机地址
    modbus_set_slave(meter->mb, addr);
    
    //读取电能
    regs = modbus_read_registers(meter->mb, meter->regs.energy_addr, 2, u16tab_reg);
    if (regs != 2)
        return -1;
    u32energy = (u16tab_reg[0] << 16) + u16tab_reg[1];
    meter->status.energy = u32energy * 0.01;//0.01kWh
    //同时读取了电压ABC->电流ABC
    regs = modbus_read_registers(meter->mb, meter->regs.volt_addr, 6, u16tab_reg);
    if (regs != 6)
        return -1;
    meter->status.volt[0] = u16tab_reg[0] * 0.1;//0.1V
    meter->status.volt[1] = u16tab_reg[1] * 0.1;//0.1V
    meter->status.volt[2] = u16tab_reg[2] * 0.1;//0.1V
    meter->status.curr[0] = u16tab_reg[3] * 0.01;//0.01A
    meter->status.curr[1] = u16tab_reg[4] * 0.01;//0.01A
    meter->status.curr[2] = u16tab_reg[5] * 0.01;//0.01A
    //计算功率 kW
    meter->status.pwr[0] = meter->status.volt[0] * meter->status.curr[0] / 1000;
    meter->status.pwr[1] = meter->status.volt[1] * meter->status.curr[1] / 1000;
    meter->status.pwr[2] = meter->status.volt[2] * meter->status.curr[2] / 1000;

    //读取频率
    if (meter->regs.freq_addr != 0xffff)
    {
        regs = modbus_read_registers(meter->mb, meter->regs.freq_addr, 1, u16tab_reg);
        if (regs != 1)
            return -1;
        meter->status.freq = u16tab_reg[0] * 0.01;//0.01Hz  
    }
    else
    {
        meter->status.freq = 50;
    }
    
    return 0;
}

static int modbus_config(meter_s *meter, meter_config_s *config)
{
    int res;
    
    meter->mb = modbus_new_rtu(config->strUART, config->band, config->parity, config->data_bit, config->stop_bit);
    if (meter->mb == NULL)
    {
        return -1;
    }
    modbus_rtu_set_serial_mode(meter->mb, config->mode);
//    modbus_set_debug(meter->mb, 1);
    modbus_set_error_recovery(meter->mb, MODBUS_ERROR_RECOVERY_PROTOCOL);
    res = modbus_connect(meter->mb);
    if (res < 0)
    {
        free(meter->mb);
        return -2;
    }
    modbus_set_response_timeout(meter->mb, 0, config->timeout_us);
    
    memcpy(&meter->config, config, sizeof(meter_config_s));
    
    return 0;
}

int meter_acrel_init(meter_s *meter)
{
    int res;
    meter_config_s config;
    
    config.strUART = "UART7";
    config.band = 9600;
    config.data_bit = 8;
    config.parity = 'N';
    config.stop_bit = 1;
    config.mode = MODBUS_RTU_RS485;
    config.timeout_us = 100000;
    
    res = modbus_config(meter, &config);
    if (res < 0)
        return res;

    cfg_get_uint16(pathMeterCfg, &meter->regs.energy_addr, "%s%d.%s", jnMeter, xSysconf.xModule.use_meter, jnMeterEnergyAddr);
    cfg_get_uint16(pathMeterCfg, &meter->regs.volt_addr, "%s%d.%s", jnMeter, xSysconf.xModule.use_meter, jnMeterVoltAddr);
    cfg_get_uint16(pathMeterCfg, &meter->regs.curr_addr, "%s%d.%s", jnMeter, xSysconf.xModule.use_meter, jnMeterCurrAddr);
    cfg_get_uint16(pathMeterCfg, &meter->regs.pwr_addr, "%s%d.%s", jnMeter, xSysconf.xModule.use_meter, jnMeterPwrAddr);
    cfg_get_uint16(pathMeterCfg, &meter->regs.freq_addr, "%s%d.%s", jnMeter, xSysconf.xModule.use_meter, jnMeterFreqAddr);
    
    if (pEVSE->info.ucPhaseLine == 1)
    {
        meter->get_all = meter_p1_get_all;
    }
    else if (pEVSE->info.ucPhaseLine == 3)
    {
        meter->get_all = meter_p3_get_all;
    }
    
    return 0;
}
