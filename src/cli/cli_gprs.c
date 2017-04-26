/**
* @file cli_gprs.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-26
*/
#include "includes.h"
#include "bsp_uart.h"

//extern UART_HandleTypeDef GPRS_UARTx_Handler;

void cli_gprs_fnt(int argc, char **argv)
{
    uint8_t buff[20];
    uint32_t len;
    if(argc == 2)
    {
        printf_safe("\nS:%s\n",argv[1]);
        strcpy(buff, argv[1]);
        strcat(buff, "\r\n");
        HAL_UART_Transmit(&GPRS_UARTx_Handler, buff, strlen(buff), 0xFFFF);
    }
    memset(buff, 0, 20);
    recvReadEx(pGprsRecvQue, buff, 0, &len);
    printf_safe("%s", buff);
}
tinysh_cmd_t cli_gprs_cmd =
{
    0,
    "gprs",
    "gprs at mode",
    0,
    cli_gprs_fnt,
    "<cr>", 0, 0
};
