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
#include "user_app.h"
UART_HandleTypeDef CLI_UARTx_Handler;
UART_HandleTypeDef RFID_UARTx_Handler;
UART_HandleTypeDef GPRS_UARTx_Handler;

Queue *pCliRecvQue;
Queue *pRfidRecvQue;
Queue *pGprsRecvQue;

static volatile uint8_t CLI_RX_Buffer[1];
static volatile uint8_t RFID_RX_Buffer[1];
static volatile uint8_t GPRS_RX_Buffer[1];

uint8_t readRecvQue(Queue *q, uint8_t *ch, uint16_t time_out)
{
    xSemaphoreTake(q->xHandleMutexQue, 100);
    while(time_out)
    {
        if((q->isEmpty(q)) != QUE_TRUE)
        {
            q->DeElem(q, ch);
            xSemaphoreGive(q->xHandleMutexQue);
            return 1;
        }
        time_out--;
    }
    xSemaphoreGive(q->xHandleMutexQue);
    return 0;
}

/** @brief
 *
 * @param pbuff uint8_t*
 * @param ulRecvLen uint32_t 取出长度,0时取出所有数据
 * @param puiRecvdLen uint32_t* 返回长度
 * @return uint8_t 1 有数据; 0 无数据
 *
 */
uint8_t recvReadEx(Queue *q, uint8_t *pbuff, uint32_t ulRecvLen, uint32_t *puiRecvdLen)
{
    uint8_t ch;
    uint32_t i;

    ch = 0;
    i = 0;

    while(readRecvQue(q, &ch, 1) == 1)
    {
        pbuff[i] = ch;
        i++;
        if(ulRecvLen != 0)
        {
            if(i == ulRecvLen)
            {
                break;
            }
        }
    }
    if(i > 0)
    {
        *puiRecvdLen = i;
        return 1;
    }
    else
    {
        return 0;
    }
}
void bsp_Uart_Init(void)
{
    pCliRecvQue = QueueCreate(CLI_QUEUE_SIZE);
    pRfidRecvQue = QueueCreate(RFID_QUEUE_SIZE);
    pGprsRecvQue = QueueCreate(GPRS_QUEUE_SIZE);

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

    GPRS_UARTx_Handler.Instance = GPRS_USARTx_BASE;
    GPRS_UARTx_Handler.Init.BaudRate = GPRS_USARTx_BAUDRATE;
    GPRS_UARTx_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    GPRS_UARTx_Handler.Init.StopBits = UART_STOPBITS_1;
    GPRS_UARTx_Handler.Init.Parity = UART_PARITY_NONE;
    GPRS_UARTx_Handler.Init.Mode = UART_MODE_TX_RX;
    GPRS_UARTx_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    GPRS_UARTx_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&GPRS_UARTx_Handler);
    HAL_UART_Receive_IT(&GPRS_UARTx_Handler, (uint8_t *)GPRS_RX_Buffer, 1);

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
     if(huart->Instance == UART4)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_UART4_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_10;//PIN_TX
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11;//PIN_RX
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(UART4_IRQn);
        HAL_NVIC_SetPriority(UART4_IRQn, bspUART4_PreemptPriority, bspUART4_SubPriority);
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

GPRS_USARTx_IRQHandler
{
    HAL_UART_IRQHandler(&GPRS_UARTx_Handler);
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
        if(HAL_UART_Receive_IT(&CLI_UARTx_Handler, (uint8_t *)CLI_RX_Buffer, 1) == HAL_OK)
        {
            pCliRecvQue->EnElem(pCliRecvQue, CLI_RX_Buffer[0]);
        }
    }
    if(huart->Instance == RFID_USARTx_BASE)
    {
        if(HAL_UART_Receive_IT(&RFID_UARTx_Handler, (uint8_t *)RFID_RX_Buffer, 1) == HAL_OK)
        {
            pRfidRecvQue->EnElem(pRfidRecvQue, RFID_RX_Buffer[0]);
        }
    }
    if(huart->Instance == GPRS_USARTx_BASE)
    {
        if(HAL_UART_Receive_IT(&GPRS_UARTx_Handler, (uint8_t *)GPRS_RX_Buffer, 1) == HAL_OK)
        {
            pGprsRecvQue->EnElem(pGprsRecvQue, GPRS_RX_Buffer[0]);
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
                                                    ThrowErrorCode(defDevID_##DEV, ERR_UART_##ERR, LEVEL, "USART ERR");   \
                                                }                                               \
                                            }
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    THROW_USART_ERR(RFID, PE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(RFID, NE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(RFID, FE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(RFID, ORE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(RFID, DMA, ERR_LEVEL_TIPS);

    THROW_USART_ERR(GPRS, PE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(GPRS, NE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(GPRS, FE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(GPRS, ORE, ERR_LEVEL_TIPS);
    THROW_USART_ERR(GPRS, DMA, ERR_LEVEL_TIPS);
}
