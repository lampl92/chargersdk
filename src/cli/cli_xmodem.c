#include "stm32f4xx.h"
#include "bsp_uart.h"
#include "cli_xmodem.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "bsp_dwt.h"

#define XMOD_SOH    0x01
#define XMOD_EOT    0x04
#define XMOD_ACK    0x06
#define XMOD_NAK    0x15
#define XMOD_CAN    0x18

extern SemaphoreHandle_t  xprintfMutex;

typedef enum
{
    XMOD_STAT_IDLE,
    XMOD_STAT_REQ_RECV,
    XMOD_STAT_RECV
}XMOD_STATE;

static uint32_t xmod_read(uint8_t *pbuff, uint32_t rlen)
{
    return uart_read(UART_PORT_CLI, pbuff, rlen, 500);    
}
static uint32_t xmod_write(uint8_t *pbuff, uint32_t wlen)
{
    return uart_write(UART_PORT_CLI, pbuff, wlen);
    
}
static void xmod_delay(uint32_t ms)
{
    bsp_DelayMS(ms);
}

void xmod_parse(uint8_t *pbuff, uint8_t *data)
{
    
}

void xmod_poll(XMOD_STATE stat)
{
    uint8_t buff[200] = { 0};
    uint32_t rl = 0;
    
    while (1)
    {
        switch (stat)
        {
        case XMOD_STAT_IDLE:
            break;
        case XMOD_STAT_REQ_RECV:
            xmod_write("C", 1);
            rl = xmod_read(buff, 500);
            if (rl > 0)
            {
                stat = XMOD_STAT_RECV;
            }
            break;
        case XMOD_STAT_RECV:
            //xmod_parse()
            break;
        default:
            break;
           
            
        }
    }
}

void cli_xmodem_fnt(int argc, char **argv)
{
    xSemaphoreTake(xprintfMutex, portMAX_DELAY);
    
    if (argc == 2 && strcmp(argv[1], "-r") == 0)
    {
        xmod_poll(XMOD_STAT_REQ_RECV);
    }
    
    xSemaphoreGive(xprintfMutex);
}

tinysh_cmd_t cli_xmodem_cmd =
{
    0,
    "xmodem",
    "xmodem eee",
    0,
    cli_xmodem_fnt,
    "-r<cr>",
    0,
    0
};



