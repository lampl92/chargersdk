#ifndef ELECTRIC_ENERGY_METER_H_INCLUDED
#define ELECTRIC_ENERGY_METER_H_INCLUDED
#include "bsp.h"
UART_HandleTypeDef UART7_Handler;
UART_HandleTypeDef UART7_RS485Handler;
void RS485_Init(u32 bound);
double Get_Electricity_meter_massage_energy(uint8_t add);
double Get_Electricity_meter_massage_current(uint8_t add);
double Get_Electricity_meter_massage_voltage(uint8_t add);
double Get_Electricity_meter_massage_power(uint8_t add);
double Get_Electricity_meter_massage_frequency(uint8_t add);
typedef union
{
    float yy_test_float;
    uint8_t an[4];
} a1;
a1 A1;
u8 RS485_TX_BUF[10];
u8 RS485_RX_BUF[64];
u8 RS485_RX_MODBUS[64];
u8 RS485_RX_MODBUS_CNT;
u8 RS485_RX_CNT;
uint8_t flag_rs485_send;


#endif /* ELECTRIC_ENERGY_METER_H_INCLUDED */
