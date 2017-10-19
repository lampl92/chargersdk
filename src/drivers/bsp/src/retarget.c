#include "bsp.h"
#include "xprintf.h"
extern UART_HandleTypeDef CLI_UARTx_Handler;

SemaphoreHandle_t  xprintfMutex = NULL;

void myputc(uint8_t ch)
{
    while((CLI_USARTx_BASE->SR&0X40)==0);
    CLI_USARTx_BASE->DR = ch;
}
#if 1
void retarget_init(void)
{
    xprintfMutex = xSemaphoreCreateMutex();

    if(xprintfMutex == NULL)
    {
        //
    }
    xdev_out(myputc);
}

int printf_safe(const char *format, ...)
{
    char  buf_str[200 + 1];
    va_list   v_args;


    va_start(v_args, format);
    (void)vsnprintf((char *)&buf_str[0],
                    (size_t      ) sizeof(buf_str),
                    (char const *) format,
                    v_args);
    va_end(v_args);

    if (xSemaphoreTake(xprintfMutex, 10) == pdPASS)
    {
        xprintf("%s", buf_str);
        xSemaphoreGive(xprintfMutex);
    }
}


#endif

#if 0
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#pragma import(__use_no_semihosting_swi)

struct __FILE
{
    int handle;
};
FILE __stdout;
FILE __stdin;
FILE __stderr;
#ifdef __GNUC__
int _write (int fd, char *ptr, int len)
{
    int i = 0;
    uint8_t ch;
    while (*ptr && (i < len))
    {
        myputc(*ptr);
        i++;
        ptr++;
    }
    return i;
}
#else
/**
  * @brief  重定向标准库函数printf对应的基础库函数.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
    /*
    由于Windows中终端换行是"\r\n", 而tinysh中全部用的是'\n'结尾,无法完成换行,
    在此进行兼容
    */

    myputc(ch);

    return ch;
}

int fgetc(FILE *f)
{
    int ch;
    HAL_UART_Receive(&CLI_UARTx_Handler, (uint8_t *)&ch, 1, 1000);
    return ch;
}

int ferror(FILE *f)
{
    return EOF;
}
#endif



void _ttywrch(int ch)
{
    char tempch = ch;
    HAL_UART_Transmit(&CLI_UARTx_Handler, (uint8_t *)&tempch, 1, 0xFFFF);
}

void _sys_exit(int x)
{
    x = x;
}

#endif
