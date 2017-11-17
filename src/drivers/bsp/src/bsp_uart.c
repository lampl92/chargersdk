/**
* @file bsp_usart.c
* @brief 串口驱动
* @author rgw
* @version V1.0
* @date 2016-11-02
*/
#include "bsp_define.h"
#include "bsp_uart.h"
#include "cli_main.h"
#include "userlib_queue.h"
#include "user_app.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"

UART_HandleTypeDef CLI_UARTx_Handler;
UART_HandleTypeDef RFID_UARTx_Handler;
UART_HandleTypeDef GPRS_UARTx_Handler;
UART_HandleTypeDef WIFI_UARTx_Handler;

static Queue *pCliRecvQue = NULL;
static Queue *pRfidRecvQue = NULL;
static Queue *pGprsRecvQue = NULL;
static Queue *pWifiRecvQue = NULL;

static volatile uint8_t CLI_RX_Buffer[1];
static volatile uint8_t RFID_RX_Buffer[1];
static volatile uint8_t GPRS_RX_Buffer[1];
static volatile uint8_t WIFI_RX_Buffer[1];

static uint8_t readRecvQue(Queue *q, uint8_t *ch, uint32_t timeout_ms);

void gprs_uart_putc(uint8_t ch)
{
    while ((GPRS_USARTx_BASE->SR & USART_SR_TC) == 0)
        ;
    GPRS_USARTx_BASE->DR = ch;
}

uint32_t uart_write(UART_Portdef uartport, uint8_t *data, uint32_t len)
{
    int i;
    UART_HandleTypeDef *pUART_Handle;
    switch(uartport)
    {
    case UART_PORT_CLI:
        pUART_Handle = &CLI_UARTx_Handler;
        break;
    case UART_PORT_RFID:
        pUART_Handle = &RFID_UARTx_Handler;
        break;
    case UART_PORT_GPRS:
        pUART_Handle = &GPRS_UARTx_Handler;
        for(i = 0; i < len; i++)
        {
            gprs_uart_putc(data[i]);
        }
        return i;
        break;
    case UART_PORT_WIFI:
        pUART_Handle = &WIFI_UARTx_Handler;
        break;
    default:
        break;
    }
    HAL_StatusTypeDef hal_res;
    hal_res = HAL_UART_Transmit(pUART_Handle, data, len, 2000);

    if(hal_res == HAL_OK)
    {
        return len;
    }
    if(hal_res == HAL_BUSY)
    {
        printf_safe("HAL busy!!\n");
    }
    return 0;
}

uint32_t uart_read(UART_Portdef uartport, uint8_t *data, uint32_t len, uint32_t timeout_ms)
{
    Queue *pRecvQue;
    uint8_t ch;
    uint32_t rl = 0;//read len
    switch(uartport)
    {
    case UART_PORT_CLI:
        pRecvQue = pCliRecvQue;
        break;
    case UART_PORT_RFID:
        pRecvQue = pRfidRecvQue;
        break;
    case UART_PORT_GPRS:
        pRecvQue = pGprsRecvQue;
        break;
    case UART_PORT_WIFI:
        pRecvQue = pWifiRecvQue;
        break;
    default:
        break;
    }

    while (readRecvQue(pRecvQue, &ch, timeout_ms) == 1)
    {
        data[rl] = ch;
        rl++;
        if (len != 0)
        {
            if (rl == len)
            {
                break;
            }
        }
    }

    return rl;
}

static uint8_t readRecvQue(Queue *q, uint8_t *ch, uint32_t timeout_ms)
{
    if (timeout_ms == 0)
    {
        timeout_ms = 1;
    }
    while (timeout_ms)
    {
#if USE_FreeRTOS
        if (xSemaphoreTake(q->xHandleMutexQue, 0) == pdPASS)
        {
#endif
            if ((q->isEmpty(q)) == QUE_FALSE)
            {
                q->DeElem(q, ch);
#if USE_FreeRTOS
                xSemaphoreGive(q->xHandleMutexQue);
#endif
                return 1;
            }
            else
            {
#if USE_FreeRTOS
                xSemaphoreGive(q->xHandleMutexQue);
#endif
                vTaskDelay(1);
                timeout_ms--;
            }
#if USE_FreeRTOS
        }
#endif
    }
    
    return 0;
}

//mode 1 创建并初始化， 2，不创建，初始化
void bsp_Uart_Init(UART_Portdef uartport, uint8_t mode)
{
    switch(uartport)
    {
    case UART_PORT_CLI:
        if(mode == 1)
        {
            pCliRecvQue = QueueCreate(CLI_QUEUE_SIZE);
        }
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
        break;
    case UART_PORT_RFID:
        if(mode == 1)
        {
            pRfidRecvQue = QueueCreate(RFID_QUEUE_SIZE);
        }
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
        break;
    case UART_PORT_GPRS:
        if(mode == 1)
        {
            pGprsRecvQue = QueueCreate(GPRS_QUEUE_SIZE);
        }
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
        break;
//    case UART_PORT_WIFI:
//        if(mode == 1)
//        {
//            pWifiRecvQue = QueueCreate(WIFI_QUEUE_SIZE);
//        }
//        WIFI_UARTx_Handler.Instance = WIFI_USARTx_BASE;
//        WIFI_UARTx_Handler.Init.BaudRate = WIFI_USARTx_BAUDRATE;
//        WIFI_UARTx_Handler.Init.WordLength = UART_WORDLENGTH_8B;
//        WIFI_UARTx_Handler.Init.StopBits = UART_STOPBITS_1;
//        WIFI_UARTx_Handler.Init.Parity = UART_PARITY_NONE;
//        WIFI_UARTx_Handler.Init.Mode = UART_MODE_TX_RX;
//        WIFI_UARTx_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//        WIFI_UARTx_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
//        HAL_UART_Init(&WIFI_UARTx_Handler);
//        HAL_UART_Receive_IT(&WIFI_UARTx_Handler, (uint8_t *)WIFI_RX_Buffer, 1);
//        break;
    default:
        break;
    }

    //uart_queue_init();

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
        GPIO_InitStruct.Pull = GPIO_NOPULL;
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
    if(huart->Instance == UART5)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_UART5_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_12;//PIN_TX
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_2;//PIN_RX
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(UART5_IRQn);
        HAL_NVIC_SetPriority(UART5_IRQn, bspUART5_PreemptPriority, bspUART5_SubPriority);
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

//WIFI_USARTx_IRQHandler
//{
//    HAL_UART_IRQHandler(&WIFI_UARTx_Handler);
//}
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
//    if(huart->Instance == WIFI_USARTx_BASE)
//    {
//        if(HAL_UART_Receive_IT(&WIFI_UARTx_Handler, (uint8_t *)WIFI_RX_Buffer, 1) == HAL_OK)
//        {
//            pWifiRecvQue->EnElem(pWifiRecvQue, WIFI_RX_Buffer[0]);
//        }
//    }
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
//    THROW_USART_ERR(RFID, PE, ERR_LEVEL_TIPS);
//    THROW_USART_ERR(RFID, NE, ERR_LEVEL_TIPS);
//    THROW_USART_ERR(RFID, FE, ERR_LEVEL_TIPS);
//    THROW_USART_ERR(RFID, ORE, ERR_LEVEL_TIPS);
//    THROW_USART_ERR(RFID, DMA, ERR_LEVEL_TIPS);
//
//    THROW_USART_ERR(GPRS, PE, ERR_LEVEL_TIPS);
//    THROW_USART_ERR(GPRS, NE, ERR_LEVEL_TIPS);
//    THROW_USART_ERR(GPRS, FE, ERR_LEVEL_TIPS);
//    THROW_USART_ERR(GPRS, ORE, ERR_LEVEL_TIPS);
//    THROW_USART_ERR(GPRS, DMA, ERR_LEVEL_TIPS);

}
