/**
* @file bsp_usart.c
* @brief
* @author rgw
* @version V1.0
* @date 2016-11-02
*/

#include "bsp.h"
#include "cli_main.h"

UART_HandleTypeDef CLI_UARTx_Handler;
UART_HandleTypeDef RFID_UARTx_Handler;

uint8_t CLI_RX_Buffer[CLI_BUFFER_SIZE];
uint8_t RFID_RX_Buffer[RFID_BUFFER_SIZE];

uint16_t CLI_RX_STA = RESET;

uart_recv_t cli_recv;
uart_recv_t rfid_recv;

int8_t cli_recv_read(uint8_t *recv, uint16_t time_out)
{
    while(time_out)
    {
        if(cli_recv.front != cli_recv.rear)
        {
            *recv=*cli_recv.front;

            cli_recv.front++;

            if (cli_recv.front >= (CLI_RX_Buffer+CLI_BUFFER_SIZE))
                cli_recv.front = CLI_RX_Buffer;

            return 0;
        }
        time_out--;
    }
    return (int8_t)-1;
}

void bsp_Uart_Init(void)
{
    RFID_UARTx_Handler.Instance = RFID_USARTx_BASE;
    RFID_UARTx_Handler.Init.BaudRate = RFID_USARTx_BAUDRATE;
    RFID_UARTx_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    RFID_UARTx_Handler.Init.StopBits = UART_STOPBITS_1;
    RFID_UARTx_Handler.Init.Parity = UART_PARITY_NONE;
    RFID_UARTx_Handler.Init.Mode = UART_MODE_TX_RX;
    RFID_UARTx_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    RFID_UARTx_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&RFID_UARTx_Handler);
    HAL_UART_Receive_IT(&RFID_UARTx_Handler, RFID_RX_Buffer, 1);
    rfid_recv.front = RFID_RX_Buffer;
    rfid_recv.rear = RFID_RX_Buffer;

    CLI_UARTx_Handler.Instance = CLI_USARTx_BASE;
    CLI_UARTx_Handler.Init.BaudRate = CLI_USARTx_BAUDRATE;
    CLI_UARTx_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    CLI_UARTx_Handler.Init.StopBits = UART_STOPBITS_1;
    CLI_UARTx_Handler.Init.Parity = UART_PARITY_NONE;
    CLI_UARTx_Handler.Init.Mode = UART_MODE_TX_RX;
    CLI_UARTx_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    CLI_UARTx_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&CLI_UARTx_Handler);
    HAL_UART_Receive_IT(&CLI_UARTx_Handler, CLI_RX_Buffer, 1);
    cli_recv.front = CLI_RX_Buffer;
    cli_recv.rear = CLI_RX_Buffer;
}


void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if(huart->Instance == USART1)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART1_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_9;//PIN_TX
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
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
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_3;//PIN_RX
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		HAL_NVIC_EnableIRQ(USART2_IRQn);
		HAL_NVIC_SetPriority(USART2_IRQn, bspUSART2_PreemptPriority, bspUSART2_SubPriority);
    }


}
CLI_USARTx_IRQHandler
{
    HAL_UART_IRQHandler(&CLI_UARTx_Handler);
}

RFID_USARTx_IRQHandler
{
    HAL_UART_IRQHandler(&RFID_UARTx_Handler);
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
    uint32_t cli_timeout,rfid_timeout;
    if(huart->Instance == CLI_USARTx_BASE)
    {
        cli_timeout = 0;
	    while(HAL_UART_Receive_IT(&CLI_UARTx_Handler, cli_recv.rear, 1) != HAL_OK)
        {
            cli_timeout++;
            if(cli_timeout > HAL_MAX_DELAY)
            {
                break;
            }
        }
        cli_recv.rear++;
        if(cli_recv.rear >= (CLI_RX_Buffer + CLI_BUFFER_SIZE))
           cli_recv.rear = CLI_RX_Buffer;
    }
    if(huart->Instance == RFID_USARTx_BASE)
    {
        rfid_timeout = 0;
        while(HAL_UART_Receive_IT(&RFID_UARTx_Handler, rfid_recv.rear, 1) != HAL_OK)
        {
            rfid_timeout++;
            if(rfid_timeout > HAL_MAX_DELAY)
            {
                break;
            }
        }
        rfid_recv.rear++;
        if(rfid_recv.rear >= (RFID_RX_Buffer + RFID_BUFFER_SIZE))
           rfid_recv.rear = RFID_RX_Buffer;
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
