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

#include "bsp_uart_queue.h"

UART_HandleTypeDef CLI_UARTx_Handler;
UART_HandleTypeDef RFID_UARTx_Handler;
UART_HandleTypeDef GPRS_UARTx_Handler;
#ifdef EVSE_DEBUG
UART_HandleTypeDef WIFI_UARTx_Handler;
#endif

Queue *pCliRecvQue;
Queue *pRfidRecvQue;
Queue *pGprsRecvQue;
#ifdef EVSE_DEBUG
Queue *pWifiRecvQue;
#endif

static volatile uint8_t CLI_RX_Buffer[1];
static volatile uint8_t RFID_RX_Buffer[1];
static volatile uint8_t GPRS_RX_Buffer[1];
#ifdef EVSE_DEBUG
static volatile uint8_t WIFI_RX_Buffer[1];
#endif

static void uart_putc(uint8_t ch)
{
    while((GPRS_USARTx_BASE->SR&0X40)==0);
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
//        for(i = 0; i < len; i++)
//        {
//            uart_putc(data[i]);
//        }
//        return i;
        break;
#ifdef EVSE_DEBUG
    case UART_PORT_WIFI:
        pUART_Handle = &WIFI_UARTx_Handler;
        break;
#endif
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
        while(1);
    }
}

uint32_t uart_read(UART_Portdef uartport, uint8_t *data, uint32_t len, uint32_t timeout)
{
    Queue *pRecvQue;
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
#ifdef EVSE_DEBUG
    case UART_PORT_WIFI:
        pRecvQue = pWifiRecvQue;
        break;
#endif
    default:
        break;
    }
    readRecvQueEx(pRecvQue, data, len, &rl);
    return rl;
}

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

uint8_t readRecvQueProto(Queue *q, uint8_t *pbuff, uint8_t head, uint8_t end, uint32_t *puiRecvdLen)
{
    uint8_t ch;
    uint32_t i;

    ch = 0;
    i = 0;
    xSemaphoreTake(q->xHandleMutexQue, 100);
    while(q->DeElem(q, &ch) == QUE_OK)
    {
        if(ch == head)
        {
            do
            {
                pbuff[i] = ch;
                i++;
                if(q->DeElem(q, &ch) != QUE_OK)
                {
                    *puiRecvdLen = 0;
                    xSemaphoreGive(q->xHandleMutexQue);
                    return 0;
                }
                if(ch == end && i != 1)
                {
                    pbuff[i] = ch;
                    i++;
                }
            }
            while(ch != end || i == 1);

            *puiRecvdLen = i;
            xSemaphoreGive(q->xHandleMutexQue);
            return 1;
        }
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
uint8_t readRecvQueEx(Queue *q, uint8_t *pbuff, uint32_t ulRecvLen, uint32_t *puiRecvdLen)
{
    uint8_t ch;
    uint32_t i;

    ch = 0;
    i = 0;

    while(readRecvQue(q, &ch, 10) == 1)
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
        *puiRecvdLen = i;
        return 0;
    }
}

uint8_t recvStrCmp(Queue *q, uint8_t *str, uint32_t len)
{
    uint8_t *p;
    p = strstr(q->elem, str);
    if(p != NULL)
    {
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
#ifdef EVSE_DEBUG
    pWifiRecvQue = QueueCreate(WIFI_QUEUE_SIZE);
#endif

    //uart_queue_init();

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

//    WIFI_UARTx_Handler.Instance = WIFI_USARTx_BASE;
//    WIFI_UARTx_Handler.Init.BaudRate = WIFI_USARTx_BAUDRATE;
//    WIFI_UARTx_Handler.Init.WordLength = UART_WORDLENGTH_8B;
//    WIFI_UARTx_Handler.Init.StopBits = UART_STOPBITS_1;
//    WIFI_UARTx_Handler.Init.Parity = UART_PARITY_NONE;
//    WIFI_UARTx_Handler.Init.Mode = UART_MODE_TX_RX;
//    WIFI_UARTx_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//    WIFI_UARTx_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
//    HAL_UART_Init(&WIFI_UARTx_Handler);
//    HAL_UART_Receive_IT(&WIFI_UARTx_Handler, (uint8_t *)WIFI_RX_Buffer, 1);

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
#ifdef EVSE_DEBUG
WIFI_USARTx_IRQHandler
{
    HAL_UART_IRQHandler(&WIFI_UARTx_Handler);
//            if(HAL_UART_Receive_IT(&WIFI_UARTx_Handler, (uint8_t *)WIFI_RX_Buffer, 1) == HAL_OK)
//        {
//            pWifiRecvQue->EnElem(pWifiRecvQue, WIFI_RX_Buffer[0]);
//        }
}
#endif
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
            //gdsl_queue_insert(queCLI, (void *)CLI_RX_Buffer);
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
#ifdef EVSE_DEBUG
    if(huart->Instance == WIFI_USARTx_BASE)
    {
        if(HAL_UART_Receive_IT(&WIFI_UARTx_Handler, (uint8_t *)WIFI_RX_Buffer, 1) == HAL_OK)
        {
            pWifiRecvQue->EnElem(pWifiRecvQue, WIFI_RX_Buffer[0]);
        }
    }
#endif
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
