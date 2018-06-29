/**
* @file bsp_usart.c
* @brief 串口驱动
* @author rgw
* @version V1.0
* @date 2016-11-02
*/
#include "bsp_uart.h"
#include "bsp_define.h"
#include "cmsis_os.h"
#include "utils.h"
#include "ring_buffer.h"
#include <string.h>

#define UART_MAX 7

uart_driver_s uart_driver[UART_MAX];

uart_driver_s *uart_get_driver_des(int handle)
{
    if (handle >= 0 && handle < UART_MAX)
    {
        if (uart_driver[handle].is_initialized == 1)
        {
            return &uart_driver[handle];
        }
    }
    return NULL;
}


void uart_driver_init(void)
{
    int i;
    for (i = 0; i < UART_MAX; i++)
    {
        uart_driver[i].handle = -1;
        uart_driver[i].rb = NULL;
    }
}

int uart_open(char *path, uint32_t band, int data_bit, char parity, int stop_bit)
{
    struct _uart_driver *driver;
    int handle = UART_PATH_ERR;
    if (strcasecmp(path, "USART1") == 0)
    {
        driver = &uart_driver[0];
        if (driver->is_initialized == 0)
        {
            driver->handle = 0;
            driver->rb = ring_uint8_init(USART1_RB_SIZE);
            if (driver->rb == NULL)
                return UART_RB_INIT_FAIL;
            osMutexDef(uart1_lock);
            driver->lock = osMutexCreate(osMutex(uart1_lock));
            driver->UARTx_Handler.Instance = USART1;
        }
        else
        {
            return UART_IS_INITIALIZED;
        }
    }
    else if (strcasecmp(path, "USART2") == 0)
    {
        driver = &uart_driver[1];
        if (driver->is_initialized == 0)
        {
            driver->handle = 1;
            driver->rb = ring_uint8_init(USART2_RB_SIZE);
            if (driver->rb == NULL)
                return UART_RB_INIT_FAIL;
            osMutexDef(uart2_lock);
            driver->lock = osMutexCreate(osMutex(uart2_lock));
            driver->UARTx_Handler.Instance = USART2;
        }
        else
        {
            return UART_IS_INITIALIZED;
        }
    }
    else if (strcasecmp(path, "USART3") == 0)
    {
        driver = &uart_driver[2];
        if (driver->is_initialized == 0)
        {
            driver->handle = 2;
            driver->rb = ring_uint8_init(USART3_RB_SIZE);
            if (driver->rb == NULL)
                return UART_RB_INIT_FAIL;
            osMutexDef(uart3_lock);
            driver->lock = osMutexCreate(osMutex(uart3_lock));
            driver->UARTx_Handler.Instance = USART3;
        }
        else
        {
            return UART_IS_INITIALIZED;
        }
    }
    else if (strcasecmp(path, "UART4") == 0)
    {
        driver = &uart_driver[3];
        if (driver->is_initialized == 0)
        {
            driver->handle = 3;
            driver->rb = ring_uint8_init(UART4_RB_SIZE);
            if (driver->rb == NULL)
                return UART_RB_INIT_FAIL;
            osMutexDef(uart4_lock);
            driver->lock = osMutexCreate(osMutex(uart4_lock));
            driver->UARTx_Handler.Instance = UART4;
        }
        else
        {
            return UART_IS_INITIALIZED;
        }
    }
    else if (strcasecmp(path, "UART5") == 0)
    {
        driver = &uart_driver[4];
        if (driver->is_initialized == 0)
        {
            driver->handle = 4;
            driver->rb = ring_uint8_init(UART5_RB_SIZE);
            if (driver->rb == NULL)
                return UART_RB_INIT_FAIL;
            osMutexDef(uart5_lock);
            driver->lock = osMutexCreate(osMutex(uart5_lock));
            driver->UARTx_Handler.Instance = UART5;
        }
        else
        {
            return UART_IS_INITIALIZED;
        }
    }
    else if (strcasecmp(path, "USART6") == 0)
    {
        driver = &uart_driver[5];
        if (driver->is_initialized == 0)
        {
            driver->handle = 5;
            driver->rb = ring_uint8_init(USART6_RB_SIZE);
            if (driver->rb == NULL)
                return UART_RB_INIT_FAIL;
            osMutexDef(uart6_lock);
            driver->lock = osMutexCreate(osMutex(uart6_lock));
            driver->UARTx_Handler.Instance = USART6;
        }
        else
        {
            return UART_IS_INITIALIZED;
        }
    }
    else if (strcasecmp(path, "UART7") == 0)
    {
        driver = &uart_driver[6];
        if (driver->is_initialized == 0)
        {
            driver->handle = 6;
            driver->rb = ring_uint8_init(UART7_RB_SIZE);
            if (driver->rb == NULL)
                return UART_RB_INIT_FAIL;
            osMutexDef(uart7_lock);
            driver->lock = osMutexCreate(osMutex(uart7_lock));
            driver->UARTx_Handler.Instance = UART7;
        }
        else
        {
            return UART_IS_INITIALIZED;
        }
    }
    else
    {
        return UART_PATH_ERR;
    }
    driver->UARTx_Handler.Init.BaudRate = band;
    switch (data_bit)
    {
    case 9:
        driver->UARTx_Handler.Init.WordLength = UART_WORDLENGTH_9B;
        break;
    case 8:
    default:
        driver->UARTx_Handler.Init.WordLength = UART_WORDLENGTH_8B;
        break;
    }
    switch (stop_bit)
    {
    case 2:
        driver->UARTx_Handler.Init.StopBits = UART_STOPBITS_2;
        break;
    case 1:
    default:
        driver->UARTx_Handler.Init.StopBits = UART_STOPBITS_1;
        break;
    }
    switch (parity)
    {
    case 'e':
    case 'E':
        driver->UARTx_Handler.Init.Parity = UART_PARITY_EVEN;
        break;
    case 'o':
    case 'O':
        driver->UARTx_Handler.Init.Parity = UART_PARITY_ODD;
        break;
    case 'n':
    case 'N':
    default:
        driver->UARTx_Handler.Init.Parity = UART_PARITY_NONE;
        break;
    }
    driver->UARTx_Handler.Init.Mode = UART_MODE_TX_RX;
    driver->UARTx_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    driver->UARTx_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&(driver->UARTx_Handler));
    HAL_UART_Receive_IT(&(driver->UARTx_Handler), (uint8_t *)(driver->rbuff), 1);

    driver->is_initialized = 1;

    return driver->handle;
}

int uart_close(int handle)
{
    int i;
    if (handle < 0 || handle >= UART_MAX)
    {
        return -1;
    }
    uart_driver[handle].handle = -1;
    uart_driver[handle].is_initialized = 0;
    ring_buffer_free(uart_driver[handle].rb);
    osMutexDelete(uart_driver[handle].lock);
    HAL_UART_DeInit(&(uart_driver[handle].UARTx_Handler));

    return 0;
}

uint32_t uart_read_fast(int handle, uint8_t *data, uint32_t len)
{
    ring_buffer_s *rb = uart_driver[handle].rb;
    uint32_t l = ring_buffer_len(rb);
    uint32_t rl = 0;
    if (l > 0)
    {
        len = utils_min(len, l);
        rl = ring_buffer_get(rb, data, len);
    }
    return rl;
}

uint32_t uart_read_wait(int handle, uint8_t *data, uint32_t len, uint32_t timeout_ms)
{
    uint32_t rl = 0;
    uint32_t sum = 0;

    do
    {
        if (len - sum == 0)
            break;
        rl = uart_read_fast(handle, data + sum, len - sum);
        sum += rl;
        if (timeout_ms == 0)
            break;
        osDelay(1);
    } while (timeout_ms--);

    return sum;
}

void __uart_putc(USART_TypeDef *USARTx_BASE, uint8_t ch)
{
    while ((USARTx_BASE->SR & USART_SR_TC) == 0)
        ;
    while ((USARTx_BASE->SR & USART_SR_TXE) == 0) 
        ;
    USARTx_BASE->DR = ch;
}

uint32_t uart_write_fast(int handle, const uint8_t *data, uint32_t len)
{
    uint32_t l = len;
    osMutexWait(uart_driver[handle].lock, osWaitForever);
#if 1
    while (l > 0)
    {
        l--;
        __uart_putc(uart_driver[handle].UARTx_Handler.Instance, *data);
        data++;
    }
#else
    HAL_UART_Transmit(&uart_driver[handle].UARTx_Handler, (uint8_t *)data, len, 100);
#endif
    osMutexRelease(uart_driver[handle].lock);

    return len;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (huart->Instance == USART1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_9; //PIN_TX
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10; //PIN_RX
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(USART1_IRQn);
        HAL_NVIC_SetPriority(USART1_IRQn, bspUSART1_PreemptPriority, bspUSART1_SubPriority);
    }

    if (huart->Instance == USART2)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART2_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_2; //PIN_TX
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3; //PIN_RX
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(USART2_IRQn);
        HAL_NVIC_SetPriority(USART2_IRQn, bspUSART2_PreemptPriority, bspUSART2_SubPriority);
    }
    if (huart->Instance == USART3)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_USART3_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_10; //PIN_TX
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11; //PIN_RX
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(USART3_IRQn);
        HAL_NVIC_SetPriority(USART3_IRQn, bspUSART3_PreemptPriority, bspUSART3_SubPriority);
    }
    if (huart->Instance == UART4)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_UART4_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_10; //PIN_TX
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11; //PIN_RX
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(UART4_IRQn);
        HAL_NVIC_SetPriority(UART4_IRQn, bspUART4_PreemptPriority, bspUART4_SubPriority);
    }
    if (huart->Instance == UART5)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_UART5_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_12; //PIN_TX
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_2; //PIN_RX
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(UART5_IRQn);
        HAL_NVIC_SetPriority(UART5_IRQn, bspUART5_PreemptPriority, bspUART5_SubPriority);
    }
    if (huart->Instance == USART6)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_USART6_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_6; //PIN_TX
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_7; //PIN_RX
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(USART6_IRQn);
        HAL_NVIC_SetPriority(USART6_IRQn, bspUSART6_PreemptPriority, bspUSART6_SubPriority);
    }
    if (huart->Instance == UART7)
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_UART7_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_7; //PIN_TX
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART7;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_6; //PIN_RX
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(UART7_IRQn);
        HAL_NVIC_SetPriority(UART7_IRQn, bspUART7_PreemptPriority, bspUART7_SubPriority);
    }
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&(uart_driver[0].UARTx_Handler));
}
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&(uart_driver[1].UARTx_Handler));
}
void USART3_IRQHandler(void)
{
    HAL_UART_IRQHandler(&(uart_driver[2].UARTx_Handler));
}
void UART4_IRQHandler(void)
{
    HAL_UART_IRQHandler(&(uart_driver[3].UARTx_Handler));
}
void UART5_IRQHandler(void)
{
    HAL_UART_IRQHandler(&(uart_driver[4].UARTx_Handler));
}
void USART6_IRQHandler(void)
{
    HAL_UART_IRQHandler(&(uart_driver[5].UARTx_Handler));
}
void UART7_IRQHandler(void)
{
    HAL_UART_IRQHandler(&(uart_driver[6].UARTx_Handler));
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
    int i;
    for (i = 0; i < UART_MAX; i++)
    {
        if (uart_driver[i].UARTx_Handler.Instance == huart->Instance)
        {
            if (HAL_UART_Receive_IT(&uart_driver[i].UARTx_Handler, (uint8_t *)uart_driver[i].rbuff, 1) == HAL_OK)
            {
                ring_buffer_put(uart_driver[i].rb, (uint8_t *)uart_driver[i].rbuff, 1);
            }
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
#define THROW_USART_ERR(DEV, ERR, LEVEL)                                        \
    if (huart->Instance == DEV##_USARTx_BASE)                                   \
    {                                                                           \
        if (huart->ErrorCode == HAL_USART_ERROR_##ERR)                          \
        {                                                                       \
            ThrowErrorCode(defDevID_##DEV, ERR_UART_##ERR, LEVEL, "USART ERR"); \
        }                                                                       \
    }
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    int i;
    for (i = 0; i < UART_MAX; i++)
    {
        if (uart_driver[i].UARTx_Handler.Instance == huart->Instance)
        {
            if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) != RESET) 
            {
                __HAL_UART_CLEAR_OREFLAG(huart);
            }
            if (__HAL_UART_GET_FLAG(huart, UART_FLAG_PE) != RESET) 
            {
                __HAL_UART_CLEAR_PEFLAG(huart);
            }
            if (__HAL_UART_GET_FLAG(huart, UART_FLAG_NE) != RESET) 
            {
                __HAL_UART_CLEAR_NEFLAG(huart);
            }
            if (__HAL_UART_GET_FLAG(huart, UART_FLAG_FE) != RESET) 
            {
                __HAL_UART_CLEAR_FEFLAG(huart);
            }
            else
            {
                HAL_UART_DeInit(huart);
                HAL_UART_Init(huart);
            }
            HAL_UART_Receive_IT(huart, (uint8_t *)uart_driver[i].rbuff, 1);
        }
    }

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
