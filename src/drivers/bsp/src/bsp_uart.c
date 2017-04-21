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
UART_HandleTypeDef UART7_Handler;
UART_HandleTypeDef UART7_RS485Handler;
typedef union
{
    float yy_test_float;
    uint8_t an[4];
} a1;
a1 A1;
u8 RS485_TX_BUF[10];
u8 RS485_RX_BUF[64];
u8 RS485_RX_MODBUS[64];
u8 RS485_RX_MODBUS_CNT;
u8 RS485_RX_CNT=0;
uint8_t flag_rs485_send=0;
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


/************************电表RS485****************/
void RS485_Init(u32 bound)
{
    //GPIO¶Ë¿ÚÉèÖÃ
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOF_CLK_ENABLE();			//Ê¹ÄÜGPIOAÊ±ÖÓ
	__HAL_RCC_UART7_CLK_ENABLE();			//Ê¹ÄÜUSART2Ê±ÖÓ

	GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7; //PA2,3
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//¸´ÓÃÍÆÍìÊä³ö
	GPIO_Initure.Pull=GPIO_PULLUP;			//ÉÏÀ­
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//¸ßËÙ
	GPIO_Initure.Alternate=GPIO_AF8_UART7;	//¸´ÓÃÎªUSART2
	HAL_GPIO_Init(GPIOF,&GPIO_Initure);	   	//³õÊ¼»¯PA2,3

    //USART ³õÊ¼»¯ÉèÖÃ
	UART7_RS485Handler.Instance=UART7;			        //USART2
	UART7_RS485Handler.Init.BaudRate=bound;		        //²¨ÌØÂÊ
	UART7_RS485Handler.Init.WordLength=UART_WORDLENGTH_8B;	//×Ö³¤Îª8Î»Êý¾Ý¸ñÊ½
	UART7_RS485Handler.Init.StopBits=UART_STOPBITS_1;		//Ò»¸öÍ£Ö¹Î»
	UART7_RS485Handler.Init.Parity=UART_PARITY_NONE;		//ÎÞÆæÅ¼Ð£ÑéÎ»
	UART7_RS485Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//ÎÞÓ²¼þÁ÷¿Ø
	UART7_RS485Handler.Init.Mode=UART_MODE_TX_RX;		    //ÊÕ·¢Ä£Ê½
	HAL_UART_Init(&UART7_RS485Handler);			        //HAL_UART_Init()»áÊ¹ÄÜUSART2

    __HAL_UART_DISABLE_IT(&UART7_RS485Handler,UART_IT_TC);
	__HAL_UART_ENABLE_IT(&UART7_RS485Handler,UART_IT_RXNE);//¿ªÆô½ÓÊÕÖÐ¶Ï
	HAL_NVIC_EnableIRQ(UART7_IRQn);				        //Ê¹ÄÜUSART1ÖÐ¶Ï
	HAL_NVIC_SetPriority(UART7_IRQn,0,0);			        //ÇÀÕ¼ÓÅÏÈ¼¶3£¬×ÓÓÅÏÈ¼¶3
    RS485_DIS;
   // RS485_EN;                                //ÉèÖÃÎª½ÓÊÕÄ£Ê½
}

//UARTµ×²ã³õÊ¼»¯£¬Ê±ÖÓÊ¹ÄÜ£¬Òý½ÅÅäÖÃ£¬ÖÐ¶ÏÅäÖÃ
//´Ëº¯Êý»á±»HAL_UART_Init()µ÷ÓÃ
//huart:´®¿Ú¾ä±ú


void UART7_IRQHandler(void)
{
    u8 res;
     if (__HAL_UART_GET_FLAG(&UART7_RS485Handler, UART_FLAG_RXNE) != RESET)
	{
	     HAL_UART_Receive(&UART7_RS485Handler,&res,1,1000);
		  if(RS485_RX_CNT<64)
		  {
		   //	RS485_RX_BUF[RS485_RX_CNT]=res;		//¼ÇÂ¼½ÓÊÕµ½µÄÖµ
		   	RS485_RX_MODBUS[RS485_RX_CNT]=res;		//¼ÇÂ¼½ÓÊÕµ½µÄÖµ
			  RS485_RX_CNT++;						//½ÓÊÕÊý¾ÝÔö¼Ó1
		  }
	}
}
//RS485·¢ËÍlen¸ö×Ö½Ú.
//buf:·¢ËÍÇøÊ×µØÖ·
//len:·¢ËÍµÄ×Ö½ÚÊý(ÎªÁËºÍ±¾´úÂëµÄ½ÓÊÕÆ¥Åä,ÕâÀï½¨Òé²»Òª³¬¹ý64¸ö×Ö½Ú)
void RS485_Send_Data(u8 *buf,u8 len)

{RUN_ON;
	RS485_EN;			//ÉèÖÃÎª·¢ËÍÄ£Ê½
	HAL_UART_Transmit(&UART7_RS485Handler,buf,len,1000);//´®¿Ú2·¢ËÍÊý¾Ý
	RS485_DIS;
    RS485_RX_CNT=0;
    RUN_OFF;
}
//RS485²éÑ¯½ÓÊÕµ½µÄÊý¾Ý
//buf:½ÓÊÕ»º´æÊ×µØÖ·
//len:¶Áµ½µÄÊý¾Ý³¤¶È
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//Ä¬ÈÏÎª0
	delay_ms(10);		//µÈ´ý10ms,Á¬Ðø³¬¹ý10msÃ»ÓÐ½ÓÊÕµ½Ò»¸öÊý¾Ý,ÔòÈÏÎª½ÓÊÕ½áÊø
	if(rxlen==RS485_RX_CNT&&rxlen)//½ÓÊÕµ½ÁËÊý¾Ý,ÇÒ½ÓÊÕÍê³ÉÁË
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];
		}
		*len=RS485_RX_CNT;	//¼ÇÂ¼±¾´ÎÊý¾Ý³¤¶È
		RS485_RX_CNT=0;		//ÇåÁã
	}
}
void rs_485_analysis(void)
{
    if(RS485_RX_MODBUS[2]==0X03)
    {
        if(Electricity_meter[0].flag.flag_va==1)
        {

        }
    }
}

uint16_t c_crc(uint8_t *pushdata,uint8_t length)
{

		uint16_t reg_crc=0xffff;
		uint16_t crc_rtn;
		uint8_t i,j;
		for(i=0;i<length;i++)
		{
				reg_crc^=*pushdata++;
				for(j = 0;j<8;j++)
				{
					if(reg_crc & 0x0001)
					{
						reg_crc = reg_crc >> 1^0xA001;
					}
					else
					{
						reg_crc = reg_crc >> 1;
					}
				}

		}

		crc_rtn = reg_crc;

		reg_crc = (crc_rtn>>8)|(crc_rtn << 8);

		return(reg_crc);
}
/***********************************************************/
/** @brief 读取电表相关信息
 * @param pvCON void*
 * @param  add uint8_t   要操作的电表的地址信息，01~255；
 * @param  cmd uint8_t   RS485/MODBUS-RTU通信协议命令如read write
 * @param  massage       获取相关电能信息voltage、current、power、electric_energy、frequency
 */
void Get_electricity_meter_massage(uint8_t add,uint8_t cmd,uint16_t massage,uint16_t num)
{
    uint16_t VREF_CRC;
    if(massage==voltage)
    {
     Electricity_meter[add].flag.flag_va=1;
    }
    if(massage==current)
    {
     Electricity_meter[add].flag.flag_ia=1;
    }
      if(massage==power)
    {
     Electricity_meter[add].flag.flag_power=1;
    }
    if(massage==electric_energy)
    {
     Electricity_meter[add].flag.flag_electric_energy=1;
    }
    if(massage==frequency)
    {
     Electricity_meter[add].flag.flag_frequency=1;
    }
    RS485_TX_BUF[0]=add;
    RS485_TX_BUF[1]=cmd;
    RS485_TX_BUF[2]=(massage>>8)&0x00ff;
    RS485_TX_BUF[3]=massage&0x00ff;
    RS485_TX_BUF[4]=(num>>8)&0x00ff;
    RS485_TX_BUF[5]=num&0x00ff;
    VREF_CRC=c_crc(&RS485_TX_BUF,6);
    RS485_TX_BUF[6]=(VREF_CRC>>8)&0Xff;
    RS485_TX_BUF[7]=VREF_CRC&0Xff;
    RS485_Send_Data(&RS485_TX_BUF,8);

}
void electricity_meter_analysis(void)
{
    uint16_t crc_vref;
     crc_vref=c_crc(&RS485_RX_MODBUS[0],5);
    if((RS485_RX_MODBUS[5]!= crc_vref/256)||(RS485_RX_MODBUS[6]!= crc_vref%256))
    {
        return;
    }
    if(RS485_RX_MODBUS[2]!=0X02)
    {
        return;
    }
    if(RS485_RX_MODBUS[1]==0X03)
    {
        if(Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_va==1)
        {
            //Electricity_meter[RS485_RX_MODBUS[0+1]].massage.massage_va=RS485_RX_MODBUS[5]<<8||RS485_RX_MODBUS[4];
            A1.an[3]=RS485_RX_MODBUS[3];
            A1.an[2]=RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_va=A1.yy_test_float;
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_va=0;
        }
        if(Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_ia==1)
        {
            A1.an[3]=RS485_RX_MODBUS[3];
            A1.an[2]=RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_ia=A1.yy_test_float;
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_ia=0;
        }
        if(Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_power==1)
        {
            A1.an[3]=RS485_RX_MODBUS[3];
            A1.an[2]=RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_power=A1.yy_test_float;
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_power=0;
        }
        if(Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_electric_energy==1)
        {
            A1.an[3]=RS485_RX_MODBUS[3];
            A1.an[2]=RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_electric_energy=A1.yy_test_float;
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_electric_energy=0;
        }
        if(Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_frequency==1)
        {
            A1.an[3]=RS485_RX_MODBUS[3];
            A1.an[2]=RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_frequency=A1.yy_test_float;
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_frequency=0;
        }

    }
}

/************************电表RS485END****************/
CLI_USARTx_IRQHandler
{
    //UBaseType_t uxSavedInterruptStatus;
    //uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

    HAL_UART_IRQHandler(&CLI_UARTx_Handler);

    //taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );
}

RFID_USARTx_IRQHandler
{
    //UBaseType_t uxSavedInterruptStatus;
    //uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

    HAL_UART_IRQHandler(&RFID_UARTx_Handler);

    //taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );
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
    //BaseType_t xHigherPriorityTaskWokenCli = pdFALSE;
    //BaseType_t xHigherPriorityTaskWokenRfid = pdFALSE;
    if(huart->Instance == CLI_USARTx_BASE)
    {
        if(HAL_UART_Receive_IT(&CLI_UARTx_Handler, (uint8_t *)CLI_RX_Buffer, 1) == HAL_OK)
        {
            //xSemaphoreTakeFromISR(pCliRecvQue->xHandleMutexQue, &xHigherPriorityTaskWokenCli);
            pCliRecvQue->EnElem(pCliRecvQue, CLI_RX_Buffer[0]);
            //xSemaphoreGiveFromISR(pCliRecvQue->xHandleMutexQue, &xHigherPriorityTaskWokenCli);
            //portYIELD_FROM_ISR( xHigherPriorityTaskWokenCli );
        }
    }
    if(huart->Instance == RFID_USARTx_BASE)
    {
        if(HAL_UART_Receive_IT(&RFID_UARTx_Handler, (uint8_t *)RFID_RX_Buffer, 1) == HAL_OK)
        {
            //xSemaphoreTakeFromISR(pRfidRecvQue->xHandleMutexQue, &xHigherPriorityTaskWokenRfid);
            pRfidRecvQue->EnElem(pRfidRecvQue, RFID_RX_Buffer[0]);
            //xSemaphoreGiveFromISR(pRfidRecvQue->xHandleMutexQue, &xHigherPriorityTaskWokenRfid);
            //portYIELD_FROM_ISR( xHigherPriorityTaskWokenRfid );
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
                                                    ThrowErrorCode(defDevID_##DEV, ERR_##DEV##_##ERR, LEVEL, "USART ERR");   \
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
