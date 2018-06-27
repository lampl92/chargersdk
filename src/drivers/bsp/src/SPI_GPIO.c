#include "SPI_GPIO.h"
#include "bsp.h"
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

static unsigned char SPI_MCUReadByte(void);
static void SPI_MCUWriteByte(unsigned char wByte);
static void ADE7953_SPI_Enable(void);
static void ADE7953_SPI_Enable(void)
{
    ADE_SCLK_H;
    ADE_MOSI_H;
    ADE_SPI_ENABLE;
    ADE_SPI_RST_ENABLE;
}
static unsigned char SPI_MCUReadByte(void)
{
    unsigned char i, rByte = 0;

    for (i = 0; i < 8; i++)
    {
        ADE_SCLK_L;
        bsp_DelayUS(10);
        rByte <<= 1;

        if (ADE_MISO)
        {
            rByte |= 0x01;
        }
        bsp_DelayUS(2);
        ADE_SCLK_H;
        bsp_DelayUS(8);
    }
    return rByte;
}

static void SPI_MCUWriteByte(unsigned char wByte)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        ADE_SCLK_L;
        bsp_DelayUS(10);
        if ((wByte & 0x80))
        {
            ADE_MOSI_H;
        }
        else
        {
            ADE_MOSI_L;
        }
        bsp_DelayUS(2);
        ADE_SCLK_H;
        wByte <<= 1;
        bsp_DelayUS(8);
    }
}

void Init_ADE7953_SPI(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    GPIO_Initure.Pin = GPIO_PIN_4 | GPIO_PIN_6;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_5;
    GPIO_Initure.Mode = GPIO_MODE_INPUT;
    GPIO_Initure.Pull = GPIO_NOPULL;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);

    ADE7953_SPI_Enable();
}

void Write_ADE7953_SPI(unsigned int ADE_Addr, unsigned char Nr_Bytes, unsigned char *pReg_Data)
{
    unsigned char i;

    unsigned char MS_Addr;
    unsigned char LS_Addr;

    LS_Addr = (unsigned char)ADE_Addr;
    MS_Addr = (unsigned char)(ADE_Addr >> 8);
    bsp_DelayUS(1);
    ADE_SPI_ENABLE;
    SPI_MCUWriteByte(MS_Addr);
    bsp_DelayUS(1);
    SPI_MCUWriteByte(LS_Addr);
    bsp_DelayUS(1);
    SPI_MCUWriteByte(0x00);
    pReg_Data = pReg_Data + (Nr_Bytes - 1);
    bsp_DelayUS(1);
    for (i = 0; i < Nr_Bytes; i++)
    {
        SPI_MCUWriteByte(*pReg_Data);
        pReg_Data--;
    }
    ADE_SPI_DISABLE;
}

void Read_ADE7953_SPI(unsigned int ADE_Addr, unsigned char Nr_Bytes,
                      unsigned char *pReg_Data)
{
    unsigned char i;
    unsigned char MS_Addr;
    unsigned char LS_Addr;
    LS_Addr = (unsigned char)ADE_Addr;
    MS_Addr = (unsigned char)(ADE_Addr >> 8);
    ADE_SPI_ENABLE;
    SPI_MCUWriteByte(MS_Addr);
    bsp_DelayUS(10);
    SPI_MCUWriteByte(LS_Addr);
    bsp_DelayUS(10);
    SPI_MCUWriteByte(0x80);
    pReg_Data = pReg_Data + (Nr_Bytes - 1);
    for (i = 0; i < Nr_Bytes; i++)
    {
        *pReg_Data = SPI_MCUReadByte();
        bsp_DelayUS(10);
        pReg_Data--;
    }
    ADE_SPI_DISABLE;
}
