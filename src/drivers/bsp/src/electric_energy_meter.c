#include "electric_energy_meter.h"
#include "user_app.h"
#include "FreeRTOS.h"
#include "task.h"

#define read     0x03
#define write    0x10

void RS485_Init(u32 bound)
{
    //GPIO¶Ë¿ÚÉèÖÃ
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOF_CLK_ENABLE();			//Ê¹ÄÜGPIOAÊ±ÖÓ
    __HAL_RCC_UART7_CLK_ENABLE();			//Ê¹ÄÜUSART2Ê±ÖÓ

    GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7; //PA2,3
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;		//¸´ÓÃÍÆÍìÊä³ö
    GPIO_Initure.Pull = GPIO_PULLUP;			//ÉÏÀ­
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;		//¸ßËÙ
    GPIO_Initure.Alternate = GPIO_AF8_UART7;	//¸´ÓÃÎªUSART2
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);	   	//³õÊ¼»¯PA2,3

        //USART ³õÊ¼»¯ÉèÖÃ
    UART7_RS485Handler.Instance = UART7;			        //USART2
    UART7_RS485Handler.Init.BaudRate = bound;		        //²¨ÌØÂÊ
    UART7_RS485Handler.Init.WordLength = UART_WORDLENGTH_8B;	//×Ö³¤Îª8Î»Êý¾Ý¸ñÊ½
    UART7_RS485Handler.Init.StopBits = UART_STOPBITS_1;		//Ò»¸öÍ£Ö¹Î»
    UART7_RS485Handler.Init.Parity = UART_PARITY_NONE;		//ÎÞÆæÅ¼Ð£ÑéÎ»
    UART7_RS485Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;	//ÎÞÓ²¼þÁ÷¿Ø
    UART7_RS485Handler.Init.Mode = UART_MODE_TX_RX;		    //ÊÕ·¢Ä£Ê½
    HAL_UART_Init(&UART7_RS485Handler);			        //HAL_UART_Init()»áÊ¹ÄÜUSART2

    __HAL_UART_DISABLE_IT(&UART7_RS485Handler, UART_IT_TC);
    __HAL_UART_ENABLE_IT(&UART7_RS485Handler, UART_IT_RXNE);//¿ªÆô½ÓÊÕÖÐ¶Ï
    HAL_NVIC_EnableIRQ(UART7_IRQn);				        //Ê¹ÄÜUSART1ÖÐ¶Ï
    HAL_NVIC_SetPriority(UART7_IRQn, bspUART7_PreemptPriority, bspUART7_SubPriority);			        //ÇÀÕ¼ÓÅÏÈ¼¶3£¬×ÓÓÅÏÈ¼¶3
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
        HAL_UART_Receive(&UART7_RS485Handler, &res, 1, 1000);
        if (RS485_RX_CNT < 64)
        {
         //	RS485_RX_BUF[RS485_RX_CNT]=res;		//¼ÇÂ¼½ÓÊÕµ½µÄÖµ
            RS485_RX_MODBUS[RS485_RX_CNT] = res;		//¼ÇÂ¼½ÓÊÕµ½µÄÖµ
            RS485_RX_CNT++;						//½ÓÊÕÊý¾ÝÔö¼Ó1
        }
    }
}
uint16_t c_crc(uint8_t *pushdata, uint8_t length)
{
    uint16_t reg_crc = 0xffff;
    uint16_t crc_rtn;
    uint8_t i, j;
    for (i = 0; i < length; i++)
    {
        reg_crc ^= *pushdata++;
        for (j = 0; j < 8; j++)
        {
            if (reg_crc & 0x0001)
            {
                reg_crc = reg_crc >> 1 ^ 0xA001;
            }
            else
            {
                reg_crc = reg_crc >> 1;
            }
        }
    }

    crc_rtn = reg_crc;

    reg_crc = (crc_rtn >> 8) | (crc_rtn << 8);

    return (reg_crc);
}
//RS485·¢ËÍlen¸ö×Ö½Ú.
//buf:·¢ËÍÇøÊ×µØÖ·
//len:·¢ËÍµÄ×Ö½ÚÊý(ÎªÁËºÍ±¾´úÂëµÄ½ÓÊÕÆ¥Åä,ÕâÀï½¨Òé²»Òª³¬¹ý64¸ö×Ö½Ú)
void RS485_Send_Data(u8 *buf, u8 len)

{
    RS485_EN;			//ÉèÖÃÎª·¢ËÍÄ£Ê½
    HAL_UART_Transmit(&UART7_RS485Handler, buf, len, 1000);//´®¿Ú2·¢ËÍÊý¾Ý
    RS485_DIS;
    RS485_RX_CNT = 0;
}
//RS485²éÑ¯½ÓÊÕµ½µÄÊý¾Ý
//buf:½ÓÊÕ»º´æÊ×µØÖ·
//len:¶Áµ½µÄÊý¾Ý³¤¶È
void RS485_Receive_Data(u8 *buf, u8 *len)
{
    u8 rxlen = RS485_RX_CNT;
    u8 i = 0;
    *len = 0;				//Ä¬ÈÏÎª0
    Delay_ms(10);		//µÈ´ý10ms,Á¬Ðø³¬¹ý10msÃ»ÓÐ½ÓÊÕµ½Ò»¸öÊý¾Ý,ÔòÈÏÎª½ÓÊÕ½áÊø
    if (rxlen == RS485_RX_CNT&&rxlen)//½ÓÊÕµ½ÁËÊý¾Ý,ÇÒ½ÓÊÕÍê³ÉÁË
    {
        for (i = 0; i < rxlen; i++)
        {
            buf[i] = RS485_RX_BUF[i];
        }
        *len = RS485_RX_CNT;	//¼ÇÂ¼±¾´ÎÊý¾Ý³¤¶È
        RS485_RX_CNT = 0;		//ÇåÁã
    }
}
void rs_485_analysis(void)
{
    if (RS485_RX_MODBUS[2] == 0X03)
    {
        if (Electricity_meter[0].flag.flag_va == 1)
        {
        }
    }
}

void electricity_meter_analysis(uint8_t add)
{ 
    uint16_t crc_vref;
    uint8_t i;
    crc_vref = c_crc(&RS485_RX_MODBUS[0], 5);
    if ((RS485_RX_MODBUS[5] != crc_vref / 256) || (RS485_RX_MODBUS[6] != crc_vref % 256))
    {
        return;
    }
    if (RS485_RX_MODBUS[2] != 0X02)
    {
        return;
    }
    if (RS485_RX_MODBUS[1] == 0X03)
    {
        if (Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_va == 1)
        {
            //Electricity_meter[RS485_RX_MODBUS[0+1]].massage.massage_va=RS485_RX_MODBUS[5]<<8||RS485_RX_MODBUS[4];
            A1.an[3] = RS485_RX_MODBUS[3];
            A1.an[2] = RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_va = RS485_RX_MODBUS[3] * 256 + RS485_RX_MODBUS[4];//时间电压得10倍A1.yy_test_float;
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_va = 0;
            flag_rs485[add] = 0;
        }
        if (Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_ia == 1)
        {
            A1.an[3] = RS485_RX_MODBUS[3];
            A1.an[2] = RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_ia = RS485_RX_MODBUS[3] * 256 + RS485_RX_MODBUS[4];//整数实际电流的100倍A1.yy_test_float;
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_ia = 0;
            flag_rs485[add] = 0;
        }
        if (Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_power == 1)
        {
            A1.an[3] = RS485_RX_MODBUS[3];
            A1.an[2] = RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_power = RS485_RX_MODBUS[3] * 256 + RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_power = 0;
            flag_rs485[add] = 0;
        }
        if (Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_electric_energy_h == 1)
        {
            A1.an[3] = RS485_RX_MODBUS[3];
            A1.an[2] = RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_electric_energy_h = RS485_RX_MODBUS[3] * 265 + RS485_RX_MODBUS[4];//A1.yy_test_float;
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_electric_energy_h = 0;
            flag_rs485[add] = 0;
        }
        if (Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_electric_energy_l == 1)
        {
            A1.an[3] = RS485_RX_MODBUS[3];
            A1.an[2] = RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_electric_energy_l = RS485_RX_MODBUS[3] * 265 + RS485_RX_MODBUS[4];//A1.yy_test_float;
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_electric_energy_l = 0;
            flag_rs485[add] = 0;
        }
        if (Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_frequency == 1)
        {
            A1.an[3] = RS485_RX_MODBUS[3];
            A1.an[2] = RS485_RX_MODBUS[4];
            Electricity_meter[RS485_RX_MODBUS[0]].massage.massage_frequency = RS485_RX_MODBUS[3] * 256 + RS485_RX_MODBUS[4];//A1.yy_test_float;
            Electricity_meter[RS485_RX_MODBUS[0]].flag.flag_frequency = 0;
            flag_rs485[add] = 0;
        }
        for (i = 0; i < 64; i++)
        {
            RS485_RX_MODBUS[i] = 0;
        }
    }
    if (flag_rs485[add] >= 5)
    {
        Electricity_meter[add].flag.flag_erro = 1;
    }
    else
    {
        Electricity_meter[add].flag.flag_erro = 0;
    }
}
/***********************************************************/
/** @brief 读取电表寄存器相关信息
 * @param pvCON void*
 * @param  add uint8_t   要操作的电表的地址信息，01~255；
 * @param  cmd uint8_t   RS485/MODBUS-RTU通信协议命令如read write
 * @param  massage       获取相关电能信息voltage、current、power、electric_energy_l、electric_energy_h、frequency
 */
void Get_electricity_meter_massage(uint8_t add, uint8_t cmd, uint16_t massage, uint16_t num)
{
    uint16_t VREF_CRC;
    if ((massage == voltage) || (massage == voltage_a) || (massage == voltage_b) || (massage == voltage_c))
    {
        Electricity_meter[add].flag.flag_va = 1;
    }
    if ((massage == current) || (massage == current_a) || (massage == current_b) || (massage == current_c))
    {
        Electricity_meter[add].flag.flag_ia = 1;
    }
    if (massage == power)
    {
        Electricity_meter[add].flag.flag_power = 1;
    }
    if (massage == electric_energy_l)
    {
        Electricity_meter[add].flag.flag_electric_energy_l = 1;
    }
    if (massage == electric_energy_h)
    {
        Electricity_meter[add].flag.flag_electric_energy_h = 1;
    }
    if (massage == frequency)
    {
        Electricity_meter[add].flag.flag_frequency = 1;
    }
    RS485_TX_BUF[0] = add;
    RS485_TX_BUF[1] = cmd;
    RS485_TX_BUF[2] = (massage >> 8) & 0x00ff;
    RS485_TX_BUF[3] = massage & 0x00ff;
    RS485_TX_BUF[4] = (num >> 8) & 0x00ff;
    RS485_TX_BUF[5] = num & 0x00ff;
    VREF_CRC = c_crc(RS485_TX_BUF, 6);
    RS485_TX_BUF[6] = (VREF_CRC >> 8) & 0Xff;
    RS485_TX_BUF[7] = VREF_CRC & 0Xff;
    RS485_Send_Data(RS485_TX_BUF, 8);
    flag_rs485[add]++;

//直接接收数据
//bsp_DelayMS(100);
    vTaskDelay(100);
    electricity_meter_analysis(add);
}
#ifdef DTSF1352
double Get_Electricity_meter_massage_energy(uint8_t add)
{
    double electricity_data;
    Get_electricity_meter_massage(add, read, electric_energy_h, 1);
    Get_electricity_meter_massage(add, read, electric_energy_l, 1);
    electricity_data =
    Electricity_meter[add].massage.massage_electric_energy_h * 65536
    + Electricity_meter[add].massage.massage_electric_energy_l / 100;
    return electricity_data;
}
double Get_Electricity_meter_massage_current(uint8_t add, uint8_t massage)
{
    double electricity_data;
    Get_electricity_meter_massage(add, read, massage, 1);
    electricity_data = Electricity_meter[add].massage.massage_ia / 100;
    return electricity_data;
}

double Get_Electricity_meter_massage_voltage(uint8_t add, uint8_t massage)
{
    double electricity_data;
    Get_electricity_meter_massage(add, read, massage, 1);
    electricity_data = Electricity_meter[add].massage.massage_va / 10;
    return electricity_data;
}

#else
double Get_Electricity_meter_massage_energy(uint8_t add)
{
    double electricity_data;
    Get_electricity_meter_massage(add, read, electric_energy_h, 1);
    Get_electricity_meter_massage(add, read, electric_energy_l, 1);
    electricity_data =
    Electricity_meter[add].massage.massage_electric_energy_h * 65536
    + Electricity_meter[add].massage.massage_electric_energy_l / 100;
    return electricity_data;
}
double Get_Electricity_meter_massage_current(uint8_t add)
{
    double electricity_data;
    Get_electricity_meter_massage(add, read, current, 1);
    electricity_data = Electricity_meter[add].massage.massage_ia / 100;
    return electricity_data;
}
double Get_Electricity_meter_massage_voltage(uint8_t add)
{
    double electricity_data;
    Get_electricity_meter_massage(add, read, voltage, 1);
    electricity_data = Electricity_meter[add].massage.massage_va / 10;
    return electricity_data;
}
double Get_Electricity_meter_massage_power(uint8_t add)
{
    double electricity_data;
    Get_electricity_meter_massage(add, read, power, 1);
    electricity_data = Electricity_meter[add].massage.massage_power / 1000;
    return electricity_data;
}
double Get_Electricity_meter_massage_frequency(uint8_t add)
{
    double electricity_data;
    Get_electricity_meter_massage(add, read, frequency, 1);
    electricity_data = Electricity_meter[add].massage.massage_frequency / 100;
    return electricity_data;
}
#endif
/************************电表RS485END****************/