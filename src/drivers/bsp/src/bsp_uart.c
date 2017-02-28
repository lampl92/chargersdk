/**
* @file bsp_usart.c
* @brief ´®¿ÚÇý¶¯
* @author rgw
* @version V1.0
* @date 2016-11-02
*/

#include "bsp.h"
#include "cli_main.h"
#include "userlib_queue.h"

UART_HandleTypeDef CLI_UARTx_Handler;
UART_HandleTypeDef RFID_UARTx_Handler;

volatile uint8_t CLI_RX_Buffer[1];
volatile uint8_t RFID_RX_Buffer[1];

Queue *pCliRecvQue;
Queue *pRfidRecvQue;

int8_t readRecvQue(Queue *q, uint8_t *ch, uint16_t time_out)
{
    xSemaphoreTake(q->xHandleMutexQue, 100);
    while(time_out)
    {
        if((q->isEmpty(q)) != QUE_TRUE)
        {
            q->DeElem(q, ch);
            xSemaphoreGive(q->xHandleMutexQue);
            return 0;
        }
        time_out--;
    }
    xSemaphoreGive(q->xHandleMutexQue);
    return (int8_t) - 1;
}

void bsp_Uart_Init(void)
{
    pCliRecvQue = QueueCreate(CLI_QUEUE_SIZE);
    pRfidRecvQue = QueueCreate(RFID_QUEUE_SIZE);

    CLI_UARTx_Handler.Instance = CLI_USARTx_BASE;
    CLI_UARTx_Handler.Init.BaudRate = CLI_USARTx_BAUDRATE;
    CLI_UARTx_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    CLI_UARTx_Handler.Init.StopBits = UART_STOPBITS_1;
    CLI_UARTx_Handler.Init.Parity = UART_PARITY_NONE;
    CLI_UARTx_Handler.Init.Mode = UART_MODE_TX_RX;
    CLI_UARTx_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    CLI_UARTx_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&CLI_UARTx_Handler);
    HAL_UART_Receive_IT(&CLI_UARTx_Handler, (uint8_t *)CLI_RX_Buffer, 1);

    RFID_UARTx_Handler.Instance = RFID_USARTx_BASE;
    RFID_UARTx_Handler.Init.BaudRate = RFID_USARTx_BAUDRATE;
    RFID_UARTx_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    RFID_UARTx_Handler.Init.StopBits = UART_STOPBITS_1;
    RFID_UARTx_Handler.Init.Parity = UART_PARITY_NONE;
    RFID_UARTx_Handler.Init.Mode = UART_MODE_TX_RX;
    RFID_UARTx_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    RFID_UARTx_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&RFID_UARTx_Handler);
    HAL_UART_Receive_IT(&RFID_UARTx_Handler, (uint8_t *)RFID_RX_Buffer, 1);
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
    if(huart->Instance == USART3)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_USART3_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_10;//PIN_TX
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11;//PIN_RX
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(USART3_IRQn);
        HAL_NVIC_SetPriority(USART3_IRQn, bspUSART3_PreemptPriority, bspUSART3_SubPriority);
    }
}
CLI_USARTx_IRQHandler
{
    UBaseType_t uxSavedInterruptStatus;
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

    HAL_UART_IRQHandler(&CLI_UARTx_Handler);

    taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );
}

RFID_USARTx_IRQHandler
{
    UBaseType_t uxSavedInterruptStatus;
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

    HAL_UART_IRQHandler(&RFID_UARTx_Handler);

    taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );
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
    BaseType_t xHigherPriorityTaskWokenCli = pdFALSE;
    BaseType_t xHigherPriorityTaskWokenRfid = pdFALSE;
    if(huart->Instance == CLI_USARTx_BASE)
    {
        if(HAL_UART_Receive_IT(&CLI_UARTx_Handler, (uint8_t *)CLI_RX_Buffer, 1) == HAL_OK)
        {
            xSemaphoreTakeFromISR(pCliRecvQue->xHandleMutexQue, &xHigherPriorityTaskWokenCli);
            pCliRecvQue->EnElem(pCliRecvQue, CLI_RX_Buffer[0]);
            xSemaphoreGiveFromISR(pCliRecvQue->xHandleMutexQue, &xHigherPriorityTaskWokenCli);
            portYIELD_FROM_ISR( xHigherPriorityTaskWokenCli );
        }
    }
    if(huart->Instance == RFID_USARTx_BASE)
    {
        if(HAL_UART_Receive_IT(&RFID_UARTx_Handler, (uint8_t *)RFID_RX_Buffer, 1) == HAL_OK)
        {
            xSemaphoreTakeFromISR(pRfidRecvQue->xHandleMutexQue, &xHigherPriorityTaskWokenRfid);
            pRfidRecvQue->EnElem(pRfidRecvQue, RFID_RX_Buffer[0]);
            xSemaphoreGiveFromISR(pRfidRecvQue->xHandleMutexQue, &xHigherPriorityTaskWokenRfid);
            portYIELD_FROM_ISR( xHigherPriorityTaskWokenRfid );
        }
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
#define THROW_USART_ERR(DEV, ERR, LEVEL)    if(huart->Instance == DEV##_USARTx_BASE)            \
                                            {                                                   \
                                                if(huart->ErrorCode == HAL_USART_ERROR_##ERR)   \
                                                {                                               \
                                                    ThrowErrorCode(defDevID_RFID, ERR_##DEV##_##ERR, LEVEL);   \
                                                }                                               \
                                            }
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    THROW_USART_ERR(RFID, PE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(RFID, NE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(RFID, FE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(RFID, ORE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(RFID, DMA, ERR_LEVEL_TIPS);
}
