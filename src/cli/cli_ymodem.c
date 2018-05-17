#include "ymodem.h"
#include "tinysh.h"
#include "bsp.h"

extern ymod_err_t ymod_write_to_file(void);
void cli_ymodem_fnt(int argc, char **argv)
{
    NVIC_SetPriority(UART4_IRQn, 1);
    ymod_write_to_file();
    NVIC_SetPriority(UART4_IRQn, 5);
}

tinysh_cmd_t cli_ymodem_cmd =
{
    0,
    "rb",
    "ymodem trans",
    0,
    cli_ymodem_fnt,
    "-r<cr>",
    0,
    0
};



