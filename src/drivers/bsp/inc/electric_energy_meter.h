#ifndef ELECTRIC_ENERGY_METER_H_INCLUDED
#define ELECTRIC_ENERGY_METER_H_INCLUDED
#include "sys_types.h"
#include "bsp_define.h"
#define DTSF1352



#define voltage_a  0x0042
#define voltage_b  0x0043
#define voltage_c  0x0044

#define current_a  0x0045
#define current_b  0x0046
#define current_c  0x0047

#define frequency_a  0
#define frequency_b  0
#define frequency_c  0

#define power_a  0
#define power_b  0
#define power_c  0

#define electric_energy_l 0x0001
#define electric_energy_h 0x0000
			  
 
#define voltage  0x000b
#define current  0x000c
#define power    0x000d
#define electric_energy_l 0x0001
#define electric_energy_h 0x0000
#define frequency 0x0011




UART_HandleTypeDef UART7_Handler;
UART_HandleTypeDef UART7_RS485Handler;
void RS485_Init(u32 bound);
double Get_Electricity_meter_massage_energy(uint8_t add);

double Get_Electricity_meter_massage_current(uint8_t add, uint8_t massage);


double Get_Electricity_meter_massage_voltage(uint8_t add, uint8_t massage);

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
