#include "bsp.h"

extern UART_HandleTypeDef CLI_UARTx_Handler;

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;
FILE __stderr;
/**
  * @brief  �ض����׼�⺯��printf��Ӧ�Ļ����⺯��.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /*
    ����Windows���ն˻�����"\r\n", ��tinysh��ȫ���õ���'\n'��β,�޷���ɻ���,
    �ڴ˽��м���
    */

    int tempch = ch;
    __set_PRIMASK(1);    //���ӹر��жϹ���,��ֹ������ʹ��ʱ���ֳ�ͻ
    if(tempch == '\n')
    {
        ch = '\r';
        HAL_UART_Transmit(&CLI_UARTx_Handler, (uint8_t *)&ch, 1, 0xFFFF);
    }
    HAL_UART_Transmit(&CLI_UARTx_Handler, (uint8_t *)&tempch, 1, 0xFFFF);
    __set_PRIMASK(0);
    return ch;
}

int fgetc(FILE *f)
{
    int ch;
    HAL_UART_Receive(&CLI_UARTx_Handler,(uint8_t *)&ch, 1, 1000);
    return ch;
}


int ferror(FILE *f)
{
    return EOF;
}

void _ttywrch(int ch)
{
    char tempch = ch;
    HAL_UART_Transmit(&CLI_UARTx_Handler, (uint8_t *)&tempch, 1, 0xFFFF);
}

void _sys_exit(int x)
{
    x = x;
}
