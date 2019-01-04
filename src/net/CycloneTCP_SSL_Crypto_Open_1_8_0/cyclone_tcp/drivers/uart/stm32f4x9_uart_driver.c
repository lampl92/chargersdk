/**
 * @file stm32f4x9_eth_driver.c
 * @brief STM32F429/439 Ethernet MAC controller
 *
 * @section License
 *
 * Copyright (C) 2010-2017 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneTCP Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.8.0
 **/

//Switch to the appropriate trace level
#define TRACE_LEVEL NIC_TRACE_LEVEL

//Dependencies
#include "stm32f4xx.h"
#include "modem.h"
#include "net_ppp.h"

#include "core/net.h"
#include "drivers/uart/stm32f4x9_uart_driver.h"
#include "ppp/ppp_hdlc.h"
#include "debug.h"


error_t stm32f4x9UartInit(void);
void stm32f4x9UartEnableIrq(void);
void stm32f4x9UartDisableIrq(void);
void stm32f4x9UartStartTx(void);


/**
 * @brief STM32F429/439 Ethernet MAC driver
 **/

const UartDriver stm32f4x9UartDriver =
{
   stm32f4x9UartInit,
   stm32f4x9UartEnableIrq,
   stm32f4x9UartDisableIrq,
   stm32f4x9UartStartTx
};


/**
 * @brief STM32F429/439 UART initialization
 * @param[in] interface Underlying network interface
 * @return Error code
 **/

error_t stm32f4x9UartInit(void)
{
   error_t error;

   //Debug message
   TRACE_INFO("Initializing STM32F4x9 PPP UART...\r\n");

   //Successful initialization
   return NO_ERROR;
}

/**
 * @brief Enable interrupts
 * @param[in] interface Underlying network interface
 **/

void stm32f4x9UartEnableIrq(void)
{
   //Enable UART interrupts
   //NVIC_EnableIRQ(UART7_IRQn);
}


/**
 * @brief Disable interrupts
 * @param[in] interface Underlying network interface
 **/

void stm32f4x9UartDisableIrq(void)
{
    //Always enable
}

void stm32f4x9UartStartTx(void)
{
    int_t a;
    uint8_t c[1];
    
    while (pppHdlcDriverReadTxQueue(net_dev->interface, &a) == FALSE)
    {
        if (a != EOF)
        {
            c[0] = (uint8_t)a;
            uart_write_fast(pModem->uart_handle, c, 1);
        }
        else
        {
            break;
        }
    }
}