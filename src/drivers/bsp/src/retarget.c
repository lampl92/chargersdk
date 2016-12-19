#include "bsp.h"
#include "xprintf.h"

extern UART_HandleTypeDef CLI_UARTx_Handler;

void myputc(uint8_t ch)
{
    __set_PRIMASK(1);    //增加关闭中断功能,防止串口在使用时出现冲突
    HAL_UART_Transmit(&CLI_UARTx_Handler, (uint8_t *)&ch, 1, 0xFFFF);
    __set_PRIMASK(0);
}
void retarget_init(void)
{
    xdev_out(myputc);
}



#if 0
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
  * @brief  重定向标准库函数printf对应的基础库函数.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /*
    由于Windows中终端换行是"\r\n", 而tinysh中全部用的是'\n'结尾,无法完成换行,
    在此进行兼容
    */

    int tempch = ch;
    __set_PRIMASK(1);    //增加关闭中断功能,防止串口在使用时出现冲突
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

//int ferror(FILE *f)
//{
//    return EOF;
//}

void _ttywrch(int ch)
{
    char tempch = ch;
    HAL_UART_Transmit(&CLI_UARTx_Handler, (uint8_t *)&tempch, 1, 0xFFFF);
}

void _sys_exit(int x)
{
    x = x;
}
#if 0
void  safe_printf(char *format, ...)
{
    char  buf_str[200 + 1];
    va_list   v_args;


    va_start(v_args, format);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) format,
                                  v_args);
    va_end(v_args);

	/* »¥³âÐÅºÅÁ¿ */
	xSemaphoreTake(xMutex, portMAX_DELAY);

    printf("%s", buf_str);

   	xSemaphoreGive(xMutex);
}
#endif
#endif
