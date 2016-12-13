/**
* @file bsp_usart.c
* @brief 
* @author rgw
* @version V1.0
* @date 2016-11-02
*/

#include "bsp.h"
#include "cli_main.h"




UART_HandleTypeDef UART1_Handler;
UART_HandleTypeDef UART2_Handler;

uint8_t aCliRxBuffer[cliRXBUFFERSIZE];
uint16_t CLI_RX_STA = RESET;



void bsp_Uart_Init(void)
{

    UART1_Handler.Instance = USART1;
    UART1_Handler.Init.BaudRate = 115200;
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    UART1_Handler.Init.StopBits = UART_STOPBITS_1;
    UART1_Handler.Init.Parity = UART_PARITY_NONE;
    UART1_Handler.Init.Mode = UART_MODE_TX_RX;
    UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART1_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&UART1_Handler);


    UART2_Handler.Instance = USART2;
    UART2_Handler.Init.BaudRate = 115200;
    UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    UART2_Handler.Init.StopBits = UART_STOPBITS_1;
    UART2_Handler.Init.Parity = UART_PARITY_NONE;
    UART2_Handler.Init.Mode = UART_MODE_TX_RX;
    UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART2_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&UART2_Handler);

    HAL_UART_Receive_IT(&CLI_UARTx_Handler, (uint8_t *)aCliRxBuffer, cliRXBUFFERSIZE);


}


void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if(huart->Instance == USART1)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART1_CLK_ENABLE();
	
		GPIO_InitStruct.Pin = GPIO_PIN_9;//PIN_TX
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       ///
		GPIO_InitStruct.Pull = GPIO_PULLUP;	        
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;  ///
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10;//PIN_RX
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		HAL_NVIC_SetPriority(USART1_IRQn, bspUSART1_PreemptPriority, bspUSART1_SubPriority);
	}

    if(huart->Instance == USART2)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART2_CLK_ENABLE();
	
		GPIO_InitStruct.Pin = GPIO_PIN_2;//PIN_TX
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       ///
		GPIO_InitStruct.Pull = GPIO_PULLUP;	        
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;  ///
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_3;//PIN_RX
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		HAL_NVIC_EnableIRQ(USART2_IRQn);
		HAL_NVIC_SetPriority(USART2_IRQn, bspUSART2_PreemptPriority, bspUSART2_SubPriority);
    }


}


CLI_USARTx_IRQHandler_PROTOTYPE
{
    uint32_t timeout;
    #ifdef USE_FreeRTOS
    UBaseType_t uxSavedInterruptStatus;
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	{
    #endif
        HAL_UART_IRQHandler(&CLI_UARTx_Handler);

        timeout=0;
        while (HAL_UART_GetState(&CLI_UARTx_Handler) != HAL_UART_STATE_READY)
        {
            timeout++;
            if(timeout > HAL_MAX_DELAY)
            {
                break;
            }
        }
        timeout=0;
        while(HAL_UART_Receive_IT(&CLI_UARTx_Handler, (uint8_t *)aCliRxBuffer, cliRXBUFFERSIZE) != HAL_OK)
        {
            timeout++; 
            if(timeout > HAL_MAX_DELAY) 
            {
                break; 
            }
        }

    #ifdef USE_FreeRTOS
    }
    taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus ); 
    #endif
}


/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handler. 
  * @note   This example shows a simple way to report end of IT Tx transfer, 
and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handler
  * @note   This example shows a simple way to report end of IT Rx transfer, 
and 
  *         you can add your own implementation.
  * @retval None
  */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == CLI_USARTx_BASE)
    {
	    CLI_RX_STA = SET;
    }
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handler
  * @note   This example shows a simple way to report transfer error, and you 
can
  *         add your own implementation.
  * @retval None
  */
 void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
   
}


#if 1

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


#endif


